#include <linux/dma-iommu.h>
#include <linux/iommu.h>
#include <linux/etherdevice.h>
#include <linux/module.h>
#include <linux/netdevice.h>
#include <linux/of_platform.h>
#include <linux/pci-epc.h>
#include <linux/pci-epf.h>
#include <linux/platform_device.h>
#include <linux/workqueue.h>

#include <./ax_pcie_net.h>


// #define BAR0_SIZE SZ_4M
#define BAR0_SIZE SZ_8M

#define SUPPORT_LINKUP 0

enum bar0_amap_type {
	META_DATA,
	EP_MEM,
	HOST_MEM,
	EP_RX_BUF,
	AMAP_MAX,
};

struct bar0_amap {
	int size;
	void *virt;
	dma_addr_t iova;
	dma_addr_t phy;
};

struct pci_epf_axnet {
	struct pci_epf *epf;
	struct pci_epc *epc;
	struct device *fdev;
	struct pci_epf_header header;
	struct bar0_amap bar0_amap[AMAP_MAX];
	struct bar_md *bar_md;
	dma_addr_t bar0_iova;
	void *bar0_va;
	struct net_device *ndev;
	struct napi_struct napi;
	bool pcie_link_status;
	u64 irq_cnt;

	struct ep_ring_buf ep_ring_buf;
	struct host_ring_buf host_ring_buf;

	enum dir_link_state tx_link_state;
	enum dir_link_state rx_link_state;
	enum os_link_state os_link_state;
	/* To synchronize network link state machine */
	struct mutex link_state_lock;
	wait_queue_head_t link_state_wq;

	struct list_head h2ep_empty_list;
	/* To protect h2ep empty list */
	spinlock_t h2ep_empty_lock;
	dma_addr_t rx_buf_iova;
	unsigned long *rx_buf_bitmap;
	int rx_num_pages;
	void __iomem *tx_dst_va;
	phys_addr_t tx_dst_pci_addr;

#if ENABLE_SIMPLE_DMA
	spinlock_t dma_wr_lock;
	spinlock_t dma_rd_lock;
#endif

	struct axnet_counter h2ep_ctrl;
	struct axnet_counter ep2h_ctrl;
	struct axnet_counter h2ep_empty;
	struct axnet_counter h2ep_full;
	struct axnet_counter ep2h_empty;
	struct axnet_counter ep2h_full;
};

static struct pci_epf_axnet *g_axnet;
static void axnet_ep_ctrl_irq_callback(struct pci_epf_axnet *axnet);
static void axnet_ep_data_irq_callback(struct pci_epf_axnet *axnet);

#if ENABLE_SIMPLE_DMA
extern void ax_pcie_write_dbi(struct pci_epc *epc, u32 reg, size_t size,
			      u32 val);
extern u32 ax_pcie_read_dbi(struct pci_epc *epc, u32 reg, size_t size);

#define	PCIE_DMA_BASE		0x380000

/* HDMA register */
#define	CHAN_ADDR_BASE		0x200
#define	RD_CHAN_EN			0x100
#define	RD_CHAN_TRAN_SIZE	0x11c
#define	RD_SAR_LOW_ADDR		0x120
#define	RD_SAR_UPPER_ADDR	0x124
#define	RD_DAR_LOW_ADDR		0x128
#define	RD_DAR_UPPER_ADDR	0x12C
#define	RD_DOORBELL_EN		0x104
#define	RD_CHAN_STATUS		0x180

#define	WR_CHAN_EN			0x000
#define	WR_CHAN_TRAN_SIZE	0x01c
#define	WR_SAR_LOW_ADDR		0x020
#define	WR_SAR_UPPER_ADDR	0x024
#define	WR_DAR_LOW_ADDR		0x028
#define	WR_DAR_UPPER_ADDR	0x02C
#define	WR_DOORBELL_EN		0x004
#define	WR_CHAN_STATUS		0x080

void ax_pcie_write_reg(u32 reg, u32 val)
{
	ax_pcie_write_dbi(g_axnet->epc, reg, 4, val);
}

u32 ax_pcie_read_reg(u32 reg)
{
	return ax_pcie_read_dbi(g_axnet->epc, reg, 4);
}

static int ax_pcie_dma_write(int chan, u64 src, u64 dst, u32 size)
{
	int val = 0;
	int retry = 100000;	//100ms
	int i;
	unsigned long flags;

	int low_src_addr = src & 0xffffffff;
	int upper_src_addr = (src >> 32);
	int low_dst_addr = dst & 0xffffffff;
	int upper_dst_addr = dst >> 32;

	pr_debug("pcie dma write, size:%d, src:%llx, dst:%llx\n", size, src,
		 dst);

	spin_lock_irqsave(&g_axnet->dma_wr_lock, flags);
	ax_pcie_write_reg(PCIE_DMA_BASE + chan * CHAN_ADDR_BASE + WR_CHAN_EN, 0x1);	//HDMA WR channel enable
	ax_pcie_write_reg(PCIE_DMA_BASE + chan * CHAN_ADDR_BASE + WR_CHAN_TRAN_SIZE, size);	//HDMA WR channel transfer size
	ax_pcie_write_reg(PCIE_DMA_BASE + chan * CHAN_ADDR_BASE + WR_SAR_LOW_ADDR, low_src_addr);	//HDMA WR SAR LOW Address
	ax_pcie_write_reg(PCIE_DMA_BASE + chan * CHAN_ADDR_BASE + WR_SAR_UPPER_ADDR, upper_src_addr);	//HDMA WR SAR HIGH Address
	ax_pcie_write_reg(PCIE_DMA_BASE + chan * CHAN_ADDR_BASE + WR_DAR_LOW_ADDR, low_dst_addr);	//HDMA WR DAR LOW Address
	ax_pcie_write_reg(PCIE_DMA_BASE + chan * CHAN_ADDR_BASE + WR_DAR_UPPER_ADDR, upper_dst_addr);	//HDMA WR DAR HIGH Address
	ax_pcie_write_reg(PCIE_DMA_BASE + chan * CHAN_ADDR_BASE + WR_DOORBELL_EN, 0x1);	//HDMA WR doorbell start

	for (i = 0; i < retry; i++) {
		/* 0x1 runing; 0x2 abort; 0x3 stop */
		val = ax_pcie_read_reg(PCIE_DMA_BASE + chan * CHAN_ADDR_BASE + WR_CHAN_STATUS);	//HDMA WR channel status
		if (val != 0x1)
			break;

		udelay(1);
	}
	spin_unlock_irqrestore(&g_axnet->dma_wr_lock, flags);

	if (i >= retry) {
		pr_err("error, pcie dma write timeout!\n");
		return -1;
	}

	if (val == 0x3) {
		pr_debug("pcie dma write success\n");
		return 0;
	} else {
		pr_err("error, pcie dma write abort!\n");
		return -1;
	}
}

static int ax_pcie_dma_read(int chan, u64 src, u64 dst, u32 size)
{
	int val = 0;
	int retry = 100000;	//100ms
	int i;
	unsigned long flags;

	int low_src_addr = src & 0xffffffff;
	int upper_src_addr = (src >> 32);
	int low_dst_addr = dst & 0xffffffff;
	int upper_dst_addr = dst >> 32;

	pr_debug("pcie dma read, size:%d, src:%llx, dst:%llx\n", size, src,
		 dst);

	spin_lock_irqsave(&g_axnet->dma_rd_lock, flags);
	ax_pcie_write_reg(PCIE_DMA_BASE + chan * CHAN_ADDR_BASE + RD_CHAN_EN, 0x1);	//HDMA RD channel enable
	ax_pcie_write_reg(PCIE_DMA_BASE + chan * CHAN_ADDR_BASE + RD_CHAN_TRAN_SIZE, size);	//HDMA RD channel transfer size
	ax_pcie_write_reg(PCIE_DMA_BASE + chan * CHAN_ADDR_BASE + RD_SAR_LOW_ADDR, low_src_addr);	//HDMA RD SAR LOW Address
	ax_pcie_write_reg(PCIE_DMA_BASE + chan * CHAN_ADDR_BASE + RD_SAR_UPPER_ADDR, upper_src_addr);	//HDMA RD SAR HIGH Address
	ax_pcie_write_reg(PCIE_DMA_BASE + chan * CHAN_ADDR_BASE + RD_DAR_LOW_ADDR, low_dst_addr);	//HDMA RD DAR LOW Address
	ax_pcie_write_reg(PCIE_DMA_BASE + chan * CHAN_ADDR_BASE + RD_DAR_UPPER_ADDR, upper_dst_addr);	//HDMA RD DAR HIGH Address
	ax_pcie_write_reg(PCIE_DMA_BASE + chan * CHAN_ADDR_BASE + RD_DOORBELL_EN, 0x1);	//HDMA RD doorbell start

	for (i = 0; i < retry; i++) {
		/* 0x1 runing; 0x2 abort; 0x3 stop */
		val = ax_pcie_read_reg(PCIE_DMA_BASE + chan * CHAN_ADDR_BASE + RD_CHAN_STATUS);	//HDMA RD channel status
		if (val != 0x1)
			break;

		udelay(1);
	}
	spin_unlock_irqrestore(&g_axnet->dma_rd_lock, flags);

	if (i >= retry) {
		pr_err("error, pcie dma read timeout!\n");
		return -1;
	}

	if (val == 0x3) {
		pr_debug("pcie dma read success\n");
		return 0;
	} else {
		pr_err("error, pcie dma read abort!\n");
		return -1;
	}
}
#endif

#if USE_MAILBOX
extern int ax_mailbox_register_notify(int mid,
				      void (*func
					    (int fromid, int regno,
					     int info_data)));
extern int ax_mailbox_read(int regno, int *data, int size);

// static void update_irq_cnt(void)
// {
//      struct ep_ring_buf *ep_ring_buf = &g_axnet->ep_ring_buf;
//      struct irq_md *irq = ep_ring_buf->h2ep_irq_msgs;

//      u32 irq_cnt = readl(&(irq->mailbox_irq_cnt));
//      pr_debug("mailbox irq cnt: %d\n", irq_cnt);

//      if(irq_cnt < 8)
//      writel(irq_cnt + 1, &(irq->mailbox_irq_cnt));
// }

static void update_irq_cnt(void)
{
	struct ep_ring_buf *ep_ring_buf = &g_axnet->ep_ring_buf;
	struct irq_md *irq = ep_ring_buf->h2ep_irq_msgs;

	g_axnet->irq_cnt++;
	writeq(g_axnet->irq_cnt, &(irq->rd_cnt));
	mb();

	pr_debug("irq->rd_cnt:%lld\n", g_axnet->irq_cnt);
}

static int clear_message_irq(int fromid, int regno, int count)
{
	unsigned int ret = 0;
	unsigned int msg[8];

	update_irq_cnt();

	ret = ax_mailbox_read(regno, (int *)&msg, count);
	pr_debug("ret = %d count = %d, msg[0] = %x\n", ret, count, msg[0]);
	if (ret != count) {
		pr_err("ax_mailbox_read failed, count:%d, ret:%d\n", count,
		       ret);
		return 0;
	}
	// update_irq_cnt();

	return 1;
}

static int
slave_request_message_irq(void (*handler) (int fromid, int regno, int count))
{
	int ret = 0;
	ret = ax_mailbox_register_notify(PCIE_MASTERID, (void *)handler);
	if (ret < 0)
		pr_err("ax mailbox register notify fail!\n");

	return ret;
}

void handle_msg(void)
{
#if 0
	int val1, val2;
	struct ep_ring_buf *ep_ring_buf = &g_axnet->ep_ring_buf;
	struct irq_md *irq = ep_ring_buf->h2ep_irq_msgs;

	val1 = readl(&(irq->irq_ctrl));
	if (val1 == 0xaa) {
		pr_debug("ctrl irq\n");
		writel(0, &(irq->irq_ctrl));
		axnet_ep_ctrl_irq_callback(g_axnet);
	}

	val2 = readl(&(irq->irq_data));
	if (val2 == 0xbb) {
		pr_debug("data irq\n");
		writel(0, &(irq->irq_data));
		axnet_ep_data_irq_callback(g_axnet);
	}
#else
	axnet_ep_ctrl_irq_callback(g_axnet);
	axnet_ep_data_irq_callback(g_axnet);
#endif
}

void slave_message_irq_handler(int fromid, int regno, int count)
{
	unsigned int status = 0;

	pr_debug("slave: mailbox msg handler, regno:%x\n", regno);

	status = clear_message_irq(fromid, regno, count);
	if (status)
		handle_msg();

	// update_irq_cnt();

}

void enable_mailbox_int(struct pci_epf_axnet *axnet, int en)
{
	struct ep_ring_buf *ep_ring_buf = &axnet->ep_ring_buf;
	struct irq_md *irq = ep_ring_buf->h2ep_irq_msgs;

	if (en) {
		pr_debug("enable mailbox irq\n");
		writel(1, &(irq->mailbox_int));
	} else {
		pr_debug("disable mailbox irq\n");
		writel(0, &(irq->mailbox_int));
	}
}
#endif

static void axnet_ep_read_ctrl_msg(struct pci_epf_axnet *axnet,
				   struct ctrl_msg *msg)
{
	struct ep_ring_buf *ep_ring_buf = &axnet->ep_ring_buf;
	struct ctrl_msg *ctrl_msg = ep_ring_buf->h2ep_ctrl_msgs;
	u32 idx;

	if (axnet_ivc_empty(&axnet->h2ep_ctrl)) {
		dev_dbg(axnet->fdev, "%s: H2EP ctrl ring empty\n", __func__);
		return;
	}

	idx = axnet_ivc_get_rd_cnt(&axnet->h2ep_ctrl) % RING_COUNT;
	memcpy(msg, &ctrl_msg[idx], sizeof(*msg));
	axnet_ivc_advance_rd(&axnet->h2ep_ctrl);
}

/* TODO Handle error case */
static int axnet_ep_write_ctrl_msg(struct pci_epf_axnet *axnet,
				   struct ctrl_msg *msg)
{
	struct host_ring_buf *host_ring_buf = &axnet->host_ring_buf;
	struct ctrl_msg *ctrl_msg = host_ring_buf->ep2h_ctrl_msgs;
	struct pci_epf *epf = axnet->epf;
	struct pci_epc *epc = axnet->epf->epc;
	u32 idx;

	pr_debug("axnet_ep_write_ctrl_msg\n");

	if (axnet_ivc_full(&axnet->ep2h_ctrl)) {
		/* Raise an interrupt to let host process EP2H ring */
		pci_epc_raise_irq(epc, epf->func_no, epf->vfunc_no,
				  PCI_EPC_IRQ_MSI, MSI_IRQ_NUM);
		dev_dbg(axnet->fdev, "%s: EP2H ctrl ring full\n", __func__);
		return -EAGAIN;
	}

	idx = axnet_ivc_get_wr_cnt(&axnet->ep2h_ctrl) % RING_COUNT;
	memcpy(&ctrl_msg[idx], msg, sizeof(*msg));
	axnet_ivc_advance_wr(&axnet->ep2h_ctrl);
	pci_epc_raise_irq(epc, epf->func_no, epf->vfunc_no, PCI_EPC_IRQ_MSI,
				MSI_IRQ_NUM);

	return 0;
}

#if !ENABLE_SIMPLE_DMA
static dma_addr_t axnet_ivoa_alloc(struct pci_epf_axnet *axnet, int size)
{
	dma_addr_t iova;
	int pageno;
	int order = get_order(size);

	//order = 0;
	pageno = bitmap_find_free_region(axnet->rx_buf_bitmap,
					 axnet->rx_num_pages, order);
	if (pageno < 0) {
		dev_err(axnet->fdev, "%s: Rx iova alloc fail, page: %d\n",
			__func__, pageno);
		return -1;
	}
	iova = axnet->rx_buf_iova + (pageno << PAGE_SHIFT);

	return iova;
}

static void axnet_ep_iova_dealloc(struct pci_epf_axnet *axnet, dma_addr_t iova)
{
	int pageno;
	struct net_device *ndev = axnet->ndev;
	int size = ndev->mtu + ETH_HLEN;
	int order = get_order(size);

	//order = 0;
	pageno = (iova - axnet->rx_buf_iova) >> PAGE_SHIFT;
	bitmap_release_region(axnet->rx_buf_bitmap, pageno, order);
}

static void axnet_ep_alloc_empty_buffers(struct pci_epf_axnet *axnet)
{
	struct ep_ring_buf *ep_ring_buf = &axnet->ep_ring_buf;
	struct pci_epf *epf = axnet->epf;
	struct pci_epc *epc = axnet->epf->epc;
	struct data_msg *h2ep_data_msg = ep_ring_buf->h2ep_data_msgs;
	struct h2ep_empty_list *h2ep_empty_ptr;

	pr_debug("axnet_ep_alloc_empty_buffers\n");

	while (!axnet_ivc_full(&axnet->h2ep_empty)) {
		dma_addr_t iova;
		void *virt;
		struct net_device *ndev = axnet->ndev;
		int len = ndev->mtu + ETH_HLEN;
		u32 idx;
		unsigned long flags;

		iova = axnet_ivoa_alloc(axnet, len);
		if (iova == -1) {
			dev_err(axnet->fdev, "%s: iova alloc failed\n",
				__func__);
			break;
		}

		virt = (iova - axnet->bar0_iova) + axnet->bar0_va;

		h2ep_empty_ptr = kmalloc(sizeof(*h2ep_empty_ptr), GFP_KERNEL);
		if (!h2ep_empty_ptr) {
			pr_err("free dma mem failed\n");
			//TOTO: we need free dma mem!
			axnet_ep_iova_dealloc(axnet, iova);
			break;
		}

		h2ep_empty_ptr->virt = virt;
		h2ep_empty_ptr->size = len;
		h2ep_empty_ptr->iova = iova;
		spin_lock_irqsave(&axnet->h2ep_empty_lock, flags);
		list_add_tail(&h2ep_empty_ptr->list, &axnet->h2ep_empty_list);
		spin_unlock_irqrestore(&axnet->h2ep_empty_lock, flags);

		idx = axnet_ivc_get_wr_cnt(&axnet->h2ep_empty) % RING_COUNT;
		h2ep_data_msg[idx].free_pcie_address = iova;
		h2ep_data_msg[idx].buffer_len = len;
		mb();
		axnet_ivc_advance_wr(&axnet->h2ep_empty);
		mb();

		pr_debug("pcie_address %d: %llx\n", idx,
			 (u64) (&(h2ep_data_msg[idx].free_pcie_address)));
		pr_debug("alloc skb buf, idx:%d, addr:%llx, len:%lld", idx,
			 h2ep_data_msg[idx].free_pcie_address,
			 h2ep_data_msg[idx].buffer_len);

	}

	pci_epc_raise_irq(epc, epf->func_no, epf->vfunc_no, PCI_EPC_IRQ_MSI,
			  MSI_IRQ_NUM);
}

static void axnet_ep_free_empty_buffers(struct pci_epf_axnet *axnet)
{
	struct h2ep_empty_list *h2ep_empty_ptr, *temp;
	unsigned long flags;

	spin_lock_irqsave(&axnet->h2ep_empty_lock, flags);
	list_for_each_entry_safe(h2ep_empty_ptr, temp, &axnet->h2ep_empty_list,
				 list) {
		list_del(&h2ep_empty_ptr->list);
		axnet_ep_iova_dealloc(axnet, h2ep_empty_ptr->iova);
		kfree(h2ep_empty_ptr);
	}
	spin_unlock_irqrestore(&axnet->h2ep_empty_lock, flags);
}
#endif


static void axnet_ep_stop_rx_work(struct pci_epf_axnet *axnet)
{
	/* TODO wait for syncpoint interrupt handlers */
}

static void axnet_ep_clear_data_msg_counters(struct pci_epf_axnet *axnet)
{
	unsigned long dbi_flags = 0;

	ax_pcie_spin_lock(&dbi_flags);
	//ctrl send & recv
	WRITE_ONCE(*axnet->ep2h_ctrl.rd, 0);
	WRITE_ONCE(*axnet->ep2h_ctrl.wr, 0);
	WRITE_ONCE(*axnet->h2ep_ctrl.rd, 0);
	WRITE_ONCE(*axnet->h2ep_ctrl.wr, 0);

	//host recv
	WRITE_ONCE(*axnet->ep2h_full.rd, 0);
	WRITE_ONCE(*axnet->ep2h_full.wr, 0);

	//host send
#if !ENABLE_SIMPLE_DMA
	WRITE_ONCE(*axnet->h2ep_empty.rd, 0);
	WRITE_ONCE(*axnet->h2ep_empty.wr, 0);
#endif
	WRITE_ONCE(*axnet->h2ep_full.rd, 0);
	WRITE_ONCE(*axnet->h2ep_full.wr, 0);
	ax_pcie_spin_unlock(&dbi_flags);
}

static void axnet_ep_update_link_state(struct net_device *ndev,
				       enum os_link_state state)
{
	if (state == OS_LINK_STATE_UP) {
		pr_info("net link is up\n");
		netif_start_queue(ndev);
		netif_carrier_on(ndev);
	} else if (state == OS_LINK_STATE_DOWN) {
		pr_info("net link is down\n");
		netif_carrier_off(ndev);
		netif_stop_queue(ndev);
	} else {
		pr_err("%s: invalid sate: %d\n", __func__, state);
	}
}

/* OS link state machine */
static void axnet_ep_update_link_sm(struct pci_epf_axnet *axnet)
{
	struct net_device *ndev = axnet->ndev;
	enum os_link_state old_state = axnet->os_link_state;

	if ((axnet->rx_link_state == DIR_LINK_STATE_UP) &&
	    (axnet->tx_link_state == DIR_LINK_STATE_UP))
		axnet->os_link_state = OS_LINK_STATE_UP;
	else
		axnet->os_link_state = OS_LINK_STATE_DOWN;

	if (axnet->os_link_state != old_state)
		axnet_ep_update_link_state(ndev, axnet->os_link_state);
}

/* One way link state machine */
static void axnet_ep_user_link_up_req(struct pci_epf_axnet *axnet)
{
	struct ctrl_msg msg;

	axnet_ep_clear_data_msg_counters(axnet);

#if !ENABLE_SIMPLE_DMA
	axnet_ep_alloc_empty_buffers(axnet);
#endif

	msg.msg_id = CTRL_MSG_LINK_UP;
	axnet_ep_write_ctrl_msg(axnet, &msg);
	axnet->rx_link_state = DIR_LINK_STATE_UP;
	axnet->bar_md->ep_link_status = DIR_LINK_STATE_UP;
	axnet_ep_update_link_sm(axnet);
}

static void axnet_ep_user_link_down_req(struct pci_epf_axnet *axnet)
{
	struct ctrl_msg msg;

	axnet->rx_link_state = DIR_LINK_STATE_SENT_DOWN;
	axnet->bar_md->ep_link_status = DIR_LINK_STATE_SENT_DOWN;
	msg.msg_id = CTRL_MSG_LINK_DOWN;
	axnet_ep_write_ctrl_msg(axnet, &msg);
	axnet_ep_update_link_sm(axnet);
}

static void axnet_ep_rcv_link_up_msg(struct pci_epf_axnet *axnet)
{
	axnet->tx_link_state = DIR_LINK_STATE_UP;
	axnet_ep_update_link_sm(axnet);
}

static void axnet_ep_rcv_link_down_msg(struct pci_epf_axnet *axnet)
{
	struct ctrl_msg msg;

	msg.msg_id = CTRL_MSG_LINK_DOWN_ACK;
	axnet_ep_write_ctrl_msg(axnet, &msg);
	axnet->tx_link_state = DIR_LINK_STATE_DOWN;
	axnet_ep_update_link_sm(axnet);
}

static void axnet_ep_rcv_link_down_ack(struct pci_epf_axnet *axnet)
{
#if !ENABLE_SIMPLE_DMA
	axnet_ep_free_empty_buffers(axnet);
#endif
	axnet->rx_link_state = DIR_LINK_STATE_DOWN;
	axnet->bar_md->ep_link_status = DIR_LINK_STATE_DOWN;
	wake_up_interruptible(&axnet->link_state_wq);
	axnet_ep_update_link_sm(axnet);
}

static int axnet_ep_open(struct net_device *ndev)
{
	struct device *fdev = ndev->dev.parent;
	struct pci_epf_axnet *axnet = dev_get_drvdata(fdev);

	pr_info("open pcie net\n");

#if SUPPORT_LINKUP
	if (!axnet->pcie_link_status) {
		dev_err(fdev, "%s: PCIe link is not up\n", __func__);
		return -ENODEV;
	}
#endif

	mutex_lock(&axnet->link_state_lock);
	if (axnet->rx_link_state == DIR_LINK_STATE_DOWN)
		axnet_ep_user_link_up_req(axnet);
	napi_enable(&axnet->napi);
	mutex_unlock(&axnet->link_state_lock);

	return 0;
}

static int axnet_ep_close(struct net_device *ndev)
{
	struct device *fdev = ndev->dev.parent;
	struct pci_epf_axnet *axnet = dev_get_drvdata(fdev);
	int ret = 0;

	pr_info("close pcie net\n");

	mutex_lock(&axnet->link_state_lock);
	napi_disable(&axnet->napi);
	if (axnet->rx_link_state == DIR_LINK_STATE_UP)
		axnet_ep_user_link_down_req(axnet);

	if (axnet->tx_link_state == DIR_LINK_STATE_UP) { //peer is up
		ret = wait_event_interruptible_timeout(axnet->link_state_wq,
							(axnet->rx_link_state ==
							DIR_LINK_STATE_DOWN),
							msecs_to_jiffies(LINK_TIMEOUT));
		ret = (ret > 0) ? 0 : -ETIMEDOUT;
		if (ret < 0) {
			pr_err
				("%s: link state machine failed: tx_state: %d rx_state: %d err: %d\n",
				__func__, axnet->tx_link_state, axnet->rx_link_state, ret);
		}
	} else {
	#if !ENABLE_SIMPLE_DMA
		axnet_ep_free_empty_buffers(axnet);
	#endif
		axnet->rx_link_state = DIR_LINK_STATE_DOWN;
		axnet->bar_md->ep_link_status = DIR_LINK_STATE_DOWN;
	}
	mutex_unlock(&axnet->link_state_lock);

	/* Stop using empty buffers(which are full in rx) of local system */
	axnet_ep_stop_rx_work(axnet);

	return 0;
}

static int axnet_ep_change_mtu(struct net_device *ndev, int new_mtu)
{
	bool set_down = false;

	if (new_mtu > AXNET_MAX_MTU || new_mtu < AXNET_MIN_MTU) {
		pr_err("MTU range is %d to %d\n", AXNET_MIN_MTU, AXNET_MAX_MTU);
		return -EINVAL;
	}

	if (netif_running(ndev)) {
		set_down = true;
		axnet_ep_close(ndev);
	}

	pr_info("changing MTU from %d to %d\n", ndev->mtu, new_mtu);

	ndev->mtu = new_mtu;

	if (set_down)
		axnet_ep_open(ndev);

	return 0;
}

static netdev_tx_t axnet_ep_start_xmit(struct sk_buff *skb,
				       struct net_device *ndev)
{
	struct device *fdev = ndev->dev.parent;
	struct pci_epf_axnet *axnet = dev_get_drvdata(fdev);
	struct host_ring_buf *host_ring_buf = &axnet->host_ring_buf;
	struct data_msg *ep2h_data_msg = host_ring_buf->ep2h_data_msgs;
	struct skb_shared_info *info = skb_shinfo(skb);
	struct pci_epf *epf = axnet->epf;
	struct pci_epc *epc = epf->epc;
	struct device *cdev = epc->dev.parent;
	dma_addr_t src_iova;
	u32 rd_idx, wr_idx;
#if ENABLE_SIMPLE_DMA
	u64 dst_iova;
	int dst_len, len;
#else
	u64 dst_masked, dst_off, dst_iova;
	int dst_len, len;
#endif
	int ret;

	pr_debug("start xmit packet\n");

	/*TODO Not expecting skb frags, remove this after testing */
	WARN_ON(info->nr_frags);

	/* Check if EP2H_EMPTY_BUF available to read */
	if (!axnet_ivc_rd_available(&axnet->ep2h_empty)) {
		pci_epc_raise_irq(epc, epf->func_no, epf->vfunc_no,
				  PCI_EPC_IRQ_MSI, MSI_IRQ_NUM);
		pr_debug("%s: No EP2H empty msg, stop tx\n", __func__);
		netif_stop_queue(ndev);
		return NETDEV_TX_BUSY;
	}

	/* Check if EP2H_FULL_BUF available to write */
	if (axnet_ivc_full(&axnet->ep2h_full)) {
		pci_epc_raise_irq(epc, epf->func_no, epf->vfunc_no,
				  PCI_EPC_IRQ_MSI, MSI_IRQ_NUM);
		pr_debug("%s: No EP2H full buf, stop tx\n", __func__);
		netif_stop_queue(ndev);
		return NETDEV_TX_BUSY;
	}

	len = skb_headlen(skb);

	src_iova = dma_map_single(cdev, skb->data, len, DMA_TO_DEVICE);
	if (dma_mapping_error(cdev, src_iova)) {
		dev_err(fdev, "%s: dma_map_single failed\n", __func__);
		dev_kfree_skb_any(skb);
		ndev->stats.tx_dropped++;
		return NETDEV_TX_OK;
	}

	/* Get EP2H empty msg */
	rd_idx = axnet_ivc_get_rd_cnt(&axnet->ep2h_empty) % RING_COUNT;
	dst_iova = ep2h_data_msg[rd_idx].free_pcie_address;
	dst_len = ep2h_data_msg[rd_idx].buffer_len;

#if !ENABLE_SIMPLE_DMA
	/*
	 * Map host dst mem to local PCIe address range.
	 * PCIe address range is SZ_64K aligned.
	 */
	dst_masked = (dst_iova & ~(SZ_64K - 1));
	dst_off = (dst_iova & (SZ_64K - 1));
	pr_debug("dst addr: %llx, dst_masked: %llx, dst_off: %llx\n", dst_iova,
		 dst_masked, dst_off);

	ret =
	    pci_epc_map_addr(epc, epf->func_no, epf->vfunc_no,
			     axnet->tx_dst_pci_addr, dst_masked, dst_len);
	if (ret < 0) {
		dev_err(fdev, "failed to map dst addr to PCIe addr range\n");
		dma_unmap_single(cdev, src_iova, len, DMA_TO_DEVICE);
		dev_kfree_skb_any(skb);
		ndev->stats.tx_dropped++;
		return NETDEV_TX_OK;
	}
#endif

	/*
	 * Advance read count after all failure cases completed, to avoid
	 * dangling buffer at host.
	 */
	axnet_ivc_advance_rd(&axnet->ep2h_empty);

#if ENABLE_SIMPLE_DMA
	/* Trigger DMA write from src_iova to dst_iova */
	ret = ax_pcie_dma_write(0, src_iova, dst_iova, len);
	if (ret) {
		ndev->stats.tx_errors++;
		dma_unmap_single(cdev, src_iova, len, DMA_TO_DEVICE);
		dev_kfree_skb_any(skb);
		pr_err("transmit packet failed\n");
		return NETDEV_TX_OK;
	}
	mb();
#else
	pr_debug("dst:%llx, src:%llx, len:%d\n",
		 (u64) (axnet->tx_dst_va + dst_off), (u64) skb->data, len);
	/* Copy skb->data to host dst address, use CPU virt addr */
	memcpy_toio((void *)(axnet->tx_dst_va + dst_off), skb->data, len);
	/*
	 * tx_dst_va is ioremap_wc() mem, add mb to make sure complete skb->data
	 * written to dst before adding it to full buffer
	 */
	mb();
#endif

	/* Push dst to EP2H full ring */
	wr_idx = axnet_ivc_get_wr_cnt(&axnet->ep2h_full) % RING_COUNT;
	ep2h_data_msg[wr_idx].packet_size = len;
	ep2h_data_msg[wr_idx].busy_pcie_address = dst_iova;
	// mb();
	axnet_ivc_advance_wr(&axnet->ep2h_full);
	pci_epc_raise_irq(epc, epf->func_no, epf->vfunc_no, PCI_EPC_IRQ_MSI,
			  MSI_IRQ_NUM);

	/* Free temp src and skb */
#if !ENABLE_SIMPLE_DMA
	pci_epc_unmap_addr(epc, epf->func_no, epf->vfunc_no,
			   axnet->tx_dst_pci_addr);
#endif
	dma_unmap_single(cdev, src_iova, len, DMA_TO_DEVICE);
	dev_kfree_skb_any(skb);

	ndev->stats.tx_packets++;
	ndev->stats.tx_bytes += len;

	return NETDEV_TX_OK;
}

static const struct net_device_ops axnet_netdev_ops = {
	.ndo_open = axnet_ep_open,
	.ndo_stop = axnet_ep_close,
	.ndo_start_xmit = axnet_ep_start_xmit,
	.ndo_change_mtu = axnet_ep_change_mtu,
};

static void axnet_ep_process_ctrl_msg(struct pci_epf_axnet *axnet)
{
	struct ctrl_msg msg;

	pr_debug("axnet_ep_process_ctrl_msg\n");

	while (axnet_ivc_rd_available(&axnet->h2ep_ctrl)) {
		axnet_ep_read_ctrl_msg(axnet, &msg);
		if (msg.msg_id == CTRL_MSG_LINK_UP)
			axnet_ep_rcv_link_up_msg(axnet);
		else if (msg.msg_id == CTRL_MSG_LINK_DOWN)
			axnet_ep_rcv_link_down_msg(axnet);
		else if (msg.msg_id == CTRL_MSG_LINK_DOWN_ACK)
			axnet_ep_rcv_link_down_ack(axnet);
	}
}

static int axnet_ep_process_h2ep_msg(struct pci_epf_axnet *axnet)
{
	struct ep_ring_buf *ep_ring_buf = &axnet->ep_ring_buf;
	struct data_msg *data_msg = ep_ring_buf->h2ep_data_msgs;
	struct pci_epf *epf = axnet->epf;
	struct pci_epc *epc = epf->epc;
#if ENABLE_SIMPLE_DMA
	dma_addr_t dst_iova;
	struct device *cdev = epc->dev.parent;
	int ret;
#endif
#if !ENABLE_SIMPLE_DMA
	struct h2ep_empty_list *h2ep_empty_ptr;
#endif
	struct net_device *ndev = axnet->ndev;
	int count = 0;

	pr_debug("axnet ep process h2ep msg\n");

	while ((count < AXNET_NAPI_WEIGHT) &&
	       axnet_ivc_rd_available(&axnet->h2ep_full)) {
		struct sk_buff *skb;
		int idx;
		u32 len;
		u64 pcie_address;
#if !ENABLE_SIMPLE_DMA
		unsigned long flags;
		int found = 0;
#endif

		/* Read H2EP full msg */
		idx = axnet_ivc_get_rd_cnt(&axnet->h2ep_full) % RING_COUNT;
		len = data_msg[idx].packet_size;
		pcie_address = data_msg[idx].busy_pcie_address;
		mb();
		pr_debug("index:%d, pcie_address:%llx, size:%d\n", idx,
			 pcie_address, len);

#if !ENABLE_SIMPLE_DMA
		/* Get H2EP msg pointer from saved list */
		spin_lock_irqsave(&axnet->h2ep_empty_lock, flags);
		list_for_each_entry(h2ep_empty_ptr, &axnet->h2ep_empty_list,
				    list) {
			if (h2ep_empty_ptr->iova == pcie_address) {
				found = 1;
				break;
			}
		}
		spin_unlock_irqrestore(&axnet->h2ep_empty_lock, flags);

		if (!found) {
			pr_debug("pcie_address addr:%llx", (u64)&(data_msg[idx].busy_pcie_address));
			pr_info
			    ("warnning, index:%d, pcie_address:%llx, size:%d  not found\n",
			     idx, pcie_address, len);
			axnet_ivc_advance_rd(&axnet->h2ep_full);
			axnet_ivc_advance_wr(&axnet->h2ep_empty);
			mb();
			ndev->stats.rx_errors++;
			count++;
			continue;	//jump this packet
		}
#endif

		/* Advance H2EP full buffer after search in local list */
		axnet_ivc_advance_rd(&axnet->h2ep_full);

		/*
		 * If H2EP network queue is stopped due to lack of H2EP_FULL
		 * queue, raising ctrl irq will help.
		 */
		// pci_epc_raise_irq(epc, epf->func_no, epf->vfunc_no, PCI_EPC_IRQ_MSI, CTRL_MSI_IRQ_NUM);

		/* Alloc new skb and copy data from full buffer */
		skb = netdev_alloc_skb(ndev, len);
		if (!skb) {
			pr_err("warning: RX netdev_alloc_skb failed, len:%d\n", len);
			ndev->stats.rx_dropped++;
			count++;
			continue; //jump this packet
		}
#if ENABLE_SIMPLE_DMA
		pr_debug("start recv packet\n");

		dst_iova =
		    dma_map_single(cdev, skb->data, len, DMA_FROM_DEVICE);
		if (dma_mapping_error(cdev, dst_iova)) {
			pr_err("%s: dma_map_single failed\n", __func__);
			dev_kfree_skb_any(skb);
			ndev->stats.rx_dropped++;
			count++;
			continue;  //jump this packet
		}

		ret = ax_pcie_dma_read(0, pcie_address, dst_iova, len);
		if (ret) {
			pr_err("receive packet failed\n");
			ndev->stats.rx_errors++;
			count++;
			continue;  //jump this packet
		}

		// dma_sync_single_for_cpu(cdev, dst_iova, len, DMA_FROM_DEVICE);
		dma_unmap_single(cdev, dst_iova, len, DMA_FROM_DEVICE);

		axnet_ivc_advance_wr(&axnet->h2ep_empty);
		mb();
		// pci_epc_raise_irq(epc, epf->func_no, epf->vfunc_no, PCI_EPC_IRQ_MSI, CTRL_MSI_IRQ_NUM);
#if 0
		char *buf = skb->data;
		int i;

		for (i = 0; i < len; i++) {
			printk("%x", ((unsigned char *)buf)[i]);
		}
		printk("\n\n");
#endif
#else
		memcpy_fromio(skb->data, h2ep_empty_ptr->virt, len);
#endif
		skb_put(skb, len);
		skb->protocol = eth_type_trans(skb, ndev);
		napi_gro_receive(&axnet->napi, skb);

		ndev->stats.rx_packets++;
		ndev->stats.rx_bytes += len;

#if !ENABLE_SIMPLE_DMA
		axnet_ivc_advance_wr(&axnet->h2ep_empty);
		mb();
		pci_epc_raise_irq(epc, epf->func_no, epf->vfunc_no,
				  PCI_EPC_IRQ_MSI, MSI_IRQ_NUM);
#endif
		count++;
	}

	/*
	* If H2EP network queue is stopped due to lack of H2EP_FULL
	* queue, raising ctrl irq will help.
	*/
	pci_epc_raise_irq(epc, epf->func_no, epf->vfunc_no, PCI_EPC_IRQ_MSI,
			  MSI_IRQ_NUM);

	return count;
}

static void axnet_ep_ctrl_irq_callback(struct pci_epf_axnet *axnet)
{
	struct net_device *ndev = axnet->ndev;

	if (netif_queue_stopped(ndev)) {
		if ((axnet->os_link_state == OS_LINK_STATE_UP) &&
		    axnet_ivc_rd_available(&axnet->ep2h_empty) &&
		    !axnet_ivc_full(&axnet->ep2h_full)) {
			pr_debug("%s: wakeup net tx queue\n", __func__);
			netif_wake_queue(ndev);
		}
	}

	if (axnet_ivc_rd_available(&axnet->h2ep_ctrl))
		axnet_ep_process_ctrl_msg(axnet);
}

static void axnet_ep_data_irq_callback(struct pci_epf_axnet *axnet)
{
	if (axnet_ivc_rd_available(&axnet->h2ep_full)) {
		//disable mailbox interrupt
		enable_mailbox_int(axnet, 0);
		napi_schedule(&axnet->napi);
	}
}

#if !USE_MAILBOX
void time_pre(struct timer_list *timer);
struct timer_list mytimer;

void time_pre(struct timer_list *timer)
{
	int val1, val2;
	struct ep_ring_buf *ep_ring_buf = &g_axnet->ep_ring_buf;
	struct irq_md *irq = ep_ring_buf->h2ep_irq_msgs;

	val1 = readl(&(irq->irq_ctrl));
	if (val1 == 0xaa) {
		pr_debug("ctrl irq\n");
		writel(0, &(irq->irq_ctrl));
		axnet_ep_ctrl_irq_callback(g_axnet);
	}

	val2 = readl(&(irq->irq_data));
	if (val2 == 0xbb) {
		pr_debug("data irq\n");
		writel(0, &(irq->irq_data));
		axnet_ep_data_irq_callback(g_axnet);
	}

	mytimer.expires = jiffies + 1 * HZ / 1000;
	mod_timer(&mytimer, mytimer.expires);
}

int my_timer_init(struct pci_epf_axnet *axnet)
{
	timer_setup(&mytimer, time_pre, 0);
	mytimer.expires = jiffies + 1 * HZ / 1000;
	add_timer(&mytimer);

	pr_info("init timer success\n");
	return 0;
}

void __exit my_timer_exit(void)
{
	if (timer_pending(&mytimer)) {
		del_timer(&mytimer);
	}
	pr_info("exit timer Success \n");
}
#endif

static int axnet_ep_poll(struct napi_struct *napi, int budget)
{
	struct pci_epf_axnet *axnet = container_of(napi, struct pci_epf_axnet,
						   napi);
	int work_done;

	pr_debug("napi poll\n");

	work_done = axnet_ep_process_h2ep_msg(axnet);
	pr_debug("work_done: %d budget: %d\n", work_done, budget);
	if (work_done < budget) {
		napi_complete(napi);
		//enable mailbox interrupt
		enable_mailbox_int(axnet, 1);
	}

	return work_done;
}

#if SUPPORT_LINKUP
static void axnet_ep_pci_epf_linkup(struct pci_epf *epf)
{
	struct pci_epf_axnet *axnet = epf_get_drvdata(epf);

	axnet->pcie_link_status = true;
}
#endif

static void axnet_ep_setup_bar0_md(struct pci_epf_axnet *axnet)
{
	struct bar_md *bar_md;
	struct ep_ring_buf *ep_ring_buf = &axnet->ep_ring_buf;
	struct host_ring_buf *host_ring_buf = &axnet->host_ring_buf;

	bar_md = axnet->bar_md;

	/* Update BAR metadata region with offsets */
	/* EP owned memory */
	bar_md->ep_own_cnt_offset = axnet->bar0_amap[META_DATA].size;
	bar_md->h2ep_ctrl_md.offset =
	    bar_md->ep_own_cnt_offset + sizeof(struct fifo_cnt);
	bar_md->h2ep_ctrl_md.size = RING_COUNT;
	bar_md->h2ep_data_md.offset =
	    bar_md->h2ep_ctrl_md.offset +
	    (RING_COUNT * sizeof(struct ctrl_msg));
	bar_md->h2ep_data_md.size = RING_COUNT;
	/* he2p irq */
	bar_md->irq_offset =
	    bar_md->h2ep_data_md.offset +
	    (RING_COUNT * sizeof(struct data_msg));

	/* Host owned memory */
	bar_md->host_own_cnt_offset =
	    bar_md->ep_own_cnt_offset + axnet->bar0_amap[EP_MEM].size;
	bar_md->ep2h_ctrl_md.offset =
	    bar_md->host_own_cnt_offset + sizeof(struct fifo_cnt);
	bar_md->ep2h_ctrl_md.size = RING_COUNT;
	bar_md->ep2h_data_md.offset =
	    bar_md->ep2h_ctrl_md.offset +
	    (RING_COUNT * sizeof(struct ctrl_msg));
	bar_md->ep2h_data_md.size = RING_COUNT;

	// ctrl send & recv
	axnet->h2ep_ctrl.rd = &ep_ring_buf->ep_cnt->ctrl_rd_cnt;
	axnet->h2ep_ctrl.wr = &ep_ring_buf->ep_cnt->ctrl_wr_cnt;
	axnet->ep2h_ctrl.rd = &host_ring_buf->host_cnt->ctrl_rd_cnt;
	axnet->ep2h_ctrl.wr = &host_ring_buf->host_cnt->ctrl_wr_cnt;

	// ep recv
	axnet->h2ep_empty.rd = &ep_ring_buf->ep_cnt->fifo_rd_cnt;
	axnet->h2ep_empty.wr = &ep_ring_buf->ep_cnt->fifo_wr_cnt;
	axnet->h2ep_full.rd = &ep_ring_buf->ep_cnt->data_rd_cnt;
	axnet->h2ep_full.wr = &ep_ring_buf->ep_cnt->data_wr_cnt;

	// ep send
	axnet->ep2h_empty.rd = &host_ring_buf->host_cnt->fifo_rd_cnt;
	axnet->ep2h_empty.wr = &host_ring_buf->host_cnt->fifo_wr_cnt;
	axnet->ep2h_full.rd = &host_ring_buf->host_cnt->data_rd_cnt;
	axnet->ep2h_full.wr = &host_ring_buf->host_cnt->data_wr_cnt;
}

static int axnet_ep_pci_epf_bind(struct pci_epf *epf)
{
	struct pci_epf_axnet *axnet = epf_get_drvdata(epf);
	struct pci_epc *epc = epf->epc;
	struct pci_epf_header *header = epf->header;
	struct device *fdev = &epf->dev;
	struct ep_ring_buf *ep_ring_buf = &axnet->ep_ring_buf;
	struct host_ring_buf *host_ring_buf = &axnet->host_ring_buf;
	struct net_device *ndev;
	struct bar_md *bar_md;
	struct bar0_amap *amap;
	int ret, size;
#if !ENABLE_SIMPLE_DMA
	int bitmap_size;
#endif
	struct pci_epf_bar *epf_bar;
#if USE_MAILBOX
	struct pci_epf_bar *epf1_bar;
#endif

	pr_info("axnet pci epf bind\n");

	axnet->epc = epc;

	ret = pci_epc_write_header(epc, epf->func_no, epf->vfunc_no, header);
	if (ret < 0) {
		dev_err(fdev, "pci_epc_write_header() failed: %d\n", ret);
		goto fail;
	}

	axnet->bar0_va = pci_epf_alloc_space(epf, BAR0_SIZE, BAR_0,
					     0, PRIMARY_INTERFACE);
	if (!axnet->bar0_va) {
		dev_err(fdev, "Failed to allocate space for BAR%d\n", BAR_0);
		return -1;
	}

	epf_bar = &epf->bar[BAR_0];
	//epf_bar->flags |= PCI_BASE_ADDRESS_MEM_TYPE_64;
	axnet->bar0_iova = epf_bar->phys_addr;
	pr_info("MEM for BAR0, virt addr: 0x%llx, phy addr: 0x%llx, size:%x \n",
		(u64) axnet->bar0_va, axnet->bar0_iova, BAR0_SIZE);
	pr_info("ring buf size: %d\n", RING_COUNT);
#if 0
	// int i=0;
	// for(i=0; i < BAR0_SIZE/4 ; i++)
	//      *(u32 *)(axnet->bar0_va + i*4) = 0x88888888;

	//test BAR0 access
	memset(axnet->bar0_va, 0, 8);
	((int *)axnet->bar0_va)[0] = 0x11111111;
	((int *)axnet->bar0_va)[1] = 0x22222222;
#else
	/* 1. BAR0 meta data memory allocation */
	axnet->bar0_amap[META_DATA].iova = axnet->bar0_iova;
	axnet->bar0_amap[META_DATA].size = PAGE_SIZE;
	axnet->bar0_amap[META_DATA].virt = axnet->bar0_va;

	axnet->bar_md = (struct bar_md *)axnet->bar0_amap[META_DATA].virt;
	bar_md = axnet->bar_md;

	/* 2. BAR0 EP memory allocation */
	amap = &axnet->bar0_amap[EP_MEM];
	amap->iova =
	    axnet->bar0_amap[META_DATA].iova + axnet->bar0_amap[META_DATA].size;
	size =
	    sizeof(struct fifo_cnt) +
	    (RING_COUNT * (sizeof(struct ctrl_msg) + sizeof(struct data_msg))) +
	    sizeof(struct irq_md);
	amap->size = PAGE_ALIGN(size);
	amap->virt = axnet->bar0_va + axnet->bar0_amap[META_DATA].size;
	pr_debug("EP_MEM, amap->iova:%llx, amap->virt:%llx, amap->size:%d\n",
		amap->iova, (u64) amap->virt, amap->size);

	ep_ring_buf->ep_cnt = (struct fifo_cnt *)amap->virt;
	ep_ring_buf->h2ep_ctrl_msgs =
	    (struct ctrl_msg *)(ep_ring_buf->ep_cnt + 1);
	ep_ring_buf->h2ep_data_msgs =
	    (struct data_msg *)(ep_ring_buf->h2ep_ctrl_msgs + RING_COUNT);
	ep_ring_buf->h2ep_irq_msgs =
	    (struct irq_md *)(ep_ring_buf->h2ep_data_msgs + RING_COUNT);

	/* Clear EP counters */
	memset(ep_ring_buf->ep_cnt, 0, sizeof(struct fifo_cnt));
	pr_debug
	    ("ep_ring_buf->ep_cnt:%llx, ep_ring_buf->h2ep_ctrl_msgs:%llx, ep_ring_buf->h2ep_data_msgs:%llx\n",
	     (u64) ep_ring_buf->ep_cnt, (u64) ep_ring_buf->h2ep_ctrl_msgs,
	     (u64) ep_ring_buf->h2ep_data_msgs);

	/* 3. BAR0 host memory allocation */
	amap = &axnet->bar0_amap[HOST_MEM];
	amap->iova =
	    axnet->bar0_amap[EP_MEM].iova + axnet->bar0_amap[EP_MEM].size;
	size =
	    (sizeof(struct fifo_cnt)) +
	    (RING_COUNT * (sizeof(struct ctrl_msg) + sizeof(struct data_msg)));
	amap->size = PAGE_ALIGN(size);
	amap->virt =
	    axnet->bar0_va + axnet->bar0_amap[META_DATA].size +
	    axnet->bar0_amap[EP_MEM].size;
	pr_debug("HOST_MEM, amap->iova:%llx, amap->virt:%llx, amap->size:%d\n",
		 amap->iova, (u64) amap->virt, amap->size);

	host_ring_buf->host_cnt = (struct fifo_cnt *)amap->virt;
	host_ring_buf->ep2h_ctrl_msgs =
	    (struct ctrl_msg *)(host_ring_buf->host_cnt + 1);
	host_ring_buf->ep2h_data_msgs =
	    (struct data_msg *)(host_ring_buf->ep2h_ctrl_msgs + RING_COUNT);

	/* Clear host counters */
	memset(host_ring_buf->host_cnt, 0, sizeof(struct fifo_cnt));
	pr_debug
	    ("host_ring_buf->host_cnt:%llx, host_ring_buf->ep2h_ctrl_msgs:%llx, host_ring_buf->ep2h_data_msgs:%llx\n",
	     (u64) host_ring_buf->host_cnt, (u64) host_ring_buf->ep2h_ctrl_msgs,
	     (u64) host_ring_buf->ep2h_data_msgs);

#if !ENABLE_SIMPLE_DMA
	/* 4. EP Rx pkt IOVA range */
	axnet->rx_buf_iova =
	    axnet->bar0_amap[HOST_MEM].iova + axnet->bar0_amap[HOST_MEM].size;
	bar_md->bar0_base_phy = axnet->bar0_iova;
	bar_md->ep_rx_pkt_offset =
	    bar_md->host_own_cnt_offset + axnet->bar0_amap[HOST_MEM].size;
	bar_md->ep_rx_pkt_size =
	    BAR0_SIZE - axnet->bar0_amap[META_DATA].size -
	    axnet->bar0_amap[EP_MEM].size - axnet->bar0_amap[HOST_MEM].size;

	/* Create bitmap for allocating RX buffers */
	axnet->rx_num_pages = (bar_md->ep_rx_pkt_size >> PAGE_SHIFT);
	bitmap_size = BITS_TO_LONGS(axnet->rx_num_pages) * sizeof(long);
	axnet->rx_buf_bitmap = devm_kzalloc(fdev, bitmap_size, GFP_KERNEL);
	if (!axnet->rx_buf_bitmap) {
		dev_err(fdev, "rx_bitmap mem alloc failed\n");
		ret = -ENOMEM;
		goto free_host_mem;
	}
#endif

	axnet_ep_setup_bar0_md(axnet);
#endif

#if !ENABLE_SIMPLE_DMA
	/* Allocate PCIe memory for RP's dst address during xmit */
	axnet->tx_dst_va = pci_epc_mem_alloc_addr(epc,
						  &axnet->tx_dst_pci_addr,
						  SZ_64K);
	if (!axnet->tx_dst_va) {
		dev_err(fdev, "failed to allocate dst PCIe address\n");
		ret = -ENOMEM;
		goto free_host_mem;
	}
	pr_debug("tx_dst_va:%llx, tx_dst_pci_addr:%llx\n",
		 (u64) axnet->tx_dst_va, axnet->tx_dst_pci_addr);
#endif

	/* Register network device */
	ndev = alloc_etherdev(0);
	if (!ndev) {
		dev_err(fdev, "alloc_etherdev() failed\n");
		ret = -ENOMEM;
		goto free_pci_mem;
	}

	strcpy(ndev->name, INTERFACE_NAME);
	eth_hw_addr_random(ndev);
	axnet->ndev = ndev;
	SET_NETDEV_DEV(ndev, fdev);
	ndev->netdev_ops = &axnet_netdev_ops;
	netif_napi_add(ndev, &axnet->napi, axnet_ep_poll, AXNET_NAPI_WEIGHT);

	ndev->mtu = AXNET_DEFAULT_MTU;

	ret = register_netdev(ndev);
	if (ret < 0) {
		dev_err(fdev, "register_netdev() failed: %d\n", ret);
		goto fail_free_netdev;
	}
	netif_carrier_off(ndev);

	axnet->rx_link_state = DIR_LINK_STATE_DOWN;
	axnet->tx_link_state = DIR_LINK_STATE_DOWN;
	axnet->os_link_state = OS_LINK_STATE_DOWN;
	mutex_init(&axnet->link_state_lock);
	init_waitqueue_head(&axnet->link_state_wq);

	INIT_LIST_HEAD(&axnet->h2ep_empty_list);
	spin_lock_init(&axnet->h2ep_empty_lock);

#if ENABLE_SIMPLE_DMA
	spin_lock_init(&axnet->dma_wr_lock);
	spin_lock_init(&axnet->dma_rd_lock);
#endif

	/* TODO Update it to 64-bit prefetch type */
	ret = pci_epc_set_bar(epc, epf->func_no, epf->vfunc_no, epf_bar);
	if (ret < 0) {
		dev_err(fdev, "pci_epc_set_bar() failed: %d\n", ret);
		goto fail_unreg_netdev;
	}

	pr_debug("addr[0]: %x, addr[1]: %x \b", ((int *)axnet->bar0_va)[0],
		 ((int *)axnet->bar0_va)[1]);

#if USE_MAILBOX
	epf1_bar = &epf->bar[BAR_1];
	epf1_bar->phys_addr = MAILBOX_REG_MAP_ADDR;
	epf1_bar->size = 65536;
	epf1_bar->barno = BAR_1;

	ret = pci_epc_set_bar(epc, epf->func_no, epf->vfunc_no, epf1_bar);
	if (ret < 0) {
		dev_err(fdev, "pci_epc_set_bar() failed: %d\n", ret);
		goto fail_unreg_netdev;
	}
#endif

	ret =
	    pci_epc_set_msi(epc, epf->func_no, epf->vfunc_no,
			    epf->msi_interrupts);
	if (ret) {
		dev_err(fdev, "pci_epc_set_msi() failed: %d\n", ret);
		goto fail_clear_bar;
	}

	bar_md->link_magic = EP_LINK_MAGIC;
	bar_md->ep_link_status = DIR_LINK_STATE_DOWN;

	g_axnet = axnet;

#if USE_MAILBOX
	slave_request_message_irq(slave_message_irq_handler);
#else
	my_timer_init(axnet);
#endif

	return 0;

fail_clear_bar:
	pci_epc_clear_bar(epc, epf->func_no, epf->vfunc_no, BAR_0);
fail_unreg_netdev:
	unregister_netdev(ndev);
fail_free_netdev:
	netif_napi_del(&axnet->napi);
	free_netdev(ndev);
free_pci_mem:
#if !ENABLE_SIMPLE_DMA
	pci_epc_mem_free_addr(epc, axnet->tx_dst_pci_addr, axnet->tx_dst_va,
			      SZ_64K);
#endif
#if !ENABLE_SIMPLE_DMA
free_host_mem:
#endif
	pci_epf_free_space(epf, axnet->bar0_va, BAR_0, PRIMARY_INTERFACE);
fail:
	return ret;
}

static void axnet_ep_pci_epf_unbind(struct pci_epf *epf)
{
	struct pci_epf_axnet *axnet = epf_get_drvdata(epf);
	struct pci_epc *epc = epf->epc;

	pr_info("axnet pci epf unbind\n");

	//call this first, it will call net close func
	unregister_netdev(axnet->ndev);

	/* disable ltssm will cause rc root port config reg clear in ax650 */
	/* pci_epc_stop(epc); */
	pci_epc_clear_bar(epc, epf->func_no, epf->vfunc_no, &epf->bar[BAR_0]);
	pci_epf_free_space(epf, axnet->bar0_va, BAR_0, PRIMARY_INTERFACE);
	pci_epc_clear_bar(epc, epf->func_no, epf->vfunc_no, &epf->bar[BAR_1]);

	netif_napi_del(&axnet->napi);
	free_netdev(axnet->ndev);

#if !ENABLE_SIMPLE_DMA
	pci_epc_mem_free_addr(epc, axnet->tx_dst_pci_addr, axnet->tx_dst_va,
			      SZ_64K);
#endif
}

static const struct pci_epf_device_id axnet_ep_epf_axnet_ids[] = {
	{.name = "pci_epf_axnet",},
	{},
};

int axnet_ep_epf_probe(struct pci_epf *epf)
{
	struct device *fdev = &epf->dev;
	struct pci_epf_axnet *axnet;

	pr_info("axnet ep epf probe\n");

	axnet = devm_kzalloc(fdev, sizeof(*axnet), GFP_KERNEL);
	if (!axnet)
		return -ENOMEM;

	epf_set_drvdata(epf, axnet);
	axnet->fdev = fdev;
	axnet->epf = epf;

	axnet->header.vendorid = PCI_VENDOR_ID_AX;
	axnet->header.deviceid = PCI_VENDOR_ID_AX_NET;
	axnet->header.revid = 0x0;
	axnet->header.baseclass_code = PCI_BASE_CLASS_NETWORK;
	axnet->header.subclass_code = (PCI_CLASS_NETWORK_OTHER & 0xff);
	axnet->header.subsys_vendor_id = 0x0;
	axnet->header.subsys_id = 0x0;
	axnet->header.interrupt_pin = PCI_INTERRUPT_INTA;
	epf->header = &axnet->header;

	return 0;
}

void axnet_ep_epf_remove(struct pci_epf *epf)
{
	pr_info("axnet ep epf remove\n");
}

static struct pci_epf_ops axnet_ep_ops = {
	.bind = axnet_ep_pci_epf_bind,
	.unbind = axnet_ep_pci_epf_unbind,
#if SUPPORT_LINKUP
	.linkup = axnet_ep_pci_epf_linkup,
#endif
};

static struct pci_epf_driver axnet_driver = {
	.driver.name = "pci_epf_axnet",
	.probe = axnet_ep_epf_probe,
	.remove = axnet_ep_epf_remove,
	.id_table = axnet_ep_epf_axnet_ids,
	.ops = &axnet_ep_ops,
	.owner = THIS_MODULE,
};

static int __init axnet_ep_pci_epf_init(void)
{
	int ret;

	pr_info("axnet ep pcie epf init\n");
	ret = pci_epf_register_driver(&axnet_driver);
	if (ret) {
		pr_err("Failed to register EPF AX vnet driver: %d\n", ret);
		return ret;
	}

	return 0;
}

module_init(axnet_ep_pci_epf_init);

static void __exit axnet_ep_pci_epf_exit(void)
{
	pr_info("axnet ep pcie epf exit\n");
	pci_epf_unregister_driver(&axnet_driver);
}

module_exit(axnet_ep_pci_epf_exit);

MODULE_DESCRIPTION("PCI EPF AX VIRTUAL NETWORK DRIVER");
MODULE_AUTHOR("AX");
MODULE_LICENSE("GPL v2");

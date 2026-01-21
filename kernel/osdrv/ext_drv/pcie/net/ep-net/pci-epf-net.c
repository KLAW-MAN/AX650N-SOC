#include <linux/dma-iommu.h>
#include <linux/iommu.h>
#include <linux/etherdevice.h>
#include <linux/module.h>
#include <linux/netdevice.h>
#include <linux/of_platform.h>
#include <linux/platform_device.h>
#include <linux/workqueue.h>

#include "../../include/ax_pcie_net.h"
#include "../../include/ax_pcie_dev.h"

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
	struct device *fdev;
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
#ifdef SHMEM_FROM_MASTER
	int link_up_checked;
	u64 ob_base;
	u64 ob_size;
	void *ob_base_virt;
	void *ob_end_virt;
#endif

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
	int opened;
};

static struct pci_epf_axnet *g_axnet;
static void axnet_ep_ctrl_irq_callback(struct pci_epf_axnet *axnet);
static void axnet_ep_data_irq_callback(struct pci_epf_axnet *axnet);


static int axnet_pcie_raise_msi_irq(unsigned int msi_irq_num)
{
	g_axera_pcie_opt->trigger_msg_irq(NULL, msi_irq_num);

	return 0;
}

phys_addr_t axnet_alloc_pci_addr(int size)
{
	return 0x50000000;
}

#if ENABLE_SIMPLE_DMA
#define PCIE_SPACE_NET_BASE	0x51400000
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

#define PCIE_DMA_CHAN  1

static void axnet_pcie_write_reg(u32 reg, u32 val)
{
	ax_pcie_dbi_write(reg, val);
}

static u32 axnet_pcie_read_reg(u32 reg)
{
	u32 val;
	ax_pcie_dbi_read(reg, &val);

	return val;
}

static int axnet_pcie_dma_write(int chan, u64 src, u64 dst, u32 size)
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
	axnet_pcie_write_reg(PCIE_DMA_BASE + chan * CHAN_ADDR_BASE + WR_CHAN_EN, 0x1);	//HDMA WR channel enable
	axnet_pcie_write_reg(PCIE_DMA_BASE + chan * CHAN_ADDR_BASE + WR_CHAN_TRAN_SIZE, size);	//HDMA WR channel transfer size
	axnet_pcie_write_reg(PCIE_DMA_BASE + chan * CHAN_ADDR_BASE + WR_SAR_LOW_ADDR, low_src_addr);	//HDMA WR SAR LOW Address
	axnet_pcie_write_reg(PCIE_DMA_BASE + chan * CHAN_ADDR_BASE + WR_SAR_UPPER_ADDR, upper_src_addr);	//HDMA WR SAR HIGH Address
	axnet_pcie_write_reg(PCIE_DMA_BASE + chan * CHAN_ADDR_BASE + WR_DAR_LOW_ADDR, low_dst_addr);	//HDMA WR DAR LOW Address
	axnet_pcie_write_reg(PCIE_DMA_BASE + chan * CHAN_ADDR_BASE + WR_DAR_UPPER_ADDR, upper_dst_addr);	//HDMA WR DAR HIGH Address
	axnet_pcie_write_reg(PCIE_DMA_BASE + chan * CHAN_ADDR_BASE + WR_DOORBELL_EN, 0x1);	//HDMA WR doorbell start

	for (i = 0; i < retry; i++) {
		/* 0x1 runing; 0x2 abort; 0x3 stop */
		val = axnet_pcie_read_reg(PCIE_DMA_BASE + chan * CHAN_ADDR_BASE + WR_CHAN_STATUS);	//HDMA WR channel status
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

static int axnet_pcie_dma_read(int chan, u64 src, u64 dst, u32 size)
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
	axnet_pcie_write_reg(PCIE_DMA_BASE + chan * CHAN_ADDR_BASE + RD_CHAN_EN, 0x1);	//HDMA RD channel enable
	axnet_pcie_write_reg(PCIE_DMA_BASE + chan * CHAN_ADDR_BASE + RD_CHAN_TRAN_SIZE, size);	//HDMA RD channel transfer size
	axnet_pcie_write_reg(PCIE_DMA_BASE + chan * CHAN_ADDR_BASE + RD_SAR_LOW_ADDR, low_src_addr);	//HDMA RD SAR LOW Address
	axnet_pcie_write_reg(PCIE_DMA_BASE + chan * CHAN_ADDR_BASE + RD_SAR_UPPER_ADDR, upper_src_addr);	//HDMA RD SAR HIGH Address
	axnet_pcie_write_reg(PCIE_DMA_BASE + chan * CHAN_ADDR_BASE + RD_DAR_LOW_ADDR, low_dst_addr);	//HDMA RD DAR LOW Address
	axnet_pcie_write_reg(PCIE_DMA_BASE + chan * CHAN_ADDR_BASE + RD_DAR_UPPER_ADDR, upper_dst_addr);	//HDMA RD DAR HIGH Address
	axnet_pcie_write_reg(PCIE_DMA_BASE + chan * CHAN_ADDR_BASE + RD_DOORBELL_EN, 0x1);	//HDMA RD doorbell start

	for (i = 0; i < retry; i++) {
		/* 0x1 runing; 0x2 abort; 0x3 stop */
		val = axnet_pcie_read_reg(PCIE_DMA_BASE + chan * CHAN_ADDR_BASE + RD_CHAN_STATUS);	//HDMA RD channel status
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

// static void axnet_update_irq_cnt(void)
// {
//      struct ep_ring_buf *ep_ring_buf = &g_axnet->ep_ring_buf;
//      struct irq_md *irq = ep_ring_buf->h2ep_irq_msgs;

//      u32 irq_cnt = readl(&(irq->mailbox_irq_cnt));
//      pr_debug("mailbox irq cnt: %d\n", irq_cnt);

//      if(irq_cnt < 8)
//      writel(irq_cnt + 1, &(irq->mailbox_irq_cnt));
// }

static void axnet_mem_info_set(struct pci_epf_axnet *axnet)
{
	struct bar_md *bar_md;
	struct bar0_amap *amap;
	struct ep_ring_buf *ep_ring_buf;
	struct host_ring_buf *host_ring_buf;
	struct axera_dev *ax_dev = g_axera_dev_map[0];
	struct device *dev;
	int size;
	int bar0_size;

	dev = &ax_dev->pdev->dev;
	bar0_size = ax_dev->bar_info[BAR_0].size / 2;
	ep_ring_buf = &axnet->ep_ring_buf;
	host_ring_buf = &axnet->host_ring_buf;

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

	/* Clear EP counters in RC */
	// memset(ep_ring_buf->ep_cnt, 0, sizeof(struct fifo_cnt));
	memset(ep_ring_buf->h2ep_irq_msgs, 0, sizeof(struct irq_md));
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

	/* Clear host counters in RC */
	// memset(host_ring_buf->host_cnt, 0, sizeof(struct fifo_cnt));
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
	    bar0_size - axnet->bar0_amap[META_DATA].size -
	    axnet->bar0_amap[EP_MEM].size - axnet->bar0_amap[HOST_MEM].size;

	/* Create bitmap for allocating RX buffers */
	axnet->rx_num_pages = (bar_md->ep_rx_pkt_size >> PAGE_SHIFT);
	bitmap_size = BITS_TO_LONGS(axnet->rx_num_pages) * sizeof(long);
	axnet->rx_buf_bitmap = devm_kzalloc(dev, bitmap_size, GFP_KERNEL);
	if (!axnet->rx_buf_bitmap) {
		dev_err(dev, "rx_bitmap mem alloc failed\n");
	}
#endif

	bar_md->link_magic = EP_LINK_MAGIC;
	bar_md->ep_link_status = DIR_LINK_STATE_DOWN;
	smp_mb();

}

static void axnet_update_irq_cnt(void)
{
	struct ep_ring_buf *ep_ring_buf = &g_axnet->ep_ring_buf;
	struct irq_md *irq = ep_ring_buf->h2ep_irq_msgs;
	unsigned long flags;

	if (!irq)
		return;

	g_axnet->irq_cnt++;
	ax_pcie_net_lock(&flags);
	writeq(g_axnet->irq_cnt, &(irq->rd_cnt));
	ax_pcie_net_unlock(&flags);
	mb();

	pr_debug("irq->rd_cnt:%lld\n", g_axnet->irq_cnt);
}

static int axnet_clear_irq(int fromid, int regno, int count)
{
	unsigned int ret = 0;
	unsigned int msg[8];

	axnet_update_irq_cnt();

	ret = ax_mailbox_read(regno, (int *)&msg, count);
	pr_debug("ret = %d count = %d, msg[0] = %x\n", ret, count, msg[0]);
	if (ret != count) {
		pr_err("ax_mailbox_read failed, count:%d, ret:%d\n", count,
		       ret);
		return 0;
	}
	// axnet_update_irq_cnt();

	return 1;
}

static int
axnet_register_irq(void (*handler) (int fromid, int regno, int count))
{
	int ret = 0;
	ret = ax_mailbox_register_notify(PCIE_NET, (void *)handler);
	if (ret < 0)
		pr_err("ax mailbox register notify fail!\n");

	return ret;
}

void axnet_enable_mailbox_int(struct pci_epf_axnet *axnet, int en)
{
	struct ep_ring_buf *ep_ring_buf = &axnet->ep_ring_buf;
	struct irq_md *irq = ep_ring_buf->h2ep_irq_msgs;
	unsigned long flags;

	ax_pcie_net_lock(&flags);
	if (en) {
		pr_debug("enable mailbox irq\n");
		writel(1, &(irq->mailbox_int));
	} else {
		pr_debug("disable mailbox irq\n");
		writel(0, &(irq->mailbox_int));
	}
	ax_pcie_net_unlock(&flags);
}
#endif

static void axnet_ep_read_ctrl_msg(struct pci_epf_axnet *axnet,
				   struct ctrl_msg *msg)
{
	struct ep_ring_buf *ep_ring_buf = &axnet->ep_ring_buf;
	struct ctrl_msg *ctrl_msg = ep_ring_buf->h2ep_ctrl_msgs;
	unsigned long flags;
	u32 idx;

	if (axnet_ivc_empty(&axnet->h2ep_ctrl)) {
		dev_dbg(axnet->fdev, "%s: H2EP ctrl ring empty\n", __func__);
		return;
	}

	idx = axnet_ivc_get_rd_cnt(&axnet->h2ep_ctrl) % RING_COUNT;
	ax_pcie_net_lock(&flags);
	memcpy(msg, &ctrl_msg[idx], sizeof(*msg));
	ax_pcie_net_unlock(&flags);
	axnet_ivc_advance_rd(&axnet->h2ep_ctrl);
}


/* TODO Handle error case */
static int axnet_ep_write_ctrl_msg(struct pci_epf_axnet *axnet,
				   struct ctrl_msg *msg)
{
	struct host_ring_buf *host_ring_buf = &axnet->host_ring_buf;
	struct ctrl_msg *ctrl_msg = host_ring_buf->ep2h_ctrl_msgs;
	unsigned long flags;
	u32 idx;

	pr_debug("axnet_ep_write_ctrl_msg\n");

	if (axnet_ivc_full(&axnet->ep2h_ctrl)) {
		/* Raise an interrupt to let host process EP2H ring */
		axnet_pcie_raise_msi_irq(MSI_IRQ_NUM);
		dev_dbg(axnet->fdev, "%s: EP2H ctrl ring full\n", __func__);
		return -EAGAIN;
	}

	idx = axnet_ivc_get_wr_cnt(&axnet->ep2h_ctrl) % RING_COUNT;
	ax_pcie_net_lock(&flags);
	memcpy(&ctrl_msg[idx], msg, sizeof(*msg));
	ax_pcie_net_unlock(&flags);
	axnet_ivc_advance_wr(&axnet->ep2h_ctrl);
	axnet_pcie_raise_msi_irq(MSI_IRQ_NUM);

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

	axnet_pcie_raise_msi_irq(MSI_IRQ_NUM);
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
	unsigned long flags;

	ax_pcie_net_lock(&flags);
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
	ax_pcie_net_unlock(&flags);
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
	unsigned long flags;

#if !ENABLE_SIMPLE_DMA
	axnet_ep_free_empty_buffers(axnet);
#endif
	axnet->rx_link_state = DIR_LINK_STATE_DOWN;
	ax_pcie_net_lock(&flags);
	axnet->bar_md->ep_link_status = DIR_LINK_STATE_DOWN;
	ax_pcie_net_unlock(&flags);
	wake_up_interruptible(&axnet->link_state_wq);
	axnet_ep_update_link_sm(axnet);
}

static int axnet_pcie_rc_net_is_up(struct pci_epf_axnet *axnet)
{
	int link_status;
	u64 val;

	val = axnet->bar_md->rc_link_status;

	if (val == DIR_LINK_STATE_UP) {
		pr_debug("link is up\n");
		link_status = 1;
	} else {
		pr_debug("link is down\n");
		link_status = 0;
	}

	return link_status;
}

static int axnet_ep_open(struct net_device *ndev)
{
	struct pci_epf_axnet *axnet = netdev_priv(ndev);

	pr_info("open pcie net\n");

#ifdef SHMEM_FROM_MASTER
	if (axnet->link_up_checked != DEVICE_CHECKED_FLAG) {
		return 0;
	}
#endif

	/* if rc send linkup req before ep driver ready, ep will miss this req
	   so we need check rc linkup req */
	if (axnet_pcie_rc_net_is_up(axnet)) {
		axnet->tx_link_state = DIR_LINK_STATE_UP;
	}

	mutex_lock(&axnet->link_state_lock);
	if (axnet->opened) {
		mutex_unlock(&axnet->link_state_lock);
		return 0;
	}
	if (axnet->rx_link_state == DIR_LINK_STATE_DOWN)
		axnet_ep_user_link_up_req(axnet);
	napi_enable(&axnet->napi);
	axnet->opened = 1;
	mutex_unlock(&axnet->link_state_lock);

	return 0;
}

static int axnet_ep_close(struct net_device *ndev)
{
	struct pci_epf_axnet *axnet = netdev_priv(ndev);
	int ret = 0;

	pr_info("close pcie net\n");

	mutex_lock(&axnet->link_state_lock);
	axnet->opened = 0;
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
	struct pci_epf_axnet *axnet = netdev_priv(ndev);
	struct host_ring_buf *host_ring_buf = &axnet->host_ring_buf;
	struct data_msg *ep2h_data_msg = host_ring_buf->ep2h_data_msgs;
	struct skb_shared_info *info = skb_shinfo(skb);
	dma_addr_t src_iova;
	u32 rd_idx, wr_idx;
	unsigned long flags;
#if ENABLE_SIMPLE_DMA
	u64 dst_iova;
	int dst_len, len;
	int ret;
#else
	u64 dst_masked, dst_off, dst_iova;
	int dst_len, len;
#endif

	pr_debug("start xmit packet\n");

	/*TODO Not expecting skb frags, remove this after testing */
	WARN_ON(info->nr_frags);

	/* Check if EP2H_EMPTY_BUF available to read */
	if (!axnet_ivc_rd_available(&axnet->ep2h_empty)) {
		axnet_pcie_raise_msi_irq(MSI_IRQ_NUM);
		pr_debug("%s: No EP2H empty msg, stop tx\n", __func__);
		netif_stop_queue(ndev);
		return NETDEV_TX_BUSY;
	}

	/* Check if EP2H_FULL_BUF available to write */
	if (axnet_ivc_full(&axnet->ep2h_full)) {
		axnet_pcie_raise_msi_irq(MSI_IRQ_NUM);
		pr_debug("%s: No EP2H full buf, stop tx\n", __func__);
		netif_stop_queue(ndev);
		return NETDEV_TX_BUSY;
	}

	len = skb_headlen(skb);

	src_iova = dma_map_single(fdev, skb->data, len, DMA_TO_DEVICE);
	if (dma_mapping_error(fdev, src_iova)) {
		dev_err(fdev, "%s: dma_map_single failed\n", __func__);
		dev_kfree_skb_any(skb);
		ndev->stats.tx_dropped++;
		return NETDEV_TX_OK;
	}

	/* Get EP2H empty msg */
	rd_idx = axnet_ivc_get_rd_cnt(&axnet->ep2h_empty) % RING_COUNT;
	ax_pcie_net_lock(&flags);
	dst_iova = ep2h_data_msg[rd_idx].free_pcie_address;
	dst_len = ep2h_data_msg[rd_idx].buffer_len;
	ax_pcie_net_unlock(&flags);
	mb();

#if !ENABLE_SIMPLE_DMA
	/*
	 * Map host dst mem to local PCIe address range.
	 * PCIe address range is SZ_64K aligned.
	 */
	dst_masked = (dst_iova & ~(SZ_64K - 1));
	dst_off = (dst_iova & (SZ_64K - 1));
	pr_debug("dst addr: %llx, dst_masked: %llx, dst_off: %llx, dst_len: %d\n", dst_iova,
		 dst_masked, dst_off, dst_len);

	g_axera_pcie_opt->start_ob_map(axnet->tx_dst_pci_addr, dst_masked, 0, dst_len);
#endif

	/*
	 * Advance read count after all failure cases completed, to avoid
	 * dangling buffer at host.
	 */
	axnet_ivc_advance_rd(&axnet->ep2h_empty);

#if ENABLE_SIMPLE_DMA
	/* Trigger DMA write from src_iova to dst_iova */
	ret = axnet_pcie_dma_write(PCIE_DMA_CHAN, src_iova, dst_iova, len);
	if (ret) {
		ndev->stats.tx_errors++;
		dma_unmap_single(fdev, src_iova, len, DMA_TO_DEVICE);
		dev_kfree_skb_any(skb);
		pr_err("transmit packet failed\n");
		return NETDEV_TX_OK;
	}
	mb();
#else
	pr_debug("transmit, dst:%llx, src:%llx, len:%d\n",
		 (u64) (axnet->tx_dst_va + dst_off), (u64) skb->data, len);
	/* Copy skb->data to host dst address, use CPU virt addr */
	axera_pcie_awmisc_enable(false);
	memcpy_toio((void *)(axnet->tx_dst_va + dst_off), skb->data, len);
	axera_pcie_awmisc_enable(true);
	/*
	 * tx_dst_va is ioremap_wc() mem, add mb to make sure complete skb->data
	 * written to dst before adding it to full buffer
	 */
	mb();
#endif

	/* Push dst to EP2H full ring */
	wr_idx = axnet_ivc_get_wr_cnt(&axnet->ep2h_full) % RING_COUNT;
	ax_pcie_net_lock(&flags);
	ep2h_data_msg[wr_idx].packet_size = len;
	ep2h_data_msg[wr_idx].busy_pcie_address = dst_iova;
	ax_pcie_net_unlock(&flags);
	mb();
	axnet_ivc_advance_wr(&axnet->ep2h_full);
	axnet_pcie_raise_msi_irq(MSI_IRQ_NUM);

	/* Free temp src and skb */
#if !ENABLE_SIMPLE_DMA
	// pci_epc_unmap_addr(epc, epf->func_no, epf->vfunc_no,
	// 		   axnet->tx_dst_pci_addr);
#endif
	dma_unmap_single(fdev, src_iova, len, DMA_TO_DEVICE);
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
	unsigned long flags;
#if ENABLE_SIMPLE_DMA
	dma_addr_t dst_iova;
	struct device *cdev = axnet->fdev;
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
		ax_pcie_net_lock(&flags);
		len = data_msg[idx].packet_size;
		pcie_address = data_msg[idx].busy_pcie_address;
		ax_pcie_net_unlock(&flags);
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
		// axnet_pcie_raise_msi_irq(MSI_IRQ_NUM);

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

		ret = axnet_pcie_dma_read(PCIE_DMA_CHAN, pcie_address, dst_iova, len);
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
		// axnet_pcie_raise_msi_irq(MSI_IRQ_NUM);
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
		axnet_pcie_raise_msi_irq(MSI_IRQ_NUM);
#endif
		count++;
	}

	/*
	* If H2EP network queue is stopped due to lack of H2EP_FULL
	* queue, raising ctrl irq will help.
	*/
	axnet_pcie_raise_msi_irq(MSI_IRQ_NUM);

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
		axnet_enable_mailbox_int(axnet, 0);
		napi_schedule(&axnet->napi);
	}
}


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
		axnet_enable_mailbox_int(axnet, 1);
	}

	return work_done;
}


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

#ifdef SHMEM_FROM_MASTER
static int axnet_handshake_host_step0(void)
{
	struct axera_dev *ax_dev = g_axera_dev_map[0];
	unsigned long shm_base;

	if (g_axnet->link_up_checked != DEVICE_CHECKED_FLAG) {
		if (*(volatile unsigned int *)(ax_dev->bar_info[BAR_0].addr + 0x20) == DEVICE_CHECKED_FLAG) {
			/* Clear host handshake flags */
			*(volatile unsigned int *)(ax_dev->bar_info[BAR_0].addr + 0x20) = 0x0;
			shm_base = *(volatile unsigned long *)(ax_dev->bar_info[BAR_0].addr + 0x30);
			pr_debug("ob shm_base = %lx\n", shm_base);
			g_axera_pcie_opt->start_ob_map(g_axnet->ob_base, shm_base, OUTBOUND_INDEX1, g_axnet->ob_size);
			*(volatile unsigned int *)(ax_dev->bar_info[BAR_0].addr + 0x24) = DEVICE_HANDSHAKE_FLAG;

			/* use the second half of bar0 mem in ep */
			g_axnet->bar0_va = g_axnet->ob_base_virt;
			if (!g_axnet->bar0_va) {
				pr_err("ob base address is null\n");
				return -1;
			}
			g_axnet->bar0_iova = g_axnet->ob_base;

			pr_debug("MEM for BAR0, virt addr: 0x%llx, phy addr: 0x%llx, size: 0x%llx \n",
				(u64) g_axnet->bar0_va, g_axnet->bar0_iova, g_axnet->ob_size);
			pr_debug("ring buf size: %d\n", RING_COUNT);

			axnet_mem_info_set(g_axnet);
			axnet_ep_setup_bar0_md(g_axnet);
			g_axnet->link_up_checked = DEVICE_CHECKED_FLAG;
			axnet_ep_open(g_axnet->ndev);
			return 1;
		}
	}
	return 0;
}
#endif

void axnet_handle_msg_and_data(void)
{
#if 0
	int val1, val2;
	struct ep_ring_buf *ep_ring_buf = &g_axnet->ep_ring_buf;
	struct irq_md *irq = ep_ring_buf->h2ep_irq_msgs;

	val1 = readl(&(irq->irq_ctrl));
	if (val1 == 0xaa) {
		pr_info("ctrl irq\n");
		writel(0, &(irq->irq_ctrl));
		axnet_ep_ctrl_irq_callback(g_axnet);
	}

	val2 = readl(&(irq->irq_data));
	if (val2 == 0xbb) {
		pr_info("data irq\n");
		writel(0, &(irq->irq_data));
		axnet_ep_data_irq_callback(g_axnet);
	}
#else
	pr_debug("handle msg\n");
#ifdef SHMEM_FROM_MASTER
	if (axnet_handshake_host_step0())
		return;
#endif

	axnet_ep_ctrl_irq_callback(g_axnet);
	axnet_ep_data_irq_callback(g_axnet);
#endif
}

void axnet_irq_handler(int fromid, int regno, int count)
{
	unsigned int status = 0;
	pr_debug("slave: mailbox msg handler, regno:%x\n", regno);

	status = axnet_clear_irq(fromid, regno, count);
	if (status)
		axnet_handle_msg_and_data();
}

static int __init axnet_ep_pci_epf_init(void)
{
	struct net_device *ndev;
	int ret;
#if !ENABLE_SIMPLE_DMA
	int bitmap_size;
#endif
	struct pci_epf_axnet *axnet;
#ifndef SHMEM_FROM_MASTER
	int bar0_size;
#endif
	struct axera_dev *ax_dev;
	struct device *dev;

	pr_info("axnet ep pcie epf init\n");

	ax_dev = g_axera_dev_map[0];
	if (ax_dev == NULL) {
		pr_err("pcie device not found\n");
		return -1;
	}
	dev = &ax_dev->pdev->dev;

	/* Register network device */
	ndev = alloc_etherdev(sizeof(struct pci_epf_axnet));
	if (!ndev) {
		dev_err(dev, "alloc_etherdev() failed\n");
		return -ENOMEM;
	}

	strcpy(ndev->name, INTERFACE_NAME);
	eth_hw_addr_random(ndev);
	SET_NETDEV_DEV(ndev, dev);
	ndev->netdev_ops = &axnet_netdev_ops;
	axnet = netdev_priv(ndev);
	netif_napi_add(ndev, &axnet->napi, axnet_ep_poll, AXNET_NAPI_WEIGHT);

	ndev->mtu = AXNET_DEFAULT_MTU;

	ret = register_netdev(ndev);
	if (ret < 0) {
		dev_err(dev, "register_netdev() failed: %d\n", ret);
		goto fail_free_netdev;
	}
	netif_carrier_off(ndev);

	g_axnet = axnet;
	axnet->fdev = dev;
	axnet->ndev = ndev;
	axnet->opened = 0;

#ifdef SHMEM_FROM_MASTER
	axnet->ob_base_virt = devm_ioremap(dev, PCIE_SPACE_NET_BASE, SZ_4M);
	axnet->ob_base = PCIE_SPACE_NET_BASE;
	axnet->ob_size = SZ_4M;
	pr_debug("ob base virt = %p\n", axnet->ob_base_virt);
#endif

#ifndef SHMEM_FROM_MASTER
	/* use the second half of bar0 mem in ep */
	axnet->bar0_va = ax_dev->bar_info[BAR_0].addr + ax_dev->bar_info[BAR_0].size / 2;
	if (!axnet->bar0_va) {
		dev_err(dev, "Failed to allocate space for BAR%d\n", BAR_0);
		ret = -1;
		goto fail_unreg_netdev;
	}
	axnet->bar0_iova = ax_dev->bar_info[BAR_0].phys_addr + ax_dev->bar_info[BAR_0].size / 2;
	bar0_size = ax_dev->bar_info[BAR_0].size / 2;

	pr_debug("MEM for BAR0, virt addr: 0x%llx, phy addr: 0x%llx, size: 0x%x \n",
		(u64) axnet->bar0_va, axnet->bar0_iova, bar0_size);
	pr_debug("ring buf size: %d\n", RING_COUNT);

	axnet_mem_info_set(axnet);

	axnet_ep_setup_bar0_md(axnet);
#endif

#if !ENABLE_SIMPLE_DMA
	/* Allocate PCIe memory for RP's dst address during xmit */
	axnet->tx_dst_pci_addr = axnet_alloc_pci_addr(SZ_64K);
	axnet->tx_dst_va = ioremap(axnet->tx_dst_pci_addr, SZ_64K);
	if (!axnet->tx_dst_va) {
		dev_err(dev, "failed to allocate dst PCIe address\n");
		ret = -ENOMEM;
		goto fail_unreg_netdev;
	}
	pr_debug("tx_dst_va: 0x%llx, tx_dst_pci_addr: 0x%llx\n",
		 (u64) axnet->tx_dst_va, axnet->tx_dst_pci_addr);
#endif

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

#ifndef SHMEM_FROM_MASTER
	pr_debug("addr[0]: %x, addr[1]: %x \b", ((int *)axnet->bar0_va)[0],
		 ((int *)axnet->bar0_va)[1]);
#endif

#if USE_MAILBOX
	axnet_register_irq(axnet_irq_handler);
#endif

	return 0;

#ifndef SHMEM_FROM_MASTER
fail_unreg_netdev:
	unregister_netdev(ndev);
#endif
fail_free_netdev:
	netif_napi_del(&axnet->napi);
	free_netdev(ndev);

	return ret;
}

module_init(axnet_ep_pci_epf_init);

static void __exit axnet_ep_pci_epf_exit(void)
{
	struct pci_epf_axnet *axnet = g_axnet;

	pr_info("axnet ep pcie epf exit\n");

	//call this first, it will call net close func
	unregister_netdev(axnet->ndev);
	netif_napi_del(&axnet->napi);
	devm_iounmap(axnet->fdev, axnet->ob_base_virt);
	free_netdev(axnet->ndev);

#if !ENABLE_SIMPLE_DMA
	/* remove outbound operation */
#endif
}

module_exit(axnet_ep_pci_epf_exit);

MODULE_DESCRIPTION("AX PCI EPF VIRTUAL NETWORK DRIVER");
MODULE_AUTHOR("AX");
MODULE_LICENSE("GPL v2");

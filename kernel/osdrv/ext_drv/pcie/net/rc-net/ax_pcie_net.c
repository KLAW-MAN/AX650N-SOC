#include <linux/etherdevice.h>
#include <linux/module.h>
#include <linux/netdevice.h>
#include <linux/pci.h>

#define PCIE_MASTER 1
#include "../../include/ax_pcie_net.h"
#include "../../include/ax_pcie_dev.h"

#define BAR_0 0
#define BAR_1 1

struct axnet_priv {
	struct net_device *ndev;
	struct napi_struct napi;
	struct pci_dev *pdev;

	void __iomem *mmio_base;
	void __iomem *mail_box_virt;
	void __iomem *dma_base;

#if ENABLE_SIMPLE_DMA
	dma_addr_t tx_phy_addr;
	void *tx_virt_addr;
#endif

	struct bar_md *bar_md;
	struct ep_ring_buf ep_mem;
	struct host_ring_buf host_mem;
	struct list_head ep2h_empty_list;
	/* To protect ep2h empty list */
	spinlock_t ep2h_empty_lock;

	spinlock_t mailbox_lock;
	spinlock_t irq_lock;
	spinlock_t h2ep_lock;
#ifdef SHMEM_FROM_MASTER
	int link_up_checked;
	u64 ob_base;
	u64 ob_size;
	dma_addr_t org_phys_addr;
	void *org_addr;
	size_t org_size;
	void *ob_base_virt;
	void *ob_end_virt;
#endif

	enum dir_link_state tx_link_state;
	enum dir_link_state rx_link_state;
	enum os_link_state os_link_state;
	/* To synchronize network link state machine */
	struct mutex link_state_lock;
	wait_queue_head_t link_state_wq;

	struct axnet_counter h2ep_ctrl;
	struct axnet_counter ep2h_ctrl;
	struct axnet_counter h2ep_empty;
	struct axnet_counter h2ep_full;
	struct axnet_counter ep2h_empty;
	struct axnet_counter ep2h_full;

};

static void axnet_host_process_ctrl_msg(struct axnet_priv *axnet);


#if ENABLE_SIMPLE_DMA
static void axnet_host_alloc_tx_buffers(struct axnet_priv *axnet)
{
	struct device *dev = &axnet->pdev->dev;

	struct ep_ring_buf *ep_ring_buf = &axnet->ep_mem;
	struct data_msg *h2ep_data_msg = ep_ring_buf->h2ep_data_msgs;
	unsigned long flags;

	struct net_device *ndev = axnet->ndev;
	int size = ndev->mtu + ETH_HLEN;
	dma_addr_t phys_addr;
	void *virt_addr;
	u32 idx = 0;

	pr_debug("axnet host alloc tx buffers\n");

	if (!axnet->tx_virt_addr) {
		virt_addr =
		    dma_alloc_coherent(dev, RING_COUNT * roundup_pow_of_two(size),
				       &phys_addr, GFP_KERNEL);
		if (!virt_addr) {
			pr_err("dma alloc failed\n");
			return;
		}

		axnet->tx_virt_addr = virt_addr;
		axnet->tx_phy_addr = phys_addr;
		pr_debug("phy addr: %llx, size: %ld\n", phys_addr,
			 RING_COUNT * roundup_pow_of_two(size));
	}

	while (!axnet_ivc_full(&axnet->h2ep_empty)) {
		dma_addr_t iova;

		iova = axnet->tx_phy_addr + idx * roundup_pow_of_two(size);
		ax_pcie_net_lock(&flags);
		h2ep_data_msg[idx].free_pcie_address = iova;
		mb();
		h2ep_data_msg[idx].buffer_len = roundup_pow_of_two(size);
		mb();
		ax_pcie_net_unlock(&flags);
		axnet_ivc_advance_wr(&axnet->h2ep_empty);
		mb();

		pr_debug("alloc tx buf, idx:%d, addr:%llx, len:%lld", idx,
			 h2ep_data_msg[idx].free_pcie_address,
			 h2ep_data_msg[idx].buffer_len);

		idx++;
	}

}

static void axnet_host_free_tx_buffers(struct axnet_priv *axnet)
{
	struct device *dev = &axnet->pdev->dev;
	struct net_device *ndev = axnet->ndev;
	int size = ndev->mtu + ETH_HLEN;

	pr_debug("axnet host free tx buffers\n");

	if (axnet->tx_virt_addr) {
		 dma_free_coherent(dev, RING_COUNT * roundup_pow_of_two(size),
		 	axnet->tx_virt_addr, axnet->tx_phy_addr);
		axnet->tx_virt_addr = NULL;
	}
}
#endif

#if USE_MAILBOX
#define MAILBOX_MAX_CHANNEL 8
static int axnet_check_mailbox_cnt(struct axnet_priv *axnet)
{
	u64 rd;
	struct irq_md *irq;
	static u64 i;
	u64 irq_cnt;
	unsigned long flags;
	// static s64 rd_tmp = -1;

	if (!axnet->ep_mem.h2ep_irq_msgs)
		return 1;

	ax_pcie_net_lock(&flags);
	irq = (void *)axnet->ep_mem.h2ep_irq_msgs;
	rd = readq(&(irq->rd_cnt));
	irq_cnt = readq(&(irq->send_cnt));
	mb();
	ax_pcie_net_unlock(&flags);

	if ((irq_cnt - rd > MAILBOX_MAX_CHANNEL)
	    && (irq_cnt >= rd)) {
		if (i % 10000 == 0)
			pr_debug
			    ("no mailbox irq avalible: irq_cnt:%lld, rd:%lld\n",
			     irq_cnt, rd);
		i++;
		return 0;
	}

	pr_debug("mailbox irq: irq_cnt:%lld, rd:%lld\n", irq_cnt, rd);

	// if(rd != rd_tmp)
	irq_cnt++;
	ax_pcie_net_lock(&flags);
	writeq(irq_cnt, &(irq->send_cnt));
	mb();
	ax_pcie_net_unlock(&flags);
	// rd_tmp = rd;
	// mb();

	return 1;
}

static int axnet_pcie_host_irq_to_slave(struct axnet_priv *axnet)
{
	int i;
	int size = 32;
	int timeout = 20;
	unsigned int infor_data, data_reg, regno;
	static volatile int mailbox_fromid = CPU0_MASTERID;
	unsigned long flags;
#ifndef IS_THIRD_PARTY_PLATFORM
	unsigned long dbi_flags = 0;
#endif
	struct irq_md *irq;
	u64 irq_cnt = 0x1;

	if (!axnet) {
		pr_err("mailbox irq: axnet is null!\n");
		return -3;
	}

	i = 0;
	while (!axnet_check_mailbox_cnt(axnet)) {
		udelay(1);
		i++;
		if (i > 50000) {
			pr_err("host irq to slave fail!");
			return -3;
		}
	}

	spin_lock_irqsave(&axnet->mailbox_lock, flags);
#ifndef IS_THIRD_PARTY_PLATFORM
	ax_pcie_spin_lock(&dbi_flags);
#endif
	regno = REG32(axnet->mail_box_virt + PCIE_NET_SLOT_REQ);	//reg req
	pr_debug("regno = %x", regno);
	while (regno == 0xffffffff && timeout > 0) {
		regno = REG32(axnet->mail_box_virt + PCIE_NET_SLOT_REQ);
		timeout--;
		udelay(10);
	}

	if (regno == 0xffffffff) {
#ifndef IS_THIRD_PARTY_PLATFORM
		ax_pcie_spin_unlock(&dbi_flags);
#endif
		return -3;
	}

	if (axnet->ep_mem.h2ep_irq_msgs) {
		irq = (void *)axnet->ep_mem.h2ep_irq_msgs;
		irq_cnt = readq(&(irq->send_cnt));
	}

	data_reg = MAILBOX_REG_BASE + PCIE_DATA + (regno << 2);	//data_reg addr
	//axera_trace(AXERA_DEBUG, "data_reg = %x", data_reg);
	for (i = 0; i < (size / 4); i++) {
		// REG32(axnet->mail_box_virt + data_reg) = 0x1;
		REG32(axnet->mail_box_virt + data_reg) = irq_cnt;
	}
	infor_data = (mailbox_fromid << 28) | PCIE_NET << 24 | size;	//use 16-19 saving masterid
	//axera_trace(AXERA_DEBUG, "infor_data = %x", infor_data);
	REG32(axnet->mail_box_virt + MAILBOX_REG_BASE + PCIE_INFO +
	      (regno << 2)) = infor_data;

#ifndef IS_THIRD_PARTY_PLATFORM
	ax_pcie_spin_unlock(&dbi_flags);
#endif
	spin_unlock_irqrestore(&axnet->mailbox_lock, flags);

	smp_mb();

	return 0;
}

static int axnet_check_mailbox_int(struct axnet_priv *axnet)
{
	int val;
	struct irq_md *irq = (void *)axnet->ep_mem.h2ep_irq_msgs;
	unsigned long flags;

	ax_pcie_net_lock(&flags);
	val = readl(&(irq->mailbox_int));
	ax_pcie_net_unlock(&flags);

	return val;
}
#endif

static int axnet_pcie_link_is_ok(struct axnet_priv *axnet)
{
	int link_status;
	u64 val;
	unsigned long flags;

	ax_pcie_net_lock(&flags);
	val = READ_ONCE(axnet->bar_md->link_magic);
	ax_pcie_net_unlock(&flags);

	if (val == EP_LINK_MAGIC) {
		pr_debug("link is ok\n");
		link_status = 1;
	} else if (val == 0xffffffffffffffff) {
		pr_info("link is not ready\n");
		link_status = 0;
	} else {
		pr_info("link has problems\n");
		link_status = 0;
	}

	return link_status;
}

static int axnet_pcie_ep_net_is_up(struct axnet_priv *axnet)
{
	int link_status;
	u64 val;
	unsigned long flags;

	ax_pcie_net_lock(&flags);
	val = READ_ONCE(axnet->bar_md->ep_link_status);
	ax_pcie_net_unlock(&flags);

	if (val == DIR_LINK_STATE_UP) {
		pr_debug("link is ok\n");
		link_status = 1;
	} else {
		pr_debug("link is down\n");
		link_status = 0;
	}

	return link_status;
}

static void axnet_host_raise_ep_ctrl_irq(struct axnet_priv *axnet)
{
	struct irq_md *irq = (void *)axnet->ep_mem.h2ep_irq_msgs;
	// unsigned long flags;

	if (!axnet_pcie_link_is_ok(axnet))
		return;

	if (irq) {
		//spin_lock_irqsave(&axnet->irq_lock, flags);
		/* Can write any value to generate sync point irq */
		// writel(0xaa, &(irq->irq_ctrl));
		//spin_unlock_irqrestore(&axnet->irq_lock, flags);
		/* BAR0 mmio address is wc mem, add mb to make sure
		 * multiple interrupt writes are not combined.
		 */
		mb();
#if USE_MAILBOX
		axnet_pcie_host_irq_to_slave(axnet);
#endif
	} else {
		pr_err("%s: invalid irq_offset\n", __func__);
	}
}

static void axnet_host_raise_ep_data_irq(struct axnet_priv *axnet)
{
	struct irq_md *irq = (void *)axnet->ep_mem.h2ep_irq_msgs;

	if (!axnet_pcie_link_is_ok(axnet))
		return;

	if (irq) {
		/* Can write any value to generate sync point irq */
		// writel(0xbb, &(irq->irq_data));
		/* BAR0 mmio address is wc mem, add mb to make sure
		 * multiple interrupt writes are not combined.
		 */
		mb();
#if USE_MAILBOX
		if (axnet_check_mailbox_int(axnet))
			axnet_pcie_host_irq_to_slave(axnet);
#endif
	} else {
		pr_err("%s: invalid irq_offset\n", __func__);
	}
}

static void axnet_host_read_ctrl_msg(struct axnet_priv *axnet,
				     struct ctrl_msg *msg)
{
	struct host_ring_buf *host_mem = &axnet->host_mem;
	struct ctrl_msg *ctrl_msg = host_mem->ep2h_ctrl_msgs;
	unsigned long flags;
	u32 idx;

	if (axnet_ivc_empty(&axnet->ep2h_ctrl)) {
		pr_debug("%s: EP2H ctrl ring is empty\n", __func__);
		return;
	}

	idx = axnet_ivc_get_rd_cnt(&axnet->ep2h_ctrl) % RING_COUNT;
	ax_pcie_net_lock(&flags);
	memcpy_fromio(msg, &ctrl_msg[idx], sizeof(*msg));
	ax_pcie_net_unlock(&flags);
	axnet_ivc_advance_rd(&axnet->ep2h_ctrl);
}

/* TODO Handle error case */
static int axnet_host_write_ctrl_msg(struct axnet_priv *axnet,
				     struct ctrl_msg *msg)
{
	struct ep_ring_buf *ep_mem = &axnet->ep_mem;
	struct ctrl_msg *ctrl_msg = ep_mem->h2ep_ctrl_msgs;
	unsigned long flags;
	u32 idx;

	pr_debug("axnet_host_write_ctrl_msg\n");

	if (axnet_ivc_full(&axnet->h2ep_ctrl)) {
		/* Raise an interrupt to let host process EP2H ring */
		axnet_host_raise_ep_ctrl_irq(axnet);
		pr_err("%s: EP2H ctrl ring is full\n", __func__);
		return -EAGAIN;
	}

	idx = axnet_ivc_get_wr_cnt(&axnet->h2ep_ctrl) % RING_COUNT;
	ax_pcie_net_lock(&flags);
	memcpy_toio(&ctrl_msg[idx], msg, sizeof(*msg));
	ax_pcie_net_unlock(&flags);
	/* BAR0 mmio address is wc mem, add mb to make sure ctrl msg is written
	 * before updating counters.
	 */
	mb();
	axnet_ivc_advance_wr(&axnet->h2ep_ctrl);
	axnet_host_raise_ep_ctrl_irq(axnet);

	return 0;
}

static void axnet_host_alloc_empty_buffers(struct axnet_priv *axnet)
{
	struct net_device *ndev = axnet->ndev;
	struct host_ring_buf *host_mem = &axnet->host_mem;
	struct data_msg *ep2h_data_msg = host_mem->ep2h_data_msgs;
	struct ep2h_empty_list *ep2h_empty_ptr;
	struct device *d = &axnet->pdev->dev;
	unsigned long flags;

	pr_debug("axnet host alloc empty buffers\n");

	while (!axnet_ivc_full(&axnet->ep2h_empty)) {
		struct sk_buff *skb;
		dma_addr_t iova;
		int len = ndev->mtu + ETH_HLEN;
		u32 idx;

		if (in_interrupt())
			skb = netdev_alloc_skb(ndev, len); //GFP_ATOMIC
		else
			skb = __netdev_alloc_skb(ndev, len, GFP_KERNEL);
		if (!skb) {
			pr_err("%s: alloc skb failed\n", __func__);
			break;
		}
		iova = dma_map_single(d, skb->data, len, DMA_FROM_DEVICE);
		if (dma_mapping_error(d, iova)) {
			pr_err("%s: dma map failed\n", __func__);
			dev_kfree_skb_any(skb);
			break;
		}

		// for debug
		// memset(skb->data, 0xaa, len);

		if (in_interrupt())
			ep2h_empty_ptr = kmalloc(sizeof(*ep2h_empty_ptr), GFP_ATOMIC);
		else
			ep2h_empty_ptr = kmalloc(sizeof(*ep2h_empty_ptr), GFP_KERNEL);
		if (!ep2h_empty_ptr) {
			dma_unmap_single(d, iova, len, DMA_FROM_DEVICE);
			dev_kfree_skb_any(skb);
			break;
		}
		ep2h_empty_ptr->skb = skb;
		ep2h_empty_ptr->iova = iova;
		ep2h_empty_ptr->len = len;
		spin_lock_irqsave(&axnet->ep2h_empty_lock, flags);
		list_add_tail(&ep2h_empty_ptr->list, &axnet->ep2h_empty_list);
		spin_unlock_irqrestore(&axnet->ep2h_empty_lock, flags);

		idx = axnet_ivc_get_wr_cnt(&axnet->ep2h_empty) % RING_COUNT;
		ax_pcie_net_lock(&flags);
		ep2h_data_msg[idx].buffer_len = len;
		mb();
		ep2h_data_msg[idx].free_pcie_address = iova;
		ax_pcie_net_unlock(&flags);

		pr_debug("alloc skb buf, idx:%d, addr:%llx, len:%d", idx, iova,
			 len);
		pr_debug("alloc skb buf, idx:%d, addr:%llx, len:%lld", idx,
			 ep2h_data_msg[idx].free_pcie_address,
			 ep2h_data_msg[idx].buffer_len);

		/* BAR0 mmio address is wc mem, add mb to make sure that empty
		 * buffers are updated before updating counters.
		 */
		mb();
		axnet_ivc_advance_wr(&axnet->ep2h_empty);

		// axnet_host_raise_ep_ctrl_irq(axnet);
	}

	axnet_host_raise_ep_ctrl_irq(axnet);
}

static void axnet_host_free_empty_buffers(struct axnet_priv *axnet)
{
	struct ep2h_empty_list *ep2h_empty_ptr, *temp;
	struct device *d = &axnet->pdev->dev;
	unsigned long flags;

	spin_lock_irqsave(&axnet->ep2h_empty_lock, flags);
	list_for_each_entry_safe(ep2h_empty_ptr, temp, &axnet->ep2h_empty_list,
				 list) {
		list_del(&ep2h_empty_ptr->list);
		dma_unmap_single(d, ep2h_empty_ptr->iova, ep2h_empty_ptr->len,
				 DMA_FROM_DEVICE);
		dev_kfree_skb_any(ep2h_empty_ptr->skb);
		kfree(ep2h_empty_ptr);
	}
	spin_unlock_irqrestore(&axnet->ep2h_empty_lock, flags);
}

static void axnet_host_stop_rx_work(struct axnet_priv *axnet)
{
	/* wait for interrupt handle to return to ensure rx is stopped */
	synchronize_irq(pci_irq_vector(axnet->pdev, MSI_IRQ_NUM));
}

static void axnet_host_clear_data_msg_counters(struct axnet_priv *axnet)
{
	unsigned long flags;
	ax_pcie_net_lock(&flags);
	//ctrl send & recv
	WRITE_ONCE(*axnet->ep2h_ctrl.rd, 0);
	WRITE_ONCE(*axnet->ep2h_ctrl.wr, 0);
	WRITE_ONCE(*axnet->h2ep_ctrl.rd, 0);
	WRITE_ONCE(*axnet->h2ep_ctrl.wr, 0);

	//host recv
	WRITE_ONCE(*axnet->ep2h_empty.rd, 0);
	WRITE_ONCE(*axnet->ep2h_empty.wr, 0);
	WRITE_ONCE(*axnet->ep2h_full.rd, 0);
	WRITE_ONCE(*axnet->ep2h_full.wr, 0);

	//host send
#if ENABLE_SIMPLE_DMA
	WRITE_ONCE(*axnet->h2ep_empty.rd, 0);
	WRITE_ONCE(*axnet->h2ep_empty.wr, 0);
#endif
	WRITE_ONCE(*axnet->h2ep_full.rd, 0);
	WRITE_ONCE(*axnet->h2ep_full.wr, 0);
	ax_pcie_net_unlock(&flags);
}

static void axnet_host_update_link_state(struct net_device *ndev,
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
static void axnet_host_update_link_sm(struct axnet_priv *axnet)
{
	struct net_device *ndev = axnet->ndev;
	enum os_link_state old_state = axnet->os_link_state;

	if ((axnet->rx_link_state == DIR_LINK_STATE_UP) &&
	    (axnet->tx_link_state == DIR_LINK_STATE_UP))
		axnet->os_link_state = OS_LINK_STATE_UP;
	else
		axnet->os_link_state = OS_LINK_STATE_DOWN;

	if (axnet->os_link_state != old_state)
		axnet_host_update_link_state(ndev, axnet->os_link_state);
}

/* One way link state machine*/
static void axnet_host_user_link_up_req(struct axnet_priv *axnet)
{
	struct ctrl_msg msg;

	axnet_host_clear_data_msg_counters(axnet);

	axnet_host_alloc_empty_buffers(axnet);
#if ENABLE_SIMPLE_DMA
	axnet_host_alloc_tx_buffers(axnet);
#endif

	msg.msg_id = CTRL_MSG_LINK_UP;
	axnet_host_write_ctrl_msg(axnet, &msg);
	axnet->rx_link_state = DIR_LINK_STATE_UP;
	axnet->bar_md->rc_link_status = DIR_LINK_STATE_UP;
	axnet_host_update_link_sm(axnet);
}

static void axnet_host_user_link_down_req(struct axnet_priv *axnet)
{
	struct ctrl_msg msg;

	axnet->bar_md->rc_link_status = DIR_LINK_STATE_SENT_DOWN;
	axnet->rx_link_state = DIR_LINK_STATE_SENT_DOWN;
	msg.msg_id = CTRL_MSG_LINK_DOWN;
	axnet_host_write_ctrl_msg(axnet, &msg);
}

static void axnet_host_rcv_link_up_msg(struct axnet_priv *axnet)
{
	pr_debug("recv link up msg\n");

	axnet->tx_link_state = DIR_LINK_STATE_UP;
	axnet_host_update_link_sm(axnet);
}

static void axnet_host_rcv_link_down_msg(struct axnet_priv *axnet)
{
	struct ctrl_msg msg;

	pr_debug("recv link down msg\n");
	/* Stop using empty buffers of remote system */
	msg.msg_id = CTRL_MSG_LINK_DOWN_ACK;
	axnet_host_write_ctrl_msg(axnet, &msg);
	axnet->tx_link_state = DIR_LINK_STATE_DOWN;
	axnet_host_update_link_sm(axnet);
}

static void axnet_host_rcv_link_down_ack(struct axnet_priv *axnet)
{
	pr_debug("recv link down ack msg\n");

	axnet->bar_md->rc_link_status = DIR_LINK_STATE_DOWN;
	axnet->rx_link_state = DIR_LINK_STATE_DOWN;
	wake_up_interruptible(&axnet->link_state_wq);
	axnet_host_update_link_sm(axnet);
}

#ifdef SHMEM_FROM_MASTER
static int axnet_handshake_slave_step0(struct net_device *ndev)
{
	struct axnet_priv *axnet = netdev_priv(ndev);
	struct axera_dev *ax_dev = (struct axera_dev *)pci_get_drvdata(axnet->pdev);
	struct device *dev = &axnet->pdev->dev;
	int size = ndev->mtu + ETH_HLEN;
	dma_addr_t phys_addr;
	void *virt_addr;

	*(volatile unsigned int *)(ax_dev->bar_virt[BAR_0]  + 0x20) = DEVICE_CHECKED_FLAG;
	*(volatile unsigned long *)(ax_dev->bar_virt[BAR_0] + 0x30) = axnet->ob_base;
	pr_debug("net_ob_base = %llx\n", axnet->ob_base);

	virt_addr =
	dma_alloc_coherent(dev, RING_COUNT * roundup_pow_of_two(size),
		       &phys_addr, GFP_KERNEL);
	if (!virt_addr) {
		pr_err("dma alloc failed\n");
		return -1;
	}
	axnet->tx_virt_addr = virt_addr;
	axnet->tx_phy_addr = phys_addr;
	pr_debug("phy addr: %llx, size: %ld\n", phys_addr, RING_COUNT * roundup_pow_of_two(size));
	axnet_pcie_host_irq_to_slave(axnet);
	return 0;
}
#endif

static int axnet_host_open(struct net_device *ndev)
{
	struct axnet_priv *axnet = netdev_priv(ndev);

	pr_info("open pcie net\n");

#ifdef SHMEM_FROM_MASTER
	if (axnet->link_up_checked != DEVICE_CHECKED_FLAG) {
		return axnet_handshake_slave_step0(ndev);
	}
#endif

	/* if ep send linkup req before rc driver ready, rc will miss this req
	   so we need check ep linkup req */
	if (axnet_pcie_ep_net_is_up(axnet)) {
		axnet->tx_link_state = DIR_LINK_STATE_UP;
	}

	mutex_lock(&axnet->link_state_lock);
	if (axnet->rx_link_state == DIR_LINK_STATE_DOWN)
		axnet_host_user_link_up_req(axnet);
	napi_enable(&axnet->napi);
	mutex_unlock(&axnet->link_state_lock);

	return 0;
}

static int axnet_host_close(struct net_device *ndev)
{
	struct axnet_priv *axnet = netdev_priv(ndev);
	int ret = 0;

	pr_info("close pcie net\n");

	mutex_lock(&axnet->link_state_lock);
	napi_disable(&axnet->napi);
	if (axnet->rx_link_state == DIR_LINK_STATE_UP)
		axnet_host_user_link_down_req(axnet);

	if (axnet->tx_link_state == DIR_LINK_STATE_UP) { //peer is up
		ret = wait_event_interruptible_timeout(axnet->link_state_wq,
							(axnet->rx_link_state ==
							DIR_LINK_STATE_DOWN),
							msecs_to_jiffies(LINK_TIMEOUT));
		ret = (ret > 0) ? 0 : -ETIMEDOUT;
		if (ret < 0) {
			pr_err
				("%s: link state machine failed: tx_state: %d rx_state: %d err: %d",
				__func__, axnet->tx_link_state, axnet->rx_link_state, ret);
		}
	} else {
		axnet->rx_link_state = DIR_LINK_STATE_DOWN;
		axnet->bar_md->rc_link_status = DIR_LINK_STATE_DOWN;
	}
	mutex_unlock(&axnet->link_state_lock);

	/* Stop using empty buffers(which are full in rx) of local system */
	axnet_host_stop_rx_work(axnet);
#if ENABLE_SIMPLE_DMA
	axnet_host_free_tx_buffers(axnet);
#endif
	axnet_host_free_empty_buffers(axnet);

	return ret;
}

static int axnet_host_change_mtu(struct net_device *ndev, int new_mtu)
{
	bool set_down = false;

	if (new_mtu > AXNET_MAX_MTU || new_mtu < AXNET_MIN_MTU) {
		pr_err("MTU range is %d to %d\n", AXNET_MIN_MTU, AXNET_MAX_MTU);
		return -EINVAL;
	}

	if (netif_running(ndev)) {
		set_down = true;
		axnet_host_close(ndev);
	}

	pr_info("changing MTU from %d to %d\n", ndev->mtu, new_mtu);
	ndev->mtu = new_mtu;

	if (set_down)
		axnet_host_open(ndev);

	return 0;
}

static netdev_tx_t axnet_host_start_xmit(struct sk_buff *skb,
					 struct net_device *ndev)
{
	struct axnet_priv *axnet = netdev_priv(ndev);
	struct skb_shared_info *info = skb_shinfo(skb);
	struct ep_ring_buf *ep_mem = &axnet->ep_mem;
	struct data_msg *h2ep_data_msg = ep_mem->h2ep_data_msgs;
	dma_addr_t dst_iova;
	u32 rd_idx;
	u32 wr_idx;
	void *dst_virt;
	int len;
	u64 bar0_base_addr;
	unsigned long flags;
	unsigned long net_flags;

	pr_debug("start xmit packet\n");

	/* TODO Not expecting skb frags, remove this after testing */
	WARN_ON(info->nr_frags);

	/* Check if H2EP_EMPTY_BUF available to read */
	spin_lock_irqsave(&axnet->h2ep_lock, flags);
	if (!axnet_ivc_rd_available(&axnet->h2ep_empty)) {
		axnet_host_raise_ep_data_irq(axnet);
		pr_debug("%s: No H2EP empty msg, stop tx\n", __func__);
		netif_stop_queue(ndev);
		spin_unlock_irqrestore(&axnet->h2ep_lock, flags);
		return NETDEV_TX_BUSY;
	}
	spin_unlock_irqrestore(&axnet->h2ep_lock, flags);

	/* Check if H2EP_FULL_BUF available to write */
	spin_lock_irqsave(&axnet->h2ep_lock, flags);
	if (axnet_ivc_full(&axnet->h2ep_full)) {
		axnet_host_raise_ep_data_irq(axnet);
		pr_debug("%s: No H2EP full buf, stop tx\n", __func__);
		netif_stop_queue(ndev);
		spin_unlock_irqrestore(&axnet->h2ep_lock, flags);
		return NETDEV_TX_BUSY;
	}
	spin_unlock_irqrestore(&axnet->h2ep_lock, flags);

	len = skb_headlen(skb);

	/* Get H2EP empty msg */
	spin_lock_irqsave(&axnet->h2ep_lock, flags);
	rd_idx = axnet_ivc_get_rd_cnt(&axnet->h2ep_empty) % RING_COUNT;
	ax_pcie_net_lock(&net_flags);
	dst_iova = *(volatile u64 *)&(h2ep_data_msg[rd_idx].free_pcie_address);
	bar0_base_addr = *(volatile u64 *)&(axnet->bar_md->bar0_base_phy);
	ax_pcie_net_unlock(&net_flags);
	mb();
	spin_unlock_irqrestore(&axnet->h2ep_lock, flags);
#if ENABLE_SIMPLE_DMA
	dst_virt = axnet->tx_virt_addr + (dst_iova - axnet->tx_phy_addr);
#else
	dst_virt = axnet->mmio_base + (dst_iova - bar0_base_addr);
#endif

	pr_debug("idx:%d, dst_iova: %llx, dst_virt: %llx\n", rd_idx, dst_iova,
		 (u64) dst_virt);
	if (((u64) dst_virt & 0x1) == 0x1) {
		pr_info
		    ("invalid addr, rd_idx:%d, dst_iova:%llx, bar0_base_addr:%llx\n",
		     rd_idx, dst_iova, bar0_base_addr);
	}
	/* Advance read count after all failure cases complated, to avoid
	 * dangling buffer at endpoint.
	 */
	spin_lock_irqsave(&axnet->h2ep_lock, flags);
	axnet_ivc_advance_rd(&axnet->h2ep_empty);
	spin_unlock_irqrestore(&axnet->h2ep_lock, flags);

#if ENABLE_SIMPLE_DMA
	memcpy(dst_virt, skb->data, len);
#else
	/* Copy skb->data to endpoint dst address, use CPU virt addr */
	ax_pcie_net_lock(&net_flags);
	memcpy_toio(dst_virt, skb->data, len);
	ax_pcie_net_unlock(&net_flags);
	/* BAR0 mmio address is wc mem, add mb to make sure that complete
	 * skb->data is written before updating counters.
	 */
	mb();
#endif

	/* Push dst to H2EP full ring */
	spin_lock_irqsave(&axnet->h2ep_lock, flags);
	wr_idx = axnet_ivc_get_wr_cnt(&axnet->h2ep_full) % RING_COUNT;
	spin_unlock_irqrestore(&axnet->h2ep_lock, flags);

	pr_debug("wr_idx:%d, dst_iova:%llx, len:%d\n", wr_idx, dst_iova, len);

	ax_pcie_net_lock(&net_flags);
	// h2ep_data_msg[wr_idx].packet_size = len;
	// h2ep_data_msg[wr_idx].busy_pcie_address = dst_iova;
	*(u64 *) &(h2ep_data_msg[wr_idx].packet_size) = len;
	mb();
	*(u64 *) &(h2ep_data_msg[wr_idx].busy_pcie_address) = dst_iova;
	ax_pcie_net_unlock(&net_flags);

	/* BAR0 mmio address is wc mem, add mb to make sure that full
	 * buffer is written before updating counters.
	 */
	mb();
	spin_lock_irqsave(&axnet->h2ep_lock, flags);
	axnet_ivc_advance_wr(&axnet->h2ep_full);
	spin_unlock_irqrestore(&axnet->h2ep_lock, flags);

	axnet_host_raise_ep_data_irq(axnet);

	/* Free skb */
	dev_kfree_skb_any(skb);

	ndev->stats.tx_packets++;
	ndev->stats.tx_bytes += len;

	return NETDEV_TX_OK;
}

static const struct net_device_ops axnet_host_netdev_ops = {
	.ndo_open = axnet_host_open,
	.ndo_stop = axnet_host_close,
	.ndo_start_xmit = axnet_host_start_xmit,
	.ndo_change_mtu = axnet_host_change_mtu,
};

static void axnet_host_setup_bar0_md(struct axnet_priv *axnet)
{
	struct ep_ring_buf *ep_mem = &axnet->ep_mem;
	struct host_ring_buf *host_mem = &axnet->host_mem;
	struct irq_md *irq;
	unsigned long flags;

	axnet->bar_md = (struct bar_md *)axnet->mmio_base;
	if (axnet_pcie_link_is_ok(axnet))
		pr_info("pcie link is ready now\n");

	ax_pcie_net_lock(&flags);
	ep_mem->ep_cnt = (struct fifo_cnt *)(axnet->mmio_base +
					     axnet->bar_md->ep_own_cnt_offset);
	ep_mem->h2ep_ctrl_msgs = (struct ctrl_msg *)(axnet->mmio_base +
						     axnet->bar_md->
						     h2ep_ctrl_md.offset);
	ep_mem->h2ep_data_msgs =
	    (struct data_msg *)(axnet->mmio_base +
				axnet->bar_md->h2ep_data_md.offset);
	ep_mem->h2ep_irq_msgs =
	    (struct irq_md *)(axnet->mmio_base + axnet->bar_md->irq_offset);

	pr_debug
	    ("mmio_base:%llx, ep_mem->ep_cnt:%llx, ep_mem->h2ep_ctrl_msgs:%llx, ep_mem->h2ep_data_msgs:%llx, ep_mem->h2ep_irq_msgs:%llx\n",
	     (u64) axnet->mmio_base, (u64) ep_mem->ep_cnt,
	     (u64) ep_mem->h2ep_ctrl_msgs, (u64) ep_mem->h2ep_data_msgs,
	     (u64) ep_mem->h2ep_irq_msgs);

	host_mem->host_cnt = (struct fifo_cnt *)(axnet->mmio_base +
						 axnet->bar_md->
						 host_own_cnt_offset);
	host_mem->ep2h_ctrl_msgs =
	    (struct ctrl_msg *)(axnet->mmio_base +
				axnet->bar_md->ep2h_ctrl_md.offset);
	host_mem->ep2h_data_msgs =
	    (struct data_msg *)(axnet->mmio_base +
				axnet->bar_md->ep2h_data_md.offset);
	ax_pcie_net_unlock(&flags);
	pr_debug
	    ("mmio_base:%llx, host_mem->host_cnt:%llx, host_mem->ep2h_data_msgs:%llx, offset:%lld\n",
	     (u64) axnet->mmio_base, (u64) host_mem->host_cnt,
	     (u64) host_mem->ep2h_data_msgs,
	     axnet->bar_md->h2ep_data_md.offset);

	//ctrl send & recv
	axnet->ep2h_ctrl.rd = &host_mem->host_cnt->ctrl_rd_cnt;
	axnet->ep2h_ctrl.wr = &host_mem->host_cnt->ctrl_wr_cnt;
	axnet->h2ep_ctrl.rd = &ep_mem->ep_cnt->ctrl_rd_cnt;
	axnet->h2ep_ctrl.wr = &ep_mem->ep_cnt->ctrl_wr_cnt;

	//host recv
	axnet->ep2h_empty.rd = &host_mem->host_cnt->fifo_rd_cnt;
	axnet->ep2h_empty.wr = &host_mem->host_cnt->fifo_wr_cnt;
	axnet->ep2h_full.rd = &host_mem->host_cnt->data_rd_cnt;
	axnet->ep2h_full.wr = &host_mem->host_cnt->data_wr_cnt;

	//host send
	axnet->h2ep_empty.rd = &ep_mem->ep_cnt->fifo_rd_cnt;
	axnet->h2ep_empty.wr = &ep_mem->ep_cnt->fifo_wr_cnt;
	axnet->h2ep_full.rd = &ep_mem->ep_cnt->data_rd_cnt;
	axnet->h2ep_full.wr = &ep_mem->ep_cnt->data_wr_cnt;

	irq = (void *)axnet->ep_mem.h2ep_irq_msgs;
	irq->mailbox_int = 1;
	irq->mailbox_irq_cnt = 8;
	axnet->bar_md->rc_link_status = DIR_LINK_STATE_DOWN;
}

static void axnet_host_process_ctrl_msg(struct axnet_priv *axnet)
{
	struct ctrl_msg msg;

	while (axnet_ivc_rd_available(&axnet->ep2h_ctrl)) {
		axnet_host_read_ctrl_msg(axnet, &msg);
		if (msg.msg_id == CTRL_MSG_LINK_UP)
			axnet_host_rcv_link_up_msg(axnet);
		else if (msg.msg_id == CTRL_MSG_LINK_DOWN)
			axnet_host_rcv_link_down_msg(axnet);
		else if (msg.msg_id == CTRL_MSG_LINK_DOWN_ACK)
			axnet_host_rcv_link_down_ack(axnet);
	}
}

static int axnet_host_process_ep2h_msg(struct axnet_priv *axnet)
{
	struct host_ring_buf *host_mem = &axnet->host_mem;
	struct data_msg *data_msg = host_mem->ep2h_data_msgs;
	struct device *d = &axnet->pdev->dev;
	struct ep2h_empty_list *ep2h_empty_ptr;
	struct net_device *ndev = axnet->ndev;
	unsigned long dbi_flags = 0;
	int count = 0;

	pr_debug("axnet host process ep2h msg\n");

	while ((count < AXNET_NAPI_WEIGHT) &&
	       axnet_ivc_rd_available(&axnet->ep2h_full)) {
		struct sk_buff *skb;
		u64 pcie_address;
		u32 len;
		int idx, found = 0;
		unsigned long flags;

		/* Read EP2H full msg */
		idx = axnet_ivc_get_rd_cnt(&axnet->ep2h_full) % RING_COUNT;
		// len = data_msg[idx].packet_size;
		// mb();
		// pcie_address = data_msg[idx].busy_pcie_address;
		// mb();
		ax_pcie_net_lock(&dbi_flags);
		len = *(u64 *) &(data_msg[idx].packet_size);
		pcie_address = *(u64 *) &(data_msg[idx].busy_pcie_address);
		ax_pcie_net_unlock(&dbi_flags);
		mb();
		pr_debug("index:%d, pcie_address:%llx, len:%d\n", idx,
			 pcie_address, len);

		spin_lock_irqsave(&axnet->ep2h_empty_lock, flags);
		list_for_each_entry(ep2h_empty_ptr, &axnet->ep2h_empty_list,
				    list) {
			if (ep2h_empty_ptr->iova == pcie_address) {
				found = 1;
				break;
			}
		}

		if (!found) {
			pr_info
			    ("warnning, index:%d, pcie_address:%llx, size:%d not found\n",
			     idx, pcie_address, len);
			axnet_ivc_advance_rd(&axnet->ep2h_full);
			ndev->stats.rx_errors++;
			count++;
			spin_unlock_irqrestore(&axnet->ep2h_empty_lock, flags);
			continue;	//jump this packet
		}

		list_del(&ep2h_empty_ptr->list);
		spin_unlock_irqrestore(&axnet->ep2h_empty_lock, flags);

		/* Advance H2EP full buffer after search in local list */
		axnet_ivc_advance_rd(&axnet->ep2h_full);
		/* If EP2H network queue is stopped due to lack of EP2H_FULL
		 * queue, raising ctrl irq will help.
		 */
		// axnet_host_raise_ep_ctrl_irq(axnet);

		pr_debug("recv a net packet\n");
		dma_unmap_single(d, pcie_address, ndev->mtu + ETH_HLEN,
				 DMA_FROM_DEVICE);
		skb = ep2h_empty_ptr->skb;
#if 0
		char *buf = skb->data;
		int i;

		for (i = 0; i < len; i++) {
			printk("%x", ((unsigned char *)buf)[i]);
		}
		printk("\n\n");
#endif
		skb_put(skb, len);
		skb->protocol = eth_type_trans(skb, ndev);
		napi_gro_receive(&axnet->napi, skb);

		ndev->stats.rx_packets++;
		ndev->stats.rx_bytes += len;

		/* Free EP2H empty list element */
		kfree(ep2h_empty_ptr);
		count++;
	}

	/* If EP2H network queue is stopped due to lack of EP2H_FULL
	* queue, raising ctrl irq will help.
	*/
	axnet_host_raise_ep_ctrl_irq(axnet);

	return count;
}

static irqreturn_t axnet_irq_ctrl(int irq, void *data)
{
	struct net_device *ndev = data;
	struct axnet_priv *axnet = netdev_priv(ndev);
	unsigned long flags;

	pr_debug("msi irq for ctrl\n");

	spin_lock_irqsave(&axnet->h2ep_lock, flags);
	if (netif_queue_stopped(ndev)) {
		if ((axnet->os_link_state == OS_LINK_STATE_UP) &&
		    axnet_ivc_rd_available(&axnet->h2ep_empty) &&
		    !axnet_ivc_full(&axnet->h2ep_full)) {
			pr_debug("%s: wakeup net tx queue\n", __func__);
			netif_wake_queue(ndev);
		}
	}
	spin_unlock_irqrestore(&axnet->h2ep_lock, flags);

	if (axnet_ivc_rd_available(&axnet->ep2h_ctrl))
		axnet_host_process_ctrl_msg(axnet);

	if (!axnet_ivc_full(&axnet->ep2h_empty) &&
	    (axnet->os_link_state == OS_LINK_STATE_UP))
		axnet_host_alloc_empty_buffers(axnet);

	return IRQ_HANDLED;
}

static irqreturn_t axnet_irq_data(int irq, void *data)
{
	struct net_device *ndev = data;
	struct axnet_priv *axnet = netdev_priv(ndev);

	pr_debug("msi irq for data\n");

	if (axnet_ivc_rd_available(&axnet->ep2h_full)) {
		disable_irq_nosync(pci_irq_vector(axnet->pdev, MSI_IRQ_NUM));
		napi_schedule(&axnet->napi);
	}

	return IRQ_HANDLED;
}

#ifdef SHMEM_FROM_MASTER
static int axnet_handshake_slave_step1(void *data)
{
	struct net_device *ndev = data;
	struct axnet_priv *axnet = netdev_priv(ndev);
	struct axera_dev *ax_dev = (struct axera_dev *)pci_get_drvdata(axnet->pdev);

	if (axnet->link_up_checked != DEVICE_CHECKED_FLAG) {
		if (*(volatile unsigned int *)(ax_dev->bar_virt[BAR_0] + 0x24) == DEVICE_HANDSHAKE_FLAG) {
			/* Clear slave handshake flag */
			*(volatile unsigned int *)(ax_dev->bar_virt[BAR_0] + 0x24) = 0x0;
			axnet->mmio_base = axnet->ob_base_virt;
			if (!axnet->mmio_base) {
				dev_err(&axnet->pdev->dev, "ob base is null\n");
			}
			pr_debug("BAR0 virtual addr: 0x%llx, size: 0x%llx\n", (u64) axnet->mmio_base, axnet->ob_size);

			/* Setup BAR0 meta data */
			axnet_host_setup_bar0_md(axnet);
			axnet->link_up_checked = DEVICE_CHECKED_FLAG;
			axnet_host_open(axnet->ndev);
		} else {
			return -1;
		}
	}
	return 0;
}
#endif

static irqreturn_t axnet_irq_handler(int irq, void *data)
{
	pr_debug("msi irq\n");

#ifdef SHMEM_FROM_MASTER
	if (axnet_handshake_slave_step1(data))
		return 0;
#endif

	axnet_irq_ctrl(irq, data);
	axnet_irq_data(irq, data);

	return IRQ_HANDLED;
}


static int axnet_host_poll(struct napi_struct *napi, int budget)
{
	struct axnet_priv *axnet = container_of(napi, struct axnet_priv, napi);
	int work_done;

	pr_debug("napi poll\n");

	work_done = axnet_host_process_ep2h_msg(axnet);
	pr_debug("work_done: %d budget: %d\n", work_done, budget);
	if (work_done < budget) {
		napi_complete(napi);
		enable_irq(pci_irq_vector(axnet->pdev, MSI_IRQ_NUM));
	}

	return work_done;
}
#ifdef SHMEM_FROM_MASTER
static int axnet_pci_alloc_shmem(struct axnet_priv *axnet)
{
	void __iomem *space;
	size_t alignment = 0xffff;
	dma_addr_t org_phys_addr;
	dma_addr_t phys_addr;
	u64 offset;
	struct pci_dev *pdev = axnet->pdev;
	int size = SZ_4M;

	space = dma_alloc_coherent(&pdev->dev, size, &org_phys_addr, GFP_KERNEL | __GFP_ZERO);
	if (!space) {
		dev_err(&pdev->dev, "failed to allocate mem space\n");
		return -1;
	}

	axnet->org_phys_addr = org_phys_addr;
	axnet->org_addr = space;
	axnet->org_size = size;
	phys_addr = (org_phys_addr + alignment) & ~alignment;
	offset = phys_addr - org_phys_addr;
	space = space + offset;
	size = size - offset;

	axnet->ob_base = phys_addr;
	axnet->ob_size = size;
	axnet->ob_base_virt = space;
	axnet->ob_end_virt = space + axnet->ob_size;
	pr_debug("phys_addr = %llx\n", axnet->ob_base);
	pr_debug("space = %p\n", space);
	pr_debug("ob_base_virt = %p\n", axnet->ob_base_virt);
	return 0;
}
#endif

static int __init axnet_pci_init_one(int idx)
{
	struct axnet_priv *axnet;
	struct net_device *ndev;
	struct axera_dev *ax_dev;
	struct device *dev;
#ifndef SHMEM_FROM_MASTER
	int bar0_size;
#endif
	int ret;

	pr_info("axnet pci init\n");

	ax_dev = g_axera_dev_map[idx];
	if (!ax_dev) {
		pr_err("pcie dev not found\n");
		return -1;
	}
    ax_dev->priv_data = NULL;
	pr_info("select dev, bus:%x, devfn:%x, vid:%x, pid:%x\n",
		ax_dev->slot_index, ax_dev->pdev->devfn, ax_dev->pdev->vendor, ax_dev->pdev->device);
	dev = &ax_dev->pdev->dev;

	ndev = alloc_etherdev(sizeof(struct axnet_priv));
	if (!ndev) {
		ret = -ENOMEM;
		pr_err("alloc_etherdev() failed\n");
		goto fail;
	}

	strcpy(ndev->name, INTERFACE_NAME);
	eth_hw_addr_random(ndev);
	SET_NETDEV_DEV(ndev, dev);
	ndev->netdev_ops = &axnet_host_netdev_ops;
	axnet = netdev_priv(ndev);
	ax_dev->priv_data = axnet;
	axnet->ndev = ndev;
	axnet->pdev = ax_dev->pdev;

#ifndef SHMEM_FROM_MASTER
	/* use the second half of bar0 mem in ep */
	bar0_size = ax_dev->bar_info[BAR_0].size / 2;
	axnet->mmio_base = ax_dev->bar_virt[BAR_0] + bar0_size;
	if (!axnet->mmio_base) {
		ret = -ENOMEM;
		dev_err(&axnet->pdev->dev, "get BAR0 address failed\n");
		goto free_netdev;
	}
	pr_debug("BAR0 virtual addr: 0x%llx, size: 0x%x\n", (u64) axnet->mmio_base, bar0_size);
#endif

#if USE_MAILBOX
	axnet->mail_box_virt = ax_dev->bar_virt[BAR_1];
	if (!axnet->mail_box_virt) {
		ret = -ENOMEM;
		dev_err(&axnet->pdev->dev, "get BAR1 address failed\n");
		goto free_netdev;
	}
	pr_debug("BAR1 virt addr: 0x%llx\n", (u64) axnet->mail_box_virt);
#endif

#ifndef SHMEM_FROM_MASTER
	pr_debug("addr[0]: %llx\n", ((u64 *) axnet->mmio_base)[0]);
	/* Setup BAR0 meta data */
	axnet_host_setup_bar0_md(axnet);
#endif

#if (LINUX_VERSION_CODE < KERNEL_VERSION(6, 1, 0))
	netif_napi_add(ndev, &axnet->napi, axnet_host_poll, AXNET_NAPI_WEIGHT);
#else
	netif_napi_add(ndev, &axnet->napi, axnet_host_poll);
#endif

	ndev->mtu = AXNET_DEFAULT_MTU;

	ret = register_netdev(ndev);
	if (ret) {
		dev_err(&axnet->pdev->dev, "register_netdev() fail: %d\n", ret);
		goto del_napi;
	}
	netif_carrier_off(ndev);

	axnet->rx_link_state = DIR_LINK_STATE_DOWN;
	axnet->tx_link_state = DIR_LINK_STATE_DOWN;
	axnet->os_link_state = OS_LINK_STATE_DOWN;
	mutex_init(&axnet->link_state_lock);
	init_waitqueue_head(&axnet->link_state_wq);
	INIT_LIST_HEAD(&axnet->ep2h_empty_list);

	spin_lock_init(&axnet->ep2h_empty_lock);
	spin_lock_init(&axnet->mailbox_lock);
	spin_lock_init(&axnet->irq_lock);
	spin_lock_init(&axnet->h2ep_lock);

	ret =
	    devm_request_irq(&ax_dev->pdev->dev, pci_irq_vector(ax_dev->pdev, MSI_IRQ_NUM),
			     axnet_irq_handler, IRQF_SHARED, ndev->name, ndev);
	if (ret < 0) {
		dev_err(&axnet->pdev->dev, "request_irq() fail: %d\n", ret);
		goto unreg_netdev;
	}

#ifdef SHMEM_FROM_MASTER
	ret = axnet_pci_alloc_shmem(axnet);
	if (ret) {
		pr_debug("axnet allc shmem failed\n");
		goto unreg_netdev;
	}
#endif

	return 0;

unreg_netdev:
	unregister_netdev(ndev);
del_napi:
	netif_napi_del(&axnet->napi);
free_netdev:
	free_netdev(ndev);
fail:
	return ret;
}

static void axnet_pci_exit_one(int idx)
{
	struct axera_dev *ax_dev = g_axera_dev_map[idx];
	struct axnet_priv *axnet = (struct axnet_priv *)ax_dev->priv_data;
	struct net_device *ndev;
	if (axnet == NULL) {
		return;
	}
	ndev = axnet->ndev;

	pr_info("axnet pci exit\n");

	//call this first, it will call net close func
	dma_free_coherent(&axnet->pdev->dev, axnet->org_size, axnet->org_addr, axnet->org_phys_addr);
	unregister_netdev(ndev);
	devm_free_irq(&axnet->pdev->dev, pci_irq_vector(axnet->pdev, MSI_IRQ_NUM), ndev);
	netif_napi_del(&axnet->napi);
	free_netdev(ndev);
}

static int __init axnet_pci_init(void)
{
	int i;
	for (i = 0; i < g_axera_pcie_opt->remote_device_number; i++) {
		axnet_pci_init_one(i);
	}
	return 0;
}

module_init(axnet_pci_init);

static void __exit axnet_pci_exit(void)
{
	int i;
	for (i = 0; i < g_axera_pcie_opt->remote_device_number; i++) {
		axnet_pci_exit_one(i);
	}
}
module_exit(axnet_pci_exit);


MODULE_DESCRIPTION("AX PCI VIRTUAL NETWORK DRIVER");
MODULE_AUTHOR("AX");
MODULE_LICENSE("GPL v2");

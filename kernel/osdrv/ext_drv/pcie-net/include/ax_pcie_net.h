#ifndef PCIE_EPF_AX_DMA_H
#define PCIE_EPF_AX_DMA_H

#define PCI_VENDOR_ID_AX 0x1F4B
#define PCI_VENDOR_ID_AX_NET 0x0222

/* Network link timeout 5 sec */
#define LINK_TIMEOUT 5000

// #define AXNET_DEFAULT_MTU 4000
#define AXNET_DEFAULT_MTU 64000
#define AXNET_MIN_MTU 68
#define AXNET_MAX_MTU AXNET_DEFAULT_MTU

#define AXNET_NAPI_WEIGHT	64

#ifdef IS_THIRD_PARTY_PLATFORM
#pragma message("build for x86 platform")
#endif

// #define RING_COUNT  100
#define RING_COUNT  60

#define MSI_IRQ_NUM 2

#define ENABLE_SIMPLE_DMA 1
#define USE_MAILBOX 1

#if USE_MAILBOX
/* slave mailbox */
#define PCIE_INFO 0x100
#define PCIE_DATA 0x0
#define	MAX_DATA_SIZE 32
#define REQUEST_TIMEOUT 60000	//60ms

#define REG32(x)	(*(volatile unsigned int *)(x))
#define REG64(x)	(*(volatile unsigned long *)(x))
#define MAILBOX_REG_MAP_ADDR 0x4520000
#define MAILBOX_REG_BASE (0xC000)

#define PCIE_MASTERID	5
#define CPU0_MASTERID	0
#define PCIE_SLOT_REQ ((MAILBOX_REG_BASE + 0x308) | (PCIE_MASTERID<<4))
#define PCIE_SLOT_UNLOCK ((MAILBOX_REG_BASE + 0x30C) | (PCIE_MASTERID<<4))
#define PCIE_INT_STATS ((MAILBOX_REG_BASE + 0x300) | (PCIE_MASTERID<<4))
#define PCIE_INT_CLR ((MAILBOX_REG_BASE + 0x304) | (PCIE_MASTERID<<4))
#endif

#define EP_LINK_MAGIC  0x12121212abababab

#define INTERFACE_NAME "ax-net%d"

struct ring_buf_md {
	u64 offset;
	u64 size;
};

struct irq_md {
	u64 irq_ctrl;
	u64 irq_data;
	u64 mailbox_int;
	u64 rd_cnt;
	u64 mailbox_irq_cnt;
};

struct bar_md {
	/* IRQ generation for control packets */
	u64 irq_offset;
	/* Ring buffers counter offset */
	u64 ep_own_cnt_offset;
	u64 host_own_cnt_offset;
	/* Ring buffers location offset */
	struct ring_buf_md ep2h_ctrl_md;
	struct ring_buf_md h2ep_ctrl_md;
	struct ring_buf_md ep2h_data_md;
	struct ring_buf_md h2ep_data_md;
	/* RAM region for use by host when programming EP DMA controller */
	u64 host_dma_offset;
	u64 host_dma_size;
	/* Endpoint will map all RX packet buffers into this region */
	u64 bar0_base_phy;
	u64 ep_rx_pkt_offset;
	u64 ep_rx_pkt_size;
	u64 link_magic;
	u64 ep_link_status;
};

enum ctrl_msg_type {
	CTRL_MSG_RESERVED,
	CTRL_MSG_LINK_UP,
	CTRL_MSG_LINK_DOWN,
	CTRL_MSG_LINK_DOWN_ACK,
};

struct ctrl_msg {
	u64 msg_id;		/* enum ctrl_msg_type */
	u64 reserved[1];
};

enum data_msg_type {
	DATA_MSG_RESERVED,
	DATA_MSG_EMPTY_BUF,
	DATA_MSG_FULL_BUF,
};

struct data_msg {
	u64 msg_id;		/* enum data_msg_type */

	u64 buffer_len;
	u64 packet_size;
	u64 free_pcie_address;
	u64 busy_pcie_address;

	u64 reserved[2];
};

struct axnet_counter {
	u64 *rd;
	u64 *wr;
};

struct fifo_cnt {
	// indicata the usage of ctrl fifo and if ctrl msg is avalible or not
	u64 ctrl_rd_cnt;	//recv, +1
	u64 ctrl_wr_cnt;	//send, +1

	// indicata the usage of fifo, for free_pcie_address
	// avalible fifo counts: wr - rd
	u64 fifo_rd_cnt;	//send, +1
	u64 fifo_wr_cnt;	//alloc new buf, +1

	// indicate if packet is avalible or not, for busy_pcie_address
	// avalible packets counts: wr - rd
	u64 data_rd_cnt;	//recv, +1
	u64 data_wr_cnt;	//send, +1
};

struct ep_ring_buf {
	struct fifo_cnt *ep_cnt;
	/* Endpoint written message buffers */
	struct ctrl_msg *h2ep_ctrl_msgs;
	struct data_msg *h2ep_data_msgs;
	struct irq_md *h2ep_irq_msgs;
};

struct host_ring_buf {
	struct fifo_cnt *host_cnt;
	/* Host written message buffers */
	struct ctrl_msg *ep2h_ctrl_msgs;
	struct data_msg *ep2h_data_msgs;
};

struct ep2h_empty_list {
	int len;
	dma_addr_t iova;
	struct sk_buff *skb;
	struct list_head list;
};

struct h2ep_empty_list {
	int size;
	struct page *page;
	void *virt;
	dma_addr_t iova;
	struct list_head list;
};

enum dir_link_state {
	DIR_LINK_STATE_DOWN,
	DIR_LINK_STATE_UP,
	DIR_LINK_STATE_SENT_DOWN,
};

enum os_link_state {
	OS_LINK_STATE_UP,
	OS_LINK_STATE_DOWN,
};

#ifdef IS_THIRD_PARTY_PLATFORM
inline void ax_pcie_spin_lock(unsigned long *flags) {};
inline void ax_pcie_spin_unlock(unsigned long *flags) {};
#else
extern void ax_pcie_spin_lock(unsigned long *flags);
extern void ax_pcie_spin_unlock(unsigned long *flags);
#endif

static inline bool axnet_ivc_empty(struct axnet_counter *counter)
{
	u64 rd, wr;

#ifdef PCIE_MASTER
	unsigned long dbi_flags = 0;;
	ax_pcie_spin_lock(&dbi_flags);
#endif
	wr = READ_ONCE(*counter->wr);
	rd = READ_ONCE(*counter->rd);
#ifdef PCIE_MASTER
	ax_pcie_spin_unlock(&dbi_flags);
#endif

	if (wr - rd > RING_COUNT)
		return true;

	return wr == rd;
}

static inline bool axnet_ivc_full(struct axnet_counter *counter)
{
	u64 rd, wr;

#ifdef PCIE_MASTER
	unsigned long dbi_flags = 0;;
	ax_pcie_spin_lock(&dbi_flags);
#endif
	wr = READ_ONCE(*counter->wr);
	rd = READ_ONCE(*counter->rd);
#ifdef PCIE_MASTER
	ax_pcie_spin_unlock(&dbi_flags);
#endif

	return wr - rd >= RING_COUNT;
}

static inline u64 axnet_ivc_rd_available(struct axnet_counter *counter)
{
	u64 rd, wr;

#ifdef PCIE_MASTER
	unsigned long dbi_flags = 0;;
	ax_pcie_spin_lock(&dbi_flags);
#endif
	wr = READ_ONCE(*counter->wr);
	rd = READ_ONCE(*counter->rd);
#ifdef PCIE_MASTER
	ax_pcie_spin_unlock(&dbi_flags);
#endif

	return wr - rd;
}

static inline u64 axnet_ivc_wr_available(struct axnet_counter *counter)
{
	u64 rd, wr;

#ifdef PCIE_MASTER
	unsigned long dbi_flags = 0;;
	ax_pcie_spin_lock(&dbi_flags);
#endif
	wr = READ_ONCE(*counter->wr);
	rd = READ_ONCE(*counter->rd);
#ifdef PCIE_MASTER
	ax_pcie_spin_unlock(&dbi_flags);
#endif

	return (RING_COUNT - (wr - rd));
}

static inline void axnet_ivc_advance_wr(struct axnet_counter *counter)
{
#ifdef PCIE_MASTER
	unsigned long dbi_flags = 0;;
	ax_pcie_spin_lock(&dbi_flags);
#endif
	WRITE_ONCE(*counter->wr, READ_ONCE(*counter->wr) + 1);
#ifdef PCIE_MASTER
	ax_pcie_spin_unlock(&dbi_flags);
#endif

	/* BAR0 mmio address is wc mem, add mb to make sure cnts are updated */
	smp_mb();
}

static inline void axnet_ivc_advance_rd(struct axnet_counter *counter)
{
#ifdef PCIE_MASTER
	unsigned long dbi_flags = 0;;
	ax_pcie_spin_lock(&dbi_flags);
#endif
	WRITE_ONCE(*counter->rd, READ_ONCE(*counter->rd) + 1);
#ifdef PCIE_MASTER
	ax_pcie_spin_unlock(&dbi_flags);
#endif

	/* BAR0 mmio address is wc mem, add mb to make sure cnts are updated */
	smp_mb();
}

static inline u64 axnet_ivc_get_wr_cnt(struct axnet_counter *counter)
{
	u64 wr;

#ifdef PCIE_MASTER
	unsigned long dbi_flags = 0;;
	ax_pcie_spin_lock(&dbi_flags);
#endif
	wr = READ_ONCE(*counter->wr);
#ifdef PCIE_MASTER
	ax_pcie_spin_unlock(&dbi_flags);
#endif

	return wr;
}

static inline u64 axnet_ivc_get_rd_cnt(struct axnet_counter *counter)
{
	u64 rd;

#ifdef PCIE_MASTER
	unsigned long dbi_flags = 0;;
	ax_pcie_spin_lock(&dbi_flags);
#endif
	rd = READ_ONCE(*counter->rd);
#ifdef PCIE_MASTER
	ax_pcie_spin_unlock(&dbi_flags);
#endif

	return rd;
}

#endif

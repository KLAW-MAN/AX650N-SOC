#ifndef __PCIE_DMA_TRANSFER_HEADER__
#define __PCIE_DMA_TRANSFER_HEADER__

// #include <linux/time64.h>
// #include <linux/timex.h>

#define DMA_ADDR_ALIGN_MASK		((1<<2)-1)

#define DMA_TIMEOUT_JIFFIES	10
#define DMA_TASK_NR		256
#define LLP_TASK_NR		32
#define PCIE_MAX_DMA_BLK	64	/* max task per block */
#define PCIE_MAX_DMATASK	256	/* max task */

enum dma_channel_type {
	PCI_DMA_READ = 0x0,
	PCI_DMA_WRITE = 0x1,
};

struct element_ctrl {
	unsigned int cycle_bit : 1;	/* bit[0] */
	unsigned int tcb : 1;	/* bit[1] */
	unsigned int llp : 1;	/* bit[2] */
	unsigned int lwie : 1;	/* bit[3] */
	unsigned int rwie : 1;	/* bit[4] */
	unsigned int rsd : 12;	/* bit[5:16] */
	unsigned int prfh : 5;	/* bit[17:21] */
	unsigned int rsd1 : 10;	/* bit[22:31] */
};

struct data_element {
	struct element_ctrl control;
	unsigned int size;
	unsigned int src_low;
	unsigned int src_high;
	unsigned int dst_low;
	unsigned int dst_high;
};

struct link_element {
	struct element_ctrl control;
	unsigned int reserved;
	unsigned int llp_low;
	unsigned int llp_high;
};

struct pcie_dma_task {
	struct list_head list;
	unsigned int state;
	unsigned int dir;
	unsigned long src;
	unsigned long dest;
	unsigned int len;
	unsigned int llpen;
	unsigned long llpaddr;
	void *private_data;
	void (*finish) (struct pcie_dma_task *task);
};

struct pcie_dma_ptask {
	struct list_head list;
	unsigned int type;
	struct pcie_dma_task task;
};

enum dma_task_status {
	DMA_TASK_TODO = 0,
	DMA_TASK_CREATED = 1,
	DMA_TASK_FINISHED = 2,
	DMA_TASK_EMPTY = 0xffffffff,
};

enum dma_channel_state {
	CHANNEL_ERROR = -1,
	CHANNEL_IDLE = 0,
	CHANNEL_BUSY = 1,
};

enum channel_transfer_object {
	__VDATA = 0,
	__CMESSAGE = 1,
	__NOBODY = 2,
};

struct dma_object {
	struct list_head busy_list_head;
	struct list_head free_list_head;
	struct pcie_dma_ptask task_array[DMA_TASK_NR];
	void *reserved;
};

struct llp_mem {
	struct list_head list;
	dma_addr_t phyaddr;
	unsigned int max_size;
	void *virtaddr;
};

struct llp_object {
	struct list_head free_list_head;
	struct llp_mem llp_array[PCIE_MAX_DMA_BLK];
};

#define MSG_DMA_ENABLE
struct dma_channel {
	unsigned int type;
	spinlock_t mlock;
	struct semaphore sem;

#ifdef MSG_DMA_ENABLE
	struct dma_object msg;
#endif
	struct dma_object data;

	unsigned int error;
	int state;
	int owner;
	void *pri;
};

struct dma_control {
	struct dma_channel write;
	struct dma_channel read;
};

struct pcie_dma_req {
	unsigned int dir;
	unsigned long src;
	unsigned long dest;
	unsigned int len;
	unsigned int last;
};

typedef struct ax_pcie_dma_block {
	unsigned long src_addr;	/* source address of dma task */
	unsigned long dst_addr;	/* destination address of dma task */
	unsigned int blk_size;	/* data block size of dma task */
} ax_pcie_dma_block_t;

typedef struct ax_pcie_dma_task {
	unsigned int count;	/* total dma task number */
	bool is_read;		/* dam task is  read or write data */
	ax_pcie_dma_block_t *pBlock;
} ax_pcie_dma_task_t;

typedef struct ax_pcie_send_task {
	struct list_head list;

	bool is_read;
	unsigned long src_phy_addr;
	unsigned long dst_phy_addr;
	unsigned int len;
	unsigned long private_data[5];
	void (*pCallBack) (struct ax_pcie_send_task *pstTask);
} ax_pcie_send_task_t;

typedef struct ax_pcie_dma_node {
	struct list_head list;

	wait_queue_head_t stwqDmaDone;
	bool bDmaDone;

	struct timespec64 ts_start;
	struct timespec64 ts_end;
	struct timespec64 ts_end2;
} ax_pcie_dma_node_t;

#define AX_IOC_PCIe_BASE		'H'
#define AX_IOC_PCIE_DMA_TRANS		_IOW(AX_IOC_PCIe_BASE, 1, struct pcie_dma_req)
#define AX_IOC_PCIE_DMA_TASK        _IOW(AX_IOC_PCIe_BASE, 4, ax_pcie_dma_task_t)
#define AX_IOC_PCIE_LLP_TASK        _IOW(AX_IOC_PCIe_BASE, 5, ax_pcie_dma_task_t)

int pcie_create_task(struct pcie_dma_task *task);
struct pcie_dma_task *__pcie_create_task(struct pcie_dma_task *task, int type);

#endif

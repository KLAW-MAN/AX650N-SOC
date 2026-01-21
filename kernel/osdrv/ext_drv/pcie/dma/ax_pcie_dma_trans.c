#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/types.h>
#include <linux/timer.h>
#include <linux/miscdevice.h>
#include <linux/init.h>
#include <asm/irq.h>
#include <asm/io.h>
#include <asm/delay.h>
#include <linux/kthread.h>
#include <linux/interrupt.h>
#include <linux/pci.h>
#include <linux/wait.h>
#include <linux/poll.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/delay.h>
#include <linux/timex.h>
#include <asm/cacheflush.h>
#include <linux/dma-mapping.h>
#include <linux/of_device.h>

#include "config.h"
#include "ax_pcie_dev.h"
#ifdef DMA_PROC_ENABLE
#include "ax_pcie_proc.h"
#endif
#include "ax_pcie_dma_trans.h"
#include "ax_pcie_dma.h"

#define PCIe_DEBUG  4
#define PCIe_INFO   3
#define PCIe_ERR    2
#define PCIe_FATAL  1
#define PCIe_CURR_LEVEL 2

#define PCIe_PRINT(level, s, params...) do { \
	if (level <= PCIe_CURR_LEVEL)  \
	printk("[%s, %d]: " s "\n", __func__, __LINE__, ##params); \
} while (0)

static struct miscdevice dma_miscdev;
static u64 dma_mask = 0x4ffffffff;

#define DMA_TRANS_TIMEOUT  5000 //5s

#define DMA_CUR_TASK(x) list_entry(x.next, struct pcie_dma_ptask, list)

static struct dma_control s_pcie_dma;
static struct llp_object s_llp_dma;

static unsigned int s_w_repeated;
static unsigned int s_r_repeated;

static struct list_head g_listHeadUserDma;

static ax_pcie_dma_node_t g_stUserDmaPool[PCIE_MAX_DMATASK];

static spinlock_t g_PcieLock;
#define PCIE_SPIN_LOCK   spin_lock_irqsave(&g_PcieLock, flags)
#define PCIE_SPIN_UNLOCK spin_unlock_irqrestore(&g_PcieLock, flags)

static atomic_t g_init;


#ifdef DMA_PROC_ENABLE
extern struct proc_message pcie_proc;

void get_trans_list_info(void)
{
	unsigned int data_read_count = 0;
	unsigned int data_write_count = 0;
	struct list_head *ptmp;

	ptmp = s_pcie_dma.write.data.busy_list_head.next;
	while (ptmp != &s_pcie_dma.write.data.busy_list_head) {
		data_write_count++;
		ptmp = ptmp->next;
	}

	ptmp = s_pcie_dma.read.data.busy_list_head.next;
	while (ptmp != &s_pcie_dma.read.data.busy_list_head) {
		data_read_count++;
		ptmp = ptmp->next;
	}

	((struct dma_list_count *)pcie_proc.dma_list.data)->data_read_busy =
	    data_read_count;
	((struct dma_list_count *)pcie_proc.dma_list.data)->data_write_busy =
	    data_write_count;
}
#endif

#ifdef PCI_TRANSFER_TIMER
static struct timer_list s_dma_timeout_timer;
static unsigned long s_last_jiffies;

/*
 * This function is for guarding every DMA transferring task
 * to be successfully transferred. If a DMA transfer does not
 * finished in a very large time interval[10ms for example],
 * we assume that it won't be finished forever, for something
 * wrong has happened in PCI link. And the only thing to do
 * is to stop PCI DMA transferring to prevent further damage
 * to the system.
 */
void pcie_timer_proc(unsigned long __data)
{
	struct pcie_dma_task *new_task;
	unsigned long flags;

	PCIe_PRINT(PCIe_ERR, "last jiffies[%lu], jiffies[%lu]",
		   s_last_jiffies, jiffies);
	s_last_jiffies = jiffies;

	/* For DMA write first */
	spin_lock_irqsave(&(s_pcie_dma.write.mlock), flags);
	if (CHANNEL_BUSY == s_pcie_dma.write.state) {
		if (__CMESSAGE == s_pcie_dma.write.owner) {
			new_task =
			    DMA_CUR_TASK(s_pcie_dma.write.msg.busy_list_head);
		} else {
			new_task =
			    DMA_CUR_TASK(s_pcie_dma.write.data.busy_list_head);
		}
		PCIe_PRINT(PCIe_ERR, "DMA task info[src_addr:0x%08lx;"
			   "dest_addr:0x%08lx;len:0x%x ]",
			   new_task->src, new_task->dest, new_task->len);

		g_axera_pcie_opt->stop_dma_transfer(PCI_DMA_WRITE);
	}
	spin_unlock_irqrestore(&(s_pcie_dma.write.mlock), flags);

	/* For DMA read */
	spin_lock_irqsave(&(s_pcie_dma.read.mlock), flags);
	if (CHANNEL_BUSY == s_pcie_dma.read.state) {
		if (__CMESSAGE == s_pcie_dma.read.owner) {
			new_task =
			    DMA_CUR_TASK(s_pcie_dma.read.msg.busy_list_head);
		} else {
			new_task =
			    DMA_CUR_TASK(s_pcie_dma.read.data.busy_list_head);
		}
		PCIe_PRINT(PCIe_ERR, "DMA task info[src_addr:0x%08lx;"
			   "dest_addr:0x%08lx;len:0x%x ]",
			   new_task->src, new_task->dest, new_task->len);

		g_axera_pcie_opt->stop_dma_transfer(PCI_DMA_READ);
	}
	spin_unlock_irqrestore(&(s_pcie_dma.read.mlock), flags);
}
#endif

static void start_dma_task(struct pcie_dma_ptask *new_task)
{
#ifdef PCI_TRANSFER_TIMER
	s_dma_timeout_timer.function = pcie_timer_proc;
	mod_timer(&s_dma_timeout_timer, jiffies + DMA_TIMEOUT_JIFFIES);
#endif

	g_axera_pcie_opt->start_dma_task(new_task);

#ifdef DMA_PROC_ENABLE
	if (PROC_LEVEL_II <= pcie_proc.proc_level) {
		if (PCI_DMA_READ == new_task->task.dir) {
			((struct dma_info *)pcie_proc.dma_info.data)->
			    read.src_address = new_task->task.src;
			((struct dma_info *)pcie_proc.dma_info.data)->
			    read.dest_address = new_task->task.dest;
			((struct dma_info *)pcie_proc.dma_info.data)->read.len =
			    new_task->task.len;
			if (__VDATA == new_task->type)
				((struct dma_count *)pcie_proc.dma_count.
				 data)->data_read++;
		} else {
			((struct dma_info *)pcie_proc.dma_info.data)->
			    write.src_address = new_task->task.src;
			((struct dma_info *)pcie_proc.dma_info.data)->
			    write.dest_address = new_task->task.dest;
			((struct dma_info *)pcie_proc.dma_info.data)->write.
			    len = new_task->task.len;
			if (__VDATA == new_task->type)
				((struct dma_count *)pcie_proc.dma_count.
				 data)->data_write++;
		}
	}
#endif
}

struct pcie_dma_ptask *__do_create_task(struct dma_channel *channel,
					struct pcie_dma_task *task, int type)
{
	struct list_head *list;
	struct list_head *busy_head = NULL;
	struct list_head *free_head = NULL;
	struct pcie_dma_ptask *new_task = NULL;
	unsigned long flags;

	spin_lock_irqsave(&channel->mlock, flags);

	if (__CMESSAGE == type) {
		free_head = &channel->msg.free_list_head;
		busy_head = &channel->msg.busy_list_head;
	} else if (__VDATA == type) {
		free_head = &channel->data.free_list_head;
		busy_head = &channel->data.busy_list_head;
	} else {
		spin_unlock_irqrestore(&channel->mlock, flags);
		PCIe_PRINT(PCIe_ERR, "Wrong DMA task type!");
		return NULL;
	}

	if (unlikely(list_empty(free_head))) {
		spin_unlock_irqrestore(&channel->mlock, flags);
		PCIe_PRINT(PCIe_ERR, "Too many DMA %s %s tasks!",
			   (__CMESSAGE == type) ? "msg" : "data",
			   (PCI_DMA_READ == task->dir) ? "read" : "write");
		return NULL;
	}

	list = free_head->next;
	list_del(list);
	new_task = list_entry(list, struct pcie_dma_ptask, list);
	if ((__CMESSAGE == type) && (DMA_TASK_EMPTY != new_task->task.state)) {
		PCIe_PRINT(PCIe_ERR,
			   "## %s state 0x%x,old_src 0x%lx,old_dest 0x%lx.",
			   (PCI_DMA_READ == task->dir) ? "read" : "write",
			   new_task->task.state, new_task->task.src,
			   new_task->task.dest);
		if ((DMA_TASK_FINISHED == new_task->task.state)
		    && (NULL != new_task->task.private_data)) {
			PCIe_PRINT(PCIe_ERR,
				   "Fatal: Thread sleep for too long!!!!!!!");
		} else {
			PCIe_PRINT(PCIe_ERR,
				   "unknow dma_task state[0x%x] in free list!",
				   new_task->task.state);
			if (NULL != new_task->task.private_data)
				PCIe_PRINT(PCIe_ERR,
					   "DMA task wait is not NULL!");

		}

		list_add_tail(list, free_head);
		spin_unlock_irqrestore(&channel->mlock, flags);
		PCIe_PRINT(PCIe_ERR, "Ignore this DMA task !");
		return NULL;
	}

	new_task->type = type;
	memcpy(&new_task->task, task, sizeof(*task));
	if (list_empty(busy_head)
	    && (CHANNEL_IDLE == channel->state)) {
		PCIe_PRINT(PCIe_DEBUG, "add task into list & start");
		/*
		 * Nothing else to be transferred, transfer current task immediately.
		 */
		list_add_tail(list, busy_head);
		channel->state = CHANNEL_BUSY;
		channel->owner = new_task->type;
		start_dma_task(new_task);
	} else {
		PCIe_PRINT(PCIe_DEBUG, "add task into list");
		/*
		 * There are some other dma tasks to be transferred, queue first,
		 * and wait to be transferred.
		 */
		list_add_tail(list, busy_head);
	}

	spin_unlock_irqrestore(&channel->mlock, flags);

	return new_task;
}

struct pcie_dma_task *__pcie_create_task(struct pcie_dma_task *task, int type)
{
	struct pcie_dma_ptask *new_task = NULL;

	if (PCI_DMA_READ == task->dir) {
		new_task = __do_create_task(&s_pcie_dma.read, task, type);
	} else if (PCI_DMA_WRITE == task->dir) {
		new_task = __do_create_task(&s_pcie_dma.write, task, type);
	} else {
		PCIe_PRINT(PCIe_ERR, "Unknow DMA direction![read/write?]");
	}

	if (NULL == new_task)
		return NULL;

	return &new_task->task;
}

EXPORT_SYMBOL(__pcie_create_task);

/*
 * Create a DMA task an transfer it.
 * Return value:
 * NULL: create a DMA task failed.
 * Not NULL: success.
 * This function is for DATA transfer.
 */
int pcie_create_task(struct pcie_dma_task *task)
{
	unsigned long src = 0;
	unsigned long dest = 0;

	src = task->src;
	dest = task->dest;

	if (!g_axera_pcie_opt->has_dma()) {
		PCIe_PRINT(PCIe_ERR,
			   "Host DMA is temporarily not supported!");
		PCIe_PRINT(PCIe_ERR, "Please use slave DMA instead.");
		return -1;
	}

	/* DMA data length should never be zero */
	if (task->len == 0) {
		PCIe_PRINT(PCIe_FATAL, "FATAL: DMA data len is zero!!");
		return -1;
	}

#if 0
	/*
	 * Both DMA src and dest address should compliant with
	 * the rule that 4 Bytes align.
	 */
	src &= DMA_ADDR_ALIGN_MASK;
	dest &= DMA_ADDR_ALIGN_MASK;
	if (unlikely(src || dest)) {
		PCIe_PRINT(PCIe_FATAL, "FATAL: DMA addr not 4Bytes align!");
		return -1;
	}
#endif

	if (NULL == __pcie_create_task(task, __VDATA)) {
		PCIe_PRINT(PCIe_ERR, "Create dma task failed!");
		return -1;
	}

	return 0;
}

EXPORT_SYMBOL(pcie_create_task);

static void dma_task_finish(struct pcie_dma_task *task)
{
	ax_pcie_dma_node_t *user_dma_node = NULL;

#ifdef MEASURE_BANDWIDTH
	unsigned long long delta_us;
	unsigned long long bandwidth;
	unsigned long long len;
	struct timespec64 ts_delta;
#endif

	user_dma_node = (ax_pcie_dma_node_t *) task->private_data;
	if (user_dma_node == NULL) {
		PCIe_PRINT(PCIe_ERR, "user_dma_node NULL\n");
		return;
	}
	user_dma_node->bDmaDone = true;

#ifdef MEASURE_BANDWIDTH
	ktime_get_boottime_ts64(&(user_dma_node->ts_end));
	ts_delta = timespec64_sub(user_dma_node->ts_end, user_dma_node->ts_start);
	delta_us = timespec64_to_ns(&ts_delta) / 1000;
	len = task->len;
	bandwidth = (len * 1000000) / delta_us / 1024 / 1024;

	PCIe_PRINT(PCIe_INFO,
		   "bandwidth1, total size: %d bytes(%d kb), total time: %lld us",
		   task->len, task->len / 1024, delta_us);
	PCIe_PRINT(PCIe_INFO, "bandwidth1, src:%lx -> dst:%lx, bandwidth: %lld MB/s",
		task->src, task->dest, bandwidth);
#endif

	wake_up(&user_dma_node->stwqDmaDone);

	PCIe_PRINT(PCIe_DEBUG, "PCIe DMA transfer success");
}

void pcie_hal_dma_finish(struct pcie_dma_task *task)
{
	ax_pcie_send_task_t *stask = (ax_pcie_send_task_t *) task->private_data;

	if (NULL != stask) {
		PCIe_PRINT(PCIe_DEBUG, "task number %ld finished, total: %ld",
			   stask->private_data[0], stask->private_data[1]);
		if (NULL != stask->pCallBack) {
			stask->pCallBack(stask);
		}
		kfree(stask);
	}
}

/* The interupter must be lock when call this function */
void pcie_hal_start_dma_task(ax_pcie_send_task_t *pTask)
{
	struct pcie_dma_task pciTask;

	memset(&pciTask, 0, sizeof(struct pcie_dma_task));
	pciTask.dir = pTask->is_read ? PCI_DMA_READ : PCI_DMA_WRITE;
	pciTask.src = pTask->src_phy_addr;
	pciTask.dest = pTask->dst_phy_addr;
	pciTask.len = pTask->len;
	pciTask.state = DMA_TASK_TODO;
	pciTask.finish = pcie_hal_dma_finish;
	pciTask.private_data = pTask;	/* point to address of pTask */

	/* If create task fail, this task will lost */
	if (0 == pcie_create_task(&pciTask)) {
		;
	} else {
		PCIe_PRINT(PCIe_ERR, "create dma task failed");
		return;
	}
}

int pcie_hal_add_dma_task(ax_pcie_send_task_t *pTask)
{
	ax_pcie_send_task_t *task_tmp;

	task_tmp = kmalloc(sizeof(ax_pcie_send_task_t), GFP_ATOMIC);
	if (!task_tmp) {
		PCIe_PRINT(PCIe_ERR, "alloc memory SEND_TASK_S failed!");
		return -1;
	}
	memcpy(task_tmp, pTask, sizeof(ax_pcie_send_task_t));

	pcie_hal_start_dma_task(task_tmp);

	return 0;
}

void pcie_hal_dma_task_done(ax_pcie_send_task_t *pstTask)
{
	ax_pcie_dma_node_t *user_dma_node = NULL;

	if (!((pstTask->private_data[0] + 1) == pstTask->private_data[1])) {
		PCIe_PRINT(PCIe_ERR, "should not call dma callback");
	}
	PCIe_PRINT(PCIe_DEBUG, "PCIe DMA multi-task done!");

	user_dma_node = (ax_pcie_dma_node_t *) pstTask->private_data[2];
	if (user_dma_node == NULL) {
		PCIe_PRINT(PCIe_ERR, "user_dma_node NULL\n");
		return;
	}
	user_dma_node->bDmaDone = true;

	wake_up(&user_dma_node->stwqDmaDone);
}

int start_pcie_dma_multi_task(ax_pcie_dma_task_t *pTask)
{
	int i;
	int ret = 0;
	unsigned long flags;
	ax_pcie_send_task_t pcie_send_task;
	ax_pcie_dma_node_t *user_dma_node = NULL;

	PCIE_SPIN_LOCK;
	if (list_empty(&g_listHeadUserDma)) {
		PCIe_PRINT(PCIe_ERR, "pcie dma busy!");
		PCIE_SPIN_UNLOCK;
		return -1;
	}

	user_dma_node = list_entry(g_listHeadUserDma.next, ax_pcie_dma_node_t, list);
	list_del(g_listHeadUserDma.next);
	PCIE_SPIN_UNLOCK;

	user_dma_node->bDmaDone = false;

	PCIe_PRINT(PCIe_DEBUG, "create pcie dma multi-task, total: %d tasks",
		   pTask->count);
	for (i = 0; i < pTask->count; i++) {
		pcie_send_task.src_phy_addr = pTask->pBlock[i].src_addr;
		pcie_send_task.dst_phy_addr = pTask->pBlock[i].dst_addr;
		pcie_send_task.len = pTask->pBlock[i].blk_size;
		pcie_send_task.is_read = pTask->is_read;
		pcie_send_task.private_data[0] = i;
		pcie_send_task.private_data[1] = pTask->count;
		pcie_send_task.private_data[2] = (unsigned long)user_dma_node;
		pcie_send_task.pCallBack = NULL;

		/* If this is the last task node, we set the callback */
		if ((i + 1) == pTask->count) {
			pcie_send_task.pCallBack = pcie_hal_dma_task_done;
		}

		if (0 != pcie_hal_add_dma_task(&pcie_send_task)) {
			PCIe_PRINT(PCIe_ERR, "pcie dma add task failed");
			ret = -1;
			break;
		}
	}

	PCIe_PRINT(PCIe_DEBUG, "wait dma multi-task done event...");
	if (0 == ret) {
		unsigned int time_left;
		time_left = wait_event_timeout(user_dma_node->stwqDmaDone,
						(user_dma_node->bDmaDone == true), DMA_TRANS_TIMEOUT);
		if (0 == time_left && user_dma_node->bDmaDone == false) {
			PCIe_PRINT(PCIe_ERR, "wait event timeout.");
			ret = -1;
		}
	}

	PCIE_SPIN_LOCK;
	list_add_tail(&user_dma_node->list, &g_listHeadUserDma);
	PCIE_SPIN_UNLOCK;

	return ret;
}

int fill_dma_llp_info(unsigned long phyllp, ax_pcie_dma_task_t *llptask, struct data_element *dataelement, struct link_element *linkelement)
{
	int num = llptask->count;
	int i;

	/* fill data element */
	for (i = 0; i < num; i++) {
		dataelement[i].control.cycle_bit = 0;
		dataelement[i].control.tcb = 0;
		dataelement[i].control.llp = 0;
		dataelement[i].control.rwie = 0;
		if (i == (num -1)) {
			dataelement[i].control.lwie = 1;
		} else {
			dataelement[i].control.lwie = 0;
		}
		dataelement[i].size = llptask->pBlock[i].blk_size;
		dataelement[i].src_low = (llptask->pBlock[i].src_addr & 0xffffffff);
		dataelement[i].src_high = (llptask->pBlock[i].src_addr >> 32);
		dataelement[i].dst_low = (llptask->pBlock[i].dst_addr & 0xffffffff);
		dataelement[i].dst_high = (llptask->pBlock[i].dst_addr >> 32);
		PCIe_PRINT(PCIe_DEBUG, "blksize[%d] = %x", i, llptask->pBlock[i].blk_size);
		PCIe_PRINT(PCIe_DEBUG, "srcaddr[%d] = %lx", i, llptask->pBlock[i].src_addr);
		PCIe_PRINT(PCIe_DEBUG, "dstaddr[%d] = %lx", i, llptask->pBlock[i].dst_addr);
	}

	/* fill link element */
	linkelement->control.cycle_bit = 0;
	linkelement->control.tcb = 1;
	linkelement->control.llp = 1;
	linkelement->reserved = 0;
	linkelement->llp_low = (phyllp & 0xffffffff);
	linkelement->llp_high = (phyllp >> 32);
	return 0;
}

int start_pcie_dma_llp_task(ax_pcie_dma_task_t *llptask)
{
	struct pcie_dma_task task;
	struct pcie_dma_task *ptask = NULL;
	struct data_element *dataelement;
	struct link_element *linkelement;
	struct list_head *list;
	struct list_head *free_head = NULL;
	struct llp_mem *new_llp = NULL;
	ax_pcie_dma_node_t *user_dma_node = NULL;
	void *virtllp;
	dma_addr_t phyllp;
	unsigned int entry_num = llptask->count;
	int time_left;
	unsigned long flags;
	int llplen;
	int ret;

#ifdef MEASURE_BANDWIDTH
	unsigned long long delta_us;
	unsigned long long bandwidth;
	unsigned long long len;
	struct timespec64 ts_delta;
#endif

	PCIE_SPIN_LOCK;
	if (list_empty(&g_listHeadUserDma)) {
		PCIe_PRINT(PCIe_ERR, "pcie dma busy!");
		PCIE_SPIN_UNLOCK;
		return -1;
	}

	user_dma_node = list_entry(g_listHeadUserDma.next, ax_pcie_dma_node_t, list);
	list_del(g_listHeadUserDma.next);

	free_head = &s_llp_dma.free_list_head;
	if (unlikely(list_empty(free_head))) {
		PCIE_SPIN_UNLOCK;
		PCIe_PRINT(PCIe_ERR, "No free llp space");
		return -1;
	}
	list = free_head->next;
	list_del(list);
	new_llp = list_entry(list, struct llp_mem, list);
	PCIE_SPIN_UNLOCK;
	user_dma_node->bDmaDone = false;

	PCIe_PRINT(PCIe_DEBUG, "new_llp.phy = %llx", new_llp->phyaddr);
	PCIe_PRINT(PCIe_DEBUG, "new_llp.virtaddr = %lx", (unsigned long)new_llp->virtaddr);

	llplen = sizeof(struct data_element) * entry_num + sizeof(struct link_element);
	virtllp = new_llp->virtaddr;
	phyllp = new_llp->phyaddr;

	PCIe_PRINT(PCIe_DEBUG, "phyllp = %llx, llplen = %x", phyllp, llplen);

	memset(virtllp, 0, llplen);
	dataelement = (struct data_element *)virtllp;
	linkelement = (struct link_element *)(virtllp + (sizeof(struct data_element) * entry_num));
	PCIe_PRINT(PCIe_DEBUG, "dataelement = 0x%lx", (unsigned long)dataelement);
	PCIe_PRINT(PCIe_DEBUG, "linkelement = 0x%lx", (unsigned long)linkelement);

	ret = fill_dma_llp_info(phyllp, llptask, dataelement, linkelement);
	if (ret < 0) {
		PCIe_PRINT(PCIe_ERR, "fill dma llp info failed");
		return ret;
	}

	memset(&task, 0, sizeof(struct pcie_dma_task));

	/* fill dma task */
	task.dir = llptask->is_read ? PCI_DMA_READ : PCI_DMA_WRITE;
	task.llpaddr = phyllp;
	task.llpen = 1;
	task.finish = dma_task_finish;
	task.private_data = (void *)user_dma_node;
	task.state = DMA_TASK_TODO;

	PCIe_PRINT(PCIe_DEBUG,
		   "ioctl: start dma task, llpaddr:%lx, llpen:%d",
		   task.llpaddr, task.llpen);

#ifdef MEASURE_BANDWIDTH
	if (req.last) {
		ktime_get_boottime_ts64(&(user_dma_node->ts_start));
	}
#endif

	ptask = __pcie_create_task(&task, __VDATA);
	if (ptask == NULL) {
		PCIe_PRINT(PCIe_ERR, "Creat DMA write task failed!");
		return -1;
	}

	PCIe_PRINT(PCIe_DEBUG, "wait dma finish event...");
	time_left = wait_event_timeout(user_dma_node->stwqDmaDone,
					    user_dma_node->bDmaDone == true, DMA_TRANS_TIMEOUT);
	if (0 == time_left && user_dma_node->bDmaDone == false) {
		PCIe_PRINT(PCIe_ERR, "dma transfer timeout.");
		return -1;
	}

#ifdef MEASURE_BANDWIDTH
	if (req.last) {
		ktime_get_boottime_ts64(&(user_dma_node->ts_end2));
		ts_delta = timespec64_sub(user_dma_node->ts_end2, user_dma_node->ts_start);
		delta_us = timespec64_to_ns(&ts_delta) / 1000;
		len = task.len;
		bandwidth = (len * 1000000) / delta_us / 1024 / 1024;

		PCIe_PRINT(PCIe_INFO,
			"bandwidth2, total size: %d bytes(%d kb), total time: %lld us",
			task.len, task.len / 1024, delta_us);
		PCIe_PRINT(PCIe_INFO, "bandwidth2, src:%lx -> dst:%lx, bandwidth: %lld MB/s",
			task.src, task.dest, bandwidth);
	}
#endif

	PCIE_SPIN_LOCK;
	list_add_tail(&user_dma_node->list, &g_listHeadUserDma);
	list_add_tail(&new_llp->list, &s_llp_dma.free_list_head);
	PCIE_SPIN_UNLOCK;

	return 0;
}

int start_pcie_dma_task(struct pcie_dma_req req)
{
	struct pcie_dma_task task;
	struct pcie_dma_task *ptask = NULL;
	int time_left;
	ax_pcie_dma_node_t *user_dma_node = NULL;
	unsigned long flags;

#ifdef MEASURE_BANDWIDTH
	unsigned long long delta_us;
	unsigned long long bandwidth;
	unsigned long long len;
	struct timespec64 ts_delta;
#endif

	if (req.last) {
		PCIE_SPIN_LOCK;
		if (list_empty(&g_listHeadUserDma)) {
			PCIe_PRINT(PCIe_ERR, "pcie dma busy!");
			PCIE_SPIN_UNLOCK;
			return -1;
		}

		user_dma_node = list_entry(g_listHeadUserDma.next, ax_pcie_dma_node_t, list);
		list_del(g_listHeadUserDma.next);
		PCIE_SPIN_UNLOCK;

		user_dma_node->bDmaDone = false;
	}

	/* fill dma task */
	memset(&task, 0, sizeof(struct pcie_dma_task));
	task.dir = req.dir;
	if (task.dir == PCI_DMA_WRITE) {
		task.src = req.src;
		task.dest = req.dest;
	} else if (task.dir == PCI_DMA_READ) {
		task.src = req.src;
		task.dest = req.dest;
	} else {
		PCIe_PRINT(PCIe_ERR, "dma direction error!");
		return -1;
	}
	task.len = req.len;
	if (req.last) {
		task.finish = dma_task_finish;
	} else {
		task.finish = NULL;
	}
	task.private_data = (void *)user_dma_node;
	task.state = DMA_TASK_TODO;

	PCIe_PRINT(PCIe_DEBUG,
		   "ioctl: start dma task, size:%d, src:%lx, dst:%lx last:%d",
		   task.len, task.src, task.dest, req.last);

#ifdef MEASURE_BANDWIDTH
	if (req.last) {
		ktime_get_boottime_ts64(&(user_dma_node->ts_start));
	}
#endif

	ptask = __pcie_create_task(&task, __VDATA);
	if (ptask == NULL) {
		PCIe_PRINT(PCIe_ERR, "Creat DMA write task failed!");
		return -1;
	}

	if (req.last == 0)
		return 0;

	PCIe_PRINT(PCIe_DEBUG, "wait dma finish event...");
	time_left = wait_event_timeout(user_dma_node->stwqDmaDone,
					    user_dma_node->bDmaDone == true, DMA_TRANS_TIMEOUT);
	if (0 == time_left && user_dma_node->bDmaDone == false) {
		PCIe_PRINT(PCIe_ERR, "dma transfer timeout.");
		return -1;
	}

#ifdef MEASURE_BANDWIDTH
	if (req.last) {
		ktime_get_boottime_ts64(&(user_dma_node->ts_end2));
		ts_delta = timespec64_sub(user_dma_node->ts_end2, user_dma_node->ts_start);
		delta_us = timespec64_to_ns(&ts_delta) / 1000;
		len = task.len;
		bandwidth = (len * 1000000) / delta_us / 1024 / 1024;

		PCIe_PRINT(PCIe_INFO,
			"bandwidth2, total size: %d bytes(%d kb), total time: %lld us",
			task.len, task.len / 1024, delta_us);
		PCIe_PRINT(PCIe_INFO, "bandwidth2, src:%lx -> dst:%lx, bandwidth: %lld MB/s",
			task.src, task.dest, bandwidth);
	}
#endif

	PCIE_SPIN_LOCK;
	list_add_tail(&user_dma_node->list, &g_listHeadUserDma);
	PCIE_SPIN_UNLOCK;

	return 0;
}

static int dma_trans_open(struct inode *inode, struct file *fp)
{
	int i;

	PCIe_PRINT(PCIe_INFO, "open Axera pcie dma dev");
	if (atomic_cmpxchg(&g_init, 0, 1) != 0) {
		return 0;
	}

	spin_lock_init(&g_PcieLock);

	INIT_LIST_HEAD(&g_listHeadUserDma);
	for (i = 0; i < PCIE_MAX_DMATASK; i++) {
		init_waitqueue_head(&g_stUserDmaPool[i].stwqDmaDone);
		g_stUserDmaPool[i].bDmaDone = false;
		list_add_tail(&g_stUserDmaPool[i].list,
					  &g_listHeadUserDma);
	}

	return 0;
}

static int dma_trans_release(struct inode *inode, struct file *fp)
{
	PCIe_PRINT(PCIe_INFO, "close Axera pcie dma dev");

	return 0;
}

static ssize_t dma_trans_read(struct file *fp, char __user *buf, size_t count, loff_t *ppos)
{
	return -ENOSYS;
}

static ssize_t dma_trans_write(struct file *fp,
			       const char __user *data,
			       size_t len, loff_t *ppos)
{
	return -ENOSYS;
}

static long dma_trans_ioctl(struct file *fp,
			    unsigned int cmd, unsigned long arg)
{
	struct pcie_dma_req req;
	ax_pcie_dma_task_t dma_task;
	ax_pcie_dma_block_t dma_block[PCIE_MAX_DMA_BLK];
	int copy_size;
	long ret = 0;

	switch (cmd) {
	case AX_IOC_PCIE_DMA_TRANS:
		PCIe_PRINT(PCIe_DEBUG, "ioctl: pcie dma transfer cmd");

		if (copy_from_user((void *)&req, (void *)arg,
					       sizeof(struct pcie_dma_req))) {
			PCIe_PRINT(PCIe_ERR,
				   "Copy DMA req from usr space error.");
			return -1;
		}

		/* dma read, dma write, dma link list */
		ret = start_pcie_dma_task(req);
		break;

	case AX_IOC_PCIE_DMA_TASK:
		PCIe_PRINT(PCIe_DEBUG, "ioctl: pcie dma multi task cmd");

		if (copy_from_user(&dma_task, (void *)arg, sizeof(ax_pcie_dma_task_t))) {
			ret = -EFAULT;
			break;
		}

		if (dma_task.count > PCIE_MAX_DMA_BLK) {
			PCIe_PRINT(PCIe_ERR,
				   "the number of dma task is too large.");
			return -EFAULT;
		}

		copy_size = sizeof(ax_pcie_dma_block_t) * dma_task.count;
		if (copy_from_user(dma_block, dma_task.pBlock, copy_size)) {
			ret = -EFAULT;
			break;
		}
		dma_task.pBlock = dma_block;

		ret = start_pcie_dma_multi_task(&dma_task);
		break;
	case AX_IOC_PCIE_LLP_TASK:
		PCIe_PRINT(PCIe_DEBUG, "ioctl: pcie dma link list task cmd");

		if (copy_from_user(&dma_task, (void *)arg, sizeof(ax_pcie_dma_task_t))) {
			ret = -EFAULT;
			break;
		}

		if (dma_task.count > PCIE_MAX_DMA_BLK) {
			PCIe_PRINT(PCIe_ERR,
				   "the number of dma task is too large.");
			return -EFAULT;
		}

		copy_size = sizeof(ax_pcie_dma_block_t) * dma_task.count;
		if (copy_from_user(dma_block, dma_task.pBlock, copy_size)) {
			ret = -EFAULT;
			break;
		}
		dma_task.pBlock = dma_block;

		ret = start_pcie_dma_llp_task(&dma_task);
		break;
	default:
		PCIe_PRINT(PCIe_ERR, "unknown ioctl cmd.");
		return -ENOIOCTLCMD;
	}

	return ret;
}

static struct file_operations pci_dma_fops = {
	.open = dma_trans_open,
	.release = dma_trans_release,
	.read = dma_trans_read,
	.write = dma_trans_write,
	.unlocked_ioctl = dma_trans_ioctl,
};

static struct miscdevice dma_miscdev = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "ax_pcie_dma_transfer",
	.fops = &pci_dma_fops,
};

static void __do_pcie_dma_trans(struct dma_channel *channel, int direction)
{
	unsigned long flags;
	struct pcie_dma_ptask *cur_task;
	int irq_status = 0;
	unsigned int *repeated;

	spin_lock_irqsave(&channel->mlock, flags);

	if (PCI_DMA_WRITE == direction) {
		repeated = &s_w_repeated;
		irq_status = g_axera_pcie_opt->clear_dma_write_local_irq(0);
	} else {
		repeated = &s_r_repeated;
		irq_status = g_axera_pcie_opt->clear_dma_read_local_irq(0);
	}

	if (unlikely(TRANSFER_ABORT == irq_status)) {
		/*
		 * DMA abort occurs:
		 * Try three times at most. If still failed, something
		 * beyond retrieved had happened. Nothing to do but BUG_ON.
		 */
		if (*repeated >= 3) {
			PCIe_PRINT(PCIe_ERR, "DMA %s abort timeout! quit!",
				   ((__CMESSAGE == channel->owner)
				    ? "msg" : "data"));
			channel->state = CHANNEL_ERROR;

			spin_unlock_irqrestore(&channel->mlock, flags);

			*repeated = 0;

			return;
		}

		(*repeated)++;
		channel->state = CHANNEL_BUSY;
		if (__CMESSAGE == channel->owner)
			start_dma_task(DMA_CUR_TASK
				       (channel->msg.busy_list_head));
		else
			start_dma_task(DMA_CUR_TASK
				       (channel->data.busy_list_head));

		PCIe_PRINT(PCIe_ERR, "DMA write abort! Need retry...");
	} else if (NORMAL_DONE == irq_status || LL_WATERMARK == irq_status) {
		/*
		 * DMA transfer done:
		 */
		PCIe_PRINT(PCIe_DEBUG, "DMA done %s",
			   (__CMESSAGE ==
			    s_pcie_dma.write.owner) ? "msg" : "data");

		*repeated = 0;
		if (__CMESSAGE == channel->owner) {
			cur_task = DMA_CUR_TASK(channel->msg.busy_list_head);
			cur_task->task.state = DMA_TASK_FINISHED;
			if (cur_task->task.finish)
				cur_task->task.finish(&cur_task->task);
			list_del(&cur_task->list);
			list_add_tail(&cur_task->list,
						  &channel->msg.free_list_head);
		} else if (__VDATA == channel->owner) {
			cur_task = DMA_CUR_TASK(channel->data.busy_list_head);
			cur_task->task.state = DMA_TASK_FINISHED;
			if (cur_task->task.finish)
				cur_task->task.finish(&cur_task->task);
			list_del(&cur_task->list);
			list_add_tail(&cur_task->list, &channel->data.free_list_head);
		} else {
			PCIe_PRINT(PCIe_ERR, "Wrong dma type[0x%x]!",
				   channel->owner);
		}

		channel->state = CHANNEL_IDLE;
		channel->owner = __NOBODY;

		/*
		 * Start new transfer if there are tasks in queue.
		 * MCC message has higher prioriy
		 */
		if (!list_empty(&channel->msg.busy_list_head)) {
			channel->owner = __CMESSAGE;
			channel->state = CHANNEL_BUSY;
			start_dma_task(DMA_CUR_TASK
				       (channel->msg.busy_list_head));
		}

		/*
		 * If there no message task to be transferred,
		 * check data task.
		 */
		if (!list_empty(&channel->data.busy_list_head)
		    && (channel->state == CHANNEL_IDLE)) {
			channel->owner = __VDATA;
			channel->state = CHANNEL_BUSY;
			start_dma_task(DMA_CUR_TASK
				       (channel->data.busy_list_head));
		}
	} else {
		PCIe_PRINT(PCIe_DEBUG, "unknown dma irq status, just pass!");
	}

	spin_unlock_irqrestore(&channel->mlock, flags);
}

irqreturn_t slave_dma_irq_handler(int irq, void *dev_id)
{
	PCIe_PRINT(PCIe_DEBUG, "slave pcie dma irq handler");

	/* For DMA write channel */
	__do_pcie_dma_trans(&s_pcie_dma.write, PCI_DMA_WRITE);
	/* For DMA read channel */
	__do_pcie_dma_trans(&s_pcie_dma.read, PCI_DMA_READ);

	return IRQ_HANDLED;
}

static void trans_list_init(struct dma_object *obj)
{
	int i = 0;

	obj->busy_list_head.next = &(obj->busy_list_head);
	obj->busy_list_head.prev = &(obj->busy_list_head);
	obj->free_list_head.next = &(obj->free_list_head);
	obj->free_list_head.prev = &(obj->free_list_head);

	for (i = (DMA_TASK_NR - 1); i >= 0; i--) {
		obj->task_array[i].task.state = DMA_TASK_EMPTY;
		obj->task_array[i].task.private_data = NULL;

		list_add_tail(&(obj->task_array[i].list),
					  &(obj->free_list_head));
	}
}

static void dma_llp_memory_free(struct device *dev)
{
	int i;
	for (i = 0; i < LLP_TASK_NR; i++) {
		if (s_llp_dma.llp_array[i].virtaddr != NULL)
			dma_free_coherent(dev, s_llp_dma.llp_array[i].max_size,
				s_llp_dma.llp_array[i].virtaddr, s_llp_dma.llp_array[i].phyaddr);
	}
}

static int dma_llp_memory_init(struct device *dev)
{
	int i = 0;
	void *virtaddr;
	dma_addr_t phyllp;
	int llplen = 0;

	INIT_LIST_HEAD(&s_llp_dma.free_list_head);

	memset(s_llp_dma.llp_array, 0, sizeof(s_llp_dma.llp_array));
	llplen = sizeof(struct data_element) * PCIE_MAX_DMA_BLK + sizeof(struct link_element);
	for (i = (LLP_TASK_NR - 1); i >= 0; i--) {
		virtaddr = dma_alloc_coherent(dev, llplen, &phyllp, GFP_KERNEL);
		if (!virtaddr) {
			PCIe_PRINT(PCIe_ERR, "virtllp dma alloc memory failed");
			dma_llp_memory_free(dev);
			return -1;
		}
		s_llp_dma.llp_array[i].phyaddr = phyllp;
		s_llp_dma.llp_array[i].virtaddr = virtaddr;
		s_llp_dma.llp_array[i].max_size = llplen;

		list_add_tail(&(s_llp_dma.llp_array[i].list),
					  &(s_llp_dma.free_list_head));
	}

	return 0;
}

static int dma_sys_init(void)
{
	/* For write channel */
	s_pcie_dma.write.type = PCI_DMA_WRITE;
	s_pcie_dma.write.error = 0x0;
	s_pcie_dma.write.state = CHANNEL_IDLE;
#ifdef MSG_DMA_ENABLE
	trans_list_init(&(s_pcie_dma.write.msg));
#endif
	trans_list_init(&(s_pcie_dma.write.data));

	spin_lock_init(&s_pcie_dma.write.mlock);
	sema_init(&s_pcie_dma.write.sem, 1);

	/* For read channel */
	s_pcie_dma.read.type = PCI_DMA_READ;
	s_pcie_dma.read.error = 0x0;
	s_pcie_dma.read.state = CHANNEL_IDLE;
#ifdef MSG_DMA_ENABLE
	trans_list_init(&(s_pcie_dma.read.msg));
#endif
	trans_list_init(&(s_pcie_dma.read.data));

	spin_lock_init(&s_pcie_dma.read.mlock);
	sema_init(&s_pcie_dma.read.sem, 1);

	return 0;
}

static void dma_sys_exit(void)
{
}

static int dma_arch_init(void)
{
	irqreturn_t (*handler) (int, void *);

	if (NULL == g_axera_pcie_opt) {
		PCIe_PRINT(PCIe_ERR, "g_axera_pcie_opt is NULL!");
		return -1;
	}

	if (g_axera_pcie_opt->dma_controller_init()) {
		PCIe_PRINT(PCIe_ERR, "PCI DMA controller init failed!");
		return -1;
	}

	handler = slave_dma_irq_handler;

	if (g_axera_pcie_opt->request_dma_resource(handler)) {
		PCIe_PRINT(PCIe_ERR, "PCI DMA request resources failed!");
		g_axera_pcie_opt->dma_controller_exit();
		return -1;
	}

	return 0;
}

static void dma_arch_exit(void)
{
	g_axera_pcie_opt->dma_controller_exit();
	g_axera_pcie_opt->release_dma_resource();
}

struct device *pci_dma_device_get(void)
{
	return dma_miscdev.this_device;
}
EXPORT_SYMBOL(pci_dma_device_get);

static int __init pci_dma_transfer_init(void)
{
	int ret;

	PCIe_PRINT(PCIe_INFO, "Axera pcie dma dev init");

	ax_pcie_dma_ops_init(g_axera_pcie_opt);

	ret = dma_sys_init();
	if (ret) {
		PCIe_PRINT(PCIe_ERR, "PCI DMA sys init failed!");
		return -1;
	}

	ret = dma_arch_init();
	if (ret) {
		PCIe_PRINT(PCIe_ERR, "PCI DMA arch init failed!");
		goto arch_init_err;
	}

	ret = misc_register(&dma_miscdev);
	if (ret) {
		PCIe_PRINT(PCIe_ERR, "Register DMA transfer Misc failed!");
		goto register_dev_failed;
	}

	of_dma_configure(dma_miscdev.this_device, NULL, 1);
	dma_miscdev.this_device->dma_mask = &dma_mask;
	dma_miscdev.this_device->coherent_dma_mask = dma_mask;

	ret = dma_llp_memory_init(dma_miscdev.this_device);
	if (ret < 0) {
		PCIe_PRINT(PCIe_ERR, "DMA llp memory init failed!");
		goto register_dev_failed;
	}

#ifdef PCI_TRANSFER_TIMER
	init_timer(&s_dma_timeout_timer);
#endif

#ifdef DMA_PROC_ENABLE
	pcie_proc.get_dma_trans_list_info = get_trans_list_info;
#endif

	atomic_set(&g_init, 0);

	return 0;

register_dev_failed:
	dma_arch_exit();
arch_init_err:
	dma_sys_exit();
	return ret;
}

static void __exit pci_dma_transfer_exit(void)
{
	PCIe_PRINT(PCIe_INFO, "Axera pcie dma dev exit");

	misc_deregister(&dma_miscdev);
	dma_llp_memory_free(dma_miscdev.this_device);

	dma_arch_exit();
	dma_sys_exit();
}

module_init(pci_dma_transfer_init);
module_exit(pci_dma_transfer_exit);

MODULE_AUTHOR("AXERA");
MODULE_DESCRIPTION("AXERA AX650 PCIE DMA DEV");
MODULE_LICENSE("GPL");

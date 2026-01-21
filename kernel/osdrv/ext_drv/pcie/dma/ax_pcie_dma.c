//axera ax650 pcie dma operations
#include "ax_pcie_dev.h"
#include "ax_pcie_dma_trans.h"
#include "ax_pcie_dma.h"

#define PCIe_DEBUG  4
#define PCIe_INFO   3
#define PCIe_ERR    2
#define PCIe_FATAL  1
#define PCIe_CURR_LEVEL 2

#define PCIe_PRINT(level, s, params...) do { \
	if (level <= PCIe_CURR_LEVEL)  \
		printk("[%s, %d]: " s "", __func__, __LINE__, ##params); \
} while (0)

static struct ax_pcie_operation *s_pcie_opt;
static spinlock_t s_dma_ops_lock;

static int get_pcie_dma_local_irq_number(unsigned int controller)
{
	unsigned int ctl_id = g_axera_pcie_opt->local_ctl_id;

	return g_axera_pcie_opt->local_controller[ctl_id]->irq;
}

static int is_dma_supported(void)
{
	return 1;
}

static void enable_pcie_dma_local_irq(unsigned int controller)
{
	unsigned int dma_channel_int;
	int chan = 0;

	ax_pcie_dbi_read(PCIE_DMA_BASE + chan * CHAN_ADDR_OFFSET +
			     RD_CHAN_INT_SETUP, &dma_channel_int);
	dma_channel_int |= DMA_LOCAL_ABORT_INT_EN | DMA_LOCAL_STOP_INT_EN;
	// clear stop/abort/watermark mask
	dma_channel_int &= ~0x7;
	ax_pcie_dbi_write(PCIE_DMA_BASE + chan * CHAN_ADDR_OFFSET +
			  RD_CHAN_INT_SETUP, dma_channel_int);

	ax_pcie_dbi_read(PCIE_DMA_BASE + chan * CHAN_ADDR_OFFSET +
			     WR_CHAN_INT_SETUP, &dma_channel_int);
	dma_channel_int |= DMA_LOCAL_ABORT_INT_EN | DMA_LOCAL_STOP_INT_EN;
	// clear stop/abort/watermark mask
	dma_channel_int &= ~0x7;
	ax_pcie_dbi_write(PCIE_DMA_BASE + chan * CHAN_ADDR_OFFSET +
			  WR_CHAN_INT_SETUP, dma_channel_int);
}

static void disable_pcie_dma_local_irq(unsigned int controller)
{
	unsigned int dma_channel_int;
	int chan = 0;

	ax_pcie_dbi_read(PCIE_DMA_BASE + chan * CHAN_ADDR_OFFSET +
			     RD_CHAN_INT_SETUP, &dma_channel_int);
	dma_channel_int &= ~(DMA_LOCAL_ABORT_INT_EN | DMA_LOCAL_STOP_INT_EN);
	ax_pcie_dbi_write(PCIE_DMA_BASE + chan * CHAN_ADDR_OFFSET +
			  RD_CHAN_INT_SETUP, dma_channel_int);

	ax_pcie_dbi_read(PCIE_DMA_BASE + chan * CHAN_ADDR_OFFSET +
			     WR_CHAN_INT_SETUP, &dma_channel_int);
	dma_channel_int &= ~(DMA_LOCAL_ABORT_INT_EN | DMA_LOCAL_STOP_INT_EN);
	ax_pcie_dbi_write(PCIE_DMA_BASE + chan * CHAN_ADDR_OFFSET +
			  WR_CHAN_INT_SETUP, dma_channel_int);
}

static void __start_dma_task(void *task)
{
	struct pcie_dma_task *new_task = &((struct pcie_dma_ptask *)task)->task;
	unsigned long flags = 0;
	int chan = 0;

	int size = new_task->len;
	int low_src_addr = new_task->src & 0xffffffff;
	int upper_src_addr = (new_task->src >> 32);
	int low_dst_addr = new_task->dest & 0xffffffff;
	int upper_dst_addr = new_task->dest >> 32;
	int low_llp_addr = new_task->llpaddr & 0xffffffff;
	int upper_llp_addr = (new_task->llpaddr >> 32);

	PCIe_PRINT(PCIe_DEBUG, "DMA[%d], size:%d, src:%lx, dst:%lx\n",
		   chan, new_task->len, new_task->src, new_task->dest);
	PCIe_PRINT(PCIe_DEBUG, "size:%d, src low:%x, src high:%x, dst low:%x, dst high:%x\n",
		   size, low_src_addr, upper_src_addr, low_dst_addr,
		   upper_dst_addr);

	spin_lock_irqsave(&s_dma_ops_lock, flags);

	if (new_task->dir == PCI_DMA_WRITE) {
		ax_pcie_dbi_write(PCIE_DMA_BASE + chan * CHAN_ADDR_OFFSET + WR_CHAN_EN, 0x1);	//HDMA WR channel enable
		if (new_task->llpen) {
			ax_pcie_dbi_write(PCIE_DMA_BASE + chan * CHAN_ADDR_OFFSET + WR_CHAN_CONTROL_SET, HDMA_LLP_EN);	//HDMA WR channel LLP enable
			ax_pcie_dbi_write(PCIE_DMA_BASE + chan * CHAN_ADDR_OFFSET + WR_CHAN_LLP_LOW, low_llp_addr); //HDMA WR channel LLP low pointer set
			ax_pcie_dbi_write(PCIE_DMA_BASE + chan * CHAN_ADDR_OFFSET + WR_CHAN_LLP_HIGH, upper_llp_addr); //HDMA WR channel LLP high pointer set
		} else {
			ax_pcie_dbi_write(PCIE_DMA_BASE + chan * CHAN_ADDR_OFFSET + WR_CHAN_CONTROL_SET, 0);	//HDMA WR channel LLP disable
			ax_pcie_dbi_write(PCIE_DMA_BASE + chan * CHAN_ADDR_OFFSET + WR_CHAN_LLP_LOW, 0); //HDMA WR channel LLP low pointer clr
			ax_pcie_dbi_write(PCIE_DMA_BASE + chan * CHAN_ADDR_OFFSET + WR_CHAN_LLP_HIGH, 0); //HDMA WR channel LLP high pointer clr
			ax_pcie_dbi_write(PCIE_DMA_BASE + chan * CHAN_ADDR_OFFSET + WR_CHAN_TRAN_SIZE, size);	//HDMA WR channel transfer size
			ax_pcie_dbi_write(PCIE_DMA_BASE + chan * CHAN_ADDR_OFFSET + WR_SAR_LOW_ADDR, low_src_addr);	//HDMA WR SAR LOW Address
			ax_pcie_dbi_write(PCIE_DMA_BASE + chan * CHAN_ADDR_OFFSET + WR_SAR_UPPER_ADDR, upper_src_addr);	//HDMA WR SAR HIGH Address
			ax_pcie_dbi_write(PCIE_DMA_BASE + chan * CHAN_ADDR_OFFSET + WR_DAR_LOW_ADDR, low_dst_addr);	//HDMA WR DAR LOW Address
			ax_pcie_dbi_write(PCIE_DMA_BASE + chan * CHAN_ADDR_OFFSET + WR_DAR_UPPER_ADDR, upper_dst_addr);	//HDMA WR DAR HIGH Address
		}

		/* start DMA transfer */
		ax_pcie_dbi_write(PCIE_DMA_BASE + chan * CHAN_ADDR_OFFSET + WR_DOORBELL_EN, 0x1);	//HDMA WR doorbell start
	} else if (new_task->dir == PCI_DMA_READ) {
		ax_pcie_dbi_write(PCIE_DMA_BASE + chan * CHAN_ADDR_OFFSET + RD_CHAN_EN, 0x1);	//HDMA RD channel enable
		if (new_task->llpen) {
			ax_pcie_dbi_write(PCIE_DMA_BASE + chan * CHAN_ADDR_OFFSET + RD_CHAN_CONTROL_SET, HDMA_LLP_EN);	//HDMA WR channel LLP enable
			ax_pcie_dbi_write(PCIE_DMA_BASE + chan * CHAN_ADDR_OFFSET + RD_CHAN_LLP_LOW, low_llp_addr); //HDMA WR channel LLP low pointer set
			ax_pcie_dbi_write(PCIE_DMA_BASE + chan * CHAN_ADDR_OFFSET + RD_CHAN_LLP_HIGH, upper_llp_addr); //HDMA WR channel LLP high pointer set
		} else {
			ax_pcie_dbi_write(PCIE_DMA_BASE + chan * CHAN_ADDR_OFFSET + RD_CHAN_CONTROL_SET, 0);	//HDMA WR channel LLP disable
			ax_pcie_dbi_write(PCIE_DMA_BASE + chan * CHAN_ADDR_OFFSET + RD_CHAN_LLP_LOW, 0); //HDMA WR channel LLP low pointer clr
			ax_pcie_dbi_write(PCIE_DMA_BASE + chan * CHAN_ADDR_OFFSET + RD_CHAN_LLP_HIGH, 0); //HDMA WR channel LLP high pointer clr
			ax_pcie_dbi_write(PCIE_DMA_BASE + chan * CHAN_ADDR_OFFSET + RD_CHAN_TRAN_SIZE, size);	//HDMA RD channel transfer size
			ax_pcie_dbi_write(PCIE_DMA_BASE + chan * CHAN_ADDR_OFFSET + RD_SAR_LOW_ADDR, low_src_addr);	//HDMA RD SAR LOW Address
			ax_pcie_dbi_write(PCIE_DMA_BASE + chan * CHAN_ADDR_OFFSET + RD_SAR_UPPER_ADDR, upper_src_addr);	//HDMA RD SAR HIGH Address
			ax_pcie_dbi_write(PCIE_DMA_BASE + chan * CHAN_ADDR_OFFSET + RD_DAR_LOW_ADDR, low_dst_addr);	//HDMA RD DAR LOW Address
			ax_pcie_dbi_write(PCIE_DMA_BASE + chan * CHAN_ADDR_OFFSET + RD_DAR_UPPER_ADDR, upper_dst_addr);	//HDMA RD DAR HIGH Address
		}

		/* start DMA transfer */
		ax_pcie_dbi_write(PCIE_DMA_BASE + chan * CHAN_ADDR_OFFSET + RD_DOORBELL_EN, 0x1);	//HDMA RD doorbell start
	} else {
		PCIe_PRINT(PCIe_ERR, "Wrong dma task data![dir 0x%x]!",
			   new_task->dir);
		PCIe_PRINT(PCIe_ERR, "Start_dma_task failed!");
	}

	spin_unlock_irqrestore(&s_dma_ops_lock, flags);
}

/*
 * return:
 * -1: err;
 *  0: Not DMA read irq;
 *  1: DMA done and clear successfully.
 *  2: DMA abort and clear successfully.
 */
static int clear_dma_read_local_irq(unsigned int controller)
{
	unsigned int read_status;
	int ret = UNKNOW_EXCEPTION;
	int chan = 0;

	ax_pcie_dbi_read(PCIE_DMA_BASE + chan * CHAN_ADDR_OFFSET +
			     RD_CHAN_INT_STATUS, &read_status);
	PCIe_PRINT(PCIe_DEBUG, "PCIe DMA irq status: read[0x%x]", read_status);

	if (0x0 == read_status) {
		PCIe_PRINT(PCIe_DEBUG, "No PCIe DMA read irq triggerred!");
		return NO_TRANSFER;
	}

	ax_pcie_dbi_write(PCIE_DMA_BASE + chan * CHAN_ADDR_OFFSET +
			  RD_CHAN_INT_CLEAR, 0x7);

	if (DMA_ABORT_INTERRUPT_BIT & read_status) {
		PCIe_PRINT(PCIe_ERR, "DMA read abort, value:%x\n", read_status);
		return TRANSFER_ABORT;
	}

	if (DMA_DONE_INTERRUPT_BIT & read_status) {
		PCIe_PRINT(PCIe_DEBUG, "pcie dma read done\n");
		return NORMAL_DONE;
	}

	if (DMA_WATERMARK_INTERRUPT_BIT & read_status) {
		PCIe_PRINT(PCIe_DEBUG, "pcie dma llp watermark\n");
		return LL_WATERMARK;
	}

	return ret;
}

/*
 * return:
 * -1: err;
 *  0: Not DMA write irq;
 *  1: DMA done and clear successfully.
 *  2: DMA abort and clear successfully.
 */
static int clear_dma_write_local_irq(unsigned int controller)
{
	unsigned int write_status;
	int ret = UNKNOW_EXCEPTION;
	int chan = 0;

	ax_pcie_dbi_read(PCIE_DMA_BASE + chan * CHAN_ADDR_OFFSET +
			     WR_CHAN_INT_STATUS, &write_status);
	PCIe_PRINT(PCIe_DEBUG, "PCIe DMA irq status: write[0x%x]",
		   write_status);
	if (0x0 == write_status) {
		PCIe_PRINT(PCIe_DEBUG, "No PCIe DMA write irq triggerred!");
		return NO_TRANSFER;
	}

	ax_pcie_dbi_write(PCIE_DMA_BASE + chan * CHAN_ADDR_OFFSET +
			  WR_CHAN_INT_CLEAR, 0x7);

	if (unlikely(DMA_ABORT_INTERRUPT_BIT & write_status)) {
		PCIe_PRINT(PCIe_ERR, "DMA write abort !!!");
		ret = TRANSFER_ABORT;
	}

	if (unlikely(DMA_DONE_INTERRUPT_BIT & write_status)) {
		PCIe_PRINT(PCIe_DEBUG, "DMA write done !!!");
		ret = NORMAL_DONE;
	}

	if (unlikely(DMA_WATERMARK_INTERRUPT_BIT & write_status)) {
		PCIe_PRINT(PCIe_DEBUG, "DMA write watermask !!!");
		ret = LL_WATERMARK;
	}

	return ret;
}

static int dma_controller_init(void)
{
	return 0;
}

static void dma_controller_exit(void)
{
}

static int request_dma_resource(irqreturn_t(*handler) (int irq, void *id))
{
	int ret = 0;
	int pci_dma_irq;

	clear_dma_read_local_irq(0);
	clear_dma_write_local_irq(0);

	pci_dma_irq = get_pcie_dma_local_irq_number(0);
	PCIe_PRINT(PCIe_DEBUG, "dma irq num:%d\n", pci_dma_irq);

	ret = request_irq(pci_dma_irq, handler, 0,
						"pcie-dma-irq", NULL);
	if (ret) {
		PCIe_PRINT(PCIe_ERR, "request PCIe DMA irq failed!");
	}

	enable_pcie_dma_local_irq(0);

	return ret;
}

static void release_dma_resource(void)
{
	int pci_dma_irq;

	pci_dma_irq = get_pcie_dma_local_irq_number(0);
	free_irq(pci_dma_irq, NULL);
}

static void clear_dma_registers(void)
{

}

void ax_pcie_dma_ops_init(struct ax_pcie_operation *pcie_opt)
{
	spin_lock_init(&s_dma_ops_lock);

	pcie_opt->dma_controller_init = dma_controller_init;
	pcie_opt->dma_controller_exit = dma_controller_exit;
	pcie_opt->request_dma_resource = request_dma_resource;
	pcie_opt->release_dma_resource = release_dma_resource;
	pcie_opt->dma_local_irq_num = get_pcie_dma_local_irq_number;
	pcie_opt->has_dma = is_dma_supported;
	pcie_opt->start_dma_task = __start_dma_task;
	pcie_opt->clear_dma_write_local_irq = clear_dma_write_local_irq;
	pcie_opt->clear_dma_read_local_irq = clear_dma_read_local_irq;
	pcie_opt->enable_dma_local_irq = enable_pcie_dma_local_irq;
	pcie_opt->disable_dma_local_irq = disable_pcie_dma_local_irq;

	s_pcie_opt = pcie_opt;

	clear_dma_registers();
}

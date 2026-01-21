#include <pcie_ax650.h>
#include <linux/string.h>
#include <pci.h>
#include <errno.h>
#include <common.h>
#include <asm/arch/ax650.h>
#include <asm/io.h>
#include <cpu_func.h>

static inline void dw_pcie_write(u32 base, u32 reg, int size, u32 val)
{
    if (size == 4)
        writel(val, (u64)(base + reg));
    else if (size == 2)
        writew(val, (u64)(base + reg));
    else if (size == 1)
        writeb(val, (u64)(base + reg));
}

static inline u32 dw_pcie_read(u32 base, u32 reg, int size)
{
    u32 val;

    if (size == 4) {
        val = readl((u64)(base + reg));
    } else if (size == 2) {
        val = readw((u64)(base + reg));
    } else if (size == 1) {
        val = readb((u64)(base + reg));
    } else {
        val = 0;
        return -1;
    }

    return val;
}

static inline void dw_pcie_write_dbi(u32 reg, int size, u32 val)
{
    dw_pcie_write(PCI_DBI_BASE, reg, size, val);
}

static inline u32 dw_pcie_read_dbi(u32 reg, int size)
{
    return dw_pcie_read(PCI_DBI_BASE, reg, size);
}

static int pcie_dma_read(int chan, u32 low_src_addr, u32 upper_src_addr, u32 low_dst_addr,
                        u32 upper_dst_addr, u32 size)
{
    int val = 0;
    int retry = 100;
    int i;

    dw_pcie_write_dbi(PCIE_DMA_BASE+chan*CHAN_ADDR_BASE+RD_CHAN_EN, 0x4, 0x1); //HDMA RD channel enable
    dw_pcie_write_dbi(PCIE_DMA_BASE+chan*CHAN_ADDR_BASE+RD_CHAN_TRAN_SIZE, 0x4, size); //HDMA RD channel transfer size
    dw_pcie_write_dbi(PCIE_DMA_BASE+chan*CHAN_ADDR_BASE+RD_SAR_LOW_ADDR, 0x4, low_src_addr); //HDMA RD SAR LOW Address
    dw_pcie_write_dbi(PCIE_DMA_BASE+chan*CHAN_ADDR_BASE+RD_SAR_UPPER_ADDR, 0x4, upper_src_addr); //HDMA RD SAR HIGH Address
    dw_pcie_write_dbi(PCIE_DMA_BASE+chan*CHAN_ADDR_BASE+RD_DAR_LOW_ADDR, 0x4, low_dst_addr); //HDMA RD DAR LOW Address
    dw_pcie_write_dbi(PCIE_DMA_BASE+chan*CHAN_ADDR_BASE+RD_DAR_UPPER_ADDR, 0x4, upper_dst_addr); //HDMA RD DAR HIGH Address
    dw_pcie_write_dbi(PCIE_DMA_BASE+chan*CHAN_ADDR_BASE+RD_DOORBELL_EN, 0x4, 0x1); //HDMA RD doorbell start

    for(i=0; i < retry; i++) {
        /* 0x1 runing; 0x2 abort; 0x3 stop */
        val = dw_pcie_read_dbi(PCIE_DMA_BASE+chan*CHAN_ADDR_BASE+RD_CHAN_STATUS, 0x4);//HDMA RD channel status
        if(val != 0x1)
            break;

        udelay(1000);
    }

    return val;
}

static int pcie_read(struct pci_msg_reg *reg, int size)
{
    int chan = 0;
    int dma_read_status = 0;
    u64 start = (((u64)(reg->upper_dst_addr) << 32) + (u64)(reg->low_dst_addr));
    u64 end = start + size;

    invalidate_dcache_range(start, end);

    dma_read_status = pcie_dma_read(chan, reg->low_src_addr, reg->upper_src_addr,
                                    reg->low_dst_addr, reg->upper_dst_addr, size);

    if(dma_read_status != 0x3)
        return PCIE_HDMA_READ_ABORT;

    return dma_read_status;
}

static int pci_ep_read_rc(struct pci_msg_reg *reg)
{
    int ret = 0;

    debug("low_src_addr = %x, upper_src_addr = %x, low_dst_addr = %x, upper_dst_addr = %x, size = %x\n",
        reg->low_src_addr, reg->upper_src_addr, reg->low_dst_addr, reg->upper_dst_addr, reg->size);

    ret = pcie_read(reg, reg->size);
    if(ret < 0)
        return ret;

    return ret;
}
static int pcie_dma_write(int chan, u32 low_src_addr, u32 upper_src_addr, u32 low_dst_addr,
                        u32 upper_dst_addr, u32 size)
{
    int val = 0;
    int retry = 100;
    int i;

    dw_pcie_write_dbi(PCIE_DMA_BASE+chan*CHAN_ADDR_BASE+WR_CHAN_EN, 0x4, 0x1); //HDMA WR channel enable
    dw_pcie_write_dbi(PCIE_DMA_BASE+chan*CHAN_ADDR_BASE+WR_CHAN_TRAN_SIZE, 0x4, size); //HDMA WR channel transfer size
    dw_pcie_write_dbi(PCIE_DMA_BASE+chan*CHAN_ADDR_BASE+WR_SAR_LOW_ADDR, 0x4, low_src_addr); //HDMA WR SAR LOW Address
    dw_pcie_write_dbi(PCIE_DMA_BASE+chan*CHAN_ADDR_BASE+WR_SAR_UPPER_ADDR, 0x4, upper_src_addr); //HDMA WR SAR HIGH Address
    dw_pcie_write_dbi(PCIE_DMA_BASE+chan*CHAN_ADDR_BASE+WR_DAR_LOW_ADDR, 0x4, low_dst_addr); //HDMA WR DAR LOW Address
    dw_pcie_write_dbi(PCIE_DMA_BASE+chan*CHAN_ADDR_BASE+WR_DAR_UPPER_ADDR, 0x4, upper_dst_addr); //HDMA WR DAR HIGH Address
    dw_pcie_write_dbi(PCIE_DMA_BASE+chan*CHAN_ADDR_BASE+WR_DOORBELL_EN, 0x4, 0x1); //HDMA WR doorbell start

    for(i=0; i < retry; i++) {
        /* 0x1 runing; 0x2 abort; 0x3 stop */
        val = dw_pcie_read_dbi(PCIE_DMA_BASE+chan*CHAN_ADDR_BASE+WR_CHAN_STATUS, 0x4);//HDMA WR channel status
        if(val != 0x1)
            break;

        udelay(1000);
    }

    return val;
}

static int pcie_write(struct pci_msg_reg *reg, int size)
{
    int chan = 0;
    int dma_write_status = 0;

    dma_write_status = pcie_dma_write(chan, reg->low_src_addr, reg->upper_src_addr,
                                    reg->low_dst_addr, reg->upper_dst_addr, size);

    if(dma_write_status != 0x3)
        return PCIE_HDMA_READ_ABORT;

    return dma_write_status;
}

static int pci_ep_write_rc(struct pci_msg_reg *reg)
{
    int ret = 0;

    debug("low_src_addr = %x, upper_src_addr = %x, low_dst_addr = %x, upper_dst_addr = %x, size = %x\n",
	reg->low_src_addr, reg->upper_src_addr, reg->low_dst_addr, reg->upper_dst_addr, reg->size);

    ret = pcie_write(reg, reg->size);
    if(ret < 0)
        return ret;

    return ret;
}

/* pcie slave download function */
int polling_remote_command(struct pci_msg_reg *reg)
{
    u32 command;
    int ret = 0;

    while (1) {
        if (reg->finish & BOOT_START_DEVICES) {
            printf("complete!\n");
            printf("pcie boot start\n");
            break;
        }

        command = reg->command;
        if (!command) {
            goto reset_handler;
        }
        reg->command = 0;

        if (command & COMMAND_READ) {
            ret = pci_ep_read_rc(reg);
            if (ret < 0) {
                reg->status |= STATUS_READ_FAIL;
                debug("Image received failed reg->status = %x\n", reg->status);
            } else {
                printf(".");
                reg->status |= STATUS_READ_SUCCESS;
                debug("Image received successfully reg->status = %x\n", reg->status);
            }
        } else if (command & COMMAND_WRITE) {
            ret = pci_ep_write_rc(reg);
            if (ret < 0) {
                reg->status |= STATUS_WRITE_FAIL;
                debug("Image received failed reg->status = %x\n", reg->status);
            } else {
                reg->status |= STATUS_WRITE_SUCCESS;
                printf(".");
                debug("Image received successfully reg->status = %x\n", reg->status);
            }
        }

reset_handler:
        udelay(100);
    }

    return ret;
}


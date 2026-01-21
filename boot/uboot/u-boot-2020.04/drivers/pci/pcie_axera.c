#include <common.h>
#include <dm.h>
#include <pci.h>
#include <generic-phy.h>
#include <power-domain.h>
#include <regmap.h>
#include <syscon.h>
#include <asm/io.h>
#include <asm-generic/gpio.h>
#include <dm/device_compat.h>
#include <linux/err.h>
#include <dm/of_access.h>
#include <dm/ofnode.h>
#include <pcie_ax650.h>
#include <linux/sizes.h>
#include <asm/arch/ax650.h>

DECLARE_GLOBAL_DATA_PTR;

#define PCIE_VENDORID_MASK  GENMASK(15, 0)
#define PCIE_DEVICEID_SHIFT 16

/* PCI DBICS registers */
#define PCIE_CONFIG_BAR0        0x10
#define PCIE_LINK_STATUS_REG        0x80
#define PCIE_LINK_STATUS_SPEED_OFF  16
#define PCIE_LINK_STATUS_SPEED_MASK (0xf << PCIE_LINK_STATUS_SPEED_OFF)
#define PCIE_LINK_STATUS_WIDTH_OFF  20
#define PCIE_LINK_STATUS_WIDTH_MASK (0xf << PCIE_LINK_STATUS_WIDTH_OFF)

#define PCIE_LINK_CAPABILITY        0x7c
#define PCIE_LINK_CTL_2         0xa0
#define TARGET_LINK_SPEED_MASK      0xf
#define LINK_SPEED_GEN_1        0x1
#define LINK_SPEED_GEN_2        0x2
#define LINK_SPEED_GEN_3        0x3

#define PLR_OFFSET          0x700
#define PCIE_PORT_DEBUG0        (PLR_OFFSET + 0x28)
#define PORT_LOGIC_LTSSM_STATE_MASK 0x1f
#define PORT_LOGIC_LTSSM_STATE_L0   0x11

#define PCIE_LINK_UP_TIMEOUT_MS     100

#define PCIE_PORT_LINK_CONTROL      0x710
#define PORT_LINK_MODE_MASK     (0x3f << 16)

#define PORT_LINK_MODE_1_LANES      (0x1 << 16)
#define PORT_LINK_MODE_2_LANES      (0x3 << 16)

#define PORT_LOGIC_LINK_WIDTH_MASK  (0x1f << 8)

#define PORT_LOGIC_LINK_WIDTH_1_LANES   (0x1 << 8)
#define PORT_LOGIC_LINK_WIDTH_2_LANES   (0x2 << 8)

/*
 * iATU Unroll-specific register definitions
 * From 4.80 core version the address translation will be made by unroll.
 * The registers are offset from atu_base
 */
#define PCIE_ATU_UNR_REGION_CTRL1   0x00
#define PCIE_ATU_UNR_REGION_CTRL2   0x04
#define PCIE_ATU_UNR_LOWER_BASE     0x08
#define PCIE_ATU_UNR_LIMIT      0x10
#define PCIE_ATU_UNR_LOWER_TARGET   0x14
#define PCIE_ATU_UNR_UPPER_TARGET   0x18

#define PCIE_ATU_REGION_INDEX1      (0x1 << 0)
#define PCIE_ATU_REGION_INDEX0      (0x0 << 0)
#define PCIE_ATU_TYPE_MEM       (0x0 << 0)
#define PCIE_ATU_TYPE_IO        (0x2 << 0)
#define PCIE_ATU_TYPE_CFG0      (0x4 << 0)
#define PCIE_ATU_TYPE_CFG1      (0x5 << 0)
#define PCIE_ATU_ENABLE         (0x1 << 31)
#define PCIE_ATU_BAR_MODE_ENABLE    (0x1 << 30)
#define PCIE_ATU_BUS(x)         (((x) & 0xff) << 24)
#define PCIE_ATU_DEV(x)         (((x) & 0x1f) << 19)
#define PCIE_ATU_FUNC(x)        (((x) & 0x7) << 16)

// /* Register address builder */
// #define PCIE_GET_ATU_OUTB_UNR_REG_OFFSET(region) ((region) << 9)

/* Offsets from App base */
#define PCIE_CMD_STATUS         0x04
#define LTSSM_EN_VAL            BIT(0)

/* Parameters for the waiting for iATU enabled routine */
#define LINK_WAIT_MAX_IATU_RETRIES  5

#define AM654_PCIE_DEV_TYPE_MASK    0x3
#define EP              0x0
#define LEG_EP              0x1
#define RC              0x4

#define PCIE0_ID    0
#define PCIE1_ID    1

#define PIPE_PCIE0_ARMISC           0xd0    /* bit[21]=1 set enable dbi inferface */
#define PIPE_PCIE0_AWMISC           0xc0    /* bit[21]=1 set enable dbi inferface */
#define PIPE_PCIE1_ARMISC           0x158   /* bit[21]=1 set enable dbi inferface */
#define PIPE_PCIE1_AWMISC           0x148   /* bit[21]=1 set enable dbi inferface */
#define AWRMISC_EN      (0x1 << 21)

#define PIPE_PCIE0_DEVICE_TYPE          0x10c   /* bit[3:0]=0xf is EP mode */
#define PIPE_PCIE1_DEVICE_TYPE          0x194   /* bit[3:0]=0xf is EP mode */

#define PIPE_PCIE0_LTSSM_REG            0x100   /* bit[4] disable: enable:1 */
#define PIPE_PCIE1_LTSSM_REG            0x188   /* bit[4] disable: enable:1 */
#define LTSSM_EN                0x10

#define AXERA_PCIE0_CPU_TO_BUS_ADDR         0x3FFFFFFF
#define AXERA_PCIE1_CPU_TO_BUS_ADDR         0x1FFFFFFF

struct axera_pcie {
    void *pipe_base;
    void *dbi_base;
    void *cfg_base;
    u32 atu_base;
    fdt_size_t cfg_size;
    int first_busno;
    struct udevice *dev;
    u32 pcie_id;
    u32 link_speed;
    u32 link_width;

    /* IO and MEM PCI regions */
    struct pci_region io;
    struct pci_region mem;
};

enum dw_pcie_device_mode {
    DW_PCIE_UNKNOWN_TYPE,
    DW_PCIE_EP_TYPE,
    DW_PCIE_LEG_EP_TYPE,
    DW_PCIE_RC_TYPE,
};

static u64 axera_pcie_cpu_addr_fixup(struct axera_pcie *pcie, u64 cpu_addr)
{
    if (pcie->pcie_id == PCIE0_ID)
        return cpu_addr & AXERA_PCIE0_CPU_TO_BUS_ADDR;
    else
        return cpu_addr & AXERA_PCIE1_CPU_TO_BUS_ADDR;
}

static int dw_pcie_write(void __iomem *addr, int size, u32 val)
{
    if (!IS_ALIGNED((uintptr_t)addr, size))
        return -1;

    if (size == 4)
        writel(val, addr);
    else if (size == 2)
        writew(val, addr);
    else if (size == 1)
        writeb(val, addr);
    else
        return -1;

    return 0;
}

static int dw_pcie_read(void __iomem *addr, int size, u32 *val)
{
    if (!IS_ALIGNED((uintptr_t)addr, size)) {
        *val = 0;
        return -1;
    }

    if (size == 4) {
        *val = readl(addr);
    } else if (size == 2) {
        *val = readw(addr);
    } else if (size == 1) {
        *val = readb(addr);
    } else {
        *val = 0;
        return -1;
    }

    return 0;
}

static inline u32 axera_pcie_readl(struct axera_pcie *pcie, u32 offset)
{
    return readl(pcie->pipe_base + offset);
}

static inline void axera_pcie_writel(struct axera_pcie *pcie, u32 offset,
                                     u32 value)
{
    writel(value, pcie->pipe_base + offset);
}

static void axera_pcie_armisc(struct axera_pcie *pcie, int enable)
{
    u32 val = 0;

    if (pcie->pcie_id == PCIE0_ID) {
        if (enable) {
            /* enable armisc */
            val = axera_pcie_readl(pcie, PIPE_PCIE0_ARMISC);
            val |= AWRMISC_EN;
            axera_pcie_writel(pcie, PIPE_PCIE0_ARMISC, val);
        } else {
            /* disable armisc */
            val = axera_pcie_readl(pcie, PIPE_PCIE0_ARMISC);
            val &= ~AWRMISC_EN;
            axera_pcie_writel(pcie, PIPE_PCIE0_ARMISC, val);
        }
    } else {
        if (enable) {
            /* enable armisc */
            val = axera_pcie_readl(pcie, PIPE_PCIE1_ARMISC);
            val |= AWRMISC_EN;
            axera_pcie_writel(pcie, PIPE_PCIE1_ARMISC, val);
        } else {
            /* disable armisc */
            val = axera_pcie_readl(pcie, PIPE_PCIE1_ARMISC);
            val &= ~AWRMISC_EN;
            axera_pcie_writel(pcie, PIPE_PCIE1_ARMISC, val);
        }
    }
}

static void axera_pcie_awmisc(struct axera_pcie *pcie, int enable)
{
    u32 val = 0;

    if (pcie->pcie_id == PCIE0_ID) {
        if (enable) {
            /* enable awmisc */
            val = axera_pcie_readl(pcie, PIPE_PCIE0_AWMISC);
            val |= AWRMISC_EN;
            axera_pcie_writel(pcie, PIPE_PCIE0_AWMISC, val);
        } else {
            /* disable awmisc */
            val = axera_pcie_readl(pcie, PIPE_PCIE0_AWMISC);
            val &= ~AWRMISC_EN;
            axera_pcie_writel(pcie, PIPE_PCIE0_AWMISC, val);
        }
    } else {
        if (enable) {
            /* enable awmisc */
            val = axera_pcie_readl(pcie, PIPE_PCIE1_AWMISC);
            val |= AWRMISC_EN;
            axera_pcie_writel(pcie, PIPE_PCIE1_AWMISC, val);
        } else {
            /* disable awmisc */
            val = axera_pcie_readl(pcie, PIPE_PCIE1_AWMISC);
            val &= ~AWRMISC_EN;
            axera_pcie_writel(pcie, PIPE_PCIE1_AWMISC, val);
        }
    }
}

static u32 axera_pcie_readl_dbi(struct axera_pcie *pcie, u32 reg)
{
    u32 val;

    axera_pcie_armisc(pcie, true);
    dw_pcie_read(pcie->dbi_base + reg, 0x4, &val);
    axera_pcie_armisc(pcie, false);

    return val;
}

static void axera_pcie_writel_dbi(struct axera_pcie *pcie, u32 reg, u32 val)
{
    axera_pcie_awmisc(pcie, true);
    dw_pcie_write(pcie->dbi_base + reg, 0x4, val);
    axera_pcie_awmisc(pcie, false);
}

static void axera_pcie_writew_dbi(struct axera_pcie *pcie, u32 reg, u32 val)
{
    axera_pcie_awmisc(pcie, true);
    dw_pcie_write(pcie->dbi_base + reg, 0x2, val);
    axera_pcie_awmisc(pcie, false);
}

static int pcie_dw_get_link_speed(struct axera_pcie *pcie)
{
    return (axera_pcie_readl_dbi(pcie, PCIE_LINK_STATUS_REG) &
            PCIE_LINK_STATUS_SPEED_MASK) >> PCIE_LINK_STATUS_SPEED_OFF;
}

static int pcie_dw_get_link_width(struct axera_pcie *pcie)
{
    return (axera_pcie_readl_dbi(pcie, PCIE_LINK_STATUS_REG) &
            PCIE_LINK_STATUS_WIDTH_MASK) >> PCIE_LINK_STATUS_WIDTH_OFF;
}

static void dw_pcie_writel_ob_unroll(struct axera_pcie *pcie, u32 index, u32 reg,
                                     u32 val)
{
    u32 offset = PCIE_GET_ATU_OUTB_UNR_REG_OFFSET(index);

    axera_pcie_writel_dbi(pcie, offset + reg, val);
}

static u32 dw_pcie_readl_ob_unroll(struct axera_pcie *pcie, u32 index, u32 reg)
{
    u32 offset = PCIE_GET_ATU_OUTB_UNR_REG_OFFSET(index);

    return axera_pcie_readl_dbi(pcie, offset + reg);
}

static void pcie_dw_prog_outbound_atu_unroll(struct axera_pcie *pcie, int index,
        int type, u64 cpu_addr,
        u64 pci_addr, u32 size)
{
    u32 retries, val;
    u64 fix_cpu_addr;

    debug("ATU programmed with: index: %d, type: %d, cpu addr: %8llx, pci addr: %8llx, size: %8x\n",
          index, type, cpu_addr, pci_addr, size);

    fix_cpu_addr = axera_pcie_cpu_addr_fixup(pcie, cpu_addr);

    dw_pcie_writel_ob_unroll(pcie, index, PCIE_ATU_UNR_LOWER_BASE,
                             lower_32_bits(fix_cpu_addr));
    dw_pcie_writel_ob_unroll(pcie, index, PCIE_ATU_UNR_UPPER_BASE,
                             upper_32_bits(fix_cpu_addr));
    dw_pcie_writel_ob_unroll(pcie, index, PCIE_ATU_UNR_LIMIT,
                             lower_32_bits(fix_cpu_addr + size - 1));
    dw_pcie_writel_ob_unroll(pcie, index, PCIE_ATU_UNR_LOWER_TARGET,
                             lower_32_bits(pci_addr));
    dw_pcie_writel_ob_unroll(pcie, index, PCIE_ATU_UNR_UPPER_TARGET,
                             upper_32_bits(pci_addr));
    dw_pcie_writel_ob_unroll(pcie, index, PCIE_ATU_UNR_REGION_CTRL1,
                             type);
    dw_pcie_writel_ob_unroll(pcie, index, PCIE_ATU_UNR_REGION_CTRL2,
                             PCIE_ATU_ENABLE);

    /*
     * Make sure ATU enable takes effect before any subsequent config
     * and I/O accesses.
     */
    for (retries = 0; retries < LINK_WAIT_MAX_IATU_RETRIES; retries++) {
        val = dw_pcie_readl_ob_unroll(pcie, index,
                                      PCIE_ATU_UNR_REGION_CTRL2);
        if (val & PCIE_ATU_ENABLE)
            return;

        udelay(LINK_WAIT_IATU);
    }
    dev_err(pci->dev, "outbound iATU is not being enabled\n");
}

static uintptr_t set_cfg_address(struct axera_pcie *pcie,
                                 pci_dev_t d, uint where)
{
    int bus = PCI_BUS(d) - pcie->first_busno;
    uintptr_t va_address;
    u32 atu_type;

    /* Use dbi_base for own configuration read and write */
    if (!bus) {
        va_address = (uintptr_t)pcie->dbi_base;
        return va_address;
    }

    if (bus == 1)
        /* For local bus, change TLP Type field to 4. */
        atu_type = PCIE_ATU_TYPE_CFG0;
    else
        /* Otherwise, change TLP Type field to 5. */
        atu_type = PCIE_ATU_TYPE_CFG1;

    /*
     * Not accessing root port configuration space?
     * Region #0 is used for Outbound CFG space access.
     * Direction = Outbound
     * Region Index = 0
     */
    d = PCI_MASK_BUS(d);
    d = PCI_ADD_BUS(bus, d);
    pcie_dw_prog_outbound_atu_unroll(pcie, PCIE_ATU_REGION_INDEX1,
                                     atu_type, (u64)pcie->cfg_base,
                                     d << 8, pcie->cfg_size);

    va_address = (uintptr_t)pcie->cfg_base;

    va_address += where & ~0x3;

    return va_address;
}

static int pcie_dw_addr_valid(pci_dev_t d, int first_busno)
{
    if ((PCI_BUS(d) == first_busno) && (PCI_DEV(d) > 0))
        return 0;
    if ((PCI_BUS(d) == first_busno + 1) && (PCI_DEV(d) > 0))
        return 0;

    return 1;
}

static int axera_pcie_read_config(const struct udevice *bus, pci_dev_t bdf,
                                  uint offset, ulong *valuep,
                                  enum pci_size_t size)
{
    struct axera_pcie *pcie = dev_get_priv(bus);
    uintptr_t va_address;
    ulong value;

    debug("PCIE CFG read: bdf=%2x:%2x:%2x ",
          PCI_BUS(bdf), PCI_DEV(bdf), PCI_FUNC(bdf));

    if (!pcie_dw_addr_valid(bdf, pcie->first_busno)) {
        debug("- out of range\n");
        *valuep = pci_get_ff(size);
        return 0;
    }

    va_address = set_cfg_address(pcie, bdf, offset);

    if ((PCI_BUS(bdf) == 0) && (PCI_DEV(bdf) == 0) && (PCI_FUNC(bdf) == 0)) {
        value = axera_pcie_readl_dbi(pcie, (offset & ~0x3));
    } else {
        value = readl(va_address);
    }

    debug("(addr,val)=(0x%04x, 0x%08lx)\n", offset, value);
    *valuep = pci_conv_32_to_size(value, offset, size);

    pcie_dw_prog_outbound_atu_unroll(pcie, PCIE_ATU_REGION_INDEX1,
                                     PCIE_ATU_TYPE_IO, pcie->io.phys_start,
                                     pcie->io.bus_start, pcie->io.size);

    return 0;
}

static int axera_pcie_write_config(struct udevice *bus, pci_dev_t bdf,
                                   uint offset, ulong value,
                                   enum pci_size_t size)
{
    struct axera_pcie *pcie = dev_get_priv(bus);
    uintptr_t va_address;
    ulong old;

    debug("PCIE CFG write: (b,d,f)=(%2d,%2d,%2d) ",
          PCI_BUS(bdf), PCI_DEV(bdf), PCI_FUNC(bdf));
    debug("(addr,val)=(0x%04x, 0x%08lx)\n", offset, value);

    if (!pcie_dw_addr_valid(bdf, pcie->first_busno)) {
        debug("- out of range\n");
        return 0;
    }

    va_address = set_cfg_address(pcie, bdf, offset);

    if ((PCI_BUS(bdf) == 0) && (PCI_DEV(bdf) == 0) && (PCI_FUNC(bdf) == 0)) {
        old = axera_pcie_readl_dbi(pcie, (offset & (~0x3)));
        value = pci_conv_size_to_32(old, value, offset, size);
        axera_pcie_writel_dbi(pcie, (offset & (~0x3)), value);
    } else {
        old = readl(va_address);
        value = pci_conv_size_to_32(old, value, offset, size);
        writel(value, va_address);
    }

    pcie_dw_prog_outbound_atu_unroll(pcie, PCIE_ATU_REGION_INDEX1,
                                     PCIE_ATU_TYPE_IO, pcie->io.phys_start,
                                     pcie->io.bus_start, pcie->io.size);

    return 0;
}

static inline void dw_pcie_dbi_write_enable(struct axera_pcie *pcie, bool en)
{
    u32 val;

    val = axera_pcie_readl_dbi(pcie, PCIE_MISC_CONTROL_1_OFF);
    if (en)
        val |= PCIE_DBI_RO_WR_EN;
    else
        val &= ~PCIE_DBI_RO_WR_EN;
    axera_pcie_writel_dbi(pcie, PCIE_MISC_CONTROL_1_OFF, val);
}


static void axera_pcie_set_ltssm_status(struct axera_pcie *pcie, int enable)
{
    u32 val = 0;

    if (pcie->pcie_id == PCIE0_ID) {
        if (enable) {
            /* set ltssm disable */
            val = axera_pcie_readl(pcie, PIPE_PCIE0_LTSSM_REG);
            val |= LTSSM_EN;
            axera_pcie_writel(pcie, PIPE_PCIE0_LTSSM_REG, val);
        } else {
            val = axera_pcie_readl(pcie, PIPE_PCIE0_LTSSM_REG);
            val &= ~LTSSM_EN;
            axera_pcie_writel(pcie, PIPE_PCIE0_LTSSM_REG, val);
        }
    } else {
        if (enable) {
            val = axera_pcie_readl(pcie, PIPE_PCIE1_LTSSM_REG);
            val |= LTSSM_EN;
            axera_pcie_writel(pcie, PIPE_PCIE1_LTSSM_REG, val);
        } else {
            val = axera_pcie_readl(pcie, PIPE_PCIE1_LTSSM_REG);
            val &= ~LTSSM_EN;
            axera_pcie_writel(pcie, PIPE_PCIE1_LTSSM_REG, val);
        }
    }
}

/**
 * pcie_dw_configure() - Configure link capabilities and speed
 *
 * @regs_base: A pointer to the PCIe controller registers
 * @cap_speed: The capabilities and speed to configure
 *
 * Configure the link capabilities and speed in the PCIe root complex.
 */
static void pcie_dw_configure(struct axera_pcie *pcie)
{
    u32 val;
    u32 cap_speed = pcie->link_speed;

    dw_pcie_dbi_write_enable(pcie, true);
    /* Set link speed */
    val = axera_pcie_readl_dbi(pcie, PCIE_LINK_CAPABILITY);
    val &= ~TARGET_LINK_SPEED_MASK;
    val |= cap_speed;
    axera_pcie_writel_dbi(pcie, PCIE_LINK_CAPABILITY, val);

    val = axera_pcie_readl_dbi(pcie, PCIE_LINK_CTL_2);
    val &= ~TARGET_LINK_SPEED_MASK;
    val |= cap_speed;
    axera_pcie_writel_dbi(pcie, PCIE_LINK_CTL_2, val);

    /* Set link width */
    val = axera_pcie_readl_dbi(pcie, PCIE_PORT_LINK_CONTROL);
    val &= ~PORT_LINK_MODE_MASK;
    switch (pcie->link_width) {
    case 1:
        val |= PORT_LINK_MODE_1_LANES;
        break;
    case 2:
        val |= PORT_LINK_MODE_2_LANES;
        break;
    default:
        printf("num-lanes %u: invalid value\n", pcie->link_width);
        return;
    }
    axera_pcie_writel_dbi(pcie, PCIE_PORT_LINK_CONTROL, val);

    val = axera_pcie_readl_dbi(pcie, PCIE_LINK_WIDTH_SPEED_CONTROL);
    val &= ~PORT_LOGIC_LINK_WIDTH_MASK;
    switch (pcie->link_width) {
    case 1:
        val |= PORT_LOGIC_LINK_WIDTH_1_LANES;
        break;
    case 2:
        val |= PORT_LOGIC_LINK_WIDTH_2_LANES;
        break;
    }
    axera_pcie_writel_dbi(pcie, PCIE_LINK_WIDTH_SPEED_CONTROL, val);
}

static int is_link_up(struct axera_pcie *pcie)
{
    u32 val;

    val = axera_pcie_readl_dbi(pcie, PCIE_PORT_DEBUG0);
    val &= PORT_LOGIC_LTSSM_STATE_MASK;

    return (val == PORT_LOGIC_LTSSM_STATE_L0);
}

static int wait_link_up(struct axera_pcie *pci)
{
    unsigned long timeout;

    timeout = get_timer(0) + PCIE_LINK_UP_TIMEOUT_MS;
    while (!is_link_up(pci)) {
        if (get_timer(0) > timeout)
            return 0;
    };

    return 1;
}

static int axera_pcie_link_up(struct axera_pcie *pcie)
{

    if (is_link_up(pcie)) {
        printf("PCI Link already up before configuration!\n");
        return 1;
    }

    /* DW pre link configurations */
    pcie_dw_configure(pcie);

    /* Initiate link training */

    axera_pcie_set_ltssm_status(pcie, true);

    /* Check that link was established */
    if (!wait_link_up(pcie))
        return 0;

    /*
     * Link can be established in Gen 1. still need to wait
     * till MAC nagaotiation is completed
     */
    udelay(100);

    return 1;
}

static void pcie_dw_setup_host(struct axera_pcie *pcie)
{
    u32 val;

    /* setup RC BARs */
    axera_pcie_writel_dbi(pcie, PCI_BASE_ADDRESS_0, 0);
    axera_pcie_writel_dbi(pcie, PCI_BASE_ADDRESS_1, 0);

    /* setup interrupt pins */
    dw_pcie_dbi_write_enable(pcie, true);
    val = axera_pcie_readl_dbi(pcie, PCI_INTERRUPT_LINE);
    val &= 0xffff00ff;
    val |= 0x00000100;
    axera_pcie_writel_dbi(pcie, PCI_INTERRUPT_LINE, val);
    dw_pcie_dbi_write_enable(pcie, false);

    /* setup bus numbers */
    val = axera_pcie_readl_dbi(pcie, PCI_PRIMARY_BUS);
    val &= 0xff000000;
    val |= 0x00ff0100;
    axera_pcie_writel_dbi(pcie, PCI_PRIMARY_BUS, val);

    /* setup command register */
    val = axera_pcie_readl_dbi(pcie, PCI_COMMAND);
    val &= 0xffff0000;
    val |= PCI_COMMAND_IO | PCI_COMMAND_MEMORY |
           PCI_COMMAND_MASTER | PCI_COMMAND_SERR;
    axera_pcie_writel_dbi(pcie, PCI_COMMAND, val);

    /* Enable write permission for the DBI read-only register */
    dw_pcie_dbi_write_enable(pcie, true);
    /* program correct class for RC */
    axera_pcie_writew_dbi(pcie, PCI_CLASS_DEVICE, PCI_CLASS_BRIDGE_PCI);
    /* Better disable write permission right after the update */
    dw_pcie_dbi_write_enable(pcie, false);

    val = axera_pcie_readl_dbi(pcie, PCIE_LINK_WIDTH_SPEED_CONTROL);
    val |= PORT_LOGIC_SPEED_CHANGE;
    axera_pcie_writel_dbi(pcie, PCIE_LINK_WIDTH_SPEED_CONTROL, val);
}

static void axera_set_device_type(struct axera_pcie *pcie, u32 type)
{
    int val;

    if (pcie->pcie_id == PCIE0_ID) {
        val = axera_pcie_readl(pcie, PIPE_PCIE0_DEVICE_TYPE);
        val |= type;
        axera_pcie_writel(pcie, PIPE_PCIE0_DEVICE_TYPE, val);
    } else {
        val = axera_pcie_readl(pcie, PIPE_PCIE1_DEVICE_TYPE);
        val |= type;
        axera_pcie_writel(pcie, PIPE_PCIE1_DEVICE_TYPE, val);
    }
}

static int axera_pcie_set_mode(struct axera_pcie *pcie,
                               enum dw_pcie_device_mode mode)
{
    u32 val;

    switch (mode) {
    case DW_PCIE_RC_TYPE:
        val = RC;
        break;
    case DW_PCIE_EP_TYPE:
        val = EP;
        break;
    default:
        dev_err(pcie->dev, "INVALID device type %d\n", mode);
        return -EINVAL;
    }

    axera_set_device_type(pcie, val);

    return 0;
}

static int axera_add_pcie_port(struct axera_pcie *pcie)
{
    struct udevice *dev = pcie->dev;
    struct udevice *ctlr = pci_get_controller(dev);
    struct pci_controller *hose = dev_get_uclass_priv(ctlr);

    pcie_dw_setup_host(pcie);

    if (!axera_pcie_link_up(pcie)) {
        printf("PCIE%d-RC: Link down\n", pcie->pcie_id);
        return -ENODEV;
    }

    printf("PCIE%d-RC: Link up (Gen%d-x%d, Bus%d)\n", pcie->pcie_id,
           pcie_dw_get_link_speed(pcie),
           pcie_dw_get_link_width(pcie),
           hose->first_busno);

    /* Store the IO and MEM windows settings for future use by the ATU */
    pcie->io.phys_start = hose->regions[0].phys_start; /* IO base */
    pcie->io.bus_start  = hose->regions[0].bus_start;  /* IO_bus_addr */
    pcie->io.size       = hose->regions[0].size;       /* IO size */

    pcie->mem.phys_start = hose->regions[1].phys_start; /* MEM base */
    pcie->mem.bus_start  = hose->regions[1].bus_start;  /* MEM_bus_addr */
    pcie->mem.size       = hose->regions[1].size;       /* MEM size */

    pcie_dw_prog_outbound_atu_unroll(pcie, PCIE_ATU_REGION_INDEX0,
                                     PCIE_ATU_TYPE_MEM,
                                     pcie->mem.phys_start,
                                     pcie->mem.bus_start, pcie->mem.size);

    return 0;
}

static void axera_pcie_clk_set(struct axera_pcie *pcie)
{
    writel(0x3 << PIPE_SYS_CLK_CLK_PIPE_BUS_SEL_LSB, PIPE_SYS_CLK_MUX_0_SET);
}

static int pcie_model_sel_read(u32 pcie_id)
{
    u32 val;

    if (pcie_id == PCIE0_ID) {
        val = readl(PIPE_PHY_GLB_PCS_CTL0_0_ADDR);
    } else {
        val = readl(PIPE_PHY_GLB_PCS_CTL1_0_ADDR);
    }
    val &= PCIE_PIPE_PHY_MODEL_SEL_MASK;
    return val;
}

static int axera_pcie_probe(struct udevice *dev)
{
    struct axera_pcie *pcie = dev_get_priv(dev);
    int model_sel;

    pcie->first_busno = dev->seq;
    pcie->dev = dev;

    model_sel = pcie_model_sel_read(pcie->pcie_id);
    if (!model_sel) {
        dev_err(dev, "model_sel = %d, pcie %d not supported\n", model_sel, pcie->pcie_id);
        return -EPERM;
    }

    axera_pcie_clk_set(pcie);

    if (device_is_compatible(dev, "axera,axera-pcie"))
        axera_pcie_set_mode(pcie, DW_PCIE_RC_TYPE);

    axera_add_pcie_port(pcie);

    return 0;
}

static int axera_pcie_ofdata_to_platdata(struct udevice *dev)
{
    struct axera_pcie *pcie = dev_get_priv(dev);
    u32 pcie_id;
    int ret;
    u32 link_speed;
    u32 link_width;

    ret = dev_read_u32(dev, "ctl-id", &pcie_id);
    if (ret)
        return -EINVAL;

    if (pcie_id != PCIE0_ID && pcie_id != PCIE1_ID) {
        printf("Invalid pcie id %d\n", pcie_id);
        return -ENOMEM;
    }

    ret = dev_read_u32(dev, "max-link-speed", &link_speed);
    if (ret)
        return -EINVAL;

    ret = dev_read_u32(dev, "num-lanes", &link_width);
    if (ret)
        return -EINVAL;

    pcie->pcie_id = pcie_id;
    pcie->link_speed = link_speed;
    pcie->link_width = link_width;

    /* Get the controller base address */
    pcie->dbi_base = (void *)dev_read_addr_name(dev, "dbi");
    if ((fdt_addr_t)pcie->dbi_base == FDT_ADDR_T_NONE)
        return -EINVAL;

    /* Get the config space base address and size */
    pcie->cfg_base = (void *)dev_read_addr_size_name(dev, "config",
                     &pcie->cfg_size);
    if ((fdt_addr_t)pcie->cfg_base == FDT_ADDR_T_NONE)
        return -EINVAL;

    /* Get the app base address and size */
    pcie->pipe_base = (void *)dev_read_addr_name(dev, "pipe_sys");
    if ((fdt_addr_t)pcie->pipe_base == FDT_ADDR_T_NONE)
        return -EINVAL;

    return 0;
}

static const struct dm_pci_ops axera_pcie_ops = {
    .read_config    = axera_pcie_read_config,
    .write_config   = axera_pcie_write_config,
};

static const struct udevice_id axera_pcie_ids[] = {
    { .compatible = "axera,axera-pcie" },
    { }
};

U_BOOT_DRIVER(axera_pcie) = {
    .name           = "axera_pcie",
    .id         = UCLASS_PCI,
    .of_match       = axera_pcie_ids,
    .ops            = &axera_pcie_ops,
    .ofdata_to_platdata = axera_pcie_ofdata_to_platdata,
    .probe          = axera_pcie_probe,
    .priv_auto_alloc_size   = sizeof(struct axera_pcie),
};
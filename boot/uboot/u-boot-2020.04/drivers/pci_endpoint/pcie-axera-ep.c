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

struct axera_pcie_ep {
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
};

enum dw_pcie_device_mode {
    DW_PCIE_UNKNOWN_TYPE,
    DW_PCIE_EP_TYPE,
    DW_PCIE_LEG_EP_TYPE,
    DW_PCIE_RC_TYPE,
};

static int bar_size[] = {BAR0_SIZE, BAR1_SIZE, 0, 0, BAR4_SIZE, 0};

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

static inline u32 axera_pcie_readl(struct axera_pcie_ep *pcie, u32 offset)
{
    return readl(pcie->pipe_base + offset);
}

static inline void axera_pcie_writel(struct axera_pcie_ep *pcie, u32 offset,
                                     u32 value)
{
    writel(value, pcie->pipe_base + offset);
}

static void axera_pcie_armisc(struct axera_pcie_ep *pcie, int enable)
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

static void axera_pcie_awmisc(struct axera_pcie_ep *pcie, int enable)
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

static u32 axera_pcie_readl_dbi(struct axera_pcie_ep *pcie, u32 reg)
{
    u32 val;

    axera_pcie_armisc(pcie, true);
    dw_pcie_read(pcie->dbi_base + reg, 0x4, &val);
    axera_pcie_armisc(pcie, false);

    return val;
}

static u32 axera_pcie_readw_dbi(struct axera_pcie_ep *pcie, u32 reg)
{
    u32 val;

    axera_pcie_armisc(pcie, true);
    dw_pcie_read(pcie->dbi_base + reg, 0x2, &val);
    axera_pcie_armisc(pcie, false);

    return val;
}

static void axera_pcie_writel_dbi(struct axera_pcie_ep *pcie, u32 reg, u32 val)
{
    axera_pcie_awmisc(pcie, true);
    dw_pcie_write(pcie->dbi_base + reg, 0x4, val);
    axera_pcie_awmisc(pcie, false);
}

static void axera_pcie_writew_dbi(struct axera_pcie_ep *pcie, u32 reg, u32 val)
{
    axera_pcie_awmisc(pcie, true);
    dw_pcie_write(pcie->dbi_base + reg, 0x2, val);
    axera_pcie_awmisc(pcie, false);
}

static void axera_pcie_writeb_dbi(struct axera_pcie_ep *pcie, u32 reg, u32 val)
{
    axera_pcie_awmisc(pcie, true);
    dw_pcie_write(pcie->dbi_base + reg, 0x1, val);
    axera_pcie_awmisc(pcie, false);
}

static void axera_pcie_writel_dbi2(struct axera_pcie_ep *pcie, u32 reg, u32 val)
{
    axera_pcie_awmisc(pcie, true);
    dw_pcie_write(pcie->dbi_base + SZ_1M + reg, 0x4, val);
    axera_pcie_awmisc(pcie, false);
}

static int pcie_dw_get_link_speed(struct axera_pcie_ep *pcie)
{
    return (axera_pcie_readl_dbi(pcie, PCIE_LINK_STATUS_REG) &
            PCIE_LINK_STATUS_SPEED_MASK) >> PCIE_LINK_STATUS_SPEED_OFF;
}

static int pcie_dw_get_link_width(struct axera_pcie_ep *pcie)
{
    return (axera_pcie_readl_dbi(pcie, PCIE_LINK_STATUS_REG) &
            PCIE_LINK_STATUS_WIDTH_MASK) >> PCIE_LINK_STATUS_WIDTH_OFF;
}

static void dw_pcie_writel_ib_unroll(struct axera_pcie_ep *pcie, u32 index, u32 reg,
                                     u32 val)
{
    u32 offset = PCIE_GET_ATU_INB_UNR_REG_OFFSET(index);

    axera_pcie_writel_dbi(pcie, offset + reg, val);
}


static void dw_pcie_prog_inbound_atu_unroll(struct axera_pcie_ep *pcie, int index, int bar, u64 cpu_addr, int type)
{
    dw_pcie_writel_ib_unroll(pcie, index, PCIE_ATU_UNR_LOWER_TARGET,
                             lower_32_bits(cpu_addr));
    dw_pcie_writel_ib_unroll(pcie, index, PCIE_ATU_UNR_UPPER_TARGET,
                             upper_32_bits(cpu_addr));

    dw_pcie_writel_ib_unroll(pcie, index, PCIE_ATU_UNR_REGION_CTRL1, type);
    dw_pcie_writel_ib_unroll(pcie, index, PCIE_ATU_UNR_REGION_CTRL2,
                             PCIE_ATU_ENABLE | PCIE_ATU_BAR_MODE_ENABLE | (bar << 8));
}

static inline void dw_pcie_dbi_write_enable(struct axera_pcie_ep *pcie, bool en)
{
    u32 val;

    val = axera_pcie_readl_dbi(pcie, PCIE_MISC_CONTROL_1_OFF);
    if (en)
        val |= PCIE_DBI_RO_WR_EN;
    else
        val &= ~PCIE_DBI_RO_WR_EN;
    axera_pcie_writel_dbi(pcie, PCIE_MISC_CONTROL_1_OFF, val);
}

static void axera_pcie_set_ltssm_status(struct axera_pcie_ep *pcie, int enable)
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

static void pcie_dw_configure(struct axera_pcie_ep *pcie)
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

static int is_link_up(struct axera_pcie_ep *pcie)
{
    u32 val;

    val = axera_pcie_readl_dbi(pcie, PCIE_PORT_DEBUG0);
    val &= PORT_LOGIC_LTSSM_STATE_MASK;

    return (val == PORT_LOGIC_LTSSM_STATE_L0);
}

static int wait_link_up(struct axera_pcie_ep *pci)
{
    unsigned long timeout;

    timeout = get_timer(0) + PCIE_LINK_UP_TIMEOUT_MS;
    while (!is_link_up(pci)) {
        if (get_timer(0) > timeout)
            return 0;
    };

    return 1;
}

static int axera_pcie_link_up(struct axera_pcie_ep *pcie)
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

static void axera_set_device_type(struct axera_pcie_ep *pcie, u32 type)
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

static int axera_pcie_set_mode(struct axera_pcie_ep *pcie,
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

static unsigned int dw_pcie_ep_find_ext_capability(struct axera_pcie_ep *pcie, int cap)
{
    u32 header;
    int pos = PCI_CFG_SPACE_SIZE;

    while (pos) {
        header = axera_pcie_readl_dbi(pcie, pos);
        if (PCI_EXT_CAP_ID(header) == cap)
            return pos;

        pos = PCI_EXT_CAP_NEXT(header);
        if (!pos)
            break;
    }

    return 0;
}

static void dw_pcie_ep_reset_bar(struct axera_pcie_ep *pcie, int flags)
{
    int bar;
    int reg;
    for (bar = BAR_0; bar <= BAR_5; bar++) {
        reg = PCI_BASE_ADDRESS_0 + (4 * bar);
        axera_pcie_writel_dbi2(pcie, reg, 0x0);
        axera_pcie_writel_dbi(pcie, reg, 0x0);
        if (flags & PCI_BASE_ADDRESS_MEM_TYPE_64) {
            axera_pcie_writel_dbi2(pcie, reg + 4, 0x0);
            axera_pcie_writel_dbi(pcie, reg + 4, 0x0);
        }
    }
}

static void dw_pcie_ep_set_bar(struct axera_pcie_ep *pcie)
{
    u32 reg;
    int index = INBOUND_INDEX0;
    int offset, i;
    int bar, nbars;
    int flags = PCI_BASE_ADDRESS_MEM_TYPE_32;
    u64 cpu_addr = PCIE_MSG_BASE;

    dw_pcie_ep_reset_bar(pcie, 0);

    for (bar = BAR_0; bar <= BAR_5; bar++) {
        switch (bar) {
        case BAR_0:
            index = INBOUND_INDEX0;
            flags = PCI_BASE_ADDRESS_MEM_TYPE_32;
            cpu_addr = PCIE_MSG_BASE;
            break;
        case BAR_1:
            index = INBOUND_INDEX1;
            flags = PCI_BASE_ADDRESS_MEM_TYPE_32;
            cpu_addr = PCIE_HWID_BASE_ADDR;
            break;
        case BAR_2:
        case BAR_3:
            continue;
        case BAR_4:
            index = INBOUND_INDEX2;
            flags = PCI_BASE_ADDRESS_MEM_TYPE_64;
            cpu_addr = PCIE_SYS_RESET_BASE_1M;
            break;
        case BAR_5:
            break;
        default:
            goto rebar;
        }

        reg = PCI_BASE_ADDRESS_0 + (4 * bar);

        dw_pcie_prog_inbound_atu_unroll(pcie, index, bar,
                                        cpu_addr, PCIE_ATU_TYPE_MEM);

        axera_pcie_writel_dbi2(pcie, reg, lower_32_bits(bar_size[bar] - 1));
        axera_pcie_writel_dbi(pcie, reg, flags);

        if (flags & PCI_BASE_ADDRESS_MEM_TYPE_64) {
            axera_pcie_writel_dbi2(pcie, reg + 4, upper_32_bits(bar_size[bar] - 1));
            axera_pcie_writel_dbi(pcie, reg + 4, 0);
            bar++;
        }
    }

rebar:
    offset = dw_pcie_ep_find_ext_capability(pcie, PCI_EXT_CAP_ID_REBAR);
    if (offset) {
        reg = axera_pcie_readl_dbi(pcie, offset + PCI_REBAR_CTRL);
        nbars = (reg & PCI_REBAR_CTRL_NBAR_MASK) >>
                PCI_REBAR_CTRL_NBAR_SHIFT;

        for (i = 0; i < nbars; i++, offset += PCI_REBAR_CTRL) {
            axera_pcie_writel_dbi(pcie, offset + PCI_REBAR_CAP, 0x10);
            reg = axera_pcie_readl_dbi(pcie, offset + PCI_REBAR_CAP + 0x4);
            reg &= (~0x3f00);
            axera_pcie_writel_dbi(pcie, offset + PCI_REBAR_CAP + 0x4, reg);
        }
    }
}

static void pcie_reset_bar_map(struct axera_pcie_ep *pcie)
{
    u32 l;
    u32 sz;
    u64 cpu_addr;
    int index = INBOUND_INDEX2;

    l = axera_pcie_readl_dbi(pcie, PCI_BASE_ADDRESS_4);
    axera_pcie_writel_dbi(pcie, PCI_BASE_ADDRESS_4, (l | (~0)));
    sz = axera_pcie_readl_dbi(pcie, PCI_BASE_ADDRESS_4);
    axera_pcie_writel_dbi(pcie, PCI_BASE_ADDRESS_4, l);
    sz = ~(sz & (~0xf));

    if (sz == 0xfffff) {
        cpu_addr = PCIE_SYS_RESET_BASE_1M;
    } else {
        cpu_addr = PCIE_SYS_RESET_BASE;
    }

    dw_pcie_prog_inbound_atu_unroll(pcie, index, BAR_4,
                                    cpu_addr, PCIE_ATU_TYPE_MEM);
}

static void dw_pcie_ep_set_msi(struct axera_pcie_ep *pcie)
{
    u32 val, reg;
    int interrupts = PCI_MSI_INT_NUM;

    reg = PCI_MSI_CAP + PCI_MSI_FLAGS;
    val = axera_pcie_readw_dbi(pcie, reg);
    val &= ~PCI_MSI_FLAGS_QMASK;
    val |= (interrupts << 1) & PCI_MSI_FLAGS_QMASK;
    axera_pcie_writew_dbi(pcie, reg, val);
}

static void pcie_dw_setup_ep(struct axera_pcie_ep *pcie)
{
    int val;

    dw_pcie_dbi_write_enable(pcie, true);
    axera_pcie_writew_dbi(pcie, PCI_VENDOR_ID, AXERA_PCIE_VENDORID);
    axera_pcie_writew_dbi(pcie, PCI_DEVICE_ID, AXERA_PCIE_DEVICEID);
    axera_pcie_writew_dbi(pcie, PCI_SUBSYSTEM_VENDOR_ID, AXERA_PCIE_VENDORID);
    axera_pcie_writew_dbi(pcie, PCI_SUBSYSTEM_ID, AXERA_PCIE_DEVICEID);

    axera_pcie_writew_dbi(pcie, PCI_CLASS_DEVICE,
                          AXERA_PCIE_SUBCLASSCODE | AXERA_PCIE_BASECLASSCODE << 8);
    axera_pcie_writeb_dbi(pcie, PCI_CACHE_LINE_SIZE, CACHE_LINE_SIZE);
    axera_pcie_writeb_dbi(pcie, PCI_INTERRUPT_PIN, INTERRUPT_PIN);

    dw_pcie_ep_set_bar(pcie);
    pcie_reset_bar_map(pcie);
    dw_pcie_ep_set_msi(pcie);

    /*disable pcie aspm support*/
    val = axera_pcie_readl_dbi(pcie, PCIE_LINK_ASPM_CAP);
    val &= ~(0x3 << 10);
    axera_pcie_writel_dbi(pcie, PCIE_LINK_ASPM_CAP, val);

    val = axera_pcie_readl_dbi(pcie, PCIE_LINK_WIDTH_SPEED_CONTROL);
    val &= ~PORT_LOGIC_SPEED_CHANGE;
    axera_pcie_writel_dbi(pcie, PCIE_LINK_WIDTH_SPEED_CONTROL, val);

    dw_pcie_dbi_write_enable(pcie, false);
}

static int axera_add_pcie_ep(struct axera_pcie_ep *pcie)
{
    pcie_dw_setup_ep(pcie);

    if (!axera_pcie_link_up(pcie)) {
        printf("PCIE%d-EP: Link down\n", pcie->pcie_id);
        return 0;
    }

    printf("PCIE%d-EP: Link up (Gen%d-x%d)\n", pcie->pcie_id,
           pcie_dw_get_link_speed(pcie),
           pcie_dw_get_link_width(pcie));

    return 0;
}

static void axera_pcie_clk_set(struct axera_pcie_ep *pcie)
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
    struct axera_pcie_ep *pcie = dev_get_priv(dev);
    int model_sel;

    pcie->dev = dev;

    model_sel = pcie_model_sel_read(pcie->pcie_id);
    if (!model_sel) {
        dev_err(dev, "model_sel = %d, pcie %d not supported\n", model_sel, pcie->pcie_id);
        return -EPERM;
    }

    axera_pcie_clk_set(pcie);

    if (device_is_compatible(dev, "axera,axera-pcie-ep"))
        axera_pcie_set_mode(pcie, DW_PCIE_EP_TYPE);

    axera_add_pcie_ep(pcie);

    return 0;
}

static int axera_pcie_ofdata_to_platdata(struct udevice *dev)
{
    struct axera_pcie_ep *pcie = dev_get_priv(dev);
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
    pcie->cfg_base = (void *)dev_read_addr_size_name(dev, "addr_space",
                     &pcie->cfg_size);
    if ((fdt_addr_t)pcie->cfg_base == FDT_ADDR_T_NONE)
        return -EINVAL;

    /* Get the app base address and size */
    pcie->pipe_base = (void *)dev_read_addr_name(dev, "pipe_sys");
    if ((fdt_addr_t)pcie->pipe_base == FDT_ADDR_T_NONE)
        return -EINVAL;

    if (pcie_id == PCIE0_ID) {
        /* Set clkreq */
        gpio_request(PCIE0_CLKREQ_GPIO, "pcie0-clkreq");
        gpio_direction_output(PCIE0_CLKREQ_GPIO, 1);
        gpio_set_value(PCIE0_CLKREQ_GPIO, 0);
    } else {
        gpio_request(PCIE1_CLKREQ_GPIO, "pcie1-clkreq");
        gpio_direction_output(PCIE1_CLKREQ_GPIO, 1);
        gpio_set_value(PCIE1_CLKREQ_GPIO, 0);
    }

    return 0;
}

static const struct udevice_id axera_pcie_ids[] = {
    { .compatible = "axera,axera-pcie-ep" },
    { }
};

U_BOOT_DRIVER(axera_pcie_ep) = {
    .name           = "axera_pcie_ep",
    .id         = UCLASS_PCI_EP,
    .of_match       = axera_pcie_ids,
    .ofdata_to_platdata = axera_pcie_ofdata_to_platdata,
    .probe          = axera_pcie_probe,
    .priv_auto_alloc_size   = sizeof(struct axera_pcie_ep),
};
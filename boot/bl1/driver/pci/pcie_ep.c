#include "cmn.h"
#include "pci.h"
#include "trace.h"
#include "timer.h"

static int bar_size[] = {BAR0_SIZE, BAR1_SIZE, 0, 0, BAR4_SIZE, 0};
static u32 sub_vendor_id = AXERA_PCIE_VENDORID;
static u32 sub_device_id = AXERA_PCIE_DEVICEID;

static inline void dw_pcie_write(u32 base, u32 reg, int size, u32 val)
{
    if (size == 4)
        writel(val, base + reg);
    else if (size == 2)
        writew(val, base + reg);
    else if (size == 1)
        writeb(val, base + reg);
}

static inline u32 dw_pcie_read(u32 base, u32 reg, int size)
{
    u32 val;

    if (size == 4) {
        val = readl(base + reg);
    } else if (size == 2) {
        val = readw(base + reg);
    } else if (size == 1) {
        val = readb(base + reg);
    } else {
        val = 0;
        return -1;
    }

    return val;
}

static void core_write_word(u32 reg, u32 val)
{
	writel(val, reg);
}

static inline void dw_pcie_write_dbi(u32 reg, int size, u32 val)
{
    dw_pcie_write(PCI_DBI_BASE, reg, size, val);
}

static inline u32 dw_pcie_read_dbi(u32 reg, int size)
{
    return dw_pcie_read(PCI_DBI_BASE, reg, size);
}

static inline void dw_pcie_write_dbi2(u32 reg, int size, u32 val)
{
    dw_pcie_write(PCI_DBI2_BASE, reg, size, val);
}

static inline u32 dw_pcie_read_dbi2(u32 reg, int size)
{
    return dw_pcie_read(PCI_DBI2_BASE, reg, size);
}

static void dw_pcie_writel_ib_unroll(u32 index, u32 reg,
				     u32 val)
{
    u32 offset = PCIE_GET_ATU_INB_UNR_REG_OFFSET(index);

    dw_pcie_write_dbi(offset + reg, 0x4, val);
}

void dw_pcie_dbi_ro_wr_en()
{
    u32 reg;
    u32 val;

    reg = PCIE_MISC_CONTROL_1_OFF;
    val = dw_pcie_read_dbi(reg, 0x4);
    val |= PCIE_DBI_RO_WR_EN;
    dw_pcie_write_dbi(reg, 0x4, val);
}

void dw_pcie_dbi_ro_wr_dis()
{
    u32 reg;
    u32 val;

    reg = PCIE_MISC_CONTROL_1_OFF;
    val = dw_pcie_read_dbi(reg, 0x4);
    val &= ~PCIE_DBI_RO_WR_EN;
    dw_pcie_write_dbi(reg, 0x4, val);
}

void pcie_device_type_set(void)
{
    /* set device type -ep mode bit[3:0]=0 */
    core_write_word(PIPE_SYS_GLB_PCIE_CTL0_19_CLR_ADDR, 1 << PIPE_SYS_GLB_DEVICE_TYPE_CLR_LSB);
}

void dw_pcie0_awrmisc_enable(u32 enable)
{
    if (enable) {
        core_write_word(PIPE_SYS_GLB_PCIE_CTL0_0_SET, 1<<PIPE_SYS_GLB_SLAVE_MISC_INFO_LSB); //awmisc
        core_write_word(PIPE_SYS_GLB_PCIE_CTL0_4_SET, 1<<PIPE_SYS_GLB_SLAVE_MISC_INFO_LSB); //armisc
    } else {
        core_write_word(PIPE_SYS_GLB_PCIE_CTL0_0_CLR, 1<<PIPE_SYS_GLB_SLAVE_MISC_INFO_LSB);
        core_write_word(PIPE_SYS_GLB_PCIE_CTL0_4_CLR, 1<<PIPE_SYS_GLB_SLAVE_MISC_INFO_LSB);
    }
}

void dw_pcie0_ltssm_enable(u32 enable)
{
    if (enable) {
        core_write_word(PIPE_SYS_GLB_PCIE_CTL0_16_SET, 1<<PIPE_SYS_GLB_APP_LTSSM_ENABLE_SET_LSB);
    } else {
        core_write_word(PIPE_SYS_GLB_PCIE_CTL0_16_CLR, 1<<PIPE_SYS_GLB_APP_LTSSM_ENABLE_SET_LSB);
    }
}

void pice_sata_mode_init()
{
    core_write_word(PIPE_PHY_GLB_PCS_CTL0_0_SET_ADDR, PIPE_PCS0_MODEL_SEL);
	core_write_word(PIPE_PHY_GLB_PCS_CTL1_0_SET_ADDR, PIPE_PCS1_MODEL_SEL);

	core_write_word(PIPE_PHY_CK_RST_CFG_CLK_EB_0_CLR_ADDR, 1<<PIPE_PHY_CK_RST_CFG_CLK_EB_0_PIPE_REF_ALT_CLK_EB_LSB);
	core_write_word(PIPE_PHY_GLB_PCS_CTL0_2_SET_ADDR, 1<<PIPE_PHY_GLB_PCS_CTL0_2_PHY0_CSR_REF_USE_PAD_LSB);
	core_write_word(PIPE_PHY_GLB_PCS_CTL0_3_SET_ADDR, 1<<PIPE_PHY_GLB_PCS_CTL0_3_PHY1_CSR_REF_USE_PAD_LSB);
	core_write_word(PIPE_PHY_GLB_PCS_CTL1_2_SET_ADDR, 1<<PIPE_PHY_GLB_PCS_CTL1_2_PHY0_CSR_REF_USE_PAD_LSB);
	core_write_word(PIPE_PHY_GLB_PCS_CTL1_3_SET_ADDR, 1<<PIPE_PHY_GLB_PCS_CTL1_3_PHY1_CSR_REF_USE_PAD_LSB);
}

static void release_pipe_reset(void)
{
	core_write_word(PIPE_PHY_GLB_PCS_CTL0_2_SET_ADDR, 1<<PIPE_PHY_GLB_PCS_CTL0_2_PHY0_CSR_REF_SSP_EN_LSB); //ref ssp en
	core_write_word(PIPE_PHY_GLB_PCS_CTL0_3_SET_ADDR, 1<<PIPE_PHY_GLB_PCS_CTL0_3_PHY1_CSR_REF_SSP_EN_LSB); //ref ssp en
	core_write_word(PIPE_PHY_GLB_PCS_CTL1_2_SET_ADDR, 1<<PIPE_PHY_GLB_PCS_CTL1_2_PHY0_CSR_REF_SSP_EN_LSB); //ref ssp en
	core_write_word(PIPE_PHY_GLB_PCS_CTL1_3_SET_ADDR, 1<<PIPE_PHY_GLB_PCS_CTL1_3_PHY1_CSR_REF_SSP_EN_LSB); //ref ssp en
	core_write_word(PIPE_PHY_GLB_PCS_CTL0_2_CLR_ADDR, 1<<PIPE_PHY_GLB_PCS_CTL0_2_PHY0_CSR_TEST_POWERDOWN_LSB);
	core_write_word(PIPE_PHY_GLB_PCS_CTL0_3_CLR_ADDR, 1<<PIPE_PHY_GLB_PCS_CTL0_3_PHY1_CSR_TEST_POWERDOWN_LSB);
	core_write_word(PIPE_PHY_GLB_PCS_CTL1_2_CLR_ADDR, 1<<PIPE_PHY_GLB_PCS_CTL1_2_PHY0_CSR_TEST_POWERDOWN_LSB);
	core_write_word(PIPE_PHY_GLB_PCS_CTL1_3_CLR_ADDR, 1<<PIPE_PHY_GLB_PCS_CTL1_3_PHY1_CSR_TEST_POWERDOWN_LSB);
	core_write_word(PIPE_PHY_CK_RST_CFG_SW_RST_0_CLR_ADDR,
			1<<PIPE_PHY_CK_RST_CFG_SW_RST_0_PCIE0_PHY_SW_PRST_LSB | 1<<PIPE_PHY_CK_RST_CFG_SW_RST_0_PCIE1_PHY_SW_PRST_LSB |
			1<<PIPE_PHY_CK_RST_CFG_SW_RST_0_PCIE2_PHY_SW_PRST_LSB | 1<<PIPE_PHY_CK_RST_CFG_SW_RST_0_PCIE3_PHY_SW_PRST_LSB);
	core_write_word(PIPE_SYS_SW_RESET_SET, 1<<PIPE_SYS_CLK_SW_RESET_SATA_SW_RST |
			1<<PIPE_SYS_CLK_SW_RESET_PCIE0_SW_PERST | 1<<PIPE_SYS_CLK_SW_RESET_PCIE0_POWER_UP_SW_RST |
			1<<PIPE_SYS_CLK_SW_RESET_PCIE1_SW_PERST | 1<<PIPE_SYS_CLK_SW_RESET_PCIE1_POWER_UP_SW_RST |
			1<<PIPE_SYS_CLK_SW_RESET_PCIE2_SW_PERST | 1<<PIPE_SYS_CLK_SW_RESET_PCIE2_POWER_UP_SW_RST |
			1<<PIPE_SYS_CLK_SW_RESET_PCIE3_SW_PERST | 1<<PIPE_SYS_CLK_SW_RESET_PCIE3_POWER_UP_SW_RST);
	core_write_word(PIPE_SYS_SW_RESET_CLR, 1<<PIPE_SYS_CLK_SW_RESET_SATA_SW_RST |
			1<<PIPE_SYS_CLK_SW_RESET_PCIE0_SW_PERST | 1<<PIPE_SYS_CLK_SW_RESET_PCIE0_POWER_UP_SW_RST |
			1<<PIPE_SYS_CLK_SW_RESET_PCIE1_SW_PERST | 1<<PIPE_SYS_CLK_SW_RESET_PCIE1_POWER_UP_SW_RST |
			1<<PIPE_SYS_CLK_SW_RESET_PCIE2_SW_PERST | 1<<PIPE_SYS_CLK_SW_RESET_PCIE2_POWER_UP_SW_RST |
			1<<PIPE_SYS_CLK_SW_RESET_PCIE3_SW_PERST | 1<<PIPE_SYS_CLK_SW_RESET_PCIE3_POWER_UP_SW_RST);
}

static void release_pipe_phy0_reset(void)
{
	//must release test powerdown first, and then release phy reset
	core_write_word(PIPE_PHY_GLB_PCS_CTL0_2_CLR_ADDR, 1<<PIPE_PHY_GLB_PCS_CTL0_2_CLR_PHY0_CSR_PHY_RESET_CLR_LSB);
	core_write_word(PIPE_PHY_GLB_PCS_CTL0_3_CLR_ADDR, 1<<PIPE_PHY_GLB_PCS_CTL0_3_CLR_PHY1_CSR_PHY_RESET_CLR_LSB);
	core_write_word(PIPE_PHY_CK_RST_CFG_SW_RST_0_CLR_ADDR,
		1<<PIPE_PHY_CK_RST_CFG_SW_RST_0_PCIE0_PHY_SW_PRST_LSB | 1<<PIPE_PHY_CK_RST_CFG_SW_RST_0_PCIE1_PHY_SW_PRST_LSB);
}

static void release_pipe_phy1_reset(void)
{
	//must release test powerdown first, and then release phy reset
	core_write_word(PIPE_PHY_GLB_PCS_CTL1_2_CLR_ADDR, 1<<PIPE_PHY_GLB_PCS_CTL1_2_CLR_PHY0_CSR_PHY_RESET_CLR_LSB);
	core_write_word(PIPE_PHY_GLB_PCS_CTL1_3_CLR_ADDR, 1<<PIPE_PHY_GLB_PCS_CTL1_3_CLR_PHY1_CSR_PHY_RESET_CLR_LSB);
	core_write_word(PIPE_PHY_CK_RST_CFG_SW_RST_0_CLR_ADDR,
		1<<PIPE_PHY_CK_RST_CFG_SW_RST_0_PCIE2_PHY_SW_PRST_LSB |1<<PIPE_PHY_CK_RST_CFG_SW_RST_0_PCIE3_PHY_SW_PRST_LSB);
}

static void release_pcie0_reset(void)
{
	core_write_word(PIPE_PHY_GLB_PCS_CTL0_4_SET_ADDR, 1<<PIPE_PHY_GLB_PCS_CTL0_4_PIPE0_RESET_N_LSB);
	core_write_word(PIPE_SYS_GLB_PCIE_CTL0_19_SET_ADDR, 1<<PIPE_SYS_GLB_PCIE_CTL0_19_SET_POWER_UP_RST_N_SET_LSB | 1<<PIPE_SYS_GLB_PCIE_CTL0_19_SET_BUTTON_RST_N_SET_LSB);
}

static void release_pcie1_reset(void)
{
	core_write_word(PIPE_PHY_GLB_PCS_CTL1_4_SET_ADDR, 1<<PIPE_PHY_GLB_PCS_CTL1_4_PIPE0_RESET_N_LSB);
	core_write_word(PIPE_SYS_GLB_PCIE_CTL1_19_SET_ADDR, 1<<PIPE_SYS_GLB_PCIE_CTL1_19_SET_POWER_UP_RST_N_SET_LSB | 1<<PIPE_SYS_GLB_PCIE_CTL1_19_SET_BUTTON_RST_N_SET_LSB);
}

static void pcie_aux_clk_set(void)
{
    core_write_word(PIPE_SYS_CLK_MUX_0_SET, 0x3<<PIPE_SYS_CLK_CLK_PIPE_BUS_SEL_LSB);
}

static void pipe_phy_config(void)
{
	//pcie phy deemph/swing config
	core_write_word(PIPEPHY_GLB + 0x14, 0x5965bc78);
	core_write_word(PIPEPHY_GLB + 0x38, 0x5965bc78);
}

void pcie_common_int()
{
    pice_sata_mode_init();
    release_pipe_reset();
    release_pipe_phy0_reset();
    release_pipe_phy1_reset();
    release_pcie0_reset();
    release_pcie1_reset();
    pipe_phy_config();
    pcie_aux_clk_set();

    /* Set clkreq */
    writel(PCIE0_G4A0_FUNCTION_SET, PCIE0_G4_FUNCTION_ADDR); //set function to gpio4_a0
    writel(GPIO_OUT, PCIE0_G4A0_OUT_ADDR); //set gpio4_a0 out
    writel(GPIO_PULLDOWN, PCIE0_G4A0_PULLDOWN_ADDR); //set gpio4_a0 pull down
}

void dw_pcie_set_speed_lane()
{
    u32 val;

    val = dw_pcie_read_dbi(PCIE_LINK_WIDTH_SPEED_CONTROL, 0x4);
    val &= ~DIRECT_SPEED_CHANGE;
    dw_pcie_write_dbi(PCIE_LINK_WIDTH_SPEED_CONTROL, 0x4, val);

    val = dw_pcie_read_dbi(PCIE_LINK_CTL2_REG, 0x4);
    val &= ~PCIE_LINK_SPEED_MASK;
    val |= PCIE_LINK_SPEED_GENX;
    dw_pcie_write_dbi(PCIE_LINK_CTL2_REG, 0x4, val);

    /* Set the number of lanes */
    val = dw_pcie_read_dbi(PCIE_PORT_LINK_CONTROL, 0x4);
    val &= ~PORT_LINK_MODE_MASK;
    val |= PORT_LINK_MODE_X_LANES;
    dw_pcie_write_dbi(PCIE_PORT_LINK_CONTROL, 0x4, val);

    /* Set link width speed control register */
	val = dw_pcie_read_dbi(PCIE_LINK_WIDTH_SPEED_CONTROL, 0x4);
	val &= ~PORT_LOGIC_LINK_WIDTH_MASK;
    val |= PORT_LOGIC_LINK_WIDTH_X_LANES;
    dw_pcie_write_dbi(PCIE_LINK_WIDTH_SPEED_CONTROL, 0x4, val);
}

void dw_pcie_ep_write_header()
{

    dw_pcie_write_dbi(PCI_VENDOR_ID, 0x2, AXERA_PCIE_VENDORID);
    dw_pcie_write_dbi(PCI_DEVICE_ID, 0x2, AXERA_PCIE_DEVICEID);
    dw_pcie_write_dbi(PCI_SUBSYSTEM_VENDOR_ID, 0x2, sub_vendor_id);
    dw_pcie_write_dbi(PCI_SUBSYSTEM_ID, 0x2, sub_device_id);

    dw_pcie_write_dbi(PCI_CLASS_DEVICE, 0x2,
                    AXERA_PCIE_SUBCLASSCODE | AXERA_PCIE_BASECLASSCODE << 8);

    dw_pcie_write_dbi(PCI_CACHE_LINE_SIZE, 0x1, CACHE_LINE_SIZE);
    dw_pcie_write_dbi(PCI_INTERRUPT_PIN, 0x1, INTERRUPT_PIN);
}

void dw_pcie_prog_inbound_atu_unroll(int index, int bar, u64 cpu_addr, int type)
{
    dw_pcie_writel_ib_unroll(index, PCIE_ATU_UNR_LOWER_TARGET,
				lower_32_bits(cpu_addr));
    dw_pcie_writel_ib_unroll(index, PCIE_ATU_UNR_UPPER_TARGET,
				upper_32_bits(cpu_addr));

    dw_pcie_writel_ib_unroll(index, PCIE_ATU_UNR_REGION_CTRL1, type);
    dw_pcie_writel_ib_unroll(index, PCIE_ATU_UNR_REGION_CTRL2,
            PCIE_ATU_ENABLE | PCIE_ATU_BAR_MODE_ENABLE | (bar << 8));
}

static void dw_pcie_ep_reset_bar(int flags)
{
    int bar;
    int reg;
    for (bar = BAR_0; bar <= BAR_5; bar++) {
        reg = PCI_BASE_ADDRESS_0 + (4 * bar);
	    dw_pcie_write_dbi2(reg, 0x4, 0x0);
	    dw_pcie_write_dbi(reg, 0x4, 0x0);
	    if (flags & PCI_BASE_ADDRESS_MEM_TYPE_64) {
		    dw_pcie_write_dbi2(reg + 4, 0x4, 0x0);
		    dw_pcie_write_dbi(reg + 4, 0x4, 0x0);
	    }
    }
}

static unsigned int dw_pcie_ep_find_ext_capability(int cap)
{
	u32 header;
	int pos = PCI_CFG_SPACE_SIZE;

	while (pos) {
		header = dw_pcie_read_dbi(pos, 0x4);
		if (PCI_EXT_CAP_ID(header) == cap)
			return pos;

		pos = PCI_EXT_CAP_NEXT(header);
		if (!pos)
			break;
	}

	return 0;
}

void dw_pcie_ep_set_bar()
{
    u32 reg;
    int index = INBOUND_INDEX0;
    int offset, i;
    int bar, nbars;
    int flags = PCI_BASE_ADDRESS_MEM_TYPE_32;
    u64 cpu_addr = PCIE_MSG_BASE;

    dw_pcie_ep_reset_bar(flags);

    for(bar = BAR_0; bar <= BAR_5; bar++) {

        switch (bar)
        {
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

        dw_pcie_prog_inbound_atu_unroll(index, bar,
                        cpu_addr, PCIE_ATU_TYPE_MEM);

        dw_pcie_write_dbi2(reg, 0x4, lower_32_bits(bar_size[bar]- 1));
        dw_pcie_write_dbi(reg, 0x4, flags);

        if (flags & PCI_BASE_ADDRESS_MEM_TYPE_64) {
            dw_pcie_write_dbi2(reg + 4, 0x4, upper_32_bits(bar_size[bar] - 1));
            dw_pcie_write_dbi(reg + 4, 0x4, 0);
            bar++;
        }
    }

rebar:
    offset = dw_pcie_ep_find_ext_capability(PCI_EXT_CAP_ID_REBAR);
    if (offset) {
		reg = dw_pcie_read_dbi(offset + PCI_REBAR_CTRL, 0x4);
		nbars = (reg & PCI_REBAR_CTRL_NBAR_MASK) >>
			PCI_REBAR_CTRL_NBAR_SHIFT;

		for (i = 0; i < nbars; i++, offset += PCI_REBAR_CTRL) {
			dw_pcie_write_dbi(offset + PCI_REBAR_CAP, 0x4, 0x10);
            reg= dw_pcie_read_dbi(offset + PCI_REBAR_CAP + 0x4, 0x4);
            reg &= (~0x3f00);
            dw_pcie_write_dbi(offset + PCI_REBAR_CAP + 0x4, 0x4, reg);
        }
	}
}

void pcie_reset_bar_map(void)
{
    u32 l;
    u32 sz;
    u64 cpu_addr;
    int index = INBOUND_INDEX2;

    l = dw_pcie_read_dbi(0x20, 0x4);
    dw_pcie_write_dbi(0x20, 0x4, (l|(~0)));
    sz = dw_pcie_read_dbi(0x20, 0x4);
    dw_pcie_write_dbi(0x20, 0x4, l);
    sz = ~(sz & (~0xf));

    if (sz == 0xfffff) {
        cpu_addr = PCIE_SYS_RESET_BASE_1M;
    } else {
        cpu_addr = PCIE_SYS_RESET_BASE;
    }

    dw_pcie_prog_inbound_atu_unroll(index, BAR_4,
                        cpu_addr, PCIE_ATU_TYPE_MEM);
}

void axera_pcie_aspm_status_set()
{
    int val;

	/*disable pcie aspm support*/
	val = dw_pcie_read_dbi(PCIE_LINK_ASPM_CAP, 0x4);
	val &= ~(0x3 << 10);
	dw_pcie_write_dbi(PCIE_LINK_ASPM_CAP, 0x4, val);
}

void dw_pcie_ep_set_msi()
{
    u32 val, reg;
    int interrupts = PCI_MSI_INT_NUM;

    reg = PCI_MSI_CAP + PCI_MSI_FLAGS;
    val = dw_pcie_read_dbi(reg, 0x2);
    val &= ~PCI_MSI_FLAGS_QMASK;
    val |= (interrupts << 1) & PCI_MSI_FLAGS_QMASK;
    dw_pcie_write_dbi(reg, 0x2, val);
}

int pcie_dma_read(int chan, u32 low_src_addr, u32 upper_src_addr, u32 low_dst_addr,
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

int pcie_read(struct pci_msg_reg *msg_header)
{
    int chan = 0;
    int dma_read_status = 0;

    dma_read_status = pcie_dma_read(chan, msg_header->low_src_addr, msg_header->upper_src_addr,
                msg_header->low_dst_addr, msg_header->upper_dst_addr, msg_header->size);

    if(dma_read_status != 0x3)
        return PCIE_HDMA_READ_ABORT;

    return dma_read_status;
}

static int read_image_data(struct pci_msg_reg *msg_header)
{
    return pcie_read(msg_header);
}

static int pci_ep_read_rc(struct pci_msg_reg *msg_header)
{
    int ret;
    char *img_addr = (char *)OCM_START_ADDR;

    log_print(LOG_PCIE_DMA_READ);

    ret = read_image_data(msg_header);
    if (ret < 0) {
        log_print(LOG_PCIE_READ_FAIL);
        return ret;
    } else {
        log_print(LOG_PCIE_READ_SUC);
        return lower_32_bits((u64)img_addr);
    }
}

int polling_remote_command(struct pci_msg_reg *msg_header)
{
    u32 command;
    int ret = 0;

    command = msg_header->command;
    if (!command)
        goto reset_handler;

    msg_header->command = 0;
    msg_header->status = 0;

    if (command & COMMAND_READ) {
        ret = pci_ep_read_rc(msg_header);
        if (ret < 0)
            msg_header->status |= STATUS_READ_FAIL;
		else
            msg_header->status |= STATUS_READ_SUCCESS;
    }

    return ret;

reset_handler:
    udelay(1000);
    return ret;
}

void set_pcie_subid(u32 vendor_id, u32 device_id)
{
    sub_vendor_id = vendor_id;
    sub_device_id = device_id;
}

void dw_ep_init()
{
    log_print(LOG_PCIE_EP_INIT);

    pcie_common_int();
    pcie_device_type_set();
    dw_pcie0_awrmisc_enable(1);
    dw_pcie_dbi_ro_wr_en();
    dw_pcie_ep_write_header();
    dw_pcie_set_speed_lane();
    dw_pcie_ep_set_bar();
    axera_pcie_aspm_status_set();
    dw_pcie_ep_set_msi();
    dw_pcie_dbi_ro_wr_dis();

    /* set ltssm enable */
    dw_pcie0_ltssm_enable(1);

}
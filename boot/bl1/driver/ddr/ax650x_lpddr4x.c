/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright (c) 2022 AXERA.
 *
 */
#include "cmn.h"
#include "boot_mode.h"
#include "pll_config.h"
#include "msgblock_1d.h"
#include "msgblock_2d.h"
#include "ax650x_lpddr4x.h"
#include "ddr_reg.h"
#include "ddrc_init.h"
#include "message.h"
#include "ddrphy_init.h"
//#include "ddr_lp_cfg.h"
/*pin swap mode, 0 for DDR_PINMUX_BE, 1 for DDR_PINMUX_FE*/
static int swap_mode __attribute__((section(".data"))) = 0;
static int ddr_clk __attribute__((section(".data"))) = 0;
extern unsigned char get_chip_type_id(void);
extern int adc_read_boardid(int channel, unsigned int *data);
extern int early_printf(const char *fmt, ...);
extern int calc_board_id(int data);

struct ddr_board_config {
	u8 board_id;
	u8 ddr_num;
	u8 rank_num;
	u8 lpdd4x;
};

static struct ddr_board_config ddr_config[] = {
	#include "ddr_board_config.h"
};

static int curr_ddr_num = DDR_NUM;

static void release_ddrcfg_reset(ulong clk_rst_base)
{
	writel((1 << 0) | (1 << 1) | (1 << 3), clk_rst_base + DDR_CLK_RST_SW_RST0_CLR);
}

static void release_ddrcore_reset(ulong clk_rst_base)
{
	writel((1 << 2), clk_rst_base + DDR_CLK_RST_SW_RST0_CLR);
}

static void release_ddrphy_reset(ulong clk_rst_base)
{
	writel((1 << 4), clk_rst_base + DDR_CLK_RST_SW_RST0_CLR);
}

static void release_ddrport_reset(ulong clk_rst_base)
{
	writel((0x1f << 5), clk_rst_base + DDR_CLK_RST_SW_RST0_CLR);
}

static void release_monitor_reset(ulong clk_rst_base)
{
	writel((0x1f << 11), clk_rst_base + DDR_CLK_RST_SW_RST0_CLR);
}

static void ddr_eb_config(ulong clk_rst_base)
{
	writel(0xf, clk_rst_base + DDR_CLK_RST_CLK_EB1);
}

void __ddr_clk_set(int clk, ulong clk_rst_base)
{
	u32 div = 0;
	u32 sel = 0;
	u32 val;
	switch (clk) {
	case DDR_CLK_2133:
		sel = 4;
		div = 1;
		break;
	case DDR_CLK_3200:
		sel = 3;
		break;
	case DDR_CLK_3733:
		if(DDR0_CLK_RST_BASE == clk_rst_base) {
			pll_set(AX_DPLL0, clk / 4);
		} else {
			pll_set(AX_DPLL1, clk / 4);
		}
		sel = 4;
		break;
	case DDR_CLK_4266:
		sel = 4;
		break;
	default:
		break;
	}
	val = readl(clk_rst_base + DDR_CLK_RST_CLK_MUX_0);
	val &= ~(0x7 << 2);
	val |= sel << 2;
	writel(val, clk_rst_base + DDR_CLK_RST_CLK_MUX_0);
	writel(div | (1 << 4), clk_rst_base + DDR_CLK_RST_CLK_DIV_0);
	udelay(1);
	writel(div, clk_rst_base + DDR_CLK_RST_CLK_DIV_0);
}

static void ddr_glb_clk_set(int idx)
{
	ulong reg_base = ax650x_ddr_cfg.reg_cfg[idx].clk_rst_base;
	u32 val;
	val = readl(reg_base + DDR_CLK_RST_CLK_MUX_0);
	val &= ~(3);
	val |= 2;
	/*glb clk set to 200M */
	writel(val, reg_base + DDR_CLK_RST_CLK_MUX_0);
}

static int ddr_init_prepare(int idx)
{
	/*wakeup ddr sys */
	writel(1 << idx, PMU_GLB_WAKEUP1_SET);
	ddr_glb_clk_set(idx);
	return 0;
}

static void enable_ddr_pwrok(ulong glb_base_addr)
{
	writel(1, glb_base_addr + DDR_GLB_PWR_OK_IN);
}

static void ddr_clk_set(ulong clk_rst_base)
{
	__ddr_clk_set(ddr_clk, clk_rst_base);
}

static void ddr_ctrl_init(int idx)
{
	ulong ctrl_base_addr = ax650x_ddr_cfg.reg_cfg[idx].umctl2_base;
	ulong glb_base_addr = ax650x_ddr_cfg.reg_cfg[idx].glb_base;
	ulong clk_rst_base = ax650x_ddr_cfg.reg_cfg[idx].clk_rst_base;
	release_ddrport_reset(clk_rst_base);
	release_monitor_reset(clk_rst_base);
	ddr_eb_config(clk_rst_base);
	release_ddrcfg_reset(clk_rst_base);
	ddr_init_ctr_reg(ctrl_base_addr, 0);
	ddrc_additional_cfg(ctrl_base_addr);
	release_ddrcore_reset(clk_rst_base);
	udelay(1);
	ddr_clk_set(clk_rst_base);
	enable_ddr_pwrok(glb_base_addr);
	udelay(1);
}

static void ddr_phy_init(int idx)
{
	ulong phy_base_addr = ax650x_ddr_cfg.reg_cfg[idx].phy_base;
	ulong clk_rst_base = ax650x_ddr_cfg.reg_cfg[idx].clk_rst_base;
	ulong ctrl_base_addr = ax650x_ddr_cfg.reg_cfg[idx].umctl2_base;
	release_ddrphy_reset(clk_rst_base);
	udelay(1);
	if(swap_mode == 0) {
		lp4_ddrphy_addr_swap_be(phy_base_addr);
		lp4_ddrphy_data_swap_be(phy_base_addr);
	} else {
		lp4_ddrphy_addr_swap_fe(phy_base_addr, 3);
		lp4_ddrphy_data_swap_fe(phy_base_addr);
	}
	ddr_pre_init_phy(ctrl_base_addr, idx, dp_slp_en);
	ddr_init_phy_config(phy_base_addr, dp_slp_en, ddr_clk);
}

static int ddr_init_complete(int idx)
{
	ulong ctrl_base_addr = ax650x_ddr_cfg.reg_cfg[idx].umctl2_base;
	ulong phy_base_addr = ax650x_ddr_cfg.reg_cfg[idx].phy_base;
	ddr_load_phy_init_engine_image(phy_base_addr, ddr_clk);
	ddr_wait_dfi_init_complete(ctrl_base_addr);
	ddr_wait_normal_stat(ctrl_base_addr, dp_slp_en);
	ddr_post_init(ctrl_base_addr, idx, dp_slp_en);
	return 0;
}

static void buslock_enable(void)
{
	writel(1 << 25, (AON_CLK_RST_BASE + AON_CLK_RST_EB1_SET));
	writel(1 << 4, (AON_CLK_RST_BASE + AON_CLK_RST_SW_RST0_CLR));
}

int ddr_interleave_mode_set(int mode)
{
	unsigned int temp;
	temp = readl((unsigned int)0x450f084);
	/*
	 * mode 0: only ddr0, address is: 0x1_0000_0000
	 * mode 1: only ddr1, address is: 0x1_0000_0000
	 * mode 2: ddr0 & ddr1 non interleave mode:
	 *		   ddr0 address: 0x1_0000_0000, ddr1 address: 0x5_0000_0000
	 * mode 3: ddr0 & ddr1 interleave mode: address is: 0x1_0000_0000
	 */
	temp &= ~(3);
	if ((get_chip_type_id() != AX650N_CHIP) && (get_chip_type_id() != AX650C_CHIP)) {
		/* AX650A interleave set 256B, AX650N and AX650C insterlave set 64B*/
		temp |= (3 << 16);
	}
	temp |= mode;
	writel(temp, (unsigned int)0x450f084);
	return 0;
}

static void ddr_lpddr_type_get(void)
{
	unsigned int adc_data;
	u8 board_id;
	unsigned int i;
	adc_read_boardid(0, &adc_data);
	board_id = (u8)calc_board_id(adc_data);
	for (i = 0; i < sizeof(ddr_config) / sizeof(struct ddr_board_config); i++) {
		if (ddr_config[i].board_id == board_id) {
			curr_ddr_num = ddr_config[i].ddr_num;
			curr_rank_num = ddr_config[i].rank_num;
			use_lpddr4 = ddr_config[i].lpdd4x ? 0 : 1;
			break;
		}
	}
}

void ddr_config_get(void)
{
#if DDR_CFG_2133
	ddr_clk = DDR_CLK_2133;
#endif
#if DDR_CFG_3200
	ddr_clk = DDR_CLK_3200;
#endif
#if DDR_CFG_3733
	ddr_clk = DDR_CLK_3733;
#endif
#if DDR_CFG_4266
	ddr_clk = DDR_CLK_4266;
#endif
	if ((get_chip_type_id() == AX650N_CHIP) || (get_chip_type_id() == AX650C_CHIP)) {
		swap_mode = 0;
	} else if (get_chip_type_id() == AX750_CHIP) {
		swap_mode = 1;
	} else {
		swap_mode = 2;
	}
	ddr_lpddr_type_get();
}
void mrw(u8 addr, u8 val, ulong umctl_base)
{
	u32 temp;
	temp = (addr << 8) | (val);
	writel(temp, umctl_base + 0x14);
	temp = (3 << 4);
	writel(temp, umctl_base + 0x10);
	writel(temp | (1 << 31), umctl_base + 0x10);
}

static void ddr_init_sequence(int ddr_num)
{
	int i;
	/* start clocks,reset phy,init phy configuration,set phy clocks,load IMEM and DMEM */
	for(i = 0; i < ddr_num; i++) {
		ddr_init_prepare(i);
		ddr_ctrl_init(i);
		ddr_phy_init(i);
		ddr_1d_imem_dmem_load(i, ddr_clk);
	}

#if DDR_2D_TRAINING
	for(i = 0; i < ddr_num; i++) {
		#if !DDR_CFG_DFS
			ddr_training_check(i);
		#endif
		ddr_2d_imem_dmem_load(i, ddr_clk);
	}
#endif

	for(i = 0; i < ddr_num; i++) {
		ddr_training_check(i);
		ddr_init_complete(i);
	}
}
void ddr_init(void)
{
	ddr_config_get();
	buslock_enable();
	ddr_init_sequence(curr_ddr_num);
	if(curr_ddr_num == 2) {
		ddr_interleave_mode_set(3);
	} else {
		ddr_interleave_mode_set(0);
	}
}

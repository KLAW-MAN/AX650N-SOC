/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright (c) 2022 AXERA.
 *
 */
#define IMEM_SIZE (32 * 1024)
#define DMEM_SIZE (16 * 1024)
static int train_2d __attribute__((section(".data"))) = 0;
struct ddr_phy_cfg {
	u32 freq;
	const struct phy_cfg_param *reg_cfg;
	u32 reg_num;
};
#if DDR_CFG_DFS
	#define DDR_PHY_TO_CLK_OFFSET	0x1FF0000
	#define DFS_FREQ_NUMS			3
	#if DDR_CFG_4266
		static u16 dfs_sence1[3] = {DDR_CLK_4266,DDR_CLK_3200,DDR_CLK_2133};
	#endif
	#if DDR_CFG_3733
		static u16 dfs_sence2[2] = {DDR_CLK_3733,DDR_CLK_2133};
	#endif
	#if DDR_CFG_3200
		static u16 dfs_sence3[2] = {DDR_CLK_3200,DDR_CLK_2133};
	#endif
static const struct phy_cfg_param dmem_2133[] = {
	{0x54003 * 2, 0x0855},
/* 	{0x54019 * 2, 0x1b34},
	{0x5401f * 2, 0x1b34},
	{0x54032 * 2, 0x3400},
	{0x54033 * 2, 0x301b},
	{0x54038 * 2, 0x3400},
	{0x54039 * 2, 0x301b}, */
};

static const struct phy_cfg_param dmem_3200[] = {
	{0x54003 * 2, 0x0c80},
/* 	{0x54019 * 2, 0x2d54},
	{0x5401f * 2, 0x2d54},
	{0x54032 * 2, 0x5400},
	{0x54033 * 2, 0x302d},
	{0x54038 * 2, 0x5400},
	{0x54039 * 2, 0x302d}, */
};

static const struct phy_cfg_param dmem_3733[] = {
	{0x54003 * 2, 0x0e94},
/* 	{0x54019 * 2, 0x3664},
	{0x5401f * 2, 0x3664},
	{0x54032 * 2, 0x6400},
	{0x54033 * 2, 0x3036},
	{0x54038 * 2, 0x6400},
	{0x54039 * 2, 0x3036}, */
};

static const struct phy_cfg_param dmem_4266[] = {
	{0x54003 * 2, 0x10aa},
/* 	{0x54019 * 2, 0x3f74},
	{0x5401f * 2, 0x3f74},
	{0x54032 * 2, 0x7400},
	{0x54033 * 2, 0x303f},
	{0x54038 * 2, 0x7400},
	{0x54039 * 2, 0x303f}, */
};

struct ddr_phy_cfg dmem_freq_cfg[] = {
	{2133, dmem_2133, ARRAY_SIZE(dmem_2133)},
	{3200, dmem_3200, ARRAY_SIZE(dmem_3200)},
	{3733, dmem_3733, ARRAY_SIZE(dmem_3733)},
	{4266, dmem_4266, ARRAY_SIZE(dmem_4266)},
};
#endif
extern int early_printf(const char *fmt, ...);
extern void __ddr_clk_set(int clk, ulong clk_rst_base);
static void wait_fw_done(ulong base_addr);
static void ddr_load_2d_imem_image(ulong base_addr);
static void ddr_load_2d_dmem_image(ulong base_addr);
static void ddr_phy_modify_dmem(ulong phy_base, u32 freq);
static const u16 imem_data_1d[IMEM_SIZE / sizeof(u16)] = {
#include "ddr_lpddr4_pmu_train_imem.h"
};

static const u16 imem_data_2d[IMEM_SIZE / sizeof(u16)] = {
#include "ddr_load_2D_imem_image.h"
};

static const u16 dmem_data_1d[] = {
#include "ddr_lpddr4_pmu_train_dmem.h"
};

static const u16 dmem_data_2d[] = {
#include "ddr_load_2D_dmem_image.h"
};

static const struct phy_cfg_param ddrphy_cfg[] = {
#include "ddr_init_phy_config.h"
};

static const struct phy_cfg_param ddrphy_pie[] = {
#include "ddr_load_phy_init_image.h"
};

#if DDR_CFG_DFS
static void ddr_pstate_set(ulong base_addr, int i)
{
	core_write_halfword(base_addr + 0xd0000 * 2, 0);
	core_write_halfword(base_addr + 0x54002 * 2, i);
	if(i == 0) {
		core_write_halfword(base_addr + 0x54008 * 2, 0x131f);
	} else if (i == 1 || i == 2) {
		core_write_halfword(base_addr + 0x54008 * 2, 0x121f);
	}
	core_write_halfword(base_addr + 0xd0000 * 2, 1);
}
#endif

static void lp4_ddrphy_addr_swap_fe(ulong base_addr, u32 chan_ena)
{
	core_write_halfword(base_addr + 0x00020100 * 2, 0x00000002);
	core_write_halfword(base_addr + 0x00020101 * 2, 0x00000004);
	core_write_halfword(base_addr + 0x00020102 * 2, 0x00000000);
	core_write_halfword(base_addr + 0x00020103 * 2, 0x00000005);
	if (base_addr == DDR0_PHY_REG_BASE) {
		core_write_halfword(base_addr + 0x00020104 * 2, 0x00000003);
		core_write_halfword(base_addr + 0x00020105 * 2, 0x00000001);
	} else {
		core_write_halfword(base_addr + 0x00020104 * 2, 0x00000001);
		core_write_halfword(base_addr + 0x00020105 * 2, 0x00000003);
	}

	core_write_halfword(base_addr + 0x00020110 * 2, 0x00000004);
	core_write_halfword(base_addr + 0x00020111 * 2, 0x00000002);
	core_write_halfword(base_addr + 0x00020112 * 2, 0x00000005);
	core_write_halfword(base_addr + 0x00020113 * 2, 0x00000000);
	core_write_halfword(base_addr + 0x00020114 * 2, 0x00000001);
	core_write_halfword(base_addr + 0x00020115 * 2, 0x00000003);
}

static void lp4_ddrphy_data_swap_fe(ulong base_addr)
{
	core_write_halfword(base_addr + (0x000100A0 + 0 * 0x1000) * 2,
			    0x00000005);
	core_write_halfword(base_addr + (0x000100A1 + 0 * 0x1000) * 2,
			    0x00000004);
	core_write_halfword(base_addr + (0x000100A2 + 0 * 0x1000) * 2,
			    0x00000003);
	core_write_halfword(base_addr + (0x000100A3 + 0 * 0x1000) * 2,
			    0x00000000);
	core_write_halfword(base_addr + (0x000100A4 + 0 * 0x1000) * 2,
			    0x00000002);
	core_write_halfword(base_addr + (0x000100A5 + 0 * 0x1000) * 2,
			    0x00000001);
	core_write_halfword(base_addr + (0x000100A6 + 0 * 0x1000) * 2,
			    0x00000007);
	core_write_halfword(base_addr + (0x000100A7 + 0 * 0x1000) * 2,
			    0x00000006);

	core_write_halfword(base_addr + (0x000100A0 + 1 * 0x1000) * 2,
			    0x00000002);
	core_write_halfword(base_addr + (0x000100A1 + 1 * 0x1000) * 2,
			    0x00000000);
	core_write_halfword(base_addr + (0x000100A2 + 1 * 0x1000) * 2,
			    0x00000003);
	core_write_halfword(base_addr + (0x000100A3 + 1 * 0x1000) * 2,
			    0x00000001);
	core_write_halfword(base_addr + (0x000100A4 + 1 * 0x1000) * 2,
			    0x00000006);
	core_write_halfword(base_addr + (0x000100A5 + 1 * 0x1000) * 2,
			    0x00000007);
	core_write_halfword(base_addr + (0x000100A6 + 1 * 0x1000) * 2,
			    0x00000005);
	core_write_halfword(base_addr + (0x000100A7 + 1 * 0x1000) * 2,
			    0x00000004);

	core_write_halfword(base_addr + (0x000100A0 + 2 * 0x1000) * 2,
			    0x00000003);
	core_write_halfword(base_addr + (0x000100A1 + 2 * 0x1000) * 2,
			    0x00000001);
	core_write_halfword(base_addr + (0x000100A2 + 2 * 0x1000) * 2,
			    0x00000002);
	core_write_halfword(base_addr + (0x000100A3 + 2 * 0x1000) * 2,
			    0x00000000);
	core_write_halfword(base_addr + (0x000100A4 + 2 * 0x1000) * 2,
			    0x00000006);
	core_write_halfword(base_addr + (0x000100A5 + 2 * 0x1000) * 2,
			    0x00000007);
	core_write_halfword(base_addr + (0x000100A6 + 2 * 0x1000) * 2,
			    0x00000005);
	core_write_halfword(base_addr + (0x000100A7 + 2 * 0x1000) * 2,
			    0x00000004);

	core_write_halfword(base_addr + (0x000100A0 + 3 * 0x1000) * 2,
			    0x00000005);
	core_write_halfword(base_addr + (0x000100A1 + 3 * 0x1000) * 2,
			    0x00000004);
	core_write_halfword(base_addr + (0x000100A2 + 3 * 0x1000) * 2,
			    0x00000003);
	core_write_halfword(base_addr + (0x000100A3 + 3 * 0x1000) * 2,
			    0x00000000);
	core_write_halfword(base_addr + (0x000100A4 + 3 * 0x1000) * 2,
			    0x00000002);
	core_write_halfword(base_addr + (0x000100A5 + 3 * 0x1000) * 2,
			    0x00000001);
	core_write_halfword(base_addr + (0x000100A6 + 3 * 0x1000) * 2,
			    0x00000007);
	core_write_halfword(base_addr + (0x000100A7 + 3 * 0x1000) * 2,
			    0x00000006);
}

static void lp4_ddrphy_addr_swap_be(ulong base_addr)
{
	core_write_halfword(base_addr + 0x00020100 * 2, 0x00000000);
	core_write_halfword(base_addr + 0x00020101 * 2, 0x00000001);
	core_write_halfword(base_addr + 0x00020102 * 2, 0x00000003);
	core_write_halfword(base_addr + 0x00020103 * 2, 0x00000005);
	core_write_halfword(base_addr + 0x00020104 * 2, 0x00000002);
	core_write_halfword(base_addr + 0x00020105 * 2, 0x00000004);
	core_write_halfword(base_addr + 0x00020110 * 2, 0x00000001);
	core_write_halfword(base_addr + 0x00020111 * 2, 0x00000000);
	core_write_halfword(base_addr + 0x00020112 * 2, 0x00000003);
	core_write_halfword(base_addr + 0x00020113 * 2, 0x00000004);
	core_write_halfword(base_addr + 0x00020114 * 2, 0x00000005);
	core_write_halfword(base_addr + 0x00020115 * 2, 0x00000002);
}

static void lp4_ddrphy_data_swap_be(ulong base_addr)
{
	core_write_halfword(base_addr + (0x000100A0 + 0 * 0x1000) * 2,
			    0x00000005);
	core_write_halfword(base_addr + (0x000100A1 + 0 * 0x1000) * 2,
			    0x00000004);
	core_write_halfword(base_addr + (0x000100A2 + 0 * 0x1000) * 2,
			    0x00000003);
	core_write_halfword(base_addr + (0x000100A3 + 0 * 0x1000) * 2,
			    0x00000000);
	core_write_halfword(base_addr + (0x000100A4 + 0 * 0x1000) * 2,
			    0x00000002);
	core_write_halfword(base_addr + (0x000100A5 + 0 * 0x1000) * 2,
			    0x00000001);
	core_write_halfword(base_addr + (0x000100A6 + 0 * 0x1000) * 2,
			    0x00000007);
	core_write_halfword(base_addr + (0x000100A7 + 0 * 0x1000) * 2,
			    0x00000006);

	core_write_halfword(base_addr + (0x000100A0 + 1 * 0x1000) * 2,
			    0x00000002);
	core_write_halfword(base_addr + (0x000100A1 + 1 * 0x1000) * 2,
			    0x00000000);
	core_write_halfword(base_addr + (0x000100A2 + 1 * 0x1000) * 2,
			    0x00000003);
	core_write_halfword(base_addr + (0x000100A3 + 1 * 0x1000) * 2,
			    0x00000001);
	core_write_halfword(base_addr + (0x000100A4 + 1 * 0x1000) * 2,
			    0x00000006);
	core_write_halfword(base_addr + (0x000100A5 + 1 * 0x1000) * 2,
			    0x00000007);
	core_write_halfword(base_addr + (0x000100A6 + 1 * 0x1000) * 2,
			    0x00000005);
	core_write_halfword(base_addr + (0x000100A7 + 1 * 0x1000) * 2,
			    0x00000004);

	core_write_halfword(base_addr + (0x000100A0 + 2 * 0x1000) * 2,
			    0x00000003);
	core_write_halfword(base_addr + (0x000100A1 + 2 * 0x1000) * 2,
			    0x00000001);
	core_write_halfword(base_addr + (0x000100A2 + 2 * 0x1000) * 2,
			    0x00000002);
	core_write_halfword(base_addr + (0x000100A3 + 2 * 0x1000) * 2,
			    0x00000000);
	core_write_halfword(base_addr + (0x000100A4 + 2 * 0x1000) * 2,
			    0x00000006);
	core_write_halfword(base_addr + (0x000100A5 + 2 * 0x1000) * 2,
			    0x00000007);
	core_write_halfword(base_addr + (0x000100A6 + 2 * 0x1000) * 2,
			    0x00000005);
	core_write_halfword(base_addr + (0x000100A7 + 2 * 0x1000) * 2,
			    0x00000004);

	core_write_halfword(base_addr + (0x000100A0 + 3 * 0x1000) * 2,
			    0x00000002);
	core_write_halfword(base_addr + (0x000100A1 + 3 * 0x1000) * 2,
			    0x00000000);
	core_write_halfword(base_addr + (0x000100A2 + 3 * 0x1000) * 2,
			    0x00000005);
	core_write_halfword(base_addr + (0x000100A3 + 3 * 0x1000) * 2,
			    0x00000003);
	core_write_halfword(base_addr + (0x000100A4 + 3 * 0x1000) * 2,
			    0x00000001);
	core_write_halfword(base_addr + (0x000100A5 + 3 * 0x1000) * 2,
			    0x00000004);
	core_write_halfword(base_addr + (0x000100A6 + 3 * 0x1000) * 2,
			    0x00000006);
	core_write_halfword(base_addr + (0x000100A7 + 3 * 0x1000) * 2,
			    0x00000007);
}

static void lp4_ddrphy_addr_swap_750(ulong base_addr)
{
	core_write_halfword(base_addr + 0x00020100 * 2, 0x00000001);
	core_write_halfword(base_addr + 0x00020101 * 2, 0x00000005);
	core_write_halfword(base_addr + 0x00020102 * 2, 0x00000003);
	core_write_halfword(base_addr + 0x00020103 * 2, 0x00000004);
	core_write_halfword(base_addr + 0x00020104 * 2, 0x00000000);
	core_write_halfword(base_addr + 0x00020105 * 2, 0x00000002);

	core_write_halfword(base_addr + 0x00020110 * 2, 0x00000001);
	core_write_halfword(base_addr + 0x00020111 * 2, 0x00000005);
	core_write_halfword(base_addr + 0x00020112 * 2, 0x00000003);
	core_write_halfword(base_addr + 0x00020113 * 2, 0x00000004);
	core_write_halfword(base_addr + 0x00020114 * 2, 0x00000000);
	core_write_halfword(base_addr + 0x00020115 * 2, 0x00000002);
}

static void lp4_ddrphy_data_swap_750(ulong base_addr)
{
	core_write_halfword(base_addr + (0x000100A0 + 0 * 0x1000) * 2, 0x00000000);
	core_write_halfword(base_addr + (0x000100A1 + 0 * 0x1000) * 2, 0x00000001);
	core_write_halfword(base_addr + (0x000100A2 + 0 * 0x1000) * 2, 0x00000002);
	core_write_halfword(base_addr + (0x000100A3 + 0 * 0x1000) * 2, 0x00000007);
	core_write_halfword(base_addr + (0x000100A4 + 0 * 0x1000) * 2, 0x00000004);
	core_write_halfword(base_addr + (0x000100A5 + 0 * 0x1000) * 2, 0x00000005);
	core_write_halfword(base_addr + (0x000100A6 + 0 * 0x1000) * 2, 0x00000006);
	core_write_halfword(base_addr + (0x000100A7 + 0 * 0x1000) * 2, 0x00000003);

	core_write_halfword(base_addr + (0x000100A0 + 1 * 0x1000) * 2, 0x00000001);
	core_write_halfword(base_addr + (0x000100A1 + 1 * 0x1000) * 2, 0x00000002);
	core_write_halfword(base_addr + (0x000100A2 + 1 * 0x1000) * 2, 0x00000003);
	core_write_halfword(base_addr + (0x000100A3 + 1 * 0x1000) * 2, 0x00000000);
	core_write_halfword(base_addr + (0x000100A4 + 1 * 0x1000) * 2, 0x00000004);
	core_write_halfword(base_addr + (0x000100A5 + 1 * 0x1000) * 2, 0x00000007);
	core_write_halfword(base_addr + (0x000100A6 + 1 * 0x1000) * 2, 0x00000006);
	core_write_halfword(base_addr + (0x000100A7 + 1 * 0x1000) * 2, 0x00000005);

	core_write_halfword(base_addr + (0x000100A0 + 2 * 0x1000) * 2, 0x00000000);
	core_write_halfword(base_addr + (0x000100A1 + 2 * 0x1000) * 2, 0x00000003);
	core_write_halfword(base_addr + (0x000100A2 + 2 * 0x1000) * 2, 0x00000002);
	core_write_halfword(base_addr + (0x000100A3 + 2 * 0x1000) * 2, 0x00000001);
	core_write_halfword(base_addr + (0x000100A4 + 2 * 0x1000) * 2, 0x00000004);
	core_write_halfword(base_addr + (0x000100A5 + 2 * 0x1000) * 2, 0x00000007);
	core_write_halfword(base_addr + (0x000100A6 + 2 * 0x1000) * 2, 0x00000006);
	core_write_halfword(base_addr + (0x000100A7 + 2 * 0x1000) * 2, 0x00000005);

	core_write_halfword(base_addr + (0x000100A0 + 3 * 0x1000) * 2, 0x00000000);
	core_write_halfword(base_addr + (0x000100A1 + 3 * 0x1000) * 2, 0x00000001);
	core_write_halfword(base_addr + (0x000100A2 + 3 * 0x1000) * 2, 0x00000002);
	core_write_halfword(base_addr + (0x000100A3 + 3 * 0x1000) * 2, 0x00000007);
	core_write_halfword(base_addr + (0x000100A4 + 3 * 0x1000) * 2, 0x00000004);
	core_write_halfword(base_addr + (0x000100A5 + 3 * 0x1000) * 2, 0x00000005);
	core_write_halfword(base_addr + (0x000100A6 + 3 * 0x1000) * 2, 0x00000006);
	core_write_halfword(base_addr + (0x000100A7 + 3 * 0x1000) * 2, 0x00000003);
}

static void ddr_pre_init_phy(ulong base_addr, u32 idx, int dp_slp_en)
{
	core_write_word(base_addr + 0x00000304, 0x00000000);	//DBG1
	if (dp_slp_en == 1) {
		core_write_word(base_addr + 0x00000030, 0x000000a0);
	} else {
		core_write_word(base_addr + 0x00000030, 0x00000020);	//PWRCTL
	}
	core_write_word(base_addr + 0x00001e04, 0x00000000);	//DCH1.DBG1
	if (dp_slp_en == 1) {
		core_write_word(base_addr + 0x00001b30, 0x000000a0);
	} else {
		core_write_word(base_addr + 0x00001b30, 0x00000020);	//DCH1.PWRCTL
	}
	core_write_word(base_addr + 0x00000320, 0x00000000);	//SWCTL
	core_write_word(base_addr + 0x000001b0, 0x00000014);	//DFIMISC
	core_write_word(base_addr + 0x000001b0, 0x00000014);	//DFIMISC
	core_write_word(base_addr + 0x00000304, 0x00000002);	//DBG1
	core_write_word(base_addr + 0x00001e04, 0x00000002);	//DCH1.DBG1
}

static void ddr_init_phy_config(ulong base_addr, int dp_slp_en, int freq)
{
	struct phy_cfg_param const *phy_cfg = ddrphy_cfg;
	int i;
	if (dp_slp_en == 1) {
		core_write_halfword(base_addr + 0x20060 * 2, 0x3);
	}
	udelay(1);
	for (i = 0; i < ARRAY_SIZE(ddrphy_cfg); i++) {
		core_write_halfword(phy_cfg[i].reg + base_addr, phy_cfg[i].val);
	}
#if (DDR_CFG_3733 || DDR_CFG_3200)
	dbi_not_open = 1;
#endif
	if (lpddr4x_6GB || lpddr4x_8GB || dbi_not_open) {
		core_write_halfword(base_addr + 0x1004a * 2, 0x500);
		core_write_halfword(base_addr + 0x1104a * 2, 0x500);
		core_write_halfword(base_addr + 0x1204a * 2, 0x500);
		core_write_halfword(base_addr + 0x1304a * 2, 0x500);
		core_write_halfword(base_addr + 0x2002d * 2, 0x0);
		core_write_halfword(base_addr + 0x12002d * 2, 0x0);
		core_write_halfword(base_addr + 0x22002d * 2, 0x0);
	}
}

static void ddr_load_1d_imem_image(ulong base_addr)
{
	u16 *ptr16;
	core_write_halfword(base_addr + 0x20060 * 2, 0x2);	// DWC_DDRPHYA_MASTER0_MemResetL
	core_write_halfword(base_addr + 0xd0000 * 2, 0x0);	// DWC_DDRPHYA_APBONLY0_MicroContMuxSel
	ptr16 = (u16 *) (base_addr + (DDR_IMEM_BASE * 2));
	for (int i = 0; i < ARRAY_SIZE(imem_data_1d); i++) {
		core_write_halfword((ulong) ptr16++, imem_data_1d[i]);
	}
	core_write_halfword(base_addr + 0xd0000 * 2, 0x1);	// DWC_DDRPHYA_APBONLY0_MicroContMuxSel
}

void ddr_load_dmem_image(ulong base_addr)
{
	u16 *ptr16;
	core_write_halfword(base_addr + 0xd0000 * 2, 0x0);	// DWC_DDRPHYA_APBONLY0_MicroContMuxSel
	ptr16 = (u16 *) (base_addr + (DDR_DMEM_BASE * 2));
	for (int i = 0; i < ARRAY_SIZE(dmem_data_1d); i++) {
		core_write_halfword((ulong) ptr16++, dmem_data_1d[i]);
	}
	core_write_halfword(base_addr + 0xd0000 * 2, 0x1);	// DWC_DDRPHYA_APBONLY0_MicroContMuxSel
}

static void wait_fw_done(ulong base_addr)
{
	u32 read_data;
	int rd_cnt = 0;
	read_data = 0;
	while ((read_data & 0xff) != 0x7) {
		read_data = get_mail(base_addr, 16);
		udelay(1);
		rd_cnt++;
#if PHY_DEBUG_ENABLE
		decode_major_message(read_data);
#endif
		if ((read_data & 0xff) == 0x7) {
			break;
		} else if(read_data == 0xff) {
			early_printf("DDR %lx training failed!\r\n", base_addr);
			while(1);
		} else if (read_data == 0x08) {
#if PHY_DEBUG_ENABLE
			decode_streaming_message(base_addr, train_2d);
#endif
		}
	}
	core_write_halfword(base_addr + 0xd0031 * 2, 0x0);
	wait_uct_shadow_regs(1, base_addr);
	core_write_halfword(base_addr + 0xd0031 * 2, 0x1);
}

static void ddr_load_2d_imem_image(ulong base_addr)
{
	u16 *ptr16;
	core_write_halfword(base_addr + 0xd0000 * 2, 0x0);	// DWC_DDRPHYA_APBONLY0_MicroContMuxSel
	ptr16 = (u16 *) (base_addr + (DDR_IMEM_BASE * 2));
	for (int i = 0; i < ARRAY_SIZE(imem_data_2d); i++) {
		core_write_halfword((ulong) ptr16++, imem_data_2d[i]);
	}
	core_write_halfword(base_addr + 0xd0000 * 2, 0x1);	// DWC_DDRPHYA_APBONLY0_MicroContMuxSel
}

static void ddr_load_2d_dmem_image(ulong base_addr)
{
	u16 *ptr16;
	core_write_halfword(base_addr + 0xd0000 * 2, 0x0);	// DWC_DDRPHYA_APBONLY0_MicroContMuxSel
	ptr16 = (u16 *) (base_addr + (DDR_DMEM_BASE * 2));
	for (int i = 0; i < ARRAY_SIZE(dmem_data_2d); i++) {
		core_write_halfword((ulong) ptr16++, dmem_data_2d[i]);
	}
	core_write_halfword(base_addr + 0xd0000 * 2, 0x1);	// DWC_DDRPHYA_APBONLY0_MicroContMuxSel
}

static void ddr_load_phy_init_engine_image(ulong base_addr, u32 freq)
{
	int i;
	core_write_halfword(base_addr + 0xd0000 * 2, 0x0);	// DWC_DDRPHYA_APBONLY0_MicroContMuxSel
	for (i = 0; i < ARRAY_SIZE(ddrphy_pie); i++) {
		core_write_halfword(ddrphy_pie[i].reg + base_addr,
				    ddrphy_pie[i].val);
	}
	core_write_halfword(base_addr + 0xd0000 * 2, 0x1);	// DWC_DDRPHYA_APBONLY0_MicroContMuxSel
}

static void ddr_phy_modify_dmem(ulong phy_base, u32 freq)
{
#if DDR_CFG_DFS
	int i;
	int count = 0;
	struct phy_cfg_param const *phy_cfg;

	core_write_halfword(phy_base + (0xd0000 * 2), 0);
	for (i = 0; i < ARRAY_SIZE(dmem_freq_cfg); i++) {
		if (dmem_freq_cfg[i].freq == freq) {
			phy_cfg = dmem_freq_cfg[i].reg_cfg;
			count = dmem_freq_cfg[i].reg_num;
			break;
		}
	}

	for (i = 0; i < count; i++) {
		core_write_halfword((phy_cfg[i].reg + phy_base),
				    phy_cfg[i].val);
	}
	core_write_halfword(phy_base + (0xd0000 * 2), 1);
#endif

	core_write_halfword(phy_base + (0xd0000 << 1), 0);
	if ((!use_lpddr4) && (curr_rank_num == 1)) {	//modify 1D_MEM
		core_write_halfword(phy_base + (0x54012 << 1), 0x0110);	//CH_A single rank
		core_write_halfword(phy_base + (0x5401b << 1), 0x1944);	//MR11 CA ODT 60ohm
		core_write_halfword(phy_base + (0x54021 << 1), 0x1944);	//MR11 CA ODT 60ohm
		core_write_halfword(phy_base + (0x5402c << 1), 0x0001);	//CH_B single rank
		core_write_halfword(phy_base + (0x54034 << 1), 0x4400);	//MR11 CA ODT 60ohm
		core_write_halfword(phy_base + (0x5403a << 1), 0x4400);	//MR11 CA ODT 60ohm
	}

	if(use_lpddr4) {
		core_write_halfword(phy_base + (0x1f04d << 1), 0xe00);	// DWC_DDRPHYA_DBYTE0_TxOdtDrvStren
		core_write_halfword(phy_base + (0x1f14d << 1), 0xe00);	// DWC_DDRPHYA_DBYTE0_TxOdtDrvStren
		core_write_halfword(phy_base + (0x1f049 << 1), 0xe3f);	// DWC_DDRPHYA_DBYTE0_TxImpedanceCtrl
		core_write_halfword(phy_base + (0x1f149 << 1), 0xe3f);	// DWC_DDRPHYA_DBYTE0_TxImpedanceCtrl
		core_write_halfword(phy_base + (0x00043 << 1), 0x7f);	// DWC_DDRPHYA_ANIB0_ATxImpedance
		core_write_halfword(phy_base + (0x01043 << 1), 0x7f);	// DWC_DDRPHYA_ANIB1_ATxImpedance
		core_write_halfword(phy_base + (0x02043 << 1), 0x7f);	// DWC_DDRPHYA_ANIB2_ATxImpedance
		core_write_halfword(phy_base + (0x03043 << 1), 0x7f);	// DWC_DDRPHYA_ANIB3_ATxImpedance
		core_write_halfword(phy_base + (0x04043 << 1), 0x7f);	// DWC_DDRPHYA_ANIB4_ATxImpedance
		core_write_halfword(phy_base + (0x05043 << 1), 0x7f);	// DWC_DDRPHYA_ANIB5_ATxImpedance
		core_write_halfword(phy_base + (0x20050 << 1), 0x11);	// DWC_DDRPHYA_MASTER0_CalDrvStr0
		core_write_halfword(phy_base + (0x2009b << 1), 0x02);	// DWC_DDRPHYA_MASTER0_CalVRefs
	}

	if (use_lpddr4) {	//modify 1D_MEM
		if (curr_rank_num == 1) {	//modify 1D_MEM
			core_write_halfword(phy_base + (0x54012 << 1), 0x0110);	//CH_A single rank
			core_write_halfword(phy_base + (0x5401b << 1), 0x1966);	//MR11 CA ODT 40ohm
			core_write_halfword(phy_base + (0x54021 << 1), 0x1966);	//MR11 CA ODT 40ohm
			core_write_halfword(phy_base + (0x5402c << 1), 0x0001);	//CH_B single rank
			core_write_halfword(phy_base + (0x54034 << 1), 0x6600);	//MR11 CA ODT 40ohm
			core_write_halfword(phy_base + (0x5403a << 1), 0x6600);	//MR11 CA ODT 40ohm
		} else {
			core_write_halfword(phy_base + (0x54012 << 1), 0x0310);	//CH_A dual rank
			core_write_halfword(phy_base + (0x5401b << 1), 0x1936);	//MR11 CA ODT 80ohm
			core_write_halfword(phy_base + (0x54021 << 1), 0x1936);	//MR11 CA ODT 80ohm
			core_write_halfword(phy_base + (0x5402c << 1), 0x0003);	//CH_B dual rank
			core_write_halfword(phy_base + (0x54034 << 1), 0x3600);	//MR11 CA ODT 80ohm
			core_write_halfword(phy_base + (0x5403a << 1), 0x3600);	//MR11 CA ODT 80ohm
		}
		core_write_halfword(phy_base + (0x5401e << 1), 0x0006);	//MR22 SOC ODT 40ohm
		core_write_halfword(phy_base + (0x54024 << 1), 0x0006);	//MR22 SOC ODT 40ohm
		core_write_halfword(phy_base + (0x54037 << 1), 0x0600);	//MR22 SOC ODT 40ohm
		core_write_halfword(phy_base + (0x5403d << 1), 0x0600);	//MR22 SOC ODT 40ohm

		core_write_halfword(phy_base + (0x5401a << 1), 0x0031);	//MR3
		core_write_halfword(phy_base + (0x54020 << 1), 0x0031);	//MR3
#if (DDR_CFG_2133 || DDR_CFG_3200)
		core_write_halfword(phy_base + (0x54033 << 1), 0xf12d);	//MR3
		core_write_halfword(phy_base + (0x54033 << 1), 0xf12d);	//MR3
		core_write_halfword(phy_base + (0x54039 << 1), 0xf12d);	//MR3
		core_write_halfword(phy_base + (0x54039 << 1), 0xf12d);	//MR3
#endif
#if DDR_CFG_3733
		core_write_halfword(phy_base + (0x54033 << 1), 0xf136);	//MR3
		core_write_halfword(phy_base + (0x54033 << 1), 0xf136);	//MR3
		core_write_halfword(phy_base + (0x54039 << 1), 0xf136);	//MR3
		core_write_halfword(phy_base + (0x54039 << 1), 0xf136);	//MR3
#endif
#if DDR_CFG_4266
		core_write_halfword(phy_base + (0x54033 << 1), 0xf13f);	//MR3
		core_write_halfword(phy_base + (0x54033 << 1), 0xf13f);	//MR3
		core_write_halfword(phy_base + (0x54039 << 1), 0xf13f);	//MR3
		core_write_halfword(phy_base + (0x54039 << 1), 0xf13f);	//MR3
#endif
	}

	if (lpddr4x_6GB || lpddr4x_8GB || dbi_not_open) {
		core_write_halfword(phy_base + (0x5401b << 1), 0x1914);
		core_write_halfword(phy_base + (0x54021 << 1), 0x1914);
		core_write_halfword(phy_base + (0x54034 << 1), 0x1400);
		core_write_halfword(phy_base + (0x5403a << 1), 0x1400);
		core_write_halfword(phy_base + (0x54035 << 1), 0x0819);
		core_write_halfword(phy_base + (0x5403b << 1), 0x0819);

		core_write_halfword(phy_base + (0x5401a << 1), 0x30);
		core_write_halfword(phy_base + (0x54020 << 1), 0x30);
		if (use_lpddr4) {
#if (DDR_CFG_2133 || DDR_CFG_3200)
				core_write_halfword(phy_base + (0x54033 << 1), 0x312d);	//MR3
				core_write_halfword(phy_base + (0x54039 << 1), 0x312d);	//MR3
#endif
#if DDR_CFG_3733
				core_write_halfword(phy_base + (0x54033 << 1), 0x3136);	//MR3
				core_write_halfword(phy_base + (0x54039 << 1), 0x3136);	//MR3
#endif
#if DDR_CFG_4266
				core_write_halfword(phy_base + (0x54033 << 1), 0x313f);	//MR3
				core_write_halfword(phy_base + (0x54039 << 1), 0x313f);	//MR3
#endif

		} else {
#if (DDR_CFG_2133 || DDR_CFG_3200)
				core_write_halfword(phy_base + (0x54033 << 1), 0x302d);	//MR3
				core_write_halfword(phy_base + (0x54039 << 1), 0x302d);	//MR3
#endif
#if DDR_CFG_3733
				core_write_halfword(phy_base + (0x54033 << 1), 0x3036);	//MR3
				core_write_halfword(phy_base + (0x54039 << 1), 0x3036);	//MR3
#endif
#if DDR_CFG_4266
				core_write_halfword(phy_base + (0x54033 << 1), 0x303f);	//MR3
				core_write_halfword(phy_base + (0x54039 << 1), 0x303f);	//MR3
#endif
		}

	}
	core_write_halfword(phy_base + (0xd0000 << 1), 1);	// DWC_DDRPHYA_APBONLY0_MicroContMuxSel
}

static void init_2D_dmem_zero(ulong base_addr)
{
	core_write_halfword(base_addr + 0xd0000 * 2, 0);
	for (int i = 0; i < 4096; i++) {
		core_write_halfword(base_addr + (0x54000 + i) * 2, 0);
	}
	core_write_halfword(base_addr + 0xd0000 * 2, 1);
}

static void ddr_training_start(ulong base_addr)
{
	core_write_halfword(base_addr + 0xd0000 * 2, 0x1);
	core_write_halfword(base_addr + 0xd0099 * 2, 0x9);
	core_write_halfword(base_addr + 0xd0099 * 2, 0x1);
	core_write_halfword(base_addr + 0xd0099 * 2, 0x0);
}

static void ddr_2d_imem_dmem_load(int idx, int freq)
{
	train_2d = 1;
#if PHY_DEBUG_ENABLE
	early_printf("#2d training data output:\r\n");
#endif
	ulong base_addr = ax650x_ddr_cfg.reg_cfg[idx].phy_base;
	init_2D_dmem_zero(base_addr);
	ddr_load_2d_imem_image(base_addr);
	ddr_load_2d_dmem_image(base_addr);
	ddr_phy_modify_dmem(base_addr, freq);
	ddr_training_start(base_addr);
}

static void ddr_training_check(int idx)
{
	ulong base_addr = ax650x_ddr_cfg.reg_cfg[idx].phy_base;
	wait_fw_done(base_addr);
	core_write_halfword(base_addr + 0xd0099 * 2, 0x1);
#if PHY_DEBUG_ENABLE
	dwc_ddrphy_init_read_msg_block(idx, 1);
#endif
}

static void ddr_1d_imem_dmem_load(int idx, int freq)
{
	train_2d = 0;
	ulong base_addr = ax650x_ddr_cfg.reg_cfg[idx].phy_base;
#if PHY_DEBUG_ENABLE
	early_printf("#1d training data output:\r\n");
#endif
	ddr_load_1d_imem_image(base_addr);
#if DDR_CFG_DFS
	for(int i = 0; i < DFS_FREQ_NUMS; i++) {
		#if DDR_CFG_4266
			freq = dfs_sence1[i];
		#endif
		#if DDR_CFG_3733
			freq = dfs_sence2[i];
			if (2 == i)break;
		#endif
		#if DDR_CFG_3200
			freq = dfs_sence3[i];
			if (2 == i)break;
		#endif
		//early_printf("ddr dfs ddr_1dtraining freq :%d\r\n",freq);
		__ddr_clk_set(freq, base_addr - DDR_PHY_TO_CLK_OFFSET);
		ddr_load_dmem_image(base_addr);
		ddr_phy_modify_dmem(base_addr, freq);
		ddr_pstate_set(base_addr,i);
		ddr_training_start(base_addr);
		ddr_training_check(idx);
	}
	#if DDR_CFG_4266
		__ddr_clk_set(DDR_CLK_4266, base_addr - DDR_PHY_TO_CLK_OFFSET);
	#endif
	#if DDR_CFG_3733
		__ddr_clk_set(DDR_CLK_3733, base_addr - DDR_PHY_TO_CLK_OFFSET);
	#endif
	#if DDR_CFG_3200
		__ddr_clk_set(DDR_CLK_3200, base_addr - DDR_PHY_TO_CLK_OFFSET);
	#endif
#else
	ddr_load_dmem_image(base_addr);
	ddr_phy_modify_dmem(base_addr, freq);
	ddr_training_start(base_addr);
#endif

}

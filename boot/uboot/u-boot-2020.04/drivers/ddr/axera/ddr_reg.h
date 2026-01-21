/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright (c) 2022 AXERA.
 *
 */
#include "ax650x_lpddr4x.h"
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long ulong;
typedef unsigned long long u64;
#define PMU_GLB_BASE 	(0x4c00000)
#define PMU_GLB_WAKEUP1_SET (PMU_GLB_BASE + 0xF4)
#define UCT_WRITE_PROT_SHADOW       0x000d0004
#define DCT_WRITE_PROT              0x000d0031
#define UCT_WRITE_ONLY_SHADOW       0x000d0032
#define UCT_DAT_WRITE_ONLY_SHADOW   0x000d0034
#define DDR0_GLB_REG_BASE       0x8000000
#define DDR0_CLK_RST_BASE       0x8010000
#define DDR0_UMCTL2_REG_BASE    0x8030000
#define DDR0_PHY_REG_BASE       0xA000000
#define DDR1_GLB_REG_BASE       0xC000000
#define DDR1_CLK_RST_BASE       0xC010000
#define DDR1_UMCTL2_REG_BASE    0xC030000
#define DDR1_PHY_REG_BASE       0xE000000
#define INTERLEAVE_SZ           16
#define DDR_SYS_GLB_AXICLK_OFF_ADDR 	0x0060
#define DDR_SYS_GLB_CORECLK_OFF_ADDR 	0x0064
#define DDR_SYS_GLB_DDR_AXI_ADDR 		0x000C
#define DDR_SYS_GLB_DDRC_SLEEP_ADDR 	0x0010
#define DDR_SYS_GLB_DDRC_WAKEUP_ADDR	0x0014
#define DDR_SYS_GLB_LPC_DATA_ADDR		0x0054
#define DDR_SYS_GLB_DFS_CTRL_ADDR		0x006C
#define DDR_SYS_GLB_DFS_CLKSRC_ADDR		0x0070
#define DDR_SYS_GLB_DFS_INT_ADDR		0x0080
#define DDRC_DFISTAT            (0x000001bc)
#define DDRC_DCH1_DFISTAT       (0x00001cbc)
#define DDRC_DBG1               (0x00000304)
#define DDRC_PWRCTL             (0x00000030)
#define DDRC_STAT               (0x00000004)
#define DDRC_MSTR               (0x00000000)
#define DDRC_MRCTRL0            (0x00000010)
#define DDRC_MRCTRL1            (0x00000014)
#define DDRC_MRSTAT             (0x00000018)
#define DDRC_MRCTRL2            (0x0000001C)
#define DDRC_DERATEEN           (0x00000020)
#define DDRC_DERATEINT          (0x00000024)
#define DDRC_MSTR2          	(0x00000028)
#define DDRC_DERATECTL          (0x0000002c)
#define DDRC_PWRCTL             (0x00000030)
#define DDRC_PWRTMG             (0x00000034)
#define DDRC_HWLPCTL            (0x00000038)
#define DDRC_HWFFCCTL           (0x0000003c)
#define DDRC_RFSHCTL0           (0x00000050)
#define DDRC_RFSHCTL1           (0x00000054)
#define DDRC_RFSHCTL3           (0x00000060)
#define DDRC_RFSHTMG            (0x00000064)
#define DDRC_RFSHTMG1           (0x00000068)
#define DDRC_CRCPARCTL0         (0x000000c0)
#define DDRC_INIT0              (0x000000d0)
#define DDRC_INIT1              (0x000000d4)
#define DDRC_INIT2              (0x000000d8)
#define DDRC_INIT3              (0x000000dc)
#define DDRC_INIT4              (0x000000e0)
#define DDRC_INIT5              (0x000000e4)
#define DDRC_INIT6              (0x000000e8)
#define DDRC_INIT7              (0x000000ec)
#define DDRC_DIMMCTL            (0x000000f0)
#define DDRC_RANKCTL            (0x000000f4)
#define DDRC_RANKCTL1           (0x000000f8)
#define DDRC_DRAMTMG0           (0x00000100)
#define DDRC_DRAMTMG1           (0x00000104)
#define DDRC_DRAMTMG2           (0x00000108)
#define DDRC_DRAMTMG3           (0x0000010c)
#define DDRC_DRAMTMG4           (0x00000110)
#define DDRC_DRAMTMG5           (0x00000114)
#define DDRC_DRAMTMG6           (0x00000118)
#define DDRC_DRAMTMG7           (0x0000011c)
#define DDRC_DRAMTMG8           (0x00000120)
#define DDRC_DRAMTMG12          (0x00000130)
#define DDRC_DRAMTMG13          (0x00000134)
#define DDRC_DRAMTMG14          (0x00000138)
#define DDRC_DRAMTMG15          (0x0000013C)
#define DDRC_DRAMTMG16          (0x00000140)
#define DDRC_DRAMTMG17          (0x00000144)
#define DDRC_RFSHTMG_HET        (0x00000150)
#define DDRC_ZQCTL0             (0x00000180)
#define DDRC_ZQCTL1             (0x00000184)
#define DDRC_ZQCTL2             (0x00000188)
#define DDRC_DFITMG0            (0x00000190)
#define DDRC_DFITMG1            (0x00000194)
#define DDRC_DFILPCFG0          (0x00000198)
#define DDRC_DFILPCFG1          (0x0000019C)
#define DDRC_DFIUPD0            (0x000001a0)
#define DDRC_DFIUPD1            (0x000001a4)
#define DDRC_DFIUPD2            (0x000001a8)
#define DDRC_DFIMISC            (0x000001b0)
#define DDRC_DFITMG2            (0x000001b4)
#define DDRC_DFITMG3            (0x000001b8)
#define DDRC_DFISTAT            (0x000001bc)
#define DDRC_DBICTL             (0x000001c0)
#define DDRC_DFIPHYMSTR         (0x000001c4)
#define DDRC_ADDRMAP0           (0x00000200)
#define DDRC_ADDRMAP1           (0x00000204)
#define DDRC_ADDRMAP2           (0x00000208)
#define DDRC_ADDRMAP3           (0x0000020c)
#define DDRC_ADDRMAP4           (0x00000210)
#define DDRC_ADDRMAP5           (0x00000214)
#define DDRC_ADDRMAP6           (0x00000218)
#define DDRC_ADDRMAP7           (0x0000021c)
#define DDRC_ADDRMAP9           (0x00000224)
#define DDRC_ADDRMAP10          (0x00000228)
#define DDRC_ADDRMAP11          (0x0000022c)
#define DDRC_ODTCFG             (0x00000240)
#define DDRC_ODTMAP             (0x00000244)
#define DDRC_SCHED              (0x00000250)
#define DDRC_SCHED1             (0x00000254)
#define DDRC_SCHED2             (0x00000258)
#define DDRC_PERFHPR1           (0x0000025c)
#define DDRC_PERFLPR1           (0x00000264)
#define DDRC_PERFWR1            (0x0000026c)
#define DDRC_SCHED3             (0x00000270)
#define DDRC_SCHED4             (0x00000274)
#define DDRC_DBG0               (0x00000300)
#define DDRC_DBG1               (0x00000304)
#define DDRC_DBGCMD             (0x0000030c)
#define DDRC_SWCTL              (0x00000320)
#define DDRC_SWCTLSTATIC        (0x00000328)
#define DDRC_POISONCFG          (0x0000036c)
#define DDRC_PCTRL_0            (0x00000490)
#define DDRC_PCTRL_1            (0x00000540)
#define DDRC_PCTRL_2            (0x000005f0)
#define DDRC_PCTRL_3            (0x000006a0)
#define DDRC_PCTRL_4            (0x00000750)
#define DDRC_DCH1_MRCTRL0       (0x00001b10)
#define DDRC_DCH1_MRCTRL1       (0x00001b14)
#define DDRC_DCH1_MRSTAT        (0x00001b18)
#define DDRC_DCH1_DERATECTL     (0x00001b2c)
#define DDRC_DCH1_PWRCTL        (0x00001b30)
#define DDRC_DCH1_HWLPCTL       (0x00001b38)
#define DDRC_DCH1_CRCPARCTL0    (0x00001bc0)
#define DDRC_DCH1_ZQCTL2        (0x00001c88)
#define DDRC_DCH1_ODTMAP        (0x00001d44)
#define DDRC_DCH1_DBG1          (0x00001e04)
#define DDRC_DCH1_DBGCMD        (0x00001e0c)
#define DDRC_FREQ1_DERATEEN     (0x00002020)
#define DDRC_FREQ1_DERATEINT    (0x00002024)
#define DDRC_FREQ1_PWRTMG       (0x00002034)
#define DDRC_FREQ1_RFSHCTL0     (0x00002050)
#define DDRC_FREQ1_RFSHTMG      (0x00002064)
#define DDRC_FREQ1_RFSHTMG1     (0x00002068)
#define DDRC_FREQ1_INIT3        (0x000020dc)
#define DDRC_FREQ1_INIT4        (0x000020e0)
#define DDRC_FREQ1_INIT6        (0x000020e8)
#define DDRC_FREQ1_INIT7        (0x000020ec)
#define DDRC_FREQ1_RANKCTL      (0x000020f4)
#define DDRC_FREQ1_RANKCTL1     (0x000020f8)
#define DDRC_FREQ1_DRAMTMG0     (0x00002100)
#define DDRC_FREQ1_DRAMTMG1     (0x00002104)
#define DDRC_FREQ1_DRAMTMG2     (0x00002108)
#define DDRC_FREQ1_DRAMTMG3     (0x0000210c)
#define DDRC_FREQ1_DRAMTMG4     (0x00002110)
#define DDRC_FREQ1_DRAMTMG5     (0x00002114)
#define DDRC_FREQ1_DRAMTMG6     (0x00002118)
#define DDRC_FREQ1_DRAMTMG7     (0x0000211c)
#define DDRC_FREQ1_DRAMTMG8     (0x00002120)
#define DDRC_FREQ1_DRAMTMG12    (0x00002130)
#define DDRC_FREQ1_DRAMTMG13    (0x00002134)
#define DDRC_FREQ1_DRAMTMG14    (0x00002138)
#define DDRC_FREQ1_DRAMTMG17    (0x00002144)
#define DDRC_FREQ1_ZQCTL0       (0x00002180)
#define DDRC_FREQ1_DFITMG0      (0x00002190)
#define DDRC_FREQ1_DFITMG1      (0x00002194)
#define DDRC_FREQ1_DFITMG2      (0x000021b4)
#define DDRC_FREQ1_ODTCFG       (0x00002240)
#define DDRC_PCCFG              (0x00000400)
#define DDRC_PCFGR_0            (0x00000404)
#define DDRC_PCFGR_1            (0x000004b4)
#define DDRC_PCFGR_2            (0x00000564)
#define DDRC_PCFGR_3            (0x00000614)
#define DDRC_PCFGR_4            (0x000006C8)
#define DDRC_PCFGW_0            (0x00000408)
#define DDRC_PCFGW_1            (0x000004b8)
#define DDRC_PCFGW_2            (0x00000568)
#define DDRC_PCFGW_3            (0x00000618)
#define DDRC_PCFGW_4            (0x000006C8)
#define DDRC_PCFGQOS0_0         (0x00000494)
#define DDRC_PCFGQOS1_0         (0x00000498)
#define DDRC_PCFGWQOS0_0        (0x0000049c)
#define DDRC_PCFGWQOS1_0        (0x000004a0)
#define DDRC_PCFGQOS0_1         (0x00000544)
#define DDRC_PCFGQOS1_1         (0x00000548)
#define DDRC_PCFGWQOS0_1        (0x0000054c)
#define DDRC_PCFGWQOS1_1        (0x00000550)
#define DDRC_PCFGQOS0_2         (0x000005f4)
#define DDRC_PCFGQOS1_2         (0x000005f8)
#define DDRC_PCFGWQOS0_2        (0x000005fc)
#define DDRC_PCFGWQOS1_2        (0x00000600)
#define DDRC_PCFGQOS0_3         (0x000006a4)
#define DDRC_PCFGQOS1_3         (0x000006a8)
#define DDRC_PCFGWQOS0_3        (0x000006ac)
#define DDRC_PCFGWQOS1_3        (0x000006b0)
#define DDRC_PCFGQOS0_4         (0x00000754)
#define DDRC_PCFGQOS1_4         (0x00000758)
#define DDRC_PCFGWQOS0_4        (0x0000075c)
#define DDRC_PCFGWQOS1_4        (0x00000760)
#define DDRC_FREQ2_DERATEEN         (0x3020)
#define DDRC_FREQ2_DERATEINT        (0x3024)
#define DDRC_FREQ2_PWRTMG           (0x3034)
#define DDRC_FREQ2_RFSHCTL0         (0x3050)
#define DDRC_FREQ2_RFSHTMG          (0x3064)
#define DDRC_FREQ2_RFSHTMG1         (0x3068)
#define DDRC_FREQ2_INIT3            (0x30dc)
#define DDRC_FREQ2_INIT4            (0x30e0)
#define DDRC_FREQ2_INIT6            (0x30e8)
#define DDRC_FREQ2_INIT7            (0x30ec)
#define DDRC_FREQ2_RANKCTL          (0x30f4)
#define DDRC_FREQ2_RANKCTL1         (0x30f8)
#define DDRC_FREQ2_DRAMTMG0         (0x3100)
#define DDRC_FREQ2_DRAMTMG1         (0x3104)
#define DDRC_FREQ2_DRAMTMG2         (0x3108)
#define DDRC_FREQ2_DRAMTMG3         (0x310c)
#define DDRC_FREQ2_DRAMTMG4         (0x3110)
#define DDRC_FREQ2_DRAMTMG5         (0x3114)
#define DDRC_FREQ2_DRAMTMG6         (0x3118)
#define DDRC_FREQ2_DRAMTMG7         (0x311c)
#define DDRC_FREQ2_DRAMTMG8         (0x3120)
#define DDRC_FREQ2_DRAMTMG9         (0x3124)
#define DDRC_FREQ2_DRAMTMG10        (0x3128)
#define DDRC_FREQ2_DRAMTMG11        (0x312c)
#define DDRC_FREQ2_DRAMTMG12        (0x3130)
#define DDRC_FREQ2_DRAMTMG13        (0x3134)
#define DDRC_FREQ2_DRAMTMG14        (0x3138)
#define DDRC_FREQ2_DRAMTMG15        (0x313C)
#define DDRC_FREQ2_DRAMTMG16        (0x3140)
#define DDRC_FREQ2_DRAMTMG17        (0x3144)
#define DDRC_FREQ2_ZQCTL0           (0x3180)
#define DDRC_FREQ2_DFITMG0          (0x3190)
#define DDRC_FREQ2_DFITMG1          (0x3194)
#define DDRC_FREQ2_DFITMG2          (0x31b4)
#define DDRC_FREQ2_DFITMG3          (0x31b8)
#define DDRC_FREQ2_ODTCFG           (0x3240)
#define DDRC_FREQ3_DERATEEN         (0x4020)
#define DDRC_FREQ3_DERATEINT        (0x4024)
#define DDRC_FREQ3_RFSHCTL0         (0x4050)
#define DDRC_FREQ3_RFSHTMG          (0x4064)
#define DDRC_FREQ3_INIT3            (0x40dc)
#define DDRC_FREQ3_INIT4            (0x40e0)
#define DDRC_FREQ3_INIT6            (0x40e8)
#define DDRC_FREQ3_INIT7            (0x40ec)
#define DDRC_FREQ3_DRAMTMG0         (0x4100)
#define DDRC_FREQ3_DRAMTMG1         (0x4104)
#define DDRC_FREQ3_DRAMTMG2         (0x4108)
#define DDRC_FREQ3_DRAMTMG3         (0x410c)
#define DDRC_FREQ3_DRAMTMG4         (0x4110)
#define DDRC_FREQ3_DRAMTMG5         (0x4114)
#define DDRC_FREQ3_DRAMTMG6         (0x4118)
#define DDRC_FREQ3_DRAMTMG7         (0x411c)
#define DDRC_FREQ3_DRAMTMG8         (0x4120)
#define DDRC_FREQ3_DRAMTMG9         (0x4124)
#define DDRC_FREQ3_DRAMTMG10        (0x4128)
#define DDRC_FREQ3_DRAMTMG11        (0x412c)
#define DDRC_FREQ3_DRAMTMG12        (0x4130)
#define DDRC_FREQ3_DRAMTMG13        (0x4134)
#define DDRC_FREQ3_DRAMTMG14        (0x4138)
#define DDRC_FREQ3_DRAMTMG15        (0x413C)
#define DDRC_FREQ3_DRAMTMG16        (0x4140)
#define DDRC_FREQ3_DRAMTMG17        (0x4144)
#define DDRC_FREQ3_ZQCTL0           (0x4180)
#define DDRC_FREQ3_DFITMG0          (0x4190)
#define DDRC_FREQ3_DFITMG1          (0x4194)
#define DDRC_FREQ3_DFITMG2          (0x41b4)
#define DDRC_FREQ3_DFITMG3          (0x41b8)
#define DDRC_FREQ3_ODTCFG           (0x4240)
#define DDRC_DFITMG0_SHADOW         (0x2190)
#define DDRC_DFITMG1_SHADOW         (0x2194)
#define DDRC_DFITMG2_SHADOW         (0x21b4)
#define DDRC_DFITMG3_SHADOW         (0x21b8)
#define DDRC_ODTCFG_SHADOW          (0x2240)
#define DDR_IMEM_BASE               (0x50000)
#define DDR_DMEM_BASE               (0x54000)
#define DDR_PIE_BASE                (0x90000)
#define DDR_CTL_NUM                 2
#define DDR_CLK_RST_SW_RST0_SET		(0x38)
#define DDR_CLK_RST_SW_RST0_CLR		(0x3C)
#define DDR_CLK_RST_CLK_EB1			(0x08)
#define DDR_CLK_RST_CLK_MUX_0		(0)
#define DDR_CLK_RST_CLK_DIV_0		(0xc)
#define DDR_GLB_PWR_OK_IN		(0x0)
#define AON_CLK_RST_BASE			(0x4210000)
#define AON_CLK_RST_EB1				(0xc)
#define AON_CLK_RST_EB1_SET				(0x40)
#define AON_CLK_RST_SW_RST0_CLR			(0x64)
struct dram_cfg_param {
	u32 reg;
	u32 val;
};
struct phy_cfg_param {
	u32 reg;
	u16 val;
};
struct ddr_reg_cfg {
	ulong umctl2_base;
	ulong phy_base;
	ulong glb_base;
	ulong clk_rst_base;
};
struct ddr_timing_cfg {
    struct ddr_reg_cfg reg_cfg[DDR_CTL_NUM];
    u32 train_2d;//is 2D training is enabled
    u32 skip_train;//skip training
    u32 freq_num;
};
#define ARRAY_SIZE(x)         (sizeof(x) / sizeof((x)[0]))
const struct ddr_timing_cfg  ax650x_ddr_cfg = {
    {
        {DDR0_UMCTL2_REG_BASE, DDR0_PHY_REG_BASE, DDR0_GLB_REG_BASE, DDR0_CLK_RST_BASE},
        {DDR1_UMCTL2_REG_BASE, DDR1_PHY_REG_BASE, DDR1_GLB_REG_BASE, DDR1_CLK_RST_BASE},
    },
    1,//trin_2d
    0,//skip_train
    1,
};
/* below is the device specific configuration, user can modified(add/delete) it as demand */
struct dev_specific_cfg {
	u8 manufacture_id; //MR5
	u8 rev_id1; //MR6
	u8 rev_id2; //MR7
	struct dram_cfg_param *ddrc_cfg;
	u32 ddrc_cfg_num;
	struct phy_cfg_param *ddrphy_cfg;
	u32 ddrphy_cfg_num;
	struct phy_cfg_param *dmem_1d_cfg;
	u32 dmem_1d_cfg_num;
	struct phy_cfg_param *dmem_2d_cfg;
	u32 dmem_2d_cfg_num;
};
/* below configuration are just example, please modified it as user's demand */
struct dram_cfg_param vendor1_ddrc_cfg[] = {
	{DDRC_DRAMTMG0, 0x1e263f28},
	{DDRC_DRAMTMG1, 0x00070738},
	{DDRC_DRAMTMG2, 0x08101116},
};
struct phy_cfg_param vendor1_ddrphy_cfg[] = {
	{0x1f04d * 2, 0x600 }, //ODT
	{0x1f14d * 2, 0x600 }, //ODT
};
struct phy_cfg_param vendor1_dmem_1d_cfg[] = {
	{0x54019 * 2, 0x366c}, //MR1 & MR2
	{0x5401a * 2, 0x0030}, //MR3 & MR4
	{0x5401b * 2, 0x1924}, //MR11 & MR12
};
struct phy_cfg_param vendor1_dmem_2d_cfg[] = {
	{0x54019 * 2, 0x366c}, //MR1 & MR2
	{0x5401a * 2, 0x0030}, //MR3 & MR4
	{0x5401b * 2, 0x1924}, //MR11 & MR12
};
struct dev_specific_cfg vendor1_speci_cfg = {
	0x5,
	0x1,
	0x40,
	vendor1_ddrc_cfg,
	ARRAY_SIZE(vendor1_ddrc_cfg),
	vendor1_ddrphy_cfg,
	ARRAY_SIZE(vendor1_ddrphy_cfg),
	vendor1_dmem_1d_cfg,
	ARRAY_SIZE(vendor1_dmem_1d_cfg),
	vendor1_dmem_2d_cfg,
	ARRAY_SIZE(vendor1_dmem_2d_cfg),
};
struct dram_cfg_param vendor2_ddrc_cfg[] = {
	{DDRC_DRAMTMG0, 0x1e263f28},
	{DDRC_DRAMTMG1, 0x00070738},
	{DDRC_DRAMTMG2, 0x08101116},
};
struct phy_cfg_param vendor2_ddrphy_cfg[] = {
	{0x1f04d * 2, 0x600 }, //ODT
	{0x1f14d * 2, 0x600 }, //ODT
};
struct phy_cfg_param vendor2_dmem_1d_cfg[] = {
	{0x54019 * 2, 0x366c}, //MR1 & MR2
	{0x5401a * 2, 0x0030}, //MR3 & MR4
	{0x5401b * 2, 0x1924}, //MR11 & MR12
};
struct phy_cfg_param vendor2_dmem_2d_cfg[] = {
	{0x54019 * 2, 0x366c}, //MR1 & MR2
	{0x5401a * 2, 0x0030}, //MR3 & MR4
	{0x5401b * 2, 0x1924}, //MR11 & MR12
};
struct dev_specific_cfg vendor2_speci_cfg = {
	0xff,
	0xb,
	0xb,
	vendor2_ddrc_cfg,
	ARRAY_SIZE(vendor2_ddrc_cfg),
	vendor2_ddrphy_cfg,
	ARRAY_SIZE(vendor2_ddrphy_cfg),
	vendor2_dmem_1d_cfg,
	ARRAY_SIZE(vendor2_dmem_1d_cfg),
	vendor2_dmem_2d_cfg,
	ARRAY_SIZE(vendor2_dmem_2d_cfg),
};
#define SUPPORTED_DEV_NUM 2
struct dev_specific_cfg *dev_speci_cfg[SUPPORTED_DEV_NUM] = {
	&vendor2_speci_cfg,
	&vendor2_speci_cfg
};

/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include <linux/kernel.h>
#include <linux/clk-provider.h>
#include <linux/clkdev.h>
#include <linux/io.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_device.h>
#include <linux/slab.h>
#include <linux/clk.h>
#include <dt-bindings/clock/ax650x-clock.h>
#include "clk-axera.h"

static void __init ax650x_pllc_clk_init(struct device_node *np)
{
    ax650x_clk_pll_init(np);
}
CLK_OF_DECLARE(ax650x_clk_pllc_glb, "axera,ax650x-pllc-clk", ax650x_pllc_clk_init);

/* clocks in AO (always on) controller */
static struct axera_fixed_rate_clock ax650x_fixed_rate_clks[] __initdata = {
    {AX650X_REF32K, "xtal_32k", NULL, 0, 32768,},
    {AX650X_REF24M, "xtal_24m", NULL, 0, 24000000,},
    {AX650X_CPLL0, "cpll0", NULL, 0, 1600000000,},
    {AX650X_CPLL1, "cpll1", NULL, 0, 1100000000,},
    {AX650X_DPLL0, "dpll0", NULL, 0, 1066000000,},
    {AX650X_DPLL1, "dpll1", NULL, 0, 1066000000,},
//    {AX650X_VPLL0, "vpll0", NULL, 0, 1188000000,},
//    {AX650X_VPLL1, "vpll1", NULL, 0, 1188000000,},
    {AX650X_APLL0, "apll0", NULL, 0, 1228800000,},
    {AX650X_APLL1, "apll1", NULL, 0, 1228800000,},
    {AX650X_NPLL, "npll", NULL, 0,   1600000000,},
    {AX650X_MPLL, "mpll", NULL, 0,   1400000000,},
    {AX650X_EPLL, "epll", NULL, 0,   1500000000,},
    {AX650X_GPLL, "gpll", NULL, 0,   1248000000,},
    {AX650X_DSPLL, "dspll", NULL, 0, 1600000000,},
//    {AX650X_NFPLL, "nfpll", NULL, 0, 1188000000,},
    {AX650X_NAPLL, "napll", NULL, 0, 2400000000,},
};

static struct axera_fixed_factor_clock ax650x_fixed_factor_clks[] __initdata = {
    {AX650X_CPLL_1600M, "cpll0_1600m", "cpll0", 1, 1, 0},
    {AX650X_CPLL_25M, "cpll0_25m", "cpll0", 1, 8, 0},
    {AX650X_CPLL_1100M, "cpll1_1100m", "cpll1", 1, 1, 0},
    {AX650X_CPLL_550M, "cpll1_550m", "cpll1", 1, 2, 0},
    {AX650X_CPLL_27P5M, "cpll_27p5m", "cpll1", 1, 10, 0},
    {AX650X_NPLL_1600M, "npll_1600m", "npll", 1, 1, 0},
    {AX650X_NPLL_800M, "npll_800m", "npll", 1, 2, 0},
    {AX650X_NPLL_800M_90, "npll_800m_90", "npll", 1, 2, 0},
    {AX650X_NPLL_800M_180, "npll_800m_180", "npll", 1, 2, 0},
    {AX650X_NPLL_800M_270, "npll_800m_270", "npll", 1, 2, 0},
    {AX650X_NPLL_400M, "npll_400m", "npll", 1, 4, 0},
    {AX650X_NPLL_400M_DIV2, "npll_400m_div2", "npll", 1, 8, 0},
    {AX650X_NPLL_200M, "npll_200m", "npll", 1, 8, 0},
    {AX650X_NPLL_80M, "npll_80m", "npll", 1, 20, 0},
    {AX650X_NPLL_20M, "npll_20m", "npll", 1, 80, 0},
    {AX650X_VPLL_594M, "vpll0_594m", "vpll0", 1, 2, CLK_SET_RATE_PARENT},
    {AX650X_VPLL1_594M, "vpll1_594m", "vpll1", 1, 2, CLK_SET_RATE_PARENT},
    {AX650X_VPLL_74P25M, "vpll0_74p25m", "vpll0", 1, 16, CLK_SET_RATE_PARENT},
    {AX650X_VPLL1_74P25M, "vpll1_74p25m", "vpll1", 1, 16, CLK_SET_RATE_PARENT},
    {AX650X_VPLL_27M, "vpll0_27m", "vpll0", 1, 44, CLK_SET_RATE_PARENT},
    {AX650X_VPLL1_27M, "vpll1_27m", "vpll1", 1, 44, CLK_SET_RATE_PARENT},
    {AX650X_VPLL_198M, "vpll0_198m", "vpll0", 1, 6, CLK_SET_RATE_PARENT},
    {AX650X_VPLL1_198M, "vpll1_198m", "vpll1", 1, 6, CLK_SET_RATE_PARENT},
    {AX650X_VPLL_118P8M, "vpll0_118p8m", "vpll0", 1, 10, CLK_SET_RATE_PARENT},
    {AX650X_VPLL1_118P8M, "vpll1_118p8m", "vpll1", 1, 10, CLK_SET_RATE_PARENT},
    {AX650X_VPLL_108M, "vpll0_108m", "vpll0", 1, 11, CLK_SET_RATE_PARENT},
    {AX650X_VPLL1_108M, "vpll1_108m", "vpll1", 1, 11, CLK_SET_RATE_PARENT},
    {AX650X_EPLL_1500M, "epll_1500m", "epll", 1, 1, 0},
    {AX650X_EPLL_750M, "epll_750m", "epll", 1, 2, 0},
    {AX650X_EPLL_750M_90, "epll_750m_90", "epll", 1, 2, 0},
    {AX650X_EPLL_750M_180, "epll_750m_180", "epll", 1, 2, 0},
    {AX650X_EPLL_750M_270, "epll_750m_270", "epll", 1, 2, 0},
    {AX650X_EPLL_187P5M, "epll_187p5m", "epll", 1, 8, 0},
    {AX650X_EPLL_500M, "epll_500m", "epll", 1, 3, 0},
    {AX650X_EPLL_500M_60, "epll_500m_60", "epll", 1, 3, 0},
    {AX650X_EPLL_500M_120, "epll_500m_120", "epll", 1, 3, 0},
    {AX650X_EPLL_500M_180, "epll_500m_180", "epll", 1, 3, 0},
    {AX650X_EPLL_250M, "epll_250m", "epll", 1, 6, 0},
    {AX650X_EPLL_250M_DIV2, "epll_250m_div2", "epll", 1, 12, 0},
    {AX650X_EPLL_125M, "epll_125m", "epll", 1, 12, 0},
    {AX650X_EPLL_300M, "epll_300m", "epll", 1, 5, 0},
    {AX650X_EPLL_300M_DIV2, "epll_300m_div2", "epll", 1, 10, 0},
    {AX650X_EPLL_150M, "epll_150m", "epll", 1, 10, 0},
    {AX650X_EPLL_30M, "epll_30m", "epll", 1, 50, 0},
    {AX650X_EPLL_75M, "epll_75m", "epll", 1, 20, 0},
    {AX650X_EPLL_100M, "epll_100m", "epll", 1, 15, 0},
    {AX650X_EPLL_50M, "epll_50m", "epll", 1, 30, 0},
    {AX650X_EPLL_25M, "epll_25m", "epll", 1, 60, 0},
    {AX650X_EPLL_20M, "epll_20m", "epll", 1, 75, 0},
    {AX650X_EPLL_5M, "epll_5m", "epll", 1, 300, 0},
    {AX650X_DPLL_1066M, "dpll0_1066m", "dpll0", 1, 1, 0},
    {AX650X_DPLL1_1066M, "dpll1_1066m", "dpll1", 1, 1, 0},
    {AX650X_DPLL_16P65M, "dpll0_16p65m", "dpll0", 1, 64, 0},
    {AX650X_DPLL1_16P65M, "dpll1_16p65m", "dpll1", 1, 64, 0},
    {AX650X_GPLL_1248M, "gpll_1248m", "gpll", 1, 1, 0},
    {AX650X_GPLL_624M, "gpll_624m", "gpll", 1, 2, 0},
    {AX650X_GPLL_624M_90, "gpll_624m_90", "gpll", 1, 2, 0},
    {AX650X_GPLL_624M_180, "gpll_624m_180", "gpll", 1, 2, 0},
    {AX650X_GPLL_624M_270, "gpll_624m_270", "gpll", 1, 2, 0},
    {AX650X_GPLL_24M, "gpll_24m", "gpll", 1, 52, 0},
    {AX650X_GPLL_24M_DIV2, "gpll_24m_div2", "gpll", 1, 104, 0},
    {AX650X_GPLL_12M, "gpll_12m", "gpll", 1, 104, 0},
    {AX650X_GPLL_26M, "gpll_26m", "gpll", 1, 48, 0},
    {AX650X_GPLL_19P2M, "gpll_19p2m", "gpll", 1, 65, 0},
    {AX650X_GPLL_416M, "gpll_416m", "gpll", 1, 3, 0},
    {AX650X_GPLL_208M, "gpll_208m", "gpll", 1, 6, 0},
    {AX650X_MPLL_1400M, "mpll_1400m", "mpll", 1, 1, 0},
    {AX650X_MPLL_700M, "mpll_700m", "mpll", 1, 2, 0},
    {AX650X_MPLL_700M_90, "mpll_700m_90", "mpll", 1, 2, 0},
    {AX650X_MPLL_700M_180, "mpll_700m_180", "mpll", 1, 2, 0},
    {AX650X_MPLL_700M_270, "mpll_700m_270", "mpll", 1, 2, 0},
    {AX650X_MPLL_350M, "mpll_350m", "mpll", 1, 4, 0},
    {AX650X_MPLL_21P875M, "mpll_21p875m", "mpll", 1, 64, 0},
    {AX650X_XTAL_24M, "free_24m", "xtal_24m", 1, 1, 0},
    {AX650X_XTAL_8M, "xtal_8m", "xtal_24m", 1, 3, 0},
    {AX650X_NFPLL_594M, "nfpll_594m", "nfpll", 1, 1, CLK_SET_RATE_PARENT | CLK_GET_RATE_NOCACHE},
    {AX650X_NFPLL_27M, "nfpll_27m", "nfpll", 1, 22, CLK_SET_RATE_PARENT | CLK_GET_RATE_NOCACHE},
    {AX650X_NFPLL_74P25M, "nfpll_74p25m", "nfpll", 1, 8, CLK_SET_RATE_PARENT | CLK_GET_RATE_NOCACHE},
    {AX650X_DSPLL_800M, "dspll_800m", "dspll", 1, 2, 0},
    {AX650X_DSPLL_800M_90, "dspll_800m_90", "dspll", 1, 2, 0},
    {AX650X_DSPLL_800M_180, "dspll_800m_180", "dspll", 1, 2, 0},
    {AX650X_DSPLL_800M_270, "dspll_800m_270", "dspll", 1, 2, 0},
    {AX650X_DSPLL_20M, "dspll_20m", "dspll", 1, 80, 0},
    {AX650X_APLL_20P48M, "apll0_20p48m", "apll0", 1, 60, 0},
    {AX650X_APLL_4096K, "apll0_4096k", "apll0", 1, 300, 0},
    {AX650X_APLL_12P288M, "apll0_12p288m", "apll0", 1, 100, 0},
    {AX650X_APLL_614P4M, "apll1_614p4m", "apll1", 1, 2, 0},
    {AX650X_APLL1_20P48M, "apll1_20p48m", "apll1", 1, 60, 0},
    {AX650X_APLL_76P8M, "apll1_76p8m", "apll1", 1, 16, 0},
    {AX650X_APLL_4096K, "apll1_4096k", "apll1", 1, 300, 0},
    {AX650X_APLL_12P288M, "apll1_12p288m", "apll1", 1, 100, 0},
    {AX650X_NAPLL_2400M, "napll_2400m", "napll", 1, 1, 0},
    {AX650X_NAPLL_800M, "napll_800m", "napll", 1, 3, 0},
    {AX650X_NAPLL_20M, "napll_20m", "napll", 1, 120, 0},
};

/*comm*/
static const char *clk_sen_ref1_sel[] __initconst = { "gpll_24m", "vpll0_27m", "vpll0_74p25m" };
static const char *clk_sen_ref0_sel[] __initconst = { "gpll_24m", "vpll0_27m", "vpll0_74p25m" };
static const char *clk_riscv_bus_sel[] __initconst = { "gpll_24m", "gpll_208m", "epll_500m" };
static const char *clk_i2s_ref3_sel[] __initconst = { "gpll_12m", "apll0_12p288m", "apll0_20p48m", "epll_25m" };
static const char *clk_i2s_ref2_sel[] __initconst = { "gpll_12m", "apll0_12p288m", "apll0_20p48m", "epll_25m" };
static const char *clk_i2s_ref1_sel[] __initconst = { "gpll_12m", "apll0_12p288m", "apll0_20p48m", "epll_25m" };
static const char *clk_i2s_ref0_sel[] __initconst = { "gpll_12m", "apll0_12p288m", "apll0_20p48m", "epll_25m" };
static const char *clk_ephy_ref_sel[] __initconst = { "epll_25m", "epll_50m", "epll_125m" };
static const char *clk_dbc_gpio_sel[] __initconst = { "rtc_32hz", "xtal_32k", "free_24m" };
static const char *clk_comm_wdt_sel[] __initconst = { "xtal_32k", "free_24m" };
static const char *clk_comm_tmr1_sel[] __initconst = { "xtal_32k", "free_24m" };
static const char *clk_comm_tmr0_sel[] __initconst = { "xtal_32k", "free_24m" };
static const char *clk_comm_rtc1_sel[] __initconst = { "xtal_32k", "xtal_8m" };
static const char *clk_comm_rtc0_sel[] __initconst = { "xtal_32k", "xtal_8m" };
static const char *clk_comm_ip_sel[] __initconst = { "gpll_24m", "epll_100m", "npll_200m" };
static const char *aclk_top_sel[] __initconst = { "gpll_24m", "gpll_624m", "mpll_700m", "epll_750m", "npll_800m" };
static const char *clk_ce_bus_sel[] __initconst = { "gpll_24m", "npll_200m", "epll_500m", "gpll_624m" };
static const char *aclk_top1_sel[] __initconst = { "gpll_24m", "gpll_624m_90", "mpll_700m_90", "epll_750m_90", "npll_800m_90" };
static const char *aclk_top2_sel[] __initconst = { "gpll_24m", "gpll_624m_180", "mpll_700m_180", "epll_750m_180", "npll_800m_180" };
static const char *aclk_top3_sel[] __initconst = { "gpll_24m", "gpll_624m_270", "mpll_700m_270", "epll_750m_270", "npll_800m_270" };
static const char *pclk_top_sel[] __initconst = { "gpll_24m", "epll_187p5m", "npll_200m", "gpll_208m" };
static const char *clk_sen_ref7_sel[] __initconst = { "gpll_24m", "vpll0_27m", "vpll0_74p25m" };
static const char *clk_sen_ref6_sel[] __initconst = { "gpll_24m", "vpll0_27m", "vpll0_74p25m" };
static const char *clk_sen_ref5_sel[] __initconst = { "gpll_24m", "vpll0_27m", "vpll0_74p25m" };
static const char *clk_sen_ref4_sel[] __initconst = { "gpll_24m", "vpll0_27m", "vpll0_74p25m" };
static const char *clk_sen_ref3_sel[] __initconst = { "gpll_24m", "vpll0_27m", "vpll0_74p25m" };
static const char *clk_sen_ref2_sel[] __initconst = { "gpll_24m", "vpll0_27m", "vpll0_74p25m" };

static struct axera_mux_clock ax650x_mux_clks_comm[] __initdata = {
    {AX650X_CLK_SEN_REF1_SEL, "clk_sen_ref1_sel", clk_sen_ref1_sel, ARRAY_SIZE(clk_sen_ref1_sel), CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, 29, 2, 0, NULL, },
    {AX650X_CLK_SEN_REF0_SEL, "clk_sen_ref0_sel", clk_sen_ref0_sel, ARRAY_SIZE(clk_sen_ref0_sel), CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, 27, 2, 0, NULL, },
    {AX650X_CLK_RISCV_BUS_SEL, "clk_riscv_bus_sel", clk_riscv_bus_sel, ARRAY_SIZE(clk_riscv_bus_sel), CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, 25, 2, 0, NULL, },
    {AX650X_CLK_I2S_REF3_SEL, "clk_i2s_ref3_sel", clk_i2s_ref3_sel, ARRAY_SIZE(clk_i2s_ref3_sel), CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, 23, 2, 0, NULL, },
    {AX650X_CLK_I2S_REF2_SEL, "clk_i2s_ref2_sel", clk_i2s_ref2_sel, ARRAY_SIZE(clk_i2s_ref2_sel), CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, 21, 2, 0, NULL, },
    {AX650X_CLK_I2S_REF1_SEL, "clk_i2s_ref1_sel", clk_i2s_ref1_sel, ARRAY_SIZE(clk_i2s_ref1_sel), CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, 19, 2, 0, NULL, },
    {AX650X_CLK_I2S_REF0_SEL, "clk_i2s_ref0_sel", clk_i2s_ref0_sel, ARRAY_SIZE(clk_i2s_ref0_sel), CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, 17, 2, 0, NULL, },
    {AX650X_CLK_EPHY_REF_SEL, "clk_ephy_ref_sel", clk_ephy_ref_sel, ARRAY_SIZE(clk_ephy_ref_sel), CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, 15, 2, 0, NULL, },
    {AX650X_CLK_DBC_GPIO_SEL, "clk_dbc_gpio_sel", clk_dbc_gpio_sel, ARRAY_SIZE(clk_dbc_gpio_sel), CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, 13, 2, 0, NULL, },
    {AX650X_CLK_COMM_WDT_SEL, "clk_comm_wdt_sel", clk_comm_wdt_sel, ARRAY_SIZE(clk_comm_wdt_sel), CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, 12, 1, 0, NULL, },
    {AX650X_CLK_COMM_TMR1_SEL, "clk_comm_tmr1_sel", clk_comm_tmr1_sel, ARRAY_SIZE(clk_comm_tmr1_sel), CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, 11, 1, 0, NULL, },
    {AX650X_CLK_COMM_TMR0_SEL, "clk_comm_tmr0_sel", clk_comm_tmr0_sel, ARRAY_SIZE(clk_comm_tmr0_sel), CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, 10, 1, 0, NULL, },
    {AX650X_CLK_COMM_RTC1_SEL, "clk_comm_rtc1_sel", clk_comm_rtc1_sel, ARRAY_SIZE(clk_comm_rtc1_sel), CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, 9, 1, 0, NULL, },
    {AX650X_CLK_COMM_RTC0_SEL, "clk_comm_rtc0_sel", clk_comm_rtc0_sel, ARRAY_SIZE(clk_comm_rtc0_sel), CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, 8, 1, 0, NULL, },
    {AX650X_CLK_COMM_IP_SEL, "clk_comm_ip_sel", clk_comm_ip_sel, ARRAY_SIZE(clk_comm_ip_sel), CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, 6, 2, 0, NULL, },
    {AX650X_ACLK_TOP_SEL, "aclk_top_sel", aclk_top_sel, ARRAY_SIZE(aclk_top_sel), CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, 3, 3, 0, NULL, },
    {AX650X_CLK_CE_BUS_SEL, "clk_ce_bus_sel", clk_ce_bus_sel, ARRAY_SIZE(clk_ce_bus_sel), CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x4, 24, 2, 0, NULL, },
    {AX650X_ACLK_TOP1_SEL, "aclk_top1_sel", aclk_top1_sel, ARRAY_SIZE(aclk_top1_sel), CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x4, 21, 3, 0, NULL, },
    {AX650X_ACLK_TOP2_SEL, "aclk_top2_sel", aclk_top2_sel, ARRAY_SIZE(aclk_top2_sel), CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x4, 18, 3, 0, NULL, },
    {AX650X_ACLK_TOP3_SEL, "aclk_top3_sel", aclk_top3_sel, ARRAY_SIZE(aclk_top3_sel), CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x4, 15, 3, 0, NULL, },
    {AX650X_PCLK_TOP_SEL, "pclk_top_sel", pclk_top_sel, ARRAY_SIZE(pclk_top_sel), CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x4, 12, 2, 0, NULL, },
    {AX650X_CLK_SEN_REF7_SEL, "clk_sen_ref7_sel", clk_sen_ref7_sel, ARRAY_SIZE(clk_sen_ref7_sel), CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x4, 10, 2, 0, NULL, },
    {AX650X_CLK_SEN_REF6_SEL, "clk_sen_ref6_sel", clk_sen_ref6_sel, ARRAY_SIZE(clk_sen_ref6_sel), CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x4, 8, 2, 0, NULL, },
    {AX650X_CLK_SEN_REF5_SEL, "clk_sen_ref5_sel", clk_sen_ref5_sel, ARRAY_SIZE(clk_sen_ref5_sel), CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x4, 6, 2, 0, NULL, },
    {AX650X_CLK_SEN_REF4_SEL, "clk_sen_ref4_sel", clk_sen_ref4_sel, ARRAY_SIZE(clk_sen_ref4_sel), CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x4, 4, 2, 0, NULL, },
    {AX650X_CLK_SEN_REF3_SEL, "clk_sen_ref3_sel", clk_sen_ref3_sel, ARRAY_SIZE(clk_sen_ref3_sel), CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x4, 2, 2, 0, NULL, },
    {AX650X_CLK_SEN_REF2_SEL, "clk_sen_ref2_sel", clk_sen_ref2_sel, ARRAY_SIZE(clk_sen_ref2_sel), CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x4, 0, 2, 0, NULL, },
};

static struct axera_gate_clock ax650x_gate_clks_comm[] __initdata = {
    {AX650X_CLK_SEN_REF7, "clk_sen_ref7", "clk_sen_ref7_divn", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 21, 0, 0x38, 21, 0x3C, 21, },
    {AX650X_CLK_SEN_REF6, "clk_sen_ref6", "clk_sen_ref6_divn", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 20, 0, 0x38, 20, 0x3C, 20, },
    {AX650X_CLK_SEN_REF5, "clk_sen_ref5", "clk_sen_ref5_divn", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 19, 0, 0x38, 19, 0x3C, 19, },
    {AX650X_CLK_SEN_REF4, "clk_sen_ref4", "clk_sen_ref4_divn", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 18, 0, 0x38, 18, 0x3C, 18, },
    {AX650X_CLK_SEN_REF3, "clk_sen_ref3", "clk_sen_ref3_divn", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 17, 0, 0x38, 17, 0x3C, 17, },
    {AX650X_CLK_SEN_REF2, "clk_sen_ref2", "clk_sen_ref2_divn", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 16, 0, 0x38, 16, 0x3C, 16, },
    {AX650X_CLK_SEN_REF1, "clk_sen_ref1", "clk_sen_ref1_divn", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 15, 0, 0x38, 15, 0x3C, 15, },
    {AX650X_CLK_SEN_REF0, "clk_sen_ref0", "clk_sen_ref0_divn", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 14, 0, 0x38, 14, 0x3C, 14, },
    {AX650X_CLK_I2S_REF3, "clk_i2s_ref3", "clk_i2s_ref3_divn", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 13, 0, 0x38, 13, 0x3C, 13, },
    {AX650X_CLK_I2S_REF2, "clk_i2s_ref2", "clk_i2s_ref2_divn", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 12, 0, 0x38, 12, 0x3C, 12, },
    {AX650X_CLK_I2S_REF1, "clk_i2s_ref1", "clk_i2s_ref1_divn", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 11, 0, 0x38, 11, 0x3C, 11, },
    {AX650X_CLK_I2S_REF0, "clk_i2s_ref0", "clk_i2s_ref0_divn", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 10, 0, 0x38, 10, 0x3C, 10, },
    {AX650X_CLK_EPHY_REF, "clk_ephy_ref", "clk_ephy_ref_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 9, 0, 0x38, 9, 0x3C, 9, },
    {AX650X_CLK_DEBUG_TLB, "clk_debug_tlb", "free_24m", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 8, 0, 0x38, 8, 0x3C, 8, },
    {AX650X_CLK_COMM_WDT, "clk_comm_wdt", "clk_comm_wdt_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 7, 0, 0x38, 7, 0x3C, 7, },
    {AX650X_CLK_COMM_TMR1, "clk_comm_tmr1", "clk_comm_tmr1_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 6, 0, 0x38, 6, 0x3C, 6, },
    {AX650X_CLK_COMM_TMR0, "clk_comm_tmr0", "clk_comm_tmr0_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 5, 0, 0x38, 5, 0x3C, 5, },
    {AX650X_CLK_COMM_RTC1, "clk_comm_rtc1", "clk_comm_rtc1_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 4, 0, 0x38, 4, 0x3C, 4, },
    {AX650X_CLK_COMM_RTC0, "clk_comm_rtc0", "clk_comm_rtc0_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 3, 0, 0x38, 3, 0x3C, 3, },
    {AX650X_CLK_COMM_ROSC, "clk_comm_rosc", "gpll_24m", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 2, 0, 0x38, 2, 0x3C, 2, },
    {AX650X_CLK_COMM_IP, "clk_comm_ip", "clk_comm_ip_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 1, 0, 0x38, 1, 0x3C, 1, },
    {AX650X_CLK_CE_CNT, "clk_ce_cnt", "gpll_24m", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 0, 0, 0x38, 0, 0x3C, 0, },
    {AX650X_PCLK_RTC0, "pclk_rtc0", "free_24m", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0xC, 31, 0, 0x40, 31, 0x44, 31, },
    {AX650X_PCLK_PUB_CE, "pclk_pub_ce", "pclk_top_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0xC, 30, 0, 0x40, 30, 0x44, 30, },
    {AX650X_PCLK_PLLC, "pclk_pllc", "free_24m", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0xC, 29, 0, 0x40, 29, 0x44, 29, },
    {AX650X_PCLK_MAILBOX, "pclk_mailbox", "pclk_top_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0xC, 28, 0, 0x40, 28, 0x44, 28, },
    {AX650X_PCLK_GPIO, "pclk_gpio", "clk_comm_wdt_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0xC, 27, 0, 0x40, 27, 0x44, 27, },
    {AX650X_PCLK_DFS_MNR, "pclk_dfs_mnr", "gpll_24m", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0xC, 26, 0, 0x40, 26, 0x44, 26, },
    {AX650X_PCLK_BUS_LOCK, "pclk_bus_lock", "pclk_top_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0xC, 25, 0, 0x40, 25, 0x44, 25, },
    {AX650X_PCLK_ADVFS1, "pclk_advfs1", "pclk_top_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0xC, 24, 0, 0x40, 24, 0x44, 24, },
    {AX650X_PCLK_ADVFS0, "pclk_advfs0", "pclk_top_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0xC, 23, 0, 0x40, 23, 0x44, 23, },
    {AX650X_CLK_WDT4, "clk_wdt4", "clk_comm_wdt", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0xC, 22, 0, 0x40, 22, 0x44, 22, },
    {AX650X_CLK_WDT3, "clk_wdt3", "clk_comm_wdt", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0xC, 21, 0, 0x40, 21, 0x44, 21, },
    {AX650X_CLK_WDT2, "clk_wdt2", "clk_comm_wdt", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0xC, 20, 0, 0x40, 20, 0x44, 20, },
    {AX650X_CLK_WDT1, "clk_wdt1", "clk_comm_wdt", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0xC, 19, 0, 0x40, 19, 0x44, 19, },
    {AX650X_CLK_WDT0, "clk_wdt0", "clk_comm_wdt", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0xC, 18, 0, 0x40, 18, 0x44, 18, },
    {AX650X_CLK_TMR64, "clk_tmr64", "clk_comm_wdt", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0xC, 17, 0, 0x40, 17, 0x44, 17, },
    {AX650X_CLK_TMR32_3, "clk_tmr32_3", "clk_comm_tmr1", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0xC, 16, 0, 0x40, 16, 0x44, 16, },
    {AX650X_CLK_TMR32_2, "clk_tmr32_2", "clk_comm_tmr1", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0xC, 15, 0, 0x40, 15, 0x44, 15, },
    {AX650X_CLK_TMR32_1, "clk_tmr32_1", "clk_comm_tmr0", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0xC, 14, 0, 0x40, 14, 0x44, 14, },
    {AX650X_CLK_TMR32_0, "clk_tmr32_0", "clk_comm_tmr0", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0xC, 13, 0, 0x40, 13, 0x44, 13, },
    {AX650X_CLK_THM, "clk_thm", "free_24m", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0xC, 12, 0, 0x40, 12, 0x44, 12, },
    {AX650X_CLK_RTC3, "clk_rtc3", "clk_comm_rtc0", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0xC, 11, 0, 0x40, 11, 0x44, 11, },
    {AX650X_CLK_RTC2, "clk_rtc2", "clk_comm_rtc0", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0xC, 10, 0, 0x40, 10, 0x44, 10, },
    {AX650X_CLK_RTC1, "clk_rtc1", "clk_comm_rtc0", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0xC, 9, 0, 0x40, 9, 0x44, 9, },
    {AX650X_CLK_RTC0, "clk_rtc0", "clk_comm_rtc0", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0xC, 8, 0, 0x40, 8, 0x44, 8, },
    {AX650X_CLK_RISCV, "clk_riscv", "clk_riscv_bus_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0xC, 7, 0, 0x40, 7, 0x44, 7, },
    {AX650X_CLK_PUB_CE, "clk_pub_ce", "gpll_24m", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0xC, 6, 0, 0x40, 6, 0x44, 6, },
    {AX650X_CLK_PUB_CE_CNT, "clk_pub_ce_cnt", "clk_ce_cnt", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0xC, 5, 0, 0x40, 5, 0x44, 5, },
    {AX650X_CLK_PINMUX, "clk_pinmux", "gpll_24m", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0xC, 4, 0, 0x40, 4, 0x44, 4, },
    {AX650X_CLK_GPIO, "clk_gpio", "clk_dbc_gpio_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0xC, 3, 0, 0x40, 3, 0x44, 3, },
    {AX650X_CLK_EFS, "clk_efs", "free_24m", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0xC, 2, 0, 0x40, 2, 0x44, 2, },
    {AX650X_CLK_ADVFS1, "clk_advfs1", "pclk_top_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0xC, 1, 0, 0x40, 1, 0x44, 1, },
    {AX650X_CLK_ADVFS0, "clk_advfs0", "pclk_top_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0xC, 0, 0, 0x40, 0, 0x44, 0, },
    {AX650X_PCLK_WDT4, "pclk_wdt4", "free_24m", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x10, 14, 0, 0x48, 14, 0x4C, 14, },
    {AX650X_PCLK_WDT3, "pclk_wdt3", "free_24m", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x10, 13, 0, 0x48, 13, 0x4C, 13, },
    {AX650X_PCLK_WDT2, "pclk_wdt2", "free_24m", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x10, 12, 0, 0x48, 12, 0x4C, 12, },
    {AX650X_PCLK_WDT1, "pclk_wdt1", "free_24m", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x10, 11, 0, 0x48, 11, 0x4C, 11, },
    {AX650X_PCLK_WDT0, "pclk_wdt0", "free_24m", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x10, 10, 0, 0x48, 10, 0x4C, 10, },
    {AX650X_PCLK_TMR64, "pclk_tmr64", "free_24m", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x10, 9, 0, 0x48, 9, 0x4C, 9, },
    {AX650X_PCLK_TMR32_3, "pclk_tmr32_3", "free_24m", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x10, 8, 0, 0x48, 8, 0x4C, 8, },
    {AX650X_PCLK_TMR32_2, "pclk_tmr32_2", "free_24m", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x10, 7, 0, 0x48, 7, 0x4C, 7, },
    {AX650X_PCLK_TMR32_1, "pclk_tmr32_1", "free_24m", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x10, 6, 0, 0x48, 6, 0x4C, 6, },
    {AX650X_PCLK_TMR32_0, "pclk_tmr32_0", "free_24m", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x10, 5, 0, 0x48, 5, 0x4C, 5, },
    {AX650X_PCLK_THM, "pclk_thm", "free_24m", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x10, 4, 0, 0x48, 4, 0x4C, 4, },
    {AX650X_PCLK_SPINLOCK, "pclk_spinlock", "pclk_top_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x10, 3, 0, 0x48, 3, 0x4C, 3, },
    {AX650X_PCLK_RTC3, "pclk_rtc3", "free_24m", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x10, 2, 0, 0x48, 2, 0x4C, 2, },
    {AX650X_PCLK_RTC2, "pclk_rtc2", "free_24m", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x10, 1, 0, 0x48, 1, 0x4C, 1, },
    {AX650X_PCLK_RTC1, "pclk_rtc1", "free_24m", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x10, 0, 0, 0x48, 0, 0x4C, 0, },
};

static struct axera_divider_clock ax650x_div_clks_comm[] __initdata = {
    {AX650X_CLK_SEN_REF1_DIVN, "clk_sen_ref1_divn", "clk_sen_ref1_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x14, 0x14, 29, 25, 4, 0, NULL, },
    {AX650X_CLK_SEN_REF0_DIVN, "clk_sen_ref0_divn", "clk_sen_ref0_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x14, 0x14, 24, 20, 4, 0, NULL, },
    {AX650X_CLK_I2S_REF3_DIVN, "clk_i2s_ref3_divn", "clk_i2s_ref3_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x14, 0x14, 19, 15, 4, 0, NULL, },
    {AX650X_CLK_I2S_REF2_DIVN, "clk_i2s_ref2_divn", "clk_i2s_ref2_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x14, 0x14, 14, 10, 4, 0, NULL, },
    {AX650X_CLK_I2S_REF1_DIVN, "clk_i2s_ref1_divn", "clk_i2s_ref1_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x14, 0x14, 9, 5, 4, 0, NULL, },
    {AX650X_CLK_I2S_REF0_DIVN, "clk_i2s_ref0_divn", "clk_i2s_ref0_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x14, 0x14, 4, 0, 4, 0, NULL, },
    {AX650X_CLK_SEN_REF7_DIVN, "clk_sen_ref7_divn", "clk_sen_ref7_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x18, 0x18, 29, 25, 4, 0, NULL, },
    {AX650X_CLK_SEN_REF6_DIVN, "clk_sen_ref6_divn", "clk_sen_ref6_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x18, 0x18, 24, 20, 4, 0, NULL, },
    {AX650X_CLK_SEN_REF5_DIVN, "clk_sen_ref5_divn", "clk_sen_ref5_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x18, 0x18, 19, 15, 4, 0, NULL, },
    {AX650X_CLK_SEN_REF4_DIVN, "clk_sen_ref4_divn", "clk_sen_ref4_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x18, 0x18, 14, 10, 4, 0, NULL, },
    {AX650X_CLK_SEN_REF3_DIVN, "clk_sen_ref3_divn", "clk_sen_ref3_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x18, 0x18, 9, 5, 4, 0, NULL, },
    {AX650X_CLK_SEN_REF2_DIVN, "clk_sen_ref2_divn", "clk_sen_ref2_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x18, 0x18, 4, 0, 4, 0, NULL, },
};

static void __init ax650x_clk_comm_clk_init(struct device_node *np)
{
    struct axera_clock_data *clk_data_top;
    clk_data_top = axera_clk_init(np, AX650X_COMM_NR_CLKS);
    if (!clk_data_top)
        return;

    axera_clk_register_fixed_rate(ax650x_fixed_rate_clks, ARRAY_SIZE(ax650x_fixed_rate_clks), clk_data_top);
    axera_clk_register_fixed_factor(ax650x_fixed_factor_clks, ARRAY_SIZE(ax650x_fixed_factor_clks), clk_data_top);

    axera_clk_register_mux(ax650x_mux_clks_comm, ARRAY_SIZE(ax650x_mux_clks_comm), clk_data_top);
    axera_clk_register_gate(ax650x_gate_clks_comm, ARRAY_SIZE(ax650x_gate_clks_comm), clk_data_top);
    axera_clk_register_divider(ax650x_div_clks_comm, ARRAY_SIZE(ax650x_div_clks_comm), clk_data_top);
}

CLK_OF_DECLARE(ax650x_clk_comm_glb, "axera,ax650x-common-clk", ax650x_clk_comm_clk_init);
/*cpu*/
static const char *pclk_sel[] __initconst = { "gpll_24m", "epll_750m" };
static const char *gicclk_sel[] __initconst = { "gpll_24m", "epll_750m" };
static const char *clk_dsu_sel[] __initconst = { "gpll_24m", "epll_750m", "cpll1_1100m" };
static const char *clk_dsu_periph_sel[] __initconst = { "gpll_24m", "epll_750m" };
static const char *clk_cpu7_sel[] __initconst = { "gpll_24m", "epll_750m", "mpll_1400m", "epll_1500m", "cpll0_1600m" };
static const char *clk_cpu6_sel[] __initconst = { "gpll_24m", "epll_750m", "mpll_1400m", "epll_1500m", "cpll0_1600m" };
static const char *clk_cpu5_sel[] __initconst = { "gpll_24m", "epll_750m", "mpll_1400m", "epll_1500m", "cpll0_1600m" };
static const char *clk_cpu4_sel[] __initconst = { "gpll_24m", "epll_750m", "mpll_1400m", "epll_1500m", "cpll0_1600m" };
static const char *clk_cpu3_sel[] __initconst = { "gpll_24m", "epll_750m", "mpll_1400m", "epll_1500m", "cpll0_1600m" };
static const char *clk_cpu2_sel[] __initconst = { "gpll_24m", "epll_750m", "mpll_1400m", "epll_1500m", "cpll0_1600m" };
static const char *clk_cpu1_sel[] __initconst = { "gpll_24m", "epll_750m", "mpll_1400m", "epll_1500m", "cpll0_1600m" };
static const char *clk_cpu0_sel[] __initconst = { "gpll_24m", "epll_750m", "mpll_1400m", "epll_1500m", "cpll0_1600m" };
static const char *atclk_sel[] __initconst = { "gpll_24m", "epll_750m" };

static struct axera_mux_clock ax650x_mux_clks_cpu[] __initdata = {
    {AX650X_PCLK_SEL, "pclk_sel", pclk_sel, ARRAY_SIZE(pclk_sel), CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, 29, 1, 0, NULL, },
    {AX650X_GICCLK_SEL, "gicclk_sel", gicclk_sel, ARRAY_SIZE(gicclk_sel), CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, 28, 1, 0, NULL, },
    {AX650X_CLK_DSU_SEL, "clk_dsu_sel", clk_dsu_sel, ARRAY_SIZE(clk_dsu_sel), CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, 26, 2, 0, NULL, },
    {AX650X_CLK_DSU_PERIPH_SEL, "clk_dsu_periph_sel", clk_dsu_periph_sel, ARRAY_SIZE(clk_dsu_periph_sel), CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, 25, 1, 0, NULL, },
    {AX650X_CLK_CPU7_SEL, "clk_cpu7_sel", clk_cpu7_sel, ARRAY_SIZE(clk_cpu7_sel), CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, 22, 3, 0, NULL, },
    {AX650X_CLK_CPU6_SEL, "clk_cpu6_sel", clk_cpu6_sel, ARRAY_SIZE(clk_cpu6_sel), CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, 19, 3, 0, NULL, },
    {AX650X_CLK_CPU5_SEL, "clk_cpu5_sel", clk_cpu5_sel, ARRAY_SIZE(clk_cpu5_sel), CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, 16, 3, 0, NULL, },
    {AX650X_CLK_CPU4_SEL, "clk_cpu4_sel", clk_cpu4_sel, ARRAY_SIZE(clk_cpu4_sel), CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, 13, 3, 0, NULL, },
    {AX650X_CLK_CPU3_SEL, "clk_cpu3_sel", clk_cpu3_sel, ARRAY_SIZE(clk_cpu3_sel), CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, 10, 3, 0, NULL, },
    {AX650X_CLK_CPU2_SEL, "clk_cpu2_sel", clk_cpu2_sel, ARRAY_SIZE(clk_cpu2_sel), CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, 7, 3, 0, NULL, },
    {AX650X_CLK_CPU1_SEL, "clk_cpu1_sel", clk_cpu1_sel, ARRAY_SIZE(clk_cpu1_sel), CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, 4, 3, 0, NULL, },
    {AX650X_CLK_CPU0_SEL, "clk_cpu0_sel", clk_cpu0_sel, ARRAY_SIZE(clk_cpu0_sel), CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, 1, 3, 0, NULL, },
    {AX650X_ATCLK_SEL, "atclk_sel", atclk_sel, ARRAY_SIZE(atclk_sel), CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, 0, 1, 0, NULL, },
};

static struct axera_gate_clock ax650x_gate_clks_cpu[] __initdata = {
    {AX650X_PCLK, "pclk", "pclk_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x4, 4, 0, 0x18, 4, 0x1C, 4, },
    {AX650X_GICCLK, "gicclk", "gicclk_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x4, 3, 0, 0x18, 3, 0x1C, 3, },
    {AX650X_CLK_DSU_PERIPH, "clk_dsu_periph", "clk_dsu_periph_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x4, 2, 0, 0x18, 2, 0x1C, 2, },
    {AX650X_CLK_CPU_ROSC, "clk_cpu_rosc", "gpll_24m", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x4, 1, 0, 0x18, 1, 0x1C, 1, },
    {AX650X_ATCLK, "atclk", "atclk_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x4, 0, 0, 0x18, 0, 0x1C, 0, },
    {AX650X_CLK_DSU_CG, "clk_dsu_cg", "clk_dsu_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 0, 0, 0x20, 0, 0x24, 0, },
};

static void __init ax650x_clk_cpu_clk_init(struct device_node *np)
{
    struct axera_clock_data *clk_data_top;
    clk_data_top = axera_clk_init(np, AX650X_CPU_NR_CLKS);
    if (!clk_data_top)
        return;

    axera_clk_register_mux(ax650x_mux_clks_cpu, ARRAY_SIZE(ax650x_mux_clks_cpu), clk_data_top);
    axera_clk_register_gate(ax650x_gate_clks_cpu, ARRAY_SIZE(ax650x_gate_clks_cpu), clk_data_top);

}

CLK_OF_DECLARE(ax650x_clk_cpu_glb, "axera,ax650x-cpu-clk", ax650x_clk_cpu_clk_init);
/*ddr*/
static const char *clk_ddr_sel[] __initconst = { "gpll_24m", "npll_200m", "gpll_624m", "npll_800m", "dpll_1066m" };
static const char *clk_ddr_glb_sel[] __initconst = { "gpll_24m", "epll_125m", "npll_200m" };

static struct axera_mux_clock ax650x_mux_clks_ddr[] __initdata = {
    {AX650X_CLK_DDR_SEL, "clk_ddr_sel", clk_ddr_sel, ARRAY_SIZE(clk_ddr_sel), CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, 2, 3, 0, NULL, },
    {AX650X_CLK_DDR_GLB_SEL, "clk_ddr_glb_sel", clk_ddr_glb_sel, ARRAY_SIZE(clk_ddr_glb_sel), CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, 0, 2, 0, NULL, },
};

static struct axera_gate_clock ax650x_gate_clks_ddr[] __initdata = {
    {AX650X_CLK_DDR_ROSC, "clk_ddr_rosc", "gpll_24m", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x4, 2, 0, 0x20, 2, 0x24, 2, },
    {AX650X_CLK_DDR_ORG, "clk_ddr_org", "clk_ddr_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x4, 1, 0, 0x20, 1, 0x24, 1, },
    {AX650X_CLK_DDR_BW_MNR, "clk_ddr_bw_mnr", "clk_ddr_glb_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x4, 0, 0, 0x20, 0, 0x24, 0, },
    {AX650X_PCLK_PERF_MNR4, "pclk_perf_mnr4", "clk_ddr_glb_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 8, 0, 0x28, 8, 0x2C, 8, },
    {AX650X_PCLK_PERF_MNR3, "pclk_perf_mnr3", "clk_ddr_glb_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 7, 0, 0x28, 7, 0x2C, 7, },
    {AX650X_PCLK_PERF_MNR2, "pclk_perf_mnr2", "clk_ddr_glb_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 6, 0, 0x28, 6, 0x2C, 6, },
    {AX650X_PCLK_PERF_MNR1, "pclk_perf_mnr1", "clk_ddr_glb_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 5, 0, 0x28, 5, 0x2C, 5, },
    {AX650X_PCLK_PERF_MNR0, "pclk_perf_mnr0", "clk_ddr_glb_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 4, 0, 0x28, 4, 0x2C, 4, },
    {AX650X_PCLK_DDR_PHY, "pclk_ddr_phy", "clk_ddr_glb_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 3, 0, 0x28, 3, 0x2C, 3, },
    {AX650X_PCLK_DDR, "pclk_ddr", "clk_ddr_glb_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 2, 0, 0x28, 2, 0x2C, 2, },
    {AX650X_DFICLK_DDR, "dficlk_ddr", "clk_ddr_divn", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 1, 0, 0x28, 1, 0x2C, 1, },
    {AX650X_CLK_DDR_CORE, "clk_ddr_core", "clk_ddr_divn", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 0, 0, 0x28, 0, 0x2C, 0, },
};

static struct axera_divider_clock ax650x_div_clks_ddr[] __initdata = {
    {AX650X_CLK_DDR_DIVN, "clk_ddr_divn", "clk_ddr_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0xC, 0xC, 4, 0, 4, 0, NULL, },
};

static void __init ax650x_clk_ddr_clk_init(struct device_node *np)
{
    struct axera_clock_data *clk_data_top;
    clk_data_top = axera_clk_init(np, AX650X_DDR_NR_CLKS);
    if (!clk_data_top)
        return;

    axera_clk_register_mux(ax650x_mux_clks_ddr, ARRAY_SIZE(ax650x_mux_clks_ddr), clk_data_top);
    axera_clk_register_gate(ax650x_gate_clks_ddr, ARRAY_SIZE(ax650x_gate_clks_ddr), clk_data_top);
    axera_clk_register_divider(ax650x_div_clks_ddr, ARRAY_SIZE(ax650x_div_clks_ddr), clk_data_top);

}

CLK_OF_DECLARE(ax650x_clk_ddr_glb, "axera,ax650x-ddr-clk", ax650x_clk_ddr_clk_init);
/*flash*/
static const char *clk_flash_ser_sel[] __initconst = { "gpll_24m", "epll_125m", "npll_200m" };
static const char *clk_flash_glb_sel[] __initconst = { "gpll_24m", "npll_200m", "mpll_350m" };
static const char *clk_emmc_lbt_sel[] __initconst = { "xtal_32k", "epll_5m" };
static const char *cclk_tx_fls_sdio_sel[] __initconst = { "gpll_24m_div2", "epll_250m_div2", "npll_400m_div2" };   ///*"gpll_416m"*/revised name
static const char *cclk_tx_emmc_sel[] __initconst = { "gpll_24m_div2", "epll_250m_div2", "npll_400m_div2" };

static struct axera_mux_clock ax650x_mux_clks_flash[] __initdata = {
    {AX650X_CLK_FLASH_SER_SEL, "clk_flash_ser_sel", clk_flash_ser_sel, ARRAY_SIZE(clk_flash_ser_sel), CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, 7, 2, 0, NULL, },
    {AX650X_CLK_FLASH_GLB_SEL, "clk_flash_glb_sel", clk_flash_glb_sel, ARRAY_SIZE(clk_flash_glb_sel), CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, 5, 2, 0, NULL, },
    {AX650X_CLK_EMMC_LBT_SEL, "clk_emmc_lbt_sel", clk_emmc_lbt_sel, ARRAY_SIZE(clk_emmc_lbt_sel), CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, 4, 1, 0, NULL, },
    {AX650X_CCLK_TX_FLS_SDIO_SEL, "cclk_tx_fls_sdio_sel", cclk_tx_fls_sdio_sel, ARRAY_SIZE(cclk_tx_fls_sdio_sel), CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, 2, 2, 0, NULL, },
    {AX650X_CCLK_TX_EMMC_SEL, "cclk_tx_emmc_sel", cclk_tx_emmc_sel, ARRAY_SIZE(cclk_tx_emmc_sel), CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, 0, 2, 0, NULL, },
};

static struct axera_gate_clock ax650x_gate_clks_flash[] __initdata = {
    {AX650X_USB2_REF_ALT_CLK, "usb2_ref_alt_clk", "gpll_12m", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x4, 10, 0, 0x20, 10, 0x24, 10, },
    {AX650X_TCLK_FLS_SDIO, "tclk_fls_sdio", "gpll_24m", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x4, 9, 0, 0x20, 9, 0x24, 9, },
    {AX650X_TCLK_EMMC, "tclk_emmc", "gpll_24m", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x4, 8, 0, 0x20, 8, 0x24, 8, },
    {AX650X_CLK_USB2_REF, "clk_usb2_ref", "gpll_24m", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x4, 7, 0, 0x20, 7, 0x24, 7, },
    {AX650X_CLK_GTMR, "clk_gtmr", "gpll_24m", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x4, 6, 0, 0x20, 6, 0x24, 6, },
    {AX650X_CLK_FLASH_SER, "clk_flash_ser", "clk_flash_ser_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x4, 5, 0, 0x20, 5, 0x24, 5, },
    {AX650X_CLK_EMMC_LBT, "clk_emmc_lbt", "clk_emmc_lbt_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x4, 4, 0, 0x20, 4, 0x24, 4, },
    {AX650X_CCLK_TX_FLS_SDIO, "cclk_tx_fls_sdio", "cclk_tx_fls_sdio_divn", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x4, 3, 0, 0x20, 3, 0x24, 3, },
    {AX650X_CCLK_TX_EMMC, "cclk_tx_emmc", "cclk_tx_emmc_divn", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x4, 2, 0, 0x20, 2, 0x24, 2, },
    {AX650X_BCLK_FLS_SDIO, "bclk_fls_sdio", "epll_300m", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x4, 1, 0, 0x20, 1, 0x24, 1, },
    {AX650X_BCLK_EMMC, "bclk_emmc", "epll_300m", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x4, 0, 0, 0x20, 0, 0x24, 0, },
    {AX650X_TMCLK_FLS_SDIO, "tmclk_fls_sdio", "gpll_24m", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 14, 0, 0x28, 14, 0x2C, 14, },
    {AX650X_TMCLK_EMMC, "tmclk_emmc", "gpll_24m", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 13, 0, 0x28, 13, 0x2C, 13, },
    {AX650X_PCLK_USB2_PHY, "pclk_usb2_phy", "clk_flash_glb_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 12, 0, 0x28, 12, 0x2C, 12, },
    {AX650X_PCLK_SPI, "pclk_spi", "clk_flash_glb_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 11, 0, 0x28, 11, 0x2C, 11, },
    {AX650X_HCLK_SPI, "hclk_spi", "clk_flash_glb_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 10, 0, 0x28, 10, 0x2C, 10, },
    {AX650X_HCLK_FLS_SDIO, "hclk_fls_sdio", "clk_flash_glb_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 9, 0, 0x28, 9, 0x2C, 9, },
    {AX650X_HCLK_EMMC, "hclk_emmc", "clk_flash_glb_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 8, 0, 0x28, 8, 0x2C, 8, },
    {AX650X_CQETMCLK_FLS_SDIO, "cqetmclk_fls_sdio", "gpll_24m", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 7, 0, 0x28, 7, 0x2C, 7, },
    {AX650X_CQETMCLK_EMMC, "cqetmclk_emmc", "gpll_24m", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 6, 0, 0x28, 6, 0x2C, 6, },
    {AX650X_CLK_P_SPI, "clk_p_spi", "clk_flash_ser", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 5, 0, 0x28, 5, 0x2C, 5, },
    {AX650X_CLK_H_SPI, "clk_h_spi", "clk_flash_ser", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 4, 0, 0x28, 4, 0x2C, 4, },
    {AX650X_CLK_DMA, "clk_dma", "clk_flash_glb_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 3, 0, 0x28, 3, 0x2C, 3, },
    {AX650X_BUS_CLK_USB2, "bus_clk_usb2", "clk_flash_glb_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 2, 0, 0x28, 2, 0x2C, 2, },
    {AX650X_ACLK_FLS_SDIO, "aclk_fls_sdio", "clk_flash_glb_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 1, 0, 0x28, 1, 0x2C, 1, },
    {AX650X_ACLK_EMMC, "aclk_emmc", "clk_flash_glb_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 0, 0, 0x28, 0, 0x2C, 0, },
};

static struct axera_divider_clock ax650x_div_clks_flash[] __initdata = {
    {AX650X_CCLK_TX_FLS_SDIO_DIVN, "cclk_tx_fls_sdio_divn", "cclk_tx_fls_sdio_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0xC, 0xC, 11, 6, 5, 0, NULL, },
    {AX650X_CCLK_TX_EMMC_DIVN, "cclk_tx_emmc_divn", "cclk_tx_emmc_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0xC, 0xC, 5, 0, 5, 0, NULL, },
};

static void __init ax650x_clk_flash_clk_init(struct device_node *np)
{
    struct axera_clock_data *clk_data_top;
    clk_data_top = axera_clk_init(np, AX650X_FLASH_NR_CLKS);
    if (!clk_data_top)
        return;

    axera_clk_register_mux(ax650x_mux_clks_flash, ARRAY_SIZE(ax650x_mux_clks_flash), clk_data_top);
    axera_clk_register_gate(ax650x_gate_clks_flash, ARRAY_SIZE(ax650x_gate_clks_flash), clk_data_top);
    axera_clk_register_divider(ax650x_div_clks_flash, ARRAY_SIZE(ax650x_div_clks_flash), clk_data_top);
}

CLK_OF_DECLARE(ax650x_clk_flash_glb, "axera,ax650x-flash-clk", ax650x_clk_flash_clk_init);
/*isp*/
static const char *clk_isp_ls_sel[] __initconst = { "epll_250m", "npll_400m", "epll_500m_120", "gpll_624m_180" };
static const char *clk_isp_hs_sel[] __initconst = { "epll_250m", "epll_500m_120", "gpll_624m_180", "mpll_700m" };
static const char *clk_isp_glb_sel[] __initconst = { "epll_187p5m", "npll_200m", "gpll_208m" };
static const char *clk_isp_dpu_out_sel[] __initconst = { "vpll1_562p5m", "vpll0_594m" };
static const char *clk_isp_bus_sel[] __initconst = { "npll_400m", "epll_500m_120", "gpll_624m_180", "npll_800m" };

static struct axera_mux_clock ax650x_mux_clks_isp[] __initdata = {
    {AX650X_CLK_ISP_LS_SEL, "clk_isp_ls_sel", clk_isp_ls_sel, ARRAY_SIZE(clk_isp_ls_sel), CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, 7, 2, 0, NULL, },
    {AX650X_CLK_ISP_HS_SEL, "clk_isp_hs_sel", clk_isp_hs_sel, ARRAY_SIZE(clk_isp_hs_sel), CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, 5, 2, 0, NULL, },
    {AX650X_CLK_ISP_GLB_SEL, "clk_isp_glb_sel", clk_isp_glb_sel, ARRAY_SIZE(clk_isp_glb_sel), CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, 3, 2, 0, NULL, },
    {AX650X_CLK_ISP_DPU_OUT_SEL, "clk_isp_dpu_out_sel", clk_isp_dpu_out_sel, ARRAY_SIZE(clk_isp_dpu_out_sel), CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, 2, 1, 0, NULL, },
    {AX650X_CLK_ISP_BUS_SEL, "clk_isp_bus_sel", clk_isp_bus_sel, ARRAY_SIZE(clk_isp_bus_sel), CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, 0, 2, 0, NULL, },
};

static struct axera_gate_clock ax650x_gate_clks_isp[] __initdata = {
    {AX650X_CLK_ISP_ROSC, "clk_isp_rosc", "gpll_24m", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x4, 2, 0, 0x24, 2, 0x28, 2, },
    {AX650X_CLK_ISP_ITS, "clk_isp_its", "epll_100m", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x4, 1, 0, 0x24, 1, 0x28, 1, },
    {AX650X_CLK_ISP_DPU_OUT, "clk_isp_dpu_out", "clk_isp_dpu_out_divn", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x4, 0, 0, 0x24, 0, 0x28, 0, },
    {AX650X_CLK_ISP_YUV, "clk_isp_yuv", "clk_isp_hs_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 4, 0, 0x2C, 4, 0x30, 4, },
    {AX650X_CLK_ISP_ITP, "clk_isp_itp", "clk_isp_hs_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 3, 0, 0x2C, 3, 0x30, 3, },
    {AX650X_CLK_ISP_IFE, "clk_isp_ife", "clk_isp_ls_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 2, 0, 0x2C, 2, 0x30, 2, },
    {AX650X_CLK_ISP_DPU, "clk_isp_dpu", "clk_isp_ls_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 1, 0, 0x2C, 1, 0x30, 1, },
    {AX650X_CLK_ISP_AXIM, "clk_isp_axim", "clk_isp_bus_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 0, 0, 0x2C, 0, 0x30, 0, },
};

static struct axera_divider_clock ax650x_div_clks_isp[] __initdata = {
    {AX650X_CLK_ISP_DPU_OUT_DIVN, "clk_isp_dpu_out_divn", "clk_isp_dpu_out_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0xC, 0xC, 4, 0, 4, 0, NULL, },
};

static void __init ax650x_clk_isp_clk_init(struct device_node *np)
{
    struct axera_clock_data *clk_data_top;
    clk_data_top = axera_clk_init(np, AX650X_ISP_NR_CLKS);
    if (!clk_data_top)
        return;

    axera_clk_register_mux(ax650x_mux_clks_isp, ARRAY_SIZE(ax650x_mux_clks_isp), clk_data_top);
    axera_clk_register_gate(ax650x_gate_clks_isp, ARRAY_SIZE(ax650x_gate_clks_isp), clk_data_top);
    axera_clk_register_divider(ax650x_div_clks_isp, ARRAY_SIZE(ax650x_div_clks_isp), clk_data_top);
}

CLK_OF_DECLARE(ax650x_clk_isp_glb, "axera,ax650x-isp-clk", ax650x_clk_isp_clk_init);
/*mm*/
static u32 clk_mm_dpu_out0_mux_table[] = {0x0, 0x1, 0x2, 0x4};
static u32 clk_mm_dpu_out1_mux_table[] = {0x0, 0x1, 0x2, 0x3};

static const char *sclk_mm_i2s1_sel[] __initconst = { "apll1_4096k", "apll0_4096k", "apll1_12p288m", "apll0_12p288m", "apll1_20p48m", "apll0_20p48m" };
static const char *sclk_mm_i2s0_sel[] __initconst = { "apll1_4096k", "apll0_4096k", "apll1_12p288m", "apll0_12p288m", "apll1_20p48m", "apll0_20p48m" };
static const char *clk_mm_rgmii_tx_sel[] __initconst = { "epll_5m", "epll_50m", "epll_250m" };
static const char *clk_mm_glb_sel[] __initconst = { "gpll_24m", "epll_75m", "epll_100m" };
static const char *clk_mm_dpu_out1_sel[] __initconst = { "vpll1_108m", "vpll1_118p8m", "vpll1_198m", "vpll1_594m" };
static const char *clk_mm_dpu_out0_sel[] __initconst = { "vpll0_108m", "vpll0_118p8m", "vpll0_198m", "vpll0_594m" };
static const char *clk_mm_core_sel[] __initconst = { "npll_400m", "epll_500m_180", "gpll_624m_270", "mpll_700m_180" };
static const char *clk_mm_bus_sel[] __initconst = { "npll_400m", "epll_500m_180", "gpll_624m_270" };

static struct axera_mux_clock ax650x_mux_clks_mm[] __initdata = {
    {AX650X_SCLK_MM_I2S1_SEL, "sclk_mm_i2s1_sel", sclk_mm_i2s1_sel, ARRAY_SIZE(sclk_mm_i2s1_sel), CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, 17, 3, 0, NULL, },
    {AX650X_SCLK_MM_I2S0_SEL, "sclk_mm_i2s0_sel", sclk_mm_i2s0_sel, ARRAY_SIZE(sclk_mm_i2s0_sel), CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, 14, 3, 0, NULL, },
    {AX650X_CLK_MM_RGMII_TX_SEL, "clk_mm_rgmii_tx_sel", clk_mm_rgmii_tx_sel, ARRAY_SIZE(clk_mm_rgmii_tx_sel), CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, 12, 2, 0, NULL, },
    {AX650X_CLK_MM_GLB_SEL, "clk_mm_glb_sel", clk_mm_glb_sel, ARRAY_SIZE(clk_mm_glb_sel), CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, 10, 2, 0, NULL, },
    {AX650X_CLK_MM_DPU_OUT1_SEL, "clk_mm_dpu_out1_sel", clk_mm_dpu_out1_sel, ARRAY_SIZE(clk_mm_dpu_out1_sel), CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, 7, 3, 0, clk_mm_dpu_out1_mux_table, },
    {AX650X_CLK_MM_DPU_OUT0_SEL, "clk_mm_dpu_out0_sel", clk_mm_dpu_out0_sel, ARRAY_SIZE(clk_mm_dpu_out0_sel), CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, 4, 3, 0, clk_mm_dpu_out0_mux_table, },
    {AX650X_CLK_MM_CORE_SEL, "clk_mm_core_sel", clk_mm_core_sel, ARRAY_SIZE(clk_mm_core_sel), CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, 2, 2, 0, NULL, },
    {AX650X_CLK_MM_BUS_SEL, "clk_mm_bus_sel", clk_mm_bus_sel, ARRAY_SIZE(clk_mm_bus_sel), CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, 0, 2, 0, NULL, },
};

static struct axera_gate_clock ax650x_gate_clks_mm[] __initdata = {
    {AX650X_SCLK_MM_I2S1, "sclk_mm_i2s1", "sclk_mm_i2s1_divn", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x4, 10, 0, 0x20, 10, 0x24, 10, },
    {AX650X_SCLK_MM_I2S0, "sclk_mm_i2s0", "sclk_mm_i2s0_divn", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x4, 9, 0, 0x20, 9, 0x24, 9, },
    {AX650X_CLK_MM_RMII_RX, "clk_mm_rmii_rx", "clk_mm_rmii_rx_divn", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x4, 8, 0, 0x20, 8, 0x24, 8, },
    {AX650X_CLK_MM_RMII_PHY, "clk_mm_rmii_phy", "epll_50m", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x4, 7, 0, 0x20, 7, 0x24, 7, },
    {AX650X_CLK_MM_RGMII_TX, "clk_mm_rgmii_tx", "clk_mm_rgmii_tx_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x4, 6, 0, 0x20, 6, 0x24, 6, },
    {AX650X_CLK_MM_ISFR, "clk_mm_isfr", "gpll_24m", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x4, 5, 0, 0x20, 5, 0x24, 5, },
    {AX650X_CLK_MM_EMAC_PTP, "clk_mm_emac_ptp", "epll_50m", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x4, 4, 0, 0x20, 4, 0x24, 4, },
    {AX650X_CLK_MM_DPU_OUT1, "clk_mm_dpu_out1", "clk_mm_dpu_out1_divn", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x4, 3, 0, 0x20, 3, 0x24, 3, },
    {AX650X_CLK_MM_DPU_OUT0, "clk_mm_dpu_out0", "clk_mm_dpu_out0_divn", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x4, 2, 0, 0x20, 2, 0x24, 2, },
    {AX650X_CLK_HDMI_32K, "clk_hdmi_32k", "xtal_32k", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x4, 1, 0, 0x20, 1, 0x24, 1, },
    {AX650X_CLK_CSI_ASYNC, "clk_csi_async", "mpll_350m", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x4, 0, 0, 0x20, 0, 0x24, 0, },
    {AX650X_PCLK_I2S1, "pclk_i2s1", "clk_mm_glb_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 16, 0, 0x28, 16, 0x2C, 16, },
    {AX650X_PCLK_I2S0, "pclk_i2s0", "clk_mm_glb_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 15, 0, 0x28, 15, 0x2C, 15, },
    {AX650X_PCLK_HDMI1, "pclk_hdmi1", "clk_mm_glb_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 14, 0, 0x28, 14, 0x2C, 14, },
    {AX650X_PCLK_HDMI0, "pclk_hdmi0", "clk_mm_glb_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 13, 0, 0x28, 13, 0x2C, 13, },
    {AX650X_PCLK_DSITX, "pclk_dsitx", "clk_mm_glb_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 12, 0, 0x28, 12, 0x2C, 12, },
    {AX650X_PCLK_CSITX, "pclk_csitx", "clk_mm_glb_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 11, 0, 0x28, 11, 0x2C, 11, },
    {AX650X_CLK_JENC, "clk_jenc", "clk_mm_bus_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 10, 0, 0x28, 10, 0x2C, 10, },
    {AX650X_CLK_JDEC, "clk_jdec", "clk_mm_bus_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 9, 0, 0x28, 9, 0x2C, 9, },
    {AX650X_CLK_HDMI_VIDEO1, "clk_hdmi_video1", "clk_mm_dpu_out1", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 8, 0, 0x28, 8, 0x2C, 8, },
    {AX650X_CLK_HDMI_VIDEO0, "clk_hdmi_video0", "clk_mm_dpu_out0", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 7, 0, 0x28, 7, 0x2C, 7, },
    {AX650X_CLK_HDMI1_ISFR, "clk_hdmi1_isfr", "clk_mm_glb_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 6, 0, 0x28, 6, 0x2C, 6, },
    {AX650X_CLK_HDMI0_ISFR, "clk_hdmi0_isfr", "clk_mm_glb_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 5, 0, 0x28, 5, 0x2C, 5, },
    {AX650X_CLK_DPU1, "clk_dpu1", "clk_mm_bus_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 4, 0, 0x28, 4, 0x2C, 4, },
    {AX650X_CLK_DPU0, "clk_dpu0", "clk_mm_bus_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 3, 0, 0x28, 3, 0x2C, 3, },
    {AX650X_CLK_CMD, "clk_cmd", "clk_mm_bus_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 2, 0, 0x28, 2, 0x2C, 2, },
    {AX650X_CLK_AXI2CSI, "clk_axi2csi", "clk_mm_bus_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 1, 0, 0x28, 1, 0x2C, 1, },
    {AX650X_ACLK_MM_EMAC, "aclk_mm_emac", "clk_mm_bus_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 0, 0, 0x28, 0, 0x2C, 0, },
};

static struct axera_divider_clock ax650x_div_clks_mm[] __initdata = {
    {AX650X_SCLK_MM_I2S1_DIVN, "sclk_mm_i2s1_divn", "sclk_mm_i2s1_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0xC, 0xC, 24, 20, 4, 0, NULL, },
    {AX650X_SCLK_MM_I2S0_DIVN, "sclk_mm_i2s0_divn", "sclk_mm_i2s0_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0xC, 0xC, 19, 15, 4, 0, NULL, },
    {AX650X_CLK_MM_RMII_RX_DIVN, "clk_mm_rmii_rx_divn", "xtal_32k", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0xC, 0xC, 14, 10, 4, 0, NULL, },
    {AX650X_CLK_MM_DPU_OUT1_DIVN, "clk_mm_dpu_out1_divn", "clk_mm_dpu_out1_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0xC, 0xC, 9, 5, 4, 0, NULL, },
    {AX650X_CLK_MM_DPU_OUT0_DIVN, "clk_mm_dpu_out0_divn", "clk_mm_dpu_out0_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0xC, 0xC, 4, 0, 4, 0, NULL, },
};

static void __init ax650x_clk_mm_clk_init(struct device_node *np)
{
    struct axera_clock_data *clk_data_top;
    clk_data_top = axera_clk_init(np, AX650X_MM_NR_CLKS);
    if (!clk_data_top)
        return;

    axera_clk_register_mux(ax650x_mux_clks_mm, ARRAY_SIZE(ax650x_mux_clks_mm), clk_data_top);
    axera_clk_register_gate(ax650x_gate_clks_mm, ARRAY_SIZE(ax650x_gate_clks_mm), clk_data_top);
    axera_clk_register_divider(ax650x_div_clks_mm, ARRAY_SIZE(ax650x_div_clks_mm), clk_data_top);
}

CLK_OF_DECLARE(ax650x_clk_mm_glb, "axera,ax650x-mm-clk", ax650x_clk_mm_clk_init);
/*mm-core*/
static struct axera_gate_clock ax650x_gate_clks_mm_core[] __initdata = {
    {AX650X_CLK_YUV_WDMA1, "clk_yuv_wdma1", "clk_mm_core_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, 6, 0, 0x8, 6, 0xC, 6, },
    {AX650X_CLK_YUV_SCALER1, "clk_yuv_scaler1", "clk_mm_core_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, 5, 0, 0x8, 5, 0xC, 5, },
    {AX650X_CLK_VGP, "clk_vgp", "clk_mm_core_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, 4, 0, 0x8, 4, 0xC, 4, },
    {AX650X_CLK_PYRA_LITE, "clk_pyra_lite", "clk_mm_core_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, 3, 0, 0x8, 3, 0xC, 3, },
    {AX650X_CLK_IVE, "clk_ive", "clk_mm_core_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, 2, 0, 0x8, 2, 0xC, 2, },
    {AX650X_CLK_GDC, "clk_gdc", "clk_mm_core_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, 1, 0, 0x8, 1, 0xC, 1, },
    {AX650X_CLK_CORE_CMD, "clk_core_cmd", "clk_mm_core_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, 0, 0, 0x8, 0, 0xC, 0, },
};

static void __init ax650x_clk_mm_core_clk_init(struct device_node *np)
{
    struct axera_clock_data *clk_data_top;
    clk_data_top = axera_clk_init(np, AX650X_MM_CORE_NR_CLKS);
    if (!clk_data_top)
        return;

    axera_clk_register_gate(ax650x_gate_clks_mm_core, ARRAY_SIZE(ax650x_gate_clks_mm_core), clk_data_top);
}

CLK_OF_DECLARE(ax650x_clk_mm_core_glb, "axera,ax650x-mm-core-clk", ax650x_clk_mm_core_clk_init);
/*mm-phy*/
static struct axera_gate_clock ax650x_gate_clks_mm_phy[] __initdata = {
    {AX650X_CLK_DPHY_TX_REF, "clk_dphy_tx_ref", "gpll_24m", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, 0, 0, 0x4, 0, 0x8, 0, },
};

static void __init ax650x_clk_mm_phy_clk_init(struct device_node *np)
{
    struct axera_clock_data *clk_data_top;
    clk_data_top = axera_clk_init(np, AX650X_MM_PHY_NR_CLKS);
    if (!clk_data_top)
        return;

    axera_clk_register_gate(ax650x_gate_clks_mm_phy, ARRAY_SIZE(ax650x_gate_clks_mm_phy), clk_data_top);
}

CLK_OF_DECLARE(ax650x_clk_mm_phy_glb, "axera,ax650x-mm-phy-clk", ax650x_clk_mm_phy_clk_init);

/*mm-misc:tdp/vpp*/
static struct axera_gate_clock ax650x_gate_clks_mm_tdp[] __initdata = {
    {AX650X_CLK_TDP0, "clk_mm_tdp0", "clk_isp_itp", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, 20, 2, 0x4, 20, 0x8, 20, },
    {AX650X_CLK_TDP1, "clk_mm_tdp1", "clk_isp_itp", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, 21, 2, 0x4, 21, 0x8, 21, },
};
static void __init ax650x_clk_mm_tdp_clk_init(struct device_node *np)
{
    struct axera_clock_data *clk_data_top;
    clk_data_top = axera_clk_init(np, AX650X_MM_TDP_NR_CLKS);
    if (!clk_data_top)
        return;

    axera_clk_register_gate(ax650x_gate_clks_mm_tdp, ARRAY_SIZE(ax650x_gate_clks_mm_tdp), clk_data_top);
}
CLK_OF_DECLARE(ax650x_clk_mm_tdp_glb, "axera,ax650x-mm-tdp-clk", ax650x_clk_mm_tdp_clk_init);

static struct axera_gate_clock ax650x_gate_clks_mm_vpp[] __initdata = {
    {AX650X_CLK_VPP, "clk_mm_vpp", "clk_isp_yuv", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, 17, 2, 0x4, 17, 0x8, 17, },
};
static void __init ax650x_clk_mm_vpp_clk_init(struct device_node *np)
{
    struct axera_clock_data *clk_data_top;
    clk_data_top = axera_clk_init(np, AX650X_MM_VPP_NR_CLKS);
    if (!clk_data_top)
        return;

    axera_clk_register_gate(ax650x_gate_clks_mm_vpp, ARRAY_SIZE(ax650x_gate_clks_mm_vpp), clk_data_top);
}
CLK_OF_DECLARE(ax650x_clk_mm_vpp_glb, "axera,ax650x-mm-vpp-clk", ax650x_clk_mm_vpp_clk_init);

/*periph*/
static const char *sclk_tdm_m1_sel[] __initconst = { "apll1_4096k", "apll0_4096k", "apll1_12p288m", "apll0_12p288m", "apll1_20p48m", "apll0_20p48m" };
static const char *sclk_tdm_m0_sel[] __initconst = { "apll1_4096k", "apll0_4096k", "apll1_12p288m", "apll0_12p288m", "apll1_20p48m", "apll0_20p48m" };
static const char *sclk_i2s_m1_sel[] __initconst = { "apll1_4096k", "apll0_4096k", "apll1_12p288m", "apll0_12p288m", "apll1_20p48m", "apll0_20p48m" };
static const char *sclk_i2s_m0_sel[] __initconst = { "apll1_4096k", "apll0_4096k", "apll1_12p288m", "apll0_12p288m", "apll1_20p48m", "apll0_20p48m" };
static const char *clk_periph_tmr1_sel[] __initconst = { "xtal_32k", "gpll_24m" };
static const char *clk_periph_tmr0_sel[] __initconst = { "xtal_32k", "gpll_24m" };
static const char *clk_periph_ser_sel[] __initconst = { "gpll_24m", "epll_100m", "epll_150m", "epll_187p5m", "npll_200m" };
static const char *clk_periph_gpio_sel[] __initconst = { "xtal_32k", "gpll_24m" };
static const char *clk_periph_glb_sel[] __initconst = { "gpll_24m", "npll_200m", "epll_300m", "mpll_350m" };

static struct axera_mux_clock ax650x_mux_clks_periph[] __initdata = {
    {AX650X_SCLK_TDM_M1_SEL, "sclk_tdm_m1_sel", sclk_tdm_m1_sel, ARRAY_SIZE(sclk_tdm_m1_sel), CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, 17, 3, 0, NULL, },
    {AX650X_SCLK_TDM_M0_SEL, "sclk_tdm_m0_sel", sclk_tdm_m0_sel, ARRAY_SIZE(sclk_tdm_m0_sel), CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, 14, 3, 0, NULL, },
    {AX650X_SCLK_I2S_M1_SEL, "sclk_i2s_m1_sel", sclk_i2s_m1_sel, ARRAY_SIZE(sclk_i2s_m1_sel), CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, 11, 3, 0, NULL, },
    {AX650X_SCLK_I2S_M0_SEL, "sclk_i2s_m0_sel", sclk_i2s_m0_sel, ARRAY_SIZE(sclk_i2s_m0_sel), CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, 8, 3, 0, NULL, },
    {AX650X_CLK_PERIPH_TMR1_SEL, "clk_periph_tmr1_sel", clk_periph_tmr1_sel, ARRAY_SIZE(clk_periph_tmr1_sel), CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, 7, 1, 0, NULL, },
    {AX650X_CLK_PERIPH_TMR0_SEL, "clk_periph_tmr0_sel", clk_periph_tmr0_sel, ARRAY_SIZE(clk_periph_tmr0_sel), CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, 6, 1, 0, NULL, },
    {AX650X_CLK_PERIPH_SER_SEL, "clk_periph_ser_sel", clk_periph_ser_sel, ARRAY_SIZE(clk_periph_ser_sel), CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, 3, 3, 0, NULL, },
    {AX650X_CLK_PERIPH_GPIO_SEL, "clk_periph_gpio_sel", clk_periph_gpio_sel, ARRAY_SIZE(clk_periph_gpio_sel), CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, 2, 1, 0, NULL, },
    {AX650X_CLK_PERIPH_GLB_SEL, "clk_periph_glb_sel", clk_periph_glb_sel, ARRAY_SIZE(clk_periph_glb_sel), CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, 0, 2, 0, NULL, },
};

static struct axera_gate_clock ax650x_gate_clks_periph[] __initdata = {
    {AX650X_SCLK_TDM_M1, "sclk_tdm_m1", "sclk_tdm_m1_divn", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x4, 7, 0, 0x40, 7, 0x44, 7, },
    {AX650X_SCLK_TDM_M0, "sclk_tdm_m0", "sclk_tdm_m0_divn", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x4, 6, 0, 0x40, 6, 0x44, 6, },
    {AX650X_SCLK_I2S_M1, "sclk_i2s_m1", "sclk_i2s_m1_divn", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x4, 5, 0, 0x40, 5, 0x44, 5, },
    {AX650X_SCLK_I2S_M0, "sclk_i2s_m0", "sclk_i2s_m0_divn", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x4, 4, 0, 0x40, 4, 0x44, 4, },
    {AX650X_CLK_PERIPH_TMR1, "clk_periph_tmr1", "clk_periph_tmr1_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x4, 3, 0, 0x40, 3, 0x44, 3, },
    {AX650X_CLK_PERIPH_TMR0, "clk_periph_tmr0", "clk_periph_tmr0_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x4, 2, 0, 0x40, 2, 0x44, 2, },
    {AX650X_CLK_PERIPH_SER, "clk_periph_ser", "clk_periph_ser_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x4, 1, 0, 0x40, 1, 0x44, 1, },
    {AX650X_CLK_PERIPH_GPIO, "clk_periph_gpio", "clk_periph_gpio_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x4, 0, 0, 0x40, 0, 0x44, 0, },
    {AX650X_CLK_PWM12, "clk_pwm12", "clk_periph_tmr0", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 31, 0, 0x48, 31, 0x4C, 31, },
    {AX650X_CLK_PWM11, "clk_pwm11", "clk_periph_tmr0", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 30, 0, 0x48, 30, 0x4C, 30, },
    {AX650X_CLK_PWM08, "clk_pwm08", "clk_periph_tmr0", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 29, 0, 0x48, 29, 0x4C, 29, },
    {AX650X_CLK_PWM07, "clk_pwm07", "clk_periph_tmr0", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 28, 0, 0x48, 28, 0x4C, 28, },
    {AX650X_CLK_PWM06, "clk_pwm06", "clk_periph_tmr0", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 27, 0, 0x48, 27, 0x4C, 27, },
    {AX650X_CLK_PWM05, "clk_pwm05", "clk_periph_tmr0", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 26, 0, 0x48, 26, 0x4C, 26, },
    {AX650X_CLK_PWM04, "clk_pwm04", "clk_periph_tmr0", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 25, 0, 0x48, 25, 0x4C, 25, },
    {AX650X_CLK_PWM03, "clk_pwm03", "clk_periph_tmr0", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 24, 0, 0x48, 24, 0x4C, 24, },
    {AX650X_CLK_PWM02, "clk_pwm02", "clk_periph_tmr0", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 23, 0, 0x48, 23, 0x4C, 23, },
    {AX650X_CLK_PWM01, "clk_pwm01", "clk_periph_tmr0", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 22, 0, 0x48, 22, 0x4C, 22, },
    {AX650X_CLK_I2C9, "clk_i2c9", "clk_periph_ser", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 21, 0, 0x48, 21, 0x4C, 21, },
    {AX650X_CLK_I2C8, "clk_i2c8", "clk_periph_ser", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 20, 0, 0x48, 20, 0x4C, 20, },
    {AX650X_CLK_I2C7, "clk_i2c7", "clk_periph_ser", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 19, 0, 0x48, 19, 0x4C, 19, },
    {AX650X_CLK_I2C6, "clk_i2c6", "clk_periph_ser", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 18, 0, 0x48, 18, 0x4C, 18, },
    {AX650X_CLK_I2C5, "clk_i2c5", "clk_periph_ser", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 17, 0, 0x48, 17, 0x4C, 17, },
    {AX650X_CLK_I2C4, "clk_i2c4", "clk_periph_ser", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 16, 0, 0x48, 16, 0x4C, 16, },
    {AX650X_CLK_I2C3, "clk_i2c3", "clk_periph_ser", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 15, 0, 0x48, 15, 0x4C, 15, },
    {AX650X_CLK_I2C2, "clk_i2c2", "clk_periph_ser", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 14, 0, 0x48, 14, 0x4C, 14, },
    {AX650X_CLK_I2C1, "clk_i2c1", "clk_periph_ser", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 13, 0, 0x48, 13, 0x4C, 13, },
    {AX650X_CLK_I2C14, "clk_i2c14", "clk_periph_ser", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 12, 0, 0x48, 12, 0x4C, 12, },
    {AX650X_CLK_I2C13, "clk_i2c13", "clk_periph_ser", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 11, 0, 0x48, 11, 0x4C, 11, },
    {AX650X_CLK_I2C12, "clk_i2c12", "clk_periph_ser", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 10, 0, 0x48, 10, 0x4C, 10, },
    {AX650X_CLK_I2C11, "clk_i2c11", "clk_periph_ser", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 9, 0, 0x48, 9, 0x4C, 9, },
    {AX650X_CLK_I2C10, "clk_i2c10", "clk_periph_ser", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 8, 0, 0x48, 8, 0x4C, 8, },
    {AX650X_CLK_I2C0, "clk_i2c0", "clk_periph_ser", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 7, 0, 0x48, 7, 0x4C, 7, },
    {AX650X_CLK_GPIO6, "clk_gpio6", "clk_periph_gpio", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 6, 0, 0x48, 6, 0x4C, 6, },
    {AX650X_CLK_GPIO5, "clk_gpio5", "clk_periph_gpio", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 5, 0, 0x48, 5, 0x4C, 5, },
    {AX650X_CLK_GPIO4, "clk_gpio4", "clk_periph_gpio", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 4, 0, 0x48, 4, 0x4C, 4, },
    {AX650X_CLK_GPIO3, "clk_gpio3", "clk_periph_gpio", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 3, 0, 0x48, 3, 0x4C, 3, },
    {AX650X_CLK_GPIO2, "clk_gpio2", "clk_periph_gpio", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 2, 0, 0x48, 2, 0x4C, 2, },
    {AX650X_CLK_GPIO1, "clk_gpio1", "clk_periph_gpio", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 1, 0, 0x48, 1, 0x4C, 1, },
    {AX650X_CLK_GPIO0, "clk_gpio0", "clk_periph_gpio", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 0, 0, 0x48, 0, 0x4C, 0, },
    {AX650X_CLK_TMR12, "clk_tmr12", "clk_periph_tmr0", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0xC, 31, 0, 0x50, 31, 0x54, 31, },
    {AX650X_CLK_TMR11, "clk_tmr11", "clk_periph_tmr0", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0xC, 30, 0, 0x50, 30, 0x54, 30, },
    {AX650X_CLK_TMR04, "clk_tmr04", "clk_periph_tmr0", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0xC, 29, 0, 0x50, 29, 0x54, 29, },
    {AX650X_CLK_TMR03, "clk_tmr03", "clk_periph_tmr0", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0xC, 28, 0, 0x50, 28, 0x54, 28, },
    {AX650X_CLK_TMR02, "clk_tmr02", "clk_periph_tmr0", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0xC, 27, 0, 0x50, 27, 0x54, 27, },
    {AX650X_CLK_TMR01, "clk_tmr01", "clk_periph_tmr0", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0xC, 26, 0, 0x50, 26, 0x54, 26, },
    {AX650X_CLK_SPI_S, "clk_spi_s", "clk_periph_ser", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0xC, 25, 0, 0x50, 25, 0x54, 25, },
    {AX650X_CLK_SPI_M3, "clk_spi_m3", "clk_periph_ser", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0xC, 24, 0, 0x50, 24, 0x54, 24, },
    {AX650X_CLK_SPI_M2, "clk_spi_m2", "clk_periph_ser", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0xC, 23, 0, 0x50, 23, 0x54, 23, },
    {AX650X_CLK_SPI_M1, "clk_spi_m1", "clk_periph_ser", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0xC, 22, 0, 0x50, 22, 0x54, 22, },
    {AX650X_CLK_PWM38, "clk_pwm38", "clk_periph_tmr1", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0xC, 21, 0, 0x50, 21, 0x54, 21, },
    {AX650X_CLK_PWM37, "clk_pwm37", "clk_periph_tmr1", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0xC, 20, 0, 0x50, 20, 0x54, 20, },
    {AX650X_CLK_PWM36, "clk_pwm36", "clk_periph_tmr1", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0xC, 19, 0, 0x50, 19, 0x54, 19, },
    {AX650X_CLK_PWM35, "clk_pwm35", "clk_periph_tmr1", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0xC, 18, 0, 0x50, 18, 0x54, 18, },
    {AX650X_CLK_PWM34, "clk_pwm34", "clk_periph_tmr1", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0xC, 17, 0, 0x50, 17, 0x54, 17, },
    {AX650X_CLK_PWM33, "clk_pwm33", "clk_periph_tmr1", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0xC, 16, 0, 0x50, 16, 0x54, 16, },
    {AX650X_CLK_PWM32, "clk_pwm32", "clk_periph_tmr1", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0xC, 15, 0, 0x50, 15, 0x54, 15, },
    {AX650X_CLK_PWM31, "clk_pwm31", "clk_periph_tmr1", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0xC, 14, 0, 0x50, 14, 0x54, 14, },
    {AX650X_CLK_PWM28, "clk_pwm28", "clk_periph_tmr1", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0xC, 13, 0, 0x50, 13, 0x54, 13, },
    {AX650X_CLK_PWM27, "clk_pwm27", "clk_periph_tmr1", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0xC, 12, 0, 0x50, 12, 0x54, 12, },
    {AX650X_CLK_PWM26, "clk_pwm26", "clk_periph_tmr1", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0xC, 11, 0, 0x50, 11, 0x54, 11, },
    {AX650X_CLK_PWM25, "clk_pwm25", "clk_periph_tmr1", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0xC, 10, 0, 0x50, 10, 0x54, 10, },
    {AX650X_CLK_PWM24, "clk_pwm24", "clk_periph_tmr1", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0xC, 9, 0, 0x50, 9, 0x54, 9, },
    {AX650X_CLK_PWM23, "clk_pwm23", "clk_periph_tmr1", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0xC, 8, 0, 0x50, 8, 0x54, 8, },
    {AX650X_CLK_PWM22, "clk_pwm22", "clk_periph_tmr1", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0xC, 7, 0, 0x50, 7, 0x54, 7, },
    {AX650X_CLK_PWM21, "clk_pwm21", "clk_periph_tmr1", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0xC, 6, 0, 0x50, 6, 0x54, 6, },
    {AX650X_CLK_PWM18, "clk_pwm18", "clk_periph_tmr0", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0xC, 5, 0, 0x50, 5, 0x54, 5, },
    {AX650X_CLK_PWM17, "clk_pwm17", "clk_periph_tmr0", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0xC, 4, 0, 0x50, 4, 0x54, 4, },
    {AX650X_CLK_PWM16, "clk_pwm16", "clk_periph_tmr0", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0xC, 3, 0, 0x50, 3, 0x54, 3, },
    {AX650X_CLK_PWM15, "clk_pwm15", "clk_periph_tmr0", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0xC, 2, 0, 0x50, 2, 0x54, 2, },
    {AX650X_CLK_PWM14, "clk_pwm14", "clk_periph_tmr0", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0xC, 1, 0, 0x50, 1, 0x54, 1, },
    {AX650X_CLK_PWM13, "clk_pwm13", "clk_periph_tmr0", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0xC, 0, 0, 0x50, 0, 0x54, 0, },
    {AX650X_PCLK_GPIO1, "pclk_gpio1", "clk_periph_glb_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x10, 31, 0, 0x58, 31, 0x5C, 31, },
    {AX650X_PCLK_GPIO0, "pclk_gpio0", "clk_periph_glb_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x10, 30, 0, 0x58, 30, 0x5C, 30, },
    {AX650X_HCLK_SPI_S, "hclk_spi_s", "clk_periph_glb_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x10, 29, 0, 0x58, 29, 0x5C, 29, },
    {AX650X_CLK_UART9, "clk_uart9", "clk_periph_ser", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x10, 28, 0, 0x58, 28, 0x5C, 28, },
    {AX650X_CLK_UART8, "clk_uart8", "clk_periph_ser", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x10, 27, 0, 0x58, 27, 0x5C, 27, },
    {AX650X_CLK_UART7, "clk_uart7", "clk_periph_ser", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x10, 26, 0, 0x58, 26, 0x5C, 26, },
    {AX650X_CLK_UART6, "clk_uart6", "clk_periph_ser", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x10, 25, 0, 0x58, 25, 0x5C, 25, },
    {AX650X_CLK_UART5, "clk_uart5", "clk_periph_ser", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x10, 24, 0, 0x58, 24, 0x5C, 24, },
    {AX650X_CLK_UART4, "clk_uart4", "clk_periph_ser", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x10, 23, 0, 0x58, 23, 0x5C, 23, },
    {AX650X_CLK_UART3, "clk_uart3", "clk_periph_ser", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x10, 22, 0, 0x58, 22, 0x5C, 22, },
    {AX650X_CLK_UART2, "clk_uart2", "clk_periph_ser", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x10, 21, 0, 0x58, 21, 0x5C, 21, },
    {AX650X_CLK_UART1, "clk_uart1", "clk_periph_ser", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x10, 20, 0, 0x58, 20, 0x5C, 20, },
    {AX650X_CLK_UART10, "clk_uart10", "clk_periph_ser", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x10, 19, 0, 0x58, 19, 0x5C, 19, },
    {AX650X_CLK_UART0, "clk_uart0", "clk_periph_ser", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x10, 18, 0, 0x58, 18, 0x5C, 18, },
    {AX650X_CLK_TMR54, "clk_tmr54", "clk_periph_tmr1", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x10, 17, 0, 0x58, 17, 0x5C, 17, },
    {AX650X_CLK_TMR53, "clk_tmr53", "clk_periph_tmr1", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x10, 16, 0, 0x58, 16, 0x5C, 16, },
    {AX650X_CLK_TMR52, "clk_tmr52", "clk_periph_tmr1", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x10, 15, 0, 0x58, 15, 0x5C, 15, },
    {AX650X_CLK_TMR51, "clk_tmr51", "clk_periph_tmr1", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x10, 14, 0, 0x58, 14, 0x5C, 14, },
    {AX650X_CLK_TMR44, "clk_tmr44", "clk_periph_tmr1", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x10, 13, 0, 0x58, 13, 0x5C, 13, },
    {AX650X_CLK_TMR43, "clk_tmr43", "clk_periph_tmr1", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x10, 12, 0, 0x58, 12, 0x5C, 12, },
    {AX650X_CLK_TMR42, "clk_tmr42", "clk_periph_tmr1", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x10, 11, 0, 0x58, 11, 0x5C, 11, },
    {AX650X_CLK_TMR41, "clk_tmr41", "clk_periph_tmr1", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x10, 10, 0, 0x58, 10, 0x5C, 10, },
    {AX650X_CLK_TMR34, "clk_tmr34", "clk_periph_tmr1", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x10, 9, 0, 0x58, 9, 0x5C, 9, },
    {AX650X_CLK_TMR33, "clk_tmr33", "clk_periph_tmr1", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x10, 8, 0, 0x58, 8, 0x5C, 8, },
    {AX650X_CLK_TMR32, "clk_tmr32", "clk_periph_tmr1", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x10, 7, 0, 0x58, 7, 0x5C, 7, },
    {AX650X_CLK_TMR31, "clk_tmr31", "clk_periph_tmr1", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x10, 6, 0, 0x58, 6, 0x5C, 6, },
    {AX650X_CLK_TMR24, "clk_tmr24", "clk_periph_tmr1", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x10, 5, 0, 0x58, 5, 0x5C, 5, },
    {AX650X_CLK_TMR23, "clk_tmr23", "clk_periph_tmr1", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x10, 4, 0, 0x58, 4, 0x5C, 4, },
    {AX650X_CLK_TMR22, "clk_tmr22", "clk_periph_tmr1", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x10, 3, 0, 0x58, 3, 0x5C, 3, },
    {AX650X_CLK_TMR21, "clk_tmr21", "clk_periph_tmr1", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x10, 2, 0, 0x58, 2, 0x5C, 2, },
    {AX650X_CLK_TMR14, "clk_tmr14", "clk_periph_tmr0", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x10, 1, 0, 0x58, 1, 0x5C, 1, },
    {AX650X_CLK_TMR13, "clk_tmr13", "clk_periph_tmr0", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x10, 0, 0, 0x58, 0, 0x5C, 0, },
    {AX650X_PCLK_TDM_M0, "pclk_tdm_m0", "clk_periph_glb_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x14, 31, 0, 0x60, 31, 0x64, 31, },
    {AX650X_PCLK_SPI_M3, "pclk_spi_m3", "clk_periph_glb_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x14, 30, 0, 0x60, 30, 0x64, 30, },
    {AX650X_PCLK_SPI_M2, "pclk_spi_m2", "clk_periph_glb_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x14, 29, 0, 0x60, 29, 0x64, 29, },
    {AX650X_PCLK_SPI_M1, "pclk_spi_m1", "clk_periph_glb_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x14, 28, 0, 0x60, 28, 0x64, 28, },
    {AX650X_PCLK_PWM3, "pclk_pwm3", "clk_periph_glb_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x14, 27, 0, 0x60, 27, 0x64, 27, },
    {AX650X_PCLK_PWM2, "pclk_pwm2", "clk_periph_glb_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x14, 26, 0, 0x60, 26, 0x64, 26, },
    {AX650X_PCLK_PWM1, "pclk_pwm1", "clk_periph_glb_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x14, 25, 0, 0x60, 25, 0x64, 25, },
    {AX650X_PCLK_PWM0, "pclk_pwm0", "clk_periph_glb_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x14, 24, 0, 0x60, 24, 0x64, 24, },
    {AX650X_PCLK_I2S_S1, "pclk_i2s_s1", "clk_periph_glb_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x14, 23, 0, 0x60, 23, 0x64, 23, },
    {AX650X_PCLK_I2S_S0, "pclk_i2s_s0", "clk_periph_glb_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x14, 22, 0, 0x60, 22, 0x64, 22, },
    {AX650X_PCLK_I2S_M1, "pclk_i2s_m1", "clk_periph_glb_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x14, 21, 0, 0x60, 21, 0x64, 21, },
    {AX650X_PCLK_I2S_M0, "pclk_i2s_m0", "clk_periph_glb_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x14, 20, 0, 0x60, 20, 0x64, 20, },
    {AX650X_PCLK_I2C9, "pclk_i2c9", "clk_periph_glb_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x14, 19, 0, 0x60, 19, 0x64, 19, },
    {AX650X_PCLK_I2C8, "pclk_i2c8", "clk_periph_glb_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x14, 18, 0, 0x60, 18, 0x64, 18, },
    {AX650X_PCLK_I2C7, "pclk_i2c7", "clk_periph_glb_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x14, 17, 0, 0x60, 17, 0x64, 17, },
    {AX650X_PCLK_I2C6, "pclk_i2c6", "clk_periph_glb_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x14, 16, 0, 0x60, 16, 0x64, 16, },
    {AX650X_PCLK_I2C5, "pclk_i2c5", "clk_periph_glb_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x14, 15, 0, 0x60, 15, 0x64, 15, },
    {AX650X_PCLK_I2C4, "pclk_i2c4", "clk_periph_glb_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x14, 14, 0, 0x60, 14, 0x64, 14, },
    {AX650X_PCLK_I2C3, "pclk_i2c3", "clk_periph_glb_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x14, 13, 0, 0x60, 13, 0x64, 13, },
    {AX650X_PCLK_I2C2, "pclk_i2c2", "clk_periph_glb_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x14, 12, 0, 0x60, 12, 0x64, 12, },
    {AX650X_PCLK_I2C1, "pclk_i2c1", "clk_periph_glb_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x14, 11, 0, 0x60, 11, 0x64, 11, },
    {AX650X_PCLK_I2C14, "pclk_i2c14", "clk_periph_glb_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x14, 10, 0, 0x60, 10, 0x64, 10, },
    {AX650X_PCLK_I2C13, "pclk_i2c13", "clk_periph_glb_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x14, 9, 0, 0x60, 9, 0x64, 9, },
    {AX650X_PCLK_I2C12, "pclk_i2c12", "clk_periph_glb_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x14, 8, 0, 0x60, 8, 0x64, 8, },
    {AX650X_PCLK_I2C11, "pclk_i2c11", "clk_periph_glb_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x14, 7, 0, 0x60, 7, 0x64, 7, },
    {AX650X_PCLK_I2C10, "pclk_i2c10", "clk_periph_glb_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x14, 6, 0, 0x60, 6, 0x64, 6, },
    {AX650X_PCLK_I2C0, "pclk_i2c0", "clk_periph_glb_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x14, 5, 0, 0x60, 5, 0x64, 5, },
    {AX650X_PCLK_GPIO6, "pclk_gpio6", "clk_periph_glb_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x14, 4, 0, 0x60, 4, 0x64, 4, },
    {AX650X_PCLK_GPIO5, "pclk_gpio5", "clk_periph_glb_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x14, 3, 0, 0x60, 3, 0x64, 3, },
    {AX650X_PCLK_GPIO4, "pclk_gpio4", "clk_periph_glb_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x14, 2, 0, 0x60, 2, 0x64, 2, },
    {AX650X_PCLK_GPIO3, "pclk_gpio3", "clk_periph_glb_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x14, 1, 0, 0x60, 1, 0x64, 1, },
    {AX650X_PCLK_GPIO2, "pclk_gpio2", "clk_periph_glb_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x14, 0, 0, 0x60, 0, 0x64, 0, },
    {AX650X_PCLK_UART9, "pclk_uart9", "clk_periph_glb_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x18, 19, 0, 0x68, 19, 0x6C, 19, },
    {AX650X_PCLK_UART8, "pclk_uart8", "clk_periph_glb_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x18, 18, 0, 0x68, 18, 0x6C, 18, },
    {AX650X_PCLK_UART7, "pclk_uart7", "clk_periph_glb_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x18, 17, 0, 0x68, 17, 0x6C, 17, },
    {AX650X_PCLK_UART6, "pclk_uart6", "clk_periph_glb_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x18, 16, 0, 0x68, 16, 0x6C, 16, },
    {AX650X_PCLK_UART5, "pclk_uart5", "clk_periph_glb_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x18, 15, 0, 0x68, 15, 0x6C, 15, },
    {AX650X_PCLK_UART4, "pclk_uart4", "clk_periph_glb_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x18, 14, 0, 0x68, 14, 0x6C, 14, },
    {AX650X_PCLK_UART3, "pclk_uart3", "clk_periph_glb_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x18, 13, 0, 0x68, 13, 0x6C, 13, },
    {AX650X_PCLK_UART2, "pclk_uart2", "clk_periph_glb_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x18, 12, 0, 0x68, 12, 0x6C, 12, },
    {AX650X_PCLK_UART1, "pclk_uart1", "clk_periph_glb_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x18, 11, 0, 0x68, 11, 0x6C, 11, },
    {AX650X_PCLK_UART10, "pclk_uart10", "clk_periph_glb_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x18, 10, 0, 0x68, 10, 0x6C, 10, },
    {AX650X_PCLK_UART0, "pclk_uart0", "clk_periph_glb_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x18, 9, 0, 0x68, 9, 0x6C, 9, },
    {AX650X_PCLK_TMR5, "pclk_tmr5", "clk_periph_glb_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x18, 8, 0, 0x68, 8, 0x6C, 8, },
    {AX650X_PCLK_TMR4, "pclk_tmr4", "clk_periph_glb_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x18, 7, 0, 0x68, 7, 0x6C, 7, },
    {AX650X_PCLK_TMR3, "pclk_tmr3", "clk_periph_glb_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x18, 6, 0, 0x68, 6, 0x6C, 6, },
    {AX650X_PCLK_TMR2, "pclk_tmr2", "clk_periph_glb_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x18, 5, 0, 0x68, 5, 0x6C, 5, },
    {AX650X_PCLK_TMR1, "pclk_tmr1", "clk_periph_glb_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x18, 4, 0, 0x68, 4, 0x6C, 4, },
    {AX650X_PCLK_TMR0, "pclk_tmr0", "clk_periph_glb_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x18, 3, 0, 0x68, 3, 0x6C, 3, },
    {AX650X_PCLK_TDM_S1, "pclk_tdm_s1", "clk_periph_glb_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x18, 2, 0, 0x68, 2, 0x6C, 2, },
    {AX650X_PCLK_TDM_S0, "pclk_tdm_s0", "clk_periph_glb_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x18, 1, 0, 0x68, 1, 0x6C, 1, },
    {AX650X_PCLK_TDM_M1, "pclk_tdm_m1", "clk_periph_glb_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x18, 0, 0, 0x68, 0, 0x6C, 0, },
};

static struct axera_divider_clock ax650x_div_clks_periph[] __initdata = {
    {AX650X_SCLK_TDM_M1_DIVN, "sclk_tdm_m1_divn", "sclk_tdm_m1_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x1C, 0x1C, 19, 15, 4, 0, NULL, },
    {AX650X_SCLK_TDM_M0_DIVN, "sclk_tdm_m0_divn", "sclk_tdm_m0_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x1C, 0x1C, 14, 10, 4, 0, NULL, },
    {AX650X_SCLK_I2S_M1_DIVN, "sclk_i2s_m1_divn", "sclk_i2s_m1_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x1C, 0x1C, 9, 5, 4, 0, NULL, },
    {AX650X_SCLK_I2S_M0_DIVN, "sclk_i2s_m0_divn", "sclk_i2s_m0_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x1C, 0x1C, 4, 0, 4, 0, NULL, },
};

static void __init ax650x_clk_periph_clk_init(struct device_node *np)
{
    struct axera_clock_data *clk_data_top;
    clk_data_top = axera_clk_init(np, AX650X_PERIPH_NR_CLKS);
    if (!clk_data_top)
        return;

    axera_clk_register_mux(ax650x_mux_clks_periph, ARRAY_SIZE(ax650x_mux_clks_periph), clk_data_top);
    axera_clk_register_gate(ax650x_gate_clks_periph, ARRAY_SIZE(ax650x_gate_clks_periph), clk_data_top);
    axera_clk_register_divider(ax650x_div_clks_periph, ARRAY_SIZE(ax650x_div_clks_periph), clk_data_top);
}

CLK_OF_DECLARE(ax650x_clk_periph_glb, "axera,ax650x-periph-clk", ax650x_clk_periph_clk_init);
/*pipe*/
static const char *clk_pp_rgmii_tx_sel[] __initconst = { "epll_5m", "epll_50m", "epll_250m" };
static const char *clk_pipe_glb_sel[] __initconst = { "gpll_24m", "epll_100m" };
static const char *clk_pipe_bus_sel[] __initconst = { "gpll_24m", "epll_300m", "npll_400m", "epll_500m_180" };
static const char *clk1_usb_suspend_sel[] __initconst = { "xtal_32k", "epll_30m" };
static const char *clk0_usb_suspend_sel[] __initconst = { "xtal_32k", "epll_30m" };
static const char *cclk_tx_pp_sdio_sel[] __initconst = { "gpll_24m_div2", "epll_250m_div2", "epll_300m_div2", "npll_400m_div2" }; ///*"gpll_416m"*/revised name
static const char *aux_clk_pcie_sel[] __initconst = { "gpll_24m", "epll_100m", "epll_250m" };

static struct axera_mux_clock ax650x_mux_clks_pipe[] __initdata = {
    {AX650X_CLK_PP_RGMII_TX_SEL, "clk_pp_rgmii_tx_sel", clk_pp_rgmii_tx_sel, ARRAY_SIZE(clk_pp_rgmii_tx_sel), CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, 9, 2, 0, NULL, },
    {AX650X_CLK_PIPE_GLB_SEL, "clk_pipe_glb_sel", clk_pipe_glb_sel, ARRAY_SIZE(clk_pipe_glb_sel), CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, 8, 1, 0, NULL, },
    {AX650X_CLK_PIPE_BUS_SEL, "clk_pipe_bus_sel", clk_pipe_bus_sel, ARRAY_SIZE(clk_pipe_bus_sel), CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, 6, 2, 0, NULL, },
    {AX650X_CLK1_USB_SUSPEND_SEL, "clk1_usb_suspend_sel", clk1_usb_suspend_sel, ARRAY_SIZE(clk1_usb_suspend_sel), CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, 5, 1, 0, NULL, },
    {AX650X_CLK0_USB_SUSPEND_SEL, "clk0_usb_suspend_sel", clk0_usb_suspend_sel, ARRAY_SIZE(clk0_usb_suspend_sel), CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, 4, 1, 0, NULL, },
    {AX650X_CCLK_TX_PP_SDIO_SEL, "cclk_tx_pp_sdio_sel", cclk_tx_pp_sdio_sel, ARRAY_SIZE(cclk_tx_pp_sdio_sel), CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, 2, 2, 0, NULL, },
    {AX650X_AUX_CLK_PCIE_SEL, "aux_clk_pcie_sel", aux_clk_pcie_sel, ARRAY_SIZE(aux_clk_pcie_sel), CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, 0, 2, 0, NULL, },
};

static struct axera_gate_clock ax650x_gate_clks_pipe[] __initdata = {
    {AX650X_TCLK_PP_SDIO, "tclk_pp_sdio", "gpll_24m", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x4, 10, 0, 0x20, 10, 0x24, 10, },
    {AX650X_CLK_USB_REF, "clk_usb_ref", "gpll_24m", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x4, 9, 0, 0x20, 9, 0x24, 9, },
    {AX650X_CLK_PP_RMII_RX, "clk_pp_rmii_rx", "clk_pp_rmii_rx_divn", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x4, 8, 0, 0x20, 8, 0x24, 8, },
    {AX650X_CLK_PP_RMII_PHY, "clk_pp_rmii_phy", "epll_50m", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x4, 7, 0, 0x20, 7, 0x24, 7, },
    {AX650X_CLK_PP_RGMII_TX, "clk_pp_rgmii_tx", "clk_pp_rgmii_tx_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x4, 6, 0, 0x20, 6, 0x24, 6, },
    {AX650X_CLK_PP_EMAC_PTP, "clk_pp_emac_ptp", "epll_50m", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x4, 5, 0, 0x20, 5, 0x24, 5, },
    {AX650X_CLK1_USB_SUSPEND, "clk1_usb_suspend", "clk1_usb_suspend_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x4, 4, 0, 0x20, 4, 0x24, 4, },
    {AX650X_CLK0_USB_SUSPEND, "clk0_usb_suspend", "clk0_usb_suspend_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x4, 3, 0, 0x20, 3, 0x24, 3, },
    {AX650X_CCLK_TX_PP_SDIO, "cclk_tx_pp_sdio", "cclk_tx_pp_sdio_divn", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x4, 2, 0, 0x20, 2, 0x24, 2, },
    {AX650X_BCLK_PP_SDIO, "bclk_pp_sdio", "epll_300m", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x4, 1, 0, 0x20, 1, 0x24, 1, },
    {AX650X_AUX_CLK_PCIE, "aux_clk_pcie", "aux_clk_pcie_divn", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x4, 0, 0, 0x20, 0, 0x24, 0, },
    {AX650X_TMCLK_PP_SDIO, "tmclk_pp_sdio", "gpll_24m", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 15, 0, 0x28, 15, 0x2C, 15, },
    {AX650X_HCLK_PP_SDIO, "hclk_pp_sdio", "clk_pipe_bus_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 14, 0, 0x28, 14, 0x2C, 14, },
    {AX650X_CQETMCLK_PP_SDIO, "cqetmclk_pp_sdio", "gpll_24m", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 13, 0, 0x28, 13, 0x2C, 13, },
    {AX650X_CLK_USB1_REF, "clk_usb1_ref", "gpll_24m", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 12, 0, 0x28, 12, 0x2C, 12, },
    {AX650X_CLK_USB0_REF, "clk_usb0_ref", "gpll_24m", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 11, 0, 0x28, 11, 0x2C, 11, },
    {AX650X_CLK_DMA2, "clk_dma2", "clk_pipe_bus_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 10, 0, 0x28, 10, 0x2C, 10, },
    {AX650X_CLK_DMA0, "clk_dma0", "clk_pipe_bus_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 9, 0, 0x28, 9, 0x2C, 9, },
    {AX650X_BUS_CLK_USB1, "bus_clk_usb1", "clk_pipe_bus_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 8, 0, 0x28, 8, 0x2C, 8, },
    {AX650X_BUS_CLK_USB0, "bus_clk_usb0", "clk_pipe_bus_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 7, 0, 0x28, 7, 0x2C, 7, },
    {AX650X_AUX_CLK_PCIE1, "aux_clk_pcie1", "aux_clk_pcie", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 6, 0, 0x28, 6, 0x2C, 6, },
    {AX650X_AUX_CLK_PCIE0, "aux_clk_pcie0", "aux_clk_pcie", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 5, 0, 0x28, 5, 0x2C, 5, },
    {AX650X_ACLK_SATA, "aclk_sata", "clk_pipe_bus_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 4, 0, 0x28, 4, 0x2C, 4, },
    {AX650X_ACLK_PP_SDIO, "aclk_pp_sdio", "clk_pipe_bus_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 3, 0, 0x28, 3, 0x2C, 3, },
    {AX650X_ACLK_PP_EMAC, "aclk_pp_emac", "clk_pipe_bus_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 2, 0, 0x28, 2, 0x2C, 2, },
    {AX650X_ACLK_PCIE1, "aclk_pcie1", "clk_pipe_bus_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 1, 0, 0x28, 1, 0x2C, 1, },
    {AX650X_ACLK_PCIE0, "aclk_pcie0", "clk_pipe_bus_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 0, 0, 0x28, 0, 0x2C, 0, },
};

static struct axera_divider_clock ax650x_div_clks_pipe[] __initdata = {
    {AX650X_CLK_PP_RMII_RX_DIVN, "clk_pp_rmii_rx_divn", "need_modify", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0xC, 0xC, 15, 11, 4, 0, NULL, },
    {AX650X_CCLK_TX_PP_SDIO_DIVN, "cclk_tx_pp_sdio_divn", "cclk_tx_pp_sdio_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0xC, 0xC, 10, 5, 5, 0, NULL, },
    {AX650X_AUX_CLK_PCIE_DIVN, "aux_clk_pcie_divn", "aux_clk_pcie_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0xC, 0xC, 4, 0, 4, 0, NULL, },
};

static void __init ax650x_clk_pipe_clk_init(struct device_node *np)
{
    struct axera_clock_data *clk_data_top;
    clk_data_top = axera_clk_init(np, AX650X_PIPE_NR_CLKS);
    if (!clk_data_top)
        return;

    axera_clk_register_mux(ax650x_mux_clks_pipe, ARRAY_SIZE(ax650x_mux_clks_pipe), clk_data_top);
    axera_clk_register_gate(ax650x_gate_clks_pipe, ARRAY_SIZE(ax650x_gate_clks_pipe), clk_data_top);
    axera_clk_register_divider(ax650x_div_clks_pipe, ARRAY_SIZE(ax650x_div_clks_pipe), clk_data_top);
}

CLK_OF_DECLARE(ax650x_clk_pipe_glb, "axera,ax650x-pipe-clk", ax650x_clk_pipe_clk_init);
/*pipe-phy*/
static struct axera_gate_clock ax650x_gate_clks_pipe_phy[] __initdata = {
    {AX650X_USB1_REF_ALT_CLK, "usb1_ref_alt_clk", "xtal_24m", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, 2, 0, 0x10, 2, 0x14, 2, },
    {AX650X_USB0_REF_ALT_CLK, "usb0_ref_alt_clk", "gpll_24m", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, 1, 0, 0x10, 1, 0x14, 1, },
    {AX650X_PIPE_REF_ALT_CLK, "pipe_ref_alt_clk", "npll_200m", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, 0, 0, 0x10, 0, 0x14, 0, },
    {AX650X_PCLK_USB1_PHY, "pclk_usb1_phy", "gpll_24m", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x4, 5, 0, 0x18, 5, 0x1C, 5, },
    {AX650X_PCLK_USB0_PHY, "pclk_usb0_phy", "gpll_24m", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x4, 4, 0, 0x18, 4, 0x1C, 4, },
    {AX650X_PCLK_PCIE3_PHY, "pclk_pcie3_phy", "gpll_24m", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x4, 3, 0, 0x18, 3, 0x1C, 3, },
    {AX650X_PCLK_PCIE2_PHY, "pclk_pcie2_phy", "gpll_24m", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x4, 2, 0, 0x18, 2, 0x1C, 2, },
    {AX650X_PCLK_PCIE1_PHY, "pclk_pcie1_phy", "gpll_24m", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x4, 1, 0, 0x18, 1, 0x1C, 1, },
    {AX650X_PCLK_PCIE0_PHY, "pclk_pcie0_phy", "gpll_24m", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x4, 0, 0, 0x18, 0, 0x1C, 0, },
};

static void __init ax650x_clk_pipe_phy_clk_init(struct device_node *np)
{
    struct axera_clock_data *clk_data_top;
    clk_data_top = axera_clk_init(np, AX650X_PIPE_PHY_NR_CLKS);
    if (!clk_data_top)
        return;

    axera_clk_register_gate(ax650x_gate_clks_pipe_phy, ARRAY_SIZE(ax650x_gate_clks_pipe_phy), clk_data_top);
}

CLK_OF_DECLARE(ax650x_clk_pipe_phy_glb, "axera,ax650x-pipe-phy-clk", ax650x_clk_pipe_phy_clk_init);

/*sec*/
static struct axera_gate_clock ax650x_gate_clks_sec[] __initdata = {
    {AX650X_PCLK_SEC_WDT, "pclk_sec_wdt", "pclk_top_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, 13, 0, 0x8, 13, 0xC, 13, },
    {AX650X_PCLK_SEC_UART, "pclk_sec_uart", "pclk_top_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, 12, 0, 0x8, 12, 0xC, 12, },
    {AX650X_PCLK_SEC_TMR1, "pclk_sec_tmr1", "pclk_top_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, 11, 0, 0x8, 11, 0xC, 11, },
    {AX650X_PCLK_SEC_TMR0, "pclk_sec_tmr0", "pclk_top_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, 10, 0, 0x8, 10, 0xC, 10, },
    {AX650X_PCLK_SEC_GPIO, "pclk_sec_gpio", "pclk_top_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, 9, 0, 0x8, 9, 0xC, 9, },
    {AX650X_PCLK_SEC_CE, "pclk_sec_ce", "pclk_top_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, 8, 0, 0x8, 8, 0xC, 8, },
    {AX650X_PCLK_FIREWALL0, "pclk_firewall0", "pclk_top_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, 7, 0, 0x8, 7, 0xC, 7, },
    {AX650X_CLK_SEC_WDT, "clk_sec_wdt", "clk_comm_wdt", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, 6, 0, 0x8, 6, 0xC, 6, },
    {AX650X_CLK_SEC_UART, "clk_sec_uart", "clk_comm_ip", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, 5, 0, 0x8, 5, 0xC, 5, },
    {AX650X_CLK_SEC_TMR1, "clk_sec_tmr1", "clk_comm_tmr1", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, 4, 0, 0x8, 4, 0xC, 4, },
    {AX650X_CLK_SEC_TMR0, "clk_sec_tmr0", "clk_comm_tmr0", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, 3, 0, 0x8, 3, 0xC, 3, },
    {AX650X_CLK_SEC_GPIO, "clk_sec_gpio", "clk_comm_wdt", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, 2, 0, 0x8, 2, 0xC, 2, },
    {AX650X_CLK_SEC_CE, "clk_sec_ce", "clk_ce_bus_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, 1, 0, 0x8, 1, 0xC, 1, },
    {AX650X_CLK_SEC_CE_CNT, "clk_sec_ce_cnt", "clk_ce_cnt", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, 0, 0, 0x8, 0, 0xC, 0, },
};

static void __init ax650x_clk_sec_clk_init(struct device_node *np)
{
    struct axera_clock_data *clk_data_top;
    clk_data_top = axera_clk_init(np, AX650X_SEC_NR_CLKS);
    if (!clk_data_top)
        return;

    axera_clk_register_gate(ax650x_gate_clks_sec, ARRAY_SIZE(ax650x_gate_clks_sec), clk_data_top);
}

CLK_OF_DECLARE(ax650x_clk_sec_glb, "axera,ax650x-sec-clk", ax650x_clk_sec_clk_init);
/*sen*/
static const char *clk_sen_spi_sel[] __initconst = { "gpll_24m", "epll_125m", "npll_200m" };
static const char *clk_sen_glb_sel[] __initconst = { "gpll_24m", "gpll_208m" };

static struct axera_mux_clock ax650x_mux_clks_sen[] __initdata = {
    {AX650X_CLK_SEN_SPI_SEL, "clk_sen_spi_sel", clk_sen_spi_sel, ARRAY_SIZE(clk_sen_spi_sel), CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, 1, 2, 0, NULL, },
    {AX650X_CLK_SEN_GLB_SEL, "clk_sen_glb_sel", clk_sen_glb_sel, ARRAY_SIZE(clk_sen_glb_sel), CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, 0, 1, 0, NULL, },
};

static struct axera_gate_clock ax650x_gate_clks_sen[] __initdata = {
    {AX650X_CLK_SEN_SPI, "clk_sen_spi", "clk_sen_spi_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x4, 0, 0, 0x18, 0, 0x1C, 0, },
    {AX650X_PCLK_SEN_SPI, "pclk_sen_spi", "clk_sen_glb_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 14, 0, 0x20, 14, 0x24, 14, },
    {AX650X_PCLK_RX3_PINMUX, "pclk_rx3_pinmux", "clk_sen_glb_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 13, 0, 0x20, 13, 0x24, 13, },
    {AX650X_PCLK_RX2_PINMUX, "pclk_rx2_pinmux", "clk_sen_glb_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 12, 0, 0x20, 12, 0x24, 12, },
    {AX650X_PCLK_RX1_PINMUX, "pclk_rx1_pinmux", "clk_sen_glb_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 11, 0, 0x20, 11, 0x24, 11, },
    {AX650X_PCLK_RX0_PINMUX, "pclk_rx0_pinmux", "clk_sen_glb_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 10, 0, 0x20, 10, 0x24, 10, },
    {AX650X_PCLK_DPHY_CSI_RX1, "pclk_dphy_csi_rx1", "clk_sen_glb_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 9, 0, 0x20, 9, 0x24, 9, },
    {AX650X_PCLK_DPHY_CSI_RX0, "pclk_dphy_csi_rx0", "clk_sen_glb_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 8, 0, 0x20, 8, 0x24, 8, },
    {AX650X_PCLK_CSI_RX7, "pclk_csi_rx7", "clk_sen_glb_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 7, 0, 0x20, 7, 0x24, 7, },
    {AX650X_PCLK_CSI_RX6, "pclk_csi_rx6", "clk_sen_glb_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 6, 0, 0x20, 6, 0x24, 6, },
    {AX650X_PCLK_CSI_RX5, "pclk_csi_rx5", "clk_sen_glb_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 5, 0, 0x20, 5, 0x24, 5, },
    {AX650X_PCLK_CSI_RX4, "pclk_csi_rx4", "clk_sen_glb_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 4, 0, 0x20, 4, 0x24, 4, },
    {AX650X_PCLK_CSI_RX3, "pclk_csi_rx3", "clk_sen_glb_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 3, 0, 0x20, 3, 0x24, 3, },
    {AX650X_PCLK_CSI_RX2, "pclk_csi_rx2", "clk_sen_glb_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 2, 0, 0x20, 2, 0x24, 2, },
    {AX650X_PCLK_CSI_RX1, "pclk_csi_rx1", "clk_sen_glb_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 1, 0, 0x20, 1, 0x24, 1, },
    {AX650X_PCLK_CSI_RX0, "pclk_csi_rx0", "clk_sen_glb_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 0, 0, 0x20, 0, 0x24, 0, },
};

static void __init ax650x_clk_sen_clk_init(struct device_node *np)
{
    struct axera_clock_data *clk_data_top;
    clk_data_top = axera_clk_init(np, AX650X_SEN_NR_CLKS);
    if (!clk_data_top)
        return;

    axera_clk_register_mux(ax650x_mux_clks_sen, ARRAY_SIZE(ax650x_mux_clks_sen), clk_data_top);
    axera_clk_register_gate(ax650x_gate_clks_sen, ARRAY_SIZE(ax650x_gate_clks_sen), clk_data_top);
}

CLK_OF_DECLARE(ax650x_clk_sen_glb, "axera,ax650x-sen-clk", ax650x_clk_sen_clk_init);
/*sen-phy*/
static struct axera_gate_clock ax650x_gate_clks_sen_phy[] __initdata = {
    {AX650X_RXPHY_TXCLKESC, "rxphy_txclkesc", "epll_20m", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, 0, 0, 0xC, 0, 0x10, 0, },
    {AX650X_PCLK_CDPHY_RX7, "pclk_cdphy_rx7", "gpll_24m", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x4, 15, 0, 0x14, 15, 0x18, 15, },
    {AX650X_PCLK_CDPHY_RX6, "pclk_cdphy_rx6", "gpll_24m", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x4, 14, 0, 0x14, 14, 0x18, 14, },
    {AX650X_PCLK_CDPHY_RX5, "pclk_cdphy_rx5", "gpll_24m", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x4, 13, 0, 0x14, 13, 0x18, 13, },
    {AX650X_PCLK_CDPHY_RX4, "pclk_cdphy_rx4", "gpll_24m", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x4, 12, 0, 0x14, 12, 0x18, 12, },
    {AX650X_PCLK_CDPHY_RX3, "pclk_cdphy_rx3", "gpll_24m", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x4, 11, 0, 0x14, 11, 0x18, 11, },
    {AX650X_PCLK_CDPHY_RX2, "pclk_cdphy_rx2", "gpll_24m", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x4, 10, 0, 0x14, 10, 0x18, 10, },
    {AX650X_PCLK_CDPHY_RX1, "pclk_cdphy_rx1", "gpll_24m", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x4, 9, 0, 0x14, 9, 0x18, 9, },
    {AX650X_PCLK_CDPHY_RX0, "pclk_cdphy_rx0", "gpll_24m", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x4, 8, 0, 0x14, 8, 0x18, 8, },
    {AX650X_CDPHY_RX7_CFG_CLK, "cdphy_rx7_cfg_clk", "gpll_24m", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x4, 7, 0, 0x14, 7, 0x18, 7, },
    {AX650X_CDPHY_RX6_CFG_CLK, "cdphy_rx6_cfg_clk", "gpll_24m", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x4, 6, 0, 0x14, 6, 0x18, 6, },
    {AX650X_CDPHY_RX5_CFG_CLK, "cdphy_rx5_cfg_clk", "gpll_24m", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x4, 5, 0, 0x14, 5, 0x18, 5, },
    {AX650X_CDPHY_RX4_CFG_CLK, "cdphy_rx4_cfg_clk", "gpll_24m", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x4, 4, 0, 0x14, 4, 0x18, 4, },
    {AX650X_CDPHY_RX3_CFG_CLK, "cdphy_rx3_cfg_clk", "gpll_24m", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x4, 3, 0, 0x14, 3, 0x18, 3, },
    {AX650X_CDPHY_RX2_CFG_CLK, "cdphy_rx2_cfg_clk", "gpll_24m", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x4, 2, 0, 0x14, 2, 0x18, 2, },
    {AX650X_CDPHY_RX1_CFG_CLK, "cdphy_rx1_cfg_clk", "gpll_24m", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x4, 1, 0, 0x14, 1, 0x18, 1, },
    {AX650X_CDPHY_RX0_CFG_CLK, "cdphy_rx0_cfg_clk", "gpll_24m", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x4, 0, 0, 0x14, 0, 0x18, 0, },
};

static void __init ax650x_clk_sen_phy_clk_init(struct device_node *np)
{
    struct axera_clock_data *clk_data_top;
    clk_data_top = axera_clk_init(np, AX650X_SEN_PHY_NR_CLKS);
    if (!clk_data_top)
        return;

    axera_clk_register_gate(ax650x_gate_clks_sen_phy, ARRAY_SIZE(ax650x_gate_clks_sen_phy), clk_data_top);
}

CLK_OF_DECLARE(ax650x_clk_sen_phy_glb, "axera,ax650x-sen-phy-clk", ax650x_clk_sen_phy_clk_init);
/*vdec*/
static const char *clk_vdec_bus_sel[] __initconst = { "npll_400m", "epll_500m_120", "gpll_624m_180" };

static struct axera_mux_clock ax650x_mux_clks_vdec[] __initdata = {
    {AX650X_CLK_VDEC_BUS_SEL, "clk_vdec_bus_sel", clk_vdec_bus_sel, ARRAY_SIZE(clk_vdec_bus_sel), CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, 0, 2, 0, NULL, },
};

static struct axera_gate_clock ax650x_gate_clks_vdec[] __initdata = {
    {AX650X_CLK_VDEC_ROSC, "clk_vdec_rosc", "gpll_24m", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x4, 0, 0, 0x18, 0, 0x1C, 0, },
    {AX650X_CLK_VDEC1, "clk_vdec1", "clk_vdec_bus_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 1, 0, 0x20, 1, 0x24, 1, },
    {AX650X_CLK_VDEC0, "clk_vdec0", "clk_vdec_bus_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 0, 0, 0x20, 0, 0x24, 0, },
};

static void __init ax650x_clk_vdec_clk_init(struct device_node *np)
{
    struct axera_clock_data *clk_data_top;
    clk_data_top = axera_clk_init(np, AX650X_VDEC_NR_CLKS);
    if (!clk_data_top)
        return;

    axera_clk_register_mux(ax650x_mux_clks_vdec, ARRAY_SIZE(ax650x_mux_clks_vdec), clk_data_top);
    axera_clk_register_gate(ax650x_gate_clks_vdec, ARRAY_SIZE(ax650x_gate_clks_vdec), clk_data_top);

}

CLK_OF_DECLARE(ax650x_clk_vdec_glb, "axera,ax650x-vdec-clk", ax650x_clk_vdec_clk_init);
/*vdsp*/
static const char *clk_vdsp_glb_sel[] __initconst = { "gpll_24m", "epll_187p5m", "npll_200m", "gpll_208m" };
static const char *clk_vdsp_bus_sel[] __initconst = { "gpll_24m", "gpll_624m_180", "mpll_700m_180", "epll_750m_180", "dspll_800m_180" };
static const char *clk_vdsp0_bus_sel[] __initconst = { "gpll_24m", "gpll_624m", "mpll_700m", "epll_750m", "dspll_800m" };
static const char *clk_vdsp1_bus_sel[] __initconst = { "gpll_24m", "gpll_624m_90", "mpll_700m_90", "epll_750m_90", "dspll_800m_90" };

static struct axera_mux_clock ax650x_mux_clks_vdsp[] __initdata = {
    {AX650X_CLK_VDSP_GLB_SEL, "clk_vdsp_glb_sel", clk_vdsp_glb_sel, ARRAY_SIZE(clk_vdsp_glb_sel), CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, 3, 2, 0, NULL, },
    {AX650X_CLK_VDSP_BUS_SEL, "clk_vdsp_bus_sel", clk_vdsp_bus_sel, ARRAY_SIZE(clk_vdsp_bus_sel), CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, 0, 3, 0, NULL, },
    {AX650X_CLK_VDSP0_BUS_SEL, "clk_vdsp0_bus_sel", clk_vdsp0_bus_sel, ARRAY_SIZE(clk_vdsp0_bus_sel), CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x4, 0, 3, 0, NULL, },
    {AX650X_CLK_VDSP1_BUS_SEL, "clk_vdsp1_bus_sel", clk_vdsp1_bus_sel, ARRAY_SIZE(clk_vdsp1_bus_sel), CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 0, 3, 0, NULL, },
};

static struct axera_gate_clock ax650x_gate_clks_vdsp[] __initdata = {
    {AX650X_CLK_VDSP_ROSC, "clk_vdsp_rosc", "gpll_24m", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0xC, 1, 0, 0x3C, 1, 0x40, 1, },
    {AX650X_CLK_VDSP_TLB, "clk_vdsp_tlb", "gpll_24m", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0xC, 0, 0, 0x3C, 0, 0x40, 0, },
    {AX650X_PCLK_IDMA_AXI_CHANGE0, "pclk_idma_axi_change0", "clk_vdsp_glb_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x10, 2, 0, 0x44, 2, 0x48, 2, },
    {AX650X_CLK_VDSP0, "clk_vdsp0", "clk_vdsp0_bus_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x10, 1, 0, 0x44, 1, 0x48, 1, },
    {AX650X_PCLK_ADDR_REMAP0, "pclk_addr_remap0", "clk_vdsp_glb_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x10, 0, 0, 0x44, 0, 0x48, 0, },
    {AX650X_PCLK_IDMA_AXI_CHANGE1, "pclk_idma_axi_change1", "clk_vdsp_glb_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x14, 2, 0, 0x4C, 2, 0x50, 2, },
    {AX650X_CLK_VDSP1, "clk_vdsp1", "clk_vdsp1_bus_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x14, 1, 0, 0x4C, 1, 0x50, 1, },
    {AX650X_PCLK_ADDR_REMAP1, "pclk_addr_remap1", "clk_vdsp_glb_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x14, 0, 0, 0x4C, 0, 0x50, 0, },
};

static void __init ax650x_clk_vdsp_clk_init(struct device_node *np)
{
    struct axera_clock_data *clk_data_top;
    clk_data_top = axera_clk_init(np, AX650X_VDSP_NR_CLKS);
    if (!clk_data_top)
        return;

    axera_clk_register_mux(ax650x_mux_clks_vdsp, ARRAY_SIZE(ax650x_mux_clks_vdsp), clk_data_top);
    axera_clk_register_gate(ax650x_gate_clks_vdsp, ARRAY_SIZE(ax650x_gate_clks_vdsp), clk_data_top);
}

CLK_OF_DECLARE(ax650x_clk_vdsp_glb, "axera,ax650x-vdsp-clk", ax650x_clk_vdsp_clk_init);
/*venc*/
static const char *clk_venc_bus_sel[] __initconst = { "epll_500m", "gpll_624m", "mpll_700m_90", "npll_800m_180" };
static const char *clk_venc1_bus_sel[] __initconst = { "npll_400m", "epll_500m", "gpll_624m" };
static const char *clk_venc0_bus_sel[] __initconst = { "npll_400m", "epll_500m", "gpll_624m" };

static struct axera_mux_clock ax650x_mux_clks_venc[] __initdata = {
    {AX650X_CLK_VENC_BUS_SEL, "clk_venc_bus_sel", clk_venc_bus_sel, ARRAY_SIZE(clk_venc_bus_sel), CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, 4, 2, 0, NULL, },
    {AX650X_CLK_VENC1_BUS_SEL, "clk_venc1_bus_sel", clk_venc1_bus_sel, ARRAY_SIZE(clk_venc1_bus_sel), CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, 2, 2, 0, NULL, },
    {AX650X_CLK_VENC0_BUS_SEL, "clk_venc0_bus_sel", clk_venc0_bus_sel, ARRAY_SIZE(clk_venc0_bus_sel), CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, 0, 2, 0, NULL, },
};

static struct axera_gate_clock ax650x_gate_clks_venc[] __initdata = {
    {AX650X_CLK_VENC_ROSC, "clk_venc_rosc", "gpll_24m", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x4, 0, 0, 0x18, 0, 0x1C, 0, },
    {AX650X_CLK_VENC1, "clk_venc1", "clk_venc1_bus_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 1, 0, 0x20, 1, 0x24, 1, },
    {AX650X_CLK_VENC0, "clk_venc0", "clk_venc0_bus_sel", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x8, 0, 0, 0x20, 0, 0x24, 0, },
};

static void __init ax650x_clk_venc_clk_init(struct device_node *np)
{
    struct axera_clock_data *clk_data_top;
    clk_data_top = axera_clk_init(np, AX650X_VENC_NR_CLKS);
    if (!clk_data_top)
        return;

    axera_clk_register_mux(ax650x_mux_clks_venc, ARRAY_SIZE(ax650x_mux_clks_venc), clk_data_top);
    axera_clk_register_gate(ax650x_gate_clks_venc, ARRAY_SIZE(ax650x_gate_clks_venc), clk_data_top);
}

CLK_OF_DECLARE(ax650x_clk_venc_glb, "axera,ax650x-venc-clk", ax650x_clk_venc_clk_init);

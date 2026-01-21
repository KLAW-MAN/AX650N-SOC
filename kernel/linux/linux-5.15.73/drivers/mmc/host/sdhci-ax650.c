/*
 * Copyright (C) 2020 Axera
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <linux/bitfield.h>
#include <linux/bitops.h>
#include <linux/iopoll.h>
#include <linux/module.h>
#include <linux/mmc/host.h>
#include <linux/mmc/mmc.h>
#include <linux/of.h>
#include <linux/reset.h>
#include "sdhci-pltfm.h"

#define DEASSERT 0
#define ASSERT 1

struct sdhci_axera_priv {
	struct clk *aclk;
	struct clk *hclk;
	struct clk *cardclk;
	struct reset_control *arst;
	struct reset_control *crst;
	struct reset_control *hrst;
	u8 dll_dc_step;
};

#define SDHCI_AXERA_200M_CLK (200000000UL)

#define SDHCI_AXERA_CTRL_HS400         0x0007
#define SDHCI_AXERA_PHY_BASE           0x300
#define AXERA_MAX_TUNING_LOOP          128
#define AXERA_MAX_DLLLOCK_LOOP         100
#define AXERA_DLL_FIXED_DC_STEP        50   /* 50ps */
#define AXERA_DLL_MAX_DC_STEPS         127  /* 127 steps */
#define SDHCI_AXERA_EMMC_CTRL_R        0x52C

/* PHY */
#define SDHCI_AXERA_PHY_CNFG           (0x00 + SDHCI_AXERA_PHY_BASE)
#define SDHCI_AXERA_PHY_CMDPAD_CNFG    (0x04 + SDHCI_AXERA_PHY_BASE)
#define SDHCI_AXERA_PHY_DATAPAD_CNFG   (0x06 + SDHCI_AXERA_PHY_BASE)
#define SDHCI_AXERA_PHY_CLKPAD_CNFG    (0x08 + SDHCI_AXERA_PHY_BASE)
#define SDHCI_AXERA_PHY_STBPAD_CNFG    (0x0A + SDHCI_AXERA_PHY_BASE)
#define SDHCI_AXERA_PHY_RSTNPAD_CNFG   (0x0C + SDHCI_AXERA_PHY_BASE)
#define SDHCI_AXERA_PHY_COMMDL_CNFG    (0x1C + SDHCI_AXERA_PHY_BASE)
#define SDHCI_AXERA_PHY_SDCLKDL_CNFG   (0x1D + SDHCI_AXERA_PHY_BASE)
#define SDHCI_AXERA_PHY_SDCLKDL_DC     (0x1E + SDHCI_AXERA_PHY_BASE)
#define SDHCI_AXERA_PHY_SMPLDL_CNFG    (0x20 + SDHCI_AXERA_PHY_BASE)
#define SDHCI_AXERA_PHY_ATDL_CNFG      (0x21 + SDHCI_AXERA_PHY_BASE)
#define SDHCI_AXERA_PHY_DLL_CTRL       (0x24 + SDHCI_AXERA_PHY_BASE)
#define SDHCI_AXERA_PHY_DLL_CNFG1      (0x25 + SDHCI_AXERA_PHY_BASE)
#define SDHCI_AXERA_PHY_DLL_CNFG2      (0x26 + SDHCI_AXERA_PHY_BASE)
#define SDHCI_AXERA_PHY_DLLDL_CNFG     (0x28 + SDHCI_AXERA_PHY_BASE)
#define SDHCI_AXERA_PHY_DLL_OFFSET     (0x29 + SDHCI_AXERA_PHY_BASE)
#define SDHCI_AXERA_PHY_DLLLBT_CNFG    (0x2C + SDHCI_AXERA_PHY_BASE)
#define SDHCI_AXERA_PHY_DLL_STATUS     (0x2E + SDHCI_AXERA_PHY_BASE)
#define SDHCI_AXERA_PHY_DLLDBG_MLKDC   (0x30 + SDHCI_AXERA_PHY_BASE)
#define SDHCI_AXERA_PHY_DLLDBG_SLKDC   (0x32 + SDHCI_AXERA_PHY_BASE)

#define SDHCI_AXERA_MSHC_P_VENDOR_SPECIFIC_AREA  0xe8    //size 16bits
#define EMMC_CTRL_R                      0x2c    //size 16bits, should DWC_MSHC_BASE + P_VENDOR_SPECIFIC_AREA[11:0] + 0x2c
#define BITS_SDHCI_AXERA_MSHC_P_VENDOR_SPECIFIC_AREA_REG_OFFSET_ADDR(x)    (((x) & 0xFFF) << 0)
#define BIT_SDHCI_AXERA_MSHC_EMMC_CTRL_EMMC_RST_N_OE           BIT(3)
#define BIT_SDHCI_AXERA_MSHC_EMMC_CTRL_EMMC_RST_N              BIT(2)

/* PHY_CNFG */
#define BIT_MSHC_PHY_CNFG_PHY_PWRGOOD       BIT(1)
#define BIT_MSHC_PHY_CNFG_PHY_RSTN          BIT(0)

/* RXSEL */
#define PHY_PAD_RXSEL_MASK                  0x7
#define PHY_PAD_RXSEL_1V8                   0x1

/*DRV STR*/
#define PHY_STRENGTH_MASK                   0xFF0000
#define PHY_STRENGTH_SHIFT                  16

/* DLL_CTRL */
#define BIT_SLV_SWDC_UPDATE                 BIT(2)
#define BIT_OFFST_EN                        BIT(1)
#define BIT_DLL_EN                          BIT(0)

/* DLL_STATUS */
#define BIT_DLL_ERROR_STS                   BIT(1)
#define BIT_DLL_LOCKED                      BIT(0)

/* DLLDL_CNFG */
#define BITS_SLV_INPSEL(x)                  ((x & 0x3) << 5)

/* EMMC_CTRL_R */
#define BIT_CARD_IS_EMMC                    BIT(0)
#define BIT_ENH_STROBE_EN                   BIT(8)



struct sdhci_axera_phy_cfg {
	const char *property;
	u16 addr;
	u8 reg_sz;
};

/*AX650X support emmc work in 3.3v/1.8v , a different PHY configuration is required. */
static const struct sdhci_axera_phy_cfg sdhci_axera_phy_cfgs[] = {
	{ "axera,phy-cnfg", SDHCI_AXERA_PHY_CNFG, sizeof(u32)},
	{ "axera,phy-cmdpad-cnfg", SDHCI_AXERA_PHY_CMDPAD_CNFG, sizeof(u16)},
	{ "axera,phy-datapad-cnfg", SDHCI_AXERA_PHY_DATAPAD_CNFG, sizeof(u16)},
	{ "axera,phy-clkpad-cnfg", SDHCI_AXERA_PHY_CLKPAD_CNFG, sizeof(u16)},
	{ "axera,phy-stbpad-cnfg", SDHCI_AXERA_PHY_STBPAD_CNFG, sizeof(u16)},
	{ "axera,phy-rstnpad-cnfg", SDHCI_AXERA_PHY_RSTNPAD_CNFG, sizeof(u16)},
	{ "axera,phy-commdl-cnfg", SDHCI_AXERA_PHY_COMMDL_CNFG, sizeof(u8)},
	{ "axera,phy-sdclkdl-cnfg", SDHCI_AXERA_PHY_SDCLKDL_CNFG, sizeof(u8)},
	{ "axera,phy-sdclkdl-dc", SDHCI_AXERA_PHY_SDCLKDL_DC, sizeof(u8)},
	{ "axera,phy-smpldl-cnfg", SDHCI_AXERA_PHY_SMPLDL_CNFG, sizeof(u8)},
	{ "axera,phy-atdl-cnfg", SDHCI_AXERA_PHY_ATDL_CNFG, sizeof(u8)},
};

static void sdhci_axera_enable_clk(struct sdhci_host *host, unsigned int clk);
static void sdhci_axera_disable_clk(struct sdhci_host *host);
extern bool __clk_is_enabled(struct clk *clk);
static int sdhci_axera_phy_init(struct sdhci_host *host)
{
	u8 temp_u8;
	u16 temp_u16;
	u32 temp_u32, prop_val;
	int ret, i;
	int cnt = 10;
	int reg;
	struct device_node *np = host->mmc->parent->of_node;
	pr_debug("%s sdhci_axera_phy_init called\n", host->hw_name);

	do {
		udelay(100);
		temp_u32 = sdhci_readl(host, SDHCI_AXERA_PHY_CNFG);
	} while (!(temp_u32 & BIT_MSHC_PHY_CNFG_PHY_PWRGOOD) && (cnt--));

	if (cnt <= 0) {
		pr_err("%s phy pwrgood is timeout\n", host->hw_name);
		return -1;
	}

	for (i = 0; i < ARRAY_SIZE(sdhci_axera_phy_cfgs); i++) {
		ret = of_property_read_u32(np, sdhci_axera_phy_cfgs[i].property,
							&prop_val);
		if (ret)
			continue;

		reg = (int)sdhci_axera_phy_cfgs[i].addr;

		switch (sdhci_axera_phy_cfgs[i].reg_sz) {
		case 1:
			temp_u8 = prop_val & 0xFF;
			sdhci_writeb(host, temp_u8, reg);
			break;
		case 2:
			temp_u16= prop_val & 0xFFFF;
			sdhci_writew(host, temp_u16, reg);
			break;
		case 4:
			sdhci_writel(host, prop_val, reg);
			break;
		default:
			ret = -1;
			return ret;
		}
	}

	udelay(10);

	//deassert reset
	temp_u32 = sdhci_readl(host, SDHCI_AXERA_PHY_CNFG);
	temp_u32 |= BIT_MSHC_PHY_CNFG_PHY_RSTN;
	sdhci_writel(host, temp_u32, SDHCI_AXERA_PHY_CNFG);

	return 0;
}

/*
 * before enhance_strobe_en and up clock to 200mhz, use fixed offset
 * to init dll delay:
 * 1. if not, when enhance_strobe bit set,
 *    host will sample response fail when send the first cmd13.
 * 2. the time enhance_strobe bit set, the clock is still 52mhz at most,
 *    cannot enable dll directly, the dll may lock fail
 */
static void sdhci_axera_init_dll(struct sdhci_host *host)
{
	/* sw update dc in progress */
	sdhci_writeb(host, BIT_SLV_SWDC_UPDATE, SDHCI_AXERA_PHY_DLL_CTRL);

	/* disable dll, enable offset */
	sdhci_writeb(host, (BIT_SLV_SWDC_UPDATE | BIT_OFFST_EN),
					SDHCI_AXERA_PHY_DLL_CTRL);
	/* set max offset*/
	sdhci_writeb(host, AXERA_DLL_MAX_DC_STEPS, SDHCI_AXERA_PHY_DLL_OFFSET);

	/* select clock source for slave dl */
	sdhci_writeb(host, BITS_SLV_INPSEL(3), SDHCI_AXERA_PHY_DLLDL_CNFG);

	/* sw update dc completed */
	sdhci_writeb(host, BIT_OFFST_EN, SDHCI_AXERA_PHY_DLL_CTRL);
}

#ifdef DEBUG_PHY_SET
#define DWC_MSHC_P_VENDOR_SPECIFIC_AREA  0xe8    //size 16bits
#define BITS_DWC_MSHC_P_VENDOR_SPECIFIC_AREA_REG_OFFSET_ADDR(x)    (((x) & 0xFFF) << 0)
#define AT_CTRL_R                      0x40    //size 32bits, should DWC_MSHC_BASE + P_VENDOR_SPECIFIC_AREA[11:0] + 0x40
#define AT_STAT_R                      0x44    //size 32bits, should DWC_MSHC_BASE + P_VENDOR_SPECIFIC_AREA[11:0] + 0x44
static void read_phy_reg(struct sdhci_host *host)
{
	u16 vendor_ptr;
	u32 at_stat_r;
	vendor_ptr = sdhci_readw(host, DWC_MSHC_P_VENDOR_SPECIFIC_AREA); //0xe8
	vendor_ptr &= BITS_DWC_MSHC_P_VENDOR_SPECIFIC_AREA_REG_OFFSET_ADDR(0xFFF);
	printk("%s: SDHCI_AXERA_PHY_CNFG        : 0x%x\n", host->hw_name, sdhci_readl(host, (0x00 + SDHCI_AXERA_PHY_BASE)));
	printk("%s: SDHCI_AXERA_PHY_CMDPAD_CNFG : 0x%x\n", host->hw_name, sdhci_readw(host, (0x04 + SDHCI_AXERA_PHY_BASE)));
	printk("%s: SDHCI_AXERA_PHY_DATAPAD_CNFG: 0x%x\n", host->hw_name, sdhci_readw(host, (0x06 + SDHCI_AXERA_PHY_BASE)));
	printk("%s: SDHCI_AXERA_PHY_CLKPAD_CNFG : 0x%x\n", host->hw_name, sdhci_readw(host, (0x08 + SDHCI_AXERA_PHY_BASE)));
	printk("%s: SDHCI_AXERA_PHY_STBPAD_CNFG : 0x%x\n", host->hw_name, sdhci_readw(host, (0x0A + SDHCI_AXERA_PHY_BASE)));
	printk("%s: SDHCI_AXERA_PHY_RSTNPAD_CNFG: 0x%x\n", host->hw_name, sdhci_readw(host, (0x0C + SDHCI_AXERA_PHY_BASE)));
	printk("%s: SDHCI_AXERA_PHY_COMMDL_CNFG : 0x%x\n", host->hw_name, sdhci_readb(host, (0x1C + SDHCI_AXERA_PHY_BASE)));
	printk("%s: SDHCI_AXERA_PHY_SDCLKDL_CNFG: 0x%x\n", host->hw_name, sdhci_readb(host, (0x1D + SDHCI_AXERA_PHY_BASE)));
	printk("%s: SDHCI_AXERA_PHY_SDCLKDL_DC  : 0x%x\n", host->hw_name, sdhci_readb(host, (0x1E + SDHCI_AXERA_PHY_BASE)));
	printk("%s: SDHCI_AXERA_PHY_SMPLDL_CNFG : 0x%x\n", host->hw_name, sdhci_readb(host, (0x20 + SDHCI_AXERA_PHY_BASE)));
	printk("%s: SDHCI_AXERA_PHY_ATDL_CNFG   : 0x%x\n", host->hw_name, sdhci_readb(host, (0x21 + SDHCI_AXERA_PHY_BASE)));
	printk("%s: SDHCI_AXERA_PHY_DLL_CTRL    : 0x%x\n", host->hw_name, sdhci_readb(host, (0x24 + SDHCI_AXERA_PHY_BASE)));
	printk("%s: SDHCI_AXERA_PHY_DLL_CNFG1   : 0x%x\n", host->hw_name, sdhci_readb(host, (0x25 + SDHCI_AXERA_PHY_BASE)));
	printk("%s: SDHCI_AXERA_PHY_DLL_CNFG2   : 0x%x\n", host->hw_name, sdhci_readb(host, (0x26 + SDHCI_AXERA_PHY_BASE)));
	printk("%s: SDHCI_AXERA_PHY_DLLDL_CNFG  : 0x%x\n", host->hw_name, sdhci_readb(host, (0x28 + SDHCI_AXERA_PHY_BASE)));
	printk("%s: SDHCI_AXERA_PHY_DLL_OFFSET  : 0x%x\n", host->hw_name, sdhci_readb(host, (0x29 + SDHCI_AXERA_PHY_BASE)));
	printk("%s: SDHCI_AXERA_PHY_DLLLBT_CNFG : 0x%x\n", host->hw_name, sdhci_readw(host, (0x2C + SDHCI_AXERA_PHY_BASE)));
	printk("%s: SDHCI_AXERA_PHY_DLL_STATUS  : 0x%x\n", host->hw_name, sdhci_readb(host, (0x2E + SDHCI_AXERA_PHY_BASE)));
	printk("%s: SDHCI_AXERA_PHY_DLLDBG_MLKDC: 0x%x\n", host->hw_name, sdhci_readb(host, (0x30 + SDHCI_AXERA_PHY_BASE)));
	printk("%s: SDHCI_AXERA_PHY_DLLDBG_SLKDC: 0x%x\n", host->hw_name, sdhci_readb(host, (0x32 + SDHCI_AXERA_PHY_BASE)));
	/* read AT_STAT_R.CENTER_PH_CODE which is phy#1 delayline2 delay code for rx sample. */
	at_stat_r = sdhci_readl(host, vendor_ptr + AT_STAT_R);
	printk("%s:AT_STAT_R:0x%x, vendor_ptr:0x%x\n", host->hw_name, at_stat_r, vendor_ptr);
}
#endif
/* when HS400 need enable dll */
static int sdhci_axera_enable_dll(struct sdhci_host *host)
{
	int i;
	int lock_stat = 0;
	unsigned long clk;
	struct sdhci_pltfm_host *pltfm_host = sdhci_priv(host);
	struct sdhci_axera_priv *priv = sdhci_pltfm_priv(pltfm_host);

	/* source clk freq then disable clk*/
	clk = clk_get_rate(priv->cardclk);
	sdhci_axera_disable_clk(host);
	sdhci_writeb(host, 0x20, SDHCI_AXERA_PHY_DLL_CNFG1);
	sdhci_writeb(host, 1, SDHCI_AXERA_PHY_DLL_CNFG2);
	sdhci_writeb(host, BITS_SLV_INPSEL(3), SDHCI_AXERA_PHY_DLLDL_CNFG);
	sdhci_writew(host, 0x0, SDHCI_AXERA_PHY_DLLLBT_CNFG);

	sdhci_axera_enable_clk(host, clk);
	/* enable dll */
	sdhci_writeb(host, BIT_DLL_EN, SDHCI_AXERA_PHY_DLL_CTRL);

	for (i = 0; i < AXERA_MAX_DLLLOCK_LOOP; i++) {
		if (sdhci_readb(host, SDHCI_AXERA_PHY_DLL_STATUS) & BIT_DLL_LOCKED) {
			if (!(sdhci_readb(host, SDHCI_AXERA_PHY_DLL_STATUS) & BIT_DLL_ERROR_STS)) {
				lock_stat = 1;
				pr_debug("%s axera sdhci dll lock success\n", host->hw_name);
				break;
			}
		}
		mdelay(1);
	}

	if (!lock_stat) {
		pr_err("%s axera sdhci dll lock fail, fixed dll used\n", host->hw_name);
		return -1;
	}

	return 0;
}

/* enable clock:
 * 1. SDHCI_CLOCK_INT_EN in SDHCI_CLOCK_CONTROL
 * 2. set clk in clk_set_rate
 * 3. enable clk
 */
static void sdhci_axera_enable_clk(struct sdhci_host *host, unsigned int clk)
{
	ktime_t timeout;
	struct sdhci_pltfm_host *pltfm_host = sdhci_priv(host);
	struct sdhci_axera_priv *priv = sdhci_pltfm_priv(pltfm_host);
	u16 temp = 0;
	if (IS_ERR(priv->cardclk)) {
		pr_err("%s priv->cardclk is err\n", __func__);
		return ;
	}
	clk_set_rate(priv->cardclk, clk);
	if (!__clk_is_enabled(priv->cardclk))
		clk_prepare_enable(priv->cardclk);

	temp = sdhci_readw(host, SDHCI_CLOCK_CONTROL);
	temp |= SDHCI_CLOCK_INT_EN;
	/* AX650X: div FREQ_SEL[15:8] inside the controller is invalid and does not need to be set. */
	sdhci_writew(host, temp, SDHCI_CLOCK_CONTROL);

	/* Wait max 20 ms */
	timeout = ktime_add_ms(ktime_get(), 20);
	while (1) {
		bool timedout = ktime_after(ktime_get(), timeout);

		clk = sdhci_readw(host, SDHCI_CLOCK_CONTROL);
		if (clk & SDHCI_CLOCK_INT_STABLE)
			break;
		if (timedout) {
			pr_err("%s: Internal clock never stabilised.\n",
			       mmc_hostname(host->mmc));
			sdhci_dumpregs(host);
			return;
		}
		udelay(10);
	}

	temp |= SDHCI_CLOCK_CARD_EN;
	sdhci_writew(host, temp, SDHCI_CLOCK_CONTROL);

}

static void sdhci_axera_disable_clk(struct sdhci_host *host)
{
	u16 clk;
	struct sdhci_pltfm_host *pltfm_host = sdhci_priv(host);
	struct sdhci_axera_priv *priv = sdhci_pltfm_priv(pltfm_host);

	if (__clk_is_enabled(priv->cardclk)) {
		clk_set_rate(priv->cardclk, 0);
		clk_disable_unprepare(priv->cardclk);
	}
	clk = sdhci_readw(host, SDHCI_CLOCK_CONTROL);
	clk &= ~SDHCI_CLOCK_CARD_EN;
	sdhci_writew(host, clk, SDHCI_CLOCK_CONTROL);
}

#define SDHCI_AXERA_MAX_DIV	60
#define SDHCI_AXERA_EXTDLY_EN 0x1
void sdhci_axera_set_clock(struct sdhci_host *host, unsigned int clock)
{
	int i;
	unsigned int div;
	int div_flag = 0;
	u32 temp = 0;
	u8 val;
	struct sdhci_pltfm_host *pltfm_host = sdhci_priv(host);
	struct sdhci_axera_priv *priv = sdhci_pltfm_priv(pltfm_host);
	unsigned long clk_src[4] = {12000000, 125000000, 200000000};

	host->mmc->actual_clock = 0;
	pr_debug("%s sdhci_axera set clock freq:%u\n", host->hw_name, clock);
	sdhci_axera_disable_clk(host);

	if (clock == 0)
		return;

	/*only 5 bits of div used in host*/
	for (i = 2; i >= 0; i--) {
		div_flag = 0;
		if (clk_src[i] <= clock) {
			div = 1;
			div_flag = 1;
			break;
		} else if ((clk_src[i] / SDHCI_AXERA_MAX_DIV) > clock) {
			continue;
		}

		for (div = 2; div <= SDHCI_AXERA_MAX_DIV; div += 2) {
			if ((clk_src[i] / div) <= clock) {
				div_flag = 1;
				break;
			}
		}

		if (div_flag == 1)
			break;
	}
	if (div_flag != 1) {
		pr_err("%s not find suitable clock div\n", host->hw_name);
		return;
	}

	host->mmc->actual_clock = clk_src[i] / (div);

	/* if freq is low than 25Mhz, set max dc count
	 */
	if (host->mmc->actual_clock < 25000000) {
		temp = AXERA_DLL_MAX_DC_STEPS;
	} else {
		/* if high freq need reset tx delay line dc
		 * ddr 1/4 cycle, sdr 1/2 cycle
		 */
		temp = host->mmc->actual_clock / 1000000;
		temp = 1000000 / temp; /* cycle in ps */
		if (host->timing == MMC_TIMING_UHS_DDR50 ||
			host->timing == MMC_TIMING_MMC_DDR52 ||
			host->timing == MMC_TIMING_MMC_HS400) {
			/* 1/4 cycle multiply coefficient*/
			temp = temp / 4 / priv->dll_dc_step;
		} else {
			temp = temp / 2 / priv->dll_dc_step;
		}

		if (temp > AXERA_DLL_MAX_DC_STEPS)
			temp = AXERA_DLL_MAX_DC_STEPS;
	}

	/*
	* sdcard low speed nend to set fix delay, sd legacy use MMC_TIMING_LEGACY.
	* emmc low speed nend to set fix delay.
	* MMC_TIMING_MMC_HS do not need to set fix delay, in hs400 mode tuning follow, mmc_hs400_to_hs200 will switch 50M(hs) to send cmd and recive resp.
	* delay ceils(full 6.4ns) + fix delay(6.4ns) + todly(max 13.7ns), respons may not be received.
	*/
	if (host->timing == MMC_TIMING_SD_HS || host->timing == MMC_TIMING_UHS_SDR12 || host->timing == MMC_TIMING_UHS_SDR25 || \
				host->timing == MMC_TIMING_LEGACY) {
		val = sdhci_readb(host, SDHCI_AXERA_PHY_SDCLKDL_CNFG);
		val |= SDHCI_AXERA_EXTDLY_EN;
		sdhci_writeb(host, val, SDHCI_AXERA_PHY_SDCLKDL_CNFG);
	} else {
		val = sdhci_readb(host, SDHCI_AXERA_PHY_SDCLKDL_CNFG);
		val &= ~SDHCI_AXERA_EXTDLY_EN;
		sdhci_writeb(host, val, SDHCI_AXERA_PHY_SDCLKDL_CNFG);
	}

	/*
	 * Drives CCLK_RX DelayLine's config input. Value here selects the input source to DelayLine.
	 * SDR104， HS200, HS400 select 3, others select 2.
	 * */
	if (host->timing != MMC_TIMING_UHS_SDR104 && host->timing != MMC_TIMING_MMC_HS200 && host->timing != MMC_TIMING_MMC_HS400) {
		val = 0x8;
		sdhci_writeb(host, val, SDHCI_AXERA_PHY_SMPLDL_CNFG);
	} else {
		val = 0xC;
		sdhci_writeb(host, val, SDHCI_AXERA_PHY_SMPLDL_CNFG);
	}

	sdhci_writeb(host, (u8)temp, SDHCI_AXERA_PHY_SDCLKDL_DC);
	pr_debug("%s set dc:%u at real freq:%u, host->timing:%d\n", host->hw_name, temp, host->mmc->actual_clock, host->timing);
	sdhci_axera_enable_clk(host, host->mmc->actual_clock);

	if (host->mmc->ios.enhanced_strobe == true)
		sdhci_axera_enable_dll(host);

	return;
}

void sdhci_axera_reset(struct sdhci_host *host, u8 mask)
{
	u16 temp;

	sdhci_reset(host, mask);


	if (mask == SDHCI_RESET_ALL) {
		pr_debug("%s axera sdhci reset all\n", host->hw_name);

		/* fixed bug: Before Phy reset is released, the signal io output voltage needs to be configured to 1.8V, otherwise there will be 3.3V glitches (the default is 3.3V). */
		sdhci_writew(host, SDHCI_CTRL_VDD_180, SDHCI_HOST_CONTROL2);

		if ((host->mmc->caps2 & MMC_CAP2_NO_SDIO) &&
			(host->mmc->caps2 & MMC_CAP2_NO_SD)) {
			pr_debug("%s axera sdhci reset is_emmc bit\n", host->hw_name);
			temp = sdhci_readw(host, SDHCI_AXERA_EMMC_CTRL_R);
			temp |= BIT_CARD_IS_EMMC;
			sdhci_writew(host, temp, SDHCI_AXERA_EMMC_CTRL_R);

			sdhci_axera_init_dll(host);
		}

		sdhci_axera_phy_init(host);
	}
}

int	sdhci_axera_prepare_hs400_tuning(struct mmc_host *mmc_host,
				struct mmc_ios *ios)
{
	struct sdhci_host *host = (struct sdhci_host *)mmc_priv(mmc_host);
	unsigned long flags;

	spin_lock_irqsave(&host->lock, flags);
	host->flags |= SDHCI_HS400_TUNING;
	spin_unlock_irqrestore(&host->lock, flags);

	sdhci_axera_enable_dll(host);

	return 0;
}

static void sdhci_axera_hs400_enhanced_strobe(struct mmc_host *mmc,
	struct mmc_ios *ios)
{
	struct sdhci_host *host = mmc_priv(mmc);
	u16 temp;

	pr_debug("%s axera enhanced strobe:%d\n", host->hw_name, ios->enhanced_strobe);
	temp = sdhci_readw(host, SDHCI_AXERA_EMMC_CTRL_R);

	if (ios->enhanced_strobe)
		temp |= BIT_ENH_STROBE_EN;
	else
		temp &= ~BIT_ENH_STROBE_EN;

	sdhci_writew(host, temp, SDHCI_AXERA_EMMC_CTRL_R);
}

void sdhci_axera_set_uhs_signaling(struct sdhci_host *host,
	unsigned timing)
{
	u16 ctrl_2;

	pr_debug("%s axera sdhci set timing: %u\n", host->hw_name, timing);
	ctrl_2 = sdhci_readw(host, SDHCI_HOST_CONTROL2);
	/* Select Bus Speed Mode for host */
	ctrl_2 &= ~SDHCI_CTRL_UHS_MASK;
	if ((timing == MMC_TIMING_MMC_HS200) ||
	    (timing == MMC_TIMING_UHS_SDR104))
		ctrl_2 |= SDHCI_CTRL_UHS_SDR104;
	else if (timing == MMC_TIMING_UHS_SDR12)
		ctrl_2 |= SDHCI_CTRL_UHS_SDR12;
	else if (timing == MMC_TIMING_UHS_SDR25)
		ctrl_2 |= SDHCI_CTRL_UHS_SDR25;
	else if (timing == MMC_TIMING_UHS_SDR50)
		ctrl_2 |= SDHCI_CTRL_UHS_SDR50;
	else if ((timing == MMC_TIMING_UHS_DDR50) ||
		 (timing == MMC_TIMING_MMC_DDR52))
		ctrl_2 |= SDHCI_CTRL_UHS_DDR50;
	else if (timing == MMC_TIMING_MMC_HS400)
		ctrl_2 |= SDHCI_AXERA_CTRL_HS400; /* AXERA is 0x7 */
	sdhci_writew(host, ctrl_2, SDHCI_HOST_CONTROL2);
}

static void sdhci_axera_abort_tuning(struct sdhci_host *host, u32 opcode)
{
	sdhci_reset_tuning(host);

	sdhci_axera_reset(host, SDHCI_RESET_CMD);
	sdhci_axera_reset(host, SDHCI_RESET_DATA);

	sdhci_end_tuning(host);

	mmc_send_abort_tuning(host->mmc, opcode);
}

static void _sdhci_axera_excute_tuning(struct sdhci_host *host, u32 opcode)
{
	int i;

	/*
	 * Issue opcode repeatedly till Execute Tuning is set to 0 or the number
	 * of loops reaches 128 times.
	 */
	for (i = 0; i < AXERA_MAX_TUNING_LOOP; i++) {
		u16 ctrl;

		sdhci_send_tuning(host, opcode);

		if (!host->tuning_done) {
			pr_err("%s: Tuning timeout, falling back to fixed sampling clock\n",
				 mmc_hostname(host->mmc));
			sdhci_axera_abort_tuning(host, opcode);
			return;
		}

		ctrl = sdhci_readw(host, SDHCI_HOST_CONTROL2);
		if (!(ctrl & SDHCI_CTRL_EXEC_TUNING)) {
			if (ctrl & SDHCI_CTRL_TUNED_CLK) {
				pr_info("axera sdhci tuned success\n");
				return; /* Success! */
			}
			break;
		}

		/* Spec does not require a delay between tuning cycles */
		if (host->tuning_delay > 0)
			mdelay(host->tuning_delay);
	}

	pr_err("%s: Tuning failed, falling back to fixed sampling clock\n",
		mmc_hostname(host->mmc));
	sdhci_reset_tuning(host);
}

static int sdhci_axera_excute_tuning(struct sdhci_host *host, u32 opcode)
{

	u32 tuning_count = 0;
	bool hs400_tuning;

	pr_debug("%s axera sdhci excute_tuning\n", host->hw_name);
	hs400_tuning = host->flags & SDHCI_HS400_TUNING;

	if (host->tuning_mode == SDHCI_TUNING_MODE_1)
		tuning_count = host->tuning_count;

	if ((host->timing == MMC_TIMING_MMC_HS200) && hs400_tuning)
		tuning_count = 0;

	host->mmc->retune_period = tuning_count;

	if (host->tuning_delay < 0)
		host->tuning_delay = opcode == MMC_SEND_TUNING_BLOCK;

	sdhci_start_tuning(host);

	_sdhci_axera_excute_tuning(host, opcode);

	sdhci_end_tuning(host);

	return 0;
}

static void sdhci_axera_try_dc_test(struct sdhci_host *host)
{
	int ret;
	u8 lock_dc;
	struct sdhci_pltfm_host *pltfm_host = sdhci_priv(host);
	struct sdhci_axera_priv *priv = sdhci_pltfm_priv(pltfm_host);

	sdhci_reset(host, SDHCI_RESET_ALL);
	/* set 200mhz-5ns/cycle and test dll*/
	ret = sdhci_axera_enable_dll(host);
	if (!ret) {
		lock_dc = sdhci_readb(host, SDHCI_AXERA_PHY_DLLDBG_MLKDC);
		priv->dll_dc_step = 5000 / lock_dc;
		pr_err("%s axera sdhci_axera_try_dc_test success, dc_step:%u, MLKDC:%u, SLKDC:%u\n",
			host->hw_name, priv->dll_dc_step, lock_dc, sdhci_readb(host, SDHCI_AXERA_PHY_DLLDBG_SLKDC));
	} else {
		pr_err("%s axera sdhci_axera_try_dc_test fail\n", host->hw_name);
		priv->dll_dc_step = AXERA_DLL_FIXED_DC_STEP;
	}

	sdhci_writew(host, 0, SDHCI_CLOCK_CONTROL); /* dis clock */
}

/*
 * for uhs-sd voltage switch need change phy rxsel setting
 */
static void sdhci_axera_voltage_switch(struct sdhci_host *host)
{
	u16 temp;
	u32 temp_u32, prop_val;
	int ret;
	struct device_node *np = host->mmc->parent->of_node;

	pr_debug("%s voltage switch to 1.8V\n", host->hw_name);

	/* CMD & DATA pad switch to 1.8V */
	temp = sdhci_readw(host, SDHCI_AXERA_PHY_CMDPAD_CNFG) & ~PHY_PAD_RXSEL_MASK;
	temp |= PHY_PAD_RXSEL_1V8;
	sdhci_writew(host, temp, SDHCI_AXERA_PHY_CMDPAD_CNFG);

	temp = sdhci_readw(host, SDHCI_AXERA_PHY_DATAPAD_CNFG) & ~PHY_PAD_RXSEL_MASK;
	temp |= PHY_PAD_RXSEL_1V8;
	sdhci_writew(host, temp, SDHCI_AXERA_PHY_DATAPAD_CNFG);

	/* driver strength switch for sd uhs mode */
	ret = of_property_read_u32(np, "axera,swt1v8-drv-str", &prop_val);
	if (!ret) {
		temp_u32 = sdhci_readl(host, SDHCI_AXERA_PHY_CNFG) & ~PHY_STRENGTH_MASK;
		temp_u32 |= (prop_val << PHY_STRENGTH_SHIFT);
		sdhci_writel(host, temp_u32, SDHCI_AXERA_PHY_CNFG);
	}
}

static void	sdhci_axera_hw_reset(struct sdhci_host *host)
{
	u16 temp_u16, vendor_ptr;

	vendor_ptr = sdhci_readw(host, SDHCI_AXERA_MSHC_P_VENDOR_SPECIFIC_AREA); //0xe8
	vendor_ptr &= BITS_SDHCI_AXERA_MSHC_P_VENDOR_SPECIFIC_AREA_REG_OFFSET_ADDR(0xFFF);
	temp_u16 = sdhci_readw(host,vendor_ptr + EMMC_CTRL_R);
	//rst_n output enable
	temp_u16 |= BIT_SDHCI_AXERA_MSHC_EMMC_CTRL_EMMC_RST_N_OE; //bit3
	sdhci_writew(host, temp_u16, vendor_ptr + EMMC_CTRL_R);

	//generate rising edge to rst
	temp_u16 &= ~BIT_SDHCI_AXERA_MSHC_EMMC_CTRL_EMMC_RST_N;
	sdhci_writew(host, temp_u16, vendor_ptr + EMMC_CTRL_R);
	udelay(50);
	temp_u16 |= BIT_SDHCI_AXERA_MSHC_EMMC_CTRL_EMMC_RST_N;
	sdhci_writew(host, temp_u16, vendor_ptr + EMMC_CTRL_R);
	//as spec card need longest 1ms
	udelay(500);

}

void sdhci_axera_set_power(struct sdhci_host *host, unsigned char mode, unsigned short vdd)
{
	u32 temp_u32;
	if (mode == MMC_POWER_OFF) {
		/* need to set the phy to reset state, otherwise configuring IP power off will make the pad power supply become 3.3v. */
		/* assert reset */
		temp_u32 = sdhci_readl(host, SDHCI_AXERA_PHY_CNFG);
		temp_u32 &= ~BIT_MSHC_PHY_CNFG_PHY_RSTN;
		sdhci_writel(host, temp_u32, SDHCI_AXERA_PHY_CNFG);
	}

	sdhci_set_power(host, mode, vdd);
	if (mode == MMC_POWER_UP) {
		//deassert reset
		temp_u32 = sdhci_readl(host, SDHCI_AXERA_PHY_CNFG);
		temp_u32 |= BIT_MSHC_PHY_CNFG_PHY_RSTN;
		sdhci_writel(host, temp_u32, SDHCI_AXERA_PHY_CNFG);
	}
}

static const struct sdhci_ops sdhci_axera_ops = {
	.set_clock = sdhci_axera_set_clock,
	.set_bus_width = sdhci_set_bus_width,
	.reset = sdhci_axera_reset,
	.hw_reset = sdhci_axera_hw_reset,
	.set_power = sdhci_axera_set_power,
	.set_uhs_signaling = sdhci_axera_set_uhs_signaling,
	.platform_execute_tuning = sdhci_axera_excute_tuning,
	.voltage_switch = sdhci_axera_voltage_switch,
};

static const struct sdhci_pltfm_data sdhci_axera_pltfm_data = {
	.ops = &sdhci_axera_ops,
	.quirks = SDHCI_QUIRK_SINGLE_POWER_WRITE,
	.quirks2 = SDHCI_QUIRK2_PRESET_VALUE_BROKEN,
};

static struct mmc_host *sdio_host = NULL;

void ax650_sdio_rescan(void)
{
	if (!sdio_host) {
		pr_err("ax650 sdio_host is NULL\n");
		return;
	}

	pr_debug("ax650 sdio_host rescan begin\n");

	mmc_detect_change(sdio_host, 0);
}
EXPORT_SYMBOL_GPL(ax650_sdio_rescan);


static int axera_get_clk_reset(struct platform_device *pdev, struct sdhci_axera_priv *priv)
{
	priv->arst = devm_reset_control_get_optional(&pdev->dev, "arst");
	if (IS_ERR(priv->arst)) {
		pr_err("couldn't get the priv->arst\n");
		return -1;
	}

	priv->crst = devm_reset_control_get_optional(&pdev->dev, "crst");
	if (IS_ERR(priv->crst)) {
		pr_err("couldn't get the priv->crst\n");
		return -1;
	}

	priv->hrst = devm_reset_control_get_optional(&pdev->dev, "hrst");
	if (IS_ERR(priv->hrst)) {
		pr_err("couldn't get the priv->hrst\n");
		return -1;
	}
	return 0;
}

static int axera_clk_reset_control(struct sdhci_axera_priv *priv, int flag)
{
	if (IS_ERR(priv->arst))
		return PTR_ERR(priv->arst);
	if (IS_ERR(priv->crst))
		return PTR_ERR(priv->crst);
	if (IS_ERR(priv->hrst))
		return PTR_ERR(priv->hrst);

	if (flag == DEASSERT) {
		reset_control_deassert(priv->arst);
		reset_control_deassert(priv->crst);
		reset_control_deassert(priv->hrst);
	}
	if (flag == ASSERT) {
		reset_control_assert(priv->arst);
		reset_control_assert(priv->crst);
		reset_control_assert(priv->hrst);
	}
	return 0;
}

static int axera_get_clk(struct platform_device *pdev, struct sdhci_axera_priv *priv)
{
	priv->aclk = devm_clk_get(&pdev->dev, "aclk");
	if (IS_ERR(priv->aclk)) {
		pr_err("%s: priv->aclk get fail\n", __func__);
		return -1;
	}
	priv->hclk = devm_clk_get(&pdev->dev, "hclk");
	if (IS_ERR(priv->hclk)) {
		pr_err("%s: priv->hclk get fail\n", __func__);
		return -1;
	}

	priv->cardclk = devm_clk_get(&pdev->dev, "cardclk");
	if (IS_ERR(priv->cardclk)) {
		pr_err("%s priv->cardclk get fail\n", __func__);
		return -1;
	}

	return 0;
}

static int axera_prepare_clk(struct sdhci_axera_priv *priv, bool prepare)
{
	int ret;

	if (IS_ERR(priv->aclk))
		return PTR_ERR(priv->aclk);
	if (IS_ERR(priv->hclk))
		return PTR_ERR(priv->hclk);
	if (IS_ERR(priv->cardclk))
		return PTR_ERR(priv->cardclk);

	if (prepare) {
		ret = clk_prepare_enable(priv->aclk);
		if (ret)
			return ret;
		ret = clk_prepare_enable(priv->hclk);
		if (ret)
			return ret;

		clk_set_rate(priv->cardclk, SDHCI_AXERA_200M_CLK);
		ret = clk_prepare_enable(priv->cardclk);
		if (ret)
			return ret;

		return ret;
	}
	clk_disable_unprepare(priv->aclk);
	clk_disable_unprepare(priv->hclk);
	clk_disable_unprepare(priv->cardclk);

	return 0;

}

static int sdhci_axera_probe(struct platform_device *pdev)
{
	struct sdhci_host *host;
	int ret;
	size_t priv_size;
	struct sdhci_axera_priv *priv;
	struct sdhci_pltfm_host *pltfm_host;

	priv_size = sizeof(struct sdhci_axera_priv);
	host = sdhci_pltfm_init(pdev, &sdhci_axera_pltfm_data, priv_size);
	if (IS_ERR(host)) {
		dev_err(&pdev->dev, "sdhci_pltfm_init fail\n");
		ret = PTR_ERR(host);
		goto ret_err;
	}

	pltfm_host = sdhci_priv(host);
	host->mmc_host_ops.prepare_hs400_tuning =
				sdhci_axera_prepare_hs400_tuning;
	host->mmc_host_ops.hs400_enhanced_strobe =
				sdhci_axera_hs400_enhanced_strobe;

	priv = sdhci_pltfm_priv(pltfm_host);
	/*dll dc step default */
	priv->dll_dc_step = AXERA_DLL_FIXED_DC_STEP;

	ret = axera_get_clk_reset(pdev, priv);
	if (ret)
		goto free;
	ret = axera_clk_reset_control(priv, DEASSERT);
	if (ret)
		goto free;

	ret = axera_get_clk(pdev, priv);
	if (ret)
		goto free;

	ret = axera_prepare_clk(priv, true);
	if (ret)
		goto free;

	sdhci_get_of_property(pdev);

	ret = mmc_of_parse(host->mmc);
	if (ret) {
		pr_err("%s: mmc_of_parse fail\n", host->hw_name);
		goto disable_clk;
	}

	host->mmc->ocr_avail = MMC_VDD_32_33 | MMC_VDD_31_32 | MMC_VDD_30_31 | MMC_VDD_29_30 | MMC_VDD_28_29 | MMC_VDD_27_28 | MMC_VDD_165_195;

	if ((host->mmc->caps2 & MMC_CAP2_NO_SDIO) &&
		(host->mmc->caps2 & MMC_CAP2_NO_SD)){
		sdhci_axera_try_dc_test(host);
		if (device_property_present(&pdev->dev, "no-1-8-v"))
			host->flags &= ~(SDHCI_SIGNALING_180 | SDHCI_SIGNALING_120);
		else
			host->flags &= ~(SDHCI_SIGNALING_330 | SDHCI_SIGNALING_120);
	}

	ret = sdhci_add_host(host);
	if (ret) {
		pr_err("%s: sdhci_add_host fail\n", host->hw_name);
		goto disable_clk;
	}
	/* SDR50 support tuning */
	host->flags |= SDHCI_SDR50_NEEDS_TUNING;
	/* if sdio device, save mmc_host pointer */
	if ((host->mmc->caps2 & MMC_CAP2_NO_MMC) &&
		(host->mmc->caps2 & MMC_CAP2_NO_SD)) {
		sdio_host = host->mmc;
	}
	ret = sdhci_axera_phy_init(host);
	if (ret)
		goto disable_clk;
	return 0;
disable_clk:
	axera_clk_reset_control(priv, ASSERT);
	axera_prepare_clk(priv, false);

free:
	sdhci_pltfm_free(pdev);

ret_err:
	return ret;
}

#ifdef CONFIG_PM_SLEEP
static int sdhci_axera_resume(struct device *dev)
{
	int ret;
	u16 temp;
	struct sdhci_host *host = dev_get_drvdata(dev);
	struct sdhci_pltfm_host *pltfm_host = sdhci_priv(host);
	struct sdhci_axera_priv *priv = sdhci_pltfm_priv(pltfm_host);

	ret = sdhci_resume_host(host);
	if (ret)
		goto disable_clk;

	ret = sdhci_axera_phy_init(host);
	if (ret)
		goto disable_clk;

	if ((host->mmc->caps2 & MMC_CAP2_NO_SDIO) &&
		(host->mmc->caps2 & MMC_CAP2_NO_SD)) {
		temp = sdhci_readw(host, SDHCI_AXERA_EMMC_CTRL_R);
		temp |= BIT_CARD_IS_EMMC;
		sdhci_writew(host, temp, SDHCI_AXERA_EMMC_CTRL_R);

		sdhci_axera_init_dll(host);
	}
	ret = axera_clk_reset_control(priv, DEASSERT);
	if (ret)
		goto disable_clk;

	ret = axera_prepare_clk(priv, true);
	if (ret)
		return ret;

	return 0;

disable_clk:
	axera_prepare_clk(priv, false);

	return ret;
}


int sdhci_axera_suspend(struct device *dev)
{
	struct sdhci_host *host = dev_get_drvdata(dev);
	struct sdhci_pltfm_host *pltfm_host = sdhci_priv(host);
	struct sdhci_axera_priv *priv = sdhci_pltfm_priv(pltfm_host);
	int ret;
	if (host->tuning_mode != SDHCI_TUNING_MODE_3)
		mmc_retune_needed(host->mmc);

	ret = sdhci_suspend_host(host);
	if (ret)
		return ret;

	axera_prepare_clk(priv, false);
	axera_clk_reset_control(priv, ASSERT);

	return 0;
}
#endif

static const struct dev_pm_ops sdhci_axera_pm_ops = {
	SET_SYSTEM_SLEEP_PM_OPS(sdhci_axera_suspend, sdhci_axera_resume)
};

static const struct of_device_id sdhci_axera_match[] = {
	{ .compatible = "axera,sdhc-ax650" },
	{ /* others */ }
};
MODULE_DEVICE_TABLE(of, sdhci_axera_match);

static struct platform_driver sdhci_axera_driver = {
	.driver = {
		.name = "sdhci-axera",
		.pm = &sdhci_axera_pm_ops,
		.of_match_table = sdhci_axera_match,
	},
	.probe = sdhci_axera_probe,
	.remove = sdhci_pltfm_unregister,
};
module_platform_driver(sdhci_axera_driver);

MODULE_AUTHOR("King <wuqianlong@axera-tech.com>");
MODULE_DESCRIPTION("Axera AX650 SD/SDIO/eMMC Host Controller Driver");
MODULE_LICENSE("GPL");

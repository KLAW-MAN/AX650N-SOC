/*
 * (C) Copyright 2020 AXERA Co., Ltd
 * SPDX-License-Identifier:     GPL-2.0+
 */
#include <asm/io.h>
#include <common.h>
#include <asm/arch/ax650.h>
#include <asm/arch/boot_mode.h>
#include <configs/ax650_emmc.h>

#define DWC_MSHC_EMMC_BASE                    0x28000000
#define DWC_MSHC_SD_BASE                      0x30E00000
#define DWC_MSHC_SDIO_BASE                    0x28001000
#define PINMUX_ADDR_MASK                      0x4251F00
#define EMMC_PINMUX_ADDR                      0x4251E00
#define SD_PINMUX_ADDR                        0x4251A00
#define SDIO_PINMUX_ADDR                      0x4251C00

extern int phy_func_en(int pinmux_addr);
extern int phy_setting(u32 ip_addr, int pad_vol);
extern unsigned char get_chip_type_id(void);

unsigned int ax650A_demo_pinmux[] = {
#include "ax650A_demo_pinmux.h"
};

unsigned int ax650A_evb_pinmux[] = {
#include "ax650A_evb_pinmux.h"
};

unsigned int ax650N_demo_pinmux[] = {
#include "ax650N_demo_pinmux.h"
};

unsigned int ax650N_evb_pinmux[] = {
#include "ax650N_evb_pinmux.h"
};

unsigned int ax650A_slt_pinmux[] = {
#include "ax650A_SLT_pinmux.h"
};

unsigned int ax650N_slt_pinmux[] = {
#include "ax650N_SLT_pinmux.h"
};

unsigned int ax650C_demo_pinmux[] = {
#include "ax650C_demo_pinmux.h"
};

struct pinmux {
	unsigned int *data;
	unsigned int size;
};

struct pinmux ax650_pinmux_tbl[] = {
	[AX650A_Demo] =
	    {ax650A_demo_pinmux,
	     sizeof(ax650A_demo_pinmux) / sizeof(unsigned int)},
	[AX650N_Demo] =
	    {ax650N_demo_pinmux,
	     sizeof(ax650N_demo_pinmux) / sizeof(unsigned int)},
	[AX650A_EVB] =
	    {ax650A_evb_pinmux,
	     sizeof(ax650A_evb_pinmux) / sizeof(unsigned int)},
	[AX650N_EVB] =
	    {ax650N_evb_pinmux,
	     sizeof(ax650N_evb_pinmux) / sizeof(unsigned int)},
	[AX650A_SLT] =
	    {ax650A_slt_pinmux,
	     sizeof(ax650A_slt_pinmux) / sizeof(unsigned int)},
	[AX650N_SLT] =
	    {ax650N_slt_pinmux,
	     sizeof(ax650N_slt_pinmux) / sizeof(unsigned int)},
};

static void i2c_select_pad(void)
{
	u32 i2c_pad_sel_reg;
	i2c_pad_sel_reg = readl(I2C_PAD_SELECT_G5_MISC);

	/* i2c0 set to i2c0_m pad */
	i2c_pad_sel_reg |= BIT(0);
	writel(i2c_pad_sel_reg, I2C_PAD_SELECT_G5_MISC);
}

static void spi_select_pad(void)
{
	u32 regval = 0;

#if defined(AX650_SPI_M1_M)
	/* spi1 set to spi1_m pad */
	regval |= BIT(12);
#endif
#if defined(AX650_SPI_M2_M)
	/* spi2 set to spi2_m pad */
	regval |= BIT(13);
#endif
	if (regval)
		writel(regval, PINMUX_G5_MISC_SET);
}

int pinmux_init(void)
{
	int index, i;
	int emmc_phy_flag = 0, sd_phy_flag = 0, sdio_phy_flag = 0, rtc_sys_flag = 0;
	index = get_board_id();
	if (index < 0 || index > 8)
		return 0;

	if (AX650C_CHIP == get_chip_type_id()) {
		ax650_pinmux_tbl[AX650N_Demo].data = ax650C_demo_pinmux;
		ax650_pinmux_tbl[AX650N_Demo].size = sizeof(ax650C_demo_pinmux) / sizeof(unsigned int);
	}

	/* enable rtc sys */
	writel(1 << FLASH_SYS_RTC_PINMUX_CFG_OFFSET, FLASH_SYS_RTC_PINMUX_CFG_SET);
	writel(RTC_PWD, RTC_SYS_PWD);
	for (i = 0; i < ax650_pinmux_tbl[index].size; i += 2) {
		writel(ax650_pinmux_tbl[index].data[i + 1],
		       (long)ax650_pinmux_tbl[index].data[i]);

		if((((long)ax650_pinmux_tbl[index].data[i] & PINMUX_ADDR_MASK) == EMMC_PINMUX_ADDR) &&
			(((ax650_pinmux_tbl[index].data[i + 1] >> 16) & 0x7) != 0)) {
				emmc_phy_flag = 1;
		}
		if((((long)ax650_pinmux_tbl[index].data[i] & PINMUX_ADDR_MASK) == SD_PINMUX_ADDR) &&
			(((ax650_pinmux_tbl[index].data[i + 1] >> 16) & 0x7) != 0)) {
				sd_phy_flag = 1;
		}
		if((((long)ax650_pinmux_tbl[index].data[i] & PINMUX_ADDR_MASK) == SDIO_PINMUX_ADDR) &&
			(((ax650_pinmux_tbl[index].data[i + 1] >> 16) & 0x7) != 0)) {
				sdio_phy_flag = 1;
		}

		if (((long)ax650_pinmux_tbl[index].data[i] & RTC_SYS_GLB_MASK) == RTC_SYS_GLB) {
			if (((long)ax650_pinmux_tbl[index].data[i] < (RTC_SYS_GLB + RTC_SYS_GLB_FUNC)) &&
				((long)ax650_pinmux_tbl[index].data[i + 1] != RTC_SYS_PROPERTY_DEF)) {
				rtc_sys_flag = 1;
			}
			if (((long)ax650_pinmux_tbl[index].data[i] >= (RTC_SYS_GLB + RTC_SYS_GLB_FUNC)) &&
				((long)ax650_pinmux_tbl[index].data[i + 1])) {
				rtc_sys_flag = 1;
			}
		}
	}
	if(emmc_phy_flag) {
		phy_func_en(EMMC_PINMUX_ADDR);
		#ifdef EMMC_PHY_SET_PAD_1V8
			phy_setting(DWC_MSHC_EMMC_BASE, 1); // default set pad work in 1.8V.
		#else
			phy_setting(DWC_MSHC_EMMC_BASE, 0); // set pad work in 3.3V.
		#endif
	}
	if(sd_phy_flag) {
		phy_func_en(SD_PINMUX_ADDR);
		#ifdef SD_PHY_SET_PAD_1V8
			phy_setting(DWC_MSHC_SD_BASE, 1); // default set pad work in 1.8V.
		#else
			phy_setting(DWC_MSHC_SD_BASE, 0); // set pad work in 3.3V.
		#endif
	}
	if(sdio_phy_flag) {
		phy_func_en(SDIO_PINMUX_ADDR);
		#ifdef SDIO_PHY_SET_PAD_1V8
			phy_setting(DWC_MSHC_SDIO_BASE, 1); // default set pad work in 1.8V.
		#else
			phy_setting(DWC_MSHC_SDIO_BASE, 0); // set pad work in 3.3V.
		#endif
	}
	if (rtc_sys_flag) {
		udelay(250000);
	}

	i2c_select_pad();
	spi_select_pad();
	return 0;
}

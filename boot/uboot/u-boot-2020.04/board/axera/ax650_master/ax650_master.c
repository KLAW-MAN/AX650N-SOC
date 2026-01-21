/*
 * (C) Copyright 2020 AXERA Co., Ltd
 *
 * SPDX-License-Identifier:     GPL-2.0+
 */
#include <common.h>
#include <dm.h>
#include <dm/pinctrl.h>
#include <dm/uclass-internal.h>
#include <power/regulator.h>
#include <asm/arch/ax650.h>
#include <dm/device-internal.h>
#include <nand.h>
#include <asm/io.h>
#include <asm-generic/gpio.h>
#include <pwm.h>

extern int chip_init(enum platform_type type);

DECLARE_GLOBAL_DATA_PTR;

U_BOOT_DEVICE(sysreset) = {
	.name = "axera_sysreset",
};

int adc_init(void)
{
	struct udevice *dev;
	int ret;

	ret = uclass_first_device_err(UCLASS_ADC, &dev);
	if (ret) {
		printf("No available ADC device\n");
		return CMD_RET_FAILURE;
	}

	return 0;
}

int board_init(void)
{
	adc_init();
	return 0;
}

void board_nand_init(void)
{
	struct udevice *dev;
	struct mtd_info *mtd;
	int ret;
	int busnum = 0;

	ret = uclass_get_device(UCLASS_MTD, busnum, &dev);
	if (ret) {
		printf("uclass_get_device: Invalid bus %d (err=%d)\n", busnum, ret);
		return;
	}
	ret = device_probe(dev);
	if (ret) {
		printf("device_probe error, (error=%d)\n", ret);
		return;
	}

	mtd = dev_get_uclass_priv(dev);
	ret = nand_register(0, mtd);
	if (ret) {
		printf("nand_register error, (error=%d)\n", ret);
		return;
	}
}

int dram_init(void)
{
	gd->ram_size = 0x80000000;
	return 0;
}

int dram_init_banksize(void)
{
	gd->bd->bi_dram[0].start = 0x280000000;
	gd->bd->bi_dram[0].size = 0x80000000;
	return 0;
}

int board_early_init_f(void)
{
	chip_init(AX650_EMMC);
	return 0;
}

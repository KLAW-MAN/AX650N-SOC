/*
 * (C) Copyright 2020 AXERA Co., Ltd
 *
 * SPDX-License-Identifier:     GPL-2.0+
 */

#ifndef __AX650_NAND_H
#define __AX650_NAND_H

#include <configs/ax650_common.h>

#define COUNTER_FREQUENCY    24000000
#define MEM_REGION_DDR_SIZE (0x200000000UL)
#define CPU_CLK_SET 1700 //1.7G
/*
 * SPL @ 32k for ~36k
 * ENV @ 96k
 * u-boot @ 128K
 */
/* #define CONFIG_ENV_OFFSET (96 * 1024) */

#define SDRAM_BANK_SIZE			(2UL << 30)

#define CONFIG_SYS_WHITE_ON_BLACK
#define FDL_BAD_BLKS_SCAN
//#define SPI_RX_SAMPLE_DLY_SCAN

#define EMMC_DEV_ID          0
#define SD_DEV_ID            0

#define EMMC_PHY_SET_PAD_1V8
#define SD_PHY_SET_PAD_1V8
#define SDIO_PHY_SET_PAD_1V8

#define CDPHY_PIN_FUNC_MULTIPLEX

//#define AX650_SPI_M1_M
//#define AX650_SPI_M2_M

#ifdef BOOTARGS_SPINAND
#undef BOOTARGS_SPINAND
#endif
#define BOOTARGS_SPINAND "mem=1024M console=ttyS0,115200n8 earlyprintk=dw_uart,board_id=0,boot_reason=0x0,noinitrd\
root=/dev/mtdblock7 rw rootfstype=ubifs ubi.mtd=7,2048 root=ubi0:rootfs init=/linuxrc \
mtdparts=spi5.0:4M(uboot),768K(env),1M(atf),1M(dtb),32M(kernel),512K(param),192M(rootfs)"

#ifdef MTDPARTS_DEFAULT
#undef MTDPARTS_DEFAULT
#endif
#define MTDPARTS_DEFAULT	"mtdparts=spi5.0:4M(uboot)," \
				"768K(env)," \
				"1M(atf)," \
				"1M(dtb)," \
				"32M(kernel)," \
				"512K(param)," \
				"192M(rootfs)"

#endif

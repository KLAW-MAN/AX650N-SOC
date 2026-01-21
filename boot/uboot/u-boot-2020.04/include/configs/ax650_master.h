/*
 * (C) Copyright 2020 AXERA Co., Ltd
 *
 * SPDX-License-Identifier:     GPL-2.0+
 */

#ifndef __AX650_EMMC_H
#define __AX650_EMMC_H

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

#define EMMC_DEV_ID          0
#define SD_DEV_ID            1

#define EMMC_PHY_SET_PAD_1V8
#define SD_PHY_SET_PAD_1V8
#define SDIO_PHY_SET_PAD_1V8

#define CDPHY_PIN_FUNC_MULTIPLEX

//#define AX650_SPI_M1_M
//#define AX650_SPI_M2_M

#endif

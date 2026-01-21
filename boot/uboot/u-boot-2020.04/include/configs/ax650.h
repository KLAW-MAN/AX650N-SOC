/*
 * (C) Copyright 2020 AXERA Co., Ltd
 *
 * SPDX-License-Identifier:     GPL-2.0+
 */

#ifndef __AX650_H
#define __AX650_H

#include <configs/ax650_common.h>

#define COUNTER_FREQUENCY    24000000

/*
 * SPL @ 32k for ~36k
 * ENV @ 96k
 * u-boot @ 128K
 */
/* #define CONFIG_ENV_OFFSET (96 * 1024) */

#define SDRAM_BANK_SIZE			(2UL << 30)

#define CONFIG_SYS_WHITE_ON_BLACK

#define EMMC_DEV_ID          0

#define CONFIG_CMDLINE_TAG
#define CONFIG_SETUP_MEMORY_TAGS
#define CONFIG_INITRD_TAG

#endif

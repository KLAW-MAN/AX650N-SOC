/*
 * Copyright (c) 2017, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef PMU_H
#define PMU_H

#include <stdint.h>

typedef enum MODULES_ENUM {
	MODULE_GROUP1_START = 0,
	MODULE_VENC0 = MODULE_GROUP1_START,
	MODULE_VENC1,
	MODULE_VDEC,
	MODULE_VDSP,
	MODULE_CPU0,
	MODULE_CPU1,
	MODULE_CPU2,
	MODULE_CPU3,
	MODULE_CPU4,
	MODULE_CPU5,
	MODULE_CPU6,
	MODULE_CPU7,

	MODULE_GROUP2_START = MODULE_CPU7 + 1,
	MODULE_DDR0 = MODULE_GROUP2_START,
	MODULE_DDR1,
	MODULE_FLASH,
	MODULE_PIPE,
	MODULE_PERI,
	MODULE_MM,
	MODULE_N0,
	MODULE_N1,
	MODULE_N2,
	MODULE_N3,
	MODULE_N4,
	MODULE_N5,
	MODULE_N6,
	MODULE_N7,
	MODULE_N8,
	MODULE_NPU,
	MODULE_CPU,
	MODULE_ISP,
	MODULE_SEN,
	MODULES_MAX,
} MODULES_ENUM;

typedef enum PWR_STATE_ENUM {
	PWR_STATE_ON = 0,
	PWR_STATE_OFF = 5,
} PWR_STATE_ENUM;

typedef enum SLP_EN_ENUM {
	SLP_EN_SET = 1,
	SLP_EN_CLR = 2,
} SLP_EN_ENUM;

int pmu_module_sleep_en(MODULES_ENUM module, SLP_EN_ENUM en_set_clr);
int pmu_get_module_state(MODULES_ENUM module, uint32_t *state);
int pmu_module_wakeup(MODULES_ENUM module);
void pmu_init(void);

#endif /* PMU_H */

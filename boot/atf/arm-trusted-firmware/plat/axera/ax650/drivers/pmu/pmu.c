/*
 * Copyright (c) 2017, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#if 0
#include <assert.h>
#include <errno.h>
#include <platform_def.h>
#include <arch_helpers.h>
#include <bl31/bl31.h>
#include <common/debug.h>
#include <drivers/console.h>
#include <drivers/delay_timer.h>
#include <plat/common/platform.h>
#include <pmu.h>
#include <ax650_def.h>

#if 0
static int cpus_power_domain_on(uint32_t cpu_id)
{
	//todo

	return 0;
}

static int cpus_power_domain_off(uint32_t cpu_id, uint32_t pd_cfg)
{
	//todo

	return 0;
}
#endif
int soc_cores_pwr_dm_on_finish(void)
{
	//todo

	return 0;
}

int soc_cores_pwr_dm_resume(void)
{
	//todo

	return 0;
}

void __dead2 soc_system_off(void)
{
	//todo

	dsb();
	while (1);
}

#if 0
#define ARCH_TIMER_TICKS_PER_US (SYS_COUNTER_FREQ_IN_TICKS / 1000000)

static void udelay(uint32_t us)
{
	uint64_t pct_orig, pct_now;
	uint64_t to_wait = ARCH_TIMER_TICKS_PER_US * us;

	isb();
	pct_orig = read_cntpct_el0();

	do {
		isb();
		pct_now = read_cntpct_el0();
	} while ((pct_now - pct_orig) <= to_wait);
}
#endif

static void ddr_suspend(void)
{
	//todo
}

static void sram_soc_enter_lp(void)
{
	//todo

	dsb();
	isb();
err_loop:
	wfi();
	/*
	 *Soc will enter low power mode and
	 *do not return to here.
	 */
	goto err_loop;
}

void sram_suspend(void)
{
	//todo
	/* ddr self-refresh and gating phy */
	ddr_suspend();

	sram_soc_enter_lp();
}

void plat_pmu_init(void)
{
	//todo
}
#endif

#include <lib/mmio.h>
#include <pmu.h>
#include <ax650_def.h>
#include <ax650_pmu_glb.h>

#define PMU_STATE_GRP_NUM           4
#define SYS_PWR_STATE_PER_GRP_MAX   8
#define SYS_PWR_STATE_LEN           4
#define SYS_PWR_STATE_MASK          0xF

#define FALSE	0
#define TRUE	1

struct pmu_module_state_order {
	uintptr_t pwr_state_addr;
	const MODULES_ENUM *modules;
};

int pmu_module_sleep_en(MODULES_ENUM module, SLP_EN_ENUM en_set_clr)
{
	int ret = 0;

	switch (module) {
	case MODULE_VENC0:
	case MODULE_VENC1:
	case MODULE_VDEC:
	case MODULE_VDSP:
	case MODULE_CPU0:
	case MODULE_CPU1:
	case MODULE_CPU2:
	case MODULE_CPU3:
	case MODULE_CPU4:
	case MODULE_CPU5:
	case MODULE_CPU6:
	case MODULE_CPU7:
		if (SLP_EN_SET == en_set_clr)
			mmio_write_32(PMU_GLB_SLP_EN_SET_ADDR,
							(1 << (module - MODULE_GROUP1_START)));
		else if (SLP_EN_CLR == en_set_clr)
			mmio_write_32(PMU_GLB_SLP_EN_CLR_ADDR,
							(1 << (module - MODULE_GROUP1_START)));
		break;

	case MODULE_DDR0:
	case MODULE_DDR1:
	case MODULE_FLASH:
	case MODULE_PIPE:
	case MODULE_PERI:
	case MODULE_MM:
	case MODULE_N0:
	case MODULE_N1:
	case MODULE_N2:
	case MODULE_N3:
	case MODULE_N4:
	case MODULE_N5:
	case MODULE_N6:
	case MODULE_N7:
	case MODULE_N8:
	case MODULE_NPU:
	case MODULE_CPU:
	case MODULE_ISP:
	case MODULE_SEN:
		if (SLP_EN_SET == en_set_clr)
			mmio_write_32(PMU_GLB_SLP_EN1_SET_ADDR,
							(1 << (module - MODULE_GROUP2_START)));
		else if (SLP_EN_CLR == en_set_clr)
			mmio_write_32(PMU_GLB_SLP_EN1_CLR_ADDR,
							(1 << (module - MODULE_GROUP2_START)));
		break;

	default:
		ret = -1;
	}

	return ret;
}

static const MODULES_ENUM grp1_modules[SYS_PWR_STATE_PER_GRP_MAX] =
				{MODULE_VENC0, MODULE_VENC1, MODULE_VDEC, MODULE_VDSP,
				 MODULE_NPU, MODULE_CPU, MODULE_ISP, -1};
static const MODULES_ENUM grp2_modules[SYS_PWR_STATE_PER_GRP_MAX] =
				{MODULE_CPU0, MODULE_CPU1 ,MODULE_CPU2, MODULE_CPU3,
				 MODULE_CPU4 ,MODULE_CPU5, MODULE_CPU6, MODULE_CPU7};
static const MODULES_ENUM grp3_modules[SYS_PWR_STATE_PER_GRP_MAX] =
				{MODULE_PIPE, MODULE_MM ,MODULE_N8, MODULE_SEN,
				 MODULE_PERI,MODULE_DDR1, MODULE_DDR0, MODULE_FLASH};
static const MODULES_ENUM grp4_modules[SYS_PWR_STATE_PER_GRP_MAX] =
				{MODULE_N0, MODULE_N1 ,MODULE_N2, MODULE_N3,
				 MODULE_N4 ,MODULE_N5, MODULE_N6, MODULE_N7};

int pmu_get_module_state(MODULES_ENUM module, uint32_t *state)
{
	int i, j;
	uint32_t value;
	int ret = -1;
	struct pmu_module_state_order pwr_state_array[PMU_STATE_GRP_NUM] = {
		{
			.pwr_state_addr = PMU_GLB_PWR_STATE0_ADDR,
			.modules = grp1_modules,
		},
		{
			.pwr_state_addr = PMU_GLB_PWR_STATE1_ADDR,
			.modules = grp2_modules,
		},
		{
			.pwr_state_addr = PMU_GLB_PWR_STATE2_ADDR,
			.modules = grp3_modules,
		},
		{
			.pwr_state_addr = PMU_GLB_PWR_STATE3_ADDR,
			.modules = grp4_modules,
		},
	};

	for (i = 0; i < PMU_STATE_GRP_NUM; i++) {
		for (j = 0; j < SYS_PWR_STATE_PER_GRP_MAX; j++) {
			if (pwr_state_array[i].modules[j] == module) {
				ret = 0;
				break;
			}
		}
		if (!ret)
			break;
	}

	if (!ret && (i < PMU_STATE_GRP_NUM)) {
		value = mmio_read_32(pwr_state_array[i].pwr_state_addr);
		*state = (value >> (j * SYS_PWR_STATE_LEN)) & SYS_PWR_STATE_MASK;
	}

	return ret;
}

static int pmu_module_wakeup_set_clr(MODULES_ENUM module, int set)
{
	int ret = 0;

	switch (module) {
	case MODULE_VENC0:
	case MODULE_VENC1:
	case MODULE_VDEC:
	case MODULE_VDSP:
	case MODULE_CPU0:
	case MODULE_CPU1:
	case MODULE_CPU2:
	case MODULE_CPU3:
	case MODULE_CPU4:
	case MODULE_CPU5:
	case MODULE_CPU6:
	case MODULE_CPU7:
		if (set)
			mmio_write_32(PMU_GLB_WAKUP_SET_ADDR,
							(1 << (module - MODULE_GROUP1_START)));
		else
			mmio_write_32(PMU_GLB_WAKUP_CLR_ADDR,
							(1 << (module - MODULE_GROUP1_START)));
		break;

	case MODULE_DDR0:
	case MODULE_DDR1:
	case MODULE_FLASH:
	case MODULE_PIPE:
	case MODULE_PERI:
	case MODULE_MM:
	case MODULE_N0:
	case MODULE_N1:
	case MODULE_N2:
	case MODULE_N3:
	case MODULE_N4:
	case MODULE_N5:
	case MODULE_N6:
	case MODULE_N7:
	case MODULE_N8:
	case MODULE_NPU:
	case MODULE_CPU:
	case MODULE_ISP:
	case MODULE_SEN:
		if (set)
			mmio_write_32(PMU_GLB_WAKUP1_SET_ADDR,
							(1 << (module - MODULE_GROUP2_START)));
		else
			mmio_write_32(PMU_GLB_WAKUP1_CLR_ADDR,
							(1 << (module - MODULE_GROUP2_START)));
		break;

	default:
		ret = -1;
	}

	return ret;

}

int pmu_module_wakeup(MODULES_ENUM module)
{
	uint32_t state;
	int ret;

	ret = pmu_module_wakeup_set_clr(module, TRUE);
	if (ret)
		goto RET;

	/* cpu core not need loop */
	if (module < MODULE_CPU0 || module > MODULE_CPU7) {
		while (!(ret = pmu_get_module_state(module, &state)) &&
				(state != PWR_STATE_ON));
		if (ret)
			goto RET;
	}

	/* cpu core0~core7 clear must not to do */
	if (module < MODULE_CPU0 || module > MODULE_CPU7)
		pmu_module_wakeup_set_clr(module, FALSE);

RET:
	return ret;
}

void pmu_init(void)
{
#if 0
	writel(0, PMU_GLB_PWR_WAIT0_ADDR);
	writel(0, PMU_GLB_PWR_WAIT1_ADDR);
	writel(0, PMU_GLB_PWR_WAIT2_ADDR);
	writel(0, PMU_GLB_PWR_WAIT3_ADDR);
	writel(0, PMU_GLB_PWR_WAIT4_ADDR);
	writel(0, PMU_GLB_PWR_WAITON0_ADDR);
	writel(0, PMU_GLB_PWR_WAITON1_ADDR);
	writel(0, PMU_GLB_PWR_WAITON2_ADDR);
	writel(0, PMU_GLB_PWR_WAITON3_ADDR);
	writel(0, PMU_GLB_PWR_WAITON4_ADDR);
#endif
}

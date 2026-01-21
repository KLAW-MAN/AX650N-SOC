/*
 * Copyright (c) 2013-2018, AX650 Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <ax650_pwrc.h>
#include <lib/bakery_lock.h>
#include <lib/mmio.h>
#include <platform_def.h>
#include <ax650_def.h>
#include <ax650_cpu_sys_comm_glb.h>
#include <ax650_cpu_sys_glb.h>
#include <plat/common/platform.h>
#include <bl31/bl31.h>

/* AX650 */
#define P_CHAN_ACTIVE_MASK_OFFSET 16
#define P_CHAN_TIMEOUT_VALUE      0x1F00  //asic suggest

extern uintptr_t ax650_sec_entrypoint;

/*
 * TODO: Someday there will be a generic power controller api. At the moment
 * each platform has its own pwrc so just exporting functions is fine.
 */
AX650_INSTANTIATE_LOCK;

static void ax650_p_channel_config()
{
	uint32_t value;

	/* dsu p-channel configuration */
	value = mmio_read_32(CPU_SYS_COMM_GLB_DSU_P_CHN_CFG_ADDR) &
						(~CPU_SYS_COMM_GLB_DSU_P_CHN_CFG_MASK_MASK);
	value |= (0x3DFE << CPU_SYS_COMM_GLB_DSU_P_CHN_CFG_MASK_POS);
	mmio_write_32(CPU_SYS_COMM_GLB_DSU_P_CHN_CFG_ADDR, value);

	value = BIT_DSU_P_CHN_ACTIVE_TIMEOUT_EN | P_CHAN_TIMEOUT_VALUE;
	mmio_write_32(CPU_SYS_COMM_GLB_DSU_P_CHN_ACTIVE_TIMEOUT_ADDR, value);
	value = BIT_DSU_P_CHN_REQ_TIMEOUT_EN | P_CHAN_TIMEOUT_VALUE;
	mmio_write_32(CPU_SYS_COMM_GLB_DSU_P_CHN_REQ_TIMEOUT_ADDR, value);
	value = BIT_DSU_P_CHN_DENY_TIMEOUT_EN | P_CHAN_TIMEOUT_VALUE;
	mmio_write_32(CPU_SYS_COMM_GLB_DSU_P_CHN_DENY_TIMEOUT_ADDR, value);
	//the value need to set update bit to 1 then clear for real updating data
	mmio_write_32(CPU_SYS_COMM_GLB_DSU_P_CHN_ACTIVE_TIMEOUT_SET_ADDR,
					BIT_DSU_P_CHN_ACTIVE_TIMEOUT_SET_UPDATE_SET);
	mmio_write_32(CPU_SYS_COMM_GLB_DSU_P_CHN_REQ_TIMEOUT_SET_ADDR,
					BIT_DSU_P_CHN_REQ_TIMEOUT_SET_UPDATE_SET);
	mmio_write_32(CPU_SYS_COMM_GLB_DSU_P_CHN_DENY_TIMEOUT_SET_ADDR,
					BIT_DSU_P_CHN_DENY_TIMEOUT_SET_UPDATE_SET);
	//clear
	mmio_write_32(CPU_SYS_COMM_GLB_DSU_P_CHN_ACTIVE_TIMEOUT_CLR_ADDR,
					BIT_DSU_P_CHN_ACTIVE_TIMEOUT_CLR_UPDATE_CLR);
	mmio_write_32(CPU_SYS_COMM_GLB_DSU_P_CHN_REQ_TIMEOUT_CLR_ADDR,
					BIT_DSU_P_CHN_REQ_TIMEOUT_CLR_UPDATE_CLR);
	mmio_write_32(CPU_SYS_COMM_GLB_DSU_P_CHN_DENY_TIMEOUT_CLR_ADDR,
					BIT_DSU_P_CHN_DENY_TIMEOUT_CLR_UPDATE_CLR);

	/* cpu core p-channel configuration*/
	// set p-channel active timetout/req timeout/deny timeout val
	value = BIT_CPU_SYS_CORES_PCHN_ACTIVE_TIMEOUT_EN | P_CHAN_TIMEOUT_VALUE;
	mmio_write_32(CPU_SYS_COMM_GLB_CA55_CORES_PCHN_ACTIVE_TIMEOUT_ADDR, value);

	value = BIT_CPU_SYS_CORES_PCHN_REQ_TIMEOUT_EN | P_CHAN_TIMEOUT_VALUE;
	mmio_write_32(CPU_SYS_COMM_GLB_CA55_CORES_PCHN_REQ_TIMEOUT_ADDR, value);

	value = BIT_CPU_SYS_CORES_PCHN_DENY_TIMEOUT_EN | P_CHAN_TIMEOUT_VALUE;
	mmio_write_32(CPU_SYS_COMM_GLB_CA55_CORES_PCHN_DENY_TIMEOUT_ADDR, value);

	//the value need to set update bit to 1 then clear for real updating data
	mmio_write_32(CPU_SYS_COMM_GLB_CA55_CORES_PCHN_ACTIVE_TIMEOUT_SET_ADDR,
					BIT_CA55_CORES_PCHN_ACTIVE_TIMEOUT_UPDATE_SET);
	mmio_write_32(CPU_SYS_COMM_GLB_CA55_CORES_PCHN_REQ_TIMEOUT_SET_ADDR,
					BIT_CA55_CORES_PCHN_REQ_TIMEOUT_UPDATE_SET);
	mmio_write_32(CPU_SYS_COMM_GLB_CA55_CORES_PCHN_DENY_TIMEOUT_SET_ADDR,
					BIT_CA55_CORES_PCHN_DENY_TIMEOUT_SET_UPDATE_SET);
	//clear
	mmio_write_32(CPU_SYS_COMM_GLB_CA55_CORES_PCHN_ACTIVE_TIMEOUT_CLR_ADDR,
					BIT_CA55_CORES_PCHN_ACTIVE_TIMEOUT_UPDATE_CLR);
	mmio_write_32(CPU_SYS_COMM_GLB_CA55_CORES_PCHN_REQ_TIMEOUT_CLR_ADDR,
					BIT_CA55_CORES_PCHN_REQ_TIMEOUT_UPDATE_CLR);
	mmio_write_32(CPU_SYS_COMM_GLB_CA55_CORES_PCHN_DENY_TIMEOUT_CLR_ADDR,
					BIT_CA55_CORES_PCHN_DENY_TIMEOUT_UPDATE_CLR);
	//ax_printf("cpu cores p_channel_timeout_config\n\r");
}

unsigned int ax650_pwrc_read_psysr(u_register_t mpidr)
{
	unsigned int core_id, state;

	//ax650_lock_get();
	core_id = plat_core_pos_by_mpidr(mpidr);
	pmu_get_module_state((MODULE_CPU0 + core_id - 0), &state);
	//ax650_lock_release();

	return state;
}

void ax650_pwrc_write_pponr(u_register_t mpidr)
{
	unsigned int core_id = plat_core_pos_by_mpidr(mpidr);
	long addr_set = CPU_SYS_COMM_GLB_CA55_CORE0_P_CHN_CFG_SET_ADDR + 4 * core_id;
	long addr_clr = CPU_SYS_COMM_GLB_CA55_CORE0_P_CHN_CFG_CLR_ADDR + 4 * core_id;

	/* clear cpu core sleep en */
	pmu_module_sleep_en(MODULE_CPU0 + core_id, SLP_EN_CLR);
	mmio_write_32(CPU_SYS_COMM_GLB_CPU_SYS_SLEEP_EN_CLR_ADDR, (1 << core_id));

	//ax650_lock_get();
	pmu_module_wakeup(MODULE_CPU0 + core_id - 0);
	//ax650_lock_release();

	mmio_write_32(addr_set, BIT_CPU_SYS_A55_CORE_P_CHN_WAKEUP_SET);  /* set */
	mmio_write_32(addr_clr, BIT_CPU_SYS_A55_CORE_P_CHN_WAKEUP_CLR); /* clear */
}

void ax650_pwrc_write_ppoffr(u_register_t mpidr)
{
	unsigned int core_id = plat_core_pos_by_mpidr(mpidr);
	uintptr_t addr;

	//ax650_lock_get();

	/* enable pmu core slp en */
	pmu_module_sleep_en(MODULE_CPU0 + core_id - 0, SLP_EN_SET);

	/* enable cpu core sleep en */
	mmio_write_32(CPU_SYS_COMM_GLB_CPU_SYS_SLEEP_EN_SET_ADDR, (1 << core_id));

	/* disable cpu core interrupt */
	mmio_write_32(CPU_SYS_COMM_GLB_CPU_INT_DISABLE_SET_ADDR, (1 << core_id));

	/* p-channel mask set 0xA */
	addr = CPU_SYS_COMM_GLB_CA55_CORE0_P_CHN_CFG_ADDR + 4 * core_id;
	mmio_write_32(addr, (0xA << P_CHAN_ACTIVE_MASK_OFFSET));

	//ax650_lock_release();
}

void ax650_sys_pwrdwn()
{
	//ax650_lock_get();

	mmio_write_32(CPU_SYS_COMM_GLB_CPU_SYS_SLEEP_EN_SET_ADDR,
					BIT_CPU_SYS_CPU_SYS_SLEEP_EN_SET);
	mmio_write_32(CPU_SYS_COMM_GLB_CPU_SYS_LIGHT_SLP_CFG_SET_ADDR,
					BIT_CPU_SYS_COMM_GLB_CPU_SYS_LIGHT_SLP_CFG_DATA_IDLE_EN_SET);
	pmu_module_sleep_en(MODULE_CPU, SLP_EN_SET);

	//ax650_lock_release();
}

#if 0
unsigned int ax650_pwrc_get_cpu_wkr(u_register_t mpidr)
{
	return PSYSR_WK(ax650_pwrc_read_psysr(mpidr));
}

void ax650_pwrc_set_wen(u_register_t mpidr)
{
	ax650_lock_get();
	mmio_write_32(PWRC_BASE + PWKUPR_OFF,
		      (unsigned int) (PWKUPR_WEN | mpidr));
	ax650_lock_release();
}

void ax650_pwrc_clr_wen(u_register_t mpidr)
{
	ax650_lock_get();
	mmio_write_32(PWRC_BASE + PWKUPR_OFF,
		      (unsigned int) mpidr);
	ax650_lock_release();
}

void ax650_pwrc_write_pcoffr(u_register_t mpidr)
{
	ax650_lock_get();
	mmio_write_32(PWRC_BASE + PCOFFR_OFF, (unsigned int) mpidr);
	ax650_lock_release();
}
#endif

/* Nothing else to do here apart from initializing the lock */
void plat_ax650_pwrc_setup(void)
{
	//uint32_t addr_lo = ax650_sec_entrypoint & 0xFFFFFFFF;
	//uint32_t addr_high = ax650_sec_entrypoint >> 32;
	uint32_t addr_lo = ((uintptr_t)bl31_warm_entrypoint) & 0xFFFFFFFF;
	uint32_t addr_high = ((uintptr_t)bl31_warm_entrypoint) >> 32;
	int i;

	ax650_lock_init();

	/* pmu config */
	pmu_init();

	/* config cpu sys p-chanel params */
	ax650_p_channel_config();

	/* set core start addr core0 later todo */
	for (i = 1; i < PLATFORM_CORE_COUNT; i++) {
		uint32_t offset = (i - 1) * 8;

		mmio_write_32((CPU_SYS_GLB_RVBARADDR_LO_CORE1_ADDR + offset), addr_lo);
		mmio_write_32((CPU_SYS_GLB_RVBARADDR_LO_CORE1_ADDR + offset + 4), addr_high);
	}

	/* set wakeup enable */
	mmio_write_32(CPU_SYS_COMM_GLB_EIC_EN_SET_ADDR, BITS_EIC_EN_ALL);
}

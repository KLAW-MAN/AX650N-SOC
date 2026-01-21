/*
 * Copyright (c) 2013-2021, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <assert.h>

#include <arch_helpers.h>
#include <common/debug.h>
#include <lib/mmio.h>
#include <lib/psci/psci.h>
#include <plat_ax650.h>
#include <ax650_pwrc.h>
#include <platform_def.h>

uintptr_t ax650_sec_entrypoint;

//temp for compile, later will adpat for ax650 todo
#define PSYSR_AFF_L2		BIT_32(31)
#define PSYSR_AFF_L1		BIT_32(30)
#define PSYSR_AFF_L0		BIT_32(29)
#define PSYSR_INVALID		U(0xffffffff)

/*******************************************************************************
 * Function which implements the common ax650 specific operations to power down a
 * cluster in response to a CPU_OFF or CPU_SUSPEND request.
 ******************************************************************************/
static void ax650_cluster_pwrdwn_common(void)
{
	//uint64_t mpidr = read_mpidr_el1();

	/* Disable coherency if this cluster is to be turned off */
	//ax650_interconnect_disable();

#if HW_ASSISTED_COHERENCY
	uint32_t reg;

	/*
	 * If we have determined this core to be the last man standing and we
	 * intend to power down the cluster proactively, we provide a hint to
	 * the power controller that cluster power is not required when all
	 * cores are powered down.
	 * Note that this is only an advisory to power controller and is supported
	 * by SoCs with DynamIQ Shared Units only.
	 */
	reg = read_clusterpwrdn();

	/* Clear and set bit 0 : Cluster power not required */
	reg &= ~DSU_CLUSTER_PWR_MASK;
	reg |= DSU_CLUSTER_PWR_OFF;
	write_clusterpwrdn(reg);
#endif

	/* Program the power controller to turn the cluster off */
	//ax650_pwrc_write_pcoffr(mpidr);
}

/******************************************************************************
 * Helper function to resume the platform from system suspend. Reinitialize
 * the system components which are not in the Always ON power domain.
 * TODO: Unify the platform setup when waking up from cold boot and system
 * resume in arm_bl31_platform_setup().
 *****************************************************************************/
void ax650_system_pwr_domain_resume(void)
{
	/* Initialize the console */
	//arm_console_runtime_init();

	/* Assert system power domain is available on the platform */
	assert(PLAT_MAX_PWR_LVL >= ARM_PWR_LVL2);

	plat_ax650_gic_resume();

	//arm_configure_sys_timer();
	//todo may not need, the gtmr is in aon
}

static void ax650_power_domain_on_finish_common(const psci_power_state_t *target_state)
{
	unsigned long mpidr;

	assert(target_state->pwr_domain_state[ARM_PWR_LVL0] ==
					ARM_LOCAL_STATE_OFF);

	/* Get the mpidr for this cpu */
	mpidr = read_mpidr_el1();

	/* Perform the common cluster specific operations */
	if (target_state->pwr_domain_state[ARM_PWR_LVL1] ==
					ARM_LOCAL_STATE_OFF) {
		/*
		 * This CPU might have woken up whilst the cluster was
		 * attempting to power down. In this case the ax650 power
		 * controller will have a pending cluster power off request
		 * which needs to be cleared by writing to the PPONR register.
		 * This prevents the power controller from interpreting a
		 * subsequent entry of this cpu into a simple wfi as a power
		 * down request.
		 */
		ax650_pwrc_write_pponr(mpidr);

		/* Enable coherency if this cluster was off */
		//ax650_interconnect_enable();
	}
	/* Perform the common system specific operations */
	if (target_state->pwr_domain_state[ARM_PWR_LVL2] ==
						ARM_LOCAL_STATE_OFF)
		ax650_system_pwr_domain_resume();

	/*
	 * Clear PWKUPR.WEN bit to ensure interrupts do not interfere
	 * with a cpu power down unless the bit is set again
	 */
	//ax650_pwrc_clr_wen(mpidr);
}

/*******************************************************************************
 * ax650 handler called when a CPU is about to enter standby.
 ******************************************************************************/
static void ax650_cpu_standby(plat_local_state_t cpu_state)
{
	u_register_t scr = read_scr_el3();

	assert(cpu_state == ARM_LOCAL_STATE_RET);

	/*
	 * Enable the Non-secure interrupt to wake the CPU.
	 * In GICv3 affinity routing mode, the Non-secure Group 1 interrupts
	 * use Physical FIQ at EL3 whereas in GICv2, Physical IRQ is used.
	 * Enabling both the bits works for both GICv2 mode and GICv3 affinity
	 * routing mode.
	 */
	write_scr_el3(scr | SCR_IRQ_BIT | SCR_FIQ_BIT);
	isb();

	/*
	 * Enter standby state.
	 * dsb is good practice before using wfi to enter low power states.
	 */
	dsb();
	wfi();

	/*
	 * Restore SCR_EL3 to the original value, synchronisation of SCR_EL3
	 * is done by eret in el3_exit() to save some execution cycles.
	 */
	write_scr_el3(scr);
}

/*******************************************************************************
 * ax650 handler called when a power domain is about to be turned on. The
 * mpidr determines the CPU to be turned on.
 ******************************************************************************/
static int ax650_pwr_domain_on(u_register_t mpidr)
{
	int rc = PSCI_E_SUCCESS;
	unsigned int state;

	/*
	 * Ensure that we do not cancel an inflight power off request for the
	 * target cpu. That would leave it in a zombie wfi. Wait for it to power
	 * off and then program the power controller to turn that CPU on.
	 */
	do {
		state = ax650_pwrc_read_psysr(mpidr);
	} while ((state != PWR_STATE_ON && state != PWR_STATE_OFF) != 0U);

	ax650_pwrc_write_pponr(mpidr);
	return rc;
}

/*******************************************************************************
 * ax650 handler called when a power domain is about to be turned off. The
 * target_state encodes the power state that each level should transition to.
 ******************************************************************************/
static void ax650_pwr_domain_off(const psci_power_state_t *target_state)
{
	assert(target_state->pwr_domain_state[ARM_PWR_LVL0] ==
					ARM_LOCAL_STATE_OFF);

	/*
	 * If execution reaches this stage then this power domain will be
	 * suspended. Perform at least the cpu specific actions followed
	 * by the cluster specific operations if applicable.
	 */

	/* Prevent interrupts from spuriously waking up this cpu */
	plat_ax650_gic_cpuif_disable();

	/* Turn redistributor off */
	plat_ax650_gic_redistif_off();

	/* Program the power controller to power off this cpu. */
	ax650_pwrc_write_ppoffr(read_mpidr_el1());

	if (target_state->pwr_domain_state[ARM_PWR_LVL1] == ARM_LOCAL_STATE_OFF)
		ax650_cluster_pwrdwn_common();

	if (target_state->pwr_domain_state[ARM_PWR_LVL2] == ARM_LOCAL_STATE_OFF)
		ax650_sys_pwrdwn();
}

/******************************************************************************
 * Helper function to save the platform state before a system suspend. Save the
 * state of the system components which are not in the Always ON power domain.
 *****************************************************************************/
void ax650_system_pwr_domain_save(void)
{
	/* Assert system power domain is available on the platform */
	assert(PLAT_MAX_PWR_LVL >= ARM_PWR_LVL2);

	plat_ax650_gic_save();

	/*
	 * Unregister console now so that it is not registered for a second
	 * time during resume.
	 */
	//arm_console_runtime_end();

	/*
	 * All the other peripheral which are configured by ARM TF are
	 * re-initialized on resume from system suspend. Hence we
	 * don't save their state here.
	 */
}


/*******************************************************************************
 * ax650 handler called when a power domain is about to be suspended. The
 * target_state encodes the power state that each level should transition to.
 ******************************************************************************/
static void ax650_pwr_domain_suspend(const psci_power_state_t *target_state)
{
	//unsigned long mpidr;

	/*
	 * ax650 has retention only at cpu level. Just return
	 * as nothing is to be done for retention.
	 */
	if (target_state->pwr_domain_state[ARM_PWR_LVL0] ==
					ARM_LOCAL_STATE_RET)
		return;

	assert(target_state->pwr_domain_state[ARM_PWR_LVL0] ==
					ARM_LOCAL_STATE_OFF);

#if 0
	/* Get the mpidr for this cpu */
	mpidr = read_mpidr_el1();

	/* Program the power controller to enable wakeup interrupts. */
	ax650_pwrc_set_wen(mpidr);
#endif

	/* Prevent interrupts from spuriously waking up this cpu */
	plat_ax650_gic_cpuif_disable();

	/*
	 * The Redistributor is not powered off as it can potentially prevent
	 * wake up events reaching the CPUIF and/or might lead to losing
	 * register context.
	 */

	/* Perform the common cluster specific operations */
	if (target_state->pwr_domain_state[ARM_PWR_LVL1] ==
					ARM_LOCAL_STATE_OFF)
		ax650_cluster_pwrdwn_common();

	/* Perform the common system specific operations */
	if (target_state->pwr_domain_state[ARM_PWR_LVL2] ==
						ARM_LOCAL_STATE_OFF)
		ax650_system_pwr_domain_save();

	/* Program the power controller to power off this cpu. */
	ax650_pwrc_write_ppoffr(read_mpidr_el1());
}

/*******************************************************************************
 * ax650 handler called when a power domain has just been powered on after
 * being turned off earlier. The target_state encodes the low power state that
 * each level has woken up from.
 ******************************************************************************/
static void ax650_pwr_domain_on_finish(const psci_power_state_t *target_state)
{
	ax650_power_domain_on_finish_common(target_state);

}

/*******************************************************************************
 * ax650 handler called when a power domain has just been powered on and the cpu
 * and its cluster are fully participating in coherent transaction on the
 * interconnect. Data cache must be enabled for CPU at this point.
 ******************************************************************************/
static void ax650_pwr_domain_on_finish_late(const psci_power_state_t *target_state)
{
	/* Program GIC per-cpu distributor or re-distributor interface */
	plat_ax650_gic_pcpu_init();

	/* Enable GIC CPU interface */
	plat_ax650_gic_cpuif_enable();
}

/*******************************************************************************
 * ax650 handler called when a power domain has just been powered on after
 * having been suspended earlier. The target_state encodes the low power state
 * that each level has woken up from.
 * TODO: At the moment we reuse the on finisher and reinitialize the secure
 * context. Need to implement a separate suspend finisher.
 ******************************************************************************/
static void ax650_pwr_domain_suspend_finish(const psci_power_state_t *target_state)
{
	/*
	 * Nothing to be done on waking up from retention from CPU level.
	 */
	if (target_state->pwr_domain_state[ARM_PWR_LVL0] ==
					ARM_LOCAL_STATE_RET)
		return;

	ax650_power_domain_on_finish_common(target_state);

	/* Enable GIC CPU interface */
	plat_ax650_gic_cpuif_enable();
}

#if 0
/*******************************************************************************
 * ax650 handlers to shutdown/reboot the system
 ******************************************************************************/
static void __dead2 ax650_system_off(void)
{
	/* Write the System Configuration Control Register */
	mmio_write_32(V2M_SYSREGS_BASE + V2M_SYS_CFGCTRL,
		V2M_CFGCTRL_START |
		V2M_CFGCTRL_RW |
		V2M_CFGCTRL_FUNC(V2M_FUNC_SHUTDOWN));
	wfi();
	ERROR("ax650 System Off: operation not handled.\n");
	panic();
}

static void __dead2 ax650_system_reset(void)
{
	/* Write the System Configuration Control Register */
	mmio_write_32(V2M_SYSREGS_BASE + V2M_SYS_CFGCTRL,
		V2M_CFGCTRL_START |
		V2M_CFGCTRL_RW |
		V2M_CFGCTRL_FUNC(V2M_FUNC_REBOOT));
	wfi();
	ERROR("ax650 System Reset: operation not handled.\n");
	panic();
}
#endif

static int ax650_node_hw_state(u_register_t target_cpu,
			     unsigned int power_level)
{
	unsigned int psysr;
	int ret;

	/*
	 * The format of 'power_level' is implementation-defined, but 0 must
	 * mean a CPU. We also allow 1 to denote the cluster
	 */
	if ((power_level != ARM_PWR_LVL0) && (power_level != ARM_PWR_LVL1))
		return PSCI_E_INVALID_PARAMS;

	/*
	 * Read the status of the given MPDIR from ax650 power controller. The
	 * power controller only gives us on/off status, so map that to expected
	 * return values of the PSCI call
	 */
	psysr = ax650_pwrc_read_psysr(target_cpu);
	if (psysr == PSYSR_INVALID)
		return PSCI_E_INVALID_PARAMS;

	if (power_level == ARM_PWR_LVL0) {
		ret = ((psysr & PSYSR_AFF_L0) != 0U) ? HW_ON : HW_OFF;
	} else {
		/* power_level == ARM_PWR_LVL1 */
		ret = ((psysr & PSYSR_AFF_L1) != 0U) ? HW_ON : HW_OFF;
	}

	return ret;
}

/*
 * The ax650 doesn't truly support power management at SYSTEM power domain. The
 * SYSTEM_SUSPEND will be down-graded to the cluster level within the platform
 * layer. The `fake` SYSTEM_SUSPEND allows us to validate some of the driver
 * save and restore sequences on ax650.
 */
#if !ARM_BL31_IN_DRAM
static void ax650_get_sys_suspend_power_state(psci_power_state_t *req_state)
{
	unsigned int i;

	for (i = ARM_PWR_LVL0; i <= PLAT_MAX_PWR_LVL; i++)
		req_state->pwr_domain_state[i] = ARM_LOCAL_STATE_OFF;
}
#endif

/*******************************************************************************
 * ARM standard platform handler called to check the validity of the power state
 * parameter.
 ******************************************************************************/
int validate_power_state(unsigned int power_state,
			    psci_power_state_t *req_state)
{
	unsigned int pstate = psci_get_pstate_type(power_state);
	unsigned int pwr_lvl = psci_get_pstate_pwrlvl(power_state);
	unsigned int i;

	assert(req_state != NULL);

	if (pwr_lvl > PLAT_MAX_PWR_LVL)
		return PSCI_E_INVALID_PARAMS;

	/* Sanity check the requested state */
	if (pstate == PSTATE_TYPE_STANDBY) {
		/*
		 * It's possible to enter standby only on power level 0
		 * Ignore any other power level.
		 */
		if (pwr_lvl != ARM_PWR_LVL0)
			return PSCI_E_INVALID_PARAMS;

		req_state->pwr_domain_state[ARM_PWR_LVL0] =
					ARM_LOCAL_STATE_RET;
	} else {
		for (i = ARM_PWR_LVL0; i <= pwr_lvl; i++)
			req_state->pwr_domain_state[i] =
					ARM_LOCAL_STATE_OFF;
	}

	/*
	 * We expect the 'state id' to be zero.
	 */
	if (psci_get_pstate_id(power_state) != 0U)
		return PSCI_E_INVALID_PARAMS;

	return PSCI_E_SUCCESS;
}


/*******************************************************************************
 * Handler to filter PSCI requests.
 ******************************************************************************/
/*
 * The system power domain suspend is only supported only via
 * PSCI SYSTEM_SUSPEND API. PSCI CPU_SUSPEND request to system power domain
 * will be downgraded to the lower level.
 */
static int ax650_validate_power_state(unsigned int power_state,
			    psci_power_state_t *req_state)
{
	int rc;
	rc = validate_power_state(power_state, req_state);

	/*
	 * Ensure that the system power domain level is never suspended
	 * via PSCI CPU SUSPEND API. Currently system suspend is only
	 * supported via PSCI SYSTEM SUSPEND API.
	 */
	req_state->pwr_domain_state[ARM_PWR_LVL2] = ARM_LOCAL_STATE_RUN;
	return rc;
}

/*
 * Custom `translate_power_state_by_mpidr` handler for ax650. Unlike in the
 * `ax650_validate_power_state`, we do not downgrade the system power
 * domain level request in `power_state` as it will be used to query the
 * PSCI_STAT_COUNT/RESIDENCY at the system power domain level.
 */
static int ax650_translate_power_state_by_mpidr(u_register_t mpidr,
		unsigned int power_state,
		psci_power_state_t *output_state)
{
	return validate_power_state(power_state, output_state);
}

/*******************************************************************************
 * ARM standard platform handler called to check the validity of the non secure
 * entrypoint. Returns 0 if the entrypoint is valid, or -1 otherwise.
 ******************************************************************************/
int ax650_validate_ns_entrypoint(uintptr_t entrypoint)
{
#if 0
	/*
	 * Check if the non secure entrypoint lies within the non
	 * secure DRAM.
	 */
	if ((entrypoint >= ARM_NS_DRAM1_BASE) && (entrypoint <
			(ARM_NS_DRAM1_BASE + ARM_NS_DRAM1_SIZE))) {
		return 0;
	}
#ifdef __aarch64__
	if ((entrypoint >= ARM_DRAM2_BASE) && (entrypoint <
			(ARM_DRAM2_BASE + ARM_DRAM2_SIZE))) {
		return 0;
	}
#endif

	return -1;
#endif
	//todo
	return 0;
}

int ax650_validate_psci_entrypoint(uintptr_t entrypoint)
{
	return (ax650_validate_ns_entrypoint(entrypoint) == 0) ? PSCI_E_SUCCESS :
		PSCI_E_INVALID_ADDRESS;
}


/*******************************************************************************
 * Export the platform handlers via plat_arm_psci_pm_ops. The ARM Standard
 * platform layer will take care of registering the handlers with PSCI.
 ******************************************************************************/
plat_psci_ops_t plat_ax650_psci_pm_ops = {
	.cpu_standby = ax650_cpu_standby,
	.pwr_domain_on = ax650_pwr_domain_on,
	.pwr_domain_off = ax650_pwr_domain_off,
	.pwr_domain_suspend = ax650_pwr_domain_suspend,
	.pwr_domain_on_finish = ax650_pwr_domain_on_finish,
	.pwr_domain_on_finish_late = ax650_pwr_domain_on_finish_late,
	.pwr_domain_suspend_finish = ax650_pwr_domain_suspend_finish,
	//.system_off = ax650_system_off,
	//.system_reset = ax650_system_reset,
	.validate_power_state = ax650_validate_power_state,
	.validate_ns_entrypoint = ax650_validate_psci_entrypoint,
	.translate_power_state_by_mpidr = ax650_translate_power_state_by_mpidr,
	.get_node_hw_state = ax650_node_hw_state,
#if !ARM_BL31_IN_DRAM
	/*
	 * The TrustZone Controller is set up during the warmboot sequence after
	 * resuming the CPU from a SYSTEM_SUSPEND. If BL31 is located in SRAM
	 * this is  not a problem but, if it is in TZC-secured DRAM, it tries to
	 * reconfigure the same memory it is running on, causing an exception.
	 */
	.get_sys_suspend_power_state = ax650_get_sys_suspend_power_state,
#endif
	//.mem_protect_chk	= arm_psci_mem_protect_chk,
	//.read_mem_protect	= arm_psci_read_mem_protect,
	//.write_mem_protect	= arm_nor_psci_write_mem_protect,
};

int plat_setup_psci_ops(uintptr_t sec_entrypoint,
			const plat_psci_ops_t **psci_ops)
{
	ax650_sec_entrypoint = sec_entrypoint;

	*psci_ops = &plat_ax650_psci_pm_ops;

	return 0;
}


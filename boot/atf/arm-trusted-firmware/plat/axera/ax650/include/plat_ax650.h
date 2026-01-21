/*
 * Copyright (c) 2013-2018, AX650 Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef PLAT_AX650_H
#define PLAT_AX650_H

#if 0
/* FVP Power controller register offset etc */
#define PPOFFR_OFF		U(0x0)
#define PPONR_OFF		U(0x4)
#define PCOFFR_OFF		U(0x8)
#define PWKUPR_OFF		U(0xc)
#define PSYSR_OFF		U(0x10)

#define PWKUPR_WEN		BIT_32(31)

#define PSYSR_AFF_L2		BIT_32(31)
#define PSYSR_AFF_L1		BIT_32(30)
#define PSYSR_AFF_L0		BIT_32(29)
#define PSYSR_WEN		BIT_32(28)
#define PSYSR_PC		BIT_32(27)
#define PSYSR_PP		BIT_32(26)

#define PSYSR_WK_SHIFT		24
#define PSYSR_WK_WIDTH		0x2
#define PSYSR_WK_MASK		((1U << PSYSR_WK_WIDTH) - 1U)
#define PSYSR_WK(x)		((x) >> PSYSR_WK_SHIFT) & PSYSR_WK_MASK

#define WKUP_COLD		U(0x0)
#define WKUP_RESET		U(0x1)
#define WKUP_PPONR		U(0x2)
#define WKUP_GICREQ		U(0x3)

#define PSYSR_INVALID		U(0xffffffff)
#endif

#if defined(IMAGE_BL31)
/*
 * Use this macro to instantiate lock before it is used in below
 * ax650_lock_xxx() macros
 */
#define AX650_INSTANTIATE_LOCK	static DEFINE_BAKERY_LOCK(ax650_lock)
#define AX650_LOCK_GET_INSTANCE	(&ax650_lock)
#define AX650_SCMI_INSTANTIATE_LOCK	spinlock_t ax650_scmi_lock

#define AX650_SCMI_LOCK_GET_INSTANCE	(&ax650_scmi_lock)

/*
 * These are wrapper macros to the Coherent Memory Bakery Lock API.
 */
#define ax650_lock_init()		bakery_lock_init(&ax650_lock)
#define ax650_lock_get()		bakery_lock_get(&ax650_lock)
#define ax650_lock_release()	bakery_lock_release(&ax650_lock)


#endif


#ifndef __ASSEMBLER__

#include <stdint.h>

/*******************************************************************************
 * Function & variable prototypes
 ******************************************************************************/


/*
 * Mandatory functions required in AX650 standard platforms
 */
unsigned int plat_ax650_get_cluster_core_count(u_register_t mpidr);
void plat_ax650_gic_driver_init(void);
void plat_ax650_gic_init(void);
void plat_ax650_gic_cpuif_enable(void);
void plat_ax650_gic_cpuif_disable(void);
void plat_ax650_gic_redistif_on(void);
void plat_ax650_gic_redistif_off(void);
void plat_ax650_gic_pcpu_init(void);
void plat_ax650_gic_save(void);
void plat_ax650_gic_resume(void);

#endif /*__ASSEMBLER__*/

#endif /* AX650_PWRC_H */

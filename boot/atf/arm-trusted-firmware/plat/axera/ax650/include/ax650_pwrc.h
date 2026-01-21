/*
 * Copyright (c) 2013-2018, AX650 Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef AX650_PWRC_H
#define AX650_PWRC_H

#include <stdint.h>
#include <pmu.h>

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

/*******************************************************************************
 * Function & variable prototypes
 ******************************************************************************/
void ax650_pwrc_write_ppoffr(u_register_t mpidr);
void ax650_pwrc_write_pponr(u_register_t mpidr);
#if 0
void ax650_pwrc_set_wen(u_register_t mpidr);
void ax650_pwrc_clr_wen(u_register_t mpidr);
void ax650_pwrc_write_pcoffr(u_register_t mpidr);
unsigned int ax650_pwrc_get_cpu_wkr(u_register_t mpidr);
#endif
unsigned int ax650_pwrc_read_psysr(u_register_t mpidr);
void plat_ax650_pwrc_setup(void);
void ax650_sys_pwrdwn();


#endif /* AX650_PWRC_H */

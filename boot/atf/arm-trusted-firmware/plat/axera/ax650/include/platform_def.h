/*
 * Copyright (c) 2017-2019, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef PLATFORM_DEF_H
#define PLATFORM_DEF_H

#include <arch.h>
#include <plat/common/common_def.h>

/*******************************************************************************
 * Platform binary types for linking
 ******************************************************************************/
#define PLATFORM_LINKER_FORMAT		"elf64-littleaarch64"
#define PLATFORM_LINKER_ARCH		aarch64

/*******************************************************************************
 * Generic platform constants
 ******************************************************************************/

/* Size of cacheable stacks */

#define PLATFORM_STACK_SIZE 0x800

#define	SYS_COUNTER_FREQ  24000000

#define FIRMWARE_WELCOME_STR		"Booting Trusted Firmware\n"

#define PLATFORM_SYSTEM_COUNT		1
#define PLATFORM_CLUSTER_COUNT		1
#define PLATFORM_CORE_COUNT         8
#define PLAT_MAX_PWR_LVL		    MPIDR_AFFLVL2

#define ARM_BL31_PLAT_PARAM_VAL		ULL(0x0f1e2d3c4b5a6978)

#define ARM_PWR_LVL0		MPIDR_AFFLVL0
#define ARM_PWR_LVL1		MPIDR_AFFLVL1
#define ARM_PWR_LVL2		MPIDR_AFFLVL2

/* Local power state for power domains in Run state. */
#define ARM_LOCAL_STATE_RUN	U(0)
/* Local power state for retention. Valid only for CPU power domains */
#define ARM_LOCAL_STATE_RET	U(1)
/* Local power state for OFF/power-down. Valid for CPU and cluster power
   domains */
#define ARM_LOCAL_STATE_OFF	U(2)

/*
 * This macro defines the deepest retention state possible. A higher state
 * id will represent an invalid or a power down state.
 */
#define PLAT_MAX_RET_STATE		U(1)

/*
 * This macro defines the deepest power down states possible. Any state ID
 * higher than this is invalid.
 */
#define PLAT_MAX_OFF_STATE		U(2)

/*******************************************************************************
 * Platform memory map related constants
 ******************************************************************************/
/* TF text, ro, rw, Size: 1MB  temp set in iram later will change to ddr todo */
#define TZRAM_BASE		(0x4800000)
#define TZRAM_SIZE		(0x20000)

/*******************************************************************************
 * BL31 specific defines.
 ******************************************************************************/
#define BL31_BASE		(TZRAM_BASE + 0x1000)
#define BL31_LIMIT		(TZRAM_BASE + TZRAM_SIZE - 0x1000)

/*******************************************************************************
 * Platform specific page table and MMU setup constants
 ******************************************************************************/
#define PLAT_VIRT_ADDR_SPACE_SIZE   (1ULL << 36)
#define PLAT_PHY_ADDR_SPACE_SIZE    (1ULL << 36)
#define MAX_XLAT_TABLES		9
#define MAX_MMAP_REGIONS	33

/*******************************************************************************
 * Declarations and constants to access the mailboxes safely. Each mailbox is
 * aligned on the biggest cache line size in the platform. This is known only
 * to the platform as it might have a combination of integrated and external
 * caches. Such alignment ensures that two maiboxes do not sit on the same cache
 * line at any cache level. They could belong to different cpus/clusters &
 * get written while being protected by different locks causing corruption of
 * a valid mailbox address.
 ******************************************************************************/
#define CACHE_WRITEBACK_SHIFT	6
#define CACHE_WRITEBACK_GRANULE	(1 << CACHE_WRITEBACK_SHIFT)

#endif /* PLATFORM_DEF_H */

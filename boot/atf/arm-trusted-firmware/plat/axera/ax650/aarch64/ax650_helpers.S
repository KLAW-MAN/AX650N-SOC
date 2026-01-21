/*
 * Copyright (c) 2017-2020, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <arch.h>
#include <asm_macros.S>


	.globl	plat_my_core_pos

func plat_my_core_pos
	mrs	x0, mpidr_el1
	mov x1, #MPIDR_CPU_MASK
	and	x0, x1, x0, LSR #MPIDR_AFF1_SHIFT
	ret
endfunc plat_my_core_pos
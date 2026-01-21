/*
 * common.S - common definitions useful for boot code
 *
 * Copyright (C) 2013 ARM Limited. All rights reserved.
 * Copyright (C) 2016 Synopsys, Inc. All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE.txt file.
 */

#define MPIDR_ID_BITS (0xff00ffffff)	//Multi-Processor Affinity Register

/*
 *CurrentEL register(32-bit, read only): current exception level. 
 * bit[31:4], bit[1:0] Reserved.
 * EL bit[3:2]:
 * 00 for EL0
 * 01 for EL1
 * 10 for EL2
 * 11 for EL3
 */
#define CURRENTEL_EL3 (3 << 2)	

/* SCTRL_EL2: System Control Register(EL2), 64-bit, Provide top level control of the
 * system, including its memory system, at EL2.
 * RES1 bits,  little-endian, caches and MMU off, no alignment checking,
 * no WXN.
 */
#define SCTLR_EL2_RESET (3 << 28 | 3 << 22 | 1 << 18 | 1 << 16 | 1 << 11 | 3 << 4)

/*
 * SPSR_EL2: Saved Program Status Register(EL2)
 *
 *
 */
#define SPSR_A    (1 << 8)    /* System Error masked */
#define SPSR_D    (1 << 9)    /* Debug masked */
#define SPSR_I    (1 << 7)    /* IRQ masked */
#define SPSR_F    (1 << 6)    /* FIQ masked */
#define SPSR_EL2H (9 << 0)    /* EL2 Handler mode */

#define SPSR_KERNEL (SPSR_A | SPSR_D | SPSR_I | SPSR_F | SPSR_EL2H)

/*
 * Drop EL to that specified by the spsr value in register mode, at
 * the address specified in register addr.
 */
.macro drop_el mode addr
msr elr_el3, \addr
msr spsr_el3, \mode
eret		//Exception Return
.endm

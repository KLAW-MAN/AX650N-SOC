/*
 * spin.S - spin-table boot protocol implementation
 *
 * Copyright (C) 2013 ARM Limited. All rights reserved.
 * Copyright (C) 2016 Synopsys, Inc. All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE.txt file.
 */

#include "common.S"

    .text

    .globl start_no_el3
    .globl start_el3

start_el3:
    /*
     * Prepare the switch to the EL2_SP1 mode from EL3
     */
    ldr x0, =SCTLR_EL2_RESET		// #define SCTLR_EL2_RESET (3 << 28 | 3 << 22 | 1 << 18 | 1 << 16 | 1 << 11 | 3 << 4)
    msr sctlr_el2, x0			// SCTLR_EL2, System Control Register, move to SR from Reg
    ldr x0, =start_no_el3           	// Return after mode switch
    mov x1, #SPSR_KERNEL		// #define SPSR_KERNEL (SPSR_A | SPSR_D | SPSR_I | SPSR_F | SPSR_EL2H) in common.S
    drop_el x1, x0			// drop_el is macro defined in common.S

start_no_el3:
    /*
     * Kernel parameters,XZR: 64-bit zero register.
     */
    b .
    mov x0, xzr
    mov x1, xzr
    mov x2, xzr
    mov x3, xzr

    mrs x4, mpidr_el1
    ldr x5, =MPIDR_ID_BITS		// #define MPIDR_ID_BITS (0xff00ffffff) in common.S
    tst x4, x5
    b.eq 2f				// if (x4)&(x5) = 0 then branch forward to 2:, CPSR bitfiled ZERO=1

    /*
     * Secondary CPUs: core1/2/3
     */
1:  wfe					// WFE: wait for event; WFI:wait for interrupt
    ldr x4, =mbox_address		//WFE/WFI: put the core in a low-power state by disabling the clocks in the core while keeping the core powered up
    ldr x4, [x4]			//config mbox_address 0x8000fff8
    ldr   x4, [x4]			
    cbz x4, 1b				// CBZ:compare and branch zero: conditionally jumps to 1b if x4 is equal to zero.    
    br x4                          	// BR: branch to the given address

    /*
     * Primary CPU: core 0
     */
2:  ldr x0, =dtb_address            	// device tree blob, binary large object, config dtb_address  0x10000000
    ldr x0, [x0]
    ldr x4, =kern_address		// config kern_address 0x80080000 in config.S
    ldr x4, [x4]
    br x4

    .ltorg

    .org 0x100

/*
 * config.S - boot wrapper configuration variables
 *
 * Copyright (C) 2016 Synopsys, Inc. All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE.txt file.
 */

    .macro config name value		// macro name: config
    .globl \name					// parameter list: name, value
\name:
    .quad \value					// 64-bit memory location, initialized memory to value
    .endm							// just like C code: long a=value;

    .data

    // Align values on an 8-byte = 64-bit boundary (size of a .quad)
    .align 3
    
    /*
     * Address at which Linux Image has been loaded in memory.
     *
     * This is the entry point of the Linux kernel, where the bootwrapper jumps
     * after it has finished initializing the design.
     */
    config kern_address 0x80080000
    
    /*
     * Address at which the Linux Device Tree blob has been loaded in memory.
     *
     * It will be in CPU register X0 when the execution jumps to the Linux
     * entry point.
     */
    config dtb_address  0x10000000

    /*
     * Address used as a mailbox to hold the secondary cores until Linux is
     * ready to initialize them.
     *
     * It must have the same value than the cpu-release-addr device tree
     * property, otherwise secondary cores will fail to boot.
     * Typically, the value at this address is 0 until Linux writes the address
     * at which the secondary cores should jump.
     */
    config mbox_address 0x8000fff8

    /*
     * Address of the GIC slave port on the bus.
     *
     * The Distributor memory regions will be probed in order to choose between
     * GIC400 and GIC500 initialization code.
     * Generic Interrupt Controller
     */
#define GIC_BASE 0x4900000
    // GIC400 Distributor registers
    config gic400d_address GIC_BASE + 0x1000
    // GIC400 CPU Interface registers
    config gic400c_address GIC_BASE + 0x2000
    // GIC500 Distributor registers
    config gic500d_address GIC_BASE
    // GIC500 Redistributor registers (the value below works only with 5 to 8 CPU cores)
    config gic500r_address GIC_BASE + 0x100000

    /*
     * Frequency, in Hz, of the global counter driving the cluster timers.
     *
     * It will be in CPU register cntfrq_el0 when the execution jumps to the
     * Linux entry point.
     */
    config cnt_freq 100000000
    
    /*
     * Flags indicating which type of Cache Coherent Interconnect is present
     */
     config cci400_present      0
     config cci500_present      0
     config cci550_present      0
     config cmn600_present      0
    
    /*
     * Cache Coherent Interconnet initialization 
     */
#define INTERCONNECT_BASE 0x6C000000

    config cci400_snoop_ctrl_0  INTERCONNECT_BASE + 0x91000
    config cci5x0_snoop_ctrl_0  INTERCONNECT_BASE + 0x1000
    config cmn600_base_addr 0x50000000

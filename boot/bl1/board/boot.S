#include "common.S"
    .text

    .globl _start
_start:					// the first line code
    mrs x0, mpidr_el1			// MPIDR_EL1: dedicated register, Multiprocessor Affinity Register for Process ID,(MPIDR_EL1)=0x80000000
    and x0, x0, #0xF00			// select CPU ID number

    bl  setup_stack			// setup_stack in stack.S

    /*PTEST entry*/
    mov  x0, #0x18
    movk x0, #0x550, lsl #16
    ldr w0, [x0]
    and w0, w0, #0x100000
    cmp w0, #0x0			// 1: enter PTEST. (bit20 default is 0)
    b.ne 1f

    /*enable I-cache*/
    mrs x0, sctlr_el3
    orr x0, x0, #(0x1<<12) 		// I bit(instruction cache)
    msr sctlr_el3, x0

    /* Stack setup is only required for the primary core */
    mrs x0, mpidr_el1			// MPIDR_EL1: dedicated register, Multiprocessor Affinity Register for Process ID,(MPIDR_EL1)=0x80000000
    ldr x1, =MPIDR_ID_BITS		// in common.S reads #define MPIDR_ID_BITS (0xff00ffffff), (x1)=0xdeadbeaf,(x1)=0xff00ffffff
    tst x0, x1				// bitwise AND, (x0) & (x1), then update CPSR
    mov x0, #0      			// CPU ID

    b.ne second_core_loop        	// secondary cores skip stack setup

    bl clear_bss_section
#ifdef ROM    
    bl copy_data_section
#endif

    bl main_rom_code

    /*branch to SPL, 0x100400*/
/*
	ldr x0, =0x00100400
    br x0
*/

1:  ldr x0, =0x00100000
    br x0

second_core_loop:
2:  wfe				// WFE: wait for event; WFI:wait for interrupt
    ldr x4, =0x05500094		// WFE/WFI: put the core in a low-power state by disabling the clocks in the core while keeping the core powered up
    ldr w4, [x4]		// config mbox_address 0x8000fff8
    cbz x4, 2b			// CBZ:compare and branch zero: conditionally jumps to 2b if x4 is equal to zero.
    lsl x4, x4, #4		// logical shift left 4 bit to match DDR's address
    br x4			// BR: branch to the given address


    .ltorg			// The LTORG directive instructs the assembler to assemble the current literal pool immediately.
				// literal pool: Literal pools are areas of constant data in a code section

    .org    0x100		// start from address 0x100


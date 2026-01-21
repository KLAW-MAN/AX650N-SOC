/*
 * Copyright (c) 2017-2019, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __AX650_DEF_H_
#define __AX650_DEF_H_

#define MAJOR_VERSION		(1)
#define MINOR_VERSION		(2)

#define SIZE_K(n)		((n) * 1024)
#define SIZE_M(n)		((n) * 1024 * 1024)

#define UART0_BASE		0x2016000
#define UART0_SIZE		SIZE_K(4)

#define UART1_BASE		0x2017000
#define UART1_SIZE		SIZE_K(4)

#define UART2_BASE		0x2018000
#define UART2_SIZE		SIZE_K(4)

#define PMU_BASE		0x4C00000
#define PMU_SIZE		SIZE_M(1)

#define IRAM0_BASE      0x4800000
#define IRAM0_SIZE      SIZE_K(128)

#define IRAM1_BASE      0x4840000
#define IRAM1_SIZE      SIZE_K(128)

#define GIC400_BASE		0x4900000
#define GIC400_SIZE		SIZE_K(32)

#define DDR0_SYS_BASE   0x8000000
#define DDR0_SYS_SIZE   SIZE_K(320)

#define DDR0_PHY_BASE   0xA000000
#define DDR0_PHY_SIZE   SIZE_M(32)

#define DDR1_SYS_BASE   0xC000000
#define DDR1_SYS_SIZE   SIZE_K(320)

#define DDR1_PHY_BASE   0xE000000
#define DDR1_PHY_SIZE   SIZE_M(32)

#define FLASH1_SYS_BASE  0x100000
#define FLASH1_SYS_SIZE  SIZE_K(320)

#define PERI_SYS_BASE   0x2000000
#define PERI_SYS_SIZE   SIZE_K(1344)

#define AON_SYS_BASE    0x4000000
#define AON_SYS_SIZE    SIZE_M(16)

#define AON_SYS_BF_IRAME_BASE    0x4000000
#define AON_SYS_BF_IRAME_SIZE    SIZE_M(7)

#define AON_SYS_IRAME_BASE    0x4800000
#define AON_SYS_IRAME_SIZE    SIZE_K(512)

#define AON_SYS_AF_IRAME_BASE    0x4C00000
#define AON_SYS_AF_IRAME_SIZE    SIZE_M(4)

#define MM_SYS_BASE     0x10000000
#define MM_SYS_SIZE     SIZE_M(2)

#define ISP_SYS_BASE     0x12000000
#define ISP_SYS_SIZE     SIZE_M(9)

#define SEN_SYS_BASE     0x13000000
#define SEN_SYS_SIZE     SIZE_M(14)

#define NPU_SYS_BASE     0x14000000
#define NPU_SYS_SIZE     SIZE_M(38)

#define VDSP_SYS_BASE     0x18000000
#define VDSP_SYS_SIZE     SIZE_M(6)

#define VDEC_SYS_BASE     0x19000000
#define VDEC_SYS_SIZE     SIZE_K(100)

#define VENC_SYS_BASE     0x1A000000
#define VENC_SYS_SIZE     SIZE_K(84)

#define FLASH2_SYS_BASE   0x20000000
#define FLASH2_SYS_SIZE   SIZE_M(129)

#define PIPE_SYS_BASE     0x30000000
#define PIPE_SYS_SIZE     SIZE_M(17)

/**************************************************************************
 * UART related constants
 **************************************************************************/
#ifndef HAPS
#define AX650_BAUDRATE	115200
#define AX650_UART_CLOCK	200000000
#else
#define AX650_BAUDRATE	38400
#define AX650_UART_CLOCK	10000000
#endif

/******************************************************************************
 * System counter frequency related constants
 ******************************************************************************/
//#define SYS_COUNTER_FREQ_IN_TICKS	24000000
//#define SYS_COUNTER_FREQ_IN_MHZ		24

/******************************************************************************
 * GIC-400 & interrupt handling related constants
 ******************************************************************************/

/* Base compatible GIC memory map */
#define AX650_GICD_BASE		(GIC400_BASE + 0x1000)
#define AX650_GICC_BASE		(GIC400_BASE + 0x2000)

/* gtimer ref regs */
#define FLASH_CLK_RST_BASE  (FLASH1_SYS_BASE + 0x10000)
#define FLASH_SW_RST_0_CLR  (FLASH_CLK_RST_BASE + 0x3C)
#define BIT_GTIMR_SW_RST_CLR (1 << 9)
#define FLASH_CLK_EB_0_SET  (FLASH_CLK_RST_BASE + 0x20)
#define BIT_CLK_GTMR_EB_SET (1 << 6)
#define GTMR_BASE           (FLASH1_SYS_BASE + 0x40000)
#define GTMR_FID            (GTMR_BASE + 0x20)
#define GTMR_CNTCR          (GTMR_BASE + 0x0)
#define BIT_GTMR_EN         (1 << 0)

/* CPU_SYS_COMM_GLB */
#define CPU_SYS_COMM_GLB_BASE_ADDR    0x4508000

/* CPU_SYS_GLB */
#define CPU_SYS_GLB_BASE_ADDR    0x4520000

/* PMU GLB BASE */
#define PMU_GLB_BASE_ADDR    0x4C00000

#endif /* AX650_DEF_H */

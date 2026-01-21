/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-08-20     zx.chen      CSI Core Peripheral Access Layer Header File for
 *                             CSKYSOC Device Series
 */

#ifndef __MC50_E907_SOC_H__
#define __MC50_E907_SOC_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifndef IHS_VALUE
#define IHS_VALUE    (24000000)
#endif

#ifndef EHS_VALUE
#define EHS_VALUE    (20000000)
#endif

/* ================================================================================ */
/* ================                Common  Section                 ================ */
/* ================================================================================ */

/* common_ck_rst_cfg */
#define COMMON_RESET_CLK_BASE         0x4210000
#define CLK_EB_1_OFFSET               0x0C
#define CLK_EB_2_OFFSET               0x10
#define SW_RST_0_OFFSET               0x1C
#define CLK_EB_1_SET_OFFSET           0x40
#define CLK_EB_1_CLR_OFFSET           0x44
#define CLK_EB_2_SET_OFFSET           0x48
#define CLK_EB_2_CLR_OFFSET           0x4c
#define SW_RST_0_SET_OFFSET           0x60
#define SW_RST_0_CLR_OFFSET           0x64

/* mailbox */
#define MC50_MAILBOX_BASE             0x452C000UL

/* hw_spinlock */
#define MC50_HWSPINLOCK_BASE          0x4510000UL

/* dummy register for riscv */
#define COMM_SYS_GLB        0x4200000
#define COMM_SYS_DUMMY_SW11 (COMM_SYS_GLB + 0x1fc)
#define RISCV_DONE_MAGIC    0x646f6e65

/* ================================================================================ */
/* ================              Peripheral Section                ================ */
/* ================================================================================ */

/* periph_clk_rst */
#define PERIPH_CLK_RST_BASE    0x02002000
#define PERIPH_CLK_EB_1        0x08
#define PERIPH_CLK_EB_1_SET    0x48
#define PERIPH_CLK_EB_1_CLR    0x4c
#define PERIPH_CLK_EB_3        0x10
#define PERIPH_CLK_EB_3_SET    0x58
#define PERIPH_CLK_EB_3_CLR    0x5c
#define PERIPH_CLK_EB_4        0x14
#define PERIPH_CLK_EB_4_SET    0x60
#define PERIPH_CLK_EB_4_CLR    0x64
#define PERIPH_CLK_EB_5        0x18
#define PERIPH_CLK_EB_5_SET    0x68
#define PERIPH_CLK_EB_5_CLR    0x6c

/* UART 0/1/2/3/4/5/6/7/8/9/10 */
#define UART0_BASE_ADDR       0x02016000
#define UART1_BASE_ADDR       0x02017000
#define UART2_BASE_ADDR       0x02018000
#define UART3_BASE_ADDR       0x02019000
#define UART4_BASE_ADDR       0x0201a000
#define UART5_BASE_ADDR       0x0201b000
#define UART6_BASE_ADDR       0x0201c000
#define UART7_BASE_ADDR       0x0201d000
#define UART8_BASE_ADDR       0x0201e000
#define UART9_BASE_ADDR       0x0201f000
#define UART10_BASE_ADDR      0x02020000

/* I2C 0/1/2/3/4/5/6/7/8/9/10/11/12/13/14 */
#define I2C0_BASE_ADDR        0x02021000
#define I2C1_BASE_ADDR        0x02022000
#define I2C2_BASE_ADDR        0x02023000
#define I2C3_BASE_ADDR        0x02024000
#define I2C4_BASE_ADDR        0x02025000
#define I2C5_BASE_ADDR        0x02026000
#define I2C6_BASE_ADDR        0x02027000
#define I2C7_BASE_ADDR        0x02028000
#define I2C8_BASE_ADDR        0x02029000
#define I2C9_BASE_ADDR        0x0202a000
#define I2C10_BASE_ADDR       0x0202b000
#define I2C11_BASE_ADDR       0x0202c000
#define I2C12_BASE_ADDR       0x0202d000
#define I2C13_BASE_ADDR       0x0202e000
#define I2C14_BASE_ADDR       0x0202f000

/* GPIO group 0/1/2/3 */
#define GPIO_GP0_BASE_ADDR           (0x02003000UL)
#define GPIO_GP1_BASE_ADDR           (0x02004000UL)
#define GPIO_GP2_BASE_ADDR           (0x02005000UL)
#define GPIO_GP3_BASE_ADDR           (0x02006000UL)
#define GPIO_GP4_BASE_ADDR           (0x02007000UL)
#define GPIO_GP5_BASE_ADDR           (0x04e00000UL)

/* timer64 0 */
#define TMR64_0_BASE_ADDR    0x4e09000
#define TMR64_0_FREQ_M       24

#ifdef __cplusplus
}
#endif

#endif  /* __MC50_E907_SOC_H__ */

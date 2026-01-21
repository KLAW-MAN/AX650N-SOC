/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include <rtdevice.h>
#include "ax_common.h"

static reg_info_t pinmux_info[] = {
    /* UART0/UART1 */
    {0x000000C3, 0x0425080C},  /* PadName = UART0_TXD            Fuction = UART0_TXD */
    {0x000000D3, 0x04250818},  /* PadName = UART0_RXD            Fuction = UART0_RXD */
    {0x000000C3, 0x04250824},  /* PadName = UART1_TXD            Fuction = UART1_TXD */
    {0x000000D3, 0x04250830},  /* PadName = UART1_RXD            Fuction = UART1_RXD */

    /* I2C0/I2C1 */
    {0x00010083, 0x0425063C},  /* PadName = GPIO4_A19            Fuction = I2C0_SCL_M */
    {0x00010083, 0x04250648},  /* PadName = GPIO4_A20            Fuction = I2C0_SDA_M */
    {0x00000083, 0x0425100C},  /* PadName = I2C1_SCL             Fuction = I2C1_SCL */
    {0x00000083, 0x04251018},  /* PadName = I2C1_SDA             Fuction = I2C1_SDA */
};

static int pinmux_init(void)
{
    for (int i = 0; i < sizeof(pinmux_info) / sizeof(pinmux_info[0]); i++) {
        ax_writel(pinmux_info[i].reg_val, pinmux_info[i].reg_addr);
    }
    return 0;
}

INIT_BOARD_EXPORT(pinmux_init);

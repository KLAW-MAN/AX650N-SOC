/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include <rthw.h>
#include <rtthread.h>
#include <rtdevice.h>
#include "board.h"
#include "drv_uart.h"
#include "drv_uart_reg.h"
#include "ax_common.h"
#include "log_2_mem.h"

static struct CSKY_UART_REG *uart_base_addr[uart_max] = {
    (struct CSKY_UART_REG *)UART0_BASE_ADDR,
    (struct CSKY_UART_REG *)UART1_BASE_ADDR,
    (struct CSKY_UART_REG *)UART2_BASE_ADDR,
    (struct CSKY_UART_REG *)UART3_BASE_ADDR,
    (struct CSKY_UART_REG *)UART4_BASE_ADDR,
    (struct CSKY_UART_REG *)UART5_BASE_ADDR,
    (struct CSKY_UART_REG *)UART6_BASE_ADDR,
    (struct CSKY_UART_REG *)UART7_BASE_ADDR,
    (struct CSKY_UART_REG *)UART8_BASE_ADDR,
    (struct CSKY_UART_REG *)UART9_BASE_ADDR,
    (struct CSKY_UART_REG *)UART10_BASE_ADDR
};

static uint32_t uart_irq_no[uart_max] = {
    INT_REQ_PERIPH_UART0,
    INT_REQ_PERIPH_UART1,
    INT_REQ_PERIPH_UART2,
    INT_REQ_PERIPH_UART3,
    INT_REQ_PERIPH_UART4,
    INT_REQ_PERIPH_UART5,
    INT_REQ_PERIPH_UART6,
    INT_REQ_PERIPH_UART7,
    INT_REQ_PERIPH_UART8,
    INT_REQ_PERIPH_UART9,
    INT_REQ_PERIPH_UART10
};

static reg_info_t uart_pclk_info[uart_max] = {
    {AX_BIT(9),  PERIPH_CLK_RST_BASE + PERIPH_CLK_EB_5_SET},
    {AX_BIT(11), PERIPH_CLK_RST_BASE + PERIPH_CLK_EB_5_SET},
    {AX_BIT(12), PERIPH_CLK_RST_BASE + PERIPH_CLK_EB_5_SET},
    {AX_BIT(13), PERIPH_CLK_RST_BASE + PERIPH_CLK_EB_5_SET},
    {AX_BIT(14), PERIPH_CLK_RST_BASE + PERIPH_CLK_EB_5_SET},
    {AX_BIT(15), PERIPH_CLK_RST_BASE + PERIPH_CLK_EB_5_SET},
    {AX_BIT(16), PERIPH_CLK_RST_BASE + PERIPH_CLK_EB_5_SET},
    {AX_BIT(17), PERIPH_CLK_RST_BASE + PERIPH_CLK_EB_5_SET},
    {AX_BIT(18), PERIPH_CLK_RST_BASE + PERIPH_CLK_EB_5_SET},
    {AX_BIT(19), PERIPH_CLK_RST_BASE + PERIPH_CLK_EB_5_SET},
    {AX_BIT(10), PERIPH_CLK_RST_BASE + PERIPH_CLK_EB_5_SET}
};

static reg_info_t uart_clk_info[uart_max] = {
    {AX_BIT(18), PERIPH_CLK_RST_BASE + PERIPH_CLK_EB_3_SET},
    {AX_BIT(20), PERIPH_CLK_RST_BASE + PERIPH_CLK_EB_3_SET},
    {AX_BIT(21), PERIPH_CLK_RST_BASE + PERIPH_CLK_EB_3_SET},
    {AX_BIT(22), PERIPH_CLK_RST_BASE + PERIPH_CLK_EB_3_SET},
    {AX_BIT(23), PERIPH_CLK_RST_BASE + PERIPH_CLK_EB_3_SET},
    {AX_BIT(24), PERIPH_CLK_RST_BASE + PERIPH_CLK_EB_3_SET},
    {AX_BIT(25), PERIPH_CLK_RST_BASE + PERIPH_CLK_EB_3_SET},
    {AX_BIT(26), PERIPH_CLK_RST_BASE + PERIPH_CLK_EB_3_SET},
    {AX_BIT(27), PERIPH_CLK_RST_BASE + PERIPH_CLK_EB_3_SET},
    {AX_BIT(28), PERIPH_CLK_RST_BASE + PERIPH_CLK_EB_3_SET},
    {AX_BIT(19), PERIPH_CLK_RST_BASE + PERIPH_CLK_EB_3_SET}
};

static struct csky_uart_dev uart_dev;

static void uart_clk_init(uart_e uart)
{
    ax_writel(uart_pclk_info[uart].reg_val, uart_pclk_info[uart].reg_addr);
    ax_writel(uart_clk_info[uart].reg_val, uart_clk_info[uart].reg_addr);
}

void csky_uart_isr(int irqno, void *param)
{
    struct csky_uart_dev *uart = CSKY_UART_DEVICE(param);
    RT_ASSERT(uart != RT_NULL);

    /* read interrupt status and clear it */
    if ((uart->addr->IIR & DW_IIR_RECV_DATA) ||
        (uart->addr->IIR & DW_IIR_RECV_LINE)) {
        rt_hw_serial_isr(&uart->parent, RT_SERIAL_EVENT_RX_IND);
    }
}

/*
 * UART interface
 */
static rt_err_t csky_uart_configure(struct rt_serial_device *serial, struct serial_configure *cfg)
{
    rt_err_t ret = RT_EOK;
    struct csky_uart_dev *uart;
    RT_ASSERT(serial != RT_NULL);

    serial->config = *cfg;

    uart = CSKY_UART_DEVICE(serial->parent.user_data);
    RT_ASSERT(uart != RT_NULL);

    uart->addr->IER = 0;
    uart->addr->MCR = 0x03; //CPR 0xf4
    uart->addr->FCR = 0x07;
    uart->addr->LCR = 0x03; // 8bits 1 stop_bit, NONE_PARITY
    uint32_t baud = (UART_CONSOLE_BASE_CLK + UART_CONSOLE_BAUDRATE / 2) / 16 / UART_CONSOLE_BAUDRATE;
    uint32_t lcr = uart->addr->LCR & ~0x80;
    uart->addr->LCR = lcr | 0x80;
    uart->addr->DLL = baud&0xFF;
    uart->addr->DLH = (baud>>8)&0xFF;
    uart->addr->LCR = lcr;
    uart->addr->IER = IER_RDA_INT_ENABLE | IIR_RECV_LINE_ENABLE;

    return ret;
}

static rt_err_t csky_uart_control(struct rt_serial_device *serial, int cmd, void *arg)
{
    rt_err_t ret = RT_EOK;
    struct csky_uart_dev *uart = CSKY_UART_DEVICE(serial->parent.user_data);

    RT_ASSERT(uart != RT_NULL);

    switch (cmd) {
    case RT_DEVICE_CTRL_CLR_INT:
        /* Disable the UART Interrupt */
        mc50_e907_interrupt_mask(uart->irqno);
        break;

    case RT_DEVICE_CTRL_SET_INT:
        /* install interrupt */
        /* Enable the UART Interrupt */
        mc50_e907_interrupt_umask(uart->irqno);
        break;

    case RT_DEVICE_CTRL_CONFIG:
        break;
    }

    return ret;
}

static int csky_uart_putc(struct rt_serial_device *serial, char c)
{
    struct csky_uart_dev *uart = CSKY_UART_DEVICE(serial->parent.user_data);

    RT_ASSERT(uart != RT_NULL);

    /* FIFO status, contain valid data */
    while ((!(uart->addr->LSR & DW_LSR_TRANS_EMPTY))) ;
    uart->addr->THR = c;

    return 1;
}

static int csky_uart_getc(struct rt_serial_device *serial)
{
    int ch = -1;
    struct csky_uart_dev *uart = CSKY_UART_DEVICE(serial->parent.user_data);

    RT_ASSERT(uart != RT_NULL);

    if (uart->addr->LSR & LSR_DATA_READY) {
        ch = uart->addr->RBR & 0xff;
    }

    return ch;
}

const static struct rt_uart_ops _uart_ops = {
    csky_uart_configure,
    csky_uart_control,
    csky_uart_putc,
    csky_uart_getc,
    RT_NULL,
};

/*
 * UART Initiation
 */
static int rt_hw_uart_init(void)
{
    rt_err_t ret = RT_EOK;

    log_2_mem_init();
    uart_clk_init(UART_CONSOLE_CHANNEL);

    struct serial_configure config = RT_SERIAL_CONFIG_DEFAULT;
    config.baud_rate = UART_CONSOLE_BAUDRATE;
    uart_dev.parent.ops = &_uart_ops;
    uart_dev.parent.config = config;
    uart_dev.irqno = uart_irq_no[UART_CONSOLE_CHANNEL] ;
    uart_dev.addr  = uart_base_addr[UART_CONSOLE_CHANNEL] ;

    ret = rt_hw_serial_register(&uart_dev.parent,
                                RT_CONSOLE_DEVICE_NAME,
                                RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX,
                                &uart_dev);

    mc50_e907_interrupt_install(uart_dev.irqno,
                                 csky_uart_isr,
                                 &(uart_dev.parent),
                                 uart_dev.parent.parent.parent.name);

    return ret;
}

INIT_BOARD_EXPORT(rt_hw_uart_init);

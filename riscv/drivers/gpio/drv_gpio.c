/*
 * File      : drv_gpio_axera.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2017, RT-Thread Development Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-02-08     RT-Thread    the first version
 */
#include <rtthread.h>
#include <rthw.h>
#include <board.h>
#include <rtdevice.h>

#include "drv_gpio.h"
#include "drv_gpio_reg.h"
#include "drivers/pin.h"
#include "ax_hwspinlock.h"
#include "dphyrx.h"

#define DBG_TAG  "GPIO"
#define DBG_LVL  DBG_WARNING
#include <rtdbg.h>

#define readl(addr)           (*(volatile unsigned int *)(addr))
#define writel(value,addr)    (*(volatile unsigned int *)(addr) = (value))

static uint32_t ax_readl_with_hwspinlock(uint32_t addr)
{
    uint32_t r;

    rt_base_t level = rt_hw_interrupt_disable();
    ax_hwspinlock_lock(AX_HWSPINLOCK_AREA_ID_GPIO);
    r = readl(addr);
    ax_hwspinlock_unlock(AX_HWSPINLOCK_AREA_ID_GPIO);
    rt_hw_interrupt_enable(level);

    return r;
}

static void ax_writel_with_hwspinlock(uint32_t value, uint32_t addr)
{
    rt_base_t level = rt_hw_interrupt_disable();
    ax_hwspinlock_lock(AX_HWSPINLOCK_AREA_ID_GPIO);
    writel(value, addr);
    ax_hwspinlock_unlock(AX_HWSPINLOCK_AREA_ID_GPIO);
    rt_hw_interrupt_enable(level);
}

static struct gpio_offset_t gpio_get_addr(enum gpio_group group)
{
    struct gpio_offset_t gpio_offset;

    switch (group) {
    case 0:
        gpio_offset.offset_gid = 0x00;
        gpio_offset.addr = GPIO_GP0_BASE_ADDR;
        break;
    case 1:
        gpio_offset.offset_gid = 0x01;
        gpio_offset.addr = GPIO_GP1_BASE_ADDR;
        break;
    case 2:
        gpio_offset.offset_gid = 0x02;
        gpio_offset.addr = GPIO_GP2_BASE_ADDR;
        break;
    case 3:
        gpio_offset.offset_gid = 0x03;
        gpio_offset.addr = GPIO_GP3_BASE_ADDR;
        break;
    case 4:
        gpio_offset.offset_gid = 0x04;
        gpio_offset.addr = GPIO_GP4_BASE_ADDR;
        break;
    case 5:
        gpio_offset.offset_gid = 0x05;
        gpio_offset.addr = GPIO_GP5_BASE_ADDR;
        break;

    default:
        LOG_W("[line]:%d port id input error\n", __LINE__);
    }

    gpio_offset.offset_dr = GPIO_SWPORTABCD_DR;
    gpio_offset.offset_ddr = GPIO_SWPORTABCD_DDR;
    gpio_offset.offset_ctl = GPIO_SWPORTABCD_CTL;
    gpio_offset.offset_ext = GPIO_EXT_PORTABCD;

    return gpio_offset;
}

/*********************************************************
**   IO
*********************************************************/
/*
 * gpio_group = 0 1 2 3 4
 * pin = 0 1 2 ... 31
 * func = 0 1 2 3 4, as follows:
 *  PIN_MODE_OUTPUT         0x00
 *  PIN_MODE_INPUT          0x01
 *  PIN_MODE_INPUT_PULLUP   0x02
 *  PIN_MODE_INPUT_PULLDOWN 0x03
 *  PIN_MODE_OUTPUT_OD      0x04
 */
static int __gpio_set_func(enum gpio_group group, enum gpio_pin pin, rt_uint8_t func)
{
    rt_uint32_t addr;
    rt_uint32_t data;
    struct gpio_offset_t offset;

    if (func & 0x8) {
        rt_kprintf("[line]:%d There is a warning with parameter input\n", __LINE__);
        return RT_EINVAL;
    }

    offset = gpio_get_addr(group);
    addr = offset.addr + offset.offset_ddr;
    data = ax_readl_with_hwspinlock(addr);

    switch (func) {
    case PIN_MODE_OUTPUT:
    case PIN_MODE_OUTPUT_OD:
        data |= (0x1 << pin);
        break;

    case PIN_MODE_INPUT:
    case PIN_MODE_INPUT_PULLUP:
    case PIN_MODE_INPUT_PULLDOWN:
        data &= ~(0x1 << pin);
        break;
    }
    ax_writel_with_hwspinlock(data, addr);

    return RT_EOK;
}

int gpio_set_func(rt_uint32_t gpio_num, gpio_func_e func)
{
    int ret;

    if (gpio_num > 192) {
        rt_kprintf("[line]: gpio %u out of range\n", __LINE__, gpio_num);
        return -1;
    }

    dphyrx_gpio_set_config(gpio_num, func);

    ret = __gpio_set_func(gpio_num / 32, gpio_num % 32, func);

    return ret;
}

static int __gpio_set_int_mode(enum gpio_group group, enum gpio_pin pin, rt_uint8_t int_mode)
{
    rt_uint32_t data;
    struct gpio_offset_t offset;

    offset = gpio_get_addr(group);
    rt_uint32_t int_en_addr = offset.addr + GPIO_INTEN;
    rt_uint32_t int_mask_addr = offset.addr + GPIO_INTMASK;
    rt_uint32_t int_type_addr = offset.addr + GPIO_INTTYPE_LEVEL;
    rt_uint32_t int_polarity_addr = offset.addr + GPIO_INT_POLARITY;
    rt_uint32_t debounce_addr = offset.addr + GPIO_DEBOUNCE;

    data = ax_readl_with_hwspinlock(int_type_addr);
    switch (int_mode) {
    case PIN_IRQ_MODE_RISING:
    case PIN_IRQ_MODE_FALLING:
    case PIN_IRQ_MODE_RISING_FALLING:
        data |= (0x1 << pin);
        break;
    case PIN_IRQ_MODE_HIGH_LEVEL:
    case PIN_IRQ_MODE_LOW_LEVEL:
        data &= ~(0x1 << pin);
        break;
    }
    ax_writel_with_hwspinlock(data, int_type_addr);

    data = ax_readl_with_hwspinlock(int_polarity_addr);
    switch (int_mode) {
    case PIN_IRQ_MODE_RISING:
    case PIN_IRQ_MODE_FALLING:
        data |= (0x1 << pin);
        break;
    case PIN_IRQ_MODE_LOW_LEVEL:
    case PIN_IRQ_MODE_HIGH_LEVEL:
        data &= ~(0x1 << pin);
        break;
    default:
        break;
    }
    ax_writel_with_hwspinlock(data, int_polarity_addr);

    data = ax_readl_with_hwspinlock(debounce_addr);
    data |= (0x1 << pin);
    ax_writel_with_hwspinlock(data, debounce_addr);

    data = ax_readl_with_hwspinlock(int_mask_addr);
    data |= (0x1 << pin);
    ax_writel_with_hwspinlock(data, int_mask_addr);

    data = ax_readl_with_hwspinlock(int_en_addr);
    data |= (0x1 << pin);
    ax_writel_with_hwspinlock(data, int_en_addr);

    return RT_EOK;
}

int gpio_set_int_mode(rt_uint32_t gpio_num, gpio_int_mode_e int_mode)
{
    if (gpio_num > 192) {
        rt_kprintf("[line]: gpio %u out of range\n", __LINE__, gpio_num);
        return -1;
    }

    int ret = __gpio_set_int_mode(gpio_num / 32, gpio_num % 32, int_mode);

    return ret;

}

static rt_uint32_t  __gpio_get_int_raw_status(enum gpio_group group, enum gpio_pin pin)
{
    rt_uint32_t addr;
    rt_uint32_t data;
    struct gpio_offset_t offset;

    offset = gpio_get_addr(group);
    addr = offset.addr + GPIO_RAW_INTSTATUS;

    data = ax_readl_with_hwspinlock(addr);

    return (data & (1 << pin));
}

int gpio_get_int_raw_status(rt_uint32_t gpio_num)
{
    if (gpio_num > 192) {
        rt_kprintf("[line]: gpio %u out of range\n", __LINE__, gpio_num);
        return -1;
    }

    int ret;
    ret = __gpio_get_int_raw_status(gpio_num / 32, gpio_num % 32);

    return ret;

}

static int __gpio_clear_int_status(enum gpio_group group, enum gpio_pin pin)
{
    rt_uint32_t addr;
    struct gpio_offset_t offset;

    offset = gpio_get_addr(group);
    addr = offset.addr + GPIO_INT_PORTA_EOI;

    ax_writel_with_hwspinlock(1 << pin, addr);

    rt_kprintf("");

    return 0;
}

int gpio_clear_int_status(rt_uint32_t gpio_num)
{
    if (gpio_num > 192) {
        rt_kprintf("[line]: gpio %u out of range\n", __LINE__, gpio_num);
        return -1;
    }

    int ret = __gpio_clear_int_status(gpio_num / 32, gpio_num % 32);

    return ret;
}

static int __gpio_set_value(enum gpio_group group, enum gpio_pin pin, rt_uint8_t value)
{
    rt_uint32_t addr;
    rt_uint32_t data;
    struct gpio_offset_t offset;

    if (value & 0xE) {
        rt_kprintf("[line]:%d There is a warning with parameter input\n", __LINE__);
        return RT_EINVAL;
    }

    offset = gpio_get_addr(group);
    addr = offset.addr + offset.offset_dr;

    data = ax_readl_with_hwspinlock(addr);
    data &= ~(0x01 << pin);
    data |= ((0x01 & value) << pin);

    ax_writel_with_hwspinlock(data, addr);

//    rt_kprintf("[line]:%d set value: group=%d, pin=%d, addr=0x%x, val=%d, data:0x%x\n", __LINE__, group, pin, addr, value, data);
    return RT_EOK;
}

int gpio_set_value(rt_uint32_t gpio_num, gpio_value_e value)
{
    int ret;
    ret = __gpio_set_value(gpio_num / 32, gpio_num % 32, value);

    return ret;
}

static int __gpio_get_value(enum gpio_group group, enum gpio_pin pin)
{
    rt_uint32_t addr;
    rt_uint32_t data;
    struct gpio_offset_t offset;

    offset = gpio_get_addr(group);
    addr = offset.addr + offset.offset_ddr;
    if (ax_readl_with_hwspinlock(addr) & (0x1 << pin)) {
        addr = offset.addr + offset.offset_dr;
    } else {
        addr = offset.addr + offset.offset_ext;
    }

    data = ax_readl_with_hwspinlock(addr) & (0x1 << pin);

//    rt_kprintf("[line]:%d get value: group=%d, pin=%d, addr=0x%08x, data=0x%08x, ret=%d\n", __LINE__, group, pin, addr, data, ((data >> pin) & 0x01));
    return ((data >> pin) & 0x01);
}

gpio_value_e gpio_get_value(rt_uint32_t gpio_num)
{
    int ret;
    ret = __gpio_get_value(gpio_num / 32, gpio_num % 32);

    return ret;
}

#ifdef RT_USING_PIN

#define PIN_MAGIC    (0x5A)
#define PIN_NUM(_N)      (sizeof(_N) / sizeof(_N[0]))

static void pin_mode(struct rt_device *dev, rt_base_t pin, rt_base_t mode)
{
    gpio_set_func(pin, mode);
}

static void pin_write(struct rt_device *dev, rt_base_t pin, rt_base_t value)
{
    gpio_set_value(pin, value);
}

static int pin_read(struct rt_device *device, rt_base_t pin)
{
    return gpio_get_value(pin);
}

const struct rt_pin_ops ops = {
    pin_mode,
    pin_write,
    pin_read,
    RT_NULL,
    RT_NULL,
    RT_NULL,
    RT_NULL,
};
#endif

int rt_hw_gpio_init(void)
{
#ifdef RT_USING_PIN
    rt_device_pin_register("gpio", &ops, RT_NULL);
#endif
    return 0;
}
INIT_DEVICE_EXPORT(rt_hw_gpio_init);

/*
 * File      : drv_gpio_axera.h
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

#ifndef __DRV_GPIO_H__
#define __DRV_GPIO_H__

#include "rtdef.h"

typedef enum {
    gpio_func_output = 0x00,
    gpio_func_input,
    gpio_func_input_pullup,
    gpio_func_input_pulldown,
    gpio_func_output_od,
} gpio_func_e;

typedef enum {
    gpio_low = 0,
    gpio_high
} gpio_value_e;

typedef enum {
    gpio_int_mode_rising = 0,
    gpio_int_mode_falling,
    gpio_int_mode_rising_falling,
    gpio_int_mode_high_level,
    gpio_int_mode_low_level,
} gpio_int_mode_e;

int gpio_set_func(rt_uint32_t gpio_num, gpio_func_e func);
int gpio_set_value(rt_uint32_t gpio_num, gpio_value_e value);
gpio_value_e gpio_get_value(rt_uint32_t gpio_num);
int gpio_set_int_mode(rt_uint32_t gpio_num, gpio_int_mode_e int_mode);
int gpio_get_int_raw_status(rt_uint32_t gpio_num);
int gpio_clear_int_status(rt_uint32_t gpio_num);

#endif /* __DRV_GPIO_H__ */

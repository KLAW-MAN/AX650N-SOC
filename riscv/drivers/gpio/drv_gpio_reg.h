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
#ifndef __DRV_GPIO_REG_H__
#define __DRV_GPIO_REG_H__

#include "rtdef.h"

/* IO port */
enum gpio_group {
    GPIO_GROUP_0 = 0,
    GPIO_GROUP_1,
    GPIO_GROUP_2,
    GPIO_GROUP_3,
    GPIO_GROUP_4,
    GPIO_GROUP_5,
    GPIO_GROUP_NUM
};

/* IO pin */
enum gpio_pin {
    GPIO_PIN_0 = 0,
    GPIO_PIN_1,
    GPIO_PIN_2,
    GPIO_PIN_3,
    GPIO_PIN_4,
    GPIO_PIN_5,
    GPIO_PIN_6,
    GPIO_PIN_7,
    GPIO_PIN_8,
    GPIO_PIN_9,
    GPIO_PIN_10,
    GPIO_PIN_11,
    GPIO_PIN_12,
    GPIO_PIN_13,
    GPIO_PIN_14,
    GPIO_PIN_15,
    GPIO_PIN_16,
    GPIO_PIN_17,
    GPIO_PIN_18,
    GPIO_PIN_19,
    GPIO_PIN_20,
    GPIO_PIN_21,
    GPIO_PIN_22,
    GPIO_PIN_23,
    GPIO_PIN_24,
    GPIO_PIN_25,
    GPIO_PIN_26,
    GPIO_PIN_27,
    GPIO_PIN_28,
    GPIO_PIN_29,
    GPIO_PIN_30,
    GPIO_PIN_31,
    GPIO_PIN_NUM,
};

struct gpio_offset_t {
    rt_uint32_t offset_dr;
    rt_uint32_t offset_ddr;
    rt_uint32_t offset_ctl;
    rt_uint32_t offset_gid; /* GPIO group id */
    rt_uint32_t offset_ext;
    rt_uint32_t offset_inttype_level;
    rt_uint32_t offset_int_polarity;
    rt_uint32_t offset_raw_intstatus;
    rt_uint32_t offset_debounce;
    rt_uint32_t offset_eoi;
    rt_uint32_t addr;
};

#define GPIO_SWPORTABCD_DR     0x00 /* for data output */
#define GPIO_SWPORTABCD_DDR    0x04
#define GPIO_SWPORTABCD_CTL    0x08
#define GPIO_INTEN             0x30
#define GPIO_INTMASK           0x34
#define GPIO_INTTYPE_LEVEL     0x38
#define GPIO_INT_POLARITY      0x3c
#define GPIO_INTSTATUS         0x40
#define GPIO_RAW_INTSTATUS     0x44
#define GPIO_DEBOUNCE          0x48
#define GPIO_INT_PORTA_EOI     0x4c
/*
 * this register always reflects the signals value on the external port.
 *
 */
#define GPIO_EXT_PORTABCD      0x50

#define GPIO_LS_SYNC        0x60
#define GPIO_ID_CODE        0x64
#define GPIO_INT_BOTHEDGE   0x68
#define GPIO_VER_ID_CODE    0x6c
#define GPIO_CONFIG_REG2    0x70
#define GPIO_CONFIG_REG1    0x74

struct axera_gpio {
    volatile rt_uint32_t pa_dr;             /* 0x00 */
    volatile rt_uint32_t pa_ddr;            /* 0x04 */
    volatile rt_uint32_t pa_ctl;            /* 0x08 */

    volatile rt_uint32_t inten;             /* 0x30 */
    volatile rt_uint32_t intmask;           /* 0x34 */
    volatile rt_uint32_t inttype_level;     /* 0x38 */
    volatile rt_uint32_t int_polarity;      /* 0x3C */
    volatile rt_uint32_t intstatus;         /* 0x40 */
    volatile rt_uint32_t raw_intstatus;     /* 0x44 */

    volatile rt_uint32_t debounce;          /* 0x48 */
    volatile rt_uint32_t pa_eoi;            /* 0x4C */
    volatile rt_uint32_t ext_pa;            /* 0x50 */

    volatile rt_uint32_t ls_sync;           /* 0x60 */
    volatile rt_uint32_t id_code;           /* 0x64 */
    volatile rt_uint32_t int_bothedge;      /* 0x68 */
    volatile rt_uint32_t ver_id_code;       /* 0x6C */
    volatile rt_uint32_t cfg_reg2;          /* 0x70 */
    volatile rt_uint32_t cfg_reg1;          /* 0x74 */
};

#endif /* __DRV_GPIO_REG_H__ */

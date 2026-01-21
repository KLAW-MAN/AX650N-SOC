/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include <rtthread.h>
#include <rthw.h>
#include <rtdevice.h>
#include <stdlib.h>
#include <stdbool.h>
#include "drv_i2c.h"

typedef struct {
    int channel;
    char name[32];
    int freq;
    uint32_t slave_addr;
    uint32_t slave_reg;
    bool is_16_bits;
} i2c_test_info_t;

static void print_usage()
{
    rt_kprintf("Usage: i2c_tool channel name freq slave_addr slave_reg is_16_bits\n"
               "       channel:    i2c channel\n"
               "       name:       i2c device name\n"
               "       freq:       i2c frequency\n"
               "       slave_addr: i2c slave device addr\n"
               "       slave_reg:  i2c slave reg addr\n"
               "       is_16_bits: i2c slave addr is 16 bits\n");
}

static void i2c_tool(rt_int32_t argc, char* argv[])
{
    int r;
    i2c_test_info_t test_info;
    struct rt_i2c_bus_device *i2c_dev;
    uint8_t data[8] = {0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19};

    if (argc != 7) {
        print_usage();
        return;
    }

    test_info.channel = atoi(argv[1]);
    rt_strcpy(test_info.name, argv[2]);
    test_info.freq = atoi(argv[3]);
    test_info.slave_addr = atoi(argv[4]);
    test_info.slave_reg = atoi(argv[5]);
    test_info.is_16_bits = atoi(argv[6]);

    r = i2c_init(test_info.channel, test_info.name, test_info.freq);
    if (r != 0) {
        rt_kprintf("i2c channel %d, frequency %d init fail\n", test_info.channel, test_info.freq);
        return;
    }

    i2c_dev = (struct rt_i2c_bus_device*)(rt_device_find(test_info.name));
    if (i2c_dev == RT_NULL) {
        rt_kprintf("cannot find i2c device %s\n", test_info.name);
        return;
    }

    i2c_wrtie_reg(i2c_dev,
                  test_info.slave_addr,
                  test_info.slave_reg,
                  data,
                  sizeof(data) / sizeof(data[0]),
                  test_info.is_16_bits);
}

MSH_CMD_EXPORT(i2c_tool, channel name freq slave_addr slave_reg is_16_bits [-h] to get help);

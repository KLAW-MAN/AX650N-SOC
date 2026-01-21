#include <rtthread.h>
#include <rthw.h>
#include <rtdevice.h>
#include <stdlib.h>

static void print_usage()
{
    rt_kprintf("Usage:gpio_tools r/w/i/o  gpio_num value\n"
                "      r/w:        read reg or write gpio\n"
                "      i/o:        set gpio direction\n"
                "      gpio_num:   gpio number\n"
                "      value:      output value\n");
}

static void gpio_tools(rt_int32_t argc, char* argv[])
{
    rt_uint32_t  gpio_num = 0;
    rt_uint32_t value = 0;
    rt_uint32_t is_write = 0;
    rt_int32_t status = 0;

    if(argc == 2) {
        if(rt_strncmp(argv[1], "-h", 10) == 0) {
            print_usage();
        } else {
            rt_kprintf("parameter is invalid, please input [gpio_tools -h] to get help\n");
        }
        return;
    }
    if(argc < 3) {
        rt_kprintf("paramters is invalid!\n");
        print_usage();
        return;
    }

    if(rt_strncmp(argv[1], "w", 5) == 0) {
        is_write = 1;
    }

    if(rt_strncmp(argv[1], "i", 5) == 0) {
        is_write = 2;
    }

    if(rt_strncmp(argv[1], "o", 5) == 0) {
        is_write = 3;
    }

    gpio_num = atoi(argv[2]);
    if(is_write == 1 || is_write == 2) {
        value = atoi(argv[3]);
        value = atoi(argv[3]);
        if(value > 0) {
            value = 1;
        }
    }

    if(is_write == 1) {
        rt_pin_write(gpio_num, value);
        status = rt_pin_read(gpio_num);
        if(status != value) {
            rt_kprintf("gpio value is not set, status=%d!\n", status);
        }
    } else if (is_write == 2) {
        rt_pin_mode(gpio_num, PIN_MODE_INPUT);
    } else if (is_write == 3) {
        rt_pin_mode(gpio_num, PIN_MODE_OUTPUT);
    } else {
        status = rt_pin_read(gpio_num);
        rt_kprintf("read gpio%d, status=%d\n", gpio_num, status);
    }
}

MSH_CMD_EXPORT(gpio_tools, r/w/i/o gpio_num [value] [-h] to get help);

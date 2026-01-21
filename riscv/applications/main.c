/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-08-20     zx.chen      first version
 */

#include <rthw.h>
#include <rtthread.h>
#include "main.h"
#include "ax_log.h"
#include "ax_common.h"
#include "soc.h"

int main(void)
{
    ax_writel(RISCV_DONE_MAGIC, COMM_SYS_DUMMY_SW11);
    AX_LOG_INFO("hello axera riscv");

    while (1) {
       rt_thread_mdelay(5000);
    }

    return 0;
}

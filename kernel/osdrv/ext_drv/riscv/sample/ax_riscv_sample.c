/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include "ax_riscv_drv.h"
#include "ax_riscv_log.h"
#include "osal_lib_ax.h"

#define AX_MBOX_CMD_TEST                 0x11
#define AX_MBOX_CMD_FB_ARM_TEST_OK       0x03
#define AX_MBOX_CMD_FB_ARM_TEST_FAIL     0x04
#define RISCV_MAILBOX_TEST_MAGIC         "test_for_mailbox"

#define MODULE_PARAM_STRLEN     32
static char riscv_sample_mode[MODULE_PARAM_STRLEN] = {'\0'};
module_param_string(mode, riscv_sample_mode, MODULE_PARAM_STRLEN, 0600);
static int times = 1;
module_param(times, int, 0600);

typedef int(*ax_riscv_sample_func_t)(void);

typedef struct {
    char *name;
    char *desc;
    ax_riscv_sample_func_t func;
} ax_riscv_sample_info_t;

static int ax_riscv_sample_setup(void)
{
    int ret;

    ret = ax_riscv_setup();
    if (ret != 0) {
        RISCV_LOG_ERROR("riscv setup fail");
    }

    return 0;
}

static int ax_riscv_sample_shutup(void)
{
    int ret;

    ret = ax_riscv_shutup();
    if (ret != 0) {
        RISCV_LOG_ERROR("riscv shutup fail");
    }

    return 0;
}

static int ax_riscv_sample_message(void)
{
    int ret;
    mbox_msg_t msg;
    int i;

    RISCV_LOG_INFO("test %u times start", times);

    ret = ax_riscv_setup();
    if (ret != 0) {
        RISCV_LOG_ERROR("riscv setup fail");
        return -1;
    }

    for (i = 0; i < times; i++) {
        msg.id = AX_MBOX_CMD_TEST;
        AX_OSAL_LIB_strcpy(msg.data, RISCV_MAILBOX_TEST_MAGIC);
        ret = ax_riscv_send_message(&msg);
        if (ret != 0) {
            RISCV_LOG_ERROR("send message fail");
            ax_riscv_shutup();
            return -1;
        }

        ret = ax_riscv_recv_message(&msg);
        if (ret != 0) {
            RISCV_LOG_ERROR("recv message fail");
            ax_riscv_shutup();
            return -1;
        }
        if (msg.id != AX_MBOX_CMD_FB_ARM_TEST_OK) {
            RISCV_LOG_ERROR("feed back fail");
            ax_riscv_shutup();
            return -1;
        }
    }

    ret = ax_riscv_shutup();
    if (ret != 0) {
        RISCV_LOG_ERROR("riscv shutup fail");
        return -1;
    }

    RISCV_LOG_INFO("test %u times end", times);

    return 0;
}

static ax_riscv_sample_info_t sample_info[] = {
    {.name = "setup", .desc = "setup riscv", .func = ax_riscv_sample_setup},
    {.name = "shutup", .desc = "shutup riscv", .func = ax_riscv_sample_shutup},
    {.name = "message", .desc = "send and recive message", .func = ax_riscv_sample_message},
};

static int ax_riscv_sample_handle(char *test_mode)
{
    int ret, i;

    for (i = 0; i < sizeof(sample_info) / sizeof(sample_info[0]); i++) {
        if (AX_OSAL_LIB_strcmp(test_mode, sample_info[i].name) == 0) {
            RISCV_LOG_INFO("%s start", sample_info[i].desc);
            ret = sample_info[i].func();
            if (ret != 0) {
                RISCV_LOG_ERROR("%s command send fail, 0x%x", sample_info[i].desc, ret);
                return ret;
            }
            RISCV_LOG_INFO("%s end", sample_info[i].desc);
            break;
        }
    }

    if (i == sizeof(sample_info) / sizeof(sample_info[0])) {
        RISCV_LOG_ERROR("sample mode [%s] not found", test_mode);
        return -1;
    }

    return 0;
}

static int __init ax_riscv_sample_driver_init(void)
{
    int ret, i;

    if (AX_OSAL_LIB_strlen(riscv_sample_mode) != 0) {
        ret = ax_riscv_sample_handle(riscv_sample_mode);
        if (ret != 0) {
            RISCV_LOG_ERROR("run sample %s fail, 0x%x", riscv_sample_mode, ret);
            return ret;
        }
    } else {
        RISCV_LOG_ERROR("please input sample mode");
        for (i = 0; i < sizeof(sample_info) / sizeof(sample_info[0]); i++) {
            RISCV_LOG_INFO("%s: %s", sample_info[i].name, sample_info[i].desc);
        }
    }

    return 0;
}

static void __exit ax_riscv_sample_driver_exit(void)
{
    RISCV_LOG_INFO("riscv module exit");
}

AX_OSAL_module_initcall(ax_riscv_sample_driver_init);
AX_OSAL_module_exit(ax_riscv_sample_driver_exit);
MODULE_DESCRIPTION("ax riscv driver");
MODULE_LICENSE("GPL");

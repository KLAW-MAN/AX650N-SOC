/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include "osal_ax.h"
#include "osal_dev_ax.h"
#include "osal_lib_ax.h"
#include "ax_riscv_drv.h"
#include "ax_riscv_log.h"
#include "ax_sync_signal_priv.h"
#include "ax_sync_signal.h"

static int ax_riscv_handle_msg(ax_ftc_cmd_msg_t *msg)
{
    int ret;

    ret = ax_riscv_send_message((mbox_msg_t *)msg);
    if (ret != 0) {
        RISCV_LOG_ERROR("send message fail");
        return ret;
    }

    ret = ax_riscv_recv_message((mbox_msg_t *)msg);
    if (ret != 0) {
        RISCV_LOG_ERROR("receive message timeout");
        return ret;
    }

    if (msg->feedback != 0) {
        RISCV_LOG_ERROR("riscv feedback error, 0x%x", msg->feedback);
        return AX_RISCV_RISCV_FB_ERROR;
    }

    return 0;
}

/****************kernel api start*******************/
int AX_RISCV_Kernel_setup_riscv(void)
{
    int ret;

    ret = ax_riscv_setup();
    if (ret != 0) {
        RISCV_LOG_ERROR("setup riscv fail");
	return ret;
    }
    return 0;
}
EXPORT_SYMBOL(AX_RISCV_Kernel_setup_riscv);

int AX_RISCV_Kernel_shutup_riscv(void)
{
    int ret;

    ret = ax_riscv_shutup();
    if (ret != 0) {
        RISCV_LOG_ERROR("shutup riscv fail");
	return ret;
    }
    return 0;
}
EXPORT_SYMBOL(AX_RISCV_Kernel_shutup_riscv);

int AX_RISCV_Kernel_get_cycle_info(uint32_t *cycle_us)
{
    uint32_t *dummy_sw11;

    if (cycle_us == NULL) {
        RISCV_LOG_ERROR("param error");
        return AX_RISCV_PARA_FAIL;
    }

    dummy_sw11 = AX_OSAL_DEV_ioremap(COMM_SYS_DUMMY_SW11, 4);
    if (dummy_sw11 == NULL) {
        RISCV_LOG_ERROR("ioremap fail");
        return AX_RISCV_MAP_FAIL;
    }

    *cycle_us = *dummy_sw11;
    AX_OSAL_DEV_iounmap(dummy_sw11);

    return 0;
}
EXPORT_SYMBOL(AX_RISCV_Kernel_get_cycle_info);

int AX_RISCV_Kernel_send_version_info(void)
{
    int ret;
    ax_ftc_version_info_msg_t msg = {
        .cmd = AX_FTC_CMD_VERSION_INFO,
    };

    snprintf(&msg.version[0], sizeof(msg.version), FTC_VERSION);

    ret = ax_riscv_handle_msg((ax_ftc_cmd_msg_t *)&msg);
    if (ret != 0) {
        RISCV_LOG_ERROR("handle fail, 0x%x", ret);
        return ret;
    }

    return 0;
}
EXPORT_SYMBOL(AX_RISCV_Kernel_send_version_info);

int AX_RISCV_Kernel_init(void)
{
    int ret;
    ax_ftc_cmd_msg_t msg = {
        .cmd = AX_FTC_CMD_INIT,
    };

    ret = ax_riscv_handle_msg(&msg);
    if (ret != 0) {
        RISCV_LOG_ERROR("handle fail, 0x%x", ret);
        return ret;
    }

    return 0;
}
EXPORT_SYMBOL(AX_RISCV_Kernel_init);

int AX_RISCV_Kernel_deinit(void)
{
    int ret;
    ax_ftc_cmd_msg_t msg = {
        .cmd = AX_FTC_CMD_DEINIT,
    };

    ret = ax_riscv_handle_msg(&msg);
    if (ret != 0) {
        RISCV_LOG_ERROR("handle fail, 0x%x", ret);
        return ret;
    }

    return 0;
}
EXPORT_SYMBOL(AX_RISCV_Kernel_deinit);

int AX_RISCV_Kernel_start(void)
{
    int ret;
    ax_ftc_cmd_msg_t msg = {
        .cmd = AX_FTC_CMD_START,
    };

    ret = ax_riscv_handle_msg(&msg);
    if (ret != 0) {
        RISCV_LOG_ERROR("handle fail, 0x%x", ret);
        return ret;
    }

    return 0;
}
EXPORT_SYMBOL(AX_RISCV_Kernel_start);

int AX_RISCV_Kernel_stop(void)
{
    int ret;
    ax_ftc_cmd_msg_t msg = {
        .cmd = AX_FTC_CMD_STOP,
    };

    ret = ax_riscv_handle_msg(&msg);
    if (ret != 0) {
        RISCV_LOG_ERROR("handle fail, 0x%x", ret);
        return ret;
    }

    return 0;
}
EXPORT_SYMBOL(AX_RISCV_Kernel_stop);

int AX_RISCV_Kernel_set_power_attr(ax_ftc_power_sync_attr_msg_t *power_attr)
{
    int ret;
    ax_ftc_power_sync_attr_msg_t msg = {0};

    AX_OSAL_LIB_memcpy(&msg, power_attr, sizeof(ax_ftc_power_sync_attr_msg_t));

    msg.cmd = AX_FTC_CMD_SET_POWER_ATTR;
    ret = ax_riscv_handle_msg((ax_ftc_cmd_msg_t *)&msg);
    if (ret != 0) {
        RISCV_LOG_ERROR("handle fail, 0x%x", ret);
        return ret;
    }

    return 0;
}
EXPORT_SYMBOL(AX_RISCV_Kernel_set_power_attr);

int AX_RISCV_Kernel_get_power_attr(ax_ftc_power_sync_attr_msg_t *power_attr)
{
    int ret;
    ax_ftc_power_sync_attr_msg_t msg = {0};

    msg.cmd = AX_FTC_CMD_GET_POWER_ATTR,
    ret = ax_riscv_handle_msg((ax_ftc_cmd_msg_t *)&msg);
    if (ret != 0) {
        RISCV_LOG_ERROR("handle fail, 0x%x", ret);
        return ret;
    }

    AX_OSAL_LIB_memcpy(power_attr, &msg, sizeof(ax_ftc_power_sync_attr_msg_t));

    return 0;
}
EXPORT_SYMBOL(AX_RISCV_Kernel_get_power_attr);

int AX_RISCV_Kernel_set_pin_info(ax_ftc_pin_ability_msg_t *pin_cfg)
{
    int ret;
    ax_ftc_pin_ability_msg_t msg = {0};

    AX_OSAL_LIB_memcpy(&msg, pin_cfg, sizeof(ax_ftc_pin_ability_msg_t));

    msg.cmd = AX_FTC_CMD_SET_PIN_INFO;
    ret = ax_riscv_handle_msg((ax_ftc_cmd_msg_t *)&msg);
    if (ret != 0) {
        RISCV_LOG_ERROR("handle fail, 0x%x", ret);
        return ret;
    }

    return 0;
}
EXPORT_SYMBOL(AX_RISCV_Kernel_set_pin_info);

int AX_RISCV_Kernel_get_pin_info(ax_ftc_pin_ability_msg_t *pin_cfg)
{
    int ret;
    ax_ftc_pin_ability_msg_t msg = {0};

    msg.cmd = AX_FTC_CMD_GET_PIN_INFO;
    ret = ax_riscv_handle_msg((ax_ftc_cmd_msg_t *)&msg);
    if (ret != 0) {
        RISCV_LOG_ERROR("handle fail, 0x%x", ret);
        return ret;
    }

    AX_OSAL_LIB_memcpy(pin_cfg, &msg, sizeof(ax_ftc_pin_ability_msg_t));

    return 0;
}
EXPORT_SYMBOL(AX_RISCV_Kernel_get_pin_info);

int AX_RISCV_Kernel_set_blink_timing_attr(ax_ftc_blink_timing_attr_msg_t *blink_attr)
{
    int ret;
    ax_ftc_blink_timing_attr_msg_t msg = {0};

    AX_OSAL_LIB_memcpy(&msg, blink_attr, sizeof(ax_ftc_blink_timing_attr_msg_t));

    msg.cmd = AX_FTC_CMD_SET_BLINK_TIMING_ATTR;
    ret = ax_riscv_handle_msg((ax_ftc_cmd_msg_t *)&msg);
    if (ret != 0) {
        RISCV_LOG_ERROR("handle fail, 0x%x", ret);
        return ret;
    }

    return 0;
}
EXPORT_SYMBOL(AX_RISCV_Kernel_set_blink_timing_attr);

int AX_RISCV_Kernel_get_blink_timing_attr(ax_ftc_blink_timing_attr_msg_t *blink_attr)
{
    int ret;
    ax_ftc_blink_timing_attr_msg_t msg = {0};

    msg.cmd = AX_FTC_CMD_GET_BLINK_TIMING_ATTR;
    ret = ax_riscv_handle_msg((ax_ftc_cmd_msg_t *)&msg);
    if (ret != 0) {
        RISCV_LOG_ERROR("handle fail, 0x%x", ret);
        return ret;
    }

    AX_OSAL_LIB_memcpy(blink_attr, &msg, sizeof(ax_ftc_blink_timing_attr_msg_t));

    return 0;
}
EXPORT_SYMBOL(AX_RISCV_Kernel_get_blink_timing_attr);

int AX_RISCV_Kernel_set_flash_timing_attr(ax_ftc_blink_timing_attr_msg_t *blink_attr)
{
    int ret;
    ax_ftc_blink_timing_attr_msg_t msg = {0};

    AX_OSAL_LIB_memcpy(&msg, blink_attr, sizeof(ax_ftc_blink_timing_attr_msg_t));

    msg.cmd = AX_FTC_CMD_SET_FLASH_TIMING_ATTR;
    ret = ax_riscv_handle_msg((ax_ftc_cmd_msg_t *)&msg);
    if (ret != 0) {
        RISCV_LOG_ERROR("handle fail, 0x%x", ret);
        return ret;
    }

    return 0;
}
EXPORT_SYMBOL(AX_RISCV_Kernel_set_flash_timing_attr);

int AX_RISCV_Kernel_set_pack_timing_attr(ax_ftc_pack_timing_attr_msg_t *pack_attr)
{
    int ret;
    ax_ftc_pack_timing_attr_msg_t msg = {0};

    AX_OSAL_LIB_memcpy(&msg, pack_attr, sizeof(ax_ftc_pack_timing_attr_msg_t));

    msg.cmd = AX_FTC_CMD_SET_PACK_TIMING_ATTR;
    ret = ax_riscv_handle_msg((ax_ftc_cmd_msg_t *)&msg);
    if (ret != 0) {
        RISCV_LOG_ERROR("handle fail, 0x%x", ret);
        return ret;
    }

    return 0;
}
EXPORT_SYMBOL(AX_RISCV_Kernel_set_pack_timing_attr);

int AX_RISCV_Kernel_get_pack_timing_attr(ax_ftc_pack_timing_attr_msg_t *pack_attr)
{
    int ret;
    ax_ftc_pack_timing_attr_msg_t msg = {0};

    msg.cmd = AX_FTC_CMD_GET_PACK_TIMING_ATTR;
    ret = ax_riscv_handle_msg((ax_ftc_cmd_msg_t *)&msg);
    if (ret != 0) {
        RISCV_LOG_ERROR("handle fail, 0x%x", ret);
        return ret;
    }

    AX_OSAL_LIB_memcpy(pack_attr, &msg, sizeof(ax_ftc_pack_timing_attr_msg_t));

    return 0;
}
EXPORT_SYMBOL(AX_RISCV_Kernel_get_pack_timing_attr);

/****************kernel api end*******************/


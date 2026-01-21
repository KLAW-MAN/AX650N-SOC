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
#include "ax_global_type.h"
#include "ax_riscv_drv.h"
#include "ax_riscv_log.h"
#include "ax_module_version.h"
#include "ax_riscv_debug.h"
#include "ax_riscv_utils.h"

#define DRIVER_NAME    "ax_riscv"
#define RISCV_FW_PATH  "/opt/bin/riscv/rtthread.bin"

#define AON_SYS_BASE                0x4200000
#define AON_SYS_RISCV_RESET_STATUS  0x378
#define COM_CLK_BASE                0x4210000
#define RISCV_REMAP_BASE            0x4211000
#define RISCV_DONE_MAGIC            0x646f6e65

#define SYSIO_PAD_LPMD_B_DEEP_SLEEP     0x00
#define SYSIO_PAD_LPMD_B_SHALLOW_SLEEP  0x01
#define SYSIO_PAD_LPMD_B_RUNNING        0x03

#define AX_BIT(n)      (1 << n)

#define AX_MBOX_CMD_RESUME      0x0f
#define AX_MBOX_CMD_SUSPEND     0x10
#define AX_MAILBOX_MASTERID_RISCV  4
AX_S32 ax_mailbox_write(AX_U32 masterid, AX_S32 regno, AX_S32 *data, AX_S32 size);
AX_S32 ax_mailbox_read(AX_S32 regno, AX_S32 *data, AX_S32 size);
AX_S32 ax_mailbox_wait_inturrpt(AX_S32 fromid);
AX_S32 ax_mailbox_request_channel(AX_U32 masterid, const AX_CHAR *channel_name, AX_S32 wait);

typedef enum {
    riscv_status_invalid,
    riscv_status_ready,
    riscv_status_active,
    riscv_status_idle,
} riscv_status_e;

static AX_MUTEX_T riscv_mutex;
riscv_status_e riscv_status;
static int riscv_set_cnt;

static int riscv_send_message(mbox_msg_t *msg)
{
    int regno, ret;

    if (msg == NULL) {
        RISCV_LOG_ERROR("param error");
        return AX_RISCV_PARA_FAIL;
    }
    regno = ax_mailbox_request_channel(AX_MAILBOX_MASTERID_RISCV, "riscv_msg", 0);
    if (regno < 0) {
        RISCV_LOG_ERROR("get regno error");
        return AX_RISCV_NORESOURCE;
    }

    ret = ax_mailbox_write(AX_MAILBOX_MASTERID_RISCV, regno, (int *)msg, sizeof(mbox_msg_t));
    if (ret != sizeof(mbox_msg_t)) {
        RISCV_LOG_ERROR("mailbox write fail 0x%x", ret);
        return AX_RISCV_MAILBOX_RW_FAIL;
    }

    return 0;
}

int riscv_recv_message(mbox_msg_t *msg)
{
    int ret, regno;

    if (msg == NULL) {
        RISCV_LOG_ERROR("param error");
        return AX_RISCV_PARA_FAIL;
    }

    regno = ax_mailbox_wait_inturrpt(AX_MAILBOX_MASTERID_RISCV);
    if (regno < 0) {
        RISCV_LOG_ERROR("receive message timeout");
        return AX_RISCV_MAILBOX_TIMEOUT_FAIL;
    }

    ret = ax_mailbox_read(regno, (int *)msg, 32);
    if (ret != 32) {
        RISCV_LOG_ERROR("return %d", ret);
        return AX_RISCV_MAILBOX_RW_FAIL;
    }

    return 0;
}


static int riscv_boot_up(void)
{
    int ret;
    uint64_t riscv_paddr, riscv_size;
    uint64_t log_mem_paddr, log_mem_size;
    uint64_t remap_size;
    uint32_t *ano_sys_base, *com_clk_base, *riscv_remap_base;
    int i, loop = 1000;
    uint32_t *dummy_sw11;

    ret = ax_riscv_utils_get_dts_reg(RISCV_DTS_NODE_RISCV, &riscv_paddr, &riscv_size);
    if (ret != 0) {
        RISCV_LOG_ERROR("get riscv memory info fail\n");
        return -1;
    }
    ret = ax_riscv_utils_get_dts_reg(RISCV_DTS_NODE_LOG_MEM, &log_mem_paddr, &log_mem_size);
    if (ret != 0) {
        RISCV_LOG_ERROR("get log memory info fail\n");
        return -1;
    }
    remap_size = riscv_size + log_mem_size;

    ret = ax_riscv_utils_loadbin(RISCV_FW_PATH, riscv_paddr);
    if (ret != 0) {
        RISCV_LOG_ERROR("load %s fail", RISCV_FW_PATH);
        return -1;
    }

    ano_sys_base = AX_OSAL_DEV_ioremap(AON_SYS_BASE, 0x1000);
    if (ano_sys_base == AX_NULL) {
        RISCV_LOG_ERROR("remap ano_sys_base fail");
        return -1;
    }
    com_clk_base = AX_OSAL_DEV_ioremap(COM_CLK_BASE, 0x1000);
    if (com_clk_base == AX_NULL) {
        AX_OSAL_DEV_iounmap(ano_sys_base);
        RISCV_LOG_ERROR("remap com_clk_base fail");
        return -1;
    }
    riscv_remap_base = AX_OSAL_DEV_ioremap(RISCV_REMAP_BASE, 0x1000);
    if (riscv_remap_base == AX_NULL) {
        AX_OSAL_DEV_iounmap(ano_sys_base);
        AX_OSAL_DEV_iounmap(com_clk_base);
        RISCV_LOG_ERROR("remap riscv_remap_base fail");
        return -1;
    }

    /* riscv pc addr */
    AX_OSAL_IO_writel(RISCV_PC_START, ano_sys_base + (0x254 / 4));
    /* set system clk to 500M */
    AX_OSAL_IO_writel(AX_BIT(26), com_clk_base + (0x28 / 4));
    AX_OSAL_IO_writel(AX_BIT(25), com_clk_base + (0x2C / 4));
    /* wdt sel */
    AX_OSAL_IO_writel(AX_BIT(12), com_clk_base + (0x28 / 4));
    /* clk_riscv_eb set */
    AX_OSAL_IO_writel(AX_BIT(7), com_clk_base + (0x40 / 4));
    /* riscv_sw_rst clear */
    AX_OSAL_IO_writel(AX_BIT(18), com_clk_base + (0x64 / 4));
    /* remap */
    AX_OSAL_IO_writel(RISCV_PC_START >> 12, riscv_remap_base + (0x00 / 4));
    AX_OSAL_IO_writel((remap_size / (4 * 1024)) - 1, riscv_remap_base + (0x04 / 4));
    AX_OSAL_IO_writel(riscv_paddr >> 12, riscv_remap_base + (0x08 / 4));
    AX_OSAL_IO_writel(AX_BIT(0), riscv_remap_base + (0x0c / 4));
    /* riscv_hold_sw_rst clear */
    AX_OSAL_IO_writel(AX_BIT(17), com_clk_base + (0x64 / 4));

    AX_OSAL_DEV_iounmap(ano_sys_base);
    AX_OSAL_DEV_iounmap(com_clk_base);
    AX_OSAL_DEV_iounmap(riscv_remap_base);

    dummy_sw11 = AX_OSAL_DEV_ioremap(COMM_SYS_DUMMY_SW11, 4);
    if (dummy_sw11 == NULL) {
        RISCV_LOG_ERROR("ioremap dummy_sw12 fail");
        return AX_RISCV_MAP_FAIL;
    }
    for (i = 0; i < loop; i++) {
        if (*dummy_sw11 == RISCV_DONE_MAGIC) {
            break;
        }
        AX_OSAL_TM_msleep(1);
    }
    if (i == loop) {
        RISCV_LOG_ERROR("riscv setup fail");
        AX_OSAL_DEV_iounmap(dummy_sw11);
        return -1;
    }
    AX_OSAL_DEV_iounmap(dummy_sw11);

    return 0;
}

static int riscv_clk_enable(void)
{
    AX_U32 *com_clk_base = AX_OSAL_DEV_ioremap(COM_CLK_BASE, 0x1000);
    if (com_clk_base == AX_NULL) {
        RISCV_LOG_ERROR("ioremap com_clk_base fail");
        return AX_RISCV_NORESOURCE;
    }
    AX_OSAL_IO_writel(AX_BIT(7), com_clk_base + (0x40 / 4));
    AX_OSAL_DEV_iounmap(com_clk_base);

    return 0;
}

static int riscv_clk_disable(void)
{
    AX_U32 *com_clk_base = AX_OSAL_DEV_ioremap(COM_CLK_BASE, 0x1000);
    if (com_clk_base == AX_NULL) {
        RISCV_LOG_ERROR("ioremap com_clk_base fail");
        return AX_RISCV_NORESOURCE;
    }
    AX_OSAL_IO_writel(AX_BIT(7), com_clk_base + (0x44 / 4));
    AX_OSAL_DEV_iounmap(com_clk_base);

    return 0;
}

static int riscv_suspend(void)
{
    int ret;
    uint32_t *ano_sys_base;
    u64 start, end;
    u32 reg, lpmd;
    mbox_msg_t msg = {.id = AX_MBOX_CMD_SUSPEND};

    ret = riscv_send_message(&msg);
    if (ret != 0) {
        RISCV_LOG_ERROR("send suspend riscv message fail");
        return ret;
    }

    ano_sys_base = AX_OSAL_DEV_ioremap(AON_SYS_BASE, 0x1000);
    if (ano_sys_base == AX_NULL) {
        RISCV_LOG_ERROR("remap ano_sys_base fail");
        return AX_RISCV_NORESOURCE;
    }
    start = AX_OSAL_TM_get_microseconds();
    while (1) {
        reg = *(ano_sys_base + (AON_SYS_RISCV_RESET_STATUS / 4));
        lpmd = (reg & 0x30) >> 4;
        if (lpmd == SYSIO_PAD_LPMD_B_DEEP_SLEEP) {
            break;
        }
        end = AX_OSAL_TM_get_microseconds();
        if ((end - start) > 20000) {
            AX_OSAL_DEV_iounmap(ano_sys_base);
            RISCV_LOG_ERROR("riscv entry suspend timeout, lpmd[0x%x] reg[0x%x]", lpmd, reg);
            return AX_RISCV_QUERY_TIMEOUT_FAIL;
        }
    }
    AX_OSAL_DEV_iounmap(ano_sys_base);
    riscv_clk_disable();

    return 0;
}

static int riscv_resume(void)
{
    int ret;
    uint32_t *ano_sys_base;
    u64 start, end;
    u32 reg, lpmd;
    mbox_msg_t msg = {.id = AX_MBOX_CMD_RESUME};

    riscv_clk_enable();
    ax_riscv_utils_interrupt_umask(RISCV_INTERRUPT_VECTOR_MAILBOX);
    ret = riscv_send_message(&msg);
    if (ret != 0) {
        RISCV_LOG_ERROR("send resume riscv message fail");
        return ret;
    }

    ano_sys_base = AX_OSAL_DEV_ioremap(AON_SYS_BASE, 0x1000);
    if (ano_sys_base == AX_NULL) {
        RISCV_LOG_ERROR("remap ano_sys_base fail");
        return AX_RISCV_NORESOURCE;
    }
    start = AX_OSAL_TM_get_microseconds();
    while (1) {
        reg = *(ano_sys_base + (AON_SYS_RISCV_RESET_STATUS / 4));
        lpmd = (reg & 0x30) >> 4;
        if (lpmd == SYSIO_PAD_LPMD_B_RUNNING) {
            break;
        }
        end = AX_OSAL_TM_get_microseconds();
        if ((end - start) > 20000) {
            AX_OSAL_DEV_iounmap(ano_sys_base);
            RISCV_LOG_ERROR("riscv resume from low power mode timeout, lpmd[0x%x], reg[0x%x]", lpmd, reg);
            return AX_RISCV_QUERY_TIMEOUT_FAIL;
        }
    }
    AX_OSAL_DEV_iounmap(ano_sys_base);

    return 0;
}

int ax_riscv_setup(void)
{
    int ret;
    AX_OSAL_SYNC_mutex_lock(&riscv_mutex);

    riscv_set_cnt++;
    if (riscv_set_cnt <= 0) {
        RISCV_LOG_ERROR("setup and shutup error, riscv_set_cnt[%d]", riscv_set_cnt);
        AX_OSAL_SYNC_mutex_unlock(&riscv_mutex);
        return AX_RISCV_STATUS_ERROR;
    }
    if (riscv_set_cnt > 1) {
        AX_OSAL_SYNC_mutex_unlock(&riscv_mutex);
        return 0;
    }

    switch (riscv_status) {
    case riscv_status_ready:
        ret = riscv_boot_up();
        if (ret != 0) {
            AX_OSAL_SYNC_mutex_unlock(&riscv_mutex);
            RISCV_LOG_ERROR("riscv_boot_up fail");
            return -1;
        }
        ax_riscv_debug_enable();
        riscv_status = riscv_status_active;
        break;
    case riscv_status_idle:
        ret = riscv_resume();
        if (ret != 0) {
            AX_OSAL_SYNC_mutex_unlock(&riscv_mutex);
            RISCV_LOG_ERROR("riscv_resume fail");
            return ret;
        }
        riscv_status = riscv_status_active;
        break;
    default:
        AX_OSAL_SYNC_mutex_unlock(&riscv_mutex);
        RISCV_LOG_ERROR("riscv status error, %d", riscv_status);
        return AX_RISCV_STATUS_ERROR;
    }

    AX_OSAL_SYNC_mutex_unlock(&riscv_mutex);
    return 0;
}
EXPORT_SYMBOL(ax_riscv_setup);

int ax_riscv_shutup(void)
{
    int ret;
    AX_OSAL_SYNC_mutex_lock(&riscv_mutex);

    riscv_set_cnt--;
    if (riscv_set_cnt < 0) {
        AX_OSAL_SYNC_mutex_unlock(&riscv_mutex);
        RISCV_LOG_ERROR("setup and shutup error, riscv_set_cnt[%d]", riscv_set_cnt);
        return AX_RISCV_STATUS_ERROR;
    }
    if (riscv_set_cnt > 0) {
        AX_OSAL_SYNC_mutex_unlock(&riscv_mutex);
        return 0;
    }

    switch (riscv_status) {
    case riscv_status_active:
        ret = riscv_suspend();
        if (ret != 0) {
            AX_OSAL_SYNC_mutex_unlock(&riscv_mutex);
            RISCV_LOG_ERROR("riscv_suspend fail");
            return ret;
        }
        riscv_status = riscv_status_idle;
        break;
    default:
        AX_OSAL_SYNC_mutex_unlock(&riscv_mutex);
        RISCV_LOG_ERROR("riscv status error, %d", riscv_status);
        return AX_RISCV_STATUS_ERROR;
    }

    AX_OSAL_SYNC_mutex_unlock(&riscv_mutex);

    return 0;
}
EXPORT_SYMBOL(ax_riscv_shutup);

int ax_riscv_send_message(mbox_msg_t *msg)
{
    int ret;

    if (msg == NULL) {
        RISCV_LOG_ERROR("params error");
        return AX_RISCV_ERR_BUTT;
    }

    AX_OSAL_SYNC_mutex_lock(&riscv_mutex);
    if (riscv_status != riscv_status_active) {
        AX_OSAL_SYNC_mutex_unlock(&riscv_mutex);
        RISCV_LOG_ERROR("status error, riscv_status[%d]", riscv_status);
        return AX_RISCV_STATUS_ERROR;
    }
    AX_OSAL_SYNC_mutex_unlock(&riscv_mutex);

    ret = riscv_send_message(msg);
    if (ret != 0) {
        RISCV_LOG_ERROR("riscv_send_message fail");
        return ret;
    }

    return 0;
}
EXPORT_SYMBOL(ax_riscv_send_message);

int ax_riscv_recv_message(mbox_msg_t *msg)
{
    int ret;

    if (msg == NULL) {
        RISCV_LOG_ERROR("params error");
        return AX_RISCV_ERR_BUTT;
    }

    AX_OSAL_SYNC_mutex_lock(&riscv_mutex);
    if (riscv_status != riscv_status_active) {
        AX_OSAL_SYNC_mutex_unlock(&riscv_mutex);
        RISCV_LOG_ERROR("status error, riscv_status[%d]", riscv_status);
        return AX_RISCV_STATUS_ERROR;
    }
    AX_OSAL_SYNC_mutex_unlock(&riscv_mutex);

    ret = riscv_recv_message(msg);
    if (ret != 0) {
        RISCV_LOG_ERROR("riscv_recv_message fail");
        return ret;
    }

    return 0;
}
EXPORT_SYMBOL(ax_riscv_recv_message);

static int ax_riscv_probe(void *pdev)
{
    riscv_status = riscv_status_ready;
    ax_riscv_debug_init();
    return 0;
}

static int ax_riscv_remove(void *pdev)
{
    ax_riscv_debug_deinit();
    return 0;
}

static const struct AX_OF_DEVICE_ID ax_riscv_of_match[] = {
    {.compatible = "axera,riscv"},
    {},
};

static struct AX_PLATFORM_DRIVER ax_riscv_driver = {
    .driver = {
        .name = DRIVER_NAME,
        .of_match_table = ax_riscv_of_match,
    },
    .probe = ax_riscv_probe,
    .remove = ax_riscv_remove,
};

static int ax_riscv_driver_init(void)
{
    int ret;

    ret = AX_OSAL_SYNC_mutex_init(&riscv_mutex);
    if (ret != 0) {
        RISCV_LOG_ERROR("init riscv_mutex fail,  %d", ret);
        return ret;
    }
    riscv_status = riscv_status_invalid;
    riscv_set_cnt = 0;

    ret = AX_OSAL_DEV_platform_driver_register((void *)&ax_riscv_driver);
    if (ret != 0) {
        RISCV_LOG_ERROR("register riscv fail, %s", axera_module_version);
        return ret;
    }

    return 0;
}

static void __exit ax_riscv_driver_exit(void)
{
    AX_OSAL_SYNC_mutex_destroy(&riscv_mutex);
    AX_OSAL_DEV_platform_driver_unregister((void *)&ax_riscv_driver);
}

AX_OSAL_module_initcall(ax_riscv_driver_init);
AX_OSAL_module_exit(ax_riscv_driver_exit);

MODULE_AUTHOR("axera");
MODULE_LICENSE("GPL");
MODULE_VERSION("1.0");
MODULE_INFO(intree, "Y");

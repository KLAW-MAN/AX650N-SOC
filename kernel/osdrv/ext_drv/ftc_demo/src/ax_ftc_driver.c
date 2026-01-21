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

#include "ax_global_type.h"
#include <linux/workqueue.h>

#include "linux/gpio.h"
#include "linux/interrupt.h"
#include "linux/string.h"
#include "linux/kstrtox.h"

#include "ax_vin_kernel_ext.h"
#include "ax_vin_error_code.h"
#include "ax_sync_signal.h"

#include "ax_ftc_priv.h"
#include "ax_sif_ftc_api.h"
#include "ax_sif_reg.h"

#include "ax_ftc_process.h"
#include "ax_ftc_event.h"

vin_ftc_node_priv_ctx_t *g_ftc_node_priv_ctx = AX_NULL;

ftc_regs *g_ftc_regs = AX_NULL;

#define FTC_REG_ADDR 0x12004200
#define FTC_RELOAD_ADDR 0x12801000
#define FTC_SEQ_ADDR 0x12004500
#define FTC_MAP_SIZE 0x100

AX_S32 ax_ftc_interrupt_handle(ax_vin_interrupt_type_e int_type, AX_VOID *pdata)
{
    AX_S32 ret = 0;
    switch (int_type) {
    case AX_VIN_INT_TYPE_SNS_FSOF:
        vin_ftc_node_event_forward();
        break;
    case AX_VIN_INT_TYPE_SNS_FEOF:
        vin_ftc_node_event_backward();
        break;
    default:
        printk("bad type\n");
        ret = -1;
        break;
    }
    return ret;
}

AX_LONG ax_ftc_ioctl(AX_U32 cmd, AX_ULONG arg, AX_VOID *private_data)
{
    AX_S32 retval = AX_VIN_SUCCESS;

    ax_sync_power_attr_t power_attr = {0};
    ax_sensor_vts_t sensor_vts = {0};
    ax_sync_signal_t vsync_attr = {0};
    AX_BOOL vsync_enable = 0;
    ax_sync_signal_t hsync_attr = {0};
    AX_BOOL hsync_enable = 0;
    ax_sync_info_t sync_info = {0};
    ax_light_sync_signal_t sync_signal = {0};
    ax_light_sync_enable_t light_enable = {0};
    VIN_CHECK_PTR(private_data);

    switch (cmd) {
    case TCM_CMD_DEV_SYNC_POWERATTR_SET:
        retval = copy_from_user(&power_attr, (AX_VOID *)arg, sizeof(ax_sync_power_attr_t));
        vin_ftc_node_set_data(DEV_NODE_CMD_SYNC_POWERATTR_SET, (AX_VOID *)&power_attr);
        if (retval != 0) {
            retval = -EFAULT;
        }
        break;
    case TCM_CMD_DEV_SYNC_POWERATTR_GET:
        retval = copy_from_user(&power_attr, (AX_VOID *)arg, sizeof(ax_sync_power_attr_t));
        vin_ftc_node_get_data(DEV_NODE_CMD_SYNC_POWERATTR_GET, (AX_VOID *)&power_attr);
        retval = copy_to_user((AX_VOID *)arg, &power_attr, sizeof(ax_sync_power_attr_t));
        if (retval != 0) {
            retval = -EFAULT;
        }
        break;
    case TCM_CMD_DEV_VSYNCATTR_SET:
        retval = copy_from_user(&vsync_attr, (AX_VOID *)arg, sizeof(ax_sync_signal_t));
        vin_ftc_node_set_data(DEV_NODE_CMD_VSYNCATTR_SET, (AX_VOID *)&vsync_attr);
        if (retval != 0) {
            retval = -EFAULT;
        }
        break;
    case TCM_CMD_DEV_VSYNCATTR_GET:
        retval = copy_from_user(&vsync_attr, (AX_VOID *)arg, sizeof(ax_sync_signal_t));
        vin_ftc_node_get_data(DEV_NODE_CMD_VSYNCATTR_GET, (AX_VOID *)&vsync_attr);
        retval = copy_to_user((AX_VOID *)arg, &vsync_attr, sizeof(ax_sync_signal_t));
        if (retval != 0) {
            retval = -EFAULT;
        }
        break;
    case TCM_CMD_DEV_VSYNC_ENABLE_SET:
        retval = copy_from_user(&vsync_enable, (AX_VOID *)arg, sizeof(AX_BOOL));
        vin_ftc_node_set_data(DEV_NODE_CMD_VSYNC_ENABLE_SET, (AX_VOID *)&vsync_enable);
        if (retval != 0) {
            retval = -EFAULT;
        }
        break;
    case TCM_CMD_DEV_HSYNCATTR_SET:
        retval = copy_from_user(&hsync_attr, (AX_VOID *)arg, sizeof(ax_sync_signal_t));
        vin_ftc_node_set_data(DEV_NODE_CMD_HSYNCATTR_SET, (AX_VOID *)&hsync_attr);
        if (retval != 0) {
            retval = -EFAULT;
        }
        break;
    case TCM_CMD_DEV_HSYNCATTR_GET:
        retval = copy_from_user(&hsync_attr, (AX_VOID *)arg, sizeof(ax_sync_signal_t));
        vin_ftc_node_get_data(DEV_NODE_CMD_HSYNCATTR_GET, (AX_VOID *)&hsync_attr);
        retval = copy_to_user((AX_VOID *)arg, &hsync_attr, sizeof(ax_sync_signal_t));
        if (retval != 0) {
            retval = -EFAULT;
        }
        break;
    case TCM_CMD_DEV_HSYNC_ENABLE_SET:
        retval = copy_from_user(&hsync_enable, (AX_VOID *)arg, sizeof(AX_BOOL));
        vin_ftc_node_set_data(DEV_NODE_CMD_HSYNC_ENABLE_SET, (AX_VOID *)&hsync_enable);
        if (retval != 0) {
            retval = -EFAULT;
        }
        break;
    case TCM_CMD_DEV_LIGHTSYNCINFO_SET:
        retval = copy_from_user(&sync_info, (AX_VOID *)arg, sizeof(ax_sync_info_t));
        vin_ftc_node_set_data(DEV_NODE_CMD_LIGHTSYNCINFO_SET, (AX_VOID *)&sync_info);
        if (retval != 0) {
            retval = -EFAULT;
        }
        break;
    case TCM_CMD_DEV_LIGHTSYNCINFO_GET:
        retval = copy_from_user(&sync_info, (AX_VOID *)arg, sizeof(ax_sync_info_t));
        vin_ftc_node_get_data(DEV_NODE_CMD_LIGHTSYNCINFO_GET, (AX_VOID *)&sync_info);
        retval = copy_to_user((AX_VOID *)arg, &sync_info, sizeof(ax_sync_info_t));
        if (retval != 0) {
            retval = -EFAULT;
        }
        break;
    case TCM_CMD_DEV_LIGHTSYNCATTR_SET:
        retval = copy_from_user(&sync_signal, (AX_VOID *)arg, sizeof(ax_light_sync_signal_t));
        vin_ftc_node_set_data(DEV_NODE_CMD_LIGHTSYNCATTR_SET, (AX_VOID *)&sync_signal);
        if (retval != 0) {
            retval = -EFAULT;
        }
        break;
    case TCM_CMD_DEV_LIGHTSYNCATTR_GET:
        retval = copy_from_user(&sync_signal, (AX_VOID *)arg, sizeof(ax_light_sync_signal_t));
        vin_ftc_node_get_data(DEV_NODE_CMD_LIGHTSYNCATTR_GET, (AX_VOID *)&sync_signal);
        retval = copy_to_user((AX_VOID *)arg, &sync_signal, sizeof(ax_light_sync_signal_t));
        if (retval != 0) {
            retval = -EFAULT;
        }
        break;
    case TCM_CMD_DEV_LIGHT_ENABLE_SET:
        retval = copy_from_user(&light_enable, (AX_VOID *)arg, sizeof(ax_light_sync_enable_t));
        vin_ftc_node_set_data(DEV_NODE_CMD_LIGHT_ENABLE_SET, (AX_VOID *)&light_enable);
        if (retval != 0) {
            retval = -EFAULT;
        }
        break;
    case TCM_CMD_DEV_FTC_INTERRUPT_EN:
        retval = CB_VIN_INT_Notify_RegisterCallback(0, ax_ftc_interrupt_handle);
        if (retval) {
            printk("CB_VIN_INT_Notify_RegisterCallback fail 0x%x\n", retval);
            return retval;
        }
        break;
    case TCM_CMD_DEV_SENSOR_VTS_SET:
        retval = copy_from_user(&sensor_vts, (AX_VOID *)arg, sizeof(ax_sensor_vts_t));
        vin_ftc_node_set_data(DEV_NODE_CMD_SENSOR_VTS_SET, (AX_VOID *)&sensor_vts);
        if (retval != 0) {
            retval = -EFAULT;
        }
        break;
    default:
        break;
    }

    return retval;
}

static struct AX_FILEOPS ax_ftc_fops = {
    .unlocked_ioctl          = ax_ftc_ioctl,
};

AX_S32 ax_ftc_driver_start (AX_VOID)
{
    AX_S32 retval = AX_VIN_SUCCESS;
    VIN_CHECK_PTR(g_ftc_node_priv_ctx);

    AX_RISCV_Kernel_setup_riscv();
    AX_RISCV_Kernel_send_version_info();
    AX_RISCV_Kernel_init();

    return retval;
}
AX_S32 ax_ftc_driver_stop (AX_VOID)
{
    AX_S32 retval = AX_VIN_SUCCESS;
    regio_global_handle_t io_handle;

    VIN_CHECK_PTR(g_ftc_node_priv_ctx);

    VIN_CHECK_PTR(g_ftc_regs);
    VIN_CHECK_PTR(g_ftc_regs->reload_reg);
    VIN_CHECK_PTR(g_ftc_regs->sen_reg);

    io_handle = g_ftc_regs->sen_reg;
    isp_sif_sensor_expo_ctrl_enable(io_handle, AX_FALSE);

    AX_RISCV_Kernel_stop();
    AX_RISCV_Kernel_deinit();

    return retval;
}

static int ax_ftc_driver_init(AX_VOID)
{
    AX_S32 retval = AX_VIN_SUCCESS;

    g_ftc_node_priv_ctx = (vin_ftc_node_priv_ctx_t *)ax_os_mem_kmalloc(AX_OS_NODE_FTC, sizeof(vin_ftc_node_priv_ctx_t),
                         AX_OSAL_GFP_KERNEL);
    if (AX_NULL == g_ftc_node_priv_ctx) {
        isp_err("kmalloc g_ftc_node_priv_ctx failed\n");
        return AX_ERR_VIN_NOMEM;
    } else {
        AX_OSAL_LIB_memset(g_ftc_node_priv_ctx, 0, sizeof(vin_ftc_node_priv_ctx_t));
    }

    g_ftc_regs = (ftc_regs *)ax_os_mem_kmalloc(AX_OS_NODE_FTC, sizeof(ftc_regs),
                         AX_OSAL_GFP_KERNEL);
    if (AX_NULL == g_ftc_regs) {
        isp_err("kmalloc g_ftc_regs failed\n");
        return AX_ERR_VIN_NOMEM;
    } else {
        AX_OSAL_LIB_memset(g_ftc_regs, 0, sizeof(ftc_regs));
    }

    g_ftc_regs->sen_reg = AX_OSAL_DEV_ioremap(FTC_REG_ADDR, FTC_MAP_SIZE);
    if (!g_ftc_regs->sen_reg) {
        printk("ISP base ioremap fail!\n");
        return AX_ERR_VIN_NOMEM;
    }

    g_ftc_regs->reload_reg = AX_OSAL_DEV_ioremap(FTC_RELOAD_ADDR, FTC_MAP_SIZE);
    if (!g_ftc_regs->reload_reg) {
        printk("ISP base ioremap fail!\n");
        return AX_ERR_VIN_NOMEM;
    }

    g_ftc_regs->seq_reg = AX_OSAL_DEV_ioremap(FTC_SEQ_ADDR, FTC_MAP_SIZE);
    if (!g_ftc_regs->seq_reg) {
        printk("ISP base ioremap fail!\n");
        return AX_ERR_VIN_NOMEM;
    }

    g_ftc_node_priv_ctx->ftc_dev = AX_OSAL_DEV_createdev(AX_FTC_DRIVER_NAME);
    if (NULL == g_ftc_node_priv_ctx->ftc_dev) {
        printk("ftc device create Error!\n");
        return AX_ERR_VIN_NOMEM;
    }

    g_ftc_node_priv_ctx->ftc_dev->fops = &ax_ftc_fops;
    retval = AX_OSAL_DEV_device_register(g_ftc_node_priv_ctx->ftc_dev);
    if (AX_ISP_SUCCESS != retval) {
        printk("ftc device register Error!\n");
        return AX_ERR_VIN_NOMEM;
    }

    retval = ax_ftc_driver_start();
    if (AX_ISP_SUCCESS != retval) {
        printk("ax_ftc_driver_start err!\n");
        return AX_ERR_VIN_NOMEM;
    }

    AX_OSAL_SYNC_spin_lock_init(&g_ftc_node_priv_ctx->ftc_cfg_slock);

    return retval;

}

static AX_VOID __exit ax_ftc_driver_exit(AX_VOID)
{

    AX_S32 retval = AX_VIN_SUCCESS;

    retval = ax_ftc_driver_stop();
    if (AX_ISP_SUCCESS != retval) {
        printk("ax_ftc_driver_stop err!\n");
        return ;
    }

    AX_OSAL_SYNC_spinLock_destory(&g_ftc_node_priv_ctx->ftc_cfg_slock);
    AX_OSAL_DEV_device_unregister(g_ftc_node_priv_ctx->ftc_dev);
    AX_OSAL_DEV_destroydev(g_ftc_node_priv_ctx->ftc_dev);

    AX_OSAL_DEV_iounmap(g_ftc_regs->sen_reg);
    AX_OSAL_DEV_iounmap(g_ftc_regs->reload_reg);
    AX_OSAL_DEV_iounmap(g_ftc_regs->seq_reg);

    if (g_ftc_node_priv_ctx) {
        ax_os_mem_kfree(AX_OS_NODE_FTC, g_ftc_node_priv_ctx);
        g_ftc_node_priv_ctx = AX_NULL;
    }

    if (g_ftc_regs) {
        ax_os_mem_kfree(AX_OS_NODE_FTC, g_ftc_regs);
        g_ftc_regs = AX_NULL;
    }

    return ;
}

AX_OSAL_module_initcall(ax_ftc_driver_init);
AX_OSAL_module_exit(ax_ftc_driver_exit);

MODULE_AUTHOR("Axera Technology Co., Ltd.");
MODULE_DESCRIPTION("Axera gpio Soc Chip Driver");
// MODULE_SUPPORTED_DEVICE("AX170/AX620");
MODULE_LICENSE("Dual BSD/GPL");
MODULE_VERSION("1.0");

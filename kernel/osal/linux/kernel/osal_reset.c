/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include <linux/device.h>
#include <linux/kdev_t.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kmod.h>
#include <linux/fs.h>
#include <linux/clk.h>
#include <linux/device.h>
#include <linux/reset.h>

#include "linux/platform_device.h"
#include "linux/device.h"

#include "osal_ax.h"
#include "osal_dev_ax.h"
#include "axdev.h"
#include "axdev_log.h"
#include "osal_lib_ax.h"

AX_VOID *AX_OSAL_DEV_devm_reset_control_get_optional(AX_VOID * pdev, const char *id, int flag)
{
	struct platform_device *pvdev;
	if (flag == 0) {
		pvdev = (struct platform_device *)pdev;
		return devm_reset_control_get_optional(&pvdev->dev, id);
	} else {
		return 0;
	}
}

EXPORT_SYMBOL(AX_OSAL_DEV_devm_reset_control_get_optional);

AX_S32 __attribute__ ((__noinline__)) AX_OSAL_DEV_reset_control_assert(AX_VOID * rstc)
{
	struct reset_control *prstc = (struct reset_control *)rstc;
	return reset_control_assert(prstc);
}

EXPORT_SYMBOL(AX_OSAL_DEV_reset_control_assert);

AX_S32 __attribute__ ((__noinline__)) AX_OSAL_DEV_reset_control_deassert(AX_VOID * rstc)
{
	struct reset_control *prstc = (struct reset_control *)rstc;
	return reset_control_deassert(prstc);
}

EXPORT_SYMBOL(AX_OSAL_DEV_reset_control_deassert);

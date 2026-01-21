/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/printk.h>
#include <linux/mutex.h>
#include <linux/slab.h>
#include "osal_pm_ax.h"

AX_S32 AX_OSAL_PM_WakeupLock(AX_S8 * lock_name)
{
	return 0;
}

EXPORT_SYMBOL(AX_OSAL_PM_WakeupLock);

AX_S32 AX_OSAL_PM_WakeupUnlock(AX_S8 * lock_name)
{
	return 0;
}

EXPORT_SYMBOL(AX_OSAL_PM_WakeupUnlock);

AX_S32 AX_OSAL_PM_SetLevel(AX_S32 pm_level)
{
	return 0;
}

EXPORT_SYMBOL(AX_OSAL_PM_SetLevel);

AX_S32 AX_OSAL_PM_GetLevel(AX_S32 * pm_level)
{
	return 0;
}

EXPORT_SYMBOL(AX_OSAL_PM_GetLevel);

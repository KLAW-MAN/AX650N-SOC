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
#include <asm/barrier.h>
#include "osal_ax.h"

AX_VOID AX_OSAL_SYNC_mb(AX_VOID)
{
	if (IS_ENABLED(CONFIG_SMP)) {
		smp_mb();
	} else {
		mb();
	}
}

EXPORT_SYMBOL(AX_OSAL_SYNC_mb);

AX_VOID AX_OSAL_SYNC_rmb(AX_VOID)
{
	if (IS_ENABLED(CONFIG_SMP)) {
		smp_rmb();
	} else {
		rmb();
	}
}

EXPORT_SYMBOL(AX_OSAL_SYNC_rmb);

AX_VOID AX_OSAL_SYNC_wmb(AX_VOID)
{
	if (IS_ENABLED(CONFIG_SMP)) {
		smp_wmb();
	} else {
		wmb();
	}
}

EXPORT_SYMBOL(AX_OSAL_SYNC_wmb);

AX_VOID AX_OSAL_SYNC_isb(AX_VOID)
{
	isb();
}

EXPORT_SYMBOL(AX_OSAL_SYNC_isb);

AX_VOID AX_OSAL_SYNC_dsb(AX_VOID)
{
	dsb(sy);
}

EXPORT_SYMBOL(AX_OSAL_SYNC_dsb);

AX_VOID AX_OSAL_SYNC_dmb(AX_VOID)
{
	dmb(sy);
}

EXPORT_SYMBOL(AX_OSAL_SYNC_dmb);

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
#include <asm/cacheflush.h>
#include <linux/dma-direction.h>
#include "osal_ax.h"
#include "osal_dev_ax.h"

AX_VOID AX_OSAL_DEV_invalidate_dcache_area(AX_VOID * addr, AX_S32 size)
{
#ifdef CONFIG_ARM64
	dcache_inval_poc((AX_ULONG) addr, (AX_ULONG) (addr + size));
#else
	dmac_inv_range(addr, addr + size);
#endif
	return;
}

EXPORT_SYMBOL(AX_OSAL_DEV_invalidate_dcache_area);

AX_VOID AX_OSAL_DEV_flush_dcache_area(AX_VOID * kvirt, AX_ULONG length)
{
#ifdef CONFIG_ARM64
	dcache_clean_inval_poc((AX_ULONG) kvirt, (AX_ULONG) (kvirt + length));
#else
	__cpuc_flush_dcache_area(kvirt, length);
#endif
}

EXPORT_SYMBOL(AX_OSAL_DEV_flush_dcache_area);

AX_VOID AX_OSAL_DEV_outer_dcache_area(AX_U64 phys_addr_start, AX_U64 phys_addr_end)
{
#ifdef CONFIG_ARM64
	return;
#else
	outer_flush_range(phys_addr_start, phys_addr_end);
	return;
#endif
}

EXPORT_SYMBOL(AX_OSAL_DEV_outer_dcache_area);

AX_VOID AX_OSAL_DEV_flush_dcache_all(AX_VOID)
{
#ifdef CONFIG_ARM64
	axera_armv8_flush_cache_all();
#endif
}

EXPORT_SYMBOL(AX_OSAL_DEV_flush_dcache_all);

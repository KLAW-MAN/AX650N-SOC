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
#include <asm/io.h>
#include <linux/uaccess.h>
#include "osal_ax.h"
#include "osal_dev_ax.h"


AX_VOID *AX_OSAL_DEV_ioremap(AX_ULONG phys_addr, AX_ULONG size)
{
	return ioremap(phys_addr, size);
}

EXPORT_SYMBOL(AX_OSAL_DEV_ioremap);

AX_VOID *AX_OSAL_DEV_ioremap_nocache(AX_ULONG phys_addr, AX_ULONG size)
{
	return ioremap_wc(phys_addr, size);
}

EXPORT_SYMBOL(AX_OSAL_DEV_ioremap_nocache);

AX_VOID *AX_OSAL_DEV_ioremap_cache(AX_ULONG phys_addr, AX_ULONG size)
{
	return ioremap_cache(phys_addr, size);
}

EXPORT_SYMBOL(AX_OSAL_DEV_ioremap_cache);

AX_VOID AX_OSAL_DEV_iounmap(AX_VOID * addr)
{
	iounmap(addr);
}

EXPORT_SYMBOL(AX_OSAL_DEV_iounmap);

AX_ULONG AX_OSAL_DEV_copy_from_user(AX_VOID * to, const AX_VOID * from, AX_ULONG n)
{
	return copy_from_user(to, from, n);
}

EXPORT_SYMBOL(AX_OSAL_DEV_copy_from_user);

AX_ULONG AX_OSAL_DEV_copy_to_user(AX_VOID * to, const AX_VOID * from, AX_ULONG n)
{
	return copy_to_user(to, from, n);
}

EXPORT_SYMBOL(AX_OSAL_DEV_copy_to_user);

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

AX_VOID AX_OSAL_prefetch(AX_VOID *addr)
{
	prefetch(addr);
}
EXPORT_SYMBOL(AX_OSAL_prefetch);

AX_VOID AX_OSAL_prefetchw(AX_VOID *addr)
{
	prefetchw(addr);
}
EXPORT_SYMBOL(AX_OSAL_prefetchw);

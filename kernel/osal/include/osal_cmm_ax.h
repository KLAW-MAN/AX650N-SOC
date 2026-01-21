/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef __OSAL_CMM_AX__H__
#define __OSAL_CMM_AX__H__

#ifdef __cplusplus
extern "C" {
#endif

#include "osal_type_ax.h"

AX_S32 ax_cmm_userdev_init(AX_VOID);
AX_S32 ax_cmm_userdev_exit(AX_VOID);

AX_S32 AX_OSAL_MemAlloc(AX_U64 *phyaddr, AX_VOID **ppviraddr, AX_U32 size, AX_U32 align, AX_S8 *token);
AX_S32 AX_OSAL_MemAllocCached(AX_U64 *phyaddr, AX_VOID **pviraddr, AX_U32 size, AX_U32 align, AX_S8 *token);
AX_S32 AX_OSAL_MemAllocAlign(AX_U64 *phyaddr, AX_VOID **ppviraddr, AX_U32 size, AX_U32 align, AX_S8 *token);
AX_S32 AX_OSAL_MemAllocCachedAlign(AX_U64 *phyaddr, AX_VOID **pviraddr, AX_U32 size, AX_U32 align, AX_S8 *token);
AX_S32 AX_OSAL_MemFlushCache(AX_U64 phyaddr, AX_VOID *pviraddr, AX_U32 size);
AX_S32 AX_OSAL_MemInvalidateCache(AX_U64 phyaddr, AX_VOID *pviraddr, AX_U32 size);
AX_S32 AX_OSAL_MemFree(AX_U64 phyaddr, AX_VOID *pviraddr);
AX_VOID *AX_OSAL_Mmap(AX_U64 phyaddr, AX_U32 size);
AX_VOID *AX_OSAL_MmapCache(AX_U64 phyaddr, AX_U32 size);
AX_S32 AX_OSAL_Munmap(AX_VOID *pviraddr);
AX_S32 AX_OSAL_MemGetBlockInfoByPhy(AX_U64 phyaddr, AX_S32 *pmemType, AX_VOID **pviraddr, AX_U32 *pblockSize);
AX_S32 AX_OSAL_MemGetBlockInfoByVirt(AX_VOID *pviraddr, AX_U64 *phyaddr, AX_S32 *pmemType);
#ifdef __cplusplus
}
#endif

#endif /*__OSAL_CMM_AX__H__*/

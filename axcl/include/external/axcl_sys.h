/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef __AXCL_SYS_API_H__
#define __AXCL_SYS_API_H__

#include "axcl_sys_type.h"

#ifdef __cplusplus
extern "C" {
#endif

AXCL_EXPORT AX_S32 AXCL_SYS_Init(AX_VOID);
AXCL_EXPORT AX_S32 AXCL_SYS_Deinit(AX_VOID);

/* CMM API */
AXCL_EXPORT AX_S32 AXCL_SYS_MemAlloc(AX_U64 *phyaddr, AX_VOID **pviraddr, AX_U32 size, AX_U32 align, const AX_S8 *token);
AXCL_EXPORT AX_S32 AXCL_SYS_MemAllocCached(AX_U64 *phyaddr, AX_VOID **pviraddr, AX_U32 size, AX_U32 align, const AX_S8 *token);
AXCL_EXPORT AX_S32 AXCL_SYS_MemFree(AX_U64 phyaddr, AX_VOID *pviraddr);
AXCL_EXPORT AX_VOID *AXCL_SYS_Mmap(AX_U64 phyaddr, AX_U32 size);
AXCL_EXPORT AX_VOID *AXCL_SYS_MmapCache(AX_U64 phyaddr, AX_U32 size);
AXCL_EXPORT AX_VOID *AXCL_SYS_MmapFast(AX_U64 phyaddr, AX_U32 size);
AXCL_EXPORT AX_VOID *AXCL_SYS_MmapCacheFast(AX_U64 phyaddr, AX_U32 size);
AXCL_EXPORT AX_S32 AXCL_SYS_Munmap(AX_VOID *pviraddr, AX_U32 size);
AXCL_EXPORT AX_S32 AXCL_SYS_MflushCache(AX_U64 phyaddr, AX_VOID *pviraddr, AX_U32 size);
AXCL_EXPORT AX_S32 AXCL_SYS_MinvalidateCache(AX_U64 phyaddr, AX_VOID *pviraddr, AX_U32 size);
AXCL_EXPORT AX_S32 AXCL_SYS_MemGetBlockInfoByPhy(AX_U64 phyaddr, AX_S32 *pmemType, AX_VOID **pviraddr, AX_U32 *pblockSize);
AXCL_EXPORT AX_S32 AXCL_SYS_MemGetBlockInfoByVirt(AX_VOID *pviraddr, AX_U64 *phyaddr, AX_S32 *pmemType);
AXCL_EXPORT AX_S32 AXCL_SYS_MemGetPartitionInfo(AX_CMM_PARTITION_INFO_T *pCmmPartitionInfo);
AXCL_EXPORT AX_S32 AXCL_SYS_MemSetConfig(const AX_MOD_INFO_T *pModInfo, const AX_S8 *pPartitionName);
AXCL_EXPORT AX_S32 AXCL_SYS_MemGetConfig(const AX_MOD_INFO_T *pModInfo, AX_S8 *pPartitionName);
AXCL_EXPORT AX_S32 AXCL_SYS_MemQueryStatus(AX_CMM_STATUS_T *pCmmStatus);

/* LINK API*/
AXCL_EXPORT AX_S32 AXCL_SYS_Link(const AX_MOD_INFO_T *pSrc, const AX_MOD_INFO_T *pDest);
AXCL_EXPORT AX_S32 AXCL_SYS_UnLink(const AX_MOD_INFO_T *pSrc, const AX_MOD_INFO_T *pDest);
AXCL_EXPORT AX_S32 AXCL_SYS_GetLinkByDest(const AX_MOD_INFO_T *pDest, AX_MOD_INFO_T *pSrc);
AXCL_EXPORT AX_S32 AXCL_SYS_GetLinkBySrc(const AX_MOD_INFO_T *pSrc, AX_LINK_DEST_T *pLinkDest);

/* POOL API */
AXCL_EXPORT AX_S32 AXCL_POOL_SetConfig(const AX_POOL_FLOORPLAN_T *pPoolFloorPlan);
AXCL_EXPORT AX_S32 AXCL_POOL_GetConfig(AX_POOL_FLOORPLAN_T *pPoolFloorPlan);
AXCL_EXPORT AX_S32 AXCL_POOL_Init(AX_VOID);
AXCL_EXPORT AX_S32 AXCL_POOL_Exit(AX_VOID);
AXCL_EXPORT AX_POOL AXCL_POOL_CreatePool(AX_POOL_CONFIG_T *pPoolConfig);
AXCL_EXPORT AX_S32 AXCL_POOL_DestroyPool(AX_POOL PoolId);
AXCL_EXPORT AX_BLK AXCL_POOL_GetBlock(AX_POOL PoolId, AX_U64 BlkSize, const AX_S8 *pPartitionName);
AXCL_EXPORT AX_S32 AXCL_POOL_ReleaseBlock(AX_BLK BlockId);
AXCL_EXPORT AX_BLK AXCL_POOL_PhysAddr2Handle(AX_U64 PhysAddr);
AXCL_EXPORT AX_U64 AXCL_POOL_Handle2PhysAddr(AX_BLK BlockId);
AXCL_EXPORT AX_U64 AXCL_POOL_Handle2MetaPhysAddr(AX_BLK BlockId);
AXCL_EXPORT AX_POOL AXCL_POOL_Handle2PoolId(AX_BLK BlockId);
AXCL_EXPORT AX_U64 AXCL_POOL_Handle2BlkSize(AX_BLK BlockId);
AXCL_EXPORT AX_S32 AXCL_POOL_MmapPool(AX_POOL PoolId);
AXCL_EXPORT AX_S32 AXCL_POOL_MunmapPool(AX_POOL PoolId);
AXCL_EXPORT AX_VOID *AXCL_POOL_GetBlockVirAddr(AX_BLK BlockId);
AXCL_EXPORT AX_VOID *AXCL_POOL_GetMetaVirAddr(AX_BLK BlockId);
AXCL_EXPORT AX_S32 AXCL_POOL_IncreaseRefCnt(AX_BLK BlockId);
AXCL_EXPORT AX_S32 AXCL_POOL_DecreaseRefCnt(AX_BLK BlockId);

/* PTS API */
AXCL_EXPORT AX_S32 AXCL_SYS_GetCurPTS(AX_U64 *pu64CurPTS);
AXCL_EXPORT AX_S32 AXCL_SYS_InitPTSBase(AX_U64 u64PTSBase);
AXCL_EXPORT AX_S32 AXCL_SYS_SyncPTS(AX_U64 u64PTSBase);

/* GET CHIP TYPE API */
AXCL_EXPORT AX_CHIP_TYPE_E AXCL_SYS_GetChipType(AX_VOID);

/* LOG API */
AXCL_EXPORT AX_S32 AXCL_SYS_SetLogLevel(AX_LOG_LEVEL_E target);
AXCL_EXPORT AX_S32 AXCL_SYS_SetLogTarget(AX_LOG_TARGET_E target);
AXCL_EXPORT AX_S32 AXCL_SYS_EnableTimestamp(AX_BOOL enable);

/* PM API */
AXCL_EXPORT AX_S32 AXCL_SYS_Sleep(AX_VOID);
AXCL_EXPORT AX_S32 AXCL_SYS_WakeLock(const AX_MOD_ID_E ModId);
AXCL_EXPORT AX_S32 AXCL_SYS_WakeUnlock(const AX_MOD_ID_E ModId);
AXCL_EXPORT AX_S32 AXCL_SYS_RegisterEventCb(const AX_MOD_ID_E ModId, NotifyEventCallback pFunction, AX_VOID *pData);
AXCL_EXPORT AX_S32 AXCL_SYS_UnregisterEventCb(const AX_MOD_ID_E ModId);

#ifdef __cplusplus
}
#endif

#endif /* __EXTERNAL_AXCL_MM_SYS_H__ */
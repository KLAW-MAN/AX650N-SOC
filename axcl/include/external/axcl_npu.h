/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef __AXCL_ENGINE_API_H__
#define __AXCL_ENGINE_API_H__

#include "axcl_npu_type.h"

#ifdef __cplusplus
extern "C" {
#endif

AXCL_EXPORT const AX_CHAR* AXCL_ENGINE_GetVersion(AX_VOID);

AXCL_EXPORT AX_VOID AXCL_ENGINE_NPUReset(AX_VOID);

AXCL_EXPORT AX_S32 AXCL_ENGINE_Init(AX_ENGINE_NPU_ATTR_T* pNpuAttr);
AXCL_EXPORT AX_S32 AXCL_ENGINE_GetVNPUAttr(AX_ENGINE_NPU_ATTR_T* pNpuAttr);
AXCL_EXPORT AX_S32 AXCL_ENGINE_Deinit(AX_VOID);

AXCL_EXPORT AX_S32 AXCL_ENGINE_GetModelType(const AX_VOID* pData, AX_U32 nDataSize, AX_ENGINE_MODEL_TYPE_T* pModelType);

AXCL_EXPORT AX_S32 AXCL_ENGINE_CreateHandle(AX_ENGINE_HANDLE* pHandle, const AX_VOID* pData, AX_U32 nDataSize);
AXCL_EXPORT AX_S32 AXCL_ENGINE_CreateHandleV2(AX_ENGINE_HANDLE* pHandle, const AX_VOID* pData, AX_U32 nDataSize, AX_ENGINE_HANDLE_EXTRA_T* pExtraParam);
AXCL_EXPORT AX_S32 AXCL_ENGINE_DestroyHandle(AX_ENGINE_HANDLE nHandle);

AXCL_EXPORT AX_S32 AXCL_ENGINE_GetIOInfo(AX_ENGINE_HANDLE nHandle, AX_ENGINE_IO_INFO_T** pIO);
AXCL_EXPORT AX_S32 AXCL_ENGINE_GetGroupIOInfoCount(AX_ENGINE_HANDLE nHandle, AX_U32* pCount);
AXCL_EXPORT AX_S32 AXCL_ENGINE_GetGroupIOInfo(AX_ENGINE_HANDLE nHandle, AX_U32 nIndex, AX_ENGINE_IO_INFO_T** pIO);

AXCL_EXPORT AX_S32 AXCL_ENGINE_GetHandleModelType(AX_ENGINE_HANDLE nHandle, AX_ENGINE_MODEL_TYPE_T* pModelType);

AXCL_EXPORT AX_S32 AXCL_ENGINE_CreateContext(AX_ENGINE_HANDLE handle);
AXCL_EXPORT AX_S32 AXCL_ENGINE_CreateContextV2(AX_ENGINE_HANDLE nHandle, AX_ENGINE_CONTEXT_T* pContext);

AXCL_EXPORT AX_S32 AXCL_ENGINE_RunSync(AX_ENGINE_HANDLE handle, AX_ENGINE_IO_T* pIO);
AXCL_EXPORT AX_S32 AXCL_ENGINE_RunSyncV2(AX_ENGINE_HANDLE handle, AX_ENGINE_CONTEXT_T context, AX_ENGINE_IO_T* pIO);
AXCL_EXPORT AX_S32 AXCL_ENGINE_RunGroupIOSync(AX_ENGINE_HANDLE handle, AX_ENGINE_CONTEXT_T context, AX_U32 nIndex, AX_ENGINE_IO_T* pIO);

AXCL_EXPORT AX_S32 AXCL_ENGINE_SetAffinity(AX_ENGINE_HANDLE nHandle, AX_ENGINE_NPU_SET_T nNpuSet);
AXCL_EXPORT AX_S32 AXCL_ENGINE_GetAffinity(AX_ENGINE_HANDLE nHandle, AX_ENGINE_NPU_SET_T* pNpuSet);

AXCL_EXPORT AX_S32 AXCL_ENGINE_GetCMMUsage(AX_ENGINE_HANDLE nHandle, AX_ENGINE_CMM_INFO* pCMMInfo);

AXCL_EXPORT const AX_CHAR* AXCL_ENGINE_GetModelToolsVersion(AX_ENGINE_HANDLE nHandle);

#ifdef __cplusplus
}
#endif

#endif /* __AXCL_ENGINE_API_H__ */
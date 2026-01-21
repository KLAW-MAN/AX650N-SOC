/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef __AXCL_DSP_API_H__
#define __AXCL_DSP_API_H__

#include "axcl_dsp_type.h"

#ifdef __cplusplus
extern "C" {
#endif

AXCL_EXPORT AX_S32 AXCL_DSP_PowerOn(AX_DSP_ID_E enDspId);
AXCL_EXPORT AX_S32 AXCL_DSP_PowerOff(AX_DSP_ID_E enDspId);
AXCL_EXPORT AX_S32 AXCL_DSP_LoadBin(AX_DSP_ID_E enDspId, const char *pszBinFileName, AX_DSP_MEM_TYPE_E enMemType);
AXCL_EXPORT AX_S32 AXCL_DSP_EnableCore(AX_DSP_ID_E enDspId);
AXCL_EXPORT AX_S32 AXCL_DSP_DisableCore(AX_DSP_ID_E enDspId);
AXCL_EXPORT AX_S32 AXCL_DSP_PRC(AX_DSP_HANDLE *phHandle, const AX_DSP_MESSAGE_T *pstMsg, AX_DSP_ID_E enDspId, AX_DSP_PRI_E enPri);
AXCL_EXPORT AX_S32 AXCL_DSP_Query(AX_DSP_ID_E enDspId, AX_DSP_HANDLE hHandle, AX_DSP_MESSAGE_T *msg, AX_BOOL bBlock);

#ifdef __cplusplus
}
#endif

#endif /* __AXCL_DSP_API_H__ */

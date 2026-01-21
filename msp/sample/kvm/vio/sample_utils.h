/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#pragma once

#include "ax_base_type.h"
#include "ax_global_type.h"

AX_BOOL SAMPLE_UTILS_SetSchedPolicy(AX_S32 nPolicy, AX_S32 nPriority);
AX_VOID SAMPLE_UTILS_SetPthreadName(AX_CHAR *name);
AX_U64 SAMPLE_UTILS_GetTickUs(AX_VOID);
AX_VOID SAMPLE_UTILS_IncRef(const AX_VIDEO_FRAME_T* pstVideoFrame);
AX_VOID SAMPLE_UTILS_DecRef(const AX_VIDEO_FRAME_T* pstVideoFrame);
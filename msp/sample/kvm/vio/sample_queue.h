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

#define RING_SAZIE  (4U)
#define RING_MASK   (RING_SAZIE - 1)

typedef struct APP_VIDEO_FRAME_S
{
    AX_VIDEO_FRAME_T stVideoFrame;
}APP_VIDEO_FRAME_T;

typedef struct APP_QUEUE_S
{
    AX_S32 nFront;
    AX_S32 nRear;
    AX_U32 nSize;
    APP_VIDEO_FRAME_T* pBuffer;
}APP_QUEUE_T;


AX_BOOL SAMPLE_QUEUE_Init(AX_U32 nDepth);
AX_BOOL SAMPLE_QUEUE_Push(const APP_VIDEO_FRAME_T* pstVideoFrame);
AX_BOOL SAMPLE_QUEUE_Pop(APP_VIDEO_FRAME_T* pstVideoFrame);
AX_VOID SAMPLE_QUEUE_Clear();
AX_VOID SAMPLE_QUEUE_DeInit();

/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include <stddef.h>
#include <stdlib.h>
#include "ax_sys_api.h"
#include "sample_queue.h"
#include "sample_utils.h"
#include "sample_log.h"


APP_VIDEO_FRAME_T g_stAppVideoFrame;

static APP_QUEUE_T g_stAppQueue = {0, 0, 0, NULL};

static AX_BOOL isFull()
{
    return g_stAppQueue.nRear == ((g_stAppQueue.nFront + 1) & RING_MASK);
}

static AX_BOOL isEmpty()
{
    return g_stAppQueue.nFront == g_stAppQueue.nRear;
}

AX_BOOL SAMPLE_QUEUE_Init(AX_U32 nDepth)
{
    g_stAppQueue.pBuffer = malloc(sizeof(APP_VIDEO_FRAME_T)*nDepth);
    if (g_stAppQueue.pBuffer == NULL)
    {
        return AX_FALSE;
    }
    return AX_TRUE;
}

AX_BOOL SAMPLE_QUEUE_Push(const APP_VIDEO_FRAME_T* pstVideoFrame)
{
    if (isFull())
    {
        return AX_FALSE;
    }
    g_stAppQueue.pBuffer[g_stAppQueue.nFront] = *pstVideoFrame;
    __sync_synchronize();
    g_stAppQueue.nFront = (g_stAppQueue.nFront + 1) & RING_MASK;
    return AX_TRUE;
}

AX_BOOL SAMPLE_QUEUE_Pop(APP_VIDEO_FRAME_T* pstVideoFrame)
{
    AX_BOOL bIsEmpty = isEmpty();
    if (bIsEmpty)
    {
        return AX_FALSE;
    }
    *pstVideoFrame = g_stAppQueue.pBuffer[g_stAppQueue.nRear];
    __sync_synchronize();
    g_stAppQueue.nRear = (g_stAppQueue.nRear + 1) & RING_MASK;
    return AX_TRUE;
}

AX_VOID SAMPLE_QUEUE_Clear()
{
    APP_VIDEO_FRAME_T stVideoFrame;
    while (SAMPLE_QUEUE_Pop(&stVideoFrame))
    {
         SAMPLE_UTILS_DecRef(&stVideoFrame.stVideoFrame);
    }
    __sync_synchronize();
    g_stAppQueue.nRear  = 0;
    g_stAppQueue.nFront = 0;
    g_stAppQueue.nSize  = 0;
}

AX_VOID SAMPLE_QUEUE_DeInit()
{
    if (g_stAppQueue.pBuffer)
    {
        free(g_stAppQueue.pBuffer);
        g_stAppQueue.pBuffer = NULL;
    }
    g_stAppQueue.nRear  = 0;
    g_stAppQueue.nFront = 0;
    g_stAppQueue.nSize  = 0;
}

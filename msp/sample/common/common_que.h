/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef __COMMON_QUE_H__
#define __COMMON_QUE_H__

#include "ax_vin_api.h"
#include "common_type.h"
#include <stdio.h>
#include <pthread.h>

#define    QUEUE_SIZE      (8)

typedef struct {
    AX_IMG_INFO_T imgInfo[QUEUE_SIZE];
    AX_S32 flashFramePipeId;
    AX_S32 head;
    AX_S32 tail;
    pthread_mutex_t lock;
    pthread_cond_t not_empty;
    pthread_cond_t not_full;
} Queue;

void COMMON_QUEUE_Init(Queue *q);
int COMMON_QUEUE_Push(Queue *q, AX_S32 flashFramePipeId, AX_IMG_INFO_T *pImgInfo);
int COMMON_QUEUE_Pop(Queue *q, AX_S32 *nFlashFramePipeId, AX_IMG_INFO_T *pImgInfo);
void COMMON_QUEUE_Destroy(Queue *q);

#endif
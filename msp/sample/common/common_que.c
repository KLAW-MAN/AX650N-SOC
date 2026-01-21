/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include "common_que.h"
#include "common_cam.h"
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include "ax_sys_api.h"

static struct timespec pop_outtime;
static struct timespec push_outtime;

void COMMON_QUEUE_Init(Queue *q)
{
    q->head = 0;
    q->tail = 0;
    pthread_mutex_init(&q->lock, NULL);
    pthread_cond_init(&q->not_empty, NULL);
    pthread_cond_init(&q->not_full, NULL);
}

int COMMON_QUEUE_Push(Queue *q, AX_S32 flashFramePipeId,  AX_IMG_INFO_T *pImgInfo)
{
    int ret = 0;
    clock_gettime(CLOCK_REALTIME, &push_outtime);
    push_outtime.tv_sec += 2;

    pthread_mutex_lock(&q->lock);
    /* is full */
    while ((q->tail + 1) % QUEUE_SIZE == q->head) {
        ret = pthread_cond_timedwait(&q->not_full, &q->lock, &push_outtime);
        if (ret) {
            if (ETIMEDOUT == ret) {
                pthread_mutex_unlock(&q->lock);
                return ret;
            }
        }
    }

    memcpy(&(q->imgInfo[q->tail]), pImgInfo, sizeof(AX_IMG_INFO_T));
    q->flashFramePipeId = flashFramePipeId;
    q->tail = (q->tail + 1) % QUEUE_SIZE;
    pthread_cond_signal(&q->not_empty);
    pthread_mutex_unlock(&q->lock);

    return ret;
}

int COMMON_QUEUE_Pop(Queue *q, AX_S32 *nFlashFramePipeId, AX_IMG_INFO_T *pImgInfo)
{
    int ret = 0;
    clock_gettime(CLOCK_REALTIME, &pop_outtime);
    pop_outtime.tv_sec += 2;

    pthread_mutex_lock(&q->lock);
    /* is empty */
    while (q->head == q->tail) {
        ret = pthread_cond_timedwait(&q->not_empty, &q->lock, &pop_outtime);
        if (ret) {
            if (ETIMEDOUT == ret) {
                pthread_mutex_unlock(&q->lock);
                return ret;
            }
        }
    }

    memcpy(pImgInfo, &(q->imgInfo[q->head]), sizeof(AX_IMG_INFO_T));
    *nFlashFramePipeId = q->flashFramePipeId;
    q->head = (q->head + 1) % QUEUE_SIZE;
    pthread_cond_signal(&q->not_full);
    pthread_mutex_unlock(&q->lock);

    return ret;
}

void COMMON_QUEUE_Destroy(Queue *q)
{
    AX_IMG_INFO_T pImgInfo={0};
    while (q->head != q->tail) {
        memcpy(&pImgInfo, &(q->imgInfo[q->head]), sizeof(AX_IMG_INFO_T));
        AX_POOL_DecreaseRefCnt(pImgInfo.tFrameInfo.stVFrame.u32BlkId[0]);
        q->head = (q->head + 1) % QUEUE_SIZE;
    }
    pthread_mutex_destroy(&q->lock);
    pthread_cond_destroy(&q->not_empty);
    pthread_cond_destroy(&q->not_full);
}


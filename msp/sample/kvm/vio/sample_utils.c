/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include <sys/time.h>
#include <time.h>
#include <stdlib.h>
#include <sys/prctl.h>
#include <pthread.h>
#include <errno.h>

#include "ax_sys_api.h"
#include "ax_pool_type.h"
#include "sample_utils.h"
#include "sample_log.h"


AX_BOOL SAMPLE_UTILS_SetSchedPolicy(AX_S32 nPolicy, AX_S32 nPriority)
{
    if (!(SCHED_FIFO == nPolicy || SCHED_RR == nPolicy))
    {
        return  AX_FALSE;
    }
    struct sched_param sch;
    AX_S32 policy;
    pthread_getschedparam(pthread_self(), &policy, &sch);
    AX_S32 nMin = sched_get_priority_min(nPolicy);
    AX_S32 nMax = sched_get_priority_max(nPolicy);
    if (nPriority < nMin || nPriority > nMax)
    {
        KVM_LOG_E("priority %d is out of range for thread policy %d is [%d - %d]", nPriority, nPolicy, nMin, nMax);
        return AX_FALSE;
    }

    sch.sched_priority = nPriority;
    if (0 != pthread_setschedparam(pthread_self(), nPolicy, &sch))
    {
        KVM_LOG_E("set thread sched to policy %d priority %d fail, %s", nPolicy, nPriority, strerror(errno));
        return AX_FALSE;
    }

    return AX_TRUE;
}


AX_VOID SAMPLE_UTILS_SetPthreadName(AX_CHAR *name)
{
    if (name != NULL)
    {
        prctl(PR_SET_NAME, name);
    }
}

AX_U64 SAMPLE_UTILS_GetTickUs(AX_VOID)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (ts.tv_sec * 1000000 + ts.tv_nsec / 1000);
}

AX_VOID SAMPLE_UTILS_IncRef(const AX_VIDEO_FRAME_T* pstVideoFrame)
{
    if (AX_INVALID_BLOCKID != pstVideoFrame->u32BlkId[0])
    {
        AX_S32 ret = AX_POOL_IncreaseRefCnt(pstVideoFrame->u32BlkId[0]);
        if (0 != ret)
        {
            KVM_LOG_E("AX_POOL_IncreaseRefCnt fail, u32BlkId = 0x%x\n", pstVideoFrame->u32BlkId[0]);
            exit(-1);
        }
    }
}

AX_VOID SAMPLE_UTILS_DecRef(const AX_VIDEO_FRAME_T* pstVideoFrame)
{
    if (AX_INVALID_BLOCKID != pstVideoFrame->u32BlkId[0])
    {
        AX_S32 ret = AX_POOL_DecreaseRefCnt(pstVideoFrame->u32BlkId[0]);
        if (0 != ret)
        {
            KVM_LOG_E("AX_POOL_DecreaseRefCnt fail, u32BlkId = 0x%x\n", pstVideoFrame->u32BlkId[0]);
            exit(-1);
        }
    }
}
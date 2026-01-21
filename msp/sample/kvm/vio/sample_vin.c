/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include "sample_log.h"
#include "sample_uxe.h"
#include "sample_vin.h"
#include "sample_queue.h"
#include "sample_utils.h"

static pthread_t g_tid;
static AX_BOOL g_bThreadStart = AX_FALSE;
static AX_U32 g_u32Width = 0;
static AX_U32 g_u32Height = 0;
static AX_U32 g_nMaxIntervalTime = 0;

static AX_VOID* SAMPLE_VIN_444_ProcessThread(AX_VOID* arg)
{
    KVM_LOG_I("+++ threadId = %lu\n", pthread_self());
    AX_U8 nDevId = SAMPLE_UXE_GetDevID();
    AX_SNS_HDR_FRAME_E eSnsFrame = AX_SNS_HDR_FRAME_L;
    AX_IMG_INFO_T imgInfo;
    memset(&imgInfo, 0, sizeof(imgInfo));
    AX_U64 nSeqNum = 0;
    AX_U64 nLastTime = 0;
    AX_U64 nCurrentTime = 0;

    SAMPLE_UTILS_SetPthreadName("VinThread");
    while (g_bThreadStart)
	{
        AX_S32 ret;
        ret = AX_VIN_GetDevFrame(nDevId, eSnsFrame, &imgInfo, 100);
        if (0 != ret)
        {
            KVM_LOG_E("AX_VIN_GetDevFrame(dev %d) fail, ret = 0x%x\n", nDevId, ret);
            continue;
        }

        if (nSeqNum == 0)
        {
            AX_U64 nTime = SAMPLE_UTILS_GetTickUs();
            KVM_LOG_I("vin first frame SeqNum = %llu, time = %llu\n", imgInfo.tFrameInfo.stVFrame.u64SeqNum, nTime);
            nSeqNum = imgInfo.tFrameInfo.stVFrame.u64SeqNum + 1;
        }
        else
        {
            if (nSeqNum != imgInfo.tFrameInfo.stVFrame.u64SeqNum)
            {
                KVM_LOG_I("from vin frame reverse order appears lastSeqNum = %llu, currentSeqNum = %llu\n", nSeqNum - 1, imgInfo.tFrameInfo.stVFrame.u64SeqNum);
            }
            nSeqNum = imgInfo.tFrameInfo.stVFrame.u64SeqNum + 1;
        }

        imgInfo.tFrameInfo.stVFrame.u64UserData = 0;

        APP_VIDEO_FRAME_T stAppVideoFrame =
        {
            .stVideoFrame = imgInfo.tFrameInfo.stVFrame,
        };

        if (imgInfo.tFrameInfo.stVFrame.u64SeqNum == 0)
        {
            nLastTime = SAMPLE_UTILS_GetTickUs();
        }
        else
        {
            nCurrentTime = SAMPLE_UTILS_GetTickUs();
            AX_U32 nDifferenceTime =  nCurrentTime - nLastTime;
            if ((nDifferenceTime > (g_nMaxIntervalTime + 500)) || (nDifferenceTime < (g_nMaxIntervalTime - 500)))
            {
                KVM_LOG_I("AX_VIN_GetDevFrame abnormal interval time, nSeqNum = %llu, time = %u(%llu, %llu)\n", imgInfo.tFrameInfo.stVFrame.u64SeqNum, nDifferenceTime, nCurrentTime, nLastTime);
            }
            nLastTime = nCurrentTime;
        }

        stAppVideoFrame.stVideoFrame.enImgFormat = AX_FORMAT_BGR888;
        stAppVideoFrame.stVideoFrame.u32Width = g_u32Width;
        stAppVideoFrame.stVideoFrame.u32Height = g_u32Height;
        stAppVideoFrame.stVideoFrame.u32PicStride[0] = stAppVideoFrame.stVideoFrame.u32Width*3;
        stAppVideoFrame.stVideoFrame.u32PicStride[1] = stAppVideoFrame.stVideoFrame.u32Width*3;
        //printf("SAMPLE_QUEUE_Push nSeqNum = %llu, time = %llu\n", stAppVideoFrame.nSeqNum, SAMPLE_UTILS_GetTickUs());
        SAMPLE_UTILS_IncRef(&stAppVideoFrame.stVideoFrame);
        AX_BOOL bRet = SAMPLE_QUEUE_Push(&stAppVideoFrame);
        if (!bRet)
        {
            KVM_LOG_E("SAMPLE_QUEUE_Push fail\n");
            SAMPLE_UTILS_DecRef(&stAppVideoFrame.stVideoFrame);
        }
        ret = AX_VIN_ReleaseDevFrame(nDevId, eSnsFrame, &imgInfo);
        if (0 != ret)
        {
            KVM_LOG_E("AX_VIN_ReleaseDevFrame(dev %d) fail, ret = 0x%x\n", nDevId, ret);
            return NULL;
        }
	}
    KVM_LOG_I("---\n");
    return NULL;
}

AX_BOOL SAMPLE_VIN_Init(HDMI_IN_TYPE_E enHdmi, AX_U32 nW, AX_U32 nH, AX_U8 nFps, AX_U16 nMHz)
{
    KVM_LOG_I("+++\n");
    g_nMaxIntervalTime = 1000000/nFps;
    AX_BOOL bRet = AX_FALSE;
    AX_IMG_FORMAT_E enVinFmt = AX_FORMAT_BAYER_RAW_8BPP;
    switch (enHdmi)
    {
    case HDMI_IN_YUV422_8BIT:
        enVinFmt = AX_FORMAT_YUV422_INTERLEAVED_YUYV;
        break;
    case HDMI_IN_YUV422_10BIT:
        enVinFmt = AX_FORMAT_YUV422_SEMIPLANAR_10BIT_P010;
        break;
    case HDMI_IN_YUV444_RGB888:
        enVinFmt = AX_FORMAT_BAYER_RAW_8BPP;
        break;
    default:
        break;
    }

    bRet = SAMPLE_UXE_Init(enVinFmt, nW, nH, nFps, nMHz);
    if (!bRet)
    {
        KVM_LOG_E("SAMPLE_UXE_Init failed\n");
        return bRet;
    }
    g_u32Width  = nW;
    g_u32Height = nH;
    KVM_LOG_I("---\n");
    return AX_TRUE;
}

AX_BOOL SAMPLE_VIN_Start()
{
    KVM_LOG_I("+++\n");
    AX_BOOL bRet = AX_FALSE;
    bRet = SAMPLE_UXE_Start();
    if (!bRet)
    {
        KVM_LOG_E("SAMPLE_UXE_Start failed\n");
        return bRet;
    }

    g_bThreadStart = AX_TRUE;
    AX_S32 nRet = pthread_create(&g_tid, NULL, SAMPLE_VIN_444_ProcessThread, NULL);
    if (nRet < 0)
    {
        KVM_LOG_E("pthread_create fail\n");
        return AX_FALSE;
    }

    KVM_LOG_I("---\n");
    return AX_TRUE;
}

AX_BOOL SAMPLE_VIN_Stop()
{
    KVM_LOG_I("+++\n");
    AX_BOOL bRet = AX_FALSE;
    g_bThreadStart = AX_FALSE;
    if (pthread_join(g_tid, NULL) != 0)
    {
        KVM_LOG_E("pthread_join fail\n");
        return AX_FALSE;
    }
    bRet = SAMPLE_UXE_Stop();
    if (!bRet)
    {
        KVM_LOG_E("SAMPLE_UXE_Stop fail\n");
        return bRet;
    }
    KVM_LOG_I("---\n");
    return AX_TRUE;
}

AX_BOOL SAMPLE_VIN_DeInit()
{
    KVM_LOG_I("+++\n");
    AX_BOOL bRet = AX_FALSE;
    bRet = SAMPLE_UXE_DeInit();
    if (!bRet)
    {
        KVM_LOG_E("SAMPLE_UXE_DeInit fail\n");
        return bRet;
    }
    KVM_LOG_I("---\n");
    return AX_TRUE;
}
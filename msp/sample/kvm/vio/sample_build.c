/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include <string.h>
#include "ax_sys_api.h"
#include "ax_buffer_tool.h"
#include "sample_vin.h"
#include "sample_vo.h"
#include "sample_uxe_gpio.h"
#include "sample_queue.h"
#include "sample_build.h"
#include "sample_log.h"


static AX_BOOL PoolInit(HDMI_IN_TYPE_E enHdmi, AX_U32 nW, AX_U32 nH)
{
    AX_S32 ret;
    ret = AX_POOL_Exit();
    if (0 != ret)
    {
        KVM_LOG_E("AX_POOL_Exit() fail, ret = 0x%x\n", ret);
        return ret;
    }

    /* allocate common pool for VIN */
    AX_POOL_FLOORPLAN_T plan;
    memset(&plan, 0, sizeof(plan));
    AX_FRAME_COMPRESS_INFO_T fbc = {.enCompressMode = AX_COMPRESS_MODE_LOSSLESS, .u32CompressLevel = 0};

    AX_IMG_FORMAT_E enImgFmt = AX_FORMAT_YUV420_SEMIPLANAR;
    AX_U32 nStride = 0;
    switch (enHdmi)
    {
    case HDMI_IN_YUV422_8BIT:
        nStride = nW;
        enImgFmt = AX_FORMAT_YUV420_SEMIPLANAR;
        break;
    case HDMI_IN_YUV422_10BIT: /* NV12 P010 */
        nStride = nW * 2;
        enImgFmt = AX_FORMAT_YUV420_SEMIPLANAR_10BIT_P010;
        break;
    case HDMI_IN_YUV444_RGB888:
        nStride = nW * 3;
        enImgFmt = AX_FORMAT_RGB888;
        break;
    default:
        KVM_LOG_E("enHdmi fail, enHdmi = %d\n", enHdmi);
        return AX_FALSE;
        break;
    }

    plan.CommPool[0].MetaSize = 4096;
    plan.CommPool[0].BlkSize = AX_VIN_GetImgBufferSize(nH, nStride, enImgFmt, &fbc, 0);
    plan.CommPool[0].BlkCnt = 8;  // fixme
    plan.CommPool[0].CacheMode = POOL_CACHE_MODE_NONCACHE;
    ret = AX_POOL_SetConfig(&plan);
    if (0 != ret)
    {
        KVM_LOG_E("AX_POOL_SetConfig() fail, ret = 0x%x\n", ret);
        return AX_FALSE;
    }

    ret = AX_POOL_Init();
    if (0 != ret)
    {
        KVM_LOG_E("AX_POOL_Init() fail ret = 0x%x\n", ret);
        return AX_FALSE;
    }
    return AX_TRUE;
}

AX_BOOL SAMPLE_BUILD_Init(HDMI_IN_TYPE_E enHdmiInFormat, AX_U32 nWidth, AX_U32 nHeight, AX_U8 nFps, AX_U16 nMHz, AX_BOOL bShowFb, AX_U8 nLogLevel)
{
    KVM_LOG_I("+++\n");
    AX_BOOL bRet;

    AX_S32 ret = AX_SYS_Init();
    if (0 != ret)
    {
        KVM_LOG_E("AX_SYS_Init() fail, ret = 0x%x\n", ret);
        return ret;
    }
    // bRet = PoolInit(enHdmiInFormat, nWidth, nHeight);
    bRet = PoolInit(HDMI_IN_YUV444_RGB888, 3840, 2160);
    if (!bRet)
    {
        KVM_LOG_E("PoolInit fail\n");
        return bRet;
    }

    bRet = SAMPLE_QUEUE_Init(RING_SAZIE);
    if (!bRet)
    {
        KVM_LOG_E("SAMPLE_QUEUE_Init fail\n");
        return bRet;
    }


    SAMPLE_QUEUE_Clear();

    bRet = SAMPLE_VIN_Init(enHdmiInFormat, nWidth, nHeight, nFps, nMHz);
    if (!bRet)
    {
        KVM_LOG_E("SAMPLE_VIN_Init fail\n");
        return bRet;
    }

    AX_BOOL bYuv422_10bit = AX_FALSE;

    if (enHdmiInFormat == HDMI_IN_YUV422_10BIT)
    {
        bYuv422_10bit = AX_TRUE;
    }

    bRet = SAMPLE_VO_Init(nWidth, nHeight, nFps, bShowFb, bYuv422_10bit);
    if (!bRet)
    {
        KVM_LOG_E("SAMPLE_VO_Init fail\n");
        return bRet;
    }

    bRet = SAMPLE_UXE_GPIO_Init(enHdmiInFormat, nWidth, nHeight, nFps, bShowFb, nLogLevel);
    if (!bRet)
    {
        KVM_LOG_E("SAMPLE_VO_Init fail\n");
        return bRet;
    }

    KVM_LOG_I("---\n");
    return AX_TRUE;
}

AX_BOOL SAMPLE_BUILD_Start()
{
    KVM_LOG_I("+++\n");
    AX_BOOL bRet;

    bRet = SAMPLE_UXE_GPIO_Start();
    if (!bRet)
    {
        KVM_LOG_E("SAMPLE_UXE_GPIO_Start fail\n");
        return bRet;
    }

    bRet = SAMPLE_VIN_Start();
    if (!bRet)
    {
        KVM_LOG_E("SAMPLE_VIN_Start fail\n");
        return bRet;
    }

    bRet = SAMPLE_VO_Start();
    if (!bRet)
    {
        KVM_LOG_E("SAMPLE_VO_Start fail\n");
        return bRet;
    }
    KVM_LOG_I("---\n");
    return AX_TRUE;
}

AX_BOOL SAMPLE_BUILD_ReStart(HDMI_IN_TYPE_E enHdmiInFormat, AX_U32 nWidth, AX_U32 nHeight, AX_U8 nFps, AX_U16 nMHz, AX_BOOL bShowFb, AX_U8 nLogLevel)
{
    KVM_LOG_I("+++\n");
    AX_BOOL bRet;

    bRet = SAMPLE_VIN_Stop();
    if (!bRet)
    {
         KVM_LOG_E("SAMPLE_VIN_Stop fail\n");
         return bRet;
    }

    bRet = SAMPLE_VO_Stop();
    if (!bRet)
    {
         KVM_LOG_E("SAMPLE_VO_Stop fail\n");
         return bRet;
    }

    SAMPLE_QUEUE_Clear();

    bRet = SAMPLE_VIN_Init(enHdmiInFormat, nWidth, nHeight, nFps, nMHz);
    if (!bRet)
    {
        KVM_LOG_E("SAMPLE_VIN_Init fail\n");
        return bRet;
    }

    AX_BOOL bYuv422_10bit = AX_FALSE;

    if (enHdmiInFormat == HDMI_IN_YUV422_10BIT)
    {
        bYuv422_10bit = AX_TRUE;
    }

    bRet = SAMPLE_VO_Init(nWidth, nHeight, nFps, bShowFb, bYuv422_10bit);
    if (!bRet)
    {
        KVM_LOG_E("SAMPLE_VO_Init fail\n");
        return bRet;
    }

    bRet = SAMPLE_VIN_Start();
    if (!bRet)
    {
        KVM_LOG_E("SAMPLE_VIN_Start fail\n");
        return bRet;
    }

    bRet = SAMPLE_VO_Start();
    if (!bRet)
    {
        KVM_LOG_E("SAMPLE_VO_Start fail\n");
        return bRet;
    }

    return AX_TRUE;
    KVM_LOG_I("---\n");
}

AX_BOOL SAMPLE_BUILD_Stop()
{
    KVM_LOG_I("+++\n");
    AX_BOOL bRet;

    bRet = SAMPLE_UXE_GPIO_Stop();
    if (!bRet)
    {
        KVM_LOG_E("SAMPLE_UXE_GPIO_Stop fail\n");
        return bRet;
    }

    bRet = SAMPLE_VIN_Stop();
    if (!bRet)
    {
         KVM_LOG_E("SAMPLE_VIN_Stop fail\n");
         return bRet;
    }

    bRet = SAMPLE_VO_Stop();
    if (!bRet)
    {
         KVM_LOG_E("SAMPLE_VO_Stop fail\n");
         return bRet;
    }

    KVM_LOG_I("---\n");
    return AX_TRUE;
}

AX_BOOL SAMPLE_BUILD_DeInit()
{
    AX_BOOL bRet;

    bRet = SAMPLE_UXE_GPIO_DeInit();
    if (!bRet)
    {
        KVM_LOG_E("SAMPLE_UXE_GPIO_DeInit fail\n");
        return bRet;
    }

    bRet = SAMPLE_VIN_DeInit();
    if (!bRet)
    {
        KVM_LOG_E("SAMPLE_VIN_DeInit fail\n");
        return bRet;
    }

    bRet = SAMPLE_VO_DeInit();
    if (!bRet)
    {
        KVM_LOG_E("SAMPLE_VO_DoInit fail\n");
        return bRet;
    }

    AX_S32 ret = AX_POOL_Exit();
    if (0 != ret)
    {
        KVM_LOG_E("AX_POOL_Exit fail, ret = 0x%x\n", ret);
        return AX_FALSE;
    }

    if (0 != AX_SYS_Deinit())
    {
        KVM_LOG_E("AX_SYS_Deinit fail\n");
        return bRet;
    }

    SAMPLE_QUEUE_DeInit();
    return AX_TRUE;
}

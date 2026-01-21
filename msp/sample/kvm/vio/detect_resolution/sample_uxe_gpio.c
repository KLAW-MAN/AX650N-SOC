/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <poll.h>
#include <errno.h>
#include <semaphore.h>
#include <math.h>

#include "sample_utils.h"
#include "sample_log.h"
#include "sample_uxe_gpio.h"
#include "sample_build.h"
#include "queryUXE.h"
#include "gpioHelper.h"

static sem_t g_sem;
typedef struct INIT_PARA_S
{
    HDMI_IN_TYPE_E enHdmiInFormat;
    AX_U32 nWidth;
    AX_U32 nHeight;
    AX_U8 nFps;
    AX_U16 nMHz;
    AX_BOOL bShowFb;
    AX_U8 nLogLevel;
}INIT_PARA_T;

static INIT_PARA_T g_InitPara =
{
    .enHdmiInFormat = HDMI_IN_YUV444_RGB888,
    .nWidth = 3480,
    .nHeight = 2160,
    .nFps = 60,
    .bShowFb = AX_FALSE,
    .nLogLevel = 0
};
 typedef struct MIPI_FORMAT_MAP_S
{
    UXE_MIPI_FORMAT_E enMipiFmt;
    AX_CHAR* arrSupportFormat[3];
} MIPI_FORMAT_MAP_T;

static GPIO_HANDLE g_handle;

static AX_VOID RestartBuild()
{
    KVM_LOG_I("+++ threadId = %lu\n", pthread_self());

    AX_BOOL bRet = SAMPLE_BUILD_ReStart(g_InitPara.enHdmiInFormat,
                                        g_InitPara.nWidth,
                                        g_InitPara.nHeight,
                                        g_InitPara.nFps,
                                        g_InitPara.nMHz,
                                        g_InitPara.bShowFb,
                                        g_InitPara.nLogLevel);
    if (!bRet)
    {
        KVM_LOG_E("SAMPLE_BUILD_Init fail\n");
        return;
    }
    KVM_LOG_I("Restart ---\n");
}

AX_VOID SAMPLE_UXE_GPIO_WaitForDetectNotify(AX_BOOL bIsRunning)
{
    KVM_LOG_I("+++\n");
    sem_wait(&g_sem);
    if (!bIsRunning)
    {
        return;
    }
    RestartBuild();
    KVM_LOG_I("---\n");
}

AX_VOID SAMPLE_UXE_GPIO_Wakeup(AX_CHAR* reason)
{
    KVM_LOG_I("+++ reason = %s\n", reason);
    sem_post(&g_sem);
    KVM_LOG_I("---\n");
}

static AX_BOOL CheckFormat(UXE_MIPI_FORMAT_E enMipiFmt, AX_U32 nWidth, AX_U32 nHeight, AX_U8 nFps)
{
    AX_BOOL bFind = AX_FALSE;
    AX_CHAR arrHdmiFormat[16] = "";
    snprintf(arrHdmiFormat, 16, "%u_%u_%u", nWidth, nHeight, nFps);
    MIPI_FORMAT_MAP_T stFormatMat[] =
    {
        {UXE_MIPI_RGB_8BIT, {"3840_2160_30", "3840_2160_60", "1920_1080_60"}},
        {UXE_MIPI_YUV422_8BIT, {"", "", ""}},
        {UXE_MIPI_YUV422_10BIT, .arrSupportFormat = {"", "", ""}}
    };

    for (AX_U8 i = 0; i < sizeof(stFormatMat)/sizeof(stFormatMat[0]); i++)
    {
        if (stFormatMat[i].enMipiFmt != enMipiFmt)
        {
            continue;
        }

        for (AX_U8 j = 0; j < sizeof(stFormatMat[i].arrSupportFormat)/sizeof(stFormatMat[i].arrSupportFormat[0]); j++)
        {
            if (strstr(stFormatMat[i].arrSupportFormat[j], arrHdmiFormat))
            {
                bFind = AX_TRUE;
                break;
            }
        }
    }

    return bFind;
}

static AX_VOID GpioDetectCallbackFun(AX_U32 gpio, AX_U8 value, AX_VOID* userdata)
{
    UXE_SIGNAL_INFO_T info;

    AX_BOOL bSleep = AX_FALSE;
    do
    {
        AX_UXE_GetSignalInfo(&info);
        if (UXE_INTERRUPT_VIDEO_READY != info.intType)
        {
            return;
        }
        if (bSleep)
        {
            break;
        }
        usleep(200*1000);
        bSleep = AX_TRUE;
    } while (1);

    AX_F32 fFps = info.pixelClk * 1000.0 / (info.HTotal * info.VTotal);
    g_InitPara.nWidth  = info.HActive;
    g_InitPara.nHeight = info.VActive;
    g_InitPara.nFps    = (AX_U32)roundf(fFps);
    g_InitPara.nMHz    = info.byteClk * 8 / 1000;

    AX_BOOL bRet = CheckFormat(info.intType, g_InitPara.nWidth, g_InitPara.nHeight, g_InitPara.nFps);
    if (!bRet)
    {
        KVM_LOG_E("GpioDetect: unsupport nWidth = %u, nHeight = %u, fps = %u(%.2f), intType = %u, mipi data rate: %u MHz\n",
                  g_InitPara.nWidth,
                  g_InitPara.nHeight,
                  g_InitPara.nFps,
                  fFps,
                  info.intType,
                  g_InitPara.nMHz);
        return;
    }

    KVM_LOG_I("GpioDetect: nWidth = %u, nHeight = %u, fps = %u, intType = %u, mipi data rate: %u MHz\n",
              g_InitPara.nWidth,
              g_InitPara.nHeight,
              g_InitPara.nFps,
              info.intType,
              g_InitPara.nMHz);

    SAMPLE_UXE_GPIO_Wakeup("GpioDetect");
}

AX_BOOL SAMPLE_UXE_GPIO_Init(HDMI_IN_TYPE_E enHdmiInFormat, AX_U32 nWidth, AX_U32 nHeight, AX_U8 nFps, AX_BOOL bShowFb, AX_U8 nLogLevel)
{
    AX_S32 nRet;
    nRet = sem_init(&g_sem, 0, 0);
    if (nRet != 0)
    {
        KVM_LOG_E("sem_init fail\n");
        return AX_FALSE;
    }

    g_InitPara.enHdmiInFormat = enHdmiInFormat;
    g_InitPara.nWidth = nWidth;
    g_InitPara.nHeight = nHeight;
    g_InitPara.nFps = nFps;
    g_InitPara.bShowFb = bShowFb;
    g_InitPara.nLogLevel = nLogLevel;
    g_handle = CreateGpioHelper(66, GPIO_IN, GPIO_EDGE_FALLING);
    if (!g_handle)
    {
        KVM_LOG_E("CreateGpioHelper failed\n");
        return AX_FALSE;
    }
    return AX_TRUE;
}

AX_BOOL SAMPLE_UXE_GPIO_Start()
{
    KVM_LOG_I("---\n");
    //set pinmux of GPIO2_A2 to GPIO function
    system("ax_lookat 0x04250E30 -s 0x00060083");
    StartGpioPolling(g_handle, GpioDetectCallbackFun, 0);
    KVM_LOG_I("---\n");
    return AX_TRUE;
}
AX_BOOL SAMPLE_UXE_GPIO_Stop()
{
    KVM_LOG_I("+++\n");

    //SAMPLE_UXE_GPIO_Wakeup("stop");
    KVM_LOG_I("**************\n");
    StopGpioPolling(g_handle);
    KVM_LOG_I("---\n");
    return AX_TRUE;
}
AX_BOOL SAMPLE_UXE_GPIO_DeInit()
{
    KVM_LOG_I("+++\n");
    DestoryGpioHelper(g_handle);
    sem_destroy(&g_sem);
    KVM_LOG_I("---\n");
    return AX_TRUE;
}
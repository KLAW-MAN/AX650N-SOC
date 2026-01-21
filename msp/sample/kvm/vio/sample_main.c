/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include<getopt.h>
#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "sample_utils.h"
#include "sample_build.h"
#include "sample_uxe_gpio.h"
#include "sample_common.h"
#include "sample_log.h"


static volatile AX_BOOL isRunning = AX_TRUE;

static void exit_handler(int s)
{
    static AX_U32 exitCount = 0;
    printf("\n====================== Caught signal: %d ======================\n", s);
    printf("please waiting to quit ...\n\n");
    isRunning = AX_FALSE;
    SAMPLE_UXE_GPIO_Wakeup("stop");
    if (exitCount++ >= 3)
    {
        printf("\n======================      Force to exit    ====================== \n");
        _exit(1);
    }
}

static void ignore_sig_pipe(void)
{
    struct sigaction sa;
    sa.sa_handler = SIG_IGN;
    sa.sa_flags = 0;
    if (sigemptyset(&sa.sa_mask) == -1 || sigaction(SIGPIPE, &sa, 0) == -1)
    {
        perror("failed to ignore SIGPIPE, sigaction");
        exit(EXIT_FAILURE);
    }
}

static AX_VOID InstallSignalHandler(AX_VOID)
{
    signal(SIGINT, exit_handler);
    signal(SIGQUIT, exit_handler);
    ignore_sig_pipe();
}

static void showUsage()
{
    printf("Usage: %s [OPTION]...\n", "progname");
    printf(" Options:\n");
    printf(" -f          yuv422 8bit:0 ; yuv422 10bit: 1 ; rgb888: 2 ; default 2\n");
    printf(" -w          set width,  default 3840\n");
    printf(" -h          set Height, default 2160\n");
    printf(" --fps=60    set fps, default 60\n");
    printf(" -s          show fb, default false\n");
    printf(" -d          set log level, no use\n");
    printf(" -v          Print the version number and exit.\n");
    printf(" --help      Print this message and exit\n");
}

// sample_kvm_vio -f 2 -w 3840 -h 2160 --fps=60 -s -d
AX_S32 main(AX_S32 argc, AX_CHAR* argv[])
{
    static struct option longOptions[] =
    {
        {"fps", required_argument, NULL, 200},
        {"help", no_argument, NULL, 1000},
        { NULL, 0, NULL, 0 }
    };

    AX_CHAR* pShortOptions = "f:w:h:sd:v";

    AX_U8 nHdmiInFormat = 2; //yuv422 8bit:0 ; yuv422 10bit: 1 ; rgb888: 2
    AX_U32 nWidth  = 3840;
    AX_U32 nHeight = 2160;
    AX_U8 nFps = 60;
    AX_BOOL bShowFb = AX_FALSE;
    AX_U8 nLogLevel = 0;

    AX_S32 opt;
    AX_S32 optionIndex = 0;
    while ((opt = getopt_long(argc, argv, pShortOptions, longOptions, &optionIndex)) != -1)
    {
        switch (opt)
        {
        case 'f':
            nHdmiInFormat = atoi(optarg);
            break;
        case 'w':
            nWidth = atoi(optarg);
            break;
        case 'h':
            nHeight = atoi(optarg);
            break;
        case 's':
            bShowFb = AX_TRUE;
            break;
        case 'd':
            nLogLevel = atoi(optarg);
            break;
        case 200:
            nFps = atoi(optarg);
            break;
        case 'v':
            printf("1.0.0\n");
            exit(-1);
        case 1000:
        default:
            showUsage();
            exit(-1);
            break;
        }
    }

    KVM_LOG_I("mainThreadId = %lu, nHdmiInFormat = %u, nWidth = %u, nHeight = %u, nFps = %u, bShowFb = %u, nLogLevel = %u\n",
              pthread_self(),
              nHdmiInFormat, nWidth, nHeight, nFps, bShowFb, nLogLevel);

    SAMPLE_UTILS_SetPthreadName("MainThread");
    InstallSignalHandler();

    HDMI_IN_TYPE_E enHdmiInFormat = (HDMI_IN_TYPE_E)nHdmiInFormat;
    AX_U16 nDataRate = 0;
    if (nWidth == 3840 && nHeight == 2160 && nFps == 60)
    {
        nDataRate = 1900;
    }
    else if (nWidth == 3840 && nHeight == 2160 && nFps == 30)
    {
        nDataRate = 1010;
    }
    else if (nWidth == 1920 && nHeight == 1080 && nFps == 60)
    {
        nDataRate = 565;
    }
    else
    {
        KVM_LOG_E("unsupport param\n");
        return -1;
    }

    SAMPLE_BUILD_Init(enHdmiInFormat, nWidth, nHeight, nFps, nDataRate, bShowFb, nLogLevel);

    SAMPLE_BUILD_Start();

    while (isRunning)
    {
        SAMPLE_UXE_GPIO_WaitForDetectNotify(isRunning);
    }

    KVM_LOG_I("SAMPLE_BUILD_Stop...\n");
    SAMPLE_BUILD_Stop();

    KVM_LOG_I("SAMPLE_BUILD_DeInit...\n");
    SAMPLE_BUILD_DeInit();

    return -1;
}

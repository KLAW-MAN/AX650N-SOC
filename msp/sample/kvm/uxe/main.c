/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "gpioHelper.h"
#include "queryUXE.h"

static volatile AX_BOOL isRunning = AX_TRUE;

enum { LOAD_REGISTER = 0, POLLING = 1, MIPI_TX_OFF = 2, MIPI_TX_ON = 3, MAX_CMD_BUTT };

static void exit_handler(int s) {
    static AX_U32 exitCount = 0;
    printf("\n====================== Caught signal: %d ======================\n", s);
    printf("please waiting to quit ...\n\n");
    isRunning = AX_FALSE;
    if (exitCount++ >= 3) {
        printf("\n======================      Force to exit    ====================== \n");
        _exit(1);
    }
}

static void ignore_sig_pipe(void) {
    struct sigaction sa;
    sa.sa_handler = SIG_IGN;
    sa.sa_flags = 0;
    if (sigemptyset(&sa.sa_mask) == -1 || sigaction(SIGPIPE, &sa, 0) == -1) {
        perror("failed to ignore SIGPIPE, sigaction");
        exit(EXIT_FAILURE);
    }
}

static AX_VOID InstallSignalHandler(AX_VOID) {
    signal(SIGINT, exit_handler);
    signal(SIGQUIT, exit_handler);
    ignore_sig_pipe();
}

static void pollingCallback(AX_U32 gpio, AX_U8 value, AX_VOID *userdata) {
    UXE_SIGNAL_INFO_T info;
    AX_UXE_GetSignalInfo(&info);
    printf("\n");
}

static void showUsage() {
    printf("Usage: %s [OPTION]...\n", "uxe");
    printf(" OPTION:\n");
    printf(" -c          0: load uxe register, 1: polling GPIO5, 2: mipi tx off, 3: mipi tx on\n");
    printf(" -h          Print this message and exit\n");
}

int main(int argc, char *argv[]) {
    printf("compile: " APP_BUILD_VERSION " " __DATE__ " " __TIME__ "\n");

    if (argc < 2) {
        showUsage();
        exit(0);
    }

    InstallSignalHandler();

    int c;
    int isExit = 0;

    AX_S32 cmd = POLLING;
    while ((c = getopt(argc, argv, "c:h")) != -1) {
        isExit = 0;
        switch (c) {
            case 'c':
                cmd = (AX_S32)atoi(optarg);
                break;
            case 'h':
                isExit = 1;
                break;
            default:
                isExit = 1;
                break;
        }
    }

    if (isExit || cmd >= MAX_CMD_BUTT) {
        showUsage();
        exit(0);
    }

    if (LOAD_REGISTER == cmd) {
        UXE_SIGNAL_INFO_T info;
        AX_UXE_GetSignalInfo(&info);
        return 0;
    } else if (MIPI_TX_OFF == cmd) {
        AX_UXE_DisableMipiTx();
        return 0;
    } else if (MIPI_TX_ON == cmd) {
        AX_UXE_EnableMipiTx();
        return 0;
    }

    printf("set pinmux of GPIO2_A2 to GPIO function\n");
    system("ax_lookat 0x04250E30 -s 0x00060083");

    GPIO_HANDLE handle = CreateGpioHelper(66, GPIO_IN, GPIO_EDGE_FALLING);
    if (!handle) {
        return 1;
    }

    printf("start polling ...\n");
    StartGpioPolling(handle, pollingCallback, 0);

    while (isRunning) {
        sleep(1);
    }

    StopGpioPolling(handle);
    DestoryGpioHelper(handle);

    printf("stopped\n");
    return 0;
}
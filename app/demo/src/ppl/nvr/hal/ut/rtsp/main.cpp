/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include <malloc.h>
#include <signal.h>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <vector>
#include "AppLogApi.h"
#include "cmdline.hpp"
#include "make_unique.hpp"
#include "obs.hpp"
#include "rtspstream.hpp"
using namespace std;

using IStreamPtr = unique_ptr<IStream>;
using IObserverPtr = unique_ptr<CRtspObserver>;
static AX_BOOL isRunning = AX_TRUE;
static AX_U32 exitCount = 0;
static AX_VOID InstallSignalHandler(AX_VOID);
static AX_VOID PrintMallocInfo(AX_VOID);
static AX_VOID DisableDynamicMallocShrink(AX_S32 nKB);

#if 0
void* operator new(size_t size) {
    void* p = malloc(size);
    std::cout << "Allocated " << size << " bytes at address " << p << std::endl;
    return p;
}

void operator delete(void* p) {
    std::cout << "Deallocated memory at address " << p << std::endl;
    free(p);
}
#endif

int main(int argc, char* argv[]) {
    cmdline::parser a;
    a.add<AX_U32>("count", 'c', "stream count", false, 64);
    a.add<string>("url", '\0', "rtsp url", false, "rtsp://192.168.2.28:8554/test_1920x1080_4096kbs_30fps_gop30_heigh_52.264");
    a.add<AX_U32>("delay", '\0', "delay ms to drop", false, 0);
    a.add<AX_U32>("shrink", '\0', "1: malloc_trim", false, 0);
    a.parse_check(argc, argv);

    InstallSignalHandler();

    APP_LOG_ATTR_T stLog;
    memset(&stLog, 0, sizeof(stLog));
    stLog.nTarget = APP_LOG_TARGET_STDOUT;
    stLog.nLv = APP_LOG_WARN;
    strcpy(stLog.szAppName, "rtsp");
    AX_APP_Log_Init(&stLog);

    AX_U32 nCount = a.get<AX_U32>("count");
    string strUrl = a.get<string>("url");
    AX_U32 nDelay = a.get<AX_U32>("delay");
    AX_U32 shrink = a.get<AX_U32>("shrink");
    printf("url %s, delay %d ms, shrink = %d\n", strUrl.c_str(), nDelay, shrink);

    if (shrink) {
        DisableDynamicMallocShrink(128);
    }

    vector<IStreamPtr> clients(nCount);
    vector<IObserverPtr> obs(nCount);
    for (AX_U32 i = 0; i < nCount; ++i) {
        clients[i] = move(make_unique<CRtspStream>());
        obs[i] = move(make_unique<CRtspObserver>(i, nDelay));
        clients[i]->RegisterObserver(obs[i].get());

        STREAM_ATTR_T stAttr;
        stAttr.strURL = strUrl;
        stAttr.nFps = 30;
        if (!clients[i]->Init(stAttr)) {
            printf("connect client %d fail\n", i);
            return 1;
        }
    }

    for (auto&& m : clients) {
        m->Start();
    }

    AX_U32 nElapsed = 0;
    while (isRunning) {
        this_thread::sleep_for(chrono::seconds(1));
        if (30 == ++nElapsed) {
            if (shrink) {
                malloc_trim(0);
            }

            PrintMallocInfo();
            nElapsed = 0;
        }
    }

    for (auto&& m : clients) {
        m->Stop();
        m->DeInit();
    }

    AX_APP_Log_DeInit();

    return 0;
}

static void exit_handler(int s) {
    printf("\n====================== Caught signal: %d ======================\n", s);
    printf("please waiting to quit ...\n\n");
    isRunning = AX_FALSE;
    if (exitCount++ >= 3) {
        printf("\n======================      Force to exit    ====================== \n");
        abort();
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

static AX_VOID PrintMallocInfo(AX_VOID) {
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::cout << "---------------- " << std::put_time(std::localtime(&t), "%F %T") << " ----------------" << std::endl;
    malloc_stats();

    struct mallinfo mi = mallinfo();

    printf(
        "arena: %d  ordblks: %d  smblks: %d  hblks: %d  hblkhd: %d  usmblks: %d  fsmblks: %d  uordblks: %d  fordblks: %d  keepcost: %d\n\n",
        mi.arena,    /* non-mmapped space allocated from system */
        mi.ordblks,  /* number of free chunks */
        mi.smblks,   /* number of fastbin blocks */
        mi.hblks,    /* number of mmapped regions */
        mi.hblkhd,   /* space in mmapped regions */
        mi.usmblks,  /* maximum total allocated space */
        mi.fsmblks,  /* space available in freed fastbin blocks */
        mi.uordblks, /* total allocated space */
        mi.fordblks, /* total free space */
        mi.keepcost  /* top-most, releasable (via malloc_trim) space */
    );
}

static AX_VOID DisableDynamicMallocShrink(AX_S32 nKB) {
    printf("%s to %d KB\n", __func__, nKB);
    mallopt(M_TRIM_THRESHOLD, nKB * 1024);
}

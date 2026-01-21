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
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <memory>
#include <string>

#include "AppLogApi.h"
#include "cmdline.h"
#include "ElapsedTimer.hpp"
#include "GlobalDef.h"
#include "make_unique.hpp"

#include "axcl_demo_builder.hpp"

static bool gIsRunning = false;
static int  gExitCount = 0;

static void exit_handler(int s) {
    printf("\n====================== Caught signal: %d ======================\n", s);
    printf("please waiting to quit ...\n\n");
    gIsRunning = false;
    gExitCount++;
    if (gExitCount >= 3) {
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

int main(int argc, char *argv[]) {
    signal(SIGINT, exit_handler);
    signal(SIGQUIT, exit_handler);
    signal(SIGTERM, exit_handler);
    ignore_sig_pipe();

    cmdline::parser a;
    a.add<std::string>("json", '\0', "axcl.json path", false, "../axcl.json");
    a.parse_check(argc, argv);
    const std::string json = a.get<std::string>("json");

    AX_MTRACE_ENTER(AXCL_DEMO);

    printf("ENTER main\n");
    //mallopt(M_MMAP_MAX, 0);
    mallopt(M_TRIM_THRESHOLD, 1024*128);

    try {
        std::unique_ptr<CAxclDemoBuilder> pApp = std::make_unique<CAxclDemoBuilder>();
        if (!pApp) {
            perror("create axcl demo builder failed.");
            return 1;
        }

        if (!pApp->Start(json)) {
            return 1;
        }

        gIsRunning = true;
        int nRunCount = 0;
        while (gIsRunning) {
            CElapsedTimer::GetInstance()->mSleep(1000);
            if (++nRunCount > 30) {
                // Release memory back to the system every 30 seconds.
                // https://linux.die.net/man/3/malloc_trim
                malloc_trim(0);

                nRunCount = 0;
            }

            if (pApp->QueryStreamsAllEof()) {
                break;
            }
        }

        pApp->Stop();

    } catch (CAXException &e) {
        printf("%s\n", e.what());
        AX_MTRACE_LEAVE;
        return 1;
    }

    AX_MTRACE_LEAVE;
    return 0;
}

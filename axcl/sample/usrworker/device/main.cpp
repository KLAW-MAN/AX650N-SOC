#include <signal.h>
#include <stdio.h>
#include <string.h>
#include "axcl_worker_runtime.h"
#include "logger.h"
#include "signal_handler.hpp"

int main(int argc, char* argv[]) {
    int ret;
    signal_handler handler;

    for (int i = 1; i < argc; i++) {
        SAMPLE_LOG_I("argv[%d]: %s", i, argv[i]);
    }

    /* initialize worker runtime */
    if (ret = axclrtWorkerInit(); 0 != ret) {
        SAMPLE_LOG_E("axclrtWorkerInit() fail, ret = %d", ret);
        return 1;
    }

    constexpr uint32_t timeout = 5000;
    char msg[4096];
    uint32_t len;
    while (!handler.is_quit()) {
        /* recv message from host */
        memset(msg, 0, sizeof(msg));
        ret = axclrtWorkerRecv(msg, sizeof(msg), &len, timeout);
        if (ret < 0) {
            continue;
        } else {
            SAMPLE_LOG_I("recv: %s", msg);
        }

        if (0 == strcmp(msg, "quit")) {
            break;
        }

        /* loop recv message to host */
        ret = axclrtWorkerSend(msg, len, timeout);
        if (ret < 0) {
            SAMPLE_LOG_E("send < %s > fail, ret = %d", msg, ret);
        }
    }

    /* deinitialize worker runtime */
    axclrtWorkerDeInit();
    return 0;
}
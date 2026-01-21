/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#pragma once

#include <atomic>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#ifdef WINDOWS
    #include <process.h>
    typedef int pid_t;
#else
    #include <unistd.h>
#endif
#include "axcl_ppl.h"

struct ppl_user_data {
    axcl_ppl ppl;
    int32_t device;
    uint32_t fps;
    pid_t pid;
    int32_t vdec;
    int32_t ivps;
    int32_t venc;
    std::atomic<uint64_t> frame_count;
    FILE *fp;

    static ppl_user_data *alloc(const char *dump_fpath) {
        ppl_user_data *p = (ppl_user_data *)malloc(sizeof(ppl_user_data));
        if (!p) {
            return nullptr;
        }

        p->ppl = nullptr;
        p->device = -1;
        p->fps = 0;
        p->pid = getpid();
        p->vdec = -1;
        p->ivps = -1;
        p->venc = -1;
        p->frame_count = 0;
        if (dump_fpath) {
            char name[256];
            sprintf(name, "%s.dump.pid%d", dump_fpath, (uint32_t)p->pid);
            p->fp = fopen(name, "wb");
        } else {
            p->fp = nullptr;
        }

        return p;
    }

    static void destroy(ppl_user_data *p) {
        if (p) {
            if (p->fp) {
                fclose(p->fp);
            }

            free(p);
        }
    }
};
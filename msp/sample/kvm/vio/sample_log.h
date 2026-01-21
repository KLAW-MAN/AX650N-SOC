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
#include "ax_global_type.h"
#include "ax_sys_log.h"

#ifdef __KVM_LOG__

#include <time.h>
#include <stdio.h>
#include <string.h>

#define COLOR_RED "\033[1;31m"
#define COLOR_GREEN "\033[1;34m"
#define COLOR_NONE "\033[0m"

#define filename(x) strrchr(x,'/')?strrchr(x,'/')+1:x

#define KVM_LOG_I(fmt, args...)\
    do\
    {\
        time_t t;\
        struct tm *ti;\
        time(&t);\
        ti = localtime(&t);\
        printf("%04d-%02d-%02d %02d:%02d:%02d ",ti->tm_year + 1900,ti->tm_mon+1,ti->tm_mday,ti->tm_hour,ti->tm_min,ti->tm_sec);\
        printf("[%s:%d->%s] ", filename(__FILE__), __LINE__, __func__);\
        printf(COLOR_GREEN "INFO:" COLOR_NONE);\
        printf(fmt, ##args);\
    }while(0)

    #define KVM_LOG_E(fmt, args...)\
    do\
    {\
        time_t t;\
        struct tm *ti;\
        time(&t);\
        ti = localtime(&t);\
        printf("%04d-%02d-%02d %02d:%02d:%02d ",ti->tm_year + 1900,ti->tm_mon+1,ti->tm_mday,ti->tm_hour,ti->tm_min,ti->tm_sec);\
        printf("[%s:%d->%s] ", filename(__FILE__), __LINE__, __func__);\
        printf(COLOR_RED "ERROR:" COLOR_NONE);\
        printf(fmt, ##args);\
    }while(0)

#else
    #define KVM_LOG_I(fmt,args...)
    #define KVM_LOG_E(fmt, args...)

#endif // __LOG
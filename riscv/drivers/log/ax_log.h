/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef __AX_LOG_H__
#define __AX_LOG_H__

#include <rthw.h>

enum AX_LOG_LEVEL {
    AX_LOG_LEVEL_EMERGE    = 1, // System is unusable
    AX_LOG_LEVEL_CRITICAL  = 2, // Critical conditions
    AX_LOG_LEVEL_ERROR     = 3, // Error conditions
    AX_LOG_LEVEL_WARNING   = 4, // Warning conditions
    AX_LOG_LEVEL_NOTICE    = 5, // Normal but significant condition
    AX_LOG_LEVEL_INFO      = 6, // Informational
    AX_LOG_LEVEL_DEBUG     = 7, // Debug-level messages
    AX_LOG_LEVEL_VERBOSE   = 8  // Verbose-level messages
};

#define AX_LOG_LEVEL_SET AX_LOG_LEVEL_ERROR

#ifdef AX_LOG_TAG
#define AX_SYNC_SIG_LOG(log_level, msg_type, format, ...) \
    { \
       rt_kprintf("[%s][%s][%d]: " format "\n", msg_type, __func__, __LINE__,  ##__VA_ARGS__); \
    }
#else
#define AX_SYNC_SIG_LOG(log_level, msg_type, format, ...) \
    if ((log_level) <= AX_LOG_LEVEL_SET) { \
       rt_kprintf("[%s][%s][%d]: " format "\n", msg_type, __func__, __LINE__,  ##__VA_ARGS__); \
    }
#endif

#define AX_LOG_CRIT(format, ...) AX_SYNC_SIG_LOG(AX_LOG_LEVEL_CRITICAL, "ERR", format, ##__VA_ARGS__)
#define AX_LOG_ERROR(format, ...) AX_SYNC_SIG_LOG(AX_LOG_LEVEL_ERROR, "ERR", format, ##__VA_ARGS__)
#define AX_LOG_WARN(format, ...) AX_SYNC_SIG_LOG(AX_LOG_LEVEL_WARNING, "WRN", format, ##__VA_ARGS__)
#define AX_LOG_INFO(format, ...) AX_SYNC_SIG_LOG(AX_LOG_LEVEL_INFO, "INFO", format, ##__VA_ARGS__)
#define AX_LOG_DBG(format, ...) AX_SYNC_SIG_LOG(AX_LOG_LEVEL_DEBUG, "DBG", format, ##__VA_ARGS__)

#endif //__AX_LOG_H__

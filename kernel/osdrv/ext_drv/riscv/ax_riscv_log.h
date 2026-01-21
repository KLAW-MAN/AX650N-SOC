/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef __AX_RISCV_LOG_H__
#define __AX_RISCV_LOG_H__

#include "osal_ax.h"
#include "ax_global_type.h"

#define RISCV_LIKELY(x) __builtin_expect(!!(x), 1)
#define RISCV_UNLIKELY(x) __builtin_expect(!!(x), 0)

#define AX_SYS_LOG_VALID 0

#define RISCV_FATAL_LEVEL    0
#define RISCV_ERR_LEVEL      1
#define RISCV_WARNING_LEVEL  2
#define RISCV_INFO_LEVEL     3
#define RISCV_DEBUG_LEVEL    4

#define RISCV_LOG_LEVEL RISCV_INFO_LEVEL

#if AX_SYS_LOG_VALID
#define RISCV_LOG_BASE(log_level, msg_type, format, ...) \
    do {  \
        if (RISCV_UNLIKELY((log_level) <= RISCV_LOG_LEVEL)) {\
            switch (log_level) { \
            case RISCV_ERR_LEVEL: \
                ax_pr_err(AX_ID_RISCV, "RISCV", "[RISCV][%s][%s %d]: " format "\n", "Error", __func__, __LINE__, ##__VA_ARGS__); \
                break; \
            case RISCV_WARNING_LEVEL: \
                ax_pr_warn(AX_ID_RISCV, "RISCV", "[RISCV][%s][%s %d]: " format "\n", "Warning", __func__, __LINE__, ##__VA_ARGS__); \
                break; \
            case RISCV_INFO_LEVEL: \
                ax_pr_info(AX_ID_RISCV, "RISCV", "[RISCV][%s][%s %d]: " format "\n", "Info", __func__, __LINE__, ##__VA_ARGS__); \
                break; \
            case RISCV_DEBUG_LEVEL: \
                ax_pr_debug(AX_ID_RISCV, "RISCV", "[RISCV][%s][%s %d]: " format "\n", "Debug", __func__, __LINE__, ##__VA_ARGS__); \
                break; \
            default: \
                break; \
            } \
        } \
    } while (0)

#else
#define riscv_log_print(format, ...)            printk(format, ##__VA_ARGS__)
#define RISCV_LOG_BASE(log_level, msg_type, format, ...) \
    do {  \
        if ((log_level) <= RISCV_LOG_LEVEL) {\
            riscv_log_print("[RISCV][%s][%s %d]: " format "\n", msg_type, __func__, __LINE__, ##__VA_ARGS__); \
        } \
    } while (0)

#endif

#define RISCV_LOG_ERROR(format, ...)   RISCV_LOG_BASE(RISCV_ERR_LEVEL, "Error", format, ##__VA_ARGS__)
#define RISCV_LOG_WARN(format, ...)    RISCV_LOG_BASE(RISCV_WARNING_LEVEL, "Warning", format, ##__VA_ARGS__)
#define RISCV_LOG_INFO(format, ...)    RISCV_LOG_BASE(RISCV_INFO_LEVEL, "Info", format, ##__VA_ARGS__)
#define RISCV_LOG_DBG(format, ...)     RISCV_LOG_BASE(RISCV_DEBUG_LEVEL, "Debug", format, ##__VA_ARGS__)

#endif //__AX_RISCV_LOG_H__

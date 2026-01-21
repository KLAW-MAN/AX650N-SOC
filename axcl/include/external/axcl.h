/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef __AXCL_H__
#define __AXCL_H__

#include "axcl_base.h"
#include "axcl_native.h"
#include "axcl_rt.h"

#ifdef __cplusplus
extern "C" {
#endif

AXCL_EXPORT axclError axclInit(const char *config);
AXCL_EXPORT axclError axclFinalize();

/**
 * @brief set axcl log level
 * @param lv log level
 *          0: trace
 *          1: debug
 *          2: info
 *          3: warning
 *          4: error
 *          5: critical
 *          6: off
 * @return 0 success, otherwise failure
*/
AXCL_EXPORT axclError axclSetLogLevel(int32_t lv);

/**
 * @brief Records an application log with below format:
 *        [date time][tid][level][APP][function][file][line]: formatted message
 *        Example:
 *          axclAppLog(5, __func__, NULL, __LINE__, "json: %s, device: %d", json, device);
 *          log:
 *          [2024-11-12 14:24:22.380][1330][C][APP][main][53]: json: ./axcl.json, device: 129
 *
 * @param lv log level, see axclSetLogLevel for reference
 * @param func function name; if set to NULL, the function name will not be printed
 * @param file file name; if set to NULL, the file name will not be printed
 * @param line line number
 * @param fmt format string for the log message, max. length is 1024.
 */
AXCL_EXPORT void axclAppLog(int32_t lv, const char *func, const char *file, uint32_t line, const char *fmt, ...);

#ifdef __cplusplus
}
#endif

#endif /* __AXCL_H__ */
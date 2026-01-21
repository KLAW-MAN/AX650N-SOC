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

#include <cstdio>
#include <cstdlib>
#include "color.hpp"

#if defined(DEBUG)

#define AXCL_ASSERT(condition, fmt, ...)                                                                                         \
    do {                                                                                                                         \
        if (!(condition)) {                                                                                                      \
            fprintf(stderr, COLOR_RED "[assert][%s][%d]: " fmt COLOR_DEFAULT "\n", __func__, __LINE__, ##__VA_ARGS__); \
            std::abort();                                                                                                        \
        }                                                                                                                        \
    } while (0)

#else

#define AXCL_ASSERT(condition, fmt, ...)                                                                                         \
    do {                                                                                                                         \
        if (!(condition)) {                                                                                                      \
            fprintf(stderr, COLOR_RED "[assert][%s][%d]: " fmt COLOR_DEFAULT "\n", __func__, __LINE__, ##__VA_ARGS__); \
        }                                                                                                                        \
    } while (0)

#endif

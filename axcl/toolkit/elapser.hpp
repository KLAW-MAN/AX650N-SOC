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

#include <chrono>
#include <cstdint>

namespace axcl {

class elapser {
public:
    enum class UNIT {
        microseconds = 1,
        milliseconds = 2,
        seconds = 3,
        nanoseconds = 4,
    };

public:
    elapser();

    void start();
    uint64_t cost(UNIT unit = UNIT::milliseconds);

    static void msleep(uint32_t ms);
    static void usleep(uint32_t us);
    static void sleep(uint32_t s);

    /**
     * @brief ONLY for slave, high precision of hw time64.
     *        Actived by macro: __HW_HIGH_PRECISION_CLOCK__
     */
    static void ax_msleep(uint32_t ms);
    static void ax_usleep(uint32_t us);

    /**
     * @brief return tick count in microseconds by CLOCK_MONOTONIC
     */
    static uint64_t ticks();

private:
    using clock = std::chrono::steady_clock;
    clock::time_point m_begin = clock::now();
};

}  // namespace axcl
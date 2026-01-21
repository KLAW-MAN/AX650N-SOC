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

#include <cstdint>
#include <mutex>

#if defined(WINDOWS)
    #include <condition_variable>
#else
    #include "condition_variable.hpp"
#endif

namespace axcl {

class event final {
public:
    event() = default;

    bool wait(int32_t timeout_ms = -1);
    void set();
    void reset();

private:
    std::mutex m_mtx;
#if defined(WINDOWS)
    std::condition_variable m_cv;
#else
    condition_variable m_cv;
#endif
    bool m_signal = false;
};

class bitmask_event final {
public:
    bitmask_event() = default;

    bool wait(int bit_mask = ~0, int32_t timeout_ms = -1);

    void set(int bit_mask);
    void reset(int bit_mask);
    void reset_all();

private:
    std::mutex m_mtx;
#if defined(WINDOWS)
    std::condition_variable m_cv;
#else
    axcl::condition_variable m_cv;
#endif
    int m_signal = 0;
};

}  // namespace axcl
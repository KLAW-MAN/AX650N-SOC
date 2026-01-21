/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include "elapser.hpp"
#include <chrono>
#include <thread>

#if defined(WINDOWS)
#else
#include <sys/time.h>
#include <time.h>
#endif

#ifdef __AX_HIGH_PRECISION_CLOCK__
#include "ax_base_type.h"
extern "C" AX_S32 AX_SYS_Usleep(unsigned long usc);
extern "C" AX_S32 AX_SYS_Msleep(unsigned long msc);
#endif

namespace axcl {

elapser::elapser() {
    start();
}

void elapser::start() {
    m_begin = clock::now();
}

uint64_t elapser::cost(UNIT unit /* = UNIT::milliseconds */) {
    auto end = clock::now();

    uint64_t _elapsed;
    switch (unit) {
        case UNIT::seconds:
            _elapsed = std::chrono::duration_cast<std::chrono::seconds>(end - m_begin).count();
            break;
        case UNIT::microseconds:
            _elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - m_begin).count();
            break;
        case UNIT::nanoseconds:
            _elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - m_begin).count();
            break;
        default:
            _elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - m_begin).count();
            break;
    }

    start();

    return _elapsed;
}

uint64_t elapser::ticks() {
#if defined(WINDOWS)
    auto now = std::chrono::steady_clock::now();
    auto duration = now.time_since_epoch();
    return std::chrono::duration_cast<std::chrono::microseconds>(duration).count();
#else
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (ts.tv_sec * 1000000 + ts.tv_nsec / 1000);
#endif
}

void elapser::msleep(uint32_t ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

void elapser::usleep(uint32_t us) {
    std::this_thread::sleep_for(std::chrono::microseconds(us));
}

void elapser::sleep(uint32_t s) {
    std::this_thread::sleep_for(std::chrono::seconds(s));
}

void elapser::ax_msleep(uint32_t ms) {
#ifdef __AX_HIGH_PRECISION_CLOCK__
    (AX_VOID) AX_SYS_Msleep(ms);
#else
    elapser::msleep(ms);
#endif
}
void elapser::ax_usleep(uint32_t us) {
#ifdef __AX_HIGH_PRECISION_CLOCK__
    (AX_VOID) AX_SYS_Usleep(us);
#else
    elapser::usleep(us);
#endif
}

}  // namespace axcl
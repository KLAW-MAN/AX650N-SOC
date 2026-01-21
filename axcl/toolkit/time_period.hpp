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

// clang-format off
#include <windows.h>
#include <mmsystem.h>
#include <stdint.h>
// clang-format on

#pragma comment(lib, "winmm.lib")

class time_period {
private:
    uint32_t m_period;
    bool m_active;

public:
    explicit time_period(uint32_t period) : m_period(period), m_active(false) {
        TIMECAPS tc;
        MMRESULT result = timeGetDevCaps(&tc, sizeof(TIMECAPS));
        if (result == TIMERR_NOERROR) {
            if (period < tc.wPeriodMin) {
                m_period = tc.wPeriodMin;
            } else if (period > tc.wPeriodMax) {
                m_period = tc.wPeriodMax;
            }
        } else {
            m_period = (period < 1) ? 1 : period;
        }

        result = timeBeginPeriod(m_period);
        if (result == TIMERR_NOERROR) {
            m_active = true;
        }
    }

    ~time_period() {
        if (m_active) {
            timeEndPeriod(m_period);
            m_active = false;
        }
    }

    uint32_t period() const {
        return m_period;
    }

    bool active() const {
        return m_active;
    }

    time_period(const time_period&) = delete;
    time_period& operator=(const time_period&) = delete;
    time_period(time_period&&) noexcept = delete;
    time_period& operator=(time_period&&) noexcept = delete;
};
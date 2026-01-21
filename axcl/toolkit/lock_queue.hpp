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
#include <mutex>
#include <queue>
#include <condition_variable>

namespace axcl {

template <typename T>
class lock_queue {
public:
    lock_queue() = default;
    virtual ~lock_queue() = default;

    void set_capacity(uint32_t capacity) {
        std::lock_guard<std::mutex> lck(m_mtx);
        m_capacity = (capacity < 0) ? (uint32_t)-1 : capacity;
    }

    int32_t get_capacity(void) const {
        std::lock_guard<std::mutex> lck(m_mtx);
        return (m_capacity == ((uint32_t)-1)) ? -1 : m_capacity;
    }

    size_t size(void) const {
        std::lock_guard<std::mutex> lck(m_mtx);
        return m_q.size();
    }

    bool full(void) const {
        std::lock_guard<std::mutex> lck(m_mtx);
        return m_q.size() >= m_capacity;
    }

    void wakeup(void) {
        std::lock_guard<std::mutex> lck(m_mtx);
        m_wakeup = true;
        m_cv.notify_one();
    }

    bool push(const T& m) {
        std::lock_guard<std::mutex> lck(m_mtx);
        if (m_q.size() >= m_capacity) {
            return false;
        }

        m_q.push(m);
        m_cv.notify_one();
        return true;
    }

    bool pop(T& m, int32_t timeout_ms = -1) {
        std::unique_lock<std::mutex> lck(m_mtx);
        bool avail = true;

        if (0 == m_q.size()) {
            if (0 == timeout_ms) {
                avail = false;
            } else {
                if (timeout_ms < 0) {
                    m_cv.wait(lck, [this]() -> bool { return !m_q.empty() || m_wakeup; });
                } else {
                    m_cv.wait_for(lck, std::chrono::milliseconds(timeout_ms), [this]() -> bool { return !m_q.empty() || m_wakeup; });
                }

                m_wakeup = false;
                avail = m_q.empty() ? false : true;
            }
        }

        if (avail) {
            m = m_q.front();
            m_q.pop();
        }

        return avail;
    }

private:
    /* delete copy and assignment ctor, std::vector<lock_queue<X>> is not allowed */
    lock_queue(const lock_queue&) = delete;
    lock_queue(lock_queue&&) = delete;
    lock_queue& operator=(const lock_queue&) = delete;
    lock_queue& operator=(lock_queue&&) = delete;

private:
    uint32_t m_capacity = (uint32_t)-1;
    std::queue<T> m_q;
    mutable std::mutex m_mtx;
    std::condition_variable m_cv;
    bool m_wakeup = false;
};

}  // namespace axcl

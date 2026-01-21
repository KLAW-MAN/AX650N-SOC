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
#include <condition_variable>  // For std::cv_status
#include <pthread.h>
#include <time.h>
#include <cstdint>
#include <errno.h>
#include <stdexcept>
#include <system_error>
#include <type_traits>
#include <limits>       // For std::numeric_limits
#include <cstring>      // For std::strerror

namespace axcl {

/**
 * @brief Clock type enumeration
 */
enum class clock_type {
    MONOTONIC = CLOCK_MONOTONIC,    ///< Monotonic clock, not affected by NTP
    REALTIME = CLOCK_REALTIME       ///< Real-time clock, affected by NTP
};

class condition_variable {
public:
    /**
     * @brief Constructor
     * @param clock Clock type, defaults to CLOCK_MONOTONIC
     * @throws std::system_error if initialization fails
     */
    explicit condition_variable(clock_type clock = clock_type::MONOTONIC) 
        : m_clock_type(static_cast<clockid_t>(clock)) {
        pthread_condattr_t attr;
        int result = pthread_condattr_init(&attr);
        if (result != 0) {
            throw std::system_error(result, std::system_category(), "pthread_condattr_init failed");
        }

        struct attr_guard {
            pthread_condattr_t* attr;
            attr_guard(pthread_condattr_t* a) : attr(a) {}
            ~attr_guard() { pthread_condattr_destroy(attr); }
        } guard(&attr);

        result = pthread_condattr_setclock(&attr, m_clock_type);
        if (result != 0) {
            throw std::system_error(result, std::system_category(), "pthread_condattr_setclock failed");
        }

        result = pthread_cond_init(&m_cond, &attr);
        if (result != 0) {
            throw std::system_error(result, std::system_category(), "pthread_cond_init failed");
        }
    }

    ~condition_variable() noexcept {
        int result = pthread_cond_destroy(&m_cond);
        if (result != 0) {
            // Cannot throw exceptions in destructor, can only log errors
            // Consider using logging system or silent handling
            // For now, we silently ignore the error as it's not critical
        }
    }

    // Disable copy and move
    condition_variable(const condition_variable&) = delete;
    condition_variable& operator=(const condition_variable&) = delete;
    condition_variable(condition_variable&&) = delete;
    condition_variable& operator=(condition_variable&&) = delete;

    /**
     * @brief Notify one waiting thread
     */
    void notify_one() noexcept {
        pthread_cond_signal(&m_cond);
    }

    /**
     * @brief Notify all waiting threads
     */
    void notify_all() noexcept {
        pthread_cond_broadcast(&m_cond);
    }

    /**
     * @brief Wait for condition to be satisfied (infinite wait)
     * @param lock Mutex lock
     */
    void wait(std::unique_lock<std::mutex>& lock) {
        if (!lock.owns_lock()) {
            throw std::system_error(std::make_error_code(std::errc::operation_not_permitted), "condition_variable::wait: mutex not locked");
        }

        int result;
        do {
            result = pthread_cond_wait(&m_cond, lock.mutex()->native_handle());
        } while (result == EINTR);

        if (result != 0) {
            throw std::system_error(result, std::system_category(), "pthread_cond_wait failed");
        }
    }

    /**
     * @brief Wait for condition to be satisfied (infinite wait with predicate)
     * @param lock Mutex lock
     * @param pred Condition predicate
     */
    template<typename Predicate>
    void wait(std::unique_lock<std::mutex>& lock, Predicate pred) {
        while (!pred()) {
            wait(lock);
        }
    }

    /**
     * @brief Wait for condition to be satisfied (with timeout)
     * @param lock Mutex lock
     * @param timeout_duration Timeout duration
     * @return std::cv_status::timeout on timeout, std::cv_status::no_timeout otherwise
     */
    template<typename Rep, typename Period>
    std::cv_status wait_for(std::unique_lock<std::mutex>& lock,
                            const std::chrono::duration<Rep, Period>& timeout_duration) {
        // Use the appropriate clock based on the condition variable's clock type
        if (m_clock_type == CLOCK_MONOTONIC) {
            return wait_until(lock, std::chrono::steady_clock::now() + timeout_duration);
        } else {
            return wait_until(lock, std::chrono::system_clock::now() + timeout_duration);
        }
    }

    /**
     * @brief Wait for condition to be satisfied (with timeout and predicate)
     * @param lock Mutex lock
     * @param timeout_duration Timeout duration
     * @param pred Condition predicate
     * @return true if condition is satisfied, false on timeout
     */
    template<typename Rep, typename Period, typename Predicate>
    bool wait_for(std::unique_lock<std::mutex>& lock,
                  const std::chrono::duration<Rep, Period>& timeout_duration,
                  Predicate pred) {
        // Use the appropriate clock based on the condition variable's clock type
        if (m_clock_type == CLOCK_MONOTONIC) {
            return wait_until(lock, std::chrono::steady_clock::now() + timeout_duration, pred);
        } else {
            return wait_until(lock, std::chrono::system_clock::now() + timeout_duration, pred);
        }
    }

    /**
     * @brief Wait for condition to be satisfied (with absolute timeout)
     * @param lock Mutex lock
     * @param timeout_time Absolute timeout time point
     * @return std::cv_status::timeout on timeout, std::cv_status::no_timeout otherwise
     */
    template<typename Clock, typename Duration>
    std::cv_status wait_until(std::unique_lock<std::mutex>& lock,
                              const std::chrono::time_point<Clock, Duration>& timeout_time) {
        if ((m_clock_type == CLOCK_MONOTONIC && !std::is_same<Clock, std::chrono::steady_clock>::value) ||
            (m_clock_type == CLOCK_REALTIME && !std::is_same<Clock, std::chrono::system_clock>::value)) {
            throw std::invalid_argument("condition_variable::wait_until: clock type mismatch");
        }

        if (!lock.owns_lock()) {
            throw std::system_error(std::make_error_code(std::errc::operation_not_permitted), "condition_variable::wait_until: mutex not locked");
        }

        struct timespec ts;
        to_timespec(timeout_time, ts);

        int result;
        do {
            result = pthread_cond_timedwait(&m_cond, lock.mutex()->native_handle(), &ts);
        } while (result == EINTR);

        if (result == ETIMEDOUT) {
            return std::cv_status::timeout;
        } else if (result != 0) {
            throw std::system_error(result, std::system_category(), "pthread_cond_timedwait failed");
        }
        return std::cv_status::no_timeout;
    }

    /**
     * @brief Wait for condition to be satisfied (with absolute timeout and predicate)
     * @param lock Mutex lock
     * @param timeout_time Absolute timeout time point
     * @param pred Condition predicate
     * @return true if condition is satisfied, false on timeout
     */
    template<typename Clock, typename Duration, typename Predicate>
    bool wait_until(std::unique_lock<std::mutex>& lock,
                    const std::chrono::time_point<Clock, Duration>& timeout_time,
                    Predicate pred) {
        while (!pred()) {
            if (wait_until(lock, timeout_time) == std::cv_status::timeout) {
                return pred();
            }
        }
        return true;
    }

    /**
     * @brief Get native handle
     */
    pthread_cond_t* native_handle() noexcept {
        return &m_cond;
    }

    /**
     * @brief Get current clock type
     */
    clock_type get_clock_type() const noexcept {
        return static_cast<clock_type>(m_clock_type);
    }

    /**
     * @brief Check if the condition variable is using monotonic clock
     * @return true if using CLOCK_MONOTONIC, false otherwise
     */
    bool is_monotonic() const noexcept {
        return m_clock_type == CLOCK_MONOTONIC;
    }

private:
    pthread_cond_t m_cond;
    clockid_t m_clock_type;

    /**
     * @brief Convert time point to timespec (optimized precision handling)
     * @tparam Clock Clock type
     * @tparam Duration Duration type
     * @param tp Time point to convert
     * @param ts Output timespec structure
     */
    template<typename Clock, typename Duration>
    static void to_timespec(const std::chrono::time_point<Clock, Duration>& tp, struct timespec& ts) {
        auto total_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(
            tp.time_since_epoch()
        ).count();

        // Use safer type conversion to avoid precision loss
        constexpr std::int64_t NS_PER_SEC = 1000000000LL;
        std::int64_t seconds = total_ns / NS_PER_SEC;
        std::int64_t nanoseconds = total_ns % NS_PER_SEC;

        // Handle negative nanoseconds case
        if (nanoseconds < 0) {
            seconds -= 1;
            nanoseconds += NS_PER_SEC;
        }

        // Check for overflow/underflow in time_t range
        constexpr std::int64_t TIME_T_MAX = static_cast<std::int64_t>(std::numeric_limits<time_t>::max());
        constexpr std::int64_t TIME_T_MIN = static_cast<std::int64_t>(std::numeric_limits<time_t>::min());

        if (seconds > TIME_T_MAX) {
            seconds = TIME_T_MAX;
            nanoseconds = NS_PER_SEC - 1;
        } else if (seconds < TIME_T_MIN) {
            seconds = TIME_T_MIN;
            nanoseconds = 0;
        }

        ts.tv_sec = static_cast<time_t>(seconds);
        ts.tv_nsec = static_cast<long>(nanoseconds);
    }
};

}  // namespace axcl
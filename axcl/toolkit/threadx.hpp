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

#include <atomic>
#include <cstdint>
#include <functional>
#include <stdexcept>
#include <string>
#include <thread>
#include "event.hpp"

namespace axcl {

class threadx final {
public:
    threadx() noexcept = default;

    template <typename Call, typename... Args>
    explicit threadx(const std::string& name, Call&& f, Args&&... args) {
        start(name, 0, -1, std::forward<Call>(f), std::forward<Args>(args)...);
    }

    template <typename Call, typename... Args>
    explicit threadx(const std::string& name, int32_t policy, int32_t priority, Call&& f, Args&&... args) {
        start(name, policy, priority, std::forward<Call>(f), std::forward<Args>(args)...);
    }

    threadx(const threadx&) = delete;
    threadx& operator=(const threadx&) = delete;
    threadx(threadx&& t) noexcept;

    /**
     * fixme: why should support this?
     * svn://gcc.gnu.org/svn/gcc/trunk/libstdc++-v3
     *  thread& operator=(thread&& __t) noexcept {
     *      if (joinable())
                std::terminate();
            swap(__t);
            return *this;
        }
    */
    threadx& operator=(threadx&&) noexcept = delete;

    template <typename Call, typename... Args>
    void start(const std::string& name, Call&& f, Args&&... args) {
        start(name, 0, -1, std::forward<Call>(f), std::forward<Args>(args)...);
    }

    template <typename Call, typename... Args>
    void start(const std::string& name, int32_t policy, int32_t priority, Call&& f, Args&&... args) {
        if (m_thread.joinable()) {
            throw std::runtime_error("thread is running");
        }

        axcl::event* evt = new axcl::event();
        m_policy = policy;
        m_priority = priority;
        m_name = name;
        m_func = std::bind(std::forward<Call>(f), std::forward<Args>(args)...);
        m_thread = std::thread(&threadx::entry, this, evt);
        evt->wait(-1);
        delete evt;
    }

    void stop() {
        m_running = false;
    }

    void join() {
        if (m_thread.joinable()) {
            m_thread.join();
        }
    }

    void detach() {
        if (m_thread.joinable()) {
            m_thread.detach();
        }
    }

    bool running() const {
        return m_running.load();
    }

    std::thread::native_handle_type native_handle() {
        return m_thread.native_handle();
    }

    uint32_t get_id() const {
        return m_tid;
    }

protected:
    void entry(axcl::event* evt);
    void swap(threadx& t) noexcept;

private:
    std::thread m_thread;
    std::atomic<bool> m_running = {false};
    std::function<void()> m_func;
    std::string m_name;
    int32_t m_policy = 0;
    int32_t m_priority = 0;
    uint32_t m_tid = 0;
};

}  // namespace axcl
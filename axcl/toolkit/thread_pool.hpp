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

#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <future>
#include <functional>
#include <stdexcept>
#include <condition_variable>
#include "axcl_logger.hpp"

namespace axcl {
class thread_pool {
public:
    thread_pool(size_t, const std::string& token = "threads", const int32_t& sched_policy = SCHED_OTHER, const uint32_t& sched_priority = 0);
    template<class F, class... Args>
    auto enqueue(F&& f, Args&&... args)
        -> std::future<typename std::result_of<F(Args...)>::type>;
    ~thread_pool();
private:
    // need to keep track of threads so we can join them
    std::vector< std::thread > workers;
    // the task queue
    std::queue< std::function<void()> > tasks;
    size_t threads_num;
    std::string threads_name;

    // synchronization
    std::mutex queue_mutex;
    std::condition_variable condition;
    bool stop;
};

// the constructor just launches some amount of workers
inline thread_pool::thread_pool(size_t threads, const std::string& token, const int32_t& sched_policy, const uint32_t& sched_priority)
    :   threads_num(threads),
        threads_name(token),
        stop(false)
{
    for(size_t i = 0;i<threads;++i) {
        workers.emplace_back(
            [i, threads, token, this]
            {
                std::string thread_name;

                if (threads > 1) {
                    thread_name = token + "_" + std::to_string(i + 1);
                }
                else {
                    thread_name = token;
                }

                pthread_setname_np(pthread_self(), thread_name.c_str());

                for(;;) {
                    std::function<void()> task;

                    {
                        std::unique_lock<std::mutex> lock(this->queue_mutex);
                        this->condition.wait(lock,
                            [this]{ return this->stop || !this->tasks.empty(); });
                        if(this->stop && this->tasks.empty())
                            return;
                        task = std::move(this->tasks.front());
                        this->tasks.pop();
                    }

                    if (task) {
                        task();
                    }
                }
            }
        );

        {
            sched_param sch;
            int policy;
            pthread_getschedparam(pthread_self(), &policy, &sch);
            sch.sched_priority = sched_priority;
            pthread_setschedparam(pthread_self(), sched_policy, &sch);
        }
    }
}

// add new work item to the pool
template<class F, class... Args>
auto thread_pool::enqueue(F&& f, Args&&... args)
    -> std::future<typename std::result_of<F(Args...)>::type>
{
    using return_type = typename std::result_of<F(Args...)>::type;

    auto task = std::make_shared< std::packaged_task<return_type()> >(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );

    std::future<return_type> res = task->get_future();
    {
        std::unique_lock<std::mutex> lock(queue_mutex);

        // don't allow enqueueing after stopping the pool
        if(stop) {
            throw std::runtime_error("enqueue on stopped thread_pool");
        }

        tasks.emplace([task](){ (*task)(); });

        if (tasks.size() > threads_num) {
           LOG_MM_W("thread_pool", "threads({}) exceed capacity({}), cur task queue({})", threads_name.c_str(), threads_num, tasks.size());
        }
    }
    condition.notify_one();
    return res;
}

// the destructor joins all threads
inline thread_pool::~thread_pool()
{
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        stop = true;
    }

    condition.notify_all();
    for (std::thread &worker: workers) {
        worker.join();
    }
}
}

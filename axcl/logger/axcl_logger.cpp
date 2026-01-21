/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include "axcl_logger.hpp"
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <chrono>
#include <iostream>
#include <memory>
#include <vector>
#include "axcl_module_version.h"
#include "os.hpp"
#include "spdlog/async.h"

#define LOGGER_NAME "axcl_logger"

namespace axcl {

class logger_impl {
public:
    logger_impl(std::string log_path, std::size_t max_size, std::size_t max_num) noexcept {
        m_logger = spdlog::get(LOGGER_NAME);
        if (!m_logger) {
            spdlog::init_thread_pool(8192, 1, []() { xp_thr_set_name(LOGGER_NAME); });

            auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
            console_sink->set_level(spdlog::level::warn);
            console_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e][%t][%L]%v");

            try {
                auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(log_path, max_size, max_num);
                file_sink->set_level(spdlog::level::trace);
                file_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e][%t][%L]%v");

                std::vector<spdlog::sink_ptr> sinks{console_sink, file_sink};
                m_logger = std::make_shared<spdlog::async_logger>(LOGGER_NAME, sinks.begin(), sinks.end(), spdlog::thread_pool(),
                                                                  spdlog::async_overflow_policy::overrun_oldest);
                spdlog::register_logger(m_logger);
            } catch (const spdlog::spdlog_ex& ex) {
                std::cerr << "file sink initialization failed: " << ex.what() << std::endl;
                m_logger = std::make_shared<spdlog::async_logger>(LOGGER_NAME, console_sink, spdlog::thread_pool(),
                                                                  spdlog::async_overflow_policy::overrun_oldest);
                spdlog::register_logger(m_logger);
            }

            m_logger->set_level(spdlog::level::debug);
            m_logger->flush_on(spdlog::level::err);
        }
    }

    ~logger_impl() {
    }

    std::shared_ptr<spdlog::logger> get_logger() const {
        return m_logger;
    }

private:
    std::shared_ptr<spdlog::logger> m_logger;
};

void logger::flush_every(uint32_t seconds) {
    spdlog::flush_every(std::chrono::seconds(seconds));
}

spdlog::level::level_enum logger::get_level(int32_t lv) {
    spdlog::level::level_enum level;
    switch (lv) {
        case 0:
            level = spdlog::level::trace;
            break;
        case 1:
            level = spdlog::level::debug;
            break;
        case 2:
            level = spdlog::level::info;
            break;
        case 3:
            level = spdlog::level::warn;
            break;
        case 4:
            level = spdlog::level::err;
            break;
        case 5:
            level = spdlog::level::critical;
            break;
        case 6:
            level = spdlog::level::off;
            break;
        default:
            level = spdlog::level::warn;
            break;
    }

    return level;
}

spdlog::logger* logger::get_instance(std::string log_path, std::size_t max_size, std::size_t max_num) {
    static logger_impl instance(log_path, max_size, max_num);
    return instance.get_logger().get();
}

}  // namespace axcl
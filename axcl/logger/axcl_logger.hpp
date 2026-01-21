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
#ifndef SPDLOG_COMPILED_LIB
/**
 * SPDLOG_COMPILED_LIB must be defined before #include "spdlog/spdlog.h" to avoid head-only.
 * refer to: https://github.com/gabime/spdlog/discussions/3208
*/
#define SPDLOG_COMPILED_LIB
#endif

#include "spdlog/spdlog.h"
#include <string>
#include "axcl_base.h"

namespace axcl {

class AXCL_EXPORT logger {
public:
    static spdlog::logger *get_instance(std::string log_path = "/tmp/axcl/axcl_logs.txt", std::size_t max_size = 5 * 1024 * 1024,
                                        std::size_t max_num = 5);

    static void flush_every(uint32_t seconds);
    static spdlog::level::level_enum get_level(int32_t lv);

private:
    logger() = delete;
    logger(const logger &) = delete;
    logger &operator=(const logger &) = delete;
};

}  // namespace axcl

// clang-format off
#define AXCL_LOGGER axcl::logger::get_instance()

#define LOG_M_D(tag, fmt, ...)  AXCL_LOGGER->log(spdlog::level::debug,    "[{}]: " fmt, tag, ##__VA_ARGS__)
#define LOG_M_I(tag, fmt, ...)  AXCL_LOGGER->log(spdlog::level::info,     "[{}]: " fmt, tag, ##__VA_ARGS__)
#define LOG_M_W(tag, fmt, ...)  AXCL_LOGGER->log(spdlog::level::warn,     "[{}]: " fmt, tag, ##__VA_ARGS__)
#define LOG_M_E(tag, fmt, ...)  AXCL_LOGGER->log(spdlog::level::err,      "[{}]: " fmt, tag, ##__VA_ARGS__)
#define LOG_M_C(tag, fmt, ...)  AXCL_LOGGER->log(spdlog::level::critical, "[{}]: " fmt, tag, ##__VA_ARGS__)

#define LOG_MM_D(tag, fmt, ...) AXCL_LOGGER->log(spdlog::level::debug,    "[{}][{}][{}]: " fmt, tag, __func__, __LINE__, ##__VA_ARGS__)
#define LOG_MM_I(tag, fmt, ...) AXCL_LOGGER->log(spdlog::level::info,     "[{}][{}][{}]: " fmt, tag, __func__, __LINE__, ##__VA_ARGS__)
#define LOG_MM_W(tag, fmt, ...) AXCL_LOGGER->log(spdlog::level::warn,     "[{}][{}][{}]: " fmt, tag, __func__, __LINE__, ##__VA_ARGS__)
#define LOG_MM_E(tag, fmt, ...) AXCL_LOGGER->log(spdlog::level::err,      "[{}][{}][{}]: " fmt, tag, __func__, __LINE__, ##__VA_ARGS__)
#define LOG_MM_C(tag, fmt, ...) AXCL_LOGGER->log(spdlog::level::critical, "[{}][{}][{}]: " fmt, tag, __func__, __LINE__, ##__VA_ARGS__)
// clang-format on
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

#include <array>
#include <vector>
#include <type_traits>

#include "common/split.hpp"

namespace common {

template <typename T, size_t N>
bool parse_args(const std::string& argument_string, std::array<T, N>& arguments, const std::string& delimiter = ",") {
    static_assert(std::is_arithmetic_v<T>, "parse_args only supports numeric types.");

    const std::vector<std::string> result = split(argument_string, delimiter);

    if (N != result.size()) {
        return false;
    }

    for (size_t i = 0; i < N; i++) {
        if constexpr (std::is_integral_v<T>) {
            arguments[i] = std::stoi(result[i]);
        } else if constexpr (std::is_floating_point_v<T>) {
            arguments[i] = std::stof(result[i]);
        }
        else {
            return false;
        }
    }

    return true;
}

}

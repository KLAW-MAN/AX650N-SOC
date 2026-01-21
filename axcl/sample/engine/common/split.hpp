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

#include <string>
#include <vector>

namespace common {

inline std::vector<std::string> split(const std::string& content, const std::string& delimiter) {
    std::vector<std::string> result;
    size_t pos1 = 0, pos2;

    while ((pos2 = content.find(delimiter, pos1)) != std::string::npos) {
        result.emplace_back(content, pos1, pos2 - pos1);
        pos1 = pos2 + delimiter.length();
    }

    if (pos1 < content.length()) {
        result.emplace_back(content, pos1);
    }

    return result;
}

}

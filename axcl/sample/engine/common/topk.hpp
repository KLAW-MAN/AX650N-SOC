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

#include <algorithm>
#include <vector>

namespace classification {

inline std::vector<std::pair<float, int>> topK(const float* data, const size_t size, const int topK, const bool reverse = false) {
    std::vector<std::pair<float, int>> indexed_data(size);
    for (size_t i = 0; i < size; ++i) {
        indexed_data[i] = {data[i], static_cast<int>(i)};
    }

    auto compare_func = [](const std::pair<float, int>& a, const std::pair<float, int>& b) -> bool {
        return a.first > b.first;
    };

    std::sort(indexed_data.begin(), indexed_data.end(), compare_func);

    if (reverse) {
        std::reverse(indexed_data.begin(), indexed_data.end());
    }

    std::vector<std::pair<float, int>> results(topK);
    for (int i = 0; i < topK; ++i) {
        results[i] = indexed_data[i];
    }

    return results;
}

}

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

#include <cstdio>
#include <algorithm>
#include <numeric>
#include <vector>

inline void display_costs(std::vector<float>& elapsed_times, float& cost_min, float& cost_max, float& cost_average) {
    if (100 <= elapsed_times.size()) {
        std::sort(elapsed_times.begin(), elapsed_times.end(), [](const float& a, const float& b){ return b > a; });
        cost_min = elapsed_times[0];
        cost_max = elapsed_times[elapsed_times.size() - 1];
        cost_average = std::accumulate(elapsed_times.begin(), elapsed_times.end(), 0.f) / static_cast<float>(elapsed_times.size());

        const float cost_median = elapsed_times[elapsed_times.size() / 2];
        const float percent_5 = elapsed_times[(elapsed_times.size() * 5) / 100];
        const float percent_90 = elapsed_times[(elapsed_times.size() * 90) / 100];
        const float percent_95 = elapsed_times[(elapsed_times.size() * 95) / 100];
        const float percent_99 = elapsed_times[(elapsed_times.size() * 99) / 100];

        fprintf(stdout, "  ---------------------------------------------------------------------------\n");
        fprintf(stdout, "  min = %7.3f ms   max = %7.3f ms   avg = %7.3f ms  median = %7.3f ms\n", cost_min, cost_max, cost_average, cost_median);
        fprintf(stdout, "   5%% = %7.3f ms   90%% = %7.3f ms   95%% = %7.3f ms     99%% = %7.3f ms\n", percent_5, percent_90, percent_95, percent_99);
        fprintf(stdout, "  ---------------------------------------------------------------------------\n\n");
        fflush(stdout);
    }
    else {
        auto [fst, snd] = std::minmax_element(elapsed_times.begin(), elapsed_times.end());
        cost_min = *fst;
        cost_max = *snd;
        cost_average = std::accumulate(elapsed_times.begin(), elapsed_times.end(), 0.f) / static_cast<float>(elapsed_times.size());
        fprintf(stdout, "  ------------------------------------------------------\n");
        fprintf(stdout, "  min = %7.3f ms   max = %7.3f ms   avg = %7.3f ms\n", cost_min, cost_max, cost_average);
        fprintf(stdout, "  ------------------------------------------------------\n\n");
    }
}

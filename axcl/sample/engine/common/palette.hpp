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

#include <opencv2/opencv.hpp>

#include <vector>

namespace common {

inline std::vector<cv::Vec3b> palette(const int n) {
    std::vector<cv::Vec3b> colors;

    const auto N = std::cbrt(static_cast<float>(n) / 2);
    float hue = 0.f, saturation = 0.f, value = 0.f;
    for (int i = 0; i < static_cast<int>(std::ceil(N)); ++i) {
        hue = (static_cast<float>(i) * 180.f) / N;
        for (int j = 0; j < static_cast<int>(std::ceil(N)); ++j) {
            saturation = 255.f - (static_cast<float>(i) * 128.f / N);
        }
        for (int j = 0; j < static_cast<int>(std::ceil(N)); ++j) {
            value = 255.f - (static_cast<float>(i) * 128.f / N);
        }

        cv::Mat hsv(1, 1, CV_8UC3, cv::Scalar(hue, saturation, value));
        cv::Mat rgb;
        cv::cvtColor(hsv, rgb, cv::COLOR_HSV2BGR);

        if (colors.size() < static_cast<size_t>(n)) {
            colors.push_back(rgb.at<cv::Vec3b>(0, 0));
        }
    }
    return colors;
}

inline cv::Vec3b get_complementary_color(const cv::Vec3b& src) {
    cv::Vec3b dst;
    dst[0] = 255 - src[0];
    dst[1] = 255 - src[1];
    dst[2] = 255 - src[2];
    return dst;
}

} // namespace common

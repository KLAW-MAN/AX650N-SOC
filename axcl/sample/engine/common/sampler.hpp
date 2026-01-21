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

#include <array>
#include <cmath>

namespace preprocess {

enum class crop_type {
    normal,
    letterbox,
    center_crop,
    imagenet,
};

enum class resize_type {
    nearest = 0,
    linear = 1,
    cubic = 2,
};


class sampler {
public:
    explicit sampler(const crop_type crop_type = crop_type::normal, const resize_type resize_type = resize_type::linear, const bool swap_rb = false)
        : crop_type_(crop_type), resize_type_(resize_type), swap_rb_(swap_rb) {}

    void operator()(cv::Mat& src, cv::Mat& dst) {
        resize(src, dst);
    }

    static cv::Size get_letterbox_size(const cv::Mat& src, const cv::Mat& dst) {
        return get_letterbox_rect(src, dst).size();
    }

    static cv::Size get_center_crop_size(const cv::Mat& src, const cv::Mat& dst) {
        return get_center_crop_rect(src, dst).size();
    }

private:
    void resize(cv::Mat& src, cv::Mat& dst) {
        switch (crop_type_) {
        case crop_type::normal:
            resize_normal(dst, src);
            break;
        case crop_type::letterbox:
            resize_letterbox(dst, src);
            break;
        case crop_type::center_crop:
            resize_center_crop(dst, src);
            break;
        case crop_type::imagenet:
            resize_imagenet(dst, src);
            break;
        }
    }

    [[nodiscard]] cv::Size get_roi_size() const {
        return roi_size_;
    }

    void set_background(const cv::Scalar& background) {
        background_ = background;
    }

    void resize_normal(cv::Mat& dst, cv::Mat& src) const {
        const cv::Size size = dst.size();
        cv::resize(dst, src, size, 0.f, 0.f, static_cast<int>(resize_type_));
        if (swap_rb_) {
            cv::cvtColor(dst, dst, cv::COLOR_BGR2RGB);
        }
    }

    static cv::Rect get_letterbox_rect(const cv::Mat& src, const cv::Mat& dst) {
        const auto src_size = src.size();
        const auto dst_size = dst.size();

        cv::Rect roi_rect;

        const auto width_ratio = static_cast<float>(dst.cols) / static_cast<float>(src.cols);
        const auto height_ratio = static_cast<float>(dst.rows) / static_cast<float>(src.rows);

        if (width_ratio < height_ratio) {
            const int new_height = static_cast<int>(std::roundf(static_cast<float>(src_size.height) * width_ratio));
            roi_rect = cv::Rect(0, (dst_size.height - new_height) / 2, dst_size.width, new_height);
        } else {
            const int new_width = static_cast<int>(std::roundf(static_cast<float>(src_size.width) * height_ratio));
            roi_rect = cv::Rect((dst_size.width - new_width) / 2, 0, new_width, dst_size.height);
        }

        return roi_rect;
    }

    static std::array<cv::Rect, 2> get_letterbox_outside_roi(const cv::Mat& mat, const cv::Rect& roi_rect) {
        std::array<cv::Rect, 2> fill_rects;

        // if y not started at zero, means top and bottom areas will be filled
        if (roi_rect.y > 0) {
            fill_rects[0] = cv::Rect(0, 0, mat.cols, roi_rect.y);
            fill_rects[1] = cv::Rect(0, roi_rect.y + roi_rect.height, mat.cols, mat.rows - roi_rect.y - roi_rect.height);
        }

        // if x not started at zero, means left and right areas will be filled
        if (roi_rect.x > 0) {
            fill_rects[0] = cv::Rect(0, 0, roi_rect.x, mat.rows);
            fill_rects[1] = cv::Rect(roi_rect.x + roi_rect.width, 0, mat.cols - roi_rect.x - roi_rect.width, mat.rows);
        }

        return fill_rects;
    }

    void resize_letterbox(cv::Mat& dst, const cv::Mat& src) {
        const auto roi_rect = get_letterbox_rect(src, dst);
        const auto fill_rects = get_letterbox_outside_roi(dst, roi_rect);

        const cv::Mat dst_roi = dst(roi_rect);
        cv::resize(src, dst_roi, dst_roi.size(), 0.f, 0.f, static_cast<int>(resize_type_));

        if (swap_rb_) {
            cv::cvtColor(dst_roi, dst_roi, cv::COLOR_BGR2RGB);
        }

        cv::rectangle(dst, fill_rects[0], background_, cv::FILLED);
        cv::rectangle(dst, fill_rects[1], background_, cv::FILLED);
    }

    static cv::Rect get_center_crop_rect(const cv::Mat& src, const cv::Mat& dst) {
        const auto src_size = src.size();
        const auto dst_size = dst.size();

        cv::Rect roi_rect;

        const auto width_ratio = static_cast<float>(src.cols) / static_cast<float>(dst.cols);
        const auto height_ratio = static_cast<float>(src.rows) / static_cast<float>(dst.rows);

        if (width_ratio > height_ratio) {
            roi_rect.width = static_cast<int>(std::roundf(static_cast<float>(dst_size.height) * height_ratio));
            roi_rect.height = src_size.height;
            roi_rect.x = (src_size.width - roi_rect.width) / 2;
            roi_rect.y = 0;
        } else {
            roi_rect.width = src_size.width;
            roi_rect.height = static_cast<int>(std::roundf(static_cast<float>(dst_size.width) * width_ratio));
            roi_rect.x = 0;
            roi_rect.y = (src_size.height - roi_rect.height) / 2;
        }

        return roi_rect;
    }

    void resize_center_crop(cv::Mat& dst, const cv::Mat& src) {
        const cv::Rect roi_rect = get_center_crop_rect(src, dst);

        const cv::Mat src_roi = src(roi_rect);
        cv::resize(src_roi, dst, dst.size(), 0.f, 0.f, static_cast<int>(resize_type_));

        if (swap_rb_) {
            cv::cvtColor(dst, dst, cv::COLOR_BGR2RGB);
        }
    }

    void resize_imagenet(cv::Mat& dst, const cv::Mat& src) {
        const cv::Rect roi_rect = get_center_crop_rect(src, dst);

        const cv::Mat src_roi = src(roi_rect);
        cv::Mat mid_256(256, 256, CV_8UC3);
        cv::resize(src_roi, mid_256, mid_256.size(), 0.f, 0.f, static_cast<int>(resize_type_));

        const cv::Rect roi_rect_256((mid_256.cols - dst.cols) / 2, (mid_256.rows - dst.rows) / 2, dst.cols, dst.rows);
        mid_256(roi_rect_256).copyTo(dst);

        if (swap_rb_) {
            cv::cvtColor(dst, dst, cv::COLOR_BGR2RGB);
        }
    }

    crop_type crop_type_{crop_type::normal};
    resize_type resize_type_{resize_type::nearest};
    cv::Size roi_size_{0, 0};
    cv::Scalar background_{0, 0, 0};
    bool swap_rb_{false};
};

} // namespace preprocess

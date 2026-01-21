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

#include "common/sigmoid.hpp"

#include <vector>
#include <stack>

namespace detection {

struct box {
    float w, h;
};

struct rect {
    float x, y, w, h;
};

struct object {
    detection::rect rect;
    float prob;
    int label;
};

// static std::vector<box> generate_anchors(const int stride, const std::vector<float>& ratios, const std::vector<float>& scales) {
//     std::vector<box> anchors;
//     for (const float scale : scales) {
//         for (const float ratio : ratios) {
//             const float w = static_cast<float>(stride) * scale * std::sqrt(ratio);
//             const float h = static_cast<float>(stride) * scale / std::sqrt(ratio);
//             anchors.emplace_back(box{w, h});
//         }
//     }
//     return anchors;
// }

class anchor_based {
public:
    anchor_based(const int class_num, const int letterbox_h, const int letterbox_w, const float prob_threshold = 0.45f, const float iou_threshold = 0.45f)
        : class_num_(class_num), input_h_(letterbox_h), input_w_(letterbox_w), prob_threshold_(prob_threshold), iou_threshold_(iou_threshold) {
        prob_logic_threshold_ = -1.0f * std::log((1.0f / prob_threshold) - 1.0f);
    }

    void reset() {
        this->objects_.clear();
    }

    void proposal(const float* feat, const int stride, const std::vector<box>& anchors) {
        proposal(feat, stride, anchors, input_h_, input_w_);
    }

    std::vector<object> pick(const int src_h, const int src_w) {
        return pick(this->objects_, iou_threshold_, input_h_, input_w_, src_h, src_w);
    }

protected:
    static float iou(const rect& a, const rect& b) {
        const float inter_x1 = std::max(a.x, b.x);
        const float inter_y1 = std::max(a.y, b.y);
        const float inter_x2 = std::min(a.x + a.w, b.x + b.w);
        const float inter_y2 = std::min(a.y + a.h, b.y + b.h);

        const float inter_area = std::max(0.0f, inter_x2 - inter_x1) * std::max(0.0f, inter_y2 - inter_y1);
        const float a_area = a.w * a.h;
        const float b_area = b.w * b.h;

        return inter_area / (a_area + b_area - inter_area);
    }

    static std::vector<object> nms(const std::vector<object>& boxes, const float iou_threshold) {
        std::vector<object> result;
        std::vector suppressed(boxes.size(), false);

        for (size_t i = 0; i < boxes.size(); ++i) {
            if (suppressed[i]) continue;

            result.push_back(boxes[i]);
            for (size_t j = i + 1; j < boxes.size(); ++j) {
                if (iou(boxes[i].rect, boxes[j].rect) > iou_threshold) {
                    suppressed[j] = true;
                }
            }
        }

        return result;
    }

    static void sort(std::vector<object>& objects) {
        if (objects.empty()) { return; }

        std::stack<std::pair<int, int>> stack;
        stack.emplace(0, objects.size() - 1);

        while (!stack.empty()) {
            int left = stack.top().first;
            int right = stack.top().second;
            stack.pop();

            int i = left;
            int j = right;
            const float p = objects[(left + right) / 2].prob;

            while (i <= j) {
                while (objects[i].prob > p) { i++; }
                while (objects[j].prob < p) { j--; }

                if (i <= j) {
                    std::swap(objects[i], objects[j]);
                    i++;
                    j--;
                }
            }

            if (left < j) { stack.emplace(left, j); }
            if (i < right) { stack.emplace(i, right); }
        }
    }

    void proposal(const float* feat, const int stride, const std::vector<box>& anchors, const int input_h, const int input_w) {
        auto feature_ptr = feat;
        const int feat_h = input_h / stride;
        const int feat_w = input_w / stride;

        for (int h = 0; h <= feat_h - 1; h++) {
            for (int w = 0; w <= feat_w - 1; w++) {
                for (size_t a = 0; a <= anchors.size() - 1; a++) {
                    if (feature_ptr[4] < prob_logic_threshold_) {
                        feature_ptr += (class_num_ + 5);
                        continue;
                    }

                    //process cls score
                    int class_index = 0;
                    float class_score = -FLT_MAX;
                    for (int s = 0; s <= class_num_ - 1; s++) {
                        if (const float score = feature_ptr[s + 5]; score > class_score) {
                            class_index = s;
                            class_score = score;
                        }
                    }

                    //process box score
                    const float box_score = feature_ptr[4];
                    if (const float final_score = sigmoid(box_score) * sigmoid(class_score); final_score >= prob_threshold_) {
                        const float dx = sigmoid(feature_ptr[0]);
                        const float dy = sigmoid(feature_ptr[1]);
                        const float dw = sigmoid(feature_ptr[2]);
                        const float dh = sigmoid(feature_ptr[3]);
                        const float pred_cx = (dx * 2.f - 0.5f + static_cast<float>(w)) * static_cast<float>(stride);
                        const float pred_cy = (dy * 2.f - 0.5f + static_cast<float>(h)) * static_cast<float>(stride);
                        const float anchor_w = anchors[a].w;
                        const float anchor_h = anchors[a].h;
                        const float pred_w = dw * dw * 4.f * anchor_w;
                        const float pred_h = dh * dh * 4.f * anchor_h;
                        const float x0 = pred_cx - pred_w * 0.5f;
                        const float y0 = pred_cy - pred_h * 0.5f;
                        const float x1 = pred_cx + pred_w * 0.5f;
                        const float y1 = pred_cy + pred_h * 0.5f;

                        object obj{{x0, y0, x1 - x0, y1 - y0}, final_score, class_index};
                        this->objects_.push_back(obj);
                    }

                    feature_ptr += (class_num_ + 5);
                }
            }
        }
    }

    static std::vector<object> pick(std::vector<object>& proposals, const float nms_threshold, const int input_h, const int input_w, const int src_h, const int src_w)
    {
        std::vector<object> objects;

        sort(proposals);
        const std::vector<object> picked = nms(proposals, nms_threshold);

        /* yolov5 draw the result */
        float scale_letterbox;
        const auto scale_h = static_cast<float>(input_h) / static_cast<float>(src_h);
        const auto scale_w = static_cast<float>(input_w)/ static_cast<float>(src_w);
        if (scale_h < scale_w) {
            scale_letterbox = scale_h;
        }
        else {
            scale_letterbox =scale_w;
        }

        const int resize_h = static_cast<int>(scale_letterbox * static_cast<float>(src_h));
        const int resize_w = static_cast<int>(scale_letterbox * static_cast<float>(src_w));

        const int tmp_h = (input_h - resize_h) / 2;
        const int tmp_w = (input_w - resize_w) / 2;

        // const float ratio_x = static_cast<float>(src_h) / static_cast<float>(resize_h);
        // const float ratio_y = static_cast<float>(src_w) / static_cast<float>(resize_w);

        const float ratio_y = static_cast<float>(src_h) / static_cast<float>(resize_h);
        const float ratio_x = static_cast<float>(src_w) / static_cast<float>(resize_w);

        const auto count = picked.size();

        objects.resize(count);
        for (size_t i = 0; i < count; i++)
        {
            objects[i] = picked[i];
            float x0 = (objects[i].rect.x);
            float y0 = (objects[i].rect.y);
            float x1 = (objects[i].rect.x + objects[i].rect.w);
            float y1 = (objects[i].rect.y + objects[i].rect.h);

            x0 = (x0 - static_cast<float>(tmp_w)) * ratio_x;
            y0 = (y0 - static_cast<float>(tmp_h)) * ratio_y;
            x1 = (x1 - static_cast<float>(tmp_w)) * ratio_x;
            y1 = (y1 - static_cast<float>(tmp_h)) * ratio_y;

            x0 = std::max(std::min(x0, static_cast<float>(src_w - 1)), 0.f);
            y0 = std::max(std::min(y0, static_cast<float>(src_h - 1)), 0.f);
            x1 = std::max(std::min(x1, static_cast<float>(src_w - 1)), 0.f);
            y1 = std::max(std::min(y1, static_cast<float>(src_h - 1)), 0.f);

            objects[i].rect.x = x0;
            objects[i].rect.y = y0;
            objects[i].rect.w = x1 - x0;
            objects[i].rect.h = y1 - y0;
        }

        return objects;
    }

private:
    int class_num_;
    const int input_h_;
    const int input_w_;
    float prob_threshold_;
    float prob_logic_threshold_;
    float iou_threshold_;
    std::vector<object> objects_;
};

} // namespace detection

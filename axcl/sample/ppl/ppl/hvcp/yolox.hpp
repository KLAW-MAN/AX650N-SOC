/**************************************************************************************************
 *
 * Copyright (c) 2019-2025 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#pragma once

#include <array>
#include <cmath>
#include <vector>

#include "types.hpp"

#define HVCP_TAG "axclite-hvcp"

#define HVCFP_CLASS_NUM 4
#define HVCFP_DEFAULT_IO_DEPTH 4
#define HVCFP_MAX_IO_DEPTH 8
#define HVCFP_DEFAULT_POST_THREAD_NUMS 1

namespace axclite {

struct BBoxRect {
    float score;
    float xmin;
    float ymin;
    float xmax;
    float ymax;
    float area;
    int label;
};

// code from skel
static const int DEFAULT_IMG_H = 576;
static const int DEFAULT_IMG_W = 1024;

static const int NUM_CLASS = HVCFP_CLASS_NUM;
//static const char *CLASS_NAMES[] = {"body", "vehicle", "cycle", "plate"};

static const float PROB_THRESHOLD_BODY_MIN = 0.3f;
static const float PROB_THRESHOLD_VEHICLE_MIN = 0.3f;
static const float PROB_THRESHOLD_CYCLE_MIN = 0.3f;
static const float PROB_THRESHOLD_PLATE_MIN = 0.3f;
static const float PROB_THRESHOLD_MIN = 0.3f;
static const float NMS_THRESHOLD = 0.45f;

static const int DEFAULT_IO_DEPTH = HVCFP_DEFAULT_IO_DEPTH;
static const int MAX_IO_DEPTH = HVCFP_MAX_IO_DEPTH;
static const int DEFAULT_POST_THREAD_NUMS = HVCFP_DEFAULT_POST_THREAD_NUMS;

typedef struct axSKEL_FILTER_CONFIG_T {
    AX_U32 nWidth;
    AX_U32 nHeight;
    AX_F32 fConfidence;
} AX_SKEL_FILTER_CONFIG_T;

static const auto MIN_FILTER_CONFIG = std::array<AX_SKEL_FILTER_CONFIG_T, NUM_CLASS> {
    AX_SKEL_FILTER_CONFIG_T{0, 0, PROB_THRESHOLD_BODY_MIN},
    AX_SKEL_FILTER_CONFIG_T{0, 0, PROB_THRESHOLD_VEHICLE_MIN},
    AX_SKEL_FILTER_CONFIG_T{0, 0, PROB_THRESHOLD_CYCLE_MIN},
    AX_SKEL_FILTER_CONFIG_T{0, 0, PROB_THRESHOLD_PLATE_MIN}
};

static const int TRACK_BUFFER = 30;  // frame number of tracking states buffers
static const float HIGH_DET_THRESH = 0.5f;  // 0.5f > m_track_thresh as high(1st)
static const float NEW_TRACK_THRESH = PROB_THRESHOLD_MIN;//(HIGH_DET_THRESH + 0.1f);   // > m_high_thresh as new track

// 3 Matching thresholds
static const float HIGH_MATCH_THRESH = 0.8f;  // 0.8f first match threshold
static const float LOW_MATCH_THRESH = 0.5f;  // 0.5f second match threshold
static const float UNCONFIRMED_MATCH_THRESH = 0.7f;  // 0.7: unconfirmed track match to remain dets


typedef struct {
    int grid0;
    int grid1;
    int stride;
} GridAndStride;

typedef struct {
    float x;
    float y;
    float score;
} ai_point_t;

typedef struct {
    Rect_<float> rect;
    int label;
    float prob;
    std::vector<ai_point_t> points;
} Object;

inline void generate_yolox_proposals(std::vector<GridAndStride> grid_strides, float *feat_ptr, const std::array<AX_SKEL_FILTER_CONFIG_T, NUM_CLASS> &stFilterArr, std::vector<Object> &objects, int wxc) {
    //const int num_grid = 3549;
    const int num_class = NUM_CLASS;
    const int num_anchors = grid_strides.size();

    float* feat_ptr_objectness = feat_ptr + 4 * wxc;
    float* feat_ptr_x_center = feat_ptr;
    float* feat_ptr_y_center = feat_ptr + wxc;
    float* feat_ptr_w = feat_ptr + 2 * wxc;
    float* feat_ptr_h = feat_ptr + 3 * wxc;

    for (int anchor_idx = 0; anchor_idx < num_anchors; anchor_idx++) {
        float box_objectness = feat_ptr_objectness[anchor_idx];
        if (box_objectness >= PROB_THRESHOLD_MIN) {
            for (int class_idx = 0; class_idx < num_class; class_idx ++) {
                float box_cls_score = feat_ptr[(5 + class_idx) * wxc + anchor_idx];
                if (box_cls_score >= stFilterArr[class_idx].fConfidence) {
                    float box_prob = box_objectness * box_cls_score;
                    if (box_prob >= stFilterArr[class_idx].fConfidence) {
                        Object obj;
                        // printf("%d,%d\n",num_anchors,anchor_idx);
                        const int grid0 = grid_strides[anchor_idx].grid0; // 0
                        const int grid1 = grid_strides[anchor_idx].grid1; // 0
                        const int stride = grid_strides[anchor_idx].stride; // 8
                        // yolox/models/yolo_head.py decode logic
                        //  outputs[..., :2] = (outputs[..., :2] + grids) * strides
                        //  outputs[..., 2:4] = torch.exp(outputs[..., 2:4]) * strides
                        float x_center = (feat_ptr_x_center[anchor_idx] + grid0) * stride;
                        float y_center = (feat_ptr_y_center[anchor_idx] + grid1) * stride;
                        float w = exp(feat_ptr_w[anchor_idx]) * stride;
                        float h = exp(feat_ptr_h[anchor_idx]) * stride;
                        float x0 = x_center - w * 0.5f;
                        float y0 = y_center - h * 0.5f;
                        obj.rect.x = x0;
                        obj.rect.y = y0;
                        obj.rect.width = w;
                        obj.rect.height = h;
                        obj.label = class_idx;
                        obj.prob = box_prob;

                        if (w >= MIN_FILTER_CONFIG[class_idx].nWidth
                            && h >= MIN_FILTER_CONFIG[class_idx].nHeight) {
                            objects.push_back(obj);
                        }
                    }
                }
            }
        }
    } // point anchor loop
}

inline float intersection_area(const Object& a, const Object& b) {
    Rect_<float> inter = a.rect & b.rect;
    return inter.area();
}

inline void qsort_descent_inplace(std::vector<Object>& faceobjects, int left, int right) {
    int i = left;
    int j = right;
    float p = faceobjects[(left + right) / 2].prob;

    while (i <= j) {
        while (faceobjects[i].prob > p) i++;

        while (faceobjects[j].prob < p) j--;

        if (i <= j) {
            // swap
            std::swap(faceobjects[i], faceobjects[j]);

            i++;
            j--;
        }
    }
    //#pragma omp parallel sections
    {
        //#pragma omp section
        {
            if (left < j) qsort_descent_inplace(faceobjects, left, j);
        }
        //#pragma omp section
        {
            if (i < right) qsort_descent_inplace(faceobjects, i, right);
        }
    }
}

inline void qsort_descent_inplace(std::vector<Object>& faceobjects) {
    if (faceobjects.empty()) return;

    qsort_descent_inplace(faceobjects, 0, faceobjects.size() - 1);
}

inline void nms_sorted_bboxes(const std::vector<Object>& faceobjects, std::vector<int>& picked, float nms_threshold) {
    picked.clear();

    const int n = faceobjects.size();

    std::vector<float> areas(n);
    for (int i = 0; i < n; i++) {
        areas[i] = faceobjects[i].rect.area();
    }

    for (int i = 0; i < n; i++) {
        const Object& a = faceobjects[i];

        int keep = 1;
        for (int j = 0; j < (int)picked.size(); j++) {
            const Object& b = faceobjects[picked[j]];

            // intersection over union
            float inter_area = intersection_area(a, b);
            float union_area = areas[i] + areas[picked[j]] - inter_area;
            // float IoU = inter_area / union_area
            if (inter_area / union_area > nms_threshold) keep = 0;
        }

        if (keep) picked.push_back(i);
    }
}

inline void get_out_bbox(std::vector<Object>& proposals, std::vector<Object>& objects, const float nms_threshold, int letterbox_rows,
                      int letterbox_cols, int src_rows, int src_cols) {
    qsort_descent_inplace(proposals);
    std::vector<int> picked;
    nms_sorted_bboxes(proposals, picked, nms_threshold);

    /* yolov5 draw the result */
    float scale_letterbox;
    int resize_rows;
    int resize_cols;
    if ((letterbox_rows * 1.0 / src_rows) < (letterbox_cols * 1.0 / src_cols)) {
        scale_letterbox = (float)(letterbox_rows * 1.0 / src_rows);
    } else {
        scale_letterbox = (float)(letterbox_cols * 1.0 / src_cols);
    }
    resize_cols = int(scale_letterbox * src_cols);
    resize_rows = int(scale_letterbox * src_rows);

    int tmp_h = (letterbox_rows - resize_rows) / 2;
    int tmp_w = (letterbox_cols - resize_cols) / 2;

    float ratio_x = (float)src_rows / resize_rows;
    float ratio_y = (float)src_cols / resize_cols;

    int count = picked.size();

    objects.resize(count);
    for (int i = 0; i < count; i++) {
        objects[i] = proposals[picked[i]];
        float x0 = (objects[i].rect.x);
        float y0 = (objects[i].rect.y);
        float x1 = (objects[i].rect.x + objects[i].rect.width);
        float y1 = (objects[i].rect.y + objects[i].rect.height);

        x0 = (x0 - tmp_w) * ratio_x;
        y0 = (y0 - tmp_h) * ratio_y;
        x1 = (x1 - tmp_w) * ratio_x;
        y1 = (y1 - tmp_h) * ratio_y;

        x0 = std::max(std::min(x0, (float)(src_cols - 1)), 0.f);
        y0 = std::max(std::min(y0, (float)(src_rows - 1)), 0.f);
        x1 = std::max(std::min(x1, (float)(src_cols - 1)), 0.f);
        y1 = std::max(std::min(y1, (float)(src_rows - 1)), 0.f);

        objects[i].rect.x = x0;
        objects[i].rect.y = y0;
        objects[i].rect.width = x1 - x0;
        objects[i].rect.height = y1 - y0;
    }
}

inline void generate_grids_and_stride(std::vector<GridAndStride>& grid_strides, const uint32_t target_w, const uint32_t target_h, std::vector<int>& strides) {
    for (auto stride : strides) {
        int num_grid_w = target_w / stride;
        int num_grid_h = target_h / stride;
        for (int g1 = 0; g1 < num_grid_h; g1++) {
            for (int g0 = 0; g0 < num_grid_w; g0++) {
                GridAndStride gs;
                gs.grid0 = g0;
                gs.grid1 = g1;
                gs.stride = stride;
                grid_strides.push_back(gs);
            }
        }
    }
}

inline void CreateGridStride(std::vector<std::vector<GridAndStride>>& grid_strides, const uint32_t ratio_count, const uint32_t letterbox_rows, const uint32_t letterbox_cols) {
    std::vector<std::vector<int>> stride = {{8}, {16}, {32}};
    for (uint32_t i = 0; i < ratio_count; ++i) {
        generate_grids_and_stride(grid_strides[i], letterbox_cols, letterbox_rows, stride[i]);
    }
}


inline void hvcp_post_process(axcl_ppl_hvcp_output& output, const hvcp_input_image_info& info, const void* io_info, const std::vector<void*>& buffers, const uint32_t src_h, const uint32_t src_w) {
    std::vector<Object> proposals;
    std::vector<Object> object_bbox;

    const auto output_count = axclrtEngineGetNumOutputs(const_cast<axclrtEngineIOInfo>(io_info));
    std::vector grid_strides(3, std::vector<GridAndStride>());
    CreateGridStride(grid_strides, output_count, info.height, info.width);

    for (uint32_t i = 0; i < output_count; ++i) {
        axclrtEngineIODims dims{};
        if (const auto ret = axclrtEngineGetOutputDims(const_cast<axclrtEngineIOInfo>(io_info), 0, i, &dims); AXCL_SUCC != ret) {
            LOG_MM_E(HVCP_TAG, "axclrtEngineGetOutputDims() fail, ret = {:#x}", static_cast<uint32_t>(ret));
            return;
        }
        const auto wxc = dims.dims[2] * dims.dims[3];
        const auto ptr = buffers[i];
        generate_yolox_proposals(grid_strides[i], static_cast<float *>(ptr), MIN_FILTER_CONFIG, proposals, wxc);
    }
    get_out_bbox(proposals, object_bbox, NMS_THRESHOLD, info.height, info.width, src_h, src_w);

    output.count = std::min<AX_U32>(MAX_HVCP_COUNT, object_bbox.size());
    for (uint32_t i = 0; i < output.count; ++i) {
        output.item[i].confidence = object_bbox[i].prob;
        output.item[i].box.x = (AX_U32)object_bbox[i].rect.x;
        output.item[i].box.y = (AX_U32)object_bbox[i].rect.y;
        output.item[i].box.w = (AX_U32)object_bbox[i].rect.width;
        output.item[i].box.h = (AX_U32)object_bbox[i].rect.height;
    }
}

}  // namespace axclite

/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef __STITCH_ALGO_H__
#define __STITCH_ALGO_H__

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_STITCH_PARTITION_NUM (8)

typedef struct {
    int x;
    int y;
} stitch_point_t;

typedef struct {
    int x;
    int y;
    int w;
    int h;
} stitch_box_t;

typedef struct {
    int part_num; /* 0: just 1 partition */
    int parts[MAX_STITCH_PARTITION_NUM];
} stitch_input_partiton_t;

typedef struct {
    /* crop box from src image */
    int src_w;
    int src_h;
    stitch_box_t box;

    /* w x h resized to stitch */
    int stitch_total_w;
    int stitch_total_h;

    stitch_input_partiton_t w_parts; /* horizontal */
    stitch_input_partiton_t h_parts; /* vertical */
} stitch_input_param_t;

typedef struct {
    int ori_crop_start;
    int ori_crop_length;
    int output_length;
    int output_crop_on_left;
    int output_crop_on_right;
    int output_right_end;
    double scale;
} stitch_output_meta_partition_t;

typedef struct {
    int part_num;
    stitch_output_meta_partition_t parts[MAX_STITCH_PARTITION_NUM];
    int ori_pad_num_end;
} stitch_output_meta_t;

typedef struct {
    /* crop and resize including overlap */
    stitch_box_t box;
    int dst_w;
    int w_scale_step;
    int dst_h;
    int h_scale_step;

    /* overlap to crop out */
    stitch_box_t overlap;
} stitch_output_partiton_t;

typedef struct {
    int part_num;
    stitch_output_partiton_t parts[MAX_STITCH_PARTITION_NUM * MAX_STITCH_PARTITION_NUM];

    /* meta output, used to check algorithm result for axera engineer */
    stitch_output_meta_t w_meta;
    stitch_output_meta_t h_meta;
} stitch_output_param_t;

int stitch_algorithm(const stitch_input_param_t* input, stitch_output_param_t* output);
void stitch_dump_output(const stitch_output_param_t* output);

#ifdef __cplusplus
}
#endif

#endif /* __STITCH_ALGO_H__ */
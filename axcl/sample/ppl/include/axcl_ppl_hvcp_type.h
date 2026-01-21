/**************************************************************************************************
 *
 * Copyright (c) 2019-2025 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef __AXCL_PPL_HVCP_TYPE_H__
#define __AXCL_PPL_HVCP_TYPE_H__

#include "axcl_ppl_type.h"

#define MAX_HVCP_COUNT (64)

typedef enum {
    HVCP_TYPE_UNKNOWN = 0,
    HVCP_TYPE_HUMAN = 1,
    HVCP_TYPE_VEHICLE = 2,
    HVCP_TYPE_CYCLE = 3,
    HVCP_TYPE_PLATE = 4,
    HVCP_TYPE_BUTT
} HVCP_TYPE_E;

typedef struct {
    AX_U32 x;
    AX_U32 y;
    AX_U32 w;
    AX_U32 h;
} hvcp_box;

typedef struct {
    HVCP_TYPE_E type;
    AX_F32 confidence;
    hvcp_box box;
} axcl_ppl_hvcp_item;

typedef struct {
    AX_U64 frame_id;
    AX_U32 count;
    axcl_ppl_hvcp_item item[MAX_HVCP_COUNT];
    AX_U64 userdata;
} axcl_ppl_hvcp_output;

typedef void (*axcl_ppl_hvcp_output_callback_func)(axcl_ppl ppl, const axcl_ppl_hvcp_output* output, AX_U64 userdata);

typedef struct {
    int32_t vnpu_kind;     ///< Type of the VNPU, refer to `axclrtEngineVNpuKind`.
    int32_t affinity;      ///< NPU affinity.
    char model_path[260];  ///< Model file path.

    /* The number of frames between each inference.
       If <= 1, inference will be performed on all frames. */
    AX_U32 skip;

    /* Callback function for handling inference output results */
    axcl_ppl_hvcp_output_callback_func callback;

    /* User data to be passed to the callback */
    AX_U64 userdata;
} axcl_hvcp_param;

typedef struct {
    AX_PAYLOAD_TYPE_E payload;
    AX_U32 width;
    AX_U32 height;
    AX_VDEC_OUTPUT_ORDER_E output_order;
    AX_VDEC_DISPLAY_MODE_E display_mode;
} axcl_ppl_hvcp_vdec_attr;

typedef struct {
    /* Attributes for H.264/H.265 video decoding */
    axcl_ppl_hvcp_vdec_attr vdec;

    /* hvcp configuration parameters */
    axcl_hvcp_param hvcp_param;
} axcl_ppl_hvcp_param;

#endif /* __AXCL_PPL_HVCP_TYPE_H__ */
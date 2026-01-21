/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef __AXCL_PPL_TYPE_H__
#define __AXCL_PPL_TYPE_H__

#include "axclite.h"

#ifdef __cplusplus
extern "C" {
#endif

#define AXCL_DEF_LITE_PPL_ERR(e)           AXCL_DEF_LITE_ERR(0, (e))
#define AXCL_ERR_LITE_PPL_NULL_POINTER     AXCL_DEF_LITE_PPL_ERR(AXCL_ERR_NULL_POINTER)
#define AXCL_ERR_LITE_PPL_ILLEGAL_PARAM    AXCL_DEF_LITE_PPL_ERR(AXCL_ERR_ILLEGAL_PARAM)
#define AXCL_ERR_LITE_PPL_UNSUPPORT        AXCL_DEF_LITE_PPL_ERR(AXCL_ERR_UNSUPPORT)
#define AXCL_ERR_LITE_PPL_NOT_STARTED      AXCL_DEF_LITE_PPL_ERR(0x81)
#define AXCL_ERR_LITE_PPL_CREATE           AXCL_DEF_LITE_PPL_ERR(0x82)
#define AXCL_ERR_LITE_PPL_INVALID_PPL      AXCL_DEF_LITE_PPL_ERR(0x83)

typedef void *axcl_ppl;
#define AXCL_INVALID_PPL (0)


typedef struct {
    const char *json;    /* axcl.json path */
    AX_U32 device_index; /* IN : device index: [0, connected device number - 1] */
    AX_S32 device_id;    /* OUT: device bus number */
    AX_U32 modules;
    AX_U32 max_vdec_grp;
    AX_U32 max_venc_thd;
    AX_VDEC_HWCLK_E vdec_clk;
} axcl_ppl_init_param;

typedef enum {
    AXCL_PPL_TRANSCODE = 0, /* VDEC -> IVPS ->VENC */
    AXCL_PPL_HVCP = 1,      /* VDEC -> IVPS -> NPU(HVCP) */
    AXCL_PPL_BUTT
} axcl_ppl_type;

typedef struct {
    axcl_ppl_type ppl;
    void *param;
} axcl_ppl_param;

typedef struct {
    AX_U8 *nalu;
    AX_U32 nalu_len;
    AX_U64 pts;
    AX_U64 userdata;
} axcl_ppl_input_stream;

typedef AX_VENC_STREAM_T axcl_ppl_encoded_stream;
typedef void (*axcl_ppl_encoded_stream_callback_func)(axcl_ppl ppl, const axcl_ppl_encoded_stream *stream, AX_U64 userdata);

typedef AX_VIDEO_FRAME_T axcl_ppl_video_frame;
typedef void (*axcl_ppl_video_frame_callback_func)(axcl_ppl ppl, const axcl_ppl_video_frame* frame, AX_U64 userdata);

#ifdef __cplusplus
}
#endif

#endif /* __AXCL_PPL_TYPE_H__ */
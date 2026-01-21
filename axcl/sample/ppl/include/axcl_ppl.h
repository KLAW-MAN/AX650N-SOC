/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef __AXCL_PPL_H__
#define __AXCL_PPL_H__

#include "axcl_ppl_type.h"
#include "axcl_base.h"

AXCL_EXPORT axclError axcl_ppl_init(axcl_ppl_init_param* param);
AXCL_EXPORT axclError axcl_ppl_deinit();
AXCL_EXPORT axclError axcl_ppl_create(axcl_ppl* ppl, const axcl_ppl_param* param);
AXCL_EXPORT axclError axcl_ppl_start(axcl_ppl ppl);
AXCL_EXPORT axclError axcl_ppl_stop(axcl_ppl ppl);
AXCL_EXPORT axclError axcl_ppl_send_stream(axcl_ppl ppl, const axcl_ppl_input_stream* stream, AX_S32 timeout);
AXCL_EXPORT axclError axcl_ppl_destroy(axcl_ppl ppl);

/**
 *            name                                     attr type        default
 *  axcl.ppl.id                             [R  ]       int32_t                            increment +1 for each axcl_ppl_create
 *
 *  axcl.ppl.transcode.vdec.grp             [R  ]       int32_t                            allocated by ax_vdec.ko
 *  axcl.ppl.transcode.ivps.grp             [R  ]       int32_t                            allocated by ax_ivps.ko
 *  axcl.ppl.transcode.venc.chn             [R  ]       int32_t                            allocated by ax_venc.ko
 *
 *  the following attributes take effect BEFORE the axcl_ppl_create function is called:
 *  axcl.ppl.transcode.vdec.blk.cnt         [R/W]       uint32_t          8                depend on stream DPB size and decode mode
 *  axcl.ppl.transcode.vdec.out.depth       [R/W]       uint32_t          4                out fifo depth
 *  axcl.ppl.transcode.ivps.in.depth        [R/W]       uint32_t          4                in fifo depth
 *  axcl.ppl.transcode.ivps.out.depth       [R  ]       uint32_t          0                out fifo depth
 *  axcl.ppl.transcode.ivps.blk.cnt         [R/W]       uint32_t          4
 *  axcl.ppl.transcode.ivps.engine          [R/W]       uint32_t   AX_IVPS_ENGINE_VPP      AX_IVPS_ENGINE_VPP|AX_IVPS_ENGINE_VGP|AX_IVPS_ENGINE_TDP
 *  axcl.ppl.transcode.venc.in.depth        [R/W]       uint32_t          4                in fifo depth
 *  axcl.ppl.transcode.venc.out.depth       [R/W]       uint32_t          4                out fifo depth
 *  axcl.ppl.transcode.venc.stop.wait       [R/W]       int32_t           0                wait for milliseconds if the in/out FIFOs are not 0 before stopping. 0: stop immediately
 */
AXCL_EXPORT axclError axcl_ppl_get_attr(axcl_ppl ppl, const char* name, void* attr);
AXCL_EXPORT axclError axcl_ppl_set_attr(axcl_ppl ppl, const char* name, const void* attr);

#endif /* __AXCL_PPL_H__ */
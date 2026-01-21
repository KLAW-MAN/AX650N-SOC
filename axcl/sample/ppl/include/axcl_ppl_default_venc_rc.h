/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef __AXCL_PPL_DEFALT_RC_H__
#define __AXCL_PPL_DEFALT_RC_H__

#include "axcl_ppl_type.h"

#ifdef __cplusplus
extern "C" {
#endif

inline AX_VENC_RC_ATTR_T axcl_default_rc_h264_cbr_1080p_4096kbps() {
    AX_VENC_RC_ATTR_T rc = {};
    rc.enRcMode = AX_VENC_RC_MODE_H264CBR;
    rc.s32FirstFrameStartQp = -1;
    rc.stFrameRate.fSrcFrameRate = 30.0F;
    rc.stFrameRate.fDstFrameRate = 30.0F;
    rc.stH264Cbr.u32Gop = 60;
    rc.stH264Cbr.u32BitRate = 4096;
    rc.stH264Cbr.u32MaxQp = 51;
    rc.stH264Cbr.u32MinQp = 10;
    rc.stH264Cbr.u32MaxIQp = 51;
    rc.stH264Cbr.u32MinIQp = 10;
    rc.stH264Cbr.u32MaxIprop = 40;
    rc.stH264Cbr.u32MinIprop = 10;
    rc.stH264Cbr.s32IntraQpDelta = -2;
    rc.stH264Cbr.u32IdrQpDeltaRange = 0;
    return rc;
}

inline AX_VENC_RC_ATTR_T axcl_default_rc_h264_vbr_1080p_4096kbps() {
    AX_VENC_RC_ATTR_T rc = {};
    rc.enRcMode = AX_VENC_RC_MODE_H264VBR;
    rc.s32FirstFrameStartQp = -1;
    rc.stFrameRate.fSrcFrameRate = 30.0F;
    rc.stFrameRate.fDstFrameRate = 30.0F;
    rc.stH264Vbr.u32Gop = 60;
    rc.stH264Vbr.u32MaxBitRate = 4096;
    rc.stH264Vbr.u32MaxQp = 44;
    rc.stH264Vbr.u32MinQp = 10;
    rc.stH264Vbr.u32MaxIQp = 51;
    rc.stH264Vbr.u32MinIQp = 24;
    rc.stH264Vbr.s32IntraQpDelta = 0;
    rc.stH264Vbr.u32IdrQpDeltaRange = 0;
    return rc;
}

inline AX_VENC_RC_ATTR_T axcl_default_rc_h264_avbr_1080p_4096kbps() {
    AX_VENC_RC_ATTR_T rc = {};
    rc.enRcMode = AX_VENC_RC_MODE_H264AVBR;
    rc.s32FirstFrameStartQp = -1;
    rc.stFrameRate.fSrcFrameRate = 30.0F;
    rc.stFrameRate.fDstFrameRate = 30.0F;
    rc.stH264AVbr.u32Gop = 60;
    rc.stH264AVbr.u32MaxBitRate = 4096;
    rc.stH264AVbr.u32MaxQp = 42;
    rc.stH264AVbr.u32MinQp = 10;
    rc.stH264AVbr.u32MaxIQp = 51;
    rc.stH264AVbr.u32MinIQp = 24;
    rc.stH264AVbr.s32IntraQpDelta = 0;
    rc.stH264AVbr.u32IdrQpDeltaRange = 0;
    return rc;
}

inline AX_VENC_RC_ATTR_T axcl_default_rc_h265_cbr_1080p_4096kbps() {
    AX_VENC_RC_ATTR_T rc = {};
    rc.enRcMode = AX_VENC_RC_MODE_H265CBR;
    rc.s32FirstFrameStartQp = -1;
    rc.stFrameRate.fSrcFrameRate = 30.0F;
    rc.stFrameRate.fDstFrameRate = 30.0F;
    rc.stH265Cbr.u32Gop = 60;
    rc.stH265Cbr.u32BitRate = 4096;
    rc.stH265Cbr.u32MaxQp = 51;
    rc.stH265Cbr.u32MinQp = 10;
    rc.stH265Cbr.u32MaxIQp = 51;
    rc.stH265Cbr.u32MinIQp = 10;
    rc.stH265Cbr.u32MaxIprop = 40;
    rc.stH265Cbr.u32MinIprop = 10;
    rc.stH265Cbr.s32IntraQpDelta = -2;
    rc.stH265Cbr.u32IdrQpDeltaRange = 0;
    return rc;
}

inline AX_VENC_RC_ATTR_T axcl_default_rc_h265_vbr_1080p_4096kbps() {
    AX_VENC_RC_ATTR_T rc = {};
    rc.enRcMode = AX_VENC_RC_MODE_H265VBR;
    rc.s32FirstFrameStartQp = -1;
    rc.stFrameRate.fSrcFrameRate = 30.0F;
    rc.stFrameRate.fDstFrameRate = 30.0F;
    rc.stH265Vbr.u32Gop = 60;
    rc.stH265Vbr.u32MaxBitRate = 4096;
    rc.stH265Vbr.u32MaxQp = 42;
    rc.stH265Vbr.u32MinQp = 10;
    rc.stH265Vbr.u32MaxIQp = 51;
    rc.stH265Vbr.u32MinIQp = 24;
    rc.stH265Vbr.s32IntraQpDelta = 0;
    rc.stH265Vbr.u32IdrQpDeltaRange = 0;
    return rc;
}

inline AX_VENC_RC_ATTR_T axcl_default_rc_h265_avbr_1080p_4096kbps() {
    AX_VENC_RC_ATTR_T rc = {};
    rc.enRcMode = AX_VENC_RC_MODE_H265AVBR;
    rc.s32FirstFrameStartQp = -1;
    rc.stFrameRate.fSrcFrameRate = 30.0F;
    rc.stFrameRate.fDstFrameRate = 30.0F;
    rc.stH265AVbr.u32Gop = 60;
    rc.stH265AVbr.u32MaxBitRate = 4096;
    rc.stH265AVbr.u32MaxQp = 44;
    rc.stH265AVbr.u32MinQp = 10;
    rc.stH265AVbr.u32MaxIQp = 51;
    rc.stH265AVbr.u32MinIQp = 24;
    rc.stH265AVbr.s32IntraQpDelta = 0;
    rc.stH265AVbr.u32IdrQpDeltaRange = 0;
    return rc;
}

#ifdef __cplusplus
}
#endif

#endif /* __AXCL_PPL_DEFALT_RC_H__ */
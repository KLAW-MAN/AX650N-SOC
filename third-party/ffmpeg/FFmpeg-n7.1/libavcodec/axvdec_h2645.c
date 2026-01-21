/*
 * Axera Semiconductor H.264 / HEVC decoder
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd.
 *
 * This file is part of FFmpeg.
 *
 * FFmpeg is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * FFmpeg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with FFmpeg; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */


#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <dlfcn.h>

#include "libavutil/opt.h"
#include "libavutil/log.h"
#include "libavutil/pixdesc.h"
#include "libavutil/mem.h"

#include "avcodec.h"
#include "codec_internal.h"
#include "decode.h"
#include "hevc/parse.h"

#include "h264_parse.h"

#include "hwconfig.h"
#include "internal.h"
#include "libavutil/buffer.h"
#include "libavutil/hwcontext_axmm.h"

#include "axdec_h2645.h"



static __inline AX_U32 AX_VDEC_GetPicBufferSize(AX_U32 uWidth, AX_U32 uHeight, AX_IMG_FORMAT_E eOutputFormat,
        AX_FRAME_COMPRESS_INFO_T *pstCompressInfo, AX_PAYLOAD_TYPE_E enType)
{
    AX_U32 picSizeInMbs = 0;
    AX_U32 picSize = 0;
    AX_U32 dmvMemSize = 0;
    AX_U32 refBuffSize = 0;
    AX_U32 uPixBits = 8;
    AX_U32 uHeightAlign = 0;
    AX_U32 uWidthAlign = 0;
    AX_U32 uAlignSize = (1 << (8));
    AX_U32 ax_fbc_tile128x2_size[AX_VDEC_FBC_COMPRESS_LEVEL_MAX] =
    {0, 32, 64, 96, 128, 160, 192, 224, 256, 288};


    // picSizeInMbs = (AX_COMM_ALIGN(uHeight, 16) >> 4) * (AX_COMM_ALIGN(uWidth, 16) >> 4);
    if ((PT_H264 == enType) || (PT_H265 == enType)) {
        switch (eOutputFormat) {
        case AX_FORMAT_YUV400:
        case AX_FORMAT_YUV420_PLANAR:
        case AX_FORMAT_YUV420_PLANAR_VU:
        case AX_FORMAT_YUV420_SEMIPLANAR:
        case AX_FORMAT_YUV420_SEMIPLANAR_VU:
        case AX_FORMAT_YUV422_PLANAR:
        case AX_FORMAT_YUV422_PLANAR_VU:
        case AX_FORMAT_YUV422_SEMIPLANAR:
        case AX_FORMAT_YUV422_SEMIPLANAR_VU:
        case AX_FORMAT_YUV422_INTERLEAVED_YUVY:
        case AX_FORMAT_YUV422_INTERLEAVED_YUYV:
        case AX_FORMAT_YUV422_INTERLEAVED_UYVY:
        case AX_FORMAT_YUV422_INTERLEAVED_VYUY:
        case AX_FORMAT_YUV444_PLANAR:
        case AX_FORMAT_YUV444_PLANAR_VU:
        case AX_FORMAT_YUV444_SEMIPLANAR:
        case AX_FORMAT_YUV444_SEMIPLANAR_VU:
        case AX_FORMAT_YUV444_PACKED:
            uPixBits = 8;
            break;
        case AX_FORMAT_YUV400_10BIT:
        case AX_FORMAT_YUV420_PLANAR_10BIT_UV_PACKED_4Y5B:
        case AX_FORMAT_YUV420_SEMIPLANAR_10BIT_P101010:
        case AX_FORMAT_YUV444_PACKED_10BIT_P101010:
            uPixBits = 10;
            break;
        case AX_FORMAT_YUV420_PLANAR_10BIT_I010:
        case AX_FORMAT_YUV420_SEMIPLANAR_10BIT_P010:
        case AX_FORMAT_YUV420_SEMIPLANAR_10BIT_P016:
        case AX_FORMAT_YUV420_SEMIPLANAR_10BIT_I016:
        case AX_FORMAT_YUV444_PACKED_10BIT_P010:
            uPixBits = 16;
            break;
        default:
            uPixBits = 8;
            break;
        }

        /* luma */
        uHeightAlign = AX_VDEC_ALIGN(uHeight, 2);
        uWidthAlign = AX_VDEC_ALIGN(uWidth, uAlignSize);
        if (pstCompressInfo && (pstCompressInfo->enCompressMode == AX_COMPRESS_MODE_LOSSY)) {
            AX_U32 ax_tile128x2_size = 128 * 2 * uPixBits / 8;

            picSize = uWidthAlign * uHeightAlign
                      * ax_fbc_tile128x2_size[pstCompressInfo->u32CompressLevel] / ax_tile128x2_size;
        } else {
            picSize = uHeightAlign * uWidthAlign;
        }

        /* chroma */
        if (eOutputFormat != AX_FORMAT_YUV400) {
            uHeightAlign = uHeightAlign >> 1;
            if (pstCompressInfo && (pstCompressInfo->enCompressMode == AX_COMPRESS_MODE_LOSSY)) {
                AX_U32 ax_tile128x2_size = 128 * 2 * uPixBits / 8;

                picSize += uWidthAlign * uHeightAlign
                           * ax_fbc_tile128x2_size[pstCompressInfo->u32CompressLevel] / ax_tile128x2_size;
            } else {
                picSize += uWidthAlign * uHeightAlign;
            }
        }

        /* buffer size of dpb pic = picSize + dir_mv_size + tbl_size */
        dmvMemSize = picSizeInMbs * 64;
        refBuffSize = picSize  + dmvMemSize + 32;
    } else if ((PT_JPEG == enType) || (PT_MJPEG == enType)) {
        picSize = (AX_VDEC_ALIGN(uHeight, 2) * AX_VDEC_ALIGN(uWidth, 64) * 3) >> 1;
        refBuffSize = picSize;
    } else {
        refBuffSize = -1;
    }

    return refBuffSize;
}

static int VdecChnAttrEnable(AVCodecContext *avctx)
{
    AXVdecContext *s = avctx->priv_data;
    int ret = -1;
    AX_VDEC_CHN VdChn = 0;
    AX_VDEC_CHN_ATTR_T pstVdChnAttr;

    memset(&pstVdChnAttr, 0, sizeof(pstVdChnAttr));

    switch (avctx->sw_pix_fmt) {
        case AV_PIX_FMT_NV12:
            pstVdChnAttr.enImgFormat = AX_FORMAT_YUV420_SEMIPLANAR;
            break;
        case AV_PIX_FMT_NV21:
            pstVdChnAttr.enImgFormat = AX_FORMAT_YUV420_SEMIPLANAR_VU;
            break;
        case AV_PIX_FMT_P010:
            pstVdChnAttr.enImgFormat = AX_FORMAT_YUV420_SEMIPLANAR_10BIT_P010;
            break;
        case AV_PIX_FMT_YUV420P:
            pstVdChnAttr.enImgFormat = AX_FORMAT_YUV420_PLANAR;
            break;
        default:
            av_log(s, AV_LOG_ERROR, "%s Unsupport ImgFormat\n", __func__);
            ret = AX_ERR_VDEC_NOT_SUPPORT;
            goto ERR_RET;
    }

    if (!avctx->width) {
        avctx->width = AX_VDEC_MAX_WIDTH;
    }
    if (!avctx->height) {
        avctx->height = AX_VDEC_MAX_HEIGHT;
    }

    /* resize for vdec scaler function */
    if ((s->resize.width && s->resize.width < avctx->width) || (s->resize.height && s->resize.height < avctx->height)) {
        pstVdChnAttr.enOutputMode = AX_VDEC_OUTPUT_SCALE;
        pstVdChnAttr.u32PicWidth = s->resize.width; /* picture output width */
        pstVdChnAttr.u32PicHeight = s->resize.height; /* picture output height */
        VdChn = 1;
        avctx->width = s->resize.width;
        avctx->height = s->resize.height;
    } else {
        pstVdChnAttr.enOutputMode = AX_VDEC_OUTPUT_ORIGINAL;
        pstVdChnAttr.u32PicWidth = avctx->width; /* picture output width */
        pstVdChnAttr.u32PicHeight = avctx->height; /* picture output height */
    }

    AX_U32 uWidth = pstVdChnAttr.u32PicWidth;
    AX_U32 uPixBits = 8;

    if (pstVdChnAttr.enImgFormat == AX_FORMAT_YUV420_SEMIPLANAR_10BIT_P010) {
        uPixBits = 10;
        if (pstVdChnAttr.stCompressInfo.enCompressMode == AX_COMPRESS_MODE_LOSSY)
            pstVdChnAttr.u32FrameStride = AX_VDEC_ALIGN(AX_VDEC_ALIGN(uWidth, 128) * uPixBits, AX_VDEC_WIDTH_ALIGN * 8) / 8;
        else
            pstVdChnAttr.u32FrameStride = AX_VDEC_ALIGN(uWidth * 2, AX_VDEC_WIDTH_ALIGN);

    } else if (pstVdChnAttr.enImgFormat == AX_FORMAT_YUV420_SEMIPLANAR_10BIT_P101010) {
        uPixBits = 10;
        if (pstVdChnAttr.stCompressInfo.enCompressMode == AX_COMPRESS_MODE_LOSSY)
            pstVdChnAttr.u32FrameStride = AX_VDEC_ALIGN(AX_VDEC_ALIGN(uWidth, 128) * uPixBits,
                                                                AX_VDEC_WIDTH_ALIGN * 8) / 8;
        else
            pstVdChnAttr.u32FrameStride = AX_VDEC_ALIGN(uWidth * 10 / 8, AX_VDEC_WIDTH_ALIGN);

    } else {
        uPixBits = 8;
        if (pstVdChnAttr.u32FrameStride == 0) {
            if (avctx->codec_id == AV_CODEC_ID_H264 || avctx->codec_id == AV_CODEC_ID_HEVC) {
                pstVdChnAttr.u32FrameStride = AX_VDEC_ALIGN(uWidth * uPixBits, AX_VDEC_WIDTH_ALIGN * 8) / 8;
            } else {
                pstVdChnAttr.u32FrameStride = AX_VDEC_ALIGN(uWidth * uPixBits, AX_JDEC_WIDTH_ALIGN * 8) / 8;
            }
        }
    }

    pstVdChnAttr.u32FrameBufSize = AX_VDEC_GetPicBufferSize(pstVdChnAttr.u32FrameStride,
                                                                        pstVdChnAttr.u32PicHeight,
                                                                        pstVdChnAttr.enImgFormat,
                                                                        &pstVdChnAttr.stCompressInfo,
                                                                        s->enCodecType);
    pstVdChnAttr.u32FrameBufCnt = s->u32FrameBufCnt;

    s->u32FrameStride = pstVdChnAttr.u32FrameStride;
    pstVdChnAttr.u32FramePadding = s->u32FramePadding;
    pstVdChnAttr.u32CropX = 0;
    pstVdChnAttr.u32CropY = 0;

    pstVdChnAttr.u32OutputFifoDepth = s->u32OutputFifoDepth;
    s->VdChn = VdChn;

    ret = s->vdec_funcs.vdec_setchnattr(s->VdGrp, VdChn, &pstVdChnAttr);
    if (ret != AX_SUCCESS) {
        av_log(s, AV_LOG_ERROR, "VdGrp=%d, AXCL_VDEC_SetChnAttr FAILED! ret:0x%x\n",
                s->VdGrp, ret);
        goto ERR_RET;
    }

    ret = s->vdec_funcs.vdec_enablechn(s->VdGrp, VdChn);
    if (ret != AX_SUCCESS) {
        av_log(s, AV_LOG_ERROR, "VdGrp=%d, VdChn=%d, AXCL_VDEC_EnableChn FAILED! ret:0x%x\n",
                s->VdGrp, VdChn, ret);
    }

ERR_RET:
    return ret;

}

static av_cold int ax_decode_close(AVCodecContext *avctx)
{
    AX_S32 sRet = 0;
    AXVdecContext *s = avctx->priv_data;

    if (!s->bInited) {
        av_log(s, AV_LOG_DEBUG, "ax_decode_close NO need close!\n");
        return 0;
    }

    av_log(s, AV_LOG_DEBUG, "%s Enter!!\n", __func__);

    sRet = s->vdec_funcs.vdec_stoprecvstream(s->VdGrp);
    if (sRet != AX_SUCCESS) {
        av_log(s, AV_LOG_ERROR, "VdGrp=%d, AXCL_VDEC_StopRecvStream FAILED! ret:0x%x\n",
                s->VdGrp, sRet);
    }

    sRet = s->vdec_funcs.vdec_destroygrp(s->VdGrp);
    if (sRet != AX_SUCCESS) {
        av_log(s, AV_LOG_ERROR, "VdGrp=%d, AXCL_VDEC_DestroyGrp FAILED! ret:0x%x\n",
                s->VdGrp, sRet);
    }

    sRet = s->vdec_funcs.vdec_deinit();
    if (sRet != AX_SUCCESS) {
        av_log(s, AV_LOG_ERROR, "%s vdec_deinit failed, ret = 0x%x\n", __func__, sRet);
    }

    if (!avctx->hw_device_ctx) {
        if (sRet = s->sysdev_funcs.sys_deinit()) {
            av_log(s, AV_LOG_ERROR, "%s sys_deinit failed ret = 0x%x\n", __func__, sRet);
        }
    }
    av_log(s, AV_LOG_DEBUG, "%s End!!\n", __func__);

    av_buffer_unref(&s->hwframe);
    av_buffer_unref(&s->hwdevice);

    dlclose(s->sys_lib);
    dlclose(s->mm_vdec_lib);

    return 0;
}

static int hwframe_ctx_init(AVCodecContext *ctx)
{
    int ret;
    AXVdecContext *vctx = ctx->priv_data;
    AVBufferRef *hw_frames_ref;
    AVHWFramesContext *frames_ctx;

    if (ctx->hw_device_ctx && !ctx->hw_frames_ctx) {
        hw_frames_ref = av_hwframe_ctx_alloc(ctx->hw_device_ctx);
        if (!hw_frames_ref) {
            av_log(ctx, AV_LOG_ERROR, "%s failed to alloc hwframe context\n", __func__);
            return AVERROR(ENOMEM);
        }

        frames_ctx = (AVHWFramesContext *)(hw_frames_ref->data);
        frames_ctx->format = AV_PIX_FMT_AXMM;
        frames_ctx->sw_format = ctx->sw_pix_fmt;
        frames_ctx->width = ctx->width;
        frames_ctx->height = ctx->height;
        frames_ctx->initial_pool_size = vctx->u32FrameBufCnt;

        ret = av_hwframe_ctx_init(hw_frames_ref);
        if (ret) {
            av_log(ctx, AV_LOG_ERROR, "%s failed to init hwframe context\n", __func__);
            av_buffer_unref(&hw_frames_ref);
            return ret;
        }

        ctx->hw_frames_ctx = av_buffer_ref(hw_frames_ref);
        av_buffer_unref(&hw_frames_ref);
        if (!ctx->hw_frames_ctx)
            return AVERROR(ENOMEM);

        vctx->hwdevice = av_buffer_ref(ctx->hw_device_ctx);
        if (!vctx->hwdevice) {
            av_buffer_unref(&ctx->hw_frames_ctx);
            ret = AVERROR(ENOMEM);
            return ret;
        }

        vctx->hwframe = av_hwframe_ctx_alloc(vctx->hwdevice);
        if (!vctx->hwframe) {
            av_buffer_unref(&ctx->hw_frames_ctx);
            av_buffer_unref(&vctx->hwdevice);
            av_log(vctx, AV_LOG_ERROR, "av_hwframe_ctx_alloc failed\n");
            ret = AVERROR(ENOMEM);
            return ret;
        }

        av_log(ctx, AV_LOG_DEBUG, "%s hwframe context init success w:%d, h:%d, f:%d, sw_pix_fmt:%d\n", __func__, ctx->width, ctx->height, ctx->pix_fmt, ctx->sw_pix_fmt);
    }

    return 0;
}

static int ax_vdec_dl(AXVdecContext *s)
{
    AXVdecFuncs *vdec_funcs = &s->vdec_funcs;
    AXSYSDevFuncs *sys_funcs = &s->sysdev_funcs;

    s->sys_lib = dlopen(LIB_SYS_NAME, RTLD_NOW | RTLD_GLOBAL);
    CHECK_WITH_RET(!s->sys_lib, AVERROR(EINVAL));

    sys_funcs->sys_init = dlsym(s->sys_lib, "AX_SYS_Init");
    CHECK_WITH_RET(!sys_funcs->sys_init, AVERROR(EINVAL));

    sys_funcs->sys_deinit = dlsym(s->sys_lib, "AX_SYS_Deinit");
    CHECK_WITH_RET(!sys_funcs->sys_deinit, AVERROR(EINVAL));

    sys_funcs->sys_getvir = dlsym(s->sys_lib, "AX_POOL_GetBlockVirAddr");
    CHECK_WITH_RET(!sys_funcs->sys_getvir, AVERROR(EINVAL));

    sys_funcs->sys_mmap = dlsym(s->sys_lib, "AX_SYS_Mmap");
    CHECK_WITH_RET(!sys_funcs->sys_mmap, AVERROR(EINVAL));

    sys_funcs->sys_munmap = dlsym(s->sys_lib, "AX_SYS_Munmap");
    CHECK_WITH_RET(!sys_funcs->sys_munmap, AVERROR(EINVAL));

    s->mm_vdec_lib = dlopen(LIB_VDEC_NAME, RTLD_NOW | RTLD_GLOBAL);
    CHECK_WITH_RET(!s->mm_vdec_lib, AVERROR(EINVAL));

    vdec_funcs->vdec_init = dlsym(s->mm_vdec_lib, "AX_VDEC_Init");
    CHECK_WITH_RET(!vdec_funcs->vdec_init, AVERROR(EINVAL));

    vdec_funcs->vdec_deinit = dlsym(s->mm_vdec_lib, "AX_VDEC_Deinit");
    CHECK_WITH_RET(!vdec_funcs->vdec_deinit, AVERROR(EINVAL));

    vdec_funcs->vdec_creategrp = dlsym(s->mm_vdec_lib, "AX_VDEC_CreateGrpEx");
    CHECK_WITH_RET(!vdec_funcs->vdec_creategrp, AVERROR(EINVAL));

    vdec_funcs->vdec_destroygrp = dlsym(s->mm_vdec_lib, "AX_VDEC_DestroyGrp");
    CHECK_WITH_RET(!vdec_funcs->vdec_destroygrp, AVERROR(EINVAL));

    vdec_funcs->vdec_selectgrp = dlsym(s->mm_vdec_lib, "AX_VDEC_SelectGrp");
    CHECK_WITH_RET(!vdec_funcs->vdec_selectgrp, AVERROR(EINVAL));

    vdec_funcs->vdec_sendstream = dlsym(s->mm_vdec_lib, "AX_VDEC_SendStream");
    CHECK_WITH_RET(!vdec_funcs->vdec_sendstream, AVERROR(EINVAL));

    vdec_funcs->vdec_getframe = dlsym(s->mm_vdec_lib, "AX_VDEC_GetChnFrame");
    CHECK_WITH_RET(!vdec_funcs->vdec_getframe, AVERROR(EINVAL));

    vdec_funcs->vdec_releaseframe = dlsym(s->mm_vdec_lib, "AX_VDEC_ReleaseChnFrame");
    CHECK_WITH_RET(!vdec_funcs->vdec_releaseframe, AVERROR(EINVAL));

    vdec_funcs->vdec_startrecvsStream = dlsym(s->mm_vdec_lib, "AX_VDEC_StartRecvStream");
    CHECK_WITH_RET(!vdec_funcs->vdec_startrecvsStream, AVERROR(EINVAL));

    vdec_funcs->vdec_stoprecvstream = dlsym(s->mm_vdec_lib, "AX_VDEC_StopRecvStream");
    CHECK_WITH_RET(!vdec_funcs->vdec_stoprecvstream, AVERROR(EINVAL));

    vdec_funcs->vdec_querystatus = dlsym(s->mm_vdec_lib, "AX_VDEC_QueryStatus");
    CHECK_WITH_RET(!vdec_funcs->vdec_querystatus, AVERROR(EINVAL));

    vdec_funcs->vdec_getstrminfo = dlsym(s->mm_vdec_lib, "AX_VDEC_ExtractStreamHeaderInfo");
    CHECK_WITH_RET(!vdec_funcs->vdec_querystatus, AVERROR(EINVAL));

    vdec_funcs->vdec_setgrpparam = dlsym(s->mm_vdec_lib, "AX_VDEC_SetGrpParam");
    CHECK_WITH_RET(!vdec_funcs->vdec_setgrpparam, AVERROR(EINVAL));

    vdec_funcs->vdec_setdispmode = dlsym(s->mm_vdec_lib, "AX_VDEC_SetDisplayMode");
    CHECK_WITH_RET(!vdec_funcs->vdec_setdispmode, AVERROR(EINVAL));

    vdec_funcs->vdec_enablechn = dlsym(s->mm_vdec_lib, "AX_VDEC_EnableChn");
    CHECK_WITH_RET(!vdec_funcs->vdec_enablechn, AVERROR(EINVAL));

    vdec_funcs->vdec_disablechn = dlsym(s->mm_vdec_lib, "AX_VDEC_DisableChn");
    CHECK_WITH_RET(!vdec_funcs->vdec_disablechn, AVERROR(EINVAL));

    vdec_funcs->vdec_setchnattr = dlsym(s->mm_vdec_lib, "AX_VDEC_SetChnAttr");
    CHECK_WITH_RET(!vdec_funcs->vdec_setchnattr, AVERROR(EINVAL));

    return 0;
}

AX_U64  u64Pts_Frm = 0;

static int hevc_get_headerinfo(AVCodecContext *avctx)
{
    AXVdecContext *s = avctx->priv_data;
    HEVCParamSets ps;
    HEVCSEI sei;

    const HEVCVPS *vps = NULL;
    const HEVCPPS *pps = NULL;
    const HEVCSPS *sps = NULL;
    int is_nalff = 0;
    int nal_length_size = 0;
    int ref_num = 0;
    int i = 0;
    int ret = 0;

    memset(&ps, 0, sizeof(ps));
    memset(&sei, 0, sizeof(sei));

    ret = ff_hevc_decode_extradata(avctx->extradata, avctx->extradata_size,
                                   &ps, &sei, &is_nalff, &nal_length_size, 0, 1, avctx);
    if (ret < 0) {
        av_log(avctx, AV_LOG_DEBUG, "%s extradata_size:%d, ret = 0x%x\n", __func__, avctx->extradata_size, ret);
    }
    for (i = 0; i < HEVC_MAX_VPS_COUNT; i++) {
        if (ps.vps_list[i]) {
            vps = ps.vps_list[i];
            break;
        }
    }

    for (i = 0; i < HEVC_MAX_PPS_COUNT; i++) {
        if (ps.pps_list[i]) {
            pps = ps.pps_list[i];
            break;
        }
    }

    if (pps) {
        if (ps.sps_list[pps->sps_id]) {
            sps = ps.sps_list[pps->sps_id];

            avctx->width = sps->width;
            avctx->height = sps->height;
            s->bit_depth_luma = sps->bit_depth;
            s->bit_depth_chroma = sps->bit_depth_chroma;

            ref_num = MAX(1, sps->temporal_layer[sps->max_sub_layers - 1].max_dec_pic_buffering);
            s->u32FrameBufCnt = ref_num + 2;
            s->u32OutputFifoDepth = ref_num + 2;
            av_log(avctx, AV_LOG_INFO, "HEVC decoder w:%d, h:%d, bit_depth:%d, bit_depth_chroma:%d, ref_num:%d\n",
                   sps->width, sps->height, sps->bit_depth, sps->bit_depth_chroma, s->u32FrameBufCnt);
        }
    }

ERR_RET:
    return 0;
}

static int h264_get_headerinfo(AVCodecContext *avctx)
{
    AXVdecContext *s = avctx->priv_data;
    int i;
    int ret = 0;
    int ref_num = 0;

    H264ParamSets ps;
    const PPS *pps = NULL;
    const SPS *sps = NULL;
    int is_avc = 0;
    int nal_length_size = 0;

    memset(&ps, 0, sizeof(ps));

    ret = ff_h264_decode_extradata(avctx->extradata, avctx->extradata_size,
                                   &ps, &is_avc, &nal_length_size, 0, avctx);
    if (ret < 0) {
       av_log(avctx, AV_LOG_DEBUG, "%s extradata_size:%d ret = 0x%x\n", __func__, avctx->extradata_size, ret);
    }

    for (i = 0; i < MAX_PPS_COUNT; i++) {
        if (ps.pps_list[i]) {
            pps = ps.pps_list[i];
            break;
        }
    }

    if (pps) {
        if (ps.sps_list[pps->sps_id]) {
            sps = ps.sps_list[pps->sps_id];
            if (!avctx->width || (!avctx->height)) {
                avctx->width = sps->mb_width * 16;
                avctx->height = sps->mb_height * 16;
            }
            s->bit_depth_luma = sps->bit_depth_luma;
            s->bit_depth_chroma = sps->bit_depth_chroma;

            ref_num = MAX(1, sps->num_reorder_frames);
            if (sps->ref_frame_count > ref_num) {
                ref_num = sps->ref_frame_count;
            }
            s->u32FrameBufCnt = ref_num + 2;
            s->u32OutputFifoDepth = ref_num + 2;
            av_log(avctx, AV_LOG_INFO, "H264 decoder w:%d, h:%d, bit_depth_luma:%d, bit_depth_chroma:%d, ref_num:%d\n",
                   sps->mb_width, sps->mb_height, sps->bit_depth_luma, sps->bit_depth_chroma, s->u32FrameBufCnt);
        }
    }

done:

    return 0;
}

static av_cold int ax_decode_init(AVCodecContext *avctx)
{
    AXVdecContext *s = avctx->priv_data;
    int ret = 0;
    AX_VDEC_GRP VdGrp = 0;
    AX_VDEC_GRP_ATTR_T pstVdGrpAttr = {0};
    AX_VDEC_MOD_ATTR_T stModAttr = {0};

    s->bInited = AX_FALSE;
    u64Pts_Frm = 0;
    enum AVPixelFormat pix_fmts[6] = { AV_PIX_FMT_AXMM,
                                       AV_PIX_FMT_NV12,
                                       AV_PIX_FMT_NV21,
                                       AV_PIX_FMT_YUV420P,
                                       AV_PIX_FMT_P010,
                                       AV_PIX_FMT_NONE };

    ret = ax_vdec_dl(s);
    if (ret) {
        av_log(s, AV_LOG_ERROR, "%s ax_vdec_dl failed, ret = 0x%x\n", __func__, ret);
        goto ERR_RET;
    }

    if (!avctx->internal->is_copy) {
        if (avctx->extradata_size > 0 && avctx->extradata) {
            if (avctx->codec_id == AV_CODEC_ID_HEVC) {
                ret = hevc_get_headerinfo(avctx);
                if (ret) {
                    av_log(s, AV_LOG_ERROR, "%s hevc_set_extradata failed, ret = 0x%x\n", __func__, ret);
                    goto ERR_RET;
                }
            } else if (avctx->codec_id == AV_CODEC_ID_H264) {
                ret = h264_get_headerinfo(avctx);
                if (ret < 0) {
                    av_log(s, AV_LOG_ERROR, "%s h264_set_extradata failed, ret = 0x%x\n", __func__, ret);
                    goto ERR_RET;
                }
            } else {
                av_log(s, AV_LOG_ERROR, "%s not support codec_id:%d\n", __func__, avctx->codec_id);
                goto ERR_RET;
            }
        } else {
            av_log(s, AV_LOG_DEBUG, "%s data size zero\n", __func__);
            goto ERR_RET;
        }
    }

    if (!avctx->width) {
        avctx->width = 8192;
    }

    if (!avctx->height) {
        avctx->height = 8192;
    }

    if (!s->u32FrameBufCnt)
        s->u32FrameBufCnt = AX_VDEC_DPB_NUM;

    if (!s->u32OutputFifoDepth)
        s->u32OutputFifoDepth = AX_VDEC_DPB_NUM;

    av_log(s, AV_LOG_DEBUG, "%s ax_vdec_init sw_pix_fmt:%d, pix_fmt:%d\n", __func__, avctx->sw_pix_fmt, avctx->pix_fmt);

    ret = ff_get_format(avctx, pix_fmts);
    if (ret < 0) {
        av_log(avctx, AV_LOG_ERROR, "ff_get_format failed: %d\n", ret);
        goto ERR_RET;
    }
    avctx->pix_fmt = ret;
    avctx->thread_count = 1;
    avctx->thread_type = FF_THREAD_FRAME;
    s->format = AV_PIX_FMT_NONE;

    if ((s->bit_depth_luma == 10) && (s->bit_depth_chroma == 10)) {
        avctx->sw_pix_fmt = AV_PIX_FMT_P010;
    } else {
        avctx->sw_pix_fmt = AV_PIX_FMT_NV12;
    }

    ret = hwframe_ctx_init(avctx);
    if (ret)
        goto ERR_RET;


    if (ret = s->sysdev_funcs.sys_init()) {
        av_log(s, AV_LOG_ERROR, "%s sys_init failed ret = 0x%x\n", __func__, ret);
        goto ERR_RET;
    }

    if (avctx->codec_id == AV_CODEC_ID_H264) {
        pstVdGrpAttr.enCodecType = PT_H264;
    } else if (avctx->codec_id == AV_CODEC_ID_HEVC) {
        pstVdGrpAttr.enCodecType = PT_H265;
    } else if (avctx->codec_id == AV_CODEC_ID_MJPEG) {
        pstVdGrpAttr.enCodecType = PT_JPEG;
    } else {
        av_log(s, AV_LOG_ERROR, "%s NOT SUPPORT CODEC ID\n", __func__);
        goto ERR_RET1;
    }

    ret = s->resize_expr && sscanf(s->resize_expr, "%dx%d",
                                   &s->resize.width, &s->resize.height);
    if (ret) {
        av_log(avctx, AV_LOG_DEBUG, "No need resize expressions w:%d, h:%d\n", s->resize.width, s->resize.height);
    }

    stModAttr.enDecModule = AX_ENABLE_BOTH_VDEC_JDEC;
    stModAttr.u32MaxGroupCount = VDEC_GRP_MAX;
    stModAttr.bVdecMc = 0;
    stModAttr.VdecVirtChn = 0;
    s->bEof = AX_FALSE;

    ret = s->vdec_funcs.vdec_init(&stModAttr);
    if (ret != AX_SUCCESS) {
        av_log(s, AV_LOG_ERROR, "%s vdec_init failed, ret = 0x%x\n", __func__, ret);
        goto ERR_RET1;
    }

    av_log(s, AV_LOG_DEBUG, "%s ax_vdec_init, H=%d, W=%d, sw_pix_fmt:%d, pix_fmt:%d, format:%d\n",
           __func__, avctx->height, avctx->width, avctx->sw_pix_fmt, avctx->pix_fmt, s->format);

    pstVdGrpAttr.u32MaxPicWidth = AX_VDEC_ALIGN(avctx->width, 256);  /*Max pic width*/
    pstVdGrpAttr.u32MaxPicHeight = AX_VDEC_ALIGN(avctx->height, 16);  /*Max pic height*/

    pstVdGrpAttr.u32StreamBufSize = STREAM_BUFFER_MIN_SIZE;
    pstVdGrpAttr.enInputMode = AX_VDEC_INPUT_MODE_FRAME;
    pstVdGrpAttr.bSdkAutoFramePool = AX_TRUE; /*use private pool*/

    ret = s->vdec_funcs.vdec_creategrp(&VdGrp, &pstVdGrpAttr);
    if (ret != AX_SUCCESS) {
        av_log(s, AV_LOG_ERROR, "%s vdec_creategrp failed, ret = 0x%x\n", __func__, ret);
        goto ERR_RET2;
    }

    s->VdGrp = VdGrp;
    s->enCodecType = pstVdGrpAttr.enCodecType;

    ret = VdecChnAttrEnable(avctx);
    if (ret != AX_SUCCESS) {
        av_log(s, AV_LOG_ERROR, "%s VdecChnAttrEnable failed, ret = 0x%x\n", __func__, ret);
        goto ERR_RET_CHNATTR_FREE;
    }

    ret = s->vdec_funcs.vdec_setdispmode(VdGrp, AX_VDEC_DISPLAY_MODE_PLAYBACK);
    if (ret != AX_SUCCESS) {
        av_log(s, AV_LOG_ERROR, "VdGrp=%d vdec_setdispmode failed, ret = 0x%x\n", VdGrp, ret);
        goto ERR_RET_CHNATTR_FREE;
    }

    AX_VDEC_GRP_PARAM_T stGrpParam;
    memset(&stGrpParam, 0, sizeof(stGrpParam));
    stGrpParam.stVdecVideoParam.enOutputOrder = AX_VDEC_OUTPUT_ORDER_DISP;
    stGrpParam.stVdecVideoParam.enVdecMode = VIDEO_DEC_MODE_IPB;

    ret = s->vdec_funcs.vdec_setgrpparam(VdGrp, &stGrpParam);
    if (ret != AX_SUCCESS) {
        av_log(s, AV_LOG_ERROR, "VdGrp=%d vdec_setgrpparam failed, ret = 0x%x\n", VdGrp, ret);
        goto ERR_RET_CHNATTR_FREE;
    }

    AX_VDEC_RECV_PIC_PARAM_T tRecvParam;
    memset(&tRecvParam, 0, sizeof(tRecvParam));

    ret = s->vdec_funcs.vdec_startrecvsStream(VdGrp, &tRecvParam);
    if (ret != AX_SUCCESS) {
        av_log(s, AV_LOG_ERROR, "VdGrp=%d vdec_startrecvsStream failed, ret = 0x%x\n", VdGrp, ret);
        goto ERR_RET_CHNATTR_FREE;
    }

    av_log(s, AV_LOG_DEBUG, "ax_decode_init END s->VdGrp=%d\n", s->VdGrp);

    s->bInited = AX_TRUE;
    return 0;

ERR_RET_CHNATTR_FREE:
    while (1) {
        ret = s->vdec_funcs.vdec_destroygrp(VdGrp);
        if (ret == AX_ERR_VDEC_BUSY) {
            av_log(s, AV_LOG_ERROR, "VdGrp=%d vdec_destroygrp failed, ret = 0x%x\n", VdGrp, ret);
            usleep(10000);
            continue;
        }
        break;
    }
ERR_RET2:
    s->vdec_funcs.vdec_deinit();
ERR_RET1:
    s->sysdev_funcs.sys_deinit();
ERR_FREE:
    av_buffer_unref(&s->hwframe);
    av_buffer_unref(&s->hwdevice);
ERR_RET:
    if (s->sys_lib)
        dlclose(s->sys_lib);
    if (s->mm_vdec_lib)
        dlclose(s->mm_vdec_lib);

    return ret;
}

static void av_buffer_free(void *opaque, uint8_t *data)
{
    AXVdecContext *s = opaque;
    AX_VIDEO_FRAME_INFO_T *pstFrameInfo = (AX_VIDEO_FRAME_INFO_T *)data;
    AX_S32 size = 0;
    AX_S32 ret;

    size = pstFrameInfo->stVFrame.u32PicStride[0] * pstFrameInfo->stVFrame.u32Height * 3 / 2;

    if (pstFrameInfo && (s->resize.width == pstFrameInfo->stVFrame.u32Width) &&
        (s->resize.height == pstFrameInfo->stVFrame.u32Height)) {
        av_log(s, AV_LOG_DEBUG, "VdGrp=%d, VdChn:%d, av_buffer_free  w:%d, h:%d\n"
               "u64PhyAddr[0]:0x%llX, BlkId[0]:0x%x, u64VirAddr[0]:%p\n",
               s->VdGrp, s->VdChn, pstFrameInfo->stVFrame.u32Width, pstFrameInfo->stVFrame.u32Height,
               pstFrameInfo->stVFrame.u64PhyAddr[0], pstFrameInfo->stVFrame.u32BlkId[0], pstFrameInfo->stVFrame.u64VirAddr[0]);

        s->sysdev_funcs.sys_munmap((AX_VOID *)pstFrameInfo->stVFrame.u64VirAddr[0], size);
        ret = s->vdec_funcs.vdec_releaseframe(s->VdGrp, s->VdChn, pstFrameInfo);
        if (ret) {
            av_log(s, AV_LOG_ERROR, "VdGrp=%d, VdChn:%d, AXCL_VDEC_ReleaseChnFrame FAILED! res:0x%x\n"
                   "u64PhyAddr[0]:0x%llX, BlkId[0]:0x%x, BlkId[1]:0x%x\n",
                   s->VdGrp, s->VdChn, ret, pstFrameInfo->stVFrame.u64PhyAddr[0],
                   pstFrameInfo->stVFrame.u32BlkId[0], pstFrameInfo->stVFrame.u32BlkId[1]);
        }
    }
}

static int ax_decode_output_frame(AVCodecContext *avctx, AVFrame *frame)
{
    AXVdecContext *s = avctx->priv_data;
    AX_S32 ret;
    AX_VIDEO_FRAME_INFO_T *pstFrameInfo = NULL;
    AX_S32 y_size = 0;
    AX_S32 uv_size = 0;

    pstFrameInfo = av_malloc(sizeof(*pstFrameInfo));
    memset(pstFrameInfo, 0x0, sizeof(AX_VIDEO_FRAME_INFO_T));

    ret = s->vdec_funcs.vdec_getframe(s->VdGrp, s->VdChn, pstFrameInfo, 100);
    if (ret == AX_SUCCESS) {
        frame->width = pstFrameInfo->stVFrame.u32Width;
        frame->height = pstFrameInfo->stVFrame.u32Height;
        frame->linesize[0] = pstFrameInfo->stVFrame.u32PicStride[0];
        frame->linesize[1] = pstFrameInfo->stVFrame.u32PicStride[1];

        frame->pts = pstFrameInfo->stVFrame.u64PTS;

        frame->pkt_pos = -1;
        frame->duration = 0;
        frame->pkt_size = -1;
        s->resize.width = pstFrameInfo->stVFrame.u32Width;
        s->resize.height = pstFrameInfo->stVFrame.u32Height;
    } else  if (ret == AX_ERR_VDEC_FLOW_END) {
        av_log(s, AV_LOG_INFO, "VdGrp=%d vdec_getframe AX_ERR_VDEC_FLOW_END\n", s->VdGrp);
        avcodec_flush_buffers(avctx);
        ret = AVERROR_EOF;
        goto ERR_RET;
    } else if (ret == AX_ERR_VDEC_STRM_ERROR) {
        av_log(s, AV_LOG_ERROR, "VdGrp=%d vdec_getframe AX_ERR_VDEC_STRM_ERROR\n", s->VdGrp);
        ret = AVERROR_INVALIDDATA;
        goto ERR_RET;
    } else if (ret == AX_ERR_VDEC_BUF_EMPTY) {
        av_log(s, AV_LOG_INFO, "VdGrp=%d vdec_getframe AX_ERR_VDEC_BUF_EMPTY\n", s->VdGrp);
        //ret = AVERROR(EAGAIN);
        goto ERR_RET;
    } else {
        av_log(s, AV_LOG_ERROR, "VdGrp=%d vdec_getframe ret = 0x%x\n", s->VdGrp, ret);
        ret = AVERROR_EXTERNAL;
        goto ERR_RET;
    }

    av_log(s, AV_LOG_DEBUG, "VdGrp=%d, VdChn:%d, AXCL_VDEC_GetChnFrame  w:%d, h:%d\n"
             "u64PhyAddr[0]:0x%llX, BlkId[0]:0x%x, u64VirAddr[0]:%p, f:%d, Stride:%d, %d\n",
             s->VdGrp, s->VdChn, pstFrameInfo->stVFrame.u32Width, pstFrameInfo->stVFrame.u32Height, pstFrameInfo->stVFrame.u64PhyAddr[0],
             pstFrameInfo->stVFrame.u32BlkId[0], pstFrameInfo->stVFrame.u64VirAddr[0], pstFrameInfo->stVFrame.enImgFormat, pstFrameInfo->stVFrame.u32PicStride[0], frame->linesize[0]);

    if (avctx->pix_fmt == AV_PIX_FMT_AXMM) {
        AX_VIDEO_FRAME_T *fr;
        AVHWFramesContext *frames_ctx;
        frames_ctx = (AVHWFramesContext*)avctx->hw_frames_ctx->data;
        AXMMFramesContext *hwfc_priv = frames_ctx->hwctx;
        hwfc_priv->p.blk_id = pstFrameInfo->stVFrame.u32BlkId[0];

        ret = av_hwframe_get_buffer(avctx->hw_frames_ctx, frame, 0);
        if (ret) {
            av_log(s, AV_LOG_ERROR, "%s failed to av_hwframe_get_buffer, ret = 0x%x, %p\n", __func__, ret, avctx->hw_frames_ctx);
            goto ERR_RET1;
        }

        fr = (AX_VIDEO_FRAME_T *)frame->data[4];
        memcpy(fr, &pstFrameInfo->stVFrame, sizeof(AX_VIDEO_FRAME_T));
        ret = s->vdec_funcs.vdec_releaseframe(s->VdGrp, s->VdChn, pstFrameInfo);
        if (ret) {
            av_log(s, AV_LOG_ERROR, "VdGrp=%d, VdChn:%d, AXCL_VDEC_ReleaseChnFrame FAILED! res:0x%x\n"
                   "u64PhyAddr[0]:0x%llX, BlkId[0]:0x%x, BlkId[1]:0x%x\n",
                   s->VdGrp, s->VdChn, ret, pstFrameInfo->stVFrame.u64PhyAddr[0],
                   pstFrameInfo->stVFrame.u32BlkId[0], pstFrameInfo->stVFrame.u32BlkId[1]);
        }
        av_free(pstFrameInfo);
    } else if (avctx->pix_fmt == AV_PIX_FMT_NV12 || avctx->pix_fmt == AV_PIX_FMT_NV21 ||
               avctx->pix_fmt == AV_PIX_FMT_YUV420P || avctx->pix_fmt == AV_PIX_FMT_P010) {
        y_size = frame->linesize[0] * frame->height;
        uv_size = frame->linesize[1] * frame->height / 2;

        pstFrameInfo->stVFrame.u64VirAddr[0] = s->sysdev_funcs.sys_mmap(pstFrameInfo->stVFrame.u64PhyAddr[0], (y_size + uv_size));
        if (!pstFrameInfo->stVFrame.u64VirAddr[0]) {
            av_log(s, AV_LOG_ERROR, "%s failed to sys_mmap\n", __func__);
            goto ERR_RET1;
        }

        frame->buf[0]  = av_buffer_create(pstFrameInfo, y_size, av_buffer_free, s, 0);
        frame->buf[0]->data = frame->data[0] = (uint8_t *)pstFrameInfo->stVFrame.u64VirAddr[0];
        pstFrameInfo->stVFrame.u64VirAddr[1] = pstFrameInfo->stVFrame.u64VirAddr[0] + y_size;
        frame->buf[1]  = av_buffer_create(pstFrameInfo, uv_size, av_buffer_default_free, NULL, 0);
        frame->buf[1]->data = frame->data[1] = (uint8_t *)pstFrameInfo->stVFrame.u64VirAddr[1];

        switch (pstFrameInfo->stVFrame.enImgFormat) {
        case AX_FORMAT_YUV420_SEMIPLANAR:
            frame->format = AV_PIX_FMT_NV12;
            break;
        case AX_FORMAT_YUV420_SEMIPLANAR_VU:
            frame->format = AV_PIX_FMT_NV21;
            break;
        case AX_FORMAT_YUV420_SEMIPLANAR_10BIT_P010 :
            frame->format = AV_PIX_FMT_P010;
            break;
        default:
            frame->format = AV_PIX_FMT_YUV420P;
            break;
        }
    } else {
        av_log(s, AV_LOG_ERROR, "%s not support format:%d\n", __func__, pstFrameInfo->stVFrame.enImgFormat);
        goto ERR_RET1;
    }

    return 0;

ERR_RET1:
    s->vdec_funcs.vdec_releaseframe(s->VdGrp, s->VdChn, pstFrameInfo);
ERR_RET:
    av_free(pstFrameInfo);
    return ret;
}

static int ax_decode_frame(AVCodecContext *avctx, struct AVFrame *frame,
                            int *got_frame, AVPacket *avpkt)
{
    AXVdecContext *s = avctx->priv_data;
    int ret;
    AX_VDEC_STREAM_T tStrInfo = {0};

    if (!s->bInited) {
        return 0;
    }

    if (avpkt->size) {
        tStrInfo.pu8Addr = avpkt->data;
        tStrInfo.u64PhyAddr = 0;
        tStrInfo.u32StreamPackLen = avpkt->size;
        tStrInfo.bEndOfStream = AX_FALSE;
        tStrInfo.u64PTS = u64Pts_Frm;
        u64Pts_Frm ++;
    } else {
        tStrInfo.pu8Addr = NULL;
        tStrInfo.u64PhyAddr = 0;
        tStrInfo.u32StreamPackLen = 0;
        tStrInfo.bEndOfStream = AX_TRUE;
    }

    av_log(s, AV_LOG_DEBUG, "%s packet data size:%d, pts:%llx, dts:%llx !\n", __func__, avpkt->size, avpkt->pts, avpkt->dts);

    if (!s->bEof) {
        ret = s->vdec_funcs.vdec_sendstream((s->VdGrp), &tStrInfo, -1);
        if (ret == AX_SUCCESS) {
            av_log(s, AV_LOG_DEBUG, "VdGrp=%d vdec_sendstream success\n", s->VdGrp);
        } else {
            av_log(s, AV_LOG_ERROR, "VdGrp=%d vdec_sendstream failed ret:0x%x\n", s->VdGrp, ret);
        }
    }

    ret = ax_decode_output_frame(avctx, frame);
    if (ret == AX_SUCCESS) {
        *got_frame = 1;
        frame->pts = avpkt->dts;
#if FF_API_PKT_PTS
FF_DISABLE_DEPRECATION_WARNINGS
        frame->pkt_pts = frame->pts;
FF_ENABLE_DEPRECATION_WARNINGS
#endif
    } else {
        *got_frame = 0;
        if (ret == AX_ERR_VDEC_BUF_EMPTY)
            ret = AX_SUCCESS;
    }

    if ((!avpkt->size) && (!avpkt->data))
        s->bEof = AX_TRUE;

ERR_RET:
    return ret;
}

const AVCodecHWConfigInternal *ff_h2645_axdec_hw_configs[] = {
    &(const AVCodecHWConfigInternal) {
        .public = {
            .pix_fmt     = AV_PIX_FMT_AXMM,
            .methods     = AV_CODEC_HW_CONFIG_METHOD_HW_DEVICE_CTX |
                           AV_CODEC_HW_CONFIG_METHOD_INTERNAL,
            .device_type = AV_HWDEVICE_TYPE_AXMM
        },
        .hwaccel = NULL,
    },
    NULL
};

#define OFFSET(x) offsetof(AXVdecContext, x)
#define VD AV_OPT_FLAG_VIDEO_PARAM | AV_OPT_FLAG_DECODING_PARAM
#define MAX_CARD_NUM 256

static const AVOption hevc_options[] = {
    { "resize",   "Resize (width)x(height)", OFFSET(resize_expr), AV_OPT_TYPE_STRING, { .str = NULL }, 0, 0, VD },
    { "fmt", "Decoder output format (software format of hardware frames)", OFFSET(out_fmt_string), AV_OPT_TYPE_STRING, { .str = NULL }, 0, 0, VD  },
    { NULL }
};

static const AVClass hevc_axdec_class = {
    .class_name = "hevc_axdec",
    .item_name  = av_default_item_name,
    .option     = hevc_options,
    .version    = LIBAVUTIL_VERSION_INT,
};

const FFCodec ff_hevc_ax_decoder = {
    .p.name           = "hevc_axdec",
    CODEC_LONG_NAME("AXERA HEVC DECODER"),
    .priv_data_size = sizeof(AXVdecContext),
    .p.type           = AVMEDIA_TYPE_VIDEO,
    .p.id             = AV_CODEC_ID_HEVC,
    .init           = ax_decode_init,
    FF_CODEC_DECODE_CB(ax_decode_frame),
    .close          = ax_decode_close,
    .p.capabilities   = AV_CODEC_CAP_DELAY | AV_CODEC_CAP_HARDWARE,
    .p.priv_class     = &hevc_axdec_class,
    .p.pix_fmts       = (const enum AVPixelFormat[]){ AV_PIX_FMT_AXMM,
                                                    AV_PIX_FMT_NV12,
                                                    AV_PIX_FMT_YUV420P,
                                                    AV_PIX_FMT_NV21,
                                                    AV_PIX_FMT_P010,
                                                    AV_PIX_FMT_NONE },
    .hw_configs     = ff_h2645_axdec_hw_configs,
    .bsfs           = "hevc_mp4toannexb",
    .p.wrapper_name   = "hevc_axdec",
};

static const AVOption options[] = {
    { "resize",   "Resize (width)x(height)", OFFSET(resize_expr), AV_OPT_TYPE_STRING, { .str = NULL }, 0, 0, VD },
    { "fmt", "Decoder output format (software format of hardware frames)", OFFSET(out_fmt_string), AV_OPT_TYPE_STRING, { .str = NULL }, 0, 0, VD  },
    { NULL }
};

static const AVClass h264_axdec_class = {
    .class_name = "h264_axdec",
    .item_name  = av_default_item_name,
    .option     = options,
    .version    = LIBAVUTIL_VERSION_INT,
};

const FFCodec ff_h264_ax_decoder = {
    .p.name           = "h264_axdec",
    CODEC_LONG_NAME("AXERA H264 DECODER"),
    .priv_data_size = sizeof(AXVdecContext),
    .p.type           = AVMEDIA_TYPE_VIDEO,
    .p.id             = AV_CODEC_ID_H264,
    .init           = ax_decode_init,
    FF_CODEC_DECODE_CB(ax_decode_frame),
    .close          = ax_decode_close,
    .p.capabilities   = AV_CODEC_CAP_DELAY | AV_CODEC_CAP_HARDWARE,
    .p.priv_class     = &h264_axdec_class,
    .p.pix_fmts       = (const enum AVPixelFormat[]){ AV_PIX_FMT_AXMM,
                                                    AV_PIX_FMT_NV12,
                                                    AV_PIX_FMT_NV21,
                                                    AV_PIX_FMT_YUV420P,
                                                    AV_PIX_FMT_P010,
                                                    AV_PIX_FMT_NONE },
    .hw_configs     = ff_h2645_axdec_hw_configs,
    .bsfs           = "h264_mp4toannexb",
    .p.wrapper_name   = "h264_axdec",
};


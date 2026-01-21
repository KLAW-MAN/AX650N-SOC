/*
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

#include <unistd.h>
#include <dlfcn.h>

#include "libavutil/pixfmt.h"
#include "libavutil/pixdesc.h"
#include "libavutil/hwcontext.h"
#include "libavutil/internal.h"
#include "libavutil/hwcontext_axmm.h"

#include "avcodec.h"
#include "codec_internal.h"
#include "encode.h"

#include "axvenc.h"

#define OFFSET(x) offsetof(AXVencContext, x)
#define VE AV_OPT_FLAG_VIDEO_PARAM | AV_OPT_FLAG_ENCODING_PARAM

static int hevc_enc_attr_init(AVCodecContext *ctx)
{
    int qmin, qmax, i_qmin, i_qmax;
    AXVencContext *vctx = ctx->priv_data;
    AX_VENC_CHN_ATTR_T *chn_attr = &vctx->chn_attr;

    chn_attr->stVencAttr.enType = PT_H265;

    if (ctx->profile == FF_PROFILE_UNKNOWN)
        ctx->profile = vctx->profile;
    if (ctx->level == FF_LEVEL_UNKNOWN)
        ctx->level = vctx->level;

    switch (ctx->profile) {
    case FF_PROFILE_UNKNOWN:
        av_log(ctx, AV_LOG_INFO, "use h.265 main profile as the default\n");
        chn_attr->stVencAttr.enProfile = AX_VENC_HEVC_MAIN_PROFILE;
        break;
    case FF_PROFILE_HEVC_MAIN:
        chn_attr->stVencAttr.enProfile = AX_VENC_HEVC_MAIN_PROFILE;
        break;
    case FF_PROFILE_HEVC_MAIN_10:
        chn_attr->stVencAttr.enProfile = AX_VENC_HEVC_MAIN_10_PROFILE;
        break;
    default:
        av_log(ctx, AV_LOG_ERROR, "profile(%d) is not supported\n", ctx->profile);
        return AVERROR_PATCHWELCOME;
    }

    chn_attr->stVencAttr.enLevel = ctx->level;

    chn_attr->stVencAttr.enStrmBitDepth = AX_VENC_STREAM_BIT_8;

    qmin = ctx->qmin > 51 ? 51 : ctx->qmin;
    qmax = ctx->qmax > 51 ? 51 : ctx->qmax;
    if (qmin > qmax)
        qmin = qmax;

    i_qmin = (vctx->i_qmin == 52) ? qmin : vctx->i_qmin;
    i_qmax = (vctx->i_qmax == 52) ? qmax : vctx->i_qmax;
    if (i_qmin > i_qmax)
        i_qmin = i_qmax;

    av_log(ctx, AV_LOG_INFO, "%s rc:%d, qp:[%d,%d], i_qp:[%d,%d], bitrate:%ld\n", __func__, vctx->rc,
           qmin, qmax, i_qmin, i_qmax, ctx->bit_rate);

    chn_attr->stRcAttr.enRcMode = vctx->rc;

    switch (vctx->rc) {
    case AX_VENC_RC_MODE_H265CBR:
        chn_attr->stRcAttr.s32FirstFrameStartQp = vctx->firstFrameStartQp;
        chn_attr->stRcAttr.stH265Cbr.u32Gop = ctx->gop_size;
        chn_attr->stRcAttr.stH265Cbr.u32BitRate = (ctx->bit_rate >= 1000) ? (ctx->bit_rate + 500) / 1000 : 2000;
        chn_attr->stRcAttr.stH265Cbr.u32MinQp = qmin;
        chn_attr->stRcAttr.stH265Cbr.u32MaxQp = qmax;
        chn_attr->stRcAttr.stH265Cbr.u32MinIQp = i_qmin;
        chn_attr->stRcAttr.stH265Cbr.u32MaxIQp = i_qmax;
        chn_attr->stRcAttr.stH265Cbr.s32IntraQpDelta = vctx->intraQpDelta;
        chn_attr->stRcAttr.stH265Cbr.u32IdrQpDeltaRange = vctx->idrQpDeltaRange;
        chn_attr->stRcAttr.stH265Cbr.u32MaxIprop = vctx->maxIprop;
        chn_attr->stRcAttr.stH265Cbr.u32MinIprop = vctx->minIprop;
        chn_attr->stRcAttr.stH265Cbr.stQpmapInfo.enQpmapQpType = AX_VENC_QPMAP_QP_DISABLE;
        chn_attr->stRcAttr.stH265Cbr.stQpmapInfo.enQpmapBlockType = AX_VENC_QPMAP_BLOCK_DISABLE;
        chn_attr->stRcAttr.stH265Cbr.stQpmapInfo.enQpmapBlockUnit = AX_VENC_QPMAP_BLOCK_UNIT_64x64;
        chn_attr->stRcAttr.stH265Cbr.stQpmapInfo.enCtbRcMode = AX_VENC_RC_CTBRC_DISABLE;
        break;
    case AX_VENC_RC_MODE_H265VBR:
        chn_attr->stRcAttr.s32FirstFrameStartQp = vctx->firstFrameStartQp;
        chn_attr->stRcAttr.stH265Vbr.u32Gop = ctx->gop_size;
        chn_attr->stRcAttr.stH265Vbr.u32MaxBitRate = (ctx->bit_rate >= 1000) ? (ctx->bit_rate + 500) / 1000 : 2000;
        chn_attr->stRcAttr.stH265Vbr.u32MinQp = qmin;
        chn_attr->stRcAttr.stH265Vbr.u32MaxQp = qmax;
        chn_attr->stRcAttr.stH265Vbr.u32MinIQp = i_qmin;
        chn_attr->stRcAttr.stH265Vbr.u32MaxIQp = i_qmax;
        chn_attr->stRcAttr.stH265Vbr.s32IntraQpDelta = vctx->intraQpDelta;
        chn_attr->stRcAttr.stH265Vbr.u32IdrQpDeltaRange = vctx->idrQpDeltaRange;
        chn_attr->stRcAttr.stH265Vbr.enVQ = vctx->enVQ;
        chn_attr->stRcAttr.stH265Vbr.u32ChangePos = vctx->changePos;
        chn_attr->stRcAttr.stH265Vbr.stQpmapInfo.enQpmapQpType = AX_VENC_QPMAP_QP_DISABLE;
        chn_attr->stRcAttr.stH265Vbr.stQpmapInfo.enQpmapBlockType = AX_VENC_QPMAP_BLOCK_DISABLE;
        chn_attr->stRcAttr.stH265Vbr.stQpmapInfo.enQpmapBlockUnit = AX_VENC_QPMAP_BLOCK_UNIT_64x64;
        chn_attr->stRcAttr.stH265Vbr.stQpmapInfo.enCtbRcMode = AX_VENC_RC_CTBRC_DISABLE;
        break;
    case AX_VENC_RC_MODE_H265AVBR:
        chn_attr->stRcAttr.s32FirstFrameStartQp = vctx->firstFrameStartQp;
        chn_attr->stRcAttr.stH265AVbr.u32Gop = ctx->gop_size;
        chn_attr->stRcAttr.stH265AVbr.u32MaxBitRate = (ctx->bit_rate >= 1000) ? (ctx->bit_rate + 500) / 1000 : 2000;
        chn_attr->stRcAttr.stH265AVbr.u32MinQp = qmin;
        chn_attr->stRcAttr.stH265AVbr.u32MaxQp = qmax;
        chn_attr->stRcAttr.stH265AVbr.u32MinIQp = i_qmin;
        chn_attr->stRcAttr.stH265AVbr.u32MaxIQp = i_qmax;
        chn_attr->stRcAttr.stH265AVbr.s32IntraQpDelta = vctx->intraQpDelta;
        chn_attr->stRcAttr.stH265AVbr.u32IdrQpDeltaRange = vctx->idrQpDeltaRange;
        chn_attr->stRcAttr.stH265AVbr.u32ChangePos = vctx->changePos;
        chn_attr->stRcAttr.stH265AVbr.u32MinStillPercent = vctx->minStillPercent;
        chn_attr->stRcAttr.stH265AVbr.u32MaxStillQp = vctx->maxStillQp;
        chn_attr->stRcAttr.stH265AVbr.stQpmapInfo.enQpmapQpType = AX_VENC_QPMAP_QP_DISABLE;
        chn_attr->stRcAttr.stH265AVbr.stQpmapInfo.enQpmapBlockType = AX_VENC_QPMAP_BLOCK_DISABLE;
        chn_attr->stRcAttr.stH265AVbr.stQpmapInfo.enQpmapBlockUnit = AX_VENC_QPMAP_BLOCK_UNIT_64x64;
        chn_attr->stRcAttr.stH265AVbr.stQpmapInfo.enCtbRcMode = AX_VENC_RC_CTBRC_DISABLE;
        break;
    case AX_VENC_RC_MODE_H265FIXQP:
        chn_attr->stRcAttr.s32FirstFrameStartQp = vctx->firstFrameStartQp;
        chn_attr->stRcAttr.stH265FixQp.u32Gop = ctx->gop_size;
        chn_attr->stRcAttr.stH265FixQp.u32IQp = i_qmax;
        chn_attr->stRcAttr.stH265FixQp.u32PQp = qmax;
        chn_attr->stRcAttr.stH265FixQp.u32BQp = 32;
        break;
    }

    return 0;
}

static av_cold int ff_hevc_axenc_init(AVCodecContext *ctx)
{
    AXVencContext *vctx = ctx->priv_data;

    if ((ctx->pix_fmt != AV_PIX_FMT_AXMM) && (!ctx->hw_device_ctx || ctx->hw_frames_ctx)){
        av_log(ctx, AV_LOG_ERROR, "%s format(%s) requires hw_device_ctx must be set\n", __func__,
               av_get_pix_fmt_name(ctx->pix_fmt));
        return AVERROR(EINVAL);
    }

    vctx->attr_init = hevc_enc_attr_init;

    return ff_axvenc_init(ctx);
}

static int ff_hevc_axenc_receive_packet(AVCodecContext *ctx, AVPacket *pkt)
{
    return ff_axvenc_receive_packet(ctx, pkt);
}

static av_cold int ff_hevc_axenc_close(AVCodecContext *ctx)
{
    return ff_axvenc_close(ctx);
}

static av_cold void ff_hevc_axenc_flush(AVCodecContext *ctx)
{
    ff_axvenc_flush(ctx);
}

static const AVOption options[] = {
    { "profile", "Set profile(support main and main_10)", OFFSET(profile), AV_OPT_TYPE_INT,
      { .i64 = FF_PROFILE_HEVC_MAIN }, FF_PROFILE_HEVC_MAIN, FF_PROFILE_HEVC_MAIN_10, VE, "profile" },

#define PROFILE(name, value)  name, NULL, 0, AV_OPT_TYPE_CONST, { .i64 = value }, 0, 0, VE, "profile"
    { PROFILE("main",     FF_PROFILE_HEVC_MAIN) },
    { PROFILE("main_10",     FF_PROFILE_HEVC_MAIN_10) },
#undef PROFILE

    { "level", "Set level", OFFSET(level), AV_OPT_TYPE_INT,
      { .i64 = AX_VENC_HEVC_LEVEL_5_1 }, AX_VENC_HEVC_LEVEL_1, AX_VENC_HEVC_LEVEL_6_2, VE, "level" },

#define LEVEL(name, value) name, NULL, 0, AV_OPT_TYPE_CONST, { .i64 = value }, 0, 0, VE, "level"
    { LEVEL("1",   AX_VENC_HEVC_LEVEL_1) },
    { LEVEL("2",   AX_VENC_HEVC_LEVEL_2) },
    { LEVEL("2.1", AX_VENC_HEVC_LEVEL_2_1) },
    { LEVEL("3",   AX_VENC_HEVC_LEVEL_3) },
    { LEVEL("3.1", AX_VENC_HEVC_LEVEL_3_1) },
    { LEVEL("4",   AX_VENC_HEVC_LEVEL_4) },
    { LEVEL("4.1", AX_VENC_HEVC_LEVEL_4_1) },
    { LEVEL("5",   AX_VENC_HEVC_LEVEL_5) },
    { LEVEL("5.1", AX_VENC_HEVC_LEVEL_5_1) },
    { LEVEL("5.2", AX_VENC_HEVC_LEVEL_5_2) },
    { LEVEL("6",   AX_VENC_HEVC_LEVEL_6) },
    { LEVEL("6.1", AX_VENC_HEVC_LEVEL_6_1) },
    { LEVEL("6.2", AX_VENC_HEVC_LEVEL_6_2) },
#undef LEVEL

    { "rc", "Override the preset rate-control", OFFSET(rc), AV_OPT_TYPE_INT, { .i64 = AX_VENC_RC_MODE_H265CBR }, AX_VENC_RC_MODE_H265CBR, AX_VENC_RC_MODE_H265QPMAP, VE, "rc" },
    { "cbr", "Constant bitrate mode", 0, AV_OPT_TYPE_CONST, { .i64 = AX_VENC_RC_MODE_H265CBR }, 0, 0, VE, "rc" },
    { "vbr", "Variable bitrate mode", 0, AV_OPT_TYPE_CONST, { .i64 = AX_VENC_RC_MODE_H265VBR }, 0, 0, VE, "rc" },
    { "avbr", "Adaptive variable bitrate mode", 0, AV_OPT_TYPE_CONST, { .i64 = AX_VENC_RC_MODE_H265AVBR }, 0, 0, VE, "rc" },
    { "fixqp", "Fix Qp mode", 0, AV_OPT_TYPE_CONST, { .i64 = AX_VENC_RC_MODE_H265FIXQP }, 0, 0, VE, "rc" },
    { "i_qmin", "I-frames minimum Qp", OFFSET(i_qmin), AV_OPT_TYPE_INT, { .i64 = 16 }, 0, 51, VE, "i_qmin" },
    { "i_qmax", "I-frames maximum Qp", OFFSET(i_qmax), AV_OPT_TYPE_INT, { .i64 = 51 }, 0, 51, VE, "i_qmax" },
    { "qaLevel", "vbr quality level", OFFSET(enVQ), AV_OPT_TYPE_INT, {.i64 = 0}, 0, 8, VE},
    { "intraQpDelta", "QP difference between target QP and intra frame QP", OFFSET(intraQpDelta), AV_OPT_TYPE_INT, {.i64 = 0}, -51, 51, VE},
    { "idrQpDeltaRange", "QP rang between CU QP and I frame QP", OFFSET(idrQpDeltaRange), AV_OPT_TYPE_INT, {.i64 = 10}, 2, 10, VE},
    { "maxIprop", "the max I P size ratio", OFFSET(maxIprop), AV_OPT_TYPE_INT, {.i64 = 40}, 1, 100, VE},
    { "minIprop", "the min I P size ratio", OFFSET(minIprop), AV_OPT_TYPE_INT, {.i64 = 10}, 1, 100, VE},
    { "changePos", "the percent of max bitrate when vbr start adjust bitrate", OFFSET(changePos), AV_OPT_TYPE_INT, {.i64 = 80}, 20, 100, VE},
    { "minStillPercent", NULL, OFFSET(minStillPercent), AV_OPT_TYPE_INT, {.i64 = 25}, 10, 100, VE},
    { "maxStillQp", NULL, OFFSET(maxStillQp), AV_OPT_TYPE_INT, {.i64 = 36}, 0, 51, VE},
    { "firstFrameStartQp", NULL, OFFSET(firstFrameStartQp), AV_OPT_TYPE_INT, {.i64 = 16}, 16, 51, VE},

    { NULL }
};

static const FFCodecDefault defaults[] = {
    { "b", "2M" },
    { "qmin", "16" },
    { "qmax", "51" },
    { "g", "30" },
    { NULL },
};

static const AVClass ff_ax_venc_class = {
    .class_name = "hevc_axenc",
    .item_name = av_default_item_name,
    .option = options,
    .version = LIBAVUTIL_VERSION_INT,
};

const enum AVPixelFormat ff_hevc_axenc_pix_fmts[] = {
    AV_PIX_FMT_NV12,
    AV_PIX_FMT_AXMM,
    AV_PIX_FMT_NONE
};

const AVCodecHWConfigInternal *ff_hevc_axenc_hw_configs[] = {
    HW_CONFIG_ENCODER_DEVICE(NV12, AXMM),
    NULL,
};
const FFCodec ff_hevc_ax_encoder = {
    .p.name           = "hevc_axenc",
    CODEC_LONG_NAME("AX H.265 Encoder"),
    .p.type           = AVMEDIA_TYPE_VIDEO,
    .p.id             = AV_CODEC_ID_HEVC,
    .init           = ff_hevc_axenc_init,
    FF_CODEC_RECEIVE_PACKET_CB(ff_hevc_axenc_receive_packet),
    .close          = ff_hevc_axenc_close,
    .flush          = ff_hevc_axenc_flush,
    .priv_data_size = sizeof(AXVencContext),
    .p.priv_class     = &ff_ax_venc_class,
    .defaults       = defaults,
    .p.capabilities   = AV_CODEC_CAP_DELAY | AV_CODEC_CAP_HARDWARE |
                      AV_CODEC_CAP_ENCODER_FLUSH,
    .caps_internal  = FF_CODEC_CAP_INIT_CLEANUP,
    .p.pix_fmts       = ff_hevc_axenc_pix_fmts,
    .p.wrapper_name   = "hevc_axenc",
    .hw_configs     = ff_hevc_axenc_hw_configs,
};
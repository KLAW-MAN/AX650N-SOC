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

#include "libavutil/buffer.h"
#include "libavutil/hwcontext.h"
#include "libavutil/log.h"
#include "libavutil/mem.h"
#include "libavutil/opt.h"
#include "libavutil/pixdesc.h"
#include "libavutil/hwcontext_axmm.h"

#include "avfilter.h"
#include "filters.h"
#include "formats.h"
#include "video.h"


typedef struct AxHWDownloadContext {
    const AVClass *class;

    AVBufferRef       *hwframes_ref;
    AVHWFramesContext *hwframes;
} AxHWDownloadContext;

static int axerahwdownload_query_formats(AVFilterContext *avctx)
{
    int err;

    if ((err = ff_formats_ref(ff_formats_pixdesc_filter(AV_PIX_FMT_FLAG_HWACCEL, 0),
                              &avctx->inputs[0]->outcfg.formats))  ||
        (err = ff_formats_ref(ff_formats_pixdesc_filter(0, AV_PIX_FMT_FLAG_HWACCEL),
                              &avctx->outputs[0]->incfg.formats)))
        return err;

    return 0;
}

static int axerahwdownload_config_input(AVFilterLink *inlink)
{
    FilterLink          *l = ff_filter_link(inlink);
    AVFilterContext *avctx = inlink->dst;
    AxHWDownloadContext *ctx = avctx->priv;

    av_buffer_unref(&ctx->hwframes_ref);

    if (!l->hw_frames_ctx) {
        av_log(ctx, AV_LOG_ERROR, "The input must have a hardware frame "
               "reference.\n");
        return AVERROR(EINVAL);
    }

    ctx->hwframes_ref = av_buffer_ref(l->hw_frames_ctx);
    if (!ctx->hwframes_ref)
        return AVERROR(ENOMEM);

    ctx->hwframes = (AVHWFramesContext*)ctx->hwframes_ref->data;

    return 0;
}

static int axerahwdownload_config_output(AVFilterLink *outlink)
{
    AVFilterContext *avctx = outlink->src;
    AVFilterLink *inlink   = avctx->inputs[0];
    AxHWDownloadContext *ctx = avctx->priv;
    enum AVPixelFormat *formats;
    int err, i, found;

    if (!ctx->hwframes_ref)
        return AVERROR(EINVAL);

    err = av_hwframe_transfer_get_formats(ctx->hwframes_ref,
                                          AV_HWFRAME_TRANSFER_DIRECTION_FROM,
                                          &formats, 0);
    if (err < 0)
        return err;

    found = 0;
    for (i = 0; formats[i] != AV_PIX_FMT_NONE; i++) {
        if (formats[i] == outlink->format) {
            found = 1;
            break;
        }
    }
    av_freep(&formats);

    if (!found) {
        av_log(ctx, AV_LOG_ERROR, "Invalid output format %s for hwframe "
               "download.\n", av_get_pix_fmt_name(outlink->format));
        return AVERROR(EINVAL);
    }

    outlink->w = inlink->w;
    outlink->h = inlink->h;

    return 0;
}

static int axerahwdownload_filter_frame(AVFilterLink *link, AVFrame *input)
{
    AVFilterContext *avctx = link->dst;
    AVFilterLink  *outlink = avctx->outputs[0];
    AxHWDownloadContext *ctx = avctx->priv;
    AVHWFramesContext *hwfc_src = NULL;
    AXMMFramesContext *hwfc_priv = NULL;
    AX_VIDEO_FRAME_T *ptDst = NULL;
    AX_IMG_FORMAT_E     enImgFormat = AX_FORMAT_INVALID;
    int err;

    if (!ctx->hwframes_ref || !input->hw_frames_ctx) {
        av_log(ctx, AV_LOG_ERROR, "Input frames must have hardware context.\n");
        err = AVERROR(EINVAL);
        goto fail;
    }
    if ((void*)ctx->hwframes != input->hw_frames_ctx->data) {
        av_log(ctx, AV_LOG_ERROR, "Input frame is not the in the configured "
               "hwframe context.\n");
        err = AVERROR(EINVAL);
        goto fail;
    }

    hwfc_src = (AVHWFramesContext *)input->hw_frames_ctx->data;
    hwfc_priv = hwfc_src->hwctx;

    if (hwfc_priv->alloc_blk) {
        ptDst = (AX_VIDEO_FRAME_T *)input->data[3];
    } else {
        ptDst = (AX_VIDEO_FRAME_T *)input->data[4];
    }

    switch (outlink->format) {
        case AV_PIX_FMT_NV12:
            enImgFormat = AX_FORMAT_YUV420_SEMIPLANAR;
            break;
        case AV_PIX_FMT_NV21:
            enImgFormat = AX_FORMAT_YUV420_SEMIPLANAR_VU;
            break;
        case AV_PIX_FMT_P010:
            enImgFormat = AX_FORMAT_YUV420_SEMIPLANAR_10BIT_P010;
            break;
        case AV_PIX_FMT_P016:
            enImgFormat = AX_FORMAT_YUV420_SEMIPLANAR_10BIT_P016;
            break;
        case AV_PIX_FMT_Y400A:
            enImgFormat = AX_FORMAT_YUV400;
            break;
        case  AV_PIX_FMT_RGB24:
            enImgFormat = AX_FORMAT_RGB888;
            break;
        default:
            av_log(ctx, AV_LOG_ERROR, "%s Unsupport ImgFormat:%d\n", __func__, outlink->format);
            err = -1;
            goto fail;
    }

    av_log(ctx, AV_LOG_DEBUG, "%s format:out:%d, in:%d, ImgFormat:%d, %d, alloc_blk:%d\n", __func__,
           outlink->format, input->format, ptDst->enImgFormat, enImgFormat, hwfc_priv->alloc_blk);

    if (enImgFormat == ptDst->enImgFormat) {
        input->format = outlink->format;
        input->data[0] = ptDst->u64VirAddr[0];
        input->data[1] = ptDst->u64VirAddr[0] + ptDst->u32Width * ptDst->u32Height;

        return ff_filter_frame(avctx->outputs[0], input);
    } else {
        av_log(ctx, AV_LOG_ERROR, "Format error for frame \n");
        err = AVERROR(EINVAL);
    }

fail:
    av_frame_free(&input);
    return err;
}

static av_cold void axerahwdownload_uninit(AVFilterContext *avctx)
{
    AxHWDownloadContext *ctx = avctx->priv;

    av_buffer_unref(&ctx->hwframes_ref);
}

static const AVClass axerahwdownload_class = {
    .class_name = "hwdownload_axera",
    .item_name  = av_default_item_name,
    .option     = NULL,
    .version    = LIBAVUTIL_VERSION_INT,
};

static const AVFilterPad axerahwdownload_inputs[] = {
    {
        .name         = "default",
        .type         = AVMEDIA_TYPE_VIDEO,
        .config_props = axerahwdownload_config_input,
        .filter_frame = axerahwdownload_filter_frame,
    },
};

static const AVFilterPad axerahwdownload_outputs[] = {
    {
        .name         = "default",
        .type         = AVMEDIA_TYPE_VIDEO,
        .config_props = axerahwdownload_config_output,
    },
};

const AVFilter ff_vf_hwdownload_axera = {
    .name          = "hwdownload_axera",
    .description   = NULL_IF_CONFIG_SMALL("Download a hardware frame to a normal frame"),
    .uninit        = axerahwdownload_uninit,
    .priv_size     = sizeof(AxHWDownloadContext),
    .priv_class    = &axerahwdownload_class,
    FILTER_INPUTS(axerahwdownload_inputs),
    FILTER_OUTPUTS(axerahwdownload_outputs),
    FILTER_QUERY_FUNC(axerahwdownload_query_formats),
    .flags_internal = FF_FILTER_FLAG_HWFRAME_AWARE,
};

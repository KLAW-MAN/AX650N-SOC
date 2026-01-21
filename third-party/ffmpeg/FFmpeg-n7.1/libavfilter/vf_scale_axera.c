/*
* Axera Semiconductor Avfilter
*
* Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd.

*
* Permission is hereby granted, free of charge, to any person obtaining a
* copy of this software and associated documentation files (the "Software"),
* to deal in the Software without restriction, including without limitation
* the rights to use, copy, modify, merge, publish, distribute, sublicense,
* and/or sell copies of the Software, and to permit persons to whom the
* Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
* DEALINGS IN THE SOFTWARE.
*/

#include <stdio.h>
#include <string.h>
#include <dlfcn.h>

#include "libavutil/avstring.h"
#include "libavutil/common.h"
#include "libavutil/hwcontext.h"
#include "libavutil/internal.h"
#include "libavutil/opt.h"
#include "libavutil/pixdesc.h"
#include "libavutil/hwcontext_axmm.h"
#include "libavutil/time.h"

#include "avfilter.h"
#include "filters.h"
#include "formats.h"
#include "scale_eval.h"
#include "video.h"
#include "ax_ivps_api.h"

static const enum AVPixelFormat supported_formats[] = {
    AV_PIX_FMT_NV12,
    AV_PIX_FMT_NV21,
    AV_PIX_FMT_P010,
    AV_PIX_FMT_P016,
    AV_PIX_FMT_Y400A,
    AV_PIX_FMT_RGB24,
};

#define DIV_UP(a, b) ( ((a) + (b) - 1) / (b) )
#define ALIGN_UP(a, b) (((a) + (b) - 1) & ~((b) - 1))
#define NUM_BUFFERS 2
#define BLOCKX 32
#define BLOCKY 16

#define LIB_IVPS_NAME    "libax_ivps.so"


#define CHECK_WITH_RET(cond, ret) \
    do { \
        if ((cond) != 0) { \
            av_log(NULL, AV_LOG_ERROR, "check %s failed\n", #cond); \
            return (ret); \
        } \
    } while (0)

typedef struct {
    AX_S32(*ivps_init)(void);
    AX_S32(*ivps_deinit)(void);
    AX_S32(*ivps_resize)(const AX_VIDEO_FRAME_T *ptSrc, AX_VIDEO_FRAME_T *ptDst, const AX_IVPS_ASPECT_RATIO_T *ptAspectRatio);
} AXIvpsFuncs;

enum ScalerFunc {
    F_BILINEAR = 0,
    F_NEAREST,

    F_NB,
};

typedef struct AXIvpsContext {
    const AVClass *class;

    enum AVPixelFormat in_fmt;
    enum AVPixelFormat out_fmt;

    struct {
        int width;
        int height;
    } planes_in[3], planes_out[3];

    AVBufferRef *frames_ctx;
    AVFrame *frame;

    AVFrame *tmp_frame;
    int passthrough;

    /**
     * Output sw format. AV_PIX_FMT_NONE for no conversion.
     */
    enum AVPixelFormat format;

    enum ScalerFunc scaler;
    char *out_format_string;

    char *w_expr;               ///< width  expression string
    char *h_expr;               ///< height expression string

    int force_original_aspect_ratio;
    int force_divisible_by;

    int initial_pool_size;

    void *ivps_lib;
    AXIvpsFuncs ivps_funcs;
} AXIvpsContext;

static int ax_ivps_dl(AXIvpsContext *s)
{
    AXIvpsFuncs *ivps_funcs = &s->ivps_funcs;

    s->ivps_lib = dlopen(LIB_IVPS_NAME, RTLD_NOW | RTLD_GLOBAL);
    CHECK_WITH_RET(!s->ivps_lib, AVERROR(EINVAL));

    ivps_funcs->ivps_init = dlsym(s->ivps_lib, "AX_IVPS_Init");
    CHECK_WITH_RET(!ivps_funcs->ivps_init, AVERROR(EINVAL));

    ivps_funcs->ivps_deinit = dlsym(s->ivps_lib, "AX_IVPS_Deinit");
    CHECK_WITH_RET(!ivps_funcs->ivps_deinit, AVERROR(EINVAL));

    ivps_funcs->ivps_resize = dlsym(s->ivps_lib, "AX_IVPS_CropResizeTdp");
    CHECK_WITH_RET(!ivps_funcs->ivps_resize, AVERROR(EINVAL));

    return 0;
}

static av_cold int axera_scale_init(AVFilterContext *ctx)
{
    AXIvpsContext *s = ctx->priv;
    int ret = -1;

    ret = ax_ivps_dl(s);
    if (ret) {
        av_log(s, AV_LOG_ERROR, "%s ax_ivps_dl failed, ret = 0x%x\n", __func__, ret);
        goto ERR_RET;
    }

    s->format = AV_PIX_FMT_NONE;
    s->passthrough = 0;
    s->frames_ctx = NULL;

    s->frame = av_frame_alloc();
    if (!s->frame) {
        ret = AVERROR(ENOMEM);
        av_log(s, AV_LOG_ERROR, "%s av_frame_alloc failed, ret = 0x%x\n", __func__, ret);
        goto ERR_RET1;
    }
    av_log(s, AV_LOG_INFO, "%s Enter!!\n", __func__);

    return 0;

ERR_RET1:
    if (s->ivps_lib)
        dlclose(s->ivps_lib);
ERR_RET:
    return ret;
}

static av_cold void axera_scale_uninit(AVFilterContext *ctx)
{
    AXIvpsContext *s = ctx->priv;
    int ret = -1;
    FilterLink *inl = ff_filter_link(ctx->inputs[0]);
    AVHWFramesContext *in_frames_ctx;
    AVHWDeviceContext *device_ctx;
    AXMMDeviceContext *hwdc_priv;

    av_log(s, AV_LOG_INFO, "%s Enter!!\n", __func__);

    ret = s->ivps_funcs.ivps_deinit();
    if (ret) {
        av_log(s, AV_LOG_ERROR, "%s ivps_deinit failed, ret = 0x%x\n", __func__, ret);
    }

    av_frame_free(&s->frame);

    if (s->frames_ctx)
        av_buffer_unref(&s->frames_ctx);

    if (s->ivps_lib)
        dlclose(s->ivps_lib);

    av_log(s, AV_LOG_DEBUG, "%s End!!\n", __func__);
}

static av_cold int init_stage(AXIvpsContext *s, AVBufferRef *device_ctx)
{
    AVBufferRef *out_ref = NULL;
    AVHWFramesContext *out_ctx;
    int in_sw, in_sh, out_sw, out_sh;
    int ret, i;

    av_pix_fmt_get_chroma_sub_sample(s->in_fmt,  &in_sw,  &in_sh);
    av_pix_fmt_get_chroma_sub_sample(s->out_fmt, &out_sw, &out_sh);
    if (!s->planes_out[0].width) {
        s->planes_out[0].width  = s->planes_in[0].width;
        s->planes_out[0].height = s->planes_in[0].height;
    }

    for (i = 1; i < FF_ARRAY_ELEMS(s->planes_in); i++) {
        s->planes_in[i].width   = s->planes_in[0].width   >> in_sw;
        s->planes_in[i].height  = s->planes_in[0].height  >> in_sh;
        s->planes_out[i].width  = s->planes_out[0].width  >> out_sw;
        s->planes_out[i].height = s->planes_out[0].height >> out_sh;
    }

    out_ref = av_hwframe_ctx_alloc(device_ctx);
    if (!out_ref)
        return AVERROR(ENOMEM);
    out_ctx = (AVHWFramesContext*)out_ref->data;

    out_ctx->format    = AV_PIX_FMT_AXMM;
    out_ctx->sw_format = s->out_fmt;
    out_ctx->width     = FFALIGN(s->planes_out[0].width,  16);
    out_ctx->height    = FFALIGN(s->planes_out[0].height, 16);
    out_ctx->initial_pool_size = s->initial_pool_size;

    ret = av_hwframe_ctx_init(out_ref);
    if (ret < 0)
        goto fail;

    av_frame_unref(s->frame);

    s->frame->width  = s->planes_out[0].width;
    s->frame->height = s->planes_out[0].height;

    av_buffer_unref(&s->frames_ctx);
    s->frames_ctx = out_ref;

    return 0;
fail:
    av_buffer_unref(&out_ref);
    return ret;
}

static int format_is_supported(enum AVPixelFormat fmt)
{
    int i;

    for (i = 0; i < FF_ARRAY_ELEMS(supported_formats); i++)
        if (supported_formats[i] == fmt)
            return 1;
    return 0;
}

static av_cold int init_processing_chain(AVFilterContext *ctx, int in_width, int in_height,
                                         int out_width, int out_height)
{
    AXIvpsContext *s = ctx->priv;
    FilterLink     *inl = ff_filter_link(ctx->inputs[0]);
    FilterLink    *outl = ff_filter_link(ctx->outputs[0]);

    AVHWFramesContext *in_frames_ctx;
    AVHWDeviceContext *device_ctx;
    AXMMDeviceContext *hwdc_priv;

    enum AVPixelFormat in_format;
    enum AVPixelFormat out_format;
    int ret;
    AVDictionary *opts = NULL;

    /* check that we have a hw context */
    if (!inl->hw_frames_ctx) {
        av_log(ctx, AV_LOG_ERROR, "No hw context provided on input\n");
        return AVERROR(EINVAL);
    }

    if (s->out_format_string) {
        s->format = av_get_pix_fmt(s->out_format_string);
        if (s->format == AV_PIX_FMT_NONE) {
            av_log(s, AV_LOG_ERROR, "Invalid output format.\n");
            return AVERROR(EINVAL);
        }
    }
    in_frames_ctx = (AVHWFramesContext*)inl->hw_frames_ctx->data;
    in_format     = in_frames_ctx->sw_format;
    out_format    = (s->format == AV_PIX_FMT_NONE) ? in_format : s->format;

    if (!format_is_supported(in_format)) {
        av_log(ctx, AV_LOG_ERROR, "Unsupported input format: %s\n",
               av_get_pix_fmt_name(in_format));
        return AVERROR(ENOSYS);
    }
    if (!format_is_supported(out_format)) {
        av_log(ctx, AV_LOG_ERROR, "Unsupported output format: %s\n",
               av_get_pix_fmt_name(out_format));
        return AVERROR(ENOSYS);
    }

    ret = s->ivps_funcs.ivps_init();
    if (ret) {
        av_log(s, AV_LOG_ERROR, "%s ivps_init failed, ret = 0x%x\n", __func__, ret);
        goto ERR_RET;
    }

    device_ctx = in_frames_ctx->device_ctx;
    hwdc_priv = device_ctx->hwctx;

    if (in_frames_ctx->initial_pool_size) {
        s->initial_pool_size = in_frames_ctx->initial_pool_size;
    } else {
        s->initial_pool_size = 4;
    }

    /* for ivps out frame */
    av_dict_set(&opts, "alloc_blk", "1", 0);

    ret = av_hwdevice_ctx_create(&s->frames_ctx, AV_HWDEVICE_TYPE_AXMM,
                                 NULL, opts, 0);
    if (ret < 0) {
        av_log(s, AV_LOG_ERROR, "Failed to create a VAAPI device. Error code: %s\n", av_err2str(ret));
        goto ERR_RET1;
    }

    av_log(ctx, AV_LOG_DEBUG, " input format: %s, output format: %s, format:%s\n",
           av_get_pix_fmt_name(in_format), av_get_pix_fmt_name(out_format), av_get_pix_fmt_name(s->format));

    av_log(ctx, AV_LOG_INFO, " initial_pool_size: %d\n", in_frames_ctx->initial_pool_size);

    if (in_width == out_width && in_height == out_height && in_format == out_format)
        s->passthrough = 1;

    s->in_fmt = in_format;
    s->out_fmt = out_format;

    s->planes_in[0].width   = in_width;
    s->planes_in[0].height  = in_height;
    s->planes_out[0].width  = out_width;
    s->planes_out[0].height = out_height;

    ret = init_stage(s, s->frames_ctx);
    if (ret < 0)
        goto ERR_RET2;

    outl->hw_frames_ctx = av_buffer_ref(s->frames_ctx);
    if (!outl->hw_frames_ctx) {
        ret = AVERROR(ENOMEM);
        goto ERR_RET2;
    }

    return 0;

ERR_RET2:
    av_buffer_unref(&s->frames_ctx);
ERR_RET1:
    s->ivps_funcs.ivps_deinit();
ERR_RET:
    return ret;
}

static av_cold int axera_scale_config_props(AVFilterLink *outlink)
{
    AVFilterContext *ctx = outlink->src;
    AVFilterLink *inlink = outlink->src->inputs[0];
    AXIvpsContext *s  = ctx->priv;

    int w, h;
    int ret;

    if ((ret = ff_scale_eval_dimensions(s,
                                        s->w_expr, s->h_expr,
                                        inlink, outlink,
                                        &w, &h)) < 0)
        goto fail;

    ff_scale_adjust_dimensions(inlink, &w, &h,
                               s->force_original_aspect_ratio, s->force_divisible_by);

    av_log(s, AV_LOG_DEBUG, "%s w:%d, h:%d, in_w:%d, in_h:%d\n", __func__, w, h, inlink->w, inlink->h);

    if (((int64_t)h * inlink->w) > INT_MAX  ||
        ((int64_t)w * inlink->h) > INT_MAX)
        av_log(ctx, AV_LOG_ERROR, "Rescaled value for width or height is too big.\n");

    outlink->w = w;
    outlink->h = h;

    ret = init_processing_chain(ctx, inlink->w, inlink->h, outlink->w, outlink->h);
    if (ret < 0)
        return ret;

    av_log(ctx, AV_LOG_VERBOSE, "ax--scaler w:%d h:%d -> w:%d h:%d\n",
           inlink->w, inlink->h, outlink->w, outlink->h);

    if (inlink->sample_aspect_ratio.num) {
        outlink->sample_aspect_ratio = av_mul_q((AVRational){outlink->h*inlink->w,
                                                             outlink->w*inlink->h},
                                                inlink->sample_aspect_ratio);
    } else {
        outlink->sample_aspect_ratio = inlink->sample_aspect_ratio;
    }

    return 0;

fail:
    return ret;
}

static int axera_scale_filter_frame(AVFilterLink *link, AVFrame *in)
{
    AVFilterContext       *ctx = link->dst;
    AXIvpsContext        *s = ctx->priv;
    AVFilterLink      *outlink = ctx->outputs[0];

    AVFrame *out = NULL;
    const AX_VIDEO_FRAME_T ptSrc = {0};
    AX_VIDEO_FRAME_T *ptDst = NULL;
    const AX_IVPS_ASPECT_RATIO_T ptAspectRatio;
    int ret = 0;
    AX_S32 retry_count = 3;

    if (s->passthrough)
        return ff_filter_frame(outlink, in);

    out = av_frame_alloc();
    if (!out) {
        ret = AVERROR(ENOMEM);
        goto ERR_RET;
    }

    /* for try get block */
    retry:
    ret = av_hwframe_get_buffer(s->frames_ctx, out, 0);
    if (ret < 0) {
        retry_count--;

        if (!retry_count) {
            av_log(s, AV_LOG_ERROR, "%s av_hwframe_get_buffer failed, ret = 0x%x\n", __func__, ret);
            goto ERR_RET;
        }
        av_usleep(1000);
        goto retry;
    }

    memset(&ptAspectRatio, 0x00, sizeof(ptAspectRatio));

    /* video frame copy to ivps source frame */
    memcpy(&ptSrc, in->data[4], sizeof(ptSrc));
    ptDst = (AX_VIDEO_FRAME_T *)out->data[3];

    switch (s->out_fmt) {
        case AV_PIX_FMT_NV12:
            ptDst->enImgFormat = AX_FORMAT_YUV420_SEMIPLANAR;
            break;
        case AV_PIX_FMT_NV21:
            ptDst->enImgFormat = AX_FORMAT_YUV420_SEMIPLANAR_VU;
            break;
        case AV_PIX_FMT_P010:
            ptDst->enImgFormat = AX_FORMAT_YUV420_SEMIPLANAR_10BIT_P010;
            break;
        case AV_PIX_FMT_P016:
            ptDst->enImgFormat = AX_FORMAT_YUV420_SEMIPLANAR_10BIT_P016;
            break;
        case AV_PIX_FMT_Y400A:
            ptDst->enImgFormat = AX_FORMAT_YUV400;
            break;
        case  AV_PIX_FMT_RGB24:
            ptDst->enImgFormat = AX_FORMAT_RGB888;
            break;
        default:
            av_log(s, AV_LOG_ERROR, "%s Unsupport ImgFormat:%d\n", __func__, s->out_fmt);
            ret = -1;
            goto ERR_RET;
    }

    ptDst->u32Width = outlink->w;
    ptDst->u32Height = outlink->h;

    ret = s->ivps_funcs.ivps_resize(&ptSrc, ptDst, &ptAspectRatio);
    if (ret) {
        av_log(s, AV_LOG_ERROR, "%s ivps_resize failed, ret = 0x%x\n", __func__, ret);
        goto ERR_RET;
    }
    ptDst->u64PTS = ptSrc.u64PTS;
    out->pts = in->pts;
#if FF_API_PKT_PTS
FF_DISABLE_DEPRECATION_WARNINGS
    out->pkt_pts = in->pts;
FF_ENABLE_DEPRECATION_WARNINGS
#endif

    av_log(s, AV_LOG_DEBUG, "%s src w:%d, h:%d, dst w:%d, h:%d, f:%d, height:%d, out_fmt:%d\n", __func__,
           in->width, in->height, outlink->w, outlink->h, out->format, out->height, s->out_fmt);

    av_frame_free(&in);
    return ff_filter_frame(outlink, out);

ERR_RET:
    av_frame_free(&in);
    av_frame_free(&out);
    return ret;
}

#define OFFSET(x) offsetof(AXIvpsContext, x)
#define FLAGS (AV_OPT_FLAG_FILTERING_PARAM|AV_OPT_FLAG_VIDEO_PARAM)

static const AVOption options[] = {
	{ "w",      "Output video width",  OFFSET(w_expr),     AV_OPT_TYPE_STRING, { .str = "iw"   }, .flags = FLAGS},
    { "h",      "Output video height", OFFSET(h_expr),     AV_OPT_TYPE_STRING, { .str = "ih"   }, .flags = FLAGS },
    { "scaler", "Scaler function", OFFSET(scaler), AV_OPT_TYPE_INT, {.i64 = F_BILINEAR}, 0, F_NB, .flags = FLAGS, "scaler" },
        { "bilinear", "Bilinear interpolation (fastest)", 0, AV_OPT_TYPE_CONST, {.i64 = F_BILINEAR}, 0, 0, .flags = FLAGS, "scaler" },
        { "nearest", "Nearest (useful for pixel art)", 0, AV_OPT_TYPE_CONST, {.i64 = F_NEAREST}, 0, 0, .flags = FLAGS, "scaler" },
    { "format", "Output video format (software format of hardware frames)", OFFSET(out_format_string), AV_OPT_TYPE_STRING, .flags = FLAGS },
    { "force_original_aspect_ratio", "decrease or increase w/h if necessary to keep the original AR", OFFSET(force_original_aspect_ratio), AV_OPT_TYPE_INT, { .i64 = 0}, 0, 2, FLAGS, "force_oar" },
    { "disable",  NULL, 0, AV_OPT_TYPE_CONST, {.i64 = 0 }, 0, 0, FLAGS, "force_oar" },
    { "decrease", NULL, 0, AV_OPT_TYPE_CONST, {.i64 = 1 }, 0, 0, FLAGS, "force_oar" },
    { "increase", NULL, 0, AV_OPT_TYPE_CONST, {.i64 = 2 }, 0, 0, FLAGS, "force_oar" },
    { "force_divisible_by", "enforce that the output resolution is divisible by a defined integer when force_original_aspect_ratio is used", OFFSET(force_divisible_by), AV_OPT_TYPE_INT, { .i64 = 1}, 1, 16, FLAGS },
    { NULL },
};

static const AVClass ax_ivps_class = {
    .class_name = "ax_scale",
    .item_name  = av_default_item_name,
    .option     = options,
    .version    = LIBAVUTIL_VERSION_INT,
};

static const AVFilterPad ax_ivps_inputs[] = {
    {
        .name        = "default",
        .type        = AVMEDIA_TYPE_VIDEO,
        .filter_frame = axera_scale_filter_frame,
    },
};

static const AVFilterPad ax_ivps_outputs[] = {
    {
        .name         = "default",
        .type         = AVMEDIA_TYPE_VIDEO,
        .config_props = axera_scale_config_props,
    },
};

const AVFilter ff_vf_ax_scale = {
    .name      = "ax_scale",
    .description = NULL_IF_CONFIG_SMALL("Frame resizer and format conversion"),

    .init          = axera_scale_init,
    .uninit        = axera_scale_uninit,
    //.query_formats = axera_scale_query_formats,

    .priv_size = sizeof(AXIvpsContext),
    .priv_class = &ax_ivps_class,

    FILTER_INPUTS(ax_ivps_inputs),
    FILTER_OUTPUTS(ax_ivps_outputs),
    FILTER_SINGLE_PIXFMT(AV_PIX_FMT_AXMM),

    .flags_internal = FF_FILTER_FLAG_HWFRAME_AWARE,
};

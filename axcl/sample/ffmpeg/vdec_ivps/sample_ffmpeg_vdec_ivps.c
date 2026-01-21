/**********************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **********************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <assert.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

#include "ax_base_type.h"
#include "ax_global_type.h"
#include "axcl.h"

#include "libavcodec/codec_id.h"
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavutil/opt.h"
#include "libavutil/pixdesc.h"
#include "libavutil/bprint.h"
#include "libavutil/hwcontext.h"

#include "libavfilter/avfilter.h"
#include "libavfilter/buffersink.h"
#include "libavfilter/buffersrc.h"
#include "sample_ffmpeg.h"


static AX_S32 gLoopExit = 0;
static AX_S32 gWriteFrames = 0;

int frame_num = 0;
static AVBufferRef *hw_device_ctx = NULL;

#define SIZE_ALIGN(x,align) ((((x)+(align)-1)/(align))*(align))

#define SAMPLE_LOG_ERR(str, arg...)  do { \
        printf("[FFMPEG_SAMPLE][AXCL_VDEC][ERROR][%s][line:%d]"str"\n", \
                __func__, __LINE__, ##arg); \
    } while (0)

#define SAMPLE_LOG_INFO(str, arg...)  do { \
        printf("[FFMPEG_SAMPLE][AXCL_VDEC][INFO][%s][line:%d]"str"\n", \
                __func__, __LINE__, ##arg); \
    } while (0)

typedef struct FilterGraphPriv {
    FilterGraph      fg;

    // name used for logging
    char             log_name[32];

    int              is_simple;
    // true when the filtergraph contains only meta filters
    // that do not modify the frame data
    int              is_meta;
    // source filters are present in the graph
    int              have_sources;
    int              disable_conversions;

    unsigned         nb_outputs_done;

    const char      *graph_desc;

    char            *nb_threads;

    // frame for temporarily holding output from the filtergraph
    AVFrame         *frame;
    // frame for sending output to the encoder
    AVFrame         *frame_enc;

    unsigned         sch_idx;
} FilterGraphPriv;

static void SigInt(int sigNo)
{
    SAMPLE_LOG_INFO("Catch signal %d\n", sigNo);
    gLoopExit = 1;
}

#define FLAGS AV_OPT_FLAG_VIDEO_PARAM | AV_OPT_FLAG_DECODING_PARAM

static enum AVPixelFormat get_format(AVCodecContext *s, const enum AVPixelFormat *pix_fmts)
{
    const enum AVPixelFormat *p;

    for (p = pix_fmts; *p != AV_PIX_FMT_NONE; p++) {
        const AVPixFmtDescriptor *desc = av_pix_fmt_desc_get(*p);

        if (desc->flags & AV_PIX_FMT_FLAG_HWACCEL)
            break;
    }

    return *p;
}

void *allocate_array_elem(void *ptr, size_t elem_size, int *nb_elems)
{
    void *new_elem;

    if (!(new_elem = av_mallocz(elem_size)) ||
        av_dynarray_add_nofree(ptr, nb_elems, new_elem) < 0)
        return NULL;

    return new_elem;
}

static InputFilterPriv *ifp_from_ifilter(InputFilter *ifilter)
{
    return (InputFilterPriv*)ifilter;
}

static OutputFilterPriv *ofp_from_ofilter(OutputFilter *ofilter)
{
    return (OutputFilterPriv*)ofilter;
}

/* read file contents into a string */
char *file_read(const char *filename)
{
    AVIOContext *pb      = NULL;
    int ret = avio_open(&pb, filename, AVIO_FLAG_READ);
    AVBPrint bprint;
    char *str;

    if (ret < 0) {
        av_log(NULL, AV_LOG_ERROR, "Error opening file %s.\n", filename);
        return NULL;
    }

    av_bprint_init(&bprint, 0, AV_BPRINT_SIZE_UNLIMITED);
    ret = avio_read_to_bprint(pb, &bprint, SIZE_MAX);
    avio_closep(&pb);
    if (ret < 0) {
        av_bprint_finalize(&bprint, NULL);
        return NULL;
    }
    ret = av_bprint_finalize(&bprint, &str);
    if (ret < 0)
        return NULL;

    return str;
}

static int filter_opt_apply(AVFilterContext *f, const char *key, const char *val)
{
    const AVOption *o = NULL;
    int ret;

    ret = av_opt_set(f, key, val, AV_OPT_SEARCH_CHILDREN);
    if (ret >= 0)
        return 0;

    if (ret == AVERROR_OPTION_NOT_FOUND && key[0] == '/')
        o = av_opt_find(f, key + 1, NULL, 0, AV_OPT_SEARCH_CHILDREN);
    if (!o)
        goto err_apply;

    // key is a valid option name prefixed with '/'
    // interpret value as a path from which to load the actual option value
    key++;

    char *data = file_read(val);
    if (!data) {
        ret = AVERROR(EIO);
        goto err_load;
    }

    ret = av_opt_set(f, key, data, AV_OPT_SEARCH_CHILDREN);
    av_freep(&data);

    if (ret < 0)
        goto err_apply;

    return 0;

err_apply:
    av_log(NULL, AV_LOG_ERROR,
           "Error applying option '%s' to filter '%s': %s\n",
           key, f->filter->name, av_err2str(ret));
    return ret;
err_load:
    av_log(NULL, AV_LOG_ERROR,
           "Error loading value for option '%s' from file '%s'\n",
           key, val);
    return ret;
}

static int graph_opts_apply(AVFilterGraphSegment *seg)
{
    for (size_t i = 0; i < seg->nb_chains; i++) {
        AVFilterChain *ch = seg->chains[i];

        for (size_t j = 0; j < ch->nb_filters; j++) {
            AVFilterParams *p = ch->filters[j];
            const AVDictionaryEntry *e = NULL;

            if(!p->filter)
                return -1;

            while ((e = av_dict_iterate(p->opts, e))) {
                int ret = filter_opt_apply(p->filter, e->key, e->value);
                if (ret < 0)
                    return ret;
            }

            av_dict_free(&p->opts);
        }
    }

    return 0;
}

static int graph_parse(AVFilterGraph *graph, const char *desc,
                       AVFilterInOut **inputs, AVFilterInOut **outputs,
                       AVBufferRef *hw_device)
{
    AVFilterGraphSegment *seg;
    int ret;

    *inputs  = NULL;
    *outputs = NULL;

    ret = avfilter_graph_segment_parse(graph, desc, 0, &seg);
    if (ret < 0)
        return ret;

    ret = avfilter_graph_segment_create_filters(seg, 0);
    if (ret < 0) {
        goto fail;
    }
    hw_device = hw_device_ctx;

    if (hw_device) {
        for (int i = 0; i < graph->nb_filters; i++) {
            AVFilterContext *f = graph->filters[i];

            if (!(f->filter->flags & AVFILTER_FLAG_HWDEVICE))
                continue;
            f->hw_device_ctx = av_buffer_ref(hw_device);
            if (!f->hw_device_ctx) {
                ret = AVERROR(ENOMEM);
                goto fail;
            }
        }
    }

    ret = graph_opts_apply(seg);
    if (ret < 0) {
        goto fail;
    }

    ret = avfilter_graph_segment_apply(seg, 0, inputs, outputs);

fail:
    avfilter_graph_segment_free(&seg);
    return ret;
}

static char *describe_filter_link(FilterGraph *fg, AVFilterInOut *inout, int in)
{
    AVFilterContext *ctx = inout->filter_ctx;
    AVFilterPad *pads = in ? ctx->input_pads  : ctx->output_pads;
    int       nb_pads = in ? ctx->nb_inputs   : ctx->nb_outputs;

    if (nb_pads > 1)
        return av_strdup(ctx->filter->name);

    return av_asprintf("%s:%s", ctx->filter->name,
                       avfilter_pad_get_name(pads, inout->pad_idx));
}

static const char *ofilter_item_name(void *obj)
{
    OutputFilterPriv *ofp = obj;

    return ofp->log_name;
}

static const AVClass ofilter_class = {
    .class_name                = "OutputFilter",
    .version                   = LIBAVUTIL_VERSION_INT,
    .item_name                 = ofilter_item_name,
    .parent_log_context_offset = offsetof(OutputFilterPriv, log_parent),
    .category                  = AV_CLASS_CATEGORY_FILTER,
};

static OutputFilter *ofilter_alloc(FilterGraph *fg, enum AVMediaType type)
{
    OutputFilterPriv *ofp;
    OutputFilter *ofilter;

    ofp = allocate_array_elem(&fg->outputs, sizeof(*ofp), &fg->nb_outputs);
    if (!ofp)
        return NULL;

    ofilter           = &ofp->ofilter;
    ofilter->class    = &ofilter_class;
    ofp->log_parent   = fg;
    ofilter->graph    = fg;
    ofilter->type     = type;
    ofp->format       = AV_PIX_FMT_NV12;
    ofp->color_space  = AVCOL_SPC_UNSPECIFIED;
    ofp->color_range  = AVCOL_RANGE_UNSPECIFIED;
    ofp->index        = fg->nb_outputs - 1;

    snprintf(ofp->log_name, sizeof(ofp->log_name), "%co%d",
             av_get_media_type_string(type)[0], ofp->index);

    return ofilter;
}

static InputFilter *ifilter_alloc(FilterGraph *fg)
{
    InputFilterPriv *ifp;
    InputFilter *ifilter;

    ifp = allocate_array_elem(&fg->inputs, sizeof(*ifp), &fg->nb_inputs);
    if (!ifp)
        return NULL;

    ifilter         = &ifp->ifilter;
    ifilter->graph  = fg;

    ifp->frame = av_frame_alloc();
    if (!ifp->frame)
        return NULL;

    ifp->index           = fg->nb_inputs - 1;
    ifp->format          = AV_PIX_FMT_NV12;
    ifp->color_space     = AVCOL_SPC_UNSPECIFIED;
    ifp->color_range     = AVCOL_RANGE_UNSPECIFIED;

    return ifilter;
}

static int set_hwframe_ctx(AVCodecContext *ctx, AVBufferRef *hw_device_ctx) {
    AVBufferRef *hw_frames_ref;
    AVHWFramesContext *frames_ctx = NULL;
    int err = 0;

    if (!(hw_frames_ref = av_hwframe_ctx_alloc(hw_device_ctx))) {
        fprintf(stderr, "Failed to create hardware frame context.\n");
        return -1;
    }
    frames_ctx = (AVHWFramesContext *)(hw_frames_ref->data);
    frames_ctx->format = AV_PIX_FMT_AXMM;
    frames_ctx->sw_format = AV_PIX_FMT_NV12;
    frames_ctx->width = ctx->width;
    frames_ctx->height = ctx->height;
    frames_ctx->initial_pool_size = 20;
    if ((err = av_hwframe_ctx_init(hw_frames_ref)) < 0) {
        fprintf(stderr,
                "Failed to initialize hw frame context."
                "Error code: %s\n",
                av_err2str(err));
        av_buffer_unref(&hw_frames_ref);
        return err;
    }
    ctx->hw_frames_ctx = av_buffer_ref(hw_frames_ref);
    if (!ctx->hw_frames_ctx) {
        err = AVERROR(ENOMEM);
    }

    av_buffer_unref(&hw_frames_ref);
    return err;
}

static int configure_output_filter(FilterGraph *fg, AVFilterGraph *graph,
                                         OutputFilter *ofilter, AVFilterInOut *out)
{
    OutputFilterPriv *ofp = ofp_from_ofilter(ofilter);
    AVFilterContext *last_filter = out->filter_ctx;
    int pad_idx = out->pad_idx;
    int ret;
    char name[255];

    snprintf(name, sizeof(name), "out_#0:0");
    ofp->format = -1;
    ret = avfilter_graph_create_filter(&ofp->filter,
                                       avfilter_get_by_name("buffersink"),
                                       name, NULL, NULL, graph);
    if (ret < 0) {
        SAMPLE_LOG_ERR("avfilter_graph_create_filter out filter:%s failed\n", name);
        return ret;
    }

    if ((ret = avfilter_link(last_filter, pad_idx, ofp->filter, 0)) < 0) {
        SAMPLE_LOG_ERR("avfilter_link out filter failed\n");
        return ret;
    }

    return 0;
}

static int configure_input_filter(FilterGraph *fg, AVFilterGraph *graph,
                                        InputFilter *ifilter, AVFilterInOut *in)
{
    InputFilterPriv *ifp = ifp_from_ifilter(ifilter);

    AVFilterContext *last_filter;
    const AVFilter *buffer_filt = avfilter_get_by_name("buffer");
    const AVPixFmtDescriptor *desc;
    AVRational fr = ifp->opts.framerate;
    AVRational sar;
    AVBPrint args;
    char name[255];
    int ret;
    AVBufferSrcParameters *par = av_buffersrc_parameters_alloc();
    if (!par)
        return AVERROR(ENOMEM);


    sar = ifp->sample_aspect_ratio;
    if(!sar.den) {
        sar = (AVRational){0,1};
    }

    av_bprint_init(&args, 0, AV_BPRINT_SIZE_AUTOMATIC);
    av_bprintf(&args,
             "video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:"
             "pixel_aspect=%d/%d:colorspace=%d:range=%d",
             ifp->width, ifp->height, ifp->format,
             ifp->time_base.num, ifp->time_base.den, sar.num, sar.den,
             ifp->color_space, ifp->color_range);
    if (fr.num && fr.den)
        av_bprintf(&args, ":frame_rate=%d/%d", fr.num, fr.den);
    snprintf(name, sizeof(name), "graph %d input from stream 0:0", fg->index);

    SAMPLE_LOG_INFO("video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:"
             "pixel_aspect=%d/%d:colorspace=%d:range=%d\n",
             ifp->width, ifp->height, ifp->format,
             ifp->time_base.num, ifp->time_base.den, sar.num, sar.den,
             ifp->color_space, ifp->color_range);

    if ((ret = avfilter_graph_create_filter(&ifp->filter, buffer_filt, name,
                                            args.str, NULL, graph)) < 0) {
        SAMPLE_LOG_ERR("avfilter_graph_create_filter failed\n");
        goto fail;
    }

    par->hw_frames_ctx = ifp->hw_frames_ctx;
    ret = av_buffersrc_parameters_set(ifp->filter, par);
    if (ret < 0) {
        SAMPLE_LOG_ERR("av_buffersrc_parameters_set failed\n");
        goto fail;
    }
    av_freep(&par);
    last_filter = ifp->filter;

    desc = av_pix_fmt_desc_get(ifp->format);
    if (!desc) {
        SAMPLE_LOG_ERR("av_pix_fmt_desc_get failed\n");
        goto fail;
  }

    ifp->displaymatrix_applied = 0;

    if ((ret = avfilter_link(last_filter, 0, in->filter_ctx, in->pad_idx)) < 0) {
        SAMPLE_LOG_ERR("avfilter_link failed\n");
        return ret;
    }

    return 0;
fail:
    av_freep(&par);

    return ret;
}

static FilterGraphPriv *fgp_from_fg(FilterGraph *fg)
{
    return (FilterGraphPriv*)fg;
}

void GraphFree(FilterGraph **pfg)
{
    FilterGraph *fg = *pfg;
    FilterGraphPriv *fgp;

    if (!fg)
        return;
    fgp = fgp_from_fg(fg);

    for (int j = 0; j < fg->nb_inputs; j++) {
        InputFilter *ifilter = fg->inputs[j];
        InputFilterPriv *ifp = ifp_from_ifilter(ifilter);


        av_frame_free(&ifp->frame);
        av_frame_free(&ifp->opts.fallback);

        av_freep(&ifp->linklabel);
        av_freep(&ifp->opts.name);
        av_freep(&ifilter->name);
        av_freep(&fg->inputs[j]);
    }
    av_freep(&fg->inputs);
    for (int j = 0; j < fg->nb_outputs; j++) {
        OutputFilter *ofilter = fg->outputs[j];
        OutputFilterPriv *ofp = ofp_from_ofilter(ofilter);

        av_dict_free(&ofp->sws_opts);
        av_dict_free(&ofp->swr_opts);

        av_freep(&ofilter->linklabel);
        av_freep(&ofilter->name);
        av_freep(&ofilter->apad);
        av_freep(&ofp->name);
        av_channel_layout_uninit(&ofp->ch_layout);
        av_freep(&fg->outputs[j]);
    }
    av_freep(&fg->outputs);
    av_frame_free(&fgp->frame);
}

static int GraphCreate(FilterGraphPriv *fgp, AVCodecContext *ctx) 
{
    int ret, i;
    FilterGraph 	 *fg;
    AVFilterInOut *inputs, *outputs, *cur;

    AVFilterGraph *graph;

    fg = &fgp->fg;

    graph = avfilter_graph_alloc();
    if (!graph) {
        return AVERROR(ENOMEM);
    }
    graph->nb_threads = 1;

    ret = graph_parse(graph, fgp->graph_desc, &inputs, &outputs, NULL);
    if (ret < 0) {
        SAMPLE_LOG_ERR("graph_parse failed\n");
        goto ERR_RET1;
    }

    for (unsigned i = 0; i < graph->nb_filters; i++) {
        const AVFilter *f = graph->filters[i]->filter;
        if (!avfilter_filter_pad_count(f, 0) &&
            !(f->flags & AVFILTER_FLAG_DYNAMIC_INPUTS)) {
            fgp->have_sources = 1;
            break;
        }
    }

    for (AVFilterInOut *cur = inputs; cur; cur = cur->next) {
        InputFilter *const ifilter = ifilter_alloc(fg);
        InputFilterPriv *ifp;

        if (!ifilter) {
            ret = AVERROR(ENOMEM);
            goto ERR_RET1;
        }

        ifp            = ifp_from_ifilter(ifilter);
        ifp->linklabel = (uint8_t *)cur->name;
        cur->name      = NULL;
        ifp->hw_frames_ctx = ctx->hw_frames_ctx;
        ifp->width = ctx->width;
        ifp->height = ctx->height;
        ifp->time_base.num = 1;
        ifp->time_base.den = 1200000;
        ifp->format = ctx->pix_fmt;
        ifp->color_space = AVCOL_SPC_BT470BG;;
        ifp->color_range = AVCOL_RANGE_JPEG;

        ifp->type      = avfilter_pad_get_type(cur->filter_ctx->input_pads,
                                               cur->pad_idx);

        if (ifp->type != AVMEDIA_TYPE_VIDEO) {
            SAMPLE_LOG_ERR("Only video filters supported currently.\n");
            ret = AVERROR(ENOSYS);
            goto ERR_RET1;
        }

        ifilter->name  = (uint8_t *)describe_filter_link(fg, cur, 1);
        if (!ifilter->name) {
            SAMPLE_LOG_ERR("describe_filter_link in failed\n");
            ret = AVERROR(ENOMEM);
            goto ERR_RET1;
        }
    }

    for (AVFilterInOut *cur = outputs; cur; cur = cur->next) {
        const enum AVMediaType type = avfilter_pad_get_type(cur->filter_ctx->output_pads,
                                                            cur->pad_idx);
        OutputFilter *const ofilter = ofilter_alloc(fg, type);

        if (!ofilter) {
            SAMPLE_LOG_ERR("ofilter_alloc failed\n");
            ret = AVERROR(ENOMEM);
            goto ERR_RET1;
        }

        ofilter->linklabel = (uint8_t *)cur->name;
        cur->name          = NULL;

        ofilter->name      = (uint8_t *)describe_filter_link(fg, cur, 0);
        if (!ofilter->name) {
            SAMPLE_LOG_ERR("describe_filter_link out failed\n");
            ret = AVERROR(ENOMEM);
            goto ERR_RET1;
        }
    }

    for (cur = inputs, i = 0; cur; cur = cur->next, i++) {
        ret = configure_input_filter(fg, graph, fg->inputs[i], cur);
        if (ret < 0) {
            SAMPLE_LOG_ERR("configure_input_filter failed\n");
            avfilter_inout_free(&inputs);
            avfilter_inout_free(&outputs);
            goto ERR_RET1;
        }
    }
    avfilter_inout_free(&inputs);

    for (cur = outputs, i = 0; cur; cur = cur->next, i++) {
        ret = configure_output_filter(fg, graph, fg->outputs[i], cur);
        if (ret < 0) {
            SAMPLE_LOG_ERR("configure_output_filter failed\n");
            avfilter_inout_free(&outputs);
            goto ERR_RET1;
        }
    }
    avfilter_inout_free(&outputs);

    ret = avfilter_graph_config(graph, NULL);
    if (ret < 0) {
        SAMPLE_LOG_ERR("avfilter_graph_config failed\n");
        goto ERR_RET1;
    }

    return 0;

ERR_RET1:
    avfilter_graph_free(&graph);

    return ret;
}

static const char *fg_item_name(void *obj)
{
    const FilterGraphPriv *fgp = obj;

    return fgp->log_name;
}

static const AVClass fg_class = {
    .class_name = "FilterGraph",
    .version    = LIBAVUTIL_VERSION_INT,
    .item_name  = fg_item_name,
    .category   = AV_CLASS_CATEGORY_FILTER,
};

static void *ScaleThreadMain(void *arg)
{
    OutputFilterPriv *ofp = (OutputFilterPriv *)arg;
    AVFilterContext *filter = ofp->filter;
    AVFrame *frame;
    int ret, i;
    AX_VOID *p_lu = NULL;
    AX_VOID *p_ch = NULL;
    FILE *fp_out = NULL;

    frame = av_frame_alloc();

    if (gWriteFrames) {
        fp_out = fopen("out.yuv", "w");
        if (fp_out == NULL) {
            SAMPLE_LOG_ERR("Unable to open output file\n");
            return NULL;
        }
    }

    while (1) {
get_frame:
        if (gLoopExit) {
            SAMPLE_LOG_INFO("ScaleThreadMain EXIT\n");
            break;
        }

        ret = av_buffersink_get_frame_flags(filter, frame,
                                            AV_BUFFERSINK_FLAG_NO_REQUEST);
        if (ret == AVERROR_EOF) {
            return NULL;
        } else if (ret == AVERROR(EAGAIN)) {
            goto get_frame;
        } else if (ret < 0) {
            SAMPLE_LOG_ERR("Error in retrieving a frame from the filtergraph\n");
            goto get_frame;
        }

        if (gWriteFrames) {
            if ((frame->format == AV_PIX_FMT_NV12) || (frame->format == AV_PIX_FMT_NV21)) {
                p_lu = frame->data[0];
                for (i = 0; i < frame->height; i++) {
                    fwrite(p_lu, 1, frame->width, fp_out);
                    p_lu += frame->linesize[0];
                }

                p_ch = frame->data[1];
                for (i = 0; i < frame->height / 2; i++) {
                    fwrite(p_ch, 1, frame->width, fp_out);
                    p_ch += frame->linesize[1];
                }
            } else if (frame->format == AV_PIX_FMT_RGB24) {
                p_lu = frame->data[0];
                for (i = 0; i < frame->height; i++) {
                    fwrite(p_lu, 1, frame->width, fp_out);
                    p_lu += frame->width;
                }
                for (i = 0; i < frame->height; i++) {
                    fwrite(p_lu, 1, frame->width, fp_out);
                    p_lu += frame->width;
                }
                for (i = 0; i < frame->height; i++) {
                    fwrite(p_lu, 1, frame->width, fp_out);
                    p_lu += frame->width;
                }
            }
            fflush(fp_out);
        }
    }

    avfilter_graph_free(&filter->graph);
    av_frame_free(&frame);
    return NULL;
}

static void PrintHelp()
{
    printf("usage: sample_vdec_ivps streamFile <args>\n");
    printf("args:\n");
    printf("  -c:       decoder name.               h264_axdec or hevc_axdec\n");
    printf("  -i:       input stream file.          need decoding stream file\n");
    printf("  -y:       write YUV frame to file.    (0: not write, others: write), dult: 0\n");
    printf("  -f:       filter for resize yuv and format switch.       ax_scale=width:height\n");
    printf("  -r:       decoder resize, just scaler down function.       widthxheight\n");
    printf("  -d:       device index from 0 to connected device num - 1, default, 0, range(0, AXCL_MAX_DEVICE_COUNT - 1)\n");
    printf("  -v:       set logging level                 \n");
}

int main(int argc, char *argv[])
{
    AX_S32 c;
    AX_S32 isExit = 0;
    AX_S32 s32Ret = -1;
    AX_S32 s32VideoIndex = 0;
    AVFormatContext *pstAvFmtCtx;
    AVPacket *pstAvPkt;
    int ret;
    int i = 0;
    int log_level = 0x1000;
    const AVCodec *codec = NULL;
    AVCodecParameters *origin_par = NULL;
    AVCodecContext *avctx = NULL;
    AVFrame *frame = NULL;

    AX_CHAR ps8StreamFile[256];
    AX_CHAR codec_names[128];
    AX_CHAR resize[16] = "1280x720";
    AX_CHAR filter_com[128];
    enum AVCodecID eCodecID = AV_CODEC_ID_H264;
    pthread_t scale;
    AVDictionary *dict = NULL;
    int device_index = 0;

    FilterGraphPriv *fgp;
    FilterGraph 	 *fg;
    InputFilterPriv *ifp;
    OutputFilterPriv *ofp;

    size_t slen = 0;
    int len;
    AX_BOOL resize_class = 0;
    AVDictionary *codec_opts = malloc(sizeof(AVDictionary *));

    signal(SIGINT, SigInt);
    signal(SIGQUIT, SigInt);
    signal(SIGTERM, SigInt);

    while ((c = getopt(argc, argv, "c:i:y:f:r:d:v:h")) != -1) {
        isExit = 0;
        switch (c) {
        case 'c':
           slen = strlen(optarg);

            len = snprintf(codec_names, slen + 1, "%s", optarg);
            if ((len < 0) || (len != slen)) {
                SAMPLE_LOG_ERR("snprintf FAILED, len:%d optarg:%s slen:%ld\n",
                                len, optarg, slen);
            }
            break;
        case 'i':
            slen = strlen(optarg);

            len = snprintf(ps8StreamFile, slen + 1, "%s", optarg);
            if ((len < 0) || (len != slen)) {
                SAMPLE_LOG_ERR("snprintf FAILED, len:%d optarg:%s slen:%ld\n",
                                len, optarg, slen);
            }
            break;
        case 'f':
            slen = strlen(optarg);

            len = snprintf(filter_com, slen + 1, "%s", optarg);
            if ((len < 0) || (len != slen)) {
                SAMPLE_LOG_ERR("snprintf FAILED, len:%d optarg:%s slen:%ld\n",
                       len, optarg, slen);
            }
            break;
        case 'y':
            gWriteFrames = atoi(optarg);
            break;
        case 'r':
            slen = strlen(optarg);

            len = snprintf(resize, slen + 1, "%s", optarg);
            if ((len < 0) || (len != slen)) {
                SAMPLE_LOG_ERR("snprintf FAILED, len:%d optarg:%s slen:%ld\n",
                                len, optarg, slen);
            }
            resize_class = 1;
            break;
        case 'd':
            device_index = atoi(optarg);
            break;
        case 'v':
            log_level = atoi(optarg);
            break;
        case 'h':
            isExit = 1;
            break;
        default:
            isExit = 1;
            break;
        }
    }

    if (isExit || (argc < 2)) {
        PrintHelp();
        exit(0);
    }

    if (device_index < 0 || device_index >= 256) {
        SAMPLE_LOG_ERR("device index %d is out of range[0, 255]\n", device_index);
        return -1;
    } else {
        char value[8];
        sprintf(value, "%d", device_index);
        av_dict_set(&dict, "device_index", value, 0);
    }

    if (!strcmp(codec_names, "h264_axdec")) {
        eCodecID = AV_CODEC_ID_H264;
    } else if (!strcmp(codec_names, "hevc_axdec")) {
        eCodecID = AV_CODEC_ID_HEVC;
    } else {
        SAMPLE_LOG_ERR("Not set codec name\n");
        PrintHelp();
        exit(0);
    }

    pstAvFmtCtx = avformat_alloc_context();
    if (pstAvFmtCtx == NULL) {
        SAMPLE_LOG_ERR("avformat_alloc_context() failed!");
        s32Ret = AVERROR_UNKNOWN;
        goto ERR_RET;
    }

    ret = avformat_open_input(&pstAvFmtCtx, ps8StreamFile, NULL, NULL);
    if (ret < 0) {
        AX_CHAR szError[64] = {0};
        av_strerror(ret, szError, 64);
        SAMPLE_LOG_ERR("open %s fail, error: %d, %s", ps8StreamFile, ret, szError);
        goto ERR_RET1;
    }

    ret = avformat_find_stream_info(pstAvFmtCtx, NULL);
    if (ret < 0) {
        SAMPLE_LOG_ERR("avformat_find_stream_info fail, error = %d", ret);
        goto ERR_RET1;
    }

    for (i = 0; i < pstAvFmtCtx->nb_streams; i++) {
        if (AVMEDIA_TYPE_VIDEO == pstAvFmtCtx->streams[i]->codecpar->codec_type) {
            s32VideoIndex = i;
            break;
        }
    }

    pstAvPkt = av_packet_alloc();
    if (!pstAvPkt) {
        SAMPLE_LOG_ERR("av_packet_alloc failed \n");
        goto ERR_RET1;
    }

    origin_par = pstAvFmtCtx->streams[s32VideoIndex]->codecpar;

    codec = avcodec_find_decoder_by_name(codec_names);
    if (!codec) {
        SAMPLE_LOG_ERR("avcodec_find_decoder error\n");
        s32Ret = AVERROR_UNKNOWN;
        goto ERR_RET2;
    }
    avctx = avcodec_alloc_context3(codec);
    if (!avctx) {
        SAMPLE_LOG_ERR("Can't allocate decoder context\n");
        s32Ret = AVERROR_UNKNOWN;
        goto ERR_RET2;
    }

    ret = avcodec_parameters_to_context(avctx, origin_par);
    if (ret) {
        SAMPLE_LOG_ERR("avcodec_parameters_to_context error\n");
        s32Ret = AVERROR_UNKNOWN;
        goto ERR_RET3;
    }

    avctx->thread_count = 2;
    avctx->thread_type = FF_THREAD_FRAME;
    avctx->debug = log_level;
    avctx->max_pixels = avctx->width * avctx->height * 3 / 2;
    avctx->get_format = get_format;

    if (resize_class) {
        av_dict_set(&codec_opts, "resize", resize, FLAGS);
    }

    ret = avcodec_open2(avctx, codec, &codec_opts);
    if (ret < 0) {
        SAMPLE_LOG_ERR("Can't open decoder\n");
        s32Ret = AVERROR_UNKNOWN;
        goto ERR_RET3;
    }

    ret = av_hwdevice_ctx_create(&hw_device_ctx, AV_HWDEVICE_TYPE_AXMM, NULL, dict, 0);
    if (ret < 0) {
        SAMPLE_LOG_ERR("Failed to create a hw device\n");
        goto ERR_RET3;
    }

    frame = av_frame_alloc();
    if (!frame) {
        SAMPLE_LOG_ERR("Can't allocate frame\n");
        s32Ret = AVERROR_UNKNOWN;
        goto ERR_RET4;
    }

    fgp = av_mallocz(sizeof(*fgp));
    if (!fgp) {
        SAMPLE_LOG_ERR("Malloc fgp failed!");
        s32Ret = AVERROR_UNKNOWN;
        goto ERR_RET5;
    }

    fg = &fgp->fg;
    fg->class = &fg_class;
    fgp->graph_desc = (const char *)filter_com;
    fg->index = -1;

    snprintf(fgp->log_name, sizeof(fgp->log_name), "fc#%d", fg->index);

    /* set hw_frames_ctx for decoder's AVCodecContext */
    if ((ret = set_hwframe_ctx(avctx, hw_device_ctx)) < 0) {
        SAMPLE_LOG_ERR("Failed to set hwframe context.\n");
        goto ERR_RET6;
    }

    fgp->frame = av_frame_alloc();
    if (!fgp->frame) {
        goto ERR_RET6;
    }

    ret = GraphCreate(fgp, avctx);
    if (ret) {
        SAMPLE_LOG_ERR("Create scaler thread failed\n");
        goto ERR_RET7;
    }

    ofp = ofp_from_ofilter(fg->outputs[0]);

    ret = pthread_create(&scale, NULL, ScaleThreadMain,
                         (void *)ofp);

    ifp = ifp_from_ifilter(fg->inputs[0]);

    while (1) {
        ret = av_read_frame(pstAvFmtCtx, pstAvPkt);
        if (ret < 0) {
            if (AVERROR_EOF == ret) {
                pstAvPkt->size = 0;
                pstAvPkt->data = NULL;
            } else {
                SAMPLE_LOG_ERR("av_read_frame fail, error: %d", ret);
                s32Ret = AVERROR_UNKNOWN;
                break;
            }
        }

        avctx->codec_type = AVMEDIA_TYPE_VIDEO;
        avctx->codec_id = eCodecID;
        avctx->width = avctx->width;
        avctx->height = avctx->height;
        avctx->coded_width = avctx->width;
        avctx->coded_height = avctx->height;
        frame->width = avctx->width;
        frame->height = avctx->height;
        frame->format = AV_PIX_FMT_NONE;

        ret = avcodec_send_packet(avctx, pstAvPkt);
        if (ret == AVERROR(EAGAIN)) {
            SAMPLE_LOG_ERR("avcodec_send_packet error\n");
            break;
        } else if (ret == AVERROR_EOF) {
            break;
        }

        av_packet_unref(pstAvPkt);

        while (ret >= 0) {
            ret = avcodec_receive_frame(avctx, frame);
            if (ret == AVERROR_EOF) {
                av_frame_unref(frame);
                gLoopExit = 1;
                break;
            }
            else if (ret == AVERROR(EAGAIN)) {
                av_frame_unref(frame);
                ret = 0;
                break;
            }

            /* frame add filter buffer */
            ret = av_buffersrc_add_frame_flags(ifp->filter, frame,
                                               AV_BUFFERSRC_FLAG_PUSH);
            if (ret < 0) {
                if (ret != AVERROR_EOF)
                    SAMPLE_LOG_ERR("Error while filtering frame: %s\n", av_err2str(ret));
            }

            av_frame_unref(frame);
            frame_num ++;
        }
    }

    SAMPLE_LOG_INFO("Get frame_num:%d\n", frame_num);
    pthread_join(scale, NULL);

    GraphFree(&fg);

    av_frame_free(&fgp->frame);
    av_freep(&fgp);
    av_buffer_unref(&hw_device_ctx);
    av_frame_free(&frame);
    av_packet_free(&pstAvPkt);
    avcodec_free_context(&avctx);
    avformat_free_context(pstAvFmtCtx);

    return 0;

ERR_RET7:
    av_frame_free(&fgp->frame);
ERR_RET6:
    av_freep(&fgp);
ERR_RET5:
    av_frame_free(&frame);
ERR_RET4:
    av_buffer_unref(&hw_device_ctx);
ERR_RET3:
    avcodec_free_context(&avctx);
ERR_RET2:
    av_packet_free(&pstAvPkt);
ERR_RET1:
    avformat_free_context(pstAvFmtCtx);
ERR_RET:
    return s32Ret;
}

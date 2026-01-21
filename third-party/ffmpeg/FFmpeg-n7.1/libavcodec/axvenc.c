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

#include "libavutil/hwcontext.h"
#include "libavutil/internal.h"
#include "libavutil/hwcontext_axmm.h"

#include "internal.h"
#include "encode.h"

#include "axvenc.h"

#define OFFSET(x) offsetof(AXVencContext, x)

#define CHECK_WITH_RET(cond, ret) \
    do { \
        if ((cond) != 0) { \
            av_log(NULL, AV_LOG_ERROR, "check %s failed\n", #cond); \
            return (ret); \
        } \
    } while (0)

#define LIB_NAME  "libax_venc.so"
#define LIBE_NAME  "libexif.so"


static int axenc_device_dl(AXVencDlFuncs *dl_funcs)
{
    dl_funcs->lib_if = dlopen(LIBE_NAME, RTLD_NOW | RTLD_GLOBAL);
    CHECK_WITH_RET(!dl_funcs->lib_if, AVERROR_EXTERNAL);

    dl_funcs->lib = dlopen(LIB_NAME, RTLD_NOW | RTLD_GLOBAL);

    CHECK_WITH_RET(!dl_funcs->lib, AVERROR_EXTERNAL);

    dl_funcs->venc_init = dlsym(dl_funcs->lib, "AX_VENC_Init");
    CHECK_WITH_RET(!dl_funcs->venc_init, AVERROR_EXTERNAL);

    dl_funcs->venc_deinit = dlsym(dl_funcs->lib, "AX_VENC_Deinit");
    CHECK_WITH_RET(!dl_funcs->venc_deinit, AVERROR_EXTERNAL);

    dl_funcs->venc_create_chn_ex = dlsym(dl_funcs->lib, "AX_VENC_CreateChnEx");
    CHECK_WITH_RET(!dl_funcs->venc_create_chn_ex, AVERROR_EXTERNAL);

    dl_funcs->venc_destroy_chn = dlsym(dl_funcs->lib, "AX_VENC_DestroyChn");
    CHECK_WITH_RET(!dl_funcs->venc_destroy_chn, AVERROR_EXTERNAL);

    dl_funcs->venc_reset_chn = dlsym(dl_funcs->lib, "AX_VENC_ResetChn");
    CHECK_WITH_RET(!dl_funcs->venc_reset_chn, AVERROR_EXTERNAL);

    dl_funcs->venc_set_chn_attr = dlsym(dl_funcs->lib, "AX_VENC_SetChnAttr");
    CHECK_WITH_RET(!dl_funcs->venc_set_chn_attr, AVERROR_EXTERNAL);

    dl_funcs->venc_get_chn_attr = dlsym(dl_funcs->lib, "AX_VENC_GetChnAttr");
    CHECK_WITH_RET(!dl_funcs->venc_get_chn_attr, AVERROR_EXTERNAL);

    dl_funcs->venc_start_recv_frame = dlsym(dl_funcs->lib, "AX_VENC_StartRecvFrame");
    CHECK_WITH_RET(!dl_funcs->venc_start_recv_frame, AVERROR_EXTERNAL);

    dl_funcs->venc_stop_recv_frame = dlsym(dl_funcs->lib, "AX_VENC_StopRecvFrame");
    CHECK_WITH_RET(!dl_funcs->venc_stop_recv_frame, AVERROR_EXTERNAL);

    dl_funcs->venc_send_frame = dlsym(dl_funcs->lib, "AX_VENC_SendFrame");
    CHECK_WITH_RET(!dl_funcs->venc_send_frame, AVERROR_EXTERNAL);

    dl_funcs->venc_get_stream = dlsym(dl_funcs->lib, "AX_VENC_GetStream");
    CHECK_WITH_RET(!dl_funcs->venc_get_stream, AVERROR_EXTERNAL);

    dl_funcs->venc_release_stream = dlsym(dl_funcs->lib, "AX_VENC_ReleaseStream");
    CHECK_WITH_RET(!dl_funcs->venc_release_stream, AVERROR_EXTERNAL);

    return 0;
}

static int venc_attr_init(AVCodecContext *ctx)
{
    AXVencContext *vctx = ctx->priv_data;
    AX_VENC_MOD_ATTR_T *mod_attr = &vctx->mod_attr;
    AX_VENC_CHN_ATTR_T *chn_attr = &vctx->chn_attr;

    mod_attr->enVencType = AX_VENC_MULTI_ENCODER;
    mod_attr->stModThdAttr.u32TotalThreadNum = 1;
    mod_attr->stModThdAttr.bExplicitSched = AX_FALSE;

    chn_attr->stVencAttr.u32PicWidthSrc = ctx->width;
    chn_attr->stVencAttr.u32PicHeightSrc = ctx->height;
    chn_attr->stVencAttr.u32MaxPicWidth = ctx->width;
    chn_attr->stVencAttr.u32MaxPicHeight = ctx->height;
    chn_attr->stVencAttr.enLinkMode = AX_VENC_UNLINK_MODE;
    chn_attr->stVencAttr.u8InFifoDepth = 3;
    chn_attr->stVencAttr.u8OutFifoDepth = 3;
    chn_attr->stVencAttr.u32BufSize = 0;

    chn_attr->stRcAttr.stFrameRate.fSrcFrameRate = (AX_F32)(ctx->framerate.num / ctx->framerate.den);
    chn_attr->stRcAttr.stFrameRate.fDstFrameRate = chn_attr->stRcAttr.stFrameRate.fSrcFrameRate;

    chn_attr->stGopAttr.enGopMode = AX_VENC_GOPMODE_NORMALP;

    vctx->recv_param.s32RecvPicNum = -1;

    if (vctx->attr_init)
        return vctx->attr_init(ctx);

    return 0;
}

static int hwframe_ctx_init(AVCodecContext *ctx)
{
    int ret;
    AXVencContext *vctx = ctx->priv_data;
    AVBufferRef *hw_frames_ref;
    AVHWFramesContext *frames_ctx;

    if ((ctx->pix_fmt != AV_PIX_FMT_AXMM) && (ctx->hw_device_ctx && !ctx->hw_frames_ctx)) {
        hw_frames_ref = av_hwframe_ctx_alloc(ctx->hw_device_ctx);
        if (!hw_frames_ref) {
            av_log(ctx, AV_LOG_ERROR, "%s failed to alloc hwframe context\n", __func__);
            return AVERROR(ENOMEM);
        }

        frames_ctx = (AVHWFramesContext *)(hw_frames_ref->data);
        frames_ctx->format = AV_PIX_FMT_AXMM;
        frames_ctx->sw_format = ctx->pix_fmt;
        frames_ctx->width = ctx->width;
        frames_ctx->height = ctx->height;
        frames_ctx->initial_pool_size = vctx->chn_attr.stVencAttr.u8InFifoDepth + 2;

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

        vctx->hw_frame = av_frame_alloc();
        if (!vctx->hw_frame)
            return AVERROR(ENOMEM);

        av_log(ctx, AV_LOG_VERBOSE, "%s hwframe context init success\n", __func__);
    }

    return 0;
}

int ff_axvenc_init(AVCodecContext *ctx)
{
    int ret;
    AXVencContext *vctx = ctx->priv_data;

    ret = axenc_device_dl(&vctx->dl_funcs);
    if (ret)
        goto err0;

    vctx->buffer_frame = av_frame_alloc();
    if (!vctx->buffer_frame) {
        ret = AVERROR(ENOMEM);
        goto err0;
    }

    ret = venc_attr_init(ctx);
    if (ret)
        goto err0;

    ret = vctx->dl_funcs.venc_init(&vctx->mod_attr);
    if (ret) {
        av_log(ctx, AV_LOG_ERROR, "%s failed to init venc, ret:0x%x\n", __func__, ret);
        ret = AVERROR_EXTERNAL;
        goto err0;
    }

    ret = vctx->dl_funcs.venc_create_chn_ex(&vctx->chn, &vctx->chn_attr);
    if (ret) {
        av_log(ctx, AV_LOG_ERROR, "%s failed to create venc chn, ret:0x%x\n", __func__, ret);
        ret = AVERROR_EXTERNAL;
        goto err1;
    }

    ret = vctx->dl_funcs.venc_start_recv_frame(vctx->chn, &vctx->recv_param);
    if (ret) {
        av_log(ctx, AV_LOG_ERROR, "%s failed to start venc chn%d, ret:0x%x\n", __func__, vctx->chn, ret);
        ret = AVERROR_EXTERNAL;
        goto err2;
    }

    ret = hwframe_ctx_init(ctx);
    if (ret)
        goto err3;

    av_log(ctx, AV_LOG_INFO, "%s create and start venc chn%d success\n", __func__, vctx->chn);

    return 0;

err3:
    vctx->dl_funcs.venc_stop_recv_frame(vctx->chn);
err2:
    vctx->dl_funcs.venc_destroy_chn(vctx->chn);
err1:
    vctx->dl_funcs.venc_deinit();
err0:
    if (vctx->dl_funcs.lib)
        dlclose(vctx->dl_funcs.lib);
    if (vctx->dl_funcs.lib_if)
        dlclose(vctx->dl_funcs.lib_if);
    if (vctx->buffer_frame)
        av_frame_free(&vctx->buffer_frame);

    av_log(ctx, AV_LOG_ERROR, "%s failed, ret = %x\n", __func__, ret);

    return ret;
}

int ff_axvenc_receive_packet(AVCodecContext *ctx, AVPacket *pkt)
{
    int ret = 0, timeout = 0;
    AXVencContext *vctx = ctx->priv_data;
    AVFrame *frame = vctx->buffer_frame;
    AVFrame *hw_frame = vctx->hw_frame;
    AX_VIDEO_FRAME_INFO_T *frame_internal = &vctx->frame_internal;
    AX_VENC_STREAM_T *stream = &vctx->stream;

    if (!frame->buf[0]) {
        ret = ff_encode_get_frame(ctx, frame);
        if ((ret == AVERROR_EOF) || (ret == AVERROR(EAGAIN))) {
            if (!hw_frame || !hw_frame->buf[0]) {
                frame = NULL;
                timeout = (ret == AVERROR_EOF) ? 200 : 0;
            } else {
                frame = hw_frame;
            }
        }
    }

    if (frame) {
        if (frame->format != AV_PIX_FMT_AXMM) {
            if (!ctx->hw_frames_ctx || !hw_frame) {
                av_log(ctx, AV_LOG_ERROR, "%s frame format(%x) invalid\n", __func__, frame->format);
                return AVERROR_BUG;
            }

            if (!hw_frame->buf[0]) {
                ret = av_hwframe_get_buffer(ctx->hw_frames_ctx, hw_frame, 0);
                if (!ret) {
                    ret = av_hwframe_transfer_data(hw_frame, frame, 0);
                    if (ret)
                        return ret;

                    av_frame_unref(frame);
                    frame = hw_frame;
                } else {
                    ret = AVERROR(EAGAIN);
                }
            } else {
                frame = hw_frame;
            }
        }

        if (frame->data[4]) {
            memcpy(&frame_internal->stVFrame, frame->data[4], sizeof(frame_internal->stVFrame));
        } else if (frame->data[3]) {
            memcpy(&frame_internal->stVFrame, frame->data[3], sizeof(frame_internal->stVFrame));
        } else {
            av_log(ctx, AV_LOG_ERROR, "%s frame data invalid\n", __func__);
            return AVERROR_BUG;
        }

#if 0
        switch (frame->format)
        {
        case AV_PIX_FMT_YUV420P:
            frame_internal->stVFrame.enImgFormat = AX_FORMAT_YUV420_PLANAR;
            break;
        case AV_PIX_FMT_NV12:
            frame_internal->stVFrame.enImgFormat = AX_FORMAT_YUV420_SEMIPLANAR;
            break;
        case AV_PIX_FMT_NV21:
            frame_internal->stVFrame.enImgFormat = AX_FORMAT_YUV420_SEMIPLANAR_VU;
            break;
        case AV_PIX_FMT_YUYV422:
            frame_internal->stVFrame.enImgFormat = AX_FORMAT_YUV422_INTERLEAVED_YUYV;
            break;
        case AV_PIX_FMT_YVYU422:
            frame_internal->stVFrame.enImgFormat = AX_FORMAT_YUV422_INTERLEAVED_UYVY;
            break;
        default:
            av_log(ctx, AV_LOG_ERROR, "%s unknown image format(%d).\n", __func__, frame->format);
            return AVERROR_EXTERNAL;
        }
#endif
        ret = vctx->dl_funcs.venc_send_frame(vctx->chn, frame_internal, 0);
        if (ret == AX_SUCCESS) {
            av_log(ctx, AV_LOG_VERBOSE, "%s send frame to venc chn%d success\n", __func__, vctx->chn);
            av_frame_unref(frame);
        } else if (ret == AX_ERR_VENC_QUEUE_FULL) {
            av_log(ctx, AV_LOG_VERBOSE, "%s venc chn%d input queue full, try again!!\n", __func__, vctx->chn);
            timeout = -1;
        } else {
            av_log(ctx, AV_LOG_ERROR, "%s send frame to venc chn%d failed, ret:0x%x\n", __func__, vctx->chn, ret);
            return AVERROR_EXTERNAL;
        }
    }

    memset(stream, 0, sizeof(*stream));
    ret = vctx->dl_funcs.venc_get_stream(vctx->chn, stream, timeout);
    if (ret == AX_SUCCESS) {
        av_log(ctx, AV_LOG_VERBOSE, "%s get stream{len:%u, type:%d, seq:%lld, pts:%lld} from venc chn%d success\n", __func__,
               stream->stPack.u32Len, stream->stPack.enCodingType,
               stream->stPack.u64SeqNum, stream->stPack.u64PTS,
               vctx->chn);
        if (stream->stPack.u32Len) {
            ret = av_new_packet(pkt, stream->stPack.u32Len);
            if (!ret) {
                memcpy(pkt->data, (void *)stream->stPack.pu8Addr,
                                               stream->stPack.u32Len);
                av_log(ctx, AV_LOG_DEBUG, "%s failed to copy data:%lld to 0x%p on venc chn%d\n", __func__,
                       stream->stPack.pu8Addr, pkt->data, vctx->chn);
            }
        }
        if (vctx->dl_funcs.venc_release_stream(vctx->chn, stream))
            av_log(ctx, AV_LOG_ERROR, "%s failed to release frame to venc chn%d\n", __func__, vctx->chn);
    } else if (ret == AX_ERR_VENC_QUEUE_EMPTY) {
        av_log(ctx, AV_LOG_VERBOSE, "%s venc chn%d output queue empty, try again!!\n", __func__, vctx->chn);
        ret = (timeout > 0) ? AVERROR_EOF : AVERROR(EAGAIN);
    } else {
        av_log(ctx, AV_LOG_ERROR, "%s failed to get stream from venc chn%d, ret:0x%x\n", __func__, vctx->chn, ret);
        return AVERROR_EXTERNAL;
    }

    return ret;
}

int ff_axvenc_close(AVCodecContext *ctx)
{
    int ret;
    AXVencContext *vctx = ctx->priv_data;

    ret = vctx->dl_funcs.venc_stop_recv_frame(vctx->chn);
    if (ret) {
        av_log(ctx, AV_LOG_ERROR, "%s failed to stop venc chn%d, ret = 0x%x\n", __func__, vctx->chn, ret);
        goto exit;
    }

    ret = vctx->dl_funcs.venc_reset_chn(vctx->chn);
    if (ret) {
        av_log(ctx, AV_LOG_ERROR, "%s failed to reset venc chn%d, ret = 0x%x\n", __func__, vctx->chn, ret);
        goto exit;
    }

    ret = vctx->dl_funcs.venc_destroy_chn(vctx->chn);
    if (ret) {
        av_log(ctx, AV_LOG_ERROR, "%s failed to destroy venc chn%d, ret = 0x%x\n", __func__, vctx->chn, ret);
        goto exit;
    }

    ret = vctx->dl_funcs.venc_deinit();
    if (ret) {
        av_log(ctx, AV_LOG_ERROR, "%s failed to deinit venc, ret = 0x%x\n", __func__, ret);
        goto exit;
    }

    av_frame_free(&vctx->buffer_frame);
    av_frame_free(&vctx->hw_frame);

    dlclose(vctx->dl_funcs.lib);
    dlclose(vctx->dl_funcs.lib_if);

exit:
    ret = ret ? AVERROR_EXTERNAL : 0;

    av_log(ctx, AV_LOG_VERBOSE, "%s %s\n", __func__, ret ? "failed" : "success");

    return ret;
}

void ff_axvenc_flush(AVCodecContext *ctx)
{
    int ret;
    AXVencContext *vctx = ctx->priv_data;

    ret = vctx->dl_funcs.venc_reset_chn(vctx->chn);
    if (ret)
        av_log(ctx, AV_LOG_ERROR, "%s failed to reset venc chn%d, ret = 0x%x\n", __func__, vctx->chn, ret);

    av_frame_unref(vctx->buffer_frame);
    av_frame_unref(vctx->hw_frame);

    av_log(ctx, AV_LOG_VERBOSE, "%s done\n", __func__);
}


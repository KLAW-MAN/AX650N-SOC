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

#include "config.h"

#include "buffer.h"
#include "common.h"
#include "hwcontext.h"
#include "hwcontext_internal.h"
#include "hwcontext_axmm.h"
#include "libavcodec/avcodec.h"
#include "mem.h"
#include "pixfmt.h"
#include "pixdesc.h"
#include "imgutils.h"


#define CHECK_WITH_RET(cond, ret) \
    do { \
        if ((cond) != 0) { \
            av_log(NULL, AV_LOG_ERROR, "check %s failed\n", #cond); \
            return (ret); \
        } \
    } while (0)

#define LIB1_NAME    "libax_sys.so"

#define AXMM_FRAME_ALIGNMENT 16
#define FR_MAGIC             0x5A5AA5A5

static const enum AVPixelFormat supported_formats[] = {
    AV_PIX_FMT_NV12,
    AV_PIX_FMT_NV21,
    AV_PIX_FMT_YUV420P,
    AV_PIX_FMT_P010,
    AV_PIX_FMT_P016,
    AV_PIX_FMT_Y400A,
    AV_PIX_FMT_RGB24,
};

static inline AX_IMG_FORMAT_E axmm_format_convert(enum AVPixelFormat fmt)
{
    switch (fmt) {
    case AV_PIX_FMT_NV12:
        return AX_FORMAT_YUV420_SEMIPLANAR;
    case AV_PIX_FMT_NV21:
        return AX_FORMAT_YUV420_SEMIPLANAR_VU;
    case AV_PIX_FMT_YUV420P:
        return AX_FORMAT_YUV420_PLANAR;
    case AV_PIX_FMT_P010:
        return AX_FORMAT_YUV420_SEMIPLANAR_10BIT_P010;
    case AV_PIX_FMT_P016:
        return AX_FORMAT_YUV420_SEMIPLANAR_10BIT_P016;
    case AV_PIX_FMT_Y400A:
        return AX_FORMAT_YUV400;
    case AV_PIX_FMT_RGB24:
        return AX_FORMAT_RGB888;
    }

    av_log(NULL, AV_LOG_ERROR, "%s pixel format '%s' is not supported\n", __func__,
           av_get_pix_fmt_name(fmt));

    return AX_FORMAT_INVALID;
}

static void axmm_device_free(AVHWDeviceContext *ctx)
{
}

static int axmm_device_dl(AXMMDeviceContext *mmctx)
{
    AXMMDeviceFuncs *mmdev_funcs = &mmctx->mmdev_funcs;

    mmctx->mm_sys_lib = dlopen(LIB1_NAME, RTLD_NOW | RTLD_GLOBAL);
    CHECK_WITH_RET(!mmctx->mm_sys_lib, AVERROR_EXTERNAL);

    mmdev_funcs->sys_init = dlsym(mmctx->mm_sys_lib, "AX_SYS_Init");
    CHECK_WITH_RET(!mmdev_funcs->sys_init, AVERROR_EXTERNAL);

    mmdev_funcs->sys_deinit = dlsym(mmctx->mm_sys_lib, "AX_SYS_Deinit");
    CHECK_WITH_RET(!mmdev_funcs->sys_deinit, AVERROR_EXTERNAL);

    mmdev_funcs->sys_mmap = dlsym(mmctx->mm_sys_lib, "AX_SYS_Mmap");
    CHECK_WITH_RET(!mmdev_funcs->sys_mmap, AVERROR_EXTERNAL);

    mmdev_funcs->sys_munmap = dlsym(mmctx->mm_sys_lib, "AX_SYS_Munmap");
    CHECK_WITH_RET(!mmdev_funcs->sys_munmap, AVERROR_EXTERNAL);

    mmdev_funcs->pool_init = dlsym(mmctx->mm_sys_lib, "AX_POOL_Init");
    CHECK_WITH_RET(!mmdev_funcs->pool_init, AVERROR_EXTERNAL);

    mmdev_funcs->pool_exit = dlsym(mmctx->mm_sys_lib, "AX_POOL_Exit");
    CHECK_WITH_RET(!mmdev_funcs->pool_exit, AVERROR_EXTERNAL);

    mmdev_funcs->pool_create = dlsym(mmctx->mm_sys_lib, "AX_POOL_CreatePool");
    CHECK_WITH_RET(!mmdev_funcs->pool_create, AVERROR_EXTERNAL);

    mmdev_funcs->pool_destroy = dlsym(mmctx->mm_sys_lib, "AX_POOL_DestroyPool");
    CHECK_WITH_RET(!mmdev_funcs->pool_destroy, AVERROR_EXTERNAL);

    mmdev_funcs->pool_get_block = dlsym(mmctx->mm_sys_lib, "AX_POOL_GetBlock");
    CHECK_WITH_RET(!mmdev_funcs->pool_get_block, AVERROR_EXTERNAL);

    mmdev_funcs->pool_release_block = dlsym(mmctx->mm_sys_lib, "AX_POOL_ReleaseBlock");
    CHECK_WITH_RET(!mmdev_funcs->pool_release_block, AVERROR_EXTERNAL);

    mmdev_funcs->pool_paddr2handle = dlsym(mmctx->mm_sys_lib, "AX_POOL_PhysAddr2Handle");
    CHECK_WITH_RET(!mmdev_funcs->pool_paddr2handle, AVERROR_EXTERNAL);

    mmdev_funcs->pool_handle2paddr = dlsym(mmctx->mm_sys_lib, "AX_POOL_Handle2PhysAddr");
    CHECK_WITH_RET(!mmdev_funcs->pool_handle2paddr, AVERROR_EXTERNAL);

    mmdev_funcs->pool_handle2vaddr = dlsym(mmctx->mm_sys_lib, "AX_POOL_GetBlockVirAddr");
    CHECK_WITH_RET(!mmdev_funcs->pool_handle2paddr, AVERROR_EXTERNAL);

    mmdev_funcs->pool_ref_get = dlsym(mmctx->mm_sys_lib, "AX_POOL_IncreaseRefCnt");
    CHECK_WITH_RET(!mmdev_funcs->pool_ref_get, AVERROR_EXTERNAL);

    mmdev_funcs->pool_ref_put = dlsym(mmctx->mm_sys_lib, "AX_POOL_DecreaseRefCnt");
    CHECK_WITH_RET(!mmdev_funcs->pool_ref_put, AVERROR_EXTERNAL);

    return 0;
}

static int axmm_device_create(AVHWDeviceContext *ctx, const char *device,
                              AVDictionary *opts, int flags)
{
    int ret;
    AXMMDeviceContext *mmctx = ctx->hwctx;

    ctx->user_opaque = mmctx;
    ctx->free = axmm_device_free;

    mmctx->alloc_blk = av_dict_get(opts, "alloc_blk", NULL, 0) ? 1 : 0;
    av_log(ctx, AV_LOG_INFO, "%s alloc_blk:%d\n", __func__, mmctx->alloc_blk);

    ret = axmm_device_dl(mmctx);
    if (ret)
        goto err0;

    if (ret = mmctx->mmdev_funcs.sys_init()) {
        av_log(ctx, AV_LOG_ERROR, "%s sys_init failed ret = 0x%x\n", __func__, ret);
        ret = AVERROR_EXTERNAL;
        goto err0;
    }

    av_log(ctx, AV_LOG_VERBOSE, "%s create device success\n", __func__);

    return 0;

err0:
    if (mmctx->mm_sys_lib) {
        dlclose(mmctx->mm_sys_lib);
        mmctx->mm_sys_lib = NULL;
    }

    return ret;
}

static int axmm_device_init(AVHWDeviceContext *ctx)
{
    av_log(ctx, AV_LOG_VERBOSE, "%s enter\n", __func__);

    return 0;
}

static void axmm_device_uninit(AVHWDeviceContext *ctx)
{
    av_log(ctx, AV_LOG_VERBOSE, "%s deactive device and cleanup mmctx\n", __func__);
}

static int axmm_frames_get_constraints(AVHWDeviceContext *ctx, const void *hwconfig,
                                       AVHWFramesConstraints *constraints)
{
    av_log(ctx, AV_LOG_VERBOSE, "%s enter\n", __func__);

    return 0;
}

static void axmm_pool_buffer_free(void *opaque, uint8_t *data)
{
}

static AVBufferRef *axmm_pool_alloc(void *opaque, int size)
{
    AVHWFramesContext *hwfc = opaque;
    AXMMDeviceContext *hwdc_priv = hwfc->device_ctx->hwctx;
    AVBufferRef *ref;

    ref = av_buffer_create(NULL, 0, &axmm_pool_buffer_free, hwfc, AV_BUFFER_FLAG_READONLY);
    if (!ref)
        av_log(hwfc, AV_LOG_ERROR, "%s create buffer ref failed on device\n", __func__);

    return ref;
}

static int axmm_frames_init(AVHWFramesContext *hwfc)
{
    int i, size;
    AVHWDeviceContext *device_ctx = hwfc->device_ctx;
    AXMMDeviceContext *hwdc_priv = device_ctx->hwctx;
    AXMMFramesContext *hwfc_priv = hwfc->hwctx;
    AVAXMMFramesContext *hwfc_ax = &hwfc_priv->p;

    for (i = 0; i < FF_ARRAY_ELEMS(supported_formats); i++) {
        if (hwfc->sw_format == supported_formats[i])
            break;
    }

    if (i == FF_ARRAY_ELEMS(supported_formats)) {
        av_log(hwfc, AV_LOG_ERROR, "%s pixel format '%s' is not supported\n", __func__,
               av_get_pix_fmt_name(hwfc->sw_format));
        return AVERROR(ENOSYS);
    }

    if (!hwfc->pool) {
        if (hwfc->initial_pool_size <= 0) {
            av_log(hwfc, AV_LOG_ERROR, "AXMM requires a fixed frame pool size\n");
            return AVERROR(EINVAL);
        }

        size = av_image_get_buffer_size(hwfc->sw_format, hwfc->width, hwfc->height, AXMM_FRAME_ALIGNMENT);
        if (size < 0) {
            av_log(hwfc, AV_LOG_ERROR, "%s buffer-%dx%d@%s invalid\n", __func__,
                   hwfc->width, hwfc->height, av_get_pix_fmt_name(hwfc->sw_format));
            return size;
        }

        hwfc_priv->alloc_blk = hwdc_priv->alloc_blk;

        if (hwfc_priv->alloc_blk) {
            hwfc_priv->pool_id = AX_INVALID_POOLID;
            hwfc_priv->pool_cfg.MetaSize = 512;
            hwfc_priv->pool_cfg.BlkCnt = hwfc->initial_pool_size;
            hwfc_priv->pool_cfg.BlkSize = size;
            hwfc_priv->pool_cfg.CacheMode = POOL_CACHE_MODE_NONCACHE;
            strcpy((char *)hwfc_priv->pool_cfg.PartitionName, "anonymous");

            hwfc_priv->pool_id = hwdc_priv->mmdev_funcs.pool_create(&hwfc_priv->pool_cfg);
            if (hwfc_priv->pool_id == AX_INVALID_POOLID) {
                av_log(hwfc, AV_LOG_ERROR, "%s create pool failed\n", __func__);
                return AVERROR_EXTERNAL;
            }
        }

        ffhwframesctx(hwfc)->pool_internal = av_buffer_pool_init2(size, hwfc, &axmm_pool_alloc, NULL);
        if (!ffhwframesctx(hwfc)->pool_internal) {
            av_log(hwfc, AV_LOG_ERROR, "%s create hwframe pool failed\n", __func__);
            if (hwfc_priv->alloc_blk) {
                hwdc_priv->mmdev_funcs.pool_destroy(hwfc_priv->pool_id);
                hwfc_priv->pool_id = AX_INVALID_POOLID;
            }
            return AVERROR_EXTERNAL;
        }
    }

    hwfc_ax->blk_id = 0;

    av_log(hwfc, AV_LOG_VERBOSE, "%s create pool%d success\n", __func__,
           hwfc_priv->pool_id);

    return 0;
}

static void axmm_frames_uninit(AVHWFramesContext *hwfc)
{
    av_log(hwfc, AV_LOG_VERBOSE, "%s enter\n", __func__);
}

static void axmm_buffer_free(void *opaque, uint8_t *data)
{
    AVHWFramesContext *hwfc = opaque;
    AXMMDeviceContext *hwdc_priv = hwfc->device_ctx->hwctx;
    AX_VIDEO_FRAME_T *fr = (AX_VIDEO_FRAME_T *)data;

    av_log(hwfc, AV_LOG_VERBOSE, "%s free blk:%x\n", __func__, fr->u32BlkId[0]);

    if (fr->u32BlkId[0]) {
        if (hwdc_priv->alloc_blk) {
            hwdc_priv->mmdev_funcs.pool_release_block(fr->u32BlkId[0]);
        } else {
            hwdc_priv->mmdev_funcs.pool_ref_put(fr->u32BlkId[0]);
        }
    }

    av_free(fr);
}

static int axmm_get_buffer(AVHWFramesContext *hwfc, AVFrame *frame)
{
    int ret;
    AX_VIDEO_FRAME_T *fr;
    AVHWDeviceContext *device_ctx = hwfc->device_ctx;
    AXMMDeviceContext *hwdc_priv = device_ctx->hwctx;
    AXMMFramesContext *hwfc_priv = hwfc->hwctx;

    frame->buf[0] = av_buffer_pool_get(hwfc->pool);
    if (!frame->buf[0]) {
        ret = AVERROR(ENOMEM);
        goto err0;
    }

    ret = av_image_fill_arrays(frame->data, frame->linesize, NULL, hwfc->sw_format,
                               hwfc->width, hwfc->height, AXMM_FRAME_ALIGNMENT);
    if (ret < 0)
        goto err0;

    fr = av_mallocz(sizeof(*fr));
    if (!fr) {
        av_log(hwfc, AV_LOG_ERROR, "%s failed to malloc fr\n", __func__);
        ret = AVERROR(ENOMEM);
        goto err0;
    }

    fr->enImgFormat = axmm_format_convert(hwfc->sw_format);
    if (fr->enImgFormat < 0) {
        av_log(hwfc, AV_LOG_ERROR, "%s sw_format(%x) invalid\n", __func__, hwfc->sw_format);
        ret = AVERROR(EINVAL);
        goto err1;
    }

    fr->u32FrameSize = hwfc_priv->pool_cfg.BlkSize;

    if (hwfc_priv->alloc_blk) {
        fr->u32BlkId[0] = hwdc_priv->mmdev_funcs.pool_get_block(hwfc_priv->pool_id, fr->u32FrameSize, NULL);
        if (fr->u32BlkId[0] == AX_INVALID_BLOCKID) {
            av_log(hwfc, AV_LOG_ERROR, "%s failed to get blk from pool%d\n", __func__,
                   hwfc_priv->pool_id);
            ret = AVERROR_EXTERNAL;
            goto err1;
        }

        fr->u64PhyAddr[0] = hwdc_priv->mmdev_funcs.pool_handle2paddr(fr->u32BlkId[0]);
        if (!fr->u64PhyAddr[0]) {
            av_log(hwfc, AV_LOG_ERROR, "%s failed to translate paddr from blk(0x%x)\n", __func__,
                   fr->u32BlkId[0]);
            ret = AVERROR_EXTERNAL;
            goto err2;
        }

        fr->u64VirAddr[0] = (AX_U64)hwdc_priv->mmdev_funcs.pool_handle2vaddr(fr->u32BlkId[0]);
        if (!fr->u64VirAddr[0]) {
            av_log(hwfc, AV_LOG_ERROR, "%s failed to translate vaddr from blk(0x%x)\n", __func__,
                   fr->u32BlkId[0]);
            ret = AVERROR_EXTERNAL;
            goto err2;
        }
    } else {
        if (hwfc_priv->p.blk_id) {
            hwdc_priv->mmdev_funcs.pool_ref_get(hwfc_priv->p.blk_id);
        }
    }

    av_log(hwfc, AV_LOG_VERBOSE, "%s alloc fr success, fr-info:[%d,%x,%x,%llx,%llx], line:%d, format:%d\n", __func__,
           hwfc_priv->pool_id, fr->u32BlkId[0],
           fr->u32FrameSize, fr->u64VirAddr[0], fr->u64PhyAddr[0], frame->linesize[0], hwfc->sw_format);

    if (hwfc->sw_format != AV_PIX_FMT_Y400A) {
        frame->buf[1] = av_buffer_create((uint8_t *)fr, sizeof(fr), &axmm_buffer_free, hwfc, AV_BUFFER_FLAG_READONLY);
        if (!frame->buf[1]) {
            av_log(hwfc, AV_LOG_ERROR, "%s failed to create buffer ref\n", __func__);
            ret = AVERROR(ENOMEM);
            goto err2;
        }
    }

    frame->data[0] = (uint8_t *)FR_MAGIC;

    if (hwfc_priv->alloc_blk)
        frame->data[3] = (uint8_t *)fr;
    else
        frame->data[4] = (uint8_t *)fr;

    frame->format  = AV_PIX_FMT_AXMM;
    frame->width   = hwfc->width;
    frame->height  = hwfc->height;

    fr->u32Width = frame->width;
    fr->u32Height = frame->height;
    fr->u32PicStride[0] = frame->linesize[0];
    fr->u32PicStride[1] = frame->linesize[1];
    fr->u32PicStride[2] = frame->linesize[2];

    return 0;

err2:
    if (fr->u32BlkId[0])
        hwdc_priv->mmdev_funcs.pool_release_block(fr->u32BlkId[0]);
err1:
    av_free(fr);
err0:
    return ret;
}

static int axmm_transfer_get_formats(AVHWFramesContext *ctx,
                                     enum AVHWFrameTransferDirection dir,
                                     enum AVPixelFormat **formats)
{
    enum AVPixelFormat *fmts;

    av_log(ctx, AV_LOG_VERBOSE, "%s enter\n", __func__);

    fmts = av_malloc_array(2, sizeof(*fmts));
    if (!fmts)
        return AVERROR(ENOMEM);

    fmts[0] = ctx->sw_format;
    fmts[1] = AV_PIX_FMT_NONE;

    *formats = fmts;

    return 0;
}

static int axmm_transfer_data(AVHWFramesContext *ctx, AVFrame *dst, const AVFrame *src)
{
    int ret;
    AVHWFramesContext *hwfc_src = NULL, *hwfc_dst = NULL;
    AXMMDeviceContext *hwdc_src = NULL, *hwdc_dst = NULL;

    hwdc_dst = (AXMMDeviceContext *)ctx->device_ctx->hwctx;

    if (src->hw_frames_ctx && dst->hw_frames_ctx) { // device to device
        AX_VIDEO_FRAME_T *fr_src, *fr_dst;

        hwfc_src = (AVHWFramesContext *)src->hw_frames_ctx->data;
        hwdc_src = (AXMMDeviceContext *)hwfc_src->device_ctx->hwctx;
        hwfc_dst = (AVHWFramesContext *)dst->hw_frames_ctx->data;
        hwdc_dst = (AXMMDeviceContext *)hwfc_dst->device_ctx->hwctx;

        if (hwfc_src->format != AV_PIX_FMT_AXMM || hwfc_src->format != hwfc_dst->format) {
            av_log(ctx, AV_LOG_ERROR, "%s format mismatch, src-fmt:%s, dst-fmt:%s\n", __func__,
                   av_get_pix_fmt_name(hwfc_src->format), av_get_pix_fmt_name(hwfc_dst->format));
            return AVERROR(EINVAL);
        }

        fr_src = (hwdc_src->alloc_blk) ? (AX_VIDEO_FRAME_T *)src->data[3] : (AX_VIDEO_FRAME_T *)src->data[4];
        fr_dst = (hwdc_dst->alloc_blk) ? (AX_VIDEO_FRAME_T *)dst->data[3] : (AX_VIDEO_FRAME_T *)dst->data[4];

        if (fr_src->u32FrameSize != fr_dst->u32FrameSize) {
            av_log(ctx, AV_LOG_ERROR, "%s data size mismatch, src-size:0x%x, dst-size:0x%x\n", __func__,
                   fr_src->u32FrameSize, fr_dst->u32FrameSize);
            return AVERROR(EINVAL);
        }

        if (!fr_dst->u64PhyAddr[0] || !fr_src->u64PhyAddr[0]) {
            av_log(ctx, AV_LOG_ERROR, "%s addr invalid, src-paddr:0x%llx, dst-paddr:0x%llx\n", __func__,
                   fr_src->u64PhyAddr[0], fr_dst->u64PhyAddr[0]);
            return AVERROR(EINVAL);
        }

        memcpy((void *)fr_dst->u64VirAddr[0], (void *)fr_src->u64VirAddr[0], fr_dst->u32FrameSize);
    } else {
        int size;
        AX_VIDEO_FRAME_T *fr;

        if ((src->linesize[0] != dst->linesize[0]) ||
            (src->linesize[1] != dst->linesize[1]) ||
            (src->linesize[2] != dst->linesize[2])) {
            av_log(ctx, AV_LOG_ERROR, "%s linesize mismatch, src-linesize:%d-%d-%d, dst-linesize:%d-%d-%d\n", __func__,
                   src->linesize[0], src->linesize[1], src->linesize[2],
                   dst->linesize[0], dst->linesize[1], dst->linesize[2]);
            return AVERROR(EINVAL);
        }

        if (dst->hw_frames_ctx) { // host to device
            hwfc_dst = (AVHWFramesContext *)dst->hw_frames_ctx->data;
            hwdc_dst = (AXMMDeviceContext *)hwfc_dst->device_ctx->hwctx;

            fr = (hwdc_dst->alloc_blk) ? (AX_VIDEO_FRAME_T *)dst->data[3] : (AX_VIDEO_FRAME_T *)dst->data[4];

            if (hwfc_dst->format != AV_PIX_FMT_AXMM) {
                av_log(hwdc_dst, AV_LOG_ERROR, "%s format(%s) is not supported \n", __func__,
                       av_get_pix_fmt_name(hwfc_dst->format));
                return AVERROR(EINVAL);
            }

            if (hwfc_dst->sw_format != src->format) {
                av_log(ctx, AV_LOG_ERROR, "%s sw format mismatch, src-fmt:%s, dst-fmt:%s\n", __func__,
                       av_get_pix_fmt_name(src->format), av_get_pix_fmt_name(hwfc_dst->sw_format));
                return AVERROR(EINVAL);
            }

            if (!fr->u64VirAddr[0] || !src->data[0]) {
                av_log(ctx, AV_LOG_ERROR, "%s addr invalid, src-addr:0x%p, dst-paddr:0x%llx\n", __func__,
                       src->data[0], fr->u64PhyAddr[0]);
                return AVERROR(EINVAL);
            }

            size = src->linesize[0] * src->height;
            memcpy((void *)fr->u64VirAddr[0], src->data[0], size);

            if (src->data[1]) {
                uint64_t paddr = fr->u64VirAddr[1] ? fr->u64VirAddr[1] : fr->u64VirAddr[0] + size;

                size = (src->linesize[1] * src->height) >> 1;
                memcpy((void *)paddr, src->data[1], size);
            }
        } else { // device to host
            hwfc_src = (AVHWFramesContext *)src->hw_frames_ctx->data;
            hwdc_src = (AXMMDeviceContext *)hwfc_src->device_ctx->hwctx;

            fr = (hwdc_src->alloc_blk) ? (AX_VIDEO_FRAME_T *)src->data[3] : (AX_VIDEO_FRAME_T *)src->data[4];

            if (hwfc_src->format != AV_PIX_FMT_AXMM) {
                av_log(hwdc_src, AV_LOG_ERROR, "%s format(%s) is not supported\n", __func__,
                       av_get_pix_fmt_name(hwfc_src->format));
                return AVERROR(EINVAL);
            }

            if (hwfc_src->sw_format != dst->format) {
                av_log(ctx, AV_LOG_ERROR, "%s sw format mismatch, src-fmt:%s, dst-fmt:%s\n", __func__,
                       av_get_pix_fmt_name(hwfc_src->sw_format), av_get_pix_fmt_name(dst->format));
                return AVERROR(EINVAL);
            }

            if (!fr->u64VirAddr[0] || !dst->data[0]) {
                av_log(ctx, AV_LOG_ERROR, "%s addr invalid, src-paddr:0x%lld, dst-addr:0x%p\n", __func__,
                       fr->u64VirAddr[0], dst->data[0]);
                return AVERROR(EINVAL);
            }

            size = dst->linesize[0] * dst->height;
            memcpy(dst->data[0], (void *)fr->u64VirAddr[0], size);

            if (dst->data[1]) {
                uint64_t paddr = fr->u64VirAddr[1] ? fr->u64VirAddr[1] : fr->u64VirAddr[0] + size;

                size = (dst->linesize[1] * dst->height) >> 1;
                memcpy(dst->data[1], (void *)paddr, size);
            }

        }
    }

    return 0;
}

const HWContextType ff_hwcontext_type_axmm = {
    .type                    = AV_HWDEVICE_TYPE_AXMM,
    .name                    = "axmm",

    .device_hwctx_size       = sizeof(AXMMDeviceContext),
    .frames_hwctx_size       = sizeof(AXMMFramesContext),

    .device_create           = axmm_device_create,
    .device_init             = axmm_device_init,
    .device_uninit           = axmm_device_uninit,
    .frames_get_constraints  = axmm_frames_get_constraints,
    .frames_init             = axmm_frames_init,
    .frames_uninit           = axmm_frames_uninit,
    .frames_get_buffer       = axmm_get_buffer,
    .transfer_get_formats    = axmm_transfer_get_formats,
    .transfer_data_to        = axmm_transfer_data,
    .transfer_data_from      = axmm_transfer_data,

    .pix_fmts = (const enum AVPixelFormat[]){ AV_PIX_FMT_AXMM, AV_PIX_FMT_NONE },
};

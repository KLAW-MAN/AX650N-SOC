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

#ifndef AVCODEC_AXVDEC_H
#define AVCODEC_AXVDEC_H

#include "config.h"

#include "libavutil/fifo.h"
#include "libavutil/opt.h"
#include "hwconfig.h"

#include "avcodec.h"

#include "ax_vdec_api.h"

#define VDEC_GRP_MAX         (164)
#define AX_SHIFT_LEFT_ALIGN(a) (1 << (a))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

#define AX_VDEC_WIDTH_ALIGN     AX_SHIFT_LEFT_ALIGN(8)
#define AX_JDEC_WIDTH_ALIGN     AX_SHIFT_LEFT_ALIGN(6)

#define AX_VDEC_HEIGHT_ALIGN     AX_SHIFT_LEFT_ALIGN(6)

#define STREAM_BUFFER_MIN_SIZE           (10 * 1024 * 1024)
#define AX_VDEC_MAX_WIDTH           8192
#define AX_VDEC_MAX_HEIGHT          8192

#define AX_VDEC_DPB_NUM          16

#define LIB_VDEC_NAME    "libax_vdec.so"
#define LIB_SYS_NAME    "libax_sys.so"

#define CHECK_WITH_RET(cond, ret) \
    do { \
        if ((cond) != 0) { \
            av_log(NULL, AV_LOG_ERROR, "check %s failed\n", #cond); \
            return (ret); \
        } \
    } while (0)

#define AX_VDEC_ALIGN(value, n) (((value) + (n) - 1) & ~((n) - 1))

typedef struct {
    AX_S32(*sys_init)(AX_VOID);
    AX_S32(*sys_deinit)(AX_VOID);
	AX_VOID *(*sys_mmap)(AX_U64 phyaddr, AX_U32 size);
	AX_S32(*sys_munmap)(AX_VOID *pviraddr, AX_U32 size);
    AX_VOID *(*sys_getvir)(AX_BLK BlockId);
} AXSYSDevFuncs;

typedef struct {
    AX_S32(*vdec_init)(const AX_VDEC_MOD_ATTR_T *pstModAtt);
    AX_S32(*vdec_deinit)(AX_VOID);

    AX_S32(*vdec_creategrp)(AX_VDEC_GRP *VdGrp, const AX_VDEC_GRP_ATTR_T *pstGrpAttr);
    AX_S32(*vdec_destroygrp)(AX_VDEC_GRP VdGrp);

    AX_S32(*vdec_selectgrp)(AX_VDEC_GRP_SET_INFO_T *pstGrpSet, AX_S32 s32MilliSec);

    AX_S32(*vdec_sendstream)(AX_VDEC_GRP VdGrp, const AX_VDEC_STREAM_T *pstStream, AX_S32 s32MilliSec);

    AX_S32(*vdec_getframe)(AX_VDEC_GRP VdGrp, AX_VDEC_CHN VdChn, AX_VIDEO_FRAME_INFO_T *pstFrameInfo, AX_S32 s32MilliSec);

    AX_S32(*vdec_releaseframe)(AX_VDEC_GRP VdGrp, AX_VDEC_CHN VdChn, const AX_VIDEO_FRAME_INFO_T *pstFrameInfo);

    AX_S32(*vdec_startrecvsStream)(AX_VDEC_GRP VdGrp, const AX_VDEC_RECV_PIC_PARAM_T *pstRecvParam);
    AX_S32(*vdec_stoprecvstream)(AX_VDEC_GRP VdGrp);

    AX_S32(*vdec_querystatus)(AX_VDEC_GRP VdGrp, AX_VDEC_GRP_STATUS_T *pstStatus);
    AX_S32(*vdec_getstrminfo)(const AX_VDEC_STREAM_T *pstStreamBuf, AX_PAYLOAD_TYPE_E enVideoType, AX_VDEC_BITSTREAM_INFO_T *pstBitStreamInfo);

    AX_S32(*vdec_setgrpparam)(AX_VDEC_GRP VdGrp, const AX_VDEC_GRP_PARAM_T* pstGrpParam);

    AX_S32(*vdec_setdispmode)(AX_VDEC_GRP VdGrp, AX_VDEC_DISPLAY_MODE_E enDisplayMode);

    AX_S32(*vdec_enablechn)(AX_VDEC_GRP VdGrp, AX_VDEC_CHN VdChn);
    AX_S32(*vdec_disablechn)(AX_VDEC_GRP VdGrp, AX_VDEC_CHN VdChn);
    AX_S32(*vdec_setchnattr)(AX_VDEC_GRP VdGrp, AX_VDEC_CHN VdChn, const AX_VDEC_CHN_ATTR_T *pstVdChnAttr);
} AXVdecFuncs;

typedef struct AXVdecContext {
    AVClass *class;
    AX_PAYLOAD_TYPE_E enCodecType;
    AX_VDEC_GRP VdGrp;
    AX_VDEC_CHN VdChn;
    char *resize_expr;

    struct {
        int width;
        int height;
    } resize;

    char *out_fmt_string;
    enum AVPixelFormat format;

    int bit_depth_luma;
    int bit_depth_chroma;

    AX_U32 u32StreamBufSize;
    AX_VDEC_INPUT_MODE_E enInputMode;
    AX_IMG_FORMAT_E enImgFormat;        /* Pixel format of target image */
    AX_U32 u32PicWidth;                 /* Width of scaler or crop target image */
    AX_U32 u32PicHeight;                /* Height of scaler or crop target image */
    AX_U32 u32FrameStride;
    AX_U32 u32FramePadding;
    AX_U32 u32CropX;
    AX_U32 u32CropY;
    AX_U32 u32FrameBufCnt;      /* frame buffer number, valid if bSdkAutoFramePool is TRUE */
    AX_U32 u32OutputFifoDepth;
    AX_BOOL bEof;
    AX_BOOL bInited;

    AVBufferRef *hwdevice;
    AVBufferRef *hwframe;

    AVPacket buffer_pkt;

    void *sys_lib;
    AXSYSDevFuncs sysdev_funcs;

    void *mm_vdec_lib;
    AXVdecFuncs vdec_funcs;
} AXVdecContext;


#endif /* AVCODEC_AXVDEC_H */


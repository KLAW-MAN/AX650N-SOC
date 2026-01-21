/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include "axclite_vdec.hpp"
#include <algorithm>
#include <chrono>
#include <thread>
#include "ax_buffer_tool.h"
#include "axclite_vdec_dispatch.hpp"
#include "axcl_logger.hpp"

#define TAG "axclite-vdec"

namespace axclite {

axclError vdec::init(const axclite_vdec_attr &attr) {
    if (!check_attr(attr)) {
        return AXCL_ERR_LITE_VDEC_ILLEGAL_PARAM;
    }

    axclError ret;

    AX_VDEC_GRP_ATTR_T grp_attr = {};
    grp_attr.enCodecType = attr.grp.payload;
    grp_attr.enInputMode = AX_VDEC_INPUT_MODE_FRAME;
    grp_attr.u32MaxPicWidth = AXCL_ALIGN_UP(attr.grp.width, 16);   /* H264 MB 16x16 */
    grp_attr.u32MaxPicHeight = AXCL_ALIGN_UP(attr.grp.height, 16); /* H264 MB 16x16 */
    grp_attr.u32StreamBufSize = grp_attr.u32MaxPicWidth * grp_attr.u32MaxPicHeight * 2;
    grp_attr.bSdkAutoFramePool = AX_TRUE;

    if (ret = AXCL_VDEC_CreateGrpEx(&m_grp, &grp_attr); AXCL_SUCC != ret) {
        LOG_MM_E(TAG, "AXCL_VDEC_CreateGrpEx fail: codec {}, {}x{}, input mode {}, stream buf size {:#x}, ret = {:#x}", m_grp,
                 static_cast<int32_t>(grp_attr.enCodecType), grp_attr.u32MaxPicWidth, grp_attr.u32MaxPicHeight,
                 static_cast<int32_t>(grp_attr.enInputMode), grp_attr.u32StreamBufSize, static_cast<uint32_t>(ret));
        return ret;
    }

    LOG_MM_I(TAG, "vdGrp {}: codec {}, {}x{}, input mode {}, stream buf size {:#x}, playback mode {}", m_grp,
             static_cast<int32_t>(grp_attr.enCodecType), grp_attr.u32MaxPicWidth, grp_attr.u32MaxPicHeight,
             static_cast<int32_t>(grp_attr.enInputMode), grp_attr.u32StreamBufSize, static_cast<int32_t>(attr.grp.display_mode));

    AX_VDEC_GRP_PARAM_T grp_param = {};
    grp_param.stVdecVideoParam.enVdecMode = VIDEO_DEC_MODE_IPB;
    grp_param.stVdecVideoParam.enOutputOrder = attr.grp.output_order;
    if (ret = AXCL_VDEC_SetGrpParam(m_grp, &grp_param); AXCL_SUCC != ret) {
        LOG_MM_E(TAG, "AXCL_VDEC_SetGrpParam(vdGrp {}) fail, ret = {:#x}", m_grp, static_cast<uint32_t>(ret));

        AXCL_VDEC_DestroyGrp(m_grp);
        m_grp = INVALID_VDGRP_ID;
        return ret;
    }

    if (ret = AXCL_VDEC_SetDisplayMode(m_grp, attr.grp.display_mode); AXCL_SUCC != ret) {
        LOG_MM_E(TAG, "AXCL_VDEC_SetDisplayMode(vdGrp {}) fail, ret = {:#x}", m_grp, static_cast<uint32_t>(ret));

        AXCL_VDEC_DestroyGrp(m_grp);
        m_grp = INVALID_VDGRP_ID;
        return ret;
    }

    for (AX_VDEC_CHN chn = 0; chn < AX_DEC_MAX_CHN_NUM; ++chn) {
        if (!attr.chn[chn].enable) {
            continue;
        }

        if (!attr.chn[chn].link) {
            m_dispatchs[chn] = std::make_unique<vdec_dispatch>(m_grp, chn);
            if (!m_dispatchs[chn]) {
                LOG_MM_E(TAG, "create vdGrp {} vdChn {} dispatch instance fail", m_grp, chn);
                return AXCL_ERR_LITE_VDEC_NO_MEMORY;
            }
        }

        AX_VDEC_CHN_ATTR_T chn_attr = {};
        chn_attr.u32PicWidth = attr.chn[chn].width;
        chn_attr.u32PicHeight = attr.chn[chn].height;
        chn_attr.u32FrameStride = AXCL_ALIGN_UP(chn_attr.u32PicWidth, VDEC_STRIDE_ALIGN);
        chn_attr.u32CropX = attr.chn[chn].x;
        chn_attr.u32CropY = attr.chn[chn].y;
        chn_attr.enImgFormat = AX_FORMAT_YUV420_SEMIPLANAR;
        chn_attr.stCompressInfo = attr.chn[chn].fbc;
        chn_attr.u32OutputFifoDepth = attr.chn[chn].fifo_depth;
        chn_attr.u32FrameBufCnt = attr.chn[chn].blk_cnt;
        if (0 == chn) {
            chn_attr.enOutputMode = (chn_attr.u32CropX > 0 || chn_attr.u32CropY > 0) ? AX_VDEC_OUTPUT_CROP : AX_VDEC_OUTPUT_ORIGINAL;
        } else {
            chn_attr.enOutputMode = (chn_attr.u32CropX > 0 || chn_attr.u32CropY > 0) ? AX_VDEC_OUTPUT_CROP : AX_VDEC_OUTPUT_SCALE;
        }
        chn_attr.u32FrameBufSize = AX_VDEC_GetPicBufferSize(chn_attr.u32PicWidth, chn_attr.u32PicHeight, chn_attr.enImgFormat,
                                                            &chn_attr.stCompressInfo, attr.grp.payload);

        LOG_MM_I(TAG, "vdChn {}: {}x{} stride {} padding {}, fifo depth {}, ouput mode {}, vb(size {:#x}, cnt {}), compress {} lv {}", chn,
                 chn_attr.u32PicWidth, chn_attr.u32PicHeight, chn_attr.u32FrameStride, chn_attr.u32FramePadding,
                 chn_attr.u32OutputFifoDepth, static_cast<int32_t>(chn_attr.enOutputMode), chn_attr.u32FrameBufSize,
                 chn_attr.u32FrameBufCnt, static_cast<int32_t>(chn_attr.stCompressInfo.enCompressMode),
                 chn_attr.stCompressInfo.u32CompressLevel);

        if (ret = AXCL_VDEC_SetChnAttr(m_grp, chn, &chn_attr); AXCL_SUCC != ret) {
            LOG_MM_E(TAG, "AXCL_VDEC_SetChnAttr(vdGrp {}, vdChn {}) fail, ret = {:#x}", m_grp, chn, static_cast<uint32_t>(ret));
            for (AX_VDEC_CHN j = 0; j < chn; ++j) {
                AXCL_VDEC_DisableChn(m_grp, j);
            }

            AXCL_VDEC_DestroyGrp(m_grp);
            m_grp = INVALID_VDGRP_ID;
            return ret;
        }

        if (ret = AXCL_VDEC_EnableChn(m_grp, chn); AXCL_SUCC != ret) {
            LOG_MM_E(TAG, "AXCL_VDEC_EnableChn(vdGrp {}, vdChn {}) fail, ret = {:#x}", m_grp, chn, static_cast<uint32_t>(ret));
            for (AX_VDEC_CHN j = 0; j < chn; ++j) {
                AXCL_VDEC_DisableChn(m_grp, j);
            }

            AXCL_VDEC_DestroyGrp(m_grp);
            m_grp = INVALID_VDGRP_ID;
            return ret;
        }
    }

    m_attr = attr;
    return AXCL_SUCC;
}

axclError vdec::deinit() {
    LOG_MM_I(TAG, "vdGrp {} +++", m_grp);

    if (INVALID_VDGRP_ID == m_grp) {
        LOG_MM_W(TAG, "invalid vdGrp {}", m_grp);
        return AXCL_SUCC;
    }

    axclError ret;
    for (AX_VDEC_CHN chn = 0; chn < AX_VDEC_MAX_CHN_NUM; ++chn) {
        if (m_attr.chn[chn].enable) {
            if (ret = AXCL_VDEC_DisableChn(m_grp, chn); AXCL_SUCC != ret) {
                LOG_MM_E(TAG, "AXCL_VDEC_DisableChn(vdGrp {}, vdChn {}) fail, ret = {:#x}", m_grp, chn, static_cast<uint32_t>(ret));
                return ret;
            }
        }
    }

    LOG_M_D(TAG, "AXCL_VDEC_DestroyGrp(vdGrp {}) +++", m_grp);
    ret = AXCL_VDEC_DestroyGrp(m_grp);
    LOG_M_D(TAG, "AXCL_VDEC_DestroyGrp(vdGrp {}) ---", m_grp);
    if (AXCL_SUCC != ret) {
        LOG_MM_E(TAG, "AXCL_VDEC_DestroyGrp(vdGrp {}) fail, ret = {:#x}", m_grp, static_cast<uint32_t>(ret));
        return ret;
    }

    for (auto &&m : m_dispatchs) {
        m = nullptr;
    }

    LOG_MM_I(TAG, "vdGrp {} ---", m_grp);
    m_grp = INVALID_VDGRP_ID;

    return AXCL_SUCC;
}

axclError vdec::start(int32_t device) {
    if (m_started) {
        LOG_MM_W(TAG, "vdGrp {} is already started", m_grp);
        return AXCL_SUCC;
    }

    LOG_MM_I(TAG, "vdGrp {} +++", m_grp);

    AX_VDEC_RECV_PIC_PARAM_T recv_param = {};
    recv_param.s32RecvPicNum = -1;
    if (axclError ret = AXCL_VDEC_StartRecvStream(m_grp, &recv_param); AXCL_SUCC != ret) {
        LOG_MM_E(TAG, "AXCL_VDEC_StartRecvStream(vdGrp {}) fail, ret = {:#x}", m_grp, static_cast<uint32_t>(ret));
        return ret;
    }

    for (auto &&m : m_dispatchs) {
        if (m) {
            if (!m->start(device)) {
                AXCL_VDEC_StopRecvStream(m_grp);
                return AXCL_ERR_LITE_VDEC_START_DISPATCH;
            }
        }
    }

    m_last_send_code = 0;
    m_started = true;

    LOG_MM_I(TAG, "vdGrp {} --", m_grp);
    return AXCL_SUCC;
}

axclError vdec::stop() {
    if (!m_started) {
        LOG_MM_W(TAG, "vdGrp {} is not started yet", m_grp);
        return AXCL_SUCC;
    }

    axclError ret;
    LOG_MM_I(TAG, "vdGrp {} +++", m_grp);

    for (auto &&m : m_dispatchs) {
        if (m) {
            if (!m->stop()) {
                return AXCL_ERR_LITE_VDEC_STOP_DISPATCH;
            }
        }
    }

    if (ret = AXCL_VDEC_StopRecvStream(m_grp); AXCL_SUCC != ret) {
        LOG_M_E(TAG, "AXCL_VDEC_StopRecvStream(vdGrp %d) fail, ret = {:#x}", m_grp, static_cast<uint32_t>(ret));
        return ret;
    }

    m_started = false;
    m_last_send_code = 0;

    if (ret = reset_grp(MAX_VDEC_RESET_GRP_RETRY_COUNT); AXCL_SUCC != ret) {
        return ret;
    }

    for (auto &&m : m_dispatchs) {
        if (m) {
            m->join();
        }
    }

    LOG_MM_I(TAG, "vdGrp {} ---", m_grp);
    return AXCL_SUCC;
}

axclError vdec::send_stream(const AX_U8 *nalu, AX_U32 len, AX_U64 pts, AX_U64 userdata, AX_S32 timeout) {
    if (!m_started) {
        return AXCL_ERR_LITE_VDEC_NOT_STARTED;
    }

    AX_VDEC_STREAM_T stream = {};
    stream.u64PTS = pts;
    stream.pu8Addr = const_cast<AX_U8 *>(nalu);
    stream.u32StreamPackLen = len;
    stream.u64UserData = userdata;
    stream.bEndOfFrame = AX_TRUE;
    if (0 == len) {
        LOG_MM_I(TAG, "vdGrp {} send stream eof", m_grp);
        stream.bEndOfStream = AX_TRUE;
    }

    if (AX_ERR_VDEC_INVALID_GRPID == m_last_send_code || AX_ERR_VDEC_INVALID_CHNID == m_last_send_code ||
        AX_ERR_VDEC_NEED_REALLOC_BUF == m_last_send_code || AX_ERR_VDEC_UNKNOWN == m_last_send_code) {
        /* fatal error of VDEC to abandon to send */
        LOG_MM_W(TAG, "last nalu send to vdGrp {} ret = {:#x}, abandon {} bytes, pts {}", m_grp, m_last_send_code, len, pts);
        return m_last_send_code;
    }

    LOG_MM_D(TAG, "AXCL_VDEC_SendStream(vdGrp {}, len {}, pts {}, timeout {}) +++", m_grp, len, pts, timeout);
    m_last_send_code = AXCL_VDEC_SendStream(m_grp, &stream, timeout);
    LOG_MM_D(TAG, "AXCL_VDEC_SendStream(vdGrp {}, len {}, pts {}, timeout {}) ---", m_grp, len, pts, timeout);
    if (0 != m_last_send_code) {
        if ((AX_ERR_VDEC_BUF_FULL == m_last_send_code) || (AX_ERR_VDEC_QUEUE_FULL == m_last_send_code)) {
            LOG_MM_W(TAG, "vdGrp {} input buffer is full, abandon {} bytes, pts {}", m_grp, len, pts);
        } else if (AX_ERR_VDEC_NOT_PERM == m_last_send_code) {
            LOG_MM_W(TAG, "AX_VDEC_SendStream(vdGrp {}, len {}, pts {}, timeout {}) not permitted, ret = {:#x}", m_grp, len, pts, timeout,
                     static_cast<uint32_t>(m_last_send_code));
        } else {
            LOG_MM_E(TAG, "AX_VDEC_SendStream(vdGrp {}, len {}, pts {}, timeout {}) fail, ret = {:#x}", m_grp, len, pts, timeout,
                     static_cast<uint32_t>(m_last_send_code));
        }

        return m_last_send_code;
    }

    return AXCL_SUCC;
}

axclError vdec::register_sink(AX_VDEC_CHN chn, sinker *sink) {
    if (!CHECK_VDEC_CHN(chn)) {
        LOG_MM_E(TAG, "invalid vdChn {}", chn);
        return AXCL_ERR_LITE_VDEC_INVALID_CHN;
    }

    if (!sink) {
        LOG_MM_E(TAG, "vdGrp {} vdChn {} nil sink", m_grp, chn);
        return AXCL_ERR_LITE_VDEC_NULL_POINTER;
    }

    if (!m_attr.chn[chn].enable) {
        LOG_MM_E(TAG, "vdGrp {} vdChn {} is not enabled", m_grp, chn);
        return AXCL_ERR_LITE_VDEC_ILLEGAL_PARAM;
    }

    if (!m_dispatchs[chn]) {
        LOG_MM_E(TAG, "dispatcher of vdGrp {} vdChn {} is nil", m_grp, chn);
        return AXCL_ERR_LITE_VDEC_NULL_POINTER;
    }

    m_dispatchs[chn]->register_sink(sink);
    return AXCL_SUCC;
}

axclError vdec::unregister_sink(AX_VDEC_CHN chn, sinker *sink) {
    if (!CHECK_VDEC_CHN(chn)) {
        LOG_MM_E(TAG, "invalid vdChn {}", chn);
        return AXCL_ERR_LITE_VDEC_INVALID_CHN;
    }

    if (!sink) {
        LOG_MM_E(TAG, "vdGrp {} vdChn {} nil sink", m_grp, chn);
        return AXCL_ERR_LITE_VDEC_NULL_POINTER;
    }

    if (!m_dispatchs[chn]) {
        LOG_MM_E(TAG, "dispatcher of vdGrp {} vdChn {} is nil", m_grp, chn);
        return AXCL_ERR_LITE_VDEC_NULL_POINTER;
    }

    m_dispatchs[chn]->unregister_sink(sink);
    return AXCL_SUCC;
}

bool vdec::check_attr(const axclite_vdec_attr &attr) {
    if (0 == attr.grp.width || 0 == attr.grp.height) {
        LOG_MM_E(TAG, "invalid vdec output width {} or height {}", attr.grp.width, attr.grp.height);
        return false;
    }

    if (0 == std::count_if(std::begin(attr.chn), std::end(attr.chn), [](const axclite_vdec_chn_attr &m) { return m.enable; })) {
        LOG_MM_E(TAG, "at least 1 output channel should be enabled");
        return false;
    }

    for (size_t i = 0; i < sizeof(attr.chn) / sizeof(attr.chn[0]); ++i) {
        if (attr.chn[i].enable && 0 == attr.chn[i].fifo_depth) {
            /**
             * reason for VDEC AX_VDEC_CHN_ATTR_T.u32OutputFifoDepth must > 0:
             * VDEC decoded -> move YUV from VSI fifo to VDEC channel fifo
             *                                                          --> dispatch to next module if link mode
             *                                                          --> fetch out by app
             */
            LOG_MM_E(TAG, "output channel {} fifo depth should > 0", i);
            return false;
        }
    }

    return true;
}

axclError vdec::reset_grp(uint32_t max_retry_count) {
    axclError ret = AXCL_SUCC;
    uint32_t retry = 0;
    while (retry++ < max_retry_count) {
        if (ret = AXCL_VDEC_ResetGrp(m_grp); AXCL_SUCC != ret) {
            if (AX_ERR_VDEC_BUSY == ret) {
                LOG_MM_W(TAG, "[{}] vdGrp {} is busy, try again to reset", retry, m_grp);
            } else {
                LOG_MM_E(TAG, "[{}] AXCL_VDEC_ResetGrp(vdGrp {}) fail, ret = {:#x}", retry, m_grp, static_cast<uint32_t>(ret));
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(40));
            continue;
        } else {
            break;
        }
    }

    if (AXCL_SUCC != ret) {
        LOG_MM_E(TAG, "AXCL_VDEC_ResetGrp(vdGrp {}) failed after try {} times, ret = {:#x}", m_grp, retry, static_cast<uint32_t>(ret));
        return ret;
    }

    LOG_MM_I(TAG, "vdGrp {} is reset", m_grp);
    return AXCL_SUCC;
}

}  // namespace axclite
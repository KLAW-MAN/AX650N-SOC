/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include "axclite_venc.hpp"
#include "axcl_logger.hpp"

#define TAG "axclite-venc"

namespace axclite {

axclError venc::init(const axclite_venc_attr& attr) {
    if (!check_attr(attr)) {
        return AXCL_ERR_LITE_VENC_ILLEGAL_PARAM;
    }

    AX_VENC_CHN_ATTR_T chn_attr = {};
    AX_VENC_ATTR_T& venc_attr = chn_attr.stVencAttr;
    venc_attr.enType = attr.chn.payload;
    venc_attr.u32MaxPicWidth = attr.chn.width;
    venc_attr.u32MaxPicHeight = attr.chn.height;
    venc_attr.enMemSource = AX_MEMORY_SOURCE_CMM;
    venc_attr.u32BufSize = attr.chn.width * attr.chn.height * 3 / 2;
    venc_attr.enProfile = attr.chn.profile;
    venc_attr.enLevel = attr.chn.level;
    venc_attr.enTier = attr.chn.tile;
    venc_attr.enStrmBitDepth = AX_VENC_STREAM_BIT_8;
    venc_attr.u32PicWidthSrc = attr.chn.width;
    venc_attr.u32PicHeightSrc = attr.chn.height;
    venc_attr.enLinkMode = attr.chn.link ? AX_VENC_LINK_MODE : AX_VENC_UNLINK_MODE;
    venc_attr.u8InFifoDepth = static_cast<AX_U8>(attr.chn.in_fifo_depth);
    venc_attr.u8OutFifoDepth = static_cast<AX_U8>(attr.chn.out_fifo_depth);
    venc_attr.flag = attr.chn.flag;

    chn_attr.stRcAttr = attr.rc;
    chn_attr.stGopAttr = attr.gop;
    if (axclError ret = AXCL_VENC_CreateChnEx(&m_chn, &chn_attr); AXCL_SUCC != ret) {
        LOG_MM_E(TAG, "AXCL_VENC_CreateChnEx() fail, ret = {:#x}", static_cast<uint32_t>(ret));
        return ret;
    }

    LOG_MM_I(TAG, "veChn {}", m_chn);

    m_dispatch = std::make_unique<venc_dispatch>(m_chn, venc_attr.u32BufSize);
    if (!m_dispatch) {
        LOG_MM_E(TAG, "create veChn {} dispatch fail", m_chn);

        AXCL_VENC_DestroyChn(m_chn);
        m_chn = INVALID_VENC_CHN;
        return AXCL_ERR_LITE_VENC_NO_MEMORY;
    }

    return AXCL_SUCC;
}

axclError venc::deinit() {
    LOG_MM_I(TAG, "veChn {} +++", m_chn);

    if (!CHECK_VENC_CHN(m_chn)) {
        LOG_MM_W(TAG, "invalid veChn {}", m_chn);
        return AXCL_SUCC;
    }

    if (axclError ret = AXCL_VENC_DestroyChn(m_chn); AXCL_SUCC != ret) {
        LOG_MM_E(TAG, "AXCL_VENC_DestroyChn(veChn {}) fail, ret = {:#x}", m_chn, static_cast<uint32_t>(ret));
        return ret;
    }

    LOG_MM_I(TAG, "veChn {} ---", m_chn);
    m_chn = INVALID_VENC_CHN;

    return AXCL_SUCC;
}

axclError venc::start(int32_t device) {
    LOG_MM_I(TAG, "veChn {} +++", m_chn);

    AX_VENC_RECV_PIC_PARAM_T recv_param = {};
    recv_param.s32RecvPicNum = -1;
    if (axclError ret = AXCL_VENC_StartRecvFrame(m_chn, &recv_param); AXCL_SUCC != ret) {
        LOG_MM_E(TAG, "AXCL_VENC_StartRecvFrame(veChn {}) fail, ret = {:#x}", m_chn, static_cast<uint32_t>(ret));
        return ret;
    }

    if (!m_dispatch->start(device)) {
        AXCL_VENC_StopRecvFrame(m_chn);
        return AXCL_ERR_LITE_VENC_START_DISPATCH;
    }

    m_started = true;
    LOG_MM_I(TAG, "veChn {} ---", m_chn);
    return AXCL_SUCC;
}

axclError venc::stop() {
    if (!m_started) {
        LOG_MM_W(TAG, "veChn {} is not started yet", m_chn);
        return AXCL_SUCC;
    }

    LOG_MM_I(TAG, "veChn {} +++", m_chn);

    m_dispatch->stop();

    if (axclError ret = AXCL_VENC_StopRecvFrame(m_chn); AXCL_SUCC != ret) {
        LOG_MM_E(TAG, "AXCL_VENC_StopRecvFrame(veChn {}) fail, ret = {:#x}", m_chn, static_cast<uint32_t>(ret));
        return ret;
    }

    if (axclError ret = AXCL_VENC_ResetChn(m_chn); AXCL_SUCC != ret) {
        LOG_MM_E(TAG, "AXCL_VENC_ResetChn(veChn {}) fail, ret = {:#x}", m_chn, static_cast<uint32_t>(ret));
        return ret;
    }

    m_dispatch->join();

    LOG_MM_I(TAG, "veChn {} ---", m_chn);
    return AXCL_SUCC;
}

axclError venc::query_status(AX_VENC_CHN_STATUS_T &status) {
    return AXCL_VENC_QueryStatus(m_chn, &status);
}

axclError venc::register_sink(sinker* sink) {
    if (!sink) {
        LOG_MM_E(TAG, "sink of veChn {} is nil", m_chn);
        return AXCL_ERR_LITE_VENC_NULL_POINTER;
    }

    if (!m_dispatch) {
        LOG_MM_E(TAG, "dispatcher of veChn {} is nil", m_chn);
        return AXCL_ERR_LITE_VENC_NULL_POINTER;
    }

    m_dispatch->register_sink(sink);
    return AXCL_SUCC;
}

axclError venc::unregister_sink(sinker* sink) {
    if (!sink) {
        LOG_MM_E(TAG, "sink of veChn {} is nil", m_chn);
        return AXCL_ERR_LITE_VENC_NULL_POINTER;
    }

    if (!m_dispatch) {
        LOG_MM_E(TAG, "dispatcher of veChn {} is nil", m_chn);
        return AXCL_ERR_LITE_VENC_NULL_POINTER;
    }

    m_dispatch->unregister_sink(sink);
    return AXCL_SUCC;
}

bool venc::check_attr(const axclite_venc_attr& attr) {
    if (0 != (attr.chn.width % 2) || 0 != (attr.chn.height % 2)) {
        LOG_MM_E(TAG, "{} x {} must be align to 2", attr.chn.width, attr.chn.height);
        return false;
    }

    return true;
}

}  // namespace axclite
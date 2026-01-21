/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include "axclite_ivps.hpp"
#include "axcl_logger.hpp"

#define TAG "axclite-ivps"

namespace axclite {

axclError ivps::init(const axclite_ivps_attr& attr) {
    if (!check_attr(attr)) {
        return AXCL_ERR_LITE_IVPS_ILLEGAL_PARAM;
    }

    AX_IVPS_GRP_ATTR_T grp_attr = {};
    grp_attr.nInFifoDepth = attr.grp.fifo_depth;
    grp_attr.ePipeline = AX_IVPS_PIPELINE_DEFAULT;
    if (axclError ret = AXCL_IVPS_CreateGrpEx(&m_grp, &grp_attr); AXCL_SUCC != ret) {
        LOG_MM_E(TAG, "AXCL_IVPS_CreateGrpEx() fail, ret = {:#x}", static_cast<uint32_t>(ret));
        return ret;
    }

    LOG_MM_I(TAG, "ivGrp {}", m_grp);

    for (AX_U32 i = 0; i < attr.chn_num; ++i) {
        if (!attr.chn[i].link) {
            m_dispatchs[i] = std::make_unique<ivps_dispatch>(m_grp, static_cast<IVPS_CHN>(i));
            if (!m_dispatchs[i]) {
                LOG_MM_E(TAG, "create ivps ivGrp {} dispatch instance fail", m_grp);

                AXCL_IVPS_DestoryGrp(m_grp);
                m_grp = INVALID_IVPS_GRP;
                return AXCL_ERR_LITE_IVPS_NO_MEMORY;
            }
        }
    }

    m_attr = attr;
    return AXCL_SUCC;
}

axclError ivps::deinit() {
    if (!CHECK_IVPS_GRP(m_grp)) {
        LOG_MM_E(TAG, "invalid ivGrp {}", m_grp);
        return AXCL_ERR_LITE_IVPS_INVALID_GRP;
    }

    LOG_MM_I(TAG, "ivGrp {} +++", m_grp);

    if (axclError ret = AXCL_IVPS_DestoryGrp(m_grp); AXCL_SUCC != ret) {
        LOG_MM_E(TAG, "AXCL_IVPS_DestoryGrp(ivGrp {}) fail, ret = {:#x}", m_grp, static_cast<uint32_t>(ret));
        return ret;
    }

    for (auto&& m : m_dispatchs) {
        m = nullptr;
    }

    LOG_MM_I(TAG, "ivGrp {} ---", m_grp);
    m_grp = INVALID_IVPS_GRP;

    return AXCL_SUCC;
}

axclError ivps::start(int32_t device) {
    LOG_MM_I(TAG, "ivGrp {} +++", m_grp);

    if (m_started) {
        LOG_MM_W(TAG, "ivGrp {} is already running", m_grp);
        return AXCL_SUCC;
    }

    AX_IVPS_PIPELINE_ATTR_T pipe_attr = {};
    pipe_attr.nOutChnNum = m_attr.chn_num;
    for (AX_U32 i = 0; i < m_attr.chn_num; ++i) {
        set_pipline_attr(pipe_attr, i, m_attr.chn[i]);
    }

    if (axclError ret = AXCL_IVPS_SetPipelineAttr(m_grp, &pipe_attr); AXCL_SUCC != ret) {
        LOG_MM_E(TAG, "AXCL_IVPS_SetPipelineAttr(ivGrp {}) fail, ret = {:#x}", m_grp, static_cast<uint32_t>(ret));
        return ret;
    }

    auto disable_ivps_chns = [this](AX_U32 chn_num) -> void {
        for (AX_U32 j = 0; j < chn_num; ++j) {
            AXCL_IVPS_DisableChn(m_grp, static_cast<IVPS_CHN>(j));
        }
    };

    for (AX_U32 i = 0; i < m_attr.chn_num; ++i) {
        const IVPS_CHN chn = static_cast<IVPS_CHN>(i);

        AX_IVPS_POOL_ATTR_T pool_attr = {};
        pool_attr.ePoolSrc = POOL_SOURCE_PRIVATE;
        pool_attr.nFrmBufNum = m_attr.chn[i].blk_cnt;
        if (axclError ret = AXCL_IVPS_SetChnPoolAttr(m_grp, chn, &pool_attr); AXCL_SUCC != ret) {
            LOG_MM_E(TAG, "AXCL_IVPS_SetChnPoolAttr(ivGrp {} ivChn {}) fail, ret = {:#x}", m_grp, chn, static_cast<uint32_t>(ret));
            disable_ivps_chns(chn);
            return ret;
        }

        if (axclError ret = AXCL_IVPS_EnableChn(m_grp, chn); AXCL_SUCC != ret) {
            LOG_MM_E(TAG, "AX_IVPS_EnableChn(ivGrp {} ivChn {}) fail, ret = {:#x}", m_grp, chn, static_cast<uint32_t>(ret));
            disable_ivps_chns(chn);
            return ret;
        }
    }

    if (m_attr.grp.backup_depth > 0) {
        if (axclError ret = AXCL_IVPS_EnableBackupFrame(m_grp, m_attr.grp.backup_depth); AXCL_SUCC != ret) {
            LOG_MM_E(TAG, "AXCL_IVPS_EnableBackupFrame(ivGrp {}, depth {}) fail, ret = {:#x}", m_grp, m_attr.grp.backup_depth,
                     static_cast<uint32_t>(ret));
            disable_ivps_chns(m_attr.chn_num);
            return ret;
        }
    }

    if (axclError ret = AXCL_IVPS_StartGrp(m_grp); AXCL_SUCC != ret) {
        LOG_MM_E(TAG, "AXCL_IVPS_StartGrp(ivGrp {}) fail, ret = {:#x}", m_grp, static_cast<uint32_t>(ret));
        disable_ivps_chns(m_attr.chn_num);
        return ret;
    }

    for (auto&& m : m_dispatchs) {
        if (m) {
            if (!m->start(device)) {
                disable_ivps_chns(m_attr.chn_num);
                AXCL_IVPS_StopGrp(m_grp);
                return AXCL_ERR_LITE_IVPS_START_DISPATCH;
            }
        }
    }

    m_started = true;

    LOG_MM_I(TAG, "ivGrp {} ---", m_grp);
    return AXCL_SUCC;
}

axclError ivps::stop() {
    LOG_MM_I(TAG, "ivGrp {} +++", m_grp);

    if (!m_started) {
        LOG_MM_W(TAG, "ivGrp {} is started", m_grp);
        return AXCL_SUCC;
    }

    for (auto&& m : m_dispatchs) {
        if (m) {
            if (!m->stop()) {
                return AXCL_ERR_LITE_IVPS_STOP_DISPATCH;
            }

            m->join();
        }
    }

    for (AX_U32 i = 0; i < m_attr.chn_num; ++i) {
        const IVPS_CHN chn = static_cast<IVPS_CHN>(i);
        if (axclError ret = AXCL_IVPS_DisableChn(m_grp, chn); AXCL_SUCC != ret) {
            LOG_MM_E(TAG, "AXCL_IVPS_DisableChn(ivGrp {} ivChn {}) fail, ret = {:#x}", m_grp, chn, static_cast<uint32_t>(ret));
            return ret;
        }
    }

    if (m_attr.grp.backup_depth > 0) {
        if (axclError ret = AXCL_IVPS_DisableBackupFrame(m_grp); AXCL_SUCC != ret) {
            LOG_MM_E(TAG, "AXCL_IVPS_DisableBackupFrame(ivGrp {}) fail, ret = {:#x}", m_grp, static_cast<uint32_t>(ret));
            return ret;
        }
    }

    if (axclError ret = AXCL_IVPS_StopGrp(m_grp); AXCL_SUCC != ret) {
        LOG_MM_E(TAG, "AXCL_IVPS_StopGrp(ivGrp {}) fail, ret = {:#x}", m_grp, static_cast<uint32_t>(ret));
        return ret;
    }

    m_started = false;

    LOG_MM_I(TAG, "ivGrp {} ---", m_grp);
    return AXCL_SUCC;
}

void ivps::set_pipline_attr(AX_IVPS_PIPELINE_ATTR_T& pipe_attr, AX_S32 chn, const axclite_ivps_chn_attr& chn_attr) {
    pipe_attr.nOutFifoDepth[chn] = chn_attr.fifo_depth;

    AX_IVPS_FILTER_T& filter = pipe_attr.tFilter[chn + 1][0];

    filter.bEngage = chn_attr.bypass ? AX_FALSE : AX_TRUE;
    filter.eEngine = chn_attr.engine;
    filter.tFRC = chn_attr.frc;
    filter.bCrop = chn_attr.crop;
    filter.tCropRect = chn_attr.box;
    filter.nDstPicWidth = chn_attr.width;
    filter.nDstPicHeight = chn_attr.height;
    if (0 == chn_attr.stride) {
        if (chn_attr.fbc.enCompressMode != AX_COMPRESS_MODE_NONE) {
            filter.nDstPicStride = AXCL_ALIGN_UP(chn_attr.width, 128);
        } else {
            filter.nDstPicStride = AXCL_ALIGN_UP(chn_attr.width, 16);
        }
        LOG_MM_W(TAG, "chn {} filter stride is 0, auto set to {}", chn, filter.nDstPicStride);
    } else {
        filter.nDstPicStride = chn_attr.stride;
    }

    filter.eDstPicFormat = chn_attr.pix_fmt;
    filter.tCompressInfo = chn_attr.fbc;
    filter.bInplace = chn_attr.inplace;
}

axclError ivps::register_sink(IVPS_CHN chn, sinker* sink) {
    if (!CHECK_IVPS_CHN(chn)) {
        LOG_MM_E(TAG, "invalid ivChn {} of ivGrp {}", chn, m_grp);
        return AXCL_ERR_LITE_IVPS_INVALID_CHN;
    }

    if (!sink) {
        LOG_MM_E(TAG, "sink of ivGrp {} ivChn {} is nil", m_grp, chn);
        return AXCL_ERR_LITE_IVPS_NULL_POINTER;
    }

    if (!m_dispatchs[chn]) {
        LOG_MM_E(TAG, "dispatcher of ivGrp {} ivChn {} is nil", m_grp, chn);
        return AXCL_ERR_LITE_IVPS_NULL_POINTER;
    }

    m_dispatchs[chn]->regist_sink(sink);
    return AXCL_SUCC;
}

axclError ivps::unregister_sink(IVPS_CHN chn, sinker* sink) {
    if (!CHECK_IVPS_CHN(chn)) {
        LOG_MM_E(TAG, "invalid ivChn {} of ivGrp {}", chn, m_grp);
        return AXCL_ERR_LITE_IVPS_INVALID_CHN;
    }

    if (!sink) {
        LOG_MM_E(TAG, "sink of ivGrp {} ivChn {} is nil", m_grp, chn);
        return AXCL_ERR_LITE_IVPS_NULL_POINTER;
    }

    if (!m_dispatchs[chn]) {
        LOG_MM_E(TAG, "dispatcher of ivGrp {} ivChn {} is nil", m_grp, chn);
        return AXCL_ERR_LITE_IVPS_NULL_POINTER;
    }

    m_dispatchs[chn]->unregist_sink(sink);
    return AXCL_SUCC;
}

axclError ivps::send_frame(const AX_VIDEO_FRAME_T& frame, AX_S32 timeout) {
    if (!CHECK_IVPS_GRP(m_grp)) {
        LOG_MM_E(TAG, "ivGrp {} is destoryed", m_grp);
        return AXCL_ERR_LITE_IVPS_INVALID_GRP;
    }

    if (!m_started) {
        LOG_MM_E(TAG, "ivGrp {} is not started yet", m_grp);
        return AXCL_ERR_LITE_IVPS_START_DISPATCH;
    }

    if (axclError ret = AXCL_IVPS_SendFrame(m_grp, &frame, timeout); AXCL_SUCC != ret) {
        LOG_MM_E(TAG, "AXCL_IVPS_SendFrame(frame {}, ivGrp {}) fail, ret = {:#x}", frame.u64SeqNum, m_grp, static_cast<uint32_t>(ret));
        return ret;
    }

    return AXCL_SUCC;
}

bool ivps::check_attr(const axclite_ivps_attr& attr) {
    for (AX_U32 i = 0; i < attr.chn_num; ++i) {
        const axclite_ivps_chn_attr& chn_attr = attr.chn[i];

        if (!chn_attr.link && (0 == chn_attr.fifo_depth)) {
            LOG_MM_E(TAG, "output ivChn {} is unlinked, but out fifo depth is 0", i);
            return false;
        }

        if (chn_attr.crop) {
            const AX_IVPS_RECT_T& box = chn_attr.box;
            if (((box.nX + box.nW) > (AX_U16)chn_attr.width) || ((box.nY + box.nH) > (AX_U16)chn_attr.height)) {
                LOG_MM_E(TAG, "ivChn {} crop rect [{} {} {}x{}] is invalid", i, box.nX, box.nY, box.nW, box.nH);
                return false;
            }
        }

        if (0 == chn_attr.blk_cnt) {
            LOG_M_E(TAG, "ivChn {} is private pool, but blk cnt is 0", i);
            return false;
        }
    }

    return true;
}

}  // namespace axclite
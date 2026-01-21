/**************************************************************************************************
 *
 * Copyright (c) 2019-2025 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include "ppl_hvcp.hpp"
#include "axclite_msys.hpp"
#include "axcl_logger.hpp"

#define TAG "ppl-hvcp"

ppl_hvcp::ppl_hvcp(int32_t ppl_id, int32_t device_id, const axcl_ppl_hvcp_param& param)
    : m_ppl_id(ppl_id), m_device_id(device_id), m_ppl_param(param) {
    m_vdec = std::make_unique<axclite::vdec>();
    m_hvcp = std::make_unique<axclite::hvcp>(this, device_id, param.hvcp_param);
}

axclError ppl_hvcp::init() {
    axclError ret;

    if (ret = m_hvcp->init(); AXCL_SUCC != ret) {
        return ret;
    }

    m_model_input_image_info = m_hvcp->get_input_image_info();

    /**
     * VDEC output channels:
     *  - channel 0: output width and height must same as input stream width and height
     *  - channel 1: support scaling down, max output [4096, 4096]
     *  - channel 2: support scaling down, max output [1920, 1080]
     *
     * To save DDR bandwidth:
     *    If model input image < input stream, output by VDEC directly, otherwise link to IVPS to scale up or CSC.
     */
    if (m_model_input_image_info.width > m_ppl_param.vdec.width || m_model_input_image_info.height > m_ppl_param.vdec.height ||
        AX_FORMAT_YUV420_SEMIPLANAR != m_model_input_image_info.img_fmt) {
        m_ivps = std::make_unique<axclite::ivps>();
    }

    if (ret = m_vdec->init(get_vdec_attr()); AXCL_SUCC != ret) {
        m_hvcp->deinit();
        return ret;
    }

    if (m_ivps) {
        if (ret = m_ivps->init(get_ivps_attr()); AXCL_SUCC != ret) {
            m_vdec->deinit();
            m_hvcp->deinit();
            return ret;
        }

        m_ivps->register_sink(0, m_hvcp.get());
    } else {
        m_vdec->register_sink(1, m_hvcp.get());
    }

    if (m_ivps) {
        MSYS()->link({AX_ID_VDEC, m_vdec->get_grp_id(), 0}, {AX_ID_IVPS, m_ivps->get_grp_id(), 0});
    }

    return AXCL_SUCC;
}

axclError ppl_hvcp::deinit() {
    if (m_ivps) {
        m_ivps->unregister_sink(0, m_hvcp.get());
        MSYS()->unlink({AX_ID_VDEC, m_vdec->get_grp_id(), 0}, {AX_ID_IVPS, m_ivps->get_grp_id(), 0});
    } else {
        m_vdec->unregister_sink(1, m_hvcp.get());
    }

    axclError ret;
    if (ret = m_hvcp->deinit(); AXCL_SUCC != ret) {
        return ret;
    }

    if (m_ivps) {
        if (ret = m_ivps->deinit(); AXCL_SUCC != ret) {
            return ret;
        }

        m_ivps = nullptr;
    }

    if (ret = m_vdec->deinit(); AXCL_SUCC != ret) {
        return ret;
    }

    return AXCL_SUCC;
}

axclError ppl_hvcp::start() {
    if (m_started) {
        LOG_MM_W(TAG, "ppl is already started");
        return AXCL_SUCC;
    }

    LOG_MM_I(TAG, "+++");

    axclError ret;
    if (ret = m_hvcp->start(); AXCL_SUCC != ret) {
        return ret;
    }

    if (m_ivps) {
        if (ret = m_ivps->start(m_device_id); AXCL_SUCC != ret) {
            m_hvcp->stop();
            return ret;
        }
    }

    if (ret = m_vdec->start(m_device_id); AXCL_SUCC != ret) {
        if (m_ivps) {
            m_ivps->stop();
        }

        m_hvcp->stop();
        return ret;
    }

    m_started = true;

    LOG_MM_I(TAG, "---");
    return AXCL_SUCC;
}

axclError ppl_hvcp::stop() {
    if (!m_started) {
        LOG_MM_W(TAG, "ppl is not started yet");
        return AXCL_SUCC;
    }

    m_hvcp->stop();

    if (m_ivps) {
        m_ivps->stop();
    }

    m_vdec->stop();

    m_started = false;
    return AXCL_SUCC;
}

axclError ppl_hvcp::send_stream(const axcl_ppl_input_stream* stream, AX_S32 timeout) {
    if (!m_started) {
        // LOG_MM_E(TAG, "ppl not started");
        return AXCL_ERR_LITE_PPL_NOT_STARTED;
    }

    if (!stream) {
        LOG_MM_E(TAG, "stream is nil");
        return AXCL_ERR_LITE_PPL_NULL_POINTER;
    }

    return m_vdec->send_stream(stream->nalu, stream->nalu_len, stream->pts, stream->userdata, timeout);
}

axclite_vdec_attr ppl_hvcp::get_vdec_attr() {
    axclite_vdec_attr attr = {};
    attr.grp.payload = m_ppl_param.vdec.payload;
    attr.grp.width = m_ppl_param.vdec.width;
    attr.grp.height = m_ppl_param.vdec.height;
    attr.grp.output_order = m_ppl_param.vdec.output_order;
    attr.grp.display_mode = m_ppl_param.vdec.display_mode;

    axclite_vdec_chn_attr& chn = m_ivps ? attr.chn[0] : attr.chn[1];
    chn.enable = AX_TRUE;
    chn.link = m_ivps ? AX_TRUE : AX_FALSE;
    chn.width = m_ivps ? m_ppl_param.vdec.width : m_model_input_image_info.width;
    chn.height = m_ivps ? m_ppl_param.vdec.height : m_model_input_image_info.height;

    if (m_ivps) {
        if (AX_IVPS_ENGINE_TDP == m_ivps_engine) {
            /* TDP cannot support resize + FBCDC (cause artifacts) */
            chn.fbc.enCompressMode = AX_COMPRESS_MODE_NONE;
            chn.fbc.u32CompressLevel = 0;
        } else {
            chn.fbc.enCompressMode = AX_COMPRESS_MODE_LOSSY;
            chn.fbc.u32CompressLevel = 4;
        }
    } else {
        chn.fbc.enCompressMode = AX_COMPRESS_MODE_NONE;
        chn.fbc.u32CompressLevel = 0;
    }

    chn.blk_cnt = m_vdec_blk_cnt;
    chn.fifo_depth = chn.blk_cnt;

    return attr;
}

axclite_ivps_attr ppl_hvcp::get_ivps_attr() {
    axclite_ivps_attr attr = {};
    attr.grp.fifo_depth = 4;
    attr.grp.backup_depth = 0;

    attr.chn_num = 1;

    axclite_ivps_chn_attr& chn = attr.chn[0];
    chn.bypass = AX_FALSE;
    chn.link = AX_FALSE;
    chn.fifo_depth = 4;
    chn.engine = static_cast<AX_IVPS_ENGINE_E>(m_ivps_engine);
    chn.crop = AX_FALSE;
    chn.width = m_model_input_image_info.width;
    chn.height = m_model_input_image_info.height;
    chn.stride = AXCL_ALIGN_UP(chn.width, 16);
    chn.pix_fmt = m_model_input_image_info.img_fmt;
    chn.fbc.enCompressMode = AX_COMPRESS_MODE_NONE;
    chn.fbc.u32CompressLevel = 0;
    chn.inplace = AX_FALSE;
    chn.blk_cnt = m_ivps_blk_cnt;

    return attr;
}

axclError ppl_hvcp::get_attr(const char* name, void* attr) {
    return AXCL_SUCC;
}

axclError ppl_hvcp::set_attr(const char* name, const void* attr) {
    return AXCL_SUCC;
}
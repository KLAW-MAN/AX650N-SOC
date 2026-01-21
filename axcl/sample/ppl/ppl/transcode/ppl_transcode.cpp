/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include "ppl_transcode.hpp"
#include <string.h>
#include "axclite_msys.hpp"
#include "axcl_logger.hpp"
#include "os.hpp"

#define TAG "ppl-transcode"

static AX_U64 get_ms_ticks() {
    return xp_time_get_tick_ms();
}

ppl_transcode::ppl_transcode(int32_t id, int32_t device, const axcl_ppl_transcode_param& param)
    : m_device(device), m_id(id), m_param(param), m_sink(this, param.cb, param.userdata) {
    m_vdec = std::make_unique<axclite::vdec>();
    m_venc = std::make_unique<axclite::venc>();

    if (m_param.venc.width > m_param.vdec.width || m_param.venc.height > m_param.vdec.height) {
        m_ivps = std::make_unique<axclite::ivps>();
    }
}

axclError ppl_transcode::init() {
    axclError ret;

    if (ret = m_vdec->init(get_vdec_attr()); AXCL_SUCC != ret) {
        return ret;
    }

    if (m_ivps) {
        if (ret = m_ivps->init(get_ivps_attr()); AXCL_SUCC != ret) {
            m_vdec->deinit();
            return ret;
        }
    }

    if (ret = m_venc->init(get_venc_attr()); AXCL_SUCC != ret) {
        if (m_ivps) {
            m_ivps->deinit();
        }

        m_vdec->deinit();
        return ret;
    }

    m_venc->register_sink(&m_sink);

    if (m_ivps) {
        MSYS()->link({AX_ID_VDEC, m_vdec->get_grp_id(), 0}, {AX_ID_IVPS, m_ivps->get_grp_id(), 0});
        MSYS()->link({AX_ID_IVPS, m_ivps->get_grp_id(), 0}, {AX_ID_VENC, 0, m_venc->get_chn_id()});
    } else {
        MSYS()->link({AX_ID_VDEC, m_vdec->get_grp_id(), 1}, {AX_ID_VENC, 0, m_venc->get_chn_id()});
    }

    return AXCL_SUCC;
}

axclError ppl_transcode::deinit() {
    m_venc->unregister_sink(&m_sink);

    if (m_ivps) {
        MSYS()->unlink({AX_ID_VDEC, m_vdec->get_grp_id(), 0}, {AX_ID_IVPS, m_ivps->get_grp_id(), 0});
        MSYS()->unlink({AX_ID_IVPS, m_ivps->get_grp_id(), 0}, {AX_ID_VENC, 0, m_venc->get_chn_id()});
    } else {
        MSYS()->unlink({AX_ID_VDEC, m_vdec->get_grp_id(), 1}, {AX_ID_VENC, 0, m_venc->get_chn_id()});
    }

    axclError ret;
    if (ret = m_venc->deinit(); AXCL_SUCC != ret) {
        return ret;
    }

    if (m_ivps) {
        if (ret = m_ivps->deinit(); AXCL_SUCC != ret) {
            return ret;
        }
    }

    if (ret = m_vdec->deinit(); AXCL_SUCC != ret) {
        return ret;
    }

    return AXCL_SUCC;
}

axclError ppl_transcode::start() {
    if (m_started) {
        LOG_MM_W(TAG, "ppl is already started");
        return AXCL_SUCC;
    }

    LOG_MM_I(TAG, "+++");

    axclError ret;
    if (ret = m_venc->start(m_device); AXCL_SUCC != ret) {
        return ret;
    }

    if (m_ivps) {
        if (ret = m_ivps->start(m_device); AXCL_SUCC != ret) {
            m_venc->stop();
            return ret;
        }
    }

    if (ret = m_vdec->start(m_device); AXCL_SUCC != ret) {
        if (m_ivps) {
            m_ivps->stop();
        }

        m_venc->stop();
        return ret;
    }

    m_started = true;
    return AXCL_SUCC;
}

axclError ppl_transcode::stop() {
    if (!m_started) {
        LOG_MM_W(TAG, "ppl is not started yet");
        return AXCL_SUCC;
    }

    if (0 != m_venc_stop_wait_time) {
        AX_U64 start = get_ms_ticks();
        do {
            AX_VENC_CHN_STATUS_T status = {};
            if (axclError ret = m_venc->query_status(status); AXCL_SUCC != ret) {
                return ret;
            }

            if (0 == (status.u32LeftPics + status.u32LeftStreamFrames)) {
                break;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(200));

        } while (m_venc_stop_wait_time < 0 || ((int32_t)(get_ms_ticks() - start) < m_venc_stop_wait_time));
    }

    m_venc->stop();
    if (m_ivps) {
        m_ivps->stop();
    }
    m_vdec->stop();

    m_started = false;
    return AXCL_SUCC;
}

axclError ppl_transcode::send_stream(const axcl_ppl_input_stream* stream, AX_S32 timeout) {
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

axclError ppl_transcode::get_attr(const char* name, void* attr) {
    if (0 == strcmp(name, "axcl.ppl.transcode.vdec.grp")) {
        *(reinterpret_cast<int32_t*>(attr)) = m_vdec->get_grp_id();
    } else if (0 == strcmp(name, "axcl.ppl.transcode.ivps.grp")) {
        if (m_ivps) {
            *(reinterpret_cast<int32_t*>(attr)) = m_ivps->get_grp_id();
        } else {
            *(reinterpret_cast<int32_t*>(attr)) = -1;
        }
    } else if (0 == strcmp(name, "axcl.ppl.transcode.venc.chn")) {
        *(reinterpret_cast<int32_t*>(attr)) = m_venc->get_chn_id();
    } else if (0 == strcmp(name, "axcl.ppl.transcode.id")) {
        *(reinterpret_cast<int32_t*>(attr)) = m_id;
    } else if (0 == strcmp(name, "axcl.ppl.transcode.vdec.blk.cnt")) {
        *(reinterpret_cast<uint32_t*>(attr)) = m_vdec_blk_cnt;
    } else if (0 == strcmp(name, "axcl.ppl.transcode.vdec.out.depth")) {
        *(reinterpret_cast<uint32_t*>(attr)) = m_vdec_out_fifo_depth;
    } else if (0 == strcmp(name, "axcl.ppl.transcode.ivps.in.depth")) {
        *(reinterpret_cast<uint32_t*>(attr)) = m_ivps_in_fifo_depth;
    } else if (0 == strcmp(name, "axcl.ppl.transcode.ivps.out.depth")) {
        *(reinterpret_cast<uint32_t*>(attr)) = m_ivps_out_fifo_depth;
    } else if (0 == strcmp(name, "axcl.ppl.transcode.ivps.blk.cnt")) {
        *(reinterpret_cast<uint32_t*>(attr)) = m_ivps_blk_cnt;
    } else if (0 == strcmp(name, "axcl.ppl.transcode.ivps.engine")) {
        *(reinterpret_cast<uint32_t*>(attr)) = m_ivps_engine;
    } else if (0 == strcmp(name, "axcl.ppl.transcode.venc.in.depth")) {
        *(reinterpret_cast<uint32_t*>(attr)) = m_venc_in_fifo_depth;
    } else if (0 == strcmp(name, "axcl.ppl.transcode.venc.out.depth")) {
        *(reinterpret_cast<uint32_t*>(attr)) = m_venc_out_fifo_depth;
    } else if (0 == strcmp(name, "axcl.ppl.transcode.venc.stop.wait")) {
        *(reinterpret_cast<int32_t*>(attr)) = m_venc_stop_wait_time;
    } else {
        LOG_MM_E(TAG, "unsupport attribute {}", name);
        return AXCL_ERR_LITE_PPL_UNSUPPORT;
    }

    return AXCL_SUCC;
}

axclError ppl_transcode::set_attr(const char* name, const void* attr) {
    if (0 == strcmp(name, "axcl.ppl.transcode.vdec.blk.cnt")) {
        m_vdec_blk_cnt = *(reinterpret_cast<const uint32_t*>(attr));
    } else if (0 == strcmp(name, "axcl.ppl.transcode.vdec.out.depth")) {
        m_vdec_out_fifo_depth = *(reinterpret_cast<const uint32_t*>(attr));
    } else if (0 == strcmp(name, "axcl.ppl.transcode.ivps.in.depth")) {
        m_ivps_in_fifo_depth = *(reinterpret_cast<const uint32_t*>(attr));
    } /* else if (0 == strcmp(name, "axcl.ppl.transcode.ivps.out.depth")) {
        m_ivps_out_fifo_depth = *(reinterpret_cast<const uint32_t*>(attr));
    } */
    else if (0 == strcmp(name, "axcl.ppl.transcode.ivps.blk.cnt")) {
        m_ivps_blk_cnt = *(reinterpret_cast<const uint32_t*>(attr));
    } else if (0 == strcmp(name, "axcl.ppl.transcode.ivps.engine")) {
        int32_t engine = *(reinterpret_cast<const uint32_t*>(attr));
        if (!(AX_IVPS_ENGINE_VPP == engine || AX_IVPS_ENGINE_VGP == engine || AX_IVPS_ENGINE_TDP == engine)) {
            LOG_MM_E(TAG, "only support AX_IVPS_ENGINE_VPP|AX_IVPS_ENGINE_VGP|AX_IVPS_ENGINE_TDP");
            return AXCL_ERR_LITE_PPL_UNSUPPORT;
        } else {
            m_ivps_engine = engine;
        }
    } else if (0 == strcmp(name, "axcl.ppl.transcode.venc.in.depth")) {
        m_venc_in_fifo_depth = *(reinterpret_cast<const uint32_t*>(attr));
    } else if (0 == strcmp(name, "axcl.ppl.transcode.venc.out.depth")) {
        m_venc_out_fifo_depth = *(reinterpret_cast<const uint32_t*>(attr));
    } else if (0 == strcmp(name, "axcl.ppl.transcode.venc.stop.wait")) {
        m_venc_stop_wait_time = *(reinterpret_cast<const int32_t*>(attr));
    } else {
        LOG_MM_E(TAG, "unsupport attribute {}", name);
        return AXCL_ERR_LITE_PPL_UNSUPPORT;
    }

    return AXCL_SUCC;
}

axclite_vdec_attr ppl_transcode::get_vdec_attr() {
    axclite_vdec_attr attr = {};
    attr.grp.payload = m_param.vdec.payload;
    attr.grp.width = m_param.vdec.width;
    attr.grp.height = m_param.vdec.height;
    attr.grp.output_order = m_param.vdec.output_order;
    attr.grp.display_mode = m_param.vdec.display_mode;

    axclite_vdec_chn_attr& chn = m_ivps ? attr.chn[0] : attr.chn[1];
    chn.enable = AX_TRUE;
    chn.link = AX_TRUE;
    chn.width = m_ivps ? m_param.vdec.width : m_param.venc.width;
    chn.height = m_ivps ? m_param.vdec.height : m_param.venc.height;
    if (m_ivps && AX_IVPS_ENGINE_TDP == m_ivps_engine) {
        /* TDP cannot support resize + FBCDC (cause artifacts) */
        chn.fbc.enCompressMode = AX_COMPRESS_MODE_NONE;
        chn.fbc.u32CompressLevel = 0;
    } else {
        chn.fbc.enCompressMode = AX_COMPRESS_MODE_LOSSY;
        chn.fbc.u32CompressLevel = 4;
    }
    chn.blk_cnt = m_vdec_blk_cnt;
    chn.fifo_depth = m_vdec_out_fifo_depth;

    return attr;
}

axclite_ivps_attr ppl_transcode::get_ivps_attr() {
    axclite_ivps_attr attr = {};
    attr.grp.fifo_depth = m_ivps_in_fifo_depth;
    attr.grp.backup_depth = 0;

    attr.chn_num = 1;

    axclite_ivps_chn_attr& chn = attr.chn[0];
    chn.bypass = AX_FALSE;
    chn.link = AX_TRUE;
    chn.fifo_depth = m_ivps_out_fifo_depth;
    chn.engine = static_cast<AX_IVPS_ENGINE_E>(m_ivps_engine);
    chn.crop = AX_FALSE;
    chn.width = m_param.venc.width;
    chn.height = m_param.venc.height;
    chn.stride = AXCL_ALIGN_UP(chn.width, 256);
    chn.pix_fmt = AX_FORMAT_YUV420_SEMIPLANAR;
    if (m_ivps && AX_IVPS_ENGINE_TDP == m_ivps_engine) {
        /* fixme: enable FBC, TDP fps down, why? why? why? */
        chn.fbc.enCompressMode = AX_COMPRESS_MODE_NONE;
        chn.fbc.u32CompressLevel = 0;
    } else {
        chn.fbc.enCompressMode = AX_COMPRESS_MODE_LOSSY;
        chn.fbc.u32CompressLevel = 4;
    }
    chn.inplace = AX_FALSE;
    chn.blk_cnt = m_ivps_blk_cnt;

    return attr;
}

axclite_venc_attr ppl_transcode::get_venc_attr() {
    axclite_venc_attr attr = {};
    attr.chn.payload = m_param.venc.payload;
    attr.chn.width = m_param.venc.width;
    attr.chn.height = m_param.venc.height;
    attr.chn.profile = m_param.venc.profile;
    attr.chn.level = m_param.venc.level;
    attr.chn.tile = m_param.venc.tile;
    attr.chn.link = AX_TRUE;
    attr.chn.in_fifo_depth = m_venc_in_fifo_depth;
    attr.chn.out_fifo_depth = m_venc_out_fifo_depth;
    attr.chn.flag = 0;  //(1 << 1); /* cached stream */

    attr.rc = m_param.venc.rc;
    attr.gop = m_param.venc.gop;

    return attr;
}
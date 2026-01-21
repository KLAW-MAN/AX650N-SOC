/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include "axclite_vdec_dispatch.hpp"
#include <string.h>
#include <algorithm>
#include <chrono>
#include "axclite_helper.hpp"
#include "axclite_sink.hpp"
#include "axcl_logger.hpp"

#define TAG "axclite-vdec-dispatch"

namespace axclite {

vdec_dispatch::vdec_dispatch(AX_VDEC_GRP grp, AX_VDEC_CHN chn) : m_grp(grp), m_chn(chn) {
}

void vdec_dispatch::dispatch_thread(int32_t device) {
    LOG_MM_D(TAG, "vdGrp {} vdChn {} +++", m_grp, m_chn);

    context_guard context_holder(device);

    axclError ret;
    constexpr AX_S32 TIMEOUT = -1;
    AX_VIDEO_FRAME_INFO_T frame = {};
    while (m_thread.running()) {
        ret = AXCL_VDEC_GetChnFrame(m_grp, m_chn, &frame, TIMEOUT);
        if (0 != ret) {
            if (AX_ERR_VDEC_FLOW_END == ret) {
                LOG_MM_I(TAG, "vdGrp {} vdChn {} received flow end", m_grp, m_chn);
                break;
            } else if (AX_ERR_VDEC_STRM_ERROR == ret) {
                LOG_MM_W(TAG, "AXCL_VDEC_GetChnFrame(vdGrp {}, vdChn {}): stream is undecodeable", m_grp, m_chn);
            } else if (AX_ERR_VDEC_UNEXIST == ret) {
                LOG_MM_D(TAG, "vdGrp {} vdChn {} maybe under reseting", m_grp, m_chn);
            } else {
                LOG_MM_E(TAG, "AXCL_VDEC_GetChnFrame(vdGrp {}, vdChn {}, timeout {}) fail, ret = {:#x}", m_grp, m_chn, TIMEOUT,
                         static_cast<uint32_t>(ret));
            }

            continue;
        }

        LOG_MM_I(TAG, "decoded vdGrp {} vdChn {} frame {} pts {} phy {:#x} {}x{} stride {} blkId {:#x}", m_grp, m_chn,
                 frame.stVFrame.u64SeqNum, frame.stVFrame.u64PTS, frame.stVFrame.u64PhyAddr[0], frame.stVFrame.u32Width,
                 frame.stVFrame.u32Height, frame.stVFrame.u32PicStride[0], frame.stVFrame.u32BlkId[0]);

        (void)dispatch_frame(frame);

        if (ret = AXCL_VDEC_ReleaseChnFrame(m_grp, m_chn, &frame); 0 != ret) {
            LOG_MM_E(TAG, "AXCL_VDEC_ReleaseChnFrame(vdGrp {}, vdChn {}, blk {:#x}) fail, ret = {:#x}", m_grp, m_chn,
                     frame.stVFrame.u32BlkId[0], static_cast<uint32_t>(ret));
        }
    }

    LOG_MM_D(TAG, "vdGrp {} vdChn {} ---", m_grp, m_chn);
}

bool vdec_dispatch::start(int32_t device) {
    if (m_started) {
        LOG_MM_W(TAG, "vdec dispatch thread of vdGrp {} vdChn {} is already started", m_grp, m_chn);
        return true;
    }

    LOG_MM_D(TAG, "vdGrp {} vdChn {} +++", m_grp, m_chn);

    char name[16];
    sprintf(name, "vdec-disp%d-%d", m_grp, m_chn);
    m_thread.start(name, SCHED_FIFO, 99, &vdec_dispatch::dispatch_thread, this, device);

    m_started = true;

    LOG_MM_D(TAG, "vdGrp {} vdChn {} ---", m_grp, m_chn);
    return true;
}

bool vdec_dispatch::stop() {
    if (!m_started) {
        LOG_MM_W(TAG, "vdec dispatch thread of vdGrp {} vdChn {} is not started yet", m_grp, m_chn);
        return true;
    }

    LOG_MM_D(TAG, "vdGrp {} vdChn {} +++", m_grp, m_chn);

    m_thread.stop();

    LOG_MM_D(TAG, "vdGrp {} vdChn {} ---", m_grp, m_chn);
    return true;
}

void vdec_dispatch::join() {
    LOG_MM_D(TAG, "vdGrp {} vdChn {} +++", m_grp, m_chn);

    m_thread.join();
    m_started = false;

    LOG_MM_D(TAG, "vdGrp {} vdChn {} ---", m_grp, m_chn);
}

void vdec_dispatch::register_sink(sinker* sink) {
    std::lock_guard<std::mutex> lck(m_mtx_sinks);
    auto it = std::find(m_lst_sinks.begin(), m_lst_sinks.end(), sink);
    if (it != m_lst_sinks.end()) {
        LOG_MM_W(TAG, "sink {} is already registed to vdGrp {} vdChn {}", reinterpret_cast<void*>(sink), m_grp, m_chn);
    } else {
        m_lst_sinks.push_back(sink);
        LOG_MM_I(TAG, "regist sink {} to vdGrp {} vdChn {} success", reinterpret_cast<void*>(sink), m_grp, m_chn);
    }
}

void vdec_dispatch::unregister_sink(sinker* sink) {
    std::lock_guard<std::mutex> lck(m_mtx_sinks);
    auto it = std::find(m_lst_sinks.begin(), m_lst_sinks.end(), sink);
    if (it != m_lst_sinks.end()) {
        m_lst_sinks.remove(sink);
        LOG_MM_I(TAG, "unregist sink {} from vdGrp {} vdChn {} success", reinterpret_cast<void*>(sink), m_grp, m_chn);
    } else {
        LOG_MM_W(TAG, "sink {} is not registed to vdGrp {} vdChn {}", reinterpret_cast<void*>(sink), m_grp, m_chn);
    }
}

bool vdec_dispatch::dispatch_frame(const AX_VIDEO_FRAME_INFO_T& frame) {
    axclite_frame axframe;
    axframe.grp = m_grp;
    axframe.chn = m_chn;
    axframe.module = AXCL_LITE_VDEC;
    axframe.frame = frame;

    std::lock_guard<std::mutex> lck(m_mtx_sinks);
    for (auto&& m : m_lst_sinks) {
        if (m) {
            (void)m->recv_frame(axframe);
        }
    }

    return true;
}

}  // namespace axclite
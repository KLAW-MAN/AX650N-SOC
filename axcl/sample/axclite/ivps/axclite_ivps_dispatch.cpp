/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include "axclite_ivps_dispatch.hpp"
#include <algorithm>
#include <chrono>
#include "axclite_helper.hpp"
#include "axclite_sink.hpp"
#include "axcl_logger.hpp"

#define TAG "axclite-ivps-dispatch"

namespace axclite {

ivps_dispatch::ivps_dispatch(IVPS_GRP grp, IVPS_CHN chn) : m_grp(grp), m_chn(chn) {
}

void ivps_dispatch::dispatch_thread(int32_t device) {
    LOG_MM_D(TAG, "ivGrp {} ivChn {} +++", m_grp, m_chn);

    context_guard context_holder(device);

    axclError ret;
    AX_VIDEO_FRAME_T frame = {};
    constexpr AX_S32 TIMEOUT = 100;
    while (1) {
        wait();

        if (!m_thread.running()) {
            break;
        }

        if (ret = AXCL_IVPS_GetChnFrame(m_grp, m_chn, &frame, TIMEOUT); AXCL_SUCC != ret) {
            if (AX_ERR_IVPS_BUF_EMPTY != ret) {
                LOG_MM_E(TAG, "AXCL_IVPS_GetChnFrame(ivGrp {} ivChn {}) fail, ret = {:#x}", m_grp, m_chn, static_cast<uint32_t>(ret));
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            continue;
        }

        LOG_MM_I(TAG, "received ivGrp {} ivChn {} frame {} pts {} phy {:#x} {}x{} stride {} blkId {:#x}", m_grp, m_chn, frame.u64SeqNum,
                 frame.u64PTS, frame.u64PhyAddr[0], frame.u32Width, frame.u32Height, frame.u32PicStride[0], frame.u32BlkId[0]);

        (void)dispatch_frame(frame);

        if (ret = AXCL_IVPS_ReleaseChnFrame(m_grp, m_chn, &frame); AXCL_SUCC != ret) {
            LOG_MM_E(TAG, "AXCL_IVPS_ReleaseChnFrame(frame {}, ivGrp {}  ivChn {}) fail, ret = {:#x}", frame.u64SeqNum, m_grp, m_chn,
                     static_cast<uint32_t>(ret));
            continue;
        }
    }

    LOG_MM_D(TAG, "ivGrp {} ivChn {} ---", m_grp, m_chn);
}

bool ivps_dispatch::start(int32_t device) {
    if (m_started) {
        LOG_MM_W(TAG, "ivps dispatch thread of ivGrp {} ivChn {} is already started", m_grp, m_chn);
        return true;
    }

    LOG_MM_D(TAG, "ivGrp {} ivChn {} +++", m_grp, m_chn);

    char name[16];
    sprintf(name, "ivps-disp%d-%d", m_grp, m_chn);
    m_thread.start(name, &ivps_dispatch::dispatch_thread, this, device);

    m_started = true;

    LOG_MM_D(TAG, "ivGrp {} ivChn {} ---", m_grp, m_chn);
    return true;
}

bool ivps_dispatch::stop() {
    if (!m_started) {
        LOG_MM_W(TAG, "ivps dispatch thread of ivGrp {} ivChn {} is not started", m_grp, m_chn);
        return true;
    }

    LOG_MM_D(TAG, "ivGrp {} ivChn {} +++", m_grp, m_chn);

    m_thread.stop();
    resume();

    LOG_MM_D(TAG, "ivGrp {} ivChn {} ---", m_grp, m_chn);
    return true;
}

void ivps_dispatch::join() {
    LOG_MM_D(TAG, "ivGrp {} ivChn {} +++", m_grp, m_chn);

    m_thread.join();
    m_started = false;

    LOG_MM_D(TAG, "ivGrp {} ivChn {} ---", m_grp, m_chn);
}

void ivps_dispatch::regist_sink(sinker* sink) {
    std::lock_guard<std::mutex> lck(m_mtx_sinks);
    auto it = std::find(m_lst_sinks.begin(), m_lst_sinks.end(), sink);
    if (it != m_lst_sinks.end()) {
        LOG_MM_W(TAG, "sink {} is already registed to ivGrp {} ivChn {}", reinterpret_cast<void*>(sink), m_grp, m_chn);
    } else {
        m_lst_sinks.push_back(sink);
        LOG_MM_I(TAG, "regist sink {} to ivGrp {} ivChn {} success", reinterpret_cast<void*>(sink), m_grp, m_chn);
    }
}

void ivps_dispatch::unregist_sink(sinker* sink) {
    std::lock_guard<std::mutex> lck(m_mtx_sinks);
    auto it = std::find(m_lst_sinks.begin(), m_lst_sinks.end(), sink);
    if (it != m_lst_sinks.end()) {
        m_lst_sinks.remove(sink);
        LOG_MM_I(TAG, "unregist sink {} from ivGrp {} ivChn {} success", reinterpret_cast<void*>(sink), m_grp, m_chn);
    } else {
        LOG_MM_W(TAG, "sink {} is not registed to ivGrp {} ivChn {}", reinterpret_cast<void*>(sink), m_grp, m_chn);
    }
}

bool ivps_dispatch::dispatch_frame(const AX_VIDEO_FRAME_T& frame) {
    axclite_frame axframe;
    axframe.grp = m_grp;
    axframe.chn = m_chn;
    axframe.module = AXCL_LITE_IVPS;
    axframe.frame.stVFrame = frame;
    axframe.frame.enModId = AX_ID_IVPS;
    axframe.frame.bEndOfStream = AX_TRUE;

    std::lock_guard<std::mutex> lck(m_mtx_sinks);
    for (auto&& m : m_lst_sinks) {
        if (m) {
            (void)m->recv_frame(axframe);
        }
    }

    return true;
}

void ivps_dispatch::wait(void) {
    std::unique_lock<std::mutex> lck(m_mtx);
    while (m_paused && m_thread.running()) {
        m_cv.wait(lck);
    }
}

void ivps_dispatch::paused(void) {
    std::lock_guard<std::mutex> lck(m_mtx);
    m_paused = true;
}

void ivps_dispatch::resume(void) {
    m_mtx.lock();
    m_paused = false;
    m_mtx.unlock();

    m_cv.notify_one();
}

}  // namespace axclite

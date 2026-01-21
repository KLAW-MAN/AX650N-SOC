/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include "axclite_venc_dispatch.hpp"
#include <algorithm>
#include <vector>
#include "axclite_helper.hpp"
#include "axclite_sink.hpp"
#include "axcl_logger.hpp"

#define TAG "axclite-venc-dispatch"

namespace axclite {

venc_dispatch::venc_dispatch(VENC_CHN chn, AX_U32 max_stream_size) : m_chn(chn), m_max_stream_size(max_stream_size) {
}

void venc_dispatch::dispatch_thread(int32_t device) {
    LOG_MM_D(TAG, "veChn {} +++", m_chn);

    context_guard context_holder(device);
    axclError ret;
    AX_VENC_STREAM_T packed, stream;

    std::vector<AX_U8> nalu;
    nalu.reserve(m_max_stream_size);
    bool dispatch = false;

    while (m_thread.running()) {
        ret = AXCL_VENC_GetStream(m_chn, &packed, -1);
        if (AXCL_SUCC != ret) {
            if (AX_ERR_VENC_FLOW_END == ret) {
                break;
            }

            if (AX_ERR_VENC_QUEUE_EMPTY == ret) {
                LOG_MM_W(TAG, "no stream in veChn {} fifo", m_chn);
            } else {
                LOG_MM_E(TAG, "AXCL_VENC_GetStream(veChn {}) fail, ret = {:#x}", m_chn, ret);
            }

            continue;
        }

        if (packed.stPack.u32Len > m_max_stream_size) {
            LOG_MM_W(TAG, "stream size {} is small, reallo size {}", m_max_stream_size, packed.stPack.u32Len);
            nalu.resize(packed.stPack.u32Len);
        }

        stream = packed;
        stream.stPack.pu8Addr = nalu.data();
        if (ret = axclrtMemcpy(reinterpret_cast<void*>(stream.stPack.pu8Addr), reinterpret_cast<void*>(packed.stPack.ulPhyAddr),
                               packed.stPack.u32Len, AXCL_MEMCPY_DEVICE_TO_HOST);
            AXCL_SUCC != ret) {
            dispatch = false;
            LOG_MM_E(TAG, "axclrtMemcpy(size: {}) from device fail, ret = {:#x}", packed.stPack.u32Len, static_cast<uint32_t>(ret));
        } else {
            dispatch = true;
        }

        if (ret = AXCL_VENC_ReleaseStream(m_chn, &stream); AXCL_SUCC != ret) {
            LOG_MM_E(TAG, "AXCL_VENC_ReleaseStream(veChn {}) fail, ret = {:#x}", m_chn, static_cast<uint32_t>(ret));
            continue;
        }

        if (dispatch) {
            (void)dispatch_stream(stream);
        }
    }

    LOG_MM_D(TAG, "veChn {} ---", m_chn);
}

bool venc_dispatch::start(int32_t device) {
    if (m_started) {
        LOG_MM_W(TAG, "venc dispatch thread of veChn {} is already started", m_chn);
        return true;
    }

    LOG_MM_D(TAG, "veChn {} +++", m_chn);

    char name[16];
    sprintf(name, "venc-disp%d", m_chn);
    m_thread.start(name, &venc_dispatch::dispatch_thread, this, device);

    m_started = true;

    LOG_MM_D(TAG, "veChn {} ---", m_chn);
    return true;
}

bool venc_dispatch::stop() {
    if (!m_started) {
        LOG_MM_W(TAG, "venc dispatch thread of veChn {} is not started", m_chn);
        return true;
    }

    LOG_MM_D(TAG, "veChn {} +++", m_chn);
    m_thread.stop();
    LOG_MM_D(TAG, "veChn {} ---", m_chn);
    return true;
}

void venc_dispatch::join() {
    LOG_MM_D(TAG, "veChn {} +++", m_chn);
    m_thread.join();
    m_started = false;
    LOG_MM_D(TAG, "veChn {} ---", m_chn);
}

void venc_dispatch::register_sink(sinker* sink) {
    std::lock_guard<std::mutex> lck(m_mtx_sinks);
    auto it = std::find(m_lst_sinks.begin(), m_lst_sinks.end(), sink);
    if (it != m_lst_sinks.end()) {
        LOG_MM_W(TAG, "sink {} is already registed to veChn {}", reinterpret_cast<void*>(sink), m_chn);
    } else {
        m_lst_sinks.push_back(sink);
        LOG_MM_I(TAG, "regist sink {} to veChn {} success", reinterpret_cast<void*>(sink), m_chn);
    }
}

void venc_dispatch::unregister_sink(sinker* sink) {
    std::lock_guard<std::mutex> lck(m_mtx_sinks);
    auto it = std::find(m_lst_sinks.begin(), m_lst_sinks.end(), sink);
    if (it != m_lst_sinks.end()) {
        m_lst_sinks.remove(sink);
        LOG_MM_I(TAG, "unregist sink {} from veChn {} success", reinterpret_cast<void*>(sink), m_chn);
    } else {
        LOG_MM_W(TAG, "sink {} is not registed to veChn {}", reinterpret_cast<void*>(sink), m_chn);
    }
}

void venc_dispatch::unregister_all_sinks() {
    std::lock_guard<std::mutex> lck(m_mtx_sinks);
    m_lst_sinks.clear();
}

bool venc_dispatch::dispatch_stream(const AX_VENC_STREAM_T& stream) {
    axclite_frame axframe;
    axframe.grp = 0;
    axframe.chn = m_chn;
    axframe.module = AXCL_LITE_VENC;
    axframe.stream = stream;

    std::lock_guard<std::mutex> lck(m_mtx_sinks);
    for (auto&& m : m_lst_sinks) {
        if (m) {
            (void)m->recv_frame(axframe);
        }
    }

    return true;
}

}  // namespace axclite
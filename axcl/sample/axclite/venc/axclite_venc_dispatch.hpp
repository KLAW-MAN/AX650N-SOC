/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#pragma once

#include <atomic>
#include <list>
#include <mutex>
#include "axclite_venc_type.h"
#include "threadx.hpp"

namespace axclite {

class sinker;
class AXCL_EXPORT venc_dispatch {
public:
    venc_dispatch(VENC_CHN chn, AX_U32 max_stream_size);

    bool start(int32_t device);
    bool stop();
    void join();

    void register_sink(sinker* sink);
    void unregister_sink(sinker* sink);
    void unregister_all_sinks();

protected:
    void dispatch_thread(int32_t device);
    bool dispatch_stream(const AX_VENC_STREAM_T& stream);

protected:
    VENC_CHN m_chn;
    AX_U32 m_max_stream_size;
    std::list<sinker*> m_lst_sinks;
    std::mutex m_mtx_sinks;
    axcl::threadx m_thread;
    std::atomic<bool> m_started = {false};
};
}  // namespace axclite
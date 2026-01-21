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
#include <condition_variable>
#include <list>
#include <mutex>
#include "axclite_ivps_type.h"
#include "threadx.hpp"

namespace axclite {

class sinker;
class AXCL_EXPORT ivps_dispatch {
public:
    ivps_dispatch(IVPS_GRP grp, IVPS_CHN chn);

    bool start(int32_t device);
    bool stop();
    void join();

    void paused();
    void resume();

    void regist_sink(sinker* sink);
    void unregist_sink(sinker* sink);

protected:
    void wait();
    void dispatch_thread(int32_t device);
    bool dispatch_frame(const AX_VIDEO_FRAME_T& frame);

protected:
    IVPS_GRP m_grp = INVALID_IVPS_GRP;
    IVPS_CHN m_chn = INVALID_IVPS_CHN;
    std::list<sinker*> m_lst_sinks;
    std::mutex m_mtx_sinks;
    std::mutex m_mtx;
    std::condition_variable m_cv;
    axcl::threadx m_thread;
    std::atomic<bool> m_paused = {false};
    std::atomic<bool> m_started = {false};
};

}  // namespace axclite
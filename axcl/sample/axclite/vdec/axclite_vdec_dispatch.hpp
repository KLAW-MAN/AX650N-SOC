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
#include "axclite_vdec_type.h"
#include "threadx.hpp"
#include "os.hpp"

namespace axclite {

class sinker;
class AXCL_EXPORT vdec_dispatch {
public:
    vdec_dispatch(AX_VDEC_GRP grp, AX_VDEC_CHN chn);

    bool start(int32_t device);
    bool stop();
    void join();

    void register_sink(sinker* sink);
    void unregister_sink(sinker* sink);

protected:
    void dispatch_thread(int32_t device);
    bool dispatch_frame(const AX_VIDEO_FRAME_INFO_T& frame);

protected:
    AX_VDEC_GRP m_grp;
    AX_VDEC_CHN m_chn;

    std::atomic<bool> m_started = {false};
    axcl::threadx m_thread;
    std::condition_variable m_cv;
    std::mutex m_mtx;

    std::mutex m_mtx_sinks;
    std::list<sinker*> m_lst_sinks;
};

}  // namespace axclite
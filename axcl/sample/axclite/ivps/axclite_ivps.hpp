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
#include <memory>
#include "axclite.h"
#include "axclite_ivps_dispatch.hpp"

namespace axclite {

class AXCL_EXPORT ivps {
public:
    ivps() = default;

    axclError init(const axclite_ivps_attr &attr);
    axclError deinit();

    axclError start(int32_t device);
    axclError stop();

    axclError send_frame(const AX_VIDEO_FRAME_T &frame, AX_S32 timeout = -1);

    axclError register_sink(IVPS_CHN chn, sinker *sink);
    axclError unregister_sink(IVPS_CHN chn, sinker *sink);

    AX_S32 get_grp_id() const {
        return static_cast<AX_S32>(m_grp);
    }

    const axclite_ivps_attr &get_attr() const {
        return m_attr;
    }

protected:
    bool check_attr(const axclite_ivps_attr &attr);
    void set_pipline_attr(AX_IVPS_PIPELINE_ATTR_T &pipe_attr, AX_S32 chn, const axclite_ivps_chn_attr &chn_attr);

protected:
    axclite_ivps_attr m_attr;
    IVPS_GRP m_grp = INVALID_IVPS_GRP;
    std::atomic<bool> m_started = {false};
    std::unique_ptr<ivps_dispatch> m_dispatchs[AX_IVPS_MAX_OUTCHN_NUM];
};

}  // namespace axclite
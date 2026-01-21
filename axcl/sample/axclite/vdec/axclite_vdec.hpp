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
#include "axclite_vdec_dispatch.hpp"

namespace axclite {

class AXCL_EXPORT vdec {
public:
    vdec() = default;

    axclError init(const axclite_vdec_attr &attr);
    axclError deinit();

    axclError start(int32_t device);
    axclError stop();

    axclError send_stream(const AX_U8 *nalu, AX_U32 len, AX_U64 pts, AX_U64 userdata = 0, AX_S32 timeout = -1);

    axclError register_sink(AX_VDEC_CHN chn, sinker *sink);
    axclError unregister_sink(AX_VDEC_CHN chn, sinker *sink);

    AX_S32 get_grp_id() const {
        return static_cast<AX_S32>(m_grp);
    }

    const axclite_vdec_attr &get_attr() const {
        return m_attr;
    }

protected:
    bool check_attr(const axclite_vdec_attr &attr);
    axclError reset_grp(uint32_t max_retry_count);

private:
    axclite_vdec_attr m_attr;
    AX_VDEC_GRP m_grp = INVALID_VDGRP_ID;
    AX_S32 m_last_send_code = 0;
    std::atomic<bool> m_started = {false};
    std::unique_ptr<vdec_dispatch> m_dispatchs[AX_VDEC_MAX_CHN_NUM];
};

}  // namespace axclite
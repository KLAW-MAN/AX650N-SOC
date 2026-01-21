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
#include "axclite_venc_dispatch.hpp"
#include "axclite_venc_type.h"

namespace axclite {

class AXCL_EXPORT venc {
public:
    venc() = default;

    axclError init(const axclite_venc_attr& attr);
    axclError deinit();

    axclError start(int32_t device);
    axclError stop();

    axclError query_status(AX_VENC_CHN_STATUS_T &status);

    axclError register_sink(sinker* sink);
    axclError unregister_sink(sinker* sink);

    AX_S32 get_chn_id() const {
        return static_cast<AX_S32>(m_chn);
    }

    const axclite_venc_attr& get_attr() const {
        return m_attr;
    }

protected:
    bool check_attr(const axclite_venc_attr& attr);

protected:
    axclite_venc_attr m_attr;
    VENC_CHN m_chn = INVALID_VENC_CHN;
    std::unique_ptr<venc_dispatch> m_dispatch;
    std::atomic<bool> m_started = {false};
};

}  // namespace axclite
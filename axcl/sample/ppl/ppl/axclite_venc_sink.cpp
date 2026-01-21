/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include "axclite_venc_sink.hpp"
#include "axcl_logger.hpp"

#define TAG "axclite-venc-sink"

namespace axclite {

venc_sinker::venc_sinker(axcl_ppl ppl, axcl_ppl_encoded_stream_callback_func func, AX_U64 userdata)
    : m_ppl(ppl), m_func(func), m_userdata(userdata) {
}

int32_t venc_sinker::recv_frame(const axclite_frame& frame) {
    if (AXCL_LITE_VENC != frame.module) {
        LOG_MM_E(TAG, "frame is not from VENC module {}", frame.module);
        return 1;
    }

    m_func(m_ppl, &frame.stream, m_userdata);
    return 0;
}

}  // namespace axclite
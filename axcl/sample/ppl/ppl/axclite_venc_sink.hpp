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

#include "axcl_ppl_type.h"
#include "axclite_sink.hpp"

namespace axclite {

class venc_sinker final : public sinker {
public:
    venc_sinker(axcl_ppl ppl, axcl_ppl_encoded_stream_callback_func func, AX_U64 userdata);
    int32_t recv_frame(const axclite_frame& frame);

private:
    axcl_ppl m_ppl;
    axcl_ppl_encoded_stream_callback_func m_func;
    AX_U64 m_userdata;
};

}  // namespace axclite
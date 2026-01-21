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
#include <mutex>
#include "axcl_ppl.h"

class ppl_core final {
public:
    ppl_core() = default;

    axclError init(axcl_ppl_init_param* param);
    axclError deinit();

    axclError create(axcl_ppl* ppl, const axcl_ppl_param* param);
    axclError destroy(axcl_ppl ppl);

    axclError start(axcl_ppl ppl);
    axclError stop(axcl_ppl ppl);

    axclError send_stream(axcl_ppl ppl, const axcl_ppl_input_stream* stream, AX_S32 timeout);

    axclError get_attr(axcl_ppl ppl, const char* name, void* attr);
    axclError set_attr(axcl_ppl ppl, const char* name, const void* attr);

private:
    axclError check_init_param(const axcl_ppl_init_param* param);

private:
    int32_t m_device = -1;
    std::mutex m_mtx;
    std::atomic<int32_t> m_id = {0};
    bool m_inited = false;
};

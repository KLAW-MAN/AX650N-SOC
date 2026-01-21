/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include "ppl_core.hpp"
#include "axcl_module_version.h"

static ppl_core g_ppl;

AXCL_EXPORT axclError axcl_ppl_init(axcl_ppl_init_param* param) {
    return g_ppl.init(param);
}

AXCL_EXPORT axclError axcl_ppl_deinit() {
    return g_ppl.deinit();
}

AXCL_EXPORT axclError axcl_ppl_create(axcl_ppl* ppl, const axcl_ppl_param* param) {
    return g_ppl.create(ppl, param);
}

AXCL_EXPORT axclError axcl_ppl_start(axcl_ppl ppl) {
    return g_ppl.start(ppl);
}

AXCL_EXPORT axclError axcl_ppl_stop(axcl_ppl ppl) {
    return g_ppl.stop(ppl);
}

AXCL_EXPORT axclError axcl_ppl_send_stream(axcl_ppl ppl, const axcl_ppl_input_stream* stream, AX_S32 timeout) {
    return g_ppl.send_stream(ppl, stream, timeout);
}

AXCL_EXPORT axclError axcl_ppl_destroy(axcl_ppl ppl) {
    return g_ppl.destroy(ppl);
}

AXCL_EXPORT axclError axcl_ppl_get_attr(axcl_ppl ppl, const char* name, void* attr) {
    return g_ppl.get_attr(ppl, name, attr);
}

AXCL_EXPORT axclError axcl_ppl_set_attr(axcl_ppl ppl, const char* name, const void* attr) {
    return g_ppl.set_attr(ppl, name, attr);
}
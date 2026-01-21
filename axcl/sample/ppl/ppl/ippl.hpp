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

#include "axcl_ppl.h"

class ippl {
public:
    virtual ~ippl() = default;

    virtual axclError init() = 0;
    virtual axclError deinit() = 0;

    virtual axclError start() = 0;
    virtual axclError stop() = 0;
    virtual axclError send_stream(const axcl_ppl_input_stream* stream, AX_S32 timeout) = 0;

    virtual axclError get_attr(const char* name, void* attr) = 0;
    virtual axclError set_attr(const char* name, const void* attr) = 0;
};
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

#include "axclite.h"
#include "res_guard.hpp"

namespace axclite {

class AXCL_EXPORT context_guard final {
public:
    explicit context_guard(int32_t device_id)
        : m_context(
              [device_id]() {
                  axclrtContext context;
                  ::axclrtCreateContext(&context, device_id);
                  return context;
              },
              [](axclrtContext& context) {
                  if (context) {
                      ::axclrtDestroyContext(context);
                  }
              }) {
    }

private:
    res_guard<axclrtContext> m_context;
};

}  // namespace axclite
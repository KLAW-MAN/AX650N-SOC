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

#include <functional>
#include <vector>
#include "axclite.h"
#include "axclite_link.hpp"
#include "singleton.hpp"

namespace axclite {

class AXCL_EXPORT msys : public axcl::singleton<msys> {
    friend class axcl::singleton<msys>;

public:
    axclError init(const axclite_msys_attr& attr);
    axclError deinit();

    axclError link(const AX_MOD_INFO_T& src, const AX_MOD_INFO_T& dst);
    axclError unlink(const AX_MOD_INFO_T& src, const AX_MOD_INFO_T& dst);

private:
    msys() noexcept = default;

    axclError set_vdec_clk(AX_VDEC_HWCLK_E clk);

private:
    linker m_link;
    std::vector<std::function<AX_S32(AX_VOID)>> m_clean_funs;
};

}  // namespace axclite

#define MSYS() axclite::msys::get_instance()
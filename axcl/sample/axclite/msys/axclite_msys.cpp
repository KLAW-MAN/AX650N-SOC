/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include "axclite_msys.hpp"

extern "C" axclError axclrtControlExecuteShellCmd(const char* cmd, const char* const args[], size_t argc, const char** output);

namespace axclite {

axclError msys::init(const axclite_msys_attr& attr) {
    axclError ret;
    if (ret = AXCL_SYS_Init(); AXCL_SUCC != ret) {
        return ret;
    } else {
        m_clean_funs.push_back(AXCL_SYS_Deinit);
    }

    if (AXCL_LITE_VDEC == (attr.modules & AXCL_LITE_VDEC) || AXCL_LITE_JDEC == (attr.modules & AXCL_LITE_JDEC)) {
        AX_VDEC_MOD_ATTR_T mod_attr = {};
        mod_attr.u32MaxGroupCount = attr.max_vdec_grp;
        if (AXCL_LITE_VDEC == (attr.modules & AXCL_LITE_VDEC) && AXCL_LITE_JDEC == (attr.modules & AXCL_LITE_JDEC)) {
            mod_attr.enDecModule = AX_ENABLE_BOTH_VDEC_JDEC;
        } else if (AXCL_LITE_VDEC == (attr.modules & AXCL_LITE_VDEC)) {
            mod_attr.enDecModule = AX_ENABLE_ONLY_VDEC;
        } else {
            mod_attr.enDecModule = AX_ENABLE_ONLY_JDEC;
        }

        if (AXCL_LITE_VDEC == (attr.modules & AXCL_LITE_VDEC)) {
            set_vdec_clk(attr.vdec_clk);
        }

        if (ret = AXCL_VDEC_Init(&mod_attr); AXCL_SUCC != ret) {
            deinit();
            return ret;
        }

        m_clean_funs.push_back(AXCL_VDEC_Deinit);
    }

    if (AXCL_LITE_VENC == (attr.modules & AXCL_LITE_VENC) || AXCL_LITE_JENC == (attr.modules & AXCL_LITE_JENC)) {
        AX_VENC_MOD_ATTR_T mod_attr = {};
        mod_attr.stModThdAttr.u32TotalThreadNum = attr.max_venc_thd;
        if (AXCL_LITE_VENC == (attr.modules & AXCL_LITE_VENC) && AXCL_LITE_JENC == (attr.modules & AXCL_LITE_JENC)) {
            mod_attr.enVencType = AX_VENC_MULTI_ENCODER;
        } else if (AXCL_LITE_VENC == (attr.modules & AXCL_LITE_VENC)) {
            mod_attr.enVencType = AX_VENC_VIDEO_ENCODER;
        } else {
            mod_attr.enVencType = AX_VENC_JPEG_ENCODER;
        }

        if (ret = AXCL_VENC_Init(&mod_attr); AXCL_SUCC != ret) {
            deinit();
            return ret;
        }

        m_clean_funs.push_back(AXCL_VENC_Deinit);
    }

    if (AXCL_LITE_IVPS == (attr.modules & AXCL_LITE_IVPS)) {
        if (ret = AXCL_IVPS_Init(); AXCL_SUCC != ret) {
            deinit();
            return ret;
        }

        m_clean_funs.push_back(AXCL_IVPS_Deinit);
    }

    return AXCL_SUCC;
}

axclError msys::deinit() {
    for (auto it = m_clean_funs.rbegin(); it != m_clean_funs.rend(); ++it) {
        (void)(*it)();
    }

    m_clean_funs.clear();
    return AXCL_SUCC;
}

axclError msys::link(const AX_MOD_INFO_T& src, const AX_MOD_INFO_T& dst) {
    return m_link.link(src, dst);
}

axclError msys::unlink(const AX_MOD_INFO_T& src, const AX_MOD_INFO_T& dst) {
    return m_link.unlink(src, dst);
}

axclError msys::set_vdec_clk(AX_VDEC_HWCLK_E clk) {
    AX_U32 hz = 0;
    if (AX_VDEC_HWCLK_500M == clk) {
        hz = 500000000;
    } else if (AX_VDEC_HWCLK_400M == clk) {
        hz = 400000000;
    } else {
        hz = 624000000;
    }

    char cmd[128] = {0};
    sprintf(cmd, "echo %d > /sys/devices/platform/soc/19000000.vdec/devfreq/19000000.vdec/userspace/set_freq", hz);
    return axclrtControlExecuteShellCmd(cmd, nullptr, 0, nullptr);
}

}  // namespace axclite
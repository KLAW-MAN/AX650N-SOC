#include "vdsp.hpp"
#include "ax_dsp_cv_api.h"
#include "ax_kvm_dsp_cv_api.h"
#include "sample_log.h"

bool init_vdsp(const std::string &itcm_path, const std::string &sram_path) {
    int ret;

    for (int i = 0; i < AX_DSP_ID_BUTT; i++) {
        ret = AX_DSP_PowerOn((AX_DSP_ID_E)i);
        if (AX_DSP_SUCCESS != ret) {
            ALOGE("AX_DSP_PowerOn(%d) fail, ret = 0x%x", i, ret);
            return false;
        }

        ret = AX_DSP_LoadBin((AX_DSP_ID_E)i, itcm_path.c_str(), AX_DSP_MEM_TYPE_ITCM);
        if (AX_DSP_SUCCESS != ret) {
            ALOGE("AX_DSP_LoadBin(%d itcm) fail, ret = 0x%x", i, ret);
            return false;
        }

        ret = AX_DSP_LoadBin((AX_DSP_ID_E)i, sram_path.c_str(), AX_DSP_MEM_TYPE_SRAM);
        if (AX_DSP_SUCCESS != ret) {
            ALOGE("AX_DSP_LoadBin(%d sram) fail, ret = 0x%x", i, ret);
            return false;
        }

        ret = AX_DSP_EnableCore((AX_DSP_ID_E)i);
        if (AX_DSP_SUCCESS != ret) {
            ALOGE("AX_DSP_EnableCore(%d) fail, ret = 0x%x", i, ret);
            return false;
        }

        ret = AX_DSP_CV_Init((AX_DSP_ID_E)i);
        if (AX_DSP_SUCCESS != ret) {
            ALOGE("AX_DSP_CV_Init(%d) fail, ret = 0x%x", i, ret);
            return false;
        }
    }

    return true;
}

void deinit_vdsp() {
    int ret;

    for (int i = 0; i < AX_DSP_ID_BUTT; i++) {
        ret = AX_DSP_CV_Exit((AX_DSP_ID_E)i);
        if (AX_DSP_SUCCESS != ret) {
            ALOGE("AX_DSP_CV_Exit(%d) fail, ret = 0x%x", i, ret);
        }

        ret = AX_DSP_DisableCore((AX_DSP_ID_E)i);
        if (AX_DSP_SUCCESS != ret) {
            ALOGE("AX_DSP_DisableCore(%d) fail, ret = 0x%x", i, ret);
        }

        ret = AX_DSP_PowerOff((AX_DSP_ID_E)i);
        if (AX_DSP_SUCCESS != ret) {
            ALOGE("AX_DSP_PowerOff(%d) fail, ret = 0x%x", i, ret);
        }
    }
}

bool split_rgb_to_nv12_by_vdsp(const image_frame &rgb, image_frame nv12[2], uint32_t algo) {
    for (int i = 0; i < 2; i++) {
        nv12[i].width = rgb.width;
        nv12[i].height = rgb.height;
        nv12[i].size = nv12[i].width * nv12[i].height * 3 / 2;
        if (!nv12[i].alloc()) {
            return false;
        }
    }

    AX_DSP_CV_SPLIT_YUV444_PARAM_T param = {};
    param.width = rgb.width;
    param.height = rgb.height;
    param.src_stride = rgb.width * 3;
    param.dst_stride_l = nv12[0].width;
    param.dst_stride_r = nv12[1].width;
    param.phySrc = rgb.phy_addr;
    param.phyDstL = nv12[0].phy_addr;
    param.phyDstR = nv12[1].phy_addr;
    param.para = (0 == algo) ? 0x00 : 0x10;
    ALOGI("split vdsp algo = 0x%x", param.para);
    int ret = AX_DSP_CV_SPLIT_YUV444_2(&param);
    if (AX_DSP_SUCCESS != ret) {
        ALOGE("AX_DSP_CV_SPLIT_YUV444_2 fail, ret = 0x%x", ret);
        for (int i = 0; i < 2; i++) {
            nv12[i].free();
        }

        return false;
    }

    return true;
}

bool merge_nv12_to_rgb_by_vdsp(const image_frame nv12[2], image_frame &rgb, uint32_t algo) {
    rgb.width = nv12[0].width;
    rgb.height = nv12[0].height;
    rgb.size = rgb.width * rgb.height * 3;
    if (!rgb.alloc()) {
        return false;
    }

    AX_DSP_CV_SPLIT_YUV444_PARAM_T param = {};
    param.width = rgb.width;
    param.height = rgb.height;
    param.src_stride = rgb.width * 3;
    param.dst_stride_l = nv12[0].width;
    param.dst_stride_r = nv12[1].width;
    param.phyDstL = nv12[0].phy_addr;
    param.phyDstR = nv12[1].phy_addr;
    param.phySrc = rgb.phy_addr;
    param.para = (0 == algo) ? 0x00 : 0x10;
    ALOGI("merge vdsp algo = 0x%x", param.para);
    int ret = AX_DSP_CV_COMBINE_YUV444_2(&param);
    if (AX_DSP_SUCCESS != ret) {
        ALOGE("AX_DSP_CV_COMBINE_YUV444_2 fail, ret = 0x%x", ret);
        rgb.free();
        return false;
    }

    return true;
}
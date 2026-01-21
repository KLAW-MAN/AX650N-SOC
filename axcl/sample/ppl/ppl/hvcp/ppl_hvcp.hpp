/**************************************************************************************************
 *
 * Copyright (c) 2019-2025 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#pragma once

#include <atomic>
#include <memory>
#include "axclite_ivps.hpp"
#include "axclite_vdec.hpp"
#include "hvcp.hpp"
#include "ippl.hpp"

/**
 * PPL: AXCL_PPL_HVCP
 *      link       unlink
 * VDEC ----> IVPS ----> NPU(HVCP)
 */
class ppl_hvcp : public ippl {
public:
    ppl_hvcp(int32_t ppl_id, int32_t device_id, const axcl_ppl_hvcp_param& param);

    axclError init() override;
    axclError deinit() override;

    axclError start() override;
    axclError stop() override;
    axclError send_stream(const axcl_ppl_input_stream* stream, AX_S32 timeout) override;

    axclError get_attr(const char* name, void* attr) override;
    axclError set_attr(const char* name, const void* attr) override;

protected:
    axclite_vdec_attr get_vdec_attr();
    axclite_ivps_attr get_ivps_attr();

private:
    int32_t m_ppl_id;
    int32_t m_device_id;

    axcl_ppl_hvcp_param m_ppl_param;
    axclite::hvcp_input_image_info m_model_input_image_info;

    std::unique_ptr<axclite::vdec> m_vdec;
    std::unique_ptr<axclite::ivps> m_ivps;
    std::unique_ptr<axclite::hvcp> m_hvcp;

    uint32_t m_vdec_blk_cnt = 8;
    uint32_t m_ivps_blk_cnt = 5;

    uint32_t m_ivps_engine = AX_IVPS_ENGINE_TDP;

    std::atomic<bool> m_started = {false};
};
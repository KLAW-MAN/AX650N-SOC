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
#include <memory>
#include "axcl_ppl_transcode_type.h"
#include "axclite_ivps.hpp"
#include "axclite_vdec.hpp"
#include "axclite_venc.hpp"
#include "axclite_venc_sink.hpp"
#include "ippl.hpp"

/**
 * PPL: AXCL_PPL_TRANSCODE
 *      link       link
 * VDEC ----> IVPS ----> VENC
 */
class ppl_transcode : public ippl {
public:
    ppl_transcode(int32_t id, int32_t device, const axcl_ppl_transcode_param& param);

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
    axclite_venc_attr get_venc_attr();

private:
    int32_t m_device;
    int32_t m_id;
    axcl_ppl_transcode_param m_param;
    std::unique_ptr<axclite::vdec> m_vdec;
    std::unique_ptr<axclite::venc> m_venc;
    std::unique_ptr<axclite::ivps> m_ivps;
    axclite::venc_sinker m_sink;
    std::atomic<bool> m_started = {false};

    uint32_t m_vdec_blk_cnt = 8;
    uint32_t m_vdec_out_fifo_depth = 4;
    uint32_t m_ivps_in_fifo_depth = 4;
    uint32_t m_ivps_out_fifo_depth = 0;
    uint32_t m_ivps_blk_cnt = 5;
    uint32_t m_ivps_engine = AX_IVPS_ENGINE_TDP;
    uint32_t m_venc_in_fifo_depth = 4;
    uint32_t m_venc_out_fifo_depth = 4;
    int32_t m_venc_stop_wait_time = 0;
};
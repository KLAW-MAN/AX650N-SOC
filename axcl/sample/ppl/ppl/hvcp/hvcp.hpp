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

#include "axcl_ppl_hvcp_type.h"
#include "axclite.h"
#include "axclite_sink.hpp"
#include "lock_queue.hpp"
#include "threadx.hpp"

#include <any>

namespace axclite {
typedef struct {
    uint32_t width;
    uint32_t height;
    AX_IMG_FORMAT_E img_fmt;
} hvcp_input_image_info;

typedef struct {
    uint64_t handle;
    uint64_t context;
    void* info;
    void* io;
    std::vector<void*> buffer;
} hvcp_engine_param;

class hvcp final : public sinker {
public:
    hvcp(axcl_ppl ppl, int32_t device_id, const axcl_hvcp_param& param);

    axclError init();
    axclError deinit();

    axclError start();
    axclError stop();

    /**
     * Receive frame from previous module eg： VDEC or IVPS.
     * Put frame into fifo.
     */
    int32_t recv_frame(const axclite_frame &frame) override;
    const hvcp_input_image_info &get_input_image_info() const;

protected:
    bool skip(const axclite_frame &frame);
    void run();
    void notify_output(const axcl_ppl_hvcp_output &output);

    bool send_frame(const axclite_frame &frame);
    bool release_frame(const axclite_frame &frame);

private:
    axcl_ppl m_ppl;
    int32_t m_device_id;
    axcl_hvcp_param m_param;
    hvcp_engine_param m_engine_param;
    axcl::lock_queue<axclite_frame> m_frames;
    axcl::threadx m_thread;
    hvcp_input_image_info m_image_info;
};

}  // namespace axclite

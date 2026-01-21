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
#include <condition_variable>
#include <functional>
#include <mutex>
#include "AXThread.hpp"
#include "ax_vo_api.h"

using HDMI_EVENT_CALLBACK = std::function<AX_BOOL(AX_HDMI_EVENT_TYPE_E enType)>;

class CHdmiEvent {
public:
    CHdmiEvent(AX_VOID) = default;

    AX_BOOL CheckPlugged(AX_HDMI_ID_E enHdmi);

    AX_BOOL Start(AX_HDMI_ID_E enHdmi, HDMI_EVENT_CALLBACK cb);
    AX_BOOL Stop(AX_VOID);
    AX_BOOL IsPlugged(AX_VOID);

    AX_VOID SetEvent(AX_HDMI_EVENT_TYPE_E enType);

protected:
    AX_VOID ListenThread(AX_VOID *);

protected:
    AX_HDMI_ID_E m_enHdmi = {AX_HDMI_ID0};
    HDMI_EVENT_CALLBACK m_cb = {nullptr};
    volatile AX_BOOL m_bPred = {AX_FALSE};
    std::mutex m_mtx;
    std::condition_variable m_cv;
    AX_HDMI_EVENT_TYPE_E m_evPlug = {AX_HDMI_EVENT_NO_PLUG};
    CAXThread m_thread;
};
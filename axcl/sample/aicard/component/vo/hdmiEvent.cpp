/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include "hdmiEvent.hpp"
#include "AppLogApi.h"

#define TAG "HDMI"

static AX_VOID HDMIPlugEventCallback(AX_HDMI_EVENT_TYPE_E enEvent, AX_VOID *pPrivateData) {
    CHdmiEvent *pThis = (CHdmiEvent *)pPrivateData;
    pThis->SetEvent(enEvent);
}

AX_VOID CHdmiEvent::ListenThread(AX_VOID *) {
    LOG_MM_I(TAG, "hdmi %d +++", m_enHdmi);

    while (1) {
        // {
        std::unique_lock<std::mutex> lck(m_mtx);
        while (!m_bPred) {
            m_cv.wait(lck);
        }
        // }

        m_bPred = AX_FALSE;

        if (!m_thread.IsRunning()) {
            break;
        }

        if (m_cb) {
            (AX_VOID) m_cb(m_evPlug);
        }
#if 0
        /*
            When plug in HDMI cable slowly, several plug in and out events will be received, such as:
                plug in
                plug out
                plug in
                plug out
                plug in
            We wait 1 sec to assume the event is stable.
        */
        std::this_thread::sleep_for(std::chrono::seconds(1));

        std::lock_guard<std::mutex> lck(m_mtx);
        m_bPred = AX_FALSE; /* ignore later event */
        if (m_cb) {
            (AX_VOID) m_cb(m_evPlug);
        }
#endif
    }

    LOG_MM_I(TAG, "hdmi %d ---", m_enHdmi);
}

AX_VOID CHdmiEvent::SetEvent(AX_HDMI_EVENT_TYPE_E enType) {
    LOG_MM_I(TAG, "hdmi %d %s +++", m_enHdmi, (AX_HDMI_EVENT_HOTPLUG == enType) ? "in" : "out");

    m_mtx.lock();
    m_evPlug = enType;
    m_bPred = AX_TRUE;
    m_mtx.unlock();

    m_cv.notify_one();

    LOG_MM_I(TAG, "hdmi %d %s ---", m_enHdmi, (AX_HDMI_EVENT_HOTPLUG == enType) ? "in" : "out");
}

AX_BOOL CHdmiEvent::Start(AX_HDMI_ID_E enHdmi, HDMI_EVENT_CALLBACK cb) {
    m_enHdmi = enHdmi;
    m_cb = cb;

    AX_CHAR name[16];
    sprintf(name, "Hdmi%dEvent", m_enHdmi);
    if (!m_thread.Start([this](AX_VOID *pArg) -> AX_VOID { ListenThread(pArg); }, this, name)) {
        LOG_MM_E(TAG, "start %s thread fail", name);
        return AX_FALSE;
    }

    AX_HDMI_CALLBACK_FUNC_T _cb;
    memset(&_cb, 0, sizeof(_cb));
    _cb.pfnHdmiEventCallback = HDMIPlugEventCallback;
    _cb.pPrivateData = this;
    AX_S32 ret = AX_VO_HDMI_RegCallbackFunc(m_enHdmi, &_cb);
    if (0 != ret) {
        LOG_MM_E(TAG, "AX_VO_HDMI_RegCallbackFunc(hdmi %d) fail, ret = 0x%x", m_enHdmi, ret);
        return AX_FALSE;
    }

    return AX_TRUE;
}

AX_BOOL CHdmiEvent::Stop(AX_VOID) {
    m_thread.Stop();

    /* wakeup thread */
    m_mtx.lock();
    m_bPred = AX_TRUE;
    m_mtx.unlock();
    m_cv.notify_one();

    m_thread.Join();

    AX_HDMI_CALLBACK_FUNC_T _cb;
    memset(&_cb, 0, sizeof(_cb));
    _cb.pfnHdmiEventCallback = HDMIPlugEventCallback;
    _cb.pPrivateData = this;
    AX_S32 ret = AX_VO_HDMI_UnRegCallbackFunc(m_enHdmi, &_cb);
    if (0 != ret) {
        LOG_MM_E(TAG, "AX_VO_HDMI_RegCallbackFunc(hdmi %d) fail, ret = 0x%x", m_enHdmi, ret);
        return AX_FALSE;
    }

    return AX_TRUE;
}

AX_BOOL CHdmiEvent::CheckPlugged(AX_HDMI_ID_E enHdmi) {
    AX_HDMI_EDID_T edid;
    if (0 == AX_VO_HDMI_Force_GetEDID(enHdmi, &edid)) {
        std::lock_guard<std::mutex> lck(m_mtx);
        m_evPlug = AX_HDMI_EVENT_HOTPLUG;
        return AX_TRUE;
    } else {
        std::lock_guard<std::mutex> lck(m_mtx);
        m_evPlug = AX_HDMI_EVENT_NO_PLUG;
        return AX_FALSE;
    }
}

AX_BOOL CHdmiEvent::IsPlugged(AX_VOID) {
    std::lock_guard<std::mutex> lck(m_mtx);
    return (AX_HDMI_EVENT_HOTPLUG == m_evPlug) ? AX_TRUE : AX_FALSE;
}
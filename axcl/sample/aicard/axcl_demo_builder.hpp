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

#include <memory>
#include <string>
#include <vector>

#include "axcl_demo_app_sys.hpp"
#include "axcl_demo_config.hpp"

#include "IObserver.h"
#include "dispatcher.hpp"
#include "IStreamHandler.hpp"
#include "transfer_helper.hpp"
#include "AxVideoDecoder.hpp"
#include "AxclVideoDecoder.hpp"
#include "AxclIvps.hpp"
#include "Vo.hpp"
#include "detector.hpp"
#include "detect_observer.hpp"
#include "CaptureObserver.hpp"

using namespace axcl_demo;

class CAxclDemoBuilder final {
public:
    CAxclDemoBuilder(AX_VOID) noexcept = default;

    AX_BOOL Start(const std::string &strJson);
    AX_BOOL Stop(AX_VOID);

    AX_BOOL QueryStreamsAllEof(AX_VOID);

protected:
    AX_BOOL Init(const std::string &strJson);
    AX_BOOL DeInit(AX_VOID);

    AX_BOOL InitHost(CAxclDemoConfig *pConfig);
    AX_BOOL InitDevice(CAxclDemoConfig *pConfig);

    AX_BOOL InitStreamer(const STREAM_CONFIG_T& streamConfig);

    /* host */
    AX_BOOL InitHostDisplay(const DISPVO_CONFIG_T& dispVoConfig);
    AX_BOOL InitHostDispatcher(const std::string& strFontPath);
    AX_BOOL InitHostTransHelper(AX_VOID);
    AX_BOOL InitHostDecoder(const STREAM_CONFIG_T& streamConfig);
    AX_BOOL StartHost(AX_VOID);
    AX_VOID StopHost(AX_VOID);

    /* device */
    AX_BOOL InitDeviceIvps(const IVPS_CONFIG_T &tIvpsConfig);
    AX_BOOL InitDeviceDecoder(const VDEC_CONFIG_T& tVdecConfig);
    AX_BOOL InitDeviceDetector(const DETECT_CONFIG_T &tDetectConfig);
    AX_BOOL InitDeviceCapture(const CAPTURE_CONFIG_T &tCaptureConfig);
    AX_BOOL StartDevice(AX_VOID);
    AX_VOID StopDevice(AX_VOID);

protected:
    AX_S32 m_nDeviceID{0};
    AX_U32 m_nDecodeGrpCountHst{0};
    AX_U32 m_nDecodeGrpCountDev{0};
    AX_BOOL m_bEnableDetect{AX_FALSE};
    AX_BOOL m_bEnableCapture{AX_FALSE};

    CAxclDemoAppSys m_sys;
    std::vector<IStreamerHandlerPtr> m_arrStreamer;

    /* host */
    std::unique_ptr<CAxVideoDecoder> m_vdecHost;
    std::shared_ptr<CVo> m_disp;
    std::unique_ptr<CTransferHelper> m_transHelper;
    std::vector<std::unique_ptr<CDispatcher>> m_arrDispatcher;
    IObserverUniquePtr m_dispObserver;
    std::vector<IObserverUniquePtr> m_arrDispatchObserver;

    /* device */
    std::unique_ptr<CAxclVideoDecoder> m_vdecDevice;
    std::unique_ptr<CDetector> m_detector;
    std::unique_ptr<CCapture> m_capture;
    IObserverUniquePtr m_detectorObserver;
    IObserverUniquePtr m_captureObserver;
    std::vector<CIVPS*> m_vecIvps;
};

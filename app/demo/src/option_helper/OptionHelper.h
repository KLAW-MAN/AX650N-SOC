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

#include <string>
#include "AXSingleton.h"
#include "IniWrapper.hpp"

/**
 * Load configuration
 */
class COptionHelper final : public CAXSingleton<COptionHelper> {
    friend class CAXSingleton<COptionHelper>;

public:
    AX_U8 GetRotation();
    AX_F32 GetVencOutBuffRatio();
    AX_U8 GetVencOutBuffCount();
    AX_F32 GetJencOutBuffRatio();
    AX_U8 GetJencOutBuffCount();
    AX_U32 GetAencOutFrmSize();
    AX_U8 GetAencOutFrmCount();
    AX_U32 GetRTSPMaxFrmSize();
    AX_U8 GetRTSPMaxFrmCount();
    AX_S32 GetNpuMode();
    AX_BOOL IsEnableOSD();
    AX_BOOL IsEnableVideoRecord();
    AX_BOOL IsVideoRecordLoop();
    std::string GetVideoSavedPath();
    AX_U32 GetVideoMaxRecordNum();
    AX_U32 GetVideoMaxRecordSize();
    /* SLT functions */
    AX_U32 GetSLTRunTime();
    AX_U32 GetSLTFpsCheckFreq();
    AX_U32 GetSLTFpsDiff();

    AX_U32 GetSetVencThreadNum();

    AX_BOOL IsEnableWebServerStatusCheck();

    std::string GetTriggerPicPath(AX_VOID);
    AX_BOOL ISEnableTriggerPicStore(AX_VOID);

    AX_BOOL IsEnableAFTest();
private:
    COptionHelper(AX_VOID) = default;
    ~COptionHelper(AX_VOID) = default;

    AX_BOOL InitOnce() override;

    std::string GetValue(const string strKey);

private:
    CIniWrapper m_iniWrapper;
};

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
#include <vector>
#include "picojson.h"

#include "AXSingleton.h"
#include "AXNVRFrameworkDefine.h"


class CRemoteDeviceParser : public CAXSingleton<CRemoteDeviceParser> {
    friend class CAXSingleton<CRemoteDeviceParser>;

public:
    std::vector<AX_NVR_DEV_INFO_T> GetRemoteDeviceMap(AX_U32 *nRemoteDeviceCnt, const std::string &strPath);
    AX_BOOL SetRemoteDeviceMap(std::vector<AX_NVR_DEV_INFO_T>& vecRemoteDevice);

private:
    AX_BOOL InitOnce() override;

private:
    std::string GetExecPath(AX_VOID);
    std::string m_strPath;

private:
    CRemoteDeviceParser(AX_VOID) = default;
    ~CRemoteDeviceParser(AX_VOID) = default;
};

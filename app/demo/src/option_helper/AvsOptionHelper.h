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
#include "AvsCfgParser.h"

#define APP_AVS_CONFIG(nScenario, stOutAvsCfg)   CAvsOptionHelper::GetInstance()->GetAvsCfg(nScenario, stOutAvsCfg)

/**
 * Load configuration
 */
class CAvsOptionHelper final : public CAXSingleton<CAvsOptionHelper> {
    friend class CAXSingleton<CAvsOptionHelper>;

public:
    AX_BOOL GetAvsCfg(AX_U8 nScenario, AX_APP_AVS_CFG_T& stOutAvsCfg);

private:
    CAvsOptionHelper(AX_VOID) = default;
    ~CAvsOptionHelper(AX_VOID) = default;

    AX_BOOL InitOnce() override;

private:
    std::map<AX_U8, AX_APP_AVS_CFG_T> mapAvsCfg;
};

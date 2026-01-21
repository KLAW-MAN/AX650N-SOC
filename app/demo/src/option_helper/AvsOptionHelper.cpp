/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include "AvsCfgParser.h"
#include "AvsOptionHelper.h"
#include "AppLogApi.h"

#define AVSOPTHELPER "AvsOptionHelper"

AX_BOOL CAvsOptionHelper::InitOnce() {
    return CAvsCfgParser::GetInstance()->GetConfig(mapAvsCfg);
}

AX_BOOL CAvsOptionHelper::GetAvsCfg(AX_U8 nScenario, AX_APP_AVS_CFG_T& stOutAvsCfg) {
    std::map<AX_U8, AX_APP_AVS_CFG_T>::iterator itScenario = mapAvsCfg.find(nScenario);
    if (mapAvsCfg.end() == itScenario) {
        LOG_MM_E(AVSOPTHELPER, "Scenario %d not found for avs.", nScenario);
        return AX_FALSE;
    }

    stOutAvsCfg = itScenario->second;

    return AX_TRUE;
}

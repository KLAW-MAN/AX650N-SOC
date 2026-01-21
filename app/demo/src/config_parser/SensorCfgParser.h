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

#include "picojson.h"
#include "AXSingleton.h"
#include "ISensor.hpp"

class CSensorCfgParser : public CAXSingleton<CSensorCfgParser> {
    friend class CAXSingleton<CSensorCfgParser>;

public:
    AX_BOOL GetConfig(std::map<AX_U8, std::map<AX_U8, SENSOR_CONFIG_T>>& mapSensorCfg,
                        AX_S32& nCurrScenario, AX_U32& nSensorCount);

private:
    CSensorCfgParser(AX_VOID) = default;
    ~CSensorCfgParser(AX_VOID) = default;

    AX_BOOL InitOnce() override;

    AX_BOOL ParseFile(const std::string& strPath,
                        std::map<AX_U8, std::map<AX_U8, SENSOR_CONFIG_T>>& mapSensorCfg);
    AX_BOOL ParseJson(picojson::object& objJsonRoot,
                        std::map<AX_U8, SENSOR_CONFIG_T>& mapBaseDev2SnsSetting,
                        std::map<AX_U8, std::map<AX_U8, SENSOR_CONFIG_T>>& mapSensorCfg);
    std::string LoadType2FileName(AX_S32 nLoadType);

    AX_BOOL SetSnsSettings(picojson::array& arrSettings, std::map<AX_U8, SENSOR_CONFIG_T>& mapDev2SnsSetting);

    std::string GetBaseJsonFile(const std::string& strJsonFullPath);

    std::map<AX_U8, SENSOR_CONFIG_T> GetBaseSnsSetting(const std::string& strJsonFullPath);
};

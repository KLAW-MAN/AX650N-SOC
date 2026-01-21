/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include <fstream>
#include "AvsCfgParser.h"
#include "AppLogApi.h"
#include "CommonUtils.hpp"

using namespace std;

#define AVS_PARSER "AVS_PARSER"

AX_BOOL CAvsCfgParser::InitOnce() {
    return AX_TRUE;
}

AX_BOOL CAvsCfgParser::GetConfig(std::map<AX_U8, AX_APP_AVS_CFG_T>& mapAvsCfg) {
    string strConfigDir = CCommonUtils::GetPPLConfigDir();
    if (strConfigDir.empty()) {
        return AX_FALSE;
    }

    string strAVSCfgFile = strConfigDir + "/avs.json";

    ifstream ifs(strAVSCfgFile.c_str());
    picojson::value v;
    ifs >> v;

    string err = picojson::get_last_error();
    if (!err.empty()) {
        LOG_M_E(AVS_PARSER, "Failed to load json config file: %s, with err: %s", strAVSCfgFile.c_str(), err.c_str());
        return AX_FALSE;
    }

    if (!v.is<picojson::object>()) {
        LOG_M_E(AVS_PARSER, "Loaded config file is not a well-formatted JSON.");
        return AX_FALSE;
    }

    if (!ParseFile(strAVSCfgFile, mapAvsCfg)) {
        return AX_FALSE;
    }

    return AX_TRUE;
}

AX_BOOL CAvsCfgParser::ParseFile(const string& strPath, std::map<AX_U8, AX_APP_AVS_CFG_T>& mapAvsCfg) {
    picojson::value v;
    ifstream fIn(strPath.c_str());
    if (!fIn.is_open()) {
        return AX_TRUE;
    }

    string strParseRet = picojson::parse(v, fIn);
    if (!strParseRet.empty() || !v.is<picojson::object>()) {
        return AX_TRUE;
    }

    return ParseJson(v.get<picojson::object>(), mapAvsCfg);
}

AX_BOOL CAvsCfgParser::ParseJson(picojson::object& objJsonRoot, std::map<AX_U8, AX_APP_AVS_CFG_T>& mapAvsCfg) {
    if (objJsonRoot.end() == objJsonRoot.find("avs_default_settings")) {
        return AX_FALSE;
    }

    AX_APP_AVS_CFG_T stAvsBaseCfg;
    picojson::object objBaseSettings = objJsonRoot["avs_default_settings"].get<picojson::object>();
    if (ParseJson(objBaseSettings, stAvsBaseCfg, AX_TRUE)) {
        if (objJsonRoot.end() == objJsonRoot.find("avs_settings")) {
            LOG_MM_E(AVS_PARSER, "Root miss 'avs_settings' config!");
            return AX_FALSE;
        }

        picojson::object objSettings = objJsonRoot["avs_settings"].get<picojson::object>();
        AX_APP_AVS_CFG_T stAvsCfg;
        for (AX_U32 i = 0; i < E_IPC_SCENARIO_MAX; i++) {
            string strScenario = CCmdLineParser::ScenarioEnum2Str((AX_IPC_SCENARIO_E)i);
            if (objSettings.end() == objSettings.find(strScenario.c_str())) {
                continue;
            }

            picojson::object &objSetting = objSettings[strScenario.c_str()].get<picojson::object>();
            stAvsCfg = stAvsBaseCfg;
            ParseJson(objSetting, stAvsCfg, AX_FALSE);

            mapAvsCfg.insert(make_pair(i, stAvsCfg));
        }
    }

   return AX_TRUE;
}

AX_BOOL CAvsCfgParser::ParseJson(picojson::object& objSetting, AX_APP_AVS_CFG_T& stAvsCfg, AX_BOOL bDefaultSetting) {
    /* pipe num */
    JSON_PARSE_DIGITAL_ITEM(AVS_PARSER, stAvsCfg.u8PipeNum, AX_U8, objSetting, "pipe_num", bDefaultSetting);

    /* surround view on flag */
    JSON_PARSE_BOOL_ITEM(AVS_PARSER, stAvsCfg.bAroundView, objSetting, "around_view_on", AX_FALSE);

    /* sync pipe flag */
    JSON_PARSE_BOOL_ITEM(AVS_PARSER, stAvsCfg.bSyncPipe, objSetting, "sync_pipe", bDefaultSetting);

    /* avs mode */
    JSON_PARSE_DIGITAL_ITEM(AVS_PARSER, stAvsCfg.u8Mode, AX_U8, objSetting, "mode", bDefaultSetting);

    /* avs engine mode */
    JSON_PARSE_DIGITAL_ITEM(AVS_PARSER, stAvsCfg.eEngineMode, AX_AVS_ENGINE_MODE_E, objSetting, "engine_mode", bDefaultSetting);

    /* dynamic seam flag */
    JSON_PARSE_BOOL_ITEM(AVS_PARSER, stAvsCfg.bDynamicSeam, objSetting, "dynamic_seam", bDefaultSetting);

    /* blend mode */
    JSON_PARSE_DIGITAL_ITEM(AVS_PARSER, stAvsCfg.u8BlendMode, AX_U8, objSetting, "blend_mode", bDefaultSetting);

    /* parameter file path */
    JSON_PARSE_STRING_ITEM(AVS_PARSER, stAvsCfg.strParamFilePath, objSetting, "param_file_path", bDefaultSetting);

    /* parameter type */
    JSON_PARSE_DIGITAL_ITEM(AVS_PARSER, stAvsCfg.u8ParamType, AX_U8, objSetting, "param_type", bDefaultSetting);

    /* prioritize handle onsite cali data */
    JSON_PARSE_BOOL_ITEM(AVS_PARSER, stAvsCfg.bPrioOnsiteCali, objSetting, "prioritize_onsitecali", AX_FALSE);

    /* projection type */
    if (objSetting.end() != objSetting.find("projection_type")) {
        AX_U8 nProjType = 0;
        nProjType = (AX_U8)objSetting["projection_type"].get<double>();

        stAvsCfg.eProjectionMode = GetAvsProjectionMode(nProjType);
    } else if (bDefaultSetting) {
        LOG_MM_E(AVS_PARSER, "'avs_default_settings' miss 'projection_type' config!");
        return AX_FALSE;
    }

    /* compress info */
    if (objSetting.end() != objSetting.find("avs_compress")) {
        stAvsCfg.stAvsCompress.enCompressMode =
            (AX_COMPRESS_MODE_E)objSetting["avs_compress"].get<picojson::array>()[0].get<double>();
        stAvsCfg.stAvsCompress.u32CompressLevel =
            objSetting["avs_compress"].get<picojson::array>()[1].get<double>();
    } else if (bDefaultSetting) {
        LOG_MM_E(AVS_PARSER, "'avs_default_settings' miss 'avs_compress' config!");
        return AX_FALSE;
    }

    /* calibration enable flag */
    JSON_PARSE_DIGITAL_ITEM(AVS_PARSER, stAvsCfg.u8CaliEnable, AX_U8, objSetting, "cali_enable", bDefaultSetting);

    /* calibration pc server ip address */
    JSON_PARSE_STRING_ITEM(AVS_PARSER, stAvsCfg.strCaliServerIP, objSetting, "cali_server_ip", bDefaultSetting);

    /* calibration pc server port */
    JSON_PARSE_DIGITAL_ITEM(AVS_PARSER, stAvsCfg.u16CaliServerPort, AX_U16, objSetting, "cali_server_port", bDefaultSetting);

    return AX_TRUE;
}

AX_AVS_PROJECTION_MODE_E CAvsCfgParser::GetAvsProjectionMode(AX_U8 nProjType) {
    AX_AVS_PROJECTION_MODE_E eAvsPrjMode = AVS_PROJECTION_CYLINDRICAL;
    switch (nProjType) {
        case 0:
            eAvsPrjMode = AVS_PROJECTION_EQUIRECTANGULER;
            break;
        case 1:
            eAvsPrjMode = AVS_PROJECTION_RECTLINEAR;
            break;
        case 2:
            eAvsPrjMode = AVS_PROJECTION_CYLINDRICAL;
            break;
        case 3:
            eAvsPrjMode = AVS_PROJECTION_CUBE_MAP;
            break;
        default:
            LOG_M_W(AVS_PARSER, "Invalid projection type(%d) set, set AVS_PROJECTION_CYLINDRICAL by default.", nProjType);
            break;
    }

    return eAvsPrjMode;
}

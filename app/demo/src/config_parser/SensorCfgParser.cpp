/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include "SensorCfgParser.h"
#include <fstream>
#include "AXStringHelper.hpp"
#include "AppLogApi.h"
#include "CommonUtils.hpp"
#include "SysUtils.hpp"

using namespace std;

#define SNS_PARSER "SNS_PARSER"
#define JSON_POSTFIX ".json"
#define JSON_BASE_TAG "_base"

AX_BOOL CSensorCfgParser::InitOnce() {
    return AX_TRUE;
}

AX_BOOL CSensorCfgParser::GetConfig(std::map<AX_U8, std::map<AX_U8, SENSOR_CONFIG_T>>& mapSensorCfg,
                                    AX_S32& nCurrScenario, AX_U32& nSensorCount) {
    string strConfigDir = CCommonUtils::GetPPLConfigDir();
    if (strConfigDir.empty()) {
        return AX_FALSE;
    }

    AX_S32 nLoadType = E_LOAD_TYPE_MAX;
    if (!CCmdLineParser::GetInstance()->GetLoadType(nLoadType)) {
        return AX_FALSE;
    }

    AX_CHAR szBoardID[16] = {0};
    if (!CSysUtils::GetBoardID(szBoardID, 16)) {
        return AX_FALSE;
    }

    string strBoardDir = "AX650A";
    if (strncmp(szBoardID, "AX650N_Demo", sizeof("AX650N_Demo") - 1) == 0 ||
        strncmp(szBoardID, "AX650N_EVB", sizeof("AX650N_EVB") - 1) == 0) {
        strBoardDir = "AX650N";
    } else if (strncmp(szBoardID, "AX650C_Demo", sizeof("AX650C_Demo") - 1) == 0 ||
               strncmp(szBoardID, "AX650C_EVB", sizeof("AX650C_EVB") - 1) == 0) {
        LOG_M_E(SNS_PARSER, "Current board: %s, not support", szBoardID);
        return AX_FALSE;
    }

    string strFileName = LoadType2FileName(nLoadType);
    string strFilePath = CAXStringHelper::Format("%s/sensor/%s/%s", strConfigDir.c_str(), strBoardDir.c_str(), strFileName.c_str());
    LOG_MM_D(SNS_PARSER, "strFilePath:%s", strFilePath.c_str());
    ifstream ifs(strFilePath.c_str());
    if (!ifs.good()) {
        LOG_M_E(SNS_PARSER, "Sensor config file: %s parse failed.", strFilePath.c_str());
        return AX_FALSE;
    }

    picojson::value v;
    ifs >> v;

    string err = picojson::get_last_error();
    if (!err.empty()) {
        LOG_M_E(SNS_PARSER, "Failed to load json config file: %s", strFilePath.c_str());
        return AX_FALSE;
    }

    if (!v.is<picojson::object>()) {
        LOG_M_E(SNS_PARSER, "Loaded config file is not a well-formatted JSON.");
        return AX_FALSE;
    }

    if (!ParseFile(strFilePath, mapSensorCfg)) {
        return AX_FALSE;
    } else {
        LOG_M(SNS_PARSER, "Parse sensor config file: %s successfully.", strFilePath.c_str());
    }
    /* Default scenario*/
    AX_S32 nCfgScenario = (E_LOAD_TYPE_SINGLE_OS08A20 == nLoadType ? E_PPL_SCENRIO_2 : E_PPL_SCENRIO_0);
    if (!CCmdLineParser::GetInstance()->GetScenario(nCfgScenario)) {
        LOG_MM(SNS_PARSER, "Apply default scenario %d.", nCfgScenario);
    }

    LOG_M(SNS_PARSER, "Load sensor config for scenario %d", nCfgScenario);

    std::map<AX_U8, SENSOR_CONFIG_T>* pMapScenario = &mapSensorCfg[nCfgScenario];

    if (nullptr == pMapScenario) {
        LOG_M_E(SNS_PARSER, "Can not get sensor config for scenario %d", nCfgScenario);
        return AX_FALSE;
    }

    nCurrScenario = nCfgScenario;
    nSensorCount = pMapScenario->size();

    return AX_TRUE;
}

AX_BOOL CSensorCfgParser::ParseFile(const string& strPath,
                                        std::map<AX_U8, std::map<AX_U8, SENSOR_CONFIG_T>>& mapSensorCfg) {
    picojson::value v;
    ifstream fIn(strPath.c_str());
    if (!fIn.is_open()) {
        return AX_FALSE;
    }

    string strParseErr = picojson::parse(v, fIn);
    if (!strParseErr.empty() || !v.is<picojson::object>()) {
        LOG_MM_E(SNS_PARSER, "Parse err: %s, is picojson::object: %d", strParseErr.c_str(), v.is<picojson::object>());
        return AX_FALSE;
    }

    std::map<AX_U8, SENSOR_CONFIG_T> mapDev2SnsSetting = GetBaseSnsSetting(strPath);

    return ParseJson(v.get<picojson::object>(), mapDev2SnsSetting, mapSensorCfg);
}

AX_BOOL CSensorCfgParser::ParseJson(picojson::object& objJsonRoot,
                                    std::map<AX_U8, SENSOR_CONFIG_T>& mapBaseDev2SnsSetting,
                                    std::map<AX_U8, std::map<AX_U8, SENSOR_CONFIG_T>>& mapSensorCfg) {
    for (AX_U32 nScenario = 0; nScenario < E_IPC_SCENARIO_MAX; nScenario++) {
        string strScenario = CCmdLineParser::ScenarioEnum2Str((AX_IPC_SCENARIO_E)nScenario);
        if (objJsonRoot.end() == objJsonRoot.find(strScenario.c_str())) {
            continue;
        }

        picojson::array& arrSettings = objJsonRoot[strScenario.c_str()].get<picojson::array>();
        if (0 == arrSettings.size()) {
            continue;
        }

        std::map<AX_U8, SENSOR_CONFIG_T> mapDev2SnsSetting = mapBaseDev2SnsSetting;
        if (!SetSnsSettings(arrSettings, mapDev2SnsSetting)) {
            LOG_MM_E(SNS_PARSER, "Load sensor config for scenario(%d) failed!", nScenario);
            return AX_FALSE;
        }

        mapSensorCfg.insert(make_pair(nScenario, mapDev2SnsSetting));
    }

    return mapSensorCfg.size() > 0 ? AX_TRUE : AX_FALSE;
}

string CSensorCfgParser::LoadType2FileName(AX_S32 nLoadType) {
    AX_IPC_LOAD_TYPE_E eLoadType = (AX_IPC_LOAD_TYPE_E)nLoadType;
    switch (eLoadType) {
        case E_LOAD_TYPE_DUAL_OS08A20: {
            return "Dual_OS08A20.json";
        }
        case E_LOAD_TYPE_SINGLE_OS08A20: {
            return "Single_OS08A20.json";
        }
        case E_LOAD_TYPE_DUAL_OS08B10: {
            return "Dual_OS08B10.json";
        }
        case E_LOAD_TYPE_SINGLE_OS08B10: {
            return "Single_OS08B10.json";
        }
        case E_LOAD_TYPE_SINGLE_SC910GS: {
            return "Single_SC910gs.json";
        }
        case E_LOAD_TYPE_PANO_OS04A10x2: {
            return "Pano_Dual_OS04A10.json";
        }
        case E_LOAD_TYPE_SINGLE_OS08A20_PANO_DUAL_OS04A10: {
            return "Single_OS08A20_Dual_OS04A10.json";
        }
        case E_LOAD_TYPE_SINGLE_OS04A10: {
            return "Single_OS04A10.json";
        }
        case E_LOAD_TYPE_QUAD_OS04A10: {
            return "Quad_OS04A10.json";
        }
        case E_LOAD_TYPE_DUMMY_QUAD_OS04A10: {
            return "Dummy_Quad_OS04A10.json";
        }
        case E_LOAD_TYPE_DUMMY_SIX_OS04A10: {
            return "Dummy_Six_OS04A10.json";
        }
        case E_LOAD_TYPE_DUMMY_EIGHT_OS04A10: {
            return "Dummy_Eight_OS04A10.json";
        }
        case E_LOAD_TYPE_DUMMY_QUAD_OS08A20: {
            return "Dummy_Quad_OS08A20.json";
        }
        case E_LOAD_TYPE_SINGLE_SC410GS: {
            return "Single_SC410gs.json";
        }
        case E_LOAD_TYPE_QUAD_OS08A20: {
            return "Quad_OS08A20.json";
        }
        case E_LOAD_TYPE_QUAD_SC500AI: {
            return "Quad_SC500AI.json";
        }
        case E_LOAD_TYPE_DUMMY_QUAD_SC500AI: {
            return "Dummy_Quad_SC500AI.json";
        }
        case E_LOAD_TYPE_QUAD_IMX678: {
            return "Quad_IMX678.json";
        }
        case E_LOAD_TYPE_DUMMY_QUAD_IMX678: {
            return "Dummy_Quad_IMX678.json";
        }
        case E_LOAD_TYPE_PANO_OS04A10x8: {
            return "Pano_OS04A10x8.json";
        }
        case E_LOAD_TYPE_PANO_SC450AIx8: {
            return "Pano_SC450AIx8.json";
        }
        default: {
            return "Dual_OS08A20.json";
        }
    }
}

AX_BOOL CSensorCfgParser::SetSnsSettings(picojson::array& arrSettings, std::map<AX_U8, SENSOR_CONFIG_T>& mapDev2SnsSetting) {
    SENSOR_CONFIG_T tBaseSensorCfg;
    PIPE_CONFIG_T tBasePipeConfig;
    AX_BOOL bIgnorLegally = AX_FALSE;

    if (!mapDev2SnsSetting.empty()) { // init with base cfg
        tBaseSensorCfg = mapDev2SnsSetting[0];
        tBaseSensorCfg.nPipeCount = 0;
        tBasePipeConfig = tBaseSensorCfg.arrPipeAttr[0];

        mapDev2SnsSetting.clear();

        bIgnorLegally = AX_TRUE;
    }

    picojson::object objSetting;
    for (size_t nSettingIndex = 0; nSettingIndex < arrSettings.size(); nSettingIndex++) {
        auto& objSetting = arrSettings[nSettingIndex].get<picojson::object>();
        SENSOR_CONFIG_T tSensorCfg = tBaseSensorCfg;

        if (objSetting.end() != objSetting.find("sns_id")) {
            tSensorCfg.nSnsID = objSetting["sns_id"].get<double>();
        } else {
            LOG_MM_E(SNS_PARSER, "Miss 'sns_id' config!");
            return AX_FALSE;
        }

        if (objSetting.end() != objSetting.find("dev_id")) {
            tSensorCfg.nDevID = objSetting["dev_id"].get<double>();
        } else {
            LOG_MM_E(SNS_PARSER, "Miss 'dev_id' config!");
            return AX_FALSE;
        }

        if (objSetting.end() != objSetting.find("rx_dev")) {
            tSensorCfg.nRxDev = objSetting["rx_dev"].get<double>();
        } else {
            tSensorCfg.nRxDev = tSensorCfg.nDevID;
        }
        LOG_M_I(SNS_PARSER, "Sns[%d]DevId[%d] nRxDev: %d",
                tSensorCfg.nSnsID, tSensorCfg.nDevID,
                tSensorCfg.nRxDev);

        if (objSetting.end() != objSetting.find("dev_node")) {
            tSensorCfg.nDevNode = objSetting["dev_node"].get<double>();
            LOG_M_D(SNS_PARSER, "Sns[%d]DevId[%d] nDevNode: %d",
                                tSensorCfg.nSnsID, tSensorCfg.nDevID,
                                tSensorCfg.nDevNode);
        } else {
            LOG_MM_E(SNS_PARSER, "Miss 'dev_node' config!");
            return AX_FALSE;
        }

        if (objSetting.end() != objSetting.find("i2c_addr")) {
            tSensorCfg.nI2cAddr = objSetting["i2c_addr"].get<double>();
            LOG_M_D(SNS_PARSER, "Sns[%d]DevId[%d] nI2cAddr: %d",
                                tSensorCfg.nSnsID, tSensorCfg.nDevID,
                                tSensorCfg.nI2cAddr);
        }

        if (objSetting.end() != objSetting.find("clk_id")) {
            tSensorCfg.nClkID = objSetting["clk_id"].get<double>();
            LOG_M_I(SNS_PARSER, "Sns[%d]DevId[%d] nClkID: %d",
                                tSensorCfg.nSnsID, tSensorCfg.nDevID,
                                tSensorCfg.nClkID);
        } else {
            LOG_MM_E(SNS_PARSER, "Miss 'clk_id' config!");
            return AX_FALSE;
        }

        if (objSetting.end() != objSetting.find("sns_mirror_flip")) {
            tSensorCfg.bSnsMirrorFlip = objSetting["sns_mirror_flip"].get<double>() == 0 ? AX_FALSE : AX_TRUE;
        }

        if (objSetting.end() != objSetting.find("stitch_mirror_flip")) {
            tSensorCfg.bStitchMirrorFlip = objSetting["stitch_mirror_flip"].get<double>() == 0 ? AX_FALSE : AX_TRUE;
        }

        if (objSetting.end() != objSetting.find("shutter_type")) {
            tSensorCfg.nShutterType = objSetting["shutter_type"].get<double>();
        }

        if (objSetting.end() != objSetting.find("sns_raw_type")) {
            tSensorCfg.arrSnsRawType[1] =  objSetting["sns_raw_type"].get<picojson::array>()[0].get<double>(); // sdr
            tSensorCfg.arrSnsRawType[2] =  objSetting["sns_raw_type"].get<picojson::array>()[1].get<double>(); // hdr
            LOG_M_I(SNS_PARSER, "Sns[%d] set sdr raw type: %d, hdr raw type: %d",
                                    tSensorCfg.nSnsID, tSensorCfg.arrSnsRawType[1], tSensorCfg.arrSnsRawType[2]);
        }

        if (objSetting.end() != objSetting.find("master_slave_select")) {
            tSensorCfg.eMasterSlaveSel = (AX_SNS_MASTER_SLAVE_E)objSetting["master_slave_select"].get<double>();
        }

        if (objSetting.end() != objSetting.find("sns_framerate")) {
            tSensorCfg.fFrameRate = objSetting["sns_framerate"].get<double>();
        } else if (!bIgnorLegally) {
            LOG_MM_E(SNS_PARSER, "Miss 'sns_framerate' config!");
            return AX_FALSE;
        }

        if (objSetting.end() != objSetting.find("sns_type")) {
            tSensorCfg.eSensorType = (SNS_TYPE_E)objSetting["sns_type"].get<double>();
        } else if (!bIgnorLegally) {
            LOG_MM_E(SNS_PARSER, "Miss 'sns_type' config!");
            return AX_FALSE;
        }

        if (objSetting.end() != objSetting.find("sns_mode")) {
            tSensorCfg.eSensorMode = (AX_SNS_HDR_MODE_E)objSetting["sns_mode"].get<double>();
        } else if (!bIgnorLegally) {
            LOG_MM_E(SNS_PARSER, "Miss 'sns_mode' config!");
            return AX_FALSE;
        }

        if (objSetting.end() != objSetting.find("setting_index")) {
            picojson::array& arrSettingIndex = objSetting["setting_index"].get<picojson::array>();
            for (size_t i = 0; i < 3; i++) {
                tSensorCfg.arrSettingIndex[i] = arrSettingIndex[i].get<double>();
            }
        }

        if (objSetting.end() != objSetting.find("dev_run_mode")) {
            tSensorCfg.eDevMode = (AX_VIN_DEV_MODE_E)objSetting["dev_run_mode"].get<double>();
        } else if (!bIgnorLegally) {
            LOG_MM_E(SNS_PARSER, "Miss 'dev_run_mode' config!");
            return AX_FALSE;
        }

        if (objSetting.end() != objSetting.find("sns_output_mode")) {
            tSensorCfg.eSnsOutputMode = (AX_SNS_OUTPUT_MODE_E)objSetting["sns_output_mode"].get<double>();
        } else if (!bIgnorLegally) {
            LOG_MM_E(SNS_PARSER, "Miss 'sns_output_mode' config!");
            return AX_FALSE;
        }

        if (objSetting.end() != objSetting.find("enable_flash")) {
            tSensorCfg.bEnableFlash = (AX_BOOL)objSetting["enable_flash"].get<double>();
        }

        if (objSetting.end() != objSetting.find("pn_framerate")) {
            tSensorCfg.strPNFrameRate = objSetting["pn_framerate"].get<string>();
        }

        PIPE_CONFIG_T tPipeConfig = tBasePipeConfig;
        if (objSetting.end() != objSetting.find("pipe_id")) {
            tPipeConfig.nPipeID = objSetting["pipe_id"].get<double>();
        } else {
            LOG_MM_E(SNS_PARSER, "Miss 'pipe_id' config!");
            return AX_FALSE;
        }

        if (objSetting.end() != objSetting.find("src_depth")) {
            tPipeConfig.nSrcDepth = objSetting["src_depth"].get<double>();
        }

        if (objSetting.end() != objSetting.find("enable_eis")) {
            tPipeConfig.bEnableEIS = objSetting["enable_eis"].get<double>() < 0 ? AX_FALSE : AX_TRUE;
            if (tPipeConfig.bEnableEIS) {
                tPipeConfig.bEISOn = objSetting["enable_eis"].get<double>() == 0 ? AX_FALSE : AX_TRUE;
            }
        }

        if (objSetting.end() != objSetting.find("pipe_framerate")) {
            tPipeConfig.fPipeFramerate = objSetting["pipe_framerate"].get<double>();
        }

        if (objSetting.end() != objSetting.find("pipe_work_mode")) {
            tPipeConfig.ePipeWorkMode = (AX_VIN_PIPE_WORK_MODE_E)objSetting["pipe_work_mode"].get<double>();
        } else if (!bIgnorLegally) {
            LOG_MM_E(SNS_PARSER, "Miss 'pipe_work_mode' config!");
            return AX_FALSE;
        }

        if (objSetting.end() != objSetting.find("pipe_sched_priority")) {
            tPipeConfig.tPipeSchedAttr.nPriority = objSetting["pipe_sched_priority"].get<double>();
        } else {
            tPipeConfig.tPipeSchedAttr.nPriority = 0;
        }

        if (objSetting.end() != objSetting.find("pipe_sched_policy")) {
            tPipeConfig.tPipeSchedAttr.ePolicy = (AX_VIN_SCHED_POLICY_E)objSetting["pipe_sched_policy"].get<double>();
        } else {
            tPipeConfig.tPipeSchedAttr.ePolicy = AX_VIN_SCHED_POLICY_FIFO;
        }

        if (objSetting.end() != objSetting.find("resolution")) {
            picojson::array& arrChnResInfo = objSetting["resolution"].get<picojson::array>();
            for (size_t i = 0; i < arrChnResInfo.size(); i++) {
                if (i >= AX_VIN_CHN_ID_MAX) {
                    continue;
                }
                tPipeConfig.arrChannelAttr[i].nWidth = arrChnResInfo[i].get<picojson::array>()[0].get<double>();
                tPipeConfig.arrChannelAttr[i].nHeight = arrChnResInfo[i].get<picojson::array>()[1].get<double>();
            }
        } else if (!bIgnorLegally) {
            LOG_MM_E(SNS_PARSER, "Miss 'resolution' config!");
            return AX_FALSE;
        }

        if (objSetting.end() != objSetting.find("yuv_depth")) {
            picojson::array& arrYuvDepth = objSetting["yuv_depth"].get<picojson::array>();
            for (size_t i = 0; i < arrYuvDepth.size(); i++) {
                if (i >= AX_VIN_CHN_ID_MAX) {
                    continue;
                }

                tPipeConfig.arrChannelAttr[i].nYuvDepth = arrYuvDepth[i].get<double>();
            }
        } else if (!bIgnorLegally) {
            LOG_MM_E(SNS_PARSER, "Miss 'yuv_depth' config!");
            return AX_FALSE;
        }

        if (objSetting.end() != objSetting.find("chn_framerate")) {
            picojson::array& arrChnFrameRate = objSetting["chn_framerate"].get<picojson::array>();
            for (size_t i = 0; i < arrChnFrameRate.size(); i++) {
                if (i >= AX_VIN_CHN_ID_MAX) {
                    continue;
                }

                tPipeConfig.arrChannelAttr[i].fFrameRate = arrChnFrameRate[i].get<double>();
            }
        }

        if (objSetting.end() != objSetting.find("enable_channel")) {
            picojson::array& arrChnEnable = objSetting["enable_channel"].get<picojson::array>();
            for (size_t i = 0; i < arrChnEnable.size(); i++) {
                if (i >= AX_VIN_CHN_ID_MAX) {
                    continue;
                }

                tPipeConfig.arrChannelAttr[i].bChnEnable = arrChnEnable[i].get<double>() == 0 ? AX_FALSE : AX_TRUE;
            }
        } else if (!bIgnorLegally) {
            LOG_MM_E(SNS_PARSER, "Miss 'enable_channel' config!");
            return AX_FALSE;
        }

        if (objSetting.end() != objSetting.find("chn_compress")) {
            picojson::array& arrChnCompressInfo = objSetting["chn_compress"].get<picojson::array>();
            for (size_t i = 0; i < arrChnCompressInfo.size(); i++) {
                if (i >= AX_VIN_CHN_ID_MAX) {
                    continue;
                }
                tPipeConfig.arrChannelAttr[i].tChnCompressInfo.enCompressMode =
                    (AX_COMPRESS_MODE_E)arrChnCompressInfo[i].get<picojson::array>()[0].get<double>();
                tPipeConfig.arrChannelAttr[i].tChnCompressInfo.u32CompressLevel =
                    arrChnCompressInfo[i].get<picojson::array>()[1].get<double>();
            }
        } else if (!bIgnorLegally) {
            LOG_MM_E(SNS_PARSER, "Miss 'chn_compress' config!");
            return AX_FALSE;
        }

        if (objSetting.end() != objSetting.find("snapshot")) {
            tPipeConfig.bSnapshot = objSetting["snapshot"].get<double>() == 0 ? AX_FALSE : AX_TRUE;
        } else if (!bIgnorLegally) {
            LOG_MM_E(SNS_PARSER, "Miss 'snapshot' config!");
            return AX_FALSE;
        }

        if (objSetting.end() != objSetting.find("continuous_frames")) {
            tPipeConfig.nContinousFrames = objSetting["continuous_frames"].get<double>();
        }

        if (objSetting.end() != objSetting.find("enable_aiisp")) {
            tPipeConfig.bAiEnable = objSetting["enable_aiisp"].get<double>() == 0 ? AX_FALSE : AX_TRUE;
        } else if (!bIgnorLegally) {
            LOG_MM_E(SNS_PARSER, "Miss 'enable_aiisp' config!");
            return AX_FALSE;
        }

        if (objSetting.end() != objSetting.find("enable_dummy")) {
            tPipeConfig.bDummyEnable = objSetting["enable_dummy"].get<double>() == 0 ? AX_FALSE : AX_TRUE;
        }

        if (objSetting.end() != objSetting.find("ife_compress")) {
            tPipeConfig.tIfeCompress.enCompressMode =
                (AX_COMPRESS_MODE_E)objSetting["ife_compress"].get<picojson::array>()[0].get<double>();
            tPipeConfig.tIfeCompress.u32CompressLevel = objSetting["ife_compress"].get<picojson::array>()[1].get<double>();
        } else if (!bIgnorLegally) {
            LOG_MM_E(SNS_PARSER, "Miss 'ife_compress' config!");
            return AX_FALSE;
        }

        if (objSetting.end() != objSetting.find("ainr_compress")) {
            tPipeConfig.tAiNrCompress.enCompressMode =
                (AX_COMPRESS_MODE_E)objSetting["ainr_compress"].get<picojson::array>()[0].get<double>();
            tPipeConfig.tAiNrCompress.u32CompressLevel = objSetting["ainr_compress"].get<picojson::array>()[1].get<double>();
        } else if (!bIgnorLegally) {
            LOG_MM_E(SNS_PARSER, "Miss 'ainr_compress' config!");
            return AX_FALSE;
        }

        if (objSetting.end() != objSetting.find("3dnr_compress")) {
            tPipeConfig.t3DNrCompress.enCompressMode =
                (AX_COMPRESS_MODE_E)objSetting["3dnr_compress"].get<picojson::array>()[0].get<double>();
            tPipeConfig.t3DNrCompress.u32CompressLevel = objSetting["3dnr_compress"].get<picojson::array>()[1].get<double>();
        } else if (!bIgnorLegally) {
            LOG_MM_E(SNS_PARSER, "Miss '3dnr_compress' config!");
            return AX_FALSE;
        }

        if (objSetting.end() != objSetting.find("tuning_ctrl")) {
            tPipeConfig.bTuning = objSetting["tuning_ctrl"].get<double>() == 0 ? AX_FALSE : AX_TRUE;
        }
        if (objSetting.end() != objSetting.find("tuning_port")) {
            tPipeConfig.nTuningPort = objSetting["tuning_port"].get<double>();
        }

        if (objSetting.end() != objSetting.find("tuning_bin")) {
            picojson::array& arrTuningBin = objSetting["tuning_bin"].get<picojson::array>();
            tPipeConfig.vecTuningBin.clear();
            for (size_t i = 0; i < arrTuningBin.size(); i++) {
                tPipeConfig.vecTuningBin.push_back(arrTuningBin[i].get<string>());
            }
        }

        if (objSetting.end() != objSetting.find("ldc")) {
            picojson::object& objGDC = objSetting["ldc"].get<picojson::object>();
            if (objGDC.end() != objGDC.find("ldc_enable") && objGDC.end() != objGDC.find("ldc_aspect") &&
                objGDC.end() != objGDC.find("ldc_x_ratio") && objGDC.end() != objGDC.find("ldc_y_ratio") &&
                objGDC.end() != objGDC.find("ldc_xy_ratio") && objGDC.end() != objGDC.find("ldc_distor_ratio")) {
                tPipeConfig.tLdcAttr.bLdcEnable = (AX_BOOL)objGDC["ldc_enable"].get<double>();
                tPipeConfig.tLdcAttr.bLdcAspect = (AX_BOOL)objGDC["ldc_aspect"].get<double>() ? AX_TRUE : AX_FALSE;
                tPipeConfig.tLdcAttr.nLdcXRatio = objGDC["ldc_x_ratio"].get<double>();
                tPipeConfig.tLdcAttr.nLdcYRatio = objGDC["ldc_y_ratio"].get<double>();
                tPipeConfig.tLdcAttr.nLdcXYRatio = objGDC["ldc_xy_ratio"].get<double>();
                tPipeConfig.tLdcAttr.nLdcDistortionRatio = objGDC["ldc_distor_ratio"].get<double>();
            }
        }

        if (objSetting.end() != objSetting.find("dev_outside_attr")) {
            picojson::object objOutSideAttr = objSetting["dev_outside_attr"].get<picojson::object>();
            tSensorCfg.tDevOutSideAttr.tPowerSyncAttr.ePowerTriggerMode = (AX_VIN_SYNC_TRIGGER_MODE_E)objOutSideAttr["power_trigger_mode"].get<double>();
            tSensorCfg.tDevOutSideAttr.tPowerSyncAttr.nGpioElecInPin = objOutSideAttr["gpio_elec_in_pin"].get<double>();
        }

        if (objSetting.end() != objSetting.find("ife_dump_raw")) {
            picojson::object& objIFEDumpRaw = objSetting["ife_dump_raw"].get<picojson::object>();
            if (objIFEDumpRaw.end() != objIFEDumpRaw.find("enable")) {
                tPipeConfig.tIFEDumpInfo.nIFEDumpRawEnable = (AX_S8)objIFEDumpRaw["enable"].get<double>();
            }

            if (2 == tPipeConfig.ePipeWorkMode && tPipeConfig.bSnapshot) { // npu offline mode, snapshot pipe must be enabled
                tPipeConfig.tIFEDumpInfo.nIFEDumpRawEnable = 1;
            }

            if (objIFEDumpRaw.end() != objIFEDumpRaw.find("depth")) {
                tPipeConfig.tIFEDumpInfo.nIFEDumpRawDepth = objIFEDumpRaw["depth"].get<double>();
            }

            if (objIFEDumpRaw.end() != objIFEDumpRaw.find("wait_time")) {
                tPipeConfig.tIFEDumpInfo.nDumpThreadWaitTime = objIFEDumpRaw["wait_time"].get<double>();
            }
        }

        if (objSetting.end() != objSetting.find("rltm_dump_raw")) {
            picojson::object& objRLTMDumpRaw = objSetting["rltm_dump_raw"].get<picojson::object>();
            if (objRLTMDumpRaw.end() != objRLTMDumpRaw.find("enable")) {
                tPipeConfig.tRLTMDumpInfo.nRLTMDumpRawEnable = (AX_S8)objRLTMDumpRaw["enable"].get<double>();
            }

            if (objRLTMDumpRaw.end() != objRLTMDumpRaw.find("depth")) {
                tPipeConfig.tRLTMDumpInfo.nRLTMDumpRawDepth = objRLTMDumpRaw["depth"].get<double>();
            }
        }

        if (objSetting.end() != objSetting.find("rotation")) {
            tPipeConfig.eRotation = (AX_VIN_ROTATION_E)objSetting["rotation"].get<double>();
            tPipeConfig.bRotationEngine = AX_TRUE;
        }

        if ((objSetting.end() != objSetting.find("mirror"))) {
            tPipeConfig.bMirror = (AX_BOOL)objSetting["mirror"].get<bool>();
            tPipeConfig.bMirrorFlipEngine = AX_TRUE;
        }

        if ((objSetting.end() != objSetting.find("flip"))) {
            tPipeConfig.bFlip = (AX_BOOL)objSetting["flip"].get<bool>();
            tPipeConfig.bMirrorFlipEngine = AX_TRUE;
        }

        if ((objSetting.end() != objSetting.find("engine_mode"))) {
            tPipeConfig.nEngineMode = objSetting["engine_mode"].get<double>();
        }

        if (mapDev2SnsSetting.find(tSensorCfg.nSnsID) != mapDev2SnsSetting.end()) {
            if (MAX_PIPE_PER_DEVICE == mapDev2SnsSetting[tSensorCfg.nSnsID].nPipeCount) {
                LOG_M_E(SNS_PARSER, "Configured sensor count exceeding the max number %d\n", MAX_PIPE_PER_DEVICE);
                return AX_FALSE;
            }

            mapDev2SnsSetting[tSensorCfg.nSnsID].arrPipeAttr[mapDev2SnsSetting[tSensorCfg.nSnsID].nPipeCount] = tPipeConfig;
            mapDev2SnsSetting[tSensorCfg.nSnsID].nPipeCount++;
        } else {
            tSensorCfg.arrPipeAttr[tSensorCfg.nPipeCount] = tPipeConfig;
            tSensorCfg.nPipeCount++;
            mapDev2SnsSetting[tSensorCfg.nSnsID] = tSensorCfg;
        }
    }

    return AX_TRUE;
}

string CSensorCfgParser::GetBaseJsonFile(const std::string& strJsonFullPath) {
    if (strJsonFullPath.empty()) {
        LOG_MM_E(SNS_PARSER, "Json file path is empty");
        return "";
    }

    string strBaseJsonFile = strJsonFullPath;
    // find ".json" pos
    size_t pos = strBaseJsonFile.find(JSON_POSTFIX);

    if (pos != std::string::npos) {
        string strBasePostFix = JSON_BASE_TAG;
        strBasePostFix += JSON_POSTFIX;
        strBaseJsonFile.replace(pos, strBasePostFix.length(), strBasePostFix);
    } else {
        strBaseJsonFile = "";
    }

    return strBaseJsonFile;
}

std::map<AX_U8, SENSOR_CONFIG_T> CSensorCfgParser::GetBaseSnsSetting(const std::string& strJsonFullPath) {
    std::map<AX_U8, SENSOR_CONFIG_T> mapDev2SnsSetting;

    string strBaseCfgPath = GetBaseJsonFile(strJsonFullPath);
    if (strBaseCfgPath.empty()) {
        LOG_MM_W(SNS_PARSER, "Do not exist base cfg file.");
        return mapDev2SnsSetting;
    }

    picojson::value v;
    ifstream fIn(strBaseCfgPath.c_str());
    if (!fIn.is_open()) {
        return mapDev2SnsSetting;
    }

    string strParseErr = picojson::parse(v, fIn);
    if (!strParseErr.empty() || !v.is<picojson::array>()) {
        LOG_MM_E(SNS_PARSER, "Parse err: %s, is picojson::array: %d", strParseErr.c_str(), v.is<picojson::array>());
        return mapDev2SnsSetting;
    }

    if (SetSnsSettings(v.get<picojson::array>(), mapDev2SnsSetting)) {
        LOG_M_C(SNS_PARSER, "Init sensor config form base config file: %s", strBaseCfgPath.c_str());
    }

    return mapDev2SnsSetting;
}

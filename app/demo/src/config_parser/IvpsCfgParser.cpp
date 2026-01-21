/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include "IvpsCfgParser.h"
#include <fstream>
#include "AppLogApi.h"
#include "CommonUtils.hpp"

using namespace std;

#define IVPS_PARSER "IVPS_PARSER"

AX_BOOL CIvpsCfgParser::InitOnce() {
    return AX_TRUE;
}

AX_BOOL CIvpsCfgParser::GetConfig(std::map<AX_U8, std::vector<IVPS_GROUP_CFG_T>> &mapScenario2GrpSetting) {
    string strConfigDir = CCommonUtils::GetPPLConfigDir();
    if (strConfigDir.empty()) {
        return AX_FALSE;
    }

    string strIvpsCfgFile = strConfigDir + "/ivps.json";

    ifstream ifs(strIvpsCfgFile.c_str());
    picojson::value v;
    ifs >> v;

    string err = picojson::get_last_error();
    if (!err.empty()) {
        LOG_M_E(IVPS_PARSER, "Failed to load json config file: %s", strIvpsCfgFile.c_str());
        return AX_FALSE;
    }

    if (!v.is<picojson::object>()) {
        LOG_M_E(IVPS_PARSER, "Loaded config file is not a well-formatted JSON.");
        return AX_FALSE;
    }

    return ParseFile(strIvpsCfgFile, mapScenario2GrpSetting);
}

AX_BOOL CIvpsCfgParser::ParseFile(const string &strPath, std::map<AX_U8, std::vector<IVPS_GROUP_CFG_T>> &mapScenario2GrpSetting) {
    picojson::value v;
    ifstream fIn(strPath.c_str());
    if (!fIn.is_open()) {
        return AX_FALSE;
    }

    string strParseRet = picojson::parse(v, fIn);
    if (!strParseRet.empty() || !v.is<picojson::object>()) {
        return AX_FALSE;
    }

    return ParseJson(v.get<picojson::object>(), mapScenario2GrpSetting);
}

AX_BOOL CIvpsCfgParser::ParseJson(picojson::object &objJsonRoot, std::map<AX_U8, std::vector<IVPS_GROUP_CFG_T>> &mapScenario2GrpSetting) {
    AX_BOOL bExistCfgBaseInfo = AX_FALSE;
    picojson::object objCfgBaseInfo;
    if (objJsonRoot.end() != objJsonRoot.find("cfg_base_info")) {
        objCfgBaseInfo = objJsonRoot["cfg_base_info"].get<picojson::object>();
        bExistCfgBaseInfo = AX_TRUE;
    }

    for (AX_U32 i = 0; i < E_IPC_SCENARIO_MAX; i++) {
        string strScenario = CCmdLineParser::ScenarioEnum2Str((AX_IPC_SCENARIO_E)i);

        if (objJsonRoot.end() == objJsonRoot.find(strScenario.c_str())) {
            continue;
        }

        picojson::array &arrGrpSetting = objJsonRoot[strScenario.c_str()].get<picojson::array>();
        if (0 == arrGrpSetting.size()) {
            return AX_FALSE;
        }

        vector<IVPS_GROUP_CFG_T> vecGrpSetting;
        AX_BOOL bModify = AX_FALSE;
        if (mapScenario2GrpSetting.end() != mapScenario2GrpSetting.find(i)) {
            vecGrpSetting = mapScenario2GrpSetting[i];
            bModify = AX_TRUE;
        }
        AX_BOOL bItemCheck = bModify ? AX_FALSE : AX_TRUE;

        for (size_t i = 0; i < arrGrpSetting.size(); i++) {
            IVPS_GROUP_CFG_T tOutConfig;
            AX_U8 nModifySettingIndex = 0;

            picojson::object &objSetting = arrGrpSetting[i].get<picojson::object>();
            JSON_PARSE_DIGITAL_ITEM(IVPS_PARSER, tOutConfig.nGrp, IVPS_GRP, objSetting, "grp_id", AX_TRUE);
            if (bModify) {
                for (auto grpSetting : vecGrpSetting) {
                    if (grpSetting.nGrp == tOutConfig.nGrp) {
                        tOutConfig = grpSetting;
                        break;
                    }
                    nModifySettingIndex++;
                }
            }

            AX_BOOL bObjFound = AX_FALSE;
            JSON_PARSE_FIND_ITEM(IVPS_PARSER, bObjFound, objSetting, "grp_info", bItemCheck);
            if (!bObjFound) {
                continue;
            }
            picojson::object &objGroupInfo = objSetting["grp_info"].get<picojson::object>();

            JSON_PARSE_BOOL_ITEM(IVPS_PARSER, tOutConfig.bEnable, objGroupInfo, "enable", AX_FALSE);
            JSON_PARSE_DIGITAL_ITEM(IVPS_PARSER, tOutConfig.nGrpChnNum, AX_U8, objGroupInfo, "grp_chn_num", bItemCheck);
            JSON_PARSE_CONVERT_STRING_ITEM(IVPS_PARSER, tOutConfig.arrGrpEngineType[0],
                                           objGroupInfo, "engine_filter_0", Str2Engine, bItemCheck);
            JSON_PARSE_CONVERT_STRING_ITEM(IVPS_PARSER, tOutConfig.arrGrpEngineType[1],
                                           objGroupInfo, "engine_filter_1", Str2Engine, bItemCheck);

            JSON_PARSE_CONVERT_ARRAY_ITEMS(IVPS_PARSER, tOutConfig.arrChnEngineType[i][0],
                                           objGroupInfo, "engine_filter_2",
                                           std::string, Str2Engine, bItemCheck);

            JSON_PARSE_CONVERT_ARRAY_ITEMS(IVPS_PARSER, tOutConfig.arrChnEngineType[i][1],
                                           objGroupInfo, "engine_filter_3",
                                           std::string, Str2Engine, bItemCheck);

            JSON_PARSE_FIND_ITEM(IVPS_PARSER, bObjFound, objGroupInfo, "grp_framerate", bItemCheck);
            if (bObjFound) {
                picojson::object &objGrpFramerate = objGroupInfo["grp_framerate"].get<picojson::object>();
                JSON_PARSE_DIGITAL_ITEM(IVPS_PARSER, tOutConfig.arrGrpFramerate[0], AX_F32, objGrpFramerate, "src", bItemCheck);
                JSON_PARSE_DIGITAL_ITEM(IVPS_PARSER, tOutConfig.arrGrpFramerate[1], AX_F32, objGrpFramerate, "dst", bItemCheck);
            }

            JSON_PARSE_FIND_ITEM(IVPS_PARSER, bObjFound, objGroupInfo, "chn_framerate", bItemCheck);
            if (bObjFound) {
                picojson::array &arrChnFramerate = objGroupInfo["chn_framerate"].get<picojson::array>();
                for (size_t i = 0; i < arrChnFramerate.size(); i++) {
                    picojson::object &objChnFramerate = arrChnFramerate[i].get<picojson::object>();
                    JSON_PARSE_DIGITAL_ITEM(IVPS_PARSER, tOutConfig.arrChnFramerate[i][0],
                                            AX_F32, objChnFramerate, "src", bItemCheck);
                    JSON_PARSE_DIGITAL_ITEM(IVPS_PARSER, tOutConfig.arrChnFramerate[i][1],
                                            AX_F32, objChnFramerate, "dst", bItemCheck);
                }
            }

            JSON_PARSE_FIND_ITEM(IVPS_PARSER, bObjFound, objGroupInfo, "grp_resolution", bItemCheck);
            if (bObjFound) {
                picojson::object &objGrpResolution = objGroupInfo["grp_resolution"].get<picojson::object>();
                JSON_PARSE_DIGITAL_ITEM(IVPS_PARSER, tOutConfig.arrGrpResolution[0],
                                        AX_S16, objGrpResolution, "w", bItemCheck);
                JSON_PARSE_DIGITAL_ITEM(IVPS_PARSER, tOutConfig.arrGrpResolution[1],
                                        AX_S16, objGrpResolution, "h", bItemCheck);
            }

            JSON_PARSE_FIND_ITEM(IVPS_PARSER, bObjFound, objGroupInfo, "chn_resolution", bItemCheck);
            if (bObjFound) {
                picojson::array &arrChnResolution = objGroupInfo["chn_resolution"].get<picojson::array>();
                for (size_t i = 0; i < arrChnResolution.size(); i++) {
                    picojson::object &objChnResolution = arrChnResolution[i].get<picojson::object>();
                    JSON_PARSE_DIGITAL_ITEM(IVPS_PARSER, tOutConfig.arrChnResolution[i][0],
                                            AX_S16, objChnResolution, "w", bItemCheck);
                    JSON_PARSE_DIGITAL_ITEM(IVPS_PARSER, tOutConfig.arrChnResolution[i][1],
                                            AX_S16, objChnResolution, "h", bItemCheck);
                }
            }


            JSON_PARSE_FIND_ITEM(IVPS_PARSER, bObjFound, objGroupInfo, "chn_link_flag", bItemCheck);
            if (bObjFound) {
                JSON_PARSE_ARRAY_ITEMS(IVPS_PARSER, tOutConfig.arrChnLinkFlag[i],
                                       objGroupInfo, "chn_link_flag", double, bItemCheck);
            }

            if (objGroupInfo.end() != objGroupInfo.find("grp_fbc")) {
                picojson::object &objGrpFBC = objGroupInfo["grp_fbc"].get<picojson::object>();
                JSON_PARSE_DIGITAL_ITEM(IVPS_PARSER, tOutConfig.arrGrpFBC[0],
                                        AX_U8, objGrpFBC, "mode", bItemCheck);
                JSON_PARSE_DIGITAL_ITEM(IVPS_PARSER, tOutConfig.arrGrpFBC[1],
                                        AX_U8, objGrpFBC, "level", bItemCheck);
            }

            JSON_PARSE_FIND_ITEM(IVPS_PARSER, bObjFound, objGroupInfo, "chn_fbc", bItemCheck);
            if (bObjFound) {
                picojson::array &arrChnFBC = objGroupInfo["chn_fbc"].get<picojson::array>();
                for (size_t i = 0; i < arrChnFBC.size(); i++) {
                    picojson::object &objChnFBC = arrChnFBC[i].get<picojson::object>();
                    JSON_PARSE_DIGITAL_ITEM(IVPS_PARSER, tOutConfig.arrChnFBC[i][0],
                                            AX_U8, objChnFBC, "mode", bItemCheck);
                    JSON_PARSE_DIGITAL_ITEM(IVPS_PARSER, tOutConfig.arrChnFBC[i][1],
                                            AX_U8, objChnFBC, "level", bItemCheck);
                }
            }

            JSON_PARSE_CONVERT_ARRAY_INT_ITEMS_TO_BOOL(IVPS_PARSER, tOutConfig.bArrChnInplace[i],
                                                       objGroupInfo, "chn_inplace", bItemCheck);

            JSON_PARSE_CONVERT_ARRAY_INT_ITEMS_TO_BOOL(IVPS_PARSER, tOutConfig.bArrChnResizeNode[i],
                                                       objGroupInfo, "chn_resize_flag", AX_FALSE);

            if (objGroupInfo.end() != objGroupInfo.find("spec_engine_config")) {
                picojson::object &objEngineCfg = objGroupInfo["spec_engine_config"].get<picojson::object>();
                if (objEngineCfg.end() != objEngineCfg.find("rotation")) {
                    tOutConfig.nRotation = objEngineCfg["rotation"].get<double>();
                    tOutConfig.bRotationEngine = AX_TRUE;
                }

                if ((objEngineCfg.end() != objEngineCfg.find("mirror"))) {
                    tOutConfig.nMirror = objEngineCfg["mirror"].get<double>();
                    tOutConfig.bMirrorFlipEngine = AX_TRUE;
                }

                if ((objEngineCfg.end() != objEngineCfg.find("flip"))) {
                    tOutConfig.nFlip = objEngineCfg["flip"].get<double>();
                    tOutConfig.bMirrorFlipEngine = AX_TRUE;
                }

                if (objEngineCfg.end() != objEngineCfg.find("ldc_enable") && objEngineCfg.end() != objEngineCfg.find("ldc_aspect") &&
                    objEngineCfg.end() != objEngineCfg.find("ldc_x_ratio") && objEngineCfg.end() != objEngineCfg.find("ldc_y_ratio") &&
                    objEngineCfg.end() != objEngineCfg.find("ldc_xy_ratio") &&
                    objEngineCfg.end() != objEngineCfg.find("ldc_distor_ratio")) {
                    tOutConfig.nLdcEnable = objEngineCfg["ldc_enable"].get<double>();
                    tOutConfig.bLdcAspect = objEngineCfg["ldc_aspect"].get<double>() ? AX_TRUE : AX_FALSE;
                    tOutConfig.nLdcXRatio = objEngineCfg["ldc_x_ratio"].get<double>();
                    tOutConfig.nLdcYRatio = objEngineCfg["ldc_y_ratio"].get<double>();
                    tOutConfig.nLdcXYRatio = objEngineCfg["ldc_xy_ratio"].get<double>();
                    tOutConfig.nLdcDistortionRatio = objEngineCfg["ldc_distor_ratio"].get<double>();
                }

                if (objEngineCfg.end() != objEngineCfg.find("osd_video_chn_linked")) {
                    picojson::array &arrLinkedVideo = objEngineCfg["osd_video_chn_linked"].get<picojson::array>();
                    for (size_t i = 0; i < arrLinkedVideo.size(); i++) {
                        tOutConfig.arrOsdLinkedVideo[i] = arrLinkedVideo[i].get<double>();
                    }
                }

                if (objEngineCfg.end() != objEngineCfg.find("osd_non_video_on_flag")) {
                    picojson::array &arrLinkJpgFlag = objEngineCfg["osd_non_video_on_flag"].get<picojson::array>();
                    for (size_t i = 0; i < arrLinkJpgFlag.size(); i++) {
                        tOutConfig.arrOsdNoVideoOnFlag[i] = arrLinkJpgFlag[i].get<double>();
                        if (0 != tOutConfig.arrOsdNoVideoOnFlag[i] && tOutConfig.arrOsdLinkedVideo[i] >= 0) {
                            LOG_M_E(IVPS_PARSER, "Senario:%s, IVPS[grp:%d] should not link both video and jpg, \
                                                  please check osd_video_chn_linked and osd_non_video_on_flag in ivps.json!",
                                                  strScenario.c_str(), tOutConfig.nGrp);
                        }
                    }
                }

                if (objEngineCfg.end() != objEngineCfg.find("mask_enable")) {
                    tOutConfig.bMaskEnable = objEngineCfg["mask_enable"].get<double>() ? AX_TRUE : AX_FALSE;
                }

                if (objEngineCfg.end() != objEngineCfg.find("mask_filter")) {
                    tOutConfig.nMaskFilter = objEngineCfg["mask_filter"].get<double>();
                }

                if (objEngineCfg.end() != objEngineCfg.find("osd_filter")) {
                    tOutConfig.nOSDFilter = objEngineCfg["osd_filter"].get<double>();
                }

                if (objEngineCfg.end() != objEngineCfg.find("osd_time_show")) {
                    tOutConfig.stOsdShowFlag.nShowTime = objEngineCfg["osd_time_show"].get<double>();
                } else {
                    tOutConfig.stOsdShowFlag.nShowTime = 1;
                }

                if (objEngineCfg.end() != objEngineCfg.find("osd_logo_show")) {
                    tOutConfig.stOsdShowFlag.nShowLogo = objEngineCfg["osd_logo_show"].get<double>();
                } else {
                    tOutConfig.stOsdShowFlag.nShowLogo = 1;
                }

                if (objEngineCfg.end() != objEngineCfg.find("osd_rect_show")) {
                    tOutConfig.stOsdShowFlag.nShowRect = objEngineCfg["osd_rect_show"].get<double>();
                } else {
                    tOutConfig.stOsdShowFlag.nShowRect = 1;
                }

                if (objEngineCfg.end() != objEngineCfg.find("osd_channel_show")) {
                    tOutConfig.stOsdShowFlag.nShowChannel = objEngineCfg["osd_channel_show"].get<double>();
                } else {
                    tOutConfig.stOsdShowFlag.nShowChannel = 1;
                }

                if (objEngineCfg.end() != objEngineCfg.find("osd_location_show")) {
                    tOutConfig.stOsdShowFlag.nShowLocation = objEngineCfg["osd_location_show"].get<double>();
                } else {
                    tOutConfig.stOsdShowFlag.nShowLocation = 1;
                }
            }

            if (objGroupInfo.end() != objGroupInfo.find("rotate_bypass")) {
                tOutConfig.bRotateBypass = objGroupInfo["rotate_bypass"].get<double>() ? AX_TRUE : AX_FALSE;
            } else {
                tOutConfig.bRotateBypass = AX_FALSE;
            }

            if (bModify) {
                vecGrpSetting[nModifySettingIndex] = tOutConfig;
            } else {
                vecGrpSetting.emplace_back(tOutConfig);
            }
        }

        // Don't use map insert fuction. Do overwrite if share info set before
        mapScenario2GrpSetting[i] = vecGrpSetting;

        if (bExistCfgBaseInfo) {
            if (objCfgBaseInfo.end() != objCfgBaseInfo.find(strScenario.c_str())) {
                picojson::array &arrScenarios = objCfgBaseInfo[strScenario.c_str()].get<picojson::array>();
                for (size_t i = 0; i < arrScenarios.size(); i++) {
                    AX_U8 nScenario = (AX_U8)arrScenarios[i].get<double>();
                    // use map insert fuction to ignor share info if set before
                    mapScenario2GrpSetting.insert(make_pair(nScenario, vecGrpSetting));
                }
            }
        }
    }

    return mapScenario2GrpSetting.size() > 0 ? AX_TRUE : AX_FALSE;
}

AX_IVPS_ENGINE_E CIvpsCfgParser::Str2Engine(string strEngine) {
    if (strEngine == "SUB") {
        return AX_IVPS_ENGINE_SUBSIDIARY;
    } else if (strEngine == "TDP") {
        return AX_IVPS_ENGINE_TDP;
    } else if (strEngine == "GDC") {
        return AX_IVPS_ENGINE_GDC;
    } else if (strEngine == "VPP") {
        return AX_IVPS_ENGINE_VPP;
    } else if (strEngine == "VGP") {
        return AX_IVPS_ENGINE_VGP;
    } else if (strEngine == "IVE") {
        return AX_IVPS_ENGINE_IVE;
    } else if (strEngine == "VO") {
        return AX_IVPS_ENGINE_VO;
    } else if (strEngine == "DSP") {
        return AX_IVPS_ENGINE_DSP;
    }

    return AX_IVPS_ENGINE_BUTT;
}

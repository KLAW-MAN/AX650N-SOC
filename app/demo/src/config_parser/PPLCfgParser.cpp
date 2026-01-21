/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include "PPLCfgParser.h"
#include <fstream>
#include "AppLogApi.h"
#include "CommonUtils.hpp"

using namespace ::std;

#define PPL_PARSER "PPL_PARSER"

AX_BOOL CPPLCfgParser::InitOnce() {
    return AX_TRUE;
}

AX_BOOL CPPLCfgParser::GetConfig(std::map<AX_U8, std::vector<IPC_MOD_RELATIONSHIP_T>>& mapScenario2Relations) {
    string strConfigDir = CCommonUtils::GetPPLConfigDir();
    if (strConfigDir.empty()) {
        return AX_FALSE;
    }

    string strPPLCfgFile = strConfigDir + "/ppl.json";

    ifstream ifs(strPPLCfgFile.c_str());
    picojson::value v;
    ifs >> v;

    string err = picojson::get_last_error();
    if (!err.empty()) {
        LOG_M_E(PPL_PARSER, "Failed to load json config file: %s, error:%s", strPPLCfgFile.c_str(), err.c_str());
        return AX_FALSE;
    }

    if (!v.is<picojson::object>()) {
        LOG_M_E(PPL_PARSER, "Loaded config file is not a well-formatted JSON.");
        return AX_FALSE;
    }

    if (!ParseFile(strPPLCfgFile, mapScenario2Relations)) {
        return AX_FALSE;
    }

    return AX_TRUE;
}

AX_BOOL CPPLCfgParser::ParseFile(const std::string& strPath,
                                    std::map<AX_U8, std::vector<IPC_MOD_RELATIONSHIP_T>>& mapScenario2Relations) {
    picojson::value v;
    ifstream fIn(strPath.c_str());
    if (!fIn.is_open()) {
        return AX_FALSE;
    }

    string strParseRet = picojson::parse(v, fIn);
    if (!strParseRet.empty() || !v.is<picojson::object>()) {
        return AX_FALSE;
    }

    return ParseJson(v.get<picojson::object>(), mapScenario2Relations);
}

AX_BOOL CPPLCfgParser::ParseJson(picojson::object& objJsonRoot,
                                    std::map<AX_U8, std::vector<IPC_MOD_RELATIONSHIP_T>>& mapScenario2Relations) {
    AX_BOOL bExistPPLBaseInfo = AX_FALSE;
    picojson::object objPPLBaseInfo;
    if (objJsonRoot.end() != objJsonRoot.find("ppl_base_info")) {
        objPPLBaseInfo = objJsonRoot["ppl_base_info"].get<picojson::object>();
        bExistPPLBaseInfo = AX_TRUE;
    }

    for (AX_U32 i = 0; i < E_IPC_SCENARIO_MAX; i++) {
        string strScenario = CCmdLineParser::ScenarioEnum2Str((AX_IPC_SCENARIO_E)i);

        if (objJsonRoot.end() == objJsonRoot.find(strScenario.c_str())) {
            continue;
        }

        picojson::array& arrRelations = objJsonRoot[strScenario.c_str()].get<picojson::array>();
        if (0 == arrRelations.size()) {
            return AX_FALSE;
        }

        vector<IPC_MOD_RELATIONSHIP_T> vecModRelations;
        AX_BOOL bModify = AX_FALSE;
        if (mapScenario2Relations.end() != mapScenario2Relations.find(i)) {
            vecModRelations = mapScenario2Relations[i];
            bModify = AX_TRUE;
        }

        for (size_t i = 0; i < arrRelations.size(); i++) {
            IPC_MOD_RELATIONSHIP_T tRelation;

            picojson::object objRelation = arrRelations[i].get<picojson::object>();

            tRelation.tSrcModChn.eModType = Str2Module(objRelation["src_mod"].get<std::string>());
            tRelation.tSrcModChn.nGroup = objRelation["src_grp"].get<double>();
            tRelation.tSrcModChn.nChannel = objRelation["src_chn"].get<double>();
            tRelation.tDstModChn.eModType = Str2Module(objRelation["dst_mod"].get<std::string>());
            tRelation.tDstModChn.nGroup = objRelation["dst_grp"].get<double>();
            tRelation.tDstModChn.nChannel = objRelation["dst_chn"].get<double>();
            tRelation.bLink = objRelation["link"].get<double>() == 0 ? AX_FALSE : AX_TRUE;

            AX_BOOL bFound = AX_FALSE;
            if (bModify) {
                for (size_t j = 0; j < vecModRelations.size(); j++) {
                    if ((vecModRelations[j].tSrcModChn.eModType == tRelation.tSrcModChn.eModType
                         && vecModRelations[j].tSrcModChn.nGroup == tRelation.tSrcModChn.nGroup
                         && vecModRelations[j].tSrcModChn.nChannel == tRelation.tSrcModChn.nChannel)
                     || (vecModRelations[j].tDstModChn.eModType == tRelation.tDstModChn.eModType
                         && vecModRelations[j].tDstModChn.nGroup == tRelation.tDstModChn.nGroup
                         && vecModRelations[j].tDstModChn.nChannel == tRelation.tDstModChn.nChannel)) {
                        vecModRelations[j] = tRelation;
                        bFound = AX_TRUE;
                        break;
                    }
                }
            }
            if (!bFound) {
                vecModRelations.emplace_back(tRelation);
            }
        }

        // Don't use map insert fuction. Do overwrite if share info set before
        mapScenario2Relations[i] = vecModRelations;

        if (bExistPPLBaseInfo) {
            if (objPPLBaseInfo.end() != objPPLBaseInfo.find(strScenario.c_str())) {
                picojson::array &arrScenarios = objPPLBaseInfo[strScenario.c_str()].get<picojson::array>();
                for (size_t i = 0; i < arrScenarios.size(); i++) {
                    AX_U8 nScenario = (AX_U8)arrScenarios[i].get<double>();
                    // use map insert fuction to ignor share info if set before
                    mapScenario2Relations.insert(make_pair(nScenario, vecModRelations));
                }
            }
        }
    }

    return mapScenario2Relations.size() > 0 ? AX_TRUE : AX_FALSE;
}

PPL_MODULE_TYPE_E CPPLCfgParser::Str2Module(string strModule) {
    if (strModule == "VIN") {
        return E_PPL_MOD_TYPE_VIN;
    } else if (strModule == "IVPS") {
        return E_PPL_MOD_TYPE_IVPS;
    } else if (strModule == "VENC") {
        return E_PPL_MOD_TYPE_VENC;
    } else if (strModule == "JENC") {
        return E_PPL_MOD_TYPE_JENC;
    } else if (strModule == "JDEC") {
        return E_PPL_MOD_TYPE_JDEC;
    } else if (strModule == "DETECT") {
        return E_PPL_MOD_TYPE_DETECT;
    } else if (strModule == "COLLECT") {
        return E_PPL_MOD_TYPE_COLLECT;
    } else if (strModule == "IVES") {
        return E_PPL_MOD_TYPE_IVES;
    } else if (strModule == "DSP") {
        return E_PPL_MOD_TYPE_DSP;
    } else if (strModule == "CAPTURE") {
        return E_PPL_MOD_TYPE_CAPTURE;
    } else if (strModule == "USER") {
        return E_PPL_MOD_TYPE_USER;
    } else if (strModule == "AVS") {
        return E_PPL_MOD_TYPE_AVS;
    } else if (strModule == "VO") {
        return E_PPL_MOD_TYPE_USER; // TODO:
    } else {
        LOG_MM_E(PPL_PARSER, "Unrecognized module specification: %s", strModule.c_str());
    }

    return E_PPL_MOD_TYPE_MAX;
}

/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include "IPCLinkage.h"
#include "CommonUtils.hpp"
#include "PPLOptionHelper.h"
#include "SensorOptionHelper.h"

using namespace AX_IPC;
using namespace std;

#define LINKAGE "LINKAGE"

AX_BOOL CLinkage::Setup() {
    vector<IPC_MOD_RELATIONSHIP_T> vecRelations;
    if (!GetCurrRelations(vecRelations)) {
        LOG_M_W(LINKAGE, "No linkage configured in ppl.json.");
        return AX_FALSE;
    }

    for (auto relation : vecRelations) {
        if (relation.Valid() && relation.bLink) {
            LINK_MOD_INFO_T tLinkInfo;
            tLinkInfo.tSrcModChn.eModType = relation.tSrcModChn.eModType;
            tLinkInfo.tSrcModChn.nGroup = relation.tSrcModChn.nGroup;
            tLinkInfo.tSrcModChn.nChannel = relation.tSrcModChn.nChannel;
            tLinkInfo.tDstModChn.eModType = relation.tDstModChn.eModType;
            tLinkInfo.tDstModChn.nGroup = relation.tDstModChn.nGroup;
            tLinkInfo.tDstModChn.nChannel = relation.tDstModChn.nChannel;
            Link(tLinkInfo);
        }
    }

    return AX_TRUE;
}

AX_BOOL CLinkage::Release() {
    vector<IPC_MOD_RELATIONSHIP_T> vecRelations;
    if (!GetCurrRelations(vecRelations)) {
        return AX_FALSE;
    }

    for (auto relation : vecRelations) {
        if (relation.Valid() && relation.bLink) {
            LINK_MOD_INFO_T tLinkInfo;
            tLinkInfo.tSrcModChn.eModType = relation.tSrcModChn.eModType;
            tLinkInfo.tSrcModChn.nGroup = relation.tSrcModChn.nGroup;
            tLinkInfo.tSrcModChn.nChannel = relation.tSrcModChn.nChannel;
            tLinkInfo.tDstModChn.eModType = relation.tDstModChn.eModType;
            tLinkInfo.tDstModChn.nGroup = relation.tDstModChn.nGroup;
            tLinkInfo.tDstModChn.nChannel = relation.tDstModChn.nChannel;
            Unlink(tLinkInfo);
        }
    }

    return AX_TRUE;
}

const AX_BOOL CLinkage::GetCurrRelations(vector<IPC_MOD_RELATIONSHIP_T>& vecRelations) const {
    AX_U8 nScenario = APP_CURR_SCENARIO();
    APP_PPL_MOD_RELATIONS(nScenario, vecRelations);

    return vecRelations.size() > 0 ? AX_TRUE : AX_FALSE;
}

IPC_MOD_RELATIONSHIP_T CLinkage::GetRelation(const LINK_MOD_INFO_T& tModLink) const {
    vector<IPC_MOD_RELATIONSHIP_T> vecRelations;
    if (!GetCurrRelations(vecRelations)) {
        return {};
    }

    for (auto relation : vecRelations) {
        if (relation.Valid()) {
            if (CCommonUtils::ModuleEqual(relation.tSrcModChn, tModLink.tSrcModChn) &&
                CCommonUtils::ModuleEqual(relation.tDstModChn, tModLink.tDstModChn)) {
                return relation;
            }
        }
    }

    return {};
}

AX_BOOL CLinkage::GetRelationsBySrcMod(const IPC_MOD_INFO_T& tSrcMod, vector<IPC_MOD_RELATIONSHIP_T>& vecOutRelations,
                                       AX_BOOL bIgnoreChn /*= AX_FALSE*/) const {
    vector<IPC_MOD_RELATIONSHIP_T> vecRelations;
    if (!GetCurrRelations(vecRelations)) {
        return AX_FALSE;
    }

    for (auto relation : vecRelations) {
        if (relation.Valid()) {
            if (CCommonUtils::ModuleEqual(relation.tSrcModChn, tSrcMod, bIgnoreChn)) {
                vecOutRelations.emplace_back(relation);
            }
        }
    }

    return vecOutRelations.size() > 0 ? AX_TRUE : AX_FALSE;
}

AX_BOOL CLinkage::GetRelationsByDstMod(const IPC_MOD_INFO_T& tDstMod, vector<IPC_MOD_RELATIONSHIP_T>& vecOutRelations,
                                       AX_BOOL bIgnoreChn /*= AX_FALSE*/) const {
    vector<IPC_MOD_RELATIONSHIP_T> vecRelations;
    if (!GetCurrRelations(vecRelations)) {
        return AX_FALSE;
    }

    for (auto relation : vecRelations) {
        if (relation.Valid()) {
            if (CCommonUtils::ModuleEqual(relation.tDstModChn, tDstMod, bIgnoreChn)) {
                vecOutRelations.emplace_back(relation);
            }
        }
    }

    return vecOutRelations.size() > 0 ? AX_TRUE : AX_FALSE;
}

AX_BOOL CLinkage::GetPrecedingMod(const IPC_MOD_INFO_T& tDstMod, IPC_MOD_INFO_T& tPrecedingMod) const {
    vector<IPC_MOD_RELATIONSHIP_T> vecRelations;
    if (!GetCurrRelations(vecRelations)) {
        return AX_FALSE;
    }

    for (auto relation : vecRelations) {
        if (relation.Valid()) {
            if (CCommonUtils::ModuleEqual(relation.tDstModChn, tDstMod, AX_FALSE)) {
                if (relation.tSrcModChn.eModType == tPrecedingMod.eModType) {
                    tPrecedingMod = relation.tSrcModChn;
                    return AX_TRUE;
                } else {
                    return GetPrecedingMod(relation.tSrcModChn, tPrecedingMod);
                }
            }
        }
    }

    return AX_FALSE;
}

AX_S32 CLinkage::DoNewLink(const LINK_MOD_INFO_T& tNewLinkModInfo, const LINK_MOD_INFO_T& tOldLinkModInfo) const {
    vector<IPC_MOD_RELATIONSHIP_T> vecRelations;
    AX_U8 nScenario = APP_CURR_SCENARIO();
    APP_PPL_MOD_RELATIONS(nScenario, vecRelations);
    if (vecRelations.size() <= 0) {
        return -1;
    }

    for (auto relation : vecRelations) {
        if (relation.Valid()) {
            if (CCommonUtils::ModuleEqual(relation.tSrcModChn, tOldLinkModInfo.tSrcModChn) &&
                CCommonUtils::ModuleEqual(relation.tDstModChn, tOldLinkModInfo.tDstModChn)) {
                IPC_MOD_RELATIONSHIP_T tNewRelationShip = relation;
                tNewRelationShip.tSrcModChn = tNewLinkModInfo.tSrcModChn;
                tNewRelationShip.tDstModChn = tNewLinkModInfo.tDstModChn;

                APP_PPL_UPDATE_MOD_RELATIONS(nScenario, relation, tNewRelationShip);

                return Link(tNewLinkModInfo);
            }
        }
    }

    LOG_M_E(LINKAGE, "Can't find tOldLinkModInfo of link src(%d, %d, %d) with dst(%d, %d, %d).",
                      tOldLinkModInfo.tSrcModChn.eModType, tOldLinkModInfo.tDstModChn.nGroup, tOldLinkModInfo.tDstModChn.nChannel,
                      tOldLinkModInfo.tDstModChn.eModType, tOldLinkModInfo.tDstModChn.nGroup, tOldLinkModInfo.tDstModChn.nChannel);
    return -1;
}

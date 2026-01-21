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
#include "PPLOptionHelper.h"
#include "CommonUtils.hpp"

AX_BOOL CPPLOptionHelper::InitOnce() {
    return CPPLCfgParser::GetInstance()->GetConfig(m_mapScenario2Relations);
}

AX_BOOL CPPLOptionHelper::GetModRelations(AX_U8 nScenario, std::vector<IPC_MOD_RELATIONSHIP_T>& vecModRelations) {
    std::map<AX_U8, std::vector<IPC_MOD_RELATIONSHIP_T>>::iterator it = m_mapScenario2Relations.find(nScenario);
    if (m_mapScenario2Relations.end() == it) {
        return AX_FALSE;
    }

    vecModRelations = it->second;

    return AX_TRUE;
}

AX_BOOL CPPLOptionHelper::UpdateModeRelations(AX_U8 nScenario, const IPC_MOD_RELATIONSHIP_T& oldModRelations, const IPC_MOD_RELATIONSHIP_T& newModRelations)
{
    std::map<AX_U8, std::vector<IPC_MOD_RELATIONSHIP_T>>::iterator it = m_mapScenario2Relations.find(nScenario);
    if (m_mapScenario2Relations.end() == it) {
        return AX_FALSE;
    }

    for (auto &relations : it->second) {
        if (0 == memcmp(&oldModRelations, &relations, sizeof(IPC_MOD_RELATIONSHIP_T))){
            relations = newModRelations;
            return AX_TRUE;
        }
    }

    LOG_M_E("PPLOpt", "Can't find relations for link src(%d, %d, %d) with dst(%d, %d, %d)",
                    oldModRelations.tSrcModChn.eModType, oldModRelations.tSrcModChn.nGroup, oldModRelations.tSrcModChn.nChannel,
                    oldModRelations.tDstModChn.eModType, oldModRelations.tDstModChn.nGroup, oldModRelations.tDstModChn.nChannel);
    return AX_FALSE;
}

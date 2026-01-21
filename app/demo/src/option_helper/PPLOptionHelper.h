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

#include <map>
#include <vector>
#include "AXSingleton.h"
#include "IPPLBuilder.h"

#define APP_PPL_MOD_RELATIONS(nScenario, vecModRelations) \
        CPPLOptionHelper::GetInstance()->GetModRelations(nScenario, vecModRelations)

#define APP_PPL_UPDATE_MOD_RELATIONS(nScenario, oldModRelations, newModRelations) \
        CPPLOptionHelper::GetInstance()->UpdateModeRelations(nScenario, oldModRelations, newModRelations)

/**
 * Load configuration
 */
class CPPLOptionHelper final : public CAXSingleton<CPPLOptionHelper> {
    friend class CAXSingleton<CPPLOptionHelper>;

public:
    AX_BOOL GetModRelations(AX_U8 nScenario, std::vector<IPC_MOD_RELATIONSHIP_T>& vecModRelations);
    AX_BOOL UpdateModeRelations(AX_U8 nScenario, const IPC_MOD_RELATIONSHIP_T& oldModRelations, const IPC_MOD_RELATIONSHIP_T& newModRelations);

private:
    CPPLOptionHelper(AX_VOID) = default;
    ~CPPLOptionHelper(AX_VOID) = default;

    AX_BOOL InitOnce() override;

private:
    std::map<AX_U8, std::vector<IPC_MOD_RELATIONSHIP_T>> m_mapScenario2Relations;
};

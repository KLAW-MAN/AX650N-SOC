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
#include "AXSingleton.h"
#include "ax_base_type.h"
#include "cmdline.hpp"

#define AX_IPC_CMD_KEY_PPL "ppl"
#define AX_IPC_CMD_KEY_TYPE "type"
#define AX_IPC_CMD_KEY_SCENARIO "scenario"
#define AX_IPC_CMD_KEY_LOG_LEVEL "loglv"
#define AX_IPC_CMD_KEY_LOG_TARGET "logTarget"
#define AX_IPC_CMD_KEY_COREDUMP "coredump"
#define AX_IPC_CMD_KEY_GDB_DEBUG "gdb"

#define AX_IPC_SCENARIO_SLT "scenario_slt"
#define AX_IPC_UT "unit_test"
#define AX_IPC_SCENARIO_INVALID "scenario_invalid"

typedef enum {
    E_LOAD_TYPE_DUAL_OS08A20 = 0,
    E_LOAD_TYPE_SINGLE_OS08A20 = 1,
    E_LOAD_TYPE_DUAL_OS08B10 = 2,
    E_LOAD_TYPE_SINGLE_OS08B10 = 3,
    E_LOAD_TYPE_SINGLE_SC910GS = 4,
    E_LOAD_TYPE_PANO_OS04A10x2 = 5,
    E_LOAD_TYPE_SINGLE_OS08A20_PANO_DUAL_OS04A10 = 6,
    E_LOAD_TYPE_SINGLE_OS04A10 = 7,
    E_LOAD_TYPE_QUAD_OS04A10 = 8,
    E_LOAD_TYPE_DUMMY_QUAD_OS04A10 = 9,
    E_LOAD_TYPE_DUMMY_SIX_OS04A10 = 10,
    E_LOAD_TYPE_DUMMY_EIGHT_OS04A10 = 11,
    E_LOAD_TYPE_DUMMY_QUAD_OS08A20 = 12,
    E_LOAD_TYPE_SINGLE_SC410GS = 13,
    E_LOAD_TYPE_QUAD_OS08A20 = 14,
    E_LOAD_TYPE_QUAD_SC500AI = 15,
    E_LOAD_TYPE_DUMMY_QUAD_SC500AI = 16,
    E_LOAD_TYPE_QUAD_IMX678 = 17,
    E_LOAD_TYPE_DUMMY_QUAD_IMX678 = 18,
    E_LOAD_TYPE_PANO_OS04A10x8 = 19,
    E_LOAD_TYPE_PANO_SC450AIx8 = 20,
    E_LOAD_TYPE_MAX
} AX_IPC_LOAD_TYPE_E;

class CCmdLineParser : public CAXSingleton<CCmdLineParser> {
    friend class CAXSingleton<CCmdLineParser>;

public:
    AX_S32 Parse(int argc, const char* const argv[]);

    AX_BOOL GetPPLIndex(AX_S32& nIndex);
    AX_S32 GetPPLIndex();
    AX_BOOL GetLoadType(AX_S32& nType);
    AX_BOOL GetScenario(AX_S32& nScenario);
    AX_S32 GetScenario();
    AX_BOOL GetLogLevel(AX_S32& nLevel);
    AX_BOOL GetLogTarget(AX_S32& nLogTarget);

    static std::string ScenarioEnum2Str(AX_U8 nScenario);
    AX_BOOL isDulSnsMode();
    AX_BOOL isUTEnabled();

private:
    CCmdLineParser(AX_VOID) = default;
    ~CCmdLineParser(AX_VOID) = default;

    AX_BOOL GetIntValue(const std::string& strKey, AX_S32& nOutVal);
    AX_BOOL GetStrValue(const std::string& strKey, std::string& strOutVal);

private:
    std::map<std::string, std::string> m_mapParams;
};
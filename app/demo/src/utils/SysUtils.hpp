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
#include <stdio.h>
#include <unistd.h>
#include <string>
#include "GlobalDef.h"
#include "AppLog.hpp"
#include "ax_base_type.h"

#define SYSUTILS "SysUtils"

class CSysUtils {
public:
    CSysUtils(AX_VOID) = default;
    ~CSysUtils(AX_VOID) = default;

    static AX_BOOL GetBoardID(AX_CHAR* szBoardID, AX_U32 nSize) {
        if (nullptr == szBoardID || 0 == nSize) {
            return AX_FALSE;
        }

        constexpr AX_CHAR DEV_PATH[] = "/proc/ax_proc/board_id";
        FILE* pFile = NULL;
        pFile = fopen(DEV_PATH, "r");
        if (pFile) {
            fread(&szBoardID[0], nSize, 1, pFile);
            fclose(pFile);
        } else {
            LOG_MM_E(SYSUTILS, "open %s fail", DEV_PATH);
            return AX_FALSE;
        }

        return AX_TRUE;
    }

    static AX_BOOL GetChipID(AX_CHAR* szChipID, AX_U32 nSize) {
        if (nullptr == szChipID || 0 == nSize) {
            return AX_FALSE;
        }

        constexpr AX_CHAR DEV_PATH[] = "/proc/ax_proc/chip_type";
        FILE *pFile = NULL;
        pFile = fopen(DEV_PATH, "r");
        if (pFile) {
            fread(&szChipID[0], nSize, 1, pFile);
            fclose(pFile);
        } else {
            LOG_MM_E(SYSUTILS, "open %s fail", DEV_PATH);
            return AX_FALSE;
        }

        return AX_TRUE;
    }
};
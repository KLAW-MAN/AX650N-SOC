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

#include "ax_dsp_api.h"
#include "AppLogApi.h"

#define DSPUTILS "DspUtils"

class CDspUtils {
public:
    CDspUtils(AX_VOID) = default;
    ~CDspUtils(AX_VOID) = default;

    static AX_BOOL InitDsp(AX_DSP_ID_E eDspId, const AX_CHAR* pSramPath, const AX_CHAR* pItcmPath) {
        AX_S32 nRet = -1;
        if (!pSramPath || !pItcmPath) {
            LOG_MM_E(DSPUTILS, "pSramPath(%p) or pItcmPath(%p) is nullptr!", pSramPath, pItcmPath);
            return AX_FALSE;
        }

        nRet = AX_DSP_PowerOn(eDspId);
        if (nRet != AX_DSP_SUCCESS) {
            LOG_MM_E(DSPUTILS, "AX DSP(%d) Poweron error: 0x%x", eDspId, nRet);
            return AX_FALSE;
        }
        nRet = AX_DSP_LoadBin(eDspId, pSramPath, AX_DSP_MEM_TYPE_SRAM);
        if (nRet != AX_DSP_SUCCESS) {
            LOG_MM_E(DSPUTILS, "AX DSP(%d) LoadBin(%s) error: 0x%x", eDspId, pSramPath, nRet);
            return AX_FALSE;
        }
        nRet = AX_DSP_LoadBin(eDspId, pItcmPath, AX_DSP_MEM_TYPE_ITCM);
        if (nRet != AX_DSP_SUCCESS) {
            LOG_MM_E(DSPUTILS, "AX DSP(%d) LoadBin(%s) error: 0x%x", eDspId, pItcmPath, nRet);
            return AX_FALSE;
        }
        nRet = AX_DSP_EnableCore(eDspId);
        if (nRet != AX_DSP_SUCCESS) {
            LOG_MM_E(DSPUTILS, "AX DSP(%d) Enable Core error: 0x%x", eDspId, nRet);
            return AX_FALSE;
        }

        return AX_TRUE;
    }

    static AX_BOOL DeInitDsp(AX_DSP_ID_E enDspId) {
        AX_DSP_DisableCore(enDspId);
        AX_DSP_PowerOff(enDspId);

        return AX_TRUE;
    }
};

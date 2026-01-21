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

#include "IObserver.h"
#include "IVPSGrpStage.h"
#define IVPS_OBS "IVPS_OBS"

class CIvpsObserver : public IObserver {
public:
    CIvpsObserver(CIVPSGrpStage* pSink) : m_pSink(pSink){};
    virtual ~CIvpsObserver(AX_VOID) = default;

public:
    AX_BOOL OnRecvData(OBS_TARGET_TYPE_E eTarget, AX_U32 nGrp, AX_U32 nChn, AX_VOID* pData) override {
        if (!m_pSink) {
            return AX_TRUE;
        }

        if (m_nTargetPipeChannel != (eTarget << 24 | nGrp << 16 | nChn)) {
            return AX_TRUE;
        }

        CAXFrame* pFrame = (CAXFrame*)pData;
        if (E_OBS_TARGET_TYPE_COLLECT == eTarget || E_OBS_TARGET_TYPE_VIN == eTarget) {
            return m_pSink->RecvFrame(pFrame);
        } else {
            LOG_M_E(IVPS_OBS, "Recieve frame from unreconigzed module: %d", eTarget);
        }

        return AX_TRUE;
    }

    AX_BOOL OnRegisterObserver(OBS_TARGET_TYPE_E eTarget, AX_U32 nGrp, AX_U32 nChn, OBS_TRANS_ATTR_PTR pParams) override {
        if (nullptr == pParams) {
            return AX_FALSE;
        }

        if (E_OBS_TARGET_TYPE_VIN == eTarget || E_OBS_TARGET_TYPE_COLLECT == eTarget || E_OBS_TARGET_TYPE_IVPS == eTarget) {
            IVPS_GROUP_CFG_T* pGrpConfig = m_pSink->GetGrpCfg();
            AX_U8 nOutChannels = pGrpConfig->nGrpChnNum;

            pGrpConfig->nSnsSrc = pParams->nSnsSrc;
            pGrpConfig->arrInGrpFBC[0] = pParams->arrOutFBC[0];
            pGrpConfig->arrInGrpFBC[1] = pParams->arrOutFBC[1];
            pGrpConfig->arrGrpInResolution[0] = pParams->nCfgWidth;
            pGrpConfig->arrGrpInResolution[1] = pParams->nCfgHeight;
            if (-1 == pGrpConfig->arrGrpResolution[0]) {
                pGrpConfig->arrGrpResolution[0] = pParams->nCfgWidth;
            }
            if(-1 == pGrpConfig->arrGrpResolution[1]) {
                pGrpConfig->arrGrpResolution[1] = pParams->nCfgHeight;
            }
            pGrpConfig->arrGrpFramerate[0] = pParams->fFramerate;
            pGrpConfig->arrGrpFramerate[1] = pParams->fFramerate;

            for (AX_U8 i = 0; i < nOutChannels; i++) {
                if (-1 == pGrpConfig->arrChnResolution[i][0]) {
                    pGrpConfig->arrChnResolution[i][0] = pParams->nCfgWidth;
                }

                if (-1 == pGrpConfig->arrChnResolution[i][1]) {
                    pGrpConfig->arrChnResolution[i][1] = pParams->nCfgHeight;
                }

                if (-1 == pGrpConfig->arrChnFramerate[i][0]) {
                    pGrpConfig->arrChnFramerate[i][0] = pParams->fFramerate;
                }

                if (-1 == pGrpConfig->arrChnFramerate[i][1]) {
                    pGrpConfig->arrChnFramerate[i][1] = pParams->fFramerate;
                }
            }
        }

        m_nTargetPipeChannel = eTarget << 24 | nGrp << 16 | nChn;

        return AX_TRUE;
    }

    AX_BOOL OnDataChange(OBS_TARGET_TYPE_E eTarget, AX_U32 nGrp, AX_U32 nChn, OBS_CHANGE_DATA_TYPE_E eDataType, AX_VOID* pData) override {
        if (m_nTargetPipeChannel != (eTarget << 24 | nGrp << 16 | nChn)) {
            return AX_TRUE;
        }

        if (!m_pSink) {
            return AX_TRUE;
        }

        if (nullptr == pData) {
            LOG_MM_E(IVPS_OBS, "pData is nullptr for eDataType(%d) from Grp(%d)Chn(%d) of %d",
                               eDataType, nGrp, nChn, eTarget);
            return AX_FALSE;
        }

        switch(eDataType) {
            case E_OBS_CHANGE_DATA_TYPE_RESOLUTION:
                {
                    OBS_TRANS_RESOLUTION_PTR pResolution = (OBS_TRANS_RESOLUTION_PTR)pData;
                    m_pSink->UpdateGrpResolution(pResolution->nWidth, pResolution->nHeight);
                    LOG_M_I(IVPS_OBS, "Update resolution(%d, %d) from Grp(%d)Chn(%d) of %d",
                                       pResolution->nWidth, pResolution->nHeight, nGrp, nChn, eTarget);
                }
                break;
            case E_OBS_CHANGE_DATA_TYPE_ROTATION:
                {
                    OBS_TRANS_ROTATE_INFO_PTR pRotation = (OBS_TRANS_ROTATE_INFO_PTR)pData;
                    m_pSink->UpdateGrpRotation(pRotation->nRotation, pRotation->bInFBC, pRotation->nWidth, pRotation->nHeight);
                    LOG_M_I(IVPS_OBS, "Update rotation(%d, %d, %d, %d) from Grp(%d)Chn(%d) of %d",
                                       pRotation->nRotation, pRotation->bInFBC, pRotation->nWidth,
                                       pRotation->nHeight, nGrp, nChn, eTarget);
                }
                break;
            case E_OBS_CHANGE_DATA_TYPE_SET_MAIN_RESOLUTION:
                {
                    OBS_TRANS_RESOLUTION_PTR pResolution = (OBS_TRANS_RESOLUTION_PTR)pData;
                    m_pSink->SetMainResolution(pResolution->nWidth, pResolution->nHeight);
                    LOG_M_I(IVPS_OBS, "Set main resolution(%d, %d) from Grp(%d)Chn(%d) of %d",
                                       pResolution->nWidth, pResolution->nHeight, nGrp, nChn, eTarget);
                }
                break;
            default:
                LOG_MM_W(IVPS_OBS, "Unsupport eDataType: %d", eDataType);
                break;
        }
        return AX_TRUE;
    }

private:
    CIVPSGrpStage* m_pSink{nullptr};
    AX_U32 m_nTargetPipeChannel{0};
};

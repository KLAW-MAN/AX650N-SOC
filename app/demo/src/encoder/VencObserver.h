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
#include "AXFrame.hpp"
#include "IObserver.h"
#include "VideoEncoder.h"

#define VENC_OBS "VENC_OBS"

class CVencObserver : public IObserver {
public:
    CVencObserver(CVideoEncoder* pSink) : m_pSink(pSink) {
    }
    virtual ~CVencObserver(AX_VOID) = default;
    AX_S32 GetChannel() {
        return m_pSink->GetChannel();
    }

public:
    AX_BOOL OnRecvData(OBS_TARGET_TYPE_E eTarget, AX_U32 nGrp, AX_U32 nChn, AX_VOID* pData) override {
        if (!m_pSink) {
            return AX_TRUE;
        }
        CAXFrame* pFrame = (CAXFrame*)pData;
        if (!pFrame) {
            return AX_TRUE;
        }

        if (m_nTargetPipeChannel != (eTarget << 24 | nGrp << 16 | nChn)) {
            return AX_TRUE;
        }

        return m_pSink->EnqueueFrame(pFrame);
    }

    AX_BOOL OnRegisterObserver(OBS_TARGET_TYPE_E eTarget, AX_U32 nGrp, AX_U32 nChn, OBS_TRANS_ATTR_PTR pParams) override {
        if (nullptr == pParams) {
            return AX_FALSE;
        }

        if (E_OBS_TARGET_TYPE_IVPS == eTarget || E_OBS_TARGET_TYPE_VIN == eTarget || E_OBS_TARGET_TYPE_COLLECT == eTarget) {
            VIDEO_CONFIG_T* tVideoConfig = m_pSink->GetChnCfg();
            tVideoConfig->fFramerate = pParams->fFramerate;
            tVideoConfig->nWidth = pParams->nWidth;
            tVideoConfig->nHeight = pParams->nHeight;
            tVideoConfig->nCfgWidth = pParams->nCfgWidth;
            tVideoConfig->nCfgHeight = pParams->nCfgHeight;
            tVideoConfig->bFBC = (0 == pParams->arrOutFBC[0]) ? AX_FALSE : AX_TRUE;
            tVideoConfig->bLink = AX_FALSE;
            tVideoConfig->nPipeSrc = pParams->nSnsSrc;

            m_nTargetPipeChannel = eTarget << 24 | nGrp << 16 | nChn;
        }

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
            LOG_MM_E(VENC_OBS, "pData nullptr for eDataType: %d", eDataType);
            return AX_FALSE;
        }

        AX_U32 nWidth = 0;
        AX_U32 nHeight = 0;
        switch(eDataType) {
            case E_OBS_CHANGE_DATA_TYPE_ROTATION:
                {
                    OBS_TRANS_ROTATE_INFO_PTR pRotation = (OBS_TRANS_ROTATE_INFO_PTR)pData;
                    nWidth  = pRotation->nWidth;
                    nHeight = pRotation->nHeight;
                }
                break;
            case E_OBS_CHANGE_DATA_TYPE_RESOLUTION:
                {
                    OBS_TRANS_RESOLUTION_PTR pResolution = (OBS_TRANS_RESOLUTION_PTR)pData;
                    nWidth  = pResolution->nWidth;
                    nHeight = pResolution->nHeight;
                }
                break;
            default:
                LOG_MM_W(VENC_OBS, "Unsupport eDataType: %d", eDataType);
                break;
        }

        if (nWidth != 0 && nHeight != 0) {
            m_pSink->StopRecv();
            /*Reset chn fifo*/
            m_pSink->ResetChn();

            m_pSink->UpdateChnResolution(nWidth, nHeight);

            m_pSink->StartRecv();

            LOG_MM_I(VENC_OBS, "Update resolution(%d, %d) by: %d with change data type(%d)",
                                nWidth, nHeight, eTarget, eDataType);
        }

        return AX_TRUE;
    }
private:
    CVideoEncoder* m_pSink{nullptr};
    AX_U32 m_nTargetPipeChannel{0};
};
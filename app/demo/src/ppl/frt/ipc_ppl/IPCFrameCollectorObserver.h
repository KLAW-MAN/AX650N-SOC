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
#include "IPCFrameCollector.h"

namespace AX_IPC {

#define COLLECTOR_OBS "COLLECTOR_OBS"

class CFrameCollectorObserver : public IObserver {
public:
    CFrameCollectorObserver(CFrameCollector* pSink) : m_pSink(pSink){};
    virtual ~CFrameCollectorObserver(AX_VOID) = default;

public:
    AX_BOOL OnRecvData(OBS_TARGET_TYPE_E eTarget, AX_U32 nGrp, AX_U32 nChn, AX_VOID* pData) override {
        if (!m_pSink) {
            return AX_TRUE;
        }
        if (nullptr == pData) {
            LOG_MM_E(COLLECTOR_OBS, "Invalid collector data(pipe=%d, chn=%d, pData=0x%08X).", nGrp, nChn, pData);
            return AX_FALSE;
        }
        CAXFrame* pFrame = (CAXFrame*)pData;
        if (E_OBS_TARGET_TYPE_IVPS == eTarget) {
            return m_pSink->RecvFrame(nGrp, nChn, (CAXFrame*)pData);
        } else {
            pFrame->FreeMem();
        }

        return AX_TRUE;
    }

    AX_BOOL OnRegisterObserver(OBS_TARGET_TYPE_E eTarget, AX_U32 nGrp, AX_U32 nChn, OBS_TRANS_ATTR_PTR pParams) override {
        COLLECTOR_ATTR_T* pConfig = m_pSink->GetCollectorCfg();
        pConfig->nWidth = pParams->nWidth;
        pConfig->nHeight = pParams->nHeight;
        pConfig->nCfgWidth = pParams->nCfgWidth;
        pConfig->nCfgHeight = pParams->nCfgHeight;
        pConfig->nSnsSrc = pParams->nSnsSrc;
        pConfig->arrFBC[0] = pParams->arrOutFBC[0];
        pConfig->arrFBC[1] = pParams->arrOutFBC[1];
        pConfig->fFramerate = pParams->fFramerate;
        LOG_MM_I(COLLECTOR_OBS, "nGrp:%d, nChn:%d, width:%d, height:%d, sensorSrc:%d, frameRate:%lf",
                                 nGrp, nChn, pParams->nWidth, pParams->nHeight, pParams->nSnsSrc, pParams->fFramerate);

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
            LOG_MM_E(COLLECTOR_OBS, "pData nullptr for eDataType(%d) from Grp(%d)Chn(%d) of %d",
                                     eDataType, nGrp, nChn, eTarget);
            return AX_FALSE;
        }

        switch(eDataType) {
            case E_OBS_CHANGE_DATA_TYPE_RESOLUTION:
            case E_OBS_CHANGE_DATA_TYPE_ROTATION:
            case E_OBS_CHANGE_DATA_TYPE_SET_MAIN_RESOLUTION:
                {
                    m_pSink->ChangeData(eDataType, pData);
                    LOG_MM_I(COLLECTOR_OBS, "Change data type(%d) from Grp(%d)Chn(%d) of %d.",
                                             eDataType, nGrp, nChn, eTarget);

                }
                break;
            default:
                LOG_MM_W(COLLECTOR_OBS, "Unsupport eDataType: %d", eDataType);
                break;
        }
        return AX_TRUE;
    }

private:
    CFrameCollector* m_pSink{nullptr};
    AX_U32 m_nTargetPipeChannel{0};
};

}  // namespace AX_IPC
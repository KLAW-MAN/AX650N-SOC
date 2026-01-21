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

#include "Capture.hpp"
#include "IObserver.h"

#define MAX_CAP_GRP (32)

class CAXFrame;


class CCaptureObserver : public IObserver {
public:
    CCaptureObserver(CCapture* pSink) : m_pSink(pSink) {
        m_nSkipFrm = m_pSink->GetSkipFrm();
        memset(m_nSeqNum, 0x0, sizeof(m_nSeqNum));
    };
    ~CCaptureObserver(AX_VOID) = default;

public:
    AX_BOOL OnRecvData(OBS_TARGET_TYPE_E eTarget, AX_U32 nGrp, AX_U32 nChn, AX_VOID* pData) override {
        CAXFrame* paxFrame = static_cast<CAXFrame*>(pData);
        if (SkipFrame(nGrp, paxFrame)) {
            return AX_TRUE;
        }
        else {
            return m_pSink->SendFrame(nGrp, paxFrame);
        }
    }

    AX_BOOL OnRegisterObserver(OBS_TARGET_TYPE_E eTarget, AX_U32 nGrp, AX_U32 nChn, OBS_TRANS_ATTR_PTR pParams) override {
        return AX_TRUE;
    }

protected:
    AX_BOOL SkipFrame(AX_U32 nGrp, CAXFrame* paxFrame) {
        return (0 == (++m_nSeqNum[nGrp] % m_nSkipFrm)) ? AX_FALSE : AX_TRUE;
    }

private:
    CCapture* m_pSink{nullptr};
    AX_U64 m_nSeqNum[MAX_CAP_GRP];
    AX_U32 m_nSkipFrm{0};
};

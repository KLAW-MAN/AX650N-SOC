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

#include <vector>
#include <list>
#include <map>

#include "AXFrame.hpp"
#include "AXThread.hpp"
#include "IObserver.h"

#define MAX_CAPTURE_GRP (32)

typedef struct CAPTURE_ATTR_S {
    AX_U32 nGrpCount;
    AX_S32 nSkipFrm;

    CAPTURE_ATTR_S(AX_VOID) {
        nGrpCount = 1;
        nSkipFrm = 10000;
    }
} CAPTURE_ATTR_T;

/**
 * @brief
 *
 */

class CCapture {
public:
    CCapture(AX_VOID) = default;
    ~CCapture(AX_VOID) = default;

    AX_BOOL Init(AX_S32 nDevID, const CAPTURE_ATTR_T& stAttr);
    AX_BOOL DeInit(AX_VOID);

    AX_BOOL Start(AX_VOID);
    AX_BOOL Stop(AX_VOID);

    AX_BOOL SendFrame(AX_U32 nGrp, CAXFrame* axFrame);
    AX_U32  GetSkipFrm() const;

protected:
    AX_VOID CapturePictureThread(AX_VOID* pArg);

private:
    AX_VOID ResetCaptureStatus(AX_U32 nGrpIndex);
    AX_VOID InitData(AX_VOID);
    AX_VOID MakeSnapshotDir(AX_VOID);
    AX_VOID SaveSnapshotPic(CAXFrame* axFrame, AX_U32 nGrp, AX_U8 *pu8Addr, AX_U32 u32Len);

private:
    AX_S32 m_nDevID{0};
    AX_U32 m_nSkipFrm{0};
    AX_U32 m_nMaxDevVideoCount{0};
    AX_CHAR m_szTime[64];

    std::vector<CAXThread*> m_vecThreadCapture;
    std::map<AX_U32, std::tuple<AX_U32, AX_U32>> m_mapCaptureThreadParams;

    CAXFrame* m_pAXFrame[MAX_CAPTURE_GRP];
    std::mutex m_mutexStat[MAX_CAPTURE_GRP];
    std::mutex m_mutexCapture[MAX_CAPTURE_GRP];
    std::condition_variable m_cvCapture[MAX_CAPTURE_GRP];

    AX_BOOL m_bCapture[MAX_CAPTURE_GRP];
    AX_U8 m_nCaptureGrp[MAX_CAPTURE_GRP];
};

inline AX_U32 CCapture::GetSkipFrm(AX_VOID) const {
    return m_nSkipFrm;
}

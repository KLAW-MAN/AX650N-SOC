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

#include "AXFrame.hpp"
#include "AXLockQ.hpp"
#include "AXResource.hpp"
#include "AXThread.hpp"

#include "axcl_skel.h"

#define DETECTOR_MAX_CHN_NUM 3

typedef struct DETECTOR_CHN_ATTR_S {
    AX_U32 nPPL;
    AX_U32 nVNPU;
    AX_BOOL bTrackEnable;

    DETECTOR_CHN_ATTR_S(AX_VOID) {
        nPPL = AXCL_SKEL_PPL_HVCP;
        nVNPU = AXCL_SKEL_NPU_DEFAULT;
        bTrackEnable = AX_FALSE;
    }
} DETECTOR_CHN_ATTR_T;

typedef struct DETECTOR_ATTR_S {
    AX_U32 nGrpCount;
    AX_S32 nSkipRate;
    AX_U32 nW;
    AX_U32 nH;
    AX_S32 nDepth;
    AX_U32 nChannelNum;
    DETECTOR_CHN_ATTR_T tChnAttr[DETECTOR_MAX_CHN_NUM];
    AX_CHAR szModelPath[128];

    DETECTOR_ATTR_S(AX_VOID) {
        nGrpCount = 1;
        nSkipRate = 1;
        nW = 0;
        nH = 0;
        nDepth = 1;
        nChannelNum = 1;
        memset(szModelPath, 0, sizeof(szModelPath));
    }
} DETECTOR_ATTR_T;

typedef struct {
    AX_U64 nSeqNum;
    AX_S32 nGrpId;
    AX_S32 nChnId;
    AX_U32 nSkelChn;
} SKEL_FRAME_PRIVATE_DATA_T;

/**
 * @brief
 *
 */
class CDetector {
public:
    CDetector(AX_VOID) = default;

    AX_BOOL Init(AX_S32 nDevID, const DETECTOR_ATTR_T& stAttr);
    AX_BOOL DeInit(AX_VOID);

    AX_BOOL InitSkel();
    AX_BOOL DeInitSkel();

    AX_BOOL Start(AX_VOID);
    AX_BOOL Stop(AX_VOID);
    AX_BOOL Clear(AX_VOID);

    AX_BOOL SendFrame(const CAXFrame& axFrame);

    AX_VOID ReleaseSkelPrivateData(SKEL_FRAME_PRIVATE_DATA_T* pData) {
        m_skelData.giveback(pData);
    }

protected:
    AX_BOOL SkipFrame(const CAXFrame& axFrame);
    AX_VOID RunDetect(AX_VOID* pArg);
    AX_VOID ClearQueue(AX_S32 nGrp);

protected:
    AX_S32 m_nDevID{0};

    CAXLockQ<CAXFrame>* m_arrFrameQ{nullptr};
    DETECTOR_ATTR_T m_stAttr;
    CAXThread m_DetectThread;

    AXCL_SKEL_HANDLE m_hSkel[DETECTOR_MAX_CHN_NUM]{NULL};
    std::mutex m_mtxSkel;
    CAXResource<SKEL_FRAME_PRIVATE_DATA_T> m_skelData;
};

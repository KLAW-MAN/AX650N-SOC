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

#include <string.h>
#include <atomic>
#include <condition_variable>
#include <list>
#include <map>
#include <memory>
#include <mutex>

#include "ax_ivps_api.h"

#include "AXThread.hpp"
#include "AXFrame.hpp"
#include "lock_queue.hpp"
#include "IObserver.h"
#include "haltype.hpp"
#include "Vo.hpp"

#define MAX_IVPS_GRP_NUM (AX_IVPS_MAX_GRP_NUM)
#define MAX_IVPS_CHN_NUM (AX_IVPS_MAX_OUTCHN_NUM)
#define INVALID_IVPS_GRP (-1)
#define INVALID_IVPS_CHN (-1)

using AX_IVPS_GRP = IVPS_GRP;

typedef struct IVPS_CHN_ATTR_S {
    AX_BOOL bEngage;
    AX_BOOL bLinked; /* static param */
    AX_U32 nFifoDepth;
    AX_IVPS_ENGINE_E enEngine;
    AX_U32 nWidth;
    AX_U32 nHeight;
    AX_U32 nStride;
    AX_FRAME_RATE_CTRL_T stFRC;
    AX_IMG_FORMAT_E enImgFormat;
    AX_BOOL bCrop;
    AX_IVPS_RECT_T stCropRect;
    AX_IVPS_ASPECT_RATIO_T stAspectRatio;
    AX_FRAME_COMPRESS_INFO_T stCompressInfo;
    AX_IVPS_POOL_ATTR_T stPoolAttr;

    IVPS_CHN_ATTR_S(AX_VOID) {
        memset(this, 0, sizeof(*this));
        enEngine = AX_IVPS_ENGINE_TDP;
        bEngage = AX_TRUE;
        bLinked = AX_TRUE;
        enImgFormat = AX_FORMAT_YUV420_SEMIPLANAR;
        stPoolAttr.ePoolSrc = POOL_SOURCE_PRIVATE;
        stPoolAttr.nFrmBufNum = 3;
    }
} IVPS_CHN_ATTR_T;

typedef struct IVPS_ATTR_S {
    AX_IVPS_GRP nGrpId;
    AX_IVPS_ENGINE_E eEngineType;
    AX_U32 nInDepth;
    AX_U32 nBackupInDepth;
    AX_U32 nChnNum;
    IVPS_CHN_ATTR_T stChnAttr[MAX_IVPS_CHN_NUM];
    IVPS_ATTR_S(AX_VOID) {
        nGrpId = INVALID_IVPS_GRP; /* INVALID_IVPS_GRP: auto allocate grp id */
        eEngineType = AX_IVPS_ENGINE_VPP;
        nInDepth = 2;
        nChnNum = 0;
        /*
            playback: nBackupInDepth = AX_VO_CHN_ATTR_T.u32FifoDepth + 1
            preview : nBackupInDepth = 0
        */
        nBackupInDepth = 0;
    }
} IVPS_ATTR_T;

class CIVPSDispatcher;
class CVo;
class CIVPS {
public:
    CIVPS(AX_VOID) = default;

    static CIVPS* CreateInstance(AX_S32 nDevID, CONST IVPS_ATTR_T& stAttr);
    AX_BOOL Destory(AX_VOID);

    AX_BOOL Start(AX_VOID);
    AX_BOOL Stop(AX_VOID);

    AX_BOOL ResetGrp(AX_VOID);

    /* static param is not updated */
    AX_BOOL UpdateChnAttr(AX_S32 ivChn, CONST IVPS_CHN_ATTR_T& stChnAttr);

    /* crop and resize for grp */
    AX_BOOL CropResize(AX_BOOL bCrop, CONST AX_IVPS_RECT_T& stCropRect);

    AX_BOOL EnableChn(AX_S32 ivChn);
    AX_BOOL DisableChn(AX_S32 ivChn);

    AX_BOOL SendFrame(CONST AX_VIDEO_FRAME_T& stVFrame, AX_S32 nTimeOut = INFINITE);

    AX_BOOL RegisterObserver(AX_S32 ivChn, IObserver* pObs);
    AX_BOOL UnRegisterObserver(AX_S32 ivChn, IObserver* pObs);

    AX_BOOL BindVo(AX_S32 ivChn, std::shared_ptr<CVo> pVo);

    CONST IVPS_ATTR_T& GetAttr(AX_VOID) CONST;
    AX_IVPS_GRP GetGrpId(AX_VOID) CONST;

    AX_BOOL Init(AX_S32 nDevID, CONST IVPS_ATTR_T& stAttr);
    AX_BOOL DeInit(AX_VOID);

protected:
    AX_BOOL CheckAttr(CONST IVPS_ATTR_T& stAttr);
    AX_VOID SetPipeFilterAttr(AX_IVPS_PIPELINE_ATTR_T& stPipeAttr, AX_S32 ivChn, CONST IVPS_CHN_ATTR_T& stChnAttr);

private:
    CIVPS(CONST CIVPS&) = delete;
    CIVPS& operator=(CONST CIVPS&) = delete;

private:
    AX_S32 m_nDevID{0};
    IVPS_ATTR_T m_stAttr;
    IVPS_GRP m_ivGrp = {INVALID_IVPS_GRP};
    std::atomic<AX_BOOL> m_bStarted = {AX_FALSE};

    /* fixme: select/epoll can be replaced to decrease threads */
    std::unique_ptr<CIVPSDispatcher> m_dispatchers[MAX_IVPS_CHN_NUM];
};

inline CONST IVPS_ATTR_T& CIVPS::GetAttr(AX_VOID) CONST {
    return m_stAttr;
}

inline AX_IVPS_GRP CIVPS::GetGrpId(AX_VOID) CONST {
    return m_ivGrp;
}

class CIVPSDispatcher {
public:
    CIVPSDispatcher(AX_S32 nDevID, IVPS_GRP ivGrp, IVPS_CHN ivChn) noexcept;

    AX_BOOL Start(AX_VOID);
    AX_BOOL Stop(AX_VOID);

    AX_VOID Paused(AX_VOID);
    AX_VOID Resume(AX_VOID);

    AX_VOID ClearQ(AX_VOID);

    AX_VOID RegisterObserver(IObserver* pObs);
    AX_VOID UnRegisterObserver(IObserver* pObs);
    AX_VOID BindVo(std::shared_ptr<CVo> pVo) {
        m_pVo = pVo;
    };

protected:
    AX_VOID DispatchThread(AX_VOID* pArg);
    AX_VOID SendThread(AX_VOID* pArg);
    AX_VOID Wait(AX_VOID);
    AX_BOOL OnRecvFrame(CONST AX_VIDEO_FRAME_T& stVFrame);

private:
    AX_S32 m_nDevID{0};
    IVPS_GRP m_ivGrp = {INVALID_IVPS_GRP};
    IVPS_CHN m_ivChn = {INVALID_IVPS_CHN};
    std::list<IObserver*> m_lstObs;
    std::mutex m_mtxObs;
    std::mutex m_mtx;
    std::condition_variable m_cv;
    CAXThread m_thread;
    CAXThread m_threadSnd;
    AX_BOOL m_bPaused = {AX_FALSE};
    AX_BOOL m_bStarted = {AX_FALSE};
    axcl::lock_queue<AX_VIDEO_FRAME_T>* m_qFrameQ{nullptr};
    std::shared_ptr<CVo> m_pVo;
};

/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include "AxclIvps.hpp"

#include <string.h>
#include <algorithm>
#include <chrono>
#include <exception>

#include "AXFrame.hpp"
#include "AppLogApi.h"

#include "axcl_rt_context.h"
#include "axcl_rt_memory.h"
#include "axcl_sys.h"
#include "axcl_ivps.h"

using namespace std;

#define TAG "AXCL_IVPS"
#define CHECK_IVPS_GRP(ivGrp) ((ivGrp) >= 0 && (ivGrp) < MAX_IVPS_GRP_NUM)
#define CHECK_IVPS_CHN(ivChn) ((ivChn) >= 0 && (ivChn) < MAX_IVPS_CHN_NUM)

#define AXCLIVPS_FIFO_DEPTH (4)

class CIvpsManager {
public:
    CIvpsManager(AX_VOID) noexcept {
        for (AX_U32 i = 0; i < MAX_IVPS_GRP_NUM; ++i) {
            m_mapIds[i] = AX_FALSE;
        }
    }

    AX_IVPS_GRP Request(AX_IVPS_GRP ivGrp = INVALID_IVPS_GRP) {
        std::lock_guard<std::mutex> lck(m_mtx);

        if (ivGrp >= 0 && ivGrp < MAX_IVPS_GRP_NUM) {
            if (m_mapIds[ivGrp]) {
                LOG_M_W(TAG, "ivGrp %d is already used", ivGrp);
            } else {
                m_mapIds[ivGrp] = AX_TRUE;
                return ivGrp;
            }
        }

        for (auto&& kv : m_mapIds) {
            if (!kv.second) {
                kv.second = AX_TRUE;
                return kv.first;
            }
        }

        return INVALID_IVPS_GRP;
    }

    AX_BOOL Giveback(AX_IVPS_GRP& ivGrp) {
        if (!CHECK_IVPS_GRP(ivGrp)) {
            return AX_FALSE;
        }

        std::lock_guard<std::mutex> lck(m_mtx);
        m_mapIds[ivGrp] = AX_FALSE;
        ivGrp = INVALID_IVPS_GRP;

        return AX_TRUE;
    }

private:
    std::map<AX_IVPS_GRP, AX_BOOL> m_mapIds;
    std::mutex m_mtx;
};

static CIvpsManager theIvpsManager;

CIVPS* CIVPS::CreateInstance(AX_S32 nDevID, CONST IVPS_ATTR_T& stAttr) {
    CIVPS* obj = new (std::nothrow) CIVPS;
    if (obj) {
        if (obj->Init(nDevID, stAttr)) {
            return obj;
        } else {
            delete obj;
            obj = nullptr;
        }
    }

    return nullptr;
}

AX_BOOL CIVPS::Destory(AX_VOID) {
    if (!DeInit()) {
        return AX_FALSE;
    }

    delete this;
    return AX_TRUE;
}

AX_BOOL CIVPS::Init(AX_S32 nDevID, CONST IVPS_ATTR_T& stAttr) {
    if (!CheckAttr(stAttr)) {
        return AX_FALSE;
    }

    m_nDevID = nDevID;
    m_stAttr = stAttr;

    DeInit();

    m_ivGrp = theIvpsManager.Request(stAttr.nGrpId);
    if (INVALID_IVPS_GRP == m_ivGrp) {
        LOG_M_E(TAG, "%s: no free ivGrp ID", __func__);
        return AX_FALSE;
    }

    do {
        AX_IVPS_GRP_ATTR_T stGrpAttr;
        memset(&stGrpAttr, 0, sizeof(stGrpAttr));
        stGrpAttr.nInFifoDepth = stAttr.nInDepth;
        stGrpAttr.ePipeline = AX_IVPS_PIPELINE_DEFAULT;
        AX_S32 ret = AXCL_IVPS_CreateGrp(m_ivGrp, &stGrpAttr);
        if (0 != ret) {
            LOG_M_E(TAG, "AXCL_IVPS_CreateGrp(ivGrp %d) fail, ret = 0x%x", m_ivGrp, ret);
            theIvpsManager.Giveback(m_ivGrp);
            break;
        }

        AX_BOOL bCreated = AX_TRUE;
        for (AX_U32 i = 0; i < stAttr.nChnNum; ++i) {
            if (!stAttr.stChnAttr[i].bLinked) {
                m_dispatchers[i] = std::make_unique<CIVPSDispatcher>(m_nDevID, m_ivGrp, (IVPS_CHN)i);
                if (!m_dispatchers[i]) {
                    bCreated = AX_FALSE;
                    break;
                }
            }
        }
        if (!bCreated) {
            break;
        }

        return AX_TRUE;

    } while (0);

    DeInit();
    return AX_FALSE;
}

AX_BOOL CIVPS::DeInit(AX_VOID) {
    if (!CHECK_IVPS_GRP(m_ivGrp)) {
        return AX_TRUE;
    }

#ifdef __DEBUG_STOP__
    LOG_M_C(TAG, "%s: ivGrp %d +++", __func__, m_ivGrp);
#endif

    AX_S32 ret = AXCL_IVPS_DestoryGrp(m_ivGrp);
    if (0 != ret) {
        LOG_M_E(TAG, "AXCL_IVPS_DestoryGrp(ivGrp %d) fail, ret = 0x%x", m_ivGrp, ret);
        return AX_FALSE;
    }

    for (auto&& m : m_dispatchers) {
        m = nullptr;
    }

#ifdef __DEBUG_STOP__
    LOG_M_C(TAG, "%s: ivGrp %d ---", __func__, m_ivGrp);
#endif

    theIvpsManager.Giveback(m_ivGrp);
    return AX_TRUE;
}

AX_BOOL CIVPS::Start(AX_VOID) {
    LOG_M_I(TAG, "%s: ivGrp %d +++", __func__, m_ivGrp);

    if (m_bStarted) {
        LOG_M_W(TAG, "IVPS ivGrp %d is already running", m_ivGrp);
        return AX_TRUE;
    }

    AX_IVPS_PIPELINE_ATTR_T stPipeAttr;
    memset(&stPipeAttr, 0, sizeof(stPipeAttr));
    stPipeAttr.nOutChnNum = m_stAttr.nChnNum;
    for (AX_U32 i = 0; i < m_stAttr.nChnNum; ++i) {
        SetPipeFilterAttr(stPipeAttr, i, m_stAttr.stChnAttr[i]);
    }

    AX_S32 ret = AXCL_IVPS_SetPipelineAttr(m_ivGrp, &stPipeAttr);
    if (0 != ret) {
        LOG_M_E(TAG, "AXCL_IVPS_SetPipelineAttr(ivGrp %d) fail, ret = 0x%x", m_ivGrp, ret);
        return AX_FALSE;
    } else {
        LOG_MM_I(TAG, "[%d] AXCL_IVPS_SetPipelineAttr OK", m_ivGrp);
    }

    auto disableChns = [this](AX_U32 nChns) -> AX_VOID {
        for (AX_U32 j = 0; j < nChns; ++j) {
            AXCL_IVPS_DisableChn(m_ivGrp, (IVPS_CHN)j);
        }
    };
    for (AX_U32 i = 0; i < m_stAttr.nChnNum; ++i) {
        ret = AXCL_IVPS_SetChnPoolAttr(m_ivGrp, (IVPS_CHN)i, &m_stAttr.stChnAttr[i].stPoolAttr);
        if (0 != ret) {
            LOG_M_E(TAG, "AXCL_IVPS_SetChnPoolAttr(ivGrp %d ivChn %d) fail, ret = 0x%x", m_ivGrp, i, ret);
            if (i > 0) {
                disableChns(i);
            }
            return AX_FALSE;
        }

        ret = AXCL_IVPS_EnableChn(m_ivGrp, (IVPS_CHN)i);
        if (0 != ret) {
            LOG_M_E(TAG, "AXCL_IVPS_EnableChn(ivGrp %d ivChn %d) fail, ret = 0x%x", m_ivGrp, i, ret);
            if (i > 0) {
                disableChns(i);
            }
            return AX_FALSE;
        } else {
            LOG_MM_I(TAG, "[%d] AXCL_IVPS_EnableChn OK", m_ivGrp);
        }
    }

    if (m_stAttr.nBackupInDepth > 0) {
        ret = AXCL_IVPS_EnableBackupFrame(m_ivGrp, m_stAttr.nBackupInDepth);
        if (0 != ret) {
            LOG_M_E(TAG, "AXCL_IVPS_EnableBackupFrame(ivGrp %d, depth %d) fail, ret = 0x%x", m_ivGrp, m_stAttr.nBackupInDepth, ret);
            disableChns(m_stAttr.nChnNum);
            return AX_FALSE;
        }
    }

    ret = AXCL_IVPS_StartGrp(m_ivGrp);
    if (0 != ret) {
        LOG_M_E(TAG, "AXCL_IVPS_StartGrp(ivGrp %d) fail, ret = 0x%x", m_ivGrp, ret);
        disableChns(m_stAttr.nChnNum);
        return AX_FALSE;
    } else {
        LOG_MM_I(TAG, "[%d] AXCL_IVPS_StartGrp OK", m_ivGrp);
    }

    for (auto&& m : m_dispatchers) {
        if (m) {
            if (!m->Start()) {
                disableChns(m_stAttr.nChnNum);
                AXCL_IVPS_StopGrp(m_ivGrp);
                return AX_FALSE;
            } else {
                LOG_MM_I(TAG, "[%d] Start dispatcher OK", m_ivGrp);
            }
        }
    }

    m_bStarted = AX_TRUE;

    LOG_M_I(TAG, "%s: ivGrp %d ---", __func__, m_ivGrp);
    return AX_TRUE;
}

AX_BOOL CIVPS::Stop(AX_VOID) {
#ifdef __DEBUG_STOP__
    LOG_M_C(TAG, "%s: ivGrp %d +++", __func__, m_ivGrp);
#else
    LOG_M_I(TAG, "%s: ivGrp %d +++", __func__, m_ivGrp);
#endif

    if (!m_bStarted) {
        return AX_TRUE;
    }

    for (auto&& m : m_dispatchers) {
        if (m) {
            if (!m->Stop()) {
                return AX_FALSE;
            }
        }
    }

    AX_S32 ret;
    for (AX_U32 i = 0; i < m_stAttr.nChnNum; ++i) {
        ret = AXCL_IVPS_DisableChn(m_ivGrp, (IVPS_CHN)i);
        if (0 != ret) {
            LOG_M_E(TAG, "AXCL_IVPS_DisableChn(ivGrp %d ivChn %d) fail, ret = 0x%x", m_ivGrp, i, ret);
            return AX_FALSE;
        }
    }

    if (m_stAttr.nBackupInDepth > 0) {
        ret = AXCL_IVPS_DisableBackupFrame(m_ivGrp);
        if (0 != ret) {
            LOG_M_E(TAG, "AXCL_IVPS_DisableBackupFrame(ivGrp %d) fail, ret = 0x%x", m_ivGrp, ret);
            return AX_FALSE;
        }
    }

    ret = AXCL_IVPS_StopGrp(m_ivGrp);
    if (0 != ret) {
        LOG_M_E(TAG, "AXCL_IVPS_StopGrp(ivGrp %d) fail, ret = 0x%x", m_ivGrp, ret);
        return AX_FALSE;
    }

    m_bStarted = AX_FALSE;

#ifdef __DEBUG_STOP__
    LOG_M_C(TAG, "%s: ivGrp %d ---", __func__, m_ivGrp);
#else
    LOG_M_I(TAG, "%s: ivGrp %d ---", __func__, m_ivGrp);
#endif
    return AX_TRUE;
}

AX_BOOL CIVPS::ResetGrp(AX_VOID) {
    AX_S32 ret = AXCL_IVPS_ResetGrp(m_ivGrp);
    if (0 != ret) {
        LOG_M_E(TAG, "AXCL_IVPS_ResetGrp(ivGrp %d) fail, ret = 0x%x", m_ivGrp, ret);
        return AX_FALSE;
    }

    LOG_M_N(TAG, "ivGrp %d is reset", m_ivGrp);
    return AX_TRUE;
}

AX_BOOL CIVPS::UpdateChnAttr(AX_S32 ivChn, CONST IVPS_CHN_ATTR_T& stChnAttr) {
    if (!CHECK_IVPS_CHN(ivChn)) {
        LOG_M_E(TAG, "%s: invalid ivChn %d", __func__, ivChn);
        return AX_FALSE;
    }

    if (!m_bStarted) {
        LOG_M_E(TAG, "%s: ivGrp %d is not started", __func__, m_ivGrp);
        return AX_FALSE;
    }

    if (m_dispatchers[ivChn]) {
        m_dispatchers[ivChn]->Paused();
    }

    AX_S32 ret;
    ret = AXCL_IVPS_DisableChn(m_ivGrp, ivChn);
    if (0 != ret) {
        LOG_M_E(TAG, "AXCL_IVPS_DisableChn(ivGrp %d ivChn %d) fail, ret = 0x%x", m_ivGrp, ivChn, ret);
        return AX_FALSE;
    }

    AX_IVPS_PIPELINE_ATTR_T stPipeAttr;
    ret = AXCL_IVPS_GetPipelineAttr(m_ivGrp, &stPipeAttr);
    if (0 != ret) {
        LOG_M_E(TAG, "%s: AXCL_IVPS_GetPipelineAttr(ivGrp %d) fail, ret = 0x%x", __func__, m_ivGrp, ret);
        return AX_FALSE;
    }

    SetPipeFilterAttr(stPipeAttr, ivChn, stChnAttr);

    ret = AXCL_IVPS_SetPipelineAttr(m_ivGrp, &stPipeAttr);
    if (0 != ret) {
        LOG_M_E(TAG, "AXCL_IVPS_SetPipelineAttr(ivGrp %d) fail, ret = 0x%x", m_ivGrp, ret);
        return AX_FALSE;
    }

    ret = AXCL_IVPS_SetChnPoolAttr(m_ivGrp, ivChn, &stChnAttr.stPoolAttr);
    if (0 != ret) {
        LOG_M_E(TAG, "AXCL_IVPS_SetChnPoolAttr(ivGrp %d ivChn %d) fail, ret = 0x%x", m_ivGrp, ivChn, ret);
        return AX_FALSE;
    }

    AX_BOOL bLinked = m_stAttr.stChnAttr[ivChn].bLinked;
    m_stAttr.stChnAttr[ivChn] = stChnAttr;
    m_stAttr.stChnAttr[ivChn].bLinked = bLinked;

    ret = AXCL_IVPS_EnableChn(m_ivGrp, ivChn);
    if (0 != ret) {
        LOG_M_E(TAG, "AXCL_IVPS_EnableChn(ivGrp %d ivChn %d) fail, ret = 0x%x", m_ivGrp, ivChn, ret);
        return AX_FALSE;
    }

    if (m_stAttr.nBackupInDepth > 0) {
        ret = AXCL_IVPS_EnableBackupFrame(m_ivGrp, m_stAttr.nBackupInDepth);
        if (0 != ret) {
            LOG_M_E(TAG, "AXCL_IVPS_EnableBackupFrame(ivGrp %d, depth %d) fail, ret = 0x%x", m_ivGrp, m_stAttr.nBackupInDepth, ret);
            AXCL_IVPS_DisableChn(m_ivGrp, ivChn);
            return AX_FALSE;
        }
    }

    if (m_dispatchers[ivChn]) {
        m_dispatchers[ivChn]->Resume();
    }

    return AX_TRUE;
}

AX_BOOL CIVPS::EnableChn(AX_S32 ivChn) {
    if (!CHECK_IVPS_CHN(ivChn)) {
        LOG_M_E(TAG, "%s: invalid ivChn %d", __func__, ivChn);
        return AX_FALSE;
    }

    if (!m_bStarted) {
        LOG_M_E(TAG, "%s: ivGrp %d is not started", __func__, m_ivGrp);
        return AX_FALSE;
    }

    AX_S32 ret = AXCL_IVPS_EnableChn(m_ivGrp, ivChn);
    if (0 != ret) {
        LOG_M_E(TAG, "AXCL_IVPS_EnableChn(ivGrp %d ivChn %d) fail, ret = 0x%x", m_ivGrp, ivChn, ret);
        return AX_FALSE;
    }

    if (m_dispatchers[ivChn]) {
        m_dispatchers[ivChn]->Resume();
    }

    return AX_TRUE;
}

AX_BOOL CIVPS::DisableChn(AX_S32 ivChn) {
    if (!CHECK_IVPS_CHN(ivChn)) {
        LOG_M_E(TAG, "%s: invalid ivChn %d", __func__, ivChn);
        return AX_FALSE;
    }

    if (!m_bStarted) {
        LOG_M_E(TAG, "%s: ivGrp %d is not started", __func__, m_ivGrp);
        return AX_FALSE;
    }

    if (m_dispatchers[ivChn]) {
        m_dispatchers[ivChn]->Paused();
    }

    AX_S32 ret = AXCL_IVPS_DisableChn(m_ivGrp, ivChn);
    if (0 != ret) {
        LOG_M_E(TAG, "AXCL_IVPS_DisableChn(ivGrp %d ivChn %d) fail, ret = 0x%x", m_ivGrp, ivChn, ret);
        return AX_FALSE;
    }

    return AX_TRUE;
}

AX_BOOL CIVPS::CropResize(AX_BOOL bCrop, CONST AX_IVPS_RECT_T& stCropRect) {
    if (!m_bStarted) {
        LOG_M_E(TAG, "%s: ivGrp %d is not started yet", __func__, m_ivGrp);
        return AX_FALSE;
    }

    AX_IVPS_CROP_INFO_T stInfo;
    stInfo.bEnable = bCrop;
    if (bCrop) {
        stInfo.eCoordMode = AX_COORD_ABS;
        stInfo.tCropRect = stCropRect;
    } else {
        stInfo.tCropRect = {0, 0, 0, 0};
    }

    AX_S32 ret = AXCL_IVPS_SetGrpCrop(m_ivGrp, &stInfo);
    if (0 != ret) {
        LOG_M_E(TAG, "AXCL_IVPS_SetGrpCrop(ivGrp %d %d [x %d, y %d, w %d, h %d]) fail, ret = 0x%x", m_ivGrp, stInfo.bEnable,
                stInfo.tCropRect.nX, stInfo.tCropRect.nY, stInfo.tCropRect.nW, stInfo.tCropRect.nH, ret);
        return AX_FALSE;
    }

    return AX_TRUE;
}

AX_VOID CIVPS::SetPipeFilterAttr(AX_IVPS_PIPELINE_ATTR_T& stPipeAttr, AX_S32 ivChn, CONST IVPS_CHN_ATTR_T& stChnAttr) {
    stPipeAttr.nOutFifoDepth[ivChn] = stChnAttr.nFifoDepth;

    AX_IVPS_FILTER_T& stChnFilter0 = stPipeAttr.tFilter[ivChn + 1][0];

    stChnFilter0.bEngage = stChnAttr.bEngage;
    stChnFilter0.eEngine = stChnAttr.enEngine;
    stChnFilter0.tFRC = stChnAttr.stFRC;
    stChnFilter0.bCrop = stChnAttr.bCrop;
    stChnFilter0.tCropRect = stChnAttr.stCropRect;
    stChnFilter0.nDstPicWidth = stChnAttr.nWidth;
    stChnFilter0.nDstPicHeight = stChnAttr.nHeight;
    stChnFilter0.nDstPicStride = stChnAttr.nStride;
    stChnFilter0.eDstPicFormat = stChnAttr.enImgFormat;
    stChnFilter0.tCompressInfo = stChnAttr.stCompressInfo;
    stChnFilter0.tAspectRatio = stChnAttr.stAspectRatio;
}

AX_BOOL CIVPS::RegisterObserver(AX_S32 ivChn, IObserver* pObs) {
    if (!CHECK_IVPS_CHN(ivChn)) {
        LOG_M_E(TAG, "%s: ivChn %d of ivGrp %d is invalid", __func__, ivChn, m_ivGrp);
        return AX_FALSE;
    }

    if (!pObs) {
        LOG_M_E(TAG, "%s: observer of ivGrp %d ivChn %d is nil", __func__, m_ivGrp, ivChn);
        return AX_FALSE;
    }

    if (!m_dispatchers[ivChn]) {
        LOG_M_E(TAG, "%s: dispatcher of ivGrp %d ivChn %d is nil", __func__, m_ivGrp, ivChn);
        return AX_FALSE;
    }

    m_dispatchers[ivChn]->RegisterObserver(pObs);
    return AX_TRUE;
}

AX_BOOL CIVPS::UnRegisterObserver(AX_S32 ivChn, IObserver* pObs) {
    if (!CHECK_IVPS_CHN(ivChn)) {
        LOG_M_E(TAG, "%s: ivChn %d of ivGrp %d is invalid", __func__, ivChn, m_ivGrp);
        return AX_FALSE;
    }

    if (!pObs) {
        LOG_M_E(TAG, "%s: observer of ivGrp %d ivChn %d is nil", __func__, m_ivGrp, ivChn);
        return AX_FALSE;
    }

    if (!m_dispatchers[ivChn]) {
        LOG_M_E(TAG, "%s: dispatcher of ivGrp %d ivChn %d is nil", __func__, m_ivGrp, ivChn);
        return AX_FALSE;
    }

    m_dispatchers[ivChn]->UnRegisterObserver(pObs);
    return AX_TRUE;
}

AX_BOOL CIVPS::BindVo(AX_S32 ivChn, std::shared_ptr<CVo> pVo) {
    if (!CHECK_IVPS_CHN(ivChn)) {
        LOG_M_E(TAG, "%s: ivChn %d of ivGrp %d is invalid", __func__, ivChn, m_ivGrp);
        return AX_FALSE;
    }

    m_dispatchers[ivChn]->BindVo(pVo);
    return AX_TRUE;
}

AX_BOOL CIVPS::SendFrame(CONST AX_VIDEO_FRAME_T& stVFrame, AX_S32 nTimeOut /* = INFINITE */) {
    if (!CHECK_IVPS_GRP(m_ivGrp)) {
        LOG_M_E(TAG, "%s: ivps is destoryed", __func__);
        return AX_FALSE;
    }

    if (!m_bStarted) {
        LOG_M_E(TAG, "%s: ivGrp %d is not started yet", __func__, m_ivGrp);
        return AX_FALSE;
    }

    AX_S32 ret = AXCL_IVPS_SendFrame(m_ivGrp, &stVFrame, nTimeOut);
    if (0 != ret) {
        LOG_M_E(TAG, "AXCL_IVPS_SendFrame(frame %lld, ivGrp %d) fail, ret = 0x%x", stVFrame.u64SeqNum, m_ivGrp, ret);
        return AX_FALSE;
    }

    return AX_TRUE;
}

AX_BOOL CIVPS::CheckAttr(CONST IVPS_ATTR_T& stAttr) {
    for (AX_U32 i = 0; i < stAttr.nChnNum; ++i) {
        CONST IVPS_CHN_ATTR_T& stChnAttr = stAttr.stChnAttr[i];

        if (!stChnAttr.bLinked && (0 == stChnAttr.nFifoDepth)) {
            LOG_M_E(TAG, "output ivChn %d is unlinked, but out fifo depth is 0", i);
            return AX_FALSE;
        }

        if (stChnAttr.bCrop) {
            CONST AX_IVPS_RECT_T& rc = stChnAttr.stCropRect;
            if (((rc.nX + rc.nW) > (AX_U16)stChnAttr.nWidth) || ((rc.nY + rc.nH) > (AX_U16)stChnAttr.nHeight)) {
                LOG_M_E(TAG, "ivChn %d crop rect [%d %d %dx%d] is invalid", i, rc.nX, rc.nY, rc.nW, rc.nH);
                return AX_FALSE;
            }
        }

        switch (stChnAttr.stPoolAttr.ePoolSrc) {
            case POOL_SOURCE_PRIVATE:
                if (0 == stChnAttr.stPoolAttr.nFrmBufNum) {
                    LOG_M_E(TAG, "ivChn %d is private pool, but vb cnt is 0", i);
                    return AX_FALSE;
                }
                break;
            case POOL_SOURCE_USER:
                if (AX_INVALID_POOLID == stChnAttr.stPoolAttr.PoolId) {
                    LOG_M_E(TAG, "ivChn %d is user pool, but pool id is invalid", i);
                    return AX_FALSE;
                }
                break;
            default:
                break;
        }
    }

    return AX_TRUE;
}

CIVPSDispatcher::CIVPSDispatcher(AX_S32 nDevID, IVPS_GRP ivGrp, IVPS_CHN ivChn) noexcept : m_nDevID(nDevID), m_ivGrp(ivGrp), m_ivChn(ivChn) {
    m_qFrameQ = new (nothrow) axcl::lock_queue<AX_VIDEO_FRAME_T>;
    m_qFrameQ->set_capacity(AXCLIVPS_FIFO_DEPTH);
}

AX_VOID CIVPSDispatcher::DispatchThread(AX_VOID*) {
    LOG_M_D(TAG, "%s: %d +++", __func__, m_ivGrp);

    axclrtContext context;
    if (axclError ret = axclrtCreateContext(&context, m_nDevID); AXCL_SUCC != ret) {
        return;
    }

    AX_VIDEO_FRAME_T stVFrame;
    AX_S32 ret;
    CONSTEXPR AX_S32 TIMEOUT = 100;
    while (1) {
        Wait();

        if (!m_thread.IsRunning()) {
            break;
        }

        ret = AXCL_IVPS_GetChnFrame(m_ivGrp, m_ivChn, &stVFrame, TIMEOUT);
        if (0 != ret) {
            if (AX_ERR_IVPS_BUF_EMPTY != ret) {
                LOG_M_E(TAG, "AXCL_IVPS_GetChnFrame(ivGrp %d ivChn %d) fail, ret = 0x%x", m_ivGrp, m_ivChn, ret);
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            continue;
        }

        LOG_M_N(TAG, "received ivGrp %d ivChn %d frame %lld pts %lld phy 0x%llx %dx%d stride %d blkId 0x%x", m_ivGrp, m_ivChn,
                stVFrame.u64SeqNum, stVFrame.u64PTS, stVFrame.u64PhyAddr[0], stVFrame.u32Width, stVFrame.u32Height,
                stVFrame.u32PicStride[0], stVFrame.u32BlkId[0]);

        (AX_VOID) OnRecvFrame(stVFrame);

        ret = AXCL_IVPS_ReleaseChnFrame(m_ivGrp, m_ivChn, &stVFrame);
        if (0 != ret) {
            LOG_M_E(TAG, "AXCL_IVPS_ReleaseChnFrame(frame %lld, ivGrp %d, ivChn %d) fail, ret = 0x%x", stVFrame.u64SeqNum, m_ivGrp, m_ivChn,
                    ret);
            continue;
        }
    }

    axclrtDestroyContext(context);
    LOG_M_D(TAG, "%s: %d ---", __func__, m_ivGrp);
}

AX_VOID CIVPSDispatcher::SendThread(AX_VOID*) {
    LOG_M_D(TAG, "%s: %d +++", __func__, m_ivGrp);

    AX_S32 ret = 0;

    while (m_threadSnd.IsRunning()) {
        AX_VIDEO_FRAME_T stVFrame;
        if (!m_qFrameQ->pop(stVFrame)) {
            break;
        }

        CAXFrame axFrame;
        axFrame.nGrp = m_ivGrp;
        axFrame.nChn = m_ivChn;
        axFrame.stFrame.stVFrame.stVFrame = stVFrame;
        axFrame.stFrame.stVFrame.bEndOfStream = AX_FALSE;
        axFrame.stFrame.stVFrame.enModId = AX_ID_IVPS;

        std::lock_guard<std::mutex> lck(m_mtxObs);
        for (auto&& m : m_lstObs) {
            if (m) {
                (AX_VOID) m->OnRecvData(E_OBS_TARGET_TYPE_IVPS, m_ivGrp, m_ivChn, &axFrame);
            }
        }

        ret = AX_POOL_DecreaseRefCnt(stVFrame.u32BlkId[0]);
        if (0 != ret) {
            LOG_M_E(TAG, "%s: AXCL_POOL_DecreaseRefCnt(frame %lld, ivGrp %d) fail, ret = 0x%x",
                    __func__, stVFrame.u64SeqNum, m_ivGrp, ret);
        }
    }

    LOG_M_D(TAG, "%s: %d ---", __func__, m_ivGrp);
}

AX_BOOL CIVPSDispatcher::Start(AX_VOID) {
    if (m_bStarted) {
        LOG_M_W(TAG, "%s: dispatch thread of ivGrp %d is already started", __func__, m_ivGrp);
        return AX_TRUE;
    }

    AX_CHAR szName[16];
    sprintf(szName, "AppIvpsDisp%d", m_ivGrp);
    if (!m_thread.Start([this](AX_VOID* pArg) -> AX_VOID { DispatchThread(pArg); }, nullptr, szName)) {
        LOG_M_E(TAG, "start IVPS dispatch thread of ivGrp %d fail", m_ivGrp);
        return AX_FALSE;
    }

    AX_CHAR szNameSnd[16];
    sprintf(szNameSnd, "AppIvpsSend%d", m_ivGrp);
    if (!m_threadSnd.Start([this](AX_VOID* pArg) -> AX_VOID { SendThread(pArg); }, this, szNameSnd)) {
        LOG_M_E(TAG, "start IVPS send thread of ivGrp %d fail", m_ivGrp);
        return AX_FALSE;
    }

    m_bStarted = AX_TRUE;
    return AX_TRUE;
}

AX_BOOL CIVPSDispatcher::Stop(AX_VOID) {
    if (!m_bStarted) {
        return AX_TRUE;
    }

    m_thread.Stop();
    Resume();
    m_thread.Join();

    m_threadSnd.Stop();
    m_qFrameQ->wakeup();
    m_threadSnd.Join();
    ClearQ();

    m_bStarted = AX_FALSE;
    return AX_TRUE;
}

AX_VOID CIVPSDispatcher::RegisterObserver(IObserver* pObs) {
    std::lock_guard<std::mutex> lck(m_mtxObs);
    auto it = std::find(m_lstObs.begin(), m_lstObs.end(), pObs);
    if (it != m_lstObs.end()) {
        LOG_M_W(TAG, "%s: observer %p is already registed to ivGrp %d ivChn %d", __func__, pObs, m_ivGrp, m_ivChn);
    } else {
        m_lstObs.push_back(pObs);
        LOG_M_I(TAG, "%s: regist observer %p to ivGrp %d ivChn %d ok", __func__, pObs, m_ivGrp, m_ivChn);
    }
}

AX_VOID CIVPSDispatcher::UnRegisterObserver(IObserver* pObs) {
    std::lock_guard<std::mutex> lck(m_mtxObs);
    auto it = std::find(m_lstObs.begin(), m_lstObs.end(), pObs);
    if (it != m_lstObs.end()) {
        m_lstObs.remove(pObs);
        LOG_M_I(TAG, "%s: unregist observer %p from ivGrp %d ivChn %d ok", __func__, pObs, m_ivGrp, m_ivChn);
    } else {
        LOG_M_W(TAG, "%s: observer %p is not registed to ivGrp %d ivChn %d", __func__, pObs, m_ivGrp, m_ivChn);
    }
}

AX_BOOL CIVPSDispatcher::OnRecvFrame(CONST AX_VIDEO_FRAME_T& stVFrame) {
     AX_U64 u64PhysAddr;
     AX_U32 u32FrameSize = stVFrame.u32FrameSize;
     AX_U32 nPriPoolID = m_pVo->GetPriPoolID(m_ivGrp);
     AX_BLK BlkId = AX_POOL_GetBlock(nPriPoolID, u32FrameSize, NULL);

     if (AX_INVALID_BLOCKID != BlkId) {
        u64PhysAddr = AX_POOL_Handle2PhysAddr(BlkId);
        if (!u64PhysAddr) {
            LOG_M_E(TAG, "[%d] AX_POOL_Handle2PhysAddr failed,BlkId=0x%x", m_ivGrp, BlkId);
            return AX_FALSE;
        }

        axclrtMemcpy((AX_VOID *)u64PhysAddr, (AX_VOID *)stVFrame.u64PhyAddr[0], stVFrame.u32FrameSize, AXCL_MEMCPY_DEVICE_TO_HOST_PHY);

        AX_VIDEO_FRAME_T tFrame = stVFrame;

        memset(tFrame.u32BlkId, 0, sizeof(tFrame.u32BlkId));
        memset(tFrame.u64VirAddr, 0, sizeof(tFrame.u64VirAddr));
        memset(tFrame.u64PhyAddr, 0, sizeof(tFrame.u64PhyAddr));

        tFrame.u32BlkId[0] = BlkId;
        tFrame.u64PhyAddr[0] = u64PhysAddr;

        AX_S32 ret = AX_POOL_IncreaseRefCnt(tFrame.u32BlkId[0]);
        if (0 != ret) {
            LOG_M_E(TAG, "%s: AXCL_POOL_IncreaseRefCnt(frame %lld, ivGrp %d) fail, ret = 0x%x",
                    __func__, tFrame.u64SeqNum, m_ivGrp, ret);
        }
        else {
            if (!m_qFrameQ->push(tFrame)) {
                LOG_M_E(TAG, "%s: push frame %lld to q full", __func__, tFrame.u64SeqNum);
                ret = AX_POOL_DecreaseRefCnt(tFrame.u32BlkId[0]);
                if (0 != ret) {
                    LOG_M_E(TAG, "%s: AXCL_POOL_DecreaseRefCnt(frame %lld, ivGrp %d) fail, ret = 0x%x",
                            __func__, tFrame.u64SeqNum, m_ivGrp, ret);
                }
            }
        }

        AX_POOL_ReleaseBlock(BlkId);
        return AX_TRUE;
    } else {
        LOG_M_E(TAG, "[%d] AX_POOL_GetBlock failed.", m_ivGrp);
    }

    return AX_TRUE;
}

AX_VOID CIVPSDispatcher::Wait(AX_VOID) {
    std::unique_lock<std::mutex> lck(m_mtx);
    while (m_bPaused && m_thread.IsRunning()) {
        m_cv.wait(lck);
    }
}

AX_VOID CIVPSDispatcher::Paused(AX_VOID) {
    std::lock_guard<std::mutex> lck(m_mtx);
    m_bPaused = AX_TRUE;
}

AX_VOID CIVPSDispatcher::Resume(AX_VOID) {
    m_mtx.lock();
    m_bPaused = AX_FALSE;
    m_mtx.unlock();

    m_cv.notify_one();
}

AX_VOID CIVPSDispatcher::ClearQ(AX_VOID) {
    if (m_qFrameQ) {
        AX_S32 ret = 0;
        AX_U32 nCount = m_qFrameQ->size();
        if (nCount > 0) {
            AX_VIDEO_FRAME_T stVFrame;
            for (AX_U32 i = 0; i < nCount; ++i) {
                if (m_qFrameQ->pop(stVFrame, 0)) {
                    ret = AXCL_POOL_DecreaseRefCnt(stVFrame.u32BlkId[0]);
                    if (0 != ret) {
                        LOG_M_E(TAG, "%s: AXCL_POOL_DecreaseRefCnt(frame %lld, ivGrp %d) fail, ret = 0x%x",
                                __func__, stVFrame.u64SeqNum, m_ivGrp, ret);
                    }
                }
            }
        }
    }
}

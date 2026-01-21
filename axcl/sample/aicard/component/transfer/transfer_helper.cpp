/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include "transfer_helper.hpp"
#include <string.h>
#include <list>

#include "AppLog.hpp"
#include "ElapsedTimer.hpp"
#include "IObserver.h"
#include "IStreamHandler.hpp"
#include "detect_result.hpp"

#include "axcl_rt_context.h"

#define TAG "TRANSFER"
using namespace std;

AX_VOID CTransferHelper::SendStreamThread(AX_VOID* pArg) {
    axclrtContext context;
    if (axclError ret = axclrtCreateContext(&context, m_nDevID); AXCL_SUCC != ret) {
        return;
    }

    std::tuple<AX_U32, AX_U32>* pParams = (std::tuple<AX_U32, AX_U32>*)pArg;
    AX_U32 vdGrp = std::get<0>(*pParams);
    AX_U32 nCookie = std::get<1>(*pParams);

    LOG_MM_C(TAG, "[%d][%d] +++", vdGrp, nCookie);

    TRANSFER_SEND_DATA_T& tSendData = m_vecSendStream[vdGrp];
    CAXThread* pThread = m_vecThreadSendStream[vdGrp];

    while (pThread->IsRunning()) {
        CAXRingElement* pData = tSendData.pRingBuffer->Get();
        if (!pData) {
            CElapsedTimer::GetInstance()->mSleep(1);
            continue;
        }

        if (m_mapObs.end() != m_mapObs.find(vdGrp)) {
            for (auto& pObs : m_mapObs[vdGrp]) {
                if (pObs) {
                    if (!pObs->OnRecvVideoData(vdGrp, pData->pBuf, pData->nSize, pData->nPts)) {
                        LOG_MM_E(TAG, "%s: axcl vdec OnRecvVideoData(vdGrp %d) fail", __func__, vdGrp);
                    }
                }
            }
        }

        // CElapsedTimer::GetInstance()->mSleep(1);
        tSendData.pRingBuffer->Pop();
    }

    axclrtDestroyContext(context);
    LOG_MM_C(TAG, "[%d][%d] ---", vdGrp, nCookie);
}

AX_BOOL CTransferHelper::Init(AX_S32 nDevID, AX_U32 nMaxHstVideoCount, AX_U32 nMaxDevVideoCount) {
    LOG_MM_C(TAG, "+++");

    m_nDevID = nDevID;
    m_nMaxHstVideoCount = nMaxHstVideoCount;
    m_nMaxDevVideoCount = nMaxDevVideoCount;

    AX_U32 nVideoCount = nMaxHstVideoCount;
    m_vecSendStream.resize(nVideoCount);
    for (AX_U8 i = 0; i < nVideoCount; i++) {
        TRANSFER_SEND_DATA_T& tData = m_vecSendStream[i];
        tData.pRingBuffer = new CAXRingBuffer(600 * 1024, 20);
    }

    for (AX_U8 i = 0; i < nVideoCount; i++) {
        m_vecThreadSendStream.push_back(new CAXThread());
        m_mapSendThreadParams[i] = make_tuple(i, i);
    }

    LOG_MM_C(TAG, "---");
    return AX_TRUE;
}

AX_BOOL CTransferHelper::DeInit(AX_VOID) {
    LOG_MM_C(TAG, "+++");

    for (auto& m : m_vecThreadSendStream) {
        if (m->IsRunning()) {
            LOG_MM_E(TAG, "Send stream thread is still running");
            return AX_FALSE;
        }
        delete(m);
        m = nullptr;
    }

    UnRegAllObservers();

    LOG_MM_C(TAG, "---");
    return AX_TRUE;
}

AX_BOOL CTransferHelper::Start(AX_VOID) {
    LOG_MM_C(TAG, "+++");

    for (AX_U8 i = 0; i < m_nMaxHstVideoCount; i++) {
        if (!m_vecThreadSendStream[i]->Start([this](AX_VOID* pArg) -> AX_VOID { SendStreamThread(pArg); }, &m_mapSendThreadParams[i], "HstTransSend")) {
            LOG_MM_E(TAG, "[%d] Create stream send thread fail.", i);
            return AX_FALSE;
        }
    }

    LOG_MM_C(TAG, "---");
    return AX_TRUE;
}

AX_BOOL CTransferHelper::Stop(AX_VOID) {
    LOG_MM_C(TAG, "+++");

    for (auto& m : m_vecThreadSendStream) {
        m->Stop();
    }
    for (auto& m : m_vecThreadSendStream) {
        m->Join();
    }

    ClearBuf();

    LOG_MM_C(TAG, "---");
    return AX_TRUE;
}

AX_BOOL CTransferHelper::OnRecvVideoData(AX_S32 nChannel, const AX_U8* pData, AX_U32 nLen, AX_U64 nPTS) {
    CAXRingElement ele((AX_U8*)pData, nLen, nPTS, AX_TRUE);

    if (!m_vecSendStream[nChannel].pRingBuffer->Put(ele)) {
        LOG_M_D(TAG, "[%d] Put video data(len=%d) into ringbuf full", nChannel, nLen);
        return AX_FALSE;
    } else {
        LOG_M_D(TAG, "[%d] Put video data into ringbuf, size=%d", nChannel, nLen);
    }

    return AX_TRUE;
}

AX_BOOL CTransferHelper::OnRecvAudioData(AX_S32 nChannel, const AX_U8* pData, AX_U32 nLen, AX_U64 nPTS) {
    return AX_TRUE;
}

AX_VOID CTransferHelper::ClearBuf(AX_VOID) {
    for (auto& m: m_vecSendStream) {
        m.pRingBuffer->Clear();
    }
}

AX_BOOL CTransferHelper::RegObserver(AX_VDEC_GRP vdGrp, IStreamObserver* pObs) {
    if (!pObs) {
        LOG_M_E(TAG, "%s observer is nil", __func__);
        return AX_FALSE;
    }

    for (auto& m : m_vecThreadSendStream) {
        if (m->IsRunning()) {
            LOG_M_E(TAG, "%s is not allowed after send stream thread is running", __func__);
            return AX_FALSE;
        }
    }

    std::lock_guard<std::mutex> lck(m_mtxObs);
    auto kv = m_mapObs.find(vdGrp);
    if (kv == m_mapObs.end()) {
        list<IStreamObserver*> lstObs;
        lstObs.push_back(pObs);
        m_mapObs[vdGrp] = lstObs;
    } else {
        for (auto& m : kv->second) {
            if (m == pObs) {
                LOG_M_W(TAG, "%s: observer %p is already registed to vdGrp %d", __func__, pObs, vdGrp);
                return AX_TRUE;
            }
        }

        kv->second.push_back(pObs);
    }

    LOG_M_I(TAG, "%s: register observer %p to vdGrp %d ok", __func__, pObs, vdGrp);
    return AX_TRUE;
}

AX_BOOL CTransferHelper::UnRegObserver(AX_VDEC_GRP vdGrp, IStreamObserver* pObs) {
    if (!pObs) {
        LOG_M_E(TAG, "%s observer is nil", __func__);
        return AX_FALSE;
    }

    for (auto& m : m_vecThreadSendStream) {
        if (m->IsRunning()) {
            LOG_M_E(TAG, "%s is not allowed after send stream thread is running", __func__);
            return AX_FALSE;
        }
    }

    std::lock_guard<std::mutex> lck(m_mtxObs);
    auto kv = m_mapObs.find(vdGrp);
    if (kv != m_mapObs.end()) {
        for (auto& m : kv->second) {
            if (m == pObs) {
                kv->second.remove(pObs);
                LOG_M_I(TAG, "%s: unregist observer %p from vdGrp %d ok", __func__, pObs, vdGrp);
                return AX_TRUE;
            }
        }
    }

    LOG_M_E(TAG, "%s: observer %p is not registed to vdGrp %d", __func__, pObs, vdGrp);
    return AX_FALSE;
}

AX_BOOL CTransferHelper::UnRegAllObservers(AX_VOID) {
    std::lock_guard<std::mutex> lck(m_mtxObs);
    m_mapObs.clear();
    return AX_TRUE;
}

/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include "istream.hpp"
#include <sys/time.h>
#include <time.h>
#include <algorithm>
#include "AppLogApi.h"

#define TAG "STREAM"

AX_U64 CStreamHelper::GetTick(AX_VOID) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (ts.tv_sec * 1000000 + ts.tv_nsec / 1000);
}

CONST STREAM_INFO_T& CStream0::GetStreamInfo(AX_VOID) CONST {
    return m_stInfo;
}

CONST STREAM_STAT_T& CStream0::QueryStatus(AX_VOID) CONST {
    return m_stStat;
}

AX_BOOL CStream0::RegisterObserver(IStreamObserver* pObs) {
    if (!pObs) {
        LOG_M_E(TAG, "%s: observer is nil", __func__);
        return AX_FALSE;
    }

    std::lock_guard<std::mutex> lck(m_mtxObs);
    auto it = std::find(m_lstObs.begin(), m_lstObs.end(), pObs);
    if (it != m_lstObs.end()) {
        LOG_M_W(TAG, "%s: observer has been registed", __func__);
    } else {
        m_lstObs.push_back(pObs);
        LOG_M_I(TAG, "%s: regist observer %p ok", __func__, pObs);
    }

    return AX_TRUE;
}

AX_BOOL CStream0::UnRegisterObserver(IStreamObserver* pObs) {
    if (!pObs) {
        LOG_M_E(TAG, "%s: observer is nil", __func__);
        return AX_FALSE;
    }

    std::lock_guard<std::mutex> lck(m_mtxObs);
    auto it = std::find(m_lstObs.begin(), m_lstObs.end(), pObs);
    if (it != m_lstObs.end()) {
        m_lstObs.remove(pObs);
        LOG_M_I(TAG, "%s: unregist observer %p ok", __func__, pObs);
        return AX_TRUE;
    } else {
        LOG_M_E(TAG, "%s: observer %p is not registed", __func__, pObs);
        return AX_FALSE;
    }
}

CStreamRingBuffer::CStreamRingBuffer(AX_U32 nBufSize, AX_U32 nBufCnt)
    : m_Capacity(nBufCnt), m_BufSize(nBufSize), m_head(0), m_tail(0), m_bDrop(AX_FALSE), m_bWakeup(AX_FALSE) {
    m_ring = new STREAM_RING_ELEMENT_T[nBufCnt];
    for (AX_U32 i = 0; i < nBufCnt; ++i) {
        m_ring[i].pBuff = new AX_U8[nBufSize];
        m_ring[i].nIdex = i;
        m_ring[i].nSize = nBufSize;
    }
}

CStreamRingBuffer::~CStreamRingBuffer(AX_VOID) {
    for (AX_U32 i = 0; i < m_Capacity; ++i) {
        delete[] m_ring[i].pBuff;
    }

    delete[] m_ring;
}

AX_BOOL CStreamRingBuffer::Push(CONST STREAM_FRAME_T& frame, AX_U64 tick) {
    std::lock_guard<std::mutex> lck(m_mtx);

    if (frame.GetFrameSize() > m_BufSize) {
        /* input frame size is too long */
        if (frame.IsKeyFrame()) {
            m_bDrop = AX_TRUE;
        }

        LOG_MM_E(TAG, "frame %llu(isKey %d) size %d is too long to drop", frame.GetSeqNum(), frame.IsKeyFrame(), frame.GetFrameSize());
        return AX_FALSE;
    }

    AX_BOOL bKeyFrame = frame.IsKeyFrame();
    if (full()) {
#ifndef __SDK_DEBUG__
        dump();
#endif

        if (bKeyFrame) {
            // mandatory to replace the tail with new key frame
            --m_tail;
            m_bDrop = AX_FALSE;
#ifndef __SDK_DEBUG__
            LOG_MM_W(TAG, "ring buffer is full, replace the last with new key frame %llu, tick %llu", frame.GetSeqNum(), tick);
#else
            LOG_MM_I(TAG, "ring buffer is full, replace the last with new key frame %llu, tick %llu", frame.GetSeqNum(), tick);
#endif
        } else {
#ifndef __SDK_DEBUG__
            LOG_MM_W(TAG, "ring buffer is full, drop frame %llu, tick %llu", frame.GetSeqNum(), tick);
#else
            LOG_MM_I(TAG, "ring buffer is full, drop frame %llu, tick %llu", frame.GetSeqNum(), tick);
#endif
            return AX_FALSE;
        }
    } else {
        if (bKeyFrame) {
            m_bDrop = AX_FALSE;
        } else {
            if (m_bDrop) {
                LOG_MM_W(TAG, "drop frame %llu because key frame is lost", frame.GetSeqNum());
                return AX_FALSE;
            }
        }
    }

    AX_U32 nIndex = (m_tail % m_Capacity);
    m_ring[nIndex].nTick = tick;
    m_ring[nIndex].CopyFrom(frame);
    ++m_tail;

    m_cv.notify_one();
    return AX_TRUE;
}

STREAM_RING_ELEMENT_T* CStreamRingBuffer::Peek(AX_S32 nTimeOut /* = -1 */) {
    std::unique_lock<std::mutex> lck(m_mtx);

    if (empty()) {
        if (0 == nTimeOut) {
            return nullptr;
        } else if (nTimeOut < 0) {
            m_cv.wait(lck, [this]() -> bool { return !empty() || m_bWakeup; });
        } else {
            m_cv.wait_for(lck, std::chrono::milliseconds(nTimeOut), [this]() -> bool { return !empty() || m_bWakeup; });

            m_bWakeup = AX_FALSE;
            if (empty()) {
                return nullptr;
            }
        }
    }

    AX_U32 nIndex = (m_head % m_Capacity);
    return &m_ring[nIndex];
}

AX_BOOL CStreamRingBuffer::Pop(AX_VOID) {
    std::lock_guard<std::mutex> lck(m_mtx);
    if (empty()) {
        LOG_MM_E(TAG, "no element to pop");
        return AX_FALSE;
    }

    ++m_head;
    return AX_TRUE;
}

AX_VOID CStreamRingBuffer::Wakeup(AX_VOID) {
    std::lock_guard<std::mutex> lck(m_mtx);
    m_bWakeup = AX_TRUE;

    m_cv.notify_one();
}

AX_VOID CStreamRingBuffer::dump(AX_VOID) {
    LOG_MM_C(TAG, "dump stream ring buffer -->");
    if (empty()) {
        LOG_MM_C(TAG, "    0 element");
    } else {
        for (AX_U64 i = m_head; i < m_tail; ++i) {
            AX_U32 index = (AX_U32)(i % m_Capacity);
            LOG_MM_C(TAG, "    [%d] seq %llu key %d size %u tick %llu", index, m_ring[index].stFrame.GetSeqNum(),
                     m_ring[index].stFrame.IsKeyFrame(), m_ring[index].stFrame.GetFrameSize(), m_ring[index].nTick);
        }
    }
}
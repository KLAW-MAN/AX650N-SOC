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
#include <condition_variable>
#include <memory>
#include <mutex>
#include "AXEvent.hpp"
#include "AXLockQ.hpp"
#include "AXRTSPClient.h"
#include "AXThread.hpp"
#include "IStreamHandler.hpp"
#include "rtspdamon.hpp"

typedef struct {
    AX_PAYLOAD_TYPE_E enPayload;
    STREAM_NALU_TYPE_E enNalu;
    AX_U64 nSeqNum;
    AX_U8 *pData;
    AX_U32 nSize;

    AX_U32 GetFrameSize(AX_VOID) const {
        return nSize;
    }

    AX_BOOL IsKeyFrame(AX_VOID) const {
        if (PT_H264 == enPayload || PT_H265 == enPayload) {
            return (NALU_TYPE_IDR == enNalu) ? AX_TRUE : AX_FALSE;
        }

        return AX_FALSE;
    }

} STREAM_DATA_T;

class CStreamRingBuffer;
class CRtspStreamer : public CStreamBaseHander {
public:
    CRtspStreamer(AX_VOID) = default;
    virtual ~CRtspStreamer(AX_VOID) = default;

    AX_BOOL Init(const STREAMER_ATTR_T &stAttr) override;
    AX_BOOL DeInit(AX_VOID) override;

    AX_BOOL Start(AX_VOID) override;
    AX_BOOL Stop(AX_VOID) override;

    AX_BOOL ReConnect(AX_VOID);

protected:
    AX_VOID EventLoopThread(AX_VOID *pArg);
#ifdef __RTSP_RECV_CACHE__
    AX_VOID DispatchThread(AX_VOID *pArg);
#endif

    /* rtsp client callback */
    void OnRecvFrame(const void *session, const unsigned char *pFrame, unsigned nSize, AX_PAYLOAD_TYPE_E ePayload, STREAM_NALU_TYPE_E eNalu,
                     struct timeval tv);
    void OnTracksInfo(const TRACKS_INFO_T &tracks);
    void OnPreparePlay(void);
    void OnCheckAlive(int resultCode, const char *resultString);

private:
    TaskScheduler *m_scheduler{nullptr};
    UsageEnvironment *m_env{nullptr};
    CAXRTSPClient *m_client{nullptr};
    CAXThread m_EventThread;
    CAXEvent m_InitEvent;
    CAXEvent m_PlayEvent;
    volatile char m_cExitThread{1};
    std::unique_ptr<CRtspDamon> m_damon;
    STREAMER_ATTR_T m_stAttr;

#ifdef __RTSP_RECV_CACHE__
    CAXThread m_dispatchThread;
    std::unique_ptr<CStreamRingBuffer> m_dispatchBuf;

    AX_U64 m_nLastTick = {0};
    AX_U32 m_nInterval = {0};  // 1000000 / fps
#endif
};

typedef struct {
    STREAM_DATA_T stFrame;
    AX_U32 nIdex;
    AX_U32 nSize;
    AX_U8 *pBuff;
    AX_U64 nTick;

    AX_VOID CopyFrom(const STREAM_DATA_T &frame) {
        stFrame = frame;
        memcpy(pBuff, frame.pData, frame.nSize);
        stFrame.pData = pBuff;
    }
} STREAM_RING_ELEMENT_T;

class CStreamRingBuffer {
public:
    CStreamRingBuffer(AX_U32 nBufSize, AX_U32 nBufCnt, AX_U32 nCookie);
    ~CStreamRingBuffer(AX_VOID);

    AX_BOOL Push(const STREAM_DATA_T &frame, AX_U64 tick);
    STREAM_RING_ELEMENT_T *Peek(AX_S32 nTimeOut = -1);
    AX_BOOL Pop(AX_VOID);
    AX_VOID Wakeup(AX_VOID);

    AX_VOID Dump(AX_VOID);

protected:
    bool empty(AX_VOID);
    bool full(AX_VOID);

private:
    STREAM_RING_ELEMENT_T *m_ring = {nullptr};
    AX_U32 m_Capacity;
    AX_U32 m_BufSize;
    AX_U32 m_head;
    AX_U32 m_tail;
    AX_BOOL m_bDrop;
    AX_BOOL m_bWakeup;
    std::mutex m_mtx;
    std::condition_variable m_cv;
    AX_U32 m_nCookie;
};

inline bool CStreamRingBuffer::empty(AX_VOID) {
    return (m_head == m_tail) ? true : false;
}

inline bool CStreamRingBuffer::full(AX_VOID) {
    return ((m_tail - m_head) == m_Capacity) ? true : false;
}

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
#include <condition_variable>
#include <list>
#include <mutex>
#include <string>
#include <unordered_map>
#include "haltype.hpp"
#include "nalu.hpp"

typedef enum { RTP_OVER_UDP = 0, RTP_OVER_TCP = 1 } RTP_TRANSPORT_MODE_E;
typedef enum { DISCONNECT = 0, RECONNECT = 1, CONNECTED = 2 } CONNECT_STATUS_E;

typedef struct STREAM_ATTR_S {
    std::string strURL;
    AX_S32 nCookie;
    AX_U32 nMaxBufSize;
    RTP_TRANSPORT_MODE_E enTransportMode;
    AX_U32 nDebugLevel;
    AX_U32 nKeepAliveInterval;  /* heart beat interval time, unit is seconds */
    AX_U32 nReconnectThreshold; /* if the count of heart beat absent from server is >= nReconnThreshold, trigger to reconnect */
    AX_S32 nFps;                /* ONLY for ffmpeg stream, < 0: no fps ctrl, 0: auto detect fps from stream, > 0: specified fps */
    STREAM_ATTR_S(AX_VOID) {
        nCookie = -1;
        nMaxBufSize = 1920 * 1080;
        enTransportMode = RTP_OVER_UDP;
        nDebugLevel = 0;
        nKeepAliveInterval = 10;
        nReconnectThreshold = 3;
        nFps = 0;
    }
} STREAM_ATTR_T;

typedef struct {
    AX_PAYLOAD_TYPE_E enPayload;
    AX_U32 nProfile;
    AX_U32 nLevel; /* level_idc */
    AX_U32 nWidth;
    AX_U32 nHeight;
    AX_U32 nNumRefs; /* num_ref_frames */
    AX_U32 nFps;
} STREAM_VIDEO_TRACK_INFO_T;

typedef struct {
    AX_PAYLOAD_TYPE_E enPayload;
    AX_U32 nBps;
    AX_U32 nSampleRate;
    AX_U32 nSampleBit;
} STREAM_AUDIO_TRACK_INFO_T;

typedef struct {
    STREAM_VIDEO_TRACK_INFO_T stInfo;
    STREAM_NALU_TYPE_E enNalu;
    AX_U8* pData;
    AX_U32 nLen;
    AX_BOOL bSkipDisplay; /* not support yet */
    AX_U64 nPTS;
    AX_U64 nSequence;
    AX_U64 nFramePts;
    AX_U64 u64UserData;
} STREAM_VIDEO_FRAME_T;

typedef struct {
    AX_PAYLOAD_TYPE_E enPayload;
    union {
        STREAM_VIDEO_TRACK_INFO_T stVideo;
        STREAM_AUDIO_TRACK_INFO_T stAudio;
    } info;
} STREAM_TRACK_INFO_T;

typedef struct {
    std::string strURL;
    AX_S32 nCookie;
    STREAM_VIDEO_TRACK_INFO_T stVideo;
    std::unordered_map<AX_VOID*, STREAM_TRACK_INFO_T> tracks;
} STREAM_INFO_T;

typedef struct {
    STREAM_AUDIO_TRACK_INFO_T stInfo;
    AX_U8* pData;
    AX_U32 nLen;
    AX_U64 nPTS;
} STREAM_AUDIO_FRAME_T;

typedef struct {
    AX_PAYLOAD_TYPE_E enPayload;
    union {
        STREAM_VIDEO_FRAME_T stVideo;
        STREAM_AUDIO_FRAME_T stAudio;
    } frame;
    AX_U64 nPrivData;
    AX_U64 nSeqNum[2];

    AX_U32 GetFrameSize(AX_VOID) CONST {
        return (PT_H264 == enPayload || PT_H265 == enPayload) ? frame.stVideo.nLen : frame.stAudio.nLen;
    }

    AX_BOOL IsKeyFrame(AX_VOID) CONST {
        if (PT_H264 == enPayload || PT_H265 == enPayload) {
            return (NALU_TYPE_IDR == frame.stVideo.enNalu) ? AX_TRUE : AX_FALSE;
        }

        return AX_FALSE;
    }

    AX_U64 GetSeqNum(AX_VOID) CONST {
        return (PT_H264 == enPayload || PT_H265 == enPayload) ? nSeqNum[0] : nSeqNum[1];
    }

} STREAM_FRAME_T;

typedef struct STREAM_STAT_S {
    AX_S32 nState;    /* 0: idle 1: running */
    AX_U64 nCount[2]; /* 0: video, 1: audio */
    STREAM_STAT_S(AX_VOID) {
        Reset();
    }
    AX_VOID Reset(AX_VOID) noexcept {
        memset(this, 0, sizeof(*this));
    }
} STREAM_STAT_T;

class CStreamHelper {
public:
    static AX_U64 GetTick(AX_VOID);
};

class IStreamObserver {
public:
    virtual ~IStreamObserver(AX_VOID) = default;
    virtual AX_BOOL OnRecvStreamData(CONST STREAM_FRAME_T& stFrame) = 0;
    virtual AX_BOOL OnRecvStreamInfo(CONST STREAM_INFO_T& stInfo) = 0;
    virtual AX_VOID OnNotifyConnStatus(CONST AX_CHAR* pUrl, CONNECT_STATUS_E enStatus) = 0;
};

class IStream {
public:
    virtual ~IStream(AX_VOID) = default;

    virtual AX_BOOL RegisterObserver(IStreamObserver* pObs) = 0;
    virtual AX_BOOL UnRegisterObserver(IStreamObserver* pObs) = 0;

    virtual AX_BOOL Init(CONST STREAM_ATTR_T& stAttr) = 0;
    virtual AX_BOOL DeInit(AX_VOID) = 0;

    virtual AX_BOOL Start(AX_VOID) = 0;
    virtual AX_BOOL Stop(AX_VOID) = 0;

    virtual CONST STREAM_STAT_T& QueryStatus(AX_VOID) CONST = 0;
    virtual CONST STREAM_INFO_T& GetStreamInfo(AX_VOID) CONST = 0;
};

class CStream0 : public IStream {
public:
    CStream0(AX_VOID) = default;

    AX_BOOL RegisterObserver(IStreamObserver* pObs) override;
    AX_BOOL UnRegisterObserver(IStreamObserver* pObs) override;

    CONST STREAM_INFO_T& GetStreamInfo(AX_VOID) CONST override;
    CONST STREAM_STAT_T& QueryStatus(AX_VOID) CONST override;

protected:
    STREAM_ATTR_T m_stAttr;
    STREAM_INFO_T m_stInfo;
    STREAM_STAT_T m_stStat;
    std::list<IStreamObserver*> m_lstObs;
    std::mutex m_mtxObs;
};

typedef struct {
    STREAM_FRAME_T stFrame;
    AX_U32 nIdex;
    AX_U32 nSize;
    AX_U8* pBuff;
    AX_U64 nTick;

    AX_VOID CopyFrom(CONST STREAM_FRAME_T& frame) {
        stFrame = frame;
        if (PT_H264 == frame.enPayload || PT_H265 == frame.enPayload) {
            memcpy(pBuff, frame.frame.stVideo.pData, frame.frame.stVideo.nLen);
            stFrame.frame.stVideo.pData = pBuff;  // swap
        } else {
            memcpy(pBuff, frame.frame.stAudio.pData, frame.frame.stAudio.nLen);
            stFrame.frame.stAudio.pData = pBuff;  // swap
        }
    }

} STREAM_RING_ELEMENT_T;

class CStreamRingBuffer {
public:
    CStreamRingBuffer(AX_U32 nBufSize, AX_U32 nBufCnt);
    ~CStreamRingBuffer(AX_VOID);

    AX_BOOL Push(CONST STREAM_FRAME_T& frame, AX_U64 tick);
    STREAM_RING_ELEMENT_T* Peek(AX_S32 nTimeOut = -1);
    AX_BOOL Pop(AX_VOID);
    AX_VOID Wakeup(AX_VOID);

protected:
    bool empty(AX_VOID);
    bool full(AX_VOID);
    AX_VOID dump(AX_VOID);

private:
    STREAM_RING_ELEMENT_T* m_ring = {nullptr};
    AX_U32 m_Capacity;
    AX_U32 m_BufSize;
    AX_U32 m_head;
    AX_U32 m_tail;
    AX_BOOL m_bDrop;
    AX_BOOL m_bWakeup;
    std::mutex m_mtx;
    std::condition_variable m_cv;
};

inline bool CStreamRingBuffer::empty(AX_VOID) {
    return (m_head == m_tail) ? true : false;
}

inline bool CStreamRingBuffer::full(AX_VOID) {
    return ((m_tail - m_head) == m_Capacity) ? true : false;
}
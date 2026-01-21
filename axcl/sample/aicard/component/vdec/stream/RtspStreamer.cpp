/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include "RtspStreamer.hpp"
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <chrono>
#include "AppLogApi.h"
#include "SpsParser.hpp"
#include "make_unique.hpp"

using namespace std;
using namespace std::placeholders;
#define CLIENT "CLIENT"

#ifdef __RTSP_RECV_CACHE__
#ifndef __SDK_DEBUG__
#define MAX_DISPATCH_BUFFER_DEPTH (3)
#else
#define MAX_DISPATCH_BUFFER_DEPTH (5)
#endif

static AX_U64 GetTick(AX_VOID) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (ts.tv_sec * 1000000 + ts.tv_nsec / 1000);
}
#endif

AX_VOID CRtspStreamer::EventLoopThread(AX_VOID *pArg) {
    m_cExitThread = 0;
    m_env->taskScheduler().doEventLoop(&m_cExitThread);
    UpdateStatus(AX_FALSE);
}

#ifdef __RTSP_RECV_CACHE__
AX_VOID CRtspStreamer::DispatchThread(AX_VOID *pArg) {
    STREAM_RING_ELEMENT_T *ele;
    while (1) {
        ele = m_dispatchBuf->Peek(-1);

        if (!m_dispatchThread.IsRunning()) {
            break;
        }

        if (ele) {
            STREAM_DATA_T stFrame = ele->stFrame;
            for (auto &&m : m_lstObs) {
                m->OnRecvVideoData(m_stInfo.nCookie, stFrame.pData, stFrame.nSize, 0);
            }

            m_dispatchBuf->Pop();
        }
    }
}
#endif

void CRtspStreamer::OnRecvFrame(const void *session, const unsigned char *pFrame, unsigned nSize, AX_PAYLOAD_TYPE_E enPayload,
                                STREAM_NALU_TYPE_E enNalu, struct timeval) {
    if (m_stInfo.session != session) {
        /* ignore other tracks except for video */
        return;
    }

    ++m_stStat.nCount;

#ifdef __RTSP_RECV_CACHE__
    STREAM_DATA_T stData;
    stData.enPayload = enPayload;
    stData.enNalu = enNalu;
    stData.pData = (AX_U8 *)pFrame;
    stData.nSize = nSize;
    stData.nSeqNum = m_stStat.nCount;

    /*
        The interval time of two rtsp frames maybe too short(eg: < 2ms), and it will cause the ringbuf full, especially 64 clients.
        I have no idea about the reason, so add delay if continuous frames come too frequently.
        Fixme if someone has found the reason why rtsp sends frame too frequently.
    */
    AX_U64 nTick = GetTick();
    if (m_nLastTick > 0) {
        AX_U32 nElapsed = nTick - m_nLastTick;
        if (nElapsed < m_nInterval) {
            AX_U32 ms = (m_nInterval - nElapsed) / 1000;
            if (ms >= 15) {
                std::this_thread::sleep_for(std::chrono::milliseconds(ms));
                nTick = GetTick();
            }
        }
    }

    m_nLastTick = nTick;

    (AX_VOID) m_dispatchBuf->Push(stData, nTick);
#else
    for (auto &&m : m_lstObs) {
        m->OnRecvVideoData(m_stInfo.nCookie, pFrame, nSize, 0);
    }
#endif
}

void CRtspStreamer::OnTracksInfo(const TRACKS_INFO_T &tracks) {
    if (0 == tracks.tracks.size()) {
        LOG_M_E(CLIENT, "%s: no tracks", m_stInfo.strPath.c_str());
        return;
    }

    for (auto &&kv : tracks.tracks) {
        const TRACK_INFO_T &track = kv.second;
        switch (track.enPayload) {
            case PT_H264:
            case PT_H265: {
                m_stInfo.session = kv.first;
                m_stInfo.eVideoType = track.enPayload;

                SPS_INFO_T sps;
                memset(&sps, 0, sizeof(sps));
                (PT_H264 == track.enPayload) ? h264_parse_sps(track.video.sps[0], track.video.len[0], &sps)
                                             : hevc_parse_sps(track.video.sps[0], track.video.len[0], &sps);

                if (sps.width > 0 && sps.height > 0) {
                    m_stInfo.nWidth = sps.width;
                    m_stInfo.nHeight = sps.height;
                }

                if (sps.fps > 0) {
                    m_stInfo.nFps = sps.fps;
                }

#ifdef __RTSP_RECV_CACHE__
                m_nInterval = 1000000 / m_stInfo.nFps;
#endif

                LOG_M_C(CLIENT, "%s: %s payload %d, %dx%d %dfps", tracks.url, track.control, track.rtpPayload, sps.width, sps.height,
                        sps.fps);
            } break;

            default:
                LOG_M_C(CLIENT, "%s: %s payload %d", tracks.url, track.control, track.rtpPayload);
                break;
        }
    }

    m_InitEvent.SetEvent();
}

void CRtspStreamer::OnPreparePlay(void) {
    m_PlayEvent.WaitEvent();
}

AX_BOOL CRtspStreamer::Init(const STREAMER_ATTR_T &stAttr) {
    LOG_M_D(CLIENT, "%s: client %d +++", __func__, stAttr.nCookie);

    m_stAttr = stAttr;

    m_stInfo.strPath = stAttr.strPath;
    m_stInfo.eStreamType = STREAM_TYPE_E::RTSP;
    m_stInfo.nCookie = stAttr.nCookie;
    m_stInfo.nWidth = stAttr.nMaxWidth;
    m_stInfo.nHeight = stAttr.nMaxHeight;
    m_stInfo.nFps = 25; /* default fps */

    m_stStat.bStarted = AX_FALSE;
    m_stStat.nCount = 0;

    AX_S32 nVerbosityLevel = 0;
    AX_CHAR *pEnv = getenv("RTSP_CLIENT_LOG");
    if (pEnv) {
        nVerbosityLevel = atoi(pEnv);
    }

    m_InitEvent.ResetEvent();
    m_PlayEvent.ResetEvent();

    RtspClientCallback cb;
    cb.OnRecvFrame = bind(&CRtspStreamer::OnRecvFrame, this, _1, _2, _3, _4, _5, _6);
    cb.OnTracksInfo = bind(&CRtspStreamer::OnTracksInfo, this, _1);
    cb.OnPreparePlay = bind(&CRtspStreamer::OnPreparePlay, this);
    cb.OnCheckAlive = std::bind(&CRtspStreamer::OnCheckAlive, this, std::placeholders::_1, std::placeholders::_2);

    try {
        m_scheduler = BasicTaskScheduler::createNew();
        m_env = BasicUsageEnvironment::createNew(*m_scheduler);

        // Begin by creating a "RTSPClient" object.  Note that there is a separate "RTSPClient" object for each stream that we wish
        // to receive (even if more than stream uses the same "rtsp://" URL).
        m_client = CAXRTSPClient::createNew(*m_env, m_stInfo.strPath.c_str(), cb, m_stInfo.nWidth * m_stInfo.nHeight, nVerbosityLevel,
                                            "AxRtspClient", 0);
        m_client->scs.keepAliveInterval = 10;

        char *env = getenv("RTP_TRANSPORT_MODE");
        if (env) {
            m_client->scs.streamTransportMode = ((1 != atoi(env)) ? 0 : 1);
            LOG_M_I(CLIENT, "client %d RTP transport mode: %d", m_stInfo.nCookie, m_client->scs.streamTransportMode);
        }

        m_client->Start();

        AX_CHAR szName[32];
        sprintf(szName, "AppRtspClient%d", m_stInfo.nCookie);
        if (!m_EventThread.Start([this](AX_VOID *pArg) -> AX_VOID { EventLoopThread(pArg); }, this, szName)) {
            m_client->Stop();
            m_client = nullptr;

            m_env->reclaim();
            m_env = nullptr;

            delete m_scheduler;
            m_scheduler = nullptr;

            LOG_M_E(CLIENT, "%s: start rtsp client thread of client %d fail", __func__, m_stInfo.nCookie);
            return AX_FALSE;
        }

        if (!m_damon) {
            RTSP_DAMON_ATTR_T stDamon;
            stDamon.strUrl = stAttr.strPath;
            stDamon.nKeepAliveInterval = m_client->scs.keepAliveInterval + 1; /* margin to damon wait_for to avoid missing condition */
            stDamon.nReconnectThreshold = 3;
            stDamon.reconnect = std::bind(&CRtspStreamer::ReConnect, this);
            m_damon.reset(CRtspDamon::CreateInstance(stDamon));
        }

        /* wait SDP is received to parse stream info in 5 seconds */
        if (!m_InitEvent.WaitEvent(5000)) {
            m_EventThread.Stop();
            m_cExitThread = 1;
            m_EventThread.Join();

            m_client->Stop();
            m_client = nullptr;

            if (m_env) {
                m_env->reclaim();
                m_env = nullptr;
            }

            if (m_scheduler) {
                delete m_scheduler;
                m_scheduler = nullptr;
            }

            return AX_FALSE;
        }

    } catch (bad_alloc &e) {
        LOG_M_E(CLIENT, "%s: create client %d fail", __func__, m_stInfo.nCookie);
        DeInit();
        return AX_FALSE;
    }

    LOG_M_D(CLIENT, "%s: client %d ---", __func__, m_stInfo.nCookie);
    return AX_TRUE;
}

AX_BOOL CRtspStreamer::DeInit(AX_VOID) {
    LOG_M_D(CLIENT, "%s: client %d +++", __func__, m_stInfo.nCookie);

    // if (m_EventThread.IsRunning()) {
    //     LOG_M_E(CLIENT, "%s: client %d is still running", __func__, m_stInfo.nCookie);
    //     return AX_FALSE;
    // }

    Stop();

    if (m_env) {
        m_env->reclaim();
        m_env = nullptr;
    }

    if (m_scheduler) {
        delete m_scheduler;
        m_scheduler = nullptr;
    }

    LOG_M_D(CLIENT, "%s: stream %d ---", __func__, m_stInfo.nCookie);
    return AX_TRUE;
}

AX_BOOL CRtspStreamer::Start(AX_VOID) {
    LOG_M_D(CLIENT, "%s: client %d +++", __func__, m_stInfo.nCookie);

    if (m_damon) {
        m_damon->Start();
    }

#ifdef __RTSP_RECV_CACHE__
    m_nLastTick = 0;

    m_dispatchBuf = std::move(
        std::make_unique<CStreamRingBuffer>(m_stInfo.nWidth * m_stInfo.nHeight * 3 / 4, MAX_DISPATCH_BUFFER_DEPTH, m_stInfo.nCookie));

    AX_CHAR szName[16];
    sprintf(szName, "RtspDisp%d", m_stInfo.nCookie);
    m_dispatchThread.Start([this](AX_VOID *pArg) -> AX_VOID { DispatchThread(pArg); }, nullptr, szName);
#endif

    m_PlayEvent.SetEvent();
    UpdateStatus(AX_TRUE);

    LOG_M_D(CLIENT, "%s: client %d ---", __func__, m_stInfo.nCookie);
    return AX_TRUE;
}

AX_BOOL CRtspStreamer::Stop(AX_VOID) {
    LOG_M_D(CLIENT, "%s: client %d +++", __func__, m_stInfo.nCookie);

    if (m_damon) {
        m_damon->Stop();
    }

#ifdef __RTSP_RECV_CACHE__
    m_dispatchThread.Stop();
    if (m_dispatchBuf) {
        m_dispatchBuf->Wakeup();
    }
    m_dispatchThread.Join();
#endif

    m_EventThread.Stop();
    /* if Init ok, but start is not invoked, then we need wakeup play event */
    m_PlayEvent.SetEvent();
    m_cExitThread = 1;
    m_EventThread.Join();

    if (m_client) {
        m_client->Stop();
        m_client = nullptr;
    }

    // LOG_M_I(CLIENT, "client %d has sent total %lld frames", m_stInfo.nCookie, m_tStatus.nCount);
    LOG_M_D(CLIENT, "%s: client %d ---", __func__, m_stInfo.nCookie);
    return AX_TRUE;
}

void CRtspStreamer::OnCheckAlive(int resultCode, const char *resultString) {
    if (0 == resultCode) {
        // LOG_M_C(TAG, "%s: resultCode = %d", __func__, resultCode);
        if (m_damon) {
            m_damon->KeepAlive();
        }
    } else {
        if (resultString) {
            LOG_M_E(CLIENT, "%s: resultCode = %d, %s", __func__, resultCode, resultString);
        } else {
            LOG_M_E(CLIENT, "%s: resultCode = %d", __func__, resultCode);
        }
    }
}

AX_BOOL CRtspStreamer::ReConnect(AX_VOID) {
    m_EventThread.Stop();
    m_cExitThread = 1;
    m_EventThread.Join();

    /* live555 taskScheduler is not thread safe, unscheduleDelayedTask after doEventLoop finished */
    if (m_env && m_client) {
        m_env->taskScheduler().unscheduleDelayedTask(m_client->scs.streamTimerTask);
    }

    if (m_client) {
        m_client->Stop();
        m_client = nullptr;
    }

    m_stStat.bStarted = AX_FALSE;

#ifdef __RTSP_RECV_CACHE__
    m_nLastTick = 0;
#endif

    if (m_env) {
        m_env->reclaim();
        m_env = nullptr;
    }

    if (m_scheduler) {
        delete m_scheduler;
        m_scheduler = nullptr;
    }

    if (!Init(m_stAttr)) {
        return AX_FALSE;
    }

    m_PlayEvent.SetEvent();
    m_stStat.bStarted = AX_TRUE;

    return AX_TRUE;
}

CStreamRingBuffer::CStreamRingBuffer(AX_U32 nBufSize, AX_U32 nBufCnt, AX_U32 nCookie)
    : m_Capacity(nBufCnt), m_BufSize(nBufSize), m_head(0), m_tail(0), m_bDrop(AX_FALSE), m_bWakeup(AX_FALSE), m_nCookie(nCookie) {
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

AX_BOOL CStreamRingBuffer::Push(const STREAM_DATA_T &frame, AX_U64 tick) {
    std::lock_guard<std::mutex> lck(m_mtx);

    if (frame.GetFrameSize() > m_BufSize) {
        /* input frame size is too long */
        if (frame.IsKeyFrame()) {
            m_bDrop = AX_TRUE;
        }

        LOG_MM_E(CLIENT, "[%d] frame%llu(isKey %d) size %d is too long to drop", m_nCookie, frame.nSeqNum, frame.IsKeyFrame(),
                 frame.GetFrameSize());
        return AX_FALSE;
    }

    AX_BOOL bKeyFrame = frame.IsKeyFrame();
    if (full()) {
#ifndef __SDK_DEBUG__
        /* if debug SDK version, performance decreases, so fifo may full */
        Dump();
#endif

        if (bKeyFrame) {
            // mandatory to replace the tail with new key frame
            --m_tail;
            m_bDrop = AX_FALSE;
#ifndef __SDK_DEBUG__
            LOG_MM_W(CLIENT, "[%d] ring buffer is full, replace the last with new key frame %llu, tick %llu", m_nCookie, frame.nSeqNum,
                     tick);
#else
            LOG_MM_I(CLIENT, "[%d] ring buffer is full, replace the last with new key frame %llu, tick %llu", m_nCookie, frame.nSeqNum,
                     tick);
#endif
        } else {
#ifndef __SDK_DEBUG__
            LOG_MM_W(CLIENT, "[%d] ring buffer is full, drop frame %llu directly, tick %llu", m_nCookie, frame.nSeqNum, tick);
#else
            LOG_MM_I(CLIENT, "[%d] ring buffer is full, drop frame %llu directly, tick %llu", m_nCookie, frame.nSeqNum, tick);
#endif
            return AX_FALSE;
        }
    } else {
        if (bKeyFrame) {
            m_bDrop = AX_FALSE;
        } else {
            if (m_bDrop) {
                LOG_MM_W(CLIENT, "[%d] drop frame because key frame %llu is lost", m_nCookie, frame.nSeqNum);
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

STREAM_RING_ELEMENT_T *CStreamRingBuffer::Peek(AX_S32 nTimeOut /* = -1 */) {
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
        LOG_MM_E(CLIENT, "[%d] no element to pop", m_nCookie);
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

AX_VOID CStreamRingBuffer::Dump(AX_VOID) {
    LOG_MM_C(CLIENT, "[%d] dump stream ring buffer -->", m_nCookie);
    if (empty()) {
        LOG_MM_C(CLIENT, "[%d]     0 element", m_nCookie);
    } else {
        for (AX_U64 i = m_head; i < m_tail; ++i) {
            AX_U32 index = (AX_U32)(i % m_Capacity);
            LOG_MM_C(CLIENT, "[%d]     [%d] seq %llu type %d size %u tick %llu", m_nCookie, index, m_ring[index].stFrame.nSeqNum,
                     m_ring[index].stFrame.enNalu, m_ring[index].stFrame.GetFrameSize(), m_ring[index].nTick);
        }
    }
}
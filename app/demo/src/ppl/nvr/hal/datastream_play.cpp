/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include "datastream_play.hpp"
#include "AppLogApi.h"
#include "ElapsedTimer.hpp"
#include "ax_vdec_api.h"
#include "fs.hpp"
#include "ax_vo_api.h"
#include <fcntl.h>
#include <unistd.h>

#define DS_MAX_SUPPORTED_DEVICE_COUNT (128)
#define DS_MAX_SUPPORTED_STREAM_COUNT (2)
#define TAG "DSIF"

using namespace std;

AX_BOOL CDataStreamPlay::Init(const AXDS_PLAY_INIT_ATTR_T& tAttr) {
    m_tInitAttr = tAttr;

    for (AX_U32 i = 0; i < m_tInitAttr.uMaxDevCnt; ++i) {
        m_mapDev2ThreadPlay[i].resize(m_tInitAttr.uStreamCnt);
        m_mapDev2ThreadParam[i].resize(m_tInitAttr.uStreamCnt);
        m_mapDev2ThreadPlayingTime[i].resize(m_tInitAttr.uStreamCnt);
        m_mapDev2SpeedFactor[i].resize(m_tInitAttr.uStreamCnt);
        m_mapDev2CurrPTS[i].resize(m_tInitAttr.uStreamCnt);
        m_mapDev2Obs[i].resize(m_tInitAttr.uStreamCnt);
        m_mapDev2Mtx[i].resize(m_tInitAttr.uStreamCnt);
        for (AX_U32 j = 0; j < m_tInitAttr.uStreamCnt; ++j) {
            m_mapDev2ThreadPlay[i][j] = new CAXThread();
            m_mapDev2Mtx[i][j] = new std::mutex();
            m_mapDev2ThreadParam[i][j].nDevice = i;
            m_mapDev2ThreadParam[i][j].nStream = j;
            m_mapDev2ThreadParam[i][j].bOnlyIFrameOnReverse = m_tInitAttr.bOnlyIFrameOnReverse;
            m_mapDev2ThreadParam[i][j].bRepeat = AX_FALSE;
            m_mapDev2ThreadParam[i][j].bExited = AX_TRUE;
            m_mapDev2SpeedFactor[i][j] = 1;
        }
    }

    return AX_TRUE;
}

AX_BOOL CDataStreamPlay::DeInit() {
    for (AX_U32 i = 0; i < m_tInitAttr.uMaxDevCnt; ++i) {
        for (AX_U32 j = 0; j < m_tInitAttr.uStreamCnt; ++j) {
            StopPlay(i, j);

            delete m_mapDev2ThreadPlay[i][j];
            m_mapDev2ThreadPlay[i][j] = nullptr;

            delete m_mapDev2Mtx[i][j];
            m_mapDev2Mtx[i][j] = nullptr;
        }
    }

    return AX_TRUE;
}

AX_BOOL CDataStreamPlay::StartPlay(AX_U8 nDeviceID, AX_U8 nStreamID, AX_S32 nDate, AX_S32 nTime /*= 0*/, AX_S32 nTick /*= 0*/, AX_BOOL bReverse /*= AX_FALSE*/, AX_BOOL bRepeat /*= AX_FALSE*/) {
    LOG_MM_I(TAG, "[%d][%d] +++", nDeviceID, nStreamID);

    if (nullptr == m_mapDev2ThreadPlay[nDeviceID][nStreamID]) {
        return AX_FALSE;
    }

    m_mapDev2ThreadParam[nDeviceID][nStreamID].nYYYYMMDD = nDate;
    m_mapDev2ThreadParam[nDeviceID][nStreamID].nHHMMSS = nTime;
    m_mapDev2ThreadParam[nDeviceID][nStreamID].nTickcount = nTick;
    m_mapDev2ThreadParam[nDeviceID][nStreamID].bReverse = bReverse;
    m_mapDev2ThreadParam[nDeviceID][nStreamID].bRepeat = bRepeat;
    m_mapDev2ThreadParam[nDeviceID][nStreamID].bExited = AX_FALSE;
    m_mapDev2ThreadParam[nDeviceID][nStreamID].pThread = m_mapDev2ThreadPlay[nDeviceID][nStreamID];

    memset(&m_mapDev2ThreadPlayingTime[nDeviceID][nStreamID], 0, sizeof(AXDS_PLAY_TIME_HISTORY_INFO_T));

    if (bReverse) {
        char szName[32];
        sprintf(szName, "playR_%d_%d", nDeviceID, nStreamID); /* play reverse */
        if (!m_mapDev2ThreadPlay[nDeviceID][nStreamID]->Start([this](AX_VOID* pArg) -> AX_VOID { ReversePlayThread(pArg); }, &m_mapDev2ThreadParam[nDeviceID][nStreamID], szName, SCHED_FIFO, 99)) {
            LOG_MM_E(TAG, "[%d][%d] Create data stream play thread failed.", nDeviceID, nStreamID);
            return AX_FALSE;
        }
    } else {
        char szName[32];
        sprintf(szName, "playF_%d_%d", nDeviceID, nStreamID); /* play forward */
        if (!m_mapDev2ThreadPlay[nDeviceID][nStreamID]->Start([this](AX_VOID* pArg) -> AX_VOID { PlayThread(pArg); }, &m_mapDev2ThreadParam[nDeviceID][nStreamID], szName, SCHED_FIFO, 99)) {
            LOG_MM_E(TAG, "[%d][%d] Create data stream play thread failed.", nDeviceID, nStreamID);
            return AX_FALSE;
        }
    }

    LOG_MM_I(TAG, "[%d][%d] ---", nDeviceID, nStreamID);
    return AX_TRUE;
}

AX_BOOL CDataStreamPlay::StopPlay(AX_U8 nDeviceID, AX_U8 nStreamID) {
    LOG_MM_I(TAG, "[%d][%d] +++", nDeviceID, nStreamID);
    if (m_mapDev2ThreadPlay[nDeviceID][nStreamID]->IsRunning() || m_mapDev2ThreadPlay[nDeviceID][nStreamID]->IsPausing()) {
        m_mapDev2ThreadPlay[nDeviceID][nStreamID]->Stop();
        m_mapDev2ThreadPlay[nDeviceID][nStreamID]->Join();
    }

    LOG_MM_I(TAG, "[%d][%d] ---", nDeviceID, nStreamID);

    return AX_TRUE;
}

AX_BOOL CDataStreamPlay::PausePlay(AX_U8 nDeviceID, AX_U8 nStreamID) {
    if (m_mapDev2ThreadPlay[nDeviceID][nStreamID]->IsRunning()) {
        m_mapDev2ThreadPlay[nDeviceID][nStreamID]->Pause();
    }

    return AX_TRUE;
}

AX_BOOL CDataStreamPlay::ResumePlay(AX_U8 nDeviceID, AX_U8 nStreamID) {
    if (m_mapDev2ThreadPlay[nDeviceID][nStreamID]->IsPausing()) {
        m_mapDev2ThreadPlay[nDeviceID][nStreamID]->Resume();
    }

    return AX_TRUE;
}

AX_BOOL CDataStreamPlay::IsPlaying(AX_U8 nDeviceID, AX_U8 nStreamID) {
    if (nullptr == m_mapDev2ThreadPlay[nDeviceID][nStreamID]) {
        return AX_FALSE;
    }

    return m_mapDev2ThreadParam[nDeviceID][nStreamID].bExited ? AX_FALSE : AX_TRUE;
}

AX_BOOL CDataStreamPlay::ChangeDirection(AX_U8 nDeviceID, AX_U8 nStreamID, AX_BOOL bReverse, AX_BOOL bRepeat) {
    if (bReverse == m_mapDev2ThreadParam[nDeviceID][nStreamID].bReverse) {
        return AX_TRUE;
    }

    AX_BOOL bIsPausing = m_mapDev2ThreadPlay[nDeviceID][nStreamID]->IsPausing();
    if (bIsPausing) {
        // Resume playing to quit thread via StopPlay
        m_mapDev2ThreadPlay[nDeviceID][nStreamID]->Resume();
    }

    StopPlay(nDeviceID, nStreamID);

    std::tuple<AX_U32, AX_U32, AX_U32> displayTime = GetCurrDisplayedTime(nDeviceID, nStreamID, 0, 0); /* Only layer0-channel0 support backward play now */

    AX_BOOL bNewReverseFlg = m_mapDev2ThreadParam[nDeviceID][nStreamID].bReverse ? AX_FALSE : AX_TRUE;
    StartPlay(nDeviceID, nStreamID, std::get<0>(displayTime), std::get<1>(displayTime), std::get<2>(displayTime), bNewReverseFlg, bRepeat);

    if (bIsPausing) {
        /* Recover to pausing status */
        m_mapDev2ThreadPlay[nDeviceID][nStreamID]->Pause();
    }

    return AX_TRUE;
}

std::tuple<AX_U32, AX_U32, AX_U32> CDataStreamPlay::GetCurrDisplayedTime(AX_U8 nDeviceID, AX_U8 nStreamID, AX_U8 nVoLayer, AX_U8 nVoChannel) {
    std::pair<AX_U32, AX_U32> pairDateTime;
    AX_U32 nTick = 0;

    AX_U64 u64ChnPts = 0;
    AX_S32 nRet = AX_VO_GetChnPTS(nVoLayer, nVoChannel, &u64ChnPts);
    if (0 != nRet || 0 == u64ChnPts) {
        LOG_M_W(TAG, "[%d][%d] Get PTS(%d) from vo channel failed, ret=0x%x.", nVoLayer, nVoChannel, u64ChnPts, nRet);

        return GetCurrPlayedTime(nDeviceID, nStreamID);
    }

    AXDS_DATETIME_T tVoDisplayedTime;
    if (FindHistoryTime(nDeviceID, nStreamID, u64ChnPts, tVoDisplayedTime)) {
        pairDateTime = CElapsedTimer::GetInstance()->GetDateTimeIntVal(tVoDisplayedTime.Value() / 1000);
        nTick = tVoDisplayedTime.uUsec;
        LOG_MM_W(TAG, "Find displayed time(%d %d %d) in history.", pairDateTime.first, pairDateTime.second, tVoDisplayedTime.uUsec);
    } else {
        LOG_MM_W(TAG, "Can not find displayed time(%lld) in history, starts at current played time.", u64ChnPts);
        return GetCurrPlayedTime(nDeviceID, nStreamID);
    }

    return make_tuple(pairDateTime.first, pairDateTime.second, nTick);
}

AX_BOOL CDataStreamPlay::ChangeSpeed(AX_U8 nDeviceID, AX_U8 nStreamID, AX_F32 fFactor) {
    if (fFactor < 1e-6) {
        LOG_M_E(TAG, "Invalid speed factor %f", fFactor);
        return AX_FALSE;
    }

    m_mapDev2SpeedFactor[nDeviceID][nStreamID] = fFactor;
    return AX_TRUE;
}

std::tuple<AX_U32, AX_U32, AX_U32> CDataStreamPlay::GetCurrPlayedTime(AX_U8 nDeviceID, AX_U8 nStreamID) {
    AXDS_PLAY_TIME_HISTORY_INFO_T& tHistoryInfo = m_mapDev2ThreadPlayingTime[nDeviceID][nStreamID];
    AXDS_DATETIME_T& tCurrPlayedTime = tHistoryInfo.tHistoryFrameTimeInfo[tHistoryInfo.nLatestIndex];
    if (0 == tCurrPlayedTime.uSec && 0 == tCurrPlayedTime.uUsec) {
        return make_tuple(0, 0, 0);
    }

    std::pair<AX_U32, AX_U32> pairDateTime = CElapsedTimer::GetInstance()->GetDateTimeIntVal(tCurrPlayedTime.uSec);

    return std::make_tuple(pairDateTime.first, pairDateTime.second, tCurrPlayedTime.uUsec);
}

AX_VOID CDataStreamPlay::PlayThread(AX_VOID* pArg) {
    AXDS_PLAY_THREAD_PARAM_T* pParams = (AXDS_PLAY_THREAD_PARAM_T*)pArg;
    AX_U32 nDeviceID = pParams->nDevice;
    AX_U32 nStreamID = pParams->nStream;
    AX_U32 nYYYYMMDD = pParams->nYYYYMMDD;
    AX_U32 nHHMMSS = pParams->nHHMMSS;
    AX_U32 nTick = pParams->nTickcount;
    AX_BOOL bRepeat = pParams->bRepeat;
    CAXThread* pThread = pParams->pThread;

    LOG_MM_I(TAG, "[%d][%d] +++", nDeviceID, nStreamID);

    CDataStreamIndFile* pInstance = CreateSearchInstance(nDeviceID, nStreamID, nYYYYMMDD, nHHMMSS, nTick);
    if (nullptr == pInstance) {
        LOG_MM_E(TAG, "Create search instance failed.");
        return;
    } else {
        LOG_MM_C(TAG, "Start play at time(%d, %d, %d)", nYYYYMMDD, nHHMMSS, nTick);
    }

    AXDS_DATETIME_T tFirstValidFrameTime = {(AX_U32)CElapsedTimer::GetTimeTVal(nYYYYMMDD, nHHMMSS), nTick};

    AXIF_FILE_INFO_EX_T tInfo = pInstance->FindInfo(0); /* Ignore the case that fps changes between different dsf */
    CDSIterator itDSStart = pInstance->begin();
    CDSIterator itDSEnd = pInstance->end();

    do {
        if (itDSStart != itDSEnd) {
            if (!pThread->IsRunning() && !pThread->IsPausing()) {
                itDSStart.Terminate();
                break;
            }

            AXDS_FRAME_HEADER_T* pFrameStart = *itDSStart;

            /* Save time for reversely play at the paused time */
            GenCurrPTS(nDeviceID, nStreamID, tInfo, AX_FALSE, AX_FALSE);

            AXDS_PLAY_TIME_HISTORY_INFO_T& tHistoryInfo = m_mapDev2ThreadPlayingTime[nDeviceID][nStreamID];
            if (tHistoryInfo.nLatestIndex == MAX_HISTORY_TIME_INFO_COUNT - 1) {
                tHistoryInfo.nLatestIndex = 0;
            } else {
                tHistoryInfo.nLatestIndex += 1;
            }

            tHistoryInfo.nHistoryPTS[tHistoryInfo.nLatestIndex] = m_mapDev2CurrPTS[nDeviceID][nStreamID];
            tHistoryInfo.tHistoryFrameTimeInfo[tHistoryInfo.nLatestIndex] = pFrameStart->tTimeStamp;

            {
                std::lock_guard<std::mutex> lck(*m_mapDev2Mtx[nDeviceID][nStreamID]);
                for (auto&& m : m_mapDev2Obs[nDeviceID][nStreamID]) {
                    if (!pThread->IsRunning() && !pThread->IsPausing()) {
                        break;
                    }

                    STREAM_FRAME_T tFrame;
                    memset(&tFrame, 0, sizeof(STREAM_FRAME_T));
                    tFrame.enPayload = tInfo.tInfo.uEncodeType;
                    tFrame.nPrivData = 0;

                    if (pFrameStart->tTimeStamp < tFirstValidFrameTime) {
                        tFrame.frame.stVideo.nPTS = -1;
                    } else {
                        tFrame.frame.stVideo.nPTS = m_mapDev2CurrPTS[nDeviceID][nStreamID];
                    }

                    tFrame.frame.stVideo.enNalu = pFrameStart->GetNaluType();
                    tFrame.frame.stVideo.pData = (AX_U8*)pFrameStart + pFrameStart->uHeaderSize;
                    tFrame.frame.stVideo.nLen = pFrameStart->uFrameSize;
                    if (m != nullptr) {
                        LOG_MM_D(TAG, "Playback frame(Type:%d, Size:%d, PTS:%lld(FILE:%lld), GopInd:%d)", tFrame.frame.stVideo.enNalu, tFrame.frame.stVideo.nLen, tFrame.frame.stVideo.nPTS, pFrameStart->tTimeStamp.Value(), pFrameStart->uFrameSubType);
                        if (!m->OnRecvStreamData(tFrame)) {
                            break;
                        }
                    }
                }
            }

            /* Give a chance to UnRegisterObserver to get the mutex in STOP process */
            std::this_thread::sleep_for(std::chrono::milliseconds(1));

            itDSStart++;
        } else {
            LOG_MM_C(TAG, "********** [%d][%d] Play reaches the END (Repeat: %d) **********", nDeviceID, nStreamID, bRepeat);

            if (bRepeat) {
                // Restart from begin
                itDSStart.Destroy();

                pInstance = CreateSearchInstance(nDeviceID, nStreamID, nYYYYMMDD);
                if (nullptr == pInstance) {
                    LOG_MM_E(TAG, "Create search instance failed.");
                    return;
                }

                itDSStart = pInstance->begin();
                itDSEnd = pInstance->end();

                tFirstValidFrameTime.Reset();
            } else {
                break;
            }
        }
    } while (1);

    itDSStart.Destroy();
    pInstance = nullptr;
    pParams->bExited = AX_TRUE;

    LOG_MM_I(TAG, "[%d][%d] ---", nDeviceID, nStreamID);
}

AX_VOID CDataStreamPlay::ReversePlayThread(AX_VOID* pArg) {
    AXDS_PLAY_THREAD_PARAM_T* pParams = (AXDS_PLAY_THREAD_PARAM_T*)pArg;
    AX_U32 nDeviceID = pParams->nDevice;
    AX_U32 nStreamID = pParams->nStream;
    AX_U32 nYYYYMMDD = pParams->nYYYYMMDD;
    AX_U32 nHHMMSS = pParams->nHHMMSS;
    AX_U32 nTick = pParams->nTickcount;
    AX_BOOL bRepeat = pParams->bRepeat;
    CAXThread* pThread = pParams->pThread;
    AX_U32 nPlayedGopCount = 0;

    LOG_MM_I(TAG, "[%d][%d] +++", nDeviceID, nStreamID);

    CDataStreamIndFile* pInstance = CreateSearchInstance(nDeviceID, nStreamID, nYYYYMMDD, nHHMMSS, nTick);
    if (nullptr == pInstance) {
        LOG_MM_E(TAG, "Create search instance failed.");
        return;
    } else {
        LOG_MM_C(TAG, "Start reverse play at time(%d, %d, %d)", nYYYYMMDD, nHHMMSS, nTick);
    }

    m_mapDev2CurrPTS[nDeviceID][nStreamID] = 0;
    AXIF_FILE_INFO_EX_T tInfo = pInstance->FindInfo(0); /* Ignore the case that fps changes between different dsf */

    CDSIterator itDSStart = pInstance->rbegin();
    CDSIterator itDSEnd = pInstance->rend();
    do {
        if (itDSStart != itDSEnd) {
            if (!pThread->IsRunning() && !pThread->IsPausing()) {
                itDSStart.Terminate();
                break;
            }

            AXIF_FRAME_RELOCATION_INFO_T tReloacteInfo = itDSStart.GetRelocatedInfo();
            nPlayedGopCount = GopPlay(nDeviceID, nStreamID, nYYYYMMDD, nHHMMSS, nTick, tInfo, tReloacteInfo, nPlayedGopCount);

            itDSStart++;
        } else {
            LOG_MM_C(TAG, "********** [%d][%d] Reverse play reaches the START (Repeat: %d) **********", nDeviceID, nStreamID, bRepeat);

            if (bRepeat) {
                // Restart from begin
                itDSStart.Destroy();

                nHHMMSS = 235959;
                nTick = 999;
                pInstance = CreateSearchInstance(nDeviceID, nStreamID, nYYYYMMDD, nHHMMSS, nTick);
                if (nullptr == pInstance) {
                    LOG_MM_E(TAG, "Create search instance failed.");
                    return;
                }

                itDSStart = pInstance->rbegin();
                itDSEnd = pInstance->rend();
            } else {
                break;
            }
        }
    } while (1);

    itDSStart.Destroy();
    pInstance = nullptr;
    pParams->bExited = AX_TRUE;

    LOG_MM_I(TAG, "[%d][%d] ---", nDeviceID, nStreamID);
}

AX_U32 CDataStreamPlay::GopPlay(AX_U8 nDeviceID, AX_U8 nStreamID, AX_U32 nYYYYMMDD, AX_U32 nHHMMSS, AX_U32 nTick, const AXIF_FILE_INFO_EX_T& tInfo, AXIF_FRAME_RELOCATION_INFO_T& tLocationInfo, AX_U32 nLastPlayedCount /*= 0*/) {
    LOG_MM_D(TAG, "[%d][%d] +++", nDeviceID, nStreamID);

    CDataStreamIndFile* pGopSearchInstance = nullptr;
    pGopSearchInstance = CreateSearchInstance(nDeviceID, nStreamID, nYYYYMMDD, 0, 0, AX_TRUE);
    CDSIterator itDSStart = pGopSearchInstance->gop_begin(tLocationInfo.nFileIndex, tLocationInfo.nFrameIndexWithinFile);
    CDSIterator itDSEnd = pGopSearchInstance->gop_end(tLocationInfo.nFileIndex, -1);
    AXDS_DATETIME_T tFirstValidFrameTime = {(AX_U32)CElapsedTimer::GetTimeTVal(nYYYYMMDD, nHHMMSS), nTick};
    std::pair<AX_U32, AX_U32> pairDateTime;

    AX_S32 nIndex = 0;
    for (; itDSStart != itDSEnd; ++itDSStart) {
        AXDS_FRAME_HEADER_T* pFrameStart = *itDSStart;
        if (nIndex != 0 && NALU_TYPE_IDR == pFrameStart->GetNaluType()) {
            /* break before next IDR frame comes */
            break;
        }

        if (nIndex == 0) {
            AX_U32 nStep = 1000000 / tInfo.tInfo.uFrameRate;
            nStep /= m_mapDev2SpeedFactor[nDeviceID][nStreamID];
            // /* Estimate the IDR's PTS */
            // m_mapDev2CurrPTS[nDeviceID][nStreamID] += nStep * tInfo.tInfo.uGop + nStep * (nLastPlayedCount > 0 ? nLastPlayedCount - 1 : 0);
            // if (nLastPlayedCount > 0) {
            //     /* Padding the PTS of current GOP if last GOP is not full */
            //     m_mapDev2CurrPTS[nDeviceID][nStreamID] -= nStep * (tInfo.tInfo.uGop - nLastPlayedCount);
            // }

            if (nLastPlayedCount > 0) {
                m_mapDev2CurrPTS[nDeviceID][nStreamID] += (nStep * (2 * nLastPlayedCount - 1));
            } else {
                m_mapDev2CurrPTS[nDeviceID][nStreamID] += nStep * tInfo.tInfo.uGop;
            }
        } else {
            GenCurrPTS(nDeviceID, nStreamID, tInfo, AX_TRUE, AX_FALSE);
        }

        AXDS_PLAY_TIME_HISTORY_INFO_T& tHistoryInfo = m_mapDev2ThreadPlayingTime[nDeviceID][nStreamID];
        if (tHistoryInfo.nLatestIndex == MAX_HISTORY_TIME_INFO_COUNT - 1) {
            tHistoryInfo.nLatestIndex = 0;
        } else {
            tHistoryInfo.nLatestIndex += 1;
        }

        tHistoryInfo.nHistoryPTS[tHistoryInfo.nLatestIndex] = m_mapDev2CurrPTS[nDeviceID][nStreamID];
        tHistoryInfo.tHistoryFrameTimeInfo[tHistoryInfo.nLatestIndex] = pFrameStart->tTimeStamp;

        AX_VDEC_GRP_STATUS_T status;
        AX_U32 nLimitBuffCnt = 1;
        while (m_mapDev2ThreadPlay[nDeviceID][nStreamID]->IsRunning() || m_mapDev2ThreadPlay[nDeviceID][nStreamID]->IsPausing()) {
            if (0 != AX_VDEC_QueryStatus(nDeviceID, &status)) {
                break;
            }

            if (status.u32LeftPics[nDeviceID] <= nLimitBuffCnt) {
                break;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }

        if (!m_mapDev2ThreadPlay[nDeviceID][nStreamID]->IsRunning() && !m_mapDev2ThreadPlay[nDeviceID][nStreamID]->IsPausing()) {
            break;
        }

        {
            std::lock_guard<std::mutex> lck(*m_mapDev2Mtx[nDeviceID][nStreamID]);
            for (auto&& m : m_mapDev2Obs[nDeviceID][nStreamID]) {
                STREAM_FRAME_T tFrame;
                memset(&tFrame, 0, sizeof(STREAM_FRAME_T));
                tFrame.enPayload = tInfo.tInfo.uEncodeType;
                if (pFrameStart->tTimeStamp > tFirstValidFrameTime) {
                    tFrame.frame.stVideo.nPTS = -1;
                } else {
                    tFrame.frame.stVideo.nPTS = m_mapDev2CurrPTS[nDeviceID][nStreamID];
                }

                pairDateTime = CElapsedTimer::GetInstance()->GetDateTimeIntVal(pFrameStart->tTimeStamp.Value() / 1000);

                tFrame.frame.stVideo.enNalu = pFrameStart->GetNaluType();
                tFrame.frame.stVideo.pData = (AX_U8*)pFrameStart + pFrameStart->uHeaderSize;
                tFrame.frame.stVideo.nLen = pFrameStart->uFrameSize;
                tFrame.frame.stVideo.u64UserData = nIndex; /* Private data saves index(0 means IDR frame) for StreamContainer to flush one GOP stream */
                if (m != nullptr) {
                    LOG_MM_D(TAG, "[%d] Reverse playback frame(Type:%d, Size:%d, PTS:%lld(FILE:%lld(%d %d)), GopInd:%d)", nIndex, tFrame.frame.stVideo.enNalu, tFrame.frame.stVideo.nLen, tFrame.frame.stVideo.nPTS, pFrameStart->tTimeStamp.Value(), pairDateTime.first, pairDateTime.second, pFrameStart->uFrameSubType);
                    if (!m->OnRecvStreamData(tFrame)) {
                        break;
                    }
                }
            }
        }

        nIndex++;

        /* Give a chance to UnRegisterObserver to get the mutex in STOP process */
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    itDSStart.Destroy();
    pGopSearchInstance = nullptr;

    LOG_MM_D(TAG, "[%d][%d] ---", nDeviceID, nStreamID);

    return nIndex;
}

std::deque<DISK_FILE_INFO_T> CDataStreamPlay::GetDeviceSubFolders(const string& strParentDir, AX_U8 nDeviceID) {
    AX_CHAR szDevPath[300] = {0};
    sprintf(szDevPath, "%s/DEV_%02d", strParentDir.c_str(), nDeviceID + 1);
    return CDiskHelper::TraverseDirs(szDevPath);
}

vector<AXDS_DEVICE_INFO_T> CDataStreamPlay::GetAllDeviceInfo(const string& strParentDir) {
    vector<AXDS_DEVICE_INFO_T> vecInfo;
    for (AX_U8 i = 0; i < DS_MAX_SUPPORTED_DEVICE_COUNT; ++i) {
        std::deque<DISK_FILE_INFO_T> dirs = GetDeviceSubFolders(strParentDir, i);
        vecInfo.emplace_back(dirs.size() > 0 ? AX_TRUE : AX_FALSE, i, dirs);
    }

    return vecInfo;
}

CVideoInfoMap CDataStreamPlay::GetVideoInfo(const string& strParentDir, const string& strYYYYmmdd) {
    CVideoInfoMap mapVideoInfo;

    AX_U8 nMaxDev = DS_MAX_SUPPORTED_DEVICE_COUNT;
    AX_U8 nMaxStream = DS_MAX_SUPPORTED_STREAM_COUNT;
    string strIndFile;
    AXIF_FILE_HEADER_T tHeader;
    AX_U32 nFileCount = 0;
    for (AX_U8 i = 0; i < nMaxDev; ++i) {
        vector<vector<AXDS_VIDEO_INFO_T>> vecStreamPeriod;
        for (AX_U8 j = 0; j < nMaxStream; ++j) {
            strIndFile = FindIndFile(strParentDir, i, j, strYYYYmmdd.c_str());
            if (!strIndFile.empty()) {
                unique_ptr<CDataStreamIndFile> file = make_unique<CDataStreamIndFile>();
                if (!file->Init(strIndFile.c_str(), AXIF_OPEN_FOR_READ)) {
                    continue;
                }

                tHeader = file->GetFileHeader();
                nFileCount = tHeader.uFileCount;

                vector<AXDS_VIDEO_INFO_T> vecPeriod;
                vecPeriod.reserve(nFileCount);

                for (CDSIFIterator itBegin = file->info_begin(), itEnd = file->info_end(); itBegin != itEnd; ++itBegin) {
                    if ((*itBegin).tInfo.uFrameCount > 0) {
                        AXDS_VIDEO_INFO_T period((*itBegin).tInfo.tStartTime, (*itBegin).tInfo.tEndTime);
                        vecPeriod.push_back(period);
                    }
                }

                file->DeInit();

                if (vecPeriod.size() > 0) {
                    vecStreamPeriod.emplace_back(vecPeriod);
                }
            }
        }

        if (vecStreamPeriod.size() > 0) {
            mapVideoInfo[i] = vecStreamPeriod;
        }
    }

    return mapVideoInfo;
}

AX_BOOL CDataStreamPlay::GetStreamInfo(AX_U8 nDeviceID, AX_U8 nStreamID, AX_U32 nDateIntVal, AXDS_STREAM_INFO_T& tOutInfo) {
    AX_CHAR szDate[32] = {0};
    sprintf(szDate, "%04d-%02d-%02d", nDateIntVal / 10000, (nDateIntVal % 10000) / 100, nDateIntVal % 100);

    string strIndFile = FindIndFile(m_tInitAttr.strParentDir, nDeviceID, nStreamID, szDate);
    if (!strIndFile.empty()) {
        unique_ptr<CDataStreamIndFile> file = make_unique<CDataStreamIndFile>();
        if (!file->Init(strIndFile.c_str(), AXIF_OPEN_FOR_READ)) {
            return AX_FALSE;
        }

        for (CDSIFIterator itBegin = file->info_begin(), itEnd = file->info_end(); itBegin != itEnd; ++itBegin) {
            AXIF_FILE_INFO_T& tInfo = (*itBegin).tInfo;
            std::pair<AX_U32, AX_U32> pairDateTime = CElapsedTimer::GetInstance()->GetDateTimeIntVal(tInfo.tStartTime.uSec);
            if (pairDateTime.first == nDateIntVal) {
                tOutInfo.uEncodeType = tInfo.uEncodeType;
                tOutInfo.uWidth = tInfo.uWidth;
                tOutInfo.uHeight = tInfo.uHeight;
                tOutInfo.uFrameRate = tInfo.uFrameRate;
                tOutInfo.uGop = tInfo.uGop;

                return AX_TRUE;
            }
        }

        file->DeInit();
    }

    return AX_FALSE;
}

CDataStreamIndFile* CDataStreamPlay::CreateSearchInstance(AX_U8 nDeviceID, AX_U8 nStreamID, AX_S32 nDate, AX_S32 nTime /*= 0*/, AX_S32 nTick /*= 0*/, AX_BOOL bGopMode /*= AX_FALSE*/) {
    AX_S32 nYear = nDate / 10000;
    AX_S32 nMonth = (nDate % 10000) / 100;
    AX_S32 nDay = nDate % 100;

    AX_CHAR szDate[32] = {0};
    sprintf(szDate, "%02d-%02d-%02d", nYear, nMonth, nDay);
    string strIndexFile = CDataStreamPlay::FindIndFile(m_tInitAttr.strParentDir, nDeviceID, nStreamID, szDate);
    if (strIndexFile.empty()) {
        return nullptr;
    }

    CDataStreamIndFile* dsif = new CDataStreamIndFile();
    if (!dsif->Init(strIndexFile.c_str(), AXIF_OPEN_FOR_READ, nDate, nTime, nTick, bGopMode)) {
        return nullptr;
    }

    return dsif;
}

AX_VOID CDataStreamPlay::DestroySearchInstance(CDataStreamIndFile* pInstance) {
    if (nullptr == pInstance) {
        return;
    }

    pInstance->DeInit();
    delete pInstance;
    pInstance = nullptr;
}

string CDataStreamPlay::FindIndFile(std::string strParentDir, AX_U8 nDeviceID, AX_U8 nStreamID, string strYYYYmmdd) {
    AX_CHAR szFilePath[300] = {0};
    sprintf(szFilePath, "%s/DEV_%02d/%s/%s", strParentDir.c_str(), nDeviceID + 1, strYYYYmmdd.c_str(), CDataStreamIndFile::FormatFileName(nStreamID).c_str());
    fs::path p(szFilePath);
    if (fs::exists(p)) {
        return szFilePath;
    }

    return string();
}

AX_VOID CDataStreamPlay::GenCurrPTS(AX_U8 nDeviceID, AX_U8 nStreamID, const AXIF_FILE_INFO_EX_T& tInfo, AX_BOOL bReverse, AX_BOOL bOnlyIFrameOnReverse) {
    AX_S32 nStep = 0;
    if (bReverse) {
        if (bOnlyIFrameOnReverse) {
            nStep = (AX_F32)tInfo.tInfo.uGop / tInfo.tInfo.uFrameRate * 1000000;
        } else {
            nStep = 1000000 / tInfo.tInfo.uFrameRate * -1;
        }
    } else {
        nStep = 1000000 / tInfo.tInfo.uFrameRate;
    }

    nStep /= m_mapDev2SpeedFactor[nDeviceID][nStreamID];

    if (nStep < 0 && m_mapDev2CurrPTS[nDeviceID][nStreamID] < (AX_U32)(nStep * -1)) {
        LOG_MM_E(TAG, "[%d][%d][reverse:%d][step:%d] Next PTS %d is not valid, ignore the frame.", nDeviceID, nStreamID, bReverse, nStep, m_mapDev2CurrPTS[nDeviceID][nStreamID] + nStep);
        return;
    }

    m_mapDev2CurrPTS[nDeviceID][nStreamID] += nStep;
}

AX_BOOL CDataStreamPlay::RegisterObserver(AX_U8 nDeviceID, AX_U8 nStreamID, IStreamObserver* pObs) {
    if (!pObs) {
        LOG_M_E(TAG, "%s: observer is nil", __func__);
        return AX_FALSE;
    }

    std::lock_guard<std::mutex> lck(*m_mapDev2Mtx[nDeviceID][nStreamID]);
    std::list<IStreamObserver*>& observers = m_mapDev2Obs[nDeviceID][nStreamID];
    auto it = std::find(observers.begin(), observers.end(), pObs);
    if (it != observers.end()) {
        LOG_M_W(TAG, "%s: observer has been registed", __func__);
    } else {
        observers.push_back(pObs);
        LOG_M_I(TAG, "%s: regist observer %p ok", __func__, pObs);
    }

    return AX_TRUE;
}

AX_BOOL CDataStreamPlay::UnRegisterObserver(AX_U8 nDeviceID, AX_U8 nStreamID, IStreamObserver* pObs) {
    if (!pObs) {
        LOG_M_E(TAG, "%s: observer is nil", __func__);
        return AX_FALSE;
    }

    std::lock_guard<std::mutex> lck(*m_mapDev2Mtx[nDeviceID][nStreamID]);

    std::list<IStreamObserver*>& observers = m_mapDev2Obs[nDeviceID][nStreamID];
    auto it = std::find(observers.begin(), observers.end(), pObs);
    if (it != observers.end()) {
        observers.remove(pObs);
        LOG_M_I(TAG, "%s: unregist observer %p ok", __func__, pObs);
        return AX_TRUE;
    } else {
        LOG_M_E(TAG, "%s: observer %p is not registed", __func__, pObs);
        return AX_FALSE;
    }
}

AX_BOOL CDataStreamPlay::FindHistoryTime(AX_U8 nDeviceID, AX_U8 nStreamID, AX_U64 nPTS, AXDS_DATETIME_T& tOutTime) {
    AXDS_PLAY_TIME_HISTORY_INFO_T& tHistoryInfo = m_mapDev2ThreadPlayingTime[nDeviceID][nStreamID];
    for (AX_U32 i = 0; i < MAX_HISTORY_TIME_INFO_COUNT; ++i) {
        if (0 != tHistoryInfo.nHistoryPTS[i] && tHistoryInfo.nHistoryPTS[i] == nPTS) {
            tOutTime = tHistoryInfo.tHistoryFrameTimeInfo[i];
            return AX_TRUE;
        }
    }

    LOG_MM_D(TAG, "Can not find history time for VO pts: %lld", nPTS);
    for (AX_U32 i = 0; i < MAX_HISTORY_TIME_INFO_COUNT; ++i) {
        LOG_MM_D(TAG, "    [%d] Saved history time info: Displayed %lld, File %lld", i, tHistoryInfo.nHistoryPTS[i], tHistoryInfo.tHistoryFrameTimeInfo[i].Value());
    }

    return AX_FALSE;
}
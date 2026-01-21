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
#include "haltype.hpp"
#include "AXThread.hpp"
#include "DiskHelper.hpp"
#include "datastreamfile.hpp"
#include "datastreamIndFile.hpp"
#include "AppLogApi.h"
#include "istream.hpp"
#include "AXNVRFrameworkDefine.h"

#include <string>
#include <map>
#include <vector>
#include <deque>
#include <stdio.h>
#include <QFutureWatcher>
#include <QFutureInterface>

#define MAX_DISK_NUMBER (16)


typedef enum {
    AX_DS_STREAM_STATUS_INIT = -1,
    AX_DS_STREAM_STATUS_STOPPED = 0,
    AX_DS_STREAM_STATUS_STARTED = 1,
    AX_DS_STREAM_STATUS_MAX
} AX_DS_STREAM_STATUS_E;

typedef struct AXDS_SAVE_INIT_ATTR {
    AX_CHAR szParentDir[MAX_DISK_NUMBER][256]; // Parent directory for generating data files
    AX_U8   uMaxDevCnt; // Maximum number of channels
    AX_U8   uStreamCnt; // Number of sub-streams
    AX_U32  uMaxDevSpace; // Maximum total video space per channel, unit: MB
    AX_U8   uMaxFilePeriod; // Maximum file storage time (unit: minutes)
    AX_BOOL bGenIFOnClose; // Whether to generate an index file when the file is closed
} AXDS_RECORD_INIT_ATTR_T;

typedef struct CURRENT_FILE_INFO {
    AX_S32 nFD;
    AX_U64 nStartSecond;
    AX_U32 nDay;

    CURRENT_FILE_INFO() {
        nFD = AX_DS_INVALID_HANDLE;
        nStartSecond = 0;
        nDay = 0;
    }
} CURRENT_FILE_INFO_T;

typedef struct REMOVE_THREAD_PARAM {
    AX_U8 nDeviceID;
    AX_U8 nStreamID;
    AX_U8 nRemoveDays;

    REMOVE_THREAD_PARAM() {
        nDeviceID = -1;
        nStreamID = -1;
        nRemoveDays = 1;
    }

    REMOVE_THREAD_PARAM(AX_U8 nDevice, AX_U8 nStream, AX_U8 nDays) {
        nDeviceID = nDevice;
        nStreamID = nStream;
        nRemoveDays = nDays;
    }
} REMOVE_THREAD_PARAM_T, *REMOVE_THREAD_PARAM_PTR;

typedef struct STREAM_MONITOR_INFO {
    AX_U32 nDevice;
    AX_U32 nStream;
    AX_U64 nSeq;
    AX_DS_STREAM_STATUS_E eStatus;
    AX_U32 nSize;
    AX_U32 nFramerate;
    AX_F32 fBitrate;

    STREAM_MONITOR_INFO() {
        memset(this, 0, sizeof(STREAM_MONITOR_INFO));
        eStatus = AX_DS_STREAM_STATUS_INIT;
    }

    AX_VOID Print() {
        if (eStatus != AX_DS_STREAM_STATUS_INIT) {
            LOG_M_C("DATA_STREAM_R", "[%d][%d] Seq: %d, Total Size: %.2f MB, Status: %s, Bitrate: %.2f kbps, FPS: %d"
                    , nDevice
                    , nStream
                    , nSeq
                    , (AX_F32)nSize / 1000 / 1000
                    , AX_DS_STREAM_STATUS_STARTED == eStatus ? "STARTED" : "STOPPED"
                    , fBitrate
                    , nFramerate);
        }
    }
} STREAM_MONITOR_INFO_T;


class CDataStreamRecord {
public:
    CDataStreamRecord(AX_VOID) = default;

    AX_BOOL Init(const AXDS_RECORD_INIT_ATTR_T& tAttr);
    AX_BOOL DeInit();
    AX_BOOL Start(AX_U8 nDeviceID, AX_U8 nStreamID, AXDSF_INIT_ATTR_T& tInitAttr);
    AX_BOOL Stop(AX_U8 nDeviceID, AX_U8 nStreamID);
    AX_BOOL StopAll();

    /* NOTICE: tFrameHeader.uFrameType must be filled by caller */
    AX_BOOL Save(AX_U8 nDeviceID, AX_U8 nStreamID, AXDS_FRAME_HEADER_T& tFrameHeader, const AX_U8* pData, AX_U32 nSize);

protected:
    AX_BOOL Prepare();

private:
    AX_BOOL CheckInitAttr(const AXDS_RECORD_INIT_ATTR_T& tAttr);
    AX_BOOL CheckStream(AX_U8 nDeviceID, AX_U8 nStreamID, AX_BOOL bCheckOpen = AX_FALSE);
    AX_BOOL CheckFrameType(AX_U8 nFrameType);
    AX_BOOL IsTypeForSwitch(AX_U8 nFrameType);
    AX_VOID SwitchDestination(AX_U8 nDeviceID, AX_U8 nStreamID, AX_U32 nSize, AX_U8 nFrameType);
    AX_S32  CreateDataFile(AX_U8 nDeviceID, AX_U8 nStreamID);
    AX_BOOL CloseDataFile(AX_U8 nDeviceID, AX_U8 nStreamID);
    AX_S32  SwitchFile(AX_U8 nDeviceID, AX_U8 nStreamID);

    AX_VOID ThreadStreamMonitor(AX_VOID* pArg);

private:
    AXDS_RECORD_INIT_ATTR_T m_tInitAttr;
    std::map<AX_U8, std::string> m_mapDev2Dir;
    std::map<AX_U8, AX_U64> m_mapDev2TotalSize;

    std::map<AX_U8, std::vector<AX_S32>> m_mapDev2Status;
    std::map<AX_U8, std::vector<CURRENT_FILE_INFO_T>> m_mapDev2FileInfo;
    std::map<AX_U8, std::vector<CDataStreamFile*>> m_mapDev2DSF;
    std::map<AX_U8, std::vector<CDataStreamIndFile*>> m_mapDev2DSIF;
    std::map<AX_U8, std::vector<std::mutex*>> m_mapDev2Mutex;
    std::map<AX_U8, std::vector<STREAM_MONITOR_INFO_T>> m_mapDev2MonitorInfo;

    std::map<AX_U8, AX_BOOL> m_mapDev2DiskFullFlg;
    /* Record the type of the previous frame to determine that file switching occurs only when an SPS or IDR frame is encountered, and consecutive occurrences of SPS-PPS-IDR will result in a switch only once. */
    AX_U8 m_nLastFrameType {0};
    AX_NVR_DATA_STREAM_CONFIG_T m_tConfig;

    CAXThread m_threadStreamMonitor;
};

class CDataStreamObserver : public IStreamObserver {
public:
    CDataStreamObserver() = default;

    CDataStreamObserver(AX_U8 nDevice, AX_U8 nStream, CDataStreamRecord* pDataStream) {
        m_nDeviceID = nDevice;
        m_nStreamID = nStream;
        m_pDataStream = pDataStream;
    };

    AX_VOID SetAttr(AX_U8 nDevice, AX_U8 nStream, CDataStreamRecord* pDataStream) {
        m_nDeviceID = nDevice;
        m_nStreamID = nStream;
        m_pDataStream = pDataStream;
    };

    AX_BOOL OnRecvStreamData(CONST STREAM_FRAME_T& tStreamFrame) override {
        AXDS_FRAME_HEADER_T tHeader;
        tHeader.uFrameType = tStreamFrame.frame.stVideo.enNalu;
        tHeader.uFrameSize = tStreamFrame.frame.stVideo.nLen;
        tHeader.tTimeStamp.Fill();
        tHeader.uFrameSubType = 0;
        tHeader.uExtDataLen = 0;

        if (m_pDataStream) {
            m_pDataStream->Save(m_nDeviceID, m_nStreamID, tHeader, tStreamFrame.frame.stVideo.pData, tStreamFrame.frame.stVideo.nLen);
        }

        return AX_TRUE;
    }

    AX_BOOL OnRecvStreamInfo(CONST STREAM_INFO_T& stInfo) override {
        LOG_M_C("OBS", "[%d - %s] tracks:", stInfo.nCookie, stInfo.strURL.c_str());
        for (auto&& kv : stInfo.tracks) {
            if (PT_H264 == kv.second.enPayload || PT_H265 == kv.second.enPayload) {
                LOG_M_C("OBS", "[%d] video: payload %d, profile %d level %d, num_ref_frames %d, %dx%d %d fps", stInfo.nCookie, kv.second.enPayload,
                        kv.second.info.stVideo.nProfile, kv.second.info.stVideo.nLevel, kv.second.info.stVideo.nNumRefs,
                        kv.second.info.stVideo.nWidth, kv.second.info.stVideo.nHeight, kv.second.info.stVideo.nFps);
            }
        }

        return AX_TRUE;
    }

    AX_VOID OnNotifyConnStatus(CONST AX_CHAR* pUrl, CONNECT_STATUS_E enStatus) override {
        switch (enStatus) {
            case CONNECTED:
                LOG_M_C("OBS", "%s is connected", pUrl);
                break;
            case RECONNECT:
                LOG_M_C("OBS", "reconnecting to %s", pUrl);
                break;
            case DISCONNECT:
                LOG_M_C("OBS", "%s is disconnected", pUrl);
                break;
        }
    }

protected:
    CDataStreamRecord* m_pDataStream {nullptr};
    AX_U8 m_nDeviceID {0};
    AX_U8 m_nStreamID {0};
};

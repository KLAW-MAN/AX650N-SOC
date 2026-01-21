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
#include "AXNVRDisplayCtrl.h"
#include "AXNVRChannel.h"
#include "AXNVRFrameworkDefine.h"
#include "datastream_play.hpp"

#include "ax_base_type.h"
#include "detector.hpp"
#include <vector>
#include <map>
#include <mutex>
using namespace std;


typedef struct _AX_NVR_FILE_MGR_ATTR_T {
    AX_U32 nMaxCount = MAX_DEVICE_PLAYBACK_COUNT;
    CAXNVRDisplayCtrl *pPrimary = nullptr;
    CDataStreamPlay *pPlayback = nullptr;
    string strPath = "";
    AX_BOOL bReverse = AX_FALSE;
    AX_BOOL bRepeat = AX_FALSE;
    AX_F32 nSpeedfactor = 1.0f;
} AX_NVR_FILE_MGR_ATTR_T;

typedef struct _AX_NVR_FILE_T {
    AX_NVR_DEV_ID nChannelId = -1;
    AX_NVR_CHN_IDX_TYPE enPlayChnIdx = AX_NVR_CHN_IDX_TYPE::MAIN;
    CAXNVRChannel *pPlaybackChn = nullptr;
    AXDS_DEVICE_INFO_T stDeviceInfo;
} AX_NVR_FILE_T;

typedef void (*UpdateCallback)(void* context);

/**
 * @brief for preivew and round-patrol
 *
 */
class CAXNVRPlaybakCtrl {
public:
    CAXNVRPlaybakCtrl(AX_VOID) = default;
    virtual ~CAXNVRPlaybakCtrl(AX_VOID) = default;

    AX_VOID Init(const AX_NVR_FILE_MGR_ATTR_T &stAttr);
    AX_VOID DeInit();

    //
    AX_VOID SetAttr(const AX_NVR_FILE_MGR_ATTR_T &stAttr);

    std::vector<AX_NVR_DEV_ID> GetDeviceFiles(AX_VOID);
    std::vector<AXDS_VIDEO_INFO_T> GetDeviceFileInfo(AX_NVR_DEV_ID nDeviceID, const string &strDate);

    // virtual AX_NVR_DEV_INFO_T SelectDeviceFile(AX_NVR_DEV_ID nDeviceID) = 0;

    // virtual void SetUpdateCallback(UpdateCallback callback, void* context) = 0;

    // virtual void SetAttr(AX_VOID) = 0;
    // virtual void GetAttr(AX_VOID) = 0;

    AX_BOOL Update(const ax_nvr_channel_vector &vecChn);
    AX_BOOL Start(const ax_nvr_channel_vector &vecChn, AX_U32 nDate, AX_U32 nTime, AX_U32 nTick, AX_BOOL bReverse, AX_BOOL bRepeat);
    AX_BOOL Stop(const ax_nvr_channel_vector &vecChn);
    AX_BOOL StopAll();
    AX_BOOL PreStopVo();
    AX_BOOL ContinueVo();

    // virtual AX_VOID SeekTo(AX_U64 mTimestamp) = 0;

    AX_BOOL PauseResume(const ax_nvr_channel_vector &vecChn, AX_BOOL bForceResume = AX_FALSE);
    AX_BOOL SetSpeed(const ax_nvr_channel_vector &vecChn, AX_F32 nSpeedfactor = 1.0, AX_BOOL bNeedReset = AX_FALSE);
    AX_BOOL Step(const ax_nvr_channel_vector &vecChn, AX_BOOL bReverse);
    AX_BOOL SwitchMainSub1(const ax_nvr_channel_vector &vecChn, AX_U32 nDefaultDate = 0, AX_U32 nDefaultTime = 0);
    AX_BOOL ChangeResolution(const ax_nvr_channel_vector &vecChn, AX_S32 nNewVoIfSync);
    AX_BOOL ZoomAndMove(const AX_NVR_DEV_ID nDeviceID, const AX_NVR_RECT_T &stCropRect, AX_BOOL bCrop);
    AX_BOOL RestorePauseState(const ax_nvr_channel_vector &vecChn);

    // get attr-resolution
    AX_BOOL GetResolution(AX_NVR_DEV_ID nDeviceID, AX_U32 &nWidth, AX_U32 &nHeight);

    // virtual AX_U32 GetCurrentStatus() = 0;
    // virtual AX_U64 GetCurrentTimestamp() = 0;

private:
    std::mutex mutex_;
    AX_BOOL m_bInit = AX_FALSE;
    AX_NVR_FILE_MGR_ATTR_T m_stAttr;

    std::map<AX_NVR_DEV_ID, AX_NVR_FILE_T> m_mapDevice;

protected:
    CAXNVRChannel *createPlaybackChannel(AX_NVR_DEV_ID nDevID);
    AX_BOOL pause_resume(const vector<AX_NVR_DEV_ID> &vecDevID);
};
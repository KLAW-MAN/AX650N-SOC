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

#include <list>
#include <map>
#include <mutex>
#include "AXFrame.hpp"
#include "AXLockQ.hpp"
#include "AXThread.hpp"
#include "BaseLinkage.h"
#include "BaseSensor.h"
#include "IModule.h"
#include "IObserver.h"
#include "WebServer.h"

namespace AX_ITS {

#define MAX_SENSOR_COUNT (2)
#define MAX_PIPE_PER_DEV (3)
#define MAX_INFO_SIZE (10)

typedef enum {
    APP_SAMPLE_PIPE_MODE_VIDEO,
    APP_SAMPLE_PIPE_MODE_PICTURE,
    APP_SAMPLE_PIPE_MODE_FLASH_SNAP, /* Snap of flash lamp */
    APP_SAMPLE_PIPE_MODE_MAX,
} APP_SAMPLE_PIPE_MODE_E;

typedef enum {
    APP_PIPE_COMBO_TYPE_NORMAL,
    APP_PIPE_COMBO_TYPE_SS,       // None FPGA
    APP_PIPE_COMBO_TYPE_TS,       // None FPGA
    APP_PIPE_COMBO_TYPE_TS_MODE0, // Electronic Police Checkpoint: random flash
    APP_PIPE_COMBO_TYPE_TS_MODE1, // Electronic Police Checkpoint: no flash
    APP_PIPE_COMBO_TYPE_TS_MODE2, // Electronic Police Checkpoint: continuous flash
    APP_PIPE_COMBO_TYPE_MAX,
} APP_PIPE_COMBO_TYPE_E;

typedef enum {
    APP_FLASH_FRAME_RANDOM = -1,
    APP_FLASH_FRAME_NONE = 0,
} APP_FLASH_FRAME_TYPE_E;

typedef struct _PIPE_FRAMERATE_INFO_T {
    AX_U8 nPipeID;
    AX_F32 fPipeFramerate;
    AX_BOOL bSnapshot;
    AX_BOOL bDummy;

    _PIPE_FRAMERATE_INFO_T(AX_U8 nPipe, AX_F32 fFramerate, AX_BOOL bSnap, AX_BOOL bDum) {
        nPipeID = nPipe;
        fPipeFramerate = fFramerate;
        bSnapshot = bSnap;
        bDummy = bDum;
    }
} PIPE_FRAMERATE_INFO_T;

typedef struct _RAW_DISPATCH_THREAD_PARAM {
    AX_U8 nSnsID;
    AX_U8 nDevID;
    AX_F32 fDevFramerate;
    vector<PIPE_FRAMERATE_INFO_T> vecTargetPipeFramerate;
    AX_SNS_HDR_MODE_E eHdrMode;
    AX_BOOL bThreadRunning;
    std::thread hThread;
    APP_PIPE_COMBO_TYPE_E ePipeComboType;
    AX_U8 nFrameCount;
    AX_U8 nContinuousFrameCount;

    _RAW_DISPATCH_THREAD_PARAM() {
        nDevID = 0;
        fDevFramerate = 0;
        vecTargetPipeFramerate.clear();
        eHdrMode = AX_SNS_LINEAR_MODE;
        bThreadRunning = AX_FALSE;
        ePipeComboType = APP_PIPE_COMBO_TYPE_NORMAL;
        nFrameCount = 0;
        nContinuousFrameCount = 0;
    }
} RAW_DISPATCH_THREAD_PARAM_T, *RAW_DISPATCH_THREAD_PARAM_PTR;

typedef struct _SNAP_DISP_THREAD_PARAM {
    AX_U8 nDevID;
    AX_F32 fDevFramerate;
    vector<PIPE_FRAMERATE_INFO_T> vecTargetPipeFramerate;
    AX_SNS_HDR_MODE_E eHdrMode;
    AX_BOOL bThreadRunning;
    std::thread hThread;

    _SNAP_DISP_THREAD_PARAM() {
        nDevID = 0;
        fDevFramerate = 0;
        vecTargetPipeFramerate.clear();
        eHdrMode = AX_SNS_LINEAR_MODE;
        bThreadRunning = AX_FALSE;
    }
} SNAP_DISP_THREAD_PARAM_T, *SNAP_DISP_THREAD_PARAM_PTR;

typedef struct _YUV_THREAD_PARAM {
    AX_U8 nSnsID;
    AX_U8 nPipeID;
    AX_U8 nIspChn;
    AX_BOOL bSnapshot;
    AX_BOOL bMultiplex;
    AX_BOOL bThreadRunning;
    std::thread hThread;
    AX_BOOL bPaused;
    AX_BOOL bDiscard;
    AX_BOOL bEnable;

    _YUV_THREAD_PARAM() {
        nPipeID = 0;
        nIspChn = 0;
        bSnapshot = AX_FALSE;
        bMultiplex = AX_FALSE;
        bThreadRunning = AX_FALSE;
        bPaused = AX_FALSE;
        bDiscard = AX_FALSE;
        bEnable = AX_TRUE;
    }
} YUV_THREAD_PARAM_T, *YUV_THREAD_PARAM_PTR;

typedef struct _SNAP_PROCESS_PARAM {
    AX_U8 nSnsID{0};
    AX_U8 nDevID{0};
    AX_U8 nPipeID{0};
    AX_VIN_PIPE_WORK_MODE_E ePipeWorkMode[AX_VIN_MAX_PIPE_NUM];
    AX_SNS_HDR_MODE_E eHdrMode{AX_SNS_LINEAR_MODE};
    AX_BOOL bDummy{AX_FALSE};
} SNAP_PROCESS_PARAM;

typedef struct _IFE_DUMP_RAW_PROCESS_PARAM {
    AX_U8 nSnsID{0};
    AX_U8 nPipeID{0};
    AX_U8 nSnapPipeId{INVALID_PIPE_ID};
    AX_SNS_HDR_MODE_E eHdrMode{AX_SNS_LINEAR_MODE};
} IFE_DUMP_RAW_PROCESS_PARAM;

class CSensorMgr : public IModule, public IFrameRelease {
public:
    CSensorMgr() = default;
    virtual ~CSensorMgr() = default;

public:
    virtual AX_BOOL Init() override;
    virtual AX_BOOL DeInit() override;
    virtual AX_BOOL Start() override;
    virtual AX_BOOL Stop() override;
    virtual AX_VOID VideoFrameRelease(CAXFrame* pFrame) override;

    AX_BOOL Start(CBaseSensor* pSensor);
    AX_BOOL Stop(CBaseSensor* pSensor);
    CBaseSensor* GetSnsInstance(AX_U32 nIndex);
    AX_U32 GetSensorCount() {
        return m_vecSensorIns.size();
    }

    AX_S8 PipeFromSns(AX_U8 nPipeID);

    AX_VOID SetLoadType(AX_U32 nLoadType) {
        m_nLoadType = nLoadType;
    }

    AX_VOID SetYuvThreadParams(AX_U32 nSnsID, AX_U32 nPipeID, AX_U32 nChannel, AX_BOOL bSnapshot,
                               AX_BOOL bMultiplex, AX_BOOL bDiscard = AX_FALSE, AX_BOOL bEnable = AX_TRUE);
    AX_BOOL RestartWithScenario(AX_S32 nScenario);

    AX_VOID RegObserver(AX_U32 nPipeID, AX_U32 nChannel, IObserver* pObserver);
    AX_VOID UnregObserver(AX_U32 nPipeID, AX_U32 nChannel, IObserver* pObserver);

    /*
        @brief: callback function intended to update sensor attributes, which will be called after initialization done and before opening.
    */
    static AX_BOOL UpdateAttrCB(ISensor* pInstance);
    AX_VOID SwitchSnsMode(AX_U32 nSnsID, AX_U32 nSnsMode, AX_S32 nFps = -1);
    AX_VOID ChangeSnsFps(AX_U32 nSnsID, AX_F32 fFrameRate);

    AX_BOOL EnableEIS(AX_U32 nSnsID, AX_BOOL bEnable);

    AX_BOOL SwitchIFERawDumpPipe(AX_U32 nSnsID, AX_U8 nPipe);

    AX_F32 GetPipeFrameRate(AX_U8 nPipeID);

    AX_BOOL ChangeShutterType(AX_U32 nSnsID, AX_U8 nShutterType);
    AX_BOOL EnableChn(AX_U8 nPipe, AX_U8 nChannel, AX_BOOL bEnable);

protected:
    AX_VOID RawDispatchThreadFunc(RAW_DISPATCH_THREAD_PARAM_T* pThreadParam);
    AX_VOID SnapDispatchThreadFunc(SNAP_DISP_THREAD_PARAM_T* pThreadParam);
    AX_VOID YuvGetThreadFunc(YUV_THREAD_PARAM_T* pThreadParam);

private:
    AX_VOID StartDispatchRawThread();
    AX_VOID StartSnapThread();
    AX_VOID StopDispatchRawThread();
    AX_VOID StopSnapThread();
    AX_VOID StartYuvGetThread();
    AX_VOID StopYuvGetThread();
    AX_VOID StartNtCtrl();
    AX_VOID StopNtCtrl();
    AX_VOID StartIFEDumpRawThread();
    AX_VOID StopIFEDumpRawThread();
    AX_VOID IFERawDumpThreadFunc(AX_VOID* pArg);
    AX_VOID StartDispatchIFEDumpRawThread();
    AX_VOID StopDispatchIFEDumpRawThread();
    AX_VOID IFEDumpRawQDispatchFunc(AX_VOID* pArg);
    AX_BOOL SnapshotProcess(AX_U8 nPipe, AX_U8 nChannel, AX_SNS_HDR_MODE_E eHdrMode, const AX_IMG_INFO_T** pArrImgInfo, AX_BOOL bDummy);
    AX_BOOL SnapshotProcess_Offline_NPU(AX_U8 nDevId, AX_U8 nPipe, AX_U8 nChannel, AX_SNS_HDR_MODE_E eHdrMode, const AX_IMG_INFO_T** pArrImgInfo, AX_BOOL bDummy, AX_U8 nIFEDumpIndex);
    AX_BOOL NotifySnapshotProcess(AX_U8 nPipe, AX_U8 nChannel);
    AX_VOID SnapProcess(AX_VOID* pArg);
    AX_VOID ClearQueue(AX_U32 nGrp);
    AX_VOID ClearIFEDumpRawFrmQueue(AX_U8 nIndex);

    AX_VOID NotifyAll(AX_S32 nPipe, AX_U32 nChannel, AX_VOID* pFrame);

    AX_U8   GetDumpIFERawParamIndex(AX_U8 nSnsID);

private:
    /*
        @brief: Load sensor configurations with specified types.
    */
    AX_U32 m_nLoadType{0};
    vector<CBaseSensor*> m_vecSensorIns;
    std::map<AX_U32, std::map<AX_U32, std::vector<IObserver*>>> m_mapObservers;
    std::map<AX_U8, RAW_DISPATCH_THREAD_PARAM_T> m_mapDev2ThreadParam;
    std::map<AX_U32, std::map<AX_U32, YUV_THREAD_PARAM_T>> m_mapYuvThreadParams;
    std::map<AX_U32, std::map<AX_U32, AX_BOOL>> m_mapChnLinkable;

    std::map<AX_U8, std::map<AX_U8, std::array<AX_VIN_SHUTTER_MODE_E, AX_SHUTTER_SEQ_NUM>>> m_mapShutterMode;

    list<CAXFrame*> m_qFrame[MAX_SENSOR_COUNT * MAX_PIPE_PER_DEVICE][AX_VIN_CHN_ID_MAX];
    mutex m_mtxFrame[MAX_SENSOR_COUNT * MAX_PIPE_PER_DEVICE][AX_VIN_CHN_ID_MAX];
    AX_BOOL m_bGetYuvFlag[MAX_SENSOR_COUNT]{AX_TRUE, AX_TRUE};

    CAXLockQ<AX_IMG_INFO_T*>* m_arrFrameQ{nullptr};
    vector<CAXLockQ<AX_IMG_INFO_T*>*> m_VecDumpRawFrameQ;
    CAXThread m_SnapThread[MAX_SENSOR_COUNT];
    CAXThread m_IFERawDumpThread[MAX_SENSOR_COUNT];
    CAXThread m_IFEDumpRawDispatchThread[MAX_SENSOR_COUNT];
    IFE_DUMP_RAW_PROCESS_PARAM m_tDumpRawParam[MAX_SENSOR_COUNT];
    TRIGGER_INFO_T m_SSTriggerInfoArr[MAX_INFO_SIZE];
    AX_U8 m_SSInfoIndex{0};
    TRIGGER_INFO_T m_TSTriggerInfoArr[MAX_INFO_SIZE];
    AX_U8 m_TSInfoIndex{0};
    std::mutex m_mtxDumpRawSwitch;
    SNAP_PROCESS_PARAM m_tSnap[MAX_SENSOR_COUNT];
};

}  // namespace AX_ITS
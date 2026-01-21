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
#include "BaseLinkage.h"
#include "BaseSensor.h"
#include "IModule.h"
#include "IObserver.h"
#include "Avs.h"

namespace AX_IPC {

#define MAX_SENSOR_COUNT (2)
#define MAX_PIPE_PER_DEV (3)
#define AF_TEST_SENSOR_COUNT (4)

typedef struct _PIPE_FRAMERATE_INFO_T {
    AX_U8 nPipeID;
    AX_F32 fPipeFramerate;

    _PIPE_FRAMERATE_INFO_T(AX_U8 nPipe, AX_F32 fFramerate) {
        nPipeID = nPipe;
        fPipeFramerate = fFramerate;
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

    _RAW_DISPATCH_THREAD_PARAM() {
        nDevID = 0;
        fDevFramerate = 0;
        vecTargetPipeFramerate.clear();
        eHdrMode = AX_SNS_LINEAR_MODE;
        bThreadRunning = AX_FALSE;
    }
} RAW_DISPATCH_THREAD_PARAM_T, *RAW_DISPATCH_THREAD_PARAM_PTR;

typedef struct _YUV_THREAD_PARAM {
    AX_U8 nSnsID;
    AX_U8 nPipeID;
    AX_U8 nIspChn;
    AX_BOOL bMultiplex;
    AX_BOOL bThreadRunning;
    std::thread hThread;

    _YUV_THREAD_PARAM() {
        nPipeID = 0;
        nIspChn = 0;
        bMultiplex = AX_FALSE;
        bThreadRunning = AX_FALSE;
    }
} YUV_THREAD_PARAM_T, *YUV_THREAD_PARAM_PTR;

typedef struct _IEF_AF_TEST_THREAD_PARAM {
    AX_U8 nSnsID;
    AX_U8 nPipeID;
    AX_BOOL bThreadRunning;
    std::thread hThread;

    _IEF_AF_TEST_THREAD_PARAM() {
        nSnsID = 0;
        nPipeID = 0;
        bThreadRunning = AX_FALSE;
    }
} IEF_AF_TEST_THREAD_PARAM_T, *IEF_AF_TEST_THREAD_PARAM_PTR;

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

    AX_BOOL SetVinStitchAttr(std::string strStitchMaskPath, AX_BOOL bAroundView, AX_BOOL bPrioOnsiteCali, const std::map<AX_U8, AX_U8>& mapStitchPipe);

    CBaseSensor* GetSnsInstance(AX_U32 nIndex);
    AX_U32 GetSensorCount() {
        return m_vecSensorIns.size();
    }

    AX_S8 PipeFromSns(AX_U8 nPipeID);

    AX_VOID SetLoadType(AX_U32 nLoadType) {
        m_nLoadType = nLoadType;
    }

    AX_VOID SetYuvThreadParams(AX_U32 nSnsID, AX_U32 nPipeID, AX_U32 nChannel, AX_BOOL bMultiplex);
    AX_BOOL RestartWithScenario(AX_S32 nScenario);

    AX_VOID RegObserver(AX_U32 nPipeID, AX_U32 nChannel, IObserver* pObserver);
    AX_VOID UnregObserver(AX_U32 nPipeID, AX_U32 nChannel, IObserver* pObserver);

    AX_BOOL ChangeDaynightMode(AX_U32 nSnsID, AX_DAYNIGHT_MODE_E eDaynightMode);

    /*
        @brief: callback function intended to update sensor attributes, which will be called after initialization done and before opening.
    */
    static AX_BOOL UpdateAttrCB(ISensor* pInstance);
    AX_VOID SwitchSnsMode(AX_U32 nSnsID, AX_U32 nSnsMode, AX_S32 nFps = -1);
    AX_VOID ChangeSnsFps(AX_U32 nSnsID, AX_F32 fFrameRate);
    AX_BOOL ChangeSnsMirrorFlip(AX_U32 nSnsID, AX_BOOL bMirror, AX_BOOL bFlip);

    AX_VOID SetAeAwbSyncRatio(const std::map<AX_U8, AX_AVS_AEAWB_SYNC_RATIO_T>& mapAeAwbSyncRatio);
    AX_S32 EnableAeAwbSyncRatio(AX_BOOL bEnable);

    AX_BOOL EnableEIS(AX_U32 nSnsID, AX_BOOL bEnable);

    AX_BOOL EZoom(AX_U32 nSnsID, AX_S32 nEZoomRatio, AX_S32 nCenterOffsetX=0, AX_S32 nCenterOffsetY=0);

    AX_BOOL UpdateLDC(AX_U32 nSnsID, AX_BOOL bLdcEnable, AX_BOOL bAspect, AX_S16 nXRatio, AX_S16 nYRatio, AX_S16 nXYRatio,
                      AX_S16 nDistorRatio);

    AX_BOOL SetChnCompress(AX_U32 nSnsID, AX_U8 nPipe, AX_U8 nChannel, AX_BOOL bCompressOn);

    AX_BOOL IsRotateEngine(AX_U32 nSnsID);
    AX_BOOL IsMirrorFlipEngine(AX_U32 nSnsID);

    AX_BOOL UpdateRotation(AX_U32 nSnsID, AX_VIN_ROTATION_E eRotation);

    AX_VIN_ROTATION_E GetRotation(AX_U32 nSnsID, AX_BOOL bIgnorePipe = AX_TRUE, AX_U8 nPipe = 0);
    AX_BOOL IsMirror(AX_U32 nSnsID, AX_BOOL bIgnorePipe = AX_TRUE, AX_U8 nPipe = 0);
    AX_BOOL IsFlip(AX_U32 nSnsID, AX_BOOL bIgnorePipe = AX_TRUE, AX_U8 nPipe = 0);

protected:
    AX_VOID RawDispatchThreadFunc(RAW_DISPATCH_THREAD_PARAM_T* pThreadParam);
    AX_VOID YuvGetThreadFunc(YUV_THREAD_PARAM_T* pThreadParam);
    AX_VOID IfeAfTestThreadFunc(IEF_AF_TEST_THREAD_PARAM_T* pThreadParam);

private:
    AX_VOID StartDispatchRawThread();
    AX_VOID StopDispatchRawThread();
    AX_VOID StartYuvGetThread();
    AX_VOID StopYuvGetThread();
    AX_VOID StartNtCtrl();
    AX_VOID StopNtCtrl();
    AX_VOID StartIfeAfTestThread();
    AX_VOID StopIfeAfTestThread();

    AX_BOOL Start(CBaseSensor* pSensor);
    AX_BOOL Stop(CBaseSensor* pSensor);
    AX_VOID StartNtCtrl(CBaseSensor* pSensor);
    AX_VOID StopNtCtrl(CBaseSensor* pSensor);

    AX_VOID NotifyAll(AX_S32 nPipe, AX_U32 nChannel, AX_VOID* pFrame);
    AX_BOOL OnDataChange(AX_U32 nPipeID, AX_U32 nChannel, OBS_CHANGE_DATA_TYPE_E eDataType, AX_VOID* pData);

    AX_S32 EnableAeAwbSyncRatio(AX_U8 nPipeId, AX_BOOL bEnable);

    AX_BOOL IsAllowDisableAf();

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

    list<CAXFrame*> m_qFrame[MAX_SENSOR_COUNT * MAX_PIPE_PER_DEVICE][AX_VIN_CHN_ID_MAX];
    mutex m_mtxFrame[MAX_SENSOR_COUNT * MAX_PIPE_PER_DEVICE][AX_VIN_CHN_ID_MAX];
    AX_BOOL m_bGetYuvFlag[MAX_SENSOR_COUNT]{AX_TRUE, AX_TRUE};

    AX_BOOL m_bSetAeAwbSyncRatio{AX_FALSE};
    std::map<AX_U8, AX_AVS_AEAWB_SYNC_RATIO_T> m_mapAeAwbSyncRatio;
    std::map<AX_U8, IEF_AF_TEST_THREAD_PARAM_T> m_mapIfeAfTestThreadParams;
    AX_BOOL m_arrAfEnable[AF_TEST_SENSOR_COUNT]{AX_FALSE};
    std::mutex m_mtxAf;
    AX_BOOL m_bPrioOnsiteCali{AX_FALSE};
};

}  // namespace AX_IPC
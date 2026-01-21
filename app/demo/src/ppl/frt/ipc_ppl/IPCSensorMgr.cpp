/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include "IPCSensorMgr.h"
#include <math.h>
#include <sys/prctl.h>
#include "AppLogApi.h"
#include "ElapsedTimer.hpp"
#include "FramerateCtrlHelper.h"
#include "GlobalDef.h"
#include "OptionHelper.h"
#include "SensorFactory.hpp"
#include "SensorOptionHelper.h"
#include "ax_nt_ctrl_api.h"
#include "ax_nt_stream_api.h"
#include "CommonUtils.hpp"

#define SNS_MGR "SNS_MGR"
#define APP_VIN_STITCH_GRP (0)

using namespace AX_IPC;

///////////////////////////////////////////////////////////////////////////////////////////
AX_BOOL CSensorMgr::Init() {
    AX_U32 nSensorCount = APP_SENSOR_COUNT();
    if (3 == nSensorCount) {
        m_bSetAeAwbSyncRatio = AX_TRUE;
    }

    AX_BOOL bIfeAfTest = (COptionHelper::GetInstance()->IsEnableAFTest() && (AF_TEST_SENSOR_COUNT == nSensorCount)) ? AX_TRUE : AX_FALSE;
    for (AX_U32 i = 0; i < nSensorCount; i++) {
        SENSOR_CONFIG_T tSensorCfg;
        if (!APP_SENSOR_CONFIG(i, tSensorCfg)) {
            LOG_M_E(SNS_MGR, "Failed to get sensor config %d", i);
            return AX_FALSE;
        }

        CBaseSensor* pSensor = (CBaseSensor*)(CSensorFactory::GetInstance()->CreateSensor(tSensorCfg));
        if (nullptr == pSensor) {
            LOG_M_E(SNS_MGR, "Failed to create sensor instance %d", i);
            return AX_FALSE;
        } else {
            LOG_M(SNS_MGR, "Create sensor instance %d ok.", i);
        }

        pSensor->RegAttrUpdCallback(UpdateAttrCB);

        if (!pSensor->Init()) {
            LOG_M_E(SNS_MGR, "Failed to initial sensor instance %d", i);
            return AX_FALSE;
        } else {
            LOG_M(SNS_MGR, "Initailize sensor %d ok.", i);
        }

        // Reset chn stride align
        AX_U8 nPipeCnt = pSensor->GetPipeCount();
        for (AX_U8 j = 0; j < nPipeCnt; j++) {
            for (AX_U8 k = 0; k < AX_VIN_CHN_ID_MAX; k++) {
                PIPE_CONFIG_T tPipeAttr = pSensor->GetSnsConfig().arrPipeAttr[j];
                AX_VIN_CHN_ATTR_T stVinChnAttr = pSensor->GetChnAttr(tPipeAttr.nPipeID, k);
                if (stVinChnAttr.nWidth > 0 && stVinChnAttr.nHeight > 0) {
                    AX_FRAME_COMPRESS_INFO_T stFrCompInfo = tPipeAttr.arrChannelAttr[k].tChnCompressInfo;
                    AX_U32 nAlign = 2;
                    if (stFrCompInfo.enCompressMode != AX_COMPRESS_MODE_NONE) {
                        nAlign = IS_PANO_SENSOR(tSensorCfg.eSensorType) ? AX_VIN_FBC_WIDTH_ALIGN_VAL : AX_VIN_IPC_FBC_WIDTH_ALIGN_VAL;
                    }
                    stVinChnAttr.nWidthStride = ALIGN_UP(stVinChnAttr.nWidth, nAlign);
                    pSensor->SetChnAttr(tPipeAttr.nPipeID, k, stVinChnAttr);
                }
            }
        }

        m_vecSensorIns.emplace_back(pSensor);

        if (bIfeAfTest) {
            m_mapIfeAfTestThreadParams[i].nSnsID = tSensorCfg.nSnsID;
            m_mapIfeAfTestThreadParams[i].nPipeID = tSensorCfg.arrPipeAttr[0].nPipeID;
            m_arrAfEnable[tSensorCfg.nSnsID] = AX_TRUE;  // AF0 enabled by default
        }
    }

    WEB_SHOW_SENSOR_MODE_E eWebSnsShowMode = E_WEB_SHOW_SENSOR_MODE_SINGLE;
    switch (nSensorCount) {
        case 1:
            eWebSnsShowMode = E_WEB_SHOW_SENSOR_MODE_SINGLE;
            break;
        case 2:
            eWebSnsShowMode = E_WEB_SHOW_SENSOR_MODE_DUAL;
            break;
        case 3:
            eWebSnsShowMode = E_WEB_SHOW_SENSOR_MODE_PANO_DUAL;
            break;
        case 4:
            eWebSnsShowMode =  E_WEB_SHOW_SENSOR_MODE_QUAD;
            break;
        case 6:
            eWebSnsShowMode =  E_WEB_SHOW_SENSOR_MODE_SIX;
            break;
        case 8:
            eWebSnsShowMode =  E_WEB_SHOW_SENSOR_MODE_EIGHT;
            break;
        default:
            LOG_MM_W(SNS_MGR, "Do not support sns count(%d) for web currently.", nSensorCount);
            break;
    }

    SET_APP_WEB_SHOW_SENSOR_MODE(eWebSnsShowMode);

    AX_S32 nPanoSnsId = (3 == nSensorCount) ? 1 : -1;
    SET_APP_WEB_PANO_SENSOR_ID(nPanoSnsId);

    return AX_TRUE;
}

AX_BOOL CSensorMgr::DeInit() {
    for (ISensor* pSensor : m_vecSensorIns) {
        CSensorFactory::GetInstance()->DestorySensor(pSensor);
    }

    return AX_TRUE;
}

AX_BOOL CSensorMgr::SetVinStitchAttr(std::string strStitchMaskPath, AX_BOOL bAroundView, AX_BOOL bPrioOnsiteCali, const std::map<AX_U8, AX_U8>& mapStitchPipe) {
    if (strStitchMaskPath.empty()) {
        LOG_MM_E(SNS_MGR, "stitch mask path is null!");
        return AX_FALSE;
    }

    if (mapStitchPipe.size() < 2) {
        LOG_MM_E(SNS_MGR, "Invalid mapStitchPipe for size(%d) less than 2!", mapStitchPipe.size());
        return AX_FALSE;
    }

    m_bPrioOnsiteCali = bPrioOnsiteCali;

    AX_S32 nRet = AX_SUCCESS;
    AX_VIN_STITCH_GRP_ATTR_T tVinStitchAttr{0};

    tVinStitchAttr.bStitchEn = 1;
    tVinStitchAttr.bAroundView = bAroundView ? 1 : 0;
    tVinStitchAttr.nMainPipeId = 0;

    tVinStitchAttr.nStitchPipeNum = mapStitchPipe.size();
    LOG_MM_I(SNS_MGR, "nStitchPipeNum: %d", tVinStitchAttr.nStitchPipeNum);

    AX_AVSCALI_STITCH_OVERLAP_INFO_T stStitchOverlapInfo;
    stStitchOverlapInfo.stPipeSeqInfo.nPipeNum = tVinStitchAttr.nStitchPipeNum;
    for (auto pipe2Index : mapStitchPipe) {
        stStitchOverlapInfo.stPipeSeqInfo.arrPipeSeq[pipe2Index.second] = pipe2Index.first;
    }
    nRet = AX_AVSCALI_LoadStitchOverlapInfo(strStitchMaskPath.c_str(), &stStitchOverlapInfo);
    if (nRet != AX_AVSCALI_SUCC) {
        LOG_MM_E(SNS_MGR, "AX_AVSCALI_LoadStitchOverlapInfo failed, nRet: 0x%x", nRet);
        return AX_FALSE;
    }

    SENSOR_CONFIG_T tSnsCfg;
    AX_U8 nMaxPipeSchedPriority = 0;
    AX_U32 nSnsCount = APP_SENSOR_COUNT();
    for (AX_U32 i = 0; i < nSnsCount; i++) {
        if (!APP_SENSOR_CONFIG(i, tSnsCfg)) {
            LOG_MM_E(SNS_MGR, "Failed to get sensor config %d", i);
            return AX_FALSE;
        }

        // only one pipe for each sensor
        AX_U8 nPipeId = tSnsCfg.arrPipeAttr[0].nPipeID;
        auto it = mapStitchPipe.find(nPipeId);
        if (it == mapStitchPipe.end()) {
            continue;
        }

        AX_U8 nIndex = it->second;

        if (tSnsCfg.arrPipeAttr[0].tPipeSchedAttr.nPriority > nMaxPipeSchedPriority) {
            tVinStitchAttr.nMainPipeId = nPipeId;
            nMaxPipeSchedPriority = tSnsCfg.arrPipeAttr[0].tPipeSchedAttr.nPriority;
        }

        tVinStitchAttr.tStitchPipeAttr[nIndex].nPipeId = nPipeId;
        tVinStitchAttr.tStitchPipeAttr[nIndex].bMirrorFlip = tSnsCfg.bStitchMirrorFlip ? 1 : 0;
        for (AX_U8 j = 0; j < AX_VIN_OVERLAP_NUM; j++) {
            tVinStitchAttr.tStitchPipeAttr[nIndex].tOverlapInfo[j].tOverlapArea.nStartX = stStitchOverlapInfo.stIspOverlapData[nIndex].stIspOverlapInfo[j].tOverlapArea.nStartX;
            tVinStitchAttr.tStitchPipeAttr[nIndex].tOverlapInfo[j].tOverlapArea.nStartY = stStitchOverlapInfo.stIspOverlapData[nIndex].stIspOverlapInfo[j].tOverlapArea.nStartY;
            tVinStitchAttr.tStitchPipeAttr[nIndex].tOverlapInfo[j].tOverlapArea.nWidth = stStitchOverlapInfo.stIspOverlapData[nIndex].stIspOverlapInfo[j].tOverlapArea.nWidth;
            tVinStitchAttr.tStitchPipeAttr[nIndex].tOverlapInfo[j].tOverlapArea.nHeight = stStitchOverlapInfo.stIspOverlapData[nIndex].stIspOverlapInfo[j].tOverlapArea.nHeight;
            tVinStitchAttr.tStitchPipeAttr[nIndex].tOverlapInfo[j].pOverlapMask = stStitchOverlapInfo.stIspOverlapData[nIndex].stIspOverlapInfo[j].pOverlapMask;
        }

        LOG_MM_I(SNS_MGR, "sns[%d] nPipeId: %d, pipeIndex: %d", i, tVinStitchAttr.tStitchPipeAttr[nIndex].nPipeId, nIndex);
    }

    LOG_MM_I(SNS_MGR, "bStitchEn: %d, bAroundView: %d, nMainPipeId: %d, nStitchPipeNum: %d",
                       tVinStitchAttr.bStitchEn, tVinStitchAttr.bAroundView,
                       tVinStitchAttr.nMainPipeId, tVinStitchAttr.nStitchPipeNum);

    nRet = AX_VIN_SetStitchGrpAttr(APP_VIN_STITCH_GRP, &tVinStitchAttr);
    if (AX_SUCCESS != nRet) {
        LOG_MM_E(SNS_MGR, "AX_VIN_SetStitchGrpAttr fail, ret:0x%x", nRet);
    }

    return AX_TRUE;
}

AX_BOOL CSensorMgr::Start() {
    for (auto pSensor : m_vecSensorIns) {
        if (!pSensor->Open()) {
            return AX_FALSE;
        }
    }

    if (m_bSetAeAwbSyncRatio) {
        if (0 != EnableAeAwbSyncRatio(1, AX_TRUE)) {
            return AX_FALSE;
        }
    }

    StartNtCtrl();

#ifdef ISP_NOT_SET_IFE_AS_DEV
    StartYuvGetThread();
    StartDispatchRawThread();
#endif

    StartIfeAfTestThread();

    return AX_TRUE;
}

AX_BOOL CSensorMgr::Stop() {
    StopIfeAfTestThread();

    StopNtCtrl();

#ifdef ISP_NOT_SET_IFE_AS_DEV
    StopYuvGetThread();
    StopDispatchRawThread();
#endif

    for (auto pSensor : m_vecSensorIns) {
        if (!pSensor->Close()) {
            return AX_FALSE;
        }
    }

    return AX_TRUE;
}

AX_BOOL CSensorMgr::Start(CBaseSensor* pSensor) {
    if (!pSensor->Open()) {
        return AX_FALSE;
    }

    return AX_TRUE;
}

AX_BOOL CSensorMgr::Stop(CBaseSensor* pSensor) {
    if (!pSensor->Close()) {
        return AX_FALSE;
    }

    return AX_TRUE;
}

AX_VOID CSensorMgr::StartDispatchRawThread() {
    for (auto pSensor : m_vecSensorIns) {
        SENSOR_CONFIG_T tSnsCfg = pSensor->GetSnsConfig();
        m_mapDev2ThreadParam[tSnsCfg.nDevID].nSnsID = tSnsCfg.nSnsID;
        m_mapDev2ThreadParam[tSnsCfg.nDevID].nDevID = tSnsCfg.nDevID;
        m_mapDev2ThreadParam[tSnsCfg.nDevID].eHdrMode = tSnsCfg.eSensorMode;
        m_mapDev2ThreadParam[tSnsCfg.nDevID].fDevFramerate = tSnsCfg.fFrameRate;
        for (AX_U8 i = 0; i < tSnsCfg.nPipeCount; i++) {
            m_mapDev2ThreadParam[tSnsCfg.nDevID].vecTargetPipeFramerate.emplace_back(tSnsCfg.arrPipeAttr[i].nPipeID,
                                                                                    tSnsCfg.arrPipeAttr[i].fPipeFramerate);
        }
    }

    for (auto& m : m_mapDev2ThreadParam) {
        RAW_DISPATCH_THREAD_PARAM_T& tParams = m.second;
        tParams.hThread = std::thread(&CSensorMgr::RawDispatchThreadFunc, this, &tParams);
    }
}

AX_VOID CSensorMgr::StopDispatchRawThread() {
    for (auto& mapDev2Param : m_mapDev2ThreadParam) {
        RAW_DISPATCH_THREAD_PARAM_T& tParams = mapDev2Param.second;
        if (tParams.hThread.joinable()) {
            tParams.bThreadRunning = AX_FALSE;
        }
    }

    for (auto& mapDev2Param : m_mapDev2ThreadParam) {
        RAW_DISPATCH_THREAD_PARAM_T& tParams = mapDev2Param.second;
        if (tParams.hThread.joinable()) {
            tParams.hThread.join();
        }
    }
}

AX_VOID CSensorMgr::StartYuvGetThread() {
    for (auto& mapChn2Param : m_mapYuvThreadParams) {
        for (auto& param : mapChn2Param.second) {
            YUV_THREAD_PARAM_T& tParams = param.second;
            tParams.hThread = std::thread(&CSensorMgr::YuvGetThreadFunc, this, &tParams);
        }
    }
}

AX_VOID CSensorMgr::StopYuvGetThread() {
    for (auto& mapChn2Param : m_mapYuvThreadParams) {
        for (auto& param : mapChn2Param.second) {
            YUV_THREAD_PARAM_T& tParams = param.second;
            if (tParams.hThread.joinable()) {
                tParams.bThreadRunning = AX_FALSE;
            }
        }
    }

    for (auto& mapChn2Param : m_mapYuvThreadParams) {
        for (auto& param : mapChn2Param.second) {
            YUV_THREAD_PARAM_T& tParams = param.second;
            if (tParams.hThread.joinable()) {
                tParams.hThread.join();
            }
        }
    }
}

AX_VOID CSensorMgr::StartNtCtrl(CBaseSensor* pSensor) {
    AX_S32 nRet = 0;
    AX_BOOL bInit = AX_FALSE;
    do {
        SENSOR_CONFIG_T tSnsCfg = pSensor->GetSnsConfig();
        /* TODO: NT only needs to be initialized once, but in this case, multiple cameras lead to repeated initialization, which needs optimization. */
        for (AX_U8 i = 0; i < pSensor->GetPipeCount(); i++) {
            if (tSnsCfg.arrPipeAttr[i].bTuning) {
                nRet = AX_NT_StreamInit(6000);
                if (0 != nRet) {
                    LOG_MM_E(SNS_MGR, "AX_NT_StreamInit failed, ret=0x%x.", nRet);
                    break;
                }

                nRet = AX_NT_CtrlInit(tSnsCfg.arrPipeAttr[i].nTuningPort);
                if (0 != nRet) {
                    LOG_MM_E(SNS_MGR, "AX_NT_CtrlInit failed, ret=0x%x.", nRet);
                    break;
                } else {
                    LOG_MM(SNS_MGR, "Enable tunning on port: %d", tSnsCfg.arrPipeAttr[i].nTuningPort);
                }

                bInit = AX_TRUE;
                break;
            }
        }

        if (!bInit) break;

        for (AX_U8 i = 0; i < pSensor->GetPipeCount(); i++) {
            AX_U8 nPipeID = tSnsCfg.arrPipeAttr[i].nPipeID;
            if (tSnsCfg.arrPipeAttr[i].bTuning) {
                for (AX_U8 j = 0; j < AX_VIN_CHN_ID_MAX; j++) {
                    if (tSnsCfg.arrPipeAttr[i].arrChannelAttr[j].tChnCompressInfo.enCompressMode != 0) {
                        LOG_M_W(SNS_MGR, "Pipe %d Channel %d is in compress mode, does not support nt streaming.", nPipeID, j);
                    }
                }

                AX_NT_SetStreamSource(nPipeID);
            }
        }
    } while (0);
}

AX_VOID CSensorMgr::StartNtCtrl() {
    AX_S32 nRet = 0;
    AX_BOOL bInit = AX_FALSE;
    for (auto pSensor : m_vecSensorIns) {
        SENSOR_CONFIG_T tSnsCfg = pSensor->GetSnsConfig();
        for (AX_U8 i = 0; i < pSensor->GetPipeCount(); i++) {
            if (tSnsCfg.arrPipeAttr[i].bTuning) {
                nRet = AX_NT_StreamInit(6000);
                if (0 != nRet) {
                    LOG_MM_E(SNS_MGR, "AX_NT_StreamInit failed, ret=0x%x.", nRet);
                    return;
                }

                nRet = AX_NT_CtrlInit(tSnsCfg.arrPipeAttr[i].nTuningPort);
                if (0 != nRet) {
                    LOG_MM_E(SNS_MGR, "AX_NT_CtrlInit failed, ret=0x%x.", nRet);
                    return;
                } else {
                    LOG_MM(SNS_MGR, "Enable tunning on port: %d", tSnsCfg.arrPipeAttr[i].nTuningPort);
                }

                bInit = AX_TRUE;
                break;
            }
        }
        if (bInit) break;
    }

    for (auto pSensor : m_vecSensorIns) {
        SENSOR_CONFIG_T tSnsCfg = pSensor->GetSnsConfig();

        for (AX_U8 i = 0; i < pSensor->GetPipeCount(); i++) {
            AX_U8 nPipeID = tSnsCfg.arrPipeAttr[i].nPipeID;
            if (tSnsCfg.arrPipeAttr[i].bTuning) {
                for (AX_U8 j = 0; j < AX_VIN_CHN_ID_MAX; j++) {
                    if (tSnsCfg.arrPipeAttr[i].arrChannelAttr[j].tChnCompressInfo.enCompressMode != 0) {
                        LOG_M_W(SNS_MGR, "Pipe %d Channel %d is in compress mode, does not support nt streaming.", nPipeID, j);
                    }
                }

                AX_NT_SetStreamSource(nPipeID);
            }
        }
    }
}

AX_VOID CSensorMgr::StopNtCtrl(CBaseSensor* pSensor) {
    AX_S32 nRet = 0;
    AX_BOOL bDeInit = AX_FALSE;
    do {
        SENSOR_CONFIG_T tSnsCfg = pSensor->GetSnsConfig();

        for (AX_U8 i = 0; i < pSensor->GetPipeCount(); i++) {
            if (tSnsCfg.arrPipeAttr[i].bTuning) {
                nRet = AX_NT_CtrlDeInit();
                if (0 != nRet) {
                    LOG_MM_E(SNS_MGR, "AX_NT_CtrlDeInit failed, ret=0x%x.", nRet);
                    break;
                }

                nRet = AX_NT_StreamDeInit();
                if (0 != nRet) {
                    LOG_MM_E(SNS_MGR, "AX_NT_StreamDeInit failed, ret=0x%x.", nRet);
                    break;
                }

                bDeInit = AX_TRUE;
                break;
            }
        }

        if (bDeInit) break;
    } while(0);
}

AX_VOID CSensorMgr::StopNtCtrl() {
    AX_S32 nRet = 0;
    AX_BOOL bDeInit = AX_FALSE;
    for (auto pSensor : m_vecSensorIns) {
        SENSOR_CONFIG_T tSnsCfg = pSensor->GetSnsConfig();

        for (AX_U8 i = 0; i < pSensor->GetPipeCount(); i++) {
            if (tSnsCfg.arrPipeAttr[i].bTuning) {
                nRet = AX_NT_CtrlDeInit();
                if (0 != nRet) {
                    LOG_MM_E(SNS_MGR, "AX_NT_CtrlDeInit failed, ret=0x%x.", nRet);
                    return;
                }

                nRet = AX_NT_StreamDeInit();
                if (0 != nRet) {
                    LOG_MM_E(SNS_MGR, "AX_NT_StreamDeInit failed, ret=0x%x.", nRet);
                    return;
                }

                bDeInit = AX_TRUE;
                break;
            }
        }

        if (bDeInit) break;
    }
}

AX_VOID CSensorMgr::StartIfeAfTestThread() {
    for (auto& m : m_mapIfeAfTestThreadParams) {
        IEF_AF_TEST_THREAD_PARAM_T& tParams = m.second;
        tParams.hThread = std::thread(&CSensorMgr::IfeAfTestThreadFunc, this, &tParams);
    }
}

AX_VOID CSensorMgr::StopIfeAfTestThread() {
    for (auto& m : m_mapIfeAfTestThreadParams) {
        IEF_AF_TEST_THREAD_PARAM_T& tParams = m.second;
        if (tParams.hThread.joinable()) {
            tParams.bThreadRunning = AX_FALSE;
        }
    }

    for (auto& m : m_mapIfeAfTestThreadParams) {
        IEF_AF_TEST_THREAD_PARAM_T& tParams = m.second;
        if (tParams.hThread.joinable()) {
            tParams.hThread.join();
        }
    }
}

AX_VOID CSensorMgr::SetYuvThreadParams(AX_U32 nSnsID, AX_U32 nPipeID, AX_U32 nChannel, AX_BOOL bMultiplex) {
    if (nChannel >= AX_VIN_CHN_ID_MAX) {
        return;
    }
    m_mapYuvThreadParams[nPipeID][nChannel].nSnsID = nSnsID;
    m_mapYuvThreadParams[nPipeID][nChannel].nPipeID = nPipeID;
    m_mapYuvThreadParams[nPipeID][nChannel].nIspChn = nChannel;
    m_mapYuvThreadParams[nPipeID][nChannel].bMultiplex = bMultiplex;
    m_mapYuvThreadParams[nPipeID][nChannel].bThreadRunning = AX_FALSE;
}

AX_VOID CSensorMgr::RegObserver(AX_U32 nPipeID, AX_U32 nChannel, IObserver* pObserver) {
    if (nullptr != pObserver) {
        AX_S8 nSensorID = PipeFromSns(nPipeID);
        if (-1 == nSensorID) {
            LOG_MM_E(SNS_MGR, "Pipe %d does not configured in sensor.json", nPipeID);
            return;
        }

        AX_F32 fFrameRate = m_vecSensorIns[nSensorID]->GetSnsConfig().arrPipeAttr->fPipeFramerate;
        AX_VIN_CHN_ATTR_T tChnAttr = m_vecSensorIns[nSensorID]->GetChnAttr(nPipeID, nChannel);

        OBS_TRANS_ATTR_T tTransAttr;
        tTransAttr.nGroup = nPipeID;
        tTransAttr.nChannel = nChannel;
        tTransAttr.fFramerate = fFrameRate;
        tTransAttr.nWidth = tChnAttr.nWidth;
        tTransAttr.nHeight = tChnAttr.nHeight;
        tTransAttr.nCfgWidth = tChnAttr.nWidth;
        tTransAttr.nCfgHeight = tChnAttr.nHeight;
        tTransAttr.arrOutFBC[0] = (AX_U8)tChnAttr.tCompressInfo.enCompressMode;
        tTransAttr.arrOutFBC[1] = (AX_U8)tChnAttr.tCompressInfo.u32CompressLevel;
        tTransAttr.bLink = AX_FALSE;
        tTransAttr.nSnsSrc = nSensorID;

        if (pObserver->OnRegisterObserver(E_OBS_TARGET_TYPE_VIN, nPipeID, nChannel, &tTransAttr)) {
            m_mapObservers[nPipeID][nChannel].push_back(pObserver);
        }
    }
}

AX_VOID CSensorMgr::UnregObserver(AX_U32 nPipeID, AX_U32 nChannel, IObserver* pObserver) {
    if (nullptr == pObserver) {
        return;
    }

    for (vector<IObserver*>::iterator it = m_mapObservers[nPipeID][nChannel].begin(); it != m_mapObservers[nPipeID][nChannel].end(); it++) {
        if (*it == pObserver) {
            m_mapObservers[nPipeID][nChannel].erase(it);
            break;
        }
    }
}

AX_BOOL CSensorMgr::OnDataChange(AX_U32 nPipe, AX_U32 nChannel, OBS_CHANGE_DATA_TYPE_E eDataType, AX_VOID* pData) {
    if (nullptr == pData) {
        return AX_TRUE;
    }

    AX_BOOL bRet = AX_TRUE;
    for (vector<IObserver*>::iterator it = m_mapObservers[nPipe][nChannel].begin(); it != m_mapObservers[nPipe][nChannel].end(); it++) {
        if (!(*it)->OnDataChange(E_OBS_TARGET_TYPE_VIN, nPipe, nChannel, eDataType, pData)) {
            bRet = AX_FALSE;
        }
    }

    return bRet;
}

AX_VOID CSensorMgr::NotifyAll(AX_S32 nPipe, AX_U32 nChannel, AX_VOID* pFrame) {
    if (nullptr == pFrame) {
        return;
    }

    if (m_mapObservers[nPipe][nChannel].size() == 0) {
        ((CAXFrame*)pFrame)->FreeMem();
        return;
    }

    for (vector<IObserver*>::iterator it = m_mapObservers[nPipe][nChannel].begin(); it != m_mapObservers[nPipe][nChannel].end(); it++) {
        (*it)->OnRecvData(E_OBS_TARGET_TYPE_VIN, nPipe, nChannel, pFrame);
    }
}

AX_VOID CSensorMgr::VideoFrameRelease(CAXFrame* pAXFrame) {
    if (pAXFrame) {
        AX_U32 nPipe = pAXFrame->nGrp;
        AX_U32 nChn = pAXFrame->nChn;

        m_mtxFrame[nPipe][nChn].lock();
        for (list<CAXFrame*>::iterator it = m_qFrame[nPipe][nChn].begin(); it != m_qFrame[nPipe][nChn].end(); it++) {
            if ((*it)->nGrp == pAXFrame->nGrp && (*it)->nChn == pAXFrame->nChn &&
                (*it)->stFrame.stVFrame.stVFrame.u64SeqNum == pAXFrame->stFrame.stVFrame.stVFrame.u64SeqNum) {
                if (!pAXFrame->bMultiplex || (*it)->DecFrmRef() == 0) {
                    AX_VIN_ReleaseYuvFrame(nPipe, (AX_VIN_CHN_ID_E)nChn, (AX_IMG_INFO_T*)(*it)->pUserDefine);
                    LOG_MM_D(SNS_MGR, "[%d][%d] AX_VIN_ReleaseYuvFrame, seq:%lld, addr:%p", nPipe, nChn,
                             pAXFrame->stFrame.stVFrame.stVFrame.u64SeqNum, pAXFrame->pUserDefine);

                    AX_IMG_INFO_T* pIspImg = (AX_IMG_INFO_T*)(*it)->pUserDefine;

                    SAFE_DELETE_PTR(pIspImg);
                    SAFE_DELETE_PTR((*it));

                    m_qFrame[nPipe][nChn].erase(it);
                }

                break;
            }
        }
        m_mtxFrame[nPipe][nChn].unlock();
    }
}

AX_VOID CSensorMgr::RawDispatchThreadFunc(RAW_DISPATCH_THREAD_PARAM_T* pThreadParam) {
    AX_U8 nDevID = pThreadParam->nDevID;
    AX_F32 fDevFramerate = pThreadParam->fDevFramerate;
    AX_U32 nDevFramerate = fDevFramerate;
    AX_SNS_HDR_MODE_E eHdrMode = pThreadParam->eHdrMode;
    vector<PIPE_FRAMERATE_INFO_T>& vecTargetPipeFramerate = pThreadParam->vecTargetPipeFramerate;

    LOG_MM(SNS_MGR, "[%d] +++", nDevID);

    AX_CHAR szName[32] = {0};
    sprintf(szName, "RAW_DISP_%d", nDevID);
    prctl(PR_SET_NAME, szName);

    if (ceil(fDevFramerate) != (AX_S32)fDevFramerate) {
        nDevFramerate = fDevFramerate * 2;
        for (auto& m : vecTargetPipeFramerate) {
            m.fPipeFramerate = m.fPipeFramerate * 2;
        }
    }

    /* Calculate framerate control for each pipe */
    AX_F32 nDevFramerateRemain = nDevFramerate;
    std::map<AX_U8, std::unique_ptr<CFramerateCtrlHelper>> mapPipe2FrmCtrl;
    for (auto& m : vecTargetPipeFramerate) {
        mapPipe2FrmCtrl[m.nPipeID] = std::unique_ptr<CFramerateCtrlHelper>(new CFramerateCtrlHelper(nDevFramerateRemain, m.fPipeFramerate));
        LOG_M(SNS_MGR, "Framerate control => [dev%d, pipe%d]: %d => %d", nDevID, m.nPipeID, nDevFramerate, (AX_U32)m.fPipeFramerate);

        nDevFramerateRemain -= m.fPipeFramerate;
    }

    AX_S32 nRet = 0;
    pThreadParam->bThreadRunning = AX_TRUE;
    AX_IMG_INFO_T tImg[4] = {0};
    AX_U64 nSeq = 0;
    while (pThreadParam->bThreadRunning) {
        for (AX_S32 i = 0; i < eHdrMode; i++) {
            nRet = AX_VIN_GetDevFrame(nDevID, (AX_SNS_HDR_FRAME_E)i, tImg + i, -1);
            if (AX_SUCCESS != nRet) {
                LOG_M_E(SNS_MGR, "AX_VIN_GetDevFrame failed, ret=0x%x.", nRet);
                continue;
            } else {
                LOG_M_D(SNS_MGR, "[%lld] Get dev frame, hdrframe=%d, seq=%lld.", nSeq, i, tImg[i].tFrameInfo.stVFrame.u64SeqNum);
            }
        }

        for (auto& m : vecTargetPipeFramerate) {
            AX_U8 nPipe = m.nPipeID;
            if (!mapPipe2FrmCtrl[nPipe].get()->FramerateCtrl()) {
                nRet = AX_VIN_SendRawFrame(nPipe, AX_VIN_FRAME_SOURCE_ID_IFE, eHdrMode, (const AX_IMG_INFO_T**)&tImg, 0);
                if (AX_SUCCESS != nRet) {
                    LOG_M_E(SNS_MGR, "AX_VIN_SendRawFrame failed, ret=0x%x.", nRet);
                    continue;
                } else {
                    LOG_M_D(SNS_MGR, "[%lld] Send raw to IFE pipe %d, frame num=%d", nSeq, nPipe, eHdrMode);
                }

                break;
            }
        }

        for (AX_S32 i = 0; i < eHdrMode; i++) {
            nRet = AX_VIN_ReleaseDevFrame(nDevID, (AX_SNS_HDR_FRAME_E)i, tImg + i);
            if (AX_SUCCESS != nRet) {
                LOG_M_E(SNS_MGR, "AX_VIN_ReleaseDevFrame failed, ret=0x%x.", nRet);
                continue;
            } else {
                LOG_M_D(SNS_MGR, "[%lld] Release dev frame, hdrframe=%d, seq=%lld.", nSeq, i, tImg[i].tFrameInfo.stVFrame.u64SeqNum);
            }
        }

        nSeq++;
    }
}

AX_VOID CSensorMgr::YuvGetThreadFunc(YUV_THREAD_PARAM_T* pThreadParam) {
    AX_U8 nPipe = pThreadParam->nPipeID;
    AX_U8 nChn = pThreadParam->nIspChn;
    AX_U8 nSnsID = pThreadParam->nSnsID;

    LOG_MM(SNS_MGR, "[%d][%d] +++", nPipe, nChn);

    AX_CHAR szName[32] = {0};
    sprintf(szName, "YUV_Get_%d_%d", nPipe, nChn);
    prctl(PR_SET_NAME, szName);

    AX_S32 nRet = 0;
    pThreadParam->bThreadRunning = AX_TRUE;
    while (pThreadParam->bThreadRunning) {
        if (!m_bGetYuvFlag[nSnsID]) {
            CElapsedTimer::GetInstance()->mSleep(10);
            continue;
        }
        AX_IMG_INFO_T* pVinImg = new (std::nothrow) AX_IMG_INFO_T();
        if (nullptr == pVinImg) {
            LOG_M_E(SNS_MGR, "Allocate buffer for YuvGetThread failed.");
            CElapsedTimer::GetInstance()->mSleep(10);
            continue;
        }

        nRet = AX_VIN_GetYuvFrame(nPipe, (AX_VIN_CHN_ID_E)nChn, pVinImg, 1000);
        if (AX_SUCCESS != nRet) {
            if (pThreadParam->bThreadRunning) {
                LOG_M_E(SNS_MGR, "[%d][%d] AX_VIN_GetYuvFrame failed, ret=0x%x, unreleased buffer=%d", nPipe, nChn, nRet,
                        m_qFrame[nPipe][nChn].size());
            }
            SAFE_DELETE_PTR(pVinImg);
            continue;
        }

        LOG_MM_D(SNS_MGR, "[%d][%d] Seq %llu, Size %d, w:%d, h:%d, PTS:%llu, [FramePhyAddr:0x%llx, FrameVirAddr:0x%llx], addr:%p", nPipe,
                 nChn, pVinImg->tFrameInfo.stVFrame.u64SeqNum, pVinImg->tFrameInfo.stVFrame.u32FrameSize,
                 pVinImg->tFrameInfo.stVFrame.u32Width, pVinImg->tFrameInfo.stVFrame.u32Height, pVinImg->tFrameInfo.stVFrame.u64PTS,
                 pVinImg->tFrameInfo.stVFrame.u64PhyAddr[0], pVinImg->tFrameInfo.stVFrame.u64VirAddr[0], pVinImg);

        ///////////////////////////// DEBUG DATA //////////////////////////////////
        // AX_VIN_ReleaseYuvFrame(nPipe, (AX_VIN_CHN_ID_E)nChn, pVinImg);
        // SAFE_DELETE_PTR(pVinImg);
        // continue;
        ///////////////////////////////////////////////////////////////

        CAXFrame* pAXFrame = new (std::nothrow) CAXFrame();
        pAXFrame->nGrp = nPipe;
        pAXFrame->nChn = nChn;
        pAXFrame->stFrame.stVFrame = pVinImg->tFrameInfo;
        pAXFrame->pFrameRelease = this;
        pAXFrame->pUserDefine = pVinImg;
        /* Here, we can not determine bMultiplex flag according to number of observers, because each observer must filter frames by target
         * pipe & channel */
        pAXFrame->bMultiplex = pThreadParam->bMultiplex;

        m_mtxFrame[nPipe][nChn].lock();
        if (m_qFrame[nPipe][nChn].size() >= 5) {
            LOG_MM_W(SNS_MGR, "[%d][%d] queue size is %d, drop this frame", nPipe, nChn, m_qFrame[nPipe][nChn].size());
            AX_VIN_ReleaseYuvFrame(nPipe, (AX_VIN_CHN_ID_E)nChn, pVinImg);
            SAFE_DELETE_PTR(pVinImg);
            SAFE_DELETE_PTR(pAXFrame);

            m_mtxFrame[nPipe][nChn].unlock();
            continue;
        }

        m_qFrame[nPipe][nChn].push_back(pAXFrame);
        m_mtxFrame[nPipe][nChn].unlock();

        NotifyAll(nPipe, nChn, pAXFrame);
    }

    LOG_MM(SNS_MGR, "[%d][%d] ---", nPipe, nChn);
}

#define MIN_IFE_AF_TEST_LOOP_COUNT (1)
#define MAX_IFE_AF_TEST_LOOP_COUNT (30)
#define IFE_AF_DONE_IRQ_TIMEOUT    (-1)
AX_VOID CSensorMgr::IfeAfTestThreadFunc(IEF_AF_TEST_THREAD_PARAM_T* pThreadParam) {
    AX_U8 nSnsId = pThreadParam->nSnsID;
    AX_U8 nPipe = pThreadParam->nPipeID;

    LOG_MM_C(SNS_MGR, "[%d] +++", nPipe);

    AX_CHAR szName[32] = {0};
    sprintf(szName, "IFE_AF_TEST_%d", nPipe);
    prctl(PR_SET_NAME, szName);

    AX_S32 nRet = 0;
    pThreadParam->bThreadRunning = AX_TRUE;
    AX_U32 nLoopCnt = 1;
    AX_ISP_IQ_AF_STAT_PARAM_T tParam = {0};
    AX_ISP_AF_STAT_INFO_T tAfStat = {0};

    while (pThreadParam->bThreadRunning) {
        nRet = AX_ISP_IQ_GetAf0StatParam(nPipe, &tParam);
        if (0 != nRet) {
            LOG_MM_E(SNS_MGR, "[%d] AX_ISP_IQ_GetAf0StatParam failed, ret: 0x%x.", nPipe, nRet);
            CElapsedTimer::GetInstance()->mSleep(10);
            continue;
        }

        {
            std::lock_guard<std::mutex> lck(m_mtxAf);
            // Enable IFE AF
            if (0 == tParam.nAfEnable) {
                tParam.nAfEnable = 1;
                nRet = AX_ISP_IQ_SetAf0StatParam(nPipe, &tParam);
                if (0 != nRet) {
                    LOG_MM_E(SNS_MGR, "[%d] AX_ISP_IQ_SetAf0StatParam(AF On) failed, ret: 0x%x.", nPipe, nRet);
                    CElapsedTimer::GetInstance()->mSleep(10);
                    continue;
                }
            }

            m_arrAfEnable[nSnsId] = AX_TRUE;
        }

        // Get random loop count
        nLoopCnt = CCommonUtils::getRandomInt(MIN_IFE_AF_TEST_LOOP_COUNT, MAX_IFE_AF_TEST_LOOP_COUNT);

        // Try to get IFE AF Statistics in nLoopCnt times
        for (AX_U8 i = 0; i < nLoopCnt; i++) {
            if (!pThreadParam->bThreadRunning) {
                break;
            }

            nRet = AX_ISP_GetIrqTimeOut(nPipe, AX_IRQ_TYPE_IFE_AF_DONE, IFE_AF_DONE_IRQ_TIMEOUT);
            if (0 != nRet) {
                LOG_MM_E(SNS_MGR, "[%d] loop[%d-%d] AX_ISP_GetIrqTimeOut failed, ret: 0x%x.", nPipe, i, nLoopCnt, nRet);
                CElapsedTimer::GetInstance()->mSleep(10);
                continue;
            }

            nRet = AX_ISP_IQ_GetAf0Statistics(nPipe, &tAfStat);
            if (0 != nRet) {
                LOG_MM_E(SNS_MGR, "[%d] loop[%d-%d] AX_ISP_IQ_GetAf0Statistics failed, ret: 0x%x.", nPipe, i, nLoopCnt, nRet);
                CElapsedTimer::GetInstance()->mSleep(10);
                continue;
            }
        }

        // Disable IFE AF
        tParam.nAfEnable = 0;

        while (pThreadParam->bThreadRunning) {
            {
                std::lock_guard<std::mutex> lck(m_mtxAf);
                if (IsAllowDisableAf()) {
                    nRet = AX_ISP_IQ_SetAf0StatParam(nPipe, &tParam);
                    if (0 != nRet) {
                        LOG_MM_E(SNS_MGR, "[%d] AX_ISP_IQ_SetAf0StatParam(AF Off) failed, ret: 0x%x.", nPipe, nRet);
                    } else {
                        m_arrAfEnable[nSnsId] = AX_FALSE;
                    }
                    break;
                }
            }
            CElapsedTimer::GetInstance()->mSleep(10);
        }
    }

    LOG_MM_C(SNS_MGR, "[%d] ---", nPipe);
}

AX_BOOL CSensorMgr::RestartWithScenario(AX_S32 nScenario) {
    if (!Stop()) {
        return AX_FALSE;
    }

    if (!DeInit()) {
        return AX_FALSE;
    }

    SET_APP_CURR_SCENARIO((AX_IPC_SCENARIO_E)nScenario);

    if (!Init()) {
        return AX_FALSE;
    }

    if (!Start()) {
        return AX_FALSE;
    }

    return AX_TRUE;
}

CBaseSensor* CSensorMgr::GetSnsInstance(AX_U32 nIndex) {
    if (nIndex >= m_vecSensorIns.size()) {
        return nullptr;
    }

    return m_vecSensorIns[nIndex];
}

AX_S8 CSensorMgr::PipeFromSns(AX_U8 nPipeID) {
    for (AX_U8 i = 0; i < GetSensorCount(); i++) {
        CBaseSensor* pSensor = GetSnsInstance(i);
        AX_U32 nPipeCount = pSensor->GetPipeCount();
        for (AX_U8 j = 0; j < nPipeCount; j++) {
            if (nPipeID == pSensor->GetSnsConfig().arrPipeAttr[j].nPipeID) {
                return i;
            }
        }
    }

    return -1;
}

AX_BOOL CSensorMgr::UpdateAttrCB(ISensor* pInstance) {
    if (nullptr == pInstance) {
        return AX_FALSE;
    }

    /* Sample code to update attributes before sensor.Open */
    // SNS_ABILITY_T tSnsAbilities = pInstance->GetAbilities();

    // AX_VIN_PIPE_ATTR_T tPipeAttr = pInstance->GetPipeAttr();
    // tPipeAttr.tCompressInfo = AX_TRUE;
    // pInstance->SetPipeAttr(tPipeAttr);

    return AX_TRUE;
}

AX_BOOL CSensorMgr::ChangeDaynightMode(AX_U32 nSnsID, AX_DAYNIGHT_MODE_E eDaynightMode) {
    CBaseSensor* pSensor = GetSnsInstance(nSnsID);
    AX_U32 nPipeCount = pSensor->GetPipeCount();
    for (AX_U8 i = 0; i < nPipeCount; i++) {
        for (AX_U8 j = 0; j < AX_VIN_CHN_ID_MAX; j++) {
            AX_VIN_CHN_ATTR_T tChnAttr = pSensor->GetChnAttr(i, j);
            if (0 == tChnAttr.nDepth) {
                continue;
            }

            AX_S32 nRet = AX_VIN_SetChnDayNightMode(i, (AX_VIN_CHN_ID_E)j, eDaynightMode);
            if (0 != nRet) {
                LOG_M_E(SNS_MGR, "[%d][%d][%d] AX_VIN_SetChnDayNightMode failed, ret=0x%x.", nSnsID, i, j, nRet);
                return AX_FALSE;
            } else {
                LOG_M_W(SNS_MGR, "[%d][%d][%d] AX_VIN_SetChnDayNightMode OK.", nSnsID, i, j);
            }
        }
    }

    return AX_TRUE;
}

AX_VOID CSensorMgr::SwitchSnsMode(AX_U32 nSnsID, AX_U32 nSnsMode, AX_S32 nFps) {
    LOG_MM_C(SNS_MGR, "+++");

    StopIfeAfTestThread();

    // Stop();
    StopNtCtrl();

    for (auto pSensor : m_vecSensorIns) {
        if (nSnsID == pSensor->GetSnsConfig().nSnsID) {
            LOG_MM_I(SNS_MGR, "Change Sensor(snsID: %d) mode to %d", nSnsID, nSnsMode);

            /* stop nt-ctrl and sensor */
            if (!Stop(pSensor)) {
                LOG_M_W(SNS_MGR, "Stop sernor %d failed.", nSnsID);
            }

            AX_U8 nDevID = pSensor->GetSnsConfig().nDevID;
            pSensor->ChangeHdrMode(nSnsMode, nFps);
            m_mapDev2ThreadParam[nDevID].eHdrMode = (AX_SNS_HDR_MODE_E)nSnsMode;
            pSensor->Init();

            // Reset chn stride align to AX_VIN_IPC_FBC_WIDTH_ALIGN_VAL
            AX_U8 nPipeCnt = pSensor->GetPipeCount();
            for (AX_U8 j = 0; j < nPipeCnt; j++) {
                for (AX_U8 k = 0; k < AX_VIN_CHN_ID_MAX; k++) {
                    PIPE_CONFIG_T tPipeAttr = pSensor->GetSnsConfig().arrPipeAttr[j];
                    AX_VIN_CHN_ATTR_T stVinChnAttr = pSensor->GetChnAttr(tPipeAttr.nPipeID, k);
                    if (stVinChnAttr.nWidth > 0 && stVinChnAttr.nHeight > 0) {
                        AX_FRAME_COMPRESS_INFO_T stFrCompInfo = tPipeAttr.arrChannelAttr[k].tChnCompressInfo;
                        stVinChnAttr.nWidthStride = ALIGN_UP(stVinChnAttr.nWidth,
                            stFrCompInfo.enCompressMode == AX_COMPRESS_MODE_NONE ? 2 : AX_VIN_IPC_FBC_WIDTH_ALIGN_VAL);
                        pSensor->SetChnAttr(tPipeAttr.nPipeID, k, stVinChnAttr);
                    }
                }
            }


            /* stop sensor and nt-ctrl */
            if (!Start(pSensor)) {
                LOG_M_W(SNS_MGR, "Start sernor %d failed.", nSnsID);
            }

            break;
        }
    }

    // Start();
    StartNtCtrl();

    StartIfeAfTestThread();

    LOG_MM_C(SNS_MGR, "---");
    return;
}

AX_VOID CSensorMgr::ChangeSnsFps(AX_U32 nSnsID, AX_F32 fFrameRate) {
    CBaseSensor* pCurSensor = GetSnsInstance(nSnsID);
    if (nullptr == pCurSensor) {
        return;
    }
    AX_SNS_ATTR_T stSnsAttr = pCurSensor->GetSnsAttr();
    stSnsAttr.fFrameRate = fFrameRate;
    pCurSensor->SetSnsAttr(stSnsAttr);
    pCurSensor->UpdateSnsAttr();
}

AX_BOOL CSensorMgr::ChangeSnsMirrorFlip(AX_U32 nSnsID, AX_BOOL bMirror, AX_BOOL bFlip) {
    CBaseSensor* pCurSensor = GetSnsInstance(nSnsID);
    if (nullptr == pCurSensor) {
        return AX_FALSE;
    }

    return pCurSensor->ChangeSnsMirrorFlip(bMirror, bFlip);
}

AX_VOID CSensorMgr::SetAeAwbSyncRatio(const std::map<AX_U8, AX_AVS_AEAWB_SYNC_RATIO_T>& mapAeAwbSyncRatio) {
    m_mapAeAwbSyncRatio = mapAeAwbSyncRatio;
}

AX_S32 CSensorMgr::EnableAeAwbSyncRatio(AX_U8 nPipeId, AX_BOOL bEnable) {
    auto it = m_mapAeAwbSyncRatio.find(nPipeId);
    if (it == m_mapAeAwbSyncRatio.end()) {
        return 0;
    }

    AX_S32 nRet = 0;
    AX_U32 nRatioOffValue = 1 << 20;

    AX_ISP_IQ_AE_SYNC_RATIO_T  tAeSyncRatio{0};
    AX_ISP_IQ_AWB_SYNC_RATIO_T tAwbSyncRatio{0};

    tAeSyncRatio.nAeSyncRatio = bEnable ? it->second.tAeSyncRatio.nAeSyncRatio : nRatioOffValue;
    tAwbSyncRatio.nRGainRatio = bEnable ? it->second.tAwbSyncRatio.nRGainRatio : nRatioOffValue;
    tAwbSyncRatio.nBGainRatio = bEnable ? it->second.tAwbSyncRatio.nBGainRatio : nRatioOffValue;

    LOG_MM_I(SNS_MGR, "Pipe[%d] bEnable: %d, nAeSyncRatio: %d, nRGainRatio: %d, nBGainRatio: %d",
                    nPipeId,
                    bEnable,
                    tAeSyncRatio.nAeSyncRatio,
                    tAwbSyncRatio.nRGainRatio,
                    tAwbSyncRatio.nBGainRatio);

    nRet = AX_ISP_IQ_SetAeSyncParam(nPipeId, &tAeSyncRatio);
    if (0 != nRet) {
        LOG_M_E(SNS_MGR, "Pipe[%d] AX_ISP_IQ_SetAeSyncParam fail, nRet: 0x%x", nPipeId, nRet);
        return nRet;
    }

    nRet = AX_ISP_IQ_SetAwbSyncParam(nPipeId, &tAwbSyncRatio);
    if (0 != nRet) {
        LOG_M_E(SNS_MGR, "Pipe[%d] AX_ISP_IQ_SetAwbSyncParam fail, nRet: 0x%x", nPipeId, nRet);
        return nRet;
    }

    return nRet;
}

AX_S32 CSensorMgr::EnableAeAwbSyncRatio(AX_BOOL bEnable) {
    AX_S32 nRet = 0;
    AX_U32 nRatioOffValue = 1 << 20;
    AX_ISP_IQ_AE_SYNC_RATIO_T  tAeSyncRatio{0};
    AX_ISP_IQ_AWB_SYNC_RATIO_T tAwbSyncRatio{0};

    for (auto& syncRatio : m_mapAeAwbSyncRatio) {
        AX_U8 nPipeId = syncRatio.first;

        tAeSyncRatio.nAeSyncRatio = bEnable ? syncRatio.second.tAeSyncRatio.nAeSyncRatio : nRatioOffValue;
        tAwbSyncRatio.nRGainRatio = bEnable ? syncRatio.second.tAwbSyncRatio.nRGainRatio : nRatioOffValue;
        tAwbSyncRatio.nBGainRatio = bEnable ? syncRatio.second.tAwbSyncRatio.nBGainRatio : nRatioOffValue;

        LOG_MM_I(SNS_MGR, "Pipe[%d] bEnable: %d, nAeSyncRatio: %d, nRGainRatio: %d, nBGainRatio: %d",
                        nPipeId,
                        bEnable,
                        tAeSyncRatio.nAeSyncRatio,
                        tAwbSyncRatio.nRGainRatio,
                        tAwbSyncRatio.nBGainRatio);

        nRet = AX_ISP_IQ_SetAeSyncParam(nPipeId, &tAeSyncRatio);
        if (0 != nRet) {
            LOG_M_E(SNS_MGR, "Pipe[%d] AX_ISP_IQ_SetAeSyncParam fail, nRet: 0x%x", nPipeId, nRet);
            break;
        }

        nRet = AX_ISP_IQ_SetAwbSyncParam(nPipeId, &tAwbSyncRatio);
        if (0 != nRet) {
            LOG_M_E(SNS_MGR, "Pipe[%d] AX_ISP_IQ_SetAwbSyncParam fail, nRet: 0x%x", nPipeId, nRet);
            break;
        }
    }

    return nRet;
}

AX_BOOL CSensorMgr::EnableEIS(AX_U32 nSnsID, AX_BOOL bEnable) {
    CBaseSensor* pCurSensor = GetSnsInstance(nSnsID);
    if (nullptr == pCurSensor) {
        LOG_MM_E(SNS_MGR, "Get sensor(%d) obj fail!", nSnsID);
        return AX_FALSE;
    }

    return pCurSensor->EnableEIS(bEnable);
}

AX_BOOL CSensorMgr::EZoom(AX_U32 nSnsID, AX_S32 nEZoomRatio, AX_S32 nCenterOffsetX, AX_S32 nCenterOffsetY)
{
    CBaseSensor* pCurSensor = GetSnsInstance(nSnsID);

    if (nullptr == pCurSensor) {
        return AX_FALSE;
    }

    return pCurSensor->EZoom(nEZoomRatio, nCenterOffsetX, nCenterOffsetY);
}

AX_BOOL CSensorMgr::UpdateLDC(AX_U32 nSnsID, AX_BOOL bLdcEnable, AX_BOOL bAspect, AX_S16 nXRatio, AX_S16 nYRatio, AX_S16 nXYRatio,
                              AX_S16 nDistorRatio) {
    CBaseSensor* pCurSensor = GetSnsInstance(nSnsID);
    if (nullptr == pCurSensor) {
        return AX_FALSE;
    }

    AX_BOOL ret = pCurSensor->UpdateLDC(bLdcEnable, bAspect, nXRatio, nYRatio, nXYRatio, nDistorRatio);
    if (!ret) {
        LOG_MM_E(SNS_MGR, "sensor%d UpdateLDC failed.[%d,%d,%d,%d,%d,%d]", nSnsID, bLdcEnable, bAspect, nXRatio, nYRatio, nXYRatio,
                 nDistorRatio);
    }
    return ret;
}

AX_BOOL CSensorMgr::SetChnCompress(AX_U32 nSnsID, AX_U8 nPipe, AX_U8 nChannel, AX_BOOL bCompressOn) {
    CBaseSensor* pCurSensor = GetSnsInstance(nSnsID);
    if (nullptr == pCurSensor) {
        return AX_FALSE;
    }

    AX_BOOL ret = pCurSensor->SetChnCompress(nPipe, nChannel, bCompressOn);
    if (!ret) {
        LOG_MM_E(SNS_MGR, "sensor%d SetChnCompress(%d) failed.", nSnsID, bCompressOn);
    }
    return ret;
}

AX_BOOL CSensorMgr::IsRotateEngine(AX_U32 nSnsID) {
    CBaseSensor* pCurSensor = GetSnsInstance(nSnsID);
    if (nullptr == pCurSensor) {
        return AX_FALSE;
    }

    return pCurSensor->IsRotateEngine();
}

AX_BOOL CSensorMgr::IsMirrorFlipEngine(AX_U32 nSnsID) {
    CBaseSensor* pCurSensor = GetSnsInstance(nSnsID);
    if (nullptr == pCurSensor) {
        return AX_FALSE;
    }

    return pCurSensor->IsMirrorFlipEngine();
}

AX_BOOL CSensorMgr::UpdateRotation(AX_U32 nSnsID, AX_VIN_ROTATION_E eRotation)
{
    CBaseSensor* pCurSensor = GetSnsInstance(nSnsID);

    if (nullptr == pCurSensor) {
        return AX_FALSE;
    }

    if (pCurSensor->UpdateRotation(eRotation)) { // Just support chn0 currently
        SENSOR_CONFIG_T stSnsCfg = pCurSensor->GetSnsConfig();
        for (AX_U8 i = 0; i < stSnsCfg.nPipeCount; i++) {
            AX_U8 nPipeID = stSnsCfg.arrPipeAttr[i].nPipeID;
            AX_VIN_CHN_ATTR_T tChnAttr = pCurSensor->GetChnAttr(nPipeID, 0);

            AX_U32 nWidth  = tChnAttr.nWidth;
            AX_U32 nHeight = tChnAttr.nHeight;

            if (AX_VIN_ROTATION_90 == eRotation || AX_VIN_ROTATION_270 == eRotation) {
                std::swap(nWidth, nHeight);
            }

            OBS_TRANS_ROTATE_INFO_T stObsTransRotation;
            stObsTransRotation.nRotation = (AX_U8)eRotation;
            stObsTransRotation.bInFBC = (0 == tChnAttr.tCompressInfo.enCompressMode) ? AX_FALSE : AX_TRUE;
            stObsTransRotation.nWidth    = nWidth;
            stObsTransRotation.nHeight   = nHeight;
            OnDataChange(nPipeID, 0, E_OBS_CHANGE_DATA_TYPE_ROTATION, &stObsTransRotation);
        }
    }

    return AX_TRUE;
}

AX_VIN_ROTATION_E CSensorMgr::GetRotation(AX_U32 nSnsID, AX_BOOL bIgnorePipe /*= AX_TRUE*/, AX_U8 nPipe/*= 0*/) {
    CBaseSensor* pCurSensor = GetSnsInstance(nSnsID);
    if (nullptr == pCurSensor) {
        LOG_MM_E(SNS_MGR, "Get sns(%d) instance fail.", nSnsID);
        return AX_VIN_ROTATION_0;
    }

    return pCurSensor->GetRotation(bIgnorePipe, nPipe);
}

AX_BOOL CSensorMgr::IsMirror(AX_U32 nSnsID, AX_BOOL bIgnorePipe /*= AX_TRUE*/, AX_U8 nPipe/*= 0*/) {
    CBaseSensor* pCurSensor = GetSnsInstance(nSnsID);
    if (nullptr == pCurSensor) {
        return AX_FALSE;
    }

    return pCurSensor->IsMirror(bIgnorePipe, nPipe);
}

AX_BOOL CSensorMgr::IsFlip(AX_U32 nSnsID, AX_BOOL bIgnorePipe /*= AX_TRUE*/, AX_U8 nPipe/*= 0*/) {
    CBaseSensor* pCurSensor = GetSnsInstance(nSnsID);
    if (nullptr == pCurSensor) {
        return AX_FALSE;
    }

    return pCurSensor->IsFlip(bIgnorePipe, nPipe);
}

AX_BOOL CSensorMgr::IsAllowDisableAf() {
    AX_U32 nSensorCount = APP_SENSOR_COUNT();
    AX_U8 nEnabledCnt = 0;
    AX_BOOL bAllowDisableAf = AX_FALSE;
    for (AX_U8 i = 0; i < nSensorCount; i++) {
        if (m_arrAfEnable[i]) {
            nEnabledCnt++;
        }
    }

    if (nEnabledCnt > 1) {
        bAllowDisableAf = AX_TRUE;
    }

    LOG_MM_I(SNS_MGR, "Current enabled AF count: %d", nEnabledCnt);

    return bAllowDisableAf;
}
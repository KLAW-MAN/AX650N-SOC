/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include <math.h>
#include "ITSSensorMgr.h"
#include <sys/prctl.h>
#include "AppLogApi.h"
#include "ElapsedTimer.hpp"
#include "FramerateCtrlHelper.h"
#include "GlobalDef.h"
#include "OptionHelper.h"
#include "SensorOptionHelper.h"
#include "SensorFactory.hpp"
#include "ax_nt_ctrl_api.h"
#include "ax_nt_stream_api.h"
#include "CmdLineParser.h"
#include "ax_vin_error_code.h"
#include "PrintHelper.h"

#define SNS_MGR "SNS_MGR"
using namespace AX_ITS;

///////////////////////////////////////////////////////////////////////////////////////////
AX_BOOL CSensorMgr::Init() {
    AX_U32 nSensorCount = APP_SENSOR_COUNT();
    for (AX_U32 i = 0; i < nSensorCount; i++) {
        SENSOR_CONFIG_T tSensorCfg;
        if (!APP_SENSOR_CONFIG(i, tSensorCfg)) {
            LOG_MM_E(SNS_MGR, "Failed to get sensor config %d", i);
            return AX_FALSE;
        }
        CBaseSensor* pSensor = (CBaseSensor*)(CSensorFactory::GetInstance()->CreateSensor(tSensorCfg));
        if (nullptr == pSensor) {
            LOG_MM_E(SNS_MGR, "Failed to create sensor instance %d", i);
            return AX_FALSE;
        } else {
            LOG_M(SNS_MGR, "Create sensor instance %d ok.", i);
        }

        pSensor->RegAttrUpdCallback(UpdateAttrCB);

        if (!pSensor->Init()) {
            LOG_MM_E(SNS_MGR, "Failed to initial sensor instance %d", i);
            return AX_FALSE;
        } else {
            LOG_M(SNS_MGR, "Initailize sensor %d ok.", i);
        }

        m_vecSensorIns.emplace_back(pSensor);

        if (pSensor->IsIFERawDumpEnable()) {
            CAXLockQ<AX_IMG_INFO_T*>* objDumpIFERawQ = new (std::nothrow) CAXLockQ<AX_IMG_INFO_T*>();
            if (!objDumpIFERawQ) {
                LOG_MM_E(SNS_MGR, "alloc queue fail");
                return AX_FALSE;
            }

            objDumpIFERawQ->SetCapacity(3);
            m_VecDumpRawFrameQ.emplace_back(objDumpIFERawQ);
        }
    }

    WEB_SHOW_SENSOR_MODE_E eWebSnsShowMode = (1 == nSensorCount) ? E_WEB_SHOW_SENSOR_MODE_SINGLE : E_WEB_SHOW_SENSOR_MODE_DUAL;
    SET_APP_WEB_SHOW_SENSOR_MODE(eWebSnsShowMode);
    m_arrFrameQ = new (std::nothrow) CAXLockQ<AX_IMG_INFO_T*>[nSensorCount];
    if (!m_arrFrameQ) {
        LOG_MM_E(SNS_MGR, "alloc queue fail");
        return AX_FALSE;
    } else {
        for (AX_U32 i = 0; i < nSensorCount; ++i) {
            m_arrFrameQ[i].SetCapacity(6);
        }
    }

    for(AX_U8 i = 0; i < nSensorCount; i++) {
        for (AX_U8 j = 0; j < 3; j++) {
            for (AX_U8 k = 0; k < AX_SHUTTER_SEQ_NUM; k++) {
                if (0 == j) {
                    m_mapShutterMode[i][j][k] = AX_VIN_SHUTTER_MODE_VIDEO;
                } else if (1 == j) {
                    if (0 == k % 2) {
                        m_mapShutterMode[i][j][k] = AX_VIN_SHUTTER_MODE_VIDEO;
                    } else {
                        m_mapShutterMode[i][j][k] = AX_VIN_SHUTTER_MODE_PICTURE;
                    }
                } else if (2 == j) {
                    if (0 == k % 2) {
                        m_mapShutterMode[i][j][k] = AX_VIN_SHUTTER_MODE_VIDEO;
                    } else {
                        m_mapShutterMode[i][j][k] = AX_VIN_SHUTTER_MODE_PICTURE;
                    }

                    if ((AX_SHUTTER_SEQ_NUM - 1) == k) {
                        srand(time(NULL));
                        AX_U8 nRandNum = rand() % AX_SHUTTER_SEQ_NUM;
                        m_mapShutterMode[i][j][nRandNum] = AX_VIN_SHUTTER_MODE_FLASH_SNAP;
                    }
                } else {
                    LOG_MM_E(SNS_MGR, "unsupported shutter type: %d", j);
                }
            }
        }
    }

    return AX_TRUE;
}

AX_BOOL CSensorMgr::DeInit() {
    for (ISensor* pSensor : m_vecSensorIns) {
        CSensorFactory::GetInstance()->DestorySensor(pSensor);
    }

    if (m_arrFrameQ) {
        delete[] m_arrFrameQ;
        m_arrFrameQ = nullptr;
    }

    for (auto objRawFramQ : m_VecDumpRawFrameQ) {
        delete objRawFramQ;
        objRawFramQ = nullptr;
    }
    m_VecDumpRawFrameQ.clear();

    return AX_TRUE;
}

AX_BOOL CSensorMgr::Start() {
    for (auto pSensor : m_vecSensorIns) {
        if (!pSensor->Open()) {
            return AX_FALSE;
        }
    }

    StartNtCtrl();
    for(auto e : m_vecSensorIns) {
        if (!((AX_SNS_DCG_HDR == e->GetSnsAttr().eSnsOutputMode) || (AX_SNS_DCG_VS_HDR == e->GetSnsAttr().eSnsOutputMode))) {
            StartYuvGetThread();
            StartDispatchRawThread();
            break;
        }
    }
    StartDispatchIFEDumpRawThread();
    StartSnapThread();
    StartIFEDumpRawThread();
    return AX_TRUE;
}

AX_BOOL CSensorMgr::Stop() {
    StopNtCtrl();
    StopIFEDumpRawThread();
    for(auto e : m_vecSensorIns) {
        if (!((AX_SNS_DCG_HDR == e->GetSnsAttr().eSnsOutputMode) || (AX_SNS_DCG_VS_HDR == e->GetSnsAttr().eSnsOutputMode))) {
            StopYuvGetThread();
            StopDispatchRawThread();
            break;
        }
    }
    StopSnapThread();
    StopDispatchIFEDumpRawThread();
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
        m_mapDev2ThreadParam[tSnsCfg.nDevID].nFrameCount = 0;
        if ((AX_TRUE == tSnsCfg.bEnableFlash) && (2 == tSnsCfg.nPipeCount)) {
            m_mapDev2ThreadParam[tSnsCfg.nDevID].ePipeComboType = APP_PIPE_COMBO_TYPE_TS;
        } else if ((AX_TRUE == tSnsCfg.bEnableFlash) && (3 == tSnsCfg.nPipeCount)) {
            m_mapDev2ThreadParam[tSnsCfg.nDevID].ePipeComboType = APP_PIPE_COMBO_TYPE_SS;
        } else if (E_PPL_SCENRIO_11 == CCmdLineParser::GetInstance()->GetScenario()) {
            for (AX_U8 i = 0; i < tSnsCfg.nPipeCount; i++) {
                if(!tSnsCfg.arrPipeAttr[i].bSnapshot){
                    continue;
                }

                if (APP_FLASH_FRAME_RANDOM == tSnsCfg.arrPipeAttr[i].nContinousFrames) {
                    m_mapDev2ThreadParam[tSnsCfg.nDevID].ePipeComboType = APP_PIPE_COMBO_TYPE_TS_MODE0;
                } else if (APP_FLASH_FRAME_NONE == tSnsCfg.arrPipeAttr[i].nContinousFrames) {
                    m_mapDev2ThreadParam[tSnsCfg.nDevID].ePipeComboType = APP_PIPE_COMBO_TYPE_TS_MODE1;
                } else if (tSnsCfg.arrPipeAttr[i].nContinousFrames > APP_FLASH_FRAME_NONE) {
                    m_mapDev2ThreadParam[tSnsCfg.nDevID].ePipeComboType = APP_PIPE_COMBO_TYPE_TS_MODE2;
                    m_mapDev2ThreadParam[tSnsCfg.nDevID].nContinuousFrameCount = tSnsCfg.arrPipeAttr[i].nContinousFrames;
                } else {
                    m_mapDev2ThreadParam[tSnsCfg.nDevID].ePipeComboType = APP_PIPE_COMBO_TYPE_TS_MODE0;
                }
            }
        } else {
            m_mapDev2ThreadParam[tSnsCfg.nDevID].ePipeComboType = APP_PIPE_COMBO_TYPE_NORMAL;
        }

        for (AX_U8 i = 0; i < tSnsCfg.nPipeCount; i++) {
            m_mapDev2ThreadParam[tSnsCfg.nDevID].vecTargetPipeFramerate.emplace_back(
                tSnsCfg.arrPipeAttr[i].nPipeID, tSnsCfg.arrPipeAttr[i].fPipeFramerate, tSnsCfg.arrPipeAttr[i].bSnapshot,
                tSnsCfg.arrPipeAttr[i].bDummyEnable);
        }
    }

    for (auto& m : m_mapDev2ThreadParam) {
        RAW_DISPATCH_THREAD_PARAM_T& tParams = m.second;
        tParams.hThread = std::thread(&CSensorMgr::RawDispatchThreadFunc, this, &tParams);
    }
}

AX_VOID CSensorMgr::StartSnapThread() {
    AX_CHAR szName[32] = {0};
    for (auto pSensor : m_vecSensorIns) {
        SENSOR_CONFIG_T tSnsCfg = pSensor->GetSnsConfig();
        for (AX_U8 i = 0; i < tSnsCfg.nPipeCount; i++) {
            if (tSnsCfg.arrPipeAttr[i].bSnapshot) {
                m_tSnap[tSnsCfg.nSnsID].nPipeID = tSnsCfg.arrPipeAttr[i].nPipeID;
                m_tSnap[tSnsCfg.nSnsID].bDummy = tSnsCfg.arrPipeAttr[i].bDummyEnable;
                m_tSnap[tSnsCfg.nSnsID].ePipeWorkMode[tSnsCfg.arrPipeAttr[i].nPipeID] = tSnsCfg.arrPipeAttr[i].ePipeWorkMode;
                break;
            }
        }
        m_tSnap[tSnsCfg.nSnsID].nDevID = tSnsCfg.nDevID;
        m_tSnap[tSnsCfg.nSnsID].nSnsID = tSnsCfg.nSnsID;
        m_tSnap[tSnsCfg.nSnsID].eHdrMode = tSnsCfg.eSensorMode;
        sprintf(szName, "AppSnap_%d", tSnsCfg.nSnsID);
        if (!m_SnapThread[tSnsCfg.nSnsID].Start(std::bind(&CSensorMgr::SnapProcess, this, std::placeholders::_1),
                                                (AX_VOID*)&m_tSnap[tSnsCfg.nSnsID], szName, SCHED_FIFO, 99)) {
            LOG_MM_E(SNS_MGR, "create snap thread fail");
        }
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

AX_VOID CSensorMgr::StopSnapThread() {
    for (int i = 0; i < MAX_SENSOR_COUNT; i++) {
        if (m_SnapThread[i].IsRunning()) {
            m_SnapThread[i].Stop();
            m_arrFrameQ[i].Wakeup();
            m_SnapThread[i].Join();

            ClearQueue(i);
        }
    }
}

AX_VOID CSensorMgr::StartYuvGetThread() {
    for (auto& mapChn2Param : m_mapYuvThreadParams) {
        for (auto& param : mapChn2Param.second) {
            YUV_THREAD_PARAM_T& tParams = param.second;
            if (!tParams.bSnapshot) {
                tParams.hThread = std::thread(&CSensorMgr::YuvGetThreadFunc, this, &tParams);
            }
        }
    }
}

AX_VOID CSensorMgr::StopYuvGetThread() {
    for (auto& mapChn2Param : m_mapYuvThreadParams) {
        for (auto& param : mapChn2Param.second) {
            YUV_THREAD_PARAM_T& tParams = param.second;
            if (!tParams.bSnapshot) {
                if (tParams.hThread.joinable()) {
                    tParams.bThreadRunning = AX_FALSE;
                }
            }
        }
    }

    for (auto& mapChn2Param : m_mapYuvThreadParams) {
        for (auto& param : mapChn2Param.second) {
            YUV_THREAD_PARAM_T& tParams = param.second;
            if (!tParams.bSnapshot) {
                if (tParams.hThread.joinable()) {
                    tParams.hThread.join();
                }
            }
        }
    }
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

AX_VOID CSensorMgr::StartIFEDumpRawThread() {
    AX_CHAR szName[32] = {0};
    for (auto pSensor : m_vecSensorIns) {
        AX_U8 i = 0;
        if (pSensor->IsIFERawDumpEnable()) {
            SENSOR_CONFIG_T tSnsCfg = pSensor->GetSnsConfig();
            m_tDumpRawParam[i].nSnsID = tSnsCfg.nSnsID;
            m_tDumpRawParam[i].nPipeID = pSensor->GetIFERawDumpPipe();
            m_tDumpRawParam[i].nSnapPipeId = pSensor->GetSnapPipe();
            m_tDumpRawParam[i].eHdrMode = tSnsCfg.eSensorMode;
            sprintf(szName, "AppGetIFEDumpRaw_%d", tSnsCfg.nSnsID);
            if (!m_IFERawDumpThread[i].Start(std::bind(&CSensorMgr::IFERawDumpThreadFunc, this, std::placeholders::_1),
                                             &i, szName)) {
                LOG_MM_E(SNS_MGR, "Sns[%d] Create ife dump raw thread fail", m_tDumpRawParam[i].nSnsID);
            }
            i++;
        }
    }
}

AX_VOID CSensorMgr::StopIFEDumpRawThread() {
    for (auto &dumpRawThread : m_IFERawDumpThread) {
        if (dumpRawThread.IsRunning()) {
            dumpRawThread.Stop();
            dumpRawThread.Join();
        }
    }
}

AX_VOID CSensorMgr::StartDispatchIFEDumpRawThread() {
    AX_CHAR szName[32] = {0};
    for (auto pSensor : m_vecSensorIns) {
        AX_U8 i = 0;
        if (pSensor->IsIFERawDumpEnable()) {
            SENSOR_CONFIG_T tSnsCfg = pSensor->GetSnsConfig();
            sprintf(szName, "AppIFERawDisp_%d", tSnsCfg.nSnsID);
            if (!m_IFEDumpRawDispatchThread[i].Start(std::bind(&CSensorMgr::IFEDumpRawQDispatchFunc, this, std::placeholders::_1),
                                             &i, szName)) {
                LOG_MM_E(SNS_MGR, "Sns[%d] Create ife dump raw dispatch frame thread fail", tSnsCfg.nSnsID);
            }
            i++;
        }
    }
}

AX_VOID CSensorMgr::StopDispatchIFEDumpRawThread() {
    for (AX_U8 i = 0; i < m_VecDumpRawFrameQ.size(); i++) {
        if (m_IFEDumpRawDispatchThread[i].IsRunning()) {
            m_IFEDumpRawDispatchThread[i].Stop();
            m_VecDumpRawFrameQ[i]->Wakeup();
            m_IFEDumpRawDispatchThread[i].Join();

            ClearIFEDumpRawFrmQueue(i);
        }
    }
}

AX_VOID CSensorMgr::SetYuvThreadParams(AX_U32 nSnsID, AX_U32 nPipeID, AX_U32 nChannel, AX_BOOL bSnapshot,
                                       AX_BOOL bMultiplex, AX_BOOL bDiscard /*= AX_FALSE*/, AX_BOOL bEnable /*= AX_TRUE*/) {
    if (nChannel >= AX_VIN_CHN_ID_MAX) {
        return;
    }
    m_mapYuvThreadParams[nPipeID][nChannel].nSnsID = nSnsID;
    m_mapYuvThreadParams[nPipeID][nChannel].nPipeID = nPipeID;
    m_mapYuvThreadParams[nPipeID][nChannel].nIspChn = nChannel;
    m_mapYuvThreadParams[nPipeID][nChannel].bSnapshot = bSnapshot;
    m_mapYuvThreadParams[nPipeID][nChannel].bMultiplex = bMultiplex;
    m_mapYuvThreadParams[nPipeID][nChannel].bThreadRunning = AX_FALSE;
    m_mapYuvThreadParams[nPipeID][nChannel].bDiscard = bDiscard;
    m_mapYuvThreadParams[nPipeID][nChannel].bEnable = bEnable;
}

AX_VOID CSensorMgr::RegObserver(AX_U32 nPipeID, AX_U32 nChannel, IObserver* pObserver) {
    if (nullptr != pObserver) {
        AX_S8 nSensorID = PipeFromSns(nPipeID);
        if (-1 == nSensorID) {
            LOG_MM_E(SNS_MGR, "Pipe %d does not configured in sensor.json", nPipeID);
            return;
        }

        // AX_VIN_PIPE_ATTR_T tPipeAttr = m_vecSensorIns[nSensorID]->GetPipeAttr(nPipeID);
        AX_VIN_CHN_ATTR_T tChnAttr = m_vecSensorIns[nSensorID]->GetChnAttr(nPipeID, nChannel);

        OBS_TRANS_ATTR_T tTransAttr;
        tTransAttr.nGroup = nPipeID;
        tTransAttr.nChannel = nChannel;
        /* vin frameRate control *1000 ,so the true frameRate is /1000*/
        tTransAttr.fFramerate = tChnAttr.tFrameRateCtrl.fDstFrameRate;
        tTransAttr.nWidth = tChnAttr.nWidth;
        tTransAttr.nHeight = tChnAttr.nHeight;
        tTransAttr.nCfgWidth = tChnAttr.nWidth;
        tTransAttr.nCfgHeight = tChnAttr.nHeight;
        tTransAttr.arrOutFBC[0] = (AX_U8)tChnAttr.tCompressInfo.enCompressMode;
        tTransAttr.arrOutFBC[1] = (AX_U8)tChnAttr.tCompressInfo.u32CompressLevel;
        tTransAttr.bLink = AX_FALSE;
        tTransAttr.nSnsSrc = PipeFromSns(nPipeID);

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

AX_VOID CSensorMgr::SnapProcess(AX_VOID* pArg) {
    SNAP_PROCESS_PARAM* pParam = (SNAP_PROCESS_PARAM*)pArg;
    AX_U8 nDevID = pParam->nDevID;
    AX_U8 nPipeID = pParam->nPipeID;
    AX_VIN_PIPE_WORK_MODE_E ePipeWorkMode = pParam->ePipeWorkMode[nPipeID];
    AX_SNS_HDR_MODE_E eHdrMode = pParam->eHdrMode;
    AX_BOOL bDummy = pParam->bDummy;

    AX_U8 nSnsId = pParam->nSnsID;
    AX_BOOL bRet = AX_FALSE;
    AX_IMG_INFO_T* pImg{nullptr};

    AX_S32 nType;
    CCmdLineParser::GetInstance()->GetLoadType(nType);
    AX_S32 nScenario;
    CCmdLineParser::GetInstance()->GetScenario(nScenario);

    AX_U8 nIFEDumpIndex = GetDumpIFERawParamIndex(nSnsId);
    LOG_MM_I(SNS_MGR, "sns[%d] nIFEDumpIndex: %d.", nSnsId, nIFEDumpIndex);

    while (m_SnapThread[nSnsId].IsRunning()) {
        if (!m_arrFrameQ[nSnsId].Pop(pImg, -1)) {
            CElapsedTimer::GetInstance()->mSleep(1);
            continue;
        }

        if (AX_VIN_PIPE_NORMAL_MODE2 == ePipeWorkMode) {
            bRet = SnapshotProcess_Offline_NPU(nDevID, nPipeID, 0, eHdrMode, (const AX_IMG_INFO_T**)pImg, bDummy, nIFEDumpIndex);
        } else {
            bRet = SnapshotProcess(nPipeID, 0, eHdrMode, (const AX_IMG_INFO_T**)pImg, bDummy);
        }

        if (bRet) {
            if (!NotifySnapshotProcess(nPipeID, 0)) {
                LOG_MM_E(SNS_MGR, "[%d] Get snapshot frame failed.", nPipeID);
            }
        } else {
            LOG_MM_E(SNS_MGR, "[%d] snapshot process failed.", nPipeID);
        }

        for (AX_S32 i = 0; i < eHdrMode; i++) {
            AX_S32 nRet = AX_VIN_ReleaseDevFrame(nDevID, (AX_SNS_HDR_FRAME_E)i, pImg + i);
            if (AX_SUCCESS != nRet) {
                LOG_MM_E(SNS_MGR, "[%d] AX_VIN_ReleaseDevFrame failed, ret=0x%x.", nDevID, nRet);
                continue;
            } else {
                LOG_MM_D(SNS_MGR, "Release dev frame, hdrframe=%d, seq=%lld.", i, pImg[i].tFrameInfo.stVFrame.u64SeqNum);
            }
        }
        SAFE_DELETE_PTR(pImg);
    }
}

AX_VOID CSensorMgr::RawDispatchThreadFunc(RAW_DISPATCH_THREAD_PARAM_T* pThreadParam) {
    AX_U8 nSnsID = pThreadParam->nSnsID;
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
    AX_S32 timeOutMs = 3000;

    AX_S32 nRet = 0;
    pThreadParam->bThreadRunning = AX_TRUE;
    AX_IMG_INFO_T tImg[4] = {0};
    AX_U64 nSeq = 0;
    AX_U32 FrmType = APP_SAMPLE_PIPE_MODE_VIDEO;
    AX_U64 frameSeqs[AX_SNS_HDR_FRAME_MAX] = {0};
    AX_U64 maxFrameSeq = 0;
    while (pThreadParam->bThreadRunning) {
        if (!m_bGetYuvFlag[nSnsID]) {
            CElapsedTimer::GetInstance()->mSleep(10);
            eHdrMode = pThreadParam->eHdrMode;
            continue;
        }
        for (AX_S32 i = 0; i < eHdrMode; i++) {
            nRet = AX_VIN_GetDevFrame(nDevID, (AX_SNS_HDR_FRAME_E)i, tImg + i, timeOutMs);
            if (AX_SUCCESS != nRet) {
                LOG_MM_E(SNS_MGR, "dev[%d] AX_VIN_GetDevFrame failed, ret=0x%x.", nDevID, nRet);
                continue;
            }
            frameSeqs[i] = tImg[i].tFrameInfo.stVFrame.u64SeqNum;
            if (frameSeqs[i] > maxFrameSeq) {
                maxFrameSeq = frameSeqs[i];
            }
        }
        if (AX_SUCCESS != nRet) {
            continue;
        }

        FrmType = APP_SAMPLE_PIPE_MODE_VIDEO;
        if (APP_PIPE_COMBO_TYPE_TS == pThreadParam->ePipeComboType) {
            /* Auto mode : Dispatch frame by devFlag*/
            FrmType = tImg[0].tIspInfo.tFtcInfo.eFrmType;

            if (APP_SAMPLE_PIPE_MODE_FLASH_SNAP == FrmType) {
                m_TSTriggerInfoArr[m_TSInfoIndex].nSeqNum = tImg[0].tFrameInfo.stVFrame.u64SeqNum;
                m_TSTriggerInfoArr[m_TSInfoIndex].nPts = tImg[0].tFrameInfo.stVFrame.u64PTS;
                m_TSTriggerInfoArr[m_TSInfoIndex].nUserData = tImg[0].tIspInfo.tFtcInfo.tFtcEventInfo[0].nUserData;
                tImg[0].tFrameInfo.stVFrame.u64UserData = (AX_U64)&m_TSTriggerInfoArr[m_TSInfoIndex];
                m_TSInfoIndex += 1;
                m_TSInfoIndex = m_TSInfoIndex % MAX_INFO_SIZE;

                AX_IMG_INFO_T* pImg = new (std::nothrow) AX_IMG_INFO_T;
                memcpy(pImg, tImg, sizeof(AX_IMG_INFO_T));
                if (!m_arrFrameQ[nSnsID].Push(pImg)) {
                    FrmType = APP_SAMPLE_PIPE_MODE_VIDEO;
                    SAFE_DELETE_PTR(pImg);
                    break;
                }
            } else {
                nRet = AX_VIN_SendRawFrame(APP_SAMPLE_PIPE_MODE_VIDEO, AX_VIN_FRAME_SOURCE_ID_IFE, eHdrMode, (const AX_IMG_INFO_T**)&tImg, 0);
                if (AX_SUCCESS != nRet) {
                    LOG_MM_E(SNS_MGR, "dev:%d, pipe[%d] AX_VIN_SendRawFrame failed, ret=0x%x.", nDevID, 0, nRet);
                    break;
                }
            }
        } else if (APP_PIPE_COMBO_TYPE_SS == pThreadParam->ePipeComboType) {
            /* Auto mode : Dispatch frame by devFlag*/
            FrmType = tImg[0].tIspInfo.tFtcInfo.eFrmType;

            if (APP_SAMPLE_PIPE_MODE_FLASH_SNAP == FrmType) {
                m_SSTriggerInfoArr[m_SSInfoIndex].nSeqNum = tImg[0].tFrameInfo.stVFrame.u64SeqNum;
                m_SSTriggerInfoArr[m_SSInfoIndex].nPts = tImg[0].tFrameInfo.stVFrame.u64PTS;
                m_SSTriggerInfoArr[m_SSInfoIndex].nUserData = tImg[0].tIspInfo.tFtcInfo.tFtcEventInfo[0].nUserData;
                tImg[0].tFrameInfo.stVFrame.u64UserData = (AX_U64)&m_SSTriggerInfoArr[m_SSInfoIndex];
                m_SSInfoIndex += 1;
                m_SSInfoIndex = m_SSInfoIndex % MAX_INFO_SIZE;

                AX_IMG_INFO_T* pImg = new (std::nothrow) AX_IMG_INFO_T;
                memcpy(pImg, tImg, sizeof(AX_IMG_INFO_T));
                if (!m_arrFrameQ[nSnsID].Push(pImg)) {
                    FrmType = APP_SAMPLE_PIPE_MODE_VIDEO;
                    SAFE_DELETE_PTR(pImg);
                    break;
                }
            } else if (APP_SAMPLE_PIPE_MODE_PICTURE == FrmType) {
                nRet = AX_VIN_SendRawFrame(APP_SAMPLE_PIPE_MODE_PICTURE, AX_VIN_FRAME_SOURCE_ID_IFE, eHdrMode, (const AX_IMG_INFO_T**)&tImg, 0);
                if (AX_SUCCESS != nRet) {
                    LOG_MM_E(SNS_MGR, "dev:%d, pipe[%d] AX_VIN_SendRawFrame failed, ret=0x%x.", nDevID, 1, nRet);
                    break;
                }
            } else {
                nRet = AX_VIN_SendRawFrame(APP_SAMPLE_PIPE_MODE_VIDEO, AX_VIN_FRAME_SOURCE_ID_IFE, eHdrMode, (const AX_IMG_INFO_T**)&tImg, 0);
                if (AX_SUCCESS != nRet) {
                    LOG_MM_E(SNS_MGR, "dev:%d, pipe[%d] AX_VIN_SendRawFrame failed, ret=0x%x.", nDevID, 0, nRet);
                    break;
                }
            }
        } else if (APP_PIPE_COMBO_TYPE_NORMAL == pThreadParam->ePipeComboType) {
            /* Manual mode : Dispatch frame by frame rate*/
            for (auto& m : vecTargetPipeFramerate) {
                AX_U8 nPipe = m.nPipeID;
                AX_BOOL bSnapshot = m.bSnapshot;
                if (!mapPipe2FrmCtrl[nPipe].get()->FramerateCtrl()) {
                    /* Snapshot pipe frames will send raw frames with manual AE parameters in user mode */
                    if (bSnapshot) {
                        FrmType = APP_SAMPLE_PIPE_MODE_FLASH_SNAP;
                        AX_IMG_INFO_T* pImg = new (std::nothrow) AX_IMG_INFO_T;
                        memcpy(pImg, tImg, sizeof(AX_IMG_INFO_T));
                        if (!m_arrFrameQ[nSnsID].Push(pImg)) {
                            FrmType = APP_SAMPLE_PIPE_MODE_VIDEO;
                            SAFE_DELETE_PTR(pImg);
                            break;
                        }
                    } else {
                        nRet = AX_VIN_SendRawFrame(nPipe, AX_VIN_FRAME_SOURCE_ID_IFE, eHdrMode, (const AX_IMG_INFO_T**)&tImg, 0);
                        if (AX_SUCCESS != nRet) {
                            LOG_MM_E(SNS_MGR, "dev:%d, pipe[%d] AX_VIN_SendRawFrame failed, ret=0x%x.", nDevID, nPipe, nRet);
                            break;
                        }
                    }
                    break;
                }
            }
        } else if (APP_PIPE_COMBO_TYPE_TS_MODE0 == pThreadParam->ePipeComboType) {
            /* Manual mode : Dispatch frame by frame rate*/
            for (auto& m : vecTargetPipeFramerate) {
                AX_U8 nPipe = m.nPipeID;
                AX_BOOL bSnapshot = m.bSnapshot;
                if (!mapPipe2FrmCtrl[nPipe].get()->FramerateCtrl()) {
                    /* Snapshot pipe frames will send raw frames with manual AE parameters in user mode */
                    if (bSnapshot) {
                        FrmType = APP_SAMPLE_PIPE_MODE_FLASH_SNAP;
                        AX_IMG_INFO_T* pImg = new (std::nothrow) AX_IMG_INFO_T;
                        memcpy(pImg, tImg, sizeof(AX_IMG_INFO_T));
                        nRet = AX_VIN_SendRawFrame(nPipe - 1, AX_VIN_FRAME_SOURCE_ID_IFE, eHdrMode, (const AX_IMG_INFO_T**)&tImg, 0);
                        if (AX_SUCCESS != nRet) {
                            LOG_MM_E(SNS_MGR, "dev:%d, snap send raw frame to pipe[%d] failed, ret=0x%x.", nDevID, nPipe - 1, nRet);
                            break;
                        }
                        if (!m_arrFrameQ[nSnsID].Push(pImg)) {
                            FrmType = APP_SAMPLE_PIPE_MODE_VIDEO;
                            SAFE_DELETE_PTR(pImg);
                            break;
                        }
                    } else {
                        nRet = AX_VIN_SendRawFrame(nPipe, AX_VIN_FRAME_SOURCE_ID_IFE, eHdrMode, (const AX_IMG_INFO_T**)&tImg, 0);
                        if (AX_SUCCESS != nRet) {
                            LOG_MM_E(SNS_MGR, "dev:%d, pipe[%d] AX_VIN_SendRawFrame failed, ret=0x%x.", nDevID, nPipe, nRet);
                            break;
                        }
                    }
                    break;
                }
            }
        } else if (APP_PIPE_COMBO_TYPE_TS_MODE1 == pThreadParam->ePipeComboType) {
            for (auto& m : vecTargetPipeFramerate) {
                AX_U8 nPipe = m.nPipeID;
                AX_BOOL bSnapshot = m.bSnapshot;
                if (!mapPipe2FrmCtrl[nPipe].get()->FramerateCtrl()) {
                    if (bSnapshot) {
                        continue;
                    }
                    nRet = AX_VIN_SendRawFrame(nPipe, AX_VIN_FRAME_SOURCE_ID_IFE, eHdrMode, (const AX_IMG_INFO_T**)&tImg, 0);
                    if (AX_SUCCESS != nRet) {
                        LOG_MM_E(SNS_MGR, "dev:%d, pipe[%d] AX_VIN_SendRawFrame failed, ret=0x%x.", nDevID, nPipe, nRet);
                        break;
                    }
                    break;
                }
            }
        } else if (APP_PIPE_COMBO_TYPE_TS_MODE2 == pThreadParam->ePipeComboType) {
            /* Manual mode : Dispatch frame by frame rate*/
            auto vec = vecTargetPipeFramerate;
            vec.insert(vec.begin(), vec.back());
            vec.pop_back();
            for (auto& m : vec) {
                AX_U8 nPipe = m.nPipeID;
                AX_BOOL bSnapshot = m.bSnapshot;
                if (!mapPipe2FrmCtrl[nPipe].get()->FramerateCtrl()) {
                    /* Snapshot pipe frames will send raw frames with manual AE parameters in user mode */
                    if (bSnapshot) {
                        if (pThreadParam->nFrameCount == ceil(fDevFramerate)) {
                            pThreadParam->nFrameCount = 0;
                        }

                        pThreadParam->nFrameCount++;

                        if((pThreadParam->nFrameCount > pThreadParam->nContinuousFrameCount) && (pThreadParam->nFrameCount < ceil(fDevFramerate))) {
                            continue;
                        }

                        FrmType = APP_SAMPLE_PIPE_MODE_FLASH_SNAP;
                        AX_IMG_INFO_T* pImg = new (std::nothrow) AX_IMG_INFO_T;
                        memcpy(pImg, tImg, sizeof(AX_IMG_INFO_T));
                        nRet = AX_VIN_SendRawFrame(nPipe - 1, AX_VIN_FRAME_SOURCE_ID_IFE, eHdrMode, (const AX_IMG_INFO_T**)&tImg, 0);
                        if (AX_SUCCESS != nRet) {
                            LOG_MM_E(SNS_MGR, "dev:%d, snap send raw frame to pipe[%d] failed, ret=0x%x.", nDevID, nPipe - 1, nRet);
                            break;
                        }

                        if (!m_arrFrameQ[nSnsID].Push(pImg)) {
                            FrmType = APP_SAMPLE_PIPE_MODE_VIDEO;
                            SAFE_DELETE_PTR(pImg);
                            break;
                        }
                    } else {
                        nRet = AX_VIN_SendRawFrame(nPipe, AX_VIN_FRAME_SOURCE_ID_IFE, eHdrMode, (const AX_IMG_INFO_T**)&tImg, 0);
                        if (AX_SUCCESS != nRet) {
                            LOG_MM_E(SNS_MGR, "dev:%d, pipe[%d] AX_VIN_SendRawFrame failed, ret=0x%x.", nDevID, nPipe, nRet);
                            break;
                        }
                    }
                    break;
                }
            }
        } else {
            LOG_MM_E(SNS_MGR, "unsupported pipe combo type %d", pThreadParam->ePipeComboType);
            break;
        }

        if (APP_SAMPLE_PIPE_MODE_FLASH_SNAP != FrmType) {
            for (AX_S32 i = 0; i < eHdrMode; i++) {
                nRet = AX_VIN_ReleaseDevFrame(nDevID, (AX_SNS_HDR_FRAME_E)i, tImg + i);
                if (AX_SUCCESS != nRet) {
                    LOG_MM_E(SNS_MGR, "[%d] AX_VIN_ReleaseDevFrame failed, ret=0x%x.", nDevID, nRet);
                    continue;
                } else {
                    LOG_MM_D(SNS_MGR, "[%lld] Release dev frame, hdrframe=%d, seq=%lld FrmType:%d.", nSeq, i,
                             tImg[i].tFrameInfo.stVFrame.u64SeqNum, FrmType);
                }
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
        if (m_mapYuvThreadParams[nSnsID][1].bPaused) {
            CElapsedTimer::GetInstance()->mSleep(10);
            continue;
        }

        if (!m_bGetYuvFlag[nSnsID]) {
            CElapsedTimer::GetInstance()->mSleep(10);
            continue;
        }

        if (!m_mapYuvThreadParams[nPipe][nChn].bEnable) {
            CElapsedTimer::GetInstance()->mSleep(16);
            continue;
        }

        AX_IMG_INFO_T* pVinImg = new (std::nothrow) AX_IMG_INFO_T();
        if (nullptr == pVinImg) {
            LOG_MM_E(SNS_MGR, "Allocate buffer for YuvGetThread failed.");
            CElapsedTimer::GetInstance()->mSleep(10);
            continue;
        }

        nRet = AX_VIN_GetYuvFrame(nPipe, (AX_VIN_CHN_ID_E)nChn, pVinImg, 1000);
        //LOG_MM_E(SNS_MGR, "USERDATA =0x%x", pVinImg->tFrameInfo.stVFrame.u64UserData);
        if (AX_SUCCESS != nRet) {
            if (pThreadParam->bThreadRunning) {
                LOG_MM_E(SNS_MGR, "[%d][%d] AX_VIN_GetYuvFrame failed, ret=0x%x, unreleased buffer=%d", nPipe, nChn, nRet,
                         m_qFrame[nPipe][nChn].size());
            }
            SAFE_DELETE_PTR(pVinImg);
            continue;
        }

        if (m_mapYuvThreadParams[nPipe][nChn].bDiscard) {
            CPrintHelper::GetInstance()->Add(E_PH_MOD_VIN_YUV, nPipe, nChn);
            AX_VIN_ReleaseYuvFrame(nPipe, (AX_VIN_CHN_ID_E)nChn, pVinImg);
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

AX_VOID CSensorMgr::IFERawDumpThreadFunc(AX_VOID* pArg) {
    AX_S32 nRet = -1;
    AX_U8 nIndex = *(AX_U8 *)pArg;

    if (nIndex >= MAX_SENSOR_COUNT) {
        LOG_MM_E(SNS_MGR, "Thread index(%d) is invalid", nIndex);
        return;
    }

    AX_U8 nSnapPipeId = m_tDumpRawParam[nIndex].nSnapPipeId;
    AX_U8 nDumpPipeID = m_tDumpRawParam[nIndex].nPipeID;
    AX_IMG_INFO_T *pDumpImg = nullptr;
    LOG_MM_I(SNS_MGR, "nSnapPipeId: %d, org dump pipe id: %d", nSnapPipeId, m_tDumpRawParam[nIndex].nPipeID);

    while (m_IFERawDumpThread[nIndex].IsRunning()) {
        {
            std::unique_lock<std::mutex> lck(m_mtxDumpRawSwitch);

            nDumpPipeID = m_tDumpRawParam[nIndex].nPipeID;
        }

        if ((nDumpPipeID == INVALID_PIPE_ID) || (nDumpPipeID == nSnapPipeId)) {
            CElapsedTimer::GetInstance()->mSleep(10);
            continue;
        }

        pDumpImg = new (std::nothrow) AX_IMG_INFO_T();
        if (nullptr == pDumpImg) {
            continue;
        }

        {
            std::unique_lock<std::mutex> lck(m_mtxDumpRawSwitch);

            nDumpPipeID = m_tDumpRawParam[nIndex].nPipeID;

            nRet = AX_VIN_GetRawFrame(nDumpPipeID, AX_VIN_PIPE_DUMP_NODE_IFE, AX_SNS_HDR_FRAME_L, pDumpImg, 300);
            if (AX_SUCCESS != nRet) {
                LOG_MM_C(SNS_MGR, "Pipe[%d] AX_VIN_GetRawFrame failed, ret=0x%x.", nDumpPipeID, nRet);
                SAFE_DELETE_PTR(pDumpImg);
                continue;
            }
        }

        // set pipe id to u64UserData
        pDumpImg->tFrameInfo.stVFrame.u64UserData = (AX_U64)nDumpPipeID;
        AX_POOL_IncreaseRefCnt(pDumpImg->tFrameInfo.stVFrame.u32BlkId[0]);
        if (!m_VecDumpRawFrameQ[nIndex]->Push(pDumpImg)) {
            AX_POOL_DecreaseRefCnt(pDumpImg->tFrameInfo.stVFrame.u32BlkId[0]);
            SAFE_DELETE_PTR(pDumpImg);
        }

        nRet = AX_VIN_ReleaseRawFrame(nDumpPipeID, AX_VIN_PIPE_DUMP_NODE_IFE, AX_SNS_HDR_FRAME_L, pDumpImg);
        if (AX_SUCCESS != nRet) {
            LOG_MM_C(SNS_MGR, "Pipe[%d] AX_VIN_ReleaseRawFrame failed, ret=0x%x.", nDumpPipeID, nRet);
        }
    }
}

AX_VOID CSensorMgr::IFEDumpRawQDispatchFunc(AX_VOID* pArg) {
    AX_U8 nIndex = *(AX_U8 *)pArg;

    if (nIndex >= MAX_SENSOR_COUNT) {
        LOG_MM_E(SNS_MGR, "Thread index(%d) is invalid", nIndex);
        return;
    }

    AX_U8 nPrePipe = INVALID_PIPE_ID;
    AX_IMG_INFO_T* pImg{nullptr};

    while (m_IFEDumpRawDispatchThread[nIndex].IsRunning()) {
        if (!m_VecDumpRawFrameQ[nIndex]->Pop(pImg, -1)) {
            CElapsedTimer::GetInstance()->mSleep(1);
            continue;
        }

        AX_U8 nCurPipe = (AX_U8)pImg->tFrameInfo.stVFrame.u64UserData;
        if (nCurPipe != nPrePipe) {
            CPrintHelper::GetInstance()->Reset(E_PH_MOD_VIN_RAW, nPrePipe);
            LOG_MM(SNS_MGR, "Queue change pipe [%d] -> [%d]", nPrePipe, nCurPipe);
            nPrePipe = nCurPipe;
        }

        AX_U32 nWaitTime = m_vecSensorIns[0]->GetSnsConfig().arrPipeAttr[nCurPipe].tIFEDumpInfo.nDumpThreadWaitTime;
        CElapsedTimer::GetInstance()->mSleep(nWaitTime);

        CPrintHelper::GetInstance()->Add(E_PH_MOD_VIN_RAW, nCurPipe);

        AX_POOL_DecreaseRefCnt(pImg->tFrameInfo.stVFrame.u32BlkId[0]);

        SAFE_DELETE_PTR(pImg);
    }
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

AX_F32 CSensorMgr::GetPipeFrameRate(AX_U8 nPipeID) {
    for (AX_U8 i = 0; i < GetSensorCount(); i++) {
        CBaseSensor* pSensor = GetSnsInstance(i);
        AX_U32 nPipeCount = pSensor->GetPipeCount();
        for (AX_U8 j = 0; j < nPipeCount; j++) {
            if (nPipeID == pSensor->GetSnsConfig().arrPipeAttr[j].nPipeID) {
                return pSensor->GetSnsConfig().arrPipeAttr[j].fPipeFramerate;
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

AX_BOOL CSensorMgr::SnapshotProcess(AX_U8 nPipe, AX_U8 nChannel, AX_SNS_HDR_MODE_E eHdrMode, const AX_IMG_INFO_T** pArrImgInfo,
                                    AX_BOOL bFirstCap) {
    AX_S32 nRet = AX_SUCCESS;
    AX_IMG_INFO_T t1stYuvFrame = {0};

    AX_U8 nPrevPipe = 0;
    for (auto pSensor : m_vecSensorIns) {
        SENSOR_CONFIG_T tSnsCfg = pSensor->GetSnsConfig();
        for (AX_U8 i = 0; i < pSensor->GetPipeCount(); i++) {
            AX_U8 nPipeID = tSnsCfg.arrPipeAttr[i].nPipeID;
            if (nPipe == nPipeID) {
                nPrevPipe = tSnsCfg.arrPipeAttr[0].nPipeID;
                break;
            }
        }
    }

    AX_IMG_INFO_T *pRawImgInfo = (AX_IMG_INFO_T *)pArrImgInfo;

    AX_ISP_IQ_AE_PARAM_T tAeParam = {0};
    AX_ISP_IQ_RLTM_PARAM_T tUserCaptureFrameRltmParam = {0};
    AX_ISP_IQ_SCENE_PARAM_T tUserCaptureSceneParam = {0};

    nRet = AX_ISP_IQ_GetAeParam(nPipe, &tAeParam);
    if (AX_SUCCESS != nRet) {
        LOG_MM_E(SNS_MGR, "AX_ISP_IQ_GetAeParam failed, ret=0x%x.", nRet);
        return AX_FALSE;
    }

    tAeParam.nEnable = AX_FALSE;
    tAeParam.tExpManual.nAGain = pRawImgInfo->tIspInfo.tExpInfo.nAgain;
    tAeParam.tExpManual.nDgain = pRawImgInfo->tIspInfo.tExpInfo.nDgain;
    tAeParam.tExpManual.nHcgLcg = pRawImgInfo->tIspInfo.tExpInfo.nHcgLcgMode;
    tAeParam.tExpManual.nHdrRatio = pRawImgInfo->tIspInfo.tExpInfo.nHdrRatio;
    tAeParam.tExpManual.nShutter = pRawImgInfo->tIspInfo.tExpInfo.nIntTime;
    tAeParam.tExpManual.nIspGain = pRawImgInfo->tIspInfo.tExpInfo.nIspGain;

    nRet = AX_ISP_IQ_SetAeParam(nPipe, &tAeParam);
    if (AX_SUCCESS != nRet) {
        LOG_MM_E(SNS_MGR, "[%d] AX_ISP_IQ_SetAeParam failed, ret=0x%x.", nPipe, nRet);
        return AX_FALSE;
    }
    LOG_M_I(SNS_MGR, "nPrevPipe:%d, nPipe:%d, again:%d, dgain:%d, hcglcg:%d, hdrratio:%d, shutter:%d, ispgain:%d", nPrevPipe, nPipe, tAeParam.tExpManual.nAGain, tAeParam.tExpManual.nDgain,\
                    tAeParam.tExpManual.nHcgLcg, tAeParam.tExpManual.nHdrRatio, tAeParam.tExpManual.nShutter, tAeParam.tExpManual.nIspGain);

    nRet = AX_ISP_IQ_GetRltmParam(nPrevPipe, &tUserCaptureFrameRltmParam);
    if (AX_SUCCESS != nRet) {
        LOG_MM_E(SNS_MGR, "[%d] AX_ISP_IQ_GetRltmParam failed, ret=0x%x", nPrevPipe, nRet);
        return AX_FALSE;
    }
    tUserCaptureFrameRltmParam.tTempoFilter.nReset = 1;
    nRet = AX_ISP_IQ_SetRltmParam(nPipe, &tUserCaptureFrameRltmParam);
    if (AX_SUCCESS != nRet) {
        LOG_MM_E(SNS_MGR, "[%d] AX_ISP_IQ_SetRltmParam failed, ret=0x%x", nPipe, nRet);
        return AX_FALSE;
    }

    /* 1. first send raw frame*/
    if (bFirstCap) {
        nRet = AX_ISP_IQ_GetSceneParam(nPipe, &tUserCaptureSceneParam);
        tUserCaptureSceneParam.nAutoMode = AX_FALSE;
        if (tUserCaptureSceneParam.tAutoParam.nSceneNum == 2) {
            /* use T2DNR when first cap & first frame */
            tUserCaptureSceneParam.tManualParam.nTnrWorkMode = tUserCaptureSceneParam.tAutoParam.nTnrWorkMode[1];
            tUserCaptureSceneParam.tManualParam.nAiWorkMode = tUserCaptureSceneParam.tAutoParam.nAiWorkMode[1];
        } else {
            LOG_MM_E(SNS_MGR, "AX_ISP_IQ_GetSceneParam invalid, nSceneNum=%d", tUserCaptureSceneParam.tAutoParam.nSceneNum);
        }
        nRet = AX_ISP_IQ_SetSceneParam(nPipe, &tUserCaptureSceneParam);
        if (0 != nRet) {
            LOG_MM_E(SNS_MGR, "AX_ISP_IQ_SetSceneParam failed, ret=0x%x", nRet);
        }
    }

    nRet = AX_ISP_RunOnce(nPipe);
    if (AX_SUCCESS != nRet) {
        LOG_MM_E(SNS_MGR, "[%d] AX_ISP_RunOnce failed, ret=0x%x.", nPipe, nRet);
    }

    nRet = AX_VIN_SendRawFrame(nPipe, AX_VIN_FRAME_SOURCE_ID_IFE, eHdrMode, pArrImgInfo, 0);
    if (AX_SUCCESS != nRet) {
        LOG_MM_E(SNS_MGR, "[%d] AX_VIN_SendRawFrame failed, ret=0x%x.", nPipe, nRet);
        return AX_FALSE;
    } else {
        LOG_M_D(SNS_MGR, "Send snapshot raw to IFE pipe %d.", nPipe);
    }

    nRet = AX_VIN_GetYuvFrame(nPipe, (AX_VIN_CHN_ID_E)nChannel, &t1stYuvFrame, 3000);
    if (AX_SUCCESS != nRet) {
        LOG_MM_E(SNS_MGR, "[%d][%d] AX_VIN_GetYuvFrame failed, ret=0x%x.", nPipe, nChannel, nRet);
        return AX_FALSE;
    }

    AX_VIN_ReleaseYuvFrame(nPipe, (AX_VIN_CHN_ID_E)nChannel, &t1stYuvFrame);

    /* 2. second send raw frame*/
    if (bFirstCap) {
        nRet = AX_ISP_IQ_GetSceneParam(nPipe, &tUserCaptureSceneParam);
        tUserCaptureSceneParam.nAutoMode = AX_FALSE;
        if (tUserCaptureSceneParam.tAutoParam.nSceneNum == 2) {
            /* use AINR when first cap & second frame */
            tUserCaptureSceneParam.tManualParam.nTnrWorkMode = tUserCaptureSceneParam.tAutoParam.nTnrWorkMode[0];
            tUserCaptureSceneParam.tManualParam.nAiWorkMode = tUserCaptureSceneParam.tAutoParam.nAiWorkMode[0];
        } else {
            LOG_MM_E(SNS_MGR, "AX_ISP_IQ_GetSceneParam invalid, nSceneNum=%d", tUserCaptureSceneParam.tAutoParam.nSceneNum);
        }
        nRet = AX_ISP_IQ_SetSceneParam(nPipe, &tUserCaptureSceneParam);
        if (0 != nRet) {
            LOG_MM_E(SNS_MGR, "AX_ISP_IQ_SetSceneParam failed, ret=0x%x", nRet);
        }
    }

    nRet = AX_ISP_RunOnce(nPipe);
    if (AX_SUCCESS != nRet) {
        LOG_MM_E(SNS_MGR, "[%d] AX_ISP_RunOnce failed, ret=0x%x.", nPipe, nRet);
    }

    nRet = AX_VIN_SendRawFrame(nPipe, AX_VIN_FRAME_SOURCE_ID_IFE, eHdrMode, pArrImgInfo, 0);
    if (AX_SUCCESS != nRet) {
        LOG_MM_E(SNS_MGR, "[%d] AX_VIN_SendRawFrame failed, ret=0x%x.", nPipe, nRet);
        return AX_FALSE;
    } else {
        LOG_M_D(SNS_MGR, "Send snapshot raw to IFE pipe %d.", nPipe);
    }

#if 0
    nRet = AX_VIN_GetYuvFrame(nPipe, (AX_VIN_CHN_ID_E)nChannel, pVinImg, 3000);
    if (AX_SUCCESS != nRet) {
        LOG_MM_E(SNS_MGR, "[%d][%d] AX_VIN_GetYuvFrame failed, ret=0x%x.", nPipe, nChannel, nRet);
        SAFE_DELETE_PTR(pVinImg);
        return AX_FALSE;
    }

    CAXFrame* pAXFrame = new (std::nothrow) CAXFrame();
    pAXFrame->nGrp = nPipe;
    pAXFrame->nChn = nChannel;
    pAXFrame->stFrame.stVFrame = pVinImg->tFrameInfo;
    pAXFrame->pFrameRelease = this;
    pAXFrame->pUserDefine = pVinImg;
    pAXFrame->bMultiplex = AX_FALSE;

    m_mtxFrame[nPipe][nChannel].lock();
    if (m_qFrame[nPipe][nChannel].size() >= 5) {
        LOG_MM_W(SNS_MGR, "[%d][%d] queue size is %d, drop this frame", nPipe, nChannel, m_qFrame[nPipe][nChannel].size());
        AX_VIN_ReleaseYuvFrame(nPipe, (AX_VIN_CHN_ID_E)nChannel, pVinImg);
        SAFE_DELETE_PTR(pVinImg);
        SAFE_DELETE_PTR(pAXFrame);

        m_mtxFrame[nPipe][nChannel].unlock();
        return AX_FALSE;
    }

    m_qFrame[nPipe][nChannel].push_back(pAXFrame);
    m_mtxFrame[nPipe][nChannel].unlock();

    NotifyAll(nPipe, nChannel, pAXFrame);
#endif

    return AX_TRUE;
}

AX_BOOL CSensorMgr::SnapshotProcess_Offline_NPU(AX_U8 nDevId, AX_U8 nPipe, AX_U8 nChannel, AX_SNS_HDR_MODE_E eHdrMode, const AX_IMG_INFO_T** pArrImgInfo, AX_BOOL bDummy, AX_U8 nIFEDumpIndex) {
    AX_S32 nRet = AX_SUCCESS;
    AX_IMG_INFO_T *pRawImgInfo = (AX_IMG_INFO_T *)pArrImgInfo;

    AX_ISP_IQ_AE_PARAM_T tAeParam = {0};
    AX_ISP_IQ_RLTM_PARAM_T tUserCaptureFrameRltmParam = {0};
    AX_IMG_INFO_T frameBufferArrIfe[AX_SNS_HDR_FRAME_MAX] = {0};
    AX_IMG_INFO_T frameBufferArrAinr[AX_SNS_HDR_FRAME_MAX] = {0};
    AX_S32 timeOutMs = 1000;

    nRet = AX_ISP_IQ_GetAeParam(nPipe, &tAeParam);
    if (AX_SUCCESS != nRet) {
        LOG_MM_E(SNS_MGR, "AX_ISP_IQ_GetAeParam failed, ret=0x%x.", nRet);
        return AX_FALSE;
    }

    tAeParam.nEnable = AX_FALSE;
    tAeParam.tExpManual.nAGain = pRawImgInfo->tIspInfo.tExpInfo.nAgain;
    tAeParam.tExpManual.nDgain = pRawImgInfo->tIspInfo.tExpInfo.nDgain;
    tAeParam.tExpManual.nHcgLcg = pRawImgInfo->tIspInfo.tExpInfo.nHcgLcgMode;
    tAeParam.tExpManual.nHdrRatio = pRawImgInfo->tIspInfo.tExpInfo.nHdrRatio;
    tAeParam.tExpManual.nShutter = pRawImgInfo->tIspInfo.tExpInfo.nIntTime;
    tAeParam.tExpManual.nIspGain = pRawImgInfo->tIspInfo.tExpInfo.nIspGain;

    nRet = AX_ISP_IQ_SetAeParam(nPipe, &tAeParam);
    if (AX_SUCCESS != nRet) {
        LOG_MM_E(SNS_MGR, "[%d] AX_ISP_IQ_SetAeParam failed, ret=0x%x.", nPipe, nRet);
        return AX_FALSE;
    }

    LOG_M_I(SNS_MGR, "nPipe:%d, again:%d, dgain:%d, hcglcg:%d, hdrratio:%d, shutter:%d, ispgain:%d", nPipe, tAeParam.tExpManual.nAGain, tAeParam.tExpManual.nDgain,\
                    tAeParam.tExpManual.nHcgLcg, tAeParam.tExpManual.nHdrRatio, tAeParam.tExpManual.nShutter, tAeParam.tExpManual.nIspGain);

    nRet = AX_ISP_IQ_GetRltmParam(nPipe, &tUserCaptureFrameRltmParam);
    if (AX_SUCCESS != nRet) {
        LOG_MM_E(SNS_MGR, "[%d] AX_ISP_IQ_GetRltmParam failed, ret=0x%x", nPipe, nRet);
        return AX_FALSE;
    }
    tUserCaptureFrameRltmParam.tTempoFilter.nReset = 1;
    nRet = AX_ISP_IQ_SetRltmParam(nPipe, &tUserCaptureFrameRltmParam);
    if (AX_SUCCESS != nRet) {
        LOG_MM_E(SNS_MGR, "[%d] AX_ISP_IQ_SetRltmParam failed, ret=0x%x", nPipe, nRet);
        return AX_FALSE;
    }

    /* 1. Run once calc param */
    nRet = AX_ISP_RunOnce(nPipe);
    if (AX_SUCCESS != nRet) {
        LOG_MM_E(SNS_MGR, "[%d] AX_ISP_RunOnce failed, ret=0x%x.", nPipe, nRet);
    }

    /* 2. Send raw frame to ife */
    nRet = AX_VIN_SendRawFrame(nPipe, AX_VIN_FRAME_SOURCE_ID_IFE, eHdrMode, pArrImgInfo, 0);
    if (AX_SUCCESS != nRet) {
        LOG_MM_E(SNS_MGR, "[%d] AX_VIN_SendRawFrame failed, ret=0x%x.", nPipe, nRet);
        return AX_FALSE;
    } else {
        LOG_M_D(SNS_MGR, "Send snapshot raw to IFE pipe %d.", nPipe);
    }

    /* 3. get raw frame from ife */
    nRet = AX_VIN_GetRawFrame(nPipe, AX_VIN_PIPE_DUMP_NODE_IFE, AX_SNS_HDR_FRAME_L, frameBufferArrIfe, timeOutMs);
    if (AX_SUCCESS != nRet) {
        if (AX_ERR_VIN_RES_EMPTY == nRet) {
            LOG_MM_E(SNS_MGR, "nonblock error, ret=0x%x", nRet);
            return AX_FALSE;
        }
        LOG_MM_E(SNS_MGR, "AX_VIN_GetRawFrame failed, ret=0x%x", nRet);
        usleep(10 * 1000);
        AX_VIN_ReleaseRawFrame(nPipe, AX_VIN_PIPE_DUMP_NODE_IFE, AX_SNS_HDR_FRAME_L, frameBufferArrIfe);
        return AX_FALSE;
    }

    if ((nIFEDumpIndex < MAX_SENSOR_COUNT) && (nPipe == m_tDumpRawParam[nIFEDumpIndex].nPipeID)) {
        AX_IMG_INFO_T* pImg = new (std::nothrow) AX_IMG_INFO_T;
        memcpy(pImg, frameBufferArrIfe, sizeof(AX_IMG_INFO_T));

        // set pipe id to u64UserData
        pImg->tFrameInfo.stVFrame.u64UserData = (AX_U64)nPipe;
        AX_POOL_IncreaseRefCnt(pImg->tFrameInfo.stVFrame.u32BlkId[0]);
        if (!m_VecDumpRawFrameQ[nIFEDumpIndex]->Push(pImg)) {
            AX_POOL_DecreaseRefCnt(pImg->tFrameInfo.stVFrame.u32BlkId[0]);
            SAFE_DELETE_PTR(pImg);
        }
    }

    /* 4. Send the raw frame obtained from IFE to ITP */
    nRet = AX_VIN_SendRawFrame(nPipe, AX_VIN_FRAME_SOURCE_ID_ITP, eHdrMode, (const AX_IMG_INFO_T **)&frameBufferArrIfe, 0);
    if (nRet != 0) {
        LOG_MM_E(SNS_MGR, "AX_VIN_SendRawFrame failed, nRet=0x%x\n", nRet);
    }

    /* 5. Send the raw frame obtained from IFE to AINR */
    nRet = AX_VIN_SendRawFrame(nPipe, AX_VIN_FRAME_SOURCE_ID_AINR, eHdrMode, (const AX_IMG_INFO_T **)&frameBufferArrIfe, 0);
    if (nRet != 0) {
        LOG_MM_E(SNS_MGR, "AX_VIN_SendRawFrame failed, nRet=0x%x\n", nRet);
    }

    /* 6. Release BLK obtained from IFE */
    nRet = AX_VIN_ReleaseRawFrame(nPipe, AX_VIN_PIPE_DUMP_NODE_IFE, AX_SNS_HDR_FRAME_L, frameBufferArrIfe);
    if (nRet != 0) {
        LOG_MM_E(SNS_MGR, "AX_VIN_ReleaseRawFrame failed, nRet=0x%x\n", nRet);
    }

    /* 7. Wait frame done interrupt */
    nRet = AX_ISP_GetIrqTimeOut(nPipe, AX_IRQ_TYPE_FRAME_DONE, timeOutMs);
    if (nRet != 0) {
        LOG_MM_E(SNS_MGR,"AX_ISP_GetIrqTimeOut failed\n");
    }

    /* 8. Second Runonce */
    nRet = AX_ISP_RunOnce(nPipe);
    if (nRet != 0) {
        LOG_MM_E(SNS_MGR,"AX_ISP_RunOnce failed\n");
    }

    /* 9. get raw frame from NPU */
    nRet = AX_VIN_GetRawFrame(nPipe, AX_VIN_PIPE_DUMP_NODE_AINR, AX_SNS_HDR_FRAME_L, frameBufferArrAinr, -1);
    if (nRet != 0) {
        LOG_MM_E(SNS_MGR,"Get Pipe raw frame failed\n");
    }

    /* 10. Send the raw frame obtained from NPU to ITP */
    nRet = AX_VIN_SendRawFrame(nPipe, AX_VIN_FRAME_SOURCE_ID_ITP, eHdrMode, (const AX_IMG_INFO_T **)&frameBufferArrAinr, 0);
    if (nRet != 0) {
        LOG_MM_E(SNS_MGR,"Send Pipe raw frame failed\n");
    }
    /* 11. Wait frame done interrupt */
    nRet = AX_ISP_GetIrqTimeOut(nPipe, AX_IRQ_TYPE_FRAME_DONE, timeOutMs);
    if (nRet != 0) {
        LOG_MM_E(SNS_MGR,"AX_ISP_GetIrqTimeOut failed\n");
    }

    /* 12. Release BLK obtained from NPU */
    nRet = AX_VIN_ReleaseRawFrame(nPipe, AX_VIN_PIPE_DUMP_NODE_AINR, AX_SNS_HDR_FRAME_L, frameBufferArrAinr);
    if (nRet != 0) {
        LOG_MM_E(SNS_MGR,"AX_VIN_ReleaseRawFrame failed\n");
    }

    return AX_TRUE;
}

AX_BOOL CSensorMgr::NotifySnapshotProcess(AX_U8 nPipe, AX_U8 nChannel) {
    AX_S32 nRet = AX_SUCCESS;
    AX_IMG_INFO_T* pVinImg = new (std::nothrow) AX_IMG_INFO_T();
    nRet = AX_VIN_GetYuvFrame(nPipe, (AX_VIN_CHN_ID_E)nChannel, pVinImg, 3000);
    if (AX_SUCCESS != nRet) {
        LOG_MM_E(SNS_MGR, "[%d][%d] AX_VIN_GetYuvFrame failed, ret=0x%x.", nPipe, nChannel, nRet);
        SAFE_DELETE_PTR(pVinImg);
        return AX_FALSE;
    }

    CAXFrame* pAXFrame = new (std::nothrow) CAXFrame();
    pAXFrame->nGrp = nPipe;
    pAXFrame->nChn = nChannel;
    pAXFrame->stFrame.stVFrame = pVinImg->tFrameInfo;
    pAXFrame->pFrameRelease = this;
    pAXFrame->pUserDefine = pVinImg;
    pAXFrame->bMultiplex = AX_FALSE;

    m_mtxFrame[nPipe][nChannel].lock();
    if (m_qFrame[nPipe][nChannel].size() >= 5) {
        LOG_MM_W(SNS_MGR, "[%d][%d] queue size is %d, drop this frame", nPipe, nChannel, m_qFrame[nPipe][nChannel].size());
        AX_VIN_ReleaseYuvFrame(nPipe, (AX_VIN_CHN_ID_E)nChannel, pVinImg);
        SAFE_DELETE_PTR(pVinImg);
        SAFE_DELETE_PTR(pAXFrame);

        m_mtxFrame[nPipe][nChannel].unlock();
        return AX_FALSE;
    }

    m_qFrame[nPipe][nChannel].push_back(pAXFrame);
    m_mtxFrame[nPipe][nChannel].unlock();

    NotifyAll(nPipe, nChannel, pAXFrame);

    return AX_TRUE;
}

AX_VOID CSensorMgr::SwitchSnsMode(AX_U32 nSnsID, AX_U32 nSnsMode, AX_S32 nFps) {

    CBaseSensor* pCurSensor = GetSnsInstance(nSnsID);
    if (nullptr == pCurSensor) {
        return;
    }
    CBaseSensor* pVsSensor = nullptr;
    if (CCmdLineParser::GetInstance()->GetPPLIndex() == 0 && CCmdLineParser::GetInstance()->GetScenario() == 8) {
        pVsSensor = GetSnsInstance(1);
    }

    m_bGetYuvFlag[nSnsID] = AX_FALSE;

    Stop(pCurSensor);
    if (pVsSensor != nullptr) {
        Stop(pVsSensor);
    }


    pCurSensor->ChangeHdrMode(nSnsMode, nFps);
    AX_U8 nDevID = pCurSensor->GetSnsConfig().nDevID;
    m_mapDev2ThreadParam[nDevID].eHdrMode = (AX_SNS_HDR_MODE_E)nSnsMode;
    pCurSensor->Init();

    Start(pCurSensor);
    if (pVsSensor != nullptr) {
        Start(pVsSensor);
    }

    m_bGetYuvFlag[nSnsID] = AX_TRUE;
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

AX_VOID CSensorMgr::ClearQueue(AX_U32 nSnsID) {
    AX_S32 nRet = AX_SUCCESS;
    AX_IMG_INFO_T* pImg{nullptr};
    CBaseSensor* pCurSensor = GetSnsInstance(nSnsID);
    SENSOR_CONFIG_T tSnsCfg = pCurSensor->GetSnsConfig();

    while (m_arrFrameQ[nSnsID].GetCount() > 0) {
        if (m_arrFrameQ[nSnsID].Pop(pImg, 0)) {
            nRet = AX_VIN_ReleaseDevFrame(tSnsCfg.nDevID, AX_SNS_HDR_FRAME_L, pImg);
            if (AX_SUCCESS != nRet) {
                LOG_M_E(SNS_MGR, "[%d] AX_VIN_ReleaseDevFrame failed, ret=0x%x.", nSnsID, nRet);
            } else {
                LOG_M_D(SNS_MGR, "nSnsID:%d ,seq=%lld ReleaseDevFrame", nSnsID, pImg->tFrameInfo.stVFrame.u64SeqNum);
            }

            SAFE_DELETE_PTR(pImg);
        }
    }
    return;
}

AX_VOID CSensorMgr::ClearIFEDumpRawFrmQueue(AX_U8 nIndex) {
    AX_IMG_INFO_T* pImg{nullptr};

    if (nIndex >= MAX_SENSOR_COUNT) {
        LOG_M_E(SNS_MGR, "Index(%d) of ife dump raw dispatch queue is invalid.", nIndex);
        return;
    }

    while (m_VecDumpRawFrameQ[nIndex]->GetCount() > 0) {
        if (m_VecDumpRawFrameQ[nIndex]->Pop(pImg, 0)) {
            AX_POOL_DecreaseRefCnt(pImg->tFrameInfo.stVFrame.u32BlkId[0]);
            SAFE_DELETE_PTR(pImg);
        }
    }
    return;
}

AX_BOOL CSensorMgr::EnableEIS(AX_U32 nSnsID, AX_BOOL bEnable) {
    CBaseSensor* pCurSensor = GetSnsInstance(nSnsID);
    if (nullptr == pCurSensor) {
        LOG_MM_E(SNS_MGR, "Get sensor(%d) obj fail!", nSnsID);
        return AX_FALSE;
    }

    return pCurSensor->EnableEIS(bEnable);
}

AX_BOOL CSensorMgr::SwitchIFERawDumpPipe(AX_U32 nSnsID, AX_U8 nPipe) {
    AX_BOOL bRet = AX_FALSE;

    CBaseSensor* pCurSensor = GetSnsInstance(nSnsID);
    if (nullptr == pCurSensor) {
        LOG_MM_E(SNS_MGR, "Get sensor(%d) obj fail!", nSnsID);
        return AX_FALSE;
    }

    AX_U8 nIndex = 0;
    for (nIndex = 0; nIndex < MAX_SENSOR_COUNT; nIndex++) {
        if (m_tDumpRawParam[nIndex].nSnsID == nSnsID) {
            break;
        }
    }

    if (nIndex >= MAX_SENSOR_COUNT) {
        LOG_MM_E(SNS_MGR, "nIndex %d SnsId(%d) is invalid!", nIndex, nSnsID);
        return AX_FALSE;
    }

    std::unique_lock<std::mutex> lck(m_mtxDumpRawSwitch);
    if (pCurSensor->SwitchIFERawDumpPipe(nPipe)) {
        m_tDumpRawParam[nIndex].nPipeID = nPipe;
        bRet = AX_TRUE;
    }

    return bRet;
}

AX_BOOL CSensorMgr::EnableChn(AX_U8 nPipe, AX_U8 nChannel, AX_BOOL bEnable) {
    for (auto pSensor : m_vecSensorIns) {
        SENSOR_CONFIG_T tSnsCfg = pSensor->GetSnsConfig();
        for (AX_U8 i = 0; i < tSnsCfg.nPipeCount; i++) {
            if (tSnsCfg.arrPipeAttr[i].nPipeID == nPipe) {
                if (m_mapYuvThreadParams[nPipe][nChannel].bEnable == bEnable) {
                    return AX_TRUE;
                }
                m_mapYuvThreadParams[nPipe][nChannel].bEnable = bEnable;
                if (pSensor->EnableChn(nPipe, nChannel, bEnable)) {
                    CPrintHelper::GetInstance()->Reset(E_PH_MOD_VIN_YUV, nPipe, nChannel);
                    return AX_TRUE;
                } else {
                    m_mapYuvThreadParams[nPipe][nChannel].bEnable = bEnable ? AX_FALSE : AX_TRUE;
                    break;
                }
            }
        }
    }

    LOG_MM_E(SNS_MGR, "nPipe(%) is invalid, only support pipe0 currently!", nPipe);
    return AX_FALSE;
}

AX_BOOL CSensorMgr::ChangeShutterType(AX_U32 nSnsID, AX_U8 nShutterType) {
    CBaseSensor* pCurSensor = GetSnsInstance(nSnsID);
    if (nullptr == pCurSensor) {
        LOG_MM_E(SNS_MGR, "Get sensor(%d) obj failed", nSnsID);
        return AX_FALSE;
    }

    AX_BOOL bRet = pCurSensor->SetShutterAttr(pCurSensor->GetSnsConfig().nDevID, m_mapShutterMode[nSnsID][nShutterType]);
    if (bRet) {
        if (0 == nShutterType) {
            m_mapYuvThreadParams[nSnsID][1].bPaused = AX_TRUE;
        } else {
            m_mapYuvThreadParams[nSnsID][1].bPaused = AX_FALSE;
        }
    } else {
        LOG_MM_E(SNS_MGR, "SetShutterAttr(%d) failed", nSnsID);
    }
    return bRet;
}

AX_U8 CSensorMgr::GetDumpIFERawParamIndex(AX_U8 nSnsID) {
    AX_U8 nIndex = 0;
    for (nIndex = 0; nIndex < MAX_SENSOR_COUNT; nIndex++) {
        if (m_tDumpRawParam[nIndex].nSnsID == nSnsID) {
            break;
        }
    }

    if (nIndex >= MAX_SENSOR_COUNT) {
        LOG_MM_W(SNS_MGR, "SnsId(%d) is invalid!", nSnsID);
        return 0xFF;
    }

    return nIndex;
}

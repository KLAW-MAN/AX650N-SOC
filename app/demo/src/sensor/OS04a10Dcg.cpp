/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include "OS04a10Dcg.h"
#include "AppLogApi.h"
#include "CmdLineParser.h"

#define SENSOR "OS04a10Dcg"
#define SDRKEY "sdr"
#define HDRKEY "hdr"

COS04a10Dcg::COS04a10Dcg(SENSOR_CONFIG_T tSensorConfig) : COS04a10(tSensorConfig) {
}

COS04a10Dcg::~COS04a10Dcg(AX_VOID) {
}

AX_VOID COS04a10Dcg::InitSnsLibraryInfo(AX_VOID) {
    m_tSnsLibInfo.strObjName = "gSnsos04a10ObjDcg";
    m_tSnsLibInfo.strLibName = "libsns_os04a10.so";
}

AX_VOID COS04a10Dcg::InitDevAttr() {
    COS04a10::InitDevAttr();
    m_tDevAttr.eSnsOutputMode = m_tSnsCfg.eSnsOutputMode;
}

#if 1
AX_BOOL COS04a10Dcg::Open() {
    if (!OpenAll()) {
        return AX_FALSE;
    }

    if (m_tSnsCfg.arrPipeAttr[0].bEnableEIS) {
        EnableEIS(m_tSnsCfg.arrPipeAttr[0].bEISOn);
    }

    return AX_TRUE;
}

AX_BOOL COS04a10Dcg::Close() {
    if (!CloseAll()) {
        return AX_FALSE;
    }

    return AX_TRUE;
}

AX_BOOL COS04a10Dcg::OpenAll() {
    LOG_MM(SENSOR, "[Dev:%d, RxDev:%d] +++", m_tSnsCfg.nDevID, m_tSnsCfg.nRxDev);

    LOG_MM(SENSOR, "Sensor Attr => w:%d h:%d framerate:%.2f sensor mode:%d rawType:%d", m_tSnsAttr.nWidth, m_tSnsAttr.nHeight,
           m_tSnsAttr.fFrameRate, m_tSnsAttr.eSnsMode, m_tSnsAttr.eRawType);

    AX_S32 nRet = 0;
    AX_U8 nDevID = m_tSnsCfg.nDevID;
    AX_U8 nRxDev = m_tSnsCfg.nRxDev;

    // SNS reset sensor obj
#ifdef NOT_DCGVS_DCG
    for (auto &m : m_mapPipe2Attr) {
        AX_U8 nPipeID = m.first;
        if (!ResetSensorObj(nDevID, nPipeID)) {
            LOG_M_E(SENSOR, "ResetSensorObj failed, ret=0x%x.", nRet);
            return AX_FALSE;
        }
    }
#endif

#ifdef NOT_DCGVS_DCG
    // MIPI proc
    nRet = AX_MIPI_RX_SetLaneCombo(AX_LANE_COMBO_MODE_4);
    if (0 != nRet) {
        LOG_M_E(SENSOR, "AX_MIPI_RX_SetLaneCombo failed, ret=0x%x.", nRet);
        return AX_FALSE;
    }

    nRet = AX_MIPI_RX_SetAttr(nRxDev, &m_tMipiRxDev);
    if (0 != nRet) {
        LOG_M_E(SENSOR, "AX_MIPI_RX_SetAttr failed, ret=0x%x.", nRet);
        return AX_FALSE;
    }

    nRet = AX_MIPI_RX_Reset(nRxDev);
    if (0 != nRet) {
        LOG_M_E(SENSOR, "AX_MIPI_RX_Reset failed, ret=0x%x.", nRet);
        return AX_FALSE;
    }

    nRet = AX_MIPI_RX_Start(nRxDev);
    if (0 != nRet) {
        LOG_M_E(SENSOR, "AX_MIPI_RX_Start failed, ret=0x%x.", nRet);
        return AX_FALSE;
    }
#endif

    // DEV proc
    nRet = AX_VIN_CreateDev(nDevID, &m_tDevAttr);
    if (0 != nRet) {
        LOG_M_E(SENSOR, "AX_VIN_CreateDev failed, ret=0x%x.", nRet);
        return AX_FALSE;
    }

    nRet = AX_VIN_SetDevPrivateDataAttr(nDevID, &m_tPrivAttr);
    if (0 != nRet) {
        LOG_M_E(SENSOR, "AX_VIN_SetDevPrivateDataAttr failed, nRet=0x%x.\n", nRet);
        return AX_FALSE;
    }

    nRet = AX_VIN_SetDevAttr(nDevID, &m_tDevAttr);
    if (0 != nRet) {
        LOG_M_E(SENSOR, "AX_VIN_SetDevAttr failed, ret=0x%x.", nRet);
        return AX_FALSE;
    }

    AX_VIN_DEV_BIND_PIPE_T tDevBindPipe = {0};
    for (AX_U8 i = 0; i < GetPipeCount(); i++) {
        AX_U8 nPipeID = m_tSnsCfg.arrPipeAttr[i].nPipeID;

        tDevBindPipe.nPipeId[i] = nPipeID;
        tDevBindPipe.nNum += 1;

        switch (m_tSnsAttr.eSnsMode) {
            case AX_SNS_LINEAR_MODE:
                tDevBindPipe.nHDRSel[i] = 0x1;
                break;
            case AX_SNS_HDR_2X_MODE:
                tDevBindPipe.nHDRSel[i] = 0x1 | 0x2;
                break;
            case AX_SNS_HDR_3X_MODE:
                tDevBindPipe.nHDRSel[i] = 0x1 | 0x2 | 0x4;
                break;
            case AX_SNS_HDR_4X_MODE:
                tDevBindPipe.nHDRSel[i] = 0x1 | 0x2 | 0x4 | 0x8;
                break;
            default:
                tDevBindPipe.nHDRSel[i] = 0x1;
                break;
        }
    }

    nRet = AX_VIN_SetDevBindPipe(nDevID, &tDevBindPipe);
    if (0 != nRet) {
        LOG_M_E(SENSOR, "AX_VIN_SetDevBindPipe failed, ret=0x%x.", nRet);
        return AX_FALSE;
    }

    nRet = AX_VIN_SetDevBindMipi(nDevID, nRxDev);
    if (0 != nRet) {
        LOG_M_E(SENSOR, "AX_VIN_SetDevBindMipi failed, ret=0x%x\n", nRet);
        return AX_FALSE;
    }

    // Snap
    if (AX_TRUE == m_tSnsCfg.bEnableFlash) {
        nRet = AX_VIN_SetSyncPowerAttr(nDevID, &m_tSnapAttr.tPowerAttr);
        if (0 != nRet) {
            LOG_M_E(SENSOR, "AX_VIN_SetSyncPowerAttr failed, nRet=0x%x.\n", nRet);
            return AX_FALSE;
        }

        nRet = AX_VIN_SetLightSyncInfo(nDevID, &m_tSnapAttr.tLightSyncAttr);
        if (0 != nRet) {
            LOG_M_E(SENSOR, "AX_VIN_SetLightSyncInfo failed, nRet=0x%x.\n", nRet);
            return AX_FALSE;
        }

        nRet = AX_VIN_SetVSyncAttr(nDevID, &m_tSnapAttr.tVsyncAttr);
        if (0 != nRet) {
            LOG_M_E(SENSOR, "AX_VIN_SetVSyncAttr failed, nRet=0x%x.\n", nRet);
            return AX_FALSE;
        }

        nRet = AX_VIN_EnableVSync(nDevID);
        if (0 != nRet) {
            LOG_M_E(SENSOR, "AX_VIN_EnableVSync failed, nRet=0x%x.\n", nRet);
            return AX_FALSE;
        }

        nRet = AX_VIN_SetHSyncAttr(nDevID, &m_tSnapAttr.tHsyncAttr);
        if (0 != nRet) {
            LOG_M_E(SENSOR, "AX_VIN_SetHSyncAttr failed, nRet=0x%x.\n", nRet);
            return AX_FALSE;
        }

        nRet = AX_VIN_EnableHSync(nDevID);
        if (0 != nRet) {
            LOG_M_E(SENSOR, "AX_VIN_EnableHSync failed, nRet=0x%x.\n", nRet);
            return AX_FALSE;
        }

        nRet = AX_VIN_SetStrobeTimingAttr(nDevID, LIGHT_STROBE, &m_tSnapAttr.tStrobeAttr);
        if (0 != nRet) {
            LOG_M_E(SENSOR, "AX_VIN_SetStrobeTimingAttr failed, nRet=0x%x.\n", nRet);
            return AX_FALSE;
        }

        nRet = AX_VIN_EnableStrobe(nDevID, LIGHT_STROBE);
        if (0 != nRet) {
            LOG_M_E(SENSOR, "AX_VIN_EnableStrobe failed, nRet=0x%x.\n", nRet);
            return AX_FALSE;
        }

        nRet = AX_VIN_SetFlashTimingAttr(nDevID, LIGHT_FLASH, &m_tSnapAttr.tFlashAttr);
        if (0 != nRet) {
            LOG_M_E(SENSOR, "AX_VIN_SetFlashTimingAttr failed, nRet=0x%x.\n", nRet);
            return AX_FALSE;
        }
    }

    // Pipe proc
    for (auto &m : m_mapPipe2Attr) {
        AX_U8 nPipeID = m.first;
        AX_VIN_PIPE_ATTR_T &tPipeAttr = m.second;

        nRet = AX_VIN_CreatePipe(nPipeID, &tPipeAttr);
        if (0 != nRet) {
            LOG_M_E(SENSOR, "AX_VIN_CreatePipe failed, ret=0x%x.", nRet);
            return AX_FALSE;
        }

        nRet = AX_VIN_SetPipeAttr(nPipeID, &tPipeAttr);
        if (0 != nRet) {
            LOG_M_E(SENSOR, "AX_VIN_SetPipeAttr failed, ret=0x%x.", nRet);
            return AX_FALSE;
        }
    }

    AX_S32 nPPLIndex;
    if (!CCmdLineParser::GetInstance()->GetPPLIndex(nPPLIndex)) {
        LOG_M_E(SENSOR, "GetPPLIndex failed.\n");
        return AX_FALSE;
    }

    for (auto &m : m_mapPipe2Attr) {
        AX_U8 nPipeID = m.first;

        if ((1 == nPPLIndex) || (2 == nPPLIndex)) {
            /* for IPC and Pano ppl*/
            nRet = AX_VIN_SetPipeFrameSource(nPipeID, AX_VIN_FRAME_SOURCE_ID_IFE, AX_VIN_FRAME_SOURCE_TYPE_DEV);
            if (0 != nRet) {
                LOG_M_E(SENSOR, "AX_VIN_SetPipeFrameSource failed, ret=0x%x.", nRet);
                return AX_FALSE;
            }
        } else if (0 == nPPLIndex) {
            /* for ITS ppl*/
            if ((AX_SNS_DCG_HDR == m_tSnsAttr.eSnsOutputMode) || (AX_SNS_DCG_VS_HDR == m_tSnsAttr.eSnsOutputMode)) {
                nRet = AX_VIN_SetPipeFrameSource(nPipeID, AX_VIN_FRAME_SOURCE_ID_IFE, AX_VIN_FRAME_SOURCE_TYPE_DEV);
                if (0 != nRet) {
                    LOG_M_E(SENSOR, "AX_VIN_SetPipeFrameSource failed, ret=0x%x.", nRet);
                    return AX_FALSE;
                }

                /* for enhance car window, flash pipe is 2 */
                if (m_tSnsCfg.bEnableFlash && nPipeID == 2) {
                    nRet = AX_VIN_SetPipeFrameSource(nPipeID, AX_VIN_FRAME_SOURCE_ID_ITP, AX_VIN_FRAME_SOURCE_TYPE_DEV);
                    if (0 != nRet) {
                        LOG_M_E(SENSOR, "AX_VIN_SetPipeFrameSource failed, ret=0x%x.", nRet);
                        return AX_FALSE;
                    }
                }
            } else {
                nRet = AX_VIN_SetPipeFrameSource(nPipeID, AX_VIN_FRAME_SOURCE_ID_IFE, AX_VIN_FRAME_SOURCE_TYPE_USER);
                if (0 != nRet) {
                    LOG_M_E(SENSOR, "AX_VIN_SetPipeFrameSource failed, ret=0x%x.", nRet);
                    return AX_FALSE;
                }

                /* for enhance car window, flash pipe is 2 */
                if (m_tSnsCfg.bEnableFlash && nPipeID == 2) {
                    nRet = AX_VIN_SetPipeFrameSource(nPipeID, AX_VIN_FRAME_SOURCE_ID_ITP, AX_VIN_FRAME_SOURCE_TYPE_USER);
                    if (0 != nRet) {
                        LOG_M_E(SENSOR, "AX_VIN_SetPipeFrameSource failed, ret=0x%x.", nRet);
                        return AX_FALSE;
                    }
                }
            }
        } else {
            LOG_M_E(SENSOR, "Unsupported ppl index.");
            return AX_FALSE;
        }
    }

    // SNS register
    for (auto &m : m_mapPipe2Attr) {
        AX_U8 nPipeID = m.first;
        if (!RegisterSensor(nPipeID, m_tSnsCfg.nDevNode, m_tSnsCfg.nI2cAddr)) {
            LOG_M_E(SENSOR, "RegisterSensor failed, ret=0x%x.", nRet);
            return AX_FALSE;
        }
    }

    // SNS Proc
    for (auto &m : m_mapPipe2Attr) {
        AX_U8 nPipeID = m.first;
        nRet = AX_ISP_SetSnsAttr(nPipeID, &m_tSnsAttr);
        if (0 != nRet) {
            LOG_M_E(SENSOR, "AX_ISP_SetSnsAttr failed, ret=0x%x.", nRet);
            return AX_FALSE;
        }

        nRet = AX_ISP_OpenSnsClk(m_tSnsClkAttr.nSnsClkIdx, m_tSnsClkAttr.eSnsClkRate);
        if (0 != nRet) {
            LOG_M_E(SENSOR, "AX_ISP_OpenSnsClk failed, ret=0x%x.", nRet);
            return AX_FALSE;
        }
    }

    for (AX_U8 i = 0; i < GetPipeCount(); i++) {
        AX_U8 nPipeID = m_tSnsCfg.arrPipeAttr[i].nPipeID;
        vector<string> vecPipeTuningBin = GetSnsConfig().arrPipeAttr[i].vecTuningBin;

        nRet = AX_ISP_Create(nPipeID);
        if (0 != nRet) {
            LOG_M_E(SENSOR, "AX_ISP_Create failed, ret=0x%x.", nRet);
            return AX_FALSE;
        }

        if (!m_algWrapper.RegisterAlgoToSensor(m_pSnsObj, nPipeID)) {
            LOG_M_E(SENSOR, "RegisterAlgoToSensor failed.");
            return AX_FALSE;
        }

        for (AX_U8 j = 0; j < vecPipeTuningBin.size(); j++) {
            std::string strKey = m_tSnsCfg.eSensorMode == AX_SNS_LINEAR_MODE ? SDRKEY : HDRKEY;
            if (!vecPipeTuningBin[j].empty()) {
                if (0 != access(vecPipeTuningBin[j].data(), F_OK)) {
                    LOG_MM_W(SENSOR, "bin file is not exist.");
                    continue;
                }

                if (std::string::npos == vecPipeTuningBin[j].find(strKey)) {
                    LOG_MM_W(SENSOR, "hdrMode and bin do not match");
                    continue;
                }

                LOG_MM_I(SENSOR, "sns[%d]pipe[%d] load bin: %s", m_tSnsCfg.nSnsID, nPipeID, vecPipeTuningBin[j].c_str());

                nRet = AX_ISP_LoadBinParams(nPipeID, vecPipeTuningBin[j].c_str());
                if (AX_SUCCESS != nRet) {
                    LOG_M_E(SENSOR, "pipe[%d], %d tuning bin , AX_ISP_LoadBinParams ret=0x%x %s. The parameters in sensor.h will be used\n",
                            i, j, nRet, vecPipeTuningBin[j].c_str());
                }
            }
        }

        nRet = AX_ISP_Open(nPipeID);
        if (0 != nRet) {
            LOG_M_E(SENSOR, "AX_ISP_Open failed, ret=0x%x.", nRet);
            return AX_FALSE;
        }
    }

    for (AX_U8 i = 0; i < GetPipeCount(); i++) {
        AX_U8 nPipeID = m_tSnsCfg.arrPipeAttr[i].nPipeID;

        for (AX_U8 j = AX_VIN_CHN_ID_MAIN; j < AX_VIN_CHN_ID_MAX; j++) {
            AX_VIN_CHN_ATTR_T &tChnAttr = m_mapPipe2ChnAttr[nPipeID][j];

            if (tChnAttr.nWidth > 0 && tChnAttr.nHeight > 0) {
                nRet = AX_VIN_SetChnAttr(nPipeID, (AX_VIN_CHN_ID_E)j, &tChnAttr);
                if (0 != nRet) {
                    LOG_M_E(SENSOR, "(%d)AX_VIN_SetChnAttr(%dX%d) failed, ret=0x%x.",
                                     nPipeID, tChnAttr.nWidth, tChnAttr.nHeight, nRet);
                    return AX_FALSE;
                }
            }

            if (m_tSnsCfg.arrPipeAttr[i].arrChannelAttr[j].bChnEnable) {
                nRet = AX_VIN_EnableChn(nPipeID, (AX_VIN_CHN_ID_E)j);
                if (0 != nRet) {
                    LOG_M_E(SENSOR, "AX_VIN_EnableChn failed, ret=0x%x.", nRet);
                    return AX_FALSE;
                }
            }
        }
    }

    for (AX_U8 i = 0; i < GetPipeCount(); i++) {
        AX_U8 nPipeID = m_tSnsCfg.arrPipeAttr[i].nPipeID;
        nRet = AX_VIN_StartPipe(nPipeID);
        if (0 != nRet) {
            LOG_M_E(SENSOR, "AX_VIN_StartPipe failed, ret=0x%x.", nRet);
            return AX_FALSE;
        }

        if (!m_tSnsCfg.arrPipeAttr[i].bSnapshot) {
            nRet = AX_ISP_Start(nPipeID);
            if (0 != nRet) {
                LOG_M_E(SENSOR, "AX_ISP_Start failed, ret=0x%x.", nRet);
                return AX_FALSE;
            }
        } /* Snapshot pipe will apply AX_ISP_RunOnce manually instead */
    }

    for (AX_U8 i = 1; i < GetPipeCount(); i++) {
        AX_U8 nPipeID = m_tSnsCfg.arrPipeAttr[i].nPipeID;
        SetAeToManual(nPipeID);
    }

    if (AX_VIN_DEV_OFFLINE == m_tDevAttr.eDevMode) {
        AX_VIN_DUMP_ATTR_T tDumpAttr;
        tDumpAttr.bEnable = AX_TRUE;
        tDumpAttr.nDepth = 3;
        nRet = AX_VIN_SetDevDumpAttr(nDevID, AX_VIN_DUMP_QUEUE_TYPE_DEV, &tDumpAttr);
        if (0 != nRet) {
            LOG_M_E(SENSOR, "AX_VIN_SetDevDumpAttr failed, ret=0x%x.", nRet);
            return AX_FALSE;
        }
    }

    nRet = AX_VIN_EnableDev(nDevID);
    if (0 != nRet) {
        LOG_M_E(SENSOR, "AX_VIN_EnableDev failed, ret=0x%x.", nRet);
        return AX_FALSE;
    }

#ifdef NOT_DCGVS_DCG
    for (AX_U8 i = 0; i < GetPipeCount(); i++) {
        AX_U8 nPipeID = m_tSnsCfg.arrPipeAttr[i].nPipeID;
        nRet = AX_ISP_StreamOn(nPipeID);
        if (0 != nRet) {
            LOG_M_E(SENSOR, "AX_ISP_StreamOn failed, ret=0x%x.", nRet);
            return AX_FALSE;
        }
    }
#endif

    LOG_MM(SENSOR, "[%d] ---", m_tSnsCfg.nDevID);

    return AX_TRUE;
}

AX_BOOL COS04a10Dcg::CloseAll() {
    LOG_MM(SENSOR, "[Dev:%d, RxDev:%d] +++", m_tSnsCfg.nDevID, m_tSnsCfg.nRxDev);

    AX_S32 nRet = AX_SUCCESS;

    AX_U8 nDevID = m_tSnsCfg.nDevID;
    // AX_U8 nRxDev = m_tSnsCfg.nRxDev;

    if (AX_TRUE == m_tSnsCfg.bEnableFlash) {
        nRet = AX_VIN_DisableVSync(nDevID);
        if (0 != nRet) {
            LOG_M_E(SENSOR, "AX_VIN_EnableVSync failed, nRet=0x%x.\n", nRet);
            return AX_FALSE;
        }

        nRet = AX_VIN_DisableHSync(nDevID);
        if (0 != nRet) {
            LOG_M_E(SENSOR, "AX_VIN_EnableHSync failed, nRet=0x%x.\n", nRet);
            return AX_FALSE;
        }

        nRet = AX_VIN_DisableStrobe(nDevID, LIGHT_STROBE);
        if (0 != nRet) {
            LOG_M_E(SENSOR, "AX_VIN_EnableStrobe failed, nRet=0x%x.\n", nRet);
            return AX_FALSE;
        }
    }

    for (AX_U8 i = 0; i < GetPipeCount(); i++) {
        AX_U8 nPipeID = m_tSnsCfg.arrPipeAttr[i].nPipeID;

        if (!m_tSnsCfg.arrPipeAttr[i].bSnapshot) {
            nRet = AX_ISP_Stop(nPipeID);
            if (0 != nRet) {
                LOG_M_E(SENSOR, "AX_ISP_Stop failed, ret=0x%x.", nRet);
                return AX_FALSE;
            }
        }
    }

    nRet = AX_VIN_DisableDev(nDevID);
    if (0 != nRet) {
        LOG_M_E(SENSOR, "AX_VIN_DisableDev failed, ret=0x%x.", nRet);
        return AX_FALSE;
    }

#ifdef NOT_DCGVS_DCG
    for (AX_U8 i = 0; i < GetPipeCount(); i++) {
        AX_U8 nPipeID = m_tSnsCfg.arrPipeAttr[i].nPipeID;

        nRet = AX_ISP_StreamOff(nPipeID);
        if (0 != nRet) {
            LOG_M_E(SENSOR, "AX_ISP_StreamOff failed, ret=0x%x.", nRet);
            return AX_FALSE;
        }
    }
#endif

    if (AX_VIN_DEV_OFFLINE == m_tSnsCfg.eDevMode) {
        AX_VIN_DUMP_ATTR_T tDumpAttr;
        tDumpAttr.bEnable = AX_FALSE;
        nRet = AX_VIN_SetDevDumpAttr(nDevID, AX_VIN_DUMP_QUEUE_TYPE_DEV, &tDumpAttr);
        if (0 != nRet) {
            LOG_M_E(SENSOR, "AX_VIN_SetDevDumpAttr failed, ret=0x%x.", nRet);
            return AX_FALSE;
        }
    }

#ifdef NOT_DCGVS_DCG
    for (AX_U8 i = 0; i < GetPipeCount(); i++) {
        nRet = AX_ISP_CloseSnsClk(m_tSnsClkAttr.nSnsClkIdx);
        if (0 != nRet) {
            LOG_M_E(SENSOR, "AX_ISP_CloseSnsClk failed, ret=0x%x.", nRet);
            return AX_FALSE;
        }
    }
#endif

    for (AX_U8 i = 0; i < GetPipeCount(); i++) {
        AX_U8 nPipeID = m_tSnsCfg.arrPipeAttr[i].nPipeID;

        nRet = AX_VIN_StopPipe(nPipeID);
        if (0 != nRet) {
            LOG_M_E(SENSOR, "AX_VIN_StopPipe failed, ret=0x%x.", nRet);
            return AX_FALSE;
        }

        for (AX_U8 j = AX_VIN_CHN_ID_MAIN; j < AX_VIN_CHN_ID_MAX; j++) {
            if (m_tSnsCfg.arrPipeAttr[i].arrChannelAttr[j].bChnEnable) {
                nRet = AX_VIN_DisableChn(nPipeID, (AX_VIN_CHN_ID_E)j);
                if (0 != nRet) {
                    LOG_M_E(SENSOR, "AX_VIN_DisableChn failed, ret=0x%x.", nRet);
                    return AX_FALSE;
                }
            }
        }

        nRet = AX_ISP_Close(nPipeID);
        if (0 != nRet) {
            LOG_M_E(SENSOR, "AX_ISP_Close failed, ret=0x%x.", nRet);
            return AX_FALSE;
        }

        if (!m_algWrapper.UnRegisterAlgoFromSensor(nPipeID)) {
            LOG_M_E(SENSOR, "UnRegisterAlgoFromSensor failed.");
            return AX_FALSE;
        }

        nRet = AX_ISP_Destroy(nPipeID);
        if (0 != nRet) {
            LOG_M_E(SENSOR, "AX_ISP_Destroy failed, ret=0x%x.", nRet);
            return AX_FALSE;
        }

        if (!UnRegisterSensor(nPipeID)) {
            LOG_M_E(SENSOR, "UnRegisterSensor failed, ret=0x%x.", nRet);
            return AX_FALSE;
        }

        nRet = AX_VIN_DestroyPipe(nPipeID);
        if (0 != nRet) {
            LOG_M_E(SENSOR, "AX_VIN_DestroyPipe failed, ret=0x%x.", nRet);
            return AX_FALSE;
        }
    }

#ifdef NOT_DCGVS_DCG
    nRet = AX_MIPI_RX_Stop(nRxDev);
    if (0 != nRet) {
        LOG_M_E(SENSOR, "AX_MIPI_RX_Stop failed, ret=0x%x.", nRet);
        return AX_FALSE;
    }
#endif

    nRet = AX_VIN_DestroyDev(nDevID);
    if (0 != nRet) {
        LOG_M_E(SENSOR, "AX_VIN_DestroyDev failed, ret=0x%x.", nRet);
        return AX_FALSE;
    }

    LOG_MM(SENSOR, "[Dev:%d, RxDev:%d] ---", m_tSnsCfg.nDevID, m_tSnsCfg.nRxDev);

    return AX_TRUE;
}
#endif
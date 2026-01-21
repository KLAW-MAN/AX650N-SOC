/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include "PanoBuilder.h"
#include "AXRtspObserver.h"
#include "AXRtspServer.h"
#include "AXTypeConverter.hpp"
#include "AppLogApi.h"
#include "AudioOptionHelper.h"
#include "AudioWrapper.hpp"
#include "CaptureObserver.hpp"
#include "CmdLineParser.h"
#include "CommonUtils.hpp"
#include "EncoderOptionHelper.h"
#include "GlobalDef.h"
#include "DetectObserver.hpp"
#include "PanoFrameCollectorObserver.h"
#include "OSDHandler.h"
#include "OsdObserver.h"
#include "OSDHelper.h"
#include "IvesObserver.hpp"
#include "IvpsOptionHelper.h"
#include "IvpsObserver.h"
#include "MpegObserver.h"
#include "AlgoOptionHelper.h"
#include "WebObserver.h"
#include "WebOptionHelper.h"
#include "WebServer.h"
#include "ax_engine_api.h"
#include "ax_venc_api.h"
#include "AvsOptionHelper.h"
#include "SensorOptionHelper.h"
#include "VencObserver.h"
#include "JencObserver.h"
#include "DspCfgParser.h"
#include "DspUtils.hpp"

using namespace AX_PANO;

#ifndef SLT
#include "./test/PanoTestSuite.h"
#include "./test/PanoTestSuiteCfgParser.h"
#endif

#define PPL "PANO_PPL"

#define APP_VENC_VIDEO_MAX_WIDTH  (8192)
#define APP_VENC_VIDEO_MAX_HEIGHT (8192)

#ifndef SLT
namespace {
AX_VOID AudioPlayCallback(AX_APP_AUDIO_CHAN_E eChan, const AX_APP_AUDIO_PLAY_FILE_RESULT_PTR pstResult) {
    if (pstResult) {
        LOG_MM(PPL, "[%d] %s play complete, status: %d", eChan, pstResult->pstrFileName, pstResult->eStatus);
    }
}
}  // namespace
#endif

AX_BOOL CPanoBuilder::Construct(AX_VOID) {
    LOG_MM(PPL, "+++");

    /* Step-1: Global initialization */
    if (AX_FALSE == InitSysMods()) {
        return AX_FALSE;
    }

    if (!CCmdLineParser::GetInstance()->GetScenario(m_nScenario)) {
        LOG_MM(PPL, "Apply default scenario %d.", m_nScenario);
    }

    /* Step-2: Linkage initialization */
    if (m_linkage.Setup()) {
        LOG_MM_W(PPL, "Linkage setup.");
    } else {
        LOG_MM_W(PPL, "No linkage setup.");
    }

    /*Step-3: Sensor initialize */
    if (AX_FALSE == InitSensor()) {
        LOG_MM_E(PPL, "Initailize SensorMgr failed.");
        return AX_FALSE;
    }

    /*Step-4: Audio initialize */
    if (AX_FALSE == InitAudio()) {
        LOG_MM_E(PPL, "Initailize Audio failed.");
        return AX_FALSE;
    }

    /*Step-5: AVS initialize, make sure init before ivps*/
    if (AX_FALSE == InitAvs()) {
        LOG_MM_E(PPL, "Initailize AVS failed.");
        return AX_FALSE;
    }

    /*Step-6: Ivps initialize, make sure init after avs */
    if (AX_FALSE == InitIvps()) {
        LOG_MM_E(PPL, "Initailize Ivps failed.");
        return AX_FALSE;
    }

    /*Step-7: venc initialize */
    if (AX_FALSE == InitVenc()) {
        LOG_MM_E(PPL, "Initailize Venc failed.");
        return AX_FALSE;
    }

    /*Step-8: capture initialize */
    if (AX_FALSE == InitCapture()) {
        LOG_MM_E(PPL, "Initailize capture failed.");
        return AX_FALSE;
    }

    /*Step-9: collector initialize */
    if (AX_FALSE == InitCollector()) {
        LOG_MM_E(PPL, "Initailize Ivps failed.");
        return AX_FALSE;
    }

    /*Step-10: jenc initialize */
    if (AX_FALSE == InitJenc()) {
        LOG_MM_E(PPL, "Initailize Jenc failed.");
        return AX_FALSE;
    }

    /*Step-11: detector initialize */
    if (AX_FALSE == InitDetector()) {
        LOG_MM_E(PPL, "Initailize detector failed.");
        return AX_FALSE;
    }

    /*Step-12: ives initialize */
    if (AX_FALSE == InitIves()) {
        LOG_MM_E(PPL, "Initailize ives failed.");
        return AX_FALSE;
    }

    /*Step-13: PPL pool initialize */
    if (AX_FALSE == InitPoolConfig()) {
        LOG_MM_E(PPL, "Initailize Pool failed.");
        return AX_FALSE;
    }

    /* Step-14: Initialize RTSP */
    if (!CAXRtspServer::GetInstance()->Init()) {
        return AX_FALSE;
    }
    LOG_M_I(PPL, "Init Rtsp successfully.");

    /*Step-15: Bind PPL*/
    if (CWebServer::GetInstance()->Init()) {
        CWebServer::GetInstance()->BindPPL(this);
    } else {
        return AX_FALSE;
    }
    LOG_M_I(PPL, "Init Webserver successfully.");

    /*Step-16: Initialize MPEG4*/
    for (auto pInstance : m_vecMpeg4Instance) {
        if (!pInstance->Init()) {
            return AX_FALSE;
        }
    }
    LOG_M_I(PPL, "Init MPEG4 successfully.");

    /*Step-17: Initialize UT*/
    if (CCmdLineParser::GetInstance()->isUTEnabled()) {
        if (CTestSuite::GetInstance()->Init()) {
            CTestSuite::GetInstance()->BindPPL(this);
        }
    }

    LOG_MM(PPL, "---");
    return AX_TRUE;
}

AX_BOOL CPanoBuilder::InitAudio() {
    LOG_MM(PPL, "+++");

#ifndef SLT
    // audio
    AX_APP_AUDIO_ATTR_T stAudioAttr = APP_AUDIO_ATTR();
    AX_APP_Audio_Init(&stAudioAttr);
#endif

    LOG_MM(PPL, "---");

    return AX_TRUE;
}

AX_BOOL CPanoBuilder::InitSensor() {
    LOG_MM(PPL, "+++");

    if (AX_FALSE == m_mgrSensor.Init()) {
        LOG_MM_E(PPL, "Initailize SensorMgr failed.");
        return AX_FALSE;
    }

    AX_APP_AVS_CFG_T stAVSCfg;
    if (!APP_AVS_CONFIG(m_nScenario, stAVSCfg)) {
        return AX_FALSE;
    }

    std::map<AX_U8, AX_U8> mapStitchPipe;
    if (!GetStitchPipeMap(mapStitchPipe)) {
        return AX_FALSE;
    }

    if (AX_FALSE == m_mgrSensor.SetVinStitchAttr(stAVSCfg.strParamFilePath, stAVSCfg.bAroundView, stAVSCfg.bPrioOnsiteCali, mapStitchPipe)) {
        LOG_MM_E(PPL, "SensorMgr SetVinStitchAttr failed.");
        return AX_FALSE;
    }

    AX_U32 nSnsCnt = m_mgrSensor.GetSensorCount();
    for (AX_U32 i = 0; i < nSnsCnt; i++) {
        CBaseSensor* pSensor = m_mgrSensor.GetSnsInstance(i);
        SENSOR_CONFIG_T tSnsConfig = pSensor->GetSnsConfig();
        AX_U8 nSnsID = tSnsConfig.nSnsID;
        AX_S32 nScenario;
        CCmdLineParser::GetInstance()->GetScenario(nScenario);
        WEB_CAMERA_ATTR_T tCamera;
        tCamera.nSnsMode = tSnsConfig.eSensorMode;
        tCamera.nRotation = 0;
        tCamera.fFramerate = tSnsConfig.fFrameRate;
        tCamera.nDayNightMode = 0;
        tCamera.nNrMode = 1;
        tCamera.bAutoCaptureEnable = AX_TRUE;
        tCamera.bSnsModeEnable = AX_FALSE;
        tCamera.bPNModeEnable = AX_FALSE;
        tCamera.bMirrorFlipEnable = AX_FALSE;
        tCamera.bRotationEnable = AX_FALSE;
        tCamera.bLdcEnable = AX_FALSE;
        CWebOptionHelper::GetInstance()->InitCameraAttr(nSnsID, tSnsConfig.eSensorType, tCamera);
        CTestSuite::GetInstance()->InitCameraAttr(nSnsID, tSnsConfig.eSensorType, tCamera);
    }

    LOG_MM(PPL, "---");
    return AX_TRUE;
}

AX_BOOL CPanoBuilder::InitIvps() {
    LOG_MM(PPL, "+++");
    for (AX_U8 i = 0; i < MAX_IVPS_GROUP_NUM; i++) {
        IVPS_GROUP_CFG_T tIvpsGrpCfg;
        /* Step 1: Get configurations  */
        if (!APP_IVPS_CONFIG(m_nScenario, i, tIvpsGrpCfg)) {
            /* Not configured for this group */
            break;
        }

        /* Step 2: Copy configurations and do initialization */
        CIVPSGrpStage* pIvpsInstance = new CIVPSGrpStage(tIvpsGrpCfg);
        if (!pIvpsInstance->Init()) {
            break;
        }

        if (COptionHelper::GetInstance()->IsEnableOSD()) {
            IOSDHelper* pHelper = new COSDHelper();
            pIvpsInstance->AttachOsdHelper(pHelper);
        }

        /* Step-3: Register IVPS observers and fill attributes from source modules */
        PPL_MOD_INFO_T tDstMod = {E_PPL_MOD_TYPE_IVPS, i, 0};
        vector<PPL_MOD_RELATIONSHIP_T> vecRelations;
        if (!GetRelationsByDstMod(tDstMod, vecRelations, AX_TRUE)) {
            LOG_MM_E(PPL, "Can not find relation for dest module: (Module:%s, Grp:%d, Chn:%d)", ModType2String(tDstMod.eModType).c_str(),
                     tDstMod.nGroup, tDstMod.nChannel);
            continue;
            // return AX_FALSE;
        }

        m_vecIvpsObs.emplace_back(CObserverMaker::CreateObserver<CIvpsObserver>(pIvpsInstance));
        /* NOTICE: Suppose each ivps group has only one frame source */
        PPL_MOD_RELATIONSHIP_T& tRelation = vecRelations[0];
        IVPS_GROUP_CFG_T* pConfig = pIvpsInstance->GetGrpCfg();

        if (E_PPL_MOD_TYPE_IVPS == tRelation.tSrcModChn.eModType) {
            m_vecIvpsInstance[tRelation.tSrcModChn.nGroup]->RegObserver(tRelation.tSrcModChn.nChannel,
                                                                        m_vecIvpsObs[m_vecIvpsObs.size() - 1].get());
        } else if (E_PPL_MOD_TYPE_VIN == tRelation.tSrcModChn.eModType) {
            m_mgrSensor.RegObserver(tRelation.tSrcModChn.nGroup, tRelation.tSrcModChn.nChannel,
                                    m_vecIvpsObs[m_vecIvpsObs.size() - 1].get());
        }

        if (tRelation.bLink) {
            if (E_PPL_MOD_TYPE_AVS == tRelation.tSrcModChn.eModType) {
                AX_APP_AVS_RESOLUTION_T stAvsResolution = m_avs.GetAvsResolution();
                CBaseSensor* pSensor = m_mgrSensor.GetSnsInstance(1); // Ref ppl
                AX_F32 fFrameRate = pSensor->GetSnsConfig().arrPipeAttr->fPipeFramerate;
                AX_U8 nOutChannels = tIvpsGrpCfg.nGrpChnNum;

                pConfig->nSnsSrc = 0;

                if (-1 == pConfig->arrGrpResolution[0]) {
                    pConfig->arrGrpResolution[0] = stAvsResolution.u32Width;
                }

                if (-1 == pConfig->arrGrpResolution[1]) {
                    pConfig->arrGrpResolution[1] = stAvsResolution.u32Height;
                }

                pConfig->arrGrpInResolution[0] = (AX_S16)stAvsResolution.u32Width;
                pConfig->arrGrpInResolution[1] = (AX_S16)stAvsResolution.u32Height;

                if (-1 == pConfig->arrGrpFramerate[0]) {
                    pConfig->arrGrpFramerate[0] = fFrameRate;
                }

                if (-1 == pConfig->arrGrpFramerate[1]) {
                    pConfig->arrGrpFramerate[1] = fFrameRate;
                }

                for (AX_U8 j = 0; j < nOutChannels; j++) {
                    if (-1 == pConfig->arrChnResolution[j][0]) {
                        if (pConfig->arrGrpResolution[0] <= APP_VENC_VIDEO_MAX_WIDTH) {
                            pConfig->arrChnResolution[j][0] = pConfig->arrGrpResolution[0];
                        } else {
                            pConfig->arrChnResolution[j][0] = APP_VENC_VIDEO_MAX_WIDTH;
                        }
                    }

                    if (-1 == pConfig->arrChnResolution[j][1]) {
                        if (pConfig->arrGrpResolution[1] <= APP_VENC_VIDEO_MAX_HEIGHT) {
                            pConfig->arrChnResolution[j][1] = pConfig->arrGrpResolution[1];
                        } else {
                            pConfig->arrChnResolution[j][1] = APP_VENC_VIDEO_MAX_HEIGHT;
                        }
                    }

                    if (-1 == pConfig->arrChnFramerate[j][0]) {
                        pConfig->arrChnFramerate[j][0] = pConfig->arrGrpFramerate[0];
                    }

                    if (-1 == pConfig->arrChnFramerate[j][1]) {
                        pConfig->arrChnFramerate[j][1] = pConfig->arrGrpFramerate[1];
                    }
                }
            }

            pConfig->nGrpLinkFlag = AX_TRUE;
        }

        /* Step-4: initialize IVPS with latest attributes */
        if (-1 != pConfig->arrGrpResolution[0]
            && -1 != pConfig->arrGrpResolution[1]) {
            if (!pIvpsInstance->InitParams()) {
                return AX_FALSE;
            }
        }

        /* Step-5: Save instance */
        m_vecIvpsInstance.emplace_back(pIvpsInstance);

        LOG_M_I(PPL, "Init ivps group %d successfully.", i);
    }

    for (auto pInstance : m_vecIvpsInstance) {
        IVPS_GROUP_CFG_T *pCfg = pInstance->GetGrpCfg();
        if (pCfg && (pCfg->nLdcEnable != (AX_U8)-1)) {
            WEB_CAMERA_ATTR_T tCamera = CWebOptionHelper::GetInstance()->GetCamera(pCfg->nSnsSrc);
            /* update web ldcAttr value */
            tCamera.tLdcAttr.bLdcEnable = (pCfg->nLdcEnable == 1) ? AX_TRUE : AX_FALSE;
            tCamera.tLdcAttr.bLdcAspect = pCfg->bLdcAspect;
            tCamera.tLdcAttr.nXRatio = pCfg->nLdcXRatio;
            tCamera.tLdcAttr.nYRatio = pCfg->nLdcYRatio;
            tCamera.tLdcAttr.nXYRatio = pCfg->nLdcXYRatio;
            tCamera.tLdcAttr.nDistorRatio = pCfg->nLdcDistortionRatio;
            CWebOptionHelper::GetInstance()->SetCamera(pCfg->nSnsSrc, tCamera);
        }

    }

    LOG_MM(PPL, "---");
    return AX_TRUE;
}

AX_BOOL CPanoBuilder::InitVenc() {
    LOG_MM(PPL, "+++");

    AX_APP_AUDIO_ATTR_T stAudioAttr = APP_AUDIO_ATTR();

    for (AX_U8 i = 0; i < MAX_VENC_CHANNEL_NUM; i++) {
        VIDEO_CONFIG_T tConfig;
        do {
            if (APP_VENC_CONFIG(m_nScenario, i, tConfig)) {
                CVideoEncoder* pVencInstance = new CVideoEncoder(tConfig);
                if (!pVencInstance->Init()) {
                    break;
                }
                PPL_MOD_INFO_T tDstMod = {E_PPL_MOD_TYPE_VENC, 0, pVencInstance->GetChannel()};
                vector<PPL_MOD_RELATIONSHIP_T> vecRelations;
                if (!GetRelationsByDstMod(tDstMod, vecRelations)) {
                    LOG_MM_E(PPL, "Can not find relation for dst module: (Module:%s, Grp:%d, Chn:%d)",
                             ModType2String(tDstMod.eModType).c_str(), tDstMod.nGroup, tDstMod.nChannel);
                    break;
                }
                VIDEO_CONFIG_T* pConfig = pVencInstance->GetChnCfg();
                PPL_MOD_RELATIONSHIP_T& tRelation = vecRelations[0];

                pConfig->nMaxWidth  = APP_ENCODER_PANO_MAX_WIDTH;
                pConfig->nMaxHeight = APP_ENCODER_PANO_MAX_HEIGHT;

                m_vecVencObs.emplace_back(CObserverMaker::CreateObserver<CVencObserver>(pVencInstance));
                if (E_PPL_MOD_TYPE_IVPS == tRelation.tSrcModChn.eModType) {
                    m_vecIvpsInstance[tRelation.tSrcModChn.nGroup]->RegObserver(tRelation.tSrcModChn.nChannel,
                                                                                m_vecVencObs[m_vecVencObs.size() - 1].get());
                } else if (E_PPL_MOD_TYPE_VIN == tRelation.tSrcModChn.eModType) {
                    m_mgrSensor.RegObserver(tRelation.tSrcModChn.nGroup, tRelation.tSrcModChn.nChannel,
                                            m_vecVencObs[m_vecVencObs.size() - 1].get());
                }

                if (tRelation.bLink) {
                    pConfig->bLink = AX_TRUE;
                }

                if (!pVencInstance->InitParams()) {
                    LOG_MM_E(PPL, "Init venc %d failed.", tConfig.nChannel);
                    return AX_FALSE;
                }

                /* initialize web's video setting */
                WEB_VIDEO_ATTR_T tVideoAttr;
                AX_U8 nEncoderType = CAXTypeConverter::EncoderType2Int(tConfig.ePayloadType);
                AX_U8 nPrevChn =
                    CWebServer::GetInstance()->RegistPreviewChnMappingInOrder(tConfig.nPipeSrc, tConfig.nChannel, nEncoderType);
                tVideoAttr.nUniChn = tConfig.nChannel;
                tVideoAttr.bEnable = AX_TRUE;
                tVideoAttr.nEncoderType = nEncoderType; /* 0: H264; 1: MJpeg; 2: H265 */
                tVideoAttr.nBitrate = pConfig->nBitrate;
                tVideoAttr.nWidth = pConfig->nWidth;
                tVideoAttr.nHeight = pConfig->nHeight;
                tVideoAttr.nRcType = CAXTypeConverter::RcMode2Int(tConfig.eRcType);
                memcpy(tVideoAttr.stEncodeCfg, tConfig.stEncodeCfg, sizeof(APP_ENC_RC_CONFIG) * APP_ENCODER_TYPE_MAX);
                tVideoAttr.bLink = pConfig->bLink;

                CWebOptionHelper::GetInstance()->InitVideoAttr(tConfig.nPipeSrc, nPrevChn, tVideoAttr);
                CTestSuite::GetInstance()->InitVideoAttr(tConfig.nPipeSrc, tConfig.nChannel, tVideoAttr);
                /* register rtsp  */
                m_vecRtspObs.emplace_back(CObserverMaker::CreateObserver<CAXRtspObserver>(CAXRtspServer::GetInstance()));
                // audio for rtsp
                AX_APP_Audio_RegPacketObserver(APP_AUDIO_RTSP_CHANNEL(),
                                               m_vecRtspObs[m_vecRtspObs.size() - 1].get());
                pVencInstance->RegObserver(m_vecRtspObs[m_vecRtspObs.size() - 1].get());

                /* register web */
                m_vecWebObs.emplace_back(CObserverMaker::CreateObserver<CWebObserver>(CWebServer::GetInstance()));
                pVencInstance->RegObserver(m_vecWebObs[m_vecWebObs.size() - 1].get());

                if (!CWebOptionHelper::GetInstance()->SetResolutionOptions(pConfig->nPipeSrc, nPrevChn, pConfig->nWidth, pConfig->nHeight)) {
                    LOG_MM_E(PPL, "Set web video chn(%d) resolutions(%dx%d) fail!", nPrevChn, pConfig->nWidth, pConfig->nHeight);
                    return AX_FALSE;
                }

                // audio for webserver
                if (0 == tConfig.nChannel) {
                    m_vecWebObs.emplace_back(CObserverMaker::CreateObserver<CWebObserver>(CWebServer::GetInstance()));
                    AX_APP_Audio_RegPacketObserver(APP_AUDIO_WEB_STREAM_CHANNEL(),
                                                   m_vecWebObs[m_vecWebObs.size() - 1].get());
                }

                if (COptionHelper::GetInstance()->IsEnableVideoRecord()) {
                    CMPEG4Encoder* pMp4Instance = new CMPEG4Encoder();
                    VIDEO_CONFIG_T* pConfig = pVencInstance->GetChnCfg();
                    MPEG4EC_INFO_T tMpeg4Info;
                    tMpeg4Info.stVideoChnInfo.nVideoChn = pConfig->nChannel;
                    tMpeg4Info.stVideoChnInfo.nPreviewChn = nPrevChn;
                    tMpeg4Info.stVideoChnInfo.nSnsId = pConfig->nPipeSrc;
                    tMpeg4Info.stVideoChnInfo.bOn = pConfig->bVideoRecordOn;

                    tMpeg4Info.nMaxFileInMBytes = COptionHelper::GetInstance()->GetVideoMaxRecordSize();
                    tMpeg4Info.bLoop = COptionHelper::GetInstance()->IsVideoRecordLoop();
                    tMpeg4Info.nMaxFileNum = COptionHelper::GetInstance()->GetVideoMaxRecordNum();

                    AX_F32 fRatio = COptionHelper::GetInstance()->GetVencOutBuffRatio();
                    AX_U32 nBuffSize = pConfig->nWidth * pConfig->nHeight * 3 / 2 * fRatio;
                    tMpeg4Info.stVideoAttr.bEnable = AX_TRUE;
                    tMpeg4Info.stVideoAttr.ePt = pConfig->ePayloadType;
                    tMpeg4Info.stVideoAttr.nfrWidth = pConfig->nWidth;
                    tMpeg4Info.stVideoAttr.nfrHeight = pConfig->nHeight;
                    tMpeg4Info.stVideoAttr.nFrameRate = pConfig->fFramerate;
                    tMpeg4Info.stVideoAttr.nBitrate = pConfig->nBitrate;
                    tMpeg4Info.stVideoAttr.nMaxFrmSize = nBuffSize;

                    AX_APP_AUDIO_CHAN_E nAudioMp4Chn = APP_AUDIO_MP4_CHANNEL();
                    AX_APP_AUDIO_ENCODER_ATTR_T stAttr;
                    if (0 == AX_APP_Audio_GetEncoderAttr(nAudioMp4Chn, &stAttr)) {
                        tMpeg4Info.stAudioAttr.bEnable = AX_TRUE;
                        tMpeg4Info.stAudioAttr.ePt = (AX_PAYLOAD_TYPE_E)stAttr.eType;
                        tMpeg4Info.stAudioAttr.nBitrate = stAttr.nBitRate;
                        tMpeg4Info.stAudioAttr.nSampleRate = (AX_U32)stAttr.eSampleRate;
                        tMpeg4Info.stAudioAttr.nChnCnt = (AX_APP_AUDIO_SOUND_MODE_MONO == stAttr.eSoundMode) ? 1 : 2;
                        tMpeg4Info.stAudioAttr.nAOT = (AX_S32)stAttr.nAOT;
                        tMpeg4Info.stAudioAttr.nMaxFrmSize = COptionHelper::GetInstance()->GetAencOutFrmSize();
                    } else {
                        tMpeg4Info.stAudioAttr.bEnable = AX_FALSE;
                    }

                    tMpeg4Info.strSavePath = COptionHelper::GetInstance()->GetVideoSavedPath();
                    if (AX_FALSE == pMp4Instance->InitParam(tMpeg4Info)) {
                        LOG_MM_E(PPL, "MP4 InitParam failed");
                        return AX_FALSE;
                    }

                    //  m_vecMpeg4Obs.emplace_back(CMPEG4Observer::NewInstance(pMp4Instance));
                    m_vecMpeg4Obs.emplace_back(CObserverMaker::CreateObserver<CMPEG4Observer>(pMp4Instance));
                    AX_APP_Audio_RegPacketObserver(nAudioMp4Chn, m_vecMpeg4Obs[m_vecMpeg4Obs.size() - 1].get());
                    pVencInstance->RegObserver(m_vecMpeg4Obs[m_vecMpeg4Obs.size() - 1].get());
                    m_vecMpeg4Instance.emplace_back(pMp4Instance);

                    SetWebRecordAttr(pMp4Instance, pConfig->nPipeSrc, nPrevChn);
                }

                m_vecVencInstance.emplace_back(pVencInstance);
            }
        } while (0);
    }

    WEB_AUDIO_ATTR_T tAudioAttr;
    tAudioAttr.fCapture_volume = (AX_F32)stAudioAttr.stCapAttr.stDevAttr.fVolume;
    tAudioAttr.fPlay_volume = (AX_F32)stAudioAttr.stPlayAttr.stDevAttr.fVolume;
    CWebServer::GetInstance()->EnableAudioPlay(APP_AUDIO_PLAY_AVAILABLE());
    CWebServer::GetInstance()->EnableAudioCapture(APP_AUDIO_CAP_AVAILABLE());
    CWebOptionHelper::GetInstance()->SetAudio(tAudioAttr);

    LOG_MM(PPL, "---");
    return AX_TRUE;
}

AX_BOOL CPanoBuilder::InitJenc() {
    LOG_MM(PPL, "+++");

    for (AX_U8 i = 0; i < MAX_JENC_CHANNEL_NUM; i++) {
        JPEG_CONFIG_T tJencCfg;

        do {
            if (APP_JENC_CONFIG(m_nScenario, i, tJencCfg)) {
                CJpegEncoder* pJencInstance = new CJpegEncoder(tJencCfg);
                if (!pJencInstance->Init()) {
                    break;
                }

                /* Step-8-2: IVPS register JENC observers and init JENC attributes according to IVPS attributes */
                PPL_MOD_INFO_T tDstMod = {E_PPL_MOD_TYPE_VENC, 0, pJencInstance->GetChannel()};
                vector<PPL_MOD_RELATIONSHIP_T> vecRelations;
                if (!GetRelationsByDstMod(tDstMod, vecRelations)) {
                    LOG_MM_E(PPL, "Can not find relation for dest module: (Module:%s, Grp:%d, Chn:%d)",
                             ModType2String(tDstMod.eModType).c_str(), tDstMod.nGroup, tDstMod.nChannel);
                    return AX_FALSE;
                }

                PPL_MOD_RELATIONSHIP_T& tRelation = vecRelations[0];
                JPEG_CONFIG_T* pConfig = pJencInstance->GetChnCfg();
                pConfig->nMaxWidth  = APP_ENCODER_PANO_MAX_WIDTH;
                pConfig->nMaxHeight = APP_ENCODER_PANO_MAX_HEIGHT;

                m_vecJencObs.emplace_back(CObserverMaker::CreateObserver<CJencObserver>(pJencInstance));
                if (E_PPL_MOD_TYPE_IVPS == tRelation.tSrcModChn.eModType) {
                    m_vecIvpsInstance[tRelation.tSrcModChn.nGroup]->RegObserver(tRelation.tSrcModChn.nChannel,
                                                                                m_vecJencObs[m_vecJencObs.size() - 1].get());
                } else if (E_PPL_MOD_TYPE_VIN == tRelation.tSrcModChn.eModType) {
                    m_mgrSensor.RegObserver(tRelation.tSrcModChn.nGroup, tRelation.tSrcModChn.nChannel,
                                            m_vecJencObs[m_vecJencObs.size() - 1].get());
                }

                if (tRelation.bLink) {
                    pConfig->bLink = AX_TRUE;
                }

                /* Step-8-3: Initialize JENC with latest attributes */
                if (!pJencInstance->InitParams()) {
                    return AX_FALSE;
                }

                /* Register unique capture channel, only the first setting will be effective */
                CWebServer::GetInstance()->RegistUniCaptureChn(pJencInstance->GetSensorSrc(), pJencInstance->GetChannel());

                /* Step-8-4: Register observers */
                m_vecWebObs.emplace_back(CObserverMaker::CreateObserver<CWebObserver>(CWebServer::GetInstance()));
                pJencInstance->RegObserver(m_vecWebObs[m_vecWebObs.size() - 1].get());

                /* Step-8-5: Save instance */
                m_vecJencInstance.emplace_back(pJencInstance);

                LOG_M_I(PPL, "Init jenc channel %d successfully.", i);
            }
        } while (0);
    }

    LOG_MM(PPL, "---");
    return AX_TRUE;
}

AX_BOOL CPanoBuilder::InitDetector() {
    LOG_MM(PPL, "+++");
    AX_U8 DetectFlag = 0;

    PPL_MOD_INFO_T tDstMod = {E_PPL_MOD_TYPE_DETECT, 0, 0};
    vector<PPL_MOD_RELATIONSHIP_T> vecRelations;
    if (GetRelationsByDstMod(tDstMod, vecRelations, AX_TRUE)) {
        for (auto& relation : vecRelations) {
            if (!relation.bLink) {
                if (E_PPL_MOD_TYPE_IVPS == relation.tSrcModChn.eModType) {
                    DetectFlag = 1;
                    m_vecDetectorObs.emplace_back(CObserverMaker::CreateObserver<CDetectObserver>(&m_detector));
                    m_vecIvpsInstance[relation.tSrcModChn.nGroup]->RegObserver(relation.tSrcModChn.nChannel,
                                                                               m_vecDetectorObs[m_vecDetectorObs.size() - 1].get());
                    CWebOptionHelper::GetInstance()->InitAiAttr(m_vecIvpsInstance[relation.tSrcModChn.nGroup]->GetGrpCfg()->nSnsSrc);
                } else if (E_PPL_MOD_TYPE_COLLECT == relation.tSrcModChn.eModType) {
                    /* Call InitAiAttr in the InitCollector function*/
                    DetectFlag = 1;
                } else {
                    LOG_MM_E(PPL, "Unsupport from %s to DETECTOR , please check the ppl.json configuration.",
                             ModType2String(relation.tSrcModChn.eModType));
                }
            } else {
                LOG_MM_E(PPL, "Detector does not support link mode, please check the ppl.json configuration.");
            }
        }
    }
    if (DetectFlag && CAlgoOptionHelper::GetInstance()->GetDetectAlgoType(0) > 0) {
        DETECTOR_ATTR_T tDetectAttr;
        tDetectAttr.strModelPath = ALGO_HVCFP_PARAM(0).strDetectModelsPath;
        tDetectAttr.ePPL = (AX_SKEL_PPL_E)CAlgoOptionHelper::GetInstance()->GetDetectAlgoType(0);
        tDetectAttr.nGrpCount = MAX_DETECTOR_GROUP_NUM;
        tDetectAttr.nGrp = tDstMod.nGroup;

        if (AX_FALSE == m_detector.Init(tDetectAttr)) {
            LOG_M_E(PPL, "Init detector failed.");
            return AX_FALSE;
        }

        if (ALGO_HVCFP_PARAM(0).bPushToWeb) {
            /* Register detector web observers */
            m_vecWebObs.emplace_back(CObserverMaker::CreateObserver<CWebObserver>(CWebServer::GetInstance()));
            m_detector.RegObserver(m_vecWebObs[m_vecWebObs.size() - 1].get());
        }

        m_vecOsdObs.emplace_back(CObserverMaker::CreateObserver<COsdObserver>(&m_vecIvpsInstance));
        m_detector.RegObserver(m_vecOsdObs[m_vecOsdObs.size() - 1].get());
    }

    LOG_MM(PPL, "---");
    return AX_TRUE;
}

AX_BOOL CPanoBuilder::InitIves() {
    for (int i = 0; i < MAX_IVES_GROUP_NUM; i++) {
        PPL_MOD_INFO_T tDstMod = {E_PPL_MOD_TYPE_IVES, i, 0};

        vector<PPL_MOD_RELATIONSHIP_T> vecRelations;
        if (GetRelationsByDstMod(tDstMod, vecRelations, AX_TRUE)) {
            for (auto& relation : vecRelations) {
                if (E_PPL_MOD_TYPE_IVPS == relation.tSrcModChn.eModType) {
                    CIVESStage* pIves = new CIVESStage();
                    m_vecIvesInstance.emplace_back(pIves);
                    m_vecIvesObs.emplace_back(CObserverMaker::CreateObserver<CIvesObserver>(pIves));
                    m_vecIvpsInstance[relation.tSrcModChn.nGroup]->RegObserver(relation.tSrcModChn.nChannel,
                                                                               m_vecIvesObs[m_vecIvesObs.size() - 1].get());
                    /*fixme: IVES requires a 640*360 resulution ,but INPLACE mode does not support resize */
                    m_vecIvpsInstance[relation.tSrcModChn.nGroup]->SetChnInplace(relation.tSrcModChn.nChannel, AX_FALSE);

                    CWebOptionHelper::GetInstance()->InitIvesAttr(tDstMod.nGroup, pIves);
                    CTestSuite::GetInstance()->InitIvesAttr(tDstMod.nGroup);

                    pIves->Init(tDstMod.nGroup);

                    // web event
                    m_vecWebObs.emplace_back(CObserverMaker::CreateObserver<CWebObserver>(CWebServer::GetInstance()));
                    pIves->RegObserver(m_vecWebObs[m_vecWebObs.size() - 1].get());
                }
            }
        }
    }

    return AX_TRUE;
}

AX_BOOL CPanoBuilder::InitCapture() {
    do {
        PPL_MOD_INFO_T tDstMod = {E_PPL_MOD_TYPE_CAPTURE, 0, 0};
        vector<PPL_MOD_RELATIONSHIP_T> vecRelations;
        if (GetRelationsByDstMod(tDstMod, vecRelations, AX_TRUE)) {
            for (const auto& relation : vecRelations) {
                if (relation.bLink || E_PPL_MOD_TYPE_COLLECT != relation.tSrcModChn.eModType) {
                    continue;
                }
                m_vecCaptureObs.emplace_back(CObserverMaker::CreateObserver<CCaptureObserver>(&m_capture));
            }
        }
    } while (0);

    return AX_TRUE;
}

AX_BOOL CPanoBuilder::InitVo() {
#if 0
    if (dispVoConfig.nDevId == -1) {
        return AX_TRUE;
    }

    if (AX_DISPDEV_TYPE::PRIMARY == enDispDev) {
        m_disp = make_unique<CVo>();
        if (!m_disp) {
            LOG_M_E(BOX, "%s: create display instance fail", __func__);
            return AX_FALSE;
        }
    } else if (AX_DISPDEV_TYPE::SECONDARY == enDispDev) {
        m_dispSecondary = make_unique<CVo>();
        if (!m_dispSecondary) {
            LOG_M_E(BOX, "%s: create secondary display instance fail", __func__);
            return AX_FALSE;
        }
    }

    VO_ATTR_T stAttr;
    stAttr.voDev = dispVoConfig.nDevId;
    stAttr.enIntfType = AX_VO_INTF_HDMI;
    stAttr.enIntfSync = (AX_VO_INTF_SYNC_E)dispVoConfig.nHDMI;
    stAttr.nBgClr = 0x000000;
    if (AX_DISPDEV_TYPE::SECONDARY == enDispDev) stAttr.nBgClr = 0x0000ff;
    stAttr.nLayerDepth = dispVoConfig.nLayerDepth;
    stAttr.nTolerance = dispVoConfig.nTolerance;
    stAttr.strResDirPath = dispVoConfig.strResDirPath;
    stAttr.bShowLogo = dispVoConfig.bShowLogo;
    stAttr.bShowNoVideo = dispVoConfig.bShowNoVideo;
    stAttr.arrChns.resize(nChnCount);
    for (auto&& m : stAttr.arrChns) {
        m.nPriority = 0;
        m.nDepth = dispVoConfig.nChnDepth;
        if (m.nDepth < 2) {
            m.nDepth = 2;
        }
    }

    if (AX_DISPDEV_TYPE::PRIMARY == enDispDev) {
        if (!m_disp->Init(stAttr)) {
            return AX_FALSE;
        }
        m_dispObserver = CObserverMaker::CreateObserver<CVoObserver>(m_disp.get(), DISPVO_CHN);
        if (!m_dispObserver) {
            LOG_M_E(BOX, "%s: create display observer instance fail", __func__);
            return AX_FALSE;
        }
    } else if (AX_DISPDEV_TYPE::SECONDARY == enDispDev) {
        if (!m_dispSecondary->Init(stAttr)) {
            return AX_FALSE;
        }
        m_dispObserverSecondary = CObserverMaker::CreateObserver<CVoObserver>(m_dispSecondary.get(), DISPVO_CHN);
        if (!m_dispObserverSecondary) {
            LOG_M_E(BOX, "%s: create display observer instance fail", __func__);
            return AX_FALSE;
        }
    }
#endif
    return AX_TRUE;
}

AX_BOOL CPanoBuilder::InitAvs(AX_VOID) {
    AX_APP_AVS_ATTR_T stAvsAttr;
    AX_APP_AVS_CFG_T stAVSCfg;
    if (!APP_AVS_CONFIG(m_nScenario, stAVSCfg)) {
        return  AX_FALSE;
    }

    stAvsAttr.bSyncPipe = stAVSCfg.bSyncPipe;
    stAvsAttr.enMode = (AX_AVS_MODE_E)stAVSCfg.u8Mode;
    stAvsAttr.eEngineMode = stAVSCfg.eEngineMode;
    stAvsAttr.enBlendMode = (AX_AVS_BLEND_MODE_E)stAVSCfg.u8BlendMode;
    stAvsAttr.enParamType = (AX_APP_AVS_PARAM_TYPE_E)stAVSCfg.u8ParamType;
    stAvsAttr.bDynamicSeam = stAVSCfg.bDynamicSeam;
    stAvsAttr.eProjectionMode = stAVSCfg.eProjectionMode;
    stAvsAttr.stAvsCompress = stAVSCfg.stAvsCompress;
    stAvsAttr.u8CaliEnable = stAVSCfg.u8CaliEnable;
    stAvsAttr.bPrioOnsiteCali = stAVSCfg.bPrioOnsiteCali;

    string strCaliDataPath = stAVSCfg.strParamFilePath;
    AX_U16 nPathLen = strCaliDataPath.length();
    if (strCaliDataPath.length() > (AX_AVSCALI_MAX_PATH_LEN - 1)) {
        LOG_MM_E(PPL, "strCaliDataPath(%s) length(%d) exceed %d.",
                      strCaliDataPath.c_str(), nPathLen, (AX_AVSCALI_MAX_PATH_LEN - 1));
        return AX_FALSE;
    }
    memcpy((void *)stAvsAttr.tCaliInitParam.strCaliDataPath, strCaliDataPath.c_str(), nPathLen);
    stAvsAttr.tCaliInitParam.tCallbacks.GeoCaliDoneCb = &CPanoBuilder::GeoCalibrateDone;
    stAvsAttr.tCaliInitParam.tCallbacks.AllCaliDoneCb = &CPanoBuilder::AllCalibrateDone;
    stAvsAttr.tCaliInitParam.pPrivData = (AX_VOID *)this;

    AX_U32 nISPchn = 0;
    PPL_MOD_INFO_T tDstMod = {E_PPL_MOD_TYPE_AVS, 0, 0};
    vector<PPL_MOD_RELATIONSHIP_T> vecRelations;
    if (!GetRelationsByDstMod(tDstMod, vecRelations, AX_TRUE)) {
        LOG_MM_E(PPL, "Can not find relation for dest module: (Module:%s, Grp:%d, Chn:%d)", ModType2String(tDstMod.eModType).c_str(),
                    tDstMod.nGroup, tDstMod.nChannel);
    }

    if (vecRelations.size() > 0) {
        nISPchn = vecRelations[0].tSrcModChn.nChannel;
    } else {
        LOG_MM_W(PPL, "Can not find relation, set nISPchn to 0 by default.");
    }

    AX_VIN_STITCH_GRP_ATTR_T tVinStitchAttr{0};
    AX_S32 nRet = AX_VIN_GetStitchGrpAttr(APP_VIN_STITCH_GRP, &tVinStitchAttr);
    if (AX_SUCCESS != nRet) {
        LOG_MM_E(PPL, "AX_VIN_GetStitchGrpAttr fail, ret:0x%x", nRet);
        return AX_FALSE;
    }

    AX_U8 nAVSPipeNum = tVinStitchAttr.nStitchPipeNum;

    stAvsAttr.tCaliInitParam.tPipeInfo.stPipeSeqInfo.nPipeNum = stAvsAttr.u8PipeNum = nAVSPipeNum;
    stAvsAttr.bAroundView = (1 == tVinStitchAttr.bAroundView) ? AX_TRUE : AX_FALSE;

    for (AX_U8 i = 0; i < nAVSPipeNum; i++) {
        stAvsAttr.tCaliInitParam.tPipeInfo.stPipeSeqInfo.arrPipeSeq[i] = tVinStitchAttr.tStitchPipeAttr[i].nPipeId;
        LOG_M_I(PPL, "Index(%d) -- Pipe(%d)", i, tVinStitchAttr.tStitchPipeAttr[i].nPipeId);
    }

    CBaseSensor* pSensor = m_mgrSensor.GetSnsInstance(0);
    AX_U8 nPipeID = pSensor->GetSnsConfig().arrPipeAttr->nPipeID;
    stAvsAttr.tCaliInitParam.tPipeInfo.nImgWidth  = pSensor->GetChnAttr(nPipeID, nISPchn).nWidth;
    stAvsAttr.tCaliInitParam.tPipeInfo.nImgHeight = pSensor->GetChnAttr(nPipeID, nISPchn).nHeight;
    stAvsAttr.tCaliInitParam.tPipeInfo.nChn       = nISPchn;

    LOG_M_I(PPL, "AVS Pipe number: %d, In img size(%d, %d), Isp chn id: %d",
                 stAvsAttr.tCaliInitParam.tPipeInfo.stPipeSeqInfo.nPipeNum,
                 stAvsAttr.tCaliInitParam.tPipeInfo.nImgWidth,
                 stAvsAttr.tCaliInitParam.tPipeInfo.nImgHeight,
                 stAvsAttr.tCaliInitParam.tPipeInfo.nChn);

    stAvsAttr.strCaliServerIP = stAVSCfg.strCaliServerIP;
    stAvsAttr.u16CaliServerPort = stAVSCfg.u16CaliServerPort;

    LOG_M_I(PPL, "u8PipeNum=%d, bSyncPipe=%d, enMode=%d, eEngineMode=%d, enBlendMode=%d, enParamType=%d, bDynamicSeam=%d, eProjectionMode=%d, avs param path: %s, compress(%d, %d)",
            stAvsAttr.u8PipeNum, stAvsAttr.bSyncPipe,stAvsAttr.enMode, stAvsAttr.eEngineMode, stAvsAttr.enBlendMode, stAvsAttr.enParamType,
            stAvsAttr.bDynamicSeam, stAvsAttr.eProjectionMode, stAvsAttr.tCaliInitParam.strCaliDataPath,
            stAvsAttr.stAvsCompress.enCompressMode, stAvsAttr.stAvsCompress.u32CompressLevel);

    if(!m_avs.Init(stAvsAttr)) {
        LOG_MM_E(PPL, "avs initializes failed!");
        return AX_FALSE;
    }

    AX_AVSCALI_AEAWB_SYNC_RATIO_INFO_T tCaliAeAwbSyncRatio = m_avs.GetAeAwbSyncRatio();
    std::map<AX_U8, AX_AVS_AEAWB_SYNC_RATIO_T> mapAeAwbSyncRatio;
    AX_U8 nRatioNum = stAvsAttr.bAroundView ? nAVSPipeNum : nAVSPipeNum - 1;
    for (AX_U8 i = 0; i < nRatioNum; i++) {
        AX_U8 nPipeId = stAvsAttr.tCaliInitParam.tPipeInfo.stPipeSeqInfo.arrPipeSeq[i];
        AX_AVS_AEAWB_SYNC_RATIO_T tAeAwbSyncRatio{0};
        tAeAwbSyncRatio.tAeSyncRatio.nAeSyncRatio = tCaliAeAwbSyncRatio.stAeAwbSyncRatioData[i].stAeSyncRatio.nAeSyncRatio;
        tAeAwbSyncRatio.tAwbSyncRatio.nRGainRatio = tCaliAeAwbSyncRatio.stAeAwbSyncRatioData[i].stAwbSyncRatio.nRGainRatio;
        tAeAwbSyncRatio.tAwbSyncRatio.nBGainRatio = tCaliAeAwbSyncRatio.stAeAwbSyncRatioData[i].stAwbSyncRatio.nBGainRatio;
        mapAeAwbSyncRatio[nPipeId] = tAeAwbSyncRatio;
    }
    m_mgrSensor.SetAeAwbSyncRatio(mapAeAwbSyncRatio);

    if (AVS_ENGINE_MULTI_VDSP == stAvsAttr.eEngineMode) {
        if (!InitVdsp()) {
            LOG_MM_E(PPL, "avs do InitVdsp failed!");
            return AX_FALSE;
        }

        m_bVdspOn = AX_TRUE;
    }

    return AX_TRUE;
}

AX_BOOL CPanoBuilder::InitCollector() {
    LOG_MM(PPL, "+++");
    for (AX_U8 i = 0; i < MAX_COLLECT_GROUP_NUM; i++) {
        PPL_MOD_INFO_T tDstMod = {E_PPL_MOD_TYPE_COLLECT, i, 0};
        vector<PPL_MOD_RELATIONSHIP_T> vecRelations;
        if (!GetRelationsByDstMod(tDstMod, vecRelations, AX_TRUE)) {
            LOG_MM_W(PPL, "Can not find relation for dest module: (Module:%s, Grp:%d, Chn:%d)", ModType2String(tDstMod.eModType).c_str(),
                     tDstMod.nGroup, tDstMod.nChannel);
            break;
        }
        CFrameCollector* pCollectorInstance = new CFrameCollector(i);
        if (!pCollectorInstance->Init()) {
            SAFE_DELETE_PTR(pCollectorInstance);
            return AX_FALSE;
        }
        m_vecCollectorInstance.emplace_back(pCollectorInstance);
    }

    /* Mulit Module to a Collect */
    for (AX_U8 i = 0; i < MAX_COLLECT_GROUP_NUM; i++) {
        PPL_MOD_INFO_T tDstMod = {E_PPL_MOD_TYPE_COLLECT, i, 0};
        vector<PPL_MOD_RELATIONSHIP_T> vecRelations;
        if (!GetRelationsByDstMod(tDstMod, vecRelations, AX_TRUE)) {
            LOG_MM_W(PPL, "Can not find relation for dest module: (Module:%s, Grp:%d, Chn:%d)", ModType2String(tDstMod.eModType).c_str(),
                     tDstMod.nGroup, tDstMod.nChannel);
            break;
        }

        for (auto& relation : vecRelations) {
            if (!relation.bLink) {
                /* Step-2*/
                if (E_PPL_MOD_TYPE_IVPS == relation.tSrcModChn.eModType) {
                    m_vecCollectorInstance[i]->RegTargetChannel(relation.tSrcModChn.nGroup, relation.tSrcModChn.nChannel);
                    m_vecCollectorObs.emplace_back(CObserverMaker::CreateObserver<CFrameCollectorObserver>(m_vecCollectorInstance[i]));
                    m_vecIvpsInstance[relation.tSrcModChn.nGroup]->RegObserver(relation.tSrcModChn.nChannel,
                                                                               m_vecCollectorObs[m_vecCollectorObs.size() - 1].get());
                    IVPS_GROUP_CFG_T* pIvpsConfig = m_vecIvpsInstance[relation.tSrcModChn.nGroup]->GetGrpCfg();
                    COLLECTOR_ATTR_T* pCollConfig = m_vecCollectorInstance[i]->GetCollectorCfg();
                    pCollConfig->nWidth = pIvpsConfig->arrChnResolution[relation.tSrcModChn.nChannel][0];
                    pCollConfig->nHeight = pIvpsConfig->arrChnResolution[relation.tSrcModChn.nChannel][1];
                    pCollConfig->fFramerate = pIvpsConfig->arrChnFramerate[relation.tSrcModChn.nChannel][1];
                    pCollConfig->arrFBC[0] = pIvpsConfig->arrChnFBC[relation.tSrcModChn.nChannel][0];
                    pCollConfig->arrFBC[1] = pIvpsConfig->arrChnFBC[relation.tSrcModChn.nChannel][1];
                    pCollConfig->nSnsSrc = pIvpsConfig->nSnsSrc;
                    LOG_MM_I(PPL, "collect  width:%d, height:%d, sensorSrc:%d, frameRate:%lf", pCollConfig->nWidth, pCollConfig->nHeight,
                             pCollConfig->nSnsSrc, pCollConfig->fFramerate);

                } else {
                    LOG_MM_W(PPL, "Collector only receives IVPS out frames now, please check the ppl.json configuration.");
                }
            } else {
                LOG_MM_E(PPL, "Collector not support link mode, please check the ppl.json configuration.");
            }
        }
    }

    /* A collect to mulit Module */
    for (AX_U8 i = 0; i < MAX_COLLECT_GROUP_NUM; i++) {
        PPL_MOD_INFO_T tSrcMod = {E_PPL_MOD_TYPE_COLLECT, i, 0};
        vector<PPL_MOD_RELATIONSHIP_T> vecRelations;
        if (!GetRelationsBySrcMod(tSrcMod, vecRelations, AX_TRUE)) {
            LOG_MM_W(PPL, "Can not find relation for dest module: (Module:%s, Grp:%d, Chn:%d)", ModType2String(tSrcMod.eModType).c_str(),
                     tSrcMod.nGroup, tSrcMod.nChannel);
            break;
        }
        for (auto& relation : vecRelations) {
            if (!relation.bLink) {
                if (E_PPL_MOD_TYPE_DETECT == relation.tDstModChn.eModType) {
                    m_vecDetectorObs.emplace_back(CObserverMaker::CreateObserver<CDetectObserver>(&m_detector));

                    if (!m_vecDetectorObs.empty()) {
                        m_vecCollectorInstance[i]->RegObserver(m_vecDetectorObs[m_vecDetectorObs.size() - 1].get());
                        /* fixme: The group of Collector is not equal to the sensor id*/
                        CWebOptionHelper::GetInstance()->InitAiAttr(m_vecCollectorInstance[i]->GetGroup());
                    }
                } else if (E_PPL_MOD_TYPE_CAPTURE == relation.tDstModChn.eModType) {
                    m_vecCollectorInstance[i]->RegObserver(m_vecCaptureObs[m_vecCaptureObs.size() - 1].get());
                } else if (E_PPL_MOD_TYPE_IVPS == relation.tDstModChn.eModType) {
                    m_vecCollectorInstance[i]->RegObserver(m_vecIvpsObs[relation.tDstModChn.nGroup].get());
                    m_vecIvpsInstance[relation.tDstModChn.nGroup]->InitParams();
                }
            } else {
                LOG_MM_E(PPL, "Collector not support link mode, please check the ppl.json configuration.");
            }
        }
    }

    RefleshIvps();

    LOG_MM(PPL, "---");
    return AX_TRUE;
}

AX_BOOL CPanoBuilder::InitPoolConfig() {
    LOG_MM(PPL, "+++");
    POOL_ATTR_T tAttr;
    tAttr.nMaxWidth = m_mgrSensor.GetSnsInstance(0)->GetSnsAttr().nWidth;
    tAttr.nMaxHeight = m_mgrSensor.GetSnsInstance(0)->GetSnsAttr().nHeight;
    tAttr.bRotatetion = AX_TRUE;

    m_pPoolConfig = new CPoolConfig(tAttr);
    if (nullptr == m_pPoolConfig) {
        return AX_FALSE;
    }

    LOG_MM(PPL, "---");
    return AX_TRUE;
}

AX_BOOL CPanoBuilder::InitVdsp() {
    AX_BOOL bRet = AX_FALSE;
    for (AX_U8 i = 0; i < DSP_ID_MAX; i++) {
        DSP_ATTR_S tDspCfg;
        if (!CDspCfgParser::GetInstance()->GetDspConfig(m_nScenario, i, tDspCfg)) {
            LOG_MM_W(PPL, "Get vdsp(%d) config for scenario(%d) failed!", i, m_nScenario);
            continue;
        }
        LOG_MM_I(PPL, "VDSP(%d) init, strSramPath: %s, strItcmPath: %s ",
                      tDspCfg.nGrp, tDspCfg.strSramPath.data(), tDspCfg.strItcmPath.data());
        bRet = CDspUtils::InitDsp((AX_DSP_ID_E)tDspCfg.nGrp, tDspCfg.strSramPath.data(), tDspCfg.strItcmPath.data());
        if (!bRet) {
            return AX_FALSE;
        }
    }

    return bRet;
}

AX_BOOL CPanoBuilder::DeInitVdsp() {
    if (!m_bVdspOn) {
        return AX_TRUE;
    }

    AX_BOOL bRet = AX_FALSE;
    for (AX_U8 i = 0; i < DSP_ID_MAX; i++) {
        DSP_ATTR_S tDspCfg;
        if (!CDspCfgParser::GetInstance()->GetDspConfig(m_nScenario, i, tDspCfg)) {
            LOG_MM_W(PPL, "Get vdsp(%d) config for scenario(%d) failed!", i, m_nScenario);
            continue;
        }

        bRet = CDspUtils::DeInitDsp((AX_DSP_ID_E)tDspCfg.nGrp);
        if (!bRet) {
            return AX_FALSE;
        }
    }

    m_bVdspOn = AX_FALSE;

    return bRet;
}

std::string CPanoBuilder::ModType2String(PPL_MODULE_TYPE_E eType) {
    switch (eType) {
        case E_PPL_MOD_TYPE_VIN: {
            return "VIN";
        }
        case E_PPL_MOD_TYPE_IVPS: {
            return "IVPS";
        }
        case E_PPL_MOD_TYPE_VENC: {
            return "VENC";
        }
        case E_PPL_MOD_TYPE_SNS_MANAGER: {
            return "SNS_MGR";
        }
        case E_PPL_MOD_TYPE_DETECT: {
            return "DETECT";
        }
        case E_PPL_MOD_TYPE_COLLECT: {
            return "COLLECT";
        }
        case E_PPL_MOD_TYPE_CAPTURE: {
            return "CAPTURE";
        }
        default:
            return "Unknown";
    }
}

AX_BOOL CPanoBuilder::Start(AX_VOID) {
    LOG_MM(PPL, "+++");

    if (!m_pPoolConfig->Start()) {
        LOG_M(PPL, "pool start failed.");
        return AX_FALSE;
    }

    if (!CWebServer::GetInstance()->Start()) {
        return AX_FALSE;
    }

    if (!CAXRtspServer::GetInstance()->Start()) {
        LOG_M_E(PPL, "Start rtsp server failed.");
        return AX_FALSE;
    }

    for (auto& pInstance : m_vecMpeg4Instance) {
        if (!pInstance->Start()) {
            return AX_FALSE;
        }
    }

    for (auto& pInstance : m_vecCollectorInstance) {
        if (!pInstance->Start()) {
            return AX_FALSE;
        }
    }

    if (!m_detector.Start()) {
        return AX_FALSE;
    }

    for (auto& pInstance : m_vecIvesInstance) {
        if (!pInstance->Start()) {
            return AX_FALSE;
        }
    }

    for (auto& pInstance : m_vecVencInstance) {
        STAGE_START_PARAM_T tStartParam;
        tStartParam.bStartProcessingThread = AX_FALSE;
        if (!pInstance->Start(&tStartParam)) {
            LOG_MM_E(PPL, "Start venc failed.");
            return AX_FALSE;
        }
    }

    for (auto& pInstance : m_vecJencInstance) {
        AX_BOOL bLinkMode = pInstance->GetChnCfg()->bLink;

        STAGE_START_PARAM_T tStartParam;
        tStartParam.bStartProcessingThread = bLinkMode ? AX_FALSE : AX_TRUE;
        if (!pInstance->Start(&tStartParam)) {
            LOG_MM_E(PPL, "Start jenc failed.");
            return AX_FALSE;
        }
    }

    for (auto& pInstance : m_vecIvpsInstance) {
        AX_BOOL bLinkMode = pInstance->GetGrpCfg()->nGrpLinkFlag == 0 ? AX_FALSE : AX_TRUE;

        STAGE_START_PARAM_T tStartParam;
        tStartParam.bStartProcessingThread = bLinkMode ? AX_FALSE : AX_TRUE;
        if (!pInstance->Start(&tStartParam)) {
            LOG_MM_E(PPL, "Start ivps failed.");
            return AX_FALSE;
        }
    }

    if (AX_FALSE == m_avs.Start()) {
        LOG_MM_W(PPL, "Start avs failed.");
    }

    if (AX_FALSE == m_mgrSensor.Start()) {
        LOG_MM_E(PPL, "Start sensor failed.");

        return AX_FALSE;
    }

#ifndef SLT
    AX_APP_Audio_Start();

    if (APP_AUDIO_PLAY_AVAILABLE()) {
        AX_APP_AUDIO_WELCOME_CFG_T stWelcomeConfig = APP_AUDIO_WELCOME_CONFIG();
        if (stWelcomeConfig.bEnable) {
            AX_APP_Audio_PlayFile(APP_AUDIO_LOCAL_PLAY_CHANNEL(), stWelcomeConfig.eType,
                                  stWelcomeConfig.strFileName.c_str(), 1, AudioPlayCallback, this);
        }
    }
#endif

    CPrintHelper::GetInstance()->Start();
    PostStartProcess();

    if (AX_FALSE == m_avs.StartAVSCalibrate()) {
        LOG_MM_E(PPL, "Start AVS calibrate failed.");
        return AX_FALSE;
    }

    LOG_MM(PPL, "---");

    return AX_TRUE;
}

AX_BOOL CPanoBuilder::Stop(AX_VOID) {
    LOG_MM(PPL, "+++");

    CPrintHelper::GetInstance()->Stop();

    if (!CWebServer::GetInstance()->Stop()) {
        return AX_FALSE;
    }

    for (auto& pInstance : m_vecMpeg4Instance) {
        if (!pInstance->Stop()) {
            return AX_FALSE;
        }
    }

    AX_APP_Audio_Stop();

    for (auto& pInstance : m_vecIvesInstance) {
        if (!pInstance->Stop()) {
            return AX_FALSE;
        }
    }

    m_avs.StopAVSCalibrate();

    if (!m_mgrSensor.Stop()) {
        return AX_FALSE;
    }

    if (!m_avs.Stop()) {
        return AX_FALSE;
    }

    for (auto& pInstance : m_vecIvpsInstance) {
        if (!pInstance->Stop()) {
            return AX_FALSE;
        }
    }

    for (auto& pInstance : m_vecVencInstance) {
        if (!pInstance->Stop()) {
            return AX_FALSE;
        }
    }

    for (auto& pInstance : m_vecJencInstance) {
        if (!pInstance->Stop()) {
            return AX_FALSE;
        }
    }

    for (auto& pInstance : m_vecCollectorInstance) {
        if (!pInstance->Stop()) {
            return AX_FALSE;
        }
    }

    if (!m_detector.Stop()) {
        return AX_FALSE;
    }

    if (!CAXRtspServer::GetInstance()->Stop()) {
        return AX_FALSE;
    }

    if (!m_pPoolConfig->Stop()) {
        return AX_FALSE;
    }

    LOG_MM(PPL, "---");

    return AX_TRUE;
}

AX_BOOL CPanoBuilder::Destroy(AX_VOID) {
    LOG_MM(PPL, "+++");

    if (!CWebServer::GetInstance()->DeInit()) {
        return AX_FALSE;
    }

    if (!CAXRtspServer::GetInstance()->DeInit()) {
        return AX_FALSE;
    }

    for (auto& pInstance : m_vecMpeg4Instance) {
        if (!pInstance->DeInit()) {
            return AX_FALSE;
        }
        SAFE_DELETE_PTR(pInstance);
    }

    for (auto& pInstance : m_vecIvpsInstance) {
        if (COptionHelper::GetInstance()->IsEnableOSD()) {
            pInstance->DetachOsdHelper();
        }

        if (!pInstance->DeInit()) {
            return AX_FALSE;
        }
        SAFE_DELETE_PTR(pInstance);
    }

    for (auto& pInstance : m_vecVencInstance) {
        if (!pInstance->DeInit()) {
            return AX_FALSE;
        }
        SAFE_DELETE_PTR(pInstance);
    }

    for (auto& pInstance : m_vecJencInstance) {
        if (!pInstance->DeInit()) {
            return AX_FALSE;
        }
        SAFE_DELETE_PTR(pInstance);
    }

    if (!m_avs.DeInit()) {
        return AX_FALSE;
    }

    if (!m_mgrSensor.DeInit()) {
        return AX_FALSE;
    }

#ifndef SLT
    AX_APP_Audio_Deinit();
#endif

    for (auto& pInstance : m_vecCollectorInstance) {
        if (!pInstance->DeInit()) {
            return AX_FALSE;
        }
        SAFE_DELETE_PTR(pInstance);
        pInstance = nullptr;
    }

    if (!m_detector.DeInit()) {
        return AX_FALSE;
    }

    for (auto& pInstance : m_vecIvesInstance) {
        if (!pInstance->DeInit()) {
            return AX_FALSE;
        }
        SAFE_DELETE_PTR(pInstance);
    }

    if (!m_linkage.Release()) {
        return AX_FALSE;
    }

    DeInitVdsp();

    SAFE_DELETE_PTR(m_pPoolConfig);

    if (!DeInitSysMods()) {
        return AX_FALSE;
    }

    LOG_MM(PPL, "---");
    return AX_TRUE;
}

AX_BOOL CPanoBuilder::ProcessWebOprs(WEB_REQUEST_TYPE_E eReqType, const AX_VOID* pJsonReq, AX_VOID** pResult /*= nullptr*/) {
    std::unique_lock<std::mutex> lck(m_mtxWebOpr, std::defer_lock);
    if (!lck.try_lock()) {
        LOG_MM_E(PPL, "Web Operation is processing.");
        return AX_FALSE;
    }

    m_vecWebOpr.clear();
    if (!CWebOptionHelper::GetInstance()->ParseWebRequest(eReqType, pJsonReq, m_vecWebOpr)) {
        LOG_MM_E(PPL, "Parse web request failed.");
        return AX_FALSE;
    }

    SortOperations(m_vecWebOpr);

    AX_U32 nOprSize = m_vecWebOpr.size();
    for (AX_U32 i = 0; i < nOprSize; i++) {
        /* Here only get the last operation results now */
        if (!DispatchOpr(m_vecWebOpr[i], pResult)) {
            LOG_MM_E(PPL, "Dispatch web operation failed.");
            return AX_FALSE;
        }
    }

    RestoreMp4Recording();

    return AX_TRUE;
}

AX_BOOL CPanoBuilder::DispatchOpr(WEB_REQ_OPERATION_T& tOperation, AX_VOID** pResult /*= nullptr*/) {
    AX_BOOL ret = AX_TRUE;
    WEB_OPERATION_TYPE_E eOperaType = tOperation.GetOperationType();
    switch (eOperaType) {
        case E_WEB_OPERATION_TYPE_SNS_MODE: {
            m_mgrSensor.SwitchSnsMode(tOperation.nSnsID, tOperation.tSnsMode.nSnsMode);
            break;
        }
        case E_WEB_OPERATION_TYPE_CAMERA_FPS: {
            m_mgrSensor.ChangeSnsFps(tOperation.nSnsID, tOperation.tSnsFpsAttr.fSnsFrameRate);
            break;
        }
        case E_WEB_OPERATION_TYPE_SNS_MIRROR_FLIP: {
            m_mgrSensor.ChangeSnsMirrorFlip(tOperation.nSnsID, tOperation.tSnsMirrorFlip.bMirror, tOperation.tSnsMirrorFlip.bFlip);
            break;
        }
        case E_WEB_OPERATION_TYPE_ROTATION: {
            // Not support for pano, do nothing
            break;
        }
        case E_WEB_OPERATION_TYPE_ENC_TYPE: {
            const AX_U8 nEncoderType = tOperation.tEncType.nEncoderType;
            AX_PAYLOAD_TYPE_E ePayloadType = CAXTypeConverter::Int2EncoderType(nEncoderType);
            CPrintHelper::GetInstance()->Reset(E_PH_MOD_VENC, tOperation.nChannel);

            for (auto& pVideoInstance : m_vecVencInstance) {
                if (pVideoInstance->GetChannel() == tOperation.nChannel) {
                    pVideoInstance->Stop();
                    CWebServer::GetInstance()->UpdateMediaTypeInPreviewChnMap(tOperation.nSnsID, tOperation.nChannel, nEncoderType);
                    pVideoInstance->UpdatePayloadType(ePayloadType);

                    pVideoInstance->StopRecv();
                    for (auto& obs : m_vecRtspObs) {
                        CAXRtspObserver* pRtstpObserver = static_cast<CAXRtspObserver*>(obs.get());
                        if (pRtstpObserver->IsSame(tOperation.nGroup, tOperation.nChannel)) {
                            pRtstpObserver->UpdateVideoPayloadType(AX_TRUE, ePayloadType);
                            break;
                        }
                    }

                    pVideoInstance->StartRecv();
                    break;
                }
            }

            CAXRtspServer::GetInstance()->RestartSessions();

            ret = AX_TRUE;
            break;
        }
        case E_WEB_OPERATION_TYPE_RESOLUTION: {
            /* Change IVPS & VENC channel's resolution */
            IPC_MOD_INFO_T tDstMod = {E_PPL_MOD_TYPE_VENC, 0, tOperation.nChannel};
            IPC_MOD_INFO_T tPrecedingMod;
            tPrecedingMod.eModType = E_PPL_MOD_TYPE_IVPS;
            AX_BOOL bFindResizeNode = AX_FALSE;
            std::vector<AX_U16> vecPrecedingGrp;

            while (GetPrecedingMod(tDstMod, tPrecedingMod)) {
                auto pInstance = m_vecIvpsInstance[tPrecedingMod.nGroup];

                if (pInstance->GetGrpCfg()->bArrChnResizeNode[tPrecedingMod.nChannel]) { // Find resize node
                    pInstance->RefreshOSDByResChange(AX_FALSE);
                    /* IVPS disable channel */
                    pInstance->EnableChannel(tPrecedingMod.nChannel, AX_FALSE);
                    /* IVPS update resolution */
                    pInstance->Resize(tPrecedingMod.nChannel, tOperation.tResolution.nWidth, tOperation.tResolution.nHeight);

                    /* OSD update position and size */
                    pInstance->RefreshOSDByResChange();

                    /* IVPS enable channel */
                    pInstance->EnableChannel(tPrecedingMod.nChannel, AX_TRUE);

                    bFindResizeNode = AX_TRUE;

                    break;
                } else {
                    vecPrecedingGrp.push_back(tPrecedingMod.nGroup);

                    pInstance->RefreshOSDByResChange();
                    for (AX_U8 nChn = 0; nChn < pInstance->GetGrpCfg()->nGrpChnNum; nChn++) {
                        pInstance->EnableChannel(nChn, AX_FALSE);
                    }
                }

                tDstMod = {E_PPL_MOD_TYPE_IVPS, tPrecedingMod.nGroup, tPrecedingMod.nChannel};
            }

            for (auto nPrecedingGrp : vecPrecedingGrp) {
                auto pInstance = m_vecIvpsInstance[nPrecedingGrp];
                pInstance->RefreshOSDByResChange(AX_TRUE);
                for (AX_U8 nChn = 0; nChn < pInstance->GetGrpCfg()->nGrpChnNum; nChn++) {
                    pInstance->EnableChannel(nChn, AX_TRUE);
                }
            }

            if (!bFindResizeNode) {
                LOG_MM_E(PPL, "Can not find preceding ivps module for venc unique channel %d.", tOperation.nChannel);
                ret = AX_FALSE;
            }

            break;
        }
        case E_WEB_OPERATION_TYPE_BITRATE: {
            WEB_VIDEO_ATTR_T tAttr;
            AX_BOOL bFound = CWebOptionHelper::GetInstance()->GetVideoByUniChn(tOperation.nSnsID, tOperation.nChannel, tAttr);
            if (!bFound) {
                LOG_MM_E(PPL, "Can not find vodeo information for venc unique channel %d.", tOperation.nChannel);
                ret = AX_FALSE;
                break;
            }
            /* Change VENC channel's bitrate */
            CVideoEncoder* pVencInstance = nullptr;
            for (auto& pInstance : m_vecVencInstance) {
                if (pInstance->GetChannel() == tAttr.nUniChn) {
                    pVencInstance = pInstance;
                }
            }

            if (nullptr != pVencInstance) {
                pVencInstance->UpdateChnBitrate(tOperation.tBitrate.nBitrate);
                break;
            } else {
                ret = AX_FALSE;
            }
            break;
        }
        case E_WEB_OPERATION_TYPE_RC_INFO: {
            WEB_VIDEO_ATTR_T tAttr;
            AX_BOOL bFound = CWebOptionHelper::GetInstance()->GetVideoByUniChn(tOperation.nSnsID, tOperation.nChannel, tAttr);
            if (!bFound) {
                LOG_MM_E(PPL, "Can not find vodeo information for venc unique channel %d.", tOperation.nChannel);
                ret = AX_FALSE;
                break;
            }
            /* Change VENC channel's rc info */
            CVideoEncoder* pVencInstance = nullptr;
            for (auto& pInstance : m_vecVencInstance) {
                if (pInstance->GetChannel() == tOperation.nChannel) {
                    pVencInstance = pInstance;
                }
            }

            if (nullptr != pVencInstance) {
                RC_INFO_T tRcInfo;

                tRcInfo.eRcType = tOperation.tRcInfo.eRcType;
                tRcInfo.nMinQp = tOperation.tRcInfo.nMinQp;
                tRcInfo.nMaxQp = tOperation.tRcInfo.nMaxQp;
                tRcInfo.nMinIQp = tOperation.tRcInfo.nMinIQp;
                tRcInfo.nMaxIQp = tOperation.tRcInfo.nMaxIQp;
                tRcInfo.nMinIProp = tOperation.tRcInfo.nMinIProp;
                tRcInfo.nMaxIProp = tOperation.tRcInfo.nMaxIProp;
                tRcInfo.nBitrate = tOperation.tRcInfo.nBitrate;
                LOG_MM_D(PPL, "rcInfo [eRcType:%d, nMinQp:%d,nMaxQp:%d,nMinIQp:%d,nMaxIQp:%d ,nMinIProp:%d,nMaxIProp:%d]", tRcInfo.eRcType,
                         tRcInfo.nMinQp, tRcInfo.nMaxQp, tRcInfo.nMinIQp, tRcInfo.nMaxIQp, tRcInfo.nMinIProp, tRcInfo.nMaxIProp);
                ret = pVencInstance->UpdateRcInfo(tRcInfo);
                tAttr.SetEncRcCfg(tRcInfo);
                CWebOptionHelper::GetInstance()->SetVideoByUniChn(tOperation.nSnsID, tAttr);
            } else {
                ret = AX_FALSE;
            }
            break;
        }
        case E_WEB_OPERATION_TYPE_DAYNIGHT: {
            AX_U32 nSensorCount = m_mgrSensor.GetSensorCount();
            for (AX_U32 i = 0; i < nSensorCount; i++) {
                CBaseSensor* pSensor = m_mgrSensor.GetSnsInstance(i);
                if (nullptr != pSensor) {
                    ret = pSensor->ChangeDaynightMode((AX_DAYNIGHT_MODE_E)tOperation.tDaynight.nDayNightMode);
                }
            }
            break;
        }
        case E_WEB_OPERATION_TYPE_IMAGE_ATTR: {
            CBaseSensor* pSensor = m_mgrSensor.GetSnsInstance(tOperation.nSnsID);
            if (nullptr != pSensor) {
                if (0 == tOperation.tImageAttr.nAutoMode) {
                    /* manual mode*/
                    APP_ISP_IMAGE_ATTR_T tImgAttr;
                    tImgAttr.nAutoMode = tOperation.tImageAttr.nAutoMode;
                    tImgAttr.nSharpness = tOperation.tImageAttr.nSharpness;
                    tImgAttr.nBrightness = tOperation.tImageAttr.nBrightness;
                    tImgAttr.nContrast = tOperation.tImageAttr.nContrast;
                    tImgAttr.nSaturation = tOperation.tImageAttr.nSaturation;

                    ret = pSensor->SetIspIQAttr(tImgAttr);
                } else {
                    /* auto mode */
                    WEB_CAMERA_ATTR_T tCamera = CWebOptionHelper::GetInstance()->GetCamera(tOperation.nSnsID);

                    APP_ISP_IMAGE_ATTR_T tImgAttr;
                    tImgAttr.nAutoMode = tOperation.tImageAttr.nAutoMode;
                    ret = pSensor->RestoreIspIQAttr(tImgAttr);

                    /* update web imageAttr value */
                    tCamera.tImageAttr.nAutoMode = tOperation.tImageAttr.nAutoMode;
                    tCamera.tImageAttr.nSharpness = tImgAttr.nSharpness;
                    tCamera.tImageAttr.nBrightness = tImgAttr.nBrightness;
                    tCamera.tImageAttr.nContrast = tImgAttr.nContrast;
                    tCamera.tImageAttr.nSaturation = tImgAttr.nSaturation;
                    CWebOptionHelper::GetInstance()->SetCamera(tOperation.nSnsID, tCamera);
                }
            }
            break;
        }
        case E_WEB_OPERATION_TYPE_LDC_ATTR: {
            for (auto pInstance : m_vecIvpsInstance) {
                if (pInstance->GetSensorSrc() == tOperation.nSnsID) {
                    IVPS_GROUP_CFG_T *pCfg = pInstance->GetGrpCfg();
                    if (pCfg && pCfg->nGrp == 0) {
                        for (AX_U8 nChn = 0; nChn < pCfg->nGrpChnNum; nChn++) {
                            pInstance->EnableChannel(nChn, AX_FALSE);
                        }
                        if (pInstance->UpdateGrpLDC(tOperation.tLdcAttr.bLdcEnable,
                                                tOperation.tLdcAttr.bLdcAspect,
                                                tOperation.tLdcAttr.nXRatio,
                                                tOperation.tLdcAttr.nYRatio,
                                                tOperation.tLdcAttr.nXYRatio,
                                                tOperation.tLdcAttr.nDistorRatio)) {
                            WEB_CAMERA_ATTR_T tCamera = CWebOptionHelper::GetInstance()->GetCamera(tOperation.nSnsID);
                            /* update web ldcAttr value */
                            tCamera.tLdcAttr.bLdcEnable = tOperation.tLdcAttr.bLdcEnable;
                            tCamera.tLdcAttr.bLdcAspect = tOperation.tLdcAttr.bLdcAspect;
                            tCamera.tLdcAttr.nXRatio = tOperation.tLdcAttr.nXRatio;
                            tCamera.tLdcAttr.nYRatio = tOperation.tLdcAttr.nYRatio;
                            tCamera.tLdcAttr.nXYRatio = tOperation.tLdcAttr.nXYRatio;
                            tCamera.tLdcAttr.nDistorRatio = tOperation.tLdcAttr.nDistorRatio;
                            CWebOptionHelper::GetInstance()->SetCamera(tOperation.nSnsID, tCamera);
                        }

                        for (AX_U8 nChn = 0; nChn < pCfg->nGrpChnNum; nChn++) {
                            pInstance->EnableChannel(nChn, AX_TRUE);
                        }
                        break;
                    }
                }
            }
            break;
        }
        case E_WEB_OPERATION_TYPE_NR_MODE: {
            // m_mgrSensor.ChangeNRMode(tOperation.tNR.nNRMode);
            LOG_MM_W(PPL, "E_WEB_OPERATION_TYPE_NR_MODE unsupport.");
            ret = AX_FALSE;
            break;
        }
        case E_WEB_OPERATION_TYPE_CAPTURE_AUTO: {
            CWebServer::GetInstance()->EnableCaptrue(tOperation.nSnsID, tOperation.tCapEnable.bOn);
            break;
        }
        case E_WEB_OPERATION_TYPE_AI_ENABLE: {
            m_detector.Enable(tOperation.nSnsID, tOperation.tAiEnable.bOn);

            for (auto& pInstance : m_vecIvpsInstance) {
                if (pInstance->GetGrpCfg()->nSnsSrc == tOperation.nSnsID) {
                    if (pInstance->GetOsdHelper()) {
                        pInstance->GetOsdHelper()->EnableAiRegion(tOperation.tAiEnable.bOn);
                    }
                }
            }
            break;
        }
        case E_WEB_OPERATION_TYPE_AI_PUSH_MODE: {
            m_detector.SetSkelPushMode(tOperation.tAiPushStategy);
            break;
        }
        case E_WEB_OPERATION_TYPE_AI_EVENT: {
            if (-1 == tOperation.nSnsID) {
                for (auto& pInstance : m_vecIvesInstance) {
                    if (!((tOperation.tEvent.tMD.bEnable == AX_FALSE) && (pInstance->GetMDCapacity() == tOperation.tEvent.tMD.bEnable))) {
                        pInstance->SetMDCapacity((AX_BOOL)tOperation.tEvent.tMD.bEnable);
                        if (AX_TRUE == pInstance->GetMDCapacity()) {
                            for (AX_S32 i = 0; i < pInstance->GetMDInstance()->GetAreaCount(); i++) {
                                pInstance->GetMDInstance()->SetThresholdY(tOperation.nSnsID, i, tOperation.tEvent.tMD.nThrsHoldY,
                                                                          tOperation.tEvent.tMD.nConfidence);
                            }
                        }
                    }

                    if (!((tOperation.tEvent.tOD.bEnable == AX_FALSE) && (pInstance->GetODCapacity() == tOperation.tEvent.tOD.bEnable))) {
                        pInstance->SetODCapacity((AX_BOOL)tOperation.tEvent.tOD.bEnable);
                        if (AX_TRUE == pInstance->GetODCapacity()) {
                            for (AX_S32 i = 0; i < pInstance->GetODInstance()->GetAreaCount(); i++) {
                                pInstance->GetODInstance()->SetThresholdY(tOperation.nSnsID, i, tOperation.tEvent.tOD.nThrsHoldY,
                                                                          tOperation.tEvent.tOD.nConfidence);
                            }
                        }
                    }

                    if (!((tOperation.tEvent.tOD.bEnable == AX_FALSE) && (pInstance->GetSCDCapacity() == tOperation.tEvent.tSCD.bEnable))) {
                        pInstance->SetSCDCapacity((AX_BOOL)tOperation.tEvent.tSCD.bEnable);
                        if (AX_TRUE == pInstance->GetSCDCapacity()) {
                            pInstance->GetSCDInstance()->SetThreshold(tOperation.nSnsID,
                                                                      tOperation.tEvent.tSCD.nThrsHoldY,
                                                                      tOperation.tEvent.tSCD.nConfidence);
                        }
                    }
                }
            } else {
                CIVESStage* pInstance{nullptr};
                try {
                    pInstance = m_vecIvesInstance.at(tOperation.nSnsID);
                } catch (std::out_of_range& e) {
                    LOG_MM_E(PPL, "nSnsID out of range %d. error: %s", tOperation.nSnsID, e.what());
                    ret = AX_FALSE;
                    break;
                }

                if (!((tOperation.tEvent.tMD.bEnable == AX_FALSE) && (pInstance->GetMDCapacity() == tOperation.tEvent.tMD.bEnable))) {
                    pInstance->SetMDCapacity((AX_BOOL)tOperation.tEvent.tMD.bEnable);
                    if (AX_TRUE == pInstance->GetMDCapacity()) {
                        for (AX_S32 i = 0; i < pInstance->GetMDInstance()->GetAreaCount(); i++) {
                            pInstance->GetMDInstance()->SetThresholdY(tOperation.nSnsID, i, tOperation.tEvent.tMD.nThrsHoldY,
                                                                      tOperation.tEvent.tMD.nConfidence);
                        }
                    }
                }

                if (!((tOperation.tEvent.tOD.bEnable == AX_FALSE) && (pInstance->GetODCapacity() == tOperation.tEvent.tOD.bEnable))) {
                    pInstance->SetODCapacity((AX_BOOL)tOperation.tEvent.tOD.bEnable);
                    if (AX_TRUE == pInstance->GetODCapacity()) {
                        for (AX_S32 i = 0; i < pInstance->GetODInstance()->GetAreaCount(); i++) {
                            pInstance->GetODInstance()->SetThresholdY(tOperation.nSnsID, i, tOperation.tEvent.tOD.nThrsHoldY,
                                                                      tOperation.tEvent.tOD.nConfidence);
                        }
                    }
                }

                if (!((tOperation.tEvent.tOD.bEnable == AX_FALSE) && (pInstance->GetSCDCapacity() == tOperation.tEvent.tSCD.bEnable))) {
                    pInstance->SetSCDCapacity((AX_BOOL)tOperation.tEvent.tSCD.bEnable);
                    if (AX_TRUE == pInstance->GetSCDCapacity()) {
                        pInstance->GetSCDInstance()->SetThreshold(tOperation.nSnsID, tOperation.tEvent.tSCD.nThrsHoldY, tOperation.tEvent.tSCD.nConfidence);
                    }
                }
            }
            break;
        }
        case E_WEB_OPERATION_TYPE_OSD_ATTR: {
            for (auto& pInstance : m_vecIvpsInstance) {
                if (pInstance->GetGrpCfg()->nSnsSrc == tOperation.nSnsID && pInstance->GetGrpCfg()->nGrp == tOperation.nGroup) {
                    pInstance->GetOsdHelper()->Refresh();
                }
            }
            break;
        }
        case E_WEB_OPERATION_TYPE_CHANNEL_SWITCH: {
            /* Open or Close IVPS channel */
            PPL_MOD_INFO_T tDstMod;
            if (1 == tOperation.tChnSwitch.nEncoderType) {
                tDstMod = {E_PPL_MOD_TYPE_MJENC, 0, tOperation.nChannel};
            } else {
                tDstMod = {E_PPL_MOD_TYPE_VENC, 0, tOperation.nChannel};
            }

            PPL_MOD_INFO_T tPrecedingMod;
            tPrecedingMod.eModType = E_PPL_MOD_TYPE_IVPS;
            if (GetPrecedingMod(tDstMod, tPrecedingMod)) {
                /* IVPS disable channel */
                m_vecIvpsInstance[tPrecedingMod.nGroup]->EnableChannel(tPrecedingMod.nChannel, tOperation.tChnSwitch.bOn);
            } else {
                LOG_MM_E(PPL, "Can not find preceding ivps module for venc/mjpeg unique channel %d.", tOperation.nChannel);
                ret = AX_FALSE;
            }
            break;
        }
        case E_WEB_OPERATION_TYPE_GET_RESOLUTION: {
            WEB_VIDEO_ATTR_T tAttr;
            AX_BOOL bFound = CWebOptionHelper::GetInstance()->GetVideoByUniChn(tOperation.nSnsID, tOperation.nChannel, tAttr);
            if (!bFound) {
                LOG_MM_E(PPL, "Can not find vodeo information for venc unique channel %d.", tOperation.nChannel);
                ret = AX_FALSE;
                break;
            }

            // IVPS_GRP nGrp = g_stageIVPS.GetGrpByVencChn(nChan);
            // IVPS_CHN nChn = g_stageIVPS.GetGrpInnerChnByVencChn(nChan);

            // WEB_CAMERA_ATTR_T tCameraAttr = GetCamera();
            // if (AX_IVPS_ROTATION_90 == tCameraAttr.nRotation || AX_IVPS_ROTATION_270 == tCameraAttr.nRotation) {
            //     AX_U8 nUniqueChn = g_stageIVPS.GetUniqueChn(nGrp, nChn);
            //     ::swap(tAttr.width, tAttr.height);
            //     if (gOptions.IsIvpsChannelFBCEnabled(g_tEPOptions[nUniqueChn].nChannel)) {
            //         tAttr.width = ALIGN_UP(tAttr.width, AX_ENCODER_FBC_WIDTH_ALIGN_VAL);
            //     }
            // }

            /* TODO: Resolution relies on rotation and fbc configurations */
            tOperation.tGetResolution.nWidth = tAttr.nWidth;
            tOperation.tGetResolution.nHeight = tAttr.nHeight;

            sprintf((AX_CHAR*)pResult, "%dx%d", tAttr.nWidth, tAttr.nHeight);

            break;
        }
        case E_WEB_OPERATION_TYPE_GET_TITLE: {
            string strConfigDir = CCommonUtils::GetPPLSpecification();
            sprintf((AX_CHAR*)pResult, "%s", strConfigDir.c_str());
            break;
        }
        case E_WEB_OPERATION_TYPE_CAPTURE:
            m_capture.CapturePicture(tOperation.nGroup, tOperation.nChannel, 80, pResult);
            ret = AX_TRUE;
            break;
        case E_WEB_OPERATION_TYPE_AUDIO_ATTR: {
            AX_APP_Audio_SetCapVolume((AX_F32)tOperation.tAudioAttr.nCapture_volume);
            AX_APP_Audio_SetPlayVolume((AX_F32)tOperation.tAudioAttr.nPlay_volume);

            ret = AX_TRUE;
            break;
        }
        case E_WEB_OPERATION_TYPE_SWITCH_3A_SYNCRATIO: {
            m_mgrSensor.EnableAeAwbSyncRatio(tOperation.b3ASyncRationOn);
            break;
        }
        case E_WEB_OPERATION_TYPE_RECORDER_SWITCH: {
            for (auto& pInstance : m_vecMpeg4Instance) {
                if (pInstance->GetSnsId() == tOperation.nSnsID
                    && pInstance->GetPreviewChn() == tOperation.nChannel) {
                    mp4_record_type_e eRecordType = (tOperation.tRecorderSwitch.nRecordType == 0)
                                                    ? MP4_RECORD_TYPE_MP4 : MP4_RECORD_TYPE_RAW;
                    pInstance->SwitchRecorder(tOperation.tRecorderSwitch.bOn, eRecordType, AX_FALSE);
                    break;
                }
            }
            break;
        }

        default:
            LOG_MM_E(PPL, "eReqType:%d is nonsupport.", eOperaType);
            ret = AX_FALSE;
            break;
    }

    if (ret && E_REQ_TYPE_VIDEO == tOperation.eReqType) {
        /*notify web restart preview*/
        WEB_EVENTS_DATA_T event;
        event.eType = E_WEB_EVENTS_TYPE_ReStartPreview;
        event.nReserved = tOperation.nSnsID;
        CWebServer::GetInstance()->SendEventsData(&event);
    }

    return ret;
}

AX_VOID CPanoBuilder::PostStartProcess(AX_VOID) {
    LOG_MM_D(PPL, "+++");
    AX_U32 nSnsCnt = m_mgrSensor.GetSensorCount();
    for (AX_U32 i = 0; i < nSnsCnt; i++) {
        CBaseSensor* pSensor = m_mgrSensor.GetSnsInstance(i);

        WEB_CAMERA_ATTR_T tCamera = CWebOptionHelper::GetInstance()->GetCamera(i);
        APP_ISP_IMAGE_ATTR_T tImgAttr;
        pSensor->GetIspIQAttr(tImgAttr);
        tCamera.tImageAttr.nAutoMode = tImgAttr.nAutoMode;
        tCamera.tImageAttr.nSharpness = tImgAttr.nSharpness;
        tCamera.tImageAttr.nBrightness = tImgAttr.nBrightness;
        tCamera.tImageAttr.nContrast = tImgAttr.nContrast;
        tCamera.tImageAttr.nSaturation = tImgAttr.nSaturation;
        CWebOptionHelper::GetInstance()->SetCamera(i, tCamera);
    }
}

AX_BOOL CPanoBuilder::ProcessTestSuiteOpers(WEB_REQ_OPERATION_T& tOperation) {
    return DispatchOpr(tOperation, nullptr);
}

AX_VOID CPanoBuilder::GeoCalibrateDone(const AX_AVSCALI_GEO_CALIDONE_RESULT_T* pCaliDoneResult) {
    if (AX_SUCCESS != pCaliDoneResult->nResult) {
        LOG_MM_W(PPL, "Geometric calibration failure, s32Result 0x%x", pCaliDoneResult->nResult);
        return;
    }

    if (nullptr == pCaliDoneResult->pPrivData) {
        LOG_MM_E(PPL, "Error, pPrivData is null!");
        return;
    }

    CPanoBuilder* pPanoBuilder = (CPanoBuilder *)pCaliDoneResult->pPrivData;

    AX_APP_AVS_RESOLUTION_T stPreAvsResolution = pPanoBuilder->m_avs.GetAvsResolution();

    pPanoBuilder->m_avs.Stop();
    pPanoBuilder->m_avs.LoadParam(AX_TRUE);

    AX_APP_AVS_RESOLUTION_T stAvsResolution = pPanoBuilder->m_avs.GetAvsResolution();

    LOG_MM_C(PPL, "Calibration completed, preResolution(%d, %d), curResolution(%d, %d), bUpdateOverlap: %d",
                   stPreAvsResolution.u32Width, stPreAvsResolution.u32Height,
                   stAvsResolution.u32Width, stAvsResolution.u32Height,
                   pCaliDoneResult->bUpdateOverlap);

    if (stPreAvsResolution.u32Width != stAvsResolution.u32Width
        || stPreAvsResolution.u32Height != stAvsResolution.u32Height) {
        PPL_MOD_INFO_T tSrcMod = {E_PPL_MOD_TYPE_AVS, 0, 0};
        pPanoBuilder->UpdatePipeMainResolution(tSrcMod, stAvsResolution.u32Width, stAvsResolution.u32Height);

        SNS_TYPE_E eSnsType = pPanoBuilder->m_mgrSensor.GetSnsInstance(0)->GetSnsConfig().eSensorType;
        CWebOptionHelper::GetInstance()->SetRes2ResOption(eSnsType, 0, 0,
                                                        stAvsResolution.u32Width,
                                                        stAvsResolution.u32Height);
        CWebOptionHelper::GetInstance()->GetSns2VideoAttr()[0][0].nWidth = stAvsResolution.u32Width;
        CWebOptionHelper::GetInstance()->GetSns2VideoAttr()[0][0].nHeight = stAvsResolution.u32Height;
    }

    pPanoBuilder->m_avs.Start();

    if (pCaliDoneResult->bUpdateOverlap) {
        if (AX_FALSE == pPanoBuilder->m_mgrSensor.UpdateVinStitchAttr()) {
            LOG_MM_E(PPL, "SensorMgr UpdateVinStitchAttr failed!");
        }
    }

    WEB_EVENTS_DATA_T event;
    event.eType = E_WEB_EVENTS_TYPE_ReStartPreview;
    event.nReserved = 0;
    CWebServer::GetInstance()->SendEventsData(&event);
}

AX_VOID CPanoBuilder::AllCalibrateDone(const AX_AVSCALI_ALL_CALIDONE_RESULT_T* pCaliDoneResult) {
    LOG_MM_C(PPL, "AllCalibrateDone result, nResult = %d, LscCaliIgnore = %d, GeoCaliIgnore = %d, AeAwbCaliIgnore = %d, IsOnSiteCali = %d",
                   pCaliDoneResult->nResult, pCaliDoneResult->bLscCaliIgnore,
                   pCaliDoneResult->bGeoCaliIgnore, pCaliDoneResult->bAeAwbCaliIgnore, pCaliDoneResult->bIsOnSiteCali);
}

AX_VOID CPanoBuilder::SortOperations(vector<WEB_REQ_OPERATION_T>& vecWebRequests) {
    std::sort(vecWebRequests.begin(), vecWebRequests.end(),
              [](WEB_REQ_OPERATION_T t1, WEB_REQ_OPERATION_T t2) { return t1.nPriority > t2.nPriority; });
}

AX_BOOL CPanoBuilder::InitSysMods(AX_VOID) {
    LOG_MM(PPL, "+++");

    m_arrMods.clear();
    m_arrMods.push_back({AX_FALSE, "LOG", bind(&CPanoBuilder::APP_LOG_Init, this), bind(&CPanoBuilder::APP_LOG_DeInit, this)});
    m_arrMods.push_back({AX_FALSE, "SYS", bind(&CPanoBuilder::APP_SYS_Init, this), bind(&CPanoBuilder::APP_SYS_DeInit, this)});
    m_arrMods.push_back({AX_FALSE, "VENC", bind(&CPanoBuilder::APP_VENC_Init, this), bind(&CPanoBuilder::APP_VENC_DeInit, this)});
    m_arrMods.push_back({AX_FALSE, "NPU", bind(&CPanoBuilder::APP_NPU_Init, this), bind(&CPanoBuilder::APP_NPU_DeInit, this)});
    m_arrMods.push_back({AX_FALSE, "VIN", AX_VIN_Init, AX_VIN_Deinit});
    m_arrMods.push_back({AX_FALSE, "MIPI_RX", AX_MIPI_RX_Init, AX_MIPI_RX_DeInit});
    m_arrMods.push_back({AX_FALSE, "IVPS", AX_IVPS_Init, AX_IVPS_Deinit});
    m_arrMods.push_back({AX_FALSE, "ACAP", bind(&CPanoBuilder::APP_ACAP_Init, this), bind(&CPanoBuilder::APP_ACAP_DeInit, this)});
    m_arrMods.push_back({AX_FALSE, "APLAY", bind(&CPanoBuilder::APP_APLAY_Init, this), bind(&CPanoBuilder::APP_APLAY_DeInit, this)});

    for (auto& m : m_arrMods) {
        AX_S32 ret = m.Init();
        if (0 != ret) {
            LOG_MM_E(PPL, "%s: init module %s fail, ret = 0x%x", __func__, m.strName.c_str(), ret);
            return AX_FALSE;
        } else {
            LOG_MM_W(PPL, "%s: init module %s OK", __func__, m.strName.c_str());
            m.bInited = AX_TRUE;
        }
    }

    CIVESStage::InitModule();

    LOG_MM(PPL, "---");

    return AX_TRUE;
}

AX_BOOL CPanoBuilder::GetRelationsBySrcMod(PPL_MOD_INFO_T& tSrcMod, vector<PPL_MOD_RELATIONSHIP_T>& vecOutRelations,
                                          AX_BOOL bIgnoreChn /*= AX_FALSE*/) const {
    return m_linkage.GetRelationsBySrcMod(tSrcMod, vecOutRelations, bIgnoreChn);
}

AX_BOOL CPanoBuilder::GetRelationsByDstMod(PPL_MOD_INFO_T& tDstMod, vector<PPL_MOD_RELATIONSHIP_T>& vecOutRelations,
                                          AX_BOOL bIgnoreChn /*= AX_FALSE*/) const {
    return m_linkage.GetRelationsByDstMod(tDstMod, vecOutRelations, bIgnoreChn);
}

AX_BOOL CPanoBuilder::GetPrecedingMod(const PPL_MOD_INFO_T& tDstMod, PPL_MOD_INFO_T& tPrecedingMod) {
    return m_linkage.GetPrecedingMod(tDstMod, tPrecedingMod);
}

AX_BOOL CPanoBuilder::DeInitSysMods(AX_VOID) {
    CIVESStage::DeinitModule();

    const auto nSize = m_arrMods.size();
    if (0 == nSize) {
        return AX_TRUE;
    }

    for (AX_S32 i = (AX_S32)(nSize - 1); i >= 0; --i) {
        if (m_arrMods[i].bInited) {
            AX_S32 ret = m_arrMods[i].DeInit();
            if (0 != ret) {
                LOG_MM_E(PPL, "%s: deinit module %s fail, ret = 0x%x", __func__, m_arrMods[i].strName.c_str(), ret);
                return AX_FALSE;
            } else {
                LOG_MM_W(PPL, "%s: deinit module %s OK", __func__, m_arrMods[i].strName.c_str());
            }

            m_arrMods[i].bInited = AX_FALSE;
        }
    }

    m_arrMods.clear();
    return AX_TRUE;
}

AX_S32 CPanoBuilder::APP_SYS_Init() {
    AX_S32 nRet = AX_SUCCESS;

    nRet = AX_SYS_Init();
    if (0 != nRet) {
        return nRet;
    }

    AX_APP_Log_SetSysModuleInited(AX_TRUE);

    nRet = AX_POOL_Exit();
    if (0 != nRet) {
        return nRet;
    }

    return AX_SUCCESS;
}

AX_S32 CPanoBuilder::APP_SYS_DeInit() {
    AX_S32 nRet = AX_SUCCESS;

    nRet = AX_POOL_Exit();
    if (0 != nRet) {
        return nRet;
    }

    AX_APP_Log_SetSysModuleInited(AX_FALSE);

    nRet = AX_SYS_Deinit();
    if (0 != nRet) {
        return nRet;
    }

    return AX_SUCCESS;
}

AX_S32 CPanoBuilder::APP_VENC_Init() {
    AX_S32 nRet = AX_SUCCESS;

    AX_VENC_MOD_ATTR_T tModAttr;
    memset(&tModAttr, 0, sizeof(AX_VENC_MOD_ATTR_T));
    tModAttr.enVencType = AX_VENC_MULTI_ENCODER;
    tModAttr.stModThdAttr.bExplicitSched = AX_FALSE;
    tModAttr.stModThdAttr.u32TotalThreadNum = COptionHelper::GetInstance()->GetSetVencThreadNum();

    nRet = AX_VENC_Init(&tModAttr);
    if (AX_SUCCESS != nRet) {
        return nRet;
    }

    return AX_SUCCESS;
}

AX_S32 CPanoBuilder::APP_VENC_DeInit() {
    AX_S32 nRet = AX_SUCCESS;

    nRet = AX_VENC_Deinit();
    if (AX_SUCCESS != nRet) {
        return nRet;
    }

    return AX_SUCCESS;
}

AX_S32 CPanoBuilder::APP_NPU_Init() {
    AX_S32 nRet = AX_SUCCESS;

    AX_ENGINE_NPU_ATTR_T tNpuAttr;
    memset(&tNpuAttr, 0, sizeof(AX_ENGINE_NPU_ATTR_T));
    tNpuAttr.eHardMode = (AX_ENGINE_NPU_MODE_T)COptionHelper::GetInstance()->GetNpuMode();
    nRet = AX_ENGINE_Init(&tNpuAttr);
    if (AX_SUCCESS != nRet) {
        return nRet;
    }

    return AX_SUCCESS;
}

AX_S32 CPanoBuilder::APP_NPU_DeInit() {
    AX_S32 nRet = AX_SUCCESS;

    nRet = AX_ENGINE_Deinit();
    if (AX_SUCCESS != nRet) {
        return nRet;
    }

    return AX_SUCCESS;
}

AX_S32 CPanoBuilder::APP_LOG_Init(AX_VOID) {
    AX_S32 nRet = AX_SUCCESS;

    APP_LOG_ATTR_T tLogAttr;
    memset(&tLogAttr, 0, sizeof(APP_LOG_ATTR_T));

    AX_S32 nLogTarget = 0;
    if (CCmdLineParser::GetInstance()->GetLogTarget(nLogTarget)) {
        tLogAttr.nTarget = nLogTarget;
    } else {
        tLogAttr.nTarget = APP_LOG_TARGET_STDOUT;
    }

    AX_S32 nLogLevel = 0;
    if (CCmdLineParser::GetInstance()->GetLogLevel(nLogLevel)) {
        tLogAttr.nLv = nLogLevel;
    } else {
        tLogAttr.nLv = APP_LOG_ERROR;
    }

    sprintf(tLogAttr.szAppName, "%s", "FRTDemo-ITS");

    nRet = AX_APP_Log_Init(&tLogAttr);
    if (AX_SUCCESS != nRet) {
        return nRet;
    }

    return AX_SUCCESS;
}

AX_S32 CPanoBuilder::APP_LOG_DeInit() {
    AX_APP_Log_DeInit();

    return AX_SUCCESS;
}

AX_S32 CPanoBuilder::APP_ACAP_Init() {
    AX_S32 nRet = AX_SUCCESS;

    if (APP_AUDIO_CAP_AVAILABLE()) {
        nRet = AX_AI_Init();

        if (AX_SUCCESS != nRet) {
            return nRet;
        }

        nRet = AX_AENC_Init();

        if (AX_SUCCESS != nRet) {
            return nRet;
        }
    }

    return AX_SUCCESS;
}

AX_S32 CPanoBuilder::APP_ACAP_DeInit() {
    AX_S32 nRet = AX_SUCCESS;

    if (APP_AUDIO_CAP_AVAILABLE()) {
        nRet = AX_AI_DeInit();

        if (AX_SUCCESS != nRet) {
            return nRet;
        }

        nRet = AX_AENC_DeInit();

        if (AX_SUCCESS != nRet) {
            return nRet;
        }
    }

    return AX_SUCCESS;
}

AX_S32 CPanoBuilder::APP_APLAY_Init() {
    AX_S32 nRet = AX_SUCCESS;

    if (APP_AUDIO_PLAY_AVAILABLE()) {
        nRet = AX_AO_Init();

        if (AX_SUCCESS != nRet) {
            return nRet;
        }

        nRet = AX_ADEC_Init();

        if (AX_SUCCESS != nRet) {
            return nRet;
        }
    }

    return AX_SUCCESS;
}

AX_S32 CPanoBuilder::APP_APLAY_DeInit() {
    AX_S32 nRet = AX_SUCCESS;

    if (APP_AUDIO_PLAY_AVAILABLE()) {
        nRet = AX_AO_DeInit();

        if (AX_SUCCESS != nRet) {
            return nRet;
        }

        nRet = AX_ADEC_DeInit();

        if (AX_SUCCESS != nRet) {
            return nRet;
        }
    }

    return AX_SUCCESS;
}

AX_BOOL CPanoBuilder::GetEncoder(AX_VOID **ppEncoder, AX_BOOL *pIsJenc, AX_S32 encoderChn) {
    if (nullptr == ppEncoder) {
        LOG_M_E(PPL, "ppEncoder is null.");
        return AX_FALSE;
    }

    *pIsJenc = AX_FALSE;

    for (auto jencIns : m_vecJencInstance) {
        if (jencIns->GetChnCfg()->nChannel == encoderChn) {
            *ppEncoder = (AX_VOID *)jencIns;
            *pIsJenc = AX_TRUE;
            return AX_TRUE;
        }
    }

    for (auto vencIns : m_vecVencInstance) {
        if (vencIns->GetChnCfg()->nChannel == encoderChn) {
            *ppEncoder = (AX_VOID *)vencIns;
            return AX_TRUE;
        }
    }

    *ppEncoder = nullptr;
    return AX_FALSE;
}

AX_BOOL CPanoBuilder::RefleshIvps() {
    LOG_MM(PPL, "+++");

    for (AX_U8 i = 0; i < MAX_IVPS_GROUP_NUM; i++) {
        IPC_MOD_INFO_T tDstMod = {E_PPL_MOD_TYPE_IVPS, i, 0};
        vector<PPL_MOD_RELATIONSHIP_T> vecRelations;
        if (!GetRelationsByDstMod(tDstMod, vecRelations, AX_TRUE)) {
            LOG_MM_W(PPL, "Can not find relation for dest module: (Module:%s, Grp:%d, Chn:%d)", ModType2String(tDstMod.eModType).c_str(),
                     tDstMod.nGroup, tDstMod.nChannel);
            continue;
        }

        /* NOTICE: Suppose each ivps group has only one frame source */
        PPL_MOD_RELATIONSHIP_T& tRelation = vecRelations[0];
        IVPS_GROUP_CFG_T* pConfig = m_vecIvpsInstance[i]->GetGrpCfg();

        if (E_PPL_MOD_TYPE_IVPS == tRelation.tSrcModChn.eModType) {
            if ((-1 == pConfig->arrGrpResolution[0])
                    || -1 == pConfig->arrGrpResolution[1]
                    || -1 == pConfig->arrGrpFramerate[0]
                    || -1 == pConfig->arrGrpFramerate[1]) {
                m_vecIvpsInstance[tRelation.tSrcModChn.nGroup]->RegObserver(tRelation.tSrcModChn.nChannel,
                                                                            m_vecIvpsObs[i].get());
                if (tRelation.bLink) {
                    pConfig->nGrpLinkFlag = AX_TRUE;
                }

                if (!m_vecIvpsInstance[i]->InitParams()) {
                    return AX_FALSE;
                }
            } else {
                continue;
            }
        }
    }

    LOG_MM(PPL, "---");
    return AX_TRUE;
}

AX_BOOL CPanoBuilder::RestoreMp4Recording() {
    AX_BOOL bRet = AX_TRUE;

    for (auto& pInstance : m_vecMpeg4Instance) {
        if (pInstance->IsNeedRestore()) {
            if (!pInstance->Restore()) {
                bRet = AX_FALSE;
            }

            SetWebRecordAttr(pInstance, pInstance->GetSnsId(), pInstance->GetPreviewChn());
        }
    }

    return bRet;
}

AX_VOID CPanoBuilder::UpdateMp4FrameRate(AX_U8 nSnsID, AX_U32 nFrameRate) {
    for (auto& pInstance : m_vecMpeg4Instance) {
        if (pInstance->GetSnsId() == nSnsID) {
            pInstance->UpdateFrameRate(nFrameRate);
        }
    }
}

AX_VOID CPanoBuilder::SetWebRecordAttr(CMPEG4Encoder* pMp4Instance, AX_U8 nSnsID, AX_U8 nPreviewChn) {
    WEB_RECORD_ATTR_T stRecordAttr;

    stRecordAttr.bSupport = pMp4Instance->IsSupport();
    stRecordAttr.bEnable  = pMp4Instance->IsMp4EncoderOn();

    MPEG4EC_TYPE_INFO_T stRecordTypeInfo = pMp4Instance->GetRecordTypeInfo();
    stRecordAttr.nRecordType = stRecordTypeInfo.nRecordType;
    stRecordAttr.nRawSteamType = stRecordTypeInfo.nVideoRawStreamType;

    CWebOptionHelper::GetInstance()->SetRecordAttr(nSnsID, nPreviewChn, stRecordAttr);
}

AX_BOOL CPanoBuilder::UpdatePipeMainResolution(const PPL_MOD_INFO_T& tSrcMod, const AX_U32 nNewWidth, const AX_U32 nNewHeight) {
    LOG_MM_I(PPL, "new resolution(%d, %d)", nNewWidth, nNewHeight);

    vector<PPL_MOD_RELATIONSHIP_T> vecRelations;
    PPL_MOD_INFO_T stSrcMod = tSrcMod;
    if (!GetRelationsBySrcMod(stSrcMod, vecRelations, AX_TRUE)){
        LOG_MM_E(PPL, "GetRelationsBySrcMod failed");
        return AX_FALSE;
    }

    AX_U8 nSnsID = 0;
    for (auto& relation : vecRelations) {
        if (E_PPL_MOD_TYPE_IVPS == relation.tDstModChn.eModType) {
            nSnsID = m_vecIvpsInstance[relation.tDstModChn.nGroup]->GetGrpCfg()->nSnsSrc;
            break;
        }
    }

    for (auto pInstance : m_vecIvpsInstance) {
        if (pInstance->GetSensorSrc() == nSnsID) {
            pInstance->GetOsdHelper()->Refresh(AX_FALSE);
            for (AX_U8 nChn = 0; nChn < pInstance->GetGrpCfg()->nGrpChnNum; nChn++) {
                pInstance->EnableChannel(nChn, AX_FALSE);
            }
        }
    }

    for (auto& relation : vecRelations) {
        if (E_PPL_MOD_TYPE_IVPS == relation.tDstModChn.eModType) {
            m_vecIvpsInstance[relation.tDstModChn.nGroup]->SetMainResolution(nNewWidth, nNewHeight);
            m_vecIvpsInstance[relation.tDstModChn.nGroup]->UpdateGrpResolution(nNewWidth, nNewHeight);
        }
    }

    for (auto pInstance : m_vecIvpsInstance) {
        if (pInstance->GetSensorSrc() == nSnsID) {
            pInstance->GetOsdHelper()->Refresh();
            for (AX_U8 nChn = 0; nChn < pInstance->GetGrpCfg()->nGrpChnNum; nChn++) {
                pInstance->EnableChannel(nChn, AX_TRUE);
            }
        }
    }

    return AX_TRUE;
}

AX_BOOL CPanoBuilder::GetStitchPipeMap(std::map<AX_U8, AX_U8>& mapStitchPipe) {
    PPL_MOD_INFO_T tDstMod = {E_PPL_MOD_TYPE_AVS, 0, 0};
    vector<PPL_MOD_RELATIONSHIP_T> vecRelations;
    if (!GetRelationsByDstMod(tDstMod, vecRelations, AX_TRUE)) {
        LOG_MM_E(PPL, "Can not find relation for dest module: (Module:%s, Grp:%d, Chn:%d)", ModType2String(tDstMod.eModType).c_str(),
                    tDstMod.nGroup, tDstMod.nChannel);
        return AX_FALSE;
    }

    AX_U8 nStitchPipeNum = (vecRelations.size() >= AX_AVS_PIPE_NUM) ? AX_AVS_PIPE_NUM : vecRelations.size();
    LOG_MM_I(PPL, "stitch pipe num: %d", nStitchPipeNum);

    for (auto& relation : vecRelations) {
        if ((AX_U32)relation.tDstModChn.nChannel < nStitchPipeNum) {
            mapStitchPipe[relation.tSrcModChn.nGroup] = relation.tDstModChn.nChannel;
            LOG_MM_I(PPL, "stitch (index, pipe) - (%d, %d)", relation.tDstModChn.nChannel, relation.tSrcModChn.nGroup);
        } else {
            LOG_MM_E(PPL, "Invalid linked avs channel: %d", relation.tDstModChn.nChannel);
            return AX_FALSE;
        }
    }

    return AX_TRUE;
}

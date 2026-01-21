/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include <stdlib.h>

#include "axcl_demo_builder.hpp"
#include "AXPoolManager.hpp"
#include "AppLogApi.h"
#include "dispatch_observer.hpp"
#include "GlobalDef.h"
#include "StreamerFactory.hpp"
#include "VoObserver.hpp"
#include "CommonDef.h"
#include "make_unique.hpp"

#include "axcl.h"

#define AXCL_DEMO "axcl_demo"
#define VDEC_CHN0 0
#define VDEC_CHN1 1
#define VDEC_CHN2 2
#define DISPVO_CHN VDEC_CHN1
#define DETECT_CHN_HST VDEC_CHN2
#define DETECT_CHN_DEV VDEC_CHN1

using namespace std;

AX_BOOL CAxclDemoBuilder::Init(const std::string &strJson) {
    CAxclDemoConfig *pConfig = CAxclDemoConfig::GetInstance();
    if (!pConfig->Init()) {
        printf("Load axcl demo config files failed.\n");
        return AX_FALSE;
    }

    /* [1]: Load configuration */
    STREAM_CONFIG_T streamConfig = pConfig->GetStreamConfig();
    VDEC_CONFIG_T vdecConfig = pConfig->GetVdecConfig();

    /* [2]: Init axcl */
    {
        /* Init axcl device */
        LOG_MM_I(AXCL_DEMO, "axcl.json: %s", strJson.c_str());
        if (axclError ret = axclInit(strJson.c_str()); AXCL_SUCC != ret) {
            LOG_MM_E(AXCL_DEMO, "axcl init fail, ret = 0x%x", ret);
            return AX_FALSE;
        }

        /* Active axcl device */
        m_nDeviceID = pConfig->GetDevConfig().nDevID;
        if (axclError ret = axclrtSetDevice(m_nDeviceID); AXCL_SUCC != ret) {
            LOG_MM_E(AXCL_DEMO, "axcl active device, ret = 0x%x", ret);
            axclFinalize();
            return AX_FALSE;
        }
    }

    /* [3]: Init system */
    m_nDecodeGrpCountHst = streamConfig.nDecodeGrps;
    m_nDecodeGrpCountDev = vdecConfig.nDecodeGrps;
    AXCL_DEMO_APP_SYS_ATTR_T tSysAttrHost{.nMaxGrp = (AX_U32)m_nDecodeGrpCountHst};
    AXCL_DEMO_APP_SYS_ATTR_T tSysAttrDevice{.nMaxGrp = (AX_U32)m_nDecodeGrpCountDev};
    if (!m_sys.Init(tSysAttrHost, tSysAttrDevice, "AxclDemo")) {
        return AX_FALSE;
    }

    /* [4]: Init streamers */
    if (!InitStreamer(streamConfig)) {
        return AX_FALSE;
    }
    else {
        CDetectResult::GetInstance()->Clear();
    }

    /* [5]: Init Host */
    if (!InitHost(pConfig)) {
        return AX_FALSE;
    }

    /* [6]: Init Device */
    if (!InitDevice(pConfig)) {
        return AX_FALSE;
    }

    return AX_TRUE;
}

AX_BOOL CAxclDemoBuilder::InitHost(CAxclDemoConfig *pConfig) {
    /* Init display and observer */
    DISPVO_CONFIG_T dispVoConfig = pConfig->GetDispVoConfig();
    dispVoConfig.nChnDepth = 4;
    if (!InitHostDisplay(dispVoConfig)) {
        return AX_FALSE;
    }

    // /* Init dispatchers */
    // if (!InitHostDispatcher(dispVoConfig.strBmpPath)) {
    //     return AX_FALSE;
    // }

    /* Init transfer stream */
    // if (!InitHostTransHelper()) {
    //     return AX_FALSE;
    // }

    // /* Init host video decoder */
    // STREAM_CONFIG_T streamConfig = pConfig->GetStreamConfig();
    // streamConfig.nChnW[DISPVO_CHN] = m_disp->GetVideoLayout()[0].u32Width;
    // streamConfig.nChnH[DISPVO_CHN] = m_disp->GetVideoLayout()[0].u32Height;
    // if (!InitHostDecoder(streamConfig)) {
    //     return AX_FALSE;
    // }

    return AX_TRUE;
}

AX_BOOL CAxclDemoBuilder::InitDevice(CAxclDemoConfig *pConfig) {
    AX_BOOL bRet = AX_FALSE;
    do {
        // /* Init detector */
        DETECT_CONFIG_T tDetectConfig = pConfig->GetDetectConfig();
        // if (!InitDeviceDetector(tDetectConfig)) {
        //     goto ERR_PROC;
        // }

        /* Init capture */
        CAPTURE_CONFIG_T tCaptureConfig = pConfig->GetCaptureConfig();
        if (!InitDeviceCapture(tCaptureConfig)) {
            goto ERR_PROC;
        }

        /* Init video decoder */
        VDEC_CONFIG_T tVdecConfig = pConfig->GetVdecConfig();
        if (TARGET_DETECT == tVdecConfig.nOutputTarget) {
            /* vdec links to detect */
            tVdecConfig.arrChnW[DETECT_CHN_DEV] = tDetectConfig.nW;
            tVdecConfig.arrChnH[DETECT_CHN_DEV] = tDetectConfig.nH;

            if (!InitDeviceDecoder(tVdecConfig)) {
                goto ERR_PROC;
            }
        } else {
            /* vdec links to ivps */
            tVdecConfig.arrChnW[VDEC_CHN0] = tVdecConfig.nMaxGrpW;
            tVdecConfig.arrChnH[VDEC_CHN0] = tVdecConfig.nMaxGrpH;

            if (!InitDeviceDecoder(tVdecConfig)) {
                goto ERR_PROC;
            }

            IVPS_CONFIG_T tIvpsConfig;
            memset(&tIvpsConfig, 0, sizeof(IVPS_CONFIG_T));
            tIvpsConfig.nGrpCount = m_nDecodeGrpCountDev;
            tIvpsConfig.nChnCount = 1;
            tIvpsConfig.arrChnW[0] = tDetectConfig.nW;
            tIvpsConfig.arrChnH[0] = tDetectConfig.nH;
            tIvpsConfig.arrChnDepth[0] = 5;
            tIvpsConfig.arrChnLink[0] = AX_FALSE;
            if (!InitDeviceIvps(tIvpsConfig)) {
                goto ERR_PROC;
            }
        }
        bRet = AX_TRUE;
    }
    while (0);
    return bRet;

ERR_PROC:
    axclrtResetDevice(m_nDeviceID);
    axclFinalize();
    return bRet;
}

AX_BOOL CAxclDemoBuilder::InitStreamer(const STREAM_CONFIG_T &streamConfig) {
    const AX_U32 nCount = streamConfig.v.size();
    m_arrStreamer.resize(nCount);
    for (AX_U32 i = 0; i < nCount; ++i) {
        STREAMER_ATTR_T stAttr;
        stAttr.strPath = streamConfig.v[i];
        stAttr.nMaxWidth = streamConfig.nMaxGrpW;
        stAttr.nMaxHeight = streamConfig.nMaxGrpH;
        stAttr.nFrmDelay = streamConfig.nFrmDelay;
        stAttr.nCookie = (AX_S32)i;
        stAttr.bLoop = AX_TRUE;
        stAttr.bSyncObs = AX_TRUE;
        stAttr.nDevID = m_nDeviceID;

        m_arrStreamer[i] = CStreamerFactory::GetInstance()->CreateHandler(stAttr.strPath);
        if (!m_arrStreamer[i]) {
            return AX_FALSE;
        }

        if (!m_arrStreamer[i]->Init(stAttr)) {
            return AX_FALSE;
        }

        LOG_M_C(AXCL_DEMO, "stream %d: %s", i, stAttr.strPath.c_str());
    }

    return AX_TRUE;
}

AX_BOOL CAxclDemoBuilder::InitHostDisplay(const DISPVO_CONFIG_T &dispVoConfig) {
    m_disp = make_shared<CVo>();
    if (!m_disp) {
        LOG_MM_E(AXCL_DEMO, "Create display instance failed.");
        return AX_FALSE;
    }

    VO_ATTR_T stAttr;
    stAttr.voDev = dispVoConfig.nDevId;
    stAttr.enIntfType = AX_VO_INTF_HDMI;
    stAttr.enIntfSync = (AX_VO_INTF_SYNC_E)dispVoConfig.nHDMI;
    stAttr.nBgClr = 0x000000;
    stAttr.nLayerDepth = dispVoConfig.nLayerDepth;
    stAttr.nTolerance = dispVoConfig.nTolerance;
    stAttr.strResDirPath = dispVoConfig.strResDirPath;
    stAttr.bShowLogo = dispVoConfig.bShowLogo;
    stAttr.bShowNoVideo = dispVoConfig.bShowNoVideo;
    stAttr.arrChns.resize(m_nDecodeGrpCountHst);
    stAttr.bListenHotPlug = AX_FALSE;

    for (auto &&m : stAttr.arrChns) {
        m.nPriority = 0;
        m.nDepth = dispVoConfig.nChnDepth;
        if (m.nDepth < 1) {
            m.nDepth = 1;
        }
    }
    if (!m_disp->Init(stAttr)) {
        return AX_FALSE;
    }

    m_dispObserver = CObserverMaker::CreateObserver<CVoObserver>(m_disp.get(), DISPVO_CHN);
    if (!m_dispObserver) {
        LOG_MM_E(AXCL_DEMO, "Create display observer instance failed.");
        return AX_FALSE;
    }

    return AX_TRUE;
}

AX_BOOL CAxclDemoBuilder::InitHostDispatcher(const string &strFontPath) {
    m_arrDispatcher.resize(m_nDecodeGrpCountHst);
    m_arrDispatchObserver.resize(m_nDecodeGrpCountHst);
    for (AX_U32 i = 0; i < m_nDecodeGrpCountHst; ++i) {
        m_arrDispatcher[i] = make_unique<CDispatcher>();
        if (!m_arrDispatcher[i]) {
            LOG_MM_E(AXCL_DEMO, "Create dispatcher %d instance failed.", i);
            return AX_FALSE;
        } else {
            if (m_dispObserver) {
                m_arrDispatcher[i]->RegObserver(m_dispObserver.get());
            }
        }

        DISPATCH_ATTR_T stAttr;
        stAttr.vdGrp = i;
        stAttr.strBmpFontPath = strFontPath;
        stAttr.nDepth = -1;
        if (!m_arrDispatcher[i]->Init(stAttr)) {
            return AX_FALSE;
        }

        m_arrDispatchObserver[i] = CObserverMaker::CreateObserver<CDispatchObserver>(m_arrDispatcher[i].get(), DISPVO_CHN);
        if (!m_arrDispatchObserver[i]) {
            LOG_MM_E(AXCL_DEMO, "Create dispatch %d observer instance failed.", i);
            return AX_FALSE;
        }
    }

    return AX_TRUE;
}

AX_BOOL CAxclDemoBuilder::InitHostTransHelper(AX_VOID) {
    LOG_MM_C(AXCL_DEMO, "+++");
    m_transHelper = make_unique<CTransferHelper>();
    if (!m_transHelper) {
        LOG_MM_E(AXCL_DEMO, "Create transfer helper instance failed.");
        return AX_FALSE;
    }

    if (!m_transHelper->Init(m_nDeviceID, m_nDecodeGrpCountHst, m_nDecodeGrpCountDev)) {
        return AX_FALSE;
    }

    // for (AX_U32 i = 0; i < m_nDecodeGrpCountHst; ++i) {
    //     m_arrStreamer[i]->RegObserver(m_transHelper.get());
    // }

    LOG_MM_C(AXCL_DEMO, "---");
    return AX_TRUE;
}

AX_BOOL CAxclDemoBuilder::InitHostDecoder(const STREAM_CONFIG_T &streamConfig) {
    LOG_MM_C(AXCL_DEMO, "+++");

    m_vdecHost = make_unique<CAxVideoDecoder>();
    if (!m_vdecHost) {
        LOG_MM_E(AXCL_DEMO, "Create host video decoder instance failed.");
        return AX_FALSE;
    }

    vector<VDEC_GRP_ATTR_T> arrVdGrps(m_nDecodeGrpCountHst);
    for (AX_U32 i = 0; i < m_nDecodeGrpCountHst; ++i) {
        const STREAMER_INFO_T &stInfo = m_arrStreamer[i]->GetStreamInfo();

        VDEC_GRP_ATTR_T tGrpAttr;
        tGrpAttr.bEnable = AX_TRUE;
        tGrpAttr.enCodecType = stInfo.eVideoType;
        tGrpAttr.nMaxWidth = ALIGN_UP(streamConfig.nMaxGrpW, 16);  /* H264 MB 16x16 */
        tGrpAttr.nMaxHeight = ALIGN_UP(streamConfig.nMaxGrpH, 16); /* H264 MB 16x16 */

        if (0 == streamConfig.nDefaultFps) {
            /* if default fps is 0 or RTSP, fps is parsed by streamer */
            tGrpAttr.nFps = stInfo.nFps;
        } else {
            /* use configured fps for file streamer */
            tGrpAttr.nFps = streamConfig.nDefaultFps;
        }

        if (STREAM_TYPE_E::FILE == stInfo.eStreamType) {
            /* TODO: debug specified fps for VO module */
            char name[32];
            sprintf(name, "VDEC_GRP%d_DECODED_FPS", i);
            const char *env = getenv(name);
            if (env) {
                tGrpAttr.nFps = atoi(env);
            }
        }

        if (tGrpAttr.nFps > 0) {
            tGrpAttr.bFramerateCtrl = AX_TRUE;
        }

        tGrpAttr.bPrivatePool = (2 == streamConfig.nUserPool) ? AX_TRUE : AX_FALSE;

        /* FILE: playback + frame or stream mode according configuration */
        tGrpAttr.eDecodeMode = AX_VDEC_DISPLAY_MODE_PLAYBACK;
        if (0 == streamConfig.nInputMode) {
            tGrpAttr.enInputMode = AX_VDEC_INPUT_MODE_FRAME;
            tGrpAttr.nMaxStreamBufSize = tGrpAttr.nMaxWidth * tGrpAttr.nMaxHeight * 2;
        } else {
            tGrpAttr.enInputMode = AX_VDEC_INPUT_MODE_STREAM;
            tGrpAttr.nMaxStreamBufSize = streamConfig.nMaxStreamBufSize;
        }

        for (AX_U32 j = 0; j < MAX_VDEC_CHN_NUM; ++j) {
            AX_VDEC_CHN_ATTR_T &tChnAttr = tGrpAttr.stChnAttr[j];
            switch (j) {
                case VDEC_CHN0:
                    /* pp0 disable, because scaler is not support */
                    tGrpAttr.bChnEnable[j] = AX_FALSE;
                    break;
                case DISPVO_CHN:
                    /* pp1 scaler max. 4096x2160 */
                    tGrpAttr.bChnEnable[j] = AX_TRUE;
                    tChnAttr.u32PicWidth = streamConfig.nChnW[j];
                    tChnAttr.u32PicHeight = streamConfig.nChnH[j];
                    tChnAttr.u32FrameStride = ALIGN_UP(tChnAttr.u32PicWidth, VDEC_STRIDE_ALIGN);
                    tChnAttr.u32OutputFifoDepth = streamConfig.nChnDepth[j];
                    tChnAttr.enOutputMode = AX_VDEC_OUTPUT_SCALE;
                    tChnAttr.enImgFormat = AX_FORMAT_YUV420_SEMIPLANAR;
                    tChnAttr.stCompressInfo.enCompressMode = AX_COMPRESS_MODE_NONE;
                    break;
                case DETECT_CHN_HST:
                    /* pp2 scaler max. 1920x1080 */
                    tGrpAttr.bChnEnable[j] = AX_FALSE;
                    break;
                default:
                    break;
            }
        }

        arrVdGrps[i] = move(tGrpAttr);
    }

    if (!m_vdecHost->Init(arrVdGrps)) {
        return AX_FALSE;
    }

    for (AX_U32 i = 0; i < m_nDecodeGrpCountHst; ++i) {
        /* register vdec to streamer */
        m_arrStreamer[i]->RegObserver(m_vdecHost.get());

        AX_VDEC_GRP vdGrp = (AX_VDEC_GRP)i;
        m_vdecHost->RegObserver(vdGrp, m_arrDispatchObserver[i].get());

        VDEC_GRP_ATTR_T tGrpAttr;
        m_vdecHost->GetGrpAttr(vdGrp, tGrpAttr);
        m_disp->SetChnFrameRate(m_disp->GetVideoChn(i), tGrpAttr.nFps);

        for (AX_U32 j = 0; j < MAX_VDEC_CHN_NUM; ++j) {
            if (!tGrpAttr.bChnEnable[j]) {
                continue;
            }

            if (tGrpAttr.bPrivatePool) {
                continue;
            }

            AX_VDEC_CHN_ATTR_T &stChn = tGrpAttr.stChnAttr[j];
            AX_U32 nBlkSize = CAxVideoDecoder::GetBlkSize(stChn.u32PicWidth, stChn.u32PicHeight, stChn.u32FrameStride, tGrpAttr.enCodecType,
                                                          &stChn.stCompressInfo, stChn.enImgFormat);

            if (0 == streamConfig.nUserPool) {
                CAXPoolManager::GetInstance()->AddBlockToFloorPlan(nBlkSize, stChn.u32OutputFifoDepth);
                LOG_M_N(AXCL_DEMO, "VDEC vdGrp %d vdChn %d blkSize %d blkCount %d", vdGrp, j, nBlkSize, stChn.u32OutputFifoDepth);
            } else {
                AX_POOL_CONFIG_T stPoolConfig;
                memset(&stPoolConfig, 0, sizeof(stPoolConfig));
                stPoolConfig.MetaSize = 4096;
                stPoolConfig.BlkSize = nBlkSize;
                stPoolConfig.BlkCnt = stChn.u32OutputFifoDepth;
                stPoolConfig.IsMergeMode = AX_FALSE;
                stPoolConfig.CacheMode = POOL_CACHE_MODE_NONCACHE;
                sprintf((AX_CHAR *)stPoolConfig.PoolName, "vdec_%d_pp%d_pool", i, j);
                AX_POOL pool = CAXPoolManager::GetInstance()->CreatePool(stPoolConfig);
                if (AX_INVALID_POOLID == pool) {
                    return AX_FALSE;
                }

                if (!m_vdecHost->AttachPool(vdGrp, (AX_VDEC_CHN)j, pool)) {
                    return AX_FALSE;
                }

                LOG_M_C(AXCL_DEMO, "pool %2d (blkSize %d blkCount %d) is attached to VDEC vdGrp %d vdChn %d", pool, stPoolConfig.BlkSize,
                        stPoolConfig.BlkCnt, vdGrp, j);
            }
        }
    }

    if (0 == streamConfig.nUserPool) {
        if (!CAXPoolManager::GetInstance()->CreateFloorPlan(512)) {
            return AX_FALSE;
        }
    }

    LOG_MM_C(AXCL_DEMO, "---");

    return AX_TRUE;
}

AX_BOOL CAxclDemoBuilder::InitDeviceIvps(const IVPS_CONFIG_T &tIvpsConfig) {
    AX_BOOL bRet = AX_FALSE;

    do {
        for (AX_S32 nGrp = 0; nGrp < tIvpsConfig.nGrpCount; ++nGrp) {
            IVPS_ATTR_T stAttr;
            stAttr.nGrpId = nGrp;
            stAttr.nChnNum = tIvpsConfig.nChnCount;
            stAttr.nBackupInDepth = 0;

            AX_IVPS_ENGINE_E enEngine = AX_IVPS_ENGINE_VPP;
            if (nGrp < 16) {
                enEngine = AX_IVPS_ENGINE_VPP;
            }
            else {
                enEngine = AX_IVPS_ENGINE_VGP;
            }

            for (AX_U32 i = 0; i < stAttr.nChnNum; ++i) {
                stAttr.stChnAttr[i].enEngine = enEngine;
                stAttr.stChnAttr[i].nWidth = m_disp->GetRectWidth();
                stAttr.stChnAttr[i].nHeight = m_disp->GetRectHeight();
                stAttr.stChnAttr[i].nStride = ALIGN_UP(stAttr.stChnAttr[i].nWidth, 16);
                stAttr.stChnAttr[i].bLinked = tIvpsConfig.arrChnLink[i] == 0 ? AX_FALSE : AX_TRUE;
                stAttr.stChnAttr[i].nFifoDepth = tIvpsConfig.arrChnDepth[i];
                stAttr.stChnAttr[i].stPoolAttr.ePoolSrc = POOL_SOURCE_PRIVATE;
                stAttr.stChnAttr[i].stPoolAttr.nFrmBufNum = 5 + 1;
            }

            CIVPS* pIvpsIns = CIVPS::CreateInstance(m_nDeviceID, stAttr);
            if (!pIvpsIns) {
                LOG_M_E(AXCL_DEMO, "[Grp%02d] IVPS initialize failed.", stAttr.nGrpId);
                break;
            }

            for (AX_U32 i = 0; i < stAttr.nChnNum; ++i) {
                if (!stAttr.stChnAttr[i].bLinked) {
                    if (0 == i) {
                        // if (m_bEnableDetect) {
                        //     pIvpsIns->RegisterObserver(0, m_detectorObserver.get());
                        // }

                        pIvpsIns->RegisterObserver(0, m_dispObserver.get());
                        pIvpsIns->BindVo(0, m_disp);

                        AX_MOD_INFO_T tPrev {AX_ID_VDEC, nGrp, 0};
                        AX_MOD_INFO_T tNext {AX_ID_IVPS, nGrp, 0};
                        AX_S32 ret = AXCL_SYS_Link(&tPrev, &tNext);
                        if (0 != ret) {
                            LOG_MM_E(AXCL_DEMO, "Link vdec(%d, %d)->ivps(%d, %d) failed.", nGrp, 0, nGrp, 0);
                        }
                    }
                }
            }

            m_vecIvps.emplace_back(pIvpsIns);
        }

        bRet = AX_TRUE;
    } while (0);

    return bRet;
}

AX_BOOL CAxclDemoBuilder::InitDeviceDecoder(const VDEC_CONFIG_T& tVdecConfig) {
    LOG_MM_C(AXCL_DEMO, "+++");

    m_vdecDevice = make_unique<CAxclVideoDecoder>();
    if (!m_vdecDevice) {
        LOG_MM_E(AXCL_DEMO, "Create device video decoder instance failed.");
        return AX_FALSE;
    }

    vector<AXCL_VDEC_GRP_ATTR_T> arrVdGrps(m_nDecodeGrpCountDev);
    for (AX_U32 i = 0; i < m_nDecodeGrpCountDev; ++i) {
        AXCL_VDEC_GRP_ATTR_T tGrpAttr;
        tGrpAttr.bEnable = AX_TRUE;
        tGrpAttr.enCodecType = tVdecConfig.eVideoType;
        tGrpAttr.nMaxWidth = ALIGN_UP(tVdecConfig.nMaxGrpW, 16);  /* H264 MB 16x16 */
        tGrpAttr.nMaxHeight = ALIGN_UP(tVdecConfig.nMaxGrpH, 16); /* H264 MB 16x16 */

        /* use configured fps for file streamer */
        tGrpAttr.nFps = tVdecConfig.nDefaultFps;
        if (tGrpAttr.nFps > 0) {
            tGrpAttr.bFramerateCtrl = AX_TRUE;
        }

        tGrpAttr.bPrivatePool = (2 == tVdecConfig.nUserPool) ? AX_TRUE : AX_FALSE;
        if (TARGET_IVPS == tVdecConfig.nOutputTarget) {
            /* vdec links to ivps only support private pool now */
            tGrpAttr.bPrivatePool = AX_TRUE;
        }

        /* FILE: playback + frame or stream mode according configuration */
        tGrpAttr.eDecodeMode = AX_VDEC_DISPLAY_MODE_PLAYBACK;
        if (0 == tVdecConfig.nInputMode) {
            tGrpAttr.enInputMode = AX_VDEC_INPUT_MODE_FRAME;
            tGrpAttr.nMaxStreamBufSize = tGrpAttr.nMaxWidth * tGrpAttr.nMaxHeight * 2;
        } else {
            tGrpAttr.enInputMode = AX_VDEC_INPUT_MODE_STREAM;
            tGrpAttr.nMaxStreamBufSize = tVdecConfig.nMaxStreamBufSize;
        }

        for (AX_U32 j = 0; j < MAX_VDEC_CHN_NUM; ++j) {
            AX_VDEC_CHN_ATTR_T &tChnAttr = tGrpAttr.stChnAttr[j];
            switch (j) {
                case VDEC_CHN0:
                    if (TARGET_IVPS == tVdecConfig.nOutputTarget) {
                        /* pp0 original 1920x1080 */
                        tGrpAttr.bChnEnable[j] = AX_TRUE;
                        tChnAttr.u32PicWidth = tVdecConfig.arrChnW[j];
                        tChnAttr.u32PicHeight = tVdecConfig.arrChnH[j];
                        tChnAttr.u32FrameStride = ALIGN_UP(tChnAttr.u32PicWidth, VDEC_STRIDE_ALIGN);
                        tChnAttr.u32OutputFifoDepth = tVdecConfig.nChnDepth[j];
                        tChnAttr.enOutputMode = AX_VDEC_OUTPUT_ORIGINAL;
                        tChnAttr.enImgFormat = AX_FORMAT_YUV420_SEMIPLANAR;
                        tChnAttr.stCompressInfo.enCompressMode = AX_COMPRESS_MODE_NONE;

                        if (tGrpAttr.bPrivatePool) {
                            tChnAttr.u32FrameBufSize = CAxclVideoDecoder::GetBlkSize(tChnAttr.u32PicWidth, tChnAttr.u32PicHeight, tChnAttr.u32FrameStride, tGrpAttr.enCodecType,
                                                                                     &tChnAttr.stCompressInfo, tChnAttr.enImgFormat);
                            tChnAttr.u32FrameBufCnt = tChnAttr.u32OutputFifoDepth;
                        }
                        // LOG_MM_E(AXCL_DEMO, "chn to ivps w:%d", tChnAttr.u32PicWidth);
                    } else {
                        tGrpAttr.bChnEnable[j] = AX_FALSE;
                        // LOG_MM_E(AXCL_DEMO, "chn 0 disabled");
                    }
                    break;
                case DETECT_CHN_DEV:
                    if (TARGET_DETECT == tVdecConfig.nOutputTarget) {
                        /* pp1 scaler max. 4096x2160 */
                        tGrpAttr.bChnEnable[j] = AX_TRUE;
                        tChnAttr.u32PicWidth = tVdecConfig.arrChnW[j];
                        tChnAttr.u32PicHeight = tVdecConfig.arrChnH[j];
                        tChnAttr.u32FrameStride = ALIGN_UP(tChnAttr.u32PicWidth, VDEC_STRIDE_ALIGN);
                        tChnAttr.u32OutputFifoDepth = tVdecConfig.nChnDepth[j];
                        tChnAttr.enOutputMode = AX_VDEC_OUTPUT_SCALE;
                        tChnAttr.enImgFormat = AX_FORMAT_YUV420_SEMIPLANAR;
                        tChnAttr.stCompressInfo.enCompressMode = AX_COMPRESS_MODE_NONE;
                        // LOG_MM_E(AXCL_DEMO, "chn detect w:%d", tChnAttr.u32PicWidth);
                    } else {
                        tGrpAttr.bChnEnable[j] = AX_FALSE;
                        // LOG_MM_E(AXCL_DEMO, "chn 1 disabled");
                    }
                    break;
                default:
                    tGrpAttr.bChnEnable[j] = AX_FALSE;
                    break;
            }
        }

        arrVdGrps[i] = move(tGrpAttr);
    }

    if (!m_vdecDevice->Init(m_nDeviceID, arrVdGrps)) {
        return AX_FALSE;
    } else {
        // m_vdecCtrlObserver = CObserverMaker::CreateObserver<CVdecCtrlObserver>(m_vdec.get(), tVdecConfig.bEnableReset);
        // if (!m_vdecCtrlObserver) {
        //     LOG_M_E(AICARD, "%s: create vdec ctrl command observer fail", __func__);
        //     return AX_FALSE;
        // }
    }

    /* host frame data send to device vdec by way of axcl vdec send */
    for (AX_U32 i = 0; i < m_nDecodeGrpCountDev; ++i) {
        AX_VDEC_GRP vdGrp = (AX_VDEC_GRP)i;
        m_arrStreamer[i]->RegObserver(m_vdecDevice.get());

        // device vdec regist device capture module
        if (m_bEnableCapture) {
            m_vdecDevice->RegObserver(vdGrp, m_captureObserver.get());
        }
    }

    if (TARGET_DETECT == tVdecConfig.nOutputTarget) {
        for (AX_U32 i = 0; i < m_nDecodeGrpCountDev; ++i) {
            // AX_VDEC_GRP vdGrp = (AX_VDEC_GRP)i;
            // if (m_bEnableDetect) {
            //     m_vdecDevice->RegObserver(vdGrp, m_detectorObserver.get());
            // }

            // AXCL_VDEC_GRP_ATTR_T tGrpAttr;
            // m_vdecDevice->GetGrpAttr(vdGrp, tGrpAttr);

            // for (AX_U32 j = 0; j < MAX_VDEC_CHN_NUM; ++j) {
            //     if (!tGrpAttr.bChnEnable[j]) {
            //         continue;
            //     }

            //     if (tGrpAttr.bPrivatePool) {
            //         continue;
            //     }

            //     AX_VDEC_CHN_ATTR_T &stChn = tGrpAttr.stChnAttr[j];
            //     AX_U32 nBlkSize = CVideoDecoder::GetBlkSize(stChn.u32PicWidth, stChn.u32PicHeight, stChn.u32FrameStride, tGrpAttr.enCodecType,
            //                                                 &stChn.stCompressInfo, stChn.enImgFormat);

            //     if (0 == tVdecConfig.nUserPool) {
            //         CAXPoolManager::GetInstance()->AddBlockToFloorPlan(nBlkSize, stChn.u32OutputFifoDepth);
            //         LOG_M_N(AXCL_DEMO, "VDEC vdGrp %d vdChn %d blkSize %d blkCount %d", vdGrp, j, nBlkSize, stChn.u32OutputFifoDepth);
            //     } else {
            //         AX_POOL_CONFIG_T stPoolConfig;
            //         memset(&stPoolConfig, 0, sizeof(stPoolConfig));
            //         stPoolConfig.MetaSize = 4096;
            //         stPoolConfig.BlkSize = nBlkSize;
            //         stPoolConfig.BlkCnt = stChn.u32OutputFifoDepth;
            //         stPoolConfig.IsMergeMode = AX_FALSE;
            //         stPoolConfig.CacheMode = POOL_CACHE_MODE_NONCACHE;
            //         sprintf((AX_CHAR *)stPoolConfig.PoolName, "vdec_%d_pp%d_pool", i, j);
            //         AX_POOL pool = CAXPoolManager::GetInstance()->CreatePool(stPoolConfig);
            //         if (AX_INVALID_POOLID == pool) {
            //             return AX_FALSE;
            //         }

            //         if (!m_vdecDevice->AttachPool(vdGrp, (AX_VDEC_CHN)j, pool)) {
            //             return AX_FALSE;
            //         }

            //         ((CVdecCtrlObserver *)(m_vdecCtrlObserver.get()))->SetPool(vdGrp, (AX_VDEC_CHN) j, pool);

            //         LOG_M_C(AXCL_DEMO, "pool %2d (blkSize %d blkCount %d) is attached to VDEC vdGrp %d vdChn %d", pool, stPoolConfig.BlkSize,
            //                 stPoolConfig.BlkCnt, vdGrp, j);
            //     }
            // }
        }

        // if (0 == tVdecConfig.nUserPool) {
        //     if (!CAXPoolManager::GetInstance()->CreateFloorPlan(512)) {
        //         return AX_FALSE;
        //     }
        // }
    }

    LOG_MM_C(AXCL_DEMO, "---");

    return AX_TRUE;
}

AX_BOOL CAxclDemoBuilder::InitDeviceDetector(const DETECT_CONFIG_T &tDetectConfig) {
    LOG_MM_C(AXCL_DEMO, "+++");

    m_bEnableDetect = tDetectConfig.bEnable;
    if (m_bEnableDetect) {
        m_detector = make_unique<CDetector>();
        if (!m_detector) {
            LOG_M_E(AXCL_DEMO, "%s: create detector instance fail", __func__);
            return AX_FALSE;
        }

        DETECTOR_ATTR_T tDetectAttr;
        tDetectAttr.nGrpCount = m_nDecodeGrpCountDev;
        tDetectAttr.nSkipRate = tDetectConfig.nSkipRate;
        tDetectAttr.nW = tDetectConfig.nW;
        tDetectAttr.nH = tDetectConfig.nH;
        tDetectAttr.nDepth = tDetectConfig.nDepth * m_nDecodeGrpCountDev;
        strcpy(tDetectAttr.szModelPath, tDetectConfig.strModelPath.c_str());
        tDetectAttr.nChannelNum = AX_MIN(tDetectConfig.nChannelNum, DETECTOR_MAX_CHN_NUM);
        for (AX_U32 i = 0; i < tDetectAttr.nChannelNum; ++i) {
            tDetectAttr.tChnAttr[i].nPPL = tDetectConfig.tChnParam[i].nPPL;
            tDetectAttr.tChnAttr[i].nVNPU = tDetectConfig.tChnParam[i].nVNPU;
            tDetectAttr.tChnAttr[i].bTrackEnable = tDetectConfig.tChnParam[i].bTrackEnable;
        }

        if (!m_detector->Init(m_nDeviceID, tDetectAttr)) {
            return AX_FALSE;
        }

        m_detectorObserver = CObserverMaker::CreateObserver<CDetectObserver>(m_detector.get(), DETECT_CHN_DEV);
        if (!m_detectorObserver) {
            LOG_M_E(AXCL_DEMO, "%s: create detect observer fail", __func__);
            return AX_FALSE;
        }
    }

    LOG_MM_C(AXCL_DEMO, "---");
    return AX_TRUE;
}

AX_BOOL CAxclDemoBuilder::InitDeviceCapture(const CAPTURE_CONFIG_T &tCaptureConfig) {
    LOG_MM_C(AXCL_DEMO, "+++");

    m_bEnableCapture = tCaptureConfig.bEnable;
    if (m_bEnableCapture) {
        CAPTURE_ATTR_T tAttr;
        tAttr.nGrpCount = m_nDecodeGrpCountDev;
        tAttr.nSkipFrm = tCaptureConfig.nSkipFrm;

        m_capture = make_unique<CCapture>();
        if (!m_capture) {
            LOG_M_E(AXCL_DEMO, "%s: create capture instance fail", __func__);
            return AX_FALSE;
        }

        if (!m_capture->Init(m_nDeviceID, tAttr)) {
            return AX_FALSE;
        }

        m_captureObserver = CObserverMaker::CreateObserver<CCaptureObserver>(m_capture.get());
        if (!m_captureObserver) {
            LOG_M_E(AXCL_DEMO, "%s: create capture observer fail", __func__);
            return AX_FALSE;
        }
    }

    LOG_MM_C(AXCL_DEMO, "---");
    return AX_TRUE;
}

AX_BOOL CAxclDemoBuilder::DeInit(AX_VOID) {
    /* destroy instances */
#define DESTORY_INSTANCE(p) \
    do {                    \
        if (p) {            \
            p->DeInit();    \
            p = nullptr;    \
        }                   \
    } while (0)

    for (auto &&m : m_arrStreamer) {
        DESTORY_INSTANCE(m);
    }

    // for (auto &&m : m_arrDispatcher) {
    //     DESTORY_INSTANCE(m);
    // }
    // m_arrDispatcher.clear();

    /* If private pool, destory consumer before producer */
    DESTORY_INSTANCE(m_disp);

    if (m_bEnableDetect) {
        DESTORY_INSTANCE(m_detector);
    }

    if (m_bEnableCapture) {
        DESTORY_INSTANCE(m_capture);
    }

    // DESTORY_INSTANCE(m_transHelper);
    // DESTORY_INSTANCE(m_vdecHost);
    DESTORY_INSTANCE(m_vdecDevice);

    for (AX_U32 i = 0;i < m_nDecodeGrpCountDev;i++) {
        AX_MOD_INFO_T tPrev {AX_ID_VDEC, (AX_S32)i, 0};
        AX_MOD_INFO_T tNext {AX_ID_IVPS, (AX_S32)i, 0};
        AX_S32 ret = AXCL_SYS_UnLink(&tPrev, &tNext);
        if (0 != ret) {
            LOG_MM_E(AXCL_DEMO, "UnLink vdec(%d, %d)->ivps(%d, %d) failed.", i, 0, i, 0);
        }
        else {
            LOG_MM_C(AXCL_DEMO, "UnLink vdec(%d, %d)->ivps(%d, %d) succ.", i, 0, i, 0);
        }
    }
    for (auto &m : m_vecIvps) {
        m->UnRegisterObserver(0, m_dispObserver.get());
        DESTORY_INSTANCE(m);
    }
    m_vecIvps.clear();

#undef DESTORY_INSTANCE

    m_sys.DeInit();

    /* quit axcl device */
    axclrtResetDevice(m_nDeviceID);
    axclFinalize();

    return AX_TRUE;
}

AX_BOOL CAxclDemoBuilder::Start(const std::string &strJson) {
    LOG_MM_W(AXCL_DEMO, "+++");

    if (!Init(strJson)) {
        DeInit();
        return AX_FALSE;
    }

    do {
        if (!StartDevice()) {
            return AX_FALSE;
        }

        if (!StartHost()) {
            return AX_FALSE;
        }

        for (auto &&m : m_arrStreamer) {
            if (m) {
                thread t([](IStreamHandler *p) { p->Start(); }, m.get());
                t.detach();
            }
        }

        return AX_TRUE;

    } while (0);

    Stop();

    LOG_MM_W(AXCL_DEMO, "---");

    return AX_FALSE;
}

AX_BOOL CAxclDemoBuilder::Stop(AX_VOID) {
    // for (auto &&m : m_arrStreamer) {
    //     m->UnRegObserver(m_vdecHost.get());
    // }

    vector<thread> v;
    v.reserve(m_arrStreamer.size());
    for (auto &&m : m_arrStreamer) {
        if (m) {
            STREAMER_STAT_T stStat;
            if (m->QueryStatus(stStat) && stStat.bStarted) {
                v.emplace_back([](IStreamHandler *p) { p->Stop(); }, m.get());
            }
        }
    }

    for (auto &&m : v) {
        if (m.joinable()) {
            m.join();
        }
    }

    StopHost();
    StopDevice();

    DeInit();
    return AX_TRUE;
}

AX_BOOL CAxclDemoBuilder::StartHost(AX_VOID) {
    if (m_disp) {
        if (!m_disp->Start()) {
            return AX_FALSE;
        }
    } else {
        LOG_MM_E(AXCL_DEMO, ">>>>>>>>>>>>>>>> DISP module is disabled <<<<<<<<<<<<<<<<<<<<<");
    }

    // for (auto &m : m_arrDispatcher) {
    //     if (!m->Start()) {
    //         return AX_FALSE;
    //     }
    // }

    // if (m_vdecHost) {
    //     if (!m_vdecHost->Start()) {
    //         return AX_FALSE;
    //     }
    // } else {
    //     LOG_MM_E(AXCL_DEMO, ">>>>>>>>>>>>>>>> VDEC module is disabled <<<<<<<<<<<<<<<<<<<<<");
    //     return AX_FALSE;
    // }

    // if (m_transHelper) {
    //     if (!m_transHelper->Start()) {
    //         return AX_FALSE;
    //     }
    // } else {
    //     LOG_MM_E(AXCL_DEMO, ">>>>>>>>>>>>>>>> TRANSFER module is disabled <<<<<<<<<<<<<<<<<<<<<");
    //     return AX_FALSE;
    // }

    return AX_TRUE;
}

AX_VOID CAxclDemoBuilder::StopHost(AX_VOID) {
    // if (m_vdecHost) {
    //     m_vdecHost->UnRegAllObservers();
    // }

    // for (auto &&m : m_arrDispatcher) {
    //     m->Clear();
    // }

    // if (m_transHelper) {
    //     if (!m_transHelper->Stop()) {
    //         LOG_MM_E(AXCL_DEMO, ">>>>>>>>>>>>>>>> TRANSFER module stop failed <<<<<<<<<<<<<<<<<<<<<");
    //     } else {
    //         LOG_MM_C(AXCL_DEMO, ">>>>>>>>>>>>>>>> TRANSFER module stop successfully <<<<<<<<<<<<<<<<<<<<<");
    //     }
    // }

    if (m_disp) {
        if (!m_disp->Stop()) {
            LOG_MM_E(AXCL_DEMO, ">>>>>>>>>>>>>>>> DISPLAY module stop failed <<<<<<<<<<<<<<<<<<<<<");
        } else {
            LOG_MM_C(AXCL_DEMO, ">>>>>>>>>>>>>>>> DISPLAY module stop successfully <<<<<<<<<<<<<<<<<<<<<");
        }
    }

    // for (auto &&m : m_arrDispatcher) {
    //     if (!m->Stop()) {
    //         LOG_MM_E(AXCL_DEMO, ">>>>>>>>>>>>>>>> DISPATCH module stop failed <<<<<<<<<<<<<<<<<<<<<");
    //     } else {
    //         LOG_MM_C(AXCL_DEMO, ">>>>>>>>>>>>>>>> [%d] DISPATCH module stop successfully <<<<<<<<<<<<<<<<<<<<<", m->GetGrp());
    //     }
    // }

    // if (m_vdecHost) {
    //     if (!m_vdecHost->Stop()) {
    //         LOG_MM_E(AXCL_DEMO, ">>>>>>>>>>>>>>>> VDEC module stop failed <<<<<<<<<<<<<<<<<<<<<");
    //     } else {
    //         LOG_MM_C(AXCL_DEMO, ">>>>>>>>>>>>>>>> VDEC module stop successfully <<<<<<<<<<<<<<<<<<<<<");
    //     }
    // }
}

AX_BOOL CAxclDemoBuilder::StartDevice(AX_VOID) {
    // if (m_detector) {
    //     if (!m_detector->Start()) {
    //         return AX_FALSE;
    //     }
    // } else {
    //     LOG_MM_E(AXCL_DEMO, ">>>>>>>>>>>>>>>> DETECTOR module is disabled <<<<<<<<<<<<<<<<<<<<<");
    // }

    if (m_capture) {
        if (!m_capture->Start()) {
            return AX_FALSE;
        }
    } else {
        LOG_MM_C(AXCL_DEMO, ">>>>>>>>>>>>>>>> CAPTURE module is disabled <<<<<<<<<<<<<<<<<<<<<");
    }

    for (auto &m : m_vecIvps) {
        if (!m->Start()) {
            LOG_MM_E(AXCL_DEMO, ">>>>>>>>>>>>>>>> IVPS module is disabled <<<<<<<<<<<<<<<<<<<<<");
            return AX_FALSE;
        }
    }

    if (m_vdecDevice) {
        if (!m_vdecDevice->Start()) {
            return AX_FALSE;
        }
    } else {
        LOG_MM_E(AXCL_DEMO, ">>>>>>>>>>>>>>>> VDEC module is disabled <<<<<<<<<<<<<<<<<<<<<");
        return AX_FALSE;
    }

    return AX_TRUE;
}

AX_VOID CAxclDemoBuilder::StopDevice(AX_VOID) {
    // if (m_detector) {
    //     m_detector->Clear();
    //     if (!m_detector->Stop()) {
    //         LOG_MM_E(AXCL_DEMO, ">>>>>>>>>>>>>>>> DETECTOR module stop failed <<<<<<<<<<<<<<<<<<<<<");
    //     } else {
    //         LOG_MM_C(AXCL_DEMO, ">>>>>>>>>>>>>>>> DETECTOR module stop successfully <<<<<<<<<<<<<<<<<<<<<");
    //     }
    // }

    if (m_capture) {
        if (!m_capture->Stop()) {
            LOG_MM_E(AXCL_DEMO, ">>>>>>>>>>>>>>>> CAPTURE module stop failed <<<<<<<<<<<<<<<<<<<<<");
        } else {
            LOG_MM_C(AXCL_DEMO, ">>>>>>>>>>>>>>>> CAPTURE module stop successfully <<<<<<<<<<<<<<<<<<<<<");
        }
    }

    if (m_vdecDevice) {
        if (!m_vdecDevice->Stop()) {
            LOG_MM_E(AXCL_DEMO, ">>>>>>>>>>>>>>>> VDEC module stop failed <<<<<<<<<<<<<<<<<<<<<");
        } else {
            LOG_MM_C(AXCL_DEMO, ">>>>>>>>>>>>>>>> VDEC module stop successfully <<<<<<<<<<<<<<<<<<<<<");
        }
    }

    for (auto &m : m_vecIvps) {
        if (!m->Stop()) {
            LOG_MM_E(AXCL_DEMO, ">>>>>>>>>>>>>>>> IVPS module stop failed <<<<<<<<<<<<<<<<<<<<<");
            continue;
        } else {
            LOG_MM_C(AXCL_DEMO, ">>>>>>>>>>>>>>>> IVPS module stop succ <<<<<<<<<<<<<<<<<<<<<");
        }
    }
}

AX_BOOL CAxclDemoBuilder::QueryStreamsAllEof(AX_VOID) {
    AX_U32 nEofCnt = 0;

    STREAMER_STAT_T stStat;
    for (auto &&m : m_arrStreamer) {
        if (m) {
            m->QueryStatus(stStat);
            if (!stStat.bStarted) {
                ++nEofCnt;
            }
        } else {
            ++nEofCnt;
        }
    }

    return (nEofCnt >= m_arrStreamer.size()) ? AX_TRUE : AX_FALSE;
}

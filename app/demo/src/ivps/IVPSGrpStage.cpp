/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include "IVPSGrpStage.h"
#include <sys/prctl.h>
#include <chrono>
#include <map>
#include "AXFrame.hpp"
#include "AXThread.hpp"
#include "AppLogApi.h"
#include "CmdLineParser.h"
#include "CommonUtils.hpp"
#include "ElapsedTimer.hpp"
#include "GlobalDef.h"
#include "IvpsOptionHelper.h"
#include "OptionHelper.h"
#include "SensorOptionHelper.h"
#define IVPS "IVPS"

#define MAX_IPC_IVPS_FRAME_TIMEOUT (1000)
#define MAX_REGION_GROUP (3)
#define TDP_CROP_RESIZE_APPLY_GRP (3)
#define TDP_CROP_RESIZE_APPLY_CHN (0)
#define IVPS_IN_FIFO_DEPTH (2)
#define IVPS_OUT_FIFO_DEPTH (2)
#define IVPS_INVALD_FILTER_INDEX  (0Xff)

CIVPSGrpStage::CIVPSGrpStage(IVPS_GROUP_CFG_T& tGrpConfig) : CAXStage(IVPS), m_tIvpsGrpCfg(tGrpConfig), m_nIvpsGrp(tGrpConfig.nGrp) {
}

AX_BOOL CIVPSGrpStage::Init() {
    SetCapacity(AX_APP_LOCKQ_CAPACITY);
    return AX_TRUE;
}

AX_BOOL CIVPSGrpStage::DeInit() {
    if (m_bStarted) {
        Stop();
    }

    return AX_TRUE;
}

AX_BOOL CIVPSGrpStage::Start(STAGE_START_PARAM_PTR pStartParams) {
    IVPS_GRP_T& tGrp = m_tIvpsGrp;

    LOG_MM_I(IVPS, "[%d] +++", m_nIvpsGrp);

    if (0 == m_tIvpsGrp.tPipelineAttr.nOutChnNum) {
        return AX_FALSE;
    }

    AX_U16 nIvpsGrp = tGrp.nGroup;
    AX_S32 ret = AX_IVPS_CreateGrp(nIvpsGrp, &tGrp.tGroupAttr);
    if (AX_SUCCESS != ret) {
        LOG_M_E(IVPS, "AX_IVPS_CreateGrp(Grp: %d) failed, ret=0x%x", nIvpsGrp, ret);
        return AX_FALSE;
    }

    ret = AX_IVPS_SetPipelineAttr(nIvpsGrp, &tGrp.tPipelineAttr);
    if (AX_SUCCESS != ret) {
        LOG_MM_E(IVPS, "AX_IVPS_SetPipelineAttr(Grp: %d) failed, ret=0x%x", nIvpsGrp, ret);
        return AX_FALSE;
    }

    for (AX_U8 nChn = 0; nChn < tGrp.tPipelineAttr.nOutChnNum; ++nChn) {
        ret = AX_IVPS_EnableChn(nIvpsGrp, nChn);
        if (AX_SUCCESS != ret) {
            LOG_M_E(IVPS, "AX_IVPS_EnableChn(Grp: %d, Chn: %d) failed, ret=0x%x", nIvpsGrp, nChn, ret);
            return AX_FALSE;
        }
        m_mapChnState[nChn] = AX_TRUE;
        LOG_M(IVPS, "Enable channel (Grp: %d, Chn: %d)", nIvpsGrp, nChn);
    }

    ret = AX_IVPS_StartGrp(nIvpsGrp);
    if (AX_SUCCESS != ret) {
        LOG_M_E(IVPS, "AX_IVPS_StartGrp(Grp: %d) failed, ret=0x%x", nIvpsGrp, ret);
        return AX_FALSE;
    }

    StartWorkThread();

    if (!StartOSD()) {
        return AX_FALSE;
    }

    m_bStarted = AX_TRUE;

    LOG_MM_I(IVPS, "[%d] ---", m_nIvpsGrp);

    return CAXStage::Start(pStartParams);
}

AX_BOOL CIVPSGrpStage::Stop() {
    LOG_MM_I(IVPS, "[%d] +++", m_nIvpsGrp);

    AX_S32 ret = AX_SUCCESS;
    if (0 == m_tIvpsGrp.tPipelineAttr.nOutChnNum) {
        return AX_FALSE;
    }

    if (!StopOSD()) {
        return AX_FALSE;
    }

    StopWorkThread();

    ret = AX_IVPS_StopGrp(m_nIvpsGrp);
    if (AX_SUCCESS != ret) {
        LOG_M_E(IVPS, "AX_IVPS_StopGrp(Grp: %d) failed, ret=0x%x", m_nIvpsGrp, ret);
    }

    for (AX_U8 chn = 0; chn < m_tIvpsGrp.tPipelineAttr.nOutChnNum; ++chn) {
        ret = AX_IVPS_DisableChn(m_nIvpsGrp, chn);
        if (AX_SUCCESS != ret) {
            LOG_M_E(IVPS, "AX_IVPS_DisableChn(Grp: %d, Channel: %d) failed, ret=0x%x", m_nIvpsGrp, chn, ret);
        }
        m_mapChnState[chn] = AX_FALSE;
    }

    ret = AX_IVPS_DestoryGrp(m_nIvpsGrp);
    if (AX_SUCCESS != ret) {
        LOG_M_E(IVPS, "AX_IVPS_DestoryGrp(Grp: %d) failed, ret=0x%x", m_nIvpsGrp, ret);
    }

    m_bStarted = AX_FALSE;

    LOG_MM_I(IVPS, "[%d] ---", m_nIvpsGrp);

    return CAXStage::Stop();
}

AX_VOID CIVPSGrpStage::FrameGetThreadFunc(IVPS_GET_THREAD_PARAM_PTR pThreadParam) {
    AX_S32 nRet = AX_SUCCESS;

    AX_U8 nIvpsGrp = pThreadParam->nIvpsGrp;
    AX_U8 nIvpsChn = pThreadParam->nIvpsChn;

    AX_CHAR szName[50] = {0};
    sprintf(szName, "APP_IVPS_Get_%d", nIvpsGrp);
    prctl(PR_SET_NAME, szName);

    AX_BOOL bLink = m_tIvpsGrpCfg.arrChnLinkFlag[nIvpsChn] == 0 ? AX_FALSE : AX_TRUE;

    LOG_MM_I(IVPS, "[%d][%d] +++ bLink:%d ", nIvpsGrp, nIvpsChn, bLink);

    pThreadParam->bExit = AX_FALSE;
    while (!pThreadParam->bExit) {
        if (!pThreadParam->nChnEnable) {
            CElapsedTimer::GetInstance()->mSleep(100);
            continue;
        }

        if (bLink) {
            CElapsedTimer::GetInstance()->mSleep(200);
            bLink = m_tIvpsGrpCfg.arrChnLinkFlag[nIvpsChn] == 0 ? AX_FALSE : AX_TRUE;
            continue;
        }

        CAXFrame* pFrame = new (std::nothrow) CAXFrame();
        if (!pFrame) {
            LOG_M_E(IVPS, "alloc MediaFrame instance fail");
            continue;
        }

        nRet = AX_IVPS_GetChnFrame(nIvpsGrp, nIvpsChn, &pFrame->stFrame.stVFrame.stVFrame, 95);
        if (AX_SUCCESS != nRet) {
            if (AX_ERR_IVPS_BUF_EMPTY == nRet) {
                delete pFrame;
                CElapsedTimer::GetInstance()->mSleep(1);
                continue;
            }
            delete pFrame;
            LOG_M(IVPS, "[%d][%d] Get ivps frame failed. ret=0x%x", nIvpsGrp, nIvpsChn, nRet);
            CElapsedTimer::GetInstance()->mSleep(1);
            continue;
        }
        LOG_MM_D(IVPS, "[%d][%d] Seq: %lld, w:%d, h:%d, size:%u, release:%p,PhyAddr:%lld", nIvpsGrp, nIvpsChn,
                 pFrame->stFrame.stVFrame.stVFrame.u64SeqNum, pFrame->stFrame.stVFrame.stVFrame.u32Width,
                 pFrame->stFrame.stVFrame.stVFrame.u32Height, pFrame->stFrame.stVFrame.stVFrame.u32FrameSize, pThreadParam->pReleaseStage,
                 pFrame->stFrame.stVFrame.stVFrame.u64PhyAddr[0]);

        pFrame->nGrp = nIvpsGrp;
        pFrame->nChn = nIvpsChn;
        pFrame->pFrameRelease = pThreadParam->pReleaseStage;
        NotifyAll(nIvpsGrp, nIvpsChn, pFrame);
    }

    LOG_MM(IVPS, "[%d][%d] ---", nIvpsGrp, nIvpsChn);
}

AX_VOID CIVPSGrpStage::VideoFrameRelease(CAXFrame* pFrame) {
    if (pFrame) {
        if (!pFrame->bMultiplex || pFrame->DecFrmRef() == 0) {
            LOG_M_D(IVPS, "[%d][%d] AX_IVPS_ReleaseChnFrame, seq=%lld, pFrqame:%p", pFrame->nGrp, pFrame->nChn,
                    pFrame->stFrame.stVFrame.stVFrame.u64SeqNum, pFrame);
            AX_IVPS_ReleaseChnFrame(pFrame->nGrp, pFrame->nChn, &pFrame->stFrame.stVFrame.stVFrame);
            delete pFrame;
        }
    }
}

AX_BOOL CIVPSGrpStage::RecvFrame(CAXFrame* pFrame) {
    if (!EnqueueFrame(pFrame)) {
        return AX_FALSE;
    }

    return AX_TRUE;
}

AX_BOOL CIVPSGrpStage::ProcessFrame(CAXFrame* pFrame) {
    if (!pFrame) {
        return AX_FALSE;
    }
    AX_S32 ret = AX_IVPS_SendFrame(m_nIvpsGrp, &pFrame->stFrame.stVFrame.stVFrame, MAX_IPC_IVPS_FRAME_TIMEOUT);
    if (AX_SUCCESS != ret) {
        LOG_M_E(IVPS, "AX_IVPS_SendFrame(Grp %d, size:%u, Seq %d) failed, ret=0x%x", m_nIvpsGrp,
                pFrame->stFrame.stVFrame.stVFrame.u32FrameSize, pFrame->stFrame.stVFrame.stVFrame.u64SeqNum, ret);
        return AX_FALSE;
    } else {
        LOG_MM_D(IVPS, "[%d] AX_IVPS_SendFrame successfully, PhyAddr:0x%llx ,size:%u, seq=%d, bMux=%d.", m_nIvpsGrp,
                 pFrame->stFrame.stVFrame.stVFrame.u64PhyAddr[0], pFrame->stFrame.stVFrame.stVFrame.u32FrameSize,
                 pFrame->stFrame.stVFrame.stVFrame.u64SeqNum, pFrame->bMultiplex);
    }

    return AX_TRUE;
}

AX_VOID CIVPSGrpStage::StartWorkThread() {
    LOG_MM_I(IVPS, "[%d] +++", m_tIvpsGrpCfg.nGrp);

    /* Start frame get thread */
    for (AX_U8 nChn = 0; nChn < m_tIvpsGrp.tPipelineAttr.nOutChnNum; ++nChn) {
        /* get thread param */
        m_tGetThreadParam[nChn].nChnEnable = AX_TRUE;
        m_tGetThreadParam[nChn].nIvpsGrp = m_nIvpsGrp;
        m_tGetThreadParam[nChn].nIvpsChn = nChn;
        m_tGetThreadParam[nChn].pReleaseStage = this;
        m_tGetThreadParam[nChn].bExit = AX_FALSE;
        m_hGetThread[nChn] = std::thread(&CIVPSGrpStage::FrameGetThreadFunc, this, &m_tGetThreadParam[nChn]);
    }

    LOG_MM_I(IVPS, "[%d] ---", m_tIvpsGrpCfg.nGrp);
}

AX_VOID CIVPSGrpStage::StopWorkThread() {
    LOG_MM_I(IVPS, "[%d] +++", m_tIvpsGrpCfg.nGrp);

    for (AX_U8 nChn = 0; nChn < m_tIvpsGrp.tPipelineAttr.nOutChnNum; ++nChn) {
        m_tGetThreadParam[nChn].bExit = AX_TRUE;
    }

    for (AX_U8 nChn = 0; nChn < m_tIvpsGrp.tPipelineAttr.nOutChnNum; ++nChn) {
        if (m_hGetThread[nChn].joinable()) {
            m_hGetThread[nChn].join();
        }
    }

    LOG_MM_I(IVPS, "[%d] ---", m_tIvpsGrpCfg.nGrp);
}

AX_BOOL CIVPSGrpStage::InitParams(AX_BOOL bVinRotate/*= AX_FALSE*/, AX_BOOL bVinMirrorFlip/*= AX_FALSE*/) {
    LOG_MM_I(IVPS, "[%d] Enable: %d +++", m_tIvpsGrpCfg.nGrp, m_tIvpsGrpCfg.bEnable);

    m_tIvpsGrp = IVPS_GRP_T{};
    m_tIvpsGrp.nGroup = m_nIvpsGrp;
    /* Config group attr */
    m_tIvpsGrp.tGroupAttr.nInFifoDepth = IVPS_IN_FIFO_DEPTH;
    m_tIvpsGrp.tGroupAttr.ePipeline = AX_IVPS_PIPELINE_DEFAULT;

    /* Config pipeline attr */
    m_tIvpsGrp.tPipelineAttr.nOutChnNum = m_tIvpsGrpCfg.nGrpChnNum;
    m_tIvpsGrp.tPipelineAttr.nInDebugFifoDepth = 0;
    for (AX_U8 i = 0; i < m_tIvpsGrpCfg.nGrpChnNum; i++) {
        m_tIvpsGrp.tPipelineAttr.nOutFifoDepth[i] = IVPS_OUT_FIFO_DEPTH;
    }

    AX_IVPS_ENGINE_E eFilterEngine{AX_IVPS_ENGINE_BUTT};

    m_arrGrpResolution[0] = m_tIvpsGrpCfg.arrGrpResolution[0];
    m_arrGrpResolution[1] = m_tIvpsGrpCfg.arrGrpResolution[1];
    InitGrpFbc();
    m_tIvpsGrpCfg.bGrpIOAccordant = IsGrpIOAccordant();

    AX_U32 nGrpWidth  = m_arrGrpResolution[0];
    AX_U32 nGrpHeight = m_arrGrpResolution[1];
    for (AX_U8 i = 0; i < AX_IVPS_MAX_FILTER_NUM_PER_OUTCHN; i++) {
        /* Config group filter */
        m_tIvpsGrp.tPipelineAttr.tFilter[0][i].eEngine = AX_IVPS_ENGINE_BUTT;
        if (m_tIvpsGrpCfg.arrGrpEngineType[i] != AX_IVPS_ENGINE_BUTT) {
            AX_U32 nStride = m_nGrpFBC[i] != AX_COMPRESS_MODE_NONE ? AX_IVPS_FBC_STRIDE_ALIGN_VAL : AX_IVPS_FBC_WIDTH_ALIGN_VAL;
            nGrpWidth  = m_arrGrpResolution[0];
            nGrpHeight = m_arrGrpResolution[1];

            GetGrpRotateResolution(m_eRotation, i, (AX_BOOL)m_nGrpFBC[i], nGrpWidth, nGrpHeight);

            m_tIvpsGrp.tPipelineAttr.tFilter[0][i].bEngage = m_tIvpsGrpCfg.bEnable;
            m_tIvpsGrp.tPipelineAttr.tFilter[0][i].eEngine = m_tIvpsGrpCfg.arrGrpEngineType[i];
            m_tIvpsGrp.tPipelineAttr.tFilter[0][i].eDstPicFormat = AX_FORMAT_YUV420_SEMIPLANAR;
            m_tIvpsGrp.tPipelineAttr.tFilter[0][i].nDstPicWidth = nGrpWidth;
            m_tIvpsGrp.tPipelineAttr.tFilter[0][i].nDstPicHeight = nGrpHeight;
            m_tIvpsGrp.tPipelineAttr.tFilter[0][i].nDstPicStride = ALIGN_UP(nGrpWidth, nStride);
            m_tIvpsGrp.tPipelineAttr.tFilter[0][i].tFRC.fSrcFrameRate = m_tIvpsGrpCfg.arrGrpFramerate[0];
            m_tIvpsGrp.tPipelineAttr.tFilter[0][i].tFRC.fDstFrameRate = m_tIvpsGrpCfg.arrGrpFramerate[1];
            m_tIvpsGrp.tPipelineAttr.tFilter[0][i].tCompressInfo.enCompressMode = (AX_COMPRESS_MODE_E)m_nGrpFBC[0];
            m_tIvpsGrp.tPipelineAttr.tFilter[0][i].tCompressInfo.u32CompressLevel = m_nGrpFBC[1];

            eFilterEngine = m_tIvpsGrpCfg.arrGrpEngineType[i];

        // if (AX_IVPS_ENGINE_GDC == m_tIvpsGrp.tPipelineAttr.tFilter[0][0].eEngine) {
        //     /* fixme: The value should be from local file*/
        //     m_tIvpsGrp.tPipelineAttr.tFilter[0][0].tGdcCfg.eDewarpType = AX_IVPS_DEWARP_BYPASS;
        //     if (AX_TRUE == m_tIvpsGrpCfg.nLdcEnable) {
        //         m_tIvpsGrp.tPipelineAttr.tFilter[0][0].tGdcCfg.eDewarpType = AX_IVPS_DEWARP_LDC;
        //         m_tIvpsGrp.tPipelineAttr.tFilter[0][0].tGdcCfg.tLdcAttr.bAspect = m_tIvpsGrpCfg.bLdcAspect;
        //         m_tIvpsGrp.tPipelineAttr.tFilter[0][0].tGdcCfg.tLdcAttr.nXRatio = m_tIvpsGrpCfg.nLdcXRatio;
        //         m_tIvpsGrp.tPipelineAttr.tFilter[0][0].tGdcCfg.tLdcAttr.nYRatio = m_tIvpsGrpCfg.nLdcYRatio;
        //         m_tIvpsGrp.tPipelineAttr.tFilter[0][0].tGdcCfg.tLdcAttr.nXYRatio = m_tIvpsGrpCfg.nLdcXYRatio;
        //         m_tIvpsGrp.tPipelineAttr.tFilter[0][0].tGdcCfg.tLdcAttr.nDistortionRatio = m_tIvpsGrpCfg.nLdcDistortionRatio;
        //     }

        // }

            if (m_tIvpsGrpCfg.bRotationEngine || m_tIvpsGrpCfg.bMirrorFlipEngine) {
                m_tIvpsGrp.tPipelineAttr.tFilter[0][i].bEngage = AX_FALSE;

                if (bVinRotate) {
                    m_tIvpsGrpCfg.bRotationEngine = AX_FALSE;
                }

                if (bVinMirrorFlip) {
                    m_tIvpsGrpCfg.bMirrorFlipEngine = AX_FALSE;
                }
            }

            LOG_MM_I(IVPS, "[%d] Grp filter%d: engine:%d, w:%d, h:%d, s:%d, frameRate[%f,%f]", m_tIvpsGrp.nGroup, i, m_tIvpsGrpCfg.arrGrpEngineType[i],
                m_tIvpsGrp.tPipelineAttr.tFilter[0][i].nDstPicWidth, m_tIvpsGrp.tPipelineAttr.tFilter[0][i].nDstPicHeight,
                m_tIvpsGrp.tPipelineAttr.tFilter[0][i].nDstPicStride, m_tIvpsGrp.tPipelineAttr.tFilter[0][i].tFRC.fSrcFrameRate,
                m_tIvpsGrp.tPipelineAttr.tFilter[0][i].tFRC.fDstFrameRate);
        }
    }

    /* Config channel filter */
    for (AX_U8 i = 0; i < m_tIvpsGrpCfg.nGrpChnNum; i++) {
        m_arrChnResolution[i][0] = m_tIvpsGrpCfg.arrChnResolution[i][0];
        m_arrChnResolution[i][1] = m_tIvpsGrpCfg.arrChnResolution[i][1];
        InitChnFbc(i);
        m_tIvpsGrpCfg.bArrChnIOAccordant[i] = IsChnIOAccordant(i);

        AX_U8 nChnFilter = i + 1;
        AX_U32 nStride = m_nChnFBC[i][0] != AX_COMPRESS_MODE_NONE ? AX_IVPS_FBC_STRIDE_ALIGN_VAL : AX_IVPS_FBC_WIDTH_ALIGN_VAL;
        m_arrChnInitAlignResolution[i][0] = nGrpWidth;
        m_arrChnInitAlignResolution[i][1] = nGrpHeight;
        for (AX_U8 j = 0; j < AX_IVPS_MAX_FILTER_NUM_PER_OUTCHN; j++) {
            m_tIvpsGrp.tPipelineAttr.tFilter[nChnFilter][j].eEngine = AX_IVPS_ENGINE_BUTT;
            if (m_tIvpsGrpCfg.arrChnEngineType[i][j] != AX_IVPS_ENGINE_BUTT) {
                GetChnRotateResolution(i, m_eRotation, j, (AX_BOOL)m_nChnFBC[i][0], m_arrChnInitAlignResolution[i][0], m_arrChnInitAlignResolution[i][1]);

                m_tIvpsGrp.tPipelineAttr.tFilter[nChnFilter][j].bEngage = m_tIvpsGrpCfg.bEnable;
                m_tIvpsGrp.tPipelineAttr.tFilter[nChnFilter][j].eEngine = m_tIvpsGrpCfg.arrChnEngineType[i][j];
                m_tIvpsGrp.tPipelineAttr.tFilter[nChnFilter][j].eDstPicFormat = AX_FORMAT_YUV420_SEMIPLANAR;

                m_tIvpsGrp.tPipelineAttr.tFilter[nChnFilter][j].nDstPicWidth = m_arrChnInitAlignResolution[i][0];
                m_tIvpsGrp.tPipelineAttr.tFilter[nChnFilter][j].nDstPicHeight = m_arrChnInitAlignResolution[i][1];
                m_tIvpsGrp.tPipelineAttr.tFilter[nChnFilter][j].nDstPicStride = ALIGN_UP(m_arrChnInitAlignResolution[i][0], nStride);
                m_tIvpsGrp.tPipelineAttr.tFilter[nChnFilter][j].tFRC.fSrcFrameRate = (m_tIvpsGrpCfg.arrChnFramerate[i][0]);
                m_tIvpsGrp.tPipelineAttr.tFilter[nChnFilter][j].tFRC.fDstFrameRate = (m_tIvpsGrpCfg.arrChnFramerate[i][1]);
                m_tIvpsGrp.tPipelineAttr.tFilter[nChnFilter][j].tCompressInfo.enCompressMode = (AX_COMPRESS_MODE_E)m_nChnFBC[i][0];
                m_tIvpsGrp.tPipelineAttr.tFilter[nChnFilter][j].tCompressInfo.u32CompressLevel = m_nChnFBC[i][1];

                switch(m_tIvpsGrp.tPipelineAttr.tFilter[nChnFilter][j].eEngine) {
                    case AX_IVPS_ENGINE_TDP:
                    case AX_IVPS_ENGINE_VPP:
                    case AX_IVPS_ENGINE_VGP:
                        m_tIvpsGrp.tPipelineAttr.tFilter[nChnFilter][j].bInplace = m_tIvpsGrpCfg.bArrChnInplace[i];
                        break;
                    default:
                        break;
                }

                eFilterEngine = m_tIvpsGrpCfg.arrChnEngineType[i][j];

                LOG_MM_I(IVPS, "[%d][%d] Chn filter 0x%02x: engine:%d, w:%d, h:%d, s:%d, inFPS:%f, outFPS:%f, Compress(%d, %d)", m_tIvpsGrp.nGroup,
                    i, nChnFilter << 4, m_tIvpsGrpCfg.arrChnEngineType[i][j], m_tIvpsGrp.tPipelineAttr.tFilter[nChnFilter][j].nDstPicWidth,
                    m_tIvpsGrp.tPipelineAttr.tFilter[nChnFilter][j].nDstPicHeight,
                    m_tIvpsGrp.tPipelineAttr.tFilter[nChnFilter][j].nDstPicStride,
                    m_tIvpsGrp.tPipelineAttr.tFilter[nChnFilter][j].tFRC.fSrcFrameRate,
                    m_tIvpsGrp.tPipelineAttr.tFilter[nChnFilter][j].tFRC.fDstFrameRate,
                    m_tIvpsGrp.tPipelineAttr.tFilter[nChnFilter][j].tCompressInfo.enCompressMode,
                    m_tIvpsGrp.tPipelineAttr.tFilter[nChnFilter][j].tCompressInfo.u32CompressLevel);
            }
        }
    }

    if (m_tIvpsGrpCfg.bEnable) {
        if (AX_IVPS_ENGINE_BUTT == eFilterEngine) {
            LOG_MM_I(IVPS, "[%d] no engine set, pls check ivps config!", m_tIvpsGrpCfg.nGrp);
            return AX_FALSE;
        }

        m_eOrgResizeEngine = eFilterEngine;

        for (AX_U8 i = 0; i < m_tIvpsGrpCfg.nGrpChnNum; i++) {
            if (-1 != m_tIvpsGrpCfg.arrOsdLinkedVideo[i]) {
                m_pOsdHelper->SetOsdConfig(m_tIvpsGrpCfg.nSnsSrc, m_tIvpsGrpCfg.nGrp, i, AX_TRUE, m_tIvpsGrpCfg.stOsdShowFlag, m_tIvpsGrpCfg.arrOsdLinkedVideo[i]);
                LOG_MM_N(IVPS, "[IVPS OSD Cfg]IVPS Grp(%d) attach to Sns(%d) enable osd for venc(vchn: %d).", m_tIvpsGrpCfg.nGrp, m_tIvpsGrpCfg.nSnsSrc, m_tIvpsGrpCfg.arrOsdLinkedVideo[i]);
            }
        }

        for (AX_U8 i = 0; i < m_tIvpsGrpCfg.nGrpChnNum; i++) {
            if (0 != m_tIvpsGrpCfg.arrOsdNoVideoOnFlag[i]) {
                m_pOsdHelper->SetOsdConfig(m_tIvpsGrpCfg.nSnsSrc, m_tIvpsGrpCfg.nGrp, 0, AX_FALSE, m_tIvpsGrpCfg.stOsdShowFlag);
                LOG_MM_N(IVPS, "[IVPS OSD Cfg]IVPS Grp(%d) attach to Sns(%d) enable osd for jpg.", m_tIvpsGrpCfg.nGrp, m_tIvpsGrpCfg.nSnsSrc);
            }
        }

        if (m_tIvpsGrpCfg.bMaskEnable) {
            m_pOsdHelper->SetPrivacyConfig(m_tIvpsGrpCfg.nSnsSrc, m_tIvpsGrpCfg.nGrp);
            LOG_MM_N(IVPS, "[IVPS OSD Cfg]IVPS Grp(%d) attach to Sns(%d) enable pricacy.", m_tIvpsGrpCfg.nGrp, m_tIvpsGrpCfg.nSnsSrc);
        }
    } else { // bypass grp, pass pre grp info to next grp
        for (AX_U8 i = 0; i < m_tIvpsGrpCfg.nGrpChnNum; i++) {
            m_nChnFBC[i][0] = m_tIvpsGrpCfg.arrInGrpFBC[0];
            m_nChnFBC[i][1] = m_tIvpsGrpCfg.arrInGrpFBC[1];

            m_tIvpsGrpCfg.arrChnFramerate[i][0] = m_tIvpsGrpCfg.arrGrpFramerate[0];
            m_tIvpsGrpCfg.arrChnFramerate[i][1] = m_tIvpsGrpCfg.arrGrpFramerate[1];

            m_tIvpsGrpCfg.arrChnResolution[i][0] = m_tIvpsGrpCfg.arrGrpInResolution[0];
            m_tIvpsGrpCfg.arrChnResolution[i][1] = m_tIvpsGrpCfg.arrGrpInResolution[1];

            LOG_MM_I(IVPS, "[%d][%d] Bypass Info: fbc(%d, %d), fps(%f, %f), resolution(%d, %d).",
                            m_tIvpsGrpCfg.nGrp, i, m_nChnFBC[i][0], m_nChnFBC[i][1],
                            m_tIvpsGrpCfg.arrChnFramerate[i][0], m_tIvpsGrpCfg.arrChnFramerate[i][1],
                            m_tIvpsGrpCfg.arrChnResolution[i][0], m_tIvpsGrpCfg.arrChnResolution[i][1]);
        }
    }

    LOG_MM_I(IVPS, "[%d] Enable: %d ---", m_tIvpsGrpCfg.nGrp, m_tIvpsGrpCfg.bEnable);

    return AX_TRUE;
}

AX_VOID CIVPSGrpStage::SetChnInplace(AX_S32 nChannel, AX_BOOL bEnable) {
    AX_S32 nChnFilter = nChannel + 1;
    m_tIvpsGrp.tPipelineAttr.tFilter[nChnFilter][1].bInplace = bEnable;
}

AX_VOID CIVPSGrpStage::RegObserver(AX_S32 nChannel, IObserver* pObserver) {
    if (nullptr != pObserver) {
        OBS_TRANS_ATTR_T tTransAttr;
        tTransAttr.fSrcFramerate = m_tIvpsGrpCfg.arrChnFramerate[nChannel][0]; /* 0: src framerate*/
        tTransAttr.fFramerate = m_tIvpsGrpCfg.arrChnFramerate[nChannel][1];    /* 1: Out framerate */
        tTransAttr.nWidth = m_arrChnInitAlignResolution[nChannel][0];
        tTransAttr.nHeight = m_arrChnInitAlignResolution[nChannel][1];
        tTransAttr.nCfgWidth = m_tIvpsGrpCfg.arrChnResolution[nChannel][0];
        tTransAttr.nCfgHeight = m_tIvpsGrpCfg.arrChnResolution[nChannel][1];
        tTransAttr.arrOutFBC[0] = m_nChnFBC[nChannel][0];
        tTransAttr.arrOutFBC[1] = m_nChnFBC[nChannel][1];
        tTransAttr.bLink = m_tIvpsGrpCfg.arrChnLinkFlag[nChannel] == 0 ? AX_FALSE : AX_TRUE;
        tTransAttr.nSnsSrc = m_tIvpsGrpCfg.nSnsSrc;
        if (pObserver->OnRegisterObserver(E_OBS_TARGET_TYPE_IVPS, m_nIvpsGrp, nChannel, &tTransAttr)) {
            for (auto obServer: m_vecObserver) {
                if (obServer == pObserver) {
                    return;
                }
            }
            m_vecObserver.emplace_back(pObserver);
            if (!tTransAttr.bLink) {
                m_vecUnlinkObserver.emplace_back(pObserver);
            }
        }
    }
}

AX_VOID CIVPSGrpStage::UnregObserver(AX_S32 nChannel, IObserver* pObserver) {
    if (nullptr == pObserver) {
        return;
    }

    for (vector<IObserver*>::iterator it = m_vecObserver.begin(); it != m_vecObserver.end(); it++) {
        if (*it == pObserver) {
            m_vecObserver.erase(it);
            break;
        }
    }

    for (vector<IObserver*>::iterator it = m_vecUnlinkObserver.begin(); it != m_vecUnlinkObserver.end(); it++) {
        if (*it == pObserver) {
            m_vecUnlinkObserver.erase(it);
            break;
        }
    }
}

AX_VOID CIVPSGrpStage::NotifyAll(AX_U32 nGrp, AX_U32 nChn, AX_VOID* pFrame) {
    if (m_vecUnlinkObserver.size() == 0) {
        ((CAXFrame*)pFrame)->FreeMem();
        return;
    }

    for (vector<IObserver*>::iterator it = m_vecUnlinkObserver.begin(); it != m_vecUnlinkObserver.end(); it++) {
        (*it)->OnRecvData(E_OBS_TARGET_TYPE_IVPS, m_nIvpsGrp, nChn, pFrame);
    }
}

AX_BOOL CIVPSGrpStage::OnDataChange(AX_U32 nChannel, OBS_CHANGE_DATA_TYPE_E eDataType, AX_VOID* pData) {
    if (nullptr == pData) {
        return AX_TRUE;
    }

    AX_BOOL bRet = AX_TRUE;
    for (vector<IObserver*>::iterator it = m_vecObserver.begin(); it != m_vecObserver.end(); it++) {
        if (!(*it)->OnDataChange(E_OBS_TARGET_TYPE_IVPS, m_tIvpsGrpCfg.nGrp, nChannel, eDataType, pData)) {
            bRet = AX_FALSE;
        }
    }

    return bRet;
}

AX_BOOL CIVPSGrpStage::EnableChannel(AX_U8 nChn, AX_BOOL bEnable /*= AX_TRUE*/) {
    AX_S32 nRet = AX_SUCCESS;
    if (bEnable) {
        nRet = AX_IVPS_EnableChn(m_tIvpsGrpCfg.nGrp, nChn);
        if (AX_SUCCESS != nRet) {
            LOG_M_E(IVPS, "AX_IVPS_EnableChn(Grp:%d, Chn:%d) failed, ret=0x%x", m_tIvpsGrpCfg.nGrp, nChn, nRet);
            return AX_FALSE;
        } else {
            LOG_M(IVPS, "AX_IVPS_EnableChn(Grp:%d, Chn:%d) successfully", m_tIvpsGrpCfg.nGrp, nChn);
        }
    } else {
        nRet = AX_IVPS_DisableChn(m_tIvpsGrpCfg.nGrp, nChn);
        if (AX_SUCCESS != nRet) {
            LOG_M_E(IVPS, "AX_IVPS_DisableChn(Grp:%d, Chn:%d) failed, ret=0x%x", m_tIvpsGrpCfg.nGrp, nChn, nRet);
            return AX_FALSE;
        } else {
            LOG_M(IVPS, "AX_IVPS_DisableChn(Grp:%d, Chn:%d) successfully", m_tIvpsGrpCfg.nGrp, nChn);
        }
    }
    m_tGetThreadParam[nChn].nChnEnable = bEnable;
    m_mapChnState[nChn] = bEnable;
    return AX_TRUE;
}

AX_BOOL CIVPSGrpStage::UpdateResolution(AX_U8 nRotation, AX_U32 nWidth, AX_U32 nHeight, AX_BOOL bRotation, AX_BOOL bDoResize/*= AX_FALSE*/) {
    AX_IVPS_PIPELINE_ATTR_T& tPipelineAttr = m_tIvpsGrp.tPipelineAttr;
    AX_U32 nGrpWidth  = nWidth;
    AX_U32 nGrpHeight = nHeight;

    /* Suppose that only one filter(0 or 1) set for Chn or Grp */
    /* Update Grp resolution */
    for (AX_U8 i = 0; i < AX_IVPS_MAX_FILTER_NUM_PER_OUTCHN; i++) {
        AX_BOOL bFBC = (0 == m_nGrpFBC[0]) ? AX_FALSE : AX_TRUE;
        AX_U32 nStrideAlign = bFBC ? AX_IVPS_FBC_STRIDE_ALIGN_VAL : AX_IVPS_FBC_WIDTH_ALIGN_VAL;
        if (tPipelineAttr.tFilter[0][i].eEngine != AX_IVPS_ENGINE_BUTT) {
            GetGrpRotateResolution((AX_IVPS_ROTATION_E)nRotation, i, bFBC, nGrpWidth, nGrpHeight);
            tPipelineAttr.tFilter[0][i].nDstPicWidth = nGrpWidth;
            tPipelineAttr.tFilter[0][i].nDstPicHeight = nGrpHeight;
            tPipelineAttr.tFilter[0][i].nDstPicStride = ALIGN_UP(nGrpWidth, nStrideAlign);

            if (!bFBC) {
                tPipelineAttr.tFilter[0][i].tCompressInfo.enCompressMode = AX_COMPRESS_MODE_NONE;
                tPipelineAttr.tFilter[0][i].tCompressInfo.u32CompressLevel = 0;
            } else {
                tPipelineAttr.tFilter[0][i].tCompressInfo.enCompressMode = (AX_COMPRESS_MODE_E)m_nGrpFBC[0];
                tPipelineAttr.tFilter[0][i].tCompressInfo.u32CompressLevel = m_nGrpFBC[1];
            }
            LOG_MM_C(IVPS, "Grp(%d) Filter%d reset resolution : w: %d, h: %d, s: %d compress[%d,%d] with rotation(%d).",
                        m_tIvpsGrpCfg.nGrp,
                        i,
                        tPipelineAttr.tFilter[0][i].nDstPicWidth, tPipelineAttr.tFilter[0][i].nDstPicHeight,
                        tPipelineAttr.tFilter[0][i].nDstPicStride, tPipelineAttr.tFilter[0][i].tCompressInfo.enCompressMode,
                        tPipelineAttr.tFilter[0][i].tCompressInfo.u32CompressLevel,
                        nRotation);
        }
    }

    AX_U16 arrChnResolution[m_tIvpsGrpCfg.nGrpChnNum][2]{0};
    for (AX_U8 i = 0; i < m_tIvpsGrpCfg.nGrpChnNum; i++) {
        AX_U32 nChnWidth  = nGrpWidth;
        AX_U32 nChnHeight = nGrpHeight;
        for (AX_U8 j = 0; j < AX_IVPS_MAX_FILTER_NUM_PER_OUTCHN; j++) {
            if (tPipelineAttr.tFilter[i + 1][j].bEngage) {
                AX_BOOL bFBC = (0 == m_nChnFBC[i][0]) ? AX_FALSE : AX_TRUE;
                AX_U32 nStrideAlign = bFBC ? AX_IVPS_FBC_STRIDE_ALIGN_VAL : AX_IVPS_FBC_WIDTH_ALIGN_VAL;
                GetChnRotateResolution(i, (AX_IVPS_ROTATION_E)nRotation, j, bFBC, nChnWidth, nChnHeight, bDoResize);

                tPipelineAttr.tFilter[i + 1][j].nDstPicWidth = nChnWidth;
                tPipelineAttr.tFilter[i + 1][j].nDstPicHeight = nChnHeight;
                tPipelineAttr.tFilter[i + 1][j].nDstPicStride = ALIGN_UP(nChnWidth, nStrideAlign);
                if (!bFBC) {
                    tPipelineAttr.tFilter[i + 1][j].tCompressInfo.enCompressMode = AX_COMPRESS_MODE_NONE;
                    tPipelineAttr.tFilter[i + 1][j].tCompressInfo.u32CompressLevel = 0;
                } else {
                    tPipelineAttr.tFilter[i + 1][j].tCompressInfo.enCompressMode = (AX_COMPRESS_MODE_E)m_nChnFBC[i][0];
                    tPipelineAttr.tFilter[i + 1][j].tCompressInfo.u32CompressLevel = m_nChnFBC[i][1];
                }

                LOG_MM_C(IVPS, "Grp(%d) Chn(%d) filter%d reset resolution: w: %d, h: %d, s: %d compress[%d,%d] with rotation(%d).",
                    m_tIvpsGrpCfg.nGrp,
                    i,
                    j,
                    tPipelineAttr.tFilter[i + 1][j].nDstPicWidth, tPipelineAttr.tFilter[i + 1][j].nDstPicHeight,
                    tPipelineAttr.tFilter[i + 1][j].nDstPicStride,
                    tPipelineAttr.tFilter[i + 1][j].tCompressInfo.enCompressMode,
                    tPipelineAttr.tFilter[i + 1][j].tCompressInfo.u32CompressLevel,
                    nRotation);
            }
        }

        arrChnResolution[i][0] = nChnWidth;
        arrChnResolution[i][1] = nChnHeight;
    }

    AX_S32 nRet = AX_IVPS_SetPipelineAttr(m_tIvpsGrpCfg.nGrp, &tPipelineAttr);
    if (AX_SUCCESS != nRet) {
        LOG_M_E(IVPS, "AX_IVPS_SetPipelineAttr(Grp %d) failed, ret=0x%x", m_tIvpsGrpCfg.nGrp, nRet);
        return AX_FALSE;
    }

    if (!bRotation) { // Resize
        for (AX_U8 i = 0; i < m_tIvpsGrpCfg.nGrpChnNum; i++) {
            OBS_TRANS_RESOLUTION_T stObsTransResolution;
            stObsTransResolution.nWidth  = arrChnResolution[i][0];
            stObsTransResolution.nHeight = arrChnResolution[i][1];
            OnDataChange(i, E_OBS_CHANGE_DATA_TYPE_RESOLUTION, &stObsTransResolution);
        }

        return AX_TRUE;
    }

    for (AX_U8 i = 0; i < m_tIvpsGrpCfg.nGrpChnNum; i++) {
        OBS_TRANS_ROTATE_INFO_T stObsTransRotation;
        stObsTransRotation.nRotation = nRotation;
        stObsTransRotation.bInFBC    = (0 == m_nChnFBC[i][0]) ? AX_FALSE : AX_TRUE;
        stObsTransRotation.nWidth    = arrChnResolution[i][0];
        stObsTransRotation.nHeight   = arrChnResolution[i][1];
        OnDataChange(i, E_OBS_CHANGE_DATA_TYPE_ROTATION, &stObsTransRotation);
    }

    return AX_TRUE;
}

AX_BOOL CIVPSGrpStage::UpdateGrpRotation(AX_U8 nRotation, AX_BOOL bInFBC, AX_U32 nWidth, AX_U32 nHeight) {
    AX_BOOL bRet = AX_TRUE;
    if (!m_tIvpsGrpCfg.bEnable) {
        for (AX_U8 i = 0; i < m_tIvpsGrpCfg.nGrpChnNum; i++) {
            OBS_TRANS_ROTATE_INFO_T stObsTransRotation;
            stObsTransRotation.nRotation = nRotation;
            stObsTransRotation.bInFBC    = (0 == bInFBC) ? AX_FALSE : AX_TRUE;
            stObsTransRotation.nWidth    = nWidth;
            stObsTransRotation.nHeight   = nHeight;

            LOG_MM_I(IVPS, "[%d] Rotate bypass, fbc: %d, resolution: (%d, %d)",
                            m_tIvpsGrpCfg.nGrp, stObsTransRotation.bInFBC, nWidth, nHeight);

            OnDataChange(i, E_OBS_CHANGE_DATA_TYPE_ROTATION, &stObsTransRotation);
        }
    } else {
        AX_IVPS_PIPELINE_ATTR_T& tPipelineAttr = m_tIvpsGrp.tPipelineAttr;
        AX_U32 nGrpWidth  = nWidth;
        AX_U32 nGrpHeight = nHeight;
        m_bGrpInFBC = bInFBC;
        m_tIvpsGrpCfg.bGrpIOAccordant = IsGrpIOAccordant();

        if (m_tIvpsGrpCfg.bEnable && m_tIvpsGrpCfg.bRotationEngine && m_tIvpsGrpCfg.bRotateBypass) {
            if ((AX_IVPS_ROTATION_E)nRotation != AX_IVPS_ROTATION_0) {
                tPipelineAttr.tFilter[0][0].bEngage = AX_FALSE;
                tPipelineAttr.tFilter[0][1].bEngage = AX_FALSE;
                for (AX_U8 i = 0; i < m_tIvpsGrpCfg.nGrpChnNum; i++) {
                    tPipelineAttr.tFilter[i + 1][0].bEngage = AX_FALSE;
                    tPipelineAttr.tFilter[i + 1][1].bEngage = AX_FALSE;
                }
            } else {
                if (m_tIvpsGrpCfg.arrGrpEngineType[0] != AX_IVPS_ENGINE_BUTT) {
                    tPipelineAttr.tFilter[0][0].bEngage = AX_TRUE;
                }
                if (m_tIvpsGrpCfg.arrGrpEngineType[1] != AX_IVPS_ENGINE_BUTT) {
                    tPipelineAttr.tFilter[0][1].bEngage = AX_TRUE;
                }

                for (AX_U8 i = 0; i < m_tIvpsGrpCfg.nGrpChnNum; i++) {
                    if (m_tIvpsGrpCfg.arrChnEngineType[i][0] != AX_IVPS_ENGINE_BUTT) {
                        tPipelineAttr.tFilter[i + 1][0].bEngage = AX_TRUE;
                    }

                    if (m_tIvpsGrpCfg.arrChnEngineType[i][1] != AX_IVPS_ENGINE_BUTT) {
                        tPipelineAttr.tFilter[i + 1][1].bEngage = AX_TRUE;
                    }
                }
            }
        }

        bRet = UpdateResolution(nRotation, nGrpWidth, nGrpHeight, AX_TRUE);

        if (bRet) {
            m_eRotation = (AX_IVPS_ROTATION_E)nRotation;
        }
    }

    return bRet;
}

AX_VOID CIVPSGrpStage::GetGrpRotateResolution(AX_IVPS_ROTATION_E eRotation, AX_U8 nFilter, AX_BOOL bFBC, AX_U32& nWidth, AX_U32& nHeight) {
    if (m_tIvpsGrpCfg.bGrpIOAccordant) {
        return;
    }

    nWidth  = m_arrGrpResolution[0];
    nHeight = m_arrGrpResolution[1];

    if (AX_IVPS_ROTATION_90 == eRotation || AX_IVPS_ROTATION_270 == eRotation) {
        std::swap(nWidth, nHeight);
    }

    AX_U32 nWidthAlign = GetGrpWidthAlign(bFBC, GetGrpEngine(nFilter));
    nWidth = ALIGN_UP(nWidth, nWidthAlign);
}

AX_VOID CIVPSGrpStage::GetChnRotateResolution(AX_U8 nChn, AX_IVPS_ROTATION_E eRotation, AX_U8 nFilter, AX_BOOL bFBC, AX_U32& nWidth, AX_U32& nHeight, AX_BOOL bDoResize/*= AX_FALSE*/) {
    if (nChn >= m_tIvpsGrpCfg.nGrpChnNum || nChn >= IVPS_MAX_CHANNEL_PER_GROUP) {
        LOG_MM_E(IVPS, "Invalid chn(%d) of ivps grp(%d)", nChn, m_tIvpsGrpCfg.nGrp);
        return;
    }

    AX_BOOL bResized = bDoResize
                       ? AX_TRUE
                         : (m_arrChnResolution[nChn][0] == m_tIvpsGrpCfg.arrChnResolution[nChn][0]
                            && m_arrChnResolution[nChn][1] == m_tIvpsGrpCfg.arrChnResolution[nChn][1]) ? AX_FALSE : AX_TRUE;

    if (m_tIvpsGrpCfg.bArrChnIOAccordant[nChn] && !bResized) {
        return;
    }

    nWidth  = m_arrChnResolution[nChn][0];
    nHeight = m_arrChnResolution[nChn][1];

    if (AX_IVPS_ROTATION_90 == eRotation || AX_IVPS_ROTATION_270 == eRotation) {
        std::swap(nWidth, nHeight);
    }

    AX_U32 nWidthAlign = GetChnWidthAlign(nChn, bFBC, GetChnEngine(nChn, nFilter), bResized);
    nWidth = ALIGN_UP(nWidth, nWidthAlign);
}

AX_BOOL CIVPSGrpStage::UpdateGrpResolution(AX_U32 nWidth, AX_U32 nHeight) {
    return UpdateResolution(m_eRotation, nWidth, nHeight, AX_FALSE);
}

AX_BOOL CIVPSGrpStage::Resize(AX_U8 nChn, AX_S32 nWidth, AX_S32 nHeight) {
    if (!m_tIvpsGrpCfg.bArrChnResizeNode[nChn]) {
        LOG_MM_E(IVPS, "Grp(%d) Chn(%d) is not resize node!", m_tIvpsGrpCfg.nGrp, nChn);
        return AX_FALSE;
    }

    AX_IVPS_PIPELINE_ATTR_T& tPipelineAttr = m_tIvpsGrp.tPipelineAttr;

    m_bChnResizeFilter = GetResizeChnFilter(nChn, m_bGrpInFBC, nWidth, nHeight);

    AX_S32 nPreWidth  = m_arrChnResolution[nChn][0];
    AX_S32 nPreHeight = m_arrChnResolution[nChn][1];

    m_arrChnResolution[nChn][0] = nWidth;
    m_arrChnResolution[nChn][1] = nHeight;

    AX_U32 nChnWidth  = nWidth;
    AX_U32 nChnHeight = nHeight;
    AX_BOOL bResized = AX_FALSE;
    for (AX_U8 i = 0; i < AX_IVPS_MAX_FILTER_NUM_PER_OUTCHN; i++) {
        if (tPipelineAttr.tFilter[nChn + 1][i].bEngage) {
            AX_BOOL bDoResize = (i == m_bChnResizeFilter) ? AX_TRUE : AX_FALSE;

            AX_BOOL bFBC = (0 == m_nChnFBC[nChn][0]) ? AX_FALSE : AX_TRUE;
            AX_U32 nStrideAlign = bFBC ? AX_IVPS_FBC_STRIDE_ALIGN_VAL : AX_IVPS_FBC_WIDTH_ALIGN_VAL;

            if (!bResized || bDoResize) {
                GetChnRotateResolution(nChn, m_eRotation, i, bFBC, nChnWidth, nChnHeight, bDoResize);
            }

            tPipelineAttr.tFilter[nChn + 1][i].nDstPicWidth = nChnWidth;
            tPipelineAttr.tFilter[nChn + 1][i].nDstPicHeight = nChnHeight;
            tPipelineAttr.tFilter[nChn + 1][i].nDstPicStride = ALIGN_UP(nChnWidth, nStrideAlign);

            if (bDoResize) {
                bResized = AX_TRUE;
            }

            LOG_MM_C(IVPS, "Filter%d resize for ivps (Grp: %d, Chn: %d): w: %d, h: %d, s: %d strideAlign:%d, bFBC: %d.",
                i,
                m_tIvpsGrpCfg.nGrp, nChn, tPipelineAttr.tFilter[nChn + 1][i].nDstPicWidth, tPipelineAttr.tFilter[nChn + 1][i].nDstPicHeight,
                tPipelineAttr.tFilter[nChn + 1][i].nDstPicStride, nStrideAlign, bFBC);
        }
    }

    AX_S32 nRet = AX_IVPS_SetPipelineAttr(m_tIvpsGrpCfg.nGrp, &tPipelineAttr);
    if (AX_SUCCESS != nRet) {
        LOG_M_E(IVPS, "AX_IVPS_SetPipelineAttr(Grp %d) failed, ret=0x%x", m_tIvpsGrpCfg.nGrp, nRet);

        m_arrChnResolution[nChn][0] = nPreWidth;
        m_arrChnResolution[nChn][1] = nPreHeight;
        return AX_FALSE;
    }

    OBS_TRANS_RESOLUTION_T stObsTransResolution;
    stObsTransResolution.nWidth  = nChnWidth;
    stObsTransResolution.nHeight = nChnHeight;
    OnDataChange(nChn, E_OBS_CHANGE_DATA_TYPE_RESOLUTION, &stObsTransResolution);

    return AX_TRUE;
}

AX_BOOL CIVPSGrpStage::Rotate(AX_IVPS_ROTATION_E eRotation) {
    if (!SetRotationEngineCfg(eRotation)) {
        return AX_FALSE;
    }

    if (AX_IVPS_ROTATION_0 == eRotation) {
        m_nGrpFBC[0] = m_tIvpsGrpCfg.arrGrpFBC[0];
        m_nGrpFBC[1] = m_tIvpsGrpCfg.arrGrpFBC[1];
        for (AX_U8 i = 0; i < m_tIvpsGrpCfg.nGrpChnNum; i++) {
            m_nChnFBC[i][0] = m_tIvpsGrpCfg.arrChnFBC[i][0];
            m_nChnFBC[i][1] = m_tIvpsGrpCfg.arrChnFBC[i][1];
        }
    } else { // rotate engine disable fbc when do rotate
        m_nGrpFBC[0] = 0;
        m_nGrpFBC[1] = 0;
        for (AX_U8 i = 0; i < m_tIvpsGrpCfg.nGrpChnNum; i++) {
            m_nChnFBC[i][0] = 0;
            m_nChnFBC[i][1] = 0;
        }
    }

    AX_U32 nWidth  = m_arrGrpResolution[0];
    AX_U32 nHeight = m_arrGrpResolution[1];
    if (AX_IVPS_ROTATION_90 == eRotation || AX_IVPS_ROTATION_270 == eRotation) {
        std::swap(nWidth, nHeight);
    }

    m_eRotation = eRotation;

    return UpdateResolution(eRotation, nWidth, nHeight, AX_TRUE);
}

AX_BOOL CIVPSGrpStage::SetMainResolution(AX_S32 nWidth, AX_S32 nHeight) {
    LOG_MM_C(IVPS, "+++ Grp(%d) new resolution:(%d, %d)", m_nIvpsGrp, nWidth, nHeight);

    if (m_tIvpsGrpCfg.arrGrpResolution[0] == m_tIvpsGrpCfg.arrGrpInResolution[0]
        && m_tIvpsGrpCfg.arrGrpResolution[1] == m_tIvpsGrpCfg.arrGrpInResolution[1]) {
        m_arrGrpResolution[0] = nWidth;
        m_arrGrpResolution[1] = nHeight;

        LOG_MM_C(IVPS, "Grp(%d) set grp resolution:(%d, %d)", m_nIvpsGrp, nWidth, nHeight);

        for (AX_U8 i = 0; i < m_tIvpsGrpCfg.nGrpChnNum; i++) {
            if (m_tIvpsGrpCfg.arrChnResolution[i][0] == m_tIvpsGrpCfg.arrGrpResolution[0]
                && m_tIvpsGrpCfg.arrChnResolution[i][1] == m_tIvpsGrpCfg.arrGrpResolution[1]) {
                m_tIvpsGrpCfg.arrChnResolution[i][0] = nWidth;
                m_tIvpsGrpCfg.arrChnResolution[i][1] = nHeight;
                m_arrChnResolution[i][0] = nWidth;
                m_arrChnResolution[i][1] = nHeight;

                LOG_MM_C(IVPS, "Grp(%d) set chn(%d) resolution:(%d, %d)", m_nIvpsGrp, i, nWidth, nHeight);
            }
        }

        m_tIvpsGrpCfg.arrGrpInResolution[0] = nWidth;
        m_tIvpsGrpCfg.arrGrpInResolution[1] = nHeight;
        m_tIvpsGrpCfg.arrGrpResolution[0] = nWidth;
        m_tIvpsGrpCfg.arrGrpResolution[1] = nHeight;

        for (AX_U8 i = 0; i < m_tIvpsGrpCfg.nGrpChnNum; i++) {
            if (m_tIvpsGrpCfg.arrChnResolution[i][0] == m_tIvpsGrpCfg.arrGrpResolution[0]
                && m_tIvpsGrpCfg.arrChnResolution[i][1] == m_tIvpsGrpCfg.arrGrpResolution[1]) {
                OBS_TRANS_RESOLUTION_T stObsTransResolution;
                stObsTransResolution.nWidth  = nWidth;
                stObsTransResolution.nHeight = nHeight;
                OnDataChange(i, E_OBS_CHANGE_DATA_TYPE_SET_MAIN_RESOLUTION, &stObsTransResolution);
            }
        }
    }

    return AX_TRUE;
}

AX_BOOL CIVPSGrpStage::UpdateGrpLDC(AX_BOOL bLdcEnable, AX_BOOL bAspect, AX_S16 nXRatio, AX_S16 nYRatio, AX_S16 nXYRatio,
                                    AX_S16 nDistorRatio) {
    AX_IVPS_PIPELINE_ATTR_T& tPipelineAttr = m_tIvpsGrp.tPipelineAttr;
    if (AX_IVPS_ENGINE_GDC == tPipelineAttr.tFilter[0][0].eEngine) {
        if (bLdcEnable) {
            tPipelineAttr.tFilter[0][0].tGdcCfg.eDewarpType = AX_IVPS_DEWARP_LDC;
            tPipelineAttr.tFilter[0][0].tGdcCfg.tLdcAttr.bAspect = bAspect;
            tPipelineAttr.tFilter[0][0].tGdcCfg.tLdcAttr.nXRatio = nXRatio;
            tPipelineAttr.tFilter[0][0].tGdcCfg.tLdcAttr.nYRatio = nYRatio;
            tPipelineAttr.tFilter[0][0].tGdcCfg.tLdcAttr.nXYRatio = nXYRatio;
            tPipelineAttr.tFilter[0][0].tGdcCfg.tLdcAttr.nDistortionRatio = nDistorRatio;
        } else {
            tPipelineAttr.tFilter[0][0].tGdcCfg.eDewarpType = AX_IVPS_DEWARP_BYPASS;
        }

        AX_S32 nRet = AX_IVPS_SetPipelineAttr(m_tIvpsGrpCfg.nGrp, &tPipelineAttr);
        if (AX_SUCCESS != nRet) {
            LOG_MM_E(IVPS, "AX_IVPS_SetPipelineAttr(Grp %d) failed, ret=0x%x", m_tIvpsGrpCfg.nGrp, nRet);
            return AX_FALSE;
        }

        return AX_TRUE;
    } else {
        return AX_FALSE;
    }
}

AX_VOID CIVPSGrpStage::RefreshOSDByResChange(AX_BOOL bShow /*AX_TRUE*/) {
    if (m_pOsdHelper) {
        m_pOsdHelper->Refresh(bShow);
    }
}

AX_BOOL CIVPSGrpStage::GetResolutionByRotate(AX_U8 nChn, AX_IVPS_ROTATION_E eRotation, AX_U32& nWidth, AX_U32& nHeight) {
    nWidth = m_arrChnResolution[nChn][0];
    nHeight = m_arrChnResolution[nChn][1];

    if (AX_IVPS_ROTATION_90 == eRotation || AX_IVPS_ROTATION_270 == eRotation) {
        std::swap(nWidth, nHeight);
    }

    return AX_TRUE;
}

AX_BOOL CIVPSGrpStage::StartOSD() {
    LOG_MM_W(IVPS, "[%d]StartOSD ...", m_tIvpsGrpCfg.nGrp);
    if (COptionHelper::GetInstance()->IsEnableOSD() && m_pOsdHelper && !m_pOsdHelper->StartOSD(this)) {
        LOG_MM_E(IVPS, "Start OSD failed.");
        return AX_FALSE;
    }

    return AX_TRUE;
}

AX_BOOL CIVPSGrpStage::StopOSD() {
    LOG_MM_W(IVPS, "[%d]StopOSD ...", m_tIvpsGrpCfg.nGrp);

    if (COptionHelper::GetInstance()->IsEnableOSD() && m_pOsdHelper && !m_pOsdHelper->StopOSD()) {
        LOG_M_E(IVPS, "Stop OSD failed.");
        return AX_FALSE;
    }

    return AX_TRUE;
}

AX_BOOL CIVPSGrpStage::ChangeMirrorFlip(AX_BOOL bMirror, AX_BOOL bFlip) {
    if (!SetMirrorFlipEngineCfg(bMirror, bFlip)) {
        return AX_FALSE;
    }

    AX_IVPS_PIPELINE_ATTR_T& tPipelineAttr = m_tIvpsGrp.tPipelineAttr;
    AX_S32 nRet = AX_IVPS_SetPipelineAttr(m_tIvpsGrpCfg.nGrp, &tPipelineAttr);
    if (AX_SUCCESS != nRet) {
        LOG_MM_E(IVPS, "AX_IVPS_SetPipelineAttr(Grp %d) failed, ret=0x%x", m_tIvpsGrpCfg.nGrp, nRet);
        return AX_FALSE;
    }

    return AX_TRUE;
}

AX_U32 CIVPSGrpStage::GetGrpWidthAlign(AX_BOOL bFBC, AX_IVPS_ENGINE_E eEngine) {
    AX_U32 nWidthAlign = 2;

    do {
        // no fbc or out resolution is equal to in, width align to 2
        if (!bFBC || m_tIvpsGrpCfg.bGrpIOAccordant) {
            return nWidthAlign;
        }

        // fbc and out resolution is not equal to in
        // Engine is VPP or VGP, width align to AX_IVPS_FBC_WIDTH_ALIGN_VAL
        switch(eEngine) {
            case AX_IVPS_ENGINE_VPP:
            case AX_IVPS_ENGINE_VGP:
                nWidthAlign = AX_IVPS_FBC_WIDTH_ALIGN_VAL;
                break;
            default:
                break;
        }
    } while(0);

    LOG_MM_I(IVPS, "nWidthAlign: %d for Grp(%d) with engine(%d) fbc(%d)",
                   nWidthAlign, m_tIvpsGrpCfg.nGrp, eEngine, bFBC);

    return nWidthAlign;
}

AX_U32 CIVPSGrpStage::GetChnWidthAlign(AX_U8 nChn, AX_BOOL bFBC, AX_IVPS_ENGINE_E eEngine, AX_BOOL bResized) {
    if (nChn >= m_tIvpsGrpCfg.nGrpChnNum || nChn >= IVPS_MAX_CHANNEL_PER_GROUP) {
        LOG_MM_E(IVPS, "Invalid chn(%d) for grp(%d)", nChn, m_tIvpsGrpCfg.nGrp);
        return AX_IVPS_ENGINE_BUTT;
    }

    AX_U32 nWidthAlign = 2;

    do {
        // no fbc or out resolution is equal to in, width align to 2
        if (!bFBC || (m_tIvpsGrpCfg.bArrChnIOAccordant[nChn] && !bResized)) {
            break;
        }

        // fbc and out resolution is not equal to in
        // Engine is VPP or VGP, width align to AX_IVPS_FBC_WIDTH_ALIGN_VAL
        switch(eEngine) {
            case AX_IVPS_ENGINE_VPP:
            case AX_IVPS_ENGINE_VGP:
                nWidthAlign = AX_IVPS_FBC_WIDTH_ALIGN_VAL;
                break;
            default:
                break;
        }
    } while(0);

    LOG_MM_I(IVPS, "nWidthAlign: %d for Grp(%d)Chn(%d) with engine(%d) fbc(%d)",
                   nWidthAlign, m_tIvpsGrpCfg.nGrp, nChn, eEngine, bFBC);

    return nWidthAlign;
}

// Suppose that only one filter(0 or 1) set for Grp
AX_IVPS_ENGINE_E CIVPSGrpStage::GetGrpEngine(AX_U8 nFilter) {
    if (nFilter >= AX_IVPS_MAX_FILTER_NUM_PER_OUTCHN) {
        LOG_M_E(IVPS, "Invalid nFilter(%d) of ivps grp(%d)", nFilter, m_tIvpsGrpCfg.nGrp);
        return AX_IVPS_ENGINE_BUTT;
    }

    AX_IVPS_ENGINE_E eEngine = m_tIvpsGrpCfg.arrGrpEngineType[nFilter];
    if (1 == nFilter && m_tIvpsGrpCfg.arrGrpEngineType[1] == AX_IVPS_ENGINE_SUBSIDIARY) {
        eEngine =  m_tIvpsGrpCfg.arrGrpEngineType[0];
    }

    LOG_MM_I(IVPS, "Grp(%d)Filter(%d) engine(%d)", m_tIvpsGrpCfg.nGrp, nFilter, eEngine);

    return eEngine;
}

// Suppose that only one filter(0 or 1) set for Chn or Grp
AX_IVPS_ENGINE_E CIVPSGrpStage::GetChnEngine(AX_U8 nChn, AX_U8 nFilter) {
    if (nChn >= m_tIvpsGrpCfg.nGrpChnNum || nChn >= IVPS_MAX_CHANNEL_PER_GROUP) {
        LOG_MM_E(IVPS, "Invalid chn(%d) of ivps grp(%d)", nChn, m_tIvpsGrpCfg.nGrp);
        return AX_IVPS_ENGINE_BUTT;
    }

    if (nFilter >= AX_IVPS_MAX_FILTER_NUM_PER_OUTCHN) {
        LOG_MM_E(IVPS, "Invalid nFilter(%d) of ivps grp(%d) chn(%d)",
                       nFilter, m_tIvpsGrpCfg.nGrp, nChn);
        return AX_IVPS_ENGINE_BUTT;
    }

    AX_IVPS_ENGINE_E eEngine = m_tIvpsGrp.tPipelineAttr.tFilter[nChn + 1][nFilter].eEngine;//m_tIvpsGrpCfg.arrChnEngineType[nChn][nFilter];

    if (AX_IVPS_ENGINE_SUBSIDIARY == eEngine) {
        if (m_tIvpsGrpCfg.arrGrpEngineType[1] != AX_IVPS_ENGINE_BUTT
            && m_tIvpsGrpCfg.arrGrpEngineType[1] != AX_IVPS_ENGINE_SUBSIDIARY) {
            eEngine =  m_tIvpsGrpCfg.arrGrpEngineType[1];
        } else {
            eEngine = m_tIvpsGrpCfg.arrGrpEngineType[0];
        }
    }

    LOG_MM_I(IVPS, "Grp(%d)Chn(%d)Filter(%d) engine(%d)", m_tIvpsGrpCfg.nGrp, nChn, nFilter, eEngine);

    return eEngine;
}

AX_BOOL CIVPSGrpStage::IsGrpIOAccordant() {
    AX_BOOL bGrpIOAccordant = AX_FALSE;
    AX_BOOL bGrpFBC = (0 == m_nGrpFBC[0]) ? AX_FALSE : AX_TRUE;

    if (m_tIvpsGrpCfg.arrGrpResolution[0] == m_tIvpsGrpCfg.arrGrpInResolution[0]
        && m_tIvpsGrpCfg.arrGrpResolution[1] == m_tIvpsGrpCfg.arrGrpInResolution[1]
        && !(!m_bGrpInFBC && bGrpFBC)) {
        bGrpIOAccordant = AX_TRUE;
    }

    LOG_MM_I(IVPS, "grp(%d) bGrpFBC: %d, m_bGrpInFBC: %d, grp(%d, %d)-grpIn(%d, %d), bGrpIOAccordant: %d",
                    m_tIvpsGrpCfg.nGrp, bGrpFBC, m_bGrpInFBC,
                    m_tIvpsGrpCfg.arrGrpResolution[0], m_tIvpsGrpCfg.arrGrpResolution[1],
                    m_tIvpsGrpCfg.arrGrpInResolution[0], m_tIvpsGrpCfg.arrGrpInResolution[1], bGrpIOAccordant);

    return bGrpIOAccordant;
}

AX_BOOL CIVPSGrpStage::IsChnIOAccordant(AX_U8 nChn) {
    AX_BOOL bChnIOAccordant = AX_FALSE;
    AX_BOOL bChnFBC = (0 == m_nChnFBC[nChn][0]) ? AX_FALSE : AX_TRUE;
    AX_BOOL bGrpFBC = (0 == m_nGrpFBC[0]) ? AX_FALSE : AX_TRUE;

    if (m_tIvpsGrpCfg.arrChnResolution[nChn][0] == m_tIvpsGrpCfg.arrGrpResolution[0]
        && m_tIvpsGrpCfg.arrChnResolution[nChn][1] == m_tIvpsGrpCfg.arrGrpResolution[1]
        && !(!bGrpFBC && bChnFBC)) {
        bChnIOAccordant = AX_TRUE;
    }

    LOG_MM_I(IVPS, "grp(%d) chn(%d) bGrpFBC: %d, bChnFBC: %d, chn(%d, %d)-grp(%d, %d), bChnIOAccordant: %d",
                    m_tIvpsGrpCfg.nGrp, nChn, bGrpFBC, bChnFBC,
                    m_tIvpsGrpCfg.arrChnResolution[nChn][0], m_tIvpsGrpCfg.arrChnResolution[nChn][1],
                    m_tIvpsGrpCfg.arrGrpResolution[0], m_tIvpsGrpCfg.arrGrpResolution[1], bChnIOAccordant);

    return bChnIOAccordant;
}

AX_BOOL CIVPSGrpStage::SetRotationEngineCfg(AX_IVPS_ROTATION_E eRotation) {
    if (!m_tIvpsGrpCfg.bEnable || !m_tIvpsGrpCfg.bRotationEngine) {
        LOG_MM_E(IVPS, "[%d] is not enable or not rotate engine, pls check ivps.json", m_tIvpsGrpCfg.nGrp);
        return AX_FALSE;
    }

    AX_IVPS_PIPELINE_ATTR_T& tPipelineAttr = m_tIvpsGrp.tPipelineAttr;
    AX_U8 nGrpFilterIndex = IVPS_INVALD_FILTER_INDEX;
    AX_BOOL bRet = AX_TRUE;

    for (AX_U8 i = 0; i < AX_IVPS_MAX_FILTER_NUM_PER_OUTCHN; i++) {
        if (tPipelineAttr.tFilter[0][i].eEngine != AX_IVPS_ENGINE_BUTT) {
            nGrpFilterIndex = i;
            break;
        }
    }

    if (IVPS_INVALD_FILTER_INDEX == nGrpFilterIndex) {
        LOG_MM_E(IVPS, "[%d] grp filter without any engine, pls check ivps.json", m_tIvpsGrpCfg.nGrp);
        return AX_FALSE;
    }

    switch (tPipelineAttr.tFilter[0][nGrpFilterIndex].eEngine) {
        case AX_IVPS_ENGINE_DSP:
            m_tIvpsGrp.tPipelineAttr.tFilter[0][nGrpFilterIndex].tGdcCfg.bHwRotation = AX_TRUE;
        case AX_IVPS_ENGINE_GDC:
            if ((m_tIvpsGrpCfg.bMirrorFlipEngine
                  && (tPipelineAttr.tFilter[0][nGrpFilterIndex].tGdcCfg.bFlip
                      || tPipelineAttr.tFilter[0][nGrpFilterIndex].tGdcCfg.bMirror))
                || (AX_IVPS_ROTATION_0 != eRotation)) {
                tPipelineAttr.tFilter[0][nGrpFilterIndex].bEngage = AX_TRUE;
            } else {
                tPipelineAttr.tFilter[0][nGrpFilterIndex].bEngage = AX_FALSE;
            }
            tPipelineAttr.tFilter[0][nGrpFilterIndex].tGdcCfg.eRotation = eRotation;
            break;
        case AX_IVPS_ENGINE_TDP:
            if ((m_tIvpsGrpCfg.bMirrorFlipEngine
                  && (tPipelineAttr.tFilter[0][nGrpFilterIndex].tTdpCfg.bFlip
                       || tPipelineAttr.tFilter[0][nGrpFilterIndex].tTdpCfg.bMirror))
                || (AX_IVPS_ROTATION_0 != eRotation)) {
                tPipelineAttr.tFilter[0][nGrpFilterIndex].bEngage = AX_TRUE;
            } else {
                tPipelineAttr.tFilter[0][nGrpFilterIndex].bEngage = AX_FALSE;
            }
            tPipelineAttr.tFilter[0][nGrpFilterIndex].tTdpCfg.eRotation = eRotation;
            break;
        default:
            LOG_MM_E(IVPS, "Grp[%d] invalid engine(%d)", m_tIvpsGrpCfg.nGrp, tPipelineAttr.tFilter[0][nGrpFilterIndex].eEngine);
            bRet = AX_FALSE;
            break;
    }

    LOG_MM_C(IVPS, "Grp[%d] filter[%d] set bEngage %d for rotation engine(%d) with rotation(%d).",
                    m_tIvpsGrpCfg.nGrp, nGrpFilterIndex,
                    tPipelineAttr.tFilter[0][nGrpFilterIndex].bEngage,
                    tPipelineAttr.tFilter[0][nGrpFilterIndex].eEngine,
                    eRotation);
    return bRet;
}

AX_BOOL CIVPSGrpStage::SetMirrorFlipEngineCfg(AX_BOOL bMirror, AX_BOOL bFlip) {
    if (!m_tIvpsGrpCfg.bEnable || !m_tIvpsGrpCfg.bMirrorFlipEngine) {
        LOG_MM_W(IVPS, "Grp(%d) is disabled or not mirror-flip engine, please check ppl.", m_tIvpsGrpCfg.nGrp);
        return AX_FALSE;
    }

    AX_IVPS_PIPELINE_ATTR_T& tPipelineAttr = m_tIvpsGrp.tPipelineAttr;
    AX_U8 nGrpFilterIndex = IVPS_INVALD_FILTER_INDEX;
    AX_BOOL bRet = AX_TRUE;

    for (AX_U8 i = 0; i < AX_IVPS_MAX_FILTER_NUM_PER_OUTCHN; i++) {
        if (tPipelineAttr.tFilter[0][i].eEngine != AX_IVPS_ENGINE_BUTT) {
            nGrpFilterIndex = i;
            break;
        }
    }

    if (IVPS_INVALD_FILTER_INDEX == nGrpFilterIndex) {
        LOG_MM_E(IVPS, "[%d] grp filter without any engine, pls check ivps.json", m_tIvpsGrpCfg.nGrp);
        return AX_FALSE;
    }

    switch (tPipelineAttr.tFilter[0][nGrpFilterIndex].eEngine) {
        case AX_IVPS_ENGINE_DSP:
        case AX_IVPS_ENGINE_GDC:
            if ((!m_tIvpsGrpCfg.bRotationEngine || (AX_IVPS_ROTATION_0 == (AX_IVPS_ROTATION_E)tPipelineAttr.tFilter[0][nGrpFilterIndex].tGdcCfg.eRotation))
                && !bMirror && !bFlip) {
                tPipelineAttr.tFilter[0][nGrpFilterIndex].bEngage = AX_FALSE;
            } else {
                tPipelineAttr.tFilter[0][nGrpFilterIndex].bEngage = AX_TRUE;
            }
            tPipelineAttr.tFilter[0][nGrpFilterIndex].tGdcCfg.bMirror = bMirror;
            tPipelineAttr.tFilter[0][nGrpFilterIndex].tGdcCfg.bFlip   = bFlip;
            break;
        case AX_IVPS_ENGINE_TDP:
            if ((!m_tIvpsGrpCfg.bRotationEngine || (AX_IVPS_ROTATION_0 == (AX_IVPS_ROTATION_E)tPipelineAttr.tFilter[0][nGrpFilterIndex].tTdpCfg.eRotation))
                && !bMirror && !bFlip) {
                tPipelineAttr.tFilter[0][nGrpFilterIndex].bEngage = AX_FALSE;
            } else {
                tPipelineAttr.tFilter[0][nGrpFilterIndex].bEngage = AX_TRUE;
            }
            tPipelineAttr.tFilter[0][nGrpFilterIndex].tTdpCfg.bMirror = bMirror;
            tPipelineAttr.tFilter[0][nGrpFilterIndex].tTdpCfg.bFlip   = bFlip;
            break;
        default:
            LOG_MM_E(IVPS, "Grp[%d] invalid engine(%d)", m_tIvpsGrpCfg.nGrp, tPipelineAttr.tFilter[0][nGrpFilterIndex].eEngine);
            bRet = AX_FALSE;
            break;
    }

    LOG_MM_C(IVPS, "Grp(%d) engine(%d) mirror(%d), flip(%d), bEngage(%d)",
                    m_tIvpsGrpCfg.nGrp,
                    tPipelineAttr.tFilter[0][nGrpFilterIndex].eEngine,
                    bMirror,
                    bFlip,
                    tPipelineAttr.tFilter[0][nGrpFilterIndex].bEngage);
    return bRet;
}

AX_U8 CIVPSGrpStage::GetResizeChnFilter(AX_U8 nChn, AX_BOOL bInFBC, AX_S32 nNewWidth, AX_S32 nNewHeight) {
    AX_U8 nFilterIndex = IVPS_INVALD_FILTER_INDEX;

    for (AX_S8 i = AX_IVPS_MAX_FILTER_NUM_PER_OUTCHN - 1; i >= 0 ; i--) {
        if (AX_IVPS_ENGINE_TDP == m_eOrgResizeEngine && bInFBC && m_tIvpsGrpCfg.arrChnEngineType[nChn][i] == AX_IVPS_ENGINE_BUTT) {
            m_tIvpsGrp.tPipelineAttr.tFilter[nChn + 1][i].eEngine = AX_IVPS_ENGINE_VGP;
            m_tIvpsGrp.tPipelineAttr.tFilter[nChn + 1][i].eDstPicFormat = AX_FORMAT_YUV420_SEMIPLANAR;
            m_tIvpsGrp.tPipelineAttr.tFilter[nChn + 1][i].tFRC.fSrcFrameRate = (m_tIvpsGrpCfg.arrChnFramerate[nChn][0]);
            m_tIvpsGrp.tPipelineAttr.tFilter[nChn + 1][i].tFRC.fDstFrameRate = (m_tIvpsGrpCfg.arrChnFramerate[nChn][1]);
            m_tIvpsGrp.tPipelineAttr.tFilter[nChn + 1][i].tCompressInfo.enCompressMode = (AX_COMPRESS_MODE_E)m_nChnFBC[nChn][0];
            m_tIvpsGrp.tPipelineAttr.tFilter[nChn + 1][i].tCompressInfo.u32CompressLevel = m_nChnFBC[nChn][1];
        }

        if (AX_IVPS_ENGINE_BUTT == m_tIvpsGrpCfg.arrChnEngineType[nChn][i]
            && AX_IVPS_ENGINE_VGP == m_tIvpsGrp.tPipelineAttr.tFilter[nChn + 1][i].eEngine) {
            if (nNewWidth == m_tIvpsGrpCfg.arrChnResolution[nChn][0] && nNewHeight == m_tIvpsGrpCfg.arrChnResolution[nChn][1]) {
                m_tIvpsGrp.tPipelineAttr.tFilter[nChn + 1][i].bEngage = AX_FALSE;
                nFilterIndex = (i + 1) % AX_IVPS_MAX_FILTER_NUM_PER_OUTCHN;
            } else if (AX_IVPS_ENGINE_TDP == m_eOrgResizeEngine) {
                m_tIvpsGrp.tPipelineAttr.tFilter[nChn + 1][i].bEngage = AX_TRUE;
                nFilterIndex = i;
            }
            break;
        }
    }

    if (IVPS_INVALD_FILTER_INDEX != nFilterIndex) {
        return nFilterIndex;
    }

    nFilterIndex = 1;
    for (AX_S8 i = AX_IVPS_MAX_FILTER_NUM_PER_OUTCHN - 1; i >= 0 ; i--) {
        if (m_tIvpsGrpCfg.arrChnEngineType[nChn][i] != AX_IVPS_ENGINE_BUTT) {
            nFilterIndex = i;
        }
    }

    return nFilterIndex;
}

AX_VOID CIVPSGrpStage::InitGrpFbc() {
    AX_BOOL bGrpSet = AX_FALSE;
    for (AX_U8 i = 0; i < AX_IVPS_MAX_FILTER_NUM_PER_OUTCHN; i++) {
        if (m_tIvpsGrpCfg.arrGrpEngineType[i] != AX_IVPS_ENGINE_BUTT) {
            bGrpSet = AX_TRUE;
            break;
        }
    }

    if (bGrpSet) {
        m_nGrpFBC[0] = m_tIvpsGrpCfg.arrGrpFBC[0];
        m_nGrpFBC[1] = m_tIvpsGrpCfg.arrGrpFBC[1];
    } else {
        m_tIvpsGrpCfg.arrGrpFBC[0] = m_tIvpsGrpCfg.arrInGrpFBC[0];
        m_tIvpsGrpCfg.arrGrpFBC[1] = m_tIvpsGrpCfg.arrInGrpFBC[1];
        m_nGrpFBC[0] = m_tIvpsGrpCfg.arrInGrpFBC[0];
        m_nGrpFBC[1] = m_tIvpsGrpCfg.arrInGrpFBC[1];
    }

    m_bGrpInFBC = (0 == m_tIvpsGrpCfg.arrInGrpFBC[0]) ? AX_FALSE : AX_TRUE;

    LOG_MM_I(IVPS, "Grp[%d] fbc(%d, %d), m_bGrpInFBC: %d", m_tIvpsGrpCfg.nGrp, m_nGrpFBC[0], m_nGrpFBC[1], m_bGrpInFBC);
}

AX_VOID CIVPSGrpStage::InitChnFbc(AX_U8 nChn) {
    AX_BOOL bChnSet = AX_FALSE;
    for (AX_U8 i = 0; i < AX_IVPS_MAX_FILTER_NUM_PER_OUTCHN; i++) {
        if (m_tIvpsGrpCfg.arrChnEngineType[nChn][i] != AX_IVPS_ENGINE_BUTT) {
            bChnSet = AX_TRUE;
            break;
        }
    }

    if (bChnSet) {
        m_nChnFBC[nChn][0] = m_tIvpsGrpCfg.arrChnFBC[nChn][0];
        m_nChnFBC[nChn][1] = m_tIvpsGrpCfg.arrChnFBC[nChn][1];
    } else {
        m_tIvpsGrpCfg.arrChnFBC[nChn][0] = m_tIvpsGrpCfg.arrGrpFBC[0];
        m_tIvpsGrpCfg.arrChnFBC[nChn][1] = m_tIvpsGrpCfg.arrGrpFBC[1];
        m_nChnFBC[nChn][0] = m_tIvpsGrpCfg.arrGrpFBC[0];
        m_nChnFBC[nChn][1] = m_tIvpsGrpCfg.arrGrpFBC[1];
    }

    LOG_MM_I(IVPS, "Grp[%d]Chn(%d) fbc(%d, %d)", m_tIvpsGrpCfg.nGrp, nChn, m_nChnFBC[nChn][0], m_nChnFBC[nChn][1]);
}

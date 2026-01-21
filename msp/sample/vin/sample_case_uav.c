/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <sys/prctl.h>


#include "ax_isp_api.h"
#include "sample_vin.h"
#include "common_sys.h"
#include "common_vin.h"
#include "common_cam.h"
#include "common_isp.h"
#include "ax_vin_error_code.h"
#include "common_nt.h"
#include "ax_buffer_tool.h"
#include "sample_case_dcg_vs.h"

COMMON_SYS_POOL_CFG_T gtSysCommPoolSingleOv48c40Sdr[] = {
    {8000, 6000, 8000, AX_FORMAT_YUV420_SEMIPLANAR, 20},   /* vin nv21/nv21 use */
};

COMMON_SYS_POOL_CFG_T gtPrivatePoolSingleOv48c40Sdr[] = {
    {8000, 6000, 8000, AX_FORMAT_BAYER_RAW_16BPP, 15},      /* vin raw16 use */
};

static AX_CAMERA_T gCams[MAX_CAMERAS] = {0};
static volatile AX_S32 gLoopExit = 0;
static pthread_t gTriggerThread[MAX_CAMERAS] = {0};
static pthread_t gGetExtRawThread[MAX_CAMERAS] = {0};
static AX_S32 g_trigger_loop_exit[MAX_CAMERAS] = {0};
static AX_S32 g_get_ext_raw_loop_exit[MAX_CAMERAS] = {0};
static AX_ISP_AF_REGFUNCS_T s_tAfFuncs = {0};

static void *TriggerThread(void *args);
static void *GetExtRawFrameThread(void *args);

static AX_VOID __sigint(int iSigNo)
{
    COMM_ISP_PRT("Catch signal %d\n", iSigNo);
    gLoopExit = 1;

    return ;
}

static AX_VOID __cal_dump_pool(COMMON_SYS_POOL_CFG_T pool[], AX_SNS_HDR_MODE_E eHdrMode, AX_S32 nFrameNum)
{
    if (NULL == pool) {
        return;
    }
    if (nFrameNum > 0) {
        switch (eHdrMode) {
        case AX_SNS_LINEAR_MODE:
            pool[0].nBlkCnt += nFrameNum;
            break;

        case AX_SNS_HDR_2X_MODE:
            pool[0].nBlkCnt += nFrameNum * 2;
            break;

        case AX_SNS_HDR_3X_MODE:
            pool[0].nBlkCnt += nFrameNum * 3;
            break;

        case AX_SNS_HDR_4X_MODE:
            pool[0].nBlkCnt += nFrameNum * 4;
            break;

        default:
            pool[0].nBlkCnt += nFrameNum;
            break;
        }
    }
}

static AX_VOID __set_pipe_hdr_mode(AX_U32 *pHdrSel, AX_SNS_HDR_MODE_E eHdrMode)
{
    if (NULL == pHdrSel) {
        return;
    }

    switch (eHdrMode) {
    case AX_SNS_LINEAR_MODE:
        *pHdrSel = 0x1;
        break;

    case AX_SNS_HDR_2X_MODE:
        *pHdrSel = 0x1 | 0x2;
        break;

    case AX_SNS_HDR_3X_MODE:
        *pHdrSel = 0x1 | 0x2 | 0x4;
        break;

    case AX_SNS_HDR_4X_MODE:
        *pHdrSel = 0x1 | 0x2 | 0x4 | 0x8;
        break;

    default:
        *pHdrSel = 0x1;
        break;
    }
}

static AX_VOID __set_vin_attr(AX_CAMERA_T *pCam, SAMPLE_SNS_TYPE_E eSnsType, AX_SNS_HDR_MODE_E eHdrMode,
                              COMMON_VIN_MODE_E eSysMode, AX_BOOL bAiispEnable)
{
    pCam->eSnsType = eSnsType;
    pCam->tDevAttr.eSnsMode = eHdrMode;
    pCam->eHdrMode = eHdrMode;
    pCam->eSysMode = eSysMode;
    pCam->tPipeAttr[pCam->nPipeId].eSnsMode = eHdrMode;
    pCam->tPipeAttr[pCam->nPipeId].bAiIspEnable = bAiispEnable;

    if ((AX_SNS_DCG_VS_HDR == pCam->tDevAttr.eSnsOutputMode) || (AX_SNS_DCG_HDR == pCam->tDevAttr.eSnsOutputMode)) {
    } else {
        pCam->tSnsAttr.eSnsMode = eHdrMode;
        if (eHdrMode > AX_SNS_LINEAR_MODE) {
            pCam->tDevAttr.eSnsOutputMode = AX_SNS_DOL_HDR;
        } else {
            pCam->tSnsAttr.eRawType = AX_RT_RAW12;
            pCam->tDevAttr.ePixelFmt = AX_FORMAT_BAYER_RAW_12BPP;
            pCam->tPipeAttr[pCam->nPipeId].ePixelFmt = AX_FORMAT_BAYER_RAW_12BPP;
        }
    }

    if (COMMON_VIN_TPG == eSysMode) {
        pCam->tDevAttr.eSnsIntfType = AX_SNS_INTF_TYPE_TPG;
    }

    pCam->bEnableDev = AX_TRUE;
    pCam->bRegisterSns = AX_TRUE;

    return;
}

static AX_U32 __sample_case_single_48c_video_mode(AX_CAMERA_T *pCamList, SAMPLE_SNS_TYPE_E eSnsType,
        SAMPLE_VIN_PARAM_T *pVinParam, COMMON_SYS_ARGS_T *pCommonArgs)
{
    AX_S32 i = 0, j = 0;
    AX_CAMERA_T *pCam = NULL;
    COMMON_VIN_MODE_E eSysMode = pVinParam->eSysMode;
    AX_SNS_HDR_MODE_E eHdrMode = pVinParam->eHdrMode;
    AX_U32 nScaleRatio = 0;
    AX_VIN_DOWN_SCALE_RATIO_E eScaleRatio[2] = {AX_VIN_DOWN_SCALE_RATIO_2X, AX_VIN_DOWN_SCALE_RATIO_2X};
    AX_BOOL bEisEn[2] = {0};
    AX_BOOL bMotionEst[2] = {0};
    AX_BOOL bExtRawDumpEn[2] = {0};
    AX_VIN_DEV_TRIGGER_MODE_E  eDevTriggerMode = AX_VIN_DEV_TRIGGER_CONTINUE;
    SAMPLE_PIPE_INFO_T tSamplePipeInfo[] = {
        {AX_VIN_PIPE_NORMAL_MODE0, SAMPLE_PIPE_MODE_VIDEO, AX_FALSE, "null.bin"},
        {AX_VIN_PIPE_NORMAL_MODE0, SAMPLE_PIPE_MODE_VIDEO, AX_FALSE, "null.bin"},
    };

    pCommonArgs->nCamCnt = 1;

    switch (pVinParam->eSysCase) {
    case SAMPLE_VIN_UAV_OV48C40_VIDEO_MODE :
        eScaleRatio[0] = AX_VIN_DOWN_SCALE_RATIO_2X;
        eScaleRatio[1] = AX_VIN_DOWN_SCALE_RATIO_2X;
        bMotionEst[0] = AX_TRUE;
        bMotionEst[1] = AX_TRUE;
        bEisEn[0] = AX_TRUE;
        bEisEn[1] = AX_TRUE;
        bExtRawDumpEn[0] = AX_FALSE;
        bExtRawDumpEn[1] = AX_TRUE;
        eDevTriggerMode = AX_VIN_DEV_TRIGGER_CONTINUE;
        break;
    default:
        eScaleRatio[0] = AX_VIN_DOWN_SCALE_RATIO_2X;
        eScaleRatio[1] = AX_VIN_DOWN_SCALE_RATIO_2X;
        bMotionEst[0] = AX_FALSE;
        bMotionEst[1] = AX_TRUE;
        break;
    }

    for (i = 0; i < pCommonArgs->nCamCnt; i++) {
        pCam = &pCamList[i];
        pCam->nPipeId = i;
        __set_vin_attr(pCam, eSnsType, eHdrMode, eSysMode, pVinParam->bAiispEnable);
        COMMON_VIN_GetSnsConfig(eSnsType, &pCam->tMipiRx, &pCam->tSnsAttr,
                                &pCam->tSnsClkAttr, &pCam->tDevAttr);

        /*video 12M*/
        pCam->tSnsAttr.nWidth = 4000;
        pCam->tSnsAttr.nHeight = 3000;
        pCam->tSnsAttr.fFrameRate = 30;
        pCam->tDevAttr.tDevImgRgn[0].nWidth = 4000;
        pCam->tDevAttr.tDevImgRgn[0].nHeight = 3000;
        pCam->tDevAttr.nWidthStride[0] = 4000;

        pCam->eBusType = COMMON_ISP_GetSnsBusType(eSnsType);
        __set_pipe_hdr_mode(&pCam->tDevBindPipe.nHDRSel[0], eHdrMode);
        __set_pipe_hdr_mode(&pCam->tDevBindPipe.nHDRSel[1], eHdrMode);

        pCam->tDevBindPipe.nNum =  2;
        pCam->tDevBindPipe.nPipeId[0] = 0;
        pCam->tDevBindPipe.nPipeId[1] = 4;
        pCam->nDevId = 2;
        pCam->nRxDev = 2;
        pCam->tSnsClkAttr.nSnsClkIdx = 1;
        pCam->tSnsClkAttr.eSnsClkRate = AX_SNS_CLK_24M;
        pCam->tDevAttr.eDevTriggerMode = eDevTriggerMode;
        pCam->tMipiRx.tMipiAttr.nDataRate = 2495;
        pCam->tMipiRx.tMipiAttr.eLaneNum = 4;
        pCam->tDevAttr.eSnsIntfType = AX_SNS_INTF_TYPE_MIPI_RAW;
        pCam->tDevAttr.ePixelFmt = AX_FORMAT_BAYER_RAW_10BPP;
        pCam->tDevAttr.eDevMode = AX_VIN_DEV_OFFLINE;
        pCam->tDevAttr.tCompressInfo.enCompressMode = AX_COMPRESS_MODE_LOSSLESS;
        pCam->tDevAttr.nWidthStride[0] = 4096;

        for (j = 0; j < pCam->tDevBindPipe.nNum; j++) {
            pCam->nPipeId = pCam->tDevBindPipe.nPipeId[j];
            pCam->ptSnsHdl[pCam->nPipeId] = COMMON_ISP_GetSnsObj(eSnsType);
            COMMON_VIN_GetPipeConfig(eSnsType,
                                     &pCam->tPipeAttr[pCam->nPipeId], pCam->tChnAttr[pCam->nPipeId], pCam->bChnEnable[pCam->nPipeId]);
            /*video 12M*/
            pCam->tPipeAttr[pCam->nPipeId].tPipeImgRgn.nWidth = 4000;
            pCam->tPipeAttr[pCam->nPipeId].tPipeImgRgn.nHeight = 3000;
            pCam->tPipeAttr[pCam->nPipeId].nWidthStride = 4000;
            pCam->tChnAttr[pCam->nPipeId][0].nWidth = 4000;
            pCam->tChnAttr[pCam->nPipeId][0].nHeight = 3000;
            pCam->tChnAttr[pCam->nPipeId][0].nWidthStride = 4000;

            if (pCam->nPipeId == 0) {
                pCam->tPipeAttr[pCam->nPipeId].tCompressInfo.enCompressMode =  AX_COMPRESS_MODE_LOSSLESS;
                pCam->tPipeAttr[pCam->nPipeId].nWidthStride = 4096;
            }

            pCam->tPipeAttr[pCam->nPipeId].tMotionAttr.bMotionEst =  bMotionEst[j];
            pCam->bEisEnable[pCam->nPipeId] =  bEisEn[j];

            if (bExtRawDumpEn[j]) {
                nScaleRatio = 2 << eScaleRatio[j];
                pCam->tPipeAttr[pCam->nPipeId].tPipeImgRgn.nStartX /= nScaleRatio;
                pCam->tPipeAttr[pCam->nPipeId].tPipeImgRgn.nStartY /= nScaleRatio;
                pCam->tPipeAttr[pCam->nPipeId].tPipeImgRgn.nHeight /= nScaleRatio;
                pCam->tPipeAttr[pCam->nPipeId].tPipeImgRgn.nWidth /= nScaleRatio;
                pCam->tPipeAttr[pCam->nPipeId].nWidthStride /= nScaleRatio;
                pCam->tPipeAttr[pCam->nPipeId].tCompressInfo.enCompressMode = AX_COMPRESS_MODE_NONE;

                pCam->tChnAttr[pCam->nPipeId][0].nWidth /= nScaleRatio;
                pCam->tChnAttr[pCam->nPipeId][0].nHeight /= nScaleRatio;
                pCam->tChnAttr[pCam->nPipeId][0].nWidthStride /= nScaleRatio;
            }

            // pCam->tIspDependAttr[0].nDependPipeId = 4;
            // pCam->tIspDependAttr[0].eDependMode = AX_ISP_DEPEND_PIPE_MODE0;

            if (bEisEn[j]) {
                pCam->tChnAttr[pCam->nPipeId][0].nWidthStride = (pCam->tChnAttr[pCam->nPipeId][0].nWidthStride + 127) / 128 * 128;
            }
            if (bExtRawDumpEn[j]) {
                pCam->tExtRawDumpAttr[pCam->nPipeId].bEnable = AX_TRUE;
                pCam->tExtRawDumpAttr[pCam->nPipeId].nDepth = 2;
                pCam->tExtRawDumpAttr[pCam->nPipeId].eScaleRatio = eScaleRatio[j];
                pCam->tPipeInfo[j].eFrameSrcType = AX_VIN_FRAME_SOURCE_TYPE_USER;
            } else {
                pCam->tPipeInfo[j].eFrameSrcType = AX_VIN_FRAME_SOURCE_TYPE_DEV;
            }

            pCam->tChnAttr[pCam->nPipeId][0].tCompressInfo.enCompressMode = AX_COMPRESS_MODE_NONE;
            pCam->tChnAttr[pCam->nPipeId][1].tCompressInfo.enCompressMode = AX_COMPRESS_MODE_NONE;
            pCam->tChnAttr[pCam->nPipeId][2].tCompressInfo.enCompressMode = AX_COMPRESS_MODE_NONE;
            pCam->tPipeInfo[j].ePipeMode = tSamplePipeInfo[j].ePipeMode;
            pCam->tPipeInfo[j].bAiispEnable = tSamplePipeInfo[j].bAiispEnable;
            pCam->tPipeInfo[j].ePipeWorkMode = tSamplePipeInfo[j].ePipeWorkMode;

            strncpy(pCam->tPipeInfo[j].szBinPath, tSamplePipeInfo[j].szBinPath, sizeof(pCam->tPipeInfo[j].szBinPath));
        }
        pCam->nPipeId = i;
    }

    return 0;
}

static AX_U32 __sample_case_single_48c_photo_mode(AX_CAMERA_T *pCamList, SAMPLE_SNS_TYPE_E eSnsType,
        SAMPLE_VIN_PARAM_T *pVinParam, COMMON_SYS_ARGS_T *pCommonArgs)
{
    AX_S32 i = 0, j = 0;
    AX_CAMERA_T *pCam = NULL;
    COMMON_VIN_MODE_E eSysMode = pVinParam->eSysMode;
    AX_SNS_HDR_MODE_E eHdrMode = pVinParam->eHdrMode;
    AX_U32 nScaleRatio = 0;
    AX_VIN_DOWN_SCALE_RATIO_E eScaleRatio[2] = {AX_VIN_DOWN_SCALE_RATIO_2X, AX_VIN_DOWN_SCALE_RATIO_2X};
    AX_BOOL bEisEn[2] = {0};
    AX_BOOL bMotionEst[2] = {0};
    AX_BOOL bExtRawDumpEn[2] = {0};
    AX_VIN_DEV_TRIGGER_MODE_E  eDevTriggerMode = AX_VIN_DEV_TRIGGER_CONTINUE;
    SAMPLE_PIPE_INFO_T tSamplePipeInfo[] = {
        {AX_VIN_PIPE_NORMAL_MODE0, SAMPLE_PIPE_MODE_VIDEO, AX_FALSE, "null.bin"},
        {AX_VIN_PIPE_NORMAL_MODE0, SAMPLE_PIPE_MODE_VIDEO, AX_FALSE, "null.bin"},
    };

    pCommonArgs->nCamCnt = 1;

    switch (pVinParam->eSysCase) {
    case SAMPLE_VIN_UAV_OV48C40_PHOTO_MODE:
        eScaleRatio[0] = AX_VIN_DOWN_SCALE_RATIO_2X;
        eScaleRatio[1] = AX_VIN_DOWN_SCALE_RATIO_2X;
        bMotionEst[0] = AX_FALSE;
        bMotionEst[1] = AX_TRUE;
        bEisEn[0] = AX_FALSE;
        bEisEn[1] = AX_TRUE;
        bExtRawDumpEn[0] = AX_FALSE;
        bExtRawDumpEn[1] = AX_TRUE;
        eDevTriggerMode = AX_VIN_DEV_TRIGGER_NON_CONTINUE;
        break;
    default:
        eScaleRatio[0] = AX_VIN_DOWN_SCALE_RATIO_2X;
        eScaleRatio[1] = AX_VIN_DOWN_SCALE_RATIO_2X;
        bMotionEst[0] = AX_FALSE;
        bMotionEst[1] = AX_TRUE;
        break;
    }

    for (i = 0; i < pCommonArgs->nCamCnt; i++) {
        pCam = &pCamList[i];
        pCam->nPipeId = i;
        __set_vin_attr(pCam, eSnsType, eHdrMode, eSysMode, pVinParam->bAiispEnable);
        COMMON_VIN_GetSnsConfig(eSnsType, &pCam->tMipiRx, &pCam->tSnsAttr,
                                &pCam->tSnsClkAttr, &pCam->tDevAttr);

        pCam->eBusType = COMMON_ISP_GetSnsBusType(eSnsType);
        __set_pipe_hdr_mode(&pCam->tDevBindPipe.nHDRSel[0], eHdrMode);
        __set_pipe_hdr_mode(&pCam->tDevBindPipe.nHDRSel[1], eHdrMode);

        pCam->tDevBindPipe.nNum =  2;
        pCam->tDevBindPipe.nPipeId[0] = 0;
        pCam->tDevBindPipe.nPipeId[1] = 4;
        pCam->nDevId = 2;
        pCam->nRxDev = 2;
        pCam->tSnsClkAttr.nSnsClkIdx = 1;
        pCam->tDevAttr.eDevTriggerMode = eDevTriggerMode;
        pCam->tMipiRx.tMipiAttr.nDataRate = 2495;
        pCam->tMipiRx.tMipiAttr.eLaneNum = 4;
        pCam->tDevAttr.eSnsIntfType = AX_SNS_INTF_TYPE_MIPI_RAW;
        pCam->tDevAttr.ePixelFmt = AX_FORMAT_BAYER_RAW_10BPP;
        pCam->tDevAttr.eDevMode = AX_VIN_DEV_OFFLINE;

        pCam->nDevId = 2;
        pCam->nRxDev = 2;
        pCam->tSnsClkAttr.nSnsClkIdx = 1;
        pCam->tSnsClkAttr.eSnsClkRate = AX_SNS_CLK_24M;
        pCam->tDevAttr.eDevWorkMode = AX_VIN_DEV_WORK_MODE_2MULTIPLEX;
        pCam->tDevAttr.tDevImgRgn[0].nStartX = 0;
        pCam->tDevAttr.tDevImgRgn[0].nStartY = 0;
        pCam->tDevAttr.tDevImgRgn[0].nWidth = 4096;
        pCam->tDevAttr.tDevImgRgn[0].nHeight = 6000;
        pCam->tDevAttr.nWidthStride[0] = 8000;

        pCam->tDevAttr.tDevImgRgn[1].nStartX = 4096;
        pCam->tDevAttr.tDevImgRgn[1].nStartY = 0;
        pCam->tDevAttr.tDevImgRgn[1].nWidth = 3904;
        pCam->tDevAttr.tDevImgRgn[1].nHeight = 6000;
        pCam->tDevAttr.nWidthStride[1] = 8000;
        pCam->tDevFrmIntAttr.bImgRgnIntEn[0] = AX_FALSE;
        pCam->tDevFrmIntAttr.bImgRgnIntEn[1] = AX_TRUE;
        pCam->tDevAttr.tCompressInfo.enCompressMode = AX_COMPRESS_MODE_LOSSLESS;

        for (j = 0; j < pCam->tDevBindPipe.nNum; j++) {
            pCam->nPipeId = pCam->tDevBindPipe.nPipeId[j];
            pCam->ptSnsHdl[pCam->nPipeId] = COMMON_ISP_GetSnsObj(eSnsType);
            COMMON_VIN_GetPipeConfig(eSnsType,
                                     &pCam->tPipeAttr[pCam->nPipeId], pCam->tChnAttr[pCam->nPipeId], pCam->bChnEnable[pCam->nPipeId]);
            if (pCam->nPipeId == 0) {
                pCam->tPipeAttr[pCam->nPipeId].tCompressInfo.enCompressMode =  AX_COMPRESS_MODE_LOSSLESS;

            }

            pCam->tPipeAttr[pCam->nPipeId].tMotionAttr.bMotionEst =  bMotionEst[j];
            pCam->bEisEnable[pCam->nPipeId] =  bEisEn[j];

            if (bExtRawDumpEn[j]) {
                nScaleRatio = 2 << eScaleRatio[j];
                pCam->tPipeAttr[pCam->nPipeId].tPipeImgRgn.nStartX /= nScaleRatio;
                pCam->tPipeAttr[pCam->nPipeId].tPipeImgRgn.nStartY /= nScaleRatio;
                pCam->tPipeAttr[pCam->nPipeId].tPipeImgRgn.nHeight /= nScaleRatio;
                pCam->tPipeAttr[pCam->nPipeId].tPipeImgRgn.nWidth /= nScaleRatio;
                pCam->tPipeAttr[pCam->nPipeId].tCompressInfo.enCompressMode = AX_COMPRESS_MODE_NONE;

                pCam->tChnAttr[pCam->nPipeId][0].nWidth /= nScaleRatio;
                pCam->tChnAttr[pCam->nPipeId][0].nHeight /= nScaleRatio;
                pCam->tChnAttr[pCam->nPipeId][0].nWidthStride /= nScaleRatio;
            }

            pCam->tIspDependAttr[0].nDependPipeId = 4;
            pCam->tIspDependAttr[0].eDependMode = AX_ISP_DEPEND_PIPE_MODE0;

            if (bEisEn[j]) {
                pCam->tChnAttr[pCam->nPipeId][0].nWidthStride = (pCam->tChnAttr[pCam->nPipeId][0].nWidthStride + 127) / 128 * 128;
            }
            if (bExtRawDumpEn[j]) {
                pCam->tExtRawDumpAttr[pCam->nPipeId].bEnable = AX_TRUE;
                pCam->tExtRawDumpAttr[pCam->nPipeId].nDepth = 2;
                pCam->tExtRawDumpAttr[pCam->nPipeId].eScaleRatio = eScaleRatio[j];
                pCam->tPipeInfo[j].eFrameSrcType = AX_VIN_FRAME_SOURCE_TYPE_USER;
            } else {
                pCam->tPipeInfo[j].eFrameSrcType = AX_VIN_FRAME_SOURCE_TYPE_DEV;
            }

            pCam->tChnAttr[pCam->nPipeId][0].tCompressInfo.enCompressMode = AX_COMPRESS_MODE_NONE;
            pCam->tChnAttr[pCam->nPipeId][1].tCompressInfo.enCompressMode = AX_COMPRESS_MODE_NONE;
            pCam->tChnAttr[pCam->nPipeId][2].tCompressInfo.enCompressMode = AX_COMPRESS_MODE_NONE;
            pCam->tPipeInfo[j].ePipeMode = tSamplePipeInfo[j].ePipeMode;
            pCam->tPipeInfo[j].bAiispEnable = tSamplePipeInfo[j].bAiispEnable;
            pCam->tPipeInfo[j].ePipeWorkMode = tSamplePipeInfo[j].ePipeWorkMode;

            strncpy(pCam->tPipeInfo[j].szBinPath, tSamplePipeInfo[j].szBinPath, sizeof(pCam->tPipeInfo[j].szBinPath));
        }
        pCam->nPipeId = i;
    }

    return 0;
}

static AX_U32 __sample_case_config(SAMPLE_VIN_PARAM_T *pVinParam, COMMON_SYS_ARGS_T *pCommonArgs,
                                   COMMON_SYS_ARGS_T *pPrivArgs)
{
    AX_CAMERA_T         *pCamList = &gCams[0];
    SAMPLE_SNS_TYPE_E   eSnsType = OMNIVISION_OS08A20;

    COMM_ISP_PRT("eSysCase %d, eSysMode %d, eHdrMode %d, bAiispEnable %d\n", pVinParam->eSysCase, pVinParam->eSysMode,
                 pVinParam->eHdrMode, pVinParam->bAiispEnable);

    switch (pVinParam->eSysCase) {

    case SAMPLE_VIN_UAV_OV48C40_VIDEO_MODE:
        eSnsType = OMNIVISION_OV48C40;
        /* pool config */
        if (AX_SNS_LINEAR_MODE == pVinParam->eHdrMode) {
            __cal_dump_pool(gtSysCommPoolSingleOv48c40Sdr, pVinParam->eHdrMode, pVinParam->nDumpFrameNum);
            pCommonArgs->nPoolCfgCnt = sizeof(gtSysCommPoolSingleOv48c40Sdr) /
                                       sizeof(gtSysCommPoolSingleOv48c40Sdr[0]);
            pCommonArgs->pPoolCfg = gtSysCommPoolSingleOv48c40Sdr;
        }

        /* private pool config */
        if (AX_SNS_LINEAR_MODE == pVinParam->eHdrMode) {
            __cal_dump_pool(gtPrivatePoolSingleOv48c40Sdr, pVinParam->eHdrMode, pVinParam->nDumpFrameNum);
            pPrivArgs->nPoolCfgCnt = sizeof(gtPrivatePoolSingleOv48c40Sdr) /
                                     sizeof(gtPrivatePoolSingleOv48c40Sdr[0]);
            pPrivArgs->pPoolCfg = gtPrivatePoolSingleOv48c40Sdr;
        }

        /* cams config */
        __sample_case_single_48c_video_mode(pCamList, eSnsType, pVinParam, pCommonArgs);
        break;
    case SAMPLE_VIN_UAV_OV48C40_PHOTO_MODE:
        eSnsType = OMNIVISION_OV48C40;
        /* pool config */
        if (AX_SNS_LINEAR_MODE == pVinParam->eHdrMode) {
            __cal_dump_pool(gtSysCommPoolSingleOv48c40Sdr, pVinParam->eHdrMode, pVinParam->nDumpFrameNum);
            pCommonArgs->nPoolCfgCnt = sizeof(gtSysCommPoolSingleOv48c40Sdr) /
                                       sizeof(gtSysCommPoolSingleOv48c40Sdr[0]);
            pCommonArgs->pPoolCfg = gtSysCommPoolSingleOv48c40Sdr;
        }

        /* private pool config */
        if (AX_SNS_LINEAR_MODE == pVinParam->eHdrMode) {
            __cal_dump_pool(gtPrivatePoolSingleOv48c40Sdr, pVinParam->eHdrMode, pVinParam->nDumpFrameNum);
            pPrivArgs->nPoolCfgCnt = sizeof(gtPrivatePoolSingleOv48c40Sdr) /
                                     sizeof(gtPrivatePoolSingleOv48c40Sdr[0]);
            pPrivArgs->pPoolCfg = gtPrivatePoolSingleOv48c40Sdr;
        }

        /* cams config */
        __sample_case_single_48c_photo_mode(pCamList, eSnsType, pVinParam, pCommonArgs);
        break;
    default:
        COMM_ISP_PRT("eSysCase %d not match, 48M need %d need %d \n", pVinParam->eSysCase,
                     SAMPLE_VIN_UAV_OV48C40_VIDEO_MODE, SAMPLE_VIN_UAV_OV48C40_VIDEO_MODE);
    }

    return 0;
}

static AX_S32 EventTriggerTask(AX_U8 nDevId, AX_CAMERA_T *pCam, AX_SNS_HDR_MODE_E eHdrMode)
{
    AX_S32 axRet = 0;

    static AX_U32 cnttt = 0;

    cnttt++;
    if (pCam->tDevAttr.eDevTriggerMode == AX_VIN_DEV_TRIGGER_NON_CONTINUE) {
        if (!(cnttt % 10)) {
            AX_VIN_DEV_TRIGGER_INFO_T tTriggerInfo = {0};
            tTriggerInfo.nTriggerNum = 3;
            axRet = AX_VIN_TriggerDev(nDevId, &tTriggerInfo);
        }
    }
    usleep(90 * 1000);

    return axRet;
}

static void *TriggerThread(void *args)
{
    AX_CHAR token[32] = {0};
    AX_CAMERA_T *pCam = (AX_CAMERA_T *)args;

    AX_U8 nDevId = pCam->nDevId;
    AX_SNS_HDR_MODE_E eHdrMode = pCam->eHdrMode;


    snprintf(token, 32, "RAW_DISP_%u", nDevId);
    prctl(PR_SET_NAME, token);

    while (!g_trigger_loop_exit[pCam->nNumber]) {
        EventTriggerTask(nDevId, pCam, eHdrMode);
    }

    return NULL;
}


AX_S32 _UAV_ISP_Init(AX_U8 nPipeId, AX_SENSOR_REGISTER_FUNC_T *ptSnsHdl, AX_BOOL bRegister3A,
                     AX_BOOL bUser3a, AX_ISP_AE_REGFUNCS_T *tAeFuncs, AX_ISP_AWB_REGFUNCS_T *tAwbFuncs,
                     AX_ISP_AF_REGFUNCS_T *tAfFuncs, AX_ISP_DEPEND_ATTR_T *ptDpendAttr, AX_CHAR *pIspParamsFile)
{
    AX_S32 axRet = 0;

    if (AX_NULL == ptSnsHdl) {
        COMM_ISP_PRT("AX_ISP Sensor Object NULL!\n");
        return -1;
    }

    axRet = AX_ISP_Create(nPipeId);
    if (0 != axRet) {
        COMM_ISP_PRT("AX_ISP_Init failed, ret=0x%x\n", axRet);
        return -1;
    }

    if (AX_ISP_DEPEND_PIPE_NONE != ptDpendAttr->eDependMode) {
        axRet = AX_ISP_SetDependAttr(nPipeId, ptDpendAttr);
        if (axRet != 0) {
            COMM_ISP_PRT("AX_ISP_SetDependAttr failed, axRet = %d\n", axRet);
        }
    }

    if (bRegister3A) {
        /* ae alg register*/
        axRet = COMMON_ISP_RegisterAeAlgLib(nPipeId, ptSnsHdl, bUser3a, tAeFuncs);
        if (0 != axRet) {
            COMM_ISP_PRT("[%u]:RegisterAeAlgLib failed, ret=0x%x.\n", nPipeId, axRet);
            return -1;
        }

        /* awb alg register*/
        axRet = COMMON_ISP_RegisterAwbAlgLib(nPipeId, ptSnsHdl, bUser3a, tAwbFuncs);
        if (0 != axRet) {
            COMM_ISP_PRT("[%u]:RegisterAwbAlgLib failed, ret=0x%x.\n", nPipeId, axRet);
            return -1;
        }
    }

    if (strcmp(pIspParamsFile, "null.bin")) {
        axRet = AX_ISP_LoadBinParams(nPipeId, pIspParamsFile);
        if (0 != axRet) {
            COMM_ISP_PRT("[%u]:AX_ISP_LoadBinParams ret=0x%x %s. The parameters in sensor.h will be used\n",
                         nPipeId, axRet, pIspParamsFile);
        }
    }

    if (AX_NULL != tAfFuncs) {
        s_tAfFuncs = *tAfFuncs;
        if (AX_NULL != s_tAfFuncs.pfnCAf_Init) {
            s_tAfFuncs.pfnCAf_Init(nPipeId);
        }
    }

    axRet = AX_ISP_Open(nPipeId);
    if (0 != axRet) {
        COMM_ISP_PRT("AX_ISP_Open failed, ret=0x%x\n", axRet);
        return -1;
    }

    return axRet;
}

static AX_S32 __case_uav_cam_open(AX_CAMERA_T *pCam)
{
    AX_S32 i = 0;
    AX_S32 axRet = 0;
    AX_U8 nPipeId = 0;
    AX_U8 nDevId = pCam->nDevId;
    AX_U32 nRxDev = pCam->nRxDev;
    AX_BOOL bIsEisVideo = AX_FALSE;

    axRet = COMMON_ISP_ResetSnsObj(nPipeId, nDevId, pCam->ptSnsHdl[pCam->nPipeId]);
    if (0 != axRet) {
        COMM_CAM_PRT("COMMON_ISP_ResetSnsObj failed, ret=0x%x.\n", axRet);
        return -1;
    }

    axRet = COMMON_VIN_StartMipi(nRxDev, &pCam->tMipiRx);
    if (0 != axRet) {
        COMM_CAM_PRT("COMMON_VIN_StartMipi failed, r-et=0x%x.\n", axRet);
        return -1;
    }

    axRet = COMMON_VIN_CreateDev(nDevId, nRxDev, &pCam->tDevAttr, &pCam->tDevBindPipe, &pCam->tDevFrmIntAttr);
    if (0 != axRet) {
        COMM_CAM_PRT("COMMON_VIN_CreateDev failed, ret=0x%x.\n", axRet);
        return -1;
    }

    for (i = 0; i < pCam->tDevBindPipe.nNum; i++) {
        nPipeId = pCam->tDevBindPipe.nPipeId[i];
        pCam->tPipeAttr[nPipeId].bAiIspEnable = pCam->tPipeInfo[i].bAiispEnable;
        pCam->tPipeAttr[nPipeId].eCombMode = pCam->tPipeInfo[i].eCombMode;
        pCam->tPipeAttr[nPipeId].ePipeWorkMode = pCam->tPipeInfo[i].ePipeWorkMode;

        axRet = COMMON_VIN_SetPipeAttr(pCam->eSysMode, nPipeId, &pCam->tPipeAttr[nPipeId],
                                       pCam->tPipeInfo[i].eFrameSrcType);
        if (0 != axRet) {
            COMM_CAM_PRT("COMMON_ISP_SetPipeAttr failed, ret=0x%x.\n", axRet);
            return -1;
        }

        if (pCam->tExtRawDumpAttr[nPipeId].bEnable) {
            axRet = AX_VIN_SetExtRawDumpAttr(nPipeId, &pCam->tExtRawDumpAttr[nPipeId]);
            if (0 != axRet) {
                COMM_CAM_PRT("AX_VIN_SetExtRawDumpAttr, ret=0x%x.\n", axRet);
            }

            bIsEisVideo = AX_TRUE;
        }

        if (pCam->bRegisterSns) {
            axRet = COMMON_ISP_RegisterSns(nPipeId, nDevId, pCam->eBusType, pCam->ptSnsHdl[pCam->nPipeId]);
            if (0 != axRet) {
                COMM_CAM_PRT("COMMON_ISP_RegisterSns failed, ret=0x%x.\n", axRet);
                return -1;
            }
            axRet = COMMON_ISP_SetSnsAttr(nPipeId, nDevId, &pCam->tSnsAttr, &pCam->tSnsClkAttr);
            if (0 != axRet) {
                COMM_CAM_PRT("COMMON_ISP_SetSnsAttr failed, ret=0x%x.\n", axRet);
                return -1;
            }
        }

        axRet = _UAV_ISP_Init(nPipeId, pCam->ptSnsHdl[pCam->nPipeId], pCam->bRegisterSns, pCam->bUser3a,
                              &pCam->tAeFuncs, &pCam->tAwbFuncs, &pCam->tAfFuncs, &pCam->tIspDependAttr[nPipeId],
                              pCam->tPipeInfo[i].szBinPath);
        if (0 != axRet) {
            COMM_CAM_PRT("COMMON_ISP_StartIsp failed, axRet = 0x%x.\n", axRet);
            return -1;
        }

        if (pCam->bEisEnable[nPipeId]) {
            AX_ISP_IQ_DIS_PARAM_T tDISParam;
            axRet = AX_ISP_IQ_GetDisParam(nPipeId, &tDISParam);
            if (AX_SUCCESS != axRet) {
                COMM_CAM_PRT("Pipe[%d] AX_ISP_IQ_GetDisParam fail, nRet:0x%x", nPipeId, axRet);
            }

            tDISParam.bDisEnable = AX_TRUE;
            axRet = AX_ISP_IQ_SetDisParam(nPipeId, &tDISParam);
            if (AX_SUCCESS != axRet) {
                COMM_CAM_PRT("Pipe[%d] AX_ISP_IQ_SetDisParam fail, nRet:0x%x", nPipeId, axRet);
            }
        }

        axRet = COMMON_VIN_StartChn(nPipeId, pCam->tChnAttr[nPipeId], pCam->bChnEnable[nPipeId]);
        if (0 != axRet) {
            COMM_CAM_PRT("COMMON_ISP_StartChn failed, nRet = 0x%x.\n", axRet);
            return -1;
        }

        axRet = AX_VIN_StartPipe(nPipeId);
        if (0 != axRet) {
            COMM_CAM_PRT("AX_VIN_StartPipe failed, ret=0x%x\n", axRet);
            return -1;
        }

        if (pCam->tPipeInfo[nPipeId].ePipeMode != SAMPLE_PIPE_MODE_FLASH_SNAP) {
            axRet = AX_ISP_Start(nPipeId);
            if (0 != axRet) {
                COMM_CAM_PRT("AX_ISP_Open failed, ret=0x%x\n", axRet);
                return -1;
            }
        }

        /* When there are uav multiple pipe, the full size pipe needs Manual AE */
        if (0 == i) {
            axRet = COMMON_ISP_SetAeToManual(nPipeId);
            if (0 != axRet) {
                COMM_CAM_PRT("COMMON_ISP_SetAeToManual failed, ret=0x%x\n", axRet);
                return -1;
            }
        }
    }

    axRet = COMMON_VIN_StartDev(nDevId, pCam->bEnableDev, &pCam->tDevAttr);
    if (0 != axRet) {
        COMM_CAM_PRT("COMMON_VIN_StartDev failed, ret=0x%x.\n", axRet);
        return -1;
    }

    if (pCam->eSysMode != COMMON_VIN_LOADRAW) {
        for (i = 0; i < pCam->tDevBindPipe.nNum; i++) {
            if (pCam->bRegisterSns) {
                axRet = AX_ISP_StreamOn(pCam->tDevBindPipe.nPipeId[i]);
                if (0 != axRet) {
                    COMM_CAM_PRT(" failed, ret=0x%x.\n", axRet);
                    return -1;
                }
            }
        }
    }

    if (pCam->bEnableDev &&
        ((pCam->tPipeAttr[pCam->nPipeId].ePipeWorkMode == AX_VIN_PIPE_NORMAL_MODE0) ||
         (pCam->tPipeAttr[pCam->nPipeId].ePipeWorkMode == AX_VIN_PIPE_NORMAL_MODE2))) {
        if (pCam->nNumber >= MAX_CAMERAS) {
            COMM_CAM_PRT("Cam number[%d] out of range\n", pCam->nNumber);
            return -1;
        }


        COMM_CAM_PRT("ePipeWorkMode:%d \n", pCam->tPipeAttr[pCam->nPipeId].ePipeWorkMode);
        g_trigger_loop_exit[pCam->nNumber] = 0;
        g_get_ext_raw_loop_exit[pCam->nNumber] = 0;
        if ((pCam->tPipeInfo[0].eFrameSrcType == AX_VIN_FRAME_SOURCE_TYPE_USER) ||
            (pCam->tDevAttr.eDevTriggerMode == AX_VIN_DEV_TRIGGER_NON_CONTINUE)) {
            if (pCam->tPipeInfo[0].ePipeWorkMode == AX_VIN_PIPE_NORMAL_MODE0) {
                pthread_create(&gTriggerThread[pCam->nNumber], NULL, TriggerThread, (AX_VOID *)pCam);
            }
        }

        if (AX_TRUE == bIsEisVideo) {
            pthread_create(&gGetExtRawThread[pCam->nNumber], NULL, GetExtRawFrameThread, (AX_VOID *)pCam);

        }
    }

    return 0;
}

static AX_S32 __case_uav_cam_close(AX_CAMERA_T *pCam)
{
    AX_U8 i = 0;
    AX_S32 axRet = 0;
    AX_U8 nPipeId = pCam->nPipeId;
    AX_U8 nDevId = pCam->nDevId;
    AX_U32 nRxDev = pCam->nRxDev;

    for (i = 0; i < pCam->tDevBindPipe.nNum; i++) {
        nPipeId = pCam->tDevBindPipe.nPipeId[i];
        if (pCam->tPipeInfo[nPipeId].ePipeMode != SAMPLE_PIPE_MODE_FLASH_SNAP) {
            axRet |= AX_ISP_Stop(nPipeId);
            if (0 != axRet) {
                COMM_ISP_PRT("AX_ISP_Stop failed, ret=0x%x.\n", axRet);
            }
        }
    }

    if (pCam->nNumber < MAX_CAMERAS) {
        g_get_ext_raw_loop_exit[pCam->nNumber] = 1;
        if (gGetExtRawThread[pCam->nNumber] != 0) {
            axRet = pthread_join(gGetExtRawThread[pCam->nNumber], NULL);
            if (axRet < 0) {
                COMM_CAM_PRT(" dispacher thread exit failed, ret=0x%x.\n", axRet);
            }
            gGetExtRawThread[pCam->nNumber] = 0;
        }

        g_trigger_loop_exit[pCam->nNumber] = 1;
        if (gTriggerThread[pCam->nNumber] != 0) {
            axRet = pthread_join(gTriggerThread[pCam->nNumber], NULL);
            if (axRet < 0) {
                COMM_CAM_PRT(" dispacher thread exit failed, ret=0x%x.\n", axRet);
            }
            gTriggerThread[pCam->nNumber] = 0;
        }

    } else {
        COMM_CAM_PRT("Cam number[%d] out of range\n", pCam->nNumber);
    }

    if (pCam->bEnableFlash == AX_TRUE) {
        axRet = COMMON_VIN_StopOutsideDev(nDevId);
        if (0 != axRet) {
            COMM_CAM_PRT("COMMON_VIN_StopOutsideDev failed, ret=0x%x.\n", axRet);
            return -1;
        }
    }

    axRet = COMMON_VIN_StopDev(nDevId, pCam->bEnableDev);
    if (0 != axRet) {
        COMM_CAM_PRT("COMMON_VIN_StopDev failed, ret=0x%x.\n", axRet);
    }

    if (pCam->eSysMode != COMMON_VIN_LOADRAW) {
        if (pCam->bRegisterSns) {
            for (i = 0; i < pCam->tDevBindPipe.nNum; i++) {
                AX_ISP_StreamOff(pCam->tDevBindPipe.nPipeId[i]);
            }
        }
    }

    axRet = AX_ISP_CloseSnsClk(pCam->tSnsClkAttr.nSnsClkIdx);
    if (0 != axRet) {
        COMM_CAM_PRT("AX_VIN_CloseSnsClk failed, ret=0x%x.\n", axRet);
    }

    for (i = 0; i < pCam->tDevBindPipe.nNum; i++) {
        nPipeId = pCam->tDevBindPipe.nPipeId[i];
        axRet = AX_VIN_StopPipe(nPipeId);
        if (0 != axRet) {
            COMM_CAM_PRT("AX_VIN_StopPipe failed, ret=0x%x.\n", axRet);
        }

        COMMON_VIN_StopChn(nPipeId);

        COMMON_ISP_DeInit(nPipeId, pCam->bRegisterSns);

        COMMON_ISP_UnRegisterSns(nPipeId);

        AX_VIN_DestroyPipe(nPipeId);
    }

    axRet = COMMON_VIN_StopMipi(nRxDev);
    if (0 != axRet) {
        COMM_CAM_PRT("COMMON_VIN_StopMipi failed, ret=0x%x.\n", axRet);
    }

    axRet = COMMON_VIN_DestroyDev(nDevId);
    if (0 != axRet) {
        COMM_CAM_PRT("COMMON_VIN_DestroyDev failed, ret=0x%x.\n", axRet);
    }

    COMM_CAM_PRT("%s: nDevId %d: exit.\n", __func__, nDevId);

    return AX_SUCCESS;
}

AX_S32 CASE_UAV_CAM_Open(AX_CAMERA_T *pCamList, AX_U8 Num)
{
    AX_U16 i = 0;
    if (pCamList == NULL) {
        return -1;
    }

    for (i = 0; i < Num; i++) {
        if (AX_SUCCESS == __case_uav_cam_open(&pCamList[i])) {
            pCamList[i].bOpen = AX_TRUE;
            COMM_CAM_PRT("camera %d is open\n", i);
        } else {
            goto EXIT;
        }
    }
    return 0;
EXIT:
    for (i = 0; i < Num; i++) {
        if (!pCamList[i].bOpen) {
            continue;
        }
        __case_uav_cam_close(&pCamList[i]);
    }
    return -1;
}

AX_S32 CASE_UAV_CAM_Close(AX_CAMERA_T *pCamList, AX_U8 Num)
{
    AX_U16 i = 0;
    if (pCamList == NULL) {
        return -1;
    }

    for (i = 0; i < Num; i++) {
        if (!pCamList[i].bOpen) {
            continue;
        }

        if (AX_SUCCESS == __case_uav_cam_close(&pCamList[i])) {
            COMM_CAM_PRT("camera %d is close\n", i);
            pCamList[i].bOpen = AX_FALSE;
        } else {
            return -1;
        }
    }

    return 0;
}

AX_S32 SAMPLE_CASE_VIN_UAV_OV48C40(SAMPLE_VIN_PARAM_T *pVinParam)
{
    AX_S32 axRet = 0;
    AX_S32 i = 0, j = 0;
    COMMON_SYS_ARGS_T tCommonArgs = {0};
    COMMON_SYS_ARGS_T tPrivArgs = {0};
    signal(SIGINT, __sigint);

    /* Step1: cam config & pool Config */
    __sample_case_config(pVinParam, &tCommonArgs, &tPrivArgs);

    /* Step2: SYS Init */
    axRet = COMMON_SYS_Init(&tCommonArgs);
    if (axRet) {
        COMM_ISP_PRT("COMMON_SYS_Init fail, ret:0x%x", axRet);
        goto EXIT_FAIL;
    }
    /* Step3: NPU Init */
    axRet = COMMON_NPU_Init(AX_ENGINE_VIRTUAL_NPU_BIG_LITTLE);
    if (axRet) {
        COMM_ISP_PRT("COMMON_NPU_Init fail, ret:0x%x", axRet);
        goto EXIT_FAIL1;
    }
    /* Step4: Cam Init */
    axRet = COMMON_CAM_Init();
    if (axRet) {
        COMM_ISP_PRT("COMMON_CAM_Init fail, ret:0x%x", axRet);
        goto EXIT_FAIL1;
    }
    axRet = COMMON_CAM_PrivPoolInit(&tPrivArgs);
    if (axRet) {
        COMM_ISP_PRT("COMMON_CAM_PrivPoolInit fail, ret:0x%x", axRet);
        goto EXIT_FAIL1;
    }
    /* Step5: Cam Open */
    axRet = CASE_UAV_CAM_Open(&gCams[0], tCommonArgs.nCamCnt);
    if (axRet) {
        COMM_ISP_PRT("COMMON_CAM_Open fail, ret:0x%x", axRet);
        goto EXIT_FAIL2;
    }
    /* Step6. NT Init (tuning socket server. optional)
    Stream default port 6000, Ctrl default port 8082 */
    axRet = COMMON_NT_Init(6000, 8082);
    if (axRet) {
        COMM_ISP_PRT("COMMON_NT_Init fail, ret:0x%x", axRet);
        goto EXIT_FAIL3;
    }
    /* update pipe attribute */
    for (i = 0; i < tCommonArgs.nCamCnt; i++) {
        for (j = 0; j < gCams[i].tDevBindPipe.nNum; j++) {
            COMMON_NT_UpdateSource(gCams[i].tDevBindPipe.nPipeId[j]);
        }
    }

    while (!gLoopExit) {
        sleep(1);
    }

    COMMON_NT_DeInit();
EXIT_FAIL3:
    CASE_UAV_CAM_Close(&gCams[0], tCommonArgs.nCamCnt);
EXIT_FAIL2:
    COMMON_CAM_Deinit();
EXIT_FAIL1:
    COMMON_SYS_DeInit();
EXIT_FAIL:
    return axRet;
}


AX_S32 COMMON_CAM_ExtRawFrameProc(AX_U8 nDevId, AX_CAMERA_T *pCam, AX_SNS_HDR_MODE_E eHdrMode)
{
    AX_S32 axRet = 0;
    AX_S32 timeOutMs = -1;
    AX_IMG_INFO_T frameBufferArr[AX_SNS_HDR_FRAME_MAX] = {0};
    AX_S32 nCapturePipeId = 4;

    /* 1. Get ext raw frame*/
    axRet = AX_VIN_GetExtRawFrame(nCapturePipeId, AX_SNS_HDR_FRAME_L, frameBufferArr, timeOutMs);
    if (axRet != 0) {
        COMM_CAM_PRT("Get Pipe Ext raw frame failed\n");
        return 0;
    }

    /* 2. send raw frame to ife*/
    axRet = AX_VIN_SendRawFrame(nCapturePipeId, AX_VIN_FRAME_SOURCE_ID_IFE, eHdrMode,
                                (const AX_IMG_INFO_T **)&frameBufferArr, 0);
    if (axRet != 0) {
        COMM_CAM_PRT("Send Pipe Ext raw frame failed\n");
    }

    /* 3. Release ext raw frame*/
    axRet = AX_VIN_ReleaseExtRawFrame(nCapturePipeId, AX_SNS_HDR_FRAME_L, frameBufferArr);
    if (axRet != 0) {
        COMM_CAM_PRT("Release Pipe raw frame failed\n");
    }
    return 0;
}

static void *GetExtRawFrameThread(void *args)
{
    AX_CHAR token[32] = {0};
    AX_CAMERA_T *pCam = (AX_CAMERA_T *)args;

    AX_U8 nDevId = pCam->nDevId;
    AX_SNS_HDR_MODE_E eHdrMode = pCam->eHdrMode;


    snprintf(token, 32, "GetExtRawFrame%u", nDevId);
    prctl(PR_SET_NAME, token);

    while (!g_get_ext_raw_loop_exit[pCam->nNumber]) {
        COMMON_CAM_ExtRawFrameProc(nDevId, pCam, eHdrMode);
    }

    return NULL;
}

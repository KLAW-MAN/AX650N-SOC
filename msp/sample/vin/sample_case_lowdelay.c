/**************************************************************************************************
 *
 * Copyright (c) 2019-2023 Axera Semiconductor (Shanghai) Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor (Shanghai) Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor (Shanghai) Co., Ltd.
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
#include "common_que.h"
#include "ax_vin_error_code.h"
#include "common_nt.h"
#include "ax_buffer_tool.h"


static COMMON_SYS_POOL_CFG_T gtSysCommPoolSingleOv48c40Sdr[] = {
    {4000, 6000, 4000, AX_FORMAT_YUV420_SEMIPLANAR, 20},   /* vin nv21/nv21 use */
    // {8000, 6000, 8000, AX_FORMAT_YUV420_SEMIPLANAR, 20},   /* vin nv21/nv21 use */
};

static COMMON_SYS_POOL_CFG_T gtPrivatePoolSingleOv48c40Sdr[] = {
    {4000, 6000, 4000, AX_FORMAT_BAYER_RAW_16BPP, 15},      /* vin raw16 use */
    // {8000, 6000, 8000, AX_FORMAT_BAYER_RAW_16BPP, 15},      /* vin raw16 use */
};

static AX_CAMERA_T gCams[MAX_CAMERAS] = {0};
static volatile AX_S32 gLoopExit = 0;
static pthread_t gDispatchThread[MAX_CAMERAS] = {0};
static pthread_t gGetYUVThread[MAX_CAMERAS] = {0};
static AX_S32 g_dispatcher_loop_exit[MAX_CAMERAS] = {0};
static AX_S32 g_getYUV_loop_exit[MAX_CAMERAS] = {0};
static Queue payloadQue[MAX_CAMERAS] = {0};
static AX_ISP_AF_REGFUNCS_T s_tAfFuncs = {0};

static void *DispatchThread(void *args);
static void *GetYUVFrameThread(void *args);
static AX_S32 SysFrameDispatch(AX_U8 nDevId, AX_CAMERA_T *pCam, AX_SNS_HDR_MODE_E eHdrMode);

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
    AX_VIN_DEV_TRIGGER_MODE_E  eDevTriggerMode = AX_VIN_DEV_TRIGGER_CONTINUE;
    SAMPLE_PIPE_INFO_T tSamplePipeInfo[] = {
        {AX_VIN_PIPE_NORMAL_MODE0, SAMPLE_PIPE_MODE_VIDEO, AX_FALSE, "null.bin"},
    };

    pCommonArgs->nCamCnt = 1;

    for (i = 0; i < pCommonArgs->nCamCnt; i++) {
        pCam = &pCamList[i];
        pCam->nPipeId = i;
        __set_vin_attr(pCam, eSnsType, eHdrMode, eSysMode, pVinParam->bAiispEnable);
        COMMON_VIN_GetSnsConfig(eSnsType, &pCam->tMipiRx, &pCam->tSnsAttr,
                                &pCam->tSnsClkAttr, &pCam->tDevAttr);

        pCam->eBusType = COMMON_ISP_GetSnsBusType(eSnsType);
        __set_pipe_hdr_mode(&pCam->tDevBindPipe.nHDRSel[0], eHdrMode);
        __set_pipe_hdr_mode(&pCam->tDevBindPipe.nHDRSel[1], eHdrMode);

        pCam->tDevBindPipe.nNum =  1;
        pCam->tDevBindPipe.nPipeId[0] = 0;

        pCam->nDevId = 2;
        pCam->nRxDev = 2;
        pCam->tSnsClkAttr.nSnsClkIdx = 1;
        pCam->tSnsClkAttr.eSnsClkRate = AX_SNS_CLK_24M;
        pCam->tDevAttr.eDevTriggerMode = eDevTriggerMode;
        pCam->tDevAttr.eDevWorkMode = AX_VIN_DEV_WORK_MODE_2MULTIPLEX;
        pCam->tDevAttr.eDevMode = AX_VIN_DEV_OFFLINE;
        pCam->tDevAttr.tDevImgRgn[0].nStartX = 0;
        pCam->tDevAttr.tDevImgRgn[0].nStartY = 0;
        pCam->tDevAttr.tDevImgRgn[0].nWidth = 4000;
        pCam->tDevAttr.tDevImgRgn[0].nHeight = 3000;
        pCam->tDevAttr.nWidthStride[0] = 4000;

        pCam->tDevAttr.tDevImgRgn[1].nStartX = 0;
        pCam->tDevAttr.tDevImgRgn[1].nStartY = 3000;
        pCam->tDevAttr.tDevImgRgn[1].nWidth = 4000;
        pCam->tDevAttr.tDevImgRgn[1].nHeight = 3000;
        pCam->tDevAttr.nWidthStride[1] = 4000;
        pCam->tDevFrmIntAttr.bImgRgnIntEn[0] = AX_TRUE;
        pCam->tDevFrmIntAttr.bImgRgnIntEn[1] = AX_TRUE;
        pCam->tDevAttr.tCompressInfo.enCompressMode = AX_COMPRESS_MODE_NONE;

        for (j = 0; j < pCam->tDevBindPipe.nNum; j++) {
            pCam->nPipeId = pCam->tDevBindPipe.nPipeId[j];
            pCam->ptSnsHdl[pCam->nPipeId] = COMMON_ISP_GetSnsObj(eSnsType);
            COMMON_VIN_GetPipeConfig(eSnsType,
                                     &pCam->tPipeAttr[pCam->nPipeId], pCam->tChnAttr[pCam->nPipeId], pCam->bChnEnable[pCam->nPipeId]);
            pCam->bChnEnable[pCam->nPipeId][0] = AX_TRUE;
            pCam->bChnEnable[pCam->nPipeId][1] = AX_FALSE;
            pCam->bChnEnable[pCam->nPipeId][2] = AX_FALSE;
            if (pCam->nPipeId == 0) {
                pCam->tPipeAttr[pCam->nPipeId].tCompressInfo.enCompressMode =  AX_COMPRESS_MODE_NONE;

            }

            pCam->tPipeAttr[pCam->nPipeId].tMotionAttr.bMotionEst =  AX_FALSE;
            pCam->bEisEnable[pCam->nPipeId] =  AX_FALSE;

            pCam->tPipeAttr[pCam->nPipeId].tPipeImgRgn.nStartX = 0;
            pCam->tPipeAttr[pCam->nPipeId].tPipeImgRgn.nStartY = 0;
            pCam->tPipeAttr[pCam->nPipeId].tPipeImgRgn.nWidth  = 4000;
            pCam->tPipeAttr[pCam->nPipeId].tPipeImgRgn.nHeight = 6000;
            pCam->tPipeAttr[pCam->nPipeId].tCompressInfo.enCompressMode = AX_COMPRESS_MODE_NONE;

            pCam->tChnAttr[pCam->nPipeId][0].nWidth  = 4000;
            pCam->tChnAttr[pCam->nPipeId][0].nHeight = 6000;
            pCam->tChnAttr[pCam->nPipeId][0].nWidthStride = 4000;

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

    case SAMPLE_VIN_LOW_DELAY_OV48C40:
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
    default:
        COMM_ISP_PRT("eSysCase %d not match, 48M lowdelay mode need %d\n", pVinParam->eSysCase,
                     SAMPLE_VIN_LOW_DELAY_OV48C40);
        break;
    }

    return 0;
}

AX_S32 __low_delay_isp_init(AX_U8 nPipeId, AX_SENSOR_REGISTER_FUNC_T *ptSnsHdl, AX_BOOL bRegister3A,
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

static AX_S32 __case_low_delay_cam_open(AX_CAMERA_T *pCam)
{
    AX_S32 i = 0;
    AX_S32 axRet = 0;
    AX_U8 nPipeId = 0;
    AX_U8 nDevId = pCam->nDevId;
    AX_U32 nRxDev = pCam->nRxDev;
    AX_VIN_LOW_DELAY_ATTR_T  tLowDelayAttr;

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

        tLowDelayAttr.bEnable = AX_TRUE;
        tLowDelayAttr.nSliceNum = 2;
        axRet = AX_VIN_SetPipeLowDelayAttr(nPipeId, &tLowDelayAttr);
        if (0 != axRet) {
            COMM_CAM_PRT("AX_VIN_SetPipeLowDelayAttr failed, ret=0x%x.\n", axRet);
            return -1;
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

        axRet = __low_delay_isp_init(nPipeId, pCam->ptSnsHdl[pCam->nPipeId], pCam->bRegisterSns, pCam->bUser3a,
                                     &pCam->tAeFuncs, &pCam->tAwbFuncs, &pCam->tAfFuncs, &pCam->tIspDependAttr[nPipeId],
                                     pCam->tPipeInfo[i].szBinPath);
        if (0 != axRet) {
            COMM_CAM_PRT("COMMON_ISP_StartIsp failed, axRet = 0x%x.\n", axRet);
            return -1;
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

        /* When there are multiple pipe, only the first pipe needs AE */
        if ((0 < i) && (pCam->bEnableFlash == AX_FALSE) && (AX_FALSE == pCam->bUser3a)) {
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
        g_dispatcher_loop_exit[pCam->nNumber] = 0;

        if (pCam->tPipeInfo[0].eFrameSrcType == AX_VIN_FRAME_SOURCE_TYPE_USER) {
            if (pCam->tPipeInfo[0].ePipeWorkMode == AX_VIN_PIPE_NORMAL_MODE0) {
                COMMON_QUEUE_Init(&payloadQue[pCam->nNumber]);
                pthread_create(&gDispatchThread[pCam->nNumber], NULL, DispatchThread, (AX_VOID *)pCam);
            }
        }

        g_getYUV_loop_exit[pCam->nNumber] = 0;
        pthread_create(&gGetYUVThread[pCam->nNumber], NULL, GetYUVFrameThread, (AX_VOID *)pCam);
    }

    return 0;
}

static AX_S32 __case_low_delay_cam_close(AX_CAMERA_T *pCam)
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
        g_dispatcher_loop_exit[pCam->nNumber] = 1;
        g_getYUV_loop_exit[pCam->nNumber] = 1;
        if (gDispatchThread[pCam->nNumber] != 0) {
            axRet = pthread_join(gDispatchThread[pCam->nNumber], NULL);
            if (axRet < 0) {
                COMM_CAM_PRT(" dispacher thread exit failed, ret=0x%x.\n", axRet);
            }
            gDispatchThread[pCam->nNumber] = 0;
            COMMON_QUEUE_Destroy(&payloadQue[pCam->nNumber]);
        }
        if (gDispatchThread[pCam->nNumber] != 0) {
            axRet = pthread_join(gGetYUVThread[pCam->nNumber], NULL);
            if (axRet < 0) {
                COMM_CAM_PRT(" get yuv thread exit failed, ret=0x%x.\n", axRet);
            }
            gGetYUVThread[pCam->nNumber] = 0;
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

AX_S32 CASE_LOW_DELAY_CAM_Open(AX_CAMERA_T *pCamList, AX_U8 Num)
{
    AX_U16 i = 0;
    if (pCamList == NULL) {
        return -1;
    }

    for (i = 0; i < Num; i++) {
        if (AX_SUCCESS == __case_low_delay_cam_open(&pCamList[i])) {
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
        __case_low_delay_cam_close(&pCamList[i]);
    }
    return -1;
}

AX_S32 CASE_LOW_DELAY_CAM_Close(AX_CAMERA_T *pCamList, AX_U8 Num)
{
    AX_U16 i = 0;
    if (pCamList == NULL) {
        return -1;
    }

    for (i = 0; i < Num; i++) {
        if (!pCamList[i].bOpen) {
            continue;
        }

        if (AX_SUCCESS == __case_low_delay_cam_close(&pCamList[i])) {
            COMM_CAM_PRT("camera %d is close\n", i);
            pCamList[i].bOpen = AX_FALSE;
        } else {
            return -1;
        }
    }

    return 0;
}

AX_S32 SAMPLE_CASE_VIN_LOW_DELAY_OV48C40(SAMPLE_VIN_PARAM_T *pVinParam)
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
    axRet = CASE_LOW_DELAY_CAM_Open(&gCams[0], tCommonArgs.nCamCnt);
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
    CASE_LOW_DELAY_CAM_Close(&gCams[0], tCommonArgs.nCamCnt);
EXIT_FAIL2:
    COMMON_CAM_Deinit();
EXIT_FAIL1:
    COMMON_SYS_DeInit();
EXIT_FAIL:
    return axRet;
}


static AX_S32 SysFrameDispatch(AX_U8 nDevId, AX_CAMERA_T *pCam, AX_SNS_HDR_MODE_E eHdrMode)
{
    AX_S32 axRet = 0;
    AX_S32 j = 0;
    AX_S32 cnt = 0;
    AX_S32 timeOutMs = 1000;
    AX_U32  nPipeId = 0;
    AX_U64 maxFrameSeq = 0;
    AX_IMG_INFO_T frameBufferArr[AX_SNS_HDR_FRAME_MAX] = {0};
    AX_VIN_DEV_BIND_PIPE_T *ptDevBindPipe = &pCam->tDevBindPipe;
    AX_U64 frameSeqs[AX_SNS_HDR_FRAME_MAX] = {0};

    AX_U32 FrmType = 0;
    AX_S32 queueIdx;
    AX_S32 flag = 0;
    queueIdx = pCam->nDevId;

    for (j = 0; j < eHdrMode; j++) {
        axRet = AX_VIN_GetDevFrame(nDevId, j, frameBufferArr + j, timeOutMs);
        if (axRet != 0) {
            if (AX_ERR_VIN_RES_EMPTY == axRet) {
                COMM_CAM_PRT("nonblock error, 0x%x\n", axRet);
                return axRet;
            }

            usleep(10 * 1000);
            AX_VIN_ReleaseDevFrame(nDevId, j, frameBufferArr + j);
            return axRet;
        }

        frameSeqs[j] = frameBufferArr[j].tFrameInfo.stVFrame.u64SeqNum;
        if (frameSeqs[j] > maxFrameSeq) {
            maxFrameSeq = frameSeqs[j];
        }
    }

    for (cnt = 0; cnt < ptDevBindPipe->nNum; cnt++) {
        if (pCam->bEnableFlash == AX_TRUE) {
            FrmType = frameBufferArr[0].tIspInfo.tFtcInfo.eFrmType;
            if (AX_ISP_FRAME_TYPE_FLASH_SNAP == FrmType) {
                if (pCam->tPipeInfo[cnt].ePipeMode == FrmType) {
                    axRet = COMMON_QUEUE_Push(&payloadQue[queueIdx], ptDevBindPipe->nPipeId[cnt], &frameBufferArr[0]);
                    if (!axRet) {
                        flag = 1;
                        break;
                    }
                }
            } else {
                if (pCam->tPipeInfo[cnt].ePipeMode == FrmType) {
                    nPipeId = ptDevBindPipe->nPipeId[cnt];
                }  else {
                    continue;
                }
                axRet = AX_VIN_SendRawFrame(ptDevBindPipe->nPipeId[cnt], AX_VIN_FRAME_SOURCE_ID_IFE, eHdrMode,
                                            (const AX_IMG_INFO_T **)&frameBufferArr, 0);
                if (axRet != 0) {
                    COMM_CAM_PRT("Send Pipe raw frame failed\n");
                }
            }
        } else {
            nPipeId = ptDevBindPipe->nPipeId[cnt];
            if (pCam->tPipeInfo[cnt].ePipeMode == SAMPLE_PIPE_MODE_FLASH_SNAP) {
                /* ITS flash lamp snap case */
                /* find the capture raw frame, this is just a sample. */

                if (frameBufferArr[0].tFrameInfo.stVFrame.u64SeqNum % 5 == 0) {
                    axRet = COMMON_QUEUE_Push(&payloadQue[queueIdx], nPipeId, &frameBufferArr[0]);
                    if (!axRet) {
                        flag = 1;
                    }
                }
            } else if ((pCam->tPipeInfo[cnt].ePipeMode == SAMPLE_PIPE_MODE_VIDEO) ||
                       (pCam->tPipeInfo[cnt].ePipeMode == SAMPLE_PIPE_MODE_PICTURE)) {
                axRet = AX_VIN_SendRawFrame(ptDevBindPipe->nPipeId[cnt], AX_VIN_FRAME_SOURCE_ID_IFE, eHdrMode,
                                            (const AX_IMG_INFO_T **)&frameBufferArr, 0);
                if (axRet != 0) {
                    COMM_CAM_PRT("Send Pipe raw frame failed\n");
                }
            }
        }
    }

    if (0 == flag) {
        for (j = 0; j < eHdrMode; j++) {
            AX_VIN_ReleaseDevFrame(nDevId, j, frameBufferArr + j);
        }
    }

    return 0;
}

static void *DispatchThread(void *args)
{
    AX_CHAR token[32] = {0};
    AX_CAMERA_T *pCam = (AX_CAMERA_T *)args;

    AX_U8 nDevId = pCam->nDevId;
    AX_SNS_HDR_MODE_E eHdrMode = pCam->eHdrMode;


    snprintf(token, 32, "RAW_DISP_%u", nDevId);
    prctl(PR_SET_NAME, token);

    while (!g_dispatcher_loop_exit[pCam->nNumber]) {
        SysFrameDispatch(nDevId, pCam, eHdrMode);
    }

    return NULL;
}

static void *GetYUVFrameThread(void *args)
{
    AX_CHAR token[32] = {0};
    AX_S32  axRet = 0;
    AX_CAMERA_T *pCam = (AX_CAMERA_T *)args;
    AX_U8 nPipeId = pCam->nPipeId;
    AX_S32 timeOutMs = 1000;
    AX_U64 nCurPTS = 0, nDeltaPTS = 0;
    AX_U64 nLastSeq = 0;

    AX_IMG_INFO_T tImgInfo = {0};

    AX_VIN_CHN_ID_E eChnId = AX_VIN_CHN_ID_MAIN;


    snprintf(token, 32, "GET_YUV_%u", nPipeId);
    prctl(PR_SET_NAME, token);

    while (!g_getYUV_loop_exit[pCam->nNumber]) {
        axRet = AX_VIN_GetYuvFrame(nPipeId, eChnId, &tImgInfo, timeOutMs);
        if (AX_SUCCESS != axRet) {
            printf("AX_VIN_GetYuvFrame fail, nRet:0x%x\n", axRet);
            continue;
        }

        axRet =  AX_VIN_GetChnIRQTimeOut(nPipeId, eChnId, 2, timeOutMs);
        if (AX_SUCCESS != axRet) {
            printf("AX_VIN_GetChnIRQTimeOut fail, nRet:0x%x\n", axRet);
            continue;
        }

        AX_SYS_GetCurPTS(&nCurPTS);

        nDeltaPTS = nCurPTS - tImgInfo.tFrameInfo.stVFrame.u64PTS;
        if (nDeltaPTS > 73000 || nDeltaPTS < 71500 || (tImgInfo.tFrameInfo.stVFrame.u64SeqNum - nLastSeq) > 1) {
            printf("YUV Frame nSeqNum:%lld, nLastSeq:%lld, nDeltaPTS:%lld\n",
                   tImgInfo.tFrameInfo.stVFrame.u64SeqNum, nLastSeq, nDeltaPTS);
        }
        nLastSeq = tImgInfo.tFrameInfo.stVFrame.u64SeqNum;
        axRet = AX_VIN_ReleaseYuvFrame(nPipeId, eChnId, &tImgInfo);
        if (AX_SUCCESS  != axRet) {
            printf("AX_VIN_ReleaseYuvFrame failed, axRet=0x%x\n", axRet);
        }

        usleep(1000);
    }

    return NULL;
}
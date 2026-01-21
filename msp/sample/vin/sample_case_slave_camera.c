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
#include <pthread.h>
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
#include "sample_case_uav.h"
#include "sample_case_lowdelay.h"
#include "vsync_ctrl.h"

// #define OVERLAP_MASK_UPDATE

/* comm pool */
COMMON_SYS_POOL_CFG_T gtSysCommPoolEightSc450aiSdr[] = {
    {2688, 1520, 2688, AX_FORMAT_YUV420_SEMIPLANAR, 12 * 8},   /* vin nv21/nv21 use */
};

COMMON_SYS_POOL_CFG_T gtSysCommPoolEightOs04a10Sdr[] = {
    {2688, 1520, 2688, AX_FORMAT_YUV420_SEMIPLANAR, 12 * 8},   /* vin nv21/nv21 use */
};

/* private pool */
COMMON_SYS_POOL_CFG_T gtPrivatePoolEightSc450aiSdr[] = {
    {2688, 1520, 2688, AX_FORMAT_BAYER_RAW_16BPP, 30 * 4},      /* vin raw16 use */
};

COMMON_SYS_POOL_CFG_T gtPrivatePoolEightOs04a10Sdr[] = {
    {2688, 1520, 2688, AX_FORMAT_BAYER_RAW_16BPP, 30 * 4},      /* vin raw16 use */
};

static AX_ISP_AF_REGFUNCS_T s_tAfFuncs = {0};
static AX_CAMERA_T gCams[MAX_CAMERAS] = {0};
static volatile AX_S32 gMainLoopExit = 0;

#ifdef OVERLAP_MASK_UPDATE
static pthread_t gUpdateMaskThread = {0};
static volatile AX_S32 gUpdateMaskLoopExit = {0};
#endif

static AX_VOID __sigint(int iSigNo)
{
    COMM_ISP_PRT("Catch signal %d\n", iSigNo);
    gMainLoopExit = 1;

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
/*
sensor 0 / 1   i2c 1  mclk 0
sensor 2 / 3   i2c 2  mclk 1
sensor 4 / 5   i2c 3  mclk 2
sensor 6 / 7   i2c 9  mclk 3
*/
static AX_U32 __sample_case_eight_sensor(AX_CAMERA_T *pCamList, SAMPLE_SNS_TYPE_E eSnsType,
        SAMPLE_VIN_PARAM_T *pVinParam, COMMON_SYS_ARGS_T *pCommonArgs)
{
    AX_S32 i = 0, j = 0;
    AX_CAMERA_T *pCam = NULL;
    COMMON_VIN_MODE_E eSysMode = pVinParam->eSysMode;
    AX_SNS_HDR_MODE_E eHdrMode = pVinParam->eHdrMode;
    AX_CHAR apd_plate_id[BOARD_ID_LEN] = {0};

    pCommonArgs->nCamCnt = 8;
    COMMON_SYS_GetApdPlateId(apd_plate_id);

    for (i = 0; i < pCommonArgs->nCamCnt; i++) {
        pCam = &pCamList[i];
        pCam->nPipeId = 0 + i;
        COMMON_VIN_GetSnsConfig(eSnsType, &pCam->tMipiRx, &pCam->tSnsAttr, &pCam->tSnsClkAttr, &pCam->tDevAttr);
        COMMON_VIN_GetPipeConfig(eSnsType, &pCam->tPipeAttr[pCam->nPipeId], pCam->tChnAttr[pCam->nPipeId],
                                 pCam->bChnEnable[pCam->nPipeId]);

        pCam->nDevId = 0 + i;
        if (i == 4) {
            pCam->nRxDev = 5;
        } else if (i == 5) {
            pCam->nRxDev = 4;
        } else if (i == 6) {
            pCam->nRxDev = 7;
        } else if (i == 7) {
            pCam->nRxDev = 6;
        } else {
            pCam->nRxDev = i;
        }

        pCam->tSnsClkAttr.nSnsClkIdx = 0 + i / 2; //mclk share
        pCam->tDevBindPipe.nNum =  1;
        pCam->tDevBindPipe.nPipeId[0] = pCam->nPipeId;
        pCam->ptSnsHdl[pCam->nPipeId] = COMMON_ISP_GetSnsObj(eSnsType);
        pCam->eBusType = COMMON_ISP_GetSnsBusType(eSnsType);
        __set_pipe_hdr_mode(&pCam->tDevBindPipe.nHDRSel[0], eHdrMode);
        __set_vin_attr(pCam, eSnsType, eHdrMode, eSysMode, pVinParam->bAiispEnable);
        pCam->tSnsAttr.eMasterSlaveSel = AX_SNS_SYNC_SLAVE;
        pCam->tSnsAttr.eRawType = AX_RT_RAW10;
        pCam->tSnsAttr.eSnsMode = AX_SNS_LINEAR_MODE;
        pCam->tDevAttr.ePixelFmt = AX_FORMAT_BAYER_RAW_10BPP;
        pCam->tDevAttr.eSnsOutputMode = AX_SNS_NORMAL;
        pCam->tPipeAttr[pCam->nPipeId].ePixelFmt = AX_FORMAT_BAYER_RAW_10BPP;
        for (j = 0; j < pCam->tDevBindPipe.nNum; j++) {
            pCam->tPipeInfo[j].eFrameSrcType = AX_VIN_FRAME_SOURCE_TYPE_DEV;
            pCam->tPipeInfo[j].ePipeMode = SAMPLE_PIPE_MODE_VIDEO;
            pCam->tPipeInfo[j].bAiispEnable = pVinParam->bAiispEnable;
            pCam->tPipeInfo[j].ePipeWorkMode = AX_VIN_PIPE_NORMAL_MODE0;
            if (pCam->tPipeInfo[j].bAiispEnable) {
                if (eHdrMode <= AX_SNS_LINEAR_MODE) {
                    strncpy(pCam->tPipeInfo[j].szBinPath, "/opt/etc/sc450ai_sdr_ai3d_t2dnr.bin", sizeof(pCam->tPipeInfo[j].szBinPath));
                } else {
                    strncpy(pCam->tPipeInfo[j].szBinPath, "/opt/etc/sc450ai_hdr_2x_ainr.bin", sizeof(pCam->tPipeInfo[j].szBinPath));
                }
            } else {
                strncpy(pCam->tPipeInfo[j].szBinPath, "null.bin", sizeof(pCam->tPipeInfo[j].szBinPath));
            }
            COMM_ISP_PRT("szBinPath: %s\n", pCam->tPipeInfo[j].szBinPath);
        }
    }

    return 0;
}

static AX_U32 __sample_case_config(SAMPLE_VIN_PARAM_T *pVinParam, COMMON_SYS_ARGS_T *pCommonArgs,
                                   COMMON_SYS_ARGS_T *pPrivArgs)
{
    AX_S32 axRet = 0;
    AX_CAMERA_T         *pCamList = &gCams[0];
    SAMPLE_SNS_TYPE_E   eSnsType = OMNIVISION_OS08A20;

    COMM_ISP_PRT("eSysCase %d, eSysMode %d, eHdrMode %d, bAiispEnable %d\n", pVinParam->eSysCase, pVinParam->eSysMode,
                 pVinParam->eHdrMode, pVinParam->bAiispEnable);

    switch (pVinParam->eSysCase) {
    case SAMPLE_VIN_EIGHT_SC450AI:
        eSnsType = SMARTSENS_SC450AI;
        /* comm pool config */
        __cal_dump_pool(gtSysCommPoolEightSc450aiSdr, pVinParam->eHdrMode, pVinParam->nDumpFrameNum);
        pCommonArgs->nPoolCfgCnt = sizeof(gtSysCommPoolEightSc450aiSdr) / sizeof(gtSysCommPoolEightSc450aiSdr[0]);
        pCommonArgs->pPoolCfg = gtSysCommPoolEightSc450aiSdr;

        /* private pool config */
        __cal_dump_pool(gtPrivatePoolEightSc450aiSdr, pVinParam->eHdrMode, pVinParam->nDumpFrameNum);
        pPrivArgs->nPoolCfgCnt = sizeof(gtPrivatePoolEightSc450aiSdr) / sizeof(gtPrivatePoolEightSc450aiSdr[0]);
        pPrivArgs->pPoolCfg = gtPrivatePoolEightSc450aiSdr;
        break;
    case SAMPLE_VIN_EIGHT_OS04A10:
        eSnsType = OMNIVISION_OS04A10_2LANE;
        /* comm pool config */
        __cal_dump_pool(gtSysCommPoolEightOs04a10Sdr, pVinParam->eHdrMode, pVinParam->nDumpFrameNum);
        pCommonArgs->nPoolCfgCnt = sizeof(gtSysCommPoolEightOs04a10Sdr) / sizeof(gtSysCommPoolEightOs04a10Sdr[0]);
        pCommonArgs->pPoolCfg = gtSysCommPoolEightOs04a10Sdr;

        /* private pool config */
        __cal_dump_pool(gtPrivatePoolEightOs04a10Sdr, pVinParam->eHdrMode, pVinParam->nDumpFrameNum);
        pPrivArgs->nPoolCfgCnt = sizeof(gtPrivatePoolEightOs04a10Sdr) / sizeof(gtPrivatePoolEightOs04a10Sdr[0]);
        pPrivArgs->pPoolCfg = gtPrivatePoolEightOs04a10Sdr;
        break;
    default:
        COMM_ISP_PRT("eSysCase %d unsupport\n", pVinParam->eSysCase);
        axRet = -1;
        break;
    }

    if (axRet) {
        return axRet;
    }

    /* cams config */
    __sample_case_eight_sensor(pCamList, eSnsType, pVinParam, pCommonArgs);

    return 0;
}


static AX_S32 __sample_case_isp_init(AX_U8 nPipeId, AX_SENSOR_REGISTER_FUNC_T *ptSnsHdl, AX_BOOL bRegister3A,
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

static AX_S32 __sample_case_cam_open(AX_CAMERA_T *pCam, AX_BOOL bMainPipe)
{
    AX_S32 i = 0;
    AX_S32 axRet = 0;
    AX_U8 nPipeId = 0;
    AX_U8 nDevId = pCam->nDevId;
    AX_U32 nRxDev = pCam->nRxDev;
    AX_VIN_SCHED_ATTR_T tSchedAttr = {0};

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

        if (bMainPipe) {
            tSchedAttr.ePolicy = AX_VIN_SCHED_POLICY_FIFO;
            tSchedAttr.nPriority = 0;
        } else {
            tSchedAttr.ePolicy = AX_VIN_SCHED_POLICY_FIFO;
            tSchedAttr.nPriority = 1;
        }
        axRet = AX_VIN_SetPipeSchedAttr(nPipeId, &tSchedAttr);
        if (0 != axRet) {
            COMM_CAM_PRT("AX_VIN_SetPipeSchedAttr failed, ret=0x%x.\n", axRet);
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

        axRet = __sample_case_isp_init(nPipeId, pCam->ptSnsHdl[pCam->nPipeId], pCam->bRegisterSns, pCam->bUser3a,
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

    return 0;
}


static AX_S32 __sample_case_cam_close(AX_CAMERA_T *pCam)
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

#ifdef OVERLAP_MASK_UPDATE
static AX_S32 UpdateMaskTask(AX_U8 *pStitchGrp)
{
    AX_S32 axRet = 0;
    AX_S32 i = 0;
    AX_VIN_STITCH_GRP_ATTR_T tStitchGrpAttrNew = {0};
    AX_VIN_STITCH_GRP_ATTR_T tStitchGrpAttr = {0};
    AX_U8 *pOverlapMask = AX_NULL;
    AX_U8 *pOverlapMaskLast = AX_NULL;


    axRet = AX_VIN_GetStitchGrpAttr(*pStitchGrp, &tStitchGrpAttr);
    if (axRet) {
        COMM_CAM_PRT("stitch grp %d get attr failed, ret:0x%x\n", *pStitchGrp, axRet);
        return axRet;
    }

    pOverlapMaskLast = tStitchGrpAttr.tStitchPipeAttr[i].tOverlapInfo[0].pOverlapMask;

    pOverlapMask = (AX_U8 *)calloc(1, 32 * 2160 / 8);
    memset(pOverlapMask, 0xFF, 32 * 2160 / 8);

    memcpy(&tStitchGrpAttrNew, &tStitchGrpAttr, sizeof(tStitchGrpAttr));
    for (i = 0; i < tStitchGrpAttrNew.nStitchPipeNum; i++) {
        tStitchGrpAttrNew.tStitchPipeAttr[i].tOverlapInfo[0].pOverlapMask = pOverlapMask;
        tStitchGrpAttrNew.tStitchPipeAttr[i].tOverlapInfo[1].pOverlapMask = pOverlapMask;
    }


    axRet = AX_VIN_SetStitchGrpAttr(*pStitchGrp, &tStitchGrpAttrNew);
    if (axRet) {
        COMM_CAM_PRT("stitch grp %d set attr failed, ret:0x%x\n", *pStitchGrp, axRet);
        goto FAIL;
    }

    free(pOverlapMaskLast);
    pOverlapMaskLast = AX_NULL;
    return axRet;

FAIL:
    free(pOverlapMask);
    pOverlapMask = AX_NULL;
    return axRet;
}

static void *UpdateMaskThread(void *args)
{
    AX_CHAR token[32] = {0};
    AX_U8 *stitch_grp = (AX_U8 *)args;


    snprintf(token, 32, "Update_Mask");
    prctl(PR_SET_NAME, token);

    while (!gUpdateMaskLoopExit) {
        UpdateMaskTask(stitch_grp);
        usleep(90 * 1000);
    }

    return NULL;
}
#endif

AX_S32 SAMPLE_CASE_EIGHT_CAM_Open(AX_CAMERA_T *pCamList, AX_U8 Num, AX_VIN_STITCH_GRP_ATTR_T *pStitchGrpAttr)
{
    AX_S32 axRet = 0;
    AX_U16 i = 0, j = 0;
    AX_BOOL bMainPipe = AX_FALSE;
    if (pCamList == NULL) {
        return -1;
    }

    for (i = 0; i < Num; i++) {
        bMainPipe = (pCamList[i].nPipeId == pStitchGrpAttr->nMainPipeId) ? AX_TRUE : AX_FALSE;
        if (AX_SUCCESS == __sample_case_cam_open(&pCamList[i], bMainPipe)) {
            pCamList[i].bOpen = AX_TRUE;
            COMM_CAM_PRT("camera %d is open\n", i);
        } else {
            goto EXIT;
        }
    }

    for (i = 0; i < Num; i++) {
        if (pCamList[i].eSysMode == COMMON_VIN_LOADRAW) {
            continue;
        }
        for (j = 0; j < pCamList[i].tDevBindPipe.nNum; j++) {
            if (AX_FALSE == pCamList[i].bRegisterSns) {
                continue;
            }
            axRet = AX_ISP_StreamOn(pCamList[i].tDevBindPipe.nPipeId[j]);
            if (0 != axRet) {
                COMM_CAM_PRT(" failed, ret=0x%x.\n", axRet);
                return -1;
            }
        }
    }
    return 0;
EXIT:
    for (i = 0; i < Num; i++) {
        if (!pCamList[i].bOpen) {
            continue;
        }
        __sample_case_cam_close(&pCamList[i]);
    }
    return -1;
}

AX_S32 SAMPLE_CASE_EIGHT_CAMERA_Close(AX_CAMERA_T *pCamList, AX_U8 Num)
{
    AX_U16 i = 0;
    if (pCamList == NULL) {
        return -1;
    }

    for (i = 0; i < Num; i++) {
        if (!pCamList[i].bOpen) {
            continue;
        }

        if (AX_SUCCESS == __sample_case_cam_close(&pCamList[i])) {
            COMM_CAM_PRT("camera %d is close\n", i);
            pCamList[i].bOpen = AX_FALSE;
        } else {
            return -1;
        }
    }

    return 0;
}

AX_S32 SAMPLE_CASE_EIGHT_CAMERA(SAMPLE_VIN_PARAM_T *pVinParam)
{
    AX_S32 axRet = 0;
    AX_S32 i = 0, j = 0;
    AX_U8 nStitchGrp = 0;
    AX_U8 *pOverlapMask = AX_NULL;
    COMMON_SYS_ARGS_T tCommonArgs = {0};
    AX_VIN_STITCH_GRP_ATTR_T tStitchGrpAttr = {0};
    COMMON_SYS_ARGS_T tPrivArgs = {0};

    signal(SIGINT, __sigint);

    /* Step1: cam config & pool Config */
    axRet = __sample_case_config(pVinParam, &tCommonArgs, &tPrivArgs);
    if (axRet) {
        COMM_ISP_PRT("__sample_case_config fail, ret:0x%x", axRet);
        return axRet;
    }

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
    tStitchGrpAttr.bAroundView = AX_FALSE;
    tStitchGrpAttr.bStitchEn = AX_TRUE;
    tStitchGrpAttr.nMainPipeId = 0;
    tStitchGrpAttr.nStitchPipeNum = 8;

    pOverlapMask = (AX_U8 *)calloc(1, 32 * 2160 / 8);
    memset(pOverlapMask, 0xFF, 32 * 2160 / 8);
    for (i = 0; i < tStitchGrpAttr.nStitchPipeNum; i++) {
        tStitchGrpAttr.tStitchPipeAttr[i].nPipeId = i;
        tStitchGrpAttr.tStitchPipeAttr[i].bMirrorFlip = 0;

        tStitchGrpAttr.tStitchPipeAttr[i].tOverlapInfo[0].tOverlapArea.nStartX = 0;      //0-3839
        tStitchGrpAttr.tStitchPipeAttr[i].tOverlapInfo[0].tOverlapArea.nStartY = 0;
        tStitchGrpAttr.tStitchPipeAttr[i].tOverlapInfo[0].tOverlapArea.nWidth = 32;
        tStitchGrpAttr.tStitchPipeAttr[i].tOverlapInfo[0].tOverlapArea.nHeight = 2160;
        tStitchGrpAttr.tStitchPipeAttr[i].tOverlapInfo[0].pOverlapMask = pOverlapMask;

        tStitchGrpAttr.tStitchPipeAttr[i].tOverlapInfo[1].tOverlapArea.nStartX = 3807;      //0-3839
        tStitchGrpAttr.tStitchPipeAttr[i].tOverlapInfo[1].tOverlapArea.nStartY = 0;
        tStitchGrpAttr.tStitchPipeAttr[i].tOverlapInfo[1].tOverlapArea.nWidth = 32;
        tStitchGrpAttr.tStitchPipeAttr[i].tOverlapInfo[1].tOverlapArea.nHeight = 2160;
        tStitchGrpAttr.tStitchPipeAttr[i].tOverlapInfo[1].pOverlapMask = pOverlapMask;
    }

    axRet = COMMON_CAM_StitchAttrInit(nStitchGrp, &tStitchGrpAttr);
    if (0 != axRet) {
        COMM_CAM_PRT("COMMON_CAM_StitchAttrInit failed, ret=0x%x.\n", axRet);
        goto EXIT_FAIL2;
    }
    axRet = COMMON_CAM_PrivPoolInit(&tPrivArgs);
    if (axRet) {
        COMM_ISP_PRT("COMMON_CAM_PrivPoolInit fail, ret:0x%x", axRet);
        goto EXIT_FAIL2;
    }
    /* Step5: Cam Open */
    axRet = SAMPLE_CASE_EIGHT_CAM_Open(&gCams[0], tCommonArgs.nCamCnt, &tStitchGrpAttr);
    if (axRet) {
        COMM_ISP_PRT("COMMON_CAM_Open fail, ret:0x%x", axRet);
        goto EXIT_FAIL2;
    }
    /*step6: vsync ctrl init*/
    /*vsync en after sensor stream on  for frame count equal*/
    axRet = ax_vsync_ctrl_init(25); //fps depends on sensor setting
    if (axRet) {
        COMM_ISP_PRT("vsync_ctrl_init fail, ret:0x%x", axRet);
        goto EXIT_FAIL3;
    }

    /*setp7 vsync en*/
    axRet = ax_vsync_ctrl_start();
    if (axRet) {
        COMM_ISP_PRT("vsync_ctrl_start fail, ret:0x%x", axRet);
        goto EXIT_FAIL4;
    }
    /* Step8. NT Init (tuning socket server. optional)
    Stream default port 6000, Ctrl default port 8082 */
    axRet = COMMON_NT_Init(6000, 8082);
    if (axRet) {
        COMM_ISP_PRT("COMMON_NT_Init fail, ret:0x%x", axRet);
        goto EXIT_FAIL4;
    }
    /* update pipe attribute */
    for (i = 0; i < tCommonArgs.nCamCnt; i++) {
        for (j = 0; j < gCams[i].tDevBindPipe.nNum; j++) {
            COMMON_NT_UpdateSource(gCams[i].tDevBindPipe.nPipeId[j]);
        }
    }

#ifdef OVERLAP_MASK_UPDATE
    pthread_create(&gUpdateMaskThread, NULL, UpdateMaskThread, (AX_VOID *)&nStitchGrp);
#endif

    while (!gMainLoopExit) {
        sleep(1);
    }

#ifdef OVERLAP_MASK_UPDATE
    gUpdateMaskLoopExit = 1;
    if (gUpdateMaskThread != 0) {
        axRet = pthread_join(gUpdateMaskThread, NULL);
        if (axRet < 0) {
            COMM_CAM_PRT(" update mask thread exit failed, ret=0x%x.\n", axRet);
        }
        gUpdateMaskThread = 0;
    }
#endif
    COMMON_NT_DeInit();
EXIT_FAIL4:
    ax_vsync_ctrl_stop();
EXIT_FAIL3:
    SAMPLE_CASE_EIGHT_CAMERA_Close(&gCams[0], tCommonArgs.nCamCnt);
EXIT_FAIL2:
#ifdef OVERLAP_MASK_UPDATE
    axRet = AX_VIN_GetStitchGrpAttr(nStitchGrp, &tStitchGrpAttr);
    if (axRet) {
        COMM_CAM_PRT("stitch grp %d get attr failed, ret:0x%x\n", nStitchGrp, axRet);
    }

    pOverlapMask = tStitchGrpAttr.tStitchPipeAttr[i].tOverlapInfo[0].pOverlapMask;
    if (pOverlapMask) {
        free(pOverlapMask);
        pOverlapMask = AX_NULL;
    }
#endif
    COMMON_CAM_Deinit();
EXIT_FAIL1:
    COMMON_SYS_DeInit();
EXIT_FAIL:
    return axRet;
}
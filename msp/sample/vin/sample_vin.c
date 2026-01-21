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
#include "sample_case_slave_camera.h"

/* comm pool */
COMMON_SYS_POOL_CFG_T gtSysCommPoolSingleDummySdr[] = {
    {3840, 2160, 3840, AX_FORMAT_YUV420_SEMIPLANAR, 10},    /* vin nv21/nv21 use */
};

COMMON_SYS_POOL_CFG_T gtSysCommPoolSingleOs08a20Sdr[] = {
    {3840, 2160, 3840, AX_FORMAT_YUV420_SEMIPLANAR, 12},    /* vin nv21/nv21 use */
};

COMMON_SYS_POOL_CFG_T gtSysCommPoolDoubleOs08a20Sdr[] = {
    {3840, 2160, 3840, AX_FORMAT_YUV420_SEMIPLANAR, 10 * 2},    /* vin nv21/nv21 use */
};

COMMON_SYS_POOL_CFG_T gtSysCommPoolDoubleOs08a20Hdr[] = {
    {3840, 2160, 3840, AX_FORMAT_YUV420_SEMIPLANAR, 50},        /* vin nv21/nv21 use */
};

COMMON_SYS_POOL_CFG_T gtSysCommPoolDoubleOs08a20MultiplePipeSdr[] = {
    {3840, 2160, 3840, AX_FORMAT_YUV420_SEMIPLANAR, 60},       /* vin nv21/nv21 use */
};

COMMON_SYS_POOL_CFG_T gtSysCommPoolDoubleOs08a20MultiplePipeHdr[] = {
    {3840, 2160, 3840, AX_FORMAT_YUV420_SEMIPLANAR, 60},       /* vin nv21/nv21 use */
};

COMMON_SYS_POOL_CFG_T gtSysCommPoolSingleSc910gsMultiplePipeSdr[] = {
    {3840, 2336, 3840, AX_FORMAT_YUV420_SEMIPLANAR, 80},       /* vin nv21/nv21 use */
};

COMMON_SYS_POOL_CFG_T gtSysCommPoolSingleSc410gsMultiplePipeSdr[] = {
    {1744, 2336, 1744, AX_FORMAT_YUV420_SEMIPLANAR, 80},       /* vin nv21/nv21 use */
};

COMMON_SYS_POOL_CFG_T gtSysCommPoolFourOs08a20Sdr[] = {
    {3840, 2160, 3840, AX_FORMAT_YUV420_SEMIPLANAR, 12 * 4},   /* vin nv21/nv21 use */
};

/* private pool */
COMMON_SYS_POOL_CFG_T gtPrivatePoolSingleDummySdr[] = {
    {3840, 2160, 3840, AX_FORMAT_BAYER_RAW_16BPP, 25},         /* vin raw16 use */
};

COMMON_SYS_POOL_CFG_T gtPrivatePoolSingleDummyUxeSdr[] = {
    {3840 * 3, 2160, 3840 * 3, AX_FORMAT_BAYER_RAW_8BPP, 25},   /* vin yuv420 use */
};

COMMON_SYS_POOL_CFG_T gtPrivatePoolSingleDummyUxeyuv420Sdr[] = {
    {3840, 2160, 3840, AX_FORMAT_YUV420_SEMIPLANAR, 25},        /* vin yuv420 use */
};

COMMON_SYS_POOL_CFG_T gtPrivatePoolSingleDummyUxeyuv420_10bitSdr[] = {
    {3840, 2160, 3840, AX_FORMAT_YUV420_SEMIPLANAR_10BIT_P010, 25},        /* vin yuv420 use */
};


COMMON_SYS_POOL_CFG_T gtPrivatePoolSingleOs08a20Sdr[] = {
    {3840, 2160, 3840, AX_FORMAT_BAYER_RAW_16BPP, 25 * 2},      /* vin raw16 use */
};

COMMON_SYS_POOL_CFG_T gtPrivatePoolDoubleOs08a20Sdr[] = {
    {3840, 2160, 3840, AX_FORMAT_BAYER_RAW_16BPP, 25 * 2},      /* vin raw16 use */
};

COMMON_SYS_POOL_CFG_T gtPrivatePoolDoubleOs08a20Hdr[] = {
    {3840, 2160, 3840, AX_FORMAT_BAYER_RAW_16BPP, 25 * 2},      /* vin raw16 use */
};

COMMON_SYS_POOL_CFG_T gtPrivatePoolDoubleOs08a20MultiplePipeSdr[] = {
    {3840, 2160, 3840, AX_FORMAT_BAYER_RAW_16BPP, 45},          /* vin raw16 use */
};

COMMON_SYS_POOL_CFG_T gtPrivatePoolDoubleOs08a20MultiplePipeHdr[] = {
    {3840, 2160, 3840, AX_FORMAT_BAYER_RAW_16BPP, 70},          /* vin raw16 use */
};

COMMON_SYS_POOL_CFG_T gtPrivatePoolSingleSc910gsMultiplePipeSdr[] = {
    {3840, 2336, 3840, AX_FORMAT_BAYER_RAW_16BPP, 45},          /* vin raw16 use */
};

COMMON_SYS_POOL_CFG_T gtPrivatePoolSingleSc410gsMultiplePipeSdr[] = {
    {1744, 2336, 1744, AX_FORMAT_BAYER_RAW_16BPP, 45},          /* vin raw16 use */
};

COMMON_SYS_POOL_CFG_T gtPrivatePoolFourOs08a20Sdr[] = {
    {3840, 2160, 3840, AX_FORMAT_BAYER_RAW_16BPP, 25 * 2},      /* vin raw16 use */
};

static AX_CAMERA_T gCams[MAX_CAMERAS] = {0};
static volatile AX_S32 gLoopExit = 0;

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

static AX_U32 __sample_case_single_dummy(AX_CAMERA_T *pCamList, SAMPLE_SNS_TYPE_E eSnsType,
        SAMPLE_VIN_PARAM_T *pVinParam, COMMON_SYS_ARGS_T *pCommonArgs)
{
    AX_S32 i = 0;
    AX_CAMERA_T *pCam = NULL;

    pCam = &pCamList[0];
    pCommonArgs->nCamCnt = 1;

    for (i = 0; i < pCommonArgs->nCamCnt; i++) {
        pCam = &pCamList[i];
        COMMON_VIN_GetSnsConfig(eSnsType, &pCam->tMipiRx, &pCam->tSnsAttr,
                                &pCam->tSnsClkAttr, &pCam->tDevAttr);
        COMMON_VIN_GetPipeConfig(eSnsType,
                                 &pCam->tPipeAttr[pCam->nPipeId], pCam->tChnAttr[pCam->nPipeId], pCam->bChnEnable[pCam->nPipeId]);
        pCam->ptSnsHdl[pCam->nPipeId] = COMMON_ISP_GetSnsObj(eSnsType);
        pCam->eBusType = COMMON_ISP_GetSnsBusType(eSnsType);
        for (AX_S32 j = 0; j < AX_VIN_MAX_PIPE_NUM; j++) {
            pCam->tPipeInfo[j].eFrameSrcType = AX_VIN_FRAME_SOURCE_TYPE_USER;
            pCam->tPipeInfo[j].ePipeMode = SAMPLE_PIPE_MODE_VIDEO;
            pCam->tPipeInfo[j].bAiispEnable = pVinParam->bAiispEnable;
            strncpy(pCam->tPipeInfo[j].szBinPath, "null.bin", sizeof(pCam->tPipeInfo[j].szBinPath));
        }
    }

    return 0;
}
static AX_U32 __sample_case_single_os08a20(AX_CAMERA_T *pCamList, SAMPLE_SNS_TYPE_E eSnsType,
        SAMPLE_VIN_PARAM_T *pVinParam, COMMON_SYS_ARGS_T *pCommonArgs)
{
    AX_CAMERA_T *pCam = NULL;
    COMMON_VIN_MODE_E eSysMode = pVinParam->eSysMode;
    AX_SNS_HDR_MODE_E eHdrMode = pVinParam->eHdrMode;
    AX_S32 j = 0;
    pCommonArgs->nCamCnt = 1;
    pCam = &pCamList[0];
    pCam->nDevId = 0;
    pCam->nRxDev = 0;
    pCam->nPipeId = 0;
    COMMON_VIN_GetSnsConfig(eSnsType, &pCam->tMipiRx, &pCam->tSnsAttr,
                            &pCam->tSnsClkAttr, &pCam->tDevAttr);
    COMMON_VIN_GetPipeConfig(eSnsType,
                             &pCam->tPipeAttr[pCam->nPipeId], pCam->tChnAttr[pCam->nPipeId], pCam->bChnEnable[pCam->nPipeId]);
    pCam->tSnsClkAttr.nSnsClkIdx = 0;
    pCam->tDevBindPipe.nNum =  1;
    pCam->tDevBindPipe.nPipeId[0] = pCam->nPipeId;
    pCam->ptSnsHdl[pCam->nPipeId] = COMMON_ISP_GetSnsObj(eSnsType);
    pCam->eBusType = COMMON_ISP_GetSnsBusType(eSnsType);
    __set_pipe_hdr_mode(&pCam->tDevBindPipe.nHDRSel[0], eHdrMode);
    __set_vin_attr(pCam, eSnsType, eHdrMode, eSysMode, pVinParam->bAiispEnable);
    for (j = 0; j < pCam->tDevBindPipe.nNum; j++) {
        pCam->tPipeInfo[j].eFrameSrcType = AX_VIN_FRAME_SOURCE_TYPE_USER;
        pCam->tPipeInfo[j].ePipeMode = SAMPLE_PIPE_MODE_VIDEO;
        pCam->tPipeInfo[j].bAiispEnable = pVinParam->bAiispEnable;
        if (pCam->tPipeInfo[j].bAiispEnable) {
            if (eHdrMode <= AX_SNS_LINEAR_MODE) {
                strncpy(pCam->tPipeInfo[j].szBinPath, "/opt/etc/os08a20_sdr_ai3d_t2dnr.bin", sizeof(pCam->tPipeInfo[j].szBinPath));
            } else {
                strncpy(pCam->tPipeInfo[j].szBinPath, "/opt/etc/os08a20_hdr_2x_ainr.bin", sizeof(pCam->tPipeInfo[j].szBinPath));
            }
        } else {
            strncpy(pCam->tPipeInfo[j].szBinPath, "null.bin", sizeof(pCam->tPipeInfo[j].szBinPath));
        }
    }
    return 0;
}

static AX_U32 __sample_case_double_os08a20(AX_CAMERA_T *pCamList, SAMPLE_SNS_TYPE_E eSnsType,
        SAMPLE_VIN_PARAM_T *pVinParam, COMMON_SYS_ARGS_T *pCommonArgs)
{
    AX_S32 i = 0, j = 0;
    AX_CAMERA_T *pCam = NULL;
    COMMON_VIN_MODE_E eSysMode = pVinParam->eSysMode;
    AX_SNS_HDR_MODE_E eHdrMode = pVinParam->eHdrMode;
    AX_CHAR apd_plate_id[BOARD_ID_LEN] = {0};

    pCommonArgs->nCamCnt = 2;
    COMMON_SYS_GetApdPlateId(apd_plate_id);

    for (i = 0; i < pCommonArgs->nCamCnt; i++) {
        pCam = &pCamList[i];
        pCam->nNumber = i;
        pCam->nPipeId = i;
        COMMON_VIN_GetSnsConfig(eSnsType, &pCam->tMipiRx, &pCam->tSnsAttr,
                                &pCam->tSnsClkAttr, &pCam->tDevAttr);
        COMMON_VIN_GetPipeConfig(eSnsType,
                                 &pCam->tPipeAttr[pCam->nPipeId], pCam->tChnAttr[pCam->nPipeId], pCam->bChnEnable[pCam->nPipeId]);
        if (i == 0) {
            pCam->nDevId = 0;
            pCam->nRxDev = 0;
            pCam->tSnsClkAttr.nSnsClkIdx = 0;
        } else if (i == 1) {
            if (!strncmp(apd_plate_id, "ADP_RX_DPHY_2X4LANE", sizeof("ADP_RX_DPHY_2X4LANE") - 1)) {
                if (!strncmp(apd_plate_id, "ADP_RX_DPHY_2X4LANE_N", sizeof("ADP_RX_DPHY_2X4LANE_N") - 1)) {
                    pCam->nDevId = 2;
                    pCam->nRxDev = 2;
                    pCam->tSnsClkAttr.nSnsClkIdx = 1;
                } else {
                    pCam->nDevId = 4;
                    pCam->nRxDev = 4;
                    pCam->tSnsClkAttr.nSnsClkIdx = 1;
                }
            }
        }

        pCam->tDevBindPipe.nNum =  1;
        pCam->tDevBindPipe.nPipeId[0] = pCam->nPipeId;
        pCam->ptSnsHdl[pCam->nPipeId] = COMMON_ISP_GetSnsObj(eSnsType);
        pCam->eBusType = COMMON_ISP_GetSnsBusType(eSnsType);
        __set_pipe_hdr_mode(&pCam->tDevBindPipe.nHDRSel[0], eHdrMode);
        __set_vin_attr(pCam, eSnsType, eHdrMode, eSysMode, pVinParam->bAiispEnable);
        for (j = 0; j < pCam->tDevBindPipe.nNum; j++) {
            pCam->tPipeInfo[j].eFrameSrcType = AX_VIN_FRAME_SOURCE_TYPE_USER;
            pCam->tPipeInfo[j].ePipeMode = SAMPLE_PIPE_MODE_VIDEO;
            pCam->tPipeInfo[j].bAiispEnable = pVinParam->bAiispEnable;
            if (pCam->tPipeInfo[j].bAiispEnable) {
                if (eHdrMode <= AX_SNS_LINEAR_MODE) {
                    strncpy(pCam->tPipeInfo[j].szBinPath, "/opt/etc/os08a20_sdr_ai3d_t2dnr.bin", sizeof(pCam->tPipeInfo[j].szBinPath));
                } else {
                    strncpy(pCam->tPipeInfo[j].szBinPath, "/opt/etc/os08a20_hdr_2x_ainr.bin", sizeof(pCam->tPipeInfo[j].szBinPath));
                }
            } else {
                strncpy(pCam->tPipeInfo[j].szBinPath, "null.bin", sizeof(pCam->tPipeInfo[j].szBinPath));
            }
            printf("j=%d szBinPath[%s]\n", j, pCam->tPipeInfo[j].szBinPath);
        }
    }

    return 0;
}

static AX_U32 __sample_case_double_os08a20_multiple_pipe(AX_CAMERA_T *pCamList, SAMPLE_SNS_TYPE_E eSnsType,
        SAMPLE_VIN_PARAM_T *pVinParam, COMMON_SYS_ARGS_T *pCommonArgs)
{
    AX_S32 i = 0, j = 0;
    AX_CAMERA_T *pCam = NULL;
    COMMON_VIN_MODE_E eSysMode = pVinParam->eSysMode;
    AX_SNS_HDR_MODE_E eHdrMode = pVinParam->eHdrMode;
    AX_CHAR apd_plate_id[BOARD_ID_LEN] = {0};
    SAMPLE_PIPE_INFO_T *pSamplePipeInfo = AX_NULL;

    SAMPLE_PIPE_INFO_T tSamplePipeInfo_sdr[] = {
        {AX_VIN_PIPE_NORMAL_MODE0, SAMPLE_PIPE_MODE_VIDEO, AX_TRUE,  "/opt/etc/os08a20_sdr_ai2dnr.bin"},      /* ai2dnr + t2dnr */
        {AX_VIN_PIPE_NORMAL_MODE0, SAMPLE_PIPE_MODE_VIDEO, AX_TRUE,  "/opt/etc/os08a20_sdr_ai3d_t2dnr.bin"},    /* ai3dnr + t3dnr */
        {AX_VIN_PIPE_NORMAL_MODE0, SAMPLE_PIPE_MODE_VIDEO, AX_FALSE, "null.bin"},                             /* t3dnr */
        {AX_VIN_PIPE_NORMAL_MODE0, SAMPLE_PIPE_MODE_VIDEO, AX_FALSE, "/opt/etc/os08a20_sdr_t2dnr.bin"},       /* t2dnr */
        {AX_VIN_PIPE_NORMAL_MODE0, SAMPLE_PIPE_MODE_VIDEO, AX_TRUE,  "/opt/etc/os08a20_sdr_ai3d_t2dnr.bin"},  /* ai3dnr + t2dnr */
        {AX_VIN_PIPE_NORMAL_MODE0, SAMPLE_PIPE_MODE_VIDEO, AX_TRUE,  "/opt/etc/os08a20_sdr_ai3d-t2d_to_t3dnr.bin"},  /* ai3d_t2dnr to t3dnr */
    };
    SAMPLE_PIPE_INFO_T tSamplePipeInfo_hdr_2x[] = {
        {AX_VIN_PIPE_NORMAL_MODE0, SAMPLE_PIPE_MODE_VIDEO, AX_FALSE,  "null.bin"},                                  /* t3dnr */
        {AX_VIN_PIPE_NORMAL_MODE0, SAMPLE_PIPE_MODE_VIDEO, AX_TRUE,  "/opt/etc/os08a20_hdr_2x_aice.bin"},          /* ai3dnr + t2dnr */
        {AX_VIN_PIPE_NORMAL_MODE0, SAMPLE_PIPE_MODE_VIDEO, AX_TRUE,  "/opt/etc/os08a20_hdr_2x_aice_to_ainr.bin"},  /* aice to ainr */
        {AX_VIN_PIPE_NORMAL_MODE0, SAMPLE_PIPE_MODE_VIDEO, AX_TRUE,  "/opt/etc/os08a20_hdr_2x_ainr.bin"},          /* ai3dnr + t2dnr */
        {AX_VIN_PIPE_NORMAL_MODE0, SAMPLE_PIPE_MODE_VIDEO, AX_TRUE,  "/opt/etc/os08a20_hdr_2x_aice.bin"},          /* aice + t2dnr */
        {AX_VIN_PIPE_NORMAL_MODE0, SAMPLE_PIPE_MODE_VIDEO, AX_FALSE, "null.bin"},                                  /* t2dnr */
    };

    if (eHdrMode == AX_SNS_LINEAR_MODE) {
        pSamplePipeInfo = &tSamplePipeInfo_sdr[0];
    } else {
        pSamplePipeInfo = &tSamplePipeInfo_hdr_2x[0];
    }

    pCommonArgs->nCamCnt = 2;
    COMMON_SYS_GetApdPlateId(apd_plate_id);

    for (i = 0; i < pCommonArgs->nCamCnt; i++) {
        pCam = &pCamList[i];
        pCam->nNumber = i;
        pCam->nPipeId = i;

        COMMON_VIN_GetSnsConfig(eSnsType, &pCam->tMipiRx, &pCam->tSnsAttr,
                                &pCam->tSnsClkAttr, &pCam->tDevAttr);

        if (i == 0) {
            pCam->nDevId = 0;
            pCam->nRxDev = 0;
            pCam->tSnsClkAttr.nSnsClkIdx = 0;

            pCam->tDevBindPipe.nNum =  3;
            pCam->tDevBindPipe.nPipeId[0] = 0;
            pCam->tDevBindPipe.nPipeId[1] = 1;
            pCam->tDevBindPipe.nPipeId[2] = 2;
        } else if (i == 1) {
            if (!strncmp(apd_plate_id, "ADP_RX_DPHY_2X4LANE", sizeof("ADP_RX_DPHY_2X4LANE") - 1)) {
                if (!strncmp(apd_plate_id, "ADP_RX_DPHY_2X4LANE_N", sizeof("ADP_RX_DPHY_2X4LANE_N") - 1)) {
                    pCam->nDevId = 2;
                    pCam->nRxDev = 2;
                    pCam->tSnsClkAttr.nSnsClkIdx = 1;
                } else {
                    pCam->nDevId = 4;
                    pCam->nRxDev = 4;
                    pCam->tSnsClkAttr.nSnsClkIdx = 1;
                }
            }

            pCam->tDevBindPipe.nNum =  3;
            pCam->tDevBindPipe.nPipeId[0] = 3;
            pCam->tDevBindPipe.nPipeId[1] = 4;
            pCam->tDevBindPipe.nPipeId[2] = 5;
        }
        pCam->ptSnsHdl[pCam->nPipeId] = COMMON_ISP_GetSnsObj(eSnsType);
        pCam->eBusType = COMMON_ISP_GetSnsBusType(eSnsType);
        __set_pipe_hdr_mode(&pCam->tDevBindPipe.nHDRSel[0], eHdrMode);
        __set_pipe_hdr_mode(&pCam->tDevBindPipe.nHDRSel[1], eHdrMode);
        __set_pipe_hdr_mode(&pCam->tDevBindPipe.nHDRSel[2], eHdrMode);
        for (j = 0; j < pCam->tDevBindPipe.nNum; j++) {
            pCam->tPipeInfo[j].eFrameSrcType = AX_VIN_FRAME_SOURCE_TYPE_USER;
            pCam->nPipeId = pCam->tDevBindPipe.nPipeId[j];
            COMMON_VIN_GetPipeConfig(eSnsType,
                                     &pCam->tPipeAttr[pCam->nPipeId], pCam->tChnAttr[pCam->nPipeId], pCam->bChnEnable[pCam->nPipeId]);
            __set_vin_attr(pCam, eSnsType, eHdrMode, eSysMode, pVinParam->bAiispEnable);

            pCam->tPipeInfo[j].ePipeMode = (pSamplePipeInfo + pCam->nPipeId)->ePipeMode;
            pCam->tPipeInfo[j].bAiispEnable = (pSamplePipeInfo + pCam->nPipeId)->bAiispEnable;
            pCam->tPipeInfo[j].ePipeWorkMode = (pSamplePipeInfo + pCam->nPipeId)->ePipeWorkMode;
            strncpy(pCam->tPipeInfo[j].szBinPath, (pSamplePipeInfo + pCam->nPipeId)->szBinPath,
                    sizeof(pCam->tPipeInfo[j].szBinPath));
        }
        pCam->nPipeId = i;
    }

    return 0;
}

static AX_U32 __sample_case_single_os08a20_its_capture(AX_CAMERA_T *pCamList, SAMPLE_SNS_TYPE_E eSnsType,
        SAMPLE_VIN_PARAM_T *pVinParam, COMMON_SYS_ARGS_T *pCommonArgs)
{
    AX_S32 i = 0, j = 0;
    AX_CAMERA_T *pCam = NULL;
    COMMON_VIN_MODE_E eSysMode = pVinParam->eSysMode;
    AX_SNS_HDR_MODE_E eHdrMode = pVinParam->eHdrMode;
    pCommonArgs->nCamCnt = 1;
    SAMPLE_PIPE_INFO_T tSamplePipeInfo[] = {
        {AX_VIN_PIPE_NORMAL_MODE0, SAMPLE_PIPE_MODE_VIDEO, AX_FALSE, "null.bin"},
        {AX_VIN_PIPE_NORMAL_MODE0, SAMPLE_PIPE_MODE_PICTURE, AX_FALSE, "/opt/etc/os08a20_sdr_t2dnr.bin"},
        {AX_VIN_PIPE_NORMAL_MODE2, SAMPLE_PIPE_MODE_FLASH_SNAP, AX_TRUE, "/opt/etc/os08a20_sdr_ai2dnr_offline_1024G_isp1_10bit.bin"},
    };

    for (i = 0; i < pCommonArgs->nCamCnt; i++) {
        pCam = &pCamList[i];
        COMMON_VIN_GetSnsConfig(eSnsType, &pCam->tMipiRx, &pCam->tSnsAttr,
                                &pCam->tSnsClkAttr, &pCam->tDevAttr);

        if (i == 0) {
            pCam->nDevId = 0;
            pCam->nRxDev = 0;
            pCam->tSnsClkAttr.nSnsClkIdx = 0;

            pCam->tDevBindPipe.nNum =  3;
            pCam->tDevBindPipe.nPipeId[0] = 0;
            pCam->tDevBindPipe.nPipeId[1] = 1;
            pCam->tDevBindPipe.nPipeId[2] = 2;
        }
        pCam->ptSnsHdl[pCam->nPipeId] = COMMON_ISP_GetSnsObj(eSnsType);
        pCam->eBusType = COMMON_ISP_GetSnsBusType(eSnsType);
        __set_pipe_hdr_mode(&pCam->tDevBindPipe.nHDRSel[0], eHdrMode);
        __set_pipe_hdr_mode(&pCam->tDevBindPipe.nHDRSel[1], eHdrMode);
        __set_pipe_hdr_mode(&pCam->tDevBindPipe.nHDRSel[2], eHdrMode);
        for (j = 0; j < pCam->tDevBindPipe.nNum; j++) {
            pCam->nPipeId = pCam->tDevBindPipe.nPipeId[j];
            COMMON_VIN_GetPipeConfig(eSnsType,
                                     &pCam->tPipeAttr[pCam->nPipeId], pCam->tChnAttr[pCam->nPipeId], pCam->bChnEnable[pCam->nPipeId]);
            __set_vin_attr(pCam, eSnsType, eHdrMode, eSysMode, pVinParam->bAiispEnable);

            pCam->tPipeInfo[j].eFrameSrcType = AX_VIN_FRAME_SOURCE_TYPE_USER;
            pCam->tPipeInfo[j].ePipeMode = tSamplePipeInfo[j].ePipeMode;
            pCam->tPipeInfo[j].bAiispEnable = tSamplePipeInfo[j].bAiispEnable;
            pCam->tPipeInfo[j].ePipeWorkMode = tSamplePipeInfo[j].ePipeWorkMode;

            strncpy(pCam->tPipeInfo[j].szBinPath, tSamplePipeInfo[j].szBinPath, sizeof(pCam->tPipeInfo[j].szBinPath));
        }
        pCam->nPipeId = i;
    }
    return 0;
}

static AX_U32 __sample_case_single_uxe_capture(AX_CAMERA_T *pCamList, SAMPLE_SNS_TYPE_E eSnsType,
        SAMPLE_VIN_PARAM_T *pVinParam, COMMON_SYS_ARGS_T *pCommonArgs)
{
    AX_CAMERA_T *pCam = NULL;
    COMMON_VIN_MODE_E eSysMode = pVinParam->eSysMode;
    AX_SNS_HDR_MODE_E eHdrMode = pVinParam->eHdrMode;
    pCommonArgs->nCamCnt = 1;


    pCam = &pCamList[0];
    COMMON_VIN_GetSnsConfig(eSnsType, &pCam->tMipiRx, &pCam->tSnsAttr,
                            &pCam->tSnsClkAttr, &pCam->tDevAttr);
    COMMON_VIN_GetPipeConfig(eSnsType,
                             &pCam->tPipeAttr[pCam->nPipeId], pCam->tChnAttr[pCam->nPipeId], pCam->bChnEnable[pCam->nPipeId]);
    /*yuv422*/
    pCam->tMipiRx.tMipiAttr.nDataRate = 1900;
    pCam->tMipiRx.tMipiAttr.eLaneNum = 8;
    pCam->tDevAttr.eSnsIntfType = AX_SNS_INTF_TYPE_MIPI_RAW;
    pCam->tDevAttr.ePixelFmt = AX_FORMAT_BAYER_RAW_8BPP;

    pCam->bDevOnly = AX_TRUE;
    pCam->nDevId = 0;
    pCam->nRxDev = 0;
    pCam->nPipeId = 0;
    pCam->tSnsClkAttr.nSnsClkIdx = 0;
    pCam->tSnsClkAttr.eSnsClkRate = AX_SNS_CLK_24M;
    pCam->tDevBindPipe.nNum =  1;
    pCam->tDevBindPipe.nPipeId[0] = pCam->nPipeId;
    pCam->ptSnsHdl[pCam->nPipeId] = COMMON_ISP_GetSnsObj(eSnsType);
    pCam->eBusType = COMMON_ISP_GetSnsBusType(eSnsType);
    __set_pipe_hdr_mode(&pCam->tDevBindPipe.nHDRSel[0], eHdrMode);
    __set_vin_attr(pCam, eSnsType, eHdrMode, eSysMode, pVinParam->bAiispEnable);

    pCam->tSnsAttr.eRawType = AX_RT_RAW8;
    pCam->tDevAttr.ePixelFmt = AX_FORMAT_BAYER_RAW_8BPP;
    pCam->tDevAttr.eDevWorkMode = AX_VIN_DEV_WORK_MODE_4MULTIPLEX;      /* hot tip: config 4 ROI */

    /* ROI config */
    pCam->tDevAttr.tDevImgRgn[0].nStartX = 0;
    pCam->tDevAttr.tDevImgRgn[0].nStartY = 0;
    pCam->tDevAttr.tDevImgRgn[0].nWidth = 3840 * 3 / 2;
    pCam->tDevAttr.tDevImgRgn[0].nHeight = 2160 / 2;
    pCam->tDevAttr.nWidthStride[0] = 3840 * 3;

    pCam->tDevAttr.tDevImgRgn[1].nStartX = 3840 * 3 / 2;
    pCam->tDevAttr.tDevImgRgn[1].nStartY = 0;
    pCam->tDevAttr.tDevImgRgn[1].nWidth = 3840 * 3 / 2;
    pCam->tDevAttr.tDevImgRgn[1].nHeight = 2160 /  2;
    pCam->tDevAttr.nWidthStride[1] = 3840 * 3;


    pCam->tDevAttr.tDevImgRgn[2].nStartX = 0;
    pCam->tDevAttr.tDevImgRgn[2].nStartY = 2160 / 2;
    pCam->tDevAttr.tDevImgRgn[2].nWidth = 3840 * 3 / 2;
    pCam->tDevAttr.tDevImgRgn[2].nHeight = 2160 / 2;
    pCam->tDevAttr.nWidthStride[2] = 3840 * 3;

    pCam->tDevAttr.tDevImgRgn[3].nStartX = 3840 * 3 / 2;
    pCam->tDevAttr.tDevImgRgn[3].nStartY = 2160 / 2;
    pCam->tDevAttr.tDevImgRgn[3].nWidth = 3840 * 3 / 2;
    pCam->tDevAttr.tDevImgRgn[3].nHeight = 2160 / 2;
    pCam->tDevAttr.nWidthStride[3] = 3840 * 3;

    /* configure the attribute of early reporting of frame interrupts */
    pCam->tDevFrmIntAttr.bImgRgnIntEn[0] = AX_FALSE;
    pCam->tDevFrmIntAttr.bImgRgnIntEn[1] = AX_FALSE;
    pCam->tDevFrmIntAttr.bImgRgnIntEn[2] = AX_FALSE;
    pCam->tDevFrmIntAttr.bImgRgnIntEn[3] = AX_TRUE;


    pCam->bRegisterSns = AX_FALSE;

    return 0;
}

static AX_U32 __sample_case_single_uxe_capture_yuv420(AX_CAMERA_T *pCamList, SAMPLE_SNS_TYPE_E eSnsType,
        SAMPLE_VIN_PARAM_T *pVinParam, COMMON_SYS_ARGS_T *pCommonArgs)
{

    AX_CAMERA_T *pCam = NULL;
    COMMON_VIN_MODE_E eSysMode = pVinParam->eSysMode;
    AX_SNS_HDR_MODE_E eHdrMode = pVinParam->eHdrMode;
    pCommonArgs->nCamCnt = 1;
    pCam = &pCamList[0];
    pCam->nPipeId = 0;
    COMMON_VIN_GetSnsConfig(eSnsType, &pCam->tMipiRx, &pCam->tSnsAttr,
                            &pCam->tSnsClkAttr, &pCam->tDevAttr);
    COMMON_VIN_GetPipeConfig(eSnsType,
                             &pCam->tPipeAttr[pCam->nPipeId], pCam->tChnAttr[pCam->nPipeId], pCam->bChnEnable[pCam->nPipeId]);
    /*yuv422*/
    pCam->tMipiRx.tMipiAttr.nDataRate = 1300;
    pCam->tDevAttr.eSnsIntfType = AX_SNS_INTF_TYPE_MIPI_YUV;
    pCam->tDevAttr.ePixelFmt = AX_FORMAT_YUV422_INTERLEAVED_YUYV;
    pCam->tDevAttr.eDevWorkMode = AX_VIN_DEV_WORK_MODE_1MULTIPLEX;

    pCam->bDevOnly = AX_TRUE;
    pCam->nDevId = 4;
    pCam->nRxDev = 4;
    pCam->tSnsClkAttr.nSnsClkIdx = 1;
    pCam->tSnsClkAttr.eSnsClkRate = AX_SNS_CLK_24M;
    pCam->tDevBindPipe.nNum =  1;
    pCam->tDevBindPipe.nPipeId[0] = pCam->nPipeId;
    pCam->ptSnsHdl[pCam->nPipeId] = COMMON_ISP_GetSnsObj(eSnsType);
    pCam->eBusType = COMMON_ISP_GetSnsBusType(eSnsType);
    __set_pipe_hdr_mode(&pCam->tDevBindPipe.nHDRSel[0], eHdrMode);
    __set_vin_attr(pCam, eSnsType, eHdrMode, eSysMode, pVinParam->bAiispEnable);

    pCam->tSnsAttr.eRawType = AX_RT_YUV422;
    pCam->tDevAttr.ePixelFmt = AX_FORMAT_YUV422_INTERLEAVED_YUYV;
    pCam->bRegisterSns = AX_FALSE;

    /*yuv422 to yuv420(nv12)*/
    pCam->tDevAttr.nConvYuv422To420En = 1;
    pCam->tDevAttr.nConvFactor = 2;

    return 0;
}

static AX_U32 __sample_case_single_uxe_capture_yuv420_4multi(AX_CAMERA_T *pCamList, SAMPLE_SNS_TYPE_E eSnsType,
        SAMPLE_VIN_PARAM_T *pVinParam, COMMON_SYS_ARGS_T *pCommonArgs)
{

    AX_CAMERA_T *pCam = NULL;
    COMMON_VIN_MODE_E eSysMode = pVinParam->eSysMode;
    AX_SNS_HDR_MODE_E eHdrMode = pVinParam->eHdrMode;
    pCommonArgs->nCamCnt = 1;
    pCam = &pCamList[0];
    pCam->nPipeId = 0;
    COMMON_VIN_GetSnsConfig(eSnsType, &pCam->tMipiRx, &pCam->tSnsAttr,
                            &pCam->tSnsClkAttr, &pCam->tDevAttr);
    COMMON_VIN_GetPipeConfig(eSnsType,
                             &pCam->tPipeAttr[pCam->nPipeId], pCam->tChnAttr[pCam->nPipeId], pCam->bChnEnable[pCam->nPipeId]);
    /*yuv422*/
    pCam->tMipiRx.tMipiAttr.nDataRate = 1300;
    pCam->tDevAttr.eSnsIntfType = AX_SNS_INTF_TYPE_MIPI_YUV;
    pCam->tDevAttr.ePixelFmt = AX_FORMAT_YUV422_INTERLEAVED_YUYV;
    pCam->tDevAttr.eDevWorkMode = AX_VIN_DEV_WORK_MODE_4MULTIPLEX;

    pCam->bDevOnly = AX_TRUE;
    pCam->nDevId = 4;
    pCam->nRxDev = 4;
    pCam->tSnsClkAttr.nSnsClkIdx = 1;
    pCam->tSnsClkAttr.eSnsClkRate = AX_SNS_CLK_24M;
    pCam->tDevBindPipe.nNum =  1;
    pCam->tDevBindPipe.nPipeId[0] = pCam->nPipeId;
    pCam->ptSnsHdl[pCam->nPipeId] = COMMON_ISP_GetSnsObj(eSnsType);
    pCam->eBusType = COMMON_ISP_GetSnsBusType(eSnsType);
    __set_pipe_hdr_mode(&pCam->tDevBindPipe.nHDRSel[0], eHdrMode);
    __set_vin_attr(pCam, eSnsType, eHdrMode, eSysMode, pVinParam->bAiispEnable);

    pCam->tSnsAttr.eRawType = AX_RT_YUV422;
    pCam->tDevAttr.ePixelFmt = AX_FORMAT_YUV422_INTERLEAVED_YUYV;
    pCam->bRegisterSns = AX_FALSE;

    /* ROI config */
    pCam->tDevAttr.tDevImgRgn[0].nStartX = 0;
    pCam->tDevAttr.tDevImgRgn[0].nStartY = 0;
    pCam->tDevAttr.tDevImgRgn[0].nWidth = 3840;
    pCam->tDevAttr.tDevImgRgn[0].nHeight = 2160 / 4;
    pCam->tDevAttr.nWidthStride[0] = 3840;


    pCam->tDevAttr.tDevImgRgn[1].nStartX = 0;
    pCam->tDevAttr.tDevImgRgn[1].nStartY = 2160 / 4 * 1;
    pCam->tDevAttr.tDevImgRgn[1].nWidth = 3840;
    pCam->tDevAttr.tDevImgRgn[1].nHeight = 2160 / 4;
    pCam->tDevAttr.nWidthStride[1] = 3840;


    pCam->tDevAttr.tDevImgRgn[2].nStartX = 0;
    pCam->tDevAttr.tDevImgRgn[2].nStartY = 2160 / 4 * 2;
    pCam->tDevAttr.tDevImgRgn[2].nWidth = 3840;
    pCam->tDevAttr.tDevImgRgn[2].nHeight = 2160 / 4;
    pCam->tDevAttr.nWidthStride[2] = 3840;


    pCam->tDevAttr.tDevImgRgn[3].nStartX = 0;
    pCam->tDevAttr.tDevImgRgn[3].nStartY = 2160 / 4 * 3;
    pCam->tDevAttr.tDevImgRgn[3].nWidth = 3840;
    pCam->tDevAttr.tDevImgRgn[3].nHeight = 2160 / 4;
    pCam->tDevAttr.nWidthStride[3] = 3840;


    /*yuv422 to yuv420(nv12)*/
    pCam->tDevAttr.nConvYuv422To420En = 1;
    pCam->tDevAttr.nConvFactor = 2;

    /* configure the attribute of early reporting of frame interrupts */
    pCam->tDevFrmIntAttr.bImgRgnIntEn[0] = AX_TRUE;
    pCam->tDevFrmIntAttr.bImgRgnIntEn[1] = AX_TRUE;
    pCam->tDevFrmIntAttr.bImgRgnIntEn[2] = AX_TRUE;
    pCam->tDevFrmIntAttr.bImgRgnIntEn[3] = AX_TRUE;

    return 0;
}

static AX_U32 __sample_case_single_uxe_capture_yuv420_10bit_4multi(AX_CAMERA_T *pCamList, SAMPLE_SNS_TYPE_E eSnsType,
        SAMPLE_VIN_PARAM_T *pVinParam, COMMON_SYS_ARGS_T *pCommonArgs)
{

    AX_CAMERA_T *pCam = NULL;
    COMMON_VIN_MODE_E eSysMode = pVinParam->eSysMode;
    AX_SNS_HDR_MODE_E eHdrMode = pVinParam->eHdrMode;
    pCommonArgs->nCamCnt = 1;
    pCam = &pCamList[0];
    pCam->nPipeId = 0;
    COMMON_VIN_GetSnsConfig(eSnsType, &pCam->tMipiRx, &pCam->tSnsAttr,
                            &pCam->tSnsClkAttr, &pCam->tDevAttr);
    COMMON_VIN_GetPipeConfig(eSnsType,
                             &pCam->tPipeAttr[pCam->nPipeId], pCam->tChnAttr[pCam->nPipeId], pCam->bChnEnable[pCam->nPipeId]);
    /*yuv422*/
    pCam->tMipiRx.tMipiAttr.nDataRate = 1600;
    pCam->tMipiRx.tMipiAttr.eLaneNum = AX_MIPI_DATA_LANE_8;
    pCam->tMipiRx.tMipiAttr.nDataLaneMap[4] = 4;
    pCam->tMipiRx.tMipiAttr.nDataLaneMap[5] = 5;
    pCam->tMipiRx.tMipiAttr.nDataLaneMap[6] = 6;
    pCam->tMipiRx.tMipiAttr.nDataLaneMap[7] = 7;
    pCam->tDevAttr.tMipiIntfAttr.szImgDt[0] = 0x1f;
    pCam->tDevAttr.tMipiIntfAttr.szImgDt[1] = 0x1f;
    pCam->tDevAttr.tMipiIntfAttr.szImgDt[2] = 0x1f;
    pCam->tDevAttr.tMipiIntfAttr.szImgDt[3] = 0x1f;
    pCam->tDevAttr.eSnsIntfType = AX_SNS_INTF_TYPE_MIPI_YUV;
    pCam->tDevAttr.ePixelFmt = AX_FORMAT_YUV422_SEMIPLANAR_10BIT_P010;
    pCam->tDevAttr.eDevWorkMode = AX_VIN_DEV_WORK_MODE_4MULTIPLEX;

    pCam->bDevOnly = AX_TRUE;
    pCam->nDevId = 4;
    pCam->nRxDev = 4;
    pCam->tSnsClkAttr.nSnsClkIdx = 1;
    pCam->tSnsClkAttr.eSnsClkRate = AX_SNS_CLK_24M;
    pCam->tDevBindPipe.nNum =  1;
    pCam->tDevBindPipe.nPipeId[0] = pCam->nPipeId;
    pCam->ptSnsHdl[pCam->nPipeId] = COMMON_ISP_GetSnsObj(eSnsType);
    pCam->eBusType = COMMON_ISP_GetSnsBusType(eSnsType);
    __set_pipe_hdr_mode(&pCam->tDevBindPipe.nHDRSel[0], eHdrMode);
    __set_vin_attr(pCam, eSnsType, eHdrMode, eSysMode, pVinParam->bAiispEnable);

    pCam->tSnsAttr.eRawType = AX_RT_YUV422;
    pCam->tDevAttr.ePixelFmt = AX_FORMAT_YUV422_SEMIPLANAR_10BIT_P010;
    pCam->bRegisterSns = AX_FALSE;

    /* ROI config */
    pCam->tDevAttr.tDevImgRgn[0].nStartX = 0;
    pCam->tDevAttr.tDevImgRgn[0].nStartY = 0;
    pCam->tDevAttr.tDevImgRgn[0].nWidth = 3840;
    pCam->tDevAttr.tDevImgRgn[0].nHeight = 2160 / 4;
    pCam->tDevAttr.nWidthStride[0] =  AX_VIN_GetImgStrideByte(pCam->tDevAttr.tDevImgRgn[0].nWidth, pCam->tDevAttr.ePixelFmt,
                                      128);

    pCam->tDevAttr.tDevImgRgn[1].nStartX = 0;
    pCam->tDevAttr.tDevImgRgn[1].nStartY = 2160 / 4 * 1;
    pCam->tDevAttr.tDevImgRgn[1].nWidth = 3840;
    pCam->tDevAttr.tDevImgRgn[1].nHeight = 2160 / 4;
    pCam->tDevAttr.nWidthStride[1] = AX_VIN_GetImgStrideByte(pCam->tDevAttr.tDevImgRgn[1].nWidth, pCam->tDevAttr.ePixelFmt,
                                     128);


    pCam->tDevAttr.tDevImgRgn[2].nStartX = 0;
    pCam->tDevAttr.tDevImgRgn[2].nStartY = 2160 / 4 * 2;
    pCam->tDevAttr.tDevImgRgn[2].nWidth = 3840;
    pCam->tDevAttr.tDevImgRgn[2].nHeight = 2160 / 4;
    pCam->tDevAttr.nWidthStride[2] = AX_VIN_GetImgStrideByte(pCam->tDevAttr.tDevImgRgn[2].nWidth, pCam->tDevAttr.ePixelFmt,
                                     128);


    pCam->tDevAttr.tDevImgRgn[3].nStartX = 0;
    pCam->tDevAttr.tDevImgRgn[3].nStartY = 2160 / 4 * 3;
    pCam->tDevAttr.tDevImgRgn[3].nWidth = 3840;
    pCam->tDevAttr.tDevImgRgn[3].nHeight = 2160 / 4;
    pCam->tDevAttr.nWidthStride[3] = AX_VIN_GetImgStrideByte(pCam->tDevAttr.tDevImgRgn[3].nWidth, pCam->tDevAttr.ePixelFmt,
                                     128);

    /*yuv422 to yuv420(nv12)*/
    pCam->tDevAttr.nConvYuv422To420En = 1;
    pCam->tDevAttr.nConvFactor = 2;

    /* configure the attribute of early reporting of frame interrupts */
    pCam->tDevFrmIntAttr.bImgRgnIntEn[0] = AX_TRUE;
    pCam->tDevFrmIntAttr.bImgRgnIntEn[1] = AX_TRUE;
    pCam->tDevFrmIntAttr.bImgRgnIntEn[2] = AX_TRUE;
    pCam->tDevFrmIntAttr.bImgRgnIntEn[3] = AX_TRUE;

    return 0;
}


static AX_U32 __sample_case_single_sc910gs_its_capture(AX_CAMERA_T *pCamList, SAMPLE_SNS_TYPE_E eSnsType,
        SAMPLE_VIN_PARAM_T *pVinParam, COMMON_SYS_ARGS_T *pCommonArgs)
{

    AX_S32 i = 0, j = 0;
    AX_CAMERA_T *pCam = NULL;
    COMMON_VIN_MODE_E eSysMode = pVinParam->eSysMode;
    AX_SNS_HDR_MODE_E eHdrMode = pVinParam->eHdrMode;
    pCommonArgs->nCamCnt = 1;
    SAMPLE_PIPE_INFO_T tSamplePipeInfo[] = {
        {AX_VIN_PIPE_NORMAL_MODE0, SAMPLE_PIPE_MODE_VIDEO, AX_FALSE, "null.bin"},
        {AX_VIN_PIPE_NORMAL_MODE0, SAMPLE_PIPE_MODE_PICTURE, AX_FALSE, "/opt/etc/sc910gs_sdr_t2dnr.bin"},
        {AX_VIN_PIPE_NORMAL_MODE2, SAMPLE_PIPE_MODE_FLASH_SNAP, AX_TRUE, "/opt/etc/sc910gs_sdr_ai2dnr_offline.bin"},
    };

    for (i = 0; i < pCommonArgs->nCamCnt; i++) {
        pCam = &pCamList[i];
        COMMON_VIN_GetSnsConfig(eSnsType, &pCam->tMipiRx, &pCam->tSnsAttr,
                                &pCam->tSnsClkAttr, &pCam->tDevAttr);

        pCam->bEnableFlash = AX_TRUE;
        COMMON_VIN_GetOutsideConfig(eSnsType, &pCam->tSensorVts, &pCam->tPowerAttr,
                                    &pCam->tVsyncAttr, &pCam->tHsyncAttr,
                                    &pCam->tLightSyncInfo, &pCam->tSnapStrobeAttr,
                                    &pCam->tSnapFlashAttr);

        if (i == 0) {
            pCam->nDevId = 0;
            pCam->nRxDev = 0;
            pCam->tSnsClkAttr.nSnsClkIdx = 0;

            pCam->tDevBindPipe.nNum =  3;
            pCam->tDevBindPipe.nPipeId[0] = 0;
            pCam->tDevBindPipe.nPipeId[1] = 1;
            pCam->tDevBindPipe.nPipeId[2] = 2;
        }
        pCam->ptSnsHdl[pCam->nPipeId] = COMMON_ISP_GetSnsObj(eSnsType);
        pCam->eBusType = COMMON_ISP_GetSnsBusType(eSnsType);
        __set_pipe_hdr_mode(&pCam->tDevBindPipe.nHDRSel[0], eHdrMode);
        __set_pipe_hdr_mode(&pCam->tDevBindPipe.nHDRSel[1], eHdrMode);
        __set_pipe_hdr_mode(&pCam->tDevBindPipe.nHDRSel[2], eHdrMode);

        for (j = 0; j < pCam->tDevBindPipe.nNum; j++) {
            pCam->nPipeId = pCam->tDevBindPipe.nPipeId[j];
            COMMON_VIN_GetPipeConfig(eSnsType,
                                     &pCam->tPipeAttr[pCam->nPipeId], pCam->tChnAttr[pCam->nPipeId], pCam->bChnEnable[pCam->nPipeId]);
            __set_vin_attr(pCam, eSnsType, eHdrMode, eSysMode, pVinParam->bAiispEnable);
            pCam->tSnsAttr.eRawType = AX_RT_RAW10;
            pCam->tDevAttr.ePixelFmt = AX_FORMAT_BAYER_RAW_10BPP;
            pCam->tPipeAttr[pCam->nPipeId].ePixelFmt = AX_FORMAT_BAYER_RAW_10BPP;
            pCam->tPipeInfo[j].eFrameSrcType = AX_VIN_FRAME_SOURCE_TYPE_USER;
            pCam->tPipeInfo[j].ePipeMode = tSamplePipeInfo[j].ePipeMode;
            pCam->tPipeInfo[j].bAiispEnable = tSamplePipeInfo[j].bAiispEnable;
            pCam->tPipeInfo[j].ePipeWorkMode = tSamplePipeInfo[j].ePipeWorkMode;
            strncpy(pCam->tPipeInfo[j].szBinPath, tSamplePipeInfo[j].szBinPath, sizeof(pCam->tPipeInfo[j].szBinPath));
        }
        pCam->nPipeId = i;
    }

    return 0;
}

static AX_U32 __sample_case_single_sc410gs_its_capture(AX_CAMERA_T *pCamList, SAMPLE_SNS_TYPE_E eSnsType,
        SAMPLE_VIN_PARAM_T *pVinParam, COMMON_SYS_ARGS_T *pCommonArgs)
{

    AX_S32 i = 0, j = 0;
    AX_CAMERA_T *pCam = NULL;
    COMMON_VIN_MODE_E eSysMode = pVinParam->eSysMode;
    AX_SNS_HDR_MODE_E eHdrMode = pVinParam->eHdrMode;
    pCommonArgs->nCamCnt = 1;
    SAMPLE_PIPE_INFO_T tSamplePipeInfo[] = {
        {AX_VIN_PIPE_NORMAL_MODE0, SAMPLE_PIPE_MODE_VIDEO, AX_FALSE, "null.bin"},
        {AX_VIN_PIPE_NORMAL_MODE0, SAMPLE_PIPE_MODE_PICTURE, AX_FALSE, "/opt/etc/sc410gs_sdr_t2dnr.bin"},
        {AX_VIN_PIPE_NORMAL_MODE2, SAMPLE_PIPE_MODE_FLASH_SNAP, AX_TRUE, "/opt/etc/sc410gs_sdr_ai2d_offline.bin"},
    };

    for (i = 0; i < pCommonArgs->nCamCnt; i++) {
        pCam = &pCamList[i];
        COMMON_VIN_GetSnsConfig(eSnsType, &pCam->tMipiRx, &pCam->tSnsAttr,
                                &pCam->tSnsClkAttr, &pCam->tDevAttr);
        pCam->bEnableFlash = AX_TRUE;
        COMMON_VIN_GetOutsideConfig(eSnsType, &pCam->tSensorVts, &pCam->tPowerAttr,
                                    &pCam->tVsyncAttr, &pCam->tHsyncAttr,
                                    &pCam->tLightSyncInfo, &pCam->tSnapStrobeAttr,
                                    &pCam->tSnapFlashAttr);

        if (i == 0) {
            pCam->nDevId = 0;
            pCam->nRxDev = 0;
            pCam->tSnsClkAttr.nSnsClkIdx = 0;

            pCam->tDevBindPipe.nNum =  3;
            pCam->tDevBindPipe.nPipeId[0] = 0;
            pCam->tDevBindPipe.nPipeId[1] = 1;
            pCam->tDevBindPipe.nPipeId[2] = 2;
        }
        pCam->ptSnsHdl[pCam->nPipeId] = COMMON_ISP_GetSnsObj(eSnsType);
        pCam->eBusType = COMMON_ISP_GetSnsBusType(eSnsType);
        __set_pipe_hdr_mode(&pCam->tDevBindPipe.nHDRSel[0], eHdrMode);
        __set_pipe_hdr_mode(&pCam->tDevBindPipe.nHDRSel[1], eHdrMode);
        __set_pipe_hdr_mode(&pCam->tDevBindPipe.nHDRSel[2], eHdrMode);

        for (j = 0; j < pCam->tDevBindPipe.nNum; j++) {
            pCam->nPipeId = pCam->tDevBindPipe.nPipeId[j];
            COMMON_VIN_GetPipeConfig(eSnsType,
                                     &pCam->tPipeAttr[pCam->nPipeId], pCam->tChnAttr[pCam->nPipeId], pCam->bChnEnable[pCam->nPipeId]);
            __set_vin_attr(pCam, eSnsType, eHdrMode, eSysMode, pVinParam->bAiispEnable);
            pCam->tSnsAttr.eRawType = AX_RT_RAW10;
            pCam->tDevAttr.ePixelFmt = AX_FORMAT_BAYER_RAW_10BPP;
            pCam->tPipeAttr[pCam->nPipeId].ePixelFmt = AX_FORMAT_BAYER_RAW_10BPP;

            pCam->tPipeInfo[j].eFrameSrcType = AX_VIN_FRAME_SOURCE_TYPE_USER;
            pCam->tPipeInfo[j].ePipeMode = tSamplePipeInfo[j].ePipeMode;
            pCam->tPipeInfo[j].bAiispEnable = tSamplePipeInfo[j].bAiispEnable;
            pCam->tPipeInfo[j].ePipeWorkMode = tSamplePipeInfo[j].ePipeWorkMode;
            strncpy(pCam->tPipeInfo[j].szBinPath, tSamplePipeInfo[j].szBinPath, sizeof(pCam->tPipeInfo[j].szBinPath));
        }
        pCam->nPipeId = i;
    }

    return 0;
}

static AX_U32 __sample_case_four_os08a20(AX_CAMERA_T *pCamList, SAMPLE_SNS_TYPE_E eSnsType,
        SAMPLE_VIN_PARAM_T *pVinParam, COMMON_SYS_ARGS_T *pCommonArgs)
{
    AX_S32 i = 0, j = 0;
    AX_CAMERA_T *pCam = NULL;
    COMMON_VIN_MODE_E eSysMode = pVinParam->eSysMode;
    AX_SNS_HDR_MODE_E eHdrMode = pVinParam->eHdrMode;
    AX_CHAR apd_plate_id[BOARD_ID_LEN] = {0};

    pCommonArgs->nCamCnt = 4;
    COMMON_SYS_GetApdPlateId(apd_plate_id);

    for (i = 0; i < pCommonArgs->nCamCnt; i++) {
        pCam = &pCamList[i];
        pCam->nPipeId = 0 + i;
        COMMON_VIN_GetSnsConfig(eSnsType, &pCam->tMipiRx, &pCam->tSnsAttr,
                                &pCam->tSnsClkAttr, &pCam->tDevAttr);
        COMMON_VIN_GetPipeConfig(eSnsType,
                                 &pCam->tPipeAttr[pCam->nPipeId], pCam->tChnAttr[pCam->nPipeId], pCam->bChnEnable[pCam->nPipeId]);

        pCam->nDevId = 0 + (i * 2);
        pCam->nRxDev = 0 + (i * 2);
        pCam->tSnsClkAttr.nSnsClkIdx = 0 + (i / 2);

        pCam->tDevBindPipe.nNum =  1;
        pCam->tDevBindPipe.nPipeId[0] = pCam->nPipeId;
        pCam->ptSnsHdl[pCam->nPipeId] = COMMON_ISP_GetSnsObj(eSnsType);
        pCam->eBusType = COMMON_ISP_GetSnsBusType(eSnsType);
        __set_pipe_hdr_mode(&pCam->tDevBindPipe.nHDRSel[0], eHdrMode);
        __set_vin_attr(pCam, eSnsType, eHdrMode, eSysMode, pVinParam->bAiispEnable);
        for (j = 0; j < pCam->tDevBindPipe.nNum; j++) {
            pCam->tPipeInfo[j].eFrameSrcType = AX_VIN_FRAME_SOURCE_TYPE_USER;
            pCam->tPipeInfo[j].ePipeMode = SAMPLE_PIPE_MODE_VIDEO;
            pCam->tPipeInfo[j].bAiispEnable = pVinParam->bAiispEnable;
            if (pCam->tPipeInfo[j].bAiispEnable) {
                if (eHdrMode <= AX_SNS_LINEAR_MODE) {
                    strncpy(pCam->tPipeInfo[j].szBinPath, "/opt/etc/os08a20_sdr_ai3d_t2dnr.bin", sizeof(pCam->tPipeInfo[j].szBinPath));
                } else {
                    strncpy(pCam->tPipeInfo[j].szBinPath, "/opt/etc/os08a20_hdr_2x_ainr.bin", sizeof(pCam->tPipeInfo[j].szBinPath));
                }
            } else {
                strncpy(pCam->tPipeInfo[j].szBinPath, "null.bin", sizeof(pCam->tPipeInfo[j].szBinPath));
            }
        }
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
    case SAMPLE_VIN_SINGLE_OS08A20:
        eSnsType = OMNIVISION_OS08A20;
        /* comm pool config */
        __cal_dump_pool(gtSysCommPoolSingleOs08a20Sdr, pVinParam->eHdrMode, pVinParam->nDumpFrameNum);
        pCommonArgs->nPoolCfgCnt = sizeof(gtSysCommPoolSingleOs08a20Sdr) / sizeof(gtSysCommPoolSingleOs08a20Sdr[0]);
        pCommonArgs->pPoolCfg = gtSysCommPoolSingleOs08a20Sdr;

        /* private pool config */
        __cal_dump_pool(gtPrivatePoolSingleOs08a20Sdr, pVinParam->eHdrMode, pVinParam->nDumpFrameNum);
        pPrivArgs->nPoolCfgCnt = sizeof(gtPrivatePoolSingleOs08a20Sdr) / sizeof(gtPrivatePoolSingleOs08a20Sdr[0]);
        pPrivArgs->pPoolCfg = gtPrivatePoolSingleOs08a20Sdr;

        /* cams config */
        __sample_case_single_os08a20(pCamList, eSnsType, pVinParam, pCommonArgs);
        break;
    case SAMPLE_VIN_DOUBLE_OS08A20:
        eSnsType = OMNIVISION_OS08A20;
        /* pool config */
        if (AX_SNS_LINEAR_MODE == pVinParam->eHdrMode) {
            __cal_dump_pool(gtSysCommPoolDoubleOs08a20Sdr, pVinParam->eHdrMode, pVinParam->nDumpFrameNum);
            pCommonArgs->nPoolCfgCnt = sizeof(gtSysCommPoolDoubleOs08a20Sdr) / sizeof(gtSysCommPoolDoubleOs08a20Sdr[0]);
            pCommonArgs->pPoolCfg  = gtSysCommPoolDoubleOs08a20Sdr;
        } else {
            __cal_dump_pool(gtSysCommPoolDoubleOs08a20Hdr, pVinParam->eHdrMode, pVinParam->nDumpFrameNum);
            pCommonArgs->nPoolCfgCnt = sizeof(gtSysCommPoolDoubleOs08a20Hdr) / sizeof(gtSysCommPoolDoubleOs08a20Hdr[0]);
            pCommonArgs->pPoolCfg  = gtSysCommPoolDoubleOs08a20Hdr;
        }

        /* private pool config */
        if (AX_SNS_LINEAR_MODE == pVinParam->eHdrMode) {
            __cal_dump_pool(gtPrivatePoolDoubleOs08a20Sdr, pVinParam->eHdrMode, pVinParam->nDumpFrameNum);
            pPrivArgs->nPoolCfgCnt = sizeof(gtPrivatePoolDoubleOs08a20Sdr) / sizeof(gtPrivatePoolDoubleOs08a20Sdr[0]);
            pPrivArgs->pPoolCfg  = gtPrivatePoolDoubleOs08a20Sdr;
        } else {
            __cal_dump_pool(gtPrivatePoolDoubleOs08a20Hdr, pVinParam->eHdrMode, pVinParam->nDumpFrameNum);
            pPrivArgs->nPoolCfgCnt = sizeof(gtPrivatePoolDoubleOs08a20Hdr) / sizeof(gtPrivatePoolDoubleOs08a20Hdr[0]);
            pPrivArgs->pPoolCfg  = gtPrivatePoolDoubleOs08a20Hdr;
        }

        /* cams config */
        __sample_case_double_os08a20(pCamList, eSnsType, pVinParam, pCommonArgs);
        break;
    case SAMPLE_VIN_DOUBLE_OS08A20_MULTIPLE_PIPE:
        eSnsType = OMNIVISION_OS08A20;
        /* pool config */
        if (AX_SNS_LINEAR_MODE == pVinParam->eHdrMode) {
            __cal_dump_pool(gtSysCommPoolDoubleOs08a20MultiplePipeSdr, pVinParam->eHdrMode, pVinParam->nDumpFrameNum);
            pCommonArgs->nPoolCfgCnt = sizeof(gtSysCommPoolDoubleOs08a20MultiplePipeSdr) /
                                       sizeof(gtSysCommPoolDoubleOs08a20MultiplePipeSdr[0]);
            pCommonArgs->pPoolCfg  = gtSysCommPoolDoubleOs08a20MultiplePipeSdr;
        } else {
            __cal_dump_pool(gtSysCommPoolDoubleOs08a20MultiplePipeHdr, pVinParam->eHdrMode, pVinParam->nDumpFrameNum);
            pCommonArgs->nPoolCfgCnt = sizeof(gtSysCommPoolDoubleOs08a20MultiplePipeHdr) /
                                       sizeof(gtSysCommPoolDoubleOs08a20MultiplePipeHdr[0]);
            pCommonArgs->pPoolCfg  = gtSysCommPoolDoubleOs08a20MultiplePipeHdr;
        }

        /* private pool config */
        if (AX_SNS_LINEAR_MODE == pVinParam->eHdrMode) {
            __cal_dump_pool(gtPrivatePoolDoubleOs08a20MultiplePipeSdr, pVinParam->eHdrMode, pVinParam->nDumpFrameNum);
            pPrivArgs->nPoolCfgCnt = sizeof(gtPrivatePoolDoubleOs08a20MultiplePipeSdr) /
                                     sizeof(gtPrivatePoolDoubleOs08a20MultiplePipeSdr[0]);
            pPrivArgs->pPoolCfg  = gtPrivatePoolDoubleOs08a20MultiplePipeSdr;
        } else {
            __cal_dump_pool(gtPrivatePoolDoubleOs08a20MultiplePipeHdr, pVinParam->eHdrMode, pVinParam->nDumpFrameNum);
            pPrivArgs->nPoolCfgCnt = sizeof(gtPrivatePoolDoubleOs08a20MultiplePipeHdr) /
                                     sizeof(gtPrivatePoolDoubleOs08a20MultiplePipeHdr[0]);
            pPrivArgs->pPoolCfg = gtPrivatePoolDoubleOs08a20MultiplePipeHdr;
        }

        /* cams config */
        __sample_case_double_os08a20_multiple_pipe(pCamList, eSnsType, pVinParam, pCommonArgs);
        break;
    case SAMPLE_VIN_SINGLE_OS08A20_ITS_CAPTURE:
        eSnsType = OMNIVISION_OS08A20;
        /* pool config */
        if (AX_SNS_LINEAR_MODE == pVinParam->eHdrMode) {
            __cal_dump_pool(gtSysCommPoolDoubleOs08a20MultiplePipeSdr, pVinParam->eHdrMode, pVinParam->nDumpFrameNum);
            pCommonArgs->nPoolCfgCnt = sizeof(gtSysCommPoolDoubleOs08a20MultiplePipeSdr) /
                                       sizeof(gtSysCommPoolDoubleOs08a20MultiplePipeSdr[0]);
            pCommonArgs->pPoolCfg = gtSysCommPoolDoubleOs08a20MultiplePipeSdr;
        }

        /* private pool config */
        if (AX_SNS_LINEAR_MODE == pVinParam->eHdrMode) {
            __cal_dump_pool(gtPrivatePoolDoubleOs08a20MultiplePipeSdr, pVinParam->eHdrMode, pVinParam->nDumpFrameNum);
            pPrivArgs->nPoolCfgCnt = sizeof(gtPrivatePoolDoubleOs08a20MultiplePipeSdr) /
                                     sizeof(gtPrivatePoolDoubleOs08a20MultiplePipeSdr[0]);
            pPrivArgs->pPoolCfg = gtPrivatePoolDoubleOs08a20MultiplePipeSdr;
        }

        /* cams config */
        __sample_case_single_os08a20_its_capture(pCamList, eSnsType, pVinParam, pCommonArgs);
        break;
    case SAMPLE_VIN_SINGLE_RGB888:
        eSnsType = SAMPLE_SNS_DUMMY;

        /* private pool config */
        __cal_dump_pool(gtPrivatePoolSingleDummyUxeSdr, pVinParam->eHdrMode, pVinParam->nDumpFrameNum);
        pPrivArgs->nPoolCfgCnt = sizeof(gtPrivatePoolSingleDummyUxeSdr) / sizeof(gtPrivatePoolSingleDummyUxeSdr[0]);
        pPrivArgs->pPoolCfg = gtPrivatePoolSingleDummyUxeSdr;

        /* cams config */
        __sample_case_single_uxe_capture(pCamList, eSnsType, pVinParam, pCommonArgs);
        break;
    case SAMPLE_VIN_SINGLE_YUV422:
        eSnsType = SAMPLE_SNS_DUMMY;

        /* private pool config */
        __cal_dump_pool(gtPrivatePoolSingleDummyUxeyuv420Sdr, pVinParam->eHdrMode, pVinParam->nDumpFrameNum);
        pPrivArgs->nPoolCfgCnt = sizeof(gtPrivatePoolSingleDummyUxeyuv420Sdr) / sizeof(gtPrivatePoolSingleDummyUxeyuv420Sdr[0]);
        pPrivArgs->pPoolCfg = gtPrivatePoolSingleDummyUxeyuv420Sdr;

        /* cams config */
        __sample_case_single_uxe_capture_yuv420(pCamList, eSnsType, pVinParam, pCommonArgs);
        break;
    case SAMPLE_VIN_SINGLE_YUV422_4MULTI:
        eSnsType = SAMPLE_SNS_DUMMY;

        /* private pool config */
        __cal_dump_pool(gtPrivatePoolSingleDummyUxeyuv420Sdr, pVinParam->eHdrMode, pVinParam->nDumpFrameNum);
        pPrivArgs->nPoolCfgCnt = sizeof(gtPrivatePoolSingleDummyUxeyuv420Sdr) / sizeof(gtPrivatePoolSingleDummyUxeyuv420Sdr[0]);
        pPrivArgs->pPoolCfg = gtPrivatePoolSingleDummyUxeyuv420Sdr;

        /* cams config */
        __sample_case_single_uxe_capture_yuv420_4multi(pCamList, eSnsType, pVinParam, pCommonArgs);
        break;
    case SAMPLE_VIN_SINGLE_YUV422_10BIT_4MULTI:
        eSnsType = SAMPLE_SNS_DUMMY;

        /* private pool config */
        __cal_dump_pool(gtPrivatePoolSingleDummyUxeyuv420_10bitSdr, pVinParam->eHdrMode, pVinParam->nDumpFrameNum);
        pPrivArgs->nPoolCfgCnt = sizeof(gtPrivatePoolSingleDummyUxeyuv420_10bitSdr) / sizeof(
                                     gtPrivatePoolSingleDummyUxeyuv420_10bitSdr[0]);
        pPrivArgs->pPoolCfg = gtPrivatePoolSingleDummyUxeyuv420_10bitSdr;

        /* cams config */
        __sample_case_single_uxe_capture_yuv420_10bit_4multi(pCamList, eSnsType, pVinParam, pCommonArgs);
        break;
    case SAMPLE_VIN_FOUR_OS08A20:
        eSnsType = OMNIVISION_OS08A20;
        /* comm pool config */
        __cal_dump_pool(gtSysCommPoolFourOs08a20Sdr, pVinParam->eHdrMode, pVinParam->nDumpFrameNum);
        pCommonArgs->nPoolCfgCnt = sizeof(gtSysCommPoolFourOs08a20Sdr) / sizeof(gtSysCommPoolFourOs08a20Sdr[0]);
        pCommonArgs->pPoolCfg = gtSysCommPoolFourOs08a20Sdr;

        /* private pool config */
        __cal_dump_pool(gtPrivatePoolFourOs08a20Sdr, pVinParam->eHdrMode, pVinParam->nDumpFrameNum);
        pPrivArgs->nPoolCfgCnt = sizeof(gtPrivatePoolFourOs08a20Sdr) / sizeof(gtPrivatePoolFourOs08a20Sdr[0]);
        pPrivArgs->pPoolCfg = gtPrivatePoolFourOs08a20Sdr;

        /* cams config */
        __sample_case_four_os08a20(pCamList, eSnsType, pVinParam, pCommonArgs);
        break;
    case SAMPLE_VIN_SINGLE_SC910GS_ITS_CAPTURE:
        eSnsType = SMARTSENS_SC910GS;
        /* pool config */
        if (AX_SNS_LINEAR_MODE == pVinParam->eHdrMode) {
            __cal_dump_pool(gtSysCommPoolSingleSc910gsMultiplePipeSdr, pVinParam->eHdrMode, pVinParam->nDumpFrameNum);
            pCommonArgs->nPoolCfgCnt = sizeof(gtSysCommPoolSingleSc910gsMultiplePipeSdr) /
                                       sizeof(gtSysCommPoolSingleSc910gsMultiplePipeSdr[0]);
            pCommonArgs->pPoolCfg = gtSysCommPoolSingleSc910gsMultiplePipeSdr;
        }

        /* private pool config */
        if (AX_SNS_LINEAR_MODE == pVinParam->eHdrMode) {
            __cal_dump_pool(gtPrivatePoolSingleSc910gsMultiplePipeSdr, pVinParam->eHdrMode, pVinParam->nDumpFrameNum);
            pPrivArgs->nPoolCfgCnt = sizeof(gtPrivatePoolSingleSc910gsMultiplePipeSdr) /
                                     sizeof(gtPrivatePoolSingleSc910gsMultiplePipeSdr[0]);
            pPrivArgs->pPoolCfg = gtPrivatePoolSingleSc910gsMultiplePipeSdr;
        }

        /* cams config */
        __sample_case_single_sc910gs_its_capture(pCamList, eSnsType, pVinParam, pCommonArgs);
        break;

    case SAMPLE_VIN_SINGLE_SC410GS_ITS_CAPTURE:
        eSnsType = SMARTSENS_SC410GS;
        /* pool config */
        if (AX_SNS_LINEAR_MODE == pVinParam->eHdrMode) {
            __cal_dump_pool(gtSysCommPoolSingleSc410gsMultiplePipeSdr, pVinParam->eHdrMode, pVinParam->nDumpFrameNum);
            pCommonArgs->nPoolCfgCnt = sizeof(gtSysCommPoolSingleSc410gsMultiplePipeSdr) /
                                       sizeof(gtSysCommPoolSingleSc410gsMultiplePipeSdr[0]);
            pCommonArgs->pPoolCfg = gtSysCommPoolSingleSc410gsMultiplePipeSdr;
        }

        /* private pool config */
        if (AX_SNS_LINEAR_MODE == pVinParam->eHdrMode) {
            __cal_dump_pool(gtPrivatePoolSingleSc410gsMultiplePipeSdr, pVinParam->eHdrMode, pVinParam->nDumpFrameNum);
            pPrivArgs->nPoolCfgCnt = sizeof(gtPrivatePoolSingleSc410gsMultiplePipeSdr) /
                                     sizeof(gtPrivatePoolSingleSc410gsMultiplePipeSdr[0]);
            pPrivArgs->pPoolCfg = gtPrivatePoolSingleSc410gsMultiplePipeSdr;
        }

        /* cams config */
        __sample_case_single_sc410gs_its_capture(pCamList, eSnsType, pVinParam, pCommonArgs);
        break;
    case SAMPLE_VIN_SINGLE_DUMMY:
    default:
        eSnsType = SAMPLE_SNS_DUMMY;
        /* pool config */
        pCommonArgs->nPoolCfgCnt = sizeof(gtSysCommPoolSingleDummySdr) / sizeof(gtSysCommPoolSingleDummySdr[0]);
        pCommonArgs->pPoolCfg = gtSysCommPoolSingleDummySdr;

        /* private pool config */
        pPrivArgs->nPoolCfgCnt = sizeof(gtPrivatePoolSingleDummySdr) / sizeof(gtPrivatePoolSingleDummySdr[0]);
        pPrivArgs->pPoolCfg = gtPrivatePoolSingleDummySdr;

        /* cams config */
        __sample_case_single_dummy(pCamList, eSnsType, pVinParam, pCommonArgs);
        break;
    }

    return 0;
}

AX_S32 SAMPLE_CASE_VIN(SAMPLE_VIN_PARAM_T *pVinParam)
{
    AX_S32 axRet = 0;
    AX_S32 i = 0, j = 0;
    COMMON_SYS_ARGS_T tCommonArgs = {0};
    COMMON_SYS_ARGS_T tPrivArgs = {0};

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
    axRet = COMMON_CAM_Open(&gCams[0], tCommonArgs.nCamCnt);
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
    COMMON_CAM_Close(&gCams[0], tCommonArgs.nCamCnt);
EXIT_FAIL2:
    COMMON_CAM_Deinit();
EXIT_FAIL1:
    COMMON_SYS_DeInit();
EXIT_FAIL:
    return axRet;
}

AX_VOID PrintHelp()
{
    COMM_ISP_PRT("command:\n");
    COMM_ISP_PRT("\t-c: VIN Sample Case:\n");
    COMM_ISP_PRT("\t\t0: Single DummySensor\n");
    COMM_ISP_PRT("\t\t1: Single OS08A20\n");
    COMM_ISP_PRT("\t\t2: Dual OS08A20\n");
    COMM_ISP_PRT("\t\t3: Dual OS08A20 Bind Multiple Pipe\n");
    COMM_ISP_PRT("\t\t4: Single OS08A20 ITS Capture\n");
    COMM_ISP_PRT("\t\t5: Four OS08A20\n");
    COMM_ISP_PRT("\t\t6: Single SC910GS ITS Capture (Without FPGA)\n");
    COMM_ISP_PRT("\t\t7: Single SC410GS ITS Capture (Without FPGA)\n");
    COMM_ISP_PRT("\t\t8: Single OS04A10 DCG HDR\n");
    COMM_ISP_PRT("\t\t9: Single OS04A10 DCG VS HDR\n");
    COMM_ISP_PRT("\t\t10: UAV OV48C40 video mode\n");
    COMM_ISP_PRT("\t\t11: UAV OV48C40 photo mode\n");
    COMM_ISP_PRT("\t\t12: LOW DELAY OV48C40 mode\n");
    COMM_ISP_PRT("\t\t13: Eight SC450AI\n");
    COMM_ISP_PRT("\t\t14: Eight OS04A10\n");

    COMM_ISP_PRT("\t\t25: Single RGB888 input\n");
    COMM_ISP_PRT("\t\t26: Single YUV input YUV422 to YUV420\n");
    COMM_ISP_PRT("\t\t27: Single YUV input YUV422 to YUV420 4 multi\n");
    COMM_ISP_PRT("\t\t28: Single YUV input YUV422 to YUV420 10bit 4 multi\n");

    COMM_ISP_PRT("\t-m: Work Mode:\n");
    COMM_ISP_PRT("\t\t0: LoadRaw Mode\n");
    COMM_ISP_PRT("\t\t1: Sensor Mode\n");
    COMM_ISP_PRT("\t\t2: TPG Mode\n");

    COMM_ISP_PRT("\t-e: SDR/HDR Mode:\n");
    COMM_ISP_PRT("\t\t1: SDR\n");
    COMM_ISP_PRT("\t\t2: HDR 2DOL\n");
    COMM_ISP_PRT("\t\t3: HDR 3DOL\n");

    COMM_ISP_PRT("\t-a: Enable AIISP:\n");
    COMM_ISP_PRT("\t\t0: Disable(default)\n");
    COMM_ISP_PRT("\t\t1: Enable\n");

    COMM_ISP_PRT("\t-d: Continue Frame Dump:\n");
    COMM_ISP_PRT("\t\tn: N Frame Blk\n");
}

int main(int argc, char *argv[])
{
    COMM_ISP_PRT("VIN Sample. Build at %s %s\n", __DATE__, __TIME__);

    int c;
    int isExit = 0;
    SAMPLE_VIN_PARAM_T tVinParam = {
        SAMPLE_VIN_SINGLE_OS08A20,
        COMMON_VIN_SENSOR,
        AX_SNS_LINEAR_MODE,
        AX_FALSE,
        0,
    };
    signal(SIGPIPE, SIG_IGN);
    signal(SIGINT, __sigint);

    if (argc < 3) {
        PrintHelp();
        exit(0);
    }

    while ((c = getopt(argc, argv, "c:m:e:d:a:h")) != -1) {
        isExit = 0;
        switch (c) {
        case 'c':
            tVinParam.eSysCase = (SAMPLE_VIN_CASE_E)atoi(optarg);
            break;
        case 'm':
            tVinParam.eSysMode = (COMMON_VIN_MODE_E)atoi(optarg);
            break;
        case 'e':
            tVinParam.eHdrMode = (AX_SNS_HDR_MODE_E)atoi(optarg);
            break;
        case 'd':
            tVinParam.nDumpFrameNum = (AX_S32)atoi(optarg);
            break;
        case 'a':
            tVinParam.bAiispEnable = (AX_BOOL)atoi(optarg);
            break;
        case 'h':
            isExit = 1;
            break;
        default:
            isExit = 1;
            break;
        }
    }

    if (isExit) {
        PrintHelp();
        exit(0);
    }

    if (tVinParam.eSysCase >= SAMPLE_VIN_BUTT || tVinParam.eSysCase <= SAMPLE_VIN_NONE) {
        COMM_ISP_PRT("error sys case : %d\n", tVinParam.eSysCase);
        exit(0);
    }

    if (tVinParam.eSysMode >= COMMON_VIN_BUTT || tVinParam.eSysMode <= COMMON_VIN_NONE) {
        COMM_ISP_PRT("error sys mode : %d\n", tVinParam.eSysMode);
        exit(0);
    }

    if ((tVinParam.eSysCase == SAMPLE_VIN_SINGLE_OS08A20_ITS_CAPTURE) && (tVinParam.eHdrMode > AX_SNS_LINEAR_MODE)) {
        COMM_ISP_PRT("single its capture case not support HDR mode\n");
        exit(0);
    }

    if (tVinParam.eSysCase == SAMPLE_VIN_SINGLE_OS04A10_DCG_HDR ||
        tVinParam.eSysCase == SAMPLE_VIN_SINGLE_OS04A10_DCG_VS_HDR) {
        SAMPLE_CASE_VIN_DCG_VS(&tVinParam);
    } else if (tVinParam.eSysCase == SAMPLE_VIN_UAV_OV48C40_VIDEO_MODE ||
               tVinParam.eSysCase == SAMPLE_VIN_UAV_OV48C40_PHOTO_MODE) {
        SAMPLE_CASE_VIN_UAV_OV48C40(&tVinParam);
    } else if (tVinParam.eSysCase == SAMPLE_VIN_LOW_DELAY_OV48C40) {
        SAMPLE_CASE_VIN_LOW_DELAY_OV48C40(&tVinParam);
    } else if (tVinParam.eSysCase == SAMPLE_VIN_EIGHT_SC450AI || tVinParam.eSysCase == SAMPLE_VIN_EIGHT_OS04A10) {
        SAMPLE_CASE_EIGHT_CAMERA(&tVinParam);
    } else {
        SAMPLE_CASE_VIN(&tVinParam);
    }

    COMM_ISP_PRT("Sample Completed\n");

    exit(0);
}

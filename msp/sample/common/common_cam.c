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

#include "ax_vin_api.h"
#include "ax_isp_api.h"
#include "ax_vin_error_code.h"
#include "ax_mipi_rx_api.h"
#include "common_que.h"
#include "common_cam.h"
#include "common_sys.h"
#include "common_type.h"
#include "ax_isp_3a_api.h"

static pthread_t gDispatchThread[MAX_CAMERAS] = {0};
static pthread_t gCaptureThread[MAX_CAMERAS] = {0};
static AX_S32 g_dispatcher_loop_exit[MAX_CAMERAS] = {0};
static AX_S32 g_capture_loop_exit[MAX_CAMERAS] = {0};

static Queue payloadQue[MAX_CAMERAS] = {0};
typedef struct {
    AX_S32 devId;
} IdxDevIdMap;

static IdxDevIdMap idxMap[MAX_CAMERAS] = {0};

static void *DispatchThread(void *args);
static void *CaptureFrameThread(void *args);

static AX_S32 SysFrameDispatch(AX_U8 nDevId, AX_CAMERA_T *pCam, AX_SNS_HDR_MODE_E eHdrMode);

AX_S32 COMMON_Cam_GetQueIdx(AX_S32 devId)
{
    AX_S32 i = -1;
    for (i = 0; i < MAX_CAMERAS; i++) {
        if (idxMap[i].devId == devId)
            return i;
    }

    return i;
}

AX_S32 COMMON_CAM_StitchAttrInit(AX_U8 nGrpId, AX_VIN_STITCH_GRP_ATTR_T *pStitchGrpAttr)
{
    AX_S32 axRet = 0;
    if (pStitchGrpAttr == NULL) {
        return -1;
    }

    axRet = AX_VIN_SetStitchGrpAttr(nGrpId, pStitchGrpAttr);
    if (0 != axRet) {
        COMM_ISP_PRT("AX_VIN_SetStitchGrpAttr failed, nRet=0x%x.\n", axRet);
        return -1;
    }

    return 0;
}


AX_S32 COMMON_NPU_Init(AX_ENGINE_NPU_MODE_T eHardMode)
{
    AX_S32 axRet = 0;

    /* NPU Init */
    AX_ENGINE_NPU_ATTR_T npu_attr;
    memset(&npu_attr, 0x0, sizeof(npu_attr));
    npu_attr.eHardMode = eHardMode;
    axRet = AX_ENGINE_Init(&npu_attr);
    if (0 != axRet) {
        COMM_CAM_PRT("AX_ENGINE_Init failed, ret=0x%x.\n", axRet);
        return -1;
    }
    return 0;
}

AX_S32 COMMON_CAM_PrivPoolInit(COMMON_SYS_ARGS_T *pPrivPoolArgs)
{
    AX_S32 axRet = 0;
    AX_POOL_FLOORPLAN_T tPoolFloorPlan = {0};

    if (pPrivPoolArgs == NULL) {
        return -1;
    }

    /* Calc Pool BlkSize/BlkCnt */
    axRet = COMMON_SYS_CalcPool(pPrivPoolArgs->pPoolCfg, pPrivPoolArgs->nPoolCfgCnt, &tPoolFloorPlan);
    if (0 != axRet) {
        COMM_CAM_PRT("COMMON_SYS_CalcPool failed, ret=0x%x.\n", axRet);
        return -1;
    }

    axRet = AX_VIN_SetPoolAttr(&tPoolFloorPlan);
    if (0 != axRet) {
        COMM_CAM_PRT("AX_VIN_SetPoolAttr fail!Error Code:0x%X\n", axRet);
        return -1;
    } else {
        printf("AX_VIN_SetPoolAttr success!\n");
    }

    return 0;
}

AX_S32 COMMON_CAM_Init(AX_VOID)
{
    AX_S32 axRet = 0;

    /* VIN Init */
    axRet = AX_VIN_Init();
    if (0 != axRet) {
        COMM_CAM_PRT("AX_VIN_Init failed, ret=0x%x.\n", axRet);
        return -1;
    }

    /* MIPI Init */
    axRet = AX_MIPI_RX_Init();
    if (0 != axRet) {
        COMM_CAM_PRT("AX_MIPI_RX_Init failed, ret=0x%x.\n", axRet);
        return -1;
    }

    return 0;
}

AX_S32 COMMON_CAM_Deinit(AX_VOID)
{
    AX_S32 axRet = 0;

    axRet = AX_MIPI_RX_DeInit();
    if (0 != axRet) {
        COMM_CAM_PRT("AX_MIPI_RX_DeInit failed, ret=0x%x.\n", axRet);
        return -1 ;
    }

    axRet = AX_VIN_Deinit();
    if (0 != axRet) {
        COMM_CAM_PRT("AX_VIN_DeInit failed, ret=0x%x.\n", axRet);
        return -1 ;
    }

    axRet = AX_ENGINE_Deinit();
    if (0 != axRet) {
        COMM_CAM_PRT("AX_ENGINE_Deinit failed, ret=0x%x.\n", axRet);
        return -1 ;
    }

    return axRet;
}

static AX_S32 __common_cam_open(AX_CAMERA_T *pCam)
{
    AX_S32 i = 0;
    AX_S32 axRet = 0;
    AX_U8 nPipeId = 0;
    AX_U8 nDevId = pCam->nDevId;
    AX_U32 nRxDev = pCam->nRxDev;
    AX_BOOL bIsSnapCam = AX_FALSE;

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
#if 0
    if (pCam->bEnableFlash == AX_TRUE) {
        axRet = COMMON_VIN_StartOutsideDev(nDevId, &pCam->tPowerAttr, &pCam->tVsyncAttr, &pCam->tHsyncAttr,
                                           &pCam->tLightSyncInfo, &pCam->tSnapStrobeAttr, &pCam->tSnapFlashAttr);
        if (0 != axRet) {
            COMM_CAM_PRT("COMMON_VIN_StartOutsideDev failed, ret=0x%x.\n", axRet);
            return -1;
        }
    }
#else
    if (pCam->bEnableFlash == AX_TRUE) {
        axRet = COMMON_TCM_StartOutsideDev(nDevId, &pCam->tSensorVts, &pCam->tPowerAttr, &pCam->tVsyncAttr, &pCam->tHsyncAttr,
                                           &pCam->tLightSyncInfo, &pCam->tSnapStrobeAttr, &pCam->tSnapFlashAttr);
        if (0 != axRet) {
            COMM_CAM_PRT("COMMON_TCM_StartOutsideDev failed, ret=0x%x.\n", axRet);
            return -1;
        }
    }
#endif
    if (!pCam->bDevOnly) {
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

            axRet = COMMON_ISP_Init(nPipeId, pCam->ptSnsHdl[pCam->nPipeId], pCam->bRegisterSns, pCam->bUser3a,
                                    &pCam->tAeFuncs, &pCam->tAwbFuncs, &pCam->tAfFuncs, &pCam->tLscFuncs,
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
            } else {
                bIsSnapCam = AX_TRUE;
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

    if ((!pCam->bDevOnly) && pCam->bEnableDev &&
        ((pCam->tPipeAttr[pCam->nPipeId].ePipeWorkMode == AX_VIN_PIPE_NORMAL_MODE0) ||
         (pCam->tPipeAttr[pCam->nPipeId].ePipeWorkMode == AX_VIN_PIPE_NORMAL_MODE2))) {
        if (pCam->nNumber >= MAX_CAMERAS) {
            COMM_CAM_PRT("Cam number[%d] out of range\n", pCam->nNumber);
            return -1;
        }

        COMM_CAM_PRT("ePipeWorkMode:%d \n", pCam->tPipeAttr[pCam->nPipeId].ePipeWorkMode);
        g_dispatcher_loop_exit[pCam->nNumber] = 0;
        g_capture_loop_exit[pCam->nNumber] = 0;
        if (pCam->tPipeInfo[0].eFrameSrcType == AX_VIN_FRAME_SOURCE_TYPE_USER) {
            if (pCam->tPipeInfo[0].ePipeWorkMode == AX_VIN_PIPE_NORMAL_MODE0) {
                pthread_create(&gDispatchThread[pCam->nNumber], NULL, DispatchThread, (AX_VOID *)pCam);
            }
        }

        if (AX_TRUE == bIsSnapCam) {
            pthread_create(&gCaptureThread[pCam->nNumber], NULL, CaptureFrameThread, (AX_VOID *)pCam);
            COMMON_QUEUE_Init(&payloadQue[pCam->nNumber]);
            idxMap[pCam->nNumber].devId = pCam->nDevId;
        }
    }

    return 0;
}

static AX_S32 __common_cam_close(AX_CAMERA_T *pCam)
{
    AX_U8 i = 0;
    AX_S32 axRet = 0;
    AX_U8 nPipeId = pCam->nPipeId;
    AX_U8 nDevId = pCam->nDevId;
    AX_U32 nRxDev = pCam->nRxDev;

    if (!pCam->bDevOnly) {
        for (i = 0; i < pCam->tDevBindPipe.nNum; i++) {
            nPipeId = pCam->tDevBindPipe.nPipeId[i];
            if (pCam->tPipeInfo[nPipeId].ePipeMode != SAMPLE_PIPE_MODE_FLASH_SNAP) {
                axRet |= AX_ISP_Stop(nPipeId);
                if (0 != axRet) {
                    COMM_ISP_PRT("AX_ISP_Stop failed, ret=0x%x.\n", axRet);
                }
            }
        }
    }

    if (pCam->nNumber < MAX_CAMERAS) {
        g_capture_loop_exit[pCam->nNumber] = 1;
        if (gCaptureThread[pCam->nNumber] != 0) {
            axRet = pthread_join(gCaptureThread[pCam->nNumber], NULL);
            if (axRet < 0) {
                COMM_CAM_PRT(" dispacher thread exit failed, ret=0x%x.\n", axRet);
            }
            gCaptureThread[pCam->nNumber] = 0;
            COMMON_QUEUE_Destroy(&payloadQue[pCam->nNumber]);
        }

        g_dispatcher_loop_exit[pCam->nNumber] = 1;
        if (gDispatchThread[pCam->nNumber] != 0) {
            axRet = pthread_join(gDispatchThread[pCam->nNumber], NULL);
            if (axRet < 0) {
                COMM_CAM_PRT(" dispacher thread exit failed, ret=0x%x.\n", axRet);
            }
            gDispatchThread[pCam->nNumber] = 0;
        }

    } else {
        COMM_CAM_PRT("Cam number[%d] out of range\n", pCam->nNumber);
    }
#if 0
    if (pCam->bEnableFlash == AX_TRUE) {
        axRet = COMMON_VIN_StopOutsideDev(nDevId);
        if (0 != axRet) {
            COMM_CAM_PRT("COMMON_VIN_StopOutsideDev failed, ret=0x%x.\n", axRet);
            return -1;
        }
    }
#else
    if (pCam->bEnableFlash == AX_TRUE) {
        axRet = COMMON_TCM_StopOutsideDev(nDevId);
        if (0 != axRet) {
            COMM_CAM_PRT("COMMON_TCM_StopOutsideDev failed, ret=0x%x.\n", axRet);
            return -1;
        }
    }

#endif
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

    if (!pCam->bDevOnly) {
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

AX_S32 COMMON_CAM_Open(AX_CAMERA_T *pCamList, AX_U8 Num)
{
    AX_U16 i = 0;
    if (pCamList == NULL) {
        return -1;
    }

    for (i = 0; i < Num; i++) {
        if (AX_SUCCESS == __common_cam_open(&pCamList[i])) {
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
        __common_cam_close(&pCamList[i]);
    }
    return -1;
}

AX_S32 COMMON_CAM_Close(AX_CAMERA_T *pCamList, AX_U8 Num)
{
    AX_U16 i = 0;
    if (pCamList == NULL) {
        return -1;
    }

    for (i = 0; i < Num; i++) {
        if (!pCamList[i].bOpen) {
            continue;
        }

        if (AX_SUCCESS == __common_cam_close(&pCamList[i])) {
            COMM_CAM_PRT("camera %d is close\n", i);
            pCamList[i].bOpen = AX_FALSE;
        } else {
            return -1;
        }
    }

    return 0;
}

static AX_BOOL SeqNumIsMatch(AX_U8 nDevId, AX_IMG_INFO_T *frameBufferArr,  AX_U64 *frameSeqs, AX_U64 maxFrameSeq,
                             AX_SNS_HDR_MODE_E eHdrMode)
{
    AX_BOOL frameSeqNotMatch = AX_FALSE;
    AX_S32 j = 0, retry_times = 0;

    do {
        frameSeqNotMatch = AX_FALSE;
        for (j = 0; j < eHdrMode; j++) {
            if (frameSeqs[j] < maxFrameSeq) {
                COMM_ISP_PRT("FrameSeq(%lld) doesn't match (max_frame_seq:%lld), drop blk_id: 0x%x\n",
                             frameSeqs[j], maxFrameSeq, frameBufferArr[j].tFrameInfo.stVFrame.u32BlkId[0]);
                AX_VIN_ReleaseDevFrame(nDevId, j, frameBufferArr + j);
                AX_VIN_GetDevFrame(nDevId, j, frameBufferArr + j, -1);
                frameSeqNotMatch = AX_TRUE;
            }
        }

        if (frameSeqNotMatch) {
            for (j = 0; j < eHdrMode; j++) {
                frameSeqs[j] = frameBufferArr[j].tFrameInfo.stVFrame.u64SeqNum;
                if (frameSeqs[j] > maxFrameSeq) {
                    maxFrameSeq = frameSeqs[j];
                }
            }
        }

        retry_times++;
        if (retry_times > 10)
            return AX_FALSE;
    } while (frameSeqNotMatch);

    return AX_TRUE;
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
    AX_BOOL isMatch = AX_FALSE;

    AX_U32 FrmType = 0;
    AX_S32 queueIdx;
    AX_S32 flag = 0;
    queueIdx = COMMON_Cam_GetQueIdx(pCam->nDevId);

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
                isMatch = SeqNumIsMatch(nDevId, frameBufferArr, frameSeqs, maxFrameSeq, eHdrMode);
                if (isMatch == AX_TRUE) {
                    axRet = AX_VIN_SendRawFrame(ptDevBindPipe->nPipeId[cnt], AX_VIN_FRAME_SOURCE_ID_IFE, eHdrMode,
                                                (const AX_IMG_INFO_T **)&frameBufferArr, 0);
                    if (axRet != 0) {
                        COMM_CAM_PRT("Send Pipe raw frame failed\n");
                    }
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
                isMatch = SeqNumIsMatch(nDevId, frameBufferArr, frameSeqs, maxFrameSeq, eHdrMode);
                if (isMatch == AX_TRUE) {
                    axRet = AX_VIN_SendRawFrame(ptDevBindPipe->nPipeId[cnt], AX_VIN_FRAME_SOURCE_ID_IFE, eHdrMode,
                                                (const AX_IMG_INFO_T **)&frameBufferArr, 0);
                    if (axRet != 0) {
                        COMM_CAM_PRT("Send Pipe raw frame failed\n");
                    }
                } else {

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

AX_S32 COMMON_CAM_CaptureFrameProc(AX_U8 nDevId, AX_CAMERA_T *pCam, AX_SNS_HDR_MODE_E eHdrMode)
{
    AX_S32 axRet = 0;
    AX_S32 timeOutMs = 1000;
    AX_U32  nPipeId = 0;
    AX_IMG_INFO_T frameBufferArrIfe[AX_SNS_HDR_FRAME_MAX] = {0};
    AX_IMG_INFO_T frameBufferArrAinr[AX_SNS_HDR_FRAME_MAX] = {0};
    AX_ISP_IQ_RLTM_PARAM_T tUserCaptureFrameRltmParam;
    AX_ISP_IQ_AE_PARAM_T tUserCaptureFrameAeParam;
    AX_S32 nCapturePipeId;
    AX_IMG_INFO_T imgInfo[4] = {0};
    AX_S32 queueIdx;

    queueIdx = COMMON_Cam_GetQueIdx(nDevId);
    axRet = COMMON_QUEUE_Pop(&payloadQue[queueIdx], &nCapturePipeId, &imgInfo[0]);
    if (axRet) {
        //COMM_CAM_PRT("axRet:0x%x\n", axRet);
        return axRet;

    }

    AX_IMG_INFO_T *pRawImgInfo = &imgInfo[0];

    /* use your capture raw frame's ae in manual mode, this is just a sample*/
    AX_ISP_IQ_GetAeParam(nCapturePipeId, &tUserCaptureFrameAeParam);
    tUserCaptureFrameAeParam.nEnable = AX_FALSE;
    tUserCaptureFrameAeParam.tExpManual.nAGain = pRawImgInfo->tIspInfo.tExpInfo.nAgain;
    tUserCaptureFrameAeParam.tExpManual.nDgain = pRawImgInfo->tIspInfo.tExpInfo.nDgain;
    tUserCaptureFrameAeParam.tExpManual.nHcgLcg = pRawImgInfo->tIspInfo.tExpInfo.nHcgLcgMode;
    tUserCaptureFrameAeParam.tExpManual.nHdrRatio = pRawImgInfo->tIspInfo.tExpInfo.nHdrRatio;
    tUserCaptureFrameAeParam.tExpManual.nShutter = pRawImgInfo->tIspInfo.tExpInfo.nIntTime;
    tUserCaptureFrameAeParam.tExpManual.nIspGain = pRawImgInfo->tIspInfo.tExpInfo.nIspGain;

    AX_ISP_IQ_SetAeParam(nCapturePipeId, &tUserCaptureFrameAeParam);

    AX_ISP_IQ_GetRltmParam(nCapturePipeId, &tUserCaptureFrameRltmParam);
    tUserCaptureFrameRltmParam.tTempoFilter.nReset = 1;
    AX_ISP_IQ_SetRltmParam(nCapturePipeId, &tUserCaptureFrameRltmParam);

    /*1. Run once calc param*/
    AX_ISP_RunOnce(nCapturePipeId);

    /* 2. send raw frame to ife*/
    axRet = AX_VIN_SendRawFrame(nCapturePipeId, AX_VIN_FRAME_SOURCE_ID_IFE, eHdrMode,
                                (const AX_IMG_INFO_T **)&imgInfo, 0);
    if (axRet != 0) {
        COMM_CAM_PRT("Send Pipe raw frame failed\n");
    }

    /* 3. Release BLK obtained from dev*/
    axRet = AX_VIN_ReleaseDevFrame(nDevId, AX_SNS_HDR_FRAME_L, imgInfo);
    if (axRet != 0) {
        COMM_CAM_PRT("AX_VIN_ReleaseDevFrame failed\n");
    }

    /* 4. get raw frame from ife*/
    axRet = AX_VIN_GetRawFrame(nCapturePipeId, AX_VIN_PIPE_DUMP_NODE_IFE, AX_SNS_HDR_FRAME_L,
                               frameBufferArrIfe, timeOutMs);
    if (axRet != 0) {
        if (AX_ERR_VIN_RES_EMPTY == axRet) {
            COMM_CAM_PRT("nonblock error, 0x%x\n", axRet);
            return axRet;
        }

        usleep(10 * 1000);
        AX_VIN_ReleaseRawFrame(nCapturePipeId, AX_VIN_PIPE_DUMP_NODE_IFE, AX_SNS_HDR_FRAME_L, frameBufferArrIfe);
        return axRet;
    }

    nPipeId = nCapturePipeId;

    /* 5. Send the raw frame obtained from IFE to ITP*/
    axRet = AX_VIN_SendRawFrame(nPipeId, AX_VIN_FRAME_SOURCE_ID_ITP, eHdrMode,
                                (const AX_IMG_INFO_T **)&frameBufferArrIfe, 0);
    if (axRet != 0) {
        COMM_CAM_PRT("Send Pipe raw frame failed\n");
    }

    /* 6. Send the raw frame obtained from IFE to AINR*/
    axRet = AX_VIN_SendRawFrame(nPipeId, AX_VIN_FRAME_SOURCE_ID_AINR, eHdrMode,
                                (const AX_IMG_INFO_T **)&frameBufferArrIfe, 0);
    if (axRet != 0) {
        COMM_CAM_PRT("Send Pipe raw frame failed\n");
    }

    /* 7. Release BLK obtained from IFE*/
    axRet = AX_VIN_ReleaseRawFrame(nCapturePipeId, AX_VIN_PIPE_DUMP_NODE_IFE, AX_SNS_HDR_FRAME_L,
                                   frameBufferArrIfe);
    if (axRet != 0) {
        COMM_CAM_PRT("release Pipe raw frame failed\n");
    }
    /* 8. Wait frame done interrupt*/
    axRet = AX_ISP_GetIrqTimeOut(nPipeId, AX_IRQ_TYPE_FRAME_DONE, timeOutMs);
    if (axRet != 0) {
        COMM_CAM_PRT("AX_ISP_GetIrqTimeOut failed\n");
    }
    /* 9. Second Runonce*/
    axRet = AX_ISP_RunOnce(nPipeId);
    if (axRet != 0) {
        COMM_CAM_PRT("AX_ISP_RunOnce failed\n");
    }
    /* 10. get raw frame from NPU*/
    axRet = AX_VIN_GetRawFrame(nPipeId, AX_VIN_PIPE_DUMP_NODE_AINR, AX_SNS_HDR_FRAME_L,
                               frameBufferArrAinr, -1);
    if (axRet != 0) {
        COMM_CAM_PRT("Get Pipe raw frame failed\n");
    }

    /* 11. Send the raw frame obtained from NPU to ITP*/
    axRet = AX_VIN_SendRawFrame(nPipeId, AX_VIN_FRAME_SOURCE_ID_ITP, eHdrMode,
                                (const AX_IMG_INFO_T **)&frameBufferArrAinr, 0);
    if (axRet != 0) {
        COMM_CAM_PRT("Send Pipe raw frame failed\n");
    }
    /* 12. Wait frame done interrupt*/
    axRet = AX_ISP_GetIrqTimeOut(nPipeId, AX_IRQ_TYPE_FRAME_DONE, timeOutMs);
    if (axRet != 0) {
        COMM_CAM_PRT("AX_ISP_GetIrqTimeOut failed\n");
    }

    /* 13. Release BLK obtained from NPU*/
    axRet = AX_VIN_ReleaseRawFrame(nCapturePipeId, AX_VIN_PIPE_DUMP_NODE_AINR, AX_SNS_HDR_FRAME_L,
                                   frameBufferArrAinr);
    if (axRet != 0) {
        COMM_CAM_PRT("AX_VIN_ReleaseRawFrame failed\n");
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

static void *CaptureFrameThread(void *args)
{
    AX_CHAR token[32] = {0};
    AX_CAMERA_T *pCam = (AX_CAMERA_T *)args;

    AX_U8 nDevId = pCam->nDevId;
    AX_SNS_HDR_MODE_E eHdrMode = pCam->eHdrMode;


    snprintf(token, 32, "RAW_DISP_%u", nDevId);
    prctl(PR_SET_NAME, token);

    while (!g_capture_loop_exit[pCam->nNumber]) {
        COMMON_CAM_CaptureFrameProc(nDevId, pCam, eHdrMode);
    }

    return NULL;
}

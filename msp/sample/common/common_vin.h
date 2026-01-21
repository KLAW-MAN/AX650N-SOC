/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef __COMMON_VIN_H__
#define __COMMON_VIN_H__

#include "ax_vin_api.h"
#include "ax_base_type.h"
#include "ax_mipi_rx_api.h"
#include "ax_isp_3a_plus.h"
#include "common_tcm.h"

typedef struct _AX_SNS_CLK_ATTR_T_ {
    AX_U8               nSnsClkIdx;
    AX_SNS_CLK_RATE_E   eSnsClkRate;
} AX_SNS_CLK_ATTR_T;

typedef enum {
    SAMPLE_SNS_TYPE_NONE = -1,
    /* ov sensor */
    OMNIVISION_OS08A20 = 0,
    OMNIVISION_OS04A10_2LANE = 1,
    OMNIVISION_OS04A10_DCG = 2,
    OMNIVISION_OS04A10_DCG_VS = 3,
    OMNIVISION_OV48C40 = 4,

    SMARTSENS_SC910GS = 10,
    SMARTSENS_SC410GS = 11,
    SMARTSENS_SC450AI = 12,

    /* dummy sensor */
    SAMPLE_SNS_DUMMY = 100,
    SAMPLE_SNS_TYPE_BUTT,
} SAMPLE_SNS_TYPE_E;

typedef enum {
    COMMON_VIN_NONE = -1,
    COMMON_VIN_LOADRAW = 0,
    COMMON_VIN_SENSOR = 1,
    COMMON_VIN_TPG = 2,
    COMMON_VIN_BUTT
} COMMON_VIN_MODE_E;

#ifndef COMM_VIN_PRT
#define COMM_VIN_PRT(fmt...)   \
do {\
    printf("[COMM_VIN][%s][%5d] ", __FUNCTION__, __LINE__);\
    printf(fmt);\
}while(0)
#endif


#ifdef __cplusplus
extern "C"
{
#endif

AX_S32 COMMON_VIN_StartMipi(AX_U8 devId, AX_MIPI_RX_DEV_T *ptMipiRx);
AX_S32 COMMON_VIN_StopMipi(AX_U8 devId);
AX_S32 COMMON_VIN_CreateDev(AX_U8 devId, AX_U32 nRxDev, AX_VIN_DEV_ATTR_T *pDevAttr,
                            AX_VIN_DEV_BIND_PIPE_T *ptDevBindPipe, AX_FRAME_INTERRUPT_ATTR_T *ptDevFrmIntAttr);
AX_S32 COMMON_VIN_DestroyDev(AX_U8 devId);
AX_S32 COMMON_VIN_StartDev(AX_U8 devId, AX_BOOL bEnableDev, AX_VIN_DEV_ATTR_T *pDevAttr);
AX_S32 COMMON_VIN_StopDev(AX_U8 devId, AX_BOOL bEnableDev);


AX_S32 COMMON_VIN_SetPipeAttr(COMMON_VIN_MODE_E eSysMode, AX_U8 nPipeId, AX_VIN_PIPE_ATTR_T *pPipeAttr, AX_VIN_FRAME_SOURCE_TYPE_E eFrameSrcType);

AX_S32 COMMON_VIN_StartChn(AX_U8 pipe, AX_VIN_CHN_ATTR_T *ptChnAttr, AX_BOOL *pbChnEnable);
AX_S32 COMMON_VIN_StopChn(AX_U8 pipe);

AX_S32 COMMON_VIN_GetSnsConfig(SAMPLE_SNS_TYPE_E eSnsType,
                               AX_MIPI_RX_DEV_T *ptMipiRx, AX_SNS_ATTR_T *ptSnsAttr,
                               AX_SNS_CLK_ATTR_T *ptSnsClkAttr, AX_VIN_DEV_ATTR_T *pDevAttr);

AX_S32 COMMON_VIN_GetPipeConfig(SAMPLE_SNS_TYPE_E eSnsType,
                               AX_VIN_PIPE_ATTR_T *pPipeAttr, AX_VIN_CHN_ATTR_T *pChnAttr, AX_BOOL *pChnEnable);

AX_S32 COMMON_VIN_GetOutsideConfig(SAMPLE_SNS_TYPE_E eSnsType, AX_VIN_SENSOR_VTS_T *ptSensorVts, AX_VIN_POWER_SYNC_ATTR_T *ptPowerAttr,
                              AX_VIN_SYNC_SIGNAL_ATTR_T *ptVsyncAttr, AX_VIN_SYNC_SIGNAL_ATTR_T *ptHsyncAttr,
                              AX_VIN_LIGHT_SYNC_INFO_T *ptLightSyncInfo, AX_VIN_STROBE_LIGHT_TIMING_ATTR_T *ptSnapStrobeAttr,
                              AX_VIN_FLASH_LIGHT_TIMING_ATTR_T *ptSnapFlashAttr);
AX_S32 COMMON_VIN_StartOutsideDev(AX_U8 devId, AX_VIN_POWER_SYNC_ATTR_T *ptSyncPowerAttr,
                              AX_VIN_SYNC_SIGNAL_ATTR_T *ptVsyncAttr, AX_VIN_SYNC_SIGNAL_ATTR_T *ptHsyncAttr,
                              AX_VIN_LIGHT_SYNC_INFO_T *ptLightSyncInfo, AX_VIN_STROBE_LIGHT_TIMING_ATTR_T *ptSnapStrobeAttr,
                              AX_VIN_FLASH_LIGHT_TIMING_ATTR_T *ptSnapFlashAttr);
AX_S32 COMMON_VIN_StopOutsideDev(AX_U8 devId);

#ifdef __cplusplus
}
#endif

#endif

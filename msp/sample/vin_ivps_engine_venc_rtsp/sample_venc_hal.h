/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef _SAMPLE_VENC_HAL_H_
#define _SAMPLE_VENC_HAL_H_

typedef struct
{
    AX_U64 totalGetStrmNum;
    AX_U64 maxDeltaPts;
    AX_U64 minDeltaPts;
    AX_U64 totalDeltaPts;
} SAMPLE_DELTA_PTS_INFO_T;

typedef struct
{
    AX_S32 chnNum;
    AX_BOOL bGetStrmStart;
    pthread_t getStrmPid;
} SAMPLE_VENC_SELECT_PARA_T;

AX_S32 SAMPLE_Ivps2VencInit(SAMPLE_GRP_T *pGrp, AX_BOOL bVencMode, AX_BOOL bVencSelect);
AX_S32 SAMPLE_Ivps2VencDeinit(SAMPLE_GRP_T *pGrp, AX_BOOL bVencMode, AX_BOOL bVencSelect);

#endif /* _SAMPLE_IVPS_HAL_H_ */
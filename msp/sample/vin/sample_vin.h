/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef _SAMPLE_VIN_H_
#define _SAMPLE_VIN_H_

#include "common_vin.h"

typedef enum {
    SAMPLE_VIN_NONE  = -1,
    SAMPLE_VIN_SINGLE_DUMMY  = 0,
    SAMPLE_VIN_SINGLE_OS08A20  = 1,
    SAMPLE_VIN_DOUBLE_OS08A20  = 2,
    SAMPLE_VIN_DOUBLE_OS08A20_MULTIPLE_PIPE = 3,
    SAMPLE_VIN_SINGLE_OS08A20_ITS_CAPTURE = 4,
    SAMPLE_VIN_FOUR_OS08A20  = 5,
    SAMPLE_VIN_SINGLE_SC910GS_ITS_CAPTURE = 6,
    SAMPLE_VIN_SINGLE_SC410GS_ITS_CAPTURE = 7,
    SAMPLE_VIN_SINGLE_OS04A10_DCG_HDR = 8,
    SAMPLE_VIN_SINGLE_OS04A10_DCG_VS_HDR = 9,
    SAMPLE_VIN_UAV_OV48C40_VIDEO_MODE = 10,
    SAMPLE_VIN_UAV_OV48C40_PHOTO_MODE = 11,
    SAMPLE_VIN_LOW_DELAY_OV48C40 = 12,
    SAMPLE_VIN_EIGHT_SC450AI = 13,
    SAMPLE_VIN_EIGHT_OS04A10 = 14,
    SAMPLE_VIN_SINGLE_RGB888 = 25, /* special config index */
    SAMPLE_VIN_SINGLE_YUV422 = 26,
    SAMPLE_VIN_SINGLE_YUV422_4MULTI = 27,
    SAMPLE_VIN_SINGLE_YUV422_10BIT_4MULTI = 28,
    SAMPLE_VIN_BUTT
} SAMPLE_VIN_CASE_E;

typedef struct {
    SAMPLE_VIN_CASE_E eSysCase;
    COMMON_VIN_MODE_E eSysMode;
    AX_SNS_HDR_MODE_E eHdrMode;
    AX_BOOL bAiispEnable;
    AX_S32 nDumpFrameNum;
} SAMPLE_VIN_PARAM_T;

#endif

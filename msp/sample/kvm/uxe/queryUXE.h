/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef __QUERY_UXE_INFO_H__
#define __QUERY_UXE_INFO_H__

#include "ax_base_type.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    UXE_INTERRUPT_VIDEO_DISAPPEAR = 0,
    UXE_INTERRUPT_VIDEO_READY = 1,
    UXE_INTERRUPT_AUDIO_DISAPPEAR = 2,
    UXE_INTERRUPT_AUDIO_READY = 3,
    UXE_INTERRUPT_BUTT,
} UXE_INTERRUPT_TYPE_E;

typedef enum {
    UXE_MIPI_RGB_6BIT = 0x00,
    UXE_MIPI_RGB_8BIT = 0x01,
    UXE_MIPI_RGB_10BIT = 0x02,
    UXE_MIPI_RGB_12BIT = 0x03,
    UXE_MIPI_YUV444_8BIT = 0x04,
    UXE_MIPI_YUV444_10BIT = 0x05,
    UXE_MIPI_YUV444_12BIT = 0x06,
    UXE_MIPI_YUV422_8BIT = 0x07,
    UXE_MIPI_YUV422_10BIT = 0x08,
    UXE_MIPI_YUV422_12BIT = 0x09,
    UXE_MIPI_YUV420_8BIT = 0x0A,
    UXE_MIPI_YUV420_10BIT = 0x0B,
    UXE_MIPI_YUV420_12BIT = 0x0C,
    UXE_MIPI_FORMAT_BUTT
} UXE_MIPI_FORMAT_E;

typedef struct {
    AX_U8 intType;   /* UXE_INTERRUPT_TYPE_E */
    AX_U32 pixelClk; /* pixel clock, KHz */
    AX_U16 HTotal;
    AX_U16 VTotal;
    AX_U16 HActive;
    AX_U16 VActive;
    AX_U16 AudioFS; /* Audio sampling frequency, KHz */
    AX_U32 byteClk; /* KHz */
    AX_U8 laneNum;
    AX_U8 mipiFmt; /* UXE_MIPI_FORMAT_E */
} UXE_SIGNAL_INFO_T;

AX_S32 AX_UXE_GetChip(AX_U16* chipId);
AX_S32 AX_UXE_GetSignalInfo(UXE_SIGNAL_INFO_T* info);
AX_S32 AX_UXE_EnableMipiTx(AX_VOID);
AX_S32 AX_UXE_DisableMipiTx(AX_VOID);

#ifdef __cplusplus
}
#endif

#endif /* __QUERY_UXE_INFO_H__ */

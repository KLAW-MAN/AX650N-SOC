/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef _COMMON_CONFIG_H__
#define _COMMON_CONFIG_H__

#include "ax_isp_common.h"
#include "ax_vin_api.h"
#include "ax_mipi_rx_api.h"
#include "common_tcm.h"

AX_MIPI_RX_DEV_T gDummyMipiRx = {
    .eInputMode = AX_INPUT_MODE_MIPI,
    .tMipiAttr.ePhyMode = AX_MIPI_PHY_TYPE_DPHY,
    .tMipiAttr.eLaneNum = AX_MIPI_DATA_LANE_4,
    .tMipiAttr.nDataRate =  400,
    .tMipiAttr.nDataLaneMap[0] = 0,
    .tMipiAttr.nDataLaneMap[1] = 1,
    .tMipiAttr.nDataLaneMap[2] = 2,
    .tMipiAttr.nDataLaneMap[3] = 3,
    .tMipiAttr.nDataLaneMap[4] = -1,
    .tMipiAttr.nDataLaneMap[5] = -1,
    .tMipiAttr.nDataLaneMap[6] = -1,
    .tMipiAttr.nDataLaneMap[7] = -1,
    .tMipiAttr.nClkLane[0]     = 1,
    .tMipiAttr.nClkLane[1]     = 0,
};

AX_SNS_ATTR_T gDummySnsAttr = {
    .nWidth = 3840,
    .nHeight = 2160,
    .fFrameRate = 30.0,
    .eSnsMode = AX_SNS_LINEAR_MODE,
    .eRawType = AX_RT_RAW10,
    .eBayerPattern = AX_BP_RGGB,
    .bTestPatternEnable = AX_FALSE,
};

AX_VIN_DEV_ATTR_T gDummyDevAttr = {
    .bImgDataEnable = AX_TRUE,
    .bNonImgDataEnable = AX_FALSE,
    .eDevMode = AX_VIN_DEV_OFFLINE,
    .eSnsIntfType = AX_SNS_INTF_TYPE_TPG,
    .tDevImgRgn[0] = {0, 0, 3840, 2160},
    .ePixelFmt = AX_FORMAT_BAYER_RAW_10BPP,
    .eBayerPattern = AX_BP_RGGB,
    .eSnsMode = AX_SNS_LINEAR_MODE,
    .eSnsOutputMode = AX_SNS_NORMAL,
    .tCompressInfo = {AX_COMPRESS_MODE_NONE, 0},
    .tFrameRateCtrl = {AX_INVALID_FRMRATE, AX_INVALID_FRMRATE},
};

AX_VIN_PIPE_ATTR_T gDummyPipeAttr = {
    .tPipeImgRgn = {0, 0, 3840, 2160},
    .eBayerPattern = AX_BP_RGGB,
    .ePixelFmt = AX_FORMAT_BAYER_RAW_10BPP,
    .eSnsMode = AX_SNS_LINEAR_MODE,
    .tCompressInfo = {AX_COMPRESS_MODE_NONE, 0},
    .tNrAttr = {{0, {AX_COMPRESS_MODE_NONE, 0}}, {1, {AX_COMPRESS_MODE_LOSSLESS, 0}}},
    .tFrameRateCtrl = {AX_INVALID_FRMRATE, AX_INVALID_FRMRATE},
};

AX_VIN_CHN_ATTR_T gDummyChn0Attr = {
    .nWidth = 3840,
    .nHeight = 2160,
    .nWidthStride = 3840,
    .eImgFormat = AX_FORMAT_YUV420_SEMIPLANAR,
    .nDepth = 2,
    .tCompressInfo = {AX_COMPRESS_MODE_NONE, 0},
    .tFrameRateCtrl = {AX_INVALID_FRMRATE, AX_INVALID_FRMRATE},
};

AX_VIN_CHN_ATTR_T gDummyChn1Attr = {
    .nWidth = 1920,
    .nHeight = 1080,
    .nWidthStride = 1920,
    .eImgFormat = AX_FORMAT_YUV420_SEMIPLANAR,
    .nDepth = 2,
    .tCompressInfo = {AX_COMPRESS_MODE_NONE, 0},
    .tFrameRateCtrl = {AX_INVALID_FRMRATE, AX_INVALID_FRMRATE},
};

AX_VIN_CHN_ATTR_T gDummyChn2Attr = {
    .nWidth = 720,
    .nHeight = 576,
    .nWidthStride = 720,
    .eImgFormat = AX_FORMAT_YUV420_SEMIPLANAR,
    .nDepth = 2,
    .tCompressInfo = {AX_COMPRESS_MODE_NONE, 0},
    .tFrameRateCtrl = {AX_INVALID_FRMRATE, AX_INVALID_FRMRATE},
};

AX_MIPI_RX_DEV_T gOs08a20MipiRx = {
    .eInputMode = AX_INPUT_MODE_MIPI,
    .tMipiAttr.ePhyMode = AX_MIPI_PHY_TYPE_DPHY,
    .tMipiAttr.eLaneNum = AX_MIPI_DATA_LANE_4,
    .tMipiAttr.nDataRate =  1440,
    .tMipiAttr.nDataLaneMap[0] = 0,
    .tMipiAttr.nDataLaneMap[1] = 1,
    .tMipiAttr.nDataLaneMap[2] = 2,
    .tMipiAttr.nDataLaneMap[3] = 3,
    .tMipiAttr.nDataLaneMap[4] = -1,
    .tMipiAttr.nDataLaneMap[5] = -1,
    .tMipiAttr.nDataLaneMap[6] = -1,
    .tMipiAttr.nDataLaneMap[7] = -1,
    .tMipiAttr.nClkLane[0]     = 1,
    .tMipiAttr.nClkLane[1]     = 0,
};

AX_SNS_ATTR_T gOs08a20SnsAttr = {
    .nWidth = 3840,
    .nHeight = 2160,
    .fFrameRate = 30.0,
    .eSnsMode = AX_SNS_LINEAR_MODE,
    .eRawType = AX_RT_RAW10,
    .eBayerPattern = AX_BP_RGGB,
    .bTestPatternEnable = AX_FALSE,
};

AX_SNS_CLK_ATTR_T gOs08a20SnsClkAttr = {
    .nSnsClkIdx = 0,
    .eSnsClkRate = AX_SNS_CLK_24M,
};

AX_VIN_DEV_ATTR_T gOs08a20DevAttr = {
    .bImgDataEnable = AX_TRUE,
    .bNonImgDataEnable = AX_FALSE,
    .eDevMode = AX_VIN_DEV_OFFLINE,
    .eSnsIntfType = AX_SNS_INTF_TYPE_MIPI_RAW,
    .tDevImgRgn[0] = {0, 0, 3840, 2160},
    .tDevImgRgn[1] = {0, 0, 3840, 2160},
    .tDevImgRgn[2] = {0, 0, 3840, 2160},
    .tDevImgRgn[3] = {0, 0, 3840, 2160},

    /* When users transfer special data, they need to configure VC&DT for szImgVc/szImgDt/szInfoVc/szInfoDt */
    //.tMipiIntfAttr.szImgVc[0] = 0,
    //.tMipiIntfAttr.szImgDt[0] = MIPI_CSI_DT_RAW10,
    //.tMipiIntfAttr.szImgVc[1] = 1,
    //.tMipiIntfAttr.szImgDt[1] = MIPI_CSI_DT_RAW10,

    .ePixelFmt = AX_FORMAT_BAYER_RAW_10BPP,
    .eBayerPattern = AX_BP_RGGB,
    .eSnsMode = AX_SNS_LINEAR_MODE,
    .eSnsOutputMode = AX_SNS_NORMAL,
    .tCompressInfo = {AX_COMPRESS_MODE_LOSSLESS, 0},
    .tFrameRateCtrl = {AX_INVALID_FRMRATE, AX_INVALID_FRMRATE},
};

AX_VIN_PIPE_ATTR_T gOs08a20PipeAttr = {
    .tPipeImgRgn = {0, 0, 3840, 2160},
    .eBayerPattern = AX_BP_RGGB,
    .ePixelFmt = AX_FORMAT_BAYER_RAW_10BPP,
    .eSnsMode = AX_SNS_LINEAR_MODE,
    .tCompressInfo = {AX_COMPRESS_MODE_LOSSLESS, 0},
    .tNrAttr = {{0, {AX_COMPRESS_MODE_LOSSLESS, 0}}, {1, {AX_COMPRESS_MODE_LOSSLESS, 0}}},
    .tFrameRateCtrl = {AX_INVALID_FRMRATE, AX_INVALID_FRMRATE},
};

AX_VIN_CHN_ATTR_T gOs08a20Chn0Attr = {
    .nWidth = 3840,
    .nHeight = 2160,
    .nWidthStride = 3840,
    .eImgFormat = AX_FORMAT_YUV420_SEMIPLANAR,
    .nDepth = 2,
    .tCompressInfo = {AX_COMPRESS_MODE_LOSSY, 4},
    .tFrameRateCtrl = {AX_INVALID_FRMRATE, AX_INVALID_FRMRATE},
};

AX_VIN_CHN_ATTR_T gOs08a20Chn1Attr = {
    .nWidth = 1920,
    .nHeight = 1080,
    .nWidthStride = 1920,
    .eImgFormat = AX_FORMAT_YUV420_SEMIPLANAR,
    .nDepth = 2,
    .tCompressInfo = {AX_COMPRESS_MODE_NONE, 0},
    .tFrameRateCtrl = {AX_INVALID_FRMRATE, AX_INVALID_FRMRATE},
};

AX_VIN_CHN_ATTR_T gOs08a20Chn2Attr = {
    .nWidth = 720,
    .nHeight = 576,
    .nWidthStride = 720,
    .eImgFormat = AX_FORMAT_YUV420_SEMIPLANAR,
    .nDepth = 2,
    .tCompressInfo = {AX_COMPRESS_MODE_NONE, 0},
    .tFrameRateCtrl = {AX_INVALID_FRMRATE, AX_INVALID_FRMRATE},
};

AX_MIPI_RX_DEV_T gOs08b10MipiRx = {
    .eInputMode = AX_INPUT_MODE_MIPI,
    .tMipiAttr.ePhyMode = AX_MIPI_PHY_TYPE_DPHY,
    .tMipiAttr.eLaneNum = AX_MIPI_DATA_LANE_4,
    .tMipiAttr.nDataRate =  1440,
    .tMipiAttr.nDataLaneMap[0] = 0,
    .tMipiAttr.nDataLaneMap[1] = 1,
    .tMipiAttr.nDataLaneMap[2] = 2,
    .tMipiAttr.nDataLaneMap[3] = 3,
    .tMipiAttr.nDataLaneMap[4] = -1,
    .tMipiAttr.nDataLaneMap[5] = -1,
    .tMipiAttr.nDataLaneMap[6] = -1,
    .tMipiAttr.nDataLaneMap[7] = -1,
    .tMipiAttr.nClkLane[0]     = 1,
    .tMipiAttr.nClkLane[1]     = 0,
};

AX_SNS_ATTR_T gOs08b10SnsAttr = {
    .nWidth = 3840,
    .nHeight = 2160,
    .fFrameRate = 25.0,
    .eSnsMode = AX_SNS_LINEAR_MODE,
    .eRawType = AX_RT_RAW10,
    .eBayerPattern = AX_BP_RGGB,
    .bTestPatternEnable = AX_FALSE,
};

AX_SNS_CLK_ATTR_T gOs08b10SnsClkAttr = {
    .nSnsClkIdx = 0,
    .eSnsClkRate = AX_SNS_CLK_24M,
};

AX_VIN_DEV_ATTR_T gOs08b10DevAttr = {
    .bImgDataEnable = AX_TRUE,
    .bNonImgDataEnable = AX_FALSE,
    .eDevMode = AX_VIN_DEV_OFFLINE,
    .eSnsIntfType = AX_SNS_INTF_TYPE_MIPI_RAW,
    .tDevImgRgn[0] = {0, 0, 3840, 2160},
    .tDevImgRgn[1] = {0, 0, 3840, 2160},
    .tDevImgRgn[2] = {0, 0, 3840, 2160},
    .tDevImgRgn[3] = {0, 0, 3840, 2160},

    /* When users transfer special data, they need to configure VC&DT for szImgVc/szImgDt/szInfoVc/szInfoDt */
    //.tMipiIntfAttr.szImgVc[0] = 0,
    //.tMipiIntfAttr.szImgDt[0] = MIPI_CSI_DT_RAW10,
    //.tMipiIntfAttr.szImgVc[1] = 1,
    //.tMipiIntfAttr.szImgDt[1] = MIPI_CSI_DT_RAW10,

    .ePixelFmt = AX_FORMAT_BAYER_RAW_10BPP,
    .eBayerPattern = AX_BP_RGGB,
    .eSnsMode = AX_SNS_LINEAR_MODE,
    .eSnsOutputMode = AX_SNS_NORMAL,
    .tCompressInfo = {AX_COMPRESS_MODE_LOSSLESS, 0},
    .tFrameRateCtrl = {AX_INVALID_FRMRATE, AX_INVALID_FRMRATE},
};

AX_VIN_PIPE_ATTR_T gOs08b10PipeAttr = {
    .tPipeImgRgn = {0, 0, 3840, 2160},
    .eBayerPattern = AX_BP_RGGB,
    .ePixelFmt = AX_FORMAT_BAYER_RAW_10BPP,
    .eSnsMode = AX_SNS_LINEAR_MODE,
    .tCompressInfo = {AX_COMPRESS_MODE_LOSSLESS, 0},
    .tNrAttr = {{0, {AX_COMPRESS_MODE_LOSSLESS, 0}}, {1, {AX_COMPRESS_MODE_LOSSLESS, 0}}},
    .tFrameRateCtrl = {AX_INVALID_FRMRATE, AX_INVALID_FRMRATE},
};

AX_VIN_CHN_ATTR_T gOs08b10Chn0Attr = {
    .nWidth = 3840,
    .nHeight = 2160,
    .nWidthStride = 3840,
    .eImgFormat = AX_FORMAT_YUV420_SEMIPLANAR,
    .nDepth = 2,
    .tCompressInfo = {AX_COMPRESS_MODE_LOSSY, 4},
    .tFrameRateCtrl = {AX_INVALID_FRMRATE, AX_INVALID_FRMRATE},
};

AX_VIN_CHN_ATTR_T gOs08b10Chn1Attr = {
    .nWidth = 1920,
    .nHeight = 1080,
    .nWidthStride = 1920,
    .eImgFormat = AX_FORMAT_YUV420_SEMIPLANAR,
    .nDepth = 2,
    .tCompressInfo = {AX_COMPRESS_MODE_NONE, 0},
    .tFrameRateCtrl = {AX_INVALID_FRMRATE, AX_INVALID_FRMRATE},
};

AX_VIN_CHN_ATTR_T gOs08b10Chn2Attr = {
    .nWidth = 720,
    .nHeight = 576,
    .nWidthStride = 720,
    .eImgFormat = AX_FORMAT_YUV420_SEMIPLANAR,
    .nDepth = 2,
    .tCompressInfo = {AX_COMPRESS_MODE_NONE, 0},
    .tFrameRateCtrl = {AX_INVALID_FRMRATE, AX_INVALID_FRMRATE},
};

AX_MIPI_RX_DEV_T gSc910gsMipiRx = {
    .eInputMode = AX_INPUT_MODE_SUBLVDS,
    .tLvdsAttr.eLaneNum = AX_SLVDS_DATA_LANE_8,
    .tLvdsAttr.nDataRate = 750,
    .tLvdsAttr.nDataLaneMap[0] = 0,
    .tLvdsAttr.nDataLaneMap[1] = 1,
    .tLvdsAttr.nDataLaneMap[2] = 2,
    .tLvdsAttr.nDataLaneMap[3] = 3,
    .tLvdsAttr.nDataLaneMap[4] = 4,
    .tLvdsAttr.nDataLaneMap[5] = 5,
    .tLvdsAttr.nDataLaneMap[6] = 6,
    .tLvdsAttr.nDataLaneMap[7] = 7,
    .tLvdsAttr.nClkLane[0]     = 1,
    .tLvdsAttr.nClkLane[1]     = 0,
};

AX_SNS_ATTR_T gSc910gsSnsAttr = {
    .nWidth = 3840,
    .nHeight = 2336,
    .fFrameRate = 50.0,
    .eSnsMode = AX_SNS_LINEAR_MODE,
    .eRawType = AX_RT_RAW10,
    .eBayerPattern = AX_BP_RGGB,
    .bTestPatternEnable = AX_FALSE,
    .eMasterSlaveSel = AX_SNS_SLAVE,
};

AX_SNS_CLK_ATTR_T gSc910gsSnsClkAttr = {
    .nSnsClkIdx = 0,
    .eSnsClkRate = AX_SNS_CLK_24M,
};

AX_VIN_DEV_ATTR_T gSc910gsDevAttr = {
    .bImgDataEnable = AX_TRUE,
    .bNonImgDataEnable = AX_FALSE,
    .eDevMode = AX_VIN_DEV_OFFLINE,
    .eSnsIntfType = AX_SNS_INTF_TYPE_SUB_LVDS,
    .tDevImgRgn[0] = {0, 1, 3840, 2336},
    .tDevImgRgn[1] = {0, 1, 3840, 2336},
    .tDevImgRgn[2] = {0, 1, 3840, 2336},
    .tDevImgRgn[3] = {0, 1, 3840, 2336},
    .tLvdsIntfAttr.nLineNum = 8,
    .tLvdsIntfAttr.nHispiFirCodeEn= 0,
    .tLvdsIntfAttr.nContSyncCodeMatchEn = 1,
    .tLvdsIntfAttr.eSyncMode = AX_VIN_LVDS_SYNC_MODE_SOF_EOF_NORMAL,
    .tLvdsIntfAttr.szSyncCode[0][0][0] = 0xab00,
    .tLvdsIntfAttr.szSyncCode[0][0][1] = 0x8000,
    .tLvdsIntfAttr.szSyncCode[1][0][0] = 0xab00,
    .tLvdsIntfAttr.szSyncCode[1][0][1] = 0x8000,
    .tLvdsIntfAttr.szSyncCode[2][0][0] = 0xab00,
    .tLvdsIntfAttr.szSyncCode[2][0][1] = 0x8000,
    .tLvdsIntfAttr.szSyncCode[3][0][0] = 0xab00,
    .tLvdsIntfAttr.szSyncCode[3][0][1] = 0x8000,
    .ePixelFmt = AX_FORMAT_BAYER_RAW_10BPP,
    .eBayerPattern = AX_BP_RGGB,
    .eSnsMode = AX_SNS_LINEAR_MODE,
    .eSnsOutputMode = AX_SNS_NORMAL,
    .tCompressInfo = {AX_COMPRESS_MODE_LOSSLESS, 0},
};

AX_VIN_PIPE_ATTR_T gSc910gsPipeAttr = {
    .tPipeImgRgn = {0, 0, 3840, 2336},
    .eBayerPattern = AX_BP_RGGB,
    .ePixelFmt = AX_FORMAT_BAYER_RAW_16BPP,
    .eSnsMode = AX_SNS_LINEAR_MODE,
    .tCompressInfo = {AX_COMPRESS_MODE_LOSSLESS, 0},
    .tNrAttr = {{0, {AX_COMPRESS_MODE_LOSSLESS, 0}}, {1, {AX_COMPRESS_MODE_LOSSLESS, 0}}},
};

AX_VIN_CHN_ATTR_T gSc910gsChn0Attr = {
    .nWidth = 3840,
    .nHeight = 2336,
    .nWidthStride = 3840,
    .eImgFormat = AX_FORMAT_YUV420_SEMIPLANAR,
    .nDepth = 2,
    .tCompressInfo = {AX_COMPRESS_MODE_LOSSY, 4},
};

AX_VIN_CHN_ATTR_T gSc910gsChn1Attr = {
    .nWidth = 1920,
    .nHeight = 1080,
    .nWidthStride = 1920,
    .eImgFormat = AX_FORMAT_YUV420_SEMIPLANAR,
    .nDepth = 2,
    .tCompressInfo = {AX_COMPRESS_MODE_NONE, 0},
};

AX_VIN_CHN_ATTR_T gSc910gsChn2Attr = {
    .nWidth = 720,
    .nHeight = 576,
    .nWidthStride = 720,
    .eImgFormat = AX_FORMAT_YUV420_SEMIPLANAR,
    .nDepth = 2,
    .tCompressInfo = {AX_COMPRESS_MODE_NONE, 0},
};

AX_VIN_POWER_SYNC_ATTR_T  gSc910gsPowerAttr =  {
    .ePowerTriggerMode = AX_VIN_SYNC_OUTSIDE_ELEC_ADAPTIVE_TRIGGER,
    .nGpioElecInPin = 22,
    .nGpioSyncOutPin = 135,
    .nFollowCycle = 3,
    .nFreqTolLeft = 2,
    .nFreqTolRight = 2,
    .nElecFreq = 50,
    .nSyncTriggerFreq = 50,
    .nSyncDelayElcUs = 2000,
    .nStrobeGpioNum[0] = 90,
    .nStrobeGpioNum[1] = 89,
    .nStrobeGpioNum[2] = 165,
    .nStrobeGpioNum[3] = 91,
    .nStrobeGpioNum[4] = 92,
    .nStrobeGpioNum[5] = 0xff,
    .nStrobeGpioNum[6] = 0xff,
};

AX_VIN_SENSOR_VTS_T  gSc910gsSensorVts =  {
    .spi_bus = 0,
    .spi_csn = 0,
    .devid = 0x7f,
    .nRegValueMask = {0xFF00, 0xFF, 0},
    .nRegAddr = {0x320E, 0x320F, 0},
};

AX_VIN_SYNC_SIGNAL_ATTR_T gSc910gsVsyncAttr = {
    .nSyncIdx = 0,
    .eSyncInv = AX_VIN_SYNC_POLARITY_HIGH,
    .nSyncFreq = 2700,
    .nSyncDutyRatio = 1,
};

AX_VIN_SYNC_SIGNAL_ATTR_T gSc910gsHsyncAttr = {
    .nSyncIdx = 0,
    .eSyncInv = AX_VIN_SYNC_POLARITY_HIGH,
    .nSyncFreq = 7407,
    .nSyncDutyRatio = 1,
};

AX_VIN_LIGHT_SYNC_INFO_T gSc910gsLightSyncInfo = {
    .nVts = 2700,
    .nHts = 7407,
    .nElecToVsyncTime = 1350,
    .nVbbTime = 100,
    .tFrmLengthRegInfo.bEnable = 1,
    .tFrmLengthRegInfo.nRegValueMask[0] = 0xFF00,
    .tFrmLengthRegInfo.nRegAddr[0] = 0x320E,
    .tFrmLengthRegInfo.nRegValueMask[1] = 0x00FF,
    .tFrmLengthRegInfo.nRegAddr[1] = 0x320F,
    .szShutterMap[0].eShutterMode = AX_VIN_SHUTTER_MODE_VIDEO,
    .szShutterMap[0].nPipeId = 0,
    .szShutterMap[1].eShutterMode = AX_VIN_SHUTTER_MODE_PICTURE,
    .szShutterMap[1].nPipeId = 1,
    .szShutterMap[2].eShutterMode = AX_VIN_SHUTTER_MODE_FLASH_SNAP,
    .szShutterMap[2].nPipeId = 2,
    .szShutterMode[0] = AX_VIN_SHUTTER_MODE_VIDEO,
    .szShutterMode[1] = AX_VIN_SHUTTER_MODE_PICTURE,
};

#define LIGHT_STROBE (2)
AX_VIN_STROBE_LIGHT_TIMING_ATTR_T gSc910gsSnapStrobeAttr = {
    .eStrobeSyncInv = AX_VIN_SYNC_POLARITY_HIGH,
    .nStrobeDutyTime = 200,
    .nStrobeDelayTime = 100,
    .fStrobeFreqRatio = 2,
};

#define LIGHT_FLASH (2)
AX_VIN_FLASH_LIGHT_TIMING_ATTR_T gSc910gsSnapFlashAttr = {
    .eFlashSyncInv = AX_VIN_SYNC_POLARITY_HIGH,
    .nFlashDutyTime = 20,
    .nFlashDelayTime = 1430,
};

/*******************sc410gs start*******************/
AX_MIPI_RX_DEV_T gSc410gsMipiRx = {
    .eInputMode = AX_INPUT_MODE_MIPI,
    .tMipiAttr.ePhyMode = AX_MIPI_PHY_TYPE_DPHY,
    .tMipiAttr.eLaneNum = AX_MIPI_DATA_LANE_4,
    .tMipiAttr.nDataRate =  704,
    .tMipiAttr.nDataLaneMap[0] = 0,
    .tMipiAttr.nDataLaneMap[1] = 1,
    .tMipiAttr.nDataLaneMap[2] = 2,
    .tMipiAttr.nDataLaneMap[3] = 3,
    .tMipiAttr.nDataLaneMap[4] = -1,
    .tMipiAttr.nDataLaneMap[5] = -1,
    .tMipiAttr.nDataLaneMap[6] = -1,
    .tMipiAttr.nDataLaneMap[7] = -1,
    .tMipiAttr.nClkLane[0]     = 1,
    .tMipiAttr.nClkLane[1]     = 0,
};

AX_SNS_ATTR_T gSc410gsSnsAttr = {
    .nWidth = 1744,
    .nHeight = 2336,
    .fFrameRate = 50.0,
    .eSnsMode = AX_SNS_LINEAR_MODE,
    .eRawType = AX_RT_RAW10,
    .eBayerPattern = AX_BP_RGGB,
    .bTestPatternEnable = AX_FALSE,
    .eMasterSlaveSel = AX_SNS_SLAVE,
};

AX_SNS_CLK_ATTR_T gSc410gsSnsClkAttr = {
    .nSnsClkIdx = 0,
    .eSnsClkRate = AX_SNS_CLK_24M,
};

AX_VIN_DEV_ATTR_T gSc410gsDevAttr = {
    .bImgDataEnable = AX_TRUE,
    .bNonImgDataEnable = AX_FALSE,
    .eDevMode = AX_VIN_DEV_OFFLINE,
    .eSnsIntfType = AX_SNS_INTF_TYPE_MIPI_RAW,
    .tDevImgRgn[0] = {0, 0, 1744, 2336},
    .tDevImgRgn[1] = {0, 0, 1744, 2336},
    .tDevImgRgn[2] = {0, 0, 1744, 2336},
    .tDevImgRgn[3] = {0, 0, 1744, 2336},
    .nWidthStride[0] = 1792,
    .nWidthStride[1] = 1792,
    .nWidthStride[2] = 1792,
    .nWidthStride[3] = 1792,
    // .tLvdsIntfAttr.nLineNum = 8,
    // .tLvdsIntfAttr.nHispiFirCodeEn= 0,
    // .tLvdsIntfAttr.nContSyncCodeMatchEn = 1,
    // .tLvdsIntfAttr.eSyncMode = AX_VIN_LVDS_SYNC_MODE_SOF_EOF_NORMAL,
    // .tLvdsIntfAttr.szSyncCode[0][0][0] = 0xab00,
    // .tLvdsIntfAttr.szSyncCode[0][0][1] = 0x8000,
    // .tLvdsIntfAttr.szSyncCode[1][0][0] = 0xab00,
    // .tLvdsIntfAttr.szSyncCode[1][0][1] = 0x8000,
    // .tLvdsIntfAttr.szSyncCode[2][0][0] = 0xab00,
    // .tLvdsIntfAttr.szSyncCode[2][0][1] = 0x8000,
    // .tLvdsIntfAttr.szSyncCode[3][0][0] = 0xab00,
    // .tLvdsIntfAttr.szSyncCode[3][0][1] = 0x8000,
    .ePixelFmt = AX_FORMAT_BAYER_RAW_10BPP,
    .eBayerPattern = AX_BP_RGGB,
    .eSnsMode = AX_SNS_LINEAR_MODE,
    .eSnsOutputMode = AX_SNS_NORMAL,
    .tCompressInfo = {AX_COMPRESS_MODE_LOSSLESS, 0},
};

AX_VIN_PIPE_ATTR_T gSc410gsPipeAttr = {
    .tPipeImgRgn = {0, 0, 1744, 2336},
    .nWidthStride = 1792,
    .eBayerPattern = AX_BP_RGGB,
    .ePixelFmt = AX_FORMAT_BAYER_RAW_16BPP,
    .eSnsMode = AX_SNS_LINEAR_MODE,
    .tCompressInfo = {AX_COMPRESS_MODE_LOSSLESS, 0},
    .tNrAttr = {{0, {AX_COMPRESS_MODE_LOSSLESS, 0}}, {1, {AX_COMPRESS_MODE_LOSSLESS, 0}}},
};

AX_VIN_CHN_ATTR_T gSc410gsChn0Attr = {
    .nWidth = 1744,
    .nHeight = 2336,
    .nWidthStride = 1744,
    .eImgFormat = AX_FORMAT_YUV420_SEMIPLANAR,
    .nDepth = 2,
    .tCompressInfo = {AX_COMPRESS_MODE_LOSSY, 4},
};

AX_VIN_CHN_ATTR_T gSc410gsChn1Attr = {
    .nWidth = 1280,
    .nHeight = 720,
    .nWidthStride = 1280,
    .eImgFormat = AX_FORMAT_YUV420_SEMIPLANAR,
    .nDepth = 2,
    .tCompressInfo = {AX_COMPRESS_MODE_NONE, 0},
};

AX_VIN_CHN_ATTR_T gSc410gsChn2Attr = {
    .nWidth = 720,
    .nHeight = 640,
    .nWidthStride = 720,
    .eImgFormat = AX_FORMAT_YUV420_SEMIPLANAR,
    .nDepth = 2,
    .tCompressInfo = {AX_COMPRESS_MODE_NONE, 0},
};

AX_VIN_POWER_SYNC_ATTR_T  gSc410gsPowerAttr =  {
    .ePowerTriggerMode = AX_VIN_SYNC_OUTSIDE_ELEC_ADAPTIVE_TRIGGER,
    .nGpioElecInPin = 22,
    .nGpioSyncOutPin = 135,
    .nFollowCycle = 3,
    .nFreqTolLeft = 2,
    .nFreqTolRight = 2,
    .nElecFreq = 50,
    .nSyncTriggerFreq = 50,
    .nSyncDelayElcUs = 2000,
    .nStrobeGpioNum[0] = 90,
    .nStrobeGpioNum[1] = 89,
    .nStrobeGpioNum[2] = 165,
    .nStrobeGpioNum[3] = 91,
    .nStrobeGpioNum[4] = 92,
    .nStrobeGpioNum[5] = 0xff,
    .nStrobeGpioNum[6] = 0xff,
};

AX_VIN_SYNC_SIGNAL_ATTR_T gSc410gsVsyncAttr = {
    .nSyncIdx = 0,
    .eSyncInv = AX_VIN_SYNC_POLARITY_HIGH,
    .nSyncFreq = 2607,
    .nSyncDutyRatio = 1,
};

AX_VIN_SYNC_SIGNAL_ATTR_T gSc410gsHsyncAttr = {
    .nSyncIdx = 0,
    .eSyncInv = AX_VIN_SYNC_POLARITY_HIGH,
    .nSyncFreq = 7671,
    .nSyncDutyRatio = 1,
};

AX_VIN_LIGHT_SYNC_INFO_T gSc410gsLightSyncInfo = {
    .nVts = 2607,
    .nHts = 7671,
    .nElecToVsyncTime = 1350,
    .nVbbTime = 100,
    .tFrmLengthRegInfo.bEnable = 1,
    .tFrmLengthRegInfo.nRegValueMask[0] = 0xFF00,
    .tFrmLengthRegInfo.nRegAddr[0] = 0x320E,
    .tFrmLengthRegInfo.nRegValueMask[1] = 0x00FF,
    .tFrmLengthRegInfo.nRegAddr[1] = 0x320F,
    .szShutterMap[0].eShutterMode = AX_VIN_SHUTTER_MODE_VIDEO,
    .szShutterMap[0].nPipeId = 0,
    .szShutterMap[1].eShutterMode = AX_VIN_SHUTTER_MODE_PICTURE,
    .szShutterMap[1].nPipeId = 1,
    .szShutterMap[2].eShutterMode = AX_VIN_SHUTTER_MODE_FLASH_SNAP,
    .szShutterMap[2].nPipeId = 2,
    .szShutterMode[0] = AX_VIN_SHUTTER_MODE_VIDEO,
    .szShutterMode[1] = AX_VIN_SHUTTER_MODE_PICTURE,

};

#define LIGHT_STROBE (2)
AX_VIN_STROBE_LIGHT_TIMING_ATTR_T gSc410gsSnapStrobeAttr = {
    .eStrobeSyncInv = AX_VIN_SYNC_POLARITY_LOW,
    .nStrobeDutyTime = 200,
    .nStrobeDelayTime = 100,
    .fStrobeFreqRatio = 2,
};

#define LIGHT_FLASH (2)
AX_VIN_FLASH_LIGHT_TIMING_ATTR_T gSc410gsSnapFlashAttr = {
    .eFlashSyncInv = AX_VIN_SYNC_POLARITY_HIGH,
    .nFlashDutyTime = 20,
    .nFlashDelayTime = 1430,
};
/*******************sc410gs end*******************/

AX_MIPI_RX_DEV_T gOs04a10Lane2MipiRx = {
    .eInputMode = AX_INPUT_MODE_MIPI,
    .tMipiAttr.ePhyMode = AX_MIPI_PHY_TYPE_DPHY,
    .tMipiAttr.eLaneNum = AX_MIPI_DATA_LANE_2,
    .tMipiAttr.nDataRate =  720,
    .tMipiAttr.nDataLaneMap[0] = 0,
    .tMipiAttr.nDataLaneMap[1] = 1,
    .tMipiAttr.nDataLaneMap[2] = -1,
    .tMipiAttr.nDataLaneMap[3] = -1,
    .tMipiAttr.nDataLaneMap[4] = -1,
    .tMipiAttr.nDataLaneMap[5] = -1,
    .tMipiAttr.nDataLaneMap[6] = -1,
    .tMipiAttr.nDataLaneMap[7] = -1,
    .tMipiAttr.nClkLane[0]     = 1,
    .tMipiAttr.nClkLane[1]     = 0,
};

AX_SNS_ATTR_T gOs04a10Lane2SnsAttr = {
    .nWidth = 2688,
    .nHeight = 1520,
    .fFrameRate = 25.0,
    .eSnsMode = AX_SNS_LINEAR_MODE,
    .eRawType = AX_RT_RAW10,
    .eBayerPattern = AX_BP_RGGB,
    .bTestPatternEnable = AX_FALSE,
    .eSnsOutputMode = AX_SNS_NORMAL,
    .eMasterSlaveSel = AX_SNS_SYNC_SLAVE,
    .nSettingIndex  =36,
};

AX_MIPI_RX_DEV_T gOs04a10MipiRx = {
    .eInputMode = AX_INPUT_MODE_MIPI,
    .tMipiAttr.ePhyMode = AX_MIPI_PHY_TYPE_DPHY,
    .tMipiAttr.eLaneNum = AX_MIPI_DATA_LANE_4,
    .tMipiAttr.nDataRate =  400,
    .tMipiAttr.nDataLaneMap[0] = 0,
    .tMipiAttr.nDataLaneMap[1] = 1,
    .tMipiAttr.nDataLaneMap[2] = 2,
    .tMipiAttr.nDataLaneMap[3] = 3,
    .tMipiAttr.nDataLaneMap[4] = -1,
    .tMipiAttr.nDataLaneMap[5] = -1,
    .tMipiAttr.nDataLaneMap[6] = -1,
    .tMipiAttr.nDataLaneMap[7] = -1,
    .tMipiAttr.nClkLane[0]     = 1,
    .tMipiAttr.nClkLane[1]     = 0,
};

AX_SNS_ATTR_T gOs04a10SnsAttr = {
    .nWidth = 2688,
    .nHeight = 1520,
    .fFrameRate = 30.0,
    .eSnsMode = AX_SNS_HDR_3X_MODE,
    .eRawType = AX_RT_RAW10,
    .eBayerPattern = AX_BP_RGGB,
    .bTestPatternEnable = AX_FALSE,
    .eSnsOutputMode = AX_SNS_DCG_HDR,
};

AX_SNS_CLK_ATTR_T gOs04a10SnsClkAttr = {
    .nSnsClkIdx = 0,
    .eSnsClkRate = AX_SNS_CLK_24M,
};

AX_VIN_DEV_ATTR_T gOs04a10DevAttr = {
    .bImgDataEnable = AX_TRUE,
    .bNonImgDataEnable = AX_FALSE,
    .eDevMode = AX_VIN_DEV_OFFLINE,
    .eSnsIntfType = AX_SNS_INTF_TYPE_MIPI_RAW,
    .tDevImgRgn[0] = {0, 0, 2688, 1520},
    .tDevImgRgn[1] = {0, 0, 2688, 1520},
    .tDevImgRgn[2] = {0, 0, 2688, 1520},
    .tDevImgRgn[3] = {0, 0, 2688, 1520},

    /* When users transfer special data, they need to configure VC&DT for szImgVc/szImgDt/szInfoVc/szInfoDt */
    //.tMipiIntfAttr.szImgVc[0] = 0,
    //.tMipiIntfAttr.szImgDt[0] = MIPI_CSI_DT_RAW10,
    //.tMipiIntfAttr.szImgVc[1] = 1,
    //.tMipiIntfAttr.szImgDt[1] = MIPI_CSI_DT_RAW10,

    .ePixelFmt = AX_FORMAT_BAYER_RAW_10BPP,
    .eBayerPattern = AX_BP_RGGB,
    .eSnsMode = AX_SNS_HDR_2X_MODE,
    .eSnsOutputMode = AX_SNS_DCG_HDR,
    .tCompressInfo = {AX_COMPRESS_MODE_LOSSLESS, 0},
    .tFrameRateCtrl = {AX_INVALID_FRMRATE, AX_INVALID_FRMRATE},
};

AX_VIN_PIPE_ATTR_T gOs04a10PipeAttr = {
    .tPipeImgRgn = {0, 0, 2688, 1520},
    .eBayerPattern = AX_BP_RGGB,
    .ePixelFmt = AX_FORMAT_BAYER_RAW_10BPP,
    .eSnsMode = AX_SNS_HDR_2X_MODE,
    .tCompressInfo = {AX_COMPRESS_MODE_LOSSLESS, 0},
    .tNrAttr = {{0, {AX_COMPRESS_MODE_LOSSLESS, 0}}, {1, {AX_COMPRESS_MODE_LOSSLESS, 0}}},
    .tFrameRateCtrl = {AX_INVALID_FRMRATE, AX_INVALID_FRMRATE},
};

AX_VIN_CHN_ATTR_T gOs04a10Chn0Attr = {
    .nWidth = 2688,
    .nHeight = 1520,
    .nWidthStride = 2688,
    .eImgFormat = AX_FORMAT_YUV420_SEMIPLANAR,
    .nDepth = 2,
    .tCompressInfo = {AX_COMPRESS_MODE_LOSSY, 4},
    .tFrameRateCtrl = {AX_INVALID_FRMRATE, AX_INVALID_FRMRATE},
};

AX_VIN_CHN_ATTR_T gOs04a10Chn1Attr = {
    .nWidth = 1920,
    .nHeight = 1080,
    .nWidthStride = 1920,
    .eImgFormat = AX_FORMAT_YUV420_SEMIPLANAR,
    .nDepth = 2,
    .tCompressInfo = {AX_COMPRESS_MODE_NONE, 0},
    .tFrameRateCtrl = {AX_INVALID_FRMRATE, AX_INVALID_FRMRATE},
};

AX_VIN_CHN_ATTR_T gOs04a10Chn2Attr = {
    .nWidth = 720,
    .nHeight = 576,
    .nWidthStride = 720,
    .eImgFormat = AX_FORMAT_YUV420_SEMIPLANAR,
    .nDepth = 2,
    .tCompressInfo = {AX_COMPRESS_MODE_NONE, 0},
    .tFrameRateCtrl = {AX_INVALID_FRMRATE, AX_INVALID_FRMRATE},
};

AX_MIPI_RX_DEV_T gOv48c40MipiRx = {
    .eInputMode = AX_INPUT_MODE_MIPI,
    .tMipiAttr.ePhyMode = AX_MIPI_PHY_TYPE_DPHY,
    .tMipiAttr.eLaneNum = AX_MIPI_DATA_LANE_4,
    .tMipiAttr.nDataRate =  2495,
    .tMipiAttr.nDataLaneMap[0] = 0,
    .tMipiAttr.nDataLaneMap[1] = 1,
    .tMipiAttr.nDataLaneMap[2] = 2,
    .tMipiAttr.nDataLaneMap[3] = 3,
    .tMipiAttr.nDataLaneMap[4] = -1,
    .tMipiAttr.nDataLaneMap[5] = -1,
    .tMipiAttr.nDataLaneMap[6] = -1,
    .tMipiAttr.nDataLaneMap[7] = -1,
    .tMipiAttr.nClkLane[0]     = 1,
    .tMipiAttr.nClkLane[1]     = 0,
};

AX_SNS_ATTR_T gOv48c40SnsAttr = {
    .nWidth = 8000,
    .nHeight = 6000,
    .fFrameRate = 15.0,
    .eSnsMode = AX_SNS_LINEAR_MODE,
    .eRawType = AX_RT_RAW10,
    .eBayerPattern = AX_BP_RGGB,
    .bTestPatternEnable = AX_FALSE,
};

AX_SNS_CLK_ATTR_T gOv48c40SnsClkAttr = {
    .nSnsClkIdx = 1,
    .eSnsClkRate = AX_SNS_CLK_24M,
};

AX_VIN_DEV_ATTR_T gOv48c40DevAttr = {
    .bImgDataEnable = AX_TRUE,
    .bNonImgDataEnable = AX_FALSE,
    .eDevMode = AX_VIN_DEV_OFFLINE,
    .eSnsIntfType = AX_SNS_INTF_TYPE_MIPI_RAW,
    .tDevImgRgn[0] = {0, 0, 8000, 6000},
    .tDevImgRgn[1] = {0, 0, 8000, 6000},
    .tDevImgRgn[2] = {0, 0, 8000, 6000},
    .tDevImgRgn[3] = {0, 0, 8000, 6000},

    .ePixelFmt = AX_FORMAT_BAYER_RAW_10BPP,
    .eBayerPattern = AX_BP_RGGB,
    .eSnsMode = AX_SNS_LINEAR_MODE,
    .eSnsOutputMode = AX_SNS_NORMAL,
    .tCompressInfo = {AX_COMPRESS_MODE_NONE, 0},
    .tFrameRateCtrl = {AX_INVALID_FRMRATE, AX_INVALID_FRMRATE},
};

AX_VIN_PIPE_ATTR_T gOv48c40PipeAttr = {
    .tPipeImgRgn = {0, 0, 8000, 6000},
    .eBayerPattern = AX_BP_RGGB,
    .ePixelFmt = AX_FORMAT_BAYER_RAW_10BPP,
    .eSnsMode = AX_SNS_LINEAR_MODE,
    .tCompressInfo = {AX_COMPRESS_MODE_NONE, 0},
    .tNrAttr = {{0, {AX_COMPRESS_MODE_NONE, 0}}, {1, {AX_COMPRESS_MODE_LOSSLESS, 0}}},
    .tFrameRateCtrl = {AX_INVALID_FRMRATE, AX_INVALID_FRMRATE},
};

AX_VIN_CHN_ATTR_T gOv48c40Chn0Attr = {
    .nWidth = 8000,
    .nHeight = 6000,
    .nWidthStride = 8000,
    .eImgFormat = AX_FORMAT_YUV420_SEMIPLANAR,
    .nDepth = 2,
    .tCompressInfo = {AX_COMPRESS_MODE_NONE, 0},
    .tFrameRateCtrl = {AX_INVALID_FRMRATE, AX_INVALID_FRMRATE},
};

AX_VIN_CHN_ATTR_T gOv48c40Chn1Attr = {
    .nWidth = 1920,
    .nHeight = 1080,
    .nWidthStride = 1920,
    .eImgFormat = AX_FORMAT_YUV420_SEMIPLANAR,
    .nDepth = 2,
    .tCompressInfo = {AX_COMPRESS_MODE_NONE, 0},
    .tFrameRateCtrl = {AX_INVALID_FRMRATE, AX_INVALID_FRMRATE},
};

AX_VIN_CHN_ATTR_T gOv48c40Chn2Attr = {
    .nWidth = 720,
    .nHeight = 576,
    .nWidthStride = 720,
    .eImgFormat = AX_FORMAT_YUV420_SEMIPLANAR,
    .nDepth = 2,
    .tCompressInfo = {AX_COMPRESS_MODE_NONE, 0},
    .tFrameRateCtrl = {AX_INVALID_FRMRATE, AX_INVALID_FRMRATE},
};

AX_MIPI_RX_DEV_T gSc450aiMipiRx = {
    .eInputMode = AX_INPUT_MODE_MIPI,
    .tMipiAttr.ePhyMode = AX_MIPI_PHY_TYPE_DPHY,
    .tMipiAttr.eLaneNum = AX_MIPI_DATA_LANE_2,
    .tMipiAttr.nDataRate =  720,
    .tMipiAttr.nDataLaneMap[0] = 0,
    .tMipiAttr.nDataLaneMap[1] = 1,
    .tMipiAttr.nDataLaneMap[2] = -1,
    .tMipiAttr.nDataLaneMap[3] = -1,
    .tMipiAttr.nDataLaneMap[4] = -1,
    .tMipiAttr.nDataLaneMap[5] = -1,
    .tMipiAttr.nDataLaneMap[6] = -1,
    .tMipiAttr.nDataLaneMap[7] = -1,
    .tMipiAttr.nClkLane[0]     = 1,
    .tMipiAttr.nClkLane[1]     = 0,
};

AX_SNS_ATTR_T gSc450aiSnsAttr = {
    .nWidth = 2688,
    .nHeight = 1520,
    .fFrameRate = 25,
    .eSnsMode = AX_SNS_LINEAR_MODE,
    .eRawType = AX_RT_RAW10,
    .eBayerPattern = AX_BP_RGGB,
    .bTestPatternEnable = AX_FALSE,
};

AX_SNS_CLK_ATTR_T gSc450aiSnsClkAttr = {
    .nSnsClkIdx = 0,
    .eSnsClkRate = AX_SNS_CLK_24M,
};

AX_VIN_DEV_ATTR_T gSc450aiDevAttr = {
    .bImgDataEnable = AX_TRUE,
    .bNonImgDataEnable = AX_FALSE,
    .eDevMode = AX_VIN_DEV_OFFLINE,
    .eSnsIntfType = AX_SNS_INTF_TYPE_MIPI_RAW,
    .tDevImgRgn[0] = {0, 0, 2688, 1520},
    .tDevImgRgn[1] = {0, 0, 2688, 1520},
    .tDevImgRgn[2] = {0, 0, 2688, 1520},
    .tDevImgRgn[3] = {0, 0, 2688, 1520},

    /* When users transfer special data, they need to configure VC&DT for szImgVc/szImgDt/szInfoVc/szInfoDt */
    //.tMipiIntfAttr.szImgVc[0] = 0,
    //.tMipiIntfAttr.szImgDt[0] = MIPI_CSI_DT_RAW10,
    //.tMipiIntfAttr.szImgVc[1] = 1,
    //.tMipiIntfAttr.szImgDt[1] = MIPI_CSI_DT_RAW10,

    .ePixelFmt = AX_FORMAT_BAYER_RAW_10BPP,
    .eBayerPattern = AX_BP_RGGB,
    .eSnsMode = AX_SNS_LINEAR_MODE,
    .eSnsOutputMode = AX_SNS_NORMAL,
    .tCompressInfo = {AX_COMPRESS_MODE_LOSSLESS, 0},
    .tFrameRateCtrl = {AX_INVALID_FRMRATE, AX_INVALID_FRMRATE},
};

AX_VIN_PIPE_ATTR_T gSc450aiPipeAttr = {
    .tPipeImgRgn = {0, 0, 2688, 1520},
    .eBayerPattern = AX_BP_RGGB,
    .ePixelFmt = AX_FORMAT_BAYER_RAW_10BPP,
    .eSnsMode = AX_SNS_LINEAR_MODE,
    .tCompressInfo = {AX_COMPRESS_MODE_LOSSLESS, 0},
    .tNrAttr = {{0, {AX_COMPRESS_MODE_LOSSLESS, 0}}, {1, {AX_COMPRESS_MODE_LOSSLESS, 0}}},
    .tFrameRateCtrl = {AX_INVALID_FRMRATE, AX_INVALID_FRMRATE},
};

AX_VIN_CHN_ATTR_T gSc450aiChn0Attr = {
    .nWidth = 2688,
    .nHeight = 1520,
    .nWidthStride = 2688,
    .eImgFormat = AX_FORMAT_YUV420_SEMIPLANAR,
    .nDepth = 2,
    .tCompressInfo = {AX_COMPRESS_MODE_LOSSY, 4},
    .tFrameRateCtrl = {AX_INVALID_FRMRATE, AX_INVALID_FRMRATE},
};

AX_VIN_CHN_ATTR_T gSc450aiChn1Attr = {
    .nWidth = 1920,
    .nHeight = 1080,
    .nWidthStride = 1920,
    .eImgFormat = AX_FORMAT_YUV420_SEMIPLANAR,
    .nDepth = 2,
    .tCompressInfo = {AX_COMPRESS_MODE_NONE, 0},
    .tFrameRateCtrl = {AX_INVALID_FRMRATE, AX_INVALID_FRMRATE},
};

AX_VIN_CHN_ATTR_T gSc450aiChn2Attr = {
    .nWidth = 720,
    .nHeight = 576,
    .nWidthStride = 720,
    .eImgFormat = AX_FORMAT_YUV420_SEMIPLANAR,
    .nDepth = 2,
    .tCompressInfo = {AX_COMPRESS_MODE_NONE, 0},
    .tFrameRateCtrl = {AX_INVALID_FRMRATE, AX_INVALID_FRMRATE},
};
#endif //_COMMON_CONFIG_H__

/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#pragma once
#include <string.h>
#include <string>
#include <vector>
#include "ax_isp_api.h"
#include "ax_mipi_rx_api.h"
#include "ax_vin_api.h"
#include "ax_isp_3a_plus.h"

#define MAX_PIPE_PER_DEVICE (3)
#define LIGHT_STROBE (0)
#define LIGHT_FLASH (2)
#define INVALID_PIPE_ID  ((AX_U8)(-1))
#define INVALID_I2C_ADDR ((AX_U8)(-1))

#define IS_PANO_SENSOR(eSensorType) (((eSensorType >= E_SNS_TYPE_OS04A10x2_PANO) && (eSensorType < E_SNS_TYPE_PANO_END)) ? AX_TRUE : AX_FALSE)

using std::string;
using std::vector;

typedef enum {
    /* New sensors should be append to the last */
    E_SNS_TYPE_OS04A10 = 0,
    E_SNS_TYPE_IMX334,
    E_SNS_TYPE_GC4653,
    E_SNS_TYPE_OS08A20,
    E_SNS_TYPE_OS08B10,
    E_SNS_TYPE_SC910GS,
    E_SNS_TYPE_SC1345, /* Not supported */
    E_SNS_TYPE_SC530AI,
    E_SNS_TYPE_SC230AI,
    E_SNS_TYPE_QUAD_OS04A10,
    E_SNS_TYPE_OS04A10_DCG,
    E_SNS_TYPE_OS04A10_VS,
    E_SNS_TYPE_SC410GS,
    E_SNS_TYPE_QUAD_OS08A20,
    E_SNS_TYPE_SC500AI,
    E_SNS_TYPE_QUAD_IMX678,
    /* HDR Split Mode: Long Frame */
    E_SNS_TYPE_LF_START = 100,
    E_SNS_TYPE_OS04A10_LF,
    E_SNS_TYPE_SC530AI_LF,
    E_SNS_TYPE_SC230AI_LF,
    E_SNS_TYPE_OS08A20_LF,
    E_SNS_TYPE_LF_END = 151,
    /* HDR Split Mode: Short Frame */
    E_SNS_TYPE_SF_START = 152,
    E_SNS_TYPE_OS04A10_SF,
    E_SNS_TYPE_SC530AI_SF,
    E_SNS_TYPE_SC230AI_SF,
    E_SNS_TYPE_OS08A20_SF,
    E_SNS_TYPE_SF_END = 203,
    /* PANO */
    E_SNS_TYPE_OS04A10x2_PANO = 204,
    E_SNS_TYPE_OS04A10x8_PANO = 205,
    E_SNS_TYPE_SC450AIx8_PANO = 206,
    E_SNS_TYPE_PANO_END = 254,
    E_SNS_TYPE_MAX = 255,
} SNS_TYPE_E;

typedef enum {
    E_DEPURPLE_MODE_NOCHANGE = -1,
    E_DEPURPLE_MODE_OFF,
    E_DEPURPLE_MODE_ON,
} DEPURPLE_MODE_E;

typedef struct axSNS_ABILITY_T {
    AX_BOOL bSupportHDR;
    AX_BOOL bSupportHDRSplit;
    axSNS_ABILITY_T(AX_VOID) {
        bSupportHDR = AX_TRUE;
        bSupportHDRSplit = AX_TRUE;
    }
} SNS_ABILITY_T;

typedef struct axSNS_CLK_ATTR_T {
    AX_U8 nSnsClkIdx;
    AX_SNS_CLK_RATE_E eSnsClkRate;

    axSNS_CLK_ATTR_T(AX_VOID) {
        nSnsClkIdx = 0;
        eSnsClkRate = AX_SNS_CLK_24M;
    }
} SNS_CLK_ATTR_T;

typedef struct axSNS_SNAP_ATTR_T {
    AX_VIN_POWER_SYNC_ATTR_T tPowerAttr;
    AX_VIN_SYNC_SIGNAL_ATTR_T tVsyncAttr;
    AX_VIN_SYNC_SIGNAL_ATTR_T tHsyncAttr;
    AX_VIN_LIGHT_SYNC_INFO_T tLightSyncAttr;
    AX_VIN_STROBE_LIGHT_TIMING_ATTR_T tStrobeAttr;
    AX_VIN_FLASH_LIGHT_TIMING_ATTR_T tFlashAttr;

    axSNS_SNAP_ATTR_T(AX_VOID) {
        memset(this, 0, sizeof(*this));
    }
} SNS_SNAP_ATTR_T;

typedef struct axISP_ALGO_INFO_T {
    AX_BOOL bActiveAe;
    AX_BOOL bUserAe;
    AX_BOOL bActiveAwb;
    AX_BOOL bUserAwb;
    AX_BOOL bActiveLsc;
    AX_BOOL bUserLsc;
    AX_ISP_AE_REGFUNCS_T tAeFns;
    AX_ISP_AWB_REGFUNCS_T tAwbFns;
    AX_ISP_LSC_REGFUNCS_T tLscFns;

    axISP_ALGO_INFO_T(AX_VOID) {
        memset(this, 0, sizeof(*this));
    }
} ISP_ALGO_INFO_T;

typedef struct _SENSOR_PIPE_MAPPING {
    AX_U8 nPipeSeq;
    AX_U8 nPipeID;

    _SENSOR_PIPE_MAPPING() {
        nPipeSeq = 0;
        nPipeID = 0;
    }
} SENSOR_PIPE_MAPPING_T;

typedef struct _CHANNEL_CONFIG_T {
    AX_S32 nWidth;
    AX_S32 nHeight;
    AX_U8 nYuvDepth;
    AX_BOOL bChnEnable;
    AX_F32 fFrameRate;
    AX_FRAME_COMPRESS_INFO_T tChnCompressInfo;

    _CHANNEL_CONFIG_T() {
        nWidth = 0;
        nHeight = 0;
        nYuvDepth = 0;
        fFrameRate = 0;
        bChnEnable = AX_FALSE;
        tChnCompressInfo.enCompressMode = AX_COMPRESS_MODE_NONE;
        tChnCompressInfo.u32CompressLevel = 0;
    }
} CHANNEL_CONFIG_T, *CHANNEL_CONFIG_PTR;

typedef struct _SENSOR_EZOOM_ATTR_T {
    AX_S32 nEZoomRatio;
    AX_S16 nCenterOffsetX;  // offset x from horizental center, not used
    AX_S16 nCenterOffsetY;  // offset y from veritcal center, not used

    _SENSOR_EZOOM_ATTR_T() {
        nEZoomRatio = 0;
        nCenterOffsetX = 0;
        nCenterOffsetX = 0;
    }
} SENSOR_EZOOM_ATTR_T;

typedef struct _SENSOR_LDC_ATTR_T {
    // LDC
    AX_BOOL bLdcEnable;
    AX_BOOL bLdcAspect;
    AX_S16 nLdcXRatio;
    AX_S16 nLdcYRatio;
    AX_S16 nLdcXYRatio;
    AX_S16 nLdcDistortionRatio;

    _SENSOR_LDC_ATTR_T() {
        bLdcEnable = AX_FALSE;
        bLdcAspect = AX_FALSE;
        nLdcXRatio = 0;
        nLdcYRatio = 0;
        nLdcXYRatio = 0;
        nLdcDistortionRatio = 0;
    }
} SENSOR_LDC_ATTR_T;

typedef struct _IFE_DUMP_INFO {
    AX_S8 nIFEDumpRawEnable;
    AX_U8 nIFEDumpRawDepth;
    AX_U32 nDumpThreadWaitTime;

    _IFE_DUMP_INFO() {
        nIFEDumpRawEnable = -1;
        nIFEDumpRawDepth = 3;
        nDumpThreadWaitTime = 1;
    }
} IFE_DUMP_INFO_T;

typedef struct _RLTM_DUMP_INFO {
    AX_S8 nRLTMDumpRawEnable;
    AX_U8 nRLTMDumpRawDepth;

    _RLTM_DUMP_INFO() {
        nRLTMDumpRawEnable = -1;
        nRLTMDumpRawDepth = 3;
    }
} RLTM_DUMP_INFO_T;

typedef struct _PIPE_CONFIG_T {
    AX_U8 nPipeID;
    AX_F32 fPipeFramerate;
    CHANNEL_CONFIG_T arrChannelAttr[AX_VIN_CHN_ID_MAX];
    AX_BOOL bAiEnable;
    AX_FRAME_COMPRESS_INFO_T tIfeCompress;
    AX_FRAME_COMPRESS_INFO_T tAiNrCompress;
    AX_FRAME_COMPRESS_INFO_T t3DNrCompress;
    SENSOR_LDC_ATTR_T tLdcAttr;
    AX_BOOL bSnapshot;
    AX_S8 nContinousFrames;
    AX_BOOL bTuning;
    AX_BOOL bDummyEnable;
    AX_U32 nTuningPort;
    AX_BOOL bEnableEIS;
    AX_BOOL bEISOn;
    AX_U8 nSrcDepth;
    AX_U8 nEngineMode;
    AX_BOOL bMirror;
    AX_BOOL bFlip;
    AX_BOOL bMirrorFlipEngine;
    AX_VIN_ROTATION_E eRotation;
    AX_BOOL bRotationEngine;
    AX_VIN_PIPE_WORK_MODE_E ePipeWorkMode;
    AX_VIN_SCHED_ATTR_T tPipeSchedAttr;
    IFE_DUMP_INFO_T  tIFEDumpInfo;
    RLTM_DUMP_INFO_T tRLTMDumpInfo;

    vector<string> vecTuningBin{};

    _PIPE_CONFIG_T() {
        nPipeID = 0;
        fPipeFramerate = 30;
        bAiEnable = AX_TRUE;
        memset(&tIfeCompress, 0, sizeof(AX_FRAME_COMPRESS_INFO_T));
        memset(&tAiNrCompress, 0, sizeof(AX_FRAME_COMPRESS_INFO_T));
        memset(&t3DNrCompress, 0, sizeof(AX_FRAME_COMPRESS_INFO_T));
        bSnapshot = AX_FALSE;
        nContinousFrames = -127;
        bTuning = AX_TRUE;
        nTuningPort = 8082;
        bEnableEIS = AX_FALSE;
        bEISOn = AX_FALSE;
        nSrcDepth = 0;
        nEngineMode = 0;
        memset(arrChannelAttr, 0, sizeof(arrChannelAttr));
        ePipeWorkMode = AX_VIN_PIPE_NORMAL_MODE0;
        memset(&tPipeSchedAttr, 0, sizeof(tPipeSchedAttr));
        bMirror = AX_FALSE;
        bFlip = AX_FALSE;
        bMirrorFlipEngine = AX_FALSE;
        eRotation = AX_VIN_ROTATION_0;
        bRotationEngine = AX_FALSE;
        bDummyEnable = AX_FALSE;
    }
} PIPE_CONFIG_T, *PIPE_CONFIG_PTR;

typedef struct _DEV_OUTSIDE_CONFIG_T {
    AX_VIN_POWER_SYNC_ATTR_T tPowerSyncAttr;
    AX_VIN_SYNC_SIGNAL_ATTR_T tHSyncSignalAttr;
    AX_VIN_SYNC_SIGNAL_ATTR_T tVSyncSignalAttr;
    AX_VIN_LIGHT_SYNC_INFO_T tLightSyncAttr;
    AX_VIN_STROBE_LIGHT_TIMING_ATTR_T tStrobeLightTimingAttr;
    AX_VIN_FLASH_LIGHT_TIMING_ATTR_T tSnapFlashAttr;
    _DEV_OUTSIDE_CONFIG_T() {
        memset(&tPowerSyncAttr, 0, sizeof(AX_VIN_POWER_SYNC_ATTR_T));
        memset(&tHSyncSignalAttr, 0, sizeof(AX_VIN_SYNC_SIGNAL_ATTR_T));
        memset(&tVSyncSignalAttr, 0, sizeof(AX_VIN_SYNC_SIGNAL_ATTR_T));
        memset(&tLightSyncAttr, 0, sizeof(AX_VIN_LIGHT_SYNC_INFO_T));
        memset(&tStrobeLightTimingAttr, 0, sizeof(AX_VIN_STROBE_LIGHT_TIMING_ATTR_T));
        memset(&tSnapFlashAttr, 0, sizeof(AX_VIN_FLASH_LIGHT_TIMING_ATTR_T));
    }
} DEV_OUTSIDE_CONFIG_T, *DEV_OUTSIDE_CONFIG;

typedef struct _SENSOR_CONFIG_T {
    AX_U8 nPanoMode;
    AX_U8 nSnsID;
    AX_U8 nDevID;
    AX_U8 nRxDev;
    AX_U8 nDevNode;
    AX_U8 nI2cAddr;
    AX_U8 nClkID;
    AX_U8 nShutterType;
    AX_BOOL bSnsMirrorFlip;
    AX_BOOL bStitchMirrorFlip;
    AX_U16 arrSnsRawType[AX_SNS_HDR_MODE_MAX]{0};
    AX_SNS_MASTER_SLAVE_E eMasterSlaveSel;
    SNS_TYPE_E eSensorType;
    AX_SNS_HDR_MODE_E eSensorMode;
    AX_VIN_DEV_MODE_E eDevMode;
    AX_SNS_OUTPUT_MODE_E eSnsOutputMode;
    AX_DAYNIGHT_MODE_E eDayNight;
    AX_BOOL bEnableFlash;
    AX_F32 fFrameRate;
    AX_U32 nPipeCount;
    PIPE_CONFIG_T arrPipeAttr[MAX_PIPE_PER_DEVICE];
    string strPNFrameRate;
    DEV_OUTSIDE_CONFIG_T tDevOutSideAttr;
    SENSOR_EZOOM_ATTR_T tEZoomAttr;
    AX_S8 arrSettingIndex[3]{0};

    _SENSOR_CONFIG_T() {
        nPanoMode = 0;
        nSnsID = 0;
        nDevID = 0;
        nRxDev = 0;
        nDevNode = 1;
        nI2cAddr = INVALID_I2C_ADDR;
        nClkID = 0;
        nShutterType = 0;
        bSnsMirrorFlip = AX_FALSE;
        bStitchMirrorFlip = AX_FALSE;
        fFrameRate = 30;
        eDevMode = AX_VIN_DEV_OFFLINE;
        eSnsOutputMode = AX_SNS_NORMAL;
        eSensorType = E_SNS_TYPE_MAX;
        eSensorMode = AX_SNS_LINEAR_MODE;
        eDayNight = AX_DAYNIGHT_MODE_DAY;
        nPipeCount = 0;
        bEnableFlash = AX_FALSE;
        eMasterSlaveSel = AX_SNS_MASTER;
        strPNFrameRate = "";
    }

    _SENSOR_CONFIG_T(AX_U8 nSnsID, AX_U8 nDevID, AX_SNS_HDR_MODE_E eSensorMode, AX_VIN_DEV_MODE_E eDevMode, AX_F32 fFrameRate,
                        AX_U32 nPipeCount, string strPNFramteRate, AX_SNS_MASTER_SLAVE_E eMasterSlaveSel = AX_SNS_MASTER,  AX_U8 nPanoMode = 0,
                        AX_U8 nRxDev = 0, AX_U8 nShutterType = 0) {
        this->nPanoMode = nPanoMode;
        this->nSnsID = nSnsID;
        this->nDevID = nDevID;
        this->nRxDev = nRxDev;
        this->fFrameRate = fFrameRate;
        this->eDevMode = eDevMode;
        this->eSnsOutputMode = AX_SNS_NORMAL;
        this->eSensorType = E_SNS_TYPE_MAX;
        this->eSensorMode = eSensorMode;
        this->nPipeCount = nPipeCount;
        this->eMasterSlaveSel = eMasterSlaveSel;
        this->strPNFrameRate = strPNFramteRate;
        this->nShutterType = nShutterType;
    }

} SENSOR_CONFIG_T, *SENSOR_CONFIG_PTR;

class ISensor {
public:
    virtual ~ISensor(AX_VOID) = default;

    /*
        @brief: initialize and lanuch ISP pipe line
    */
    virtual AX_BOOL Open(AX_VOID) = 0;
    virtual AX_BOOL Close(AX_VOID) = 0;

    /*
        @brief: ISP algorithm including AE, AF, AWB, LSC ...
                by default, AE and AWB is actived with Axera algorithm.
    */
    virtual AX_VOID RegisterIspAlgo(const ISP_ALGO_INFO_T& tAlg) = 0;

    /*
        @brief: sensor and ISP properties set and get
                NOTE: property should be set before Start()
     */
    virtual const AX_SNS_ATTR_T& GetSnsAttr(AX_VOID) const = 0;
    virtual AX_VOID SetSnsAttr(const AX_SNS_ATTR_T& tSnsAttr) = 0;

    virtual const SNS_CLK_ATTR_T& GetSnsClkAttr(AX_VOID) const = 0;
    virtual AX_VOID SetSnsClkAttr(const SNS_CLK_ATTR_T& tClkAttr) = 0;

    virtual const AX_VIN_DEV_ATTR_T& GetDevAttr(AX_VOID) const = 0;
    virtual AX_VOID SetDevAttr(const AX_VIN_DEV_ATTR_T& tDevAttr) = 0;

    virtual const AX_MIPI_RX_DEV_T& GetMipiRxAttr(AX_VOID) const = 0;
    virtual AX_VOID SetMipiRxAttr(const AX_MIPI_RX_DEV_T& tMipiRxAttr) = 0;

    virtual const AX_VIN_PIPE_ATTR_T& GetPipeAttr(AX_U8 nPipe) const = 0;
    virtual AX_VOID SetPipeAttr(AX_U8 nPipe, const AX_VIN_PIPE_ATTR_T& tPipeAttr) = 0;

    virtual const AX_VIN_CHN_ATTR_T& GetChnAttr(AX_U8 nPipe, AX_U8 nChannel) const = 0;
    virtual AX_VOID SetChnAttr(AX_U8 nPipe, AX_U8 nChannel, const AX_VIN_CHN_ATTR_T& tChnAttr) = 0;

    virtual const SNS_ABILITY_T& GetAbilities(AX_VOID) const = 0;
};

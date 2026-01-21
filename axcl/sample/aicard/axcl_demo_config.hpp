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

#include <string>
#include <vector>

#include "ax_global_type.h"

#include "AXSingleton.h"
#include "IniWrapper.hpp"

namespace axcl_demo {

typedef struct {
    AX_U32 nUserPool;
    AX_U32 nMaxGrpW;
    AX_U32 nMaxGrpH;
    AX_U32 nChnW[3];
    AX_U32 nChnH[3];
    AX_U32 nDefaultFps;
    AX_S32 nChnDepth[3];
    AX_U32 nInputMode;
    AX_U32 nMaxStreamBufSize;
    AX_U32 nDecodeGrps;
    AX_U32 nFrmDelay;
    std::vector<std::string> v;
} STREAM_CONFIG_T;

typedef struct {
    AX_PAYLOAD_TYPE_E eVideoType;
    AX_U32 nOutputTarget; // 0: vdec; 1: ivps
    AX_U32 nUserPool;
    AX_U32 nMaxGrpW;
    AX_U32 nMaxGrpH;
    AX_U32 arrChnW[3];
    AX_U32 arrChnH[3];
    AX_U32 nDefaultFps;
    AX_S32 nChnDepth[3];
    AX_U32 nInputMode;
    AX_U32 nMaxStreamBufSize;
    AX_U32 nDecodeGrps;
    std::vector<std::string> v;
    AX_BOOL bEnableReset;
} VDEC_CONFIG_T;

typedef struct {
    AX_U32 nDevId;
    AX_U32 nHDMI;
    AX_U32 nChnDepth;
    AX_U32 nLayerDepth;
    AX_U32 nTolerance;
    AX_BOOL bShowLogo;
    AX_BOOL bShowNoVideo;
    std::string strResDirPath;
    std::string strBmpPath;
} DISPVO_CONFIG_T;

typedef struct {
    AX_S32 nGrpCount;
    AX_U32 nChnCount;
    AX_U32 arrChnDepth[3];
    AX_U32 arrChnW[3];
    AX_U32 arrChnH[3];
    AX_U32 arrChnLink[3];
} IVPS_CONFIG_T;

typedef struct {
    AX_U32 nPPL;
    AX_U32 nVNPU;
    AX_BOOL bTrackEnable;
} DETECT_CHN_PARAM_T;

typedef struct {
    AX_BOOL bEnable {AX_FALSE};
    AX_BOOL bEnableSimulator {AX_FALSE};
    AX_U32 nW;
    AX_U32 nH;
    AX_U32 nSkipRate;
    AX_S32 nDepth;
    AX_S32 nChannelNum;
    DETECT_CHN_PARAM_T tChnParam[3];
    std::string strModelPath;
} DETECT_CONFIG_T;

typedef struct {
    AX_S32 nDevID {0};
    // pending
} DEV_CONFIG_T;

typedef struct {
    AX_BOOL bEnable {AX_FALSE};
    AX_S32 nSkipFrm {0};
    // pending
} CAPTURE_CONFIG_T;

/**
 * @brief
 *
 */
class CAxclDemoConfig : public CAXSingleton<CAxclDemoConfig> {
    friend class CAXSingleton<CAxclDemoConfig>;

public:
    AX_BOOL Init(AX_VOID);

    /* host */
    STREAM_CONFIG_T GetStreamConfig(AX_VOID);
    DISPVO_CONFIG_T GetDispVoConfig(AX_VOID);

    /* device */
    DEV_CONFIG_T     GetDevConfig(AX_VOID);
    VDEC_CONFIG_T    GetVdecConfig(AX_VOID);
    DETECT_CONFIG_T  GetDetectConfig(AX_VOID);
    CAPTURE_CONFIG_T GetCaptureConfig(AX_VOID);

private:
    CAxclDemoConfig(AX_VOID) = default;
    virtual ~CAxclDemoConfig(AX_VOID) = default;

    string GetExecPath(AX_VOID);

private:
    CIniWrapper m_IniHost, m_IniDevice;
};

}  // namespace axcl_demo

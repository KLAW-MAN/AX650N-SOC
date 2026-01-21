/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include <unistd.h>

#include "axcl_demo_config.hpp"
#include "GlobalDef.h"
#include "ax_global_type.h"
#include "ax_venc_rc.h"

using namespace std;
using namespace axcl_demo;

string CAxclDemoConfig::GetExecPath(AX_VOID) {
    string strPath;
    AX_CHAR szPath[260] = {0};
    ssize_t sz = readlink("/proc/self/exe", szPath, sizeof(szPath));
    if (sz <= 0) {
        strPath = "./";
    } else {
        strPath = szPath;
        strPath = strPath.substr(0, strPath.rfind('/') + 1);
    }

    return strPath;
}

AX_BOOL CAxclDemoConfig::Init(AX_VOID) {
    string strIniPath = GetExecPath() + "axcl_host.conf";
    if (!m_IniHost.Load(strIniPath)) {
        return AX_FALSE;
    }

    strIniPath = GetExecPath() + "axcl_device.conf";
    if (!m_IniDevice.Load(strIniPath)) {
        return AX_FALSE;
    }

    return AX_TRUE;
}

STREAM_CONFIG_T CAxclDemoConfig::GetStreamConfig(AX_VOID) {
    STREAM_CONFIG_T conf;
    const AX_CHAR *SECT = "STREAM";

    conf.nMaxGrpW = m_IniHost.GetIntValue(SECT, "max width", 1920);
    conf.nMaxGrpH = m_IniHost.GetIntValue(SECT, "max height", 1080);
    for (AX_U32 i = 0; i < 3; ++i) {
        AX_CHAR szKey[32];
        sprintf(szKey, "chn%d depth", i);
        conf.nChnDepth[i] = m_IniHost.GetIntValue(SECT, szKey, 8);
    }
    conf.nDefaultFps = m_IniHost.GetIntValue(SECT, "default fps", 30);
    conf.nInputMode = m_IniHost.GetIntValue(SECT, "input mode", 0);
    conf.nFrmDelay = m_IniHost.GetIntValue(SECT, "frame delay", 0);

    conf.nUserPool = m_IniHost.GetIntValue(SECT, "user pool", 1);
    if (conf.nUserPool > 2) {
        conf.nUserPool = 1;
    }

    conf.nMaxStreamBufSize = m_IniHost.GetIntValue(SECT, "max stream buf size", 0x200000);
    AX_U32 nCount = m_IniHost.GetIntValue(SECT, "count", 1);
    if (nCount > 0) {
        conf.v.resize(nCount);
        for (AX_U32 i = 1; i <= nCount; ++i) {
            AX_CHAR szKey[32];
            sprintf(szKey, "stream%02d", i);
            conf.v[i - 1] = m_IniHost.GetStringValue(SECT, szKey, "");
        }
    }

    conf.nDecodeGrps = m_IniHost.GetIntValue(SECT, "vdec count", 0);
    if (0 == conf.nDecodeGrps || conf.nDecodeGrps > nCount) {
        conf.nDecodeGrps = nCount;
    }

    return conf; /* RVO: optimized by compiler */
}

DISPVO_CONFIG_T CAxclDemoConfig::GetDispVoConfig(AX_VOID) {
    DISPVO_CONFIG_T conf;
    const AX_CHAR *SECT = "DISPC";

    conf.nDevId = m_IniHost.GetIntValue(SECT, "dev", 0);
    conf.nHDMI = m_IniHost.GetIntValue(SECT, "HDMI", 10);
    conf.nLayerDepth = m_IniHost.GetIntValue(SECT, "layer depth", 3);
    /* if 0, vo using default tolerance, VO_LAYER_TOLERATION_DEF = 10*1000*1000 */
    conf.nTolerance = m_IniHost.GetIntValue(SECT, "tolerance", 0);
    conf.bShowLogo = (AX_BOOL)m_IniHost.GetIntValue(SECT, "show logo", 1);
    conf.bShowNoVideo = (AX_BOOL)m_IniHost.GetIntValue(SECT, "show no video", 1);
    conf.strResDirPath = GetExecPath() + "res";
    conf.strBmpPath = conf.strResDirPath + "/font.bmp";

    return conf; /* RVO: optimized by compiler */
}

DEV_CONFIG_T CAxclDemoConfig::GetDevConfig(AX_VOID) {
    DEV_CONFIG_T conf;
    const AX_CHAR *SECT = "DEVICE";

    conf.nDevID = m_IniDevice.GetIntValue(SECT, "device id", 0);

    return conf; /* RVO: optimized by compiler */
}

VDEC_CONFIG_T CAxclDemoConfig::GetVdecConfig(AX_VOID) {
    VDEC_CONFIG_T conf;
    const AX_CHAR *SECT = "VDEC";

    conf.eVideoType = (m_IniDevice.GetIntValue(SECT, "video type", 0) == 0 ? PT_H264 : PT_H265);
    conf.nOutputTarget = m_IniDevice.GetIntValue(SECT, "output target", 0);
    conf.nMaxGrpW = m_IniDevice.GetIntValue(SECT, "width", 1920);
    conf.nMaxGrpH = m_IniDevice.GetIntValue(SECT, "height", 1080);
    conf.bEnableReset = (AX_BOOL)m_IniDevice.GetIntValue(SECT, "enable reset", 0);
    for (AX_U32 i = 0; i < 3; ++i) {
        AX_CHAR szKey[32];
        sprintf(szKey, "chn%d depth", i);
        conf.nChnDepth[i] = m_IniDevice.GetIntValue(SECT, szKey, 8);
    }
    conf.nDefaultFps = m_IniDevice.GetIntValue(SECT, "fps", 30);
    conf.nInputMode = m_IniDevice.GetIntValue(SECT, "input mode", 0);

    conf.nUserPool = m_IniDevice.GetIntValue(SECT, "user pool", 1);
    if (conf.nUserPool > 2) {
        conf.nUserPool = 1;
    }

    conf.nMaxStreamBufSize = m_IniDevice.GetIntValue(SECT, "max stream buf size", 0x200000);
    AX_U32 nCount = m_IniDevice.GetIntValue(SECT, "count", 1);
    if (nCount > 0) {
        conf.v.resize(nCount);
        for (AX_U32 i = 1; i <= nCount; ++i) {
            AX_CHAR szKey[32];
            sprintf(szKey, "stream%02d", i);
            conf.v[i - 1] = m_IniDevice.GetStringValue(SECT, szKey, "");
        }
    }

    conf.nDecodeGrps = m_IniDevice.GetIntValue(SECT, "vdec count", 0);
    if (0 == conf.nDecodeGrps || conf.nDecodeGrps > nCount) {
        conf.nDecodeGrps = nCount;
    }

    return conf; /* RVO: optimized by compiler */
}

DETECT_CONFIG_T CAxclDemoConfig::GetDetectConfig(AX_VOID) {
    DETECT_CONFIG_T conf;
    const AX_CHAR *SECT = "DETECT";

    conf.bEnable = (AX_BOOL)m_IniDevice.GetIntValue(SECT, "enable", 0);
    conf.bEnableSimulator = (AX_BOOL)m_IniDevice.GetIntValue(SECT, "enable simulator", 0);
    conf.nW = m_IniDevice.GetIntValue(SECT, "width", 960);
    conf.nH = m_IniDevice.GetIntValue(SECT, "height", 640);
    conf.nSkipRate = m_IniDevice.GetIntValue(SECT, "skip rate", 1);
    conf.nDepth = m_IniDevice.GetIntValue(SECT, "fifo depth", 1);
    conf.nChannelNum = m_IniDevice.GetIntValue(SECT, "channel num", 1);
    conf.nChannelNum = AX_MIN(conf.nChannelNum, 3);

    for (AX_S32 i = 0; i < conf.nChannelNum; ++i) {
        std::string str = "channel" + std::to_string(i) + " attr";

        vector<AX_S32> vec;
        m_IniDevice.GetIntValue(SECT, str, vec);

        if (vec.size() == 3) {
            conf.tChnParam[i].nPPL = vec[0];
            conf.tChnParam[i].bTrackEnable = (AX_BOOL)vec[1];
            conf.tChnParam[i].nVNPU = vec[2];
        } else {
            conf.tChnParam[i].nPPL = 1;
            conf.tChnParam[i].bTrackEnable = AX_FALSE;
            conf.tChnParam[i].nVNPU = 0;
        }
    }

    conf.strModelPath = m_IniDevice.GetStringValue(SECT, "model path", "");

    return conf; /* RVO: optimized by compiler */
}

CAPTURE_CONFIG_T CAxclDemoConfig::GetCaptureConfig(AX_VOID) {
    CAPTURE_CONFIG_T conf;
    const AX_CHAR *SECT = "CAPTURE";

    conf.bEnable = (AX_BOOL)m_IniDevice.GetIntValue(SECT, "enable", 0);
    conf.nSkipFrm = (AX_BOOL)m_IniDevice.GetIntValue(SECT, "skip frm", 10000);

    return conf; /* RVO: optimized by compiler */
}

/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include "OptionHelper.h"
#include "CommonUtils.hpp"

AX_BOOL COptionHelper::InitOnce() {
    string strConfigDir = CCommonUtils::GetPPLConfigDir();
    if (strConfigDir.empty()) {
        return AX_FALSE;
    }

    string strConfigFile = strConfigDir + "/options.ini";

    return m_iniWrapper.Load(strConfigFile);
}

string COptionHelper::GetValue(const string strKey) {
    return m_iniWrapper.GetStringValue("options", strKey, "");
}

AX_U8 COptionHelper::GetRotation() {
    string strVal = GetValue("Rotation");
    if (!strVal.empty()) {
        return atoi(strVal.c_str());
    }

    return 0;
}

AX_F32 COptionHelper::GetVencOutBuffRatio() {
    string strVal = GetValue("WebVencFrmSizeRatio");
    if (!strVal.empty()) {
        return atof(strVal.c_str());
    }

    return 0;
}

AX_U8 COptionHelper::GetVencOutBuffCount() {
    string strVal = GetValue("WebVencFrmSizeCount");
    if (!strVal.empty()) {
        return atof(strVal.c_str());
    }

    return 0;
}

AX_F32 COptionHelper::GetJencOutBuffRatio() {
    string strVal = GetValue("WebJencFrmSizeRatio");
    if (!strVal.empty()) {
        return atof(strVal.c_str());
    }

    return 0;
}

AX_U8 COptionHelper::GetJencOutBuffCount() {
    string strVal = GetValue("WebJencFrmSizeCount");
    if (!strVal.empty()) {
        return atof(strVal.c_str());
    }

    return 0;
}

AX_U32 COptionHelper::GetAencOutFrmSize() {
    string strVal = GetValue("WebAencFrmSize");
    if (!strVal.empty()) {
        return atof(strVal.c_str());
    }

    return 8192;
}

AX_U8 COptionHelper::GetAencOutFrmCount() {
    string strVal = GetValue("WebAencFrmCount");
    if (!strVal.empty()) {
        return atof(strVal.c_str());
    }

    return 0;
}

AX_U32 COptionHelper::GetRTSPMaxFrmSize() {
    string strVal = GetValue("RTSPMaxFrmSize");
    if (!strVal.empty()) {
        return atoi(strVal.c_str());
    }

    return 0;
}

AX_U8 COptionHelper::GetRTSPMaxFrmCount() {
    string strVal = GetValue("RTSPMaxFrmCount");
    if (!strVal.empty()) {
        return atoi(strVal.c_str());
    }

    return 0;
}

AX_S32 COptionHelper::GetNpuMode() {
    string strVal = GetValue("NpuMode");
    if (!strVal.empty()) {
        return atoi(strVal.c_str());
    }

    return 0;
}

AX_BOOL COptionHelper::IsEnableOSD() {
    string strVal = GetValue("EnableOSD");
    AX_U32 ret = 0;
    if (!strVal.empty()) {
        ret = atoi(strVal.c_str());
    }
    return ret ? AX_TRUE : AX_FALSE;
}

AX_BOOL COptionHelper::IsEnableVideoRecord() {
    string strVal = GetValue("EnableVideoRecord");
    AX_U32 ret = 0;
    if (!strVal.empty()) {
        ret = atoi(strVal.c_str());
    }
    return ret ? AX_TRUE : AX_FALSE;
}

string COptionHelper::GetVideoSavedPath() {
    return GetValue("VideoRecordSavedPath");
}

AX_U32 COptionHelper::GetVideoMaxRecordNum() {
    string strVal = GetValue("VideoMaxRecordNum");
    if (!strVal.empty()) {
        return atoi(strVal.c_str());
    }

    return 0;
}

AX_U32 COptionHelper::GetVideoMaxRecordSize() {
    string strVal = GetValue("VideoMaxFileSize");
    if (!strVal.empty()) {
        return atoi(strVal.c_str());
    }

    return 0;
}

AX_BOOL COptionHelper::IsVideoRecordLoop() {
    string strVal = GetValue("VideoRecordLoop");
    AX_U32 ret = 0;
    if (!strVal.empty()) {
        ret = atoi(strVal.c_str());
    }
    return ret ? AX_TRUE : AX_FALSE;
}

AX_U32 COptionHelper::GetSLTRunTime() {
    string strVal = GetValue("RunTime");
    if (!strVal.empty()) {
        return atoi(strVal.c_str());
    }

    return 0;
}

AX_U32 COptionHelper::GetSLTFpsCheckFreq() {
    string strVal = GetValue("FPSCheckFreq");
    if (!strVal.empty()) {
        return atoi(strVal.c_str());
    }

    return 0;
}

AX_U32 COptionHelper::GetSLTFpsDiff() {
    string strVal = GetValue("FPSDiff");
    if (!strVal.empty()) {
        return atoi(strVal.c_str());
    }

    return 0;
}

AX_U32 COptionHelper::GetSetVencThreadNum() {
    string strVal = GetValue("SetVencThreadNum");
    if (!strVal.empty()) {
        return atoi(strVal.c_str());
    }
    return 1;
}

AX_BOOL COptionHelper::IsEnableWebServerStatusCheck() {
	string strVal = GetValue("WebServerStatusCheck");
    AX_U32 ret = 0;
    if (!strVal.empty()) {
        ret = atoi(strVal.c_str());
    }
    return ret ? AX_TRUE : AX_FALSE;
}

std::string COptionHelper::GetTriggerPicPath(AX_VOID) {
    return GetValue("TriggerPicPath");
}

AX_BOOL COptionHelper::ISEnableTriggerPicStore(AX_VOID) {
    string strVal = GetValue("EnableTriggerPicStore");
    AX_U32 ret = 0;
    if (!strVal.empty()) {
        ret = atoi(strVal.c_str());
    }
    return ret ? AX_TRUE : AX_FALSE;
}

AX_BOOL COptionHelper::IsEnableAFTest() {
    string strVal = GetValue("EnableAFTest");
    AX_U32 ret = 0;
    if (!strVal.empty()) {
        ret = atoi(strVal.c_str());
    }
    return ret ? AX_TRUE : AX_FALSE;
}

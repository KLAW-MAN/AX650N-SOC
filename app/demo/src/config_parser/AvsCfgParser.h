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

#include "picojson.h"
#include "AXSingleton.h"
#include "ax_avs_api.h"

typedef struct _AX_APP_AVS_CFG_T {
    AX_U8 u8PipeNum;
    AX_U8 u8Mode;
    AX_U8 u8BlendMode;
    AX_U8 u8ParamType;
    AX_BOOL bAroundView;
    AX_BOOL bPrioOnsiteCali;
    AX_AVS_ENGINE_MODE_E eEngineMode;
    AX_AVS_PROJECTION_MODE_E eProjectionMode;
    AX_BOOL bSyncPipe;
    AX_BOOL bDynamicSeam;
    AX_FRAME_COMPRESS_INFO_T stAvsCompress;
    std::string strParamFilePath;
    AX_U8 u8CaliEnable;
    std::string strCaliServerIP;
    AX_U16 u16CaliServerPort;

    AX_U8 arrPipeSeq[AX_AVS_PIPE_NUM];
    AX_U8 u8MasterPipeId;

    _AX_APP_AVS_CFG_T() {
        u8PipeNum = 2;
        u8Mode = 0;
        u8BlendMode = 1;
        u8ParamType = 0;
        bAroundView = AX_FALSE;
        bPrioOnsiteCali = AX_TRUE;
        eEngineMode = AVS_ENGINE_GDC;
        eProjectionMode = AVS_PROJECTION_CYLINDRICAL;
        bSyncPipe = AX_TRUE;
        bDynamicSeam = AX_FALSE;
        stAvsCompress = {AX_COMPRESS_MODE_LOSSY, 4};
        strParamFilePath = "/param/avs/os04a10/";
        u8CaliEnable = 0;
        strCaliServerIP = "192.168.2.10";
        u16CaliServerPort = 9999;
    }
} AX_APP_AVS_CFG_T, *AX_APP_AVS_CFG_PTR;

typedef enum {
    E_AVS_PARAM_TYPE_NORMAL = 0,
    E_AVS_PARAM_TYPE_MESHTABLE,
    E_AVS_PARAM_TYPE_MAX,
} AX_APP_AVS_PARAM_TYPE_E;

class CAvsCfgParser : public CAXSingleton<CAvsCfgParser> {
    friend class CAXSingleton<CAvsCfgParser>;

public:
    AX_BOOL GetConfig(std::map<AX_U8, AX_APP_AVS_CFG_T>& mapAvsCfg);

private:
    CAvsCfgParser(AX_VOID) = default;
    ~CAvsCfgParser(AX_VOID) = default;

    AX_BOOL InitOnce() override;
    AX_BOOL ParseFile(const std::string& strPath, std::map<AX_U8, AX_APP_AVS_CFG_T>& mapAvsCfg);
    AX_BOOL ParseJson(picojson::object& objJsonRoot, std::map<AX_U8, AX_APP_AVS_CFG_T>& mapAvsCfg);
    AX_BOOL ParseJson(picojson::object& objSetting, AX_APP_AVS_CFG_T& stAvsCfg, AX_BOOL bDefaultSetting);
    AX_AVS_PROJECTION_MODE_E GetAvsProjectionMode(AX_U8 nProjType);
};
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

#include <vector>
#include "ax_base_type.h"
#include "ax_global_type.h"
#include "ax_ivps_api.h"
#include "AXAlgo.hpp"

class CIVPSGrpStage;
typedef struct _OSD_SHOW_FLAG_T {
    AX_U8 nShowTime;
    AX_U8 nShowLogo;
    AX_U8 nShowRect;
    AX_U8 nShowChannel;
    AX_U8 nShowLocation;
} OSD_SHOW_FLAG_T, *OSD_SHOW_FLAG_PTR;

class IOSDHelper {
public:
    virtual ~IOSDHelper() = default;

public:
    virtual AX_BOOL StartOSD(CIVPSGrpStage* pIvpsInstance) = 0;
    virtual AX_BOOL StopOSD() = 0;
    virtual AX_BOOL EnableAiRegion(AX_BOOL bEnable = AX_TRUE) = 0;
    virtual AX_U8 GetAttachedFilter() = 0;
    virtual AX_BOOL Refresh(AX_BOOL bShow = AX_TRUE) = 0;
    virtual AX_BOOL UpdateOSDRect(const std::vector<AX_IVPS_RGN_POLYGON_T>& vecRgn) = 0;
    virtual AX_BOOL UpdateOSDRect(const std::vector<AX_APP_ALGO_BOX_T>& vecBox) = 0;
    virtual AX_BOOL SetOsdConfig(AX_U8 nSnsID, AX_S32 nGrp, AX_S32 nChn, AX_BOOL bVideo, const OSD_SHOW_FLAG_T stOsdShowFlag, AX_S16 nLinkedVId = -1) = 0;
    virtual AX_BOOL SetPrivacyConfig(AX_U8 nSnsID, AX_S32 nGrp) = 0;
};
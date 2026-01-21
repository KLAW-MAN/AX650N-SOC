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

#include <functional>
#include <string>
#include <vector>

#include "ax_global_type.h"

typedef struct {
    AX_U32 nMaxGrp;
} AXCL_DEMO_APP_SYS_ATTR_T;

class CAxclDemoAppSys {
public:
    CAxclDemoAppSys(AX_VOID) = default;
    virtual ~CAxclDemoAppSys(AX_VOID) = default;

    AX_BOOL Init(const AXCL_DEMO_APP_SYS_ATTR_T& stAttrHost,
                 const AXCL_DEMO_APP_SYS_ATTR_T& stAttrDevice,
                 const std::string& strAppName = "AxclDemo");
    AX_BOOL DeInit(AX_VOID);

protected:
    static std::string GetSdkVersion(AX_VOID);

    AX_BOOL InitAppLog(const std::string& strAppName);
    AX_BOOL DeInitAppLog(AX_VOID);

    virtual AX_BOOL InitSysMods(AX_VOID);
    virtual AX_BOOL DeInitSysMods(AX_VOID);

    virtual AX_S32 APP_SYS_Init(AX_VOID);
    virtual AX_S32 APP_SYS_DeInit(AX_VOID);
    virtual AX_S32 APP_VDEC_Init(AX_VOID);
    virtual AX_S32 APP_VDEC_DeInit(AX_VOID);
    virtual AX_S32 APP_VENC_Init(AX_VOID);
    virtual AX_S32 APP_VENC_DeInit(AX_VOID);
    virtual AX_S32 APP_IVPS_Init(AX_VOID);
    virtual AX_S32 APP_IVPS_DeInit(AX_VOID);
    virtual AX_S32 APP_SKEL_Init(AX_VOID);
    virtual AX_S32 APP_SKEL_DeInit(AX_VOID);

private:
    typedef struct {
        AX_BOOL bInited;
        std::string strName;
        std::function<AX_S32(AX_VOID)> Init;
        std::function<AX_S32(AX_VOID)> DeInit;
    } SYS_MOD_T;

    AXCL_DEMO_APP_SYS_ATTR_T m_tAttrHost, m_tAttrDevice;
    std::vector<SYS_MOD_T> m_arrMods;
};

/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "axcl_demo_app_sys.hpp"
#include "AXPoolManager.hpp"
#include "AppLogApi.h"
#include "arraysize.h"

/* host */
#include "ax_sys_api.h"
#include "ax_vdec_api.h"
#include "ax_ivps_api.h"
#include "ax_vo_api.h"

/* device */
#include "axcl_sys.h"
#include "axcl_vdec.h"
#include "axcl_venc.h"
#include "axcl_ivps.h"

#include "axcl_demo_config.hpp"

using namespace std;
using namespace axcl_demo;

AX_BOOL CAxclDemoAppSys::Init(const AXCL_DEMO_APP_SYS_ATTR_T& stAttrHost,
                              const AXCL_DEMO_APP_SYS_ATTR_T& stAttrDevice,
                              const std::string& strAppName) {
    if (0 == stAttrHost.nMaxGrp || 0 == stAttrDevice.nMaxGrp) {
        LOG_E("%s: 0 grp", __func__);
        return AX_FALSE;
    } else {
        m_tAttrHost = stAttrHost;
        m_tAttrDevice = stAttrDevice;
    }

    if (!InitAppLog(strAppName)) {
        return AX_FALSE;
    }

    if (!InitSysMods()) {
        DeInitAppLog();
        return AX_FALSE;
    }

    LOG_C("============== APP(APP Ver: %s, SDK Ver: %s) Started %s %s ==============\n", AXCL_BUILD_VERSION, GetSdkVersion().c_str(),
          __DATE__, __TIME__);
    return AX_TRUE;
}

AX_BOOL CAxclDemoAppSys::DeInit(AX_VOID) {
    if (!DeInitSysMods()) {
        return AX_FALSE;
    }

    LOG_C("============== APP(APP Ver: %s, SDK Ver: %s) Exited %s %s ==============\n", AXCL_BUILD_VERSION, GetSdkVersion().c_str(),
          __DATE__, __TIME__);

    DeInitAppLog();

    return AX_TRUE;
}

AX_BOOL CAxclDemoAppSys::InitAppLog(const string& strAppName) {
    APP_LOG_ATTR_T stAttr;
    memset(&stAttr, 0, sizeof(stAttr));
    stAttr.nTarget = APP_LOG_TARGET_STDOUT;
    stAttr.nLv = APP_LOG_WARN;
    strncpy(stAttr.szAppName, strAppName.c_str(), arraysize(stAttr.szAppName) - 1);

    AX_CHAR* env1 = getenv("APP_LOG_TARGET");
    if (env1) {
        stAttr.nTarget = atoi(env1);
    }

    AX_CHAR* env2 = getenv("APP_LOG_LEVEL");
    if (env2) {
        stAttr.nLv = atoi(env2);
    }

    return (0 == AX_APP_Log_Init(&stAttr)) ? AX_TRUE : AX_FALSE;
}

AX_BOOL CAxclDemoAppSys::DeInitAppLog(AX_VOID) {
    AX_APP_Log_DeInit();
    return AX_TRUE;
}

AX_BOOL CAxclDemoAppSys::InitSysMods(AX_VOID) {
    m_arrMods.clear();
    m_arrMods.reserve(8);
    m_arrMods.push_back({AX_FALSE, "SYS", bind(&CAxclDemoAppSys::APP_SYS_Init, this), bind(&CAxclDemoAppSys::APP_SYS_DeInit, this)});
    m_arrMods.push_back({AX_FALSE, "VDEC", bind(&CAxclDemoAppSys::APP_VDEC_Init, this), bind(&CAxclDemoAppSys::APP_VDEC_DeInit, this)});
    m_arrMods.push_back({AX_FALSE, "VENC", bind(&CAxclDemoAppSys::APP_VENC_Init, this), bind(&CAxclDemoAppSys::APP_VENC_DeInit, this)});
    m_arrMods.push_back({AX_FALSE, "IVPS", bind(&CAxclDemoAppSys::APP_IVPS_Init, this), bind(&CAxclDemoAppSys::APP_IVPS_DeInit, this)});
    // m_arrMods.push_back({AX_FALSE, "SKEL", bind(&CAxclDemoAppSys::APP_SKEL_Init, this), bind(&CAxclDemoAppSys::APP_SKEL_DeInit, this)});

#ifndef __DUMMY_VO__
    m_arrMods.push_back({AX_FALSE, "DISP", AX_VO_Init, AX_VO_Deinit}); // for host only
#endif

    for (auto& m : m_arrMods) {
        AX_S32 ret = m.Init();
        if (0 != ret) {
            LOG_E("%s: init module %s fail, ret = 0x%x", __func__, m.strName.c_str(), ret);
            return AX_FALSE;
        } else {
            m.bInited = AX_TRUE;
        }
    }

    return AX_TRUE;
}

AX_BOOL CAxclDemoAppSys::DeInitSysMods(AX_VOID) {
    const auto nSize = m_arrMods.size();
    if (0 == nSize) {
        return AX_TRUE;
    }

    for (AX_S32 i = (AX_S32)(nSize - 1); i >= 0; --i) {
        if (m_arrMods[i].bInited) {
            AX_S32 ret = m_arrMods[i].DeInit();
            if (0 != ret) {
                LOG_E("%s: deinit module %s fail, ret = 0x%x", __func__, m_arrMods[i].strName.c_str(), ret);
                return AX_FALSE;
            }

            m_arrMods[i].bInited = AX_FALSE;
        }
    }

    m_arrMods.clear();
    return AX_TRUE;
}

AX_S32 CAxclDemoAppSys::APP_SKEL_Init(AX_VOID) {
    /* only for axcl device */
    return 0;
}

AX_S32 CAxclDemoAppSys::APP_SKEL_DeInit(AX_VOID) {
    /* only for axcl device */
    return 0;
}

AX_S32 CAxclDemoAppSys::APP_IVPS_Init(AX_VOID) {
    AX_S32 ret = AX_IVPS_Init();
    if (0 != ret) {
        LOG_E("%s: AX_IVPS_Init() fail, ret = 0x%x", __func__, ret);
        return ret;
    }

    ret = AXCL_IVPS_Init();
    if (0 != ret) {
        LOG_E("%s: AXCL_IVPS_Init() fail, ret = 0x%x", __func__, ret);
        return ret;
    }

    return 0;
}

AX_S32 CAxclDemoAppSys::APP_IVPS_DeInit(AX_VOID) {
    AX_S32 ret = AX_IVPS_Deinit();
    if (0 != ret) {
        LOG_E("%s: AX_IVPS_Deinit() fail, ret = 0x%x", __func__, ret);
        return ret;
    }

    ret = AXCL_IVPS_Deinit();
    if (0 != ret) {
        LOG_E("%s: AXCL_IVPS_Deinit() fail, ret = 0x%x", __func__, ret);
        return ret;
    }

    return 0;
}

AX_S32 CAxclDemoAppSys::APP_VDEC_Init(AX_VOID) {
    /* for host */
    AX_VDEC_MOD_ATTR_T stModAttr;
    memset(&stModAttr, 0, sizeof(stModAttr));
    stModAttr.u32MaxGroupCount = m_tAttrHost.nMaxGrp;
    stModAttr.enDecModule = AX_ENABLE_ONLY_VDEC;
    AX_S32 ret = AX_VDEC_Init(&stModAttr);
    if (0 != ret) {
        LOG_E("%s: AX_VDEC_Init() fail, ret = 0x%x", __func__, ret);
        return ret;
    }

    /* for device */
    AX_VDEC_MOD_ATTR_T _stModAttr;
    memset(&_stModAttr, 0, sizeof(_stModAttr));
    _stModAttr.u32MaxGroupCount = m_tAttrDevice.nMaxGrp;
    _stModAttr.enDecModule = AX_ENABLE_ONLY_VDEC;
    ret = AXCL_VDEC_Init(&_stModAttr);
    if (0 != ret) {
        LOG_E("%s: AXCL_VDEC_Init() fail, ret = 0x%x", __func__, ret);
        return ret;
    }

    return 0;
}

AX_S32 CAxclDemoAppSys::APP_VDEC_DeInit(AX_VOID) {
    /* for host */
    AX_S32 ret = AX_VDEC_Deinit();
    if (0 != ret) {
        LOG_E("%s: AX_VDEC_Deinit() fail, ret = 0x%x", __func__, ret);
    }

    /* for device */
    ret = AXCL_VDEC_Deinit();
    if (0 != ret) {
        LOG_E("%s: AXCL_VDEC_Deinit() fail, ret = 0x%x", __func__, ret);
    }

    return 0;
}

AX_S32 CAxclDemoAppSys::APP_VENC_Init(AX_VOID) {
    /* only for axcl device */
    AX_VENC_MOD_ATTR_T stModAttr;
    memset(&stModAttr, 0x0, sizeof(stModAttr));
    stModAttr.enVencType = AX_VENC_MULTI_ENCODER;
    stModAttr.stModThdAttr.u32TotalThreadNum = 1;
    stModAttr.stModThdAttr.bExplicitSched = AX_FALSE;

    AX_S32 ret = AXCL_VENC_Init(&stModAttr);
    if (0 != ret) {
        LOG_E("%s: AXCL_VENC_Init() fail, ret = 0x%x", __func__, ret);
        return ret;
    }

    return 0;
}

AX_S32 CAxclDemoAppSys::APP_VENC_DeInit(AX_VOID) {
    /* only for axcl device */
    AX_S32 ret = AXCL_VENC_Deinit();
    if (0 != ret) {
        LOG_E("%s: AXCL_VENC_Deinit() fail, ret = 0x%x", __func__, ret);
    }

    return 0;
}

AX_S32 CAxclDemoAppSys::APP_SYS_Init(AX_VOID) {
    /* for host */
    AX_S32 ret = AX_SYS_Init();
    if (0 != ret) {
        LOG_E("%s: AX_SYS_Init() fail, ret = 0x%x", __func__, ret);
        return ret;
    }

    /* for device */
    ret = AXCL_SYS_Init();
    if (0 != ret) {
        LOG_E("%s: AXCL_SYS_Init() fail, ret = 0x%x", __func__, ret);
        return ret;
    }

    AX_APP_Log_SetSysModuleInited(AX_TRUE);

    ret = AX_POOL_Exit();
    if (0 != ret) {
        LOG_E("%s: AX_POOL_Exit() fail, ret = 0x%x", __func__, ret);
        return ret;
    }

    ret = AXCL_POOL_Exit();
    if (0 != ret) {
        LOG_E("%s: AXCL_POOL_Exit() fail, ret = 0x%x", __func__, ret);
        return ret;
    }

    return 0;
}

AX_S32 CAxclDemoAppSys::APP_SYS_DeInit(AX_VOID) {
    AX_S32 ret = AX_SUCCESS;

    if (!CAXPoolManager::GetInstance()->DestoryAllPools()) {
        return -1;
    }

    AX_APP_Log_SetSysModuleInited(AX_FALSE);

    /* for host */
    ret = AX_SYS_Deinit();
    if (0 != ret) {
        LOG_E("%s: AX_SYS_Deinit() fail, ret = 0x%x", __func__, ret);
    }

    /* for device */
    ret = AXCL_SYS_Deinit();
    if (0 != ret) {
        LOG_E("%s: AXCL_SYS_Deinit() fail, ret = 0x%x", __func__, ret);
    }

    return 0;
}

string CAxclDemoAppSys::GetSdkVersion(AX_VOID) {
    string strSdkVer{"Unknown"};
    if (FILE* fp = fopen("/proc/ax_proc/version", "r")) {
        constexpr AX_U32 SDK_VERSION_PREFIX_LEN = strlen("Ax_Version") + 1;
        AX_CHAR szSdkVer[64] = {0};
        fread(&szSdkVer[0], 64, 1, fp);
        fclose(fp);
        szSdkVer[strlen(szSdkVer) - 1] = 0;
        strSdkVer = szSdkVer + SDK_VERSION_PREFIX_LEN;
    }

    return strSdkVer;
}

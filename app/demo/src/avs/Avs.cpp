/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include "Avs.h"
#include "ax_avs_api.h"
#include "AppLogApi.h"
#include <thread>
#include <chrono>
#include "IObserver.h"

#define TAG "AVS"
#define DEFAULT_AVS_CHN_DEPTH   (2)

extern string g_SDKVersion;

AX_BOOL CAvs::Init(const AX_APP_AVS_ATTR_T& stAvsAttr) {
    memset((AX_VOID *)&m_stAvsGrpAttr, 0, sizeof(AX_AVS_GRP_ATTR_T));
    memset((AX_VOID *)&m_stAvsChnAttr, 0, sizeof(AX_AVS_CHN_ATTR_T));

    m_SDKVersion = g_SDKVersion;
    m_stAvsAttr = stAvsAttr;

    m_stAvsChnAttr.u32Depth = DEFAULT_AVS_CHN_DEPTH;
    m_stAvsChnAttr.stCompressInfo = stAvsAttr.stAvsCompress;
    m_stAvsChnAttr.bBlockEnable = AX_FALSE;

    m_stAvsGrpAttr.stPipeSeqInfo = m_stAvsAttr.tCaliInitParam.tPipeInfo.stPipeSeqInfo;
    m_stAvsGrpAttr.stAVSGrpAttr.bSyncPipe = m_stAvsAttr.bSyncPipe;
    m_stAvsGrpAttr.stAVSGrpAttr.enMode = m_stAvsAttr.enMode;
    m_stAvsGrpAttr.stAVSGrpAttr.eEngineMode = m_stAvsAttr.eEngineMode;
    m_stAvsGrpAttr.stAVSGrpAttr.bDynamicSeam = m_stAvsAttr.bDynamicSeam;
    m_stAvsGrpAttr.stAVSGrpAttr.enBlendMode = m_stAvsAttr.enBlendMode;
    m_stAvsGrpAttr.bPrioOnsiteCali = m_stAvsAttr.bPrioOnsiteCali;
    m_tAeAwbSyncRatio.stPipeSeqInfo = m_stAvsGrpAttr.stPipeSeqInfo;
    if (!stAvsAttr.bAroundView) {
        m_tAeAwbSyncRatio.stPipeSeqInfo.nPipeNum -= 1;
    }

    switch(m_stAvsAttr.enParamType) {
        case E_AVS_PARAM_TYPE_NORMAL:
            m_stAvsGrpAttr.stAVSGrpAttr.enCalibrationMode = AVS_CALIBRATION_PARAM_CAMERA;
            m_stAvsGrpAttr.stAVSGrpAttr.stGrpCameraParam.enCameraType = AVS_CAMERA_TYPE_PINHOLE;
            m_stAvsGrpAttr.stAVSGrpAttr.stOutAttr.enPrjMode = m_stAvsAttr.eProjectionMode;
            break;
        case E_AVS_PARAM_TYPE_MESHTABLE:
            m_stAvsGrpAttr.stAVSGrpAttr.enCalibrationMode = AVS_CALIBRATION_PARAM_TRANSFORM;
            break;
        default:
            LOG_MM_E(TAG, "Unsupported avs parameter type 0x%x", m_stAvsAttr.enParamType);
            return AX_FALSE;
    }

    AX_S32 s32Ret = AX_AVSCALI_Init(&m_stAvsAttr.tCaliInitParam);
    if (AX_SUCCESS != s32Ret) {
        LOG_MM_E(TAG, "AX_AVSCALI_Init failed, s32Ret 0x%x", s32Ret);
        return AX_FALSE;
    }

    return LoadParam();
}

AX_BOOL CAvs::DeInit() {
    AX_S32 s32Ret {-1};

    s32Ret = AX_AVSCALI_DeInit();
    if (AX_SUCCESS != s32Ret) {
        LOG_MM_W(TAG, "AX_AVSCALI_DeInit failed, s32Ret 0x%x", s32Ret);
    }

    return AX_TRUE;
}

AX_BOOL CAvs::Start(AX_VOID) {
    AX_S32 s32Ret {-1};

    s32Ret = AX_AVS_CreateGrp(m_nGrp, &m_stAvsGrpAttr.stAVSGrpAttr);
    if (AX_SUCCESS != s32Ret) {
        LOG_MM_E(TAG, "AX_AVS_CreateGrp failed, s32Ret 0x%x", s32Ret);
        return AX_FALSE;
    }

    s32Ret = AX_AVS_SetChnAttr(m_nGrp, m_nChn, &m_stAvsChnAttr);
    if (AX_SUCCESS != s32Ret) {
        LOG_MM_E(TAG, "AX_AVS_SetChnAttr failed, s32Ret 0x%x", s32Ret);
        return AX_FALSE;
    }

    s32Ret = AX_AVS_EnableChn(m_nGrp, m_nChn);
    if (AX_SUCCESS != s32Ret) {
        LOG_MM_E(TAG, "AX_AVS_EnableChn failed, s32Ret 0x%x", s32Ret);
        return AX_FALSE;
    }

    s32Ret = AX_AVS_StartGrp(m_nGrp);
    if (AX_SUCCESS != s32Ret) {
        LOG_MM_E(TAG, "AX_AVS_StartGrp failed, s32Ret 0x%x", s32Ret);
        return AX_FALSE;
    }

    return AX_TRUE;
}

AX_BOOL CAvs::Stop() {
    AX_S32 s32Ret {-1};

    s32Ret = AX_AVS_DisableChn(m_nGrp, m_nChn);
    if (AX_SUCCESS != s32Ret) {
        LOG_MM_W(TAG, "AX_AVS_DisableChn failed, s32Ret 0x%x", s32Ret);
    }

    s32Ret = AX_AVS_StopGrp(m_nGrp);
    if (AX_SUCCESS != s32Ret) {
        LOG_MM_W(TAG, "AX_AVS_StopGrp failed, s32Ret 0x%x", s32Ret);
    }

    s32Ret = AX_AVS_DestroyGrp(m_nGrp);
    if (AX_SUCCESS != s32Ret) {
        LOG_MM_E(TAG, "AX_AVS_DestroyGrp failed, s32Ret 0x%x", s32Ret);
    }

    return AX_TRUE;
}

AX_BOOL CAvs::LoadParam(AX_BOOL bOnlyAvsParam /*= AX_FALSE*/) {
    AX_S32 s32Ret{-1};

    s32Ret = AX_AVSCALI_LoadAvsParam(nullptr, &m_stAvsGrpAttr, &m_bIsCalibrated);
    if (AX_SUCCESS != s32Ret) {
        if (AX_ERR_AVSCALI_DATA_IMCOMPATIBLE == s32Ret) {
            LOG_MM_E(TAG, "Broken avs parameters, need to enable cali, and recalibrate");
        }

        if (!(AX_ERR_AVSCALI_DATA_IMCOMPATIBLE == s32Ret && m_stAvsAttr.u8CaliEnable)) {
            LOG_MM_E(TAG, "Broken avs parameters, need to enable cali, and recalibrate, ret=0x%x caliEnable=%d", s32Ret, m_stAvsAttr.u8CaliEnable);
            return AX_FALSE;
        }

        return AX_TRUE;
    }

    g_SDKVersion = m_SDKVersion + (m_bIsCalibrated ? " Calibrated" : " UnCalibrated");

    if (!bOnlyAvsParam) {
        s32Ret = AX_AVSCALI_LoadAeAwbSyncRatio(nullptr, &m_tAeAwbSyncRatio);
        if (AX_SUCCESS != s32Ret) {
            LOG_MM_E(TAG, "Failed to load ae/awb sync ratio, s32Ret 0x%x", s32Ret);
        }
    }

    m_stAvsResolution.u32Width  = m_stAvsGrpAttr.stAVSGrpAttr.stOutAttr.u32Width;
    m_stAvsResolution.u32Height = m_stAvsGrpAttr.stAVSGrpAttr.stOutAttr.u32Height;

    m_stAvsChnAttr.stOutAttr.u32Width = m_stAvsGrpAttr.stAVSGrpAttr.stOutAttr.u32Width;
    m_stAvsChnAttr.stOutAttr.u32Height = m_stAvsGrpAttr.stAVSGrpAttr.stOutAttr.u32Height;

    LOG_MM_I(TAG, "tOutFrame size: (%d, %d)", m_stAvsGrpAttr.stAVSGrpAttr.stOutAttr.u32Width, m_stAvsGrpAttr.stAVSGrpAttr.stOutAttr.u32Height);

    return AX_TRUE;
}

AX_BOOL CAvs::StartAVSCalibrate() {
    if (!m_stAvsAttr.u8CaliEnable) {
        return AX_TRUE;
    }

    LOG_MM_I(TAG, "Start Calibrate, snsNum: %d, isp chn: %d, width: %d, height: %d, avs cali param path: %s, ip:%s, port:%d",
            m_stAvsAttr.tCaliInitParam.tPipeInfo.stPipeSeqInfo.nPipeNum,
            m_stAvsAttr.tCaliInitParam.tPipeInfo.nChn,
            m_stAvsAttr.tCaliInitParam.tPipeInfo.nImgWidth, m_stAvsAttr.tCaliInitParam.tPipeInfo.nImgHeight,
            m_stAvsAttr.tCaliInitParam.strCaliDataPath,
            m_stAvsAttr.strCaliServerIP.c_str(), m_stAvsAttr.u16CaliServerPort);

    AX_S32 s32Ret{-1};

    std::this_thread::sleep_for(std::chrono::milliseconds(1500));

    s32Ret = AX_AVSCALI_Start(m_stAvsAttr.strCaliServerIP.c_str(), m_stAvsAttr.u16CaliServerPort);
    if (AX_SUCCESS != s32Ret) {
        LOG_MM_E(TAG, "AX_AVSCALI_Start failed, s32Ret 0x%x", s32Ret);
        return AX_FALSE;
    }

    return AX_TRUE;
}

AX_VOID CAvs::StopAVSCalibrate() {
    if (m_stAvsAttr.u8CaliEnable) {
        AX_S32 s32Ret = AX_AVSCALI_Stop();
        if (AX_SUCCESS != s32Ret) {
            LOG_MM_W(TAG, "AX_AVSCALI_Stop failed, s32Ret 0x%x", s32Ret);
        }
    }
}

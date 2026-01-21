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

#include <map>
#include <string>
#include <mutex>
#include <thread>
#include "GlobalDef.h"
#include "IModule.h"
#include "IspAlgoWrapper.hpp"

typedef struct axSNS_LIB_INFO_T {
    std::string strLibName;
    std::string strObjName;
} SNS_LIB_INFO_T;

typedef struct _APP_ISP_IMAGE_ATTR_T {
    AX_U8 nAutoMode{0}; /*1:Auto; 0:Manual*/
    AX_U8 nSharpness{0};
    AX_U8 nBrightness{0};
    AX_U8 nContrast{0};
    AX_U8 nSaturation{0};
    AX_U8 nYCprocEn{0};
} APP_ISP_IMAGE_ATTR_T;

typedef AX_BOOL (*SensorAttrUpdCallback)(ISensor* pInstance);

using namespace std;
class CBaseSensor : public ISensor {
public:
    CBaseSensor(SENSOR_CONFIG_T tSensorConfig);
    virtual ~CBaseSensor(AX_VOID) = default;

public:
    virtual AX_BOOL Init(AX_VOID);
    virtual AX_BOOL DeInit(AX_VOID);

    virtual AX_BOOL Open() override;
    virtual AX_BOOL Close() override;

    virtual AX_VOID RegisterIspAlgo(const ISP_ALGO_INFO_T& tAlg) override;

    virtual AX_IMG_FORMAT_E GetMaxImgFmt();
    virtual AX_SNS_HDR_MODE_E GetMaxHdrMode();

    const AX_SNS_ATTR_T& GetSnsAttr(AX_VOID) const override;
    AX_VOID SetSnsAttr(const AX_SNS_ATTR_T& tSnsAttr) override;

    const SNS_CLK_ATTR_T& GetSnsClkAttr(AX_VOID) const override;
    AX_VOID SetSnsClkAttr(const SNS_CLK_ATTR_T& tClkAttr);

    const AX_VIN_DEV_ATTR_T& GetDevAttr(AX_VOID) const override;
    AX_VOID SetDevAttr(const AX_VIN_DEV_ATTR_T& tDevAttr) override;

    const AX_MIPI_RX_DEV_T& GetMipiRxAttr(AX_VOID) const override;
    AX_VOID SetMipiRxAttr(const AX_MIPI_RX_DEV_T& tMipiRxAttr) override;

    const AX_VIN_PIPE_ATTR_T& GetPipeAttr(AX_U8 nPipe) const override;
    AX_VOID SetPipeAttr(AX_U8 nPipe, const AX_VIN_PIPE_ATTR_T& tPipeAttr) override;

    const AX_VIN_CHN_ATTR_T& GetChnAttr(AX_U8 nPipe, AX_U8 nChannel) const override;
    AX_VOID SetChnAttr(AX_U8 nPipe, AX_U8 nChannel, const AX_VIN_CHN_ATTR_T& tChnAttr) override;

    const SNS_ABILITY_T& GetAbilities(AX_VOID) const override;

    const SENSOR_CONFIG_T& GetSnsConfig(AX_VOID) const;

    AX_U32 GetPipeCount();
    AX_VOID RegAttrUpdCallback(SensorAttrUpdCallback callback);

    AX_VOID InitSensor(AX_U8 nPipe);
    AX_VOID ExitSensor(AX_U8 nPipe);

    AX_BOOL RestoreIspIQAttr(APP_ISP_IMAGE_ATTR_T& tAttr);
    AX_BOOL GetIspIQAttr(APP_ISP_IMAGE_ATTR_T& tAttr);
    AX_BOOL SetIspIQAttr(const APP_ISP_IMAGE_ATTR_T& tAttr);
    AX_BOOL ChangeDaynightMode(AX_DAYNIGHT_MODE_E eDaynightMode);
    AX_VOID ChangeHdrMode(AX_U32 nSnsMode, AX_S32 nFps = -1);
    AX_BOOL UpdateSnsAttr();
    AX_BOOL TriggerFlash(AX_S8 nLaneId, AX_S8 nFlashId, AX_U8 nTriggerCount, AX_U8 nFrameInterval);
    AX_BOOL ChangeSnsMirrorFlip(AX_BOOL bMirror, AX_BOOL bFlip);
    AX_U32  EnableMultiCamSync(AX_BOOL bEnable);
    AX_BOOL EnableEIS(AX_BOOL bEnable);
    AX_BOOL EZoom(AX_S32 nEZoomRatio, AX_S32 nCenterOffsetX=0, AX_S32 nCenterOffsetY=0);
    AX_BOOL UpdateLDC(AX_BOOL bLdcEnable, AX_BOOL bAspect, AX_S16 nXRatio, AX_S16 nYRatio, AX_S16 nXYRatio, AX_S16 nDistorRatio);

    AX_BOOL SetChnCompress(AX_U8 nPipe, AX_U8 nChannel, AX_BOOL bCompressOn);

    AX_BOOL IsIFERawDumpEnable();
    AX_BOOL IsNpuOfflinePipe(AX_U8 nPipe);
    AX_U8   GetIFERawDumpPipe();
    AX_BOOL SwitchIFERawDumpPipe(AX_U8 nPipe);

    AX_U8   GetSnapPipe();

    AX_BOOL EnableChn(AX_U8 nPipe, AX_U8 nChannel, AX_BOOL bEnable);

    AX_BOOL SetShutterAttr(AX_U8 nDevId, std::array<AX_VIN_SHUTTER_MODE_E, AX_SHUTTER_SEQ_NUM>& arr);

    AX_BOOL IsRotateEngine();
    AX_BOOL IsMirrorFlipEngine();

    AX_BOOL UpdateRotation(AX_VIN_ROTATION_E eRotation);

    AX_VIN_WARP_ATTR_T GetVinWarpAttr(AX_U8 nEngineMode);

    AX_VIN_ROTATION_E GetRotation(AX_BOOL bIgnorePipe = AX_TRUE, AX_U8 nPipe = 0);
    AX_BOOL IsMirror(AX_BOOL bIgnorePipe = AX_TRUE, AX_U8 nPipe = 0);
    AX_BOOL IsFlip(AX_BOOL bIgnorePipe = AX_TRUE, AX_U8 nPipe = 0);

    AX_BOOL OpenDev();
    AX_BOOL IspStreamOn();
    AX_VOID RestoreAttr();

protected:
    virtual AX_VOID InitSnsLibraryInfo(AX_VOID) = 0;
    virtual AX_VOID InitSnsAttr() = 0;
    virtual AX_VOID InitSnsClkAttr() = 0;
    virtual AX_VOID InitDevAttr() = 0;
    virtual AX_VOID InitPrivAttr() = 0;
    virtual AX_VOID InitPipeAttr() = 0;
    virtual AX_VOID InitMipiRxAttr() = 0;
    virtual AX_VOID InitChnAttr() = 0;
    virtual AX_VOID InitAbilities() = 0;
    virtual AX_VOID InitTriggerAttr() = 0;

protected:
    virtual AX_BOOL RegisterSensor(AX_U8 nPipe, AX_U8 nDevNode, AX_U8 nI2cAddr);
    virtual AX_BOOL UnRegisterSensor(AX_U8 nPipe);
    virtual AX_BOOL ResetSensorObj(AX_U8 nDevId, AX_U8 nPipe);
    AX_BOOL SetAeToManual(AX_U8 nPipe);
    AX_S32 COMMON_HW_GetSensorResetGpioNum(AX_U8 nDevId);
    AX_BOOL OpenAll();
    AX_BOOL CloseAll();

private:
    AX_BOOL InitISP(AX_VOID);
    AX_S32 CalcValueToIspIQ(AX_S32 nVal, AX_U8 nIntBits, AX_U8 nFracBits);
    AX_F32 CalcIspIQToValue(AX_S32 nVal, AX_U8 nIntBits, AX_U8 nFracBits);
    AX_S32 COMMON_HW_GetApdPlateId(AX_CHAR* apd_plate_id);
    AX_BOOL IsSnsRawTypeSet();
    AX_IMG_FORMAT_E GetDevPixelFmt();
    AX_BOOL SetIFEDumpAttr();

protected:
    SENSOR_CONFIG_T m_tSnsCfg;
    AX_VOID* m_pSnsLib{nullptr};
    AX_SENSOR_REGISTER_FUNC_T* m_pSnsObj{nullptr};
    AX_SNS_CONNECT_TYPE_E m_eSnsBusType{ISP_SNS_CONNECT_I2C_TYPE};
    SNS_LIB_INFO_T m_tSnsLibInfo;

    AX_SNS_ATTR_T m_tSnsAttr;
    SNS_CLK_ATTR_T m_tSnsClkAttr;
    AX_VIN_DEV_ATTR_T m_tDevAttr;
    AX_VIN_PRIVATE_DATA_ATTR_T m_tPrivAttr;
    std::map<AX_U8, AX_VIN_PIPE_ATTR_T> m_mapPipe2Attr;
    AX_MIPI_RX_DEV_T m_tMipiRxDev;
    std::map<AX_U8, std::map<AX_U8, AX_VIN_CHN_ATTR_T>> m_mapPipe2ChnAttr;
    SNS_ABILITY_T m_tAbilities;
    SNS_SNAP_ATTR_T m_tSnapAttr;

    AX_IMG_FORMAT_E m_eImgFormatSDR;
    AX_IMG_FORMAT_E m_eImgFormatHDR;

    CIspAlgoWrapper m_algWrapper;

    SensorAttrUpdCallback m_cbAttrUpd{nullptr};

    SENSOR_PIPE_MAPPING_T m_arrPipeMapping[MAX_PIPE_PER_DEVICE];
    APP_ISP_IMAGE_ATTR_T m_tImageAttr;

    AX_BOOL m_bSnapshotProcCall{AX_FALSE};

    std::mutex m_mtx;

    AX_BOOL m_bSensorStarted{AX_FALSE};
};

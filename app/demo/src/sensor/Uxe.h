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
#include "BaseSensor.h"
#define MAX_MIPI_RX_DEV_ID_BUTT (8)

class CUxe : public CBaseSensor {
public:
    CUxe(AX_IMG_FORMAT_E eImgFormat, AX_U32 nW = 3840, AX_U32 nH = 2160, AX_U32 nFps = 60);
    virtual ~CUxe(AX_VOID);

    AX_BOOL Init(AX_VOID) override;
    AX_BOOL DeInit(AX_VOID) override;

    AX_BOOL Open() override;
    AX_BOOL Close() override;

    const SENSOR_CONFIG_T& GetSensorCfg();
    AX_VOID SetMipiDevId(AX_U32 devId);

protected:
    virtual AX_VOID InitSnsLibraryInfo(AX_VOID) override;
    virtual AX_VOID InitSnsAttr() override;
    virtual AX_VOID InitSnsClkAttr() override;
    virtual AX_VOID InitDevAttr() override;
    virtual AX_VOID InitPrivAttr() override;
    virtual AX_VOID InitPipeAttr() override;
    virtual AX_VOID InitMipiRxAttr() override;
    virtual AX_VOID InitChnAttr() override;
    virtual AX_VOID InitAbilities() override;
    virtual AX_VOID InitTriggerAttr() override;

private:
    AX_IMG_FORMAT_E m_eImgFormat;
    AX_FRAME_INTERRUPT_ATTR_T m_tDevFrmIntAttr;
    AX_U32 m_nW;
    AX_U32 m_nH;
    AX_U32 m_nFps;
    AX_U32 m_nMipiDev = {MAX_MIPI_RX_DEV_ID_BUTT};
};

inline AX_VOID CUxe::SetMipiDevId(AX_U32 devId) {
    if (devId < MAX_MIPI_RX_DEV_ID_BUTT) {
        m_nMipiDev = devId;
    }
}

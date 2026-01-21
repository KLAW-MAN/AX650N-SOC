/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include "OS04a10Vs.h"

COS04a10Vs::COS04a10Vs(SENSOR_CONFIG_T tSensorConfig) : COS04a10(tSensorConfig) {
}

COS04a10Vs::~COS04a10Vs(AX_VOID) {
}

AX_VOID COS04a10Vs::InitSnsLibraryInfo(AX_VOID) {
    m_tSnsLibInfo.strObjName = "gSnsos04a10ObjVs";
    m_tSnsLibInfo.strLibName = "libsns_os04a10.so";
}

AX_VOID COS04a10Vs::InitDevAttr() {
    COS04a10::InitDevAttr();
    m_tDevAttr.eSnsOutputMode = m_tSnsCfg.eSnsOutputMode;
    m_tDevAttr.tMipiIntfAttr.szImgVc[0] = 0x02;
    m_tDevAttr.tMipiIntfAttr.szImgDt[0] = 0x2b;
    m_tDevAttr.eSnsMode = AX_SNS_LINEAR_MODE;
}
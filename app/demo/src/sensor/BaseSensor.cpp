/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include "BaseSensor.h"
#include <dlfcn.h>
#include <sys/prctl.h>
#include "AppLogApi.h"
#include "CommonUtils.hpp"
#include "ElapsedTimer.hpp"
#include "ax_nt_ctrl_api.h"
#include "ax_nt_stream_api.h"
#include "SensorOptionHelper.h"
#include "CmdLineParser.h"

#define SENSOR "SENSOR"
#define SDRKEY "sdr"
#define HDRKEY "hdr"
#define BOARD_ID_LEN 128

CBaseSensor::CBaseSensor(SENSOR_CONFIG_T tSensorConfig)
    : m_tSnsCfg(tSensorConfig), m_eImgFormatSDR(AX_FORMAT_BAYER_RAW_10BPP), m_eImgFormatHDR(AX_FORMAT_BAYER_RAW_10BPP) {
}

AX_BOOL CBaseSensor::InitISP(AX_VOID) {
    memset(&m_tSnsAttr, 0, sizeof(AX_SNS_ATTR_T));
    memset(&m_tDevAttr, 0, sizeof(AX_VIN_DEV_ATTR_T));
    memset(&m_tPrivAttr, 0, sizeof(AX_VIN_PRIVATE_DATA_ATTR_T));
    memset(&m_tMipiRxDev, 0, sizeof(AX_MIPI_RX_DEV_T));

    InitMipiRxAttr();
    InitSnsAttr();
    InitSnsClkAttr();
    InitDevAttr();
    InitPrivAttr();
    InitPipeAttr();
    InitChnAttr();
    InitSnsLibraryInfo();
    InitTriggerAttr();

    if (m_cbAttrUpd) {
        m_cbAttrUpd(this);
    }

    if (IsSnsRawTypeSet()) {
        m_tSnsAttr.eRawType = (AX_RAW_TYPE_E)m_tSnsCfg.arrSnsRawType[m_tSnsCfg.eSensorMode];
        m_tDevAttr.ePixelFmt = GetDevPixelFmt();
        for (AX_U8 i = 0; i < m_tSnsCfg.nPipeCount; i++) {
            AX_U8 nPipe = m_tSnsCfg.arrPipeAttr[i].nPipeID;
            m_mapPipe2Attr[nPipe].ePixelFmt = m_tDevAttr.ePixelFmt;
        }
        LOG_MM_I(SENSOR, "Sns[%d] reset eRawType: %d, ePixelFmt: 0x%x for sns_mode: %d",
                          m_tSnsCfg.nSnsID, m_tSnsAttr.eRawType, m_tDevAttr.ePixelFmt, m_tSnsCfg.eSensorMode);
    }

    return AX_TRUE;
}

AX_BOOL CBaseSensor::Init() {
    if (!InitISP()) {
        LOG_M_E(SENSOR, "Sensor %d init failed.", m_tSnsCfg.nSnsID);
        return AX_FALSE;
    }

    return AX_TRUE;
}

AX_BOOL CBaseSensor::DeInit() {

    memset(&m_tSnsAttr, 0, sizeof(AX_SNS_ATTR_T));
    memset(&m_tDevAttr, 0, sizeof(AX_VIN_DEV_ATTR_T));
    memset(&m_tPrivAttr, 0,  sizeof(AX_VIN_PRIVATE_DATA_ATTR_T));
    memset(&m_tMipiRxDev, 0, sizeof(AX_MIPI_RX_DEV_T));

    m_cbAttrUpd = nullptr;

    return AX_TRUE;
}

AX_BOOL CBaseSensor::Open() {
    if (!OpenAll()) {
        return AX_FALSE;
    }

    RestoreAttr();

    return AX_TRUE;
}

AX_BOOL CBaseSensor::Close() {
    if (!CloseAll()) {
        return AX_FALSE;
    }

    return AX_TRUE;
}

AX_BOOL CBaseSensor::OpenAll() {
    std::lock_guard<std::mutex> _ApiLck(m_mtx);

    LOG_MM(SENSOR, "Sns[%d] [Dev:%d, RxDev:%d] +++", m_tSnsCfg.nSnsID, m_tSnsCfg.nDevID, m_tSnsCfg.nRxDev);

    if (!OpenDev()) {
        return AX_FALSE;
    }

    if (!IspStreamOn()) {
        return AX_FALSE;
    }

    LOG_MM(SENSOR, "Sns[%d] [Dev:%d, RxDev:%d] +++", m_tSnsCfg.nSnsID, m_tSnsCfg.nDevID, m_tSnsCfg.nRxDev);

    return AX_TRUE;
}

AX_BOOL CBaseSensor::CloseAll() {
    std::lock_guard<std::mutex> _ApiLck(m_mtx);

    LOG_MM(SENSOR, "Sns[%d] [Dev:%d, RxDev:%d] +++", m_tSnsCfg.nSnsID, m_tSnsCfg.nDevID, m_tSnsCfg.nRxDev);

    AX_S32 nRet = AX_SUCCESS;

    m_bSensorStarted = AX_TRUE;

    AX_U8 nDevID = m_tSnsCfg.nDevID;
    AX_U8 nRxDev = m_tSnsCfg.nRxDev;

    if (AX_TRUE == m_tSnsCfg.bEnableFlash) {
        nRet = AX_VIN_DisableVSync(nDevID);
        if (0 != nRet) {
            LOG_M_E(SENSOR, "AX_VIN_EnableVSync failed, nRet=0x%x.\n", nRet);
            return AX_FALSE;
        }

        nRet = AX_VIN_DisableHSync(nDevID);
        if (0 != nRet) {
            LOG_M_E(SENSOR, "AX_VIN_EnableHSync failed, nRet=0x%x.\n", nRet);
            return AX_FALSE;
        }

        nRet = AX_VIN_DisableStrobe(nDevID, LIGHT_STROBE);
        if (0 != nRet) {
            LOG_M_E(SENSOR, "AX_VIN_EnableStrobe failed, nRet=0x%x.\n", nRet);
            return AX_FALSE;
        }
    }

    for (AX_U8 i = 0; i < GetPipeCount(); i++) {
        AX_U8 nPipeID = m_tSnsCfg.arrPipeAttr[i].nPipeID;

        if (!m_tSnsCfg.arrPipeAttr[i].bSnapshot) {
            nRet = AX_ISP_Stop(nPipeID);
            if (0 != nRet) {
                LOG_M_E(SENSOR, "AX_ISP_Stop failed, ret=0x%x.", nRet);
                return AX_FALSE;
            }
        }
    }

    nRet = AX_VIN_DisableDev(nDevID);
    if (0 != nRet) {
        LOG_M_E(SENSOR, "AX_VIN_DisableDev failed, ret=0x%x.", nRet);
        return AX_FALSE;
    }

    for (AX_U8 i = 0; i < GetPipeCount(); i++) {
        AX_U8 nPipeID = m_tSnsCfg.arrPipeAttr[i].nPipeID;

        nRet = AX_ISP_StreamOff(nPipeID);
        if (0 != nRet) {
            LOG_M_E(SENSOR, "AX_ISP_StreamOff failed, ret=0x%x.", nRet);
            return AX_FALSE;
        }
    }

    if (AX_VIN_DEV_OFFLINE == m_tSnsCfg.eDevMode) {
        AX_VIN_DUMP_ATTR_T tDumpAttr;
        tDumpAttr.bEnable = AX_FALSE;
        nRet = AX_VIN_SetDevDumpAttr(nDevID, AX_VIN_DUMP_QUEUE_TYPE_DEV, &tDumpAttr);
        if (0 != nRet) {
            LOG_M_E(SENSOR, "AX_VIN_SetDevDumpAttr failed, ret=0x%x.", nRet);
            return AX_FALSE;
        }
    }

    for (AX_U8 i = 0; i < GetPipeCount(); i++) {
        nRet = AX_ISP_CloseSnsClk(m_tSnsClkAttr.nSnsClkIdx);
        if (0 != nRet) {
            LOG_M_E(SENSOR, "AX_ISP_CloseSnsClk failed, ret=0x%x.", nRet);
            return AX_FALSE;
        }
    }

     for (AX_U8 i = 0; i < GetPipeCount(); i++) {
        AX_U8 nPipeID = m_tSnsCfg.arrPipeAttr[i].nPipeID;

        nRet = AX_VIN_StopPipe(nPipeID);
        if (0 != nRet) {
            LOG_M_E(SENSOR, "AX_VIN_StopPipe failed, ret=0x%x.", nRet);
            return AX_FALSE;
        }

        for (AX_U8 j = AX_VIN_CHN_ID_MAIN; j < AX_VIN_CHN_ID_MAX; j++) {
            if (m_tSnsCfg.arrPipeAttr[i].arrChannelAttr[j].bChnEnable) {
                nRet = AX_VIN_DisableChn(nPipeID, (AX_VIN_CHN_ID_E)j);
                if (0 != nRet) {
                    LOG_M_E(SENSOR, "AX_VIN_DisableChn failed, ret=0x%x.", nRet);
                    return AX_FALSE;
                }
            }
        }

        nRet = AX_ISP_Close(nPipeID);
        if (0 != nRet) {
            LOG_M_E(SENSOR, "AX_ISP_Close failed, ret=0x%x.", nRet);
            return AX_FALSE;
        }

        if (!m_algWrapper.UnRegisterAlgoFromSensor(nPipeID)) {
            LOG_M_E(SENSOR, "UnRegisterAlgoFromSensor failed.");
            return AX_FALSE;
        }

        nRet = AX_ISP_Destroy(nPipeID);
        if (0 != nRet) {
            LOG_M_E(SENSOR, "AX_ISP_Destroy failed, ret=0x%x.", nRet);
            return AX_FALSE;
        }

        if (!UnRegisterSensor(nPipeID)) {
            LOG_M_E(SENSOR, "UnRegisterSensor failed, ret=0x%x.", nRet);
            return AX_FALSE;
        }

        nRet = AX_VIN_DestroyPipe(nPipeID);
        if (0 != nRet) {
            LOG_M_E(SENSOR, "AX_VIN_DestroyPipe failed, ret=0x%x.", nRet);
            return AX_FALSE;
        }
    }

    nRet = AX_MIPI_RX_Stop(nRxDev);
    if (0 != nRet) {
        LOG_M_E(SENSOR, "AX_MIPI_RX_Stop failed, ret=0x%x.", nRet);
        return AX_FALSE;
    }

    nRet = AX_VIN_DestroyDev(nDevID);
    if (0 != nRet) {
        LOG_M_E(SENSOR, "AX_VIN_DestroyDev failed, ret=0x%x.", nRet);
        return AX_FALSE;
    }

    LOG_MM(SENSOR, "Sns[%d] [Dev:%d, RxDev:%d] +++", m_tSnsCfg.nSnsID, m_tSnsCfg.nDevID, m_tSnsCfg.nRxDev);

    return AX_TRUE;
}

const AX_SNS_ATTR_T &CBaseSensor::GetSnsAttr(AX_VOID) const {
    return m_tSnsAttr;
}

AX_VOID CBaseSensor::SetSnsAttr(const AX_SNS_ATTR_T &tSnsAttr) {
    m_tSnsAttr = tSnsAttr;
}

const SNS_CLK_ATTR_T &CBaseSensor::GetSnsClkAttr(AX_VOID) const {
    return m_tSnsClkAttr;
}

AX_VOID CBaseSensor::SetSnsClkAttr(const SNS_CLK_ATTR_T &tClkAttr) {
    m_tSnsClkAttr = tClkAttr;
}

const AX_VIN_DEV_ATTR_T &CBaseSensor::GetDevAttr(AX_VOID) const {
    return m_tDevAttr;
}

AX_VOID CBaseSensor::SetDevAttr(const AX_VIN_DEV_ATTR_T &tDevAttr) {
    m_tDevAttr = tDevAttr;
}

const AX_MIPI_RX_DEV_T &CBaseSensor::GetMipiRxAttr(AX_VOID) const {
    return m_tMipiRxDev;
}

AX_VOID CBaseSensor::SetMipiRxAttr(const AX_MIPI_RX_DEV_T &tMipiRxAttr) {
    m_tMipiRxDev = tMipiRxAttr;
}

const AX_VIN_PIPE_ATTR_T &CBaseSensor::GetPipeAttr(AX_U8 nPipe) const {
    std::map<AX_U8, AX_VIN_PIPE_ATTR_T>::const_iterator itFinder = m_mapPipe2Attr.find(nPipe);
    if (itFinder == m_mapPipe2Attr.end()) {
        LOG_MM_E(SENSOR, "[Dev:%d] Pipe %d not configured.", m_tSnsCfg.nDevID, nPipe);
    }

    return itFinder->second;
}

AX_VOID CBaseSensor::SetPipeAttr(AX_U8 nPipe, const AX_VIN_PIPE_ATTR_T &tPipeAttr) {
    m_mapPipe2Attr[nPipe] = tPipeAttr;
}

const AX_VIN_CHN_ATTR_T &CBaseSensor::GetChnAttr(AX_U8 nPipe, AX_U8 nChannel) const {
    std::map<AX_U8, std::map<AX_U8, AX_VIN_CHN_ATTR_T>>::const_iterator itFinder = m_mapPipe2ChnAttr.find(nPipe);
    if (itFinder == m_mapPipe2ChnAttr.end()) {
        LOG_MM_E(SENSOR, "[Dev:%d] Pipe %d not configured.", m_tSnsCfg.nDevID, nPipe);
    }

    return itFinder->second.find(nChannel)->second;
}

AX_VOID CBaseSensor::SetChnAttr(AX_U8 nPipe, AX_U8 nChannel, const AX_VIN_CHN_ATTR_T &tChnAttr) {
    m_mapPipe2ChnAttr[nPipe][nChannel] = tChnAttr;
}

const SNS_ABILITY_T &CBaseSensor::GetAbilities(AX_VOID) const {
    return m_tAbilities;
}

const SENSOR_CONFIG_T &CBaseSensor::GetSnsConfig(AX_VOID) const {
    return m_tSnsCfg;
}

AX_U32 CBaseSensor::GetPipeCount() {
    return m_mapPipe2Attr.size();
}

AX_VOID CBaseSensor::RegisterIspAlgo(const ISP_ALGO_INFO_T &tAlg) {
    m_algWrapper.SetupAlgoInfo(tAlg);
}

AX_BOOL CBaseSensor::RegisterSensor(AX_U8 nPipe, AX_U8 nDevNode, AX_U8 nI2cAddr) {
    AX_S32 ret;
    AX_SNS_COMMBUS_T tSnsBusInfo;
    memset(&tSnsBusInfo, 0, sizeof(tSnsBusInfo));

    m_pSnsLib = dlopen(m_tSnsLibInfo.strLibName.c_str(), RTLD_LAZY);
    if (!m_pSnsLib) {
        LOG_M_E(SENSOR, "Load %s fail, %s", m_tSnsLibInfo.strLibName.c_str(), strerror(errno));
        return AX_FALSE;
    }

    m_pSnsObj = (AX_SENSOR_REGISTER_FUNC_T *)dlsym(m_pSnsLib, m_tSnsLibInfo.strObjName.c_str());
    if (!m_pSnsObj) {
        LOG_M_E(SENSOR, "Find symbol(%s) fail, %s", m_tSnsLibInfo.strObjName.c_str(), strerror(errno));
        goto __FAIL0__;
    }

    ret = AX_ISP_RegisterSensor(nPipe, m_pSnsObj);
    if (ret) {
        LOG_M_E(SENSOR, "AX_ISP_RegisterSensor(pipe: %d), ret = 0x%08X", nPipe, ret);
        goto __FAIL0__;
    }

    if (ISP_SNS_CONNECT_I2C_TYPE == m_eSnsBusType) {
        tSnsBusInfo.I2cDev = nDevNode;
    } else {
        tSnsBusInfo.SpiDev.bit4SpiDev = nDevNode;
        tSnsBusInfo.SpiDev.bit4SpiCs = 0;
    }

    if (m_pSnsObj->pfn_sensor_set_bus_info) {
        ret = m_pSnsObj->pfn_sensor_set_bus_info(nPipe, tSnsBusInfo);
        if (0 != ret) {
            LOG_M_E(SENSOR, "Sensor set bus info fail, ret = 0x%08X.", ret);
            goto __FAIL1__;
        } else {
            LOG_M_I(SENSOR, "set sensor bus idx %d", tSnsBusInfo.I2cDev);
        }
    } else {
        LOG_M_E(SENSOR, "sensor set buf info is not supported!");
        goto __FAIL1__;
    }

    if ((INVALID_I2C_ADDR != nI2cAddr) && (NULL != m_pSnsObj->pfn_sensor_set_slaveaddr)) {
        ret = m_pSnsObj->pfn_sensor_set_slaveaddr(nPipe, nI2cAddr);
        if (0 != ret) {
            LOG_M_E(SENSOR, "Set sensor slave addr failed with 0x%08X.", ret);
            goto __FAIL1__;
        }
        LOG_M_I(SENSOR, "Set sensor slave addr %d", nI2cAddr);
    }

    if (m_pSnsObj->pfn_sensor_mirror_flip) {
        AX_SNS_MIRRORFLIP_TYPE_E eSnsMirrorFlip = m_tSnsCfg.bSnsMirrorFlip ? AX_SNS_MF_MIRROR_FLIP : AX_SNS_MF_NORMAL;
        ret = m_pSnsObj->pfn_sensor_mirror_flip(nPipe, eSnsMirrorFlip);
        if (0 != ret) {
            LOG_M_E(SENSOR, "Sensor set mirror_flip fail, ret = 0x%08X.", ret);
            goto __FAIL1__;
        }
    }

    return AX_TRUE;

__FAIL1__:
    AX_ISP_UnRegisterSensor(nPipe);

__FAIL0__:
    dlclose(m_pSnsLib);
    m_pSnsLib = nullptr;
    m_pSnsObj = nullptr;
    return AX_FALSE;
}

AX_BOOL CBaseSensor::UnRegisterSensor(AX_U8 nPipe) {
    if (m_pSnsLib) {
        dlclose(m_pSnsLib);

        AX_S32 ret = AX_ISP_UnRegisterSensor(nPipe);
        if (0 != ret) {
            LOG_M_E(SENSOR, "AX_ISP_UnRegisterSensor(pipe: %d) fail, ret = 0x%08X", nPipe, ret);
            return AX_FALSE;
        }

        m_pSnsLib = nullptr;
        m_pSnsObj = nullptr;
    }

    return AX_TRUE;
}

AX_BOOL CBaseSensor::ResetSensorObj(AX_U8 nDevId, AX_U8 nPipe) {
    AX_BOOL ret = AX_TRUE;

    m_pSnsLib = dlopen(m_tSnsLibInfo.strLibName.c_str(), RTLD_LAZY);
    if (!m_pSnsLib) {
        LOG_M_E(SENSOR, "Load %s fail, %s", m_tSnsLibInfo.strLibName.c_str(), strerror(errno));
        return AX_FALSE;
    }

    m_pSnsObj = (AX_SENSOR_REGISTER_FUNC_T *)dlsym(m_pSnsLib, m_tSnsLibInfo.strObjName.c_str());
    if (!m_pSnsObj) {
        LOG_M_E(SENSOR, "Find symbol(%s) fail, %s", m_tSnsLibInfo.strObjName.c_str(), strerror(errno));
        ret = AX_FALSE;
        goto __FAIL__;
    }

    if (m_pSnsObj->pfn_sensor_reset) {
        if (0 != m_pSnsObj->pfn_sensor_reset(nPipe, COMMON_HW_GetSensorResetGpioNum(nDevId))) {
            LOG_M_E(SENSOR, "sensor reset fail, ret = 0x%08X.", ret);
            ret = AX_FALSE;
            goto __FAIL__;
        }
    } else {
        LOG_M_E(SENSOR, "sensor reset is not supported!");
        ret = AX_FALSE;
        goto __FAIL__;
    }

__FAIL__:
    dlclose(m_pSnsLib);
    m_pSnsLib = nullptr;
    m_pSnsObj = nullptr;
    return ret;
}

AX_VOID CBaseSensor::InitSensor(AX_U8 nPipe) {
    if (m_pSnsObj && m_pSnsObj->pfn_sensor_init) {
        m_pSnsObj->pfn_sensor_init(nPipe);
    }

    m_algWrapper.RegisterAlgoToSensor(m_pSnsObj, nPipe);
}

AX_VOID CBaseSensor::ExitSensor(AX_U8 nPipe) {
    m_algWrapper.UnRegisterAlgoFromSensor(nPipe);

    if (m_pSnsObj && m_pSnsObj->pfn_sensor_exit) {
        m_pSnsObj->pfn_sensor_exit(nPipe);
    }
}

AX_VOID CBaseSensor::RegAttrUpdCallback(SensorAttrUpdCallback callback) {
    if (nullptr == callback) {
        return;
    }

    m_cbAttrUpd = callback;
}

AX_IMG_FORMAT_E CBaseSensor::GetMaxImgFmt() {
    return m_eImgFormatSDR >= m_eImgFormatHDR ? m_eImgFormatSDR : m_eImgFormatHDR;
}

AX_SNS_HDR_MODE_E CBaseSensor::GetMaxHdrMode() {
    if (!GetAbilities().bSupportHDR) {
        return AX_SNS_LINEAR_MODE;
    }

    return AX_SNS_HDR_4X_MODE;
}

AX_BOOL CBaseSensor::SetAeToManual(AX_U8 nPipe) {
    AX_S32 nRet = AX_SUCCESS;
    AX_ISP_IQ_AE_PARAM_T tIspAeParam = {0};

    nRet = AX_ISP_IQ_GetAeParam(nPipe, &tIspAeParam);
    if (0 != nRet) {
        LOG_M_E(SENSOR, "AX_ISP_IQ_GetAeParam failed, ret=0x%x.", nRet);
        return AX_FALSE;
    }

    tIspAeParam.nEnable = AX_FALSE;

    nRet = AX_ISP_IQ_SetAeParam(nPipe, &tIspAeParam);
    if (0 != nRet) {
        LOG_M_E(SENSOR, "AX_ISP_IQ_SetAeParam failed, ret=0x%x.", nRet);
        return AX_FALSE;
    }

    return AX_TRUE;
}

AX_BOOL CBaseSensor::ChangeDaynightMode(AX_DAYNIGHT_MODE_E eDaynightMode) {
    std::lock_guard<std::mutex> _ApiLck(m_mtx);
    SENSOR_CONFIG_T tSnsCfg = GetSnsConfig();
    for (AX_U8 i = 0; i < tSnsCfg.nPipeCount; i++) {
        if (!m_bSensorStarted) {
            continue;
        }

        AX_U8 nPipId = tSnsCfg.arrPipeAttr[i].nPipeID;

        for (AX_U8 j = 0; j < AX_VIN_CHN_ID_MAX; j++) {
            if (!tSnsCfg.arrPipeAttr[i].arrChannelAttr[j].bChnEnable) {
                continue;
            }

            AX_S32 nRet = AX_VIN_SetChnDayNightMode(nPipId, (AX_VIN_CHN_ID_E)j, eDaynightMode);
            if (0 != nRet) {
                LOG_M_E(SENSOR, "[%d][%d] AX_VIN_SetChnDayNightMode failed, ret=0x%x.", nPipId, j, nRet);
                return AX_FALSE;
            } else {
                LOG_M_W(SENSOR, "[%d][%d] AX_VIN_SetChnDayNightMode OK.", nPipId, j);
            }
        }
    }

    m_tSnsCfg.eDayNight = eDaynightMode;

    return AX_TRUE;
}

AX_VOID CBaseSensor::ChangeHdrMode(AX_U32 nSnsMode, AX_S32 nFps) {
    m_tSnsCfg.eSensorMode = (AX_SNS_HDR_MODE_E)nSnsMode;
    if (-1 != nFps) {
        m_tSnsCfg.fFrameRate = (AX_F32)nFps;
        for (AX_U32 i = 0; i < m_tSnsCfg.nPipeCount; i++) {
            m_tSnsCfg.arrPipeAttr[i].fPipeFramerate = (AX_F32)nFps;
            LOG_MM_I(SENSOR, "[%d] fPipeFramerate %f", m_tSnsCfg.arrPipeAttr[i].nPipeID, m_tSnsCfg.arrPipeAttr[i].fPipeFramerate);
        }
    }
    return;
}

AX_BOOL CBaseSensor::ChangeSnsMirrorFlip(AX_BOOL bMirror, AX_BOOL bFlip)
{
    std::lock_guard<std::mutex> _ApiLck(m_mtx);

    LOG_M_W(SENSOR, "SNS[%d] change mirror=%d, flip=%d.", m_tSnsCfg.nSnsID, bMirror, bFlip);
    AX_S32 nRet = 0;
    AX_BOOL bSucces = AX_TRUE;

    for (AX_U8 i = 0; i < GetPipeCount(); i++) {
        if (!m_bSensorStarted || !m_tSnsCfg.arrPipeAttr[i].bMirrorFlipEngine) {
            continue;
        }

        AX_U8 nPipeID = m_tSnsCfg.arrPipeAttr[i].nPipeID;
        AX_BOOL bValue = AX_FALSE;

        // mirror
        nRet = AX_VIN_GetPipeMirror(nPipeID, &bValue);
        if (nRet == 0 && bMirror != bValue) {
            LOG_M_W(SENSOR, "SNS[%d] change pipe[%d] mirror=%d.", m_tSnsCfg.nSnsID, nPipeID, bMirror);
            nRet = AX_VIN_SetPipeMirror(nPipeID, bMirror);
        } else if (nRet != 0) {
            LOG_M_E(SENSOR, "AX_VIN_GetPipeMirror failed, ret=0x%x.", nRet);
            LOG_M_W(SENSOR, "SNS[%d] change pipe[%d] mirror=%d.", m_tSnsCfg.nSnsID, nPipeID, bMirror);
            nRet = AX_VIN_SetPipeMirror(nPipeID, bMirror);
        }
        if (nRet != 0) {
            bSucces = AX_FALSE;
        } else {
            m_tSnsCfg.arrPipeAttr[i].bMirror= bMirror;
        }

        // flip
        for (AX_U8 j = AX_VIN_CHN_ID_MAIN; j < AX_VIN_CHN_ID_MAX; j++) {
            if (m_tSnsCfg.arrPipeAttr[i].arrChannelAttr[j].bChnEnable) {
                nRet = AX_VIN_GetChnFlip(nPipeID, (AX_VIN_CHN_ID_E)j, &bValue);
                if (nRet == 0 && bFlip != bValue) {
                    LOG_M_W(SENSOR, "SNS[%d] change pipe[%d] chn[%d] flip=%d.", m_tSnsCfg.nSnsID, nPipeID, j, bFlip);
                    nRet = AX_VIN_SetChnFlip(nPipeID, (AX_VIN_CHN_ID_E)j, bFlip);
                } else if (nRet != 0) {
                    LOG_M_E(SENSOR, "AX_VIN_GetChnFlip failed, ret=0x%x.", nRet);
                    LOG_M_W(SENSOR, "SNS[%d] change pipe[%d] chn[%d] flip=%d.", m_tSnsCfg.nSnsID, nPipeID, j, bFlip);
                    nRet = AX_VIN_SetChnFlip(nPipeID, (AX_VIN_CHN_ID_E)j, bFlip);
                }
                if (nRet != 0) {
                    bSucces = AX_FALSE;
                } else {
                    m_tSnsCfg.arrPipeAttr[i].bFlip = bFlip;
                }
            }
        }
    }

    return bSucces;
}

AX_BOOL CBaseSensor::GetIspIQAttr(APP_ISP_IMAGE_ATTR_T &tAttr) {
    std::lock_guard<std::mutex> _ApiLck(m_mtx);
    AX_S32 ret = 0;
    // restore sharpness
    AX_ISP_IQ_SHARPEN_PARAM_T IspShpParam;
    SENSOR_CONFIG_T tSnsCfg = GetSnsConfig();
    for (AX_U32 i = 0; i < tSnsCfg.nPipeCount; i++) {
        if (!m_bSensorStarted) {
            continue;
        }

        AX_U8 nPipId = tSnsCfg.arrPipeAttr[i].nPipeID;
        ret = AX_ISP_IQ_GetShpParam(nPipId, &IspShpParam);
        tAttr.nSharpness = CalcIspIQToValue(IspShpParam.tAutoParam.nShpGain[0][0][0], 4, 4);
        // restore Ycproc
        AX_ISP_IQ_YCPROC_PARAM_T IspYcprocParam;

        ret = AX_ISP_IQ_GetYcprocParam(nPipId, &IspYcprocParam);

        if (0 != ret) {
            LOG_MM_E(SENSOR, "AX_ISP_IQ_GetYcprocParam faled, ret=0x%x.", ret);
            return AX_FALSE;
        }

        tAttr.nBrightness = CalcIspIQToValue(IspYcprocParam.nBrightness, 4, 8);
        tAttr.nContrast = CalcIspIQToValue(IspYcprocParam.nContrast, 4, 8);
        tAttr.nSaturation = CalcIspIQToValue(IspYcprocParam.nSaturation, 4, 12);
        tAttr.nAutoMode = IspShpParam.nAutoMode;
        tAttr.nYCprocEn = IspYcprocParam.nYCprocEn;
        LOG_MM_I(SENSOR, "autoMode:%d, tAttr.nSharpness:%d, tAttr.nBrightness:%d, tAttr.nContrast:%d,tAttr.nSaturation:%d",
                 IspShpParam.nAutoMode, tAttr.nSharpness, tAttr.nBrightness, tAttr.nContrast, tAttr.nSaturation);
        memcpy(&m_tImageAttr, &tAttr, sizeof(APP_ISP_IMAGE_ATTR_T));
    }

    return AX_TRUE;
}

AX_BOOL CBaseSensor::SetIspIQAttr(const APP_ISP_IMAGE_ATTR_T &tAttr) {
    std::lock_guard<std::mutex> _ApiLck(m_mtx);
    LOG_MM_C(SENSOR, "+++");
    AX_S32 ret = 0;
    AX_S32 nShpGain = CalcValueToIspIQ(tAttr.nSharpness, 4, 4);
    AX_S32 nBrightness = CalcValueToIspIQ(tAttr.nBrightness, 4, 8);
    AX_S32 nContrast = CalcValueToIspIQ(tAttr.nContrast, 4, 8);
    AX_S32 nSaturation = CalcValueToIspIQ(tAttr.nSaturation, 4, 12);
    // set sharpness
    AX_ISP_IQ_SHARPEN_PARAM_T IspShpParam;
    SENSOR_CONFIG_T tSnsCfg = GetSnsConfig();
    for (AX_U32 i = 0; i < tSnsCfg.nPipeCount; i++) {
        if (!m_bSensorStarted) {
            continue;
        }

        AX_U8 nPipId = tSnsCfg.arrPipeAttr[i].nPipeID;
        ret = AX_ISP_IQ_GetShpParam(nPipId, &IspShpParam);
        IspShpParam.nAutoMode = tAttr.nAutoMode;
        for (int i = 0; i < AX_ISP_SHP_HPF_NUM; i++) {
            IspShpParam.tManualParam.nShpGain[i][0] = nShpGain;
            IspShpParam.tManualParam.nShpGain[i][1] = nShpGain;
        }
        ret = AX_ISP_IQ_SetShpParam(nPipId, &IspShpParam);

        if (0 != ret) {
            LOG_MM_E(SENSOR, "AX_ISP_IQ_SetShpParam faled, ret=0x%x.", ret);
            return AX_FALSE;
        }

        // set Ycproc
        AX_ISP_IQ_YCPROC_PARAM_T IspYcprocParam;

        ret = AX_ISP_IQ_GetYcprocParam(nPipId, &IspYcprocParam);

        if (0 != ret) {
            LOG_MM_E(SENSOR, "AX_ISP_IQ_GetYcprocParam faled, ret=0x%x.", ret);
            return AX_FALSE;
        }
        LOG_MM_I(SENSOR, "nSharpness:%d, nBrightness:%d, nContrast:%d,nSaturation:%d", tAttr.nSharpness, tAttr.nBrightness, tAttr.nContrast,
                 tAttr.nSaturation);
        IspYcprocParam.nBrightness = nBrightness;
        IspYcprocParam.nContrast = nContrast;
        IspYcprocParam.nSaturation = nSaturation;
        IspYcprocParam.nYCprocEn = AX_TRUE;

        ret = AX_ISP_IQ_SetYcprocParam(nPipId, &IspYcprocParam);

        if (0 != ret) {
            LOG_MM_E(SENSOR, "AX_ISP_IQ_SetYcprocParam faled, ret=0x%x.", ret);

            return AX_FALSE;
        }
    }
    LOG_MM_C(SENSOR, "---");

    return AX_TRUE;
}

AX_BOOL CBaseSensor::RestoreIspIQAttr(APP_ISP_IMAGE_ATTR_T &tAttr) {
    std::lock_guard<std::mutex> _ApiLck(m_mtx);
    LOG_MM_C(SENSOR, "+++");

    AX_S32 ret = 0;
    // restore sharpness
    AX_ISP_IQ_SHARPEN_PARAM_T IspShpParam;
    SENSOR_CONFIG_T tSnsCfg = GetSnsConfig();
    for (AX_U32 i = 0; i < tSnsCfg.nPipeCount; i++) {
        if (!m_bSensorStarted) {
            continue;
        }

        AX_U8 nPipId = tSnsCfg.arrPipeAttr[i].nPipeID;
        ret = AX_ISP_IQ_GetShpParam(nPipId, &IspShpParam);

        IspShpParam.nAutoMode = tAttr.nAutoMode;
        AX_S32 nShpGain = CalcValueToIspIQ(m_tImageAttr.nSharpness, 4, 4);
        AX_U8 *pArry = &IspShpParam.tAutoParam.nShpGain[0][0][0];
        for (AX_U32 index = 0; index < AX_ISP_AUTO_TABLE_MAX_NUM * AX_ISP_SHP_HPF_NUM * AX_ISP_SHP_GAIN_SIZE; index++) {
            *pArry = nShpGain;
        }

        ret = AX_ISP_IQ_SetShpParam(nPipId, &IspShpParam);

        if (0 != ret) {
            LOG_MM_E(SENSOR, "AX_ISP_IQ_SetShpParam faled, ret=0x%x.", ret);
            return AX_FALSE;
        }

        // restore Ycproc
        AX_ISP_IQ_YCPROC_PARAM_T IspYcprocParam;
        ret = AX_ISP_IQ_GetYcprocParam(nPipId, &IspYcprocParam);
        if (0 != ret) {
            LOG_MM_E(SENSOR, "AX_ISP_IQ_GetYcprocParam faled, ret=0x%x.", ret);
            return AX_FALSE;
        }
        IspYcprocParam.nBrightness = CalcValueToIspIQ(m_tImageAttr.nBrightness, 4, 8);
        IspYcprocParam.nContrast = CalcValueToIspIQ(m_tImageAttr.nContrast, 4, 8);
        IspYcprocParam.nSaturation = CalcValueToIspIQ(m_tImageAttr.nSaturation, 4, 12);
        IspYcprocParam.nYCprocEn = m_tImageAttr.nYCprocEn;
        m_tImageAttr.nAutoMode = tAttr.nAutoMode;

        tAttr = m_tImageAttr;

        ret = AX_ISP_IQ_SetYcprocParam(nPipId, &IspYcprocParam);
        LOG_MM_I(SENSOR, "nPipId:%d, nSharpness:%d, nBrightness:%d, nContrast:%d, nSaturation:%d", nPipId, m_tImageAttr.nSharpness,
                 m_tImageAttr.nBrightness, m_tImageAttr.nContrast, m_tImageAttr.nSaturation);
    }
    LOG_MM_C(SENSOR, "---");

    return AX_TRUE;
}

AX_S32 CBaseSensor::CalcValueToIspIQ(AX_S32 nVal, AX_U8 nIntBits, AX_U8 nFracBits) {
    AX_F32 fVal = (AX_F32)((AX_F32)nVal * ((1 << nIntBits) - 1) / 100);
    AX_U32 nVal_Int = (AX_U32)fVal;
    AX_U32 nVal_Frac = (AX_U32)(round(((fVal - nVal_Int) * 100.0f) * ((1 << nFracBits) - 1) / 100));
    AX_U32 nDataMask = (AX_U32)(((AX_U64)1 << (nIntBits + nFracBits)) - 1);

    nVal = ((AX_U32)((nVal_Int << nFracBits) & nDataMask)) | ((AX_U32)(nVal_Frac & (nDataMask >> nIntBits)));

    return nVal;
}

AX_F32 CBaseSensor::CalcIspIQToValue(AX_S32 nVal, AX_U8 nIntBits, AX_U8 nFracBits) {
    AX_U32 nDataMask = (AX_U32)(((AX_U64)1 << (nIntBits + nFracBits)) - 1);
    AX_U32 nVal_Int = (AX_U32)(nVal >> nFracBits);
    AX_U32 nVal_Frac = (AX_U32)(nVal & (nDataMask >> nIntBits));
    AX_F32 fVal = ((nVal_Frac * 100.0f / ((1 << nFracBits) - 1) / 100) + nVal_Int) * 100 / ((1 << nIntBits) - 1);

    return fVal;
}

AX_BOOL CBaseSensor::UpdateSnsAttr() {
    for (AX_U32 i = 0; i < m_tSnsCfg.nPipeCount; i++) {
        if (!m_bSensorStarted) {
            continue;
        }

        AX_U8 nPipId = m_tSnsCfg.arrPipeAttr[i].nPipeID;
        AX_S32 nRet = AX_ISP_SetSnsAttr(nPipId, &m_tSnsAttr);
        LOG_MM_I(SENSOR,"pipe:[%d] AX_ISP_SetSnsAttr, sensor mode=%d, fps=%f.", nPipId, m_tSnsAttr.eSnsMode, m_tSnsAttr.fFrameRate);
        if (0 != nRet) {
            LOG_MM_E(SENSOR, "pipe:[%d] AX_ISP_SetSnsAttr failed, ret=0x%x.", nPipId, nRet);
            return AX_FALSE;
        } else {
            LOG_MM_I(SENSOR, "pipe:[%d] AX_ISP_SetSnsAttr OK.", nPipId);
        }
    }
    return AX_TRUE;
}

AX_BOOL CBaseSensor::TriggerFlash(AX_S8 nLaneId, AX_S8 nFlashId, AX_U8 nTriggerCount, AX_U8 nFrameInterval) {
    if (-1 != nFlashId) {
        AX_S32 nRet = -1;
        /* flush flash pipe */
        nRet = AX_ISP_FflushSensor(m_tSnsCfg.arrPipeAttr[m_tSnsCfg.nPipeCount-1].nPipeID);
        if (nRet != AX_SUCCESS) {
            LOG_MM_E(SENSOR, "AX_ISP_FflushSensor failed, nRet = 0x%x", nRet);
            return AX_FALSE;
        }
        AX_VIN_FLASH_TRIGGER_DATA_T tFlashData = {0};

        tFlashData.nTriggerNum = nTriggerCount;
        tFlashData.nIntervalFrmNum = nFrameInterval;
        tFlashData.nUserData = nFlashId;
        nRet = AX_VIN_TriggerFlash(m_tSnsCfg.nDevID, nFlashId, &tFlashData);
        if (nRet != AX_SUCCESS) {
            LOG_MM_E(SENSOR, "AX_VIN_TriggerFlash fail nRet = %d\n", nRet);
            return AX_FALSE;
        }
        return AX_TRUE;
    } else {
        return AX_FALSE;
    }
}

AX_S32 CBaseSensor::COMMON_HW_GetApdPlateId(AX_CHAR *apd_plate_id) {
    FILE *pFile = NULL;
    AX_CHAR temp[BOARD_ID_LEN] = {0};

    pFile = fopen("/proc/ax_proc/apd_plate_id", "r");
    if (pFile) {
        fread(temp, BOARD_ID_LEN, 1, pFile);
        fclose(pFile);
    } else {
        LOG_MM_E(SENSOR, "fopen /proc/ax_proc/apd_plate_id failed!!!\n");
        return -1;
    }

    strncpy(apd_plate_id, temp, BOARD_ID_LEN * sizeof(AX_CHAR));
    return 0;
}

AX_S32 CBaseSensor::COMMON_HW_GetSensorResetGpioNum(AX_U8 nDevId) {
    AX_CHAR apd_plate_id[BOARD_ID_LEN] = {0};
    COMMON_HW_GetApdPlateId(apd_plate_id);

    if (!strncmp(apd_plate_id, "ADP_RX_DPHY_2X4LANE", sizeof("ADP_RX_DPHY_2X4LANE") - 1) ||
        !strncmp(apd_plate_id, "ADP_RX_DPHY_2X4LANE_N", sizeof("ADP_RX_DPHY_2X4LANE_N") - 1)) {
        if (!strncmp(apd_plate_id, "ADP_RX_DPHY_2X4LANE_N", sizeof("ADP_RX_DPHY_2X4LANE_N") - 1)) {
            switch (nDevId) {
                case 0:
                    return 64;
                case 2:
                    return 65;
                default:
                    return 64;
            }
        } else {
            switch (nDevId) {
                case 0:
                    return 64;
                case 4:
                    return 65;
                default:
                    return 64;
            }
        }
    } else if (!strncmp(apd_plate_id, "ADP_RX_DPHY_8X2LANE", sizeof("ADP_RX_DPHY_8X2LANE") - 1) ||
               !strncmp(apd_plate_id, "ADP_RX_CPHY_8X2TRIO", sizeof("ADP_RX_CPHY_8X2TRIO") - 1)) {
        switch (nDevId) {
            case 0:
                return 64;
            case 1:
                return 65;
            case 2:
                return 22;
            case 3:
                return 135;
            case 4:
                return 130;
            case 5:
                return 131;
            case 6:
                return 132;
            case 7:
                return 133;
            default:
                return 64;
        }
    } else if (!strncmp(apd_plate_id, "ADP_RX_DPHY_4X4LANE", sizeof("ADP_RX_DPHY_4X4LANE") - 1) ||
               !strncmp(apd_plate_id, "ADP_RX_CPHY_4X3TRIO", sizeof("ADP_RX_CPHY_4X3TRIO") - 1)) {
        switch (nDevId) {
            case 0:
                return 64;
            case 2:
                return 65;
            case 4:
                return 22;
            case 6:
                return 135;
            default:
                return 64;
        }
    } else if (!strncmp(apd_plate_id, "ADP_RX_DPHY_2X8_LVDS_1X16LANE", sizeof("ADP_RX_DPHY_2X8_LVDS_1X16LANE") - 1)) {
        switch (nDevId) {
            default:
                return 64;
        }
    } else if (!strncmp(apd_plate_id, "ADP_RX_DVP_1X10BIT_DPHY_1X2LANE_BT601_656_1120",
                        sizeof("ADP_RX_DVP_1X10BIT_DPHY_1X2LANE_BT601_656_1120") - 1)) {
        switch (nDevId) {
            default:
                return 64;
        }
    } else if (!strncmp(apd_plate_id, "ADP_RX_BT1120_2X10BIT", sizeof("ADP_RX_BT1120_2X10BIT") - 1)) {
        switch (nDevId) {
            default:
                return 64;
        }
    } else if (!strncmp(apd_plate_id, "ADP_RX_BT1120_2X8BIT_DPHY_2X2LANE", sizeof("ADP_RX_BT1120_2X8BIT_DPHY_2X2LANE") - 1)) {
        switch (nDevId) {
            case 3:
                return 135;
            case 7:
                return 133;
            default:
                return 64;
        }
    } else if (!strncmp(apd_plate_id, "ADP_RX_BT601_656_2X10BIT_DPHY_2X2LANE", sizeof("ADP_RX_BT601_656_2X10BIT_DPHY_2X2LANE") - 1)) {
        switch (nDevId) {
            case 2:
                return 22;
            case 6:
                return 132;
            default:
                return 64;
        }
    } else if (!strncmp(apd_plate_id, "ADP_RX_BT601_656_2X8BIT_DPHY_4X2LANE", sizeof("ADP_RX_BT601_656_2X8BIT_DPHY_4X2LANE") - 1)) {
        switch (nDevId) {
            case 2:
                return 22;
            case 3:
                return 135;
            case 6:
                return 132;
            case 7:
                return 133;
            default:
                return 64;
        }
    } else if (!strncmp(apd_plate_id, "ADP_RX_DVP_16bit_HDMI_TO_DPHY_2X4LANE", sizeof("ADP_RX_DVP_16bit_HDMI_TO_DPHY_2X4LANE") - 1)) {
        switch (nDevId) {
            default:
                return 64;
        }
    }

    return 64;
}

AX_U32 CBaseSensor::EnableMultiCamSync(AX_BOOL bEnable)
{
    std::lock_guard<std::mutex> _ApiLck(m_mtx);
    AX_S32 nRet = 0;

    for (auto &m : m_mapPipe2Attr) {
        AX_U8 nPipeID = m.first;

        AX_ISP_IQ_AWB_PARAM_T stIspAwbParam = {0};
        nRet = AX_ISP_IQ_GetAwbParam(nPipeID, &stIspAwbParam);
        if (0 != nRet) {
            LOG_M_E(SENSOR, "[%d]AX_ISP_IQ_GetAwbParam failed, ret=0x%x.", nPipeID, nRet);
            return nRet;
        }
        stIspAwbParam.nEnable = 1;
        stIspAwbParam.tAutoParam.nMultiCamSyncMode = bEnable ? 1 : 0; /* 0：INDEPEND MODE； 1： MASTER SLAVE MODE; 2: OVERLAP MODE */
        nRet = AX_ISP_IQ_SetAwbParam(nPipeID, &stIspAwbParam);
        if (0 != nRet) {
            LOG_M_E(SENSOR, "[%d]AX_ISP_IQ_SetAwbParam failed, ret=0x%x.", nPipeID, nRet);
            return nRet;
        }

        AX_ISP_IQ_AE_PARAM_T stIspAeParam = {0};
        nRet = AX_ISP_IQ_GetAeParam(nPipeID, &stIspAeParam);
        if (0 != nRet) {
            LOG_M_E(SENSOR, "[%d]AX_ISP_IQ_GetAeParam failed, ret=0x%x.", nPipeID, nRet);
            return nRet;
        }
        stIspAeParam.nEnable = 1;
        stIspAeParam.tAeAlgAuto.nMultiCamSyncMode = bEnable ? 3 : 0; /* 0：INDEPEND MODE； 1： MASTER SLAVE MODE; 2: OVERLAP MODE; 3: SPLICE MODE */
        nRet = AX_ISP_IQ_SetAeParam(nPipeID, &stIspAeParam);
        if (0 != nRet) {
            LOG_M_E(SENSOR, "[%d]AX_ISP_IQ_SetAeParam failed, ret=0x%x.", nPipeID, nRet);
            return nRet;
        }
    }

    return 0;
}

AX_BOOL CBaseSensor::EnableEIS(AX_BOOL bEnable) {
    std::lock_guard<std::mutex> _ApiLck(m_mtx);
    AX_BOOL bRet = AX_FALSE;
    do {
        if (!m_bSensorStarted) {
            break;
        }

        if (m_tSnsCfg.nPipeCount < 1) {
            break;
        }

        int nPipeIndex = 0;  /* Sensor's Pipe0 configuration for EIS, IPCDemo has only one pipe, and ITSDemo configures only the video path Pipe. */
        AX_U8 nPipeID = m_tSnsCfg.arrPipeAttr[nPipeIndex].nPipeID;
        AX_ISP_IQ_DIS_PARAM_T tDISParam;
        AX_S32 nRet = AX_ISP_IQ_GetDisParam(nPipeID, &tDISParam);
        if (AX_SUCCESS != nRet) {
            LOG_MM_E(SENSOR, "Pipe[%d] AX_ISP_IQ_GetDisParam fail, nRet:0x%x", nPipeID, nRet);
            break;
        }

        LOG_MM_I(SENSOR, "Pipe[%d] EIS enable:[%d] -> [%d], delay number: %d",
                          nPipeID, tDISParam.bDisEnable, bEnable, tDISParam.tDisV1Param.nDelayFrameNum);

        tDISParam.bDisEnable = bEnable;

        nRet = AX_ISP_IQ_SetDisParam(nPipeID, &tDISParam);
        if (AX_SUCCESS != nRet) {
            LOG_MM_E(SENSOR, "Pipe[%d] AX_ISP_IQ_SetDisParam fail, nRet:0x%x", nPipeID, nRet);
            break;
        }

        bRet = AX_TRUE;

    } while(0);

    return bRet;
}

AX_BOOL CBaseSensor::EZoom(AX_S32 nEZoomRatio, AX_S32 nCenterOffsetX, AX_S32 nCenterOffsetY) {
    std::lock_guard<std::mutex> _ApiLck(m_mtx);
    LOG_MM_I(SENSOR, "EZoom +++, EZoomRatio=%d", nEZoomRatio);
    if (nEZoomRatio < 0 || nEZoomRatio > AX_EZOOM_MAX) {
        return AX_FALSE;
    }

    AX_S32 nRet = 0;
    for (AX_U32 i = 0; i < m_tSnsCfg.nPipeCount; i++) {
        AX_U8 nPipId = m_tSnsCfg.arrPipeAttr[i].nPipeID;

        AX_VIN_CROP_INFO_T tCropInfo;
        AX_VIN_GetChnCrop(nPipId, AX_VIN_CHN_ID_MAIN, &tCropInfo);
        if (nEZoomRatio == 0) {
            if (tCropInfo.bEnable) {
                tCropInfo.bEnable = AX_FALSE;
                nRet = AX_VIN_SetChnCrop(nPipId, AX_VIN_CHN_ID_MAIN, &tCropInfo);
                if (AX_SUCCESS != nRet) {
                    LOG_MM_E(SENSOR, "AX_VIN_SetChnCrop failed ret=0x%x", nRet);
                    return AX_FALSE;
                }
            }
            m_tSnsCfg.tEZoomAttr.nEZoomRatio = nEZoomRatio;
        } else {
            AX_U32 nWidth = m_tSnsAttr.nWidth;
            AX_U32 nHeight = m_tSnsAttr.nHeight;
            AX_U32 nStepW = nWidth / AX_EZOOM_MAX;
            AX_U32 nStepH = nHeight / AX_EZOOM_MAX;

            tCropInfo.bEnable = AX_TRUE;
            tCropInfo.eCoordMode = AX_VIN_COORD_ABS;
            tCropInfo.tCropRect.nWidth = nStepW * (AX_EZOOM_MAX - nEZoomRatio + 1);
            tCropInfo.tCropRect.nHeight = nStepH * (AX_EZOOM_MAX - nEZoomRatio + 1);
            tCropInfo.tCropRect.nStartX = (nWidth - tCropInfo.tCropRect.nWidth) / 2;
            tCropInfo.tCropRect.nStartY = (nHeight - tCropInfo.tCropRect.nHeight) / 2;

            LOG_MM_I(SENSOR, "crop info enable=%d, coord_mode=%d, rect=[%d, %d, %d, %d]", tCropInfo.bEnable, tCropInfo.eCoordMode,
              tCropInfo.tCropRect.nStartX, tCropInfo.tCropRect.nStartY, tCropInfo.tCropRect.nWidth, tCropInfo.tCropRect.nHeight);

            nRet = AX_VIN_SetChnCrop(nPipId, AX_VIN_CHN_ID_MAIN, &tCropInfo);
            if (AX_SUCCESS != nRet) {
                LOG_MM_E(SENSOR, "AX_VIN_SetChnCrop failed ret=0x%x", nRet);
                return AX_FALSE;
            }
            m_tSnsCfg.tEZoomAttr.nEZoomRatio = nEZoomRatio;
            m_tSnsCfg.tEZoomAttr.nCenterOffsetX = nCenterOffsetX;
            m_tSnsCfg.tEZoomAttr.nCenterOffsetY = nCenterOffsetY;
        }

    }

    LOG_MM_I(SENSOR, "EZoom ---, nZoom=%d", nEZoomRatio);
    return AX_TRUE;
}

AX_BOOL CBaseSensor::UpdateLDC(AX_BOOL bLdcEnable, AX_BOOL bAspect, AX_S16 nXRatio, AX_S16 nYRatio, AX_S16 nXYRatio, AX_S16 nDistorRatio) {
    std::lock_guard<std::mutex> _ApiLck(m_mtx);
    LOG_MM_I(SENSOR, "UpdateLDC+++");
    for (AX_U32 i = 0; i < m_tSnsCfg.nPipeCount; i++) {
        AX_U8 nPipId = m_tSnsCfg.arrPipeAttr[i].nPipeID;
        AX_ISP_IQ_LDC_PARAM_T tLdcAttr;
        AX_ISP_IQ_GetLdcParam(nPipId, &tLdcAttr);
        tLdcAttr = {.nLdcEnable = bLdcEnable,
                    .nType = AX_ISP_IQ_LDC_TYPE_V1,
                    .tLdcV1Param =
                        {
                            .bAspect = bAspect,
                            .nXRatio = nXRatio,
                            .nYRatio = nYRatio,
                            .nXYRatio = nXYRatio,
                            .nCenterXOffset = 0,
                            .nCenterYOffset = 0,
                            .nDistortionRatio = nDistorRatio,
                            .nSpreadCoef = 0,
                        },
                    .tLdcV2Param = {.nMatrix =
                                        {
                                            {0, 0, 0, /*0 - 2*/},
                                            {0, 0, 0, /*0 - 2*/},
                                            {0, 0, 100, /*0 - 2*/},
                                        },
                                    .nDistortionCoeff = {0, 0, 0, 0, 0, 0, 0, 0, /*0 - 7*/}}};

        if (m_bSensorStarted) {
            AX_S32 nRet = AX_ISP_IQ_SetLdcParam(nPipId, &tLdcAttr);
            if (AX_SUCCESS != nRet) {
                LOG_MM_E(SENSOR, "AX_ISP_IQ_SetLdcParam failed ret=0x%x", nRet);
                return AX_FALSE;
            }
        }

        m_tSnsCfg.arrPipeAttr[i].tLdcAttr.bLdcEnable = bLdcEnable;
        m_tSnsCfg.arrPipeAttr[i].tLdcAttr.bLdcAspect = bAspect;
        m_tSnsCfg.arrPipeAttr[i].tLdcAttr.nLdcXRatio = nXRatio;
        m_tSnsCfg.arrPipeAttr[i].tLdcAttr.nLdcYRatio = nYRatio;
        m_tSnsCfg.arrPipeAttr[i].tLdcAttr.nLdcXYRatio = nXYRatio;
        m_tSnsCfg.arrPipeAttr[i].tLdcAttr.nLdcDistortionRatio = nDistorRatio;
    }

    LOG_MM_I(SENSOR, "UpdateLDC---");
    return AX_TRUE;
}

AX_BOOL CBaseSensor::SetChnCompress(AX_U8 nPipe, AX_U8 nChannel, AX_BOOL bCompressOn) {
    LOG_MM_I(SENSOR, "Pipe[%d]Chn[%d] bCompressOn: %d", nPipe, nChannel, bCompressOn);
    AX_VIN_CHN_ATTR_T stChnAttr{0};
    AX_S32 nRet = AX_VIN_GetChnAttr(nPipe, (AX_VIN_CHN_ID_E)nChannel, &stChnAttr);
    if (AX_SUCCESS != nRet) {
        LOG_MM_E(SENSOR, "AX_VIN_GetChnAttr failed ret=0x%x", nRet);
        return AX_FALSE;
    }

    if (bCompressOn) {
        stChnAttr.tCompressInfo.enCompressMode = AX_COMPRESS_MODE_LOSSY;
        stChnAttr.tCompressInfo.u32CompressLevel = 4;
    } else {
        stChnAttr.tCompressInfo.enCompressMode = AX_COMPRESS_MODE_NONE;
        stChnAttr.tCompressInfo.u32CompressLevel = 0;
    }

    nRet = AX_VIN_SetChnAttr(nPipe, (AX_VIN_CHN_ID_E)nChannel, &stChnAttr);
    if (AX_SUCCESS != nRet) {
        LOG_MM_E(SENSOR, "AX_VIN_SetChnAttr failed ret=0x%x", nRet);
        return AX_FALSE;
    }

    return AX_TRUE;
}

AX_BOOL CBaseSensor::IsSnsRawTypeSet() {
    AX_RAW_TYPE_E eRawType = (AX_RAW_TYPE_E)m_tSnsCfg.arrSnsRawType[m_tSnsCfg.eSensorMode];
    AX_BOOL bRet = AX_FALSE;

    switch (eRawType) {
        case AX_RT_RAW8:
        case AX_RT_RAW10:
        case AX_RT_RAW12:
        case AX_RT_RAW14:
        case AX_RT_RAW16:
            bRet = AX_TRUE;
            break;
        default:
            break;
    }

    LOG_MM_I(SENSOR, "eSensorMode: %d, eRawType: %d", m_tSnsCfg.eSensorMode, eRawType);

    return bRet;
}

AX_IMG_FORMAT_E CBaseSensor::GetDevPixelFmt() {
    AX_IMG_FORMAT_E ePixelFmt = AX_FORMAT_BAYER_RAW_10BPP_PACKED;

    switch (m_tSnsAttr.eRawType) {
        case AX_RT_RAW8:
            ePixelFmt = AX_FORMAT_BAYER_RAW_8BPP;
            break;
        case AX_RT_RAW10:
            ePixelFmt = AX_FORMAT_BAYER_RAW_10BPP_PACKED;
            break;
        case AX_RT_RAW12:
            ePixelFmt = AX_FORMAT_BAYER_RAW_12BPP_PACKED;
            break;
        case AX_RT_RAW14:
            ePixelFmt = AX_FORMAT_BAYER_RAW_14BPP_PACKED;
            break;
        case AX_RT_RAW16:
            ePixelFmt = AX_FORMAT_BAYER_RAW_16BPP;
            break;
        default:
            LOG_MM_E(SENSOR, "Invalid sns raw type: %d", m_tSnsAttr.eRawType);
            break;
    }

    LOG_MM_I(SENSOR, "eRawType: %d, ePixelFmt: 0x%x", m_tSnsAttr.eRawType, ePixelFmt);

    return ePixelFmt;
}

AX_BOOL CBaseSensor::IsIFERawDumpEnable() {
    for (AX_U8 i = 0; i < m_tSnsCfg.nPipeCount; i++) {
        if (m_tSnsCfg.arrPipeAttr[i].tIFEDumpInfo.nIFEDumpRawEnable >= 0) {
            return AX_TRUE;
        }
    }

    return AX_FALSE;
}

AX_BOOL CBaseSensor::IsNpuOfflinePipe(AX_U8 nPipe) {
    for (AX_U8 i = 0; i < m_tSnsCfg.nPipeCount; i++) {
        if (nPipe == m_tSnsCfg.arrPipeAttr[i].nPipeID
            && AX_VIN_PIPE_NORMAL_MODE2 == m_tSnsCfg.arrPipeAttr[i].ePipeWorkMode
            && m_tSnsCfg.arrPipeAttr[i].bSnapshot) {
            return AX_TRUE;
        }
    }

    return AX_FALSE;
}

AX_U8 CBaseSensor::GetIFERawDumpPipe() {
    for (AX_U8 i = 0; i < m_tSnsCfg.nPipeCount; i++) {
        if (m_tSnsCfg.arrPipeAttr[i].tIFEDumpInfo.nIFEDumpRawEnable > 0) {
            return m_tSnsCfg.arrPipeAttr[i].nPipeID;
        }
    }

    return INVALID_PIPE_ID;
}

AX_BOOL CBaseSensor::SwitchIFERawDumpPipe(AX_U8 nPipe) {
    AX_U8 nPipeAttrIndex = 0;

    for (AX_U8 i = 0; i < m_tSnsCfg.nPipeCount; i++) {
        if (nPipe == m_tSnsCfg.arrPipeAttr[i].nPipeID) {
            nPipeAttrIndex = i;
        }
    }

    if (nPipeAttrIndex >= m_tSnsCfg.nPipeCount) {
        LOG_M_E(SENSOR, "Pipe(%d) is invalid!", nPipe);
        return AX_FALSE;
    }

    for (AX_U8 i = 0; i < m_tSnsCfg.nPipeCount; i++) {
        if (IsNpuOfflinePipe(m_tSnsCfg.arrPipeAttr[i].nPipeID)) { // NPU offline mode pipe, keep enabled, do nothing here
            continue;
        }

        if (m_tSnsCfg.arrPipeAttr[i].tIFEDumpInfo.nIFEDumpRawEnable != -1) {
            AX_VIN_DUMP_ATTR_T stDumpAttr;
            AX_S32 nRet = AX_VIN_GetPipeDumpAttr(m_tSnsCfg.arrPipeAttr[i].nPipeID, AX_VIN_PIPE_DUMP_NODE_IFE, AX_VIN_DUMP_QUEUE_TYPE_EXT, &stDumpAttr);
            if (0 != nRet) {
                LOG_M_E(SENSOR, "Sns[%d]Pipe[%d]AX_VIN_GetPipeDumpAttr failed, ret=0x%x.",
                                    m_tSnsCfg.nSnsID, m_tSnsCfg.arrPipeAttr[i].nPipeID, nRet);
                return AX_FALSE;
            }

            if (nPipe == m_tSnsCfg.arrPipeAttr[i].nPipeID) {
                stDumpAttr.bEnable = AX_TRUE;
            } else if (stDumpAttr.bEnable) {
                stDumpAttr.bEnable = AX_FALSE;
            } else {
                continue;
            }

            nRet = AX_VIN_SetPipeDumpAttr(m_tSnsCfg.arrPipeAttr[i].nPipeID, AX_VIN_PIPE_DUMP_NODE_IFE, AX_VIN_DUMP_QUEUE_TYPE_EXT, &stDumpAttr);
            if (0 != nRet) {
                LOG_M_E(SENSOR, "Sns[%d]Pipe[%d]AX_VIN_SetPipeDumpAttr failed with param-(enable:%d, depth:%d, dumpType:%d), ret=0x%x.",
                                    m_tSnsCfg.nSnsID, m_tSnsCfg.arrPipeAttr[i].nPipeID,
                                    stDumpAttr.bEnable, stDumpAttr.nDepth, AX_VIN_DUMP_QUEUE_TYPE_EXT, nRet);
                return AX_FALSE;
            }

            LOG_MM_I(SENSOR, "Sns[%d]pipe[%d] AX_VIN_SetPipeDumpAttr with param-(enable:%d, depth:%d, dumpType:%d)",
                                m_tSnsCfg.nSnsID, m_tSnsCfg.arrPipeAttr[i].nPipeID,
                                stDumpAttr.bEnable, stDumpAttr.nDepth, AX_VIN_DUMP_QUEUE_TYPE_EXT);
        }
    }

    return AX_TRUE;
}

AX_BOOL CBaseSensor::SetIFEDumpAttr() {
    AX_S32 nRet = -1;
    for (AX_U8 i = 0; i < m_tSnsCfg.nPipeCount; i++) {
        if (m_tSnsCfg.arrPipeAttr[i].tIFEDumpInfo.nIFEDumpRawEnable != -1) {
            AX_VIN_DUMP_ATTR_T stDumpAttr;
            stDumpAttr.bEnable = (m_tSnsCfg.arrPipeAttr[i].tIFEDumpInfo.nIFEDumpRawEnable == 0) ? AX_FALSE : AX_TRUE;
            stDumpAttr.nDepth = m_tSnsCfg.arrPipeAttr[i].tIFEDumpInfo.nIFEDumpRawDepth;
            nRet = AX_VIN_SetPipeDumpAttr(m_tSnsCfg.arrPipeAttr[i].nPipeID, AX_VIN_PIPE_DUMP_NODE_IFE, AX_VIN_DUMP_QUEUE_TYPE_EXT, &stDumpAttr);
            if (0 != nRet) {
                LOG_M_E(SENSOR, "Sns[%d]Pipe[%d]AX_VIN_SetPipeDumpAttr for IFE node failed with param-(enable:%d, depth:%d, dumpType:%d), ret=0x%x.",
                                    m_tSnsCfg.nSnsID, m_tSnsCfg.arrPipeAttr[i].nPipeID,
                                    stDumpAttr.bEnable, stDumpAttr.nDepth, AX_VIN_DUMP_QUEUE_TYPE_EXT, nRet);
                return AX_FALSE;
            }
            LOG_MM_I(SENSOR, "Sns[%d]pipe[%d] AX_VIN_SetPipeDumpAttr for IFE node with param-(enable:%d, depth:%d, dumpType:%d)",
                                m_tSnsCfg.nSnsID, m_tSnsCfg.arrPipeAttr[i].nPipeID,
                                stDumpAttr.bEnable, stDumpAttr.nDepth, AX_VIN_DUMP_QUEUE_TYPE_EXT);
        }

        if (m_tSnsCfg.arrPipeAttr[i].tRLTMDumpInfo.nRLTMDumpRawEnable != -1) {
            AX_VIN_DUMP_ATTR_T stDumpAttr;
            stDumpAttr.bEnable = (m_tSnsCfg.arrPipeAttr[i].tRLTMDumpInfo.nRLTMDumpRawEnable == 0) ? AX_FALSE : AX_TRUE;
            stDumpAttr.nDepth = m_tSnsCfg.arrPipeAttr[i].tRLTMDumpInfo.nRLTMDumpRawDepth;
            nRet = AX_VIN_SetPipeDumpAttr(m_tSnsCfg.arrPipeAttr[i].nPipeID, AX_VIN_PIPE_DUMP_NODE_RLTM, AX_VIN_DUMP_QUEUE_TYPE_EXT, &stDumpAttr);
            if (0 != nRet) {
                LOG_M_E(SENSOR, "Sns[%d]Pipe[%d]AX_VIN_SetPipeDumpAttr for RLTM node failed with param-(enable:%d, depth:%d, dumpType:%d), ret=0x%x.",
                                    m_tSnsCfg.nSnsID, m_tSnsCfg.arrPipeAttr[i].nPipeID,
                                    stDumpAttr.bEnable, stDumpAttr.nDepth, AX_VIN_DUMP_QUEUE_TYPE_EXT, nRet);
                return AX_FALSE;
            }
            LOG_MM_I(SENSOR, "Sns[%d]pipe[%d] AX_VIN_SetPipeDumpAttr for RLTM node with param-(enable:%d, depth:%d, dumpType:%d)",
                                m_tSnsCfg.nSnsID, m_tSnsCfg.arrPipeAttr[i].nPipeID,
                                stDumpAttr.bEnable, stDumpAttr.nDepth, AX_VIN_DUMP_QUEUE_TYPE_EXT);
        }
    }

    return AX_TRUE;
}

AX_BOOL CBaseSensor::SetShutterAttr(AX_U8 nDevId, std::array<AX_VIN_SHUTTER_MODE_E, AX_SHUTTER_SEQ_NUM>& arr) {
    std::lock_guard<std::mutex> _ApiLck(m_mtx);
    if (AX_TRUE == m_tSnsCfg.bEnableFlash) {
        AX_VIN_LIGHT_SYNC_INFO_T tLightSyncAttr;
        AX_S32 s32Ret = -1;
        s32Ret = AX_VIN_GetLightSyncInfo(nDevId, &tLightSyncAttr);
        if (AX_SUCCESS != s32Ret) {
            LOG_MM_E(SENSOR, "AX_VIN_GetLightSyncInfo failed , ret: 0x%x", s32Ret);
            return AX_FALSE;
        }

        for (AX_U8 i = 0; i < AX_SHUTTER_SEQ_NUM; i++) {
            tLightSyncAttr.szShutterMode[i] = arr[i];
            LOG_MM_I(SENSOR, "tLightSyncAttr.szShutterMode[%d]:%d", i, tLightSyncAttr.szShutterMode[i]);
        }

        s32Ret = AX_VIN_SetLightSyncInfo(nDevId, &tLightSyncAttr);
        if (AX_SUCCESS != s32Ret) {
            LOG_MM_E(SENSOR, "AX_VIN_SetLightSyncInfo failed , ret: 0x%x", s32Ret);
            return AX_FALSE;
        }
        return AX_TRUE;
    }
    return AX_FALSE;
}

AX_BOOL CBaseSensor::EnableChn(AX_U8 nPipe, AX_U8 nChannel, AX_BOOL bEnable) {
    AX_S32 nRet = -1;

    if (bEnable) {
        nRet = AX_VIN_EnableChn(nPipe, (AX_VIN_CHN_ID_E)nChannel);
        if (0 != nRet) {
            LOG_MM_E(SENSOR, "AX_VIN_EnableChn(pipe:%d, chn: %d) fail, ret=0x%x",
                           nPipe, nChannel, nRet);
            return AX_FALSE;
        }
    } else {
        nRet = AX_VIN_DisableChn(nPipe, (AX_VIN_CHN_ID_E)nChannel);
        if (0 != nRet) {
            LOG_MM_E(SENSOR, "AX_VIN_DisableChn(pipe:%d, chn: %d) fail, ret=0x%x",
                           nPipe, nChannel, nRet);
            return AX_FALSE;
        }
    }

    LOG_MM_C(SENSOR, "Sns[%d]pipe[%d] %s chn: %d.",
                      m_tSnsCfg.nSnsID, nPipe, bEnable ? "enable" : "disable", nChannel);

    return AX_TRUE;
}

AX_U8 CBaseSensor::GetSnapPipe() {
    for (AX_U8 i = 0; i < m_tSnsCfg.nPipeCount; i++) {
        if (m_tSnsCfg.arrPipeAttr[i].bSnapshot) {
            return m_tSnsCfg.arrPipeAttr[i].nPipeID;
        }
    }

    return INVALID_PIPE_ID;
}

AX_BOOL CBaseSensor::IsRotateEngine() {
    for (AX_U8 i = 0; i < m_tSnsCfg.nPipeCount; i++) {
        if (m_tSnsCfg.arrPipeAttr[i].bRotationEngine) {
            return AX_TRUE;
        }
    }

    return AX_FALSE;
}

AX_BOOL CBaseSensor::IsMirrorFlipEngine() {
    for (AX_U8 i = 0; i < m_tSnsCfg.nPipeCount; i++) {
        if (m_tSnsCfg.arrPipeAttr[i].bMirrorFlipEngine) {
            return AX_TRUE;
        }
    }

    return AX_FALSE;
}

AX_BOOL CBaseSensor::UpdateRotation(AX_VIN_ROTATION_E eRotation) {
    LOG_MM_I(SENSOR, "UpdateRotation+++");
    AX_S32 nRet = 0;
    for (AX_U32 i = 0; i < m_tSnsCfg.nPipeCount; i++) {
        AX_U8 nPipId = m_tSnsCfg.arrPipeAttr[i].nPipeID;

        AX_VIN_ROTATION_E eRotionOld = AX_VIN_ROTATION_0;

        nRet = AX_VIN_GetChnRotation(nPipId, AX_VIN_CHN_ID_MAIN, &eRotionOld);
        if (AX_SUCCESS != nRet) {
            LOG_MM_E(SENSOR, "AX_VIN_GetChnRotation failed ret=0x%x", nRet);
        }

        if (eRotation != eRotionOld) {
            nRet = AX_VIN_SetChnRotation(nPipId, AX_VIN_CHN_ID_MAIN, eRotation);
            if (AX_SUCCESS != nRet) {
                LOG_MM_E(SENSOR, "AX_VIN_SetChnRotation failed ret=0x%x", nRet);
                return AX_FALSE;
            }

            m_tSnsCfg.arrPipeAttr[i].eRotation = eRotation;
        } else {
            LOG_MM_I(SENSOR, "Ratation %d not changed", eRotation);
        }
    }

    if (m_tSnsCfg.tEZoomAttr.nEZoomRatio) {
        EZoom(m_tSnsCfg.tEZoomAttr.nEZoomRatio, m_tSnsCfg.tEZoomAttr.nCenterOffsetX, m_tSnsCfg.tEZoomAttr.nCenterOffsetY);
    }

    LOG_MM_I(SENSOR, "UpdateRotation---");

    return AX_TRUE;
}

AX_VIN_WARP_ATTR_T CBaseSensor::GetVinWarpAttr(AX_U8 nEngineMode) {
    AX_VIN_WARP_ATTR_T stWarpAttr{AX_WARP_ENGINE_GDC, AX_GDC_MODE_CORE0};

    switch (nEngineMode) {
        case 0:
            stWarpAttr.eWarpEngine = AX_WARP_ENGINE_GDC;
            stWarpAttr.uWarpMode.eGdcMode = AX_GDC_MODE_CORE0;
            break;
        case 1:
            stWarpAttr.eWarpEngine = AX_WARP_ENGINE_VDSP;
            stWarpAttr.uWarpMode.eVdspMode = AX_VDSP_MODE_CORE0;
            break;
        case 2:
            stWarpAttr.eWarpEngine = AX_WARP_ENGINE_VDSP;
            stWarpAttr.uWarpMode.eVdspMode = AX_VDSP_MODE_CORE1;
            break;
        case 3:
            stWarpAttr.eWarpEngine = AX_WARP_ENGINE_VDSP;
            stWarpAttr.uWarpMode.eVdspMode = AX_VDSP_MODE_AUTO;
            break;
        default:
            stWarpAttr.eWarpEngine = AX_WARP_ENGINE_GDC;
            stWarpAttr.uWarpMode.eGdcMode = AX_GDC_MODE_CORE0;
            LOG_MM_W(SENSOR, "Invalid engineMode(%d), use GDC by default.", nEngineMode);
            break;
    }

    LOG_MM_I(SENSOR, "Warp engine(%s) mode(%d) for nEngineMode(%d).",
                      (stWarpAttr.eWarpEngine == AX_WARP_ENGINE_GDC) ? "GDC" : "VDSP",
                      (stWarpAttr.eWarpEngine == AX_WARP_ENGINE_GDC) ?
                        (AX_U8)stWarpAttr.uWarpMode.eGdcMode : (AX_U8)stWarpAttr.uWarpMode.eVdspMode,
                      nEngineMode);

    return stWarpAttr;
}

AX_VIN_ROTATION_E CBaseSensor::GetRotation(AX_BOOL bIgnorePipe /*= AX_TRUE*/, AX_U8 nPipe /*= 0*/) {
    for (AX_U8 i = 0; i < m_tSnsCfg.nPipeCount; i++) {
        if (m_tSnsCfg.arrPipeAttr[i].eRotation != AX_VIN_ROTATION_0) {
            if (bIgnorePipe || m_tSnsCfg.arrPipeAttr[i].nPipeID == nPipe) {
                return m_tSnsCfg.arrPipeAttr[i].eRotation;
            }
        }
    }

    return AX_VIN_ROTATION_0;
}

AX_BOOL CBaseSensor::IsMirror(AX_BOOL bIgnorePipe /*= AX_TRUE*/, AX_U8 nPipe /*= 0*/) {
    for (AX_U8 i = 0; i < m_tSnsCfg.nPipeCount; i++) {
        if (m_tSnsCfg.arrPipeAttr[i].bMirror) {
            if (bIgnorePipe || m_tSnsCfg.arrPipeAttr[i].nPipeID == nPipe) {
                return AX_TRUE;
            }
        }
    }

    return AX_FALSE;
}

AX_BOOL CBaseSensor::IsFlip(AX_BOOL bIgnorePipe /*= AX_TRUE*/, AX_U8 nPipe /*= 0*/) {
    for (AX_U8 i = 0; i < m_tSnsCfg.nPipeCount; i++) {
        if (m_tSnsCfg.arrPipeAttr[i].bFlip) {
            if (bIgnorePipe || m_tSnsCfg.arrPipeAttr[i].nPipeID == nPipe) {
                return AX_TRUE;
            }
        }
    }

    return AX_FALSE;
}

AX_BOOL CBaseSensor::OpenDev() {
    LOG_MM_I(SENSOR, "Sns[%d] [Dev:%d, RxDev:%d] +++", m_tSnsCfg.nSnsID, m_tSnsCfg.nDevID, m_tSnsCfg.nRxDev);

    LOG_MM_I(SENSOR, "Sensor Attr => w:%d h:%d framerate:%.2f sensor mode:%d rawType:%d",
                    m_tSnsAttr.nWidth, m_tSnsAttr.nHeight, m_tSnsAttr.fFrameRate, m_tSnsAttr.eSnsMode, m_tSnsAttr.eRawType);

    AX_S32 nRet = 0;
    AX_U8 nDevID = m_tSnsCfg.nDevID;
    AX_U8 nRxDev = m_tSnsCfg.nRxDev;

    // SNS reset sensor obj
    for (auto &m : m_mapPipe2Attr) {
        AX_U8 nPipeID = m.first;
        if (!ResetSensorObj(nDevID, nPipeID)) {
            LOG_M_E(SENSOR, "ResetSensorObj failed, ret=0x%x.", nRet);
            return AX_FALSE;
        }
    }

    // MIPI proc
    nRet = AX_MIPI_RX_SetLaneCombo(AX_LANE_COMBO_MODE_4);
    if (0 != nRet) {
        LOG_M_E(SENSOR, "AX_MIPI_RX_SetLaneCombo failed, ret=0x%x.", nRet);
        return AX_FALSE;
    }

    nRet = AX_MIPI_RX_SetAttr(nRxDev, &m_tMipiRxDev);
    if (0 != nRet) {
        LOG_M_E(SENSOR, "AX_MIPI_RX_SetAttr failed, ret=0x%x.", nRet);
        return AX_FALSE;
    }

    nRet = AX_MIPI_RX_Reset(nRxDev);
    if (0 != nRet) {
        LOG_M_E(SENSOR, "AX_MIPI_RX_Reset failed, ret=0x%x.", nRet);
        return AX_FALSE;
    }

    nRet = AX_MIPI_RX_Start(nRxDev);
    if (0 != nRet) {
        LOG_M_E(SENSOR, "AX_MIPI_RX_Start failed, ret=0x%x.", nRet);
        return AX_FALSE;
    }

    // DEV proc
    nRet = AX_VIN_CreateDev(nDevID, &m_tDevAttr);
    if (0 != nRet) {
        LOG_M_E(SENSOR, "AX_VIN_CreateDev failed, ret=0x%x.", nRet);
        return AX_FALSE;
    }

    nRet = AX_VIN_SetDevPrivateDataAttr(nDevID, &m_tPrivAttr);
    if (0 != nRet) {
        LOG_M_E(SENSOR, "AX_VIN_SetDevPrivateDataAttr failed, nRet=0x%x.\n", nRet);
        return AX_FALSE;
    }

    nRet = AX_VIN_SetDevAttr(nDevID, &m_tDevAttr);
    if (0 != nRet) {
        LOG_M_E(SENSOR, "AX_VIN_SetDevAttr failed, ret=0x%x.", nRet);
        return AX_FALSE;
    }

    AX_VIN_DEV_BIND_PIPE_T tDevBindPipe = {0};
    for (AX_U8 i = 0; i < GetPipeCount(); i++) {
        AX_U8 nPipeID = m_tSnsCfg.arrPipeAttr[i].nPipeID;

        tDevBindPipe.nPipeId[i] = nPipeID;
        tDevBindPipe.nNum += 1;

        switch (m_tSnsAttr.eSnsMode) {
            case AX_SNS_LINEAR_MODE:
                tDevBindPipe.nHDRSel[i] = 0x1;
                break;
            case AX_SNS_HDR_2X_MODE:
                tDevBindPipe.nHDRSel[i] = 0x1 | 0x2;
                break;
            case AX_SNS_HDR_3X_MODE:
                tDevBindPipe.nHDRSel[i] = 0x1 | 0x2 | 0x4;
                break;
            case AX_SNS_HDR_4X_MODE:
                tDevBindPipe.nHDRSel[i] = 0x1 | 0x2 | 0x4 | 0x8;
                break;
            default:
                tDevBindPipe.nHDRSel[i] = 0x1;
                break;
        }
    }

    nRet = AX_VIN_SetDevBindPipe(nDevID, &tDevBindPipe);
    if (0 != nRet) {
        LOG_M_E(SENSOR, "AX_VIN_SetDevBindPipe failed, ret=0x%x.", nRet);
        return AX_FALSE;
    }

    nRet = AX_VIN_SetDevBindMipi(nDevID, nRxDev);
    if (0 != nRet) {
        LOG_M_E(SENSOR, "AX_VIN_SetDevBindMipi failed, ret=0x%x\n", nRet);
        return AX_FALSE;
    }

    // Snap
    if (AX_TRUE == m_tSnsCfg.bEnableFlash) {
        nRet = AX_VIN_SetSyncPowerAttr(nDevID, &m_tSnapAttr.tPowerAttr);
        if (0 != nRet) {
            LOG_M_E(SENSOR, "AX_VIN_SetSyncPowerAttr failed, nRet=0x%x.\n", nRet);
            return AX_FALSE;
        }

        nRet = AX_VIN_SetLightSyncInfo(nDevID, &m_tSnapAttr.tLightSyncAttr);
        if (0 != nRet) {
            LOG_M_E(SENSOR, "AX_VIN_SetLightSyncInfo failed, nRet=0x%x.\n", nRet);
            return AX_FALSE;
        }

        nRet = AX_VIN_SetVSyncAttr(nDevID, &m_tSnapAttr.tVsyncAttr);
        if (0 != nRet) {
            LOG_M_E(SENSOR, "AX_VIN_SetVSyncAttr failed, nRet=0x%x.\n", nRet);
            return AX_FALSE;
        }

        nRet = AX_VIN_EnableVSync(nDevID);
        if (0 != nRet) {
            LOG_M_E(SENSOR, "AX_VIN_EnableVSync failed, nRet=0x%x.\n", nRet);
            return AX_FALSE;
        }

        nRet = AX_VIN_SetHSyncAttr(nDevID, &m_tSnapAttr.tHsyncAttr);
        if (0 != nRet) {
            LOG_M_E(SENSOR, "AX_VIN_SetHSyncAttr failed, nRet=0x%x.\n", nRet);
            return AX_FALSE;
        }

        nRet = AX_VIN_EnableHSync(nDevID);
        if (0 != nRet) {
            LOG_M_E(SENSOR, "AX_VIN_EnableHSync failed, nRet=0x%x.\n", nRet);
            return AX_FALSE;
        }

        nRet = AX_VIN_SetStrobeTimingAttr(nDevID, LIGHT_STROBE, &m_tSnapAttr.tStrobeAttr);
        if (0 != nRet) {
            LOG_M_E(SENSOR, "AX_VIN_SetStrobeTimingAttr failed, nRet=0x%x.\n", nRet);
            return AX_FALSE;
        }

        nRet = AX_VIN_EnableStrobe(nDevID, LIGHT_STROBE);
        if (0 != nRet) {
            LOG_M_E(SENSOR, "AX_VIN_EnableStrobe failed, nRet=0x%x.\n", nRet);
            return AX_FALSE;
        }

        nRet = AX_VIN_SetFlashTimingAttr(nDevID, LIGHT_FLASH, &m_tSnapAttr.tFlashAttr);
        if (0 != nRet) {
            LOG_M_E(SENSOR, "AX_VIN_SetFlashTimingAttr failed, nRet=0x%x.\n", nRet);
            return AX_FALSE;
        }
    }

    // Pipe proc
    for (auto &m : m_mapPipe2Attr) {
        AX_U8 nPipeID = m.first;
        AX_VIN_PIPE_ATTR_T &tPipeAttr = m.second;

        nRet = AX_VIN_CreatePipe(nPipeID, &tPipeAttr);
        if (0 != nRet) {
            LOG_M_E(SENSOR, "AX_VIN_CreatePipe failed, ret=0x%x.", nRet);
            return AX_FALSE;
        }

        nRet = AX_VIN_SetPipeAttr(nPipeID, &tPipeAttr);
        if (0 != nRet) {
            LOG_M_E(SENSOR, "AX_VIN_SetPipeAttr failed, ret=0x%x.", nRet);
            return AX_FALSE;
        }

        for(AX_U8 i = 0; i < m_tSnsCfg.nPipeCount; i++) {
            if (nPipeID == m_tSnsCfg.arrPipeAttr[i].nPipeID) {
                AX_VIN_SCHED_ATTR_T tSchedAttr;
                tSchedAttr.ePolicy = m_tSnsCfg.arrPipeAttr[i].tPipeSchedAttr.ePolicy;
                tSchedAttr.nPriority = m_tSnsCfg.arrPipeAttr[i].tPipeSchedAttr.nPriority;
                nRet = AX_VIN_SetPipeSchedAttr(nPipeID, &tSchedAttr);
                if (0 != nRet) {
                    LOG_M_E(SENSOR, "AX_VIN_SetPipeSchedAttr failed, ret=0x%x.", nRet);
                    return AX_FALSE;
                }
            }
        }
    }

    AX_S32 nPPLIndex;
    if (!CCmdLineParser::GetInstance()->GetPPLIndex(nPPLIndex)) {
        LOG_M_E(SENSOR, "GetPPLIndex failed.\n");
        return AX_FALSE;
    }

    for (auto &m : m_mapPipe2Attr) {
        AX_U8 nPipeID = m.first;

        if ((1 == nPPLIndex) || (2 == nPPLIndex)) {
            /* for IPC and Pano ppl*/
            nRet = AX_VIN_SetPipeFrameSource(nPipeID, AX_VIN_FRAME_SOURCE_ID_IFE, AX_VIN_FRAME_SOURCE_TYPE_DEV);
            if (0 != nRet) {
                LOG_M_E(SENSOR, "AX_VIN_SetPipeFrameSource failed, ret=0x%x.", nRet);
                return AX_FALSE;
            }
        } else if (0 == nPPLIndex) {
            /* for ITS ppl*/
            if ((AX_SNS_DCG_HDR == m_tSnsAttr.eSnsOutputMode) || (AX_SNS_DCG_VS_HDR == m_tSnsAttr.eSnsOutputMode)) {
                nRet = AX_VIN_SetPipeFrameSource(nPipeID, AX_VIN_FRAME_SOURCE_ID_IFE, AX_VIN_FRAME_SOURCE_TYPE_DEV);
                if (0 != nRet) {
                    LOG_M_E(SENSOR, "AX_VIN_SetPipeFrameSource failed, ret=0x%x.", nRet);
                    return AX_FALSE;
                }
            } else {
                if (AX_VIN_PIPE_NORMAL_MODE0 == m.second.ePipeWorkMode) {
                    nRet = AX_VIN_SetPipeFrameSource(nPipeID, AX_VIN_FRAME_SOURCE_ID_IFE, AX_VIN_FRAME_SOURCE_TYPE_USER);
                    if (0 != nRet) {
                        LOG_M_E(SENSOR, "AX_VIN_SetPipeFrameSource failed, ret=0x%x.", nRet);
                        return AX_FALSE;
                    }
                } else if (AX_VIN_PIPE_NORMAL_MODE1 == m.second.ePipeWorkMode) {
                    nRet = AX_VIN_SetPipeFrameSource(nPipeID, AX_VIN_FRAME_SOURCE_ID_ITP, AX_VIN_FRAME_SOURCE_TYPE_USER);
                    if (0 != nRet) {
                        LOG_M_E(SENSOR, "AX_VIN_SetPipeFrameSource failed, ret=0x%x.", nRet);
                        return AX_FALSE;
                    }
                } else if (AX_VIN_PIPE_NORMAL_MODE2 == m.second.ePipeWorkMode) {
                    nRet = AX_VIN_SetPipeFrameSource(nPipeID, AX_VIN_FRAME_SOURCE_ID_IFE, AX_VIN_FRAME_SOURCE_TYPE_USER);
                    if (0 != nRet) {
                        LOG_M_E(SENSOR, "AX_VIN_SetPipeFrameSource failed, ret=0x%x.", nRet);
                        return AX_FALSE;
                    }

                    nRet = AX_VIN_SetPipeFrameSource(nPipeID, AX_VIN_FRAME_SOURCE_ID_ITP, AX_VIN_FRAME_SOURCE_TYPE_USER);
                    if (0 != nRet) {
                        LOG_M_E(SENSOR, "AX_VIN_SetPipeFrameSource failed, ret=0x%x.", nRet);
                        return AX_FALSE;
                    }

                    nRet = AX_VIN_SetPipeFrameSource(nPipeID, AX_VIN_FRAME_SOURCE_ID_AINR, AX_VIN_FRAME_SOURCE_TYPE_USER);
                    if (0 != nRet) {
                        LOG_M_E(SENSOR, "AX_VIN_SetPipeFrameSource failed, ret=0x%x.", nRet);
                        return AX_FALSE;
                    }
                }
            }
        } else {
            LOG_M_E(SENSOR, "Unsupported ppl index.");
            return AX_FALSE;
        }
    }

    if (!SetIFEDumpAttr()) {
        return AX_FALSE;
    }

    // SNS register
    for (auto &m : m_mapPipe2Attr) {
        AX_U8 nPipeID = m.first;
        if (!RegisterSensor(nPipeID, m_tSnsCfg.nDevNode, m_tSnsCfg.nI2cAddr)) {
            LOG_M_E(SENSOR, "RegisterSensor failed, ret=0x%x.", nRet);
            return AX_FALSE;
        }
    }

    // SNS Proc
    for (auto &m : m_mapPipe2Attr) {
        if (m_tSnsCfg.eSensorMode == AX_SNS_LINEAR_MODE
            || m_tSnsCfg.eSensorMode == AX_SNS_HDR_2X_MODE) {
            m_tSnsAttr.nSettingIndex = m_tSnsCfg.arrSettingIndex[m_tSnsCfg.eSensorMode];
        }

        AX_U8 nPipeID = m.first;
        nRet = AX_ISP_SetSnsAttr(nPipeID, &m_tSnsAttr);
        if (0 != nRet) {
            LOG_M_E(SENSOR, "AX_ISP_SetSnsAttr failed, ret=0x%x.", nRet);
            return AX_FALSE;
        }

        nRet = AX_ISP_OpenSnsClk(m_tSnsClkAttr.nSnsClkIdx, m_tSnsClkAttr.eSnsClkRate);
        if (0 != nRet) {
            LOG_M_E(SENSOR, "AX_ISP_OpenSnsClk failed, ret=0x%x.", nRet);
            return AX_FALSE;
        }
    }

    for (AX_U8 i = 0; i < GetPipeCount(); i++) {
        AX_U8 nPipeID = m_tSnsCfg.arrPipeAttr[i].nPipeID;
        vector<string> vecPipeTuningBin = GetSnsConfig().arrPipeAttr[i].vecTuningBin;

        /* Configure the input queue depth of the Pipe to prevent frame drops and reduce jitter. The JSON configuration is for multi-camera IPC, and the default app configuration is 0. */
        /* In mode0 (ITS), configure at the AX_VIN_FRAME_SOURCE_ID_IFE node. */
        /* In mode1 (IPC), configure at the AX_VIN_FRAME_SOURCE_ID_ITP node. */
        if (m_tSnsCfg.arrPipeAttr[i].nSrcDepth != 0) {
            AX_VIN_FRAME_SOURCE_ID_E eSrcId = AX_VIN_FRAME_SOURCE_ID_MIN;
            switch (m_tSnsCfg.arrPipeAttr[i].ePipeWorkMode)
            {
            case AX_VIN_PIPE_NORMAL_MODE0:
                eSrcId = AX_VIN_FRAME_SOURCE_ID_IFE;
                break;
            case AX_VIN_PIPE_NORMAL_MODE1:
                eSrcId = AX_VIN_FRAME_SOURCE_ID_ITP;
                break;
            default:
                break;
            }
            if (eSrcId != AX_VIN_FRAME_SOURCE_ID_MIN) {
                nRet = AX_VIN_SetPipeSourceDepth(nPipeID, eSrcId, m_tSnsCfg.arrPipeAttr[i].nSrcDepth);
                if (0 != nRet) {
                    LOG_M_E(SENSOR, "AX_VIN_SetPipeSourceDepth failed, ret=0x%x.", nRet);
                    return AX_FALSE;
                }
            }
        }

        nRet = AX_ISP_Create(nPipeID);
        if (0 != nRet) {
            LOG_M_E(SENSOR, "AX_ISP_Create failed, ret=0x%x.", nRet);
            return AX_FALSE;
        }

        if (!m_algWrapper.RegisterAlgoToSensor(m_pSnsObj, nPipeID)) {
            LOG_M_E(SENSOR, "RegisterAlgoToSensor failed.");
            return AX_FALSE;
        }

        for (AX_U8 j = 0; j < vecPipeTuningBin.size(); j++) {
            std::string strKey = m_tSnsCfg.eSensorMode == AX_SNS_LINEAR_MODE ? SDRKEY : HDRKEY;
            if (!vecPipeTuningBin[j].empty()) {
                if (0 != access(vecPipeTuningBin[j].data(), F_OK)) {
                    LOG_MM_W(SENSOR, "bin file is not exist.");
                    continue;
                }

                if (std::string::npos == vecPipeTuningBin[j].find(strKey)) {
                    LOG_MM_W(SENSOR, "hdrMode and bin do not match");
                    continue;
                }

                LOG_MM_I(SENSOR, "sns[%d]pipe[%d] load bin: %s", m_tSnsCfg.nSnsID, nPipeID, vecPipeTuningBin[j].c_str());

                nRet = AX_ISP_LoadBinParams(nPipeID, vecPipeTuningBin[j].c_str());
                if (AX_SUCCESS != nRet) {
                    LOG_M_E(SENSOR, "pipe[%d], %d tuning bin , AX_ISP_LoadBinParams ret=0x%x %s. The parameters in sensor.h will be used\n",
                            i, j, nRet, vecPipeTuningBin[j].c_str());
                }
            }
        }

        nRet = AX_ISP_Open(nPipeID);
        if (0 != nRet) {
            LOG_M_E(SENSOR, "AX_ISP_Open failed, ret=0x%x.", nRet);
            return AX_FALSE;
        }
    }

    for (AX_U8 i = 0; i < GetPipeCount(); i++) {
        AX_U8 nPipeID = m_tSnsCfg.arrPipeAttr[i].nPipeID;

        if (m_tSnsCfg.arrPipeAttr[i].bMirror) {
            nRet = AX_VIN_SetPipeMirror(nPipeID, AX_TRUE);
            if (0 != nRet) {
                LOG_M_E(SENSOR, "[%d] AX_VIN_SetPipeMirror failed, ret=0x%x.", nPipeID, nRet);
                return AX_FALSE;
            }
        }

        for (AX_U8 j = AX_VIN_CHN_ID_MAIN; j < AX_VIN_CHN_ID_MAX; j++) {
            AX_VIN_CHN_ATTR_T &tChnAttr = m_mapPipe2ChnAttr[nPipeID][j];

            if (tChnAttr.nWidth > 0 && tChnAttr.nHeight > 0) {
                nRet = AX_VIN_SetChnAttr(nPipeID, (AX_VIN_CHN_ID_E)j, &tChnAttr);
                if (0 != nRet) {
                    LOG_M_E(SENSOR, "(%d)AX_VIN_SetChnAttr(%dX%d) failed, ret=0x%x.",
                                     nPipeID, tChnAttr.nWidth, tChnAttr.nHeight, nRet);
                    return AX_FALSE;
                }
            }

            if (m_tSnsCfg.arrPipeAttr[i].arrChannelAttr[j].bChnEnable) {
                if (m_tSnsCfg.arrPipeAttr[i].bFlip) {
                    nRet = AX_VIN_SetChnFlip(nPipeID, (AX_VIN_CHN_ID_E)j, AX_TRUE);
                    if (0 != nRet) {
                        LOG_M_E(SENSOR, "[%d] AX_VIN_SetChnFlip failed, ret=0x%x.", nPipeID, nRet);
                        return AX_FALSE;
                    }
                }

                if ((0 == j) && (m_tSnsCfg.arrPipeAttr[i].eRotation != AX_VIN_ROTATION_0)) {
                    nRet = AX_VIN_SetChnRotation(nPipeID, AX_VIN_CHN_ID_MAIN, m_tSnsCfg.arrPipeAttr[i].eRotation);
                    if (AX_SUCCESS != nRet) {
                        LOG_MM_E(SENSOR, "[%d] AX_VIN_SetChnRotation(%d) for chn 0 failed, ret=0x%x", nPipeID, nRet);
                        return AX_FALSE;
                    }
                }

                nRet = AX_VIN_EnableChn(nPipeID, (AX_VIN_CHN_ID_E)j);
                if (0 != nRet) {
                    LOG_M_E(SENSOR, "AX_VIN_EnableChn failed, ret=0x%x.", nRet);
                    return AX_FALSE;
                }
            }
        }
    }

    for (AX_U8 i = 0; i < GetPipeCount(); i++) {
        AX_U8 nPipeID = m_tSnsCfg.arrPipeAttr[i].nPipeID;
        nRet = AX_VIN_StartPipe(nPipeID);
        if (0 != nRet) {
            LOG_M_E(SENSOR, "AX_VIN_StartPipe failed, ret=0x%x.", nRet);
            return AX_FALSE;
        }

        if (!m_tSnsCfg.arrPipeAttr[i].bSnapshot) {
            nRet = AX_ISP_Start(nPipeID);
            if (0 != nRet) {
                LOG_M_E(SENSOR, "AX_ISP_Start failed, ret=0x%x.", nRet);
                return AX_FALSE;
            }
        } /* Snapshot pipe will apply AX_ISP_RunOnce manually instead */
    }

    for (AX_U8 i = 1; i < GetPipeCount(); i++) {
        AX_U8 nPipeID = m_tSnsCfg.arrPipeAttr[i].nPipeID;
        SetAeToManual(nPipeID);
    }

    if (AX_VIN_DEV_OFFLINE == m_tDevAttr.eDevMode) {
        AX_VIN_DUMP_ATTR_T tDumpAttr;
        tDumpAttr.bEnable = AX_TRUE;
        tDumpAttr.nDepth = 3;
        nRet = AX_VIN_SetDevDumpAttr(nDevID, AX_VIN_DUMP_QUEUE_TYPE_DEV, &tDumpAttr);
        if (0 != nRet) {
            LOG_M_E(SENSOR, "AX_VIN_SetDevDumpAttr failed, ret=0x%x.", nRet);
            return AX_FALSE;
        }
    }

    nRet = AX_VIN_EnableDev(nDevID);
    if (0 != nRet) {
        LOG_M_E(SENSOR, "AX_VIN_EnableDev failed, ret=0x%x.", nRet);
        return AX_FALSE;
    }

    LOG_MM_I(SENSOR, "Sns[%d] [Dev:%d, RxDev:%d] ---", m_tSnsCfg.nSnsID, m_tSnsCfg.nDevID, m_tSnsCfg.nRxDev);

    return AX_TRUE;
}

AX_BOOL CBaseSensor::IspStreamOn() {
    LOG_MM_I(SENSOR, "Sns[%d] [Dev:%d, RxDev:%d] +++", m_tSnsCfg.nSnsID, m_tSnsCfg.nDevID, m_tSnsCfg.nRxDev);

    AX_S32 nRet = 0;
    for (AX_U8 i = 0; i < GetPipeCount(); i++) {
        AX_U8 nPipeID = m_tSnsCfg.arrPipeAttr[i].nPipeID;
        nRet = AX_ISP_StreamOn(nPipeID);
        if (0 != nRet) {
            LOG_M_E(SENSOR, "AX_ISP_StreamOn failed, ret=0x%x.", nRet);
            return AX_FALSE;
        }
    }

    m_bSensorStarted = AX_TRUE;

    LOG_MM_I(SENSOR, "Sns[%d] [Dev:%d, RxDev:%d] ---", m_tSnsCfg.nSnsID, m_tSnsCfg.nDevID, m_tSnsCfg.nRxDev);

    return AX_TRUE;
}

AX_VOID CBaseSensor::RestoreAttr() {
    // ldc
    if (m_tSnsCfg.arrPipeAttr[0].tLdcAttr.bLdcEnable) {
        UpdateLDC(m_tSnsCfg.arrPipeAttr[0].tLdcAttr.bLdcEnable,
                    m_tSnsCfg.arrPipeAttr[0].tLdcAttr.bLdcAspect,
                    m_tSnsCfg.arrPipeAttr[0].tLdcAttr.nLdcXRatio,
                    m_tSnsCfg.arrPipeAttr[0].tLdcAttr.nLdcYRatio,
                    m_tSnsCfg.arrPipeAttr[0].tLdcAttr.nLdcXYRatio,
                    m_tSnsCfg.arrPipeAttr[0].tLdcAttr.nLdcDistortionRatio);
    }

    if (m_tSnsCfg.arrPipeAttr[0].bEnableEIS) {
        EnableEIS(m_tSnsCfg.arrPipeAttr[0].bEISOn);
    }

    if (m_tSnsCfg.tEZoomAttr.nEZoomRatio != 0) {
        EZoom(m_tSnsCfg.tEZoomAttr.nEZoomRatio, m_tSnsCfg.tEZoomAttr.nCenterOffsetX, m_tSnsCfg.tEZoomAttr.nCenterOffsetY);
    }

    // day night mode
    if (m_tSnsCfg.eDayNight == AX_DAYNIGHT_MODE_NIGHT) {
        ChangeDaynightMode(m_tSnsCfg.eDayNight);
    }
}

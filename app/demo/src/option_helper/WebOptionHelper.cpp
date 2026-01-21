/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include "WebOptionHelper.h"
#include "AlgoOptionHelper.h"
#include "AudioOptionHelper.h"
#include "CmdLineParser.h"
#include "ElapsedTimer.hpp"
#include "GlobalDef.h"
#include "OptionHelper.h"
#include "OsdConfig.h"
#include "PrintHelper.h"
#include "SensorOptionHelper.h"
#include "appweb.h"
#include "http.h"
#include "mpr.h"

#define WEB_OPTION "WEB_OPTION"
#define JSON2INT(val) picojson::value(double(val))
#define JSON2BOOL(val) picojson::value(bool(val))

static inline MprJson* ReadJsonValeInArry(MprJson* object, AX_S32 index) {
    MprJson* result = nullptr;
    if (object->type & MPR_JSON_ARRAY) {
        MprJson *child = nullptr;
        AX_S32 i = 0;
        for (ITERATE_JSON(object, child, i)) {
            if (i == index) {
                result = child;
                break;
            }
        }
    }

    return result;
}

static MprJson* AppenJsonChild(MprJson *obj, MprJson *child)
{
    MprJson      *prior;
    if (!obj || !child) {
        return 0;
    }
    if (obj->children) {
        prior = obj->children->prev;
        child->next = obj->children;
        child->prev = prior;
        prior->next->prev = child;
        prior->next = child;
    } else {
        child->next = child->prev = child;
        obj->children = child;
    }
    child->name = NULL;
    obj->length++;
    return child;
}

static inline MprJson* ReadJsonValeIn2DArry(MprJson* object, AX_S32 i, AX_S32 j) {
    MprJson* target = ReadJsonValeInArry(object, i);
    return ReadJsonValeInArry(target, j);
}

static AX_VOID WriteOSDStringAttrToJson(MprJson* root, const AX_CHAR* key, const OSD_CFG_T& tOsdConfig, AX_S32 nOffsetX, AX_S32 nOffsetY, AX_BOOL bTimeAttr = AX_FALSE) {
    AX_U32 nColor =  bTimeAttr ? tOsdConfig.tTimeAttr.nColor : tOsdConfig.tStrAttr.nColor;
    AX_S32 nFontSize = bTimeAttr ? tOsdConfig.tTimeAttr.nFontSize : tOsdConfig.tStrAttr.nFontSize;
    auto jsonItem = mprCreateJson(MPR_JSON_OBJ);
    if (!bTimeAttr) {
        WRITE_JSON_PSTR(jsonItem, "text", tOsdConfig.tStrAttr.szStr);
    }
    WRITE_JSON_BOOL(jsonItem, "enable", tOsdConfig.bEnable);
    WRITE_JSON_HEX(jsonItem, "color", "0x%06X", nColor & 0xFFFFFF);
    WRITE_JSON_INT(jsonItem, "fontsize", nFontSize);
    WRITE_JSON_INT(jsonItem, "align", tOsdConfig.eAlign);
    auto jsonRect = mprCreateJson(MPR_JSON_ARRAY);
    APPEND_JSON_2_INT(jsonRect, nOffsetX, nOffsetY);
    APPEND_JSON_2_INT(jsonRect, tOsdConfig.nBoundaryW, tOsdConfig.nBoundaryH);
    WRITE_JSON_OBJ(jsonItem, "rect", jsonRect);
    WRITE_JSON_OBJ(root, key, jsonItem);
}

using namespace std;

CWebOptionHelper::CWebOptionHelper(AX_VOID) {
}

CWebOptionHelper::~CWebOptionHelper(AX_VOID) {
    m_mapSnsID2Type.clear();
    m_mapSns2FramerateOpt.clear();
    m_mapSns2CameraSetting.clear();
    m_mapSns2VideoAttr.clear();
    m_mapSns2AiAttr.clear();
    m_mapCapabilities.clear();
    m_mapSnsType2ResOptions.clear();
    m_mapSns2ChnStatInfo.clear();
    m_mapSns2OsdConfig.clear();
    m_mapSns2AudioAttr.clear();
    m_pairVideo2Ivps.clear();
    m_mapSns2PrivacyConfig.clear();
    m_mapVideoMainRes.clear();
}

AX_BOOL CWebOptionHelper::InitOnce() {
    m_mapCapabilities["sys"] = 1;
    m_mapCapabilities["cam"] = 1;
    m_mapCapabilities["img"] = 1;
    m_mapCapabilities["ai"] = 1;
    m_mapCapabilities["audio"] = APP_AUDIO_AVAILABLE() ? 1 : 0;
    m_mapCapabilities["video"] = 1;
    m_mapCapabilities["overlay"] = COptionHelper::GetInstance()->IsEnableOSD() ? 1 : 0;
    m_mapCapabilities["record"] = COptionHelper::GetInstance()->IsEnableVideoRecord() ? 1 : 0;
    m_mapCapabilities["storage"] = 0;
    m_mapCapabilities["playback"] = 1;

    /* os08a20*/
    m_mapSns2FramerateOpt[E_SNS_TYPE_OS08A20] = "[[50, 60], [25, 30]]";

    /* Quad os08a20*/
    m_mapSns2FramerateOpt[E_SNS_TYPE_QUAD_OS08A20] = "[[25], [25]]";

    /* os08b10*/
    m_mapSns2FramerateOpt[E_SNS_TYPE_OS08B10] = "[[25],[25]]";

    /* SC910gs*/
    m_mapSns2FramerateOpt[E_SNS_TYPE_SC910GS] = "[[25],[25]]";

    /* PANO DUAL OS04A10*/
    m_mapSns2FramerateOpt[E_SNS_TYPE_OS04A10x2_PANO] = "[[30], [30]]";

    /* PANO EIGHT OS04A10*/
    m_mapSns2FramerateOpt[E_SNS_TYPE_OS04A10x8_PANO] = "[[25], [25]]";

    /* os04a10*/
    m_mapSns2FramerateOpt[E_SNS_TYPE_OS04A10] = "[[50, 60], [25, 30]]";

    /* Quad os04a10*/
    m_mapSns2FramerateOpt[E_SNS_TYPE_QUAD_OS04A10] = "[[25, 30], [25, 30]]";

    /* SC410gs*/
    m_mapSns2FramerateOpt[E_SNS_TYPE_SC410GS] = "[[30],[30]]";

    /* SC500AI */
    m_mapSns2FramerateOpt[E_SNS_TYPE_SC500AI] = "[[25, 30], [25, 30]]";

    /* Quad IMX678*/
    m_mapSns2FramerateOpt[E_SNS_TYPE_QUAD_IMX678] = "[[25], [25]]";

    return AX_TRUE;
}

AX_VOID CWebOptionHelper::InitCameraAttr(AX_U8 nSnsID, AX_U8 nSnsType, const WEB_CAMERA_ATTR_T& tCameraAttr) {
    m_mapSns2CameraSetting[nSnsID] = tCameraAttr;
    m_mapSnsID2Type[nSnsID] = nSnsType;

    if (m_mapSns2CameraSetting[nSnsID].bTriggerEnable) {
        for(AX_U8 i = 0; i < MAX_LANE_COUNT; i++) {
            m_mapSns2CameraSetting[nSnsID].tFlashAttr[i].nLaneId = i + 1;
            m_mapSns2CameraSetting[nSnsID].tFlashAttr[i].nFlashId = (0 == i) ? 2 : -1;
            m_mapSns2CameraSetting[nSnsID].tFlashAttr[i].nTriggerCount = 1;
            m_mapSns2CameraSetting[nSnsID].tFlashAttr[i].nFrameInterval = 3;
        }
    }
}

AX_VOID CWebOptionHelper::InitVideoAttr(AX_U8 nSnsID, AX_U8 nChnID, const WEB_VIDEO_ATTR_T& tVideoAttr) {
    m_mapSns2VideoAttr[nSnsID][nChnID] = tVideoAttr;
    memcpy(m_mapSns2VideoAttr[nSnsID][nChnID].stEncodeCfg, tVideoAttr.stEncodeCfg, sizeof(APP_ENC_RC_CONFIG) * APP_ENCODER_TYPE_MAX);
    if (0 == nChnID) {
        m_mapVideoMainRes[nSnsID] = make_pair(tVideoAttr.nWidth, tVideoAttr.nHeight);
    }
}

AX_VOID CWebOptionHelper::InitIvesAttr(AX_U8 nSnsID, CIVESStage* pIves) {
    AX_U8 u8ThresHoldY{0};
    AX_U8 u8Confidence{0};
    AX_S32 s32ThresHoldY{0};
    AX_S32 s32Confidence{0};

    m_mapSns2AiAttr[nSnsID].tEvents.tOD.bEnable = CAlgoOptionHelper::GetInstance()->IsEnableOD(nSnsID);
    auto pODInstance = pIves->GetODInstance();
    if (nullptr == pODInstance) {
        m_mapSns2AiAttr[nSnsID].tEvents.tOD.nThrsHoldY = 100;
        m_mapSns2AiAttr[nSnsID].tEvents.tOD.nConfidence = 80;
    } else {
        pODInstance->GetDefaultThresholdY(nSnsID, u8ThresHoldY, u8Confidence);
        m_mapSns2AiAttr[nSnsID].tEvents.tOD.nThrsHoldY = u8ThresHoldY;
        m_mapSns2AiAttr[nSnsID].tEvents.tOD.nConfidence = u8Confidence;
    }

    m_mapSns2AiAttr[nSnsID].tEvents.tMD.bEnable = CAlgoOptionHelper::GetInstance()->IsEnableMD(nSnsID);
    auto pMDInstance = pIves->GetMDInstance();
    if (nullptr == pMDInstance) {
        m_mapSns2AiAttr[nSnsID].tEvents.tMD.nThrsHoldY = 20;
        m_mapSns2AiAttr[nSnsID].tEvents.tMD.nConfidence = 50;
    } else {
        pMDInstance->GetDefaultThresholdY(nSnsID, u8ThresHoldY, u8Confidence);
        m_mapSns2AiAttr[nSnsID].tEvents.tMD.nThrsHoldY = u8ThresHoldY;
        m_mapSns2AiAttr[nSnsID].tEvents.tMD.nConfidence = u8Confidence;
    }

    m_mapSns2AiAttr[nSnsID].tEvents.tSCD.bEnable = CAlgoOptionHelper::GetInstance()->IsEnableSCD(nSnsID);
    auto pSCDInstance = pIves->GetSCDInstance();
    if (nullptr == pSCDInstance) {
        m_mapSns2AiAttr[nSnsID].tEvents.tSCD.nThrsHoldY = 60;
        m_mapSns2AiAttr[nSnsID].tEvents.tSCD.nConfidence = 60;
    } else {
        pSCDInstance->GetThreshold(nSnsID, s32ThresHoldY, s32Confidence);
        m_mapSns2AiAttr[nSnsID].tEvents.tSCD.nThrsHoldY = s32ThresHoldY;
        m_mapSns2AiAttr[nSnsID].tEvents.tSCD.nConfidence = s32Confidence;
    }
}

AX_BOOL CWebOptionHelper::InitAiAttr(AX_U8 nSnsID) {
    m_mapSns2AiAttr[nSnsID].bEnable = AX_TRUE;
    m_mapSns2AiAttr[nSnsID].tPushStategy.ePushMode = (E_AI_DETECT_PUSH_MODE_TYPE)ALGO_HVCFP_PARAM(nSnsID).stPushStrategy.ePushMode;
    m_mapSns2AiAttr[nSnsID].tPushStategy.nPushCounts = ALGO_HVCFP_PARAM(nSnsID).stPushStrategy.nPushCount;
    m_mapSns2AiAttr[nSnsID].tPushStategy.nPushIntervalMs = ALGO_HVCFP_PARAM(nSnsID).stPushStrategy.nInterval;

    return AX_TRUE;
}

AX_BOOL CWebOptionHelper::GetCapSettingStr(AX_CHAR* pOutBuf, AX_U32 nSize) {
    if (nullptr == pOutBuf || 0 == nSize) {
        return AX_FALSE;
    }

    AX_U8 nSnsCount = APP_WEB_SHOW_SENSOR_COUNT();
    AX_U8 nImgMultiSnsMode = ((APP_WEB_SHOW_SENSOR_MODE() == E_WEB_SHOW_SENSOR_MODE_PANO_DUAL) || 1 == nSnsCount) ? 0 : 1;
    m_mapCapabilities["img"] =(APP_WEB_SHOW_SENSOR_MODE() == E_WEB_SHOW_SENSOR_MODE_PANO_SINGLE) ? 0 : 1;

    AX_S32 nCount = snprintf(pOutBuf, nSize,
                             "{sns_count: %d, img_page_support_multi_sns: %d, support_page_sys: %d, support_page_cam: %d, support_page_img: %d, support_page_ai: "
                             "%d, support_page_audio: %d, "
                             "support_page_video: %d, "
                             "support_page_overlay: %d, support_page_record: %d, support_page_storage: %d, support_page_playback: %d}",
                             nSnsCount, nImgMultiSnsMode, m_mapCapabilities["sys"], m_mapCapabilities["cam"], m_mapCapabilities["img"],
                             m_mapCapabilities["ai"], m_mapCapabilities["audio"], m_mapCapabilities["video"], m_mapCapabilities["overlay"],
                             m_mapCapabilities["record"], m_mapCapabilities["storage"], m_mapCapabilities["playback"]);

    return nCount > 0 ? AX_TRUE : AX_FALSE;
}

WEB_CAMERA_ATTR_T& CWebOptionHelper::GetCamera(AX_U8 nSnsID) {
    std::lock_guard<std::mutex> lck(m_mapSns2MtxOption[nSnsID]);
    return m_mapSns2CameraSetting[nSnsID];
}

AX_VOID CWebOptionHelper::SetCamera(AX_U8 nSnsID, const WEB_CAMERA_ATTR_T& tCamera) {
    std::lock_guard<std::mutex> lck(m_mapSns2MtxOption[nSnsID]);
    m_mapSns2CameraSetting[nSnsID] = tCamera;
}

MprJson* CWebOptionHelper::GetCameraJson(AX_U8 nSnsID) {
    WEB_CAMERA_ATTR_T attr = m_mapSns2CameraSetting[nSnsID];
    MprJson* json = mprCreateJson(MPR_JSON_OBJ);

    std::lock_guard<std::mutex> lck(m_mapSns2MtxOption[nSnsID]);
    WRITE_JSON_INT(json, "sns_work_mode", attr.nSnsMode);
    WRITE_JSON_INT(json, "rotation", attr.nRotation);
    WRITE_JSON_INT(json, "framerate", attr.fFramerate);
    WRITE_JSON_INT(json, "daynight", attr.nDayNightMode);
    WRITE_JSON_INT(json, "nr_mode", attr.nNrMode);
    WRITE_JSON_BOOL(json, "mirror", attr.bMirror);
    WRITE_JSON_BOOL(json, "flip", attr.bFlip);
    WRITE_JSON_BOOL(json, "auto_capture", attr.bAutoCapture);
    WRITE_JSON_BOOL(json, "auto_capture_enable", attr.bAutoCaptureEnable);
    WRITE_JSON_INT(json, "switch_work_mode_enable", attr.bSnsModeEnable);
    WRITE_JSON_INT(json, "switch_PN_mode_enable", attr.bPNModeEnable);
    WRITE_JSON_BOOL(json, "switch_mirror_flip_enable", attr.bMirrorFlipEnable);
    WRITE_JSON_BOOL(json, "switch_rotation_enable", attr.bRotationEnable);
    WRITE_JSON_BOOL(json, "eis", attr.bEISOn);
    WRITE_JSON_BOOL(json, "eis_support", attr.bEISEnable);
    WRITE_JSON_BOOL(json, "disable_camera_select", attr.bDisableCamSelect);
    WRITE_JSON_INT(json, "shutter_type", attr.nShutterType);
    WRITE_JSON_BOOL(json, "shutter_option_enable", attr.bShutterOptEnable);
    WRITE_JSON_BOOL(json, "vsubchn_on", attr.bVSubChnOn);
    WRITE_JSON_BOOL(json, "switch_vsubchn_enable", attr.bSwitchVSubChnEnable);
    WRITE_JSON_INT(json, "dump_raw_pipe", attr.nDumpRawPipe);
    WRITE_JSON_BOOL(json, "switch_dump_raw_pipe_enable", attr.bSwitchDumpRawPipeEnable);

    return json;
}

AX_BOOL CWebOptionHelper::GetImageStr(AX_U8 nSnsID, AX_CHAR* pOutBuf, AX_U32 nSize) {
    if (nullptr == pOutBuf || 0 == nSize) {
        return AX_FALSE;
    }
    WEB_CAMERA_ATTR_T attr = m_mapSns2CameraSetting[nSnsID];
    std::lock_guard<std::mutex> lck(m_mapSns2MtxOption[nSnsID]);
    picojson::object json;
    json["isp_auto_mode"] = JSON2INT(attr.tImageAttr.nAutoMode);
    json["sharpness"] = JSON2INT(attr.tImageAttr.nSharpness);
    json["brightness"] = JSON2INT(attr.tImageAttr.nBrightness);
    json["contrast"] = JSON2INT(attr.tImageAttr.nContrast);
    json["saturation"] = JSON2INT(attr.tImageAttr.nSaturation);
    std::string strJson = picojson::value(json).serialize().data();
    sprintf(pOutBuf, strJson.data(), strJson.size());
    return strJson.size() ? AX_TRUE : AX_FALSE;
}

AX_BOOL CWebOptionHelper::GetLdcStr(AX_U8 nSnsID, AX_CHAR* pOutBuf, AX_U32 nSize) {
    if (nullptr == pOutBuf || 0 == nSize) {
        return AX_FALSE;
    }
    WEB_CAMERA_ATTR_T attr = m_mapSns2CameraSetting[nSnsID];
    std::lock_guard<std::mutex> lck(m_mapSns2MtxOption[nSnsID]);
    picojson::object json;
    json["ldc_support"] = JSON2BOOL(attr.bLdcEnable);
    json["ldc_enable"] = JSON2BOOL(attr.tLdcAttr.bLdcEnable);
    json["aspect"] = JSON2BOOL(attr.tLdcAttr.bLdcAspect);
    json["x_ratio"] = JSON2INT(attr.tLdcAttr.nXRatio);
    json["y_ratio"] = JSON2INT(attr.tLdcAttr.nYRatio);
    json["xy_ratio"] = JSON2INT(attr.tLdcAttr.nXYRatio);
    json["distor_ratio"] = JSON2INT(attr.tLdcAttr.nDistorRatio);
    std::string strJson = picojson::value(json).serialize().data();
    sprintf(pOutBuf, strJson.data(), strJson.size());
    return strJson.size() ? AX_TRUE : AX_FALSE;
}

std::vector<WEB_VIDEO_ATTR_T> CWebOptionHelper::GetVideoUniChnVec(AX_U8 nSnsID) {
    std::vector<WEB_VIDEO_ATTR_T> vecUni;
    for (auto item : m_mapSns2VideoAttr[nSnsID]) {
        vecUni.emplace_back(item.second);
    }
    return vecUni;
}

AX_U8 CWebOptionHelper::GetVideoCount(AX_U8 nSnsID) {
    std::map<AX_U8, WEB_VIDEO_ATTR_T>& mapSnsVideoAttr = m_mapSns2VideoAttr[nSnsID];
    return mapSnsVideoAttr.size();
}

WEB_VIDEO_ATTR_T& CWebOptionHelper::GetVideo(AX_U8 nSnsID, AX_U32 nChnID) {
    std::lock_guard<std::mutex> lck(m_mapSns2MtxOption[nSnsID]);
    return m_mapSns2VideoAttr[nSnsID][nChnID];
}

AX_VOID CWebOptionHelper::SetVideo(AX_U8 nSnsID, AX_U32 nChnID, const WEB_VIDEO_ATTR_T& tVideo) {
    std::lock_guard<std::mutex> lck(m_mapSns2MtxOption[nSnsID]);
    m_mapSns2VideoAttr[nSnsID][nChnID] = tVideo;
}

AX_BOOL CWebOptionHelper::GetVideoByUniChn(AX_U8 nSnsID, AX_U32 nUniChn, WEB_VIDEO_ATTR_T& tVideoAttr) {
    std::lock_guard<std::mutex> lck(m_mapSns2MtxOption[nSnsID]);
    for (const auto& item : m_mapSns2VideoAttr[nSnsID]) {
        if (item.second.nUniChn == nUniChn) {
            tVideoAttr = item.second;
            return AX_TRUE;
        }
    }

    LOG_M_W(WEB_OPTION, "[%d,%d] GetVideoByUniChn failed, no found!", nSnsID, nUniChn);
    return AX_FALSE;
}

AX_VOID CWebOptionHelper::SetVideoByUniChn(AX_U8 nSnsID, const WEB_VIDEO_ATTR_T& tVideoAttr) {
    std::lock_guard<std::mutex> lck(m_mapSns2MtxOption[nSnsID]);
    for (auto& item : m_mapSns2VideoAttr[nSnsID]) {
        if (item.second.nUniChn == tVideoAttr.nUniChn) {
            item.second = tVideoAttr;
            return;
        }
    }

    LOG_M_W(WEB_OPTION, "[%d,%d] SetVideoByUniChn failed, no found!", nSnsID, tVideoAttr.nUniChn);
}

WEB_RECORD_ATTR_T& CWebOptionHelper::GetRecordAttr(AX_U8 nSnsID, AX_U32 nChnID) {
    std::lock_guard<std::mutex> lck(m_mapSns2MtxOption[nSnsID]);
    return m_mapSns2RecordAttr[nSnsID][nChnID];
}

AX_VOID CWebOptionHelper::SetRecordAttr(AX_U8 nSnsID, AX_U32 nChnID, const WEB_RECORD_ATTR_T& tRecordAttr) {
    std::lock_guard<std::mutex> lck(m_mapSns2MtxOption[nSnsID]);
    m_mapSns2RecordAttr[nSnsID][nChnID] = tRecordAttr;
}

MprJson* CWebOptionHelper::GetFlashAttrJson(AX_U8 nSnsID) {
    MprJson* arrFlashAttrJson = mprCreateJson(MPR_JSON_ARRAY);

    std::lock_guard<std::mutex> lck(m_mapSns2MtxOption[nSnsID]);

    for (AX_U8 i = 0; i < MAX_LANE_COUNT; i++) {
        const WEB_OPR_FLASH_ATTR_T& attr = m_mapSns2CameraSetting[nSnsID].tFlashAttr[i];
        MprJson* objFlashAttr = mprCreateJson(MPR_JSON_OBJ);
        WRITE_JSON_INT(objFlashAttr, "lane_id", attr.nLaneId);
        WRITE_JSON_INT(objFlashAttr, "flash_id", attr.nFlashId);
        WRITE_JSON_INT(objFlashAttr, "trigger_count",  attr.nTriggerCount);
        WRITE_JSON_INT(objFlashAttr, "frame_interval",  attr.nFrameInterval);
        APPEND_JSON_OBJ(arrFlashAttrJson, objFlashAttr);
    }

    return arrFlashAttrJson;
}

picojson::object CWebOptionHelper::GetVideoAttrJsonObj(AX_U8 nSnsID, AX_U32 nPrevChnID) {
    // index of sensor group.
    AX_U8 nChannelID = nPrevChnID;

    std::lock_guard<std::mutex> lck(m_mapSns2MtxOption[nSnsID]);
    WEB_VIDEO_ATTR_T* pAttr = &m_mapSns2VideoAttr[nSnsID][nChannelID];

    picojson::object json;
    picojson::array encRcJson;
    for (int i = 0; i < APP_ENCODER_TYPE_MAX; i++) {
        picojson::object encRc;
        AX_U32 nEncType = CAXTypeConverter::EncoderType2Int(pAttr->stEncodeCfg[i].ePayloadType);
        encRc["encoder_type"] = picojson::value(double(nEncType));
        picojson::array valRc;
        for (AX_U32 j = 0; j < APP_RC_TYPE_MAX; j++) {
            RC_INFO_T rcInfo = pAttr->stEncodeCfg[i].stRCInfo[j];

            picojson::object rc;
            AX_U32 nRcType = CAXTypeConverter::RcMode2Int(rcInfo.eRcType);
            rc["rc_type"] = picojson::value(double(nRcType));
            rc["min_qp"] = picojson::value(double(rcInfo.nMinQp));
            rc["max_qp"] = picojson::value(double(rcInfo.nMaxQp));
            rc["min_iqp"] = picojson::value(double(rcInfo.nMinIQp));
            rc["max_iqp"] = picojson::value(double(rcInfo.nMaxIQp));
            rc["min_iprop"] = picojson::value(double(rcInfo.nMinIProp));
            rc["max_iprop"] = picojson::value(double(rcInfo.nMaxIProp));

            valRc.push_back(picojson::value(rc));
        }
        encRc["rc"] = picojson::value(valRc);
        encRcJson.push_back(picojson::value(encRc));
    }
    json["enc_rc_info"] = picojson::value(encRcJson);

    const auto resolutionOpts = GetChnResolutionList(nSnsID, nPrevChnID);
    picojson::array aryResolutionOpts;
    for (const auto& item : resolutionOpts) {
        aryResolutionOpts.push_back(picojson::value(item));
    }
    json["resolution_opt"] = picojson::value(aryResolutionOpts);

    return json;
}

picojson::object CWebOptionHelper::GetVideoJsonObj(AX_U8 nSnsID, AX_U32 nPrevChnID) {
    // index of sensor group.
    AX_U8 nChannelID = nPrevChnID;

    std::lock_guard<std::mutex> lck(m_mapSns2MtxOption[nSnsID]);
    WEB_VIDEO_ATTR_T* pAttr = &m_mapSns2VideoAttr[nSnsID][nChannelID];

    picojson::object json;
    json["enable_stream"] = picojson::value(bool(pAttr->bEnable));
    json["enable_res_chg"] = picojson::value(bool(1));
    json["encoder_type"] = picojson::value(double(pAttr->nEncoderType));
    json["bit_rate"] = picojson::value(double(pAttr->nBitrate));
    json["resolution"] = picojson::value(std::string(GenResStr(pAttr->nWidth, pAttr->nHeight)));
    json["rc_type"] = picojson::value(double(pAttr->nRcType));
    RC_INFO_T rcInfoCur;
    pAttr->GetEncRcCfg(rcInfoCur);
    json["min_qp"] = picojson::value(double(rcInfoCur.nMinQp));
    json["max_qp"] = picojson::value(double(rcInfoCur.nMaxQp));
    json["min_iqp"] = picojson::value(double(rcInfoCur.nMinIQp));
    json["max_iqp"] = picojson::value(double(rcInfoCur.nMaxIQp));
    json["min_iprop"] = picojson::value(double(rcInfoCur.nMinIProp));
    json["max_iprop"] = picojson::value(double(rcInfoCur.nMaxIProp));

    return json;
}

MprJson* CWebOptionHelper::GetRecordJson(AX_U8 nSnsID) {
    std::lock_guard<std::mutex> lck(m_mapSns2MtxOption[nSnsID]);

    MprJson* objRecordAttrJson = mprCreateJson(MPR_JSON_OBJ);

    AX_U8 nChnNum = GetVideoCount(nSnsID);
    WRITE_JSON_INT(objRecordAttrJson, "chn_num", nChnNum);
    WRITE_JSON_STR(objRecordAttrJson, "store_path", COptionHelper::GetInstance()->GetVideoSavedPath());

    MprJson* arrChnAttr = mprCreateJson(MPR_JSON_ARRAY);
    for (AX_U8 i = 0; i < nChnNum; i++) {
        MprJson* objChnAttr = mprCreateJson(MPR_JSON_OBJ);
        WRITE_JSON_BOOL(objChnAttr, "support", m_mapSns2RecordAttr[nSnsID][i].bSupport);
        WRITE_JSON_BOOL(objChnAttr, "enable", m_mapSns2RecordAttr[nSnsID][i].bEnable);
        WRITE_JSON_INT(objChnAttr, "record_type", m_mapSns2RecordAttr[nSnsID][i].nRecordType);
        WRITE_JSON_INT(objChnAttr, "raw_stream", m_mapSns2RecordAttr[nSnsID][i].nRawSteamType);
        APPEND_JSON_OBJ(arrChnAttr, objChnAttr);
    }

    WRITE_JSON_OBJ(objRecordAttrJson, "chn_attr", arrChnAttr);

    return objRecordAttrJson;
}

std::vector<std::string> CWebOptionHelper::GetChnResolutionList(AX_U8 nSnsID, AX_U32 nChnID) {
    std::vector<string> strResList;
    for (auto& res : m_mapSnsType2ResOptions[m_mapSnsID2Type[nSnsID]][nChnID]) {
        strResList.emplace_back(res);
    }

    return strResList;
}

AX_BOOL CWebOptionHelper::SetResolutionOptions(AX_U8 nSnsID, AX_U32 nChnID, AX_U32 nMaxWidth, AX_U32 nMaxHeight) {
    LOG_MM_I(WEB_OPTION, "sns(%d)chn(%d) resolution : %dx%d", nSnsID, nChnID, nMaxWidth, nMaxHeight);

    auto itSnsType = m_mapSnsID2Type.find(nSnsID);
    if (itSnsType == m_mapSnsID2Type.end()) {
        LOG_MM_E(WEB_OPTION, "Can't find sensor type for sns(%d)", nSnsID);
        return AX_FALSE;
    }

    std::multimap<AX_U8, WEB_VIDEO_RESOLUTION_INFO> mmapResolutionList;
    AX_U8 nSnsType = itSnsType->second;
    switch (nSnsType) {
        case E_SNS_TYPE_OS04A10x2_PANO:
        case E_SNS_TYPE_OS04A10x8_PANO:
            mmapResolutionList = m_mmapPano4MVideoResolutionList;
            break;
        case E_SNS_TYPE_SC410GS:
            mmapResolutionList = m_mmapSC410VideoResolutionList;
            break;
        default:
            mmapResolutionList = m_mmapComVideoResolutionList;
            break;
    }

    std::multimap<AX_U8, WEB_VIDEO_RESOLUTION_INFO>::iterator it, start, end;
    if (mmapResolutionList.end() == mmapResolutionList.find(nChnID)) {
        LOG_MM_E(WEB_OPTION, "sns(%d) can not find video resolution list for chn(%d)", nSnsID, nChnID);
        return AX_FALSE;
    }

    m_mapSnsType2ResOptions[nSnsType][nChnID].clear();

    AX_U32 nVResolution = nMaxWidth * nMaxHeight;
    AX_BOOL bMaxResolutionSet = AX_FALSE;
    start = mmapResolutionList.lower_bound(nChnID);
    end   = mmapResolutionList.upper_bound(nChnID);
    for (it = start; it != end; ++it) {
        if (it->second.nMagic > nVResolution) {
            continue;
        }

        if (it->second.nMagic == nVResolution) {
            bMaxResolutionSet = AX_TRUE;
        } else {
            if (!bMaxResolutionSet) {
                string strResolution = to_string(nMaxWidth) + "x" + to_string(nMaxHeight);
                m_mapSnsType2ResOptions[nSnsType][nChnID].push_back(strResolution);
                bMaxResolutionSet = AX_TRUE;
            }
        }

        m_mapSnsType2ResOptions[nSnsType][nChnID].push_back(it->second.strResolution);
    }

    return AX_TRUE;
}

std::string CWebOptionHelper::GetFramerateOptStr(AX_U8 nSnsID) {
    return m_mapSns2FramerateOpt[m_mapSnsID2Type[nSnsID]];
}

AX_BOOL CWebOptionHelper::GetVideoFramerateStr(AX_U8 nSnsID, AX_CHAR* pOutBuf, AX_U32 nBufferSize) {
    std::lock_guard<std::mutex> lck(m_mapSns2MtxOption[nSnsID]);
    AX_U8 nSize = GetVideoCount(nSnsID);
    AX_U8 aryFramerateOpts[3] = {0};
    for (AX_U8 i = 0; i < nSize; i++) {
        AX_U8 nUniChn = m_mapSns2VideoAttr[nSnsID][i].nUniChn;
        aryFramerateOpts[i] = CPrintHelper::GetInstance()->GetVencFramerate(nUniChn);
    }
    sprintf(pOutBuf, "[%d, %d, %d]", aryFramerateOpts[0], aryFramerateOpts[1], aryFramerateOpts[2]);
    return AX_TRUE;
}

AX_BOOL CWebOptionHelper::StatVencOutBytes(AX_U8 nSnsID, AX_U32 nUniChn, AX_U32 nBytes) {
    std::lock_guard<std::mutex> lck(m_mtxChnStat);
    if (m_mapSns2ChnStatInfo.find(nSnsID) == m_mapSns2ChnStatInfo.end()) {
        m_mapSns2ChnStatInfo[nSnsID][nUniChn].nStartTick = CElapsedTimer::GetInstance()->GetTickCount();
    }

    if (m_mapSns2ChnStatInfo[nSnsID].find(nUniChn) == m_mapSns2ChnStatInfo[nSnsID].end()) {
        m_mapSns2ChnStatInfo[nSnsID][nUniChn].nStartTick = CElapsedTimer::GetInstance()->GetTickCount();
    }

    if (0 == m_mapSns2ChnStatInfo[nSnsID][nUniChn].nStartTick) {
        m_mapSns2ChnStatInfo[nSnsID][nUniChn].nStartTick = CElapsedTimer::GetInstance()->GetTickCount();
    }

    m_mapSns2ChnStatInfo[nSnsID][nUniChn].nVencOutBytes += nBytes;

    return AX_TRUE;
}

AX_BOOL CWebOptionHelper::GetAssistBitrateStr(AX_U8 nSnsID, AX_U32 nUniChn, AX_CHAR* pOutBuf, AX_U32 nSize) {
    if (nullptr == pOutBuf || 0 == nSize) {
        return AX_FALSE;
    }

    std::lock_guard<std::mutex> lck(m_mtxChnStat);
    if (m_mapSns2ChnStatInfo.find(nSnsID) == m_mapSns2ChnStatInfo.end()) {
        return AX_FALSE;
    }

    if (m_mapSns2ChnStatInfo[nSnsID].find(nUniChn) == m_mapSns2ChnStatInfo[nSnsID].end()) {
        return AX_FALSE;
    }

    AX_U64 nEndTick = CElapsedTimer::GetInstance()->GetTickCount();
    AX_U64 nGap = nEndTick - m_mapSns2ChnStatInfo[nSnsID][nUniChn].nStartTick;

    AX_F64 fBitrate = m_mapSns2ChnStatInfo[nSnsID][nUniChn].nVencOutBytes / (AX_F64)nGap * 8;

    AX_S32 nCount = snprintf(pOutBuf, nSize, "%.2fkbps", fBitrate);
    if (nCount <= 0) {
        return AX_FALSE;
    }

    m_mapSns2ChnStatInfo[nSnsID][nUniChn].nVencOutBytes = 0;
    m_mapSns2ChnStatInfo[nSnsID][nUniChn].nStartTick = nEndTick;

    return AX_TRUE;
}

AX_BOOL CWebOptionHelper::GetOsdConfig(AX_U8 nSnsID, AX_S32 nGroup, AX_S32 nChn, std::vector<OSD_CFG_T>& vecOsdCfg) {
    std::lock_guard<std::mutex> lck(m_mtxOverlay);

    vecOsdCfg = m_mapSns2OsdConfig[nSnsID].mapGrpChnConfig[nGroup][nChn];
    return AX_TRUE;
}

AX_BOOL CWebOptionHelper::SetOsdConfig(AX_U8 nSnsID, AX_S32 nGroup, AX_S32 nChn, std::vector<OSD_CFG_T>& vecOsdCfg) {
    std::lock_guard<std::mutex> lck(m_mtxOverlay);

    m_mapSns2OsdConfig[nSnsID].mapGrpChnConfig[nGroup][nChn] = vecOsdCfg;
    return AX_TRUE;
}

AX_BOOL CWebOptionHelper::GetPrivacyConfig(AX_U8 nSnsID, AX_S32 nGroup, OSD_CFG_T& stOsdCfg) {
    std::lock_guard<std::mutex> lck(m_mtxOverlay);

    auto it = m_mapSns2PrivacyConfig.find(nSnsID);
    if (it != m_mapSns2PrivacyConfig.end()) {
        for (auto& grpCfgIt : it->second) {
            if (nGroup == grpCfgIt.first) {
                stOsdCfg = grpCfgIt.second;
                return AX_TRUE;
            }
       }
    }

    return AX_FALSE;
}

AX_BOOL CWebOptionHelper::SetPrivacyConfig(AX_U8 nSnsID, AX_S32 nGroup, const OSD_CFG_T& stOsdCfg) {
    std::lock_guard<std::mutex> lck(m_mtxOverlay);

    m_mapSns2PrivacyConfig[nSnsID][nGroup] = stOsdCfg;

    return AX_TRUE;
}

AX_BOOL CWebOptionHelper::IsOsdSwitchOpen(AX_S32 nIvpsGroup) {
    for (auto& m : m_mapSns2OsdConfig) {
        if (m.second.mapGrpChnConfig.find(nIvpsGroup) != m.second.mapGrpChnConfig.end()) {
            return m.second.bEnable;
        }
    }

    return AX_FALSE;
}

WEB_AUDIO_ATTR_T& CWebOptionHelper::GetAudio() {
    std::lock_guard<std::mutex> lck(m_mtxOverlay);
    return m_mapSns2AudioAttr[0];
}

AX_VOID CWebOptionHelper::SetAudio(const WEB_AUDIO_ATTR_T& tAudio) {
    std::lock_guard<std::mutex> lck(m_mtxOverlay);
    m_mapSns2AudioAttr[0] = tAudio;
}

AX_BOOL CWebOptionHelper::GetAiInfoStr(AX_U8 nSnsID, AX_CHAR* pOutBuf, AX_U32 nSize) {
    if (nullptr == pOutBuf || 0 == nSize) {
        return AX_FALSE;
    }

    std::lock_guard<std::mutex> lck(m_mapSns2MtxAi[nSnsID]);

    AX_CHAR szPushStrgy[128] = {0};
    AX_CHAR szDetModelAttr[512] = {0};
    AX_CHAR szEvent[256] = {0};

    if (!GetPushStrgyStr(nSnsID, szPushStrgy, 128) || !GetDetectModelAttrStr(szDetModelAttr, 512) || !GetEventsStr(nSnsID, szEvent, 256)) {
        return AX_FALSE;
    }

    AX_S32 nCount = snprintf(pOutBuf, nSize,
                             "{ai_enable: %s, \
                               detect_model: %s, \
                               detect_fps: %d, \
                               push_strategy: {%s}, \
                               detect_only: %s, \
                               %s: {%s}, \
                               events: {%s}}",
                               ADAPTER_INT2BOOLSTR(m_mapSns2AiAttr[nSnsID].bEnable), GetDetectModelStr().c_str(), 10, szPushStrgy,
                               ADAPTER_INT2BOOLSTR(1), GetDetectModelStr().c_str(), szDetModelAttr, szEvent);

    return nCount > 0 ? AX_TRUE : AX_FALSE;
}
AX_BOOL CWebOptionHelper::GetPushStrgyStr(AX_U8 nSnsID, AX_CHAR* pOutBuf, AX_U32 nSize) {
    if (nullptr == pOutBuf || 0 == nSize) {
        return AX_FALSE;
    }
    std::string strPushMode = GetPushModeStr(m_mapSns2AiAttr[nSnsID].tPushStategy.ePushMode);
    AX_U32 nPushCounts = m_mapSns2AiAttr[nSnsID].tPushStategy.nPushCounts;
    AX_U32 nPushIntervalMs = m_mapSns2AiAttr[nSnsID].tPushStategy.nPushIntervalMs;
    AX_S32 nCount = snprintf(pOutBuf, nSize, "push_mode: %s, push_interval:%d, push_count: %d, push_same_frame: %s", strPushMode.c_str(),
                             nPushIntervalMs, nPushCounts, ADAPTER_INT2BOOLSTR(1));

    return nCount > 0 ? AX_TRUE : AX_FALSE;
}

AX_BOOL CWebOptionHelper::GetDetectModelAttrStr(AX_CHAR* pOutBuf, AX_U32 nSize) {
    if (nullptr == pOutBuf || 0 == nSize) {
        return AX_FALSE;
    }

    AX_S32 nCount = 0;
    nCount = snprintf(pOutBuf, nSize,
                      "face_detect: {enable: %s, draw_rect: %s}, \
                       body_detect: {enable: %s, draw_rect: %s}, \
                       vechicle_detect: {enable: %s, draw_rect: %s}, \
                       cycle_detect: {enable: %s, draw_rect: %s}, \
                       plate_detect: {enable: %s, draw_rect: %s}, \
                       plate_identify: {enable: %s}",
                       ADAPTER_INT2BOOLSTR(0), ADAPTER_INT2BOOLSTR(0), ADAPTER_INT2BOOLSTR(0), ADAPTER_INT2BOOLSTR(0),
                       ADAPTER_INT2BOOLSTR(0), ADAPTER_INT2BOOLSTR(0), ADAPTER_INT2BOOLSTR(0), ADAPTER_INT2BOOLSTR(0),
                       ADAPTER_INT2BOOLSTR(0), ADAPTER_INT2BOOLSTR(0), ADAPTER_INT2BOOLSTR(0));

    /*
        AX_S32 nCount = 0;
        if (E_AI_DETECT_MODEL_TYPE_FACEHUMAN == m_tAiAttr.eDetectModel) {
            nCount = snprintf(pOutBuf, nSize, "face_detect: {enable: %s, draw_rect: %s}, \
                body_detect: {enable: %s, draw_rect: %s}, \
                face_identify: {enable: %s}",
                ADAPTER_INT2BOOLSTR(m_tAiAttr.tHumanFaceSetting.tFace.nEnable),
                ADAPTER_INT2BOOLSTR(m_tAiAttr.tHumanFaceSetting.tFace.nDrawRect),
                ADAPTER_INT2BOOLSTR(m_tAiAttr.tHumanFaceSetting.tBody.nEnable),
                ADAPTER_INT2BOOLSTR(m_tAiAttr.tHumanFaceSetting.tBody.nDrawRect),
                ADAPTER_INT2BOOLSTR(m_tAiAttr.tHumanFaceSetting.nEnableFI));
        } else if (E_AI_DETECT_MODEL_TYPE_HVCFP == m_tAiAttr.eDetectModel) {
            nCount = snprintf(pOutBuf, nSize, "face_detect: {enable: %s, draw_rect: %s}, \
                body_detect: {enable: %s, draw_rect: %s}, \
                vechicle_detect: {enable: %s, draw_rect: %s}, \
                cycle_detect: {enable: %s, draw_rect: %s}, \
                plate_detect: {enable: %s, draw_rect: %s}, \
                plate_identify: {enable: %s}",
                ADAPTER_INT2BOOLSTR(m_tAiAttr.tHvcfpSetting.tFace.nEnable),
                ADAPTER_INT2BOOLSTR(m_tAiAttr.tHvcfpSetting.tFace.nDrawRect),
                ADAPTER_INT2BOOLSTR(m_tAiAttr.tHvcfpSetting.tBody.nEnable),
                ADAPTER_INT2BOOLSTR(m_tAiAttr.tHvcfpSetting.tBody.nDrawRect),
                ADAPTER_INT2BOOLSTR(m_tAiAttr.tHvcfpSetting.tVechicle.nEnable),
                ADAPTER_INT2BOOLSTR(m_tAiAttr.tHvcfpSetting.tVechicle.nDrawRect),
                ADAPTER_INT2BOOLSTR(m_tAiAttr.tHvcfpSetting.tCycle.nEnable),
                ADAPTER_INT2BOOLSTR(m_tAiAttr.tHvcfpSetting.tCycle.nDrawRect),
                ADAPTER_INT2BOOLSTR(m_tAiAttr.tHvcfpSetting.tPlate.nEnable),
                ADAPTER_INT2BOOLSTR(m_tAiAttr.tHvcfpSetting.tPlate.nDrawRect),
                ADAPTER_INT2BOOLSTR(m_tAiAttr.tHvcfpSetting.nEnablePI));
        }
    */
    return nCount > 0 ? AX_TRUE : AX_FALSE;
}

AX_BOOL CWebOptionHelper::GetEventsStr(AX_U8 nSnsID, AX_CHAR* pOutBuf, AX_U32 nSize) {
    if (nullptr == pOutBuf || 0 == nSize) {
        return AX_FALSE;
    }

    AX_S32 nCount =
        snprintf(pOutBuf, nSize,
                 "motion_detect: {enable: %s, threshold_y: %d, confidence: %d}, \
                  occlusion_detect: {enable: %s, threshold_y: %d, confidence: %d}, \
                  scene_change_detect: {enable: %s, threshold_y: %d, confidence: %d}",
                  ADAPTER_BOOL2BOOLSTR(m_mapSns2AiAttr[nSnsID].tEvents.tMD.bEnable), m_mapSns2AiAttr[nSnsID].tEvents.tMD.nThrsHoldY,
                  m_mapSns2AiAttr[nSnsID].tEvents.tMD.nConfidence, ADAPTER_BOOL2BOOLSTR(m_mapSns2AiAttr[nSnsID].tEvents.tOD.bEnable),
                  m_mapSns2AiAttr[nSnsID].tEvents.tOD.nThrsHoldY, m_mapSns2AiAttr[nSnsID].tEvents.tOD.nConfidence,
                  ADAPTER_BOOL2BOOLSTR(m_mapSns2AiAttr[nSnsID].tEvents.tSCD.bEnable), m_mapSns2AiAttr[nSnsID].tEvents.tSCD.nThrsHoldY,
                  m_mapSns2AiAttr[nSnsID].tEvents.tSCD.nConfidence);

    return nCount > 0 ? AX_TRUE : AX_FALSE;
}

std::string CWebOptionHelper::GetDetectModelStr() {
    return "hvcfp";

    // if (E_AI_DETECT_MODEL_TYPE_FACEHUMAN == m_tAiAttr.eDetectModel) {
    //     return "facehuman";
    // } else if (E_AI_DETECT_MODEL_TYPE_HVCFP == m_tAiAttr.eDetectModel) {
    //     return "hvcfp";
    // } else {
    //     return "";
    // }
}

AX_BOOL CWebOptionHelper::ParseWebRequest(WEB_REQUEST_TYPE_E eReqType, const AX_VOID* pJsonReq, vector<WEB_REQ_OPERATION_T>& vecWebOpr) {
    if (!CheckRequest(eReqType, pJsonReq)) {
        return AX_FALSE;
    }

    AX_U8 nSnsID = 0;
    MprJson* pJson = (MprJson*)pJsonReq;

    switch (eReqType) {
        case E_REQ_TYPE_TRIGGER: {
            WEB_SET_INT(nSnsID, pJson, "src_id");
            LOG_MM_C(WEB_OPTION, "[Trigger] Web request: sensor %d", nSnsID);
            AX_BOOL bTrigger = AX_FALSE;
            WEB_SET_BOOL(bTrigger, pJson, "trigger_attr.trigger");
            if (bTrigger) {
                WEB_REQ_OPERATION_T tOperation;
                tOperation.nSnsID = nSnsID;
                tOperation.nChannel = -1;
                tOperation.eReqType = E_REQ_TYPE_CAMERA;
                tOperation.SetOperaType(E_WEB_OPERATION_TYPE_TRIGGER);
                vecWebOpr.emplace_back(tOperation);
                LOG_MM_I(WEB_OPTION, "[%d] camera trigger", nSnsID);
            }

            break;
        }
        case E_REQ_TYPE_CAMERA: {
            AX_BOOL bFpsFlag = AX_FALSE;
            WEB_SET_INT(nSnsID, pJson, "src_id");
            if (m_mapSns2CameraSetting.find(nSnsID) == m_mapSns2CameraSetting.end()) {
                LOG_M_E(WEB_OPTION, "Camera setting for sensor: %d not configured.");
                return AX_FALSE;
            }
            LOG_MM_C(WEB_OPTION, "[Camera] Web request: sensor %d", nSnsID);

            AX_U8 nSnsMode = 0;
            MprJson* pJsonCamera = mprReadJsonObj(pJson, "camera_attr");
            WEB_SET_INT(nSnsMode, pJsonCamera, "sns_work_mode");
            if (nSnsMode != m_mapSns2CameraSetting[nSnsID].nSnsMode) {
                bFpsFlag = AX_TRUE;

                WEB_REQ_OPERATION_T tOperation;
                tOperation.nSnsID = nSnsID;
                tOperation.nChannel = -1;
                tOperation.eReqType = E_REQ_TYPE_CAMERA;
                tOperation.SetOperaType(E_WEB_OPERATION_TYPE_SNS_MODE);
                tOperation.tSnsMode.nSnsMode = nSnsMode;
                WEB_SET_INT(tOperation.tSnsMode.nFps, pJsonCamera, "framerate");
                vecWebOpr.emplace_back(tOperation);
                LOG_MM_C(WEB_OPTION, "[%d] sensor work mode changed: [%d] => [%d]", nSnsID, m_mapSns2CameraSetting[nSnsID].nSnsMode,
                         nSnsMode);
                m_mapSns2CameraSetting[nSnsID].nSnsMode = nSnsMode;
            }

            AX_U8 nRotation = 0;
            WEB_SET_INT(nRotation, pJsonCamera, "rotation");
            if (nRotation != m_mapSns2CameraSetting[nSnsID].nRotation) {
                WEB_REQ_OPERATION_T tOperation;
                tOperation.nSnsID = nSnsID;
                tOperation.nChannel = -1;
                tOperation.eReqType = E_REQ_TYPE_CAMERA;
                tOperation.SetOperaType(E_WEB_OPERATION_TYPE_ROTATION);
                tOperation.tRotation.nRotation = nRotation;
                vecWebOpr.emplace_back(tOperation);

                LOG_MM_C(WEB_OPTION, "[%d] camera rotation changed: [%d] => [%d]", nSnsID, m_mapSns2CameraSetting[nSnsID].nRotation,
                         nRotation);
                m_mapSns2CameraSetting[nSnsID].nRotation = nRotation;
            }

            AX_U8 nDayNightMode = 0;
            WEB_SET_INT(nDayNightMode, pJson, "camera_attr.daynight");
            if (nDayNightMode != m_mapSns2CameraSetting[nSnsID].nDayNightMode) {
                WEB_REQ_OPERATION_T tOperation;
                tOperation.nSnsID = nSnsID;
                tOperation.nChannel = -1;
                tOperation.eReqType = E_REQ_TYPE_CAMERA;
                tOperation.SetOperaType(E_WEB_OPERATION_TYPE_DAYNIGHT);
                tOperation.tDaynight.nDayNightMode = nDayNightMode;
                vecWebOpr.emplace_back(tOperation);

                LOG_MM_C(WEB_OPTION, "[%d] daynight changed: [%d] => [%d]", nSnsID, m_mapSns2CameraSetting[nSnsID].nDayNightMode,
                         nDayNightMode);
                m_mapSns2CameraSetting[nSnsID].nDayNightMode = nDayNightMode;
            }

            cchar* szEnable = mprReadJson(pJsonCamera, "auto_capture");
            AX_BOOL bCapEnable = ((szEnable && strcmp(szEnable, "true") == 0) ? AX_TRUE : AX_FALSE);
            if (bCapEnable != m_mapSns2CameraSetting[nSnsID].bAutoCapture) {
                WEB_REQ_OPERATION_T tOperation;
                tOperation.nSnsID = nSnsID;
                tOperation.nChannel = -1;
                tOperation.eReqType = E_REQ_TYPE_CAMERA;
                tOperation.SetOperaType(E_WEB_OPERATION_TYPE_CAPTURE_AUTO);
                tOperation.tCapEnable.bOn = bCapEnable;
                vecWebOpr.emplace_back(tOperation);
                LOG_MM_C(WEB_OPTION, "[%d] capture_enable, changed: [%d] => [%d]", nSnsID, m_mapSns2CameraSetting[nSnsID].bAutoCapture,
                         bCapEnable);
                m_mapSns2CameraSetting[nSnsID].bAutoCapture = bCapEnable;
            }

            AX_U32 nFrameRate = 0;
            WEB_SET_INT(nFrameRate, pJsonCamera, "framerate");
            if (!bFpsFlag && nFrameRate != m_mapSns2CameraSetting[nSnsID].fFramerate) {
                WEB_REQ_OPERATION_T tOperation;
                tOperation.nSnsID = nSnsID;
                tOperation.nChannel = -1;
                tOperation.eReqType = E_REQ_TYPE_CAMERA;
                tOperation.SetOperaType(E_WEB_OPERATION_TYPE_CAMERA_FPS);
                tOperation.tSnsFpsAttr.fSnsFrameRate = nFrameRate;

                vecWebOpr.emplace_back(tOperation);
                LOG_MM_I(WEB_OPTION, "[%d] camera fps, changed:[%f]=> [%d]", nSnsID, m_mapSns2CameraSetting[nSnsID].fFramerate, nFrameRate);
                m_mapSns2CameraSetting[nSnsID].fFramerate = nFrameRate;
            }

            cchar* szMirrorEnable = mprReadJson(pJsonCamera, "mirror");
            cchar* szFlipEnable = mprReadJson(pJsonCamera, "flip");
            if (nullptr != szMirrorEnable && nullptr != szFlipEnable) {
                AX_BOOL bMirrorEnable = (strcmp(szMirrorEnable, "true") == 0 ? AX_TRUE : AX_FALSE);
                AX_BOOL bFlipEnable = (strcmp(szFlipEnable, "true") == 0 ? AX_TRUE : AX_FALSE);

                if (m_mapSns2CameraSetting[nSnsID].bMirror != bMirrorEnable || m_mapSns2CameraSetting[nSnsID].bFlip != bFlipEnable) {
                    WEB_REQ_OPERATION_T tOperation;
                    tOperation.nSnsID = nSnsID;
                    tOperation.nChannel = -1;
                    tOperation.eReqType = E_REQ_TYPE_CAMERA;
                    tOperation.SetOperaType(E_WEB_OPERATION_TYPE_SNS_MIRROR_FLIP);
                    tOperation.tSnsMirrorFlip.bMirror = bMirrorEnable;
                    tOperation.tSnsMirrorFlip.bFlip = bFlipEnable;

                    vecWebOpr.emplace_back(tOperation);
                    LOG_MM_I(WEB_OPTION, "[%d] camera mirror flip, changed:[%d %d]=> [%d %d]", nSnsID,
                             m_mapSns2CameraSetting[nSnsID].bMirror, m_mapSns2CameraSetting[nSnsID].bFlip, bMirrorEnable, bFlipEnable);
                    m_mapSns2CameraSetting[nSnsID].bMirror = bMirrorEnable;
                    m_mapSns2CameraSetting[nSnsID].bFlip = bFlipEnable;
                }
            }

            if (m_mapSns2CameraSetting[nSnsID].bTriggerEnable) {
                AX_BOOL bChanged = AX_FALSE;
                WEB_REQ_OPERATION_T tOperation;
                tOperation.nSnsID = nSnsID;
                tOperation.nChannel = -1;
                tOperation.eReqType = E_REQ_TYPE_CAMERA;
                tOperation.SetOperaType(E_WEB_OPERATION_TYPE_FLASH_SETTING);

                memcpy(tOperation.tFlashAttr, GetCamera(nSnsID).tFlashAttr, sizeof(tOperation.tFlashAttr));

                for (AX_U8 i = 0; i < MAX_LANE_COUNT; i++) {
                    AX_CHAR name[32] = {0};
                    sprintf(name,"flash_attr[%d].flash_id", i);
                    AX_S8 nFlashId = 0;
                    WEB_SET_INT(nFlashId, pJson, name);

                    memset(name, 0, sizeof(name));
                    sprintf(name,"flash_attr[%d].trigger_count", i);
                    AX_S8 nTriggerCount = 0;
                    WEB_SET_INT(nTriggerCount, pJson, name);

                    memset(name, 0, sizeof(name));
                    sprintf(name,"flash_attr[%d].frame_interval", i);
                    AX_S8 nFrameInterval = 0;
                    WEB_SET_INT(nFrameInterval, pJson, name);

                    WEB_OPR_FLASH_ATTR_T tOldAttr;
                    memcpy(&tOldAttr, &GetCamera(nSnsID).tFlashAttr[i], sizeof(WEB_OPR_FLASH_ATTR_T));

                    if ((nFlashId != tOldAttr.nFlashId) || (nTriggerCount != tOldAttr.nTriggerCount) || (nFrameInterval != tOldAttr.nFrameInterval)) {
                        tOperation.tFlashAttr[i].nFlashId = nFlashId;
                        tOperation.tFlashAttr[i].nTriggerCount = nTriggerCount;
                        tOperation.tFlashAttr[i].nFrameInterval = nFrameInterval;
                        bChanged = AX_TRUE;
                        LOG_MM_C(WEB_OPTION, "[%d] flashsetting lane [%d] changed: [%d,%d,%d] => [%d,%d,%d]", nSnsID, i, tOldAttr.nFlashId, tOldAttr.nTriggerCount,\
                                tOldAttr.nFrameInterval, nFlashId, nTriggerCount, nFrameInterval);
                    }
                }

                if (bChanged) {
                    vecWebOpr.emplace_back(tOperation);
                }
            }

            cchar* szEISOn = mprReadJson(pJsonCamera, "eis");
            AX_BOOL bEISOn = ((szEISOn && strcmp(szEISOn, "true") == 0) ? AX_TRUE : AX_FALSE);
            if (bEISOn != m_mapSns2CameraSetting[nSnsID].bEISOn) {
                WEB_REQ_OPERATION_T tOperation;
                tOperation.nSnsID = nSnsID;
                tOperation.nChannel = -1;
                tOperation.eReqType = E_REQ_TYPE_CAMERA;
                tOperation.SetOperaType(E_WEB_OPERATION_TYPE_EIS);
                tOperation.tEIS.bOn = bEISOn;
                vecWebOpr.emplace_back(tOperation);
                LOG_MM_C(WEB_OPTION, "[%d] eis_enable, changed: [%d] => [%d]", nSnsID, m_mapSns2CameraSetting[nSnsID].bEISOn,
                         bEISOn);
                m_mapSns2CameraSetting[nSnsID].bEISOn = bEISOn;
            }

            AX_U8 nShutterType = 0;
            WEB_SET_INT(nShutterType, pJson, "camera_attr.shutter_type");
            if (nShutterType != m_mapSns2CameraSetting[nSnsID].nShutterType) {
                WEB_REQ_OPERATION_T tOperation;
                tOperation.nSnsID = nSnsID;
                tOperation.nChannel = -1;
                tOperation.eReqType = E_REQ_TYPE_CAMERA;
                tOperation.SetOperaType(E_WEB_OPERATION_TYPE_SHUTTER_TYPE);
                tOperation.tShutter.nShutterType = nShutterType;
                vecWebOpr.emplace_back(tOperation);

                LOG_MM_C(WEB_OPTION, "[%d] shutter type changed: [%d] => [%d]", nSnsID, m_mapSns2CameraSetting[nSnsID].nShutterType,
                         nShutterType);
                m_mapSns2CameraSetting[nSnsID].nShutterType = nShutterType;
            }

            AX_U32 nDumpRawPipe = 0;
            WEB_SET_INT(nDumpRawPipe, pJsonCamera, "dump_raw_pipe");
            if (nDumpRawPipe != m_mapSns2CameraSetting[nSnsID].nDumpRawPipe) {
                WEB_REQ_OPERATION_T tOperation;
                tOperation.nSnsID = nSnsID;
                tOperation.nChannel = -1;
                tOperation.eReqType = E_REQ_TYPE_CAMERA;
                tOperation.SetOperaType(E_WEB_OPERATION_TYPE_GET_DUMPRAWPIPE);
                tOperation.tDumpRawPipe.nPipeId = nDumpRawPipe;
                vecWebOpr.emplace_back(tOperation);
                LOG_MM_C(WEB_OPTION, "[%d] nDumpRawPipe, changed: [%d] => [%d]",
                                      nSnsID, m_mapSns2CameraSetting[nSnsID].nDumpRawPipe, nDumpRawPipe);
            }

            cchar* szVSubChnOn = mprReadJson(pJsonCamera, "vsubchn_on");
            AX_BOOL bVSubChnOn = ((szVSubChnOn && strcmp(szVSubChnOn, "true") == 0) ? AX_TRUE : AX_FALSE);
            if (bVSubChnOn != m_mapSns2CameraSetting[nSnsID].bVSubChnOn) {
                WEB_REQ_OPERATION_T tOperation;
                tOperation.nSnsID = nSnsID;
                tOperation.nChannel = 1;
                tOperation.eReqType = E_REQ_TYPE_CAMERA;
                tOperation.SetOperaType(E_WEB_OPERATION_TYPE_VSUBCHN_SWITCH);
                tOperation.tVSubChnEnable.bOn = bVSubChnOn;
                vecWebOpr.emplace_back(tOperation);
                LOG_MM_C(WEB_OPTION, "[%d] bVSubChnOn, changed: [%d] => [%d]",
                                      nSnsID, m_mapSns2CameraSetting[nSnsID].bVSubChnOn, bVSubChnOn);
            }
            break;
        }
        case E_REQ_TYPE_IMAGE: {
            WEB_SET_INT(nSnsID, pJson, "src_id");
            if (m_mapSns2CameraSetting.find(nSnsID) == m_mapSns2CameraSetting.end()) {
                LOG_M_E(WEB_OPTION, "Camera setting for sensor: %d not configured.");
                return AX_FALSE;
            }
            LOG_MM_C(WEB_OPTION, "[Image] Web request: sensor %d", nSnsID);

            AX_U8 nAutoMode = 0;
            AX_U8 nSharpness = 0;
            AX_U8 nBrightness = 0;
            AX_U8 nContrast = 0;
            AX_U8 nSaturation = 0;

            MprJson* pJsonImage = mprReadJsonObj(pJson, "image_attr");
            WEB_SET_INT(nSharpness, pJsonImage, "sharpness_val");
            WEB_SET_INT(nBrightness, pJsonImage, "brightness_val");
            WEB_SET_INT(nContrast, pJsonImage, "contrast_val");
            WEB_SET_INT(nSaturation, pJsonImage, "saturation_val");
            WEB_SET_INT(nAutoMode, pJsonImage, "isp_auto_mode");

            if (nAutoMode != m_mapSns2CameraSetting[nSnsID].tImageAttr.nAutoMode ||
                nSharpness != m_mapSns2CameraSetting[nSnsID].tImageAttr.nSharpness ||
                nBrightness != m_mapSns2CameraSetting[nSnsID].tImageAttr.nBrightness ||
                nContrast != m_mapSns2CameraSetting[nSnsID].tImageAttr.nContrast ||
                nSaturation != m_mapSns2CameraSetting[nSnsID].tImageAttr.nSaturation) {
                WEB_REQ_OPERATION_T tOperation;
                tOperation.nSnsID = nSnsID;
                tOperation.nChannel = -1;
                tOperation.eReqType = E_REQ_TYPE_IMAGE;
                tOperation.SetOperaType(E_WEB_OPERATION_TYPE_IMAGE_ATTR);
                tOperation.tImageAttr.nSharpness = nSharpness;
                tOperation.tImageAttr.nBrightness = nBrightness;
                tOperation.tImageAttr.nContrast = nContrast;
                tOperation.tImageAttr.nSaturation = nSaturation;
                tOperation.tImageAttr.nAutoMode = nAutoMode;

                vecWebOpr.emplace_back(tOperation);
                LOG_MM_C(WEB_OPTION, "[%d] image_attr changed: [%d]-[%d %d %d %d] => [%d]-[%d %d %d %d]", nSnsID,
                         m_mapSns2CameraSetting[nSnsID].tImageAttr.nAutoMode, m_mapSns2CameraSetting[nSnsID].tImageAttr.nSharpness,
                         m_mapSns2CameraSetting[nSnsID].tImageAttr.nBrightness, m_mapSns2CameraSetting[nSnsID].tImageAttr.nContrast,
                         m_mapSns2CameraSetting[nSnsID].tImageAttr.nSaturation, nAutoMode, nSharpness, nBrightness, nContrast,
                         nSaturation);

                m_mapSns2CameraSetting[nSnsID].tImageAttr.nSharpness = nSharpness;
                m_mapSns2CameraSetting[nSnsID].tImageAttr.nBrightness = nBrightness;
                m_mapSns2CameraSetting[nSnsID].tImageAttr.nContrast = nContrast;
                m_mapSns2CameraSetting[nSnsID].tImageAttr.nSaturation = nSaturation;
                m_mapSns2CameraSetting[nSnsID].tImageAttr.nAutoMode = nAutoMode;
            }

            if (m_mapSns2CameraSetting[nSnsID].bLdcEnable) {
                MprJson* pJsonLDC = mprReadJsonObj(pJson, "ldc_attr");
                cchar* szLdcEnable = mprReadJson(pJsonLDC, "ldc_enable");
                cchar* szAspect = mprReadJson(pJsonLDC, "aspect");
                cchar* szXRatio = mprReadJson(pJsonLDC, "x_ratio");
                cchar* szYRatio = mprReadJson(pJsonLDC, "y_ratio");
                cchar* szXYRatio = mprReadJson(pJsonLDC, "xy_ratio");
                cchar* szDistorRatio = mprReadJson(pJsonLDC, "distor_ratio");
                if (nullptr != szLdcEnable && nullptr != szAspect && nullptr != szXRatio && nullptr != szYRatio && nullptr != szXYRatio &&
                    nullptr != szDistorRatio) {
                    AX_BOOL bLdcEnable = (strcmp(szLdcEnable, "true") == 0 ? AX_TRUE : AX_FALSE);
                    AX_BOOL bLdcAspect = (strcmp(szAspect, "true") == 0 ? AX_TRUE : AX_FALSE);
                    AX_S16 nXRatio = atoi(szXRatio);
                    AX_S16 nYRatio = atoi(szYRatio);
                    AX_S16 nXYRatio = atoi(szXYRatio);
                    AX_S16 nDistorRatio = atoi(szDistorRatio);
                    if (bLdcEnable != m_mapSns2CameraSetting[nSnsID].tLdcAttr.bLdcEnable ||
                        bLdcAspect != m_mapSns2CameraSetting[nSnsID].tLdcAttr.bLdcAspect ||
                        nXRatio != m_mapSns2CameraSetting[nSnsID].tLdcAttr.nXRatio ||
                        nYRatio != m_mapSns2CameraSetting[nSnsID].tLdcAttr.nYRatio ||
                        nXYRatio != m_mapSns2CameraSetting[nSnsID].tLdcAttr.nXYRatio ||
                        nDistorRatio != m_mapSns2CameraSetting[nSnsID].tLdcAttr.nDistorRatio) {
                        WEB_REQ_OPERATION_T tOperation;
                        tOperation.nSnsID = nSnsID;
                        tOperation.nChannel = -1;
                        tOperation.eReqType = E_REQ_TYPE_IMAGE;
                        tOperation.SetOperaType(E_WEB_OPERATION_TYPE_LDC_ATTR);
                        tOperation.tLdcAttr.bLdcEnable = bLdcEnable;
                        tOperation.tLdcAttr.bLdcAspect = bLdcAspect;
                        tOperation.tLdcAttr.nXRatio = nXRatio;
                        tOperation.tLdcAttr.nYRatio = nYRatio;
                        tOperation.tLdcAttr.nXYRatio = nXYRatio;
                        tOperation.tLdcAttr.nDistorRatio = nDistorRatio;

                        vecWebOpr.emplace_back(tOperation);
                        LOG_MM_C(WEB_OPTION, "[%d] ldc_attr changed: [%d]-[%d %d %d %d %d] => [%d]-[%d %d %d %d %d]", nSnsID,
                                 m_mapSns2CameraSetting[nSnsID].tLdcAttr.bLdcEnable, m_mapSns2CameraSetting[nSnsID].tLdcAttr.bLdcAspect,
                                 m_mapSns2CameraSetting[nSnsID].tLdcAttr.nXRatio, m_mapSns2CameraSetting[nSnsID].tLdcAttr.nYRatio,
                                 m_mapSns2CameraSetting[nSnsID].tLdcAttr.nXYRatio, m_mapSns2CameraSetting[nSnsID].tLdcAttr.nDistorRatio,
                                 bLdcEnable, bLdcAspect, nXRatio, nYRatio, nXYRatio, nDistorRatio);

                        m_mapSns2CameraSetting[nSnsID].tLdcAttr.bLdcEnable = bLdcEnable;
                        m_mapSns2CameraSetting[nSnsID].tLdcAttr.bLdcAspect = bLdcAspect;
                        m_mapSns2CameraSetting[nSnsID].tLdcAttr.nXRatio = nXRatio;
                        m_mapSns2CameraSetting[nSnsID].tLdcAttr.nYRatio = nYRatio;
                        m_mapSns2CameraSetting[nSnsID].tLdcAttr.nXYRatio = nXYRatio;
                        m_mapSns2CameraSetting[nSnsID].tLdcAttr.nDistorRatio = nDistorRatio;
                    }
                }
            }
            break;
        }
        case E_REQ_TYPE_AUDIO: {

            WEB_AUDIO_ATTR_T tOldAttr = GetAudio();
            AX_U8 nCapture_volume = (AX_U8)tOldAttr.fCapture_volume;
            AX_U8 nPlay_volume = (AX_U8)tOldAttr.fPlay_volume;
            LOG_MM_C(WEB_OPTION, "[Audio] Web request: ...");
            WEB_SET_INT(nCapture_volume, mprReadJsonObj(pJson, "capture_attr"), "volume_val");
            WEB_SET_INT(nPlay_volume, mprReadJsonObj(pJson, "play_attr"), "volume_val");

            if ((nCapture_volume != (AX_U8)tOldAttr.fCapture_volume) || (nPlay_volume != (AX_U8)tOldAttr.fPlay_volume)) {
                WEB_REQ_OPERATION_T tOperation;
                tOperation.nSnsID = 0;
                tOperation.nGroup = 0;
                tOperation.eReqType = E_REQ_TYPE_AUDIO;
                tOperation.SetOperaType(E_WEB_OPERATION_TYPE_AUDIO_ATTR);
                tOperation.tAudioAttr.nCapture_volume = nCapture_volume;
                tOperation.tAudioAttr.nPlay_volume = nPlay_volume;
                vecWebOpr.emplace_back(tOperation);
                m_mapSns2AudioAttr[0].fCapture_volume = nCapture_volume;
                m_mapSns2AudioAttr[0].fPlay_volume = nPlay_volume;
            }
            break;
        }
        case E_REQ_TYPE_VIDEO: {
            WEB_SET_INT(nSnsID, pJson, "src_id");
            AX_BOOL bResolutionChanged = AX_FALSE;

            AX_U8 nSize = GetVideoCount(nSnsID);
            for (AX_U8 i = 0; i < nSize; i++) {
                char szKey[64] = {0};
                sprintf(szKey, "video%d", i);
                MprJson* jsonObj = mprReadJsonObj(pJson, szKey);
                if (!jsonObj) {
                    LOG_M_I(WEB_OPTION, "[video] [%d] mprReadJsonObj %s is empty", nSnsID, szKey);
                    continue;
                }

                LOG_MM_C(WEB_OPTION, "[video] Web request: sensor %d => %s", nSnsID, szKey);

                WEB_VIDEO_ATTR_T tOldAttr = GetVideo(nSnsID, i);
                AX_U8 nEncoderType = 0;
                WEB_SET_INT(nEncoderType, jsonObj, "encoder_type");
                if (nEncoderType != tOldAttr.nEncoderType) {
                    WEB_REQ_OPERATION_T tOperation;
                    tOperation.nSnsID = nSnsID;
                    tOperation.nGroup = 0;
                    tOperation.nChannel = m_mapSns2VideoAttr[nSnsID][i].nUniChn;
                    tOperation.eReqType = E_REQ_TYPE_VIDEO;
                    tOperation.SetOperaType(E_WEB_OPERATION_TYPE_ENC_TYPE);
                    tOperation.tEncType.nEncoderType = nEncoderType;

                    vecWebOpr.emplace_back(tOperation);
                    LOG_MM_C(WEB_OPTION, "[%d,%d] encode type changed: [%d] => [ %d]", nSnsID, i, tOldAttr.nEncoderType, nEncoderType);
                    m_mapSns2VideoAttr[nSnsID][i].nEncoderType = nEncoderType;
                }

                cchar * szResStr = mprReadJson(jsonObj, "resolution");
                if (szResStr) {
                    string strResStr = szResStr;
                    AX_U32 nWidth = 0;
                    AX_U32 nHeight = 0;
                    if (ParseResStr(strResStr, nWidth, nHeight) && (nWidth != tOldAttr.nWidth || nHeight != tOldAttr.nHeight)) {
                        WEB_REQ_OPERATION_T tOperation;
                        tOperation.nSnsID = nSnsID;
                        tOperation.nGroup = 0;
                        tOperation.nChannel = m_mapSns2VideoAttr[nSnsID][i].nUniChn; /* unique channel id corresponding to venc id */
                        tOperation.eReqType = E_REQ_TYPE_VIDEO;
                        tOperation.SetOperaType(E_WEB_OPERATION_TYPE_RESOLUTION);
                        tOperation.tResolution.nWidth = nWidth;
                        tOperation.tResolution.nHeight = nHeight;
                        tOperation.tResolution.nEncoderType = nEncoderType;

                        vecWebOpr.emplace_back(tOperation);
                        LOG_MM_C(WEB_OPTION, "[%d,%d] encodeType:%d changed: [%d, %d] => [%d, %d]", nSnsID, i,
                                tOperation.tResolution.nEncoderType, tOldAttr.nWidth, tOldAttr.nHeight, nWidth, nHeight);

                        m_mapSns2VideoAttr[nSnsID][i].nWidth = nWidth;
                        m_mapSns2VideoAttr[nSnsID][i].nHeight = nHeight;

                        if (!bResolutionChanged) {
                            bResolutionChanged = AX_TRUE;
                        }
                    }
                }

                cchar* szBitrate = mprReadJson(jsonObj, "bit_rate");
                if (szBitrate) {
                    AX_U32 nBitrate = (AX_S32)atoi(szBitrate);
                    if (nBitrate != tOldAttr.nBitrate) {
                        WEB_REQ_OPERATION_T tOperation;
                        tOperation.nSnsID = nSnsID;
                        tOperation.nGroup = 0;
                        tOperation.nChannel = m_mapSns2VideoAttr[nSnsID][i].nUniChn;
                        tOperation.eReqType = E_REQ_TYPE_VIDEO;
                        tOperation.SetOperaType(E_WEB_OPERATION_TYPE_BITRATE);
                        tOperation.tBitrate.nBitrate = nBitrate;
                        tOperation.tResolution.nEncoderType = nEncoderType;

                        vecWebOpr.emplace_back(tOperation);
                        LOG_MM_C(WEB_OPTION, "[%d,%d] bitrate changed: [%d] => [%d]", nSnsID, i, tOldAttr.nBitrate, nBitrate);

                        m_mapSns2VideoAttr[nSnsID][i].nBitrate = nBitrate;
                    }
                } else {
                    LOG_M_E(WEB_OPTION, "[%d,%d] Empty bitrate value received.", nSnsID, i);
                }

                AX_BOOL bEnable = AX_FALSE;
                WEB_SET_BOOL(bEnable, jsonObj, "enable_stream");
                if (bEnable != tOldAttr.bEnable) {
                    WEB_REQ_OPERATION_T tOperation;
                    tOperation.nSnsID = nSnsID;
                    tOperation.nGroup = 0;
                    tOperation.nChannel = m_mapSns2VideoAttr[nSnsID][i].nUniChn;
                    tOperation.eReqType = E_REQ_TYPE_VIDEO;
                    tOperation.SetOperaType(E_WEB_OPERATION_TYPE_CHANNEL_SWITCH);
                    tOperation.tChnSwitch.bOn = bEnable;
                    tOperation.tChnSwitch.nEncoderType = nEncoderType;

                    vecWebOpr.emplace_back(tOperation);
                    LOG_MM_C(WEB_OPTION, "[%d,%d] vencChn:%d enable stream changed: [%d] => [%d]", nSnsID, i, tOperation.nChannel,
                             tOldAttr.bEnable, bEnable);

                    m_mapSns2VideoAttr[nSnsID][i].bEnable = bEnable;
                }

                cchar* cMin_qp = mprReadJson(jsonObj, "min_qp");
                cchar* cMax_qp = mprReadJson(jsonObj, "max_qp");
                cchar* cMin_iqp = mprReadJson(jsonObj, "min_iqp");
                cchar* cMax_iqp = mprReadJson(jsonObj, "max_iqp");
                cchar* cMin_iprop = mprReadJson(jsonObj, "min_iprop");
                cchar* cMax_iprop = mprReadJson(jsonObj, "max_iprop");
                cchar* cBitrate = mprReadJson(jsonObj, "bit_rate");

                if (nullptr != cMin_qp && nullptr != cMax_qp && nullptr != cMin_iqp && nullptr != cMax_iqp && nullptr != cMin_iprop &&
                    nullptr != cMax_iprop && nullptr != cBitrate) {
                    AX_U32 nRcType = atoi(mprReadJson(jsonObj, "rc_type"));
                    AX_U32 nMinQp = (AX_U32)atoi(cMin_qp);
                    AX_U32 nMaxQp = (AX_U32)atoi(cMax_qp);
                    AX_U32 nMinIQp = (AX_U32)atoi(cMin_iqp);
                    AX_U32 nMaxIQp = (AX_U32)atoi(cMax_iqp);
                    AX_U32 nMinIProp = (AX_U32)atoi(cMin_iprop);
                    AX_U32 nMaxIProp = (AX_U32)atoi(cMax_iprop);
                    AX_U32 nBitrate = (AX_U32)atoi(cBitrate);
                    RC_INFO_T rOldRcInfo;
                    tOldAttr.GetEncRcCfg(rOldRcInfo);
                    if (nRcType != tOldAttr.nRcType || nMinQp != rOldRcInfo.nMinQp || nMaxQp != rOldRcInfo.nMaxQp ||
                        nMinIQp != rOldRcInfo.nMinIQp || nMaxIQp != rOldRcInfo.nMaxIQp || nMinIProp != rOldRcInfo.nMinIProp ||
                        nMaxIProp != rOldRcInfo.nMaxIProp) {
                        WEB_REQ_OPERATION_T tOperation;
                        tOperation.nSnsID = nSnsID;
                        tOperation.nGroup = 0;
                        tOperation.nChannel = m_mapSns2VideoAttr[nSnsID][i].nUniChn;
                        tOperation.eReqType = E_REQ_TYPE_VIDEO;
                        tOperation.SetOperaType(E_WEB_OPERATION_TYPE_RC_INFO);
                        tOperation.tRcInfo.eRcType = CAXTypeConverter::FormatRcMode(nEncoderType, nRcType);
                        tOperation.tRcInfo.nMinQp = nMinQp;
                        tOperation.tRcInfo.nMaxQp = nMaxQp;
                        tOperation.tRcInfo.nMinIQp = nMinIQp;
                        tOperation.tRcInfo.nMaxIQp = nMaxIQp;
                        tOperation.tRcInfo.nMaxIProp = nMaxIProp;
                        tOperation.tRcInfo.nMinIProp = nMinIProp;
                        tOperation.tRcInfo.nBitrate = nBitrate;
                        tOperation.tRcInfo.nEncoderType = nEncoderType;

                        vecWebOpr.emplace_back(tOperation);
                        LOG_MM_C(WEB_OPTION, "[%d,%d] rc info changed: [%d- %d %d %d %d %d %d] => [%d- %d %d %d %d %d %d]", nSnsID, i,
                                 tOldAttr.nRcType, rOldRcInfo.nMinQp, rOldRcInfo.nMaxQp, rOldRcInfo.nMinIQp, rOldRcInfo.nMaxIQp,
                                 rOldRcInfo.nMinIProp, rOldRcInfo.nMaxIProp, nRcType, nMinQp, nMaxQp, nMinIQp, nMaxIQp, nMinIProp,
                                 nMaxIProp);

                        RC_INFO_T rRcInfo;
                        rRcInfo.eRcType = tOperation.tRcInfo.eRcType;
                        rRcInfo.nMinQp = nMinQp;
                        rRcInfo.nMaxQp = nMaxQp;
                        rRcInfo.nMinIQp = nMinIQp;
                        rRcInfo.nMaxIQp = nMaxIQp;
                        rRcInfo.nMinIProp = nMinIProp;
                        rRcInfo.nMaxIProp = nMaxIProp;

                        m_mapSns2VideoAttr[nSnsID][i].SetEncRcCfg(rRcInfo);
                        m_mapSns2VideoAttr[nSnsID][i].nRcType = nRcType;
                    }
                }
            }

            break;
        }
        case E_REQ_TYPE_AI: {
            WEB_SET_INT(nSnsID, pJson, "src_id");
            if (m_mapSns2AiAttr.find(nSnsID) == m_mapSns2AiAttr.end()) {
                LOG_M_E(WEB_OPTION, "AI settings for sensor: %d not configured.");
                return AX_FALSE;
            }

            LOG_MM_C(WEB_OPTION, "[AI] Web request: sensor %d", nSnsID);

            AX_BOOL bAiEnable = AX_FALSE;
            MprJson* pJsonAI = mprReadJsonObj(pJson, "ai_attr");
            WEB_SET_BOOL(bAiEnable, pJsonAI, "ai_enable");
            if (bAiEnable != m_mapSns2AiAttr[nSnsID].bEnable) {
                WEB_REQ_OPERATION_T tOperation;
                tOperation.nSnsID = nSnsID; /* Enable attribute only controls the sensor level's osd's display */
                tOperation.nChannel = -1;
                tOperation.eReqType = E_REQ_TYPE_AI;
                tOperation.SetOperaType(E_WEB_OPERATION_TYPE_AI_ENABLE);
                tOperation.tAiEnable.bOn = bAiEnable;

                vecWebOpr.emplace_back(tOperation);
                LOG_MM_C(WEB_OPTION, "[%d] ai_enable changed: [%d] => [ %d]", nSnsID, m_mapSns2AiAttr[nSnsID].bEnable, bAiEnable);
                m_mapSns2AiAttr[nSnsID].bEnable = bAiEnable;
            }

            MprJson* pJsonPushStategy = mprReadJsonObj(pJsonAI, "push_strategy");
            if (pJsonPushStategy) {
                WEB_REQ_OPERATION_T tOperation;
                tOperation.nGroup = nSnsID; /* Enable attribute only controls the sensor level's osd's display */
                tOperation.nSnsID = nSnsID;
                tOperation.nChannel = -1;
                tOperation.eReqType = E_REQ_TYPE_AI;
                tOperation.SetOperaType(E_WEB_OPERATION_TYPE_AI_PUSH_MODE);
                cchar* szPushMode = mprReadJson(pJsonPushStategy, "push_mode");
                if (szPushMode) {
                    std::string strPushMode = szPushMode;
                    tOperation.tAiPushStategy.ePushMode = ParseResAiStr(strPushMode);
                    WEB_SET_INT(tOperation.tAiPushStategy.nPushCounts, pJsonPushStategy, "push_count");
                    WEB_SET_INT(tOperation.tAiPushStategy.nPushIntervalMs, pJsonPushStategy, "push_interval");

                    if (m_mapSns2AiAttr[nSnsID].tPushStategy.ePushMode != tOperation.tAiPushStategy.ePushMode ||
                        m_mapSns2AiAttr[nSnsID].tPushStategy.nPushCounts != tOperation.tAiPushStategy.nPushCounts ||
                        m_mapSns2AiAttr[nSnsID].tPushStategy.nPushIntervalMs != tOperation.tAiPushStategy.nPushIntervalMs) {

                        vecWebOpr.emplace_back(tOperation);
                        LOG_MM_C(WEB_OPTION, "[%d] push_strategy changed: [%d %d %d] => [%d %d %d]", nSnsID,
                                m_mapSns2AiAttr[nSnsID].tPushStategy.ePushMode, m_mapSns2AiAttr[nSnsID].tPushStategy.nPushCounts,
                                m_mapSns2AiAttr[nSnsID].tPushStategy.nPushIntervalMs, tOperation.tAiPushStategy.ePushMode,
                                tOperation.tAiPushStategy.nPushCounts, tOperation.tAiPushStategy.nPushIntervalMs);

                        m_mapSns2AiAttr[nSnsID].tPushStategy.ePushMode = tOperation.tAiPushStategy.ePushMode;
                        m_mapSns2AiAttr[nSnsID].tPushStategy.nPushCounts = tOperation.tAiPushStategy.nPushCounts;
                        m_mapSns2AiAttr[nSnsID].tPushStategy.nPushIntervalMs = tOperation.tAiPushStategy.nPushIntervalMs;
                    }
                }
            }

            MprJson* pJsonEvents = mprReadJsonObj(pJsonAI, "events");
            if (pJsonEvents) {
                WEB_REQ_OPERATION_T tOperation{};
                tOperation.nSnsID = nSnsID;
                tOperation.nChannel = -1;
                tOperation.eReqType = E_REQ_TYPE_AI;
                tOperation.SetOperaType(E_WEB_OPERATION_TYPE_AI_EVENT);
                MprJson* pJsonMotion = mprReadJsonObj(pJsonEvents, "motion_detect");
                WEB_SET_BOOL(tOperation.tEvent.tMD.bEnable, pJsonMotion, "enable");
                WEB_SET_INT(tOperation.tEvent.tMD.nThrsHoldY, pJsonMotion, "threshold_y");
                WEB_SET_INT(tOperation.tEvent.tMD.nConfidence, pJsonMotion, "confidence");

                MprJson* pJsonOcclusion = mprReadJsonObj(pJsonEvents, "occlusion_detect");
                WEB_SET_BOOL(tOperation.tEvent.tOD.bEnable, pJsonOcclusion, "enable");
                WEB_SET_INT(tOperation.tEvent.tOD.nThrsHoldY, pJsonOcclusion, "threshold_y");
                WEB_SET_INT(tOperation.tEvent.tOD.nConfidence, pJsonOcclusion, "confidence");

                MprJson* pJsonSceneChange = mprReadJsonObj(pJsonEvents, "scene_change_detect");
                WEB_SET_BOOL(tOperation.tEvent.tSCD.bEnable, pJsonSceneChange, "enable");
                WEB_SET_INT(tOperation.tEvent.tSCD.nThrsHoldY, pJsonSceneChange, "threshold_y");
                WEB_SET_INT(tOperation.tEvent.tSCD.nConfidence, pJsonSceneChange, "confidence");

                if (m_mapSns2AiAttr[nSnsID].tEvents.tMD.bEnable != tOperation.tEvent.tMD.bEnable ||
                    m_mapSns2AiAttr[nSnsID].tEvents.tMD.nThrsHoldY != tOperation.tEvent.tMD.nThrsHoldY ||
                    m_mapSns2AiAttr[nSnsID].tEvents.tMD.nConfidence != tOperation.tEvent.tMD.nConfidence ||
                    m_mapSns2AiAttr[nSnsID].tEvents.tOD.bEnable != tOperation.tEvent.tOD.bEnable ||
                    m_mapSns2AiAttr[nSnsID].tEvents.tOD.nThrsHoldY != tOperation.tEvent.tOD.nThrsHoldY ||
                    m_mapSns2AiAttr[nSnsID].tEvents.tOD.nConfidence != tOperation.tEvent.tOD.nConfidence ||
                    m_mapSns2AiAttr[nSnsID].tEvents.tSCD.bEnable != tOperation.tEvent.tSCD.bEnable ||
                    m_mapSns2AiAttr[nSnsID].tEvents.tSCD.nThrsHoldY != tOperation.tEvent.tSCD.nThrsHoldY ||
                    m_mapSns2AiAttr[nSnsID].tEvents.tSCD.nConfidence != tOperation.tEvent.tSCD.nConfidence ) {

                    vecWebOpr.emplace_back(tOperation);
                    LOG_MM_C(WEB_OPTION,
                            "[%d] ai events changed: MD[%d %d %d] => [%d %d %d], OD[%d %d %d] => [%d %d %d], SCD[%d %d %d] => [%d %d %d]",
                            nSnsID, m_mapSns2AiAttr[nSnsID].tEvents.tMD.bEnable, m_mapSns2AiAttr[nSnsID].tEvents.tMD.nThrsHoldY,
                            m_mapSns2AiAttr[nSnsID].tEvents.tMD.nConfidence, tOperation.tEvent.tMD.bEnable, tOperation.tEvent.tMD.nThrsHoldY,
                            tOperation.tEvent.tMD.nConfidence, m_mapSns2AiAttr[nSnsID].tEvents.tOD.bEnable,
                            m_mapSns2AiAttr[nSnsID].tEvents.tOD.nThrsHoldY, m_mapSns2AiAttr[nSnsID].tEvents.tOD.nConfidence,
                            tOperation.tEvent.tOD.bEnable, tOperation.tEvent.tOD.nThrsHoldY, tOperation.tEvent.tOD.nConfidence,
                            m_mapSns2AiAttr[nSnsID].tEvents.tSCD.bEnable, m_mapSns2AiAttr[nSnsID].tEvents.tSCD.nThrsHoldY,
                            m_mapSns2AiAttr[nSnsID].tEvents.tSCD.nConfidence, tOperation.tEvent.tSCD.bEnable,
                            tOperation.tEvent.tSCD.nThrsHoldY, tOperation.tEvent.tSCD.nConfidence);

                    m_mapSns2AiAttr[nSnsID].tEvents.tMD.bEnable = tOperation.tEvent.tMD.bEnable;
                    m_mapSns2AiAttr[nSnsID].tEvents.tMD.nThrsHoldY = tOperation.tEvent.tMD.nThrsHoldY;
                    m_mapSns2AiAttr[nSnsID].tEvents.tMD.nConfidence = tOperation.tEvent.tMD.nConfidence;

                    m_mapSns2AiAttr[nSnsID].tEvents.tOD.bEnable = tOperation.tEvent.tOD.bEnable;
                    m_mapSns2AiAttr[nSnsID].tEvents.tOD.nThrsHoldY = tOperation.tEvent.tOD.nThrsHoldY;
                    m_mapSns2AiAttr[nSnsID].tEvents.tOD.nConfidence = tOperation.tEvent.tOD.nConfidence;

                    m_mapSns2AiAttr[nSnsID].tEvents.tSCD.bEnable = tOperation.tEvent.tSCD.bEnable;
                    m_mapSns2AiAttr[nSnsID].tEvents.tSCD.nThrsHoldY = tOperation.tEvent.tSCD.nThrsHoldY;
                    m_mapSns2AiAttr[nSnsID].tEvents.tSCD.nConfidence = tOperation.tEvent.tSCD.nConfidence;
                }
            }
            break;
        }

        case E_REQ_TYPE_OSD: {
            WEB_SET_INT(nSnsID, pJson, "src_id");
            AX_S32 nType = 0;
            auto it = m_mapSns2PrivacyConfig.find(nSnsID);
            if (it != m_mapSns2PrivacyConfig.end()) {
                for (auto& grpCfgIt : it->second) {
                    MprJson* jsonPrivacy = mprReadJsonObj(pJson, "privacy_attr");
                    if (jsonPrivacy) {
                        OSD_CFG_T tOsdConfig = grpCfgIt.second;
                        WEB_SET_BOOL(tOsdConfig.bEnable, jsonPrivacy, "enable");
                        WEB_SET_HEX(tOsdConfig.tPrivacyAttr.nColor, jsonPrivacy, "color");
                        WEB_SET_INT(nType, jsonPrivacy, "type");
                        tOsdConfig.tPrivacyAttr.eType = (OSD_PRIVACY_TYPE_E)nType;
                        WEB_SET_INT(tOsdConfig.tPrivacyAttr.nLineWidth, jsonPrivacy, "linewidth");
                        WEB_SET_BOOL(tOsdConfig.tPrivacyAttr.bSolid, jsonPrivacy, "solid");
                        MprJson* jsonPoints = mprReadJsonObj(jsonPrivacy, "points");
                        WEB_SET_INT_OBJ(tOsdConfig.tPrivacyAttr.tPt[0].x, ReadJsonValeIn2DArry(jsonPoints, 0, 0));
                        WEB_SET_INT_OBJ(tOsdConfig.tPrivacyAttr.tPt[0].y, ReadJsonValeIn2DArry(jsonPoints, 0, 1));
                        WEB_SET_INT_OBJ(tOsdConfig.tPrivacyAttr.tPt[1].x, ReadJsonValeIn2DArry(jsonPoints, 1, 0));
                        WEB_SET_INT_OBJ(tOsdConfig.tPrivacyAttr.tPt[1].y, ReadJsonValeIn2DArry(jsonPoints, 1, 1));
                        WEB_SET_INT_OBJ(tOsdConfig.tPrivacyAttr.tPt[2].x, ReadJsonValeIn2DArry(jsonPoints, 2, 0));
                        WEB_SET_INT_OBJ(tOsdConfig.tPrivacyAttr.tPt[2].y, ReadJsonValeIn2DArry(jsonPoints, 2, 1));
                        WEB_SET_INT_OBJ(tOsdConfig.tPrivacyAttr.tPt[3].x, ReadJsonValeIn2DArry(jsonPoints, 3, 0));
                        WEB_SET_INT_OBJ(tOsdConfig.tPrivacyAttr.tPt[3].y, ReadJsonValeIn2DArry(jsonPoints, 3, 1));
                        MprJson* jsonResolution = mprReadJsonObj(jsonPrivacy, "sourceResolution");
                        WEB_SET_INT_OBJ(tOsdConfig.tPrivacyAttr.nBaseWidth, ReadJsonValeInArry(jsonResolution, 0));
                        WEB_SET_INT_OBJ(tOsdConfig.tPrivacyAttr.nBaseHeight, ReadJsonValeInArry(jsonResolution, 1));

                        AX_U32 nIvpsGrp = grpCfgIt.first;
                        if (!SetPrivacyConfig(nSnsID, nIvpsGrp, tOsdConfig)) {
                            LOG_MM_E(WEB_OPTION, "nIvpsGrp:%d SetPrivacyConfig Failed.", nIvpsGrp);
                            continue;
                        }

                        WEB_REQ_OPERATION_T tOperation;
                        tOperation.nSnsID = nSnsID; /* Enable attribute only controls the sensor level's osd's display */
                        tOperation.nGroup = nIvpsGrp;
                        tOperation.nChannel = -1;
                        tOperation.eReqType = E_REQ_TYPE_OSD;
                        tOperation.SetOperaType(E_WEB_OPERATION_TYPE_OSD_ATTR);
                        vecWebOpr.emplace_back(tOperation);
                    }
                }
            }

            if (m_mapSns2OsdConfig.find(nSnsID) == m_mapSns2OsdConfig.end()) {
                LOG_M_E(WEB_OPTION, "OSD settings for sensor: %d not configured.", nSnsID);
                return AX_FALSE;
            }

            MprJson* jsonOverly = mprReadJsonObj(pJson, "overlay_attr");
            MprJson* jsonPrivacy = mprReadJsonObj(pJson, "privacy_attr");
            for (AX_U8 nVideo = 0; nVideo < MAX_OSD_CHN_COUNT; nVideo++) {
                MprJson* jsonOverlyChn = ReadJsonValeInArry(jsonOverly, nVideo);
                if (jsonOverlyChn && jsonPrivacy) {
                    AX_U32 nIndex = 0;
                    AX_S32 nVideoW = 0;
                    AX_S32 nVideoH = 0;

                    MprJson* pJsonVideo = mprReadJsonObj(jsonOverlyChn, "video");
                    WEB_SET_INT(nIndex, pJsonVideo, "id");
                    WEB_SET_INT(nVideoW, pJsonVideo, "width");
                    WEB_SET_INT(nVideoH, pJsonVideo, "height");

                    std::pair<AX_U8, AX_U8> pairIndex;
                    if (!OverlayChnIndex2IvpsGrp(nSnsID, nIndex, pairIndex)) {
                        continue;
                    }
                    AX_U32 nIvpsGrp = pairIndex.first;
                    AX_U32 nIvpsChn = pairIndex.second;

                    std::vector<OSD_CFG_T> vecOsdCfg;
                    if (AX_FALSE == GetOsdConfig(nSnsID, nIvpsGrp, nIvpsChn, vecOsdCfg)) {
                        LOG_MM_E(WEB_OPTION, "nIvpsGrp:%d GetOsdConfig Failed.", nIvpsGrp);
                        return AX_FALSE;
                    }

                    for (AX_U32 i = 0; i < vecOsdCfg.size(); i++) {
                        OSD_CFG_T& tOsdConfig = vecOsdCfg[i];
                        OSD_TYPE_E eType = tOsdConfig.eType;
                        tOsdConfig.bChanged = AX_TRUE;
                        cchar * pText = nullptr;

                        switch (eType) {
                            case OSD_TYPE_TIME: {
                                MprJson* pJsonTime = mprReadJsonObj(jsonOverlyChn, "time");
                                WEB_SET_BOOL(tOsdConfig.bEnable, pJsonTime, "enable");
                                WEB_SET_HEX(tOsdConfig.tTimeAttr.nColor, pJsonTime, "color");
                                MprJson* jsonRect = mprReadJsonObj(pJsonTime, "rect");
                                WEB_SET_INT_OBJ(tOsdConfig.nBoundaryX, ReadJsonValeInArry(jsonRect, 0));
                                WEB_SET_INT_OBJ(tOsdConfig.nBoundaryY, ReadJsonValeInArry(jsonRect, 1));
                                tOsdConfig.nBoundaryX = OverlayBoudingX(nVideoW, tOsdConfig.nBoundaryW, tOsdConfig.nBoundaryX,
                                                                        tOsdConfig.eAlign, tOsdConfig.eType);
                                tOsdConfig.nBoundaryY = OverlayBoudingY(nVideoH, tOsdConfig.nBoundaryH, tOsdConfig.nBoundaryY,
                                                                        tOsdConfig.eAlign, tOsdConfig.eType);
                                break;
                            }
                            case OSD_TYPE_PICTURE: {
                                MprJson* pJsonLogo = mprReadJsonObj(jsonOverlyChn, "logo");
                                WEB_SET_BOOL(tOsdConfig.bEnable, pJsonLogo, "enable");
                                MprJson* jsonRect = mprReadJsonObj(pJsonLogo, "rect");
                                WEB_SET_INT_OBJ(tOsdConfig.nBoundaryX, ReadJsonValeInArry(jsonRect, 0));
                                WEB_SET_INT_OBJ(tOsdConfig.nBoundaryY, ReadJsonValeInArry(jsonRect, 1));
                                tOsdConfig.nBoundaryX = OverlayBoudingX(nVideoW, tOsdConfig.nBoundaryW, tOsdConfig.nBoundaryX,
                                                                        tOsdConfig.eAlign, tOsdConfig.eType);
                                tOsdConfig.nBoundaryY = OverlayBoudingY(nVideoH, tOsdConfig.nBoundaryH, tOsdConfig.nBoundaryY,
                                                                        tOsdConfig.eAlign, tOsdConfig.eType);

                                break;
                            }
                            case OSD_TYPE_STRING_CHANNEL:{
                                MprJson* pJsonChannel = mprReadJsonObj(jsonOverlyChn, "channel");
                                WEB_SET_BOOL(tOsdConfig.bEnable, pJsonChannel, "enable");
                                WEB_SET_HEX(tOsdConfig.tStrAttr.nColor, pJsonChannel, "color");
                                MprJson* jsonRect = mprReadJsonObj(pJsonChannel, "rect");
                                WEB_SET_INT_OBJ(tOsdConfig.nBoundaryX, ReadJsonValeInArry(jsonRect, 0));
                                WEB_SET_INT_OBJ(tOsdConfig.nBoundaryY, ReadJsonValeInArry(jsonRect, 1));
                                pText = mprReadJson(pJsonChannel, "text");
                                if (pText) {
                                    strcpy(tOsdConfig.tStrAttr.szStr, pText);
                                }

                                tOsdConfig.nBoundaryX = OverlayBoudingX(nVideoW, tOsdConfig.nBoundaryW, tOsdConfig.nBoundaryX,
                                                                        tOsdConfig.eAlign, tOsdConfig.eType);
                                tOsdConfig.nBoundaryY = OverlayBoudingY(nVideoH, tOsdConfig.nBoundaryH, tOsdConfig.nBoundaryY,
                                                                        tOsdConfig.eAlign, tOsdConfig.eType);
                                break;
                            }
                            case OSD_TYPE_STRING_LOCATION: {
                                MprJson* pJsonLocation = mprReadJsonObj(jsonOverlyChn, "location");
                                WEB_SET_BOOL(tOsdConfig.bEnable, pJsonLocation, "enable");
                                WEB_SET_HEX(tOsdConfig.tStrAttr.nColor, pJsonLocation, "color");
                                MprJson* jsonRect = mprReadJsonObj(pJsonLocation, "rect");
                                WEB_SET_INT_OBJ(tOsdConfig.nBoundaryX, ReadJsonValeInArry(jsonRect, 0));
                                WEB_SET_INT_OBJ(tOsdConfig.nBoundaryY, ReadJsonValeInArry(jsonRect, 1));
                                pText = mprReadJson(pJsonLocation, "text");
                                if (pText) {
                                    strcpy(tOsdConfig.tStrAttr.szStr, pText);
                                }

                                tOsdConfig.nBoundaryX = OverlayBoudingX(nVideoW, tOsdConfig.nBoundaryW, tOsdConfig.nBoundaryX,
                                                                        tOsdConfig.eAlign, tOsdConfig.eType);
                                tOsdConfig.nBoundaryY = OverlayBoudingY(nVideoH, tOsdConfig.nBoundaryH, tOsdConfig.nBoundaryY,
                                                                        tOsdConfig.eAlign, tOsdConfig.eType);
                                break;
                            }
                            case OSD_TYPE_RECT:
                                break;
                            default:
                                break;
                        }
                    }
                    if (AX_FALSE == SetOsdConfig(nSnsID, nIvpsGrp, nIvpsChn, vecOsdCfg)) {
                        LOG_MM_E(WEB_OPTION, "nIvpsGrp:%d SetOsdConfig Failed.", nIvpsGrp);
                        return AX_FALSE;
                    }

                    WEB_REQ_OPERATION_T tOperation;
                    tOperation.nSnsID = nSnsID; /* Enable attribute only controls the sensor level's osd's display */
                    tOperation.nGroup = nIvpsGrp;
                    tOperation.nChannel = -1;
                    tOperation.eReqType = E_REQ_TYPE_OSD;
                    tOperation.SetOperaType(E_WEB_OPERATION_TYPE_OSD_ATTR);
                    vecWebOpr.emplace_back(tOperation);
                }
            }
            break;
        }
        case E_REQ_TYPE_GET_SYSTEM_INFO: {
            WEB_REQ_OPERATION_T tOperation;
            tOperation.eReqType = E_REQ_TYPE_GET_SYSTEM_INFO;
            tOperation.SetOperaType(E_WEB_OPERATION_TYPE_GET_TITLE);
            vecWebOpr.emplace_back(tOperation);
            break;
        }
        case E_REQ_TYPE_GET_ASSIST_INFO: {
            AX_U8 nChnID = 0;
            WEB_SET_INT(nSnsID, pJson, "src_id");
            WEB_SET_INT(nChnID, pJson, "stream");
            WEB_REQ_OPERATION_T tOperation;
            tOperation.nSnsID = nSnsID;
            tOperation.nChannel = m_mapSns2VideoAttr[nSnsID][nChnID].nUniChn;
            tOperation.eReqType = E_REQ_TYPE_GET_ASSIST_INFO;
            tOperation.SetOperaType(E_WEB_OPERATION_TYPE_GET_RESOLUTION);

            vecWebOpr.emplace_back(tOperation);

            break;
        }
        case E_REQ_TYPE_CAPTURE: {
            AX_U8 nChnID = 0;
            WEB_SET_INT(nSnsID, pJson, "src_id");
            WEB_SET_INT(nChnID, pJson, "stream");
            WEB_REQ_OPERATION_T tOperation;
            tOperation.nSnsID = nSnsID;
            tOperation.nGroup = nSnsID;
            tOperation.nChannel = m_mapSns2VideoAttr[nSnsID][nChnID].nUniChn;
            tOperation.eReqType = E_REQ_TYPE_CAPTURE;
            tOperation.SetOperaType(E_WEB_OPERATION_TYPE_CAPTURE);
            vecWebOpr.emplace_back(tOperation);

            break;
        }
        case E_REQ_TYPE_EZOOM: {
            WEB_SET_INT(nSnsID, pJson, "src_id");
            AX_S32 nEZoomRatio = 0;
            AX_S32 nCenterOffsetX = 0;
            AX_S32 nCenterOffsetY = 0;
            MprJson* pJsonEzoom = mprReadJsonObj(pJson, "ezoom");
            WEB_SET_INT(nEZoomRatio, pJsonEzoom, "ezoom_ratio");
            WEB_SET_INT(nCenterOffsetX, pJsonEzoom, "center_offset_x");
            WEB_SET_INT(nCenterOffsetY, pJsonEzoom, "center_offset_y");
            WEB_REQ_OPERATION_T tOperation;
            tOperation.nSnsID = nSnsID;
            tOperation.nChannel = -1;
            tOperation.eReqType = E_REQ_TYPE_EZOOM;
            tOperation.SetOperaType(E_WEB_OPERATION_TYPE_EZOOM);
            tOperation.tEZoom.nEZoomRatio = nEZoomRatio;
            tOperation.tEZoom.nCenterOffsetX = nCenterOffsetX;
            tOperation.tEZoom.nCenterOffsetY = nCenterOffsetY;
            vecWebOpr.emplace_back(tOperation);
            break;
        }
        case E_REQ_TYPE_SWITCH_3A_SYNCRATIO: {
            AX_BOOL b3ASyncOn = AX_FALSE;
            WEB_SET_BOOL(b3ASyncOn, pJson, "sr3a");
            LOG_M_I(WEB_OPTION, "3A sync enable: %d", b3ASyncOn);

            WEB_REQ_OPERATION_T tOperation;
            tOperation.b3ASyncRationOn = b3ASyncOn;
            tOperation.SetOperaType(E_WEB_OPERATION_TYPE_SWITCH_3A_SYNCRATIO);
            vecWebOpr.emplace_back(tOperation);
            break;
        }
        case E_REQ_TYPE_RECORD: {
            WEB_SET_INT(nSnsID, pJson, "src_id");

            AX_U8 nChnNum = GetVideoCount(nSnsID);
            for (AX_U8 i = 0; i < nChnNum; i++) {
                AX_CHAR name[32] = {0};
                sprintf(name,"chn_attr[%d].enable", i);
                AX_BOOL bEnable = AX_FALSE;
                WEB_SET_BOOL(bEnable, pJson, name);

                memset(name, 0, sizeof(name));
                sprintf(name,"chn_attr[%d].record_type", i);
                AX_U8 nRecordType = 0;
                WEB_SET_INT(nRecordType, pJson, name);

                if (bEnable != m_mapSns2RecordAttr[nSnsID][i].bEnable
                    || nRecordType != m_mapSns2RecordAttr[nSnsID][i].nRecordType) {
                    WEB_REQ_OPERATION_T tOperation;
                    tOperation.nSnsID = nSnsID;
                    tOperation.nChannel = i;
                    tOperation.eReqType = E_REQ_TYPE_RECORD;
                    tOperation.SetOperaType(E_WEB_OPERATION_TYPE_RECORDER_SWITCH);
                    tOperation.tRecorderSwitch.bOn = bEnable;
                    tOperation.tRecorderSwitch.nRecordType = nRecordType;

                    vecWebOpr.emplace_back(tOperation);
                    LOG_MM_C(WEB_OPTION, "sns[%d]prevChn[%d] recorder changed: enable[%d] => [%d], recordType[%d] => [%d]",
                                          nSnsID, i,
                                          m_mapSns2RecordAttr[nSnsID][i].bEnable, bEnable,
                                          m_mapSns2RecordAttr[nSnsID][i].nRecordType, nRecordType);
                }
            }
            break;
        }
        default:
            LOG_MM_E(WEB_OPTION, "Invalid web request: unknown type(%d)", eReqType);
            return AX_FALSE;
    }

    return AX_TRUE;
}

AI_EVENTS_OPTION_T& CWebOptionHelper::GetAiEvent(AX_U8 nSnsID) {
    std::lock_guard<std::mutex> lck(m_mapSns2MtxAi[nSnsID]);
    return m_mapSns2AiAttr[nSnsID].tEvents;
}

AX_VOID CWebOptionHelper::SetAiEvent(AX_U8 nSnsID, AI_EVENTS_OPTION_T& tAiEvent) {
    std::lock_guard<std::mutex> lck(m_mapSns2MtxAi[nSnsID]);
    m_mapSns2AiAttr[nSnsID].tEvents = tAiEvent;
}

AX_BOOL CWebOptionHelper::CheckRequest(WEB_REQUEST_TYPE_E eReqType, const AX_VOID* pJsonReq) {
    if (nullptr == pJsonReq) {
        return AX_FALSE;
    }

    MprJson* pJson = (MprJson*)pJsonReq;
    switch (eReqType) {
        case E_REQ_TYPE_CAMERA: {
            // if (nullptr == mprReadJson(pJson, "camera_attr.framerate")
            //     || nullptr == mprReadJson(pJson, "camera_attr.sns_work_mode")
            //     || nullptr == mprReadJson(pJson, "camera_attr.rotation")
            //     || nullptr == mprReadJson(pJson, "camera_attr.daynight")
            //     || nullptr == mprReadJson(pJson, "camera_attr.nr_mode")
            //     || nullptr == mprReadJson(pJson, "camera_attr.framerate")) {
            //     // LOG_M_E(WEB, "Not well-formatted json data received for camera setting request.");
            //     return AX_FALSE;
            // }
            break;
        }
        case E_REQ_TYPE_VIDEO: {
            if (nullptr == mprReadJsonObj(pJson, "src_id")) {
                LOG_M_E(WEB_OPTION, "Not well-formatted json data received for video setting request.");
                return AX_FALSE;
            }
            break;
        }
        case E_REQ_TYPE_AI: {
            break;
        }
        case E_REQ_TYPE_GET_ASSIST_INFO: {
            break;
        }
        default:
            break;
    }

    return AX_TRUE;
}

AX_BOOL CWebOptionHelper::ParseResStr(const std::string& strResolution, AX_U32& nWidth, AX_U32& nHeight) {
    AX_U8 nCount = sscanf(strResolution.c_str(), "%dx%d", &nWidth, &nHeight);
    if (2 == nCount) {
        return AX_TRUE;
    }

    return AX_FALSE;
}

string CWebOptionHelper::GenResStr(AX_U32 nWidth, AX_U32 nHeight) {
    return to_string(nWidth) + "x" + to_string(nHeight);
}

E_AI_DETECT_PUSH_MODE_TYPE CWebOptionHelper::ParseResAiStr(string& strAiPushMode) {
    E_AI_DETECT_PUSH_MODE_TYPE mode;
    if (strAiPushMode == "FAST") {
        mode = E_AI_DETECT_PUSH_MODE_TYPE_FAST;
    } else if (strAiPushMode == "INTERVAL") {
        mode = E_AI_DETECT_PUSH_MODE_TYPE_INTERVAL;
    } else {
        mode = E_AI_DETECT_PUSH_MODE_TYPE_BEST;
    }

    return mode;
}

std::string CWebOptionHelper::GetPushModeStr(AX_S32 mode) {
    std::string strMode;
    switch (mode) {
        case E_AI_DETECT_PUSH_MODE_TYPE_FAST:
            strMode = "FAST";
            break;
        case E_AI_DETECT_PUSH_MODE_TYPE_INTERVAL:
            strMode = "INTERVAL";
            break;
        case E_AI_DETECT_PUSH_MODE_TYPE_BEST:
            strMode = "BEST_FRAME";
            break;
        default:
            strMode = "BEST_FRAME";
    }

    return strMode;
}

MprJson* CWebOptionHelper::GetOsdJson(AX_S32 nSnsID) {
    std::lock_guard<std::mutex> lck(m_mtxOverlay);
    AX_U32 nVideoCount = (AX_U32)m_pairVideo2Ivps.size();
    auto jsonOverlay = mprCreateJson(MPR_JSON_ARRAY);
    for (AX_U32 i = 0; i < nVideoCount; i++) {
        std::pair<AX_U8, AX_U8> pairOsd;
        if (!OverlayChnIndex2IvpsGrp(nSnsID, i, pairOsd)) {
            break;
        }
        AX_U32 nGrp = pairOsd.first;
        AX_U32 nChn = pairOsd.second;
        if (m_mapSns2VideoAttr[nSnsID].find(i) == m_mapSns2VideoAttr[nSnsID].end()) {
            break;
        }

        auto jsonOverlayItem = mprCreateJson(MPR_JSON_OBJ);
        AX_U32 nWidth = m_mapSns2VideoAttr[nSnsID][i].nWidth;
        AX_U32 nHeight = m_mapSns2VideoAttr[nSnsID][i].nHeight;
        AX_U8 nRotation = m_mapSns2CameraSetting[nSnsID].nRotation;
        if (AX_IVPS_ROTATION_90 == nRotation || AX_IVPS_ROTATION_270 == nRotation) {
            ::swap(nWidth, nHeight);
        }

        auto jsonVideo = mprCreateJson(MPR_JSON_OBJ);
        WRITE_JSON_INT(jsonVideo, "id", i);
        WRITE_JSON_INT(jsonVideo, "width", nWidth);
        WRITE_JSON_INT(jsonVideo, "height", nHeight);
        WRITE_JSON_OBJ(jsonOverlayItem, "video", jsonVideo);
        AX_U32 nSize = m_mapSns2OsdConfig[nSnsID].mapGrpChnConfig[nGrp][nChn].size();
        OSD_CFG_T tOsdConfig{};
        for (AX_U8 n = 0; n < nSize; n++) {
            if (OSD_TYPE_TIME == m_mapSns2OsdConfig[nSnsID].mapGrpChnConfig[nGrp][nChn][n].eType) {
                AX_U32 nFontSize = COSDStyle::GetInstance()->GetTimeFontSize(nWidth, nHeight);
                m_mapSns2OsdConfig[nSnsID].mapGrpChnConfig[nGrp][nChn][n].tTimeAttr.nFontSize = nFontSize;

                m_mapSns2OsdConfig[nSnsID].mapGrpChnConfig[nGrp][nChn][n].nBoundaryW = ALIGN_UP(nFontSize / 2, BASE_FONT_SIZE) * 20;
                m_mapSns2OsdConfig[nSnsID].mapGrpChnConfig[nGrp][nChn][n].nBoundaryH = ALIGN_UP(nFontSize, BASE_FONT_SIZE);
                tOsdConfig = m_mapSns2OsdConfig[nSnsID].mapGrpChnConfig[nGrp][nChn][n];
                break;
            }
        }
        AX_U32 nOffsetX = 0;
        if (AX_FALSE == tOsdConfig.bChanged) {
            nOffsetX = COSDStyle::GetInstance()->GetBoundaryX(nWidth, nHeight);
        } else {
            nOffsetX =
                CWebOptionHelper::OverlayOffsetX(nWidth, tOsdConfig.nBoundaryW, tOsdConfig.nBoundaryX, tOsdConfig.eAlign, tOsdConfig.eType);
        }
        AX_U32 nOffsetY = CWebOptionHelper::OverlayOffsetY(nHeight, tOsdConfig.nBoundaryH, tOsdConfig.nBoundaryY, tOsdConfig.eAlign, tOsdConfig.eType);
        WriteOSDStringAttrToJson(jsonOverlayItem, "time", tOsdConfig, nOffsetX, nOffsetY, AX_TRUE);

        for (AX_U8 n = 0; n < nSize; n++) {
            if (OSD_TYPE_PICTURE == m_mapSns2OsdConfig[nSnsID].mapGrpChnConfig[nGrp][nChn][n].eType) {
                tOsdConfig = m_mapSns2OsdConfig[nSnsID].mapGrpChnConfig[nGrp][nChn][n];
                break;
            }
        }

        auto jsonLogo = mprCreateJson(MPR_JSON_OBJ);
        WRITE_JSON_BOOL(jsonLogo, "enable", tOsdConfig.bEnable);
        WRITE_JSON_INT(jsonLogo, "align", tOsdConfig.eAlign);
        auto jsonRect = mprCreateJson(MPR_JSON_ARRAY);
        nOffsetX = CWebOptionHelper::OverlayOffsetX(nWidth, tOsdConfig.nBoundaryW, tOsdConfig.nBoundaryX, tOsdConfig.eAlign, tOsdConfig.eType);
        nOffsetY = CWebOptionHelper::OverlayOffsetY(nHeight, tOsdConfig.nBoundaryH, tOsdConfig.nBoundaryY, tOsdConfig.eAlign, tOsdConfig.eType);
        APPEND_JSON_2_INT(jsonRect, nOffsetX, nOffsetY);
        APPEND_JSON_2_INT(jsonRect, tOsdConfig.nBoundaryW, tOsdConfig.nBoundaryH);
        WRITE_JSON_OBJ(jsonLogo, "rect", jsonRect);
        WRITE_JSON_OBJ(jsonOverlayItem, "logo", jsonLogo);

        for (AX_U8 n = 0; n < nSize; n++) {
            if (OSD_TYPE_STRING_CHANNEL == m_mapSns2OsdConfig[nSnsID].mapGrpChnConfig[nGrp][nChn][n].eType) {
                tOsdConfig = m_mapSns2OsdConfig[nSnsID].mapGrpChnConfig[nGrp][nChn][n];
                break;
            }
        }

        nOffsetX = CWebOptionHelper::OverlayOffsetX(nWidth, tOsdConfig.nBoundaryW, tOsdConfig.nBoundaryX, tOsdConfig.eAlign, tOsdConfig.eType);
        nOffsetY = CWebOptionHelper::OverlayOffsetY(nHeight, tOsdConfig.nBoundaryH, tOsdConfig.nBoundaryY, tOsdConfig.eAlign, tOsdConfig.eType);
        WriteOSDStringAttrToJson(jsonOverlayItem, "channel", tOsdConfig, nOffsetX, nOffsetY);

        for (AX_U8 n = 0; n < nSize; n++) {
            if (OSD_TYPE_STRING_LOCATION == m_mapSns2OsdConfig[nSnsID].mapGrpChnConfig[nGrp][nChn][n].eType) {
                tOsdConfig = m_mapSns2OsdConfig[nSnsID].mapGrpChnConfig[nGrp][nChn][n];
                break;
            }
        }
        nOffsetX = CWebOptionHelper::OverlayOffsetX(nWidth, tOsdConfig.nBoundaryW, tOsdConfig.nBoundaryX, tOsdConfig.eAlign, tOsdConfig.eType);
        nOffsetY = CWebOptionHelper::OverlayOffsetY(nHeight, tOsdConfig.nBoundaryH, tOsdConfig.nBoundaryY, tOsdConfig.eAlign, tOsdConfig.eType);
        WriteOSDStringAttrToJson(jsonOverlayItem, "location", tOsdConfig, nOffsetX, nOffsetY);
        APPEND_JSON_OBJ(jsonOverlay, jsonOverlayItem);
    }

    return jsonOverlay;
}

MprJson* CWebOptionHelper::GetPrivacyJson(AX_S32 nSnsID) {
    MprJson* json = NULL;
    do {
        /* primary */
        std::vector<AX_U16> vecIvpsGrps = GetPrivacyIvpsGrps(nSnsID);
        if (0 == vecIvpsGrps.size()) {
            break;
        }

        // All grps in a sensor are sharing the same privacy config
        AX_U16 nGrp = vecIvpsGrps[0];
        OSD_CFG_T tOsdConfig = m_mapSns2PrivacyConfig[nSnsID][nGrp];
        AX_U32 nWidth = m_mapVideoMainRes[nSnsID].first;
        AX_U32 nHeight = m_mapVideoMainRes[nSnsID].second;
        if (1 == m_mapSns2CameraSetting[nSnsID].nRotation || 3 == m_mapSns2CameraSetting[nSnsID].nRotation) {
            std::swap(nWidth, nHeight);
        }
        json = mprCreateJson(MPR_JSON_OBJ);
        WRITE_JSON_BOOL(json, "enable", tOsdConfig.bEnable);
        WRITE_JSON_INT(json, "type", tOsdConfig.tPrivacyAttr.eType);
        WRITE_JSON_INT(json, "linewidth", tOsdConfig.tPrivacyAttr.nLineWidth);
        WRITE_JSON_HEX(json, "color", "0x%06X", tOsdConfig.tPrivacyAttr.nColor & 0xFFFFFF);
        WRITE_JSON_BOOL(json, "solid", tOsdConfig.tPrivacyAttr.bSolid);

        auto jsonPointArray = mprCreateJson(MPR_JSON_ARRAY);
        auto jsonPoint = mprCreateJson(MPR_JSON_ARRAY);
        APPEND_JSON_2_INT(jsonPoint, tOsdConfig.tPrivacyAttr.tPt[0].x, tOsdConfig.tPrivacyAttr.tPt[0].y);
        APPEND_JSON_OBJ(jsonPointArray, jsonPoint);
        jsonPoint = mprCreateJson(MPR_JSON_ARRAY);
        APPEND_JSON_2_INT(jsonPoint, tOsdConfig.tPrivacyAttr.tPt[1].x, tOsdConfig.tPrivacyAttr.tPt[1].y);
        APPEND_JSON_OBJ(jsonPointArray, jsonPoint);
        jsonPoint = mprCreateJson(MPR_JSON_ARRAY);
        APPEND_JSON_2_INT(jsonPoint, tOsdConfig.tPrivacyAttr.tPt[2].x, tOsdConfig.tPrivacyAttr.tPt[2].y);
        APPEND_JSON_OBJ(jsonPointArray, jsonPoint);
        jsonPoint = mprCreateJson(MPR_JSON_ARRAY);
        APPEND_JSON_2_INT(jsonPoint, tOsdConfig.tPrivacyAttr.tPt[3].x, tOsdConfig.tPrivacyAttr.tPt[3].y);
        APPEND_JSON_OBJ(jsonPointArray, jsonPoint);
        WRITE_JSON_OBJ(json, "points", jsonPointArray);
        auto jsonResolution = mprCreateJson(MPR_JSON_ARRAY);
        APPEND_JSON_2_INT(jsonResolution, nWidth, nHeight);
        WRITE_JSON_OBJ(json, "sourceResolution", jsonResolution);

    } while (0);

    return json;
}

AX_S32 CWebOptionHelper::OverlayOffsetX(AX_S32 nWidth, AX_S32 nOsdWidth, AX_S32 nXMargin, OSD_ALIGN_TYPE_E eAlign, OSD_TYPE_E eType) {
    AX_S32 Offset = 0;
    if (OSD_ALIGN_TYPE_LEFT_TOP == eAlign || OSD_ALIGN_TYPE_LEFT_BOTTOM == eAlign) {
        if (nWidth < nOsdWidth) {
            Offset = nXMargin;
        } else {
            if (nWidth - nOsdWidth > nXMargin) {
                Offset = nXMargin;
            } else {
                Offset = nWidth - nOsdWidth;
            }
        }
        Offset = ALIGN_UP(Offset, OSD_ALIGN_X_OFFSET);
        if (eType == OSD_TYPE_STRING_LOCATION || eType == OSD_TYPE_STRING_CHANNEL) {
            Offset = (Offset / OSD_ALIGN_X_OFFSET - 1) * OSD_ALIGN_X_OFFSET;
        }

    } else if (OSD_ALIGN_TYPE_RIGHT_TOP == eAlign || OSD_ALIGN_TYPE_RIGHT_BOTTOM == eAlign) {
        if (nWidth < nOsdWidth) {
            Offset = 0;
        } else {
            if (nWidth - nOsdWidth > nXMargin) {
                Offset = nWidth - (nOsdWidth + nXMargin) - (OSD_ALIGN_X_OFFSET - 1);
                if (Offset < 0) {
                    Offset = 0;
                }
                Offset = ALIGN_UP(Offset, OSD_ALIGN_X_OFFSET);
            } else {
                Offset = 0;
            }
        }
    }
    if (Offset < 0) {
        Offset = 0;
    }
    return Offset;
}

AX_S32 CWebOptionHelper::OverlayOffsetY(AX_S32 nHeight, AX_S32 nOsdHeight, AX_S32 nYMargin, OSD_ALIGN_TYPE_E eAlign, OSD_TYPE_E eType) {
    AX_S32 Offset = 0;
    if (OSD_ALIGN_TYPE_LEFT_TOP == eAlign || OSD_ALIGN_TYPE_RIGHT_TOP == eAlign) {
        if (nHeight < nOsdHeight) {
            Offset = nYMargin;
        } else {
            if (nHeight - nOsdHeight > nYMargin) {
                Offset = nYMargin;
            } else {
                Offset = nHeight - nOsdHeight;
            }
        }
        Offset = ALIGN_UP(Offset, OSD_ALIGN_Y_OFFSET);
    } else if (OSD_ALIGN_TYPE_LEFT_BOTTOM == eAlign || OSD_ALIGN_TYPE_RIGHT_BOTTOM == eAlign) {
        if (nHeight < nOsdHeight) {
            Offset = 0;
        } else {
            if (nHeight - nOsdHeight > nYMargin) {
                Offset = nHeight - (nOsdHeight + nYMargin) - (OSD_ALIGN_Y_OFFSET - 1);
                if (Offset < 0) {
                    Offset = 0;
                }
                Offset = ALIGN_UP(Offset, OSD_ALIGN_Y_OFFSET);
            } else {
                Offset = 0;
            }
        }
    }
    if (Offset < 0) {
        Offset = 0;
    }
    return Offset;
}

AX_S32 CWebOptionHelper::OverlayBoudingX(AX_S32 nWidth, AX_S32 nOsdWidth, AX_S32 nBoudingX, OSD_ALIGN_TYPE_E eAlign, OSD_TYPE_E eType) {
    AX_S32 x = 0;
    if (OSD_ALIGN_TYPE_LEFT_TOP == eAlign || OSD_ALIGN_TYPE_LEFT_BOTTOM == eAlign) {
        if (nWidth < nOsdWidth) {
            x = nBoudingX;
        } else {
            if (nWidth - nOsdWidth > nBoudingX) {
                x = nBoudingX;
            } else {
                x = nWidth - nOsdWidth;
            }
        }
    } else if (OSD_ALIGN_TYPE_RIGHT_TOP == eAlign || OSD_ALIGN_TYPE_RIGHT_BOTTOM == eAlign) {
        if (nWidth < nOsdWidth) {
            x = 0;
        } else {
            if (nWidth - nOsdWidth > nBoudingX) {
                x = nWidth - (nOsdWidth + nBoudingX);
            } else {
                x = 0;
            }
        }
    }
    if (x < 0) {
        x = 0;
    }
    return x;
}

AX_S32 CWebOptionHelper::OverlayBoudingY(AX_S32 nHeight, AX_S32 nOsdHeight, AX_S32 nBoudingY, OSD_ALIGN_TYPE_E eAlign, OSD_TYPE_E eType) {
    AX_S32 y = 0;
    if (OSD_ALIGN_TYPE_LEFT_TOP == eAlign || OSD_ALIGN_TYPE_RIGHT_TOP == eAlign) {
        if (nHeight < nOsdHeight) {
            y = nBoudingY;
        } else {
            if (nHeight - nOsdHeight > nBoudingY) {
                y = nBoudingY;
            } else {
                y = nHeight - nOsdHeight;
            }
        }
    } else if (OSD_ALIGN_TYPE_LEFT_BOTTOM == eAlign || OSD_ALIGN_TYPE_RIGHT_BOTTOM == eAlign) {
        if (nHeight < nOsdHeight) {
            y = 0;
        } else {
            if (nHeight - nOsdHeight > nBoudingY) {
                y = nHeight - (nOsdHeight + nBoudingY);
            } else {
                y = 0;
            }
        }
    }
    if (y < 0) {
        y = 0;
    }
    return y;
}

AX_VOID CWebOptionHelper::SetIvpsGrp2VideoIndex(std::pair<AX_U8, AX_U8> pairSnsVideoId, std::pair<AX_U8, AX_U8> pairIvpsGrpChn) {
    m_pairVideo2Ivps[pairSnsVideoId] = pairIvpsGrpChn;
}

AX_BOOL CWebOptionHelper::OverlayChnIndex2IvpsGrp(AX_S32 nSnsID, AX_U32 nIndex, std::pair<AX_U8, AX_U8>& ret) {
    pair<AX_U8, AX_U8> pairIvps = make_pair(nSnsID, nIndex);
    auto iter = m_pairVideo2Ivps.find(pairIvps);
    if (iter == m_pairVideo2Ivps.end()) {
        return AX_FALSE;
    }

    ret = iter->second;
    return AX_TRUE;
}

AX_BOOL CWebOptionHelper::SetRes2ResOption(SNS_TYPE_E eSnsType, AX_U32 nChnID, AX_U8 nIndex, AX_U32 nWidth, AX_U32 nHeight) {
    if (eSnsType >= E_SNS_TYPE_MAX) {
        LOG_MM_E(WEB_OPTION, "Invalid eSnsType: %d", eSnsType);
        return AX_FALSE;
    }

    string strResolution = to_string(nWidth) + "x" + to_string(nHeight);

    m_mapSnsType2ResOptions[eSnsType][nChnID][nIndex] = strResolution;

    return AX_TRUE;
}

std::vector<AX_U16> CWebOptionHelper::GetPrivacyIvpsGrps(AX_U8 nSnsID) {
    std::vector<AX_U16> vecIvpsGrps;

    auto it = m_mapSns2PrivacyConfig.find(nSnsID);
    if (it != m_mapSns2PrivacyConfig.end()) {
        for (auto& grpCfgIt : it->second) {
            vecIvpsGrps.push_back(grpCfgIt.first);
        }
    }

    return vecIvpsGrps;
}

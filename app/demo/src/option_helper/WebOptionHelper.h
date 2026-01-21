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

#include <iostream>
#include <map>
#include <mutex>
#include "AXSingleton.h"
#include "AXTypeConverter.hpp"
#include "ISensor.hpp"
#include "IVESStage.h"
#include "OSDHandler.h"
#include "picojson.h"
#include "WebServer.h"

#define MAX_VIDEO_ATTR_NUM 4
#define MAX_REGION_NUM (32)

struct MprJson;

typedef struct _WEB_CAMERA_ATTR_T {
    /* State */
    AX_U8 nSnsMode;
    AX_U8 nRotation;
    AX_F32 fFramerate;
    AX_U8 nDayNightMode;
    AX_U8 nNrMode;
    AX_U8 nShutterType;
    AX_U8 nDumpRawPipe;
    AX_BOOL bMirror;
    AX_BOOL bFlip;
    AX_BOOL bAutoCapture;
    AX_BOOL bEISOn;
    AX_BOOL bVSubChnOn;

    /* Capabilities */
    AX_BOOL bAutoCaptureEnable;
    AX_BOOL bSnapShotEnable;
    AX_BOOL bFlashEnable;
    AX_BOOL bSnsModeEnable;
    AX_BOOL bPNModeEnable;
    AX_BOOL bTriggerEnable;
    AX_BOOL bMirrorFlipEnable;
    AX_BOOL bRotationEnable;
    AX_BOOL bLdcEnable;
    AX_BOOL bEZoomEnable;
    AX_BOOL bEISEnable;
    AX_BOOL bSwitchVSubChnEnable;
    AX_BOOL bSwitchDumpRawPipeEnable;
    AX_BOOL bDisableCamSelect;
    AX_BOOL bDisableSnapshotButton;
    AX_BOOL bShutterOptEnable;
    WEB_OPR_IMAGE_ATTR_T tImageAttr;
    WEB_OPR_LDC_ATTR_T tLdcAttr;
    WEB_OPR_EZOOM_ATTR_T tEZoomAttr;
    WEB_OPR_FLASH_ATTR_T tFlashAttr[MAX_LANE_COUNT];
    _WEB_CAMERA_ATTR_T() {
        nSnsMode = 0;              /* 0: SDR; 1: HDR; 2: SDR/HDR Split */
        nRotation = 0;             /* 0:0°; 1:90°; 2:180°; 3:270° */
        fFramerate = 15.0;
        nDayNightMode = 0;         /* 0:Day mode; 1:Night Mode */
        nNrMode = 1;               /* 0:Dummy mode; 1:Nr Mode */
        nShutterType = 0;          /* 0:One shutter; 1: Two shutters; 2: Three shutters*/
        nDumpRawPipe = 0;
        bAutoCapture = AX_TRUE;
        bSnsModeEnable = AX_FALSE; /*Switch SDR/HDR */
        bPNModeEnable = AX_FALSE;  /* Switch sensor fps*/
        bAutoCaptureEnable = AX_TRUE;  /* Switch capture */
        bSnapShotEnable = AX_TRUE;  /* Switch manual capture */
        bFlashEnable = AX_FALSE;   /* Switch flash */
        bTriggerEnable = AX_FALSE; /* Trigger flash */
        bMirror = AX_FALSE;
        bFlip = AX_FALSE;
        bEISOn = AX_FALSE;
        bVSubChnOn = AX_TRUE;
        bMirrorFlipEnable = AX_FALSE;
        bRotationEnable = AX_FALSE;
        bLdcEnable = AX_FALSE;
        bEZoomEnable = AX_FALSE;
        bEISEnable = AX_FALSE; /*Switch EIS*/
        bDisableCamSelect = AX_FALSE;
        bDisableSnapshotButton = AX_FALSE;
        bShutterOptEnable = AX_FALSE; /* Option shutter */
        bSwitchVSubChnEnable = AX_FALSE;
        bSwitchDumpRawPipeEnable = AX_FALSE;
    }

} WEB_CAMERA_ATTR_T, *WEB_CAMERA_ATTR_PTR;

typedef struct _WEB_AUDIO_ATTR_T {
    AX_F32 fCapture_volume{0.00f};
    AX_F32 fPlay_volume{0.00f};
} WEB_AUDIO_ATTR_T, *WEB_AUDIO_ATTR_PTR;

typedef struct _WEB_VIDEO_ATTR_T {
    AX_U8 nUniChn;
    AX_BOOL bEnable;
    AX_U8 nEncoderType; /* 0: H264; 1: MJpeg 2: H265 */
    AX_U32 nBitrate;
    AX_U32 nWidth;
    AX_U32 nHeight;
    AX_BOOL bLink;
    APP_ENC_RC_CONFIG stEncodeCfg[APP_ENCODER_TYPE_MAX];
    AX_U8 nRcType;
    _WEB_VIDEO_ATTR_T() {
        nUniChn = 0;
        bEnable = AX_TRUE;
        bLink = AX_TRUE;
        nEncoderType = 0;
        nBitrate = 4096;
        nWidth = 0;
        nHeight = 0;
        nRcType = 0;
    }

    AX_BOOL IsSwitcherChanged(const _WEB_VIDEO_ATTR_T tAnother) {
        return (this->bEnable == tAnother.bEnable) ? AX_FALSE : AX_TRUE;
    }

    AX_BOOL IsResolutionChanged(const _WEB_VIDEO_ATTR_T tAnother) {
        return (this->nWidth == tAnother.nWidth && this->nHeight == tAnother.nHeight) ? AX_FALSE : AX_TRUE;
    }

    AX_BOOL IsAttrChanged(const _WEB_VIDEO_ATTR_T tAnother) {
        return (this->nEncoderType == tAnother.nEncoderType && this->nBitrate == tAnother.nBitrate && this->nWidth == tAnother.nWidth &&
                this->nHeight == tAnother.nHeight)
                   ? AX_FALSE
                   : AX_TRUE;
    }
    AX_VOID SetEncRcCfg(RC_INFO_T& tRcInfo) {
        for (int i = 0; i < APP_ENCODER_TYPE_MAX; i++) {
            if (nEncoderType == CAXTypeConverter::EncoderType2Int(stEncodeCfg[i].ePayloadType)) {
                stEncodeCfg[i].SetRcInfo(tRcInfo.eRcType, tRcInfo);
            }
        }
    }
    AX_VOID GetEncRcCfg(RC_INFO_T& tRcInfo) {
        for (int i = 0; i < APP_ENCODER_TYPE_MAX; i++) {
            if (nEncoderType == CAXTypeConverter::EncoderType2Int(stEncodeCfg[i].ePayloadType)) {
                stEncodeCfg[i].GetRcInfo(CAXTypeConverter::FormatRcMode(nEncoderType, nRcType), tRcInfo);
            }
        }
    }

} WEB_VIDEO_ATTR_T, *WEB_VIDEO_ATTR_PTR;

typedef struct _AI_ATTR_T {
    AX_BOOL bEnable{AX_FALSE};
    AI_PUSH_STATEGY_T tPushStategy;
    // AX_U16                  nDetectOnly;
    // AI_CONFIG_T             tConfig;
    // AI_DETECT_MODEL_TYPE_E  eDetectModel;
    AI_EVENTS_OPTION_T tEvents;
    // union {
    //     AI_MODEL_FALCEHUMAN_ATTR_T tHumanFaceSetting;
    //     AI_MODEL_HVCFP_ATTR_T      tHvcfpSetting;
    // };

    _AI_ATTR_T() {
        // tConfig.nWidth = DETECT_DEFAULT_WIDTH;
        // tConfig.nHeight = DETECT_DEFAULT_HEIGHT;
        // tConfig.nDetectFps = DETECT_DEFAULT_FRAMERATE_CTRL;
        // tConfig.nAiFps = DETECT_DEFAULT_AI_FRAMERATE_CTRL;
        // tConfig.nIvesFps = DETECT_DEFAULT_IVES_FRAMERATE_CTRL;
    }
} AI_ATTR_T, *AI_ATTR_PTR;

typedef struct _WEB_RECORD_ATTR_T {
    AX_BOOL bSupport{AX_TRUE};
    AX_BOOL bEnable{AX_FALSE};
    AX_U8   nRecordType{0};    // 0: MP4;  1: Raw stream
    AX_U8   nRawSteamType{0};  // 0: H264; 1: H265; 2: MJPG
} WEB_RECORD_ATTR_T, *WEB_RECORD_ATTR_PTR;

typedef struct _STATISTICS_INFO_T {
    AX_U64 nStartTick;
    AX_U32 nVencOutBytes;
    AX_F32 fBitrate;

    _STATISTICS_INFO_T() {
        memset(this, 0, sizeof(_STATISTICS_INFO_T));
    }
} STATISTICS_INFO_T, *STATISTICS_INFO_PTR;

typedef struct _WEB_OSD_CONFIG_T {
    OSD_TYPE_E eType;  // 0: picture, 1: string, 2: time, 3: rect
    AX_U32 nSleepMs;
    _WEB_OSD_CONFIG_T() {
        eType = OSD_TYPE_MAX;
        nSleepMs = 1000;
    }
} WEB_OSD_CONFIG_T;

typedef struct _WEB_AI_SENSOR_CONFIG_T {
    AX_BOOL bEnable;
} WEB_AI_SENSOR_CONFIG_T;

typedef struct _WEB_VIDEO_RESOLUTION_INFO {
    AX_U32 nMagic;
    string strResolution;
} WEB_VIDEO_RESOLUTION_INFO;

class CWebOptionHelper : public CAXSingleton<CWebOptionHelper> {
    friend class CAXSingleton<CWebOptionHelper>;

public:
    AX_VOID InitCameraAttr(AX_U8 nSnsID, AX_U8 nSnsType, const WEB_CAMERA_ATTR_T& tCameraAttr);
    AX_VOID InitVideoAttr(AX_U8 nSnsID, AX_U8 nChnID, const WEB_VIDEO_ATTR_T& tVideoAttr);
    AX_BOOL InitAiAttr(AX_U8 nSnsID);
    AX_VOID InitIvesAttr(AX_U8 nSnsID, CIVESStage* pIves);
    AX_BOOL GetCapSettingStr(AX_CHAR* pOutBuf, AX_U32 nSize);
    WEB_CAMERA_ATTR_T& GetCamera(AX_U8 nSnsID);
    AX_VOID SetCamera(AX_U8 nSnsID, const WEB_CAMERA_ATTR_T& tCamera);
    MprJson* GetCameraJson(AX_U8 nSnsID);
    AX_BOOL GetImageStr(AX_U8 nSnsID, AX_CHAR* pOutBuf, AX_U32 nSize);
    AX_BOOL GetLdcStr(AX_U8 nSnsID, AX_CHAR* pOutBuf, AX_U32 nSize);
    AX_U8 GetVideoCount(AX_U8 nSnsID);
    std::vector<WEB_VIDEO_ATTR_T> GetVideoUniChnVec(AX_U8 nSnsID);
    WEB_VIDEO_ATTR_T& GetVideo(AX_U8 nSnsID, AX_U32 nChnID);
    AX_VOID SetVideo(AX_U8 nSnsID, AX_U32 nChnID, const WEB_VIDEO_ATTR_T& tVideo);
    picojson::object GetVideoJsonObj(AX_U8 nSnsID, AX_U32 nChnID);
    picojson::object GetVideoAttrJsonObj(AX_U8 nSnsID, AX_U32 nChnID);
    WEB_RECORD_ATTR_T& GetRecordAttr(AX_U8 nSnsID, AX_U32 nChnID);
    MprJson* GetRecordJson(AX_U8 nSnsID);
    AX_VOID SetRecordAttr(AX_U8 nSnsID, AX_U32 nChnID, const WEB_RECORD_ATTR_T& tRecordAttr);
    MprJson* GetFlashAttrJson(AX_U8 nSnsID);
    AX_BOOL GetVideoByUniChn(AX_U8 nSnsID, AX_U32 nUniChn, WEB_VIDEO_ATTR_T& tVideoAttr);
    AX_VOID SetVideoByUniChn(AX_U8 nSnsID, const WEB_VIDEO_ATTR_T& tVideo);
    std::string GetFramerateOptStr(AX_U8 nSnsID);
    AX_BOOL GetVideoFramerateStr(AX_U8 nSnsID, AX_CHAR* pOutBuf, AX_U32 nBufferSize);
    AX_BOOL GetOsdConfig(AX_U8 nSnsId, AX_S32 nGroup, AX_S32 nChn, std::vector<OSD_CFG_T>& vec_osdCfg);
    AX_BOOL SetOsdConfig(AX_U8 nSnsId, AX_S32 nGroup, AX_S32 nChn, std::vector<OSD_CFG_T>& vec_osdCfg);
    MprJson* GetOsdJson(AX_S32 nSnsID);
    MprJson* GetPrivacyJson(AX_S32 nSnsID);
    AX_BOOL IsOsdSwitchOpen(AX_S32 nIvpsGroup);

    WEB_AUDIO_ATTR_T& GetAudio();
    AX_VOID SetAudio(const WEB_AUDIO_ATTR_T& tAudio);

    /* AI functions */
    AX_BOOL GetAiInfoStr(AX_U8 nSnsID, AX_CHAR* pOutBuf, AX_U32 nSize);
    std::string GetDetectModelStr();
    AX_BOOL GetPushStrgyStr(AX_U8 nSnsID, AX_CHAR* pOutBuf, AX_U32 nSize);
    std::string GetPushModeStr(AX_S32 mode);
    AX_BOOL GetDetectModelAttrStr(AX_CHAR* pOutBuf, AX_U32 nSize);
    AX_BOOL GetEventsStr(AX_U8 nSnsID, AX_CHAR* pOutBuf, AX_U32 nSize);
    E_AI_DETECT_PUSH_MODE_TYPE ParseResAiStr(std::string& strAiPushMode);

    AX_BOOL StatVencOutBytes(AX_U8 nSnsID, AX_U32 nUniChn, AX_U32 nBytes);
    AX_BOOL GetAssistBitrateStr(AX_U8 nSnsID, AX_U32 nUniChn, AX_CHAR* pOutBuf, AX_U32 nSize);

    AX_BOOL ParseWebRequest(WEB_REQUEST_TYPE_E eReqType, const AX_VOID* pJsonReq, std::vector<WEB_REQ_OPERATION_T>& vecWebOpr);

    AI_EVENTS_OPTION_T& GetAiEvent(AX_U8 nSnsID);
    AX_VOID SetAiEvent(AX_U8 nSnsID, AI_EVENTS_OPTION_T& tAiEvent);
    AX_VOID SetIvpsGrp2VideoIndex(std::pair<AX_U8, AX_U8> pairSnsVideoId, std::pair<AX_U8, AX_U8> pairIvpsGrpChn);

    AX_BOOL SetRes2ResOption(SNS_TYPE_E eSnsType, AX_U32 nChnID, AX_U8 nIndex, AX_U32 nWidth, AX_U32 nHeight);

    std::map<AX_U8, std::map<AX_U8, WEB_VIDEO_ATTR_T>>& GetSns2VideoAttr(AX_VOID) {
        return m_mapSns2VideoAttr;
    }

    std::map<AX_U8, std::map<AX_U8, std::vector<std::string>>>& GetSnsType2ResOptions(AX_VOID) {
        return m_mapSnsType2ResOptions;
    }

    AX_BOOL ParseResStr(const std::string& strResolution, AX_U32& nWidth, AX_U32& nHeight);

    AX_BOOL GetPrivacyConfig(AX_U8 nSnsID, AX_S32 nGroup, OSD_CFG_T& stOsdCfg);
    AX_BOOL SetPrivacyConfig(AX_U8 nSnsID, AX_S32 nGroup, const OSD_CFG_T& stOsdCfg);

    AX_BOOL SetResolutionOptions(AX_U8 nSnsID, AX_U32 nChnID, AX_U32 nMaxWidth, AX_U32 nMaxHeight);

private:
    CWebOptionHelper(AX_VOID);
    ~CWebOptionHelper(AX_VOID);

    virtual AX_BOOL InitOnce() override;

    AX_BOOL CheckRequest(WEB_REQUEST_TYPE_E eReqType, const AX_VOID* pJsonReq);
    std::vector<std::string> GetChnResolutionList(AX_U8 nSnsID, AX_U32 nChnID);
    std::string GenResStr(AX_U32 nWidth, AX_U32 nHeight);
    AX_S32 OverlayOffsetX(AX_S32 nWidth, AX_S32 nOsdWidth, AX_S32 nXMargin, OSD_ALIGN_TYPE_E eAlign, OSD_TYPE_E eType);
    AX_S32 OverlayOffsetY(AX_S32 nHeight, AX_S32 nOsdHeight, AX_S32 nYMargin, OSD_ALIGN_TYPE_E eAlign, OSD_TYPE_E eType);
    AX_S32 OverlayBoudingX(AX_S32 nWidth, AX_S32 nOsdWidth, AX_S32 nBoudingX, OSD_ALIGN_TYPE_E eAlign, OSD_TYPE_E eType);
    AX_S32 OverlayBoudingY(AX_S32 nHeight, AX_S32 nOsdHeight, AX_S32 nBoudingY, OSD_ALIGN_TYPE_E eAlign, OSD_TYPE_E eType);
    AX_BOOL OverlayChnIndex2IvpsGrp(AX_S32 nSnsID, AX_U32 nIndex, std::pair<AX_U8, AX_U8>& ret);
    std::vector<AX_U16> GetPrivacyIvpsGrps(AX_U8 nSnsID);

private:
    std::map<AX_U8, std::mutex> m_mapSns2MtxOption;
    std::map<AX_U8, std::mutex> m_mapSns2MtxAi;
    std::mutex m_mtxOverlay;
    std::mutex m_mtxChnStat;

    std::map<AX_U8, AX_U8> m_mapSnsID2Type;
    std::map<AX_U8, std::string> m_mapSns2FramerateOpt;
    // sensor id : camera attribute
    std::map<AX_U8, WEB_CAMERA_ATTR_T> m_mapSns2CameraSetting;
    // sensor id : <preview index : video attribute>
    std::map<AX_U8, std::map<AX_U8, WEB_VIDEO_ATTR_T>> m_mapSns2VideoAttr;
    std::map<AX_U8, AI_ATTR_T> m_mapSns2AiAttr;
    std::map<std::string, AX_U8> m_mapCapabilities;
    std::map<AX_U8, std::map<AX_U8, std::vector<std::string>>> m_mapSnsType2ResOptions;
    std::map<AX_U8, std::map<AX_U8, STATISTICS_INFO_T>> m_mapSns2ChnStatInfo;
    std::map<AX_U8, OSD_SENSOR_CONFIG_T> m_mapSns2OsdConfig;
    std::map<AX_U8, WEB_AUDIO_ATTR_T> m_mapSns2AudioAttr;
    // sensor id : <preview index : record attribute>
    std::map<AX_U8, std::map<AX_U8, WEB_RECORD_ATTR_T>> m_mapSns2RecordAttr;

    /*pair[sensorID, videoIndex] = pair[ivpsGrp, ivpsChn] */
    std::map<std::pair<AX_U8, AX_U8>, std::pair<AX_U8, AX_U8>> m_pairVideo2Ivps;
    std::map<AX_U8, std::map<AX_U8, OSD_CFG_T>> m_mapSns2PrivacyConfig;

    std::map<AX_U8, std::pair<AX_U32, AX_U32>> m_mapVideoMainRes;

    std::multimap<AX_U8, WEB_VIDEO_RESOLUTION_INFO> m_mmapComVideoResolutionList = {
            {0, {3840*2160, "3840x2160"}}, {0, {3072*2048, "3072x2048"}}, {0, {3072*1728, "3072x1728"}},
            {0, {2624*1944, "2624x1944"}}, {0, {2688*1520, "2688x1520"}}, {0, {2048*1536, "2048x1536"}},
            {0, {2304*1296, "2304x1296"}}, {0, {1920*1080, "1920x1080"}}, {0, {1280*960, "1280x960"}},
            {0, {1280*720, "1280x720"}},

            {1, {1920*1080, "1920x1080"}}, {1, {1280*720, "1280x720"}}, {1, {720*576, "720x576"}},
            {1, {640*480, "640x480"}},     {1, {384*288, "384x288"}},

            {2, {1920*1080, "1920x1080"}}, {2, {1280*720, "1280x720"}}, {2, {720*576, "720x576"}},
            {2, {640*480, "640x480"}},     {2, {384*288, "384x288"}}
    };

    std::multimap<AX_U8, WEB_VIDEO_RESOLUTION_INFO> m_mmapPano4MVideoResolutionList = {
            {0, {2560*768, "2560x768"}}, {0, {1280*364, "1280x364"}},

            {1, {2560*768, "2560x768"}}, {1, {1280*364, "1280x364"}},

            {2, {1280*364, "1280x364"}},
    };

    std::multimap<AX_U8, WEB_VIDEO_RESOLUTION_INFO> m_mmapSC410VideoResolutionList = {
            {0, {1744*2336, "1744x2336"}}, {0, {1280*720, "1280x720"}}, {0, {720*640, "720x640"}},

            {1, {1744*2336, "1744x2336"}}, {1, {1280*720, "1280x720"}}, {1, {720*640, "720x640"}},
    };
};

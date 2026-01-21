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
#include <string>
#include <cstring>
#include <stdlib.h>
#include <unistd.h>
#include <mutex>
#include "ax_global_type.h"
#include "mp4_api.h"

#define MP4_DEFAULT_RECORD_FILE_NUM  (10)
#define MP4_DEFAULT_RECORD_FILE_SIZE (128) //MB
#define MP4_DEFAULT_RECORD_PATH      "./"

typedef enum axMPEG4_SESS_MEDIA_TYPE_E {
    MPEG4_SESS_MEDIA_VIDEO,
    MPEG4_SESS_MEDIA_AUDIO,
    MPEG4_SESS_MEDIA_BUTT
} MPEG4_SESS_MEDIA_TYPE_E;

typedef struct MPEG4EC_VIDEO_CHN_INFO_S {
    AX_U8 nVideoChn;
    AX_U8 nPreviewChn;
    AX_U8 nSnsId;
    AX_BOOL bOn;
} MPEG4EC_VIDEO_CHN_INFO_T;

typedef struct MPEG4EC_TYPE_INFO_S {
    AX_U8 nRecordType{0};
    AX_U8 nVideoRawStreamType{0};
} MPEG4EC_TYPE_INFO_T;

typedef struct MPEG4EC_INFO_S {
    AX_U32 nMaxFileInMBytes;
    AX_U32 nMaxFileNum;
    AX_BOOL bLoop;
    mp4_record_type_e e_record_type;
    std::string strSavePath;

    MPEG4EC_VIDEO_CHN_INFO_T stVideoChnInfo;
    struct mpeg4_video_sess_attr {
        AX_BOOL bEnable;
        AX_PAYLOAD_TYPE_E ePt;
        AX_U32 nFrameRate;
        AX_S32 nfrWidth;
        AX_S32 nfrHeight;
        AX_S32 nBitrate;
        AX_U32 nMaxFrmSize;
    } stVideoAttr;
    struct mpeg4_audio_sess_attr {
        AX_BOOL bEnable;
        AX_PAYLOAD_TYPE_E ePt;
        AX_S32 nBitrate;
        AX_U32 nMaxFrmSize;
        AX_U32 nSampleRate;
        AX_U8 nChnCnt;
        AX_S32 nAOT; // audio object type
    } stAudioAttr;

    MPEG4EC_INFO_S() {
        nMaxFileInMBytes = MP4_DEFAULT_RECORD_FILE_SIZE;
        nMaxFileNum = MP4_DEFAULT_RECORD_FILE_NUM;
        bLoop = AX_TRUE;
        e_record_type = MP4_RECORD_TYPE_MP4;
        strSavePath = MP4_DEFAULT_RECORD_PATH;
        memset(&stVideoChnInfo, 0x00, sizeof(stVideoChnInfo));
        memset(&stVideoAttr, 0x00, sizeof(stVideoAttr));
        memset(&stAudioAttr, 0x00, sizeof(stAudioAttr));
    }
} MPEG4EC_INFO_T;

class CMPEG4Encoder {
public:
    CMPEG4Encoder();
    virtual ~CMPEG4Encoder();

    virtual AX_BOOL Init();
    virtual AX_BOOL DeInit();
    virtual AX_BOOL Start();
    virtual AX_BOOL Stop();

    AX_BOOL InitParam(const MPEG4EC_INFO_T& stMpeg4Info);
    AX_BOOL SendRawFrame(AX_U8 nChn, AX_VOID* data, AX_U32 size, AX_U64 nPts = 0, AX_BOOL bIFrame = AX_FALSE);
    AX_BOOL SendAudioFrame(AX_U8 nChn, AX_VOID* data, AX_U32 size, AX_U64 nPts = 0);

    AX_VOID StatusReport(const AX_CHAR *szFileName, mp4_status_e eStatus);

    AX_U8 GetVideoChn();
    AX_U8 GetPreviewChn();
    AX_U8 GetSnsId();
    MPEG4EC_TYPE_INFO_T GetRecordTypeInfo();
    AX_BOOL IsMp4EncoderOn();
    AX_BOOL IsMp4Recording();
    AX_BOOL IsNeedRestore();
    AX_BOOL IsSupport();

    AX_BOOL SwitchRecorder(AX_BOOL bOn,
                           mp4_record_type_e eRecordType = MP4_RECORD_TYPE_BUTT,
                           AX_BOOL bRestoreImmed = AX_FALSE);
    AX_VOID UpdateResolution(AX_U32 nWidth, AX_U32 nHeight);
    AX_VOID UpdateFrameRate(AX_U32 nFrameRate);
    AX_VOID UpdateBitRate(AX_U32 nBitRate);
    AX_BOOL UpdateVideoPayloadType(AX_PAYLOAD_TYPE_E ePayloadType);

    AX_BOOL Restore();

private:
    std::string GetRawSteamType();
    std::string GetRecordFileFormat();
    mp4_object_e GetVideoObjType(AX_PAYLOAD_TYPE_E ePayloadType);

public:
    MPEG4EC_VIDEO_CHN_INFO_T m_stMp4VideoChnInfo{0};
    mp4_info_t m_stMp4Info;
    MP4_HANDLE m_Mp4Handle{nullptr};
    std::string m_strVideoSavedPath{""};
    std::string m_strFileNamePrefix{""};
    AX_BOOL m_bRestore{AX_FALSE};
};

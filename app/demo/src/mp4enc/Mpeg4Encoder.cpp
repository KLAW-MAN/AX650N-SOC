/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include "Mpeg4Encoder.h"
#include "AppLogApi.h"

#define MPEG4 "MPEG4"

#define MP4_DEFAULT_VIDEO_DEPTH (5)
#define MP4_DEFAULT_AUDIO_DEPTH (5)

#define MP4_FORMAT_NAME     "mp4"
#define MP4_RAW_FORMAT_H264 "h264"
#define MP4_RAW_FORMAT_H265 "h265"
#define MP4_RAW_FORMAT_RAW  "raw"
#define MP4_RAW_STREAM_H264    "h264"
#define MP4_RAW_STREAM_H265    "h265"
#define MP4_RAW_STREAM_MJPG    "mjpg"
#define MP4_RAW_STREAM_UNKNOWN "unknown"

namespace {
// mp4 status callback
AX_VOID mpeg4_status_report_callback(MP4_HANDLE handle, const char *file_name, mp4_status_e status, void *user_data) {
    CMPEG4Encoder *inst = (CMPEG4Encoder *)user_data;

    if (inst) {
        inst->StatusReport(file_name, status);
    }
}
}

CMPEG4Encoder::CMPEG4Encoder() {
    memset(&m_stMp4Info, 0x00, sizeof(m_stMp4Info));
}

CMPEG4Encoder::~CMPEG4Encoder() {
}

AX_BOOL CMPEG4Encoder::Init() {
    return AX_TRUE;
}

AX_BOOL CMPEG4Encoder::Start() {
    if (m_stMp4VideoChnInfo.bOn) {
        if (m_Mp4Handle) {
            LOG_MM_W(MPEG4, "Stop old mp4 recorder before start!", m_stMp4VideoChnInfo.nVideoChn);
            Stop();
        }

        m_Mp4Handle = mp4_create(&m_stMp4Info);
        if (!m_Mp4Handle) {
            LOG_MM_E(MPEG4, "Create mp4 handle for venc(%d) failed!", m_stMp4VideoChnInfo.nVideoChn);
            return AX_FALSE;
        }
    }

    return AX_TRUE;
}

AX_BOOL CMPEG4Encoder::Stop() {
    if (m_Mp4Handle) {
        mp4_destroy(m_Mp4Handle);
        m_Mp4Handle = nullptr;
    }

    return AX_TRUE;
}

AX_BOOL CMPEG4Encoder::DeInit() {
    Stop();

    return AX_TRUE;
}

AX_BOOL CMPEG4Encoder::InitParam(const MPEG4EC_INFO_T& stMpeg4Info) {
    m_stMp4VideoChnInfo = stMpeg4Info.stVideoChnInfo;

    m_stMp4Info.loop = stMpeg4Info.bLoop;
    m_stMp4Info.max_file_size = (stMpeg4Info.nMaxFileInMBytes <= 0) ? MP4_DEFAULT_RECORD_FILE_SIZE : (int)stMpeg4Info.nMaxFileInMBytes;
    m_stMp4Info.max_file_num = (0 == stMpeg4Info.nMaxFileNum) ? MP4_DEFAULT_RECORD_FILE_NUM : stMpeg4Info.nMaxFileNum;
    m_strVideoSavedPath = stMpeg4Info.strSavePath;
    m_stMp4Info.dest_path = (char *)m_strVideoSavedPath.c_str();
    m_stMp4Info.e_record_type = stMpeg4Info.e_record_type;
    m_stMp4Info.user_data = this;

    m_strFileNamePrefix = "sns" + std::to_string(m_stMp4VideoChnInfo.nSnsId) + "_"
                           + "ch" + std::to_string(m_stMp4VideoChnInfo.nPreviewChn) + "_";
    m_stMp4Info.file_name_prefix = (char *)m_strFileNamePrefix.c_str();

    m_stMp4Info.sr_callback = mpeg4_status_report_callback;

    if (stMpeg4Info.stVideoAttr.bEnable) {
        m_stMp4Info.video.enable = true;
        m_stMp4Info.video.object = GetVideoObjType(stMpeg4Info.stVideoAttr.ePt);
        if (m_stMp4Info.video.object != MP4_OBJECT_AVC && m_stMp4Info.video.object != MP4_OBJECT_HEVC) {
            LOG_MM_E(MPEG4, "Don't support video payload type: %d currently!", stMpeg4Info.stVideoAttr.ePt);
            return AX_FALSE;
        }
        m_stMp4Info.video.framerate = (int)stMpeg4Info.stVideoAttr.nFrameRate;
        m_stMp4Info.video.bitrate = (int)stMpeg4Info.stVideoAttr.nBitrate;
        m_stMp4Info.video.width = (int)stMpeg4Info.stVideoAttr.nfrWidth;
        m_stMp4Info.video.height = (int)stMpeg4Info.stVideoAttr.nfrHeight;
        m_stMp4Info.video.depth = MP4_DEFAULT_VIDEO_DEPTH;
        m_stMp4Info.video.max_frm_size = stMpeg4Info.stVideoAttr.nMaxFrmSize;
    }

    if (stMpeg4Info.stAudioAttr.bEnable) {
        m_stMp4Info.audio.enable = true;
        if (stMpeg4Info.stAudioAttr.ePt == PT_G711A) {
            m_stMp4Info.audio.object = MP4_OBJECT_ALAW;
        }
        else if (stMpeg4Info.stAudioAttr.ePt == PT_G711U) {
            m_stMp4Info.audio.object = MP4_OBJECT_ULAW;
        }
        else if (stMpeg4Info.stAudioAttr.ePt == PT_AAC) {
            m_stMp4Info.audio.object = MP4_OBJECT_AAC;
        }
        else {
            LOG_MM_E(MPEG4, "Don't support audio payload type: %d currently!", stMpeg4Info.stAudioAttr.ePt);
            return AX_FALSE;
        }

        m_stMp4Info.audio.samplerate = (int)stMpeg4Info.stAudioAttr.nSampleRate;
        m_stMp4Info.audio.bitrate = (int)stMpeg4Info.stAudioAttr.nBitrate;
        m_stMp4Info.audio.chncnt = (int)stMpeg4Info.stAudioAttr.nChnCnt;
        m_stMp4Info.audio.aot = (int)stMpeg4Info.stAudioAttr.nAOT;
        m_stMp4Info.audio.depth = MP4_DEFAULT_AUDIO_DEPTH;
        m_stMp4Info.audio.max_frm_size = stMpeg4Info.stAudioAttr.nMaxFrmSize;
    }

    return AX_TRUE;
}

AX_BOOL CMPEG4Encoder::SendRawFrame(AX_U8 nChn, AX_VOID* data, AX_U32 size, AX_U64 nPts /*=0*/, AX_BOOL bIFrame /*=AX_FALSE*/) {
    if (m_Mp4Handle && (0 == mp4_send(m_Mp4Handle, MP4_DATA_VIDEO, data, size, nPts, (bool)bIFrame))) {
        return AX_TRUE;
    }

    return AX_FALSE;
}

AX_BOOL CMPEG4Encoder::SendAudioFrame(AX_U8 nChn, AX_VOID* data, AX_U32 size, AX_U64 nPts /*=0*/) {
    if (m_Mp4Handle && (0 == mp4_send(m_Mp4Handle, MP4_DATA_AUDIO, data, size, nPts, true))) {
        return AX_TRUE;
    }

    return AX_FALSE;
}

AX_VOID CMPEG4Encoder::StatusReport(const AX_CHAR *szFileName, mp4_status_e eStatus) {
    const AX_CHAR* status_str[MP4_STATUS_BUTT] = {"", "start", "complete", "deleted", "failure", "disk full"};

    if (eStatus == MP4_STATUS_FAILURE
        || eStatus == MP4_STATUS_DISK_FULL) {
        LOG_MM_E(MPEG4, "%s status: %s", szFileName ? szFileName: "", status_str[eStatus]);
    }
    else {
        LOG_MM_N(MPEG4, "%s status: %s", szFileName ? szFileName: "", status_str[eStatus]);
    }
}

AX_U8 CMPEG4Encoder::GetVideoChn() {
    return m_stMp4VideoChnInfo.nVideoChn;
}

AX_U8 CMPEG4Encoder::GetPreviewChn() {
    return m_stMp4VideoChnInfo.nPreviewChn;
}

AX_U8 CMPEG4Encoder::GetSnsId() {
    return m_stMp4VideoChnInfo.nSnsId;
}

AX_BOOL CMPEG4Encoder::IsMp4EncoderOn() {
    return m_stMp4VideoChnInfo.bOn;
}

AX_BOOL CMPEG4Encoder::IsMp4Recording() {
    return m_Mp4Handle ? AX_TRUE : AX_FALSE;
}

AX_BOOL CMPEG4Encoder::SwitchRecorder(AX_BOOL bOn,
                                      mp4_record_type_e eRecordType/*= MP4_RECORD_TYPE_BUTT*/,
                                      AX_BOOL bRestoreImmed/*= AX_FALSE*/) {
    if (eRecordType != MP4_RECORD_TYPE_BUTT) {
        m_stMp4Info.e_record_type = eRecordType;
    }

    if (bRestoreImmed) {
        if (bOn && (nullptr == m_Mp4Handle)) {
            m_Mp4Handle = mp4_create(&m_stMp4Info);
            if (!m_Mp4Handle) {
                LOG_MM_E(MPEG4, "Create mp4 handle for venc(%d) failed!", m_stMp4VideoChnInfo.nVideoChn);
                return AX_FALSE;
            }
        } else if (!bOn && m_Mp4Handle) {
            mp4_destroy(m_Mp4Handle);
            m_Mp4Handle = nullptr;
        }
    } else {
        Stop();
        m_bRestore = AX_TRUE;
    }


    m_stMp4VideoChnInfo.bOn = bOn;

    return AX_TRUE;
}

AX_VOID CMPEG4Encoder::UpdateResolution(AX_U32 nWidth, AX_U32 nHeight) {
    if (m_stMp4Info.video.width != (int)nWidth || m_stMp4Info.video.height != (int)nHeight) {
        Stop();

        m_stMp4Info.video.width = (int)nWidth;
        m_stMp4Info.video.height = (int)nHeight;

        m_bRestore = AX_TRUE;
    }
}

AX_VOID CMPEG4Encoder::UpdateFrameRate(AX_U32 nFrameRate) {
    if (m_stMp4Info.video.framerate != (int)nFrameRate) {
        Stop();

        m_stMp4Info.video.framerate = (int)nFrameRate;

        m_bRestore = AX_TRUE;
    }
}

AX_VOID CMPEG4Encoder::UpdateBitRate(AX_U32 nBitRate) {
    if (m_stMp4Info.video.bitrate != (int)nBitRate) {
        Stop();

        m_stMp4Info.video.bitrate = (int)nBitRate;

        m_bRestore = AX_TRUE;
    }
}

AX_BOOL CMPEG4Encoder::UpdateVideoPayloadType(AX_PAYLOAD_TYPE_E ePayloadType) {
    Stop();

    switch (ePayloadType) {
        case PT_H264:
            m_stMp4Info.video.object = MP4_OBJECT_AVC;
            break;
        case PT_H265:
            m_stMp4Info.video.object = MP4_OBJECT_HEVC;
            break;
        case PT_MJPEG:
            m_stMp4Info.video.object = MP4_OBJECT_MJPG;
            break;
        default:
            m_stMp4Info.video.object = MP4_OBJECT_BUTT;
            break;
    }

    m_bRestore = AX_TRUE;

    return AX_TRUE;
}

AX_BOOL CMPEG4Encoder::IsNeedRestore() {
    return m_bRestore;
}

AX_BOOL CMPEG4Encoder::Restore() {
    if (!m_bRestore) {
        return AX_TRUE;
    }

    m_bRestore = AX_FALSE;

    if (m_stMp4VideoChnInfo.bOn && (nullptr == m_Mp4Handle)) {
        if (m_stMp4Info.video.object != MP4_OBJECT_AVC && m_stMp4Info.video.object != MP4_OBJECT_HEVC) {
            LOG_MM_I(MPEG4, "[Warning] Only support h264/h265 stream, please check video out stream type for sns(%d) preview chn(%d)",
                            m_stMp4VideoChnInfo.nSnsId, m_stMp4VideoChnInfo.nPreviewChn);
            return AX_FALSE;
        }

        m_Mp4Handle = mp4_create(&m_stMp4Info);
        if (!m_Mp4Handle) {
            LOG_MM_E(MPEG4, "Create mp4 handle for venc(%d) failed!", m_stMp4VideoChnInfo.nVideoChn);
            return AX_FALSE;
        }
    }

    return AX_TRUE;
}

std::string CMPEG4Encoder::GetRawSteamType() {
    std::string strRawStreamType = "";
    switch (m_stMp4Info.video.object) {
        case MP4_OBJECT_AVC:
            strRawStreamType = MP4_RAW_STREAM_H264;
            break;
        case MP4_OBJECT_HEVC:
            strRawStreamType = MP4_RAW_STREAM_H265;
            break;
        case MP4_OBJECT_MJPG:
            strRawStreamType = MP4_RAW_STREAM_MJPG;
            break;
        default:
            strRawStreamType = MP4_RAW_STREAM_UNKNOWN;
            break;
    }

    return strRawStreamType;
}

std::string CMPEG4Encoder::GetRecordFileFormat() {
    std::string strFormat = "";
    switch (m_stMp4Info.video.object) {
        case MP4_OBJECT_AVC:
            strFormat = MP4_RAW_FORMAT_H264;
            break;
        case MP4_OBJECT_HEVC:
            strFormat = MP4_RAW_FORMAT_H265;
            break;
        case MP4_OBJECT_MJPG:
            strFormat = MP4_RAW_FORMAT_RAW;
            break;
        default:
            strFormat = MP4_RAW_FORMAT_RAW;
            break;
    }

    return strFormat;
}

mp4_object_e CMPEG4Encoder::GetVideoObjType(AX_PAYLOAD_TYPE_E ePayloadType) {
    mp4_object_e eMp4Obj = MP4_OBJECT_BUTT;
    switch (ePayloadType) {
        case PT_H264:
            eMp4Obj = MP4_OBJECT_AVC;
            break;
        case PT_H265:
            eMp4Obj = MP4_OBJECT_HEVC;
            break;
        case PT_MJPEG:
            eMp4Obj = MP4_OBJECT_MJPG;
            break;
        default:
            break;
    }

    return eMp4Obj;
}

AX_BOOL CMPEG4Encoder::IsSupport() {
    if (m_stMp4Info.video.object != MP4_OBJECT_AVC
        && m_stMp4Info.video.object != MP4_OBJECT_HEVC) {
        LOG_MM_C(MPEG4, "[Warning] Only support h264/h265 stream currently, please check video out stream type for sns(%d) preview chn(%d)",
                        m_stMp4VideoChnInfo.nSnsId, m_stMp4VideoChnInfo.nPreviewChn);
        return AX_FALSE;
    }

    if (m_stMp4Info.e_record_type == MP4_RECORD_TYPE_MP4) {
        if (m_stMp4Info.audio.enable
            && m_stMp4Info.audio.object != MP4_OBJECT_ALAW
            && m_stMp4Info.audio.object != MP4_OBJECT_ULAW
            && m_stMp4Info.audio.object != MP4_OBJECT_AAC) {
            LOG_MM_C(MPEG4, "Don't support audio obj type: %d currently for sns(%d) preview chn(%d)",
                            m_stMp4Info.audio.object, m_stMp4VideoChnInfo.nSnsId, m_stMp4VideoChnInfo.nPreviewChn);
            return AX_FALSE;
        }
    }

    return AX_TRUE;
}

// The values must be consistent with type index of "Recorder.vue"
MPEG4EC_TYPE_INFO_T CMPEG4Encoder::GetRecordTypeInfo() {
    MPEG4EC_TYPE_INFO_T stRecordTypeInfo;

    switch (m_stMp4Info.e_record_type) {
        case MP4_RECORD_TYPE_MP4:
            stRecordTypeInfo.nRecordType = 0;
            break;
        case MP4_RECORD_TYPE_RAW:
            stRecordTypeInfo.nRecordType = 1;
            break;
        default:
            break;
    }

    switch (m_stMp4Info.video.object) {
        case MP4_OBJECT_AVC:
            stRecordTypeInfo.nVideoRawStreamType = 0;
            break;
        case MP4_OBJECT_HEVC:
            stRecordTypeInfo.nVideoRawStreamType = 1;
            break;
        case MP4_OBJECT_MJPG:
            stRecordTypeInfo.nVideoRawStreamType = 2;
            break;
        default:
            break;
    }

    return stRecordTypeInfo;
}

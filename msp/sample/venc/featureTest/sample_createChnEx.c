/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include <assert.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

#include "ax_sys_api.h"
#include "ax_venc_comm.h"
#include "common_venc.h"
#include "sample_cmd_params.h"
#include "sample_global.h"
#include "sample_unit_test.h"
#include "sample_venc_log.h"
#include "sample_createChnEx.h"

static AX_S32 gAliveChn[MAX_VENC_CHN_NUM] = {
    [0 ... MAX_VENC_CHN_NUM - 1] = -1,
};

static AX_BOOL gLoopExit = AX_FALSE;
static SAMPLE_VENC_SENDFRAME_PARA_T gstFrmParam[MAX_VENC_CHN_NUM];
static SAMPLE_VENC_GETSTREAM_PARA_T gstStrmParam[MAX_VENC_CHN_NUM];

static void SigHandler(AX_S32 signo)
{
    SAMPLE_LOG("catch signal(%d).\n", signo);
    gLoopExit = AX_TRUE;
}

AX_S32 UTestCreateChnEx(SAMPLE_VENC_CMD_PARA_T *pCml)
{
    AX_S32 s32Ret = 0;
    AX_U32 chnNum;
    AX_S32 idx;
    chnNum = pCml->chnNum;
    AX_PAYLOAD_TYPE_E enType;
    SAMPLE_VENC_RC_E enRcMode = pCml->rcMode;
    AX_U64 gVencCaseLoopExit = 0;

    signal(SIGINT, SigHandler);

    if (pCml->qpMapQpType || pCml->qpMapBlkType) {
        SAMPLE_LOG_ERR(" This UT not support QpMap!\n");
        return -1;
    }

    for (idx = 0; idx < chnNum; idx++) {
        memset(&gstFrmParam[idx], 0, sizeof(SAMPLE_VENC_SENDFRAME_PARA_T));
        memset(&gstStrmParam[idx], 0, sizeof(SAMPLE_VENC_GETSTREAM_PARA_T));

        if (pCml->bChnCustom)
            enType = SampleGetCodecType(pCml->codecType);
        else
            enType = SampleGetCodecType(idx % SAMPLE_ALL_CODEC_TYPE);

        if (PT_BUTT == enType) {
            SAMPLE_LOG_ERR("Idx-%d: Invalid codec type!\n", idx);
            s32Ret = -1;
            goto FREE;
        }

        s32Ret = SAMPLE_VENC_CreateChnEx_Start(&gAliveChn[idx], enType, enRcMode, pCml);
        if (AX_SUCCESS != s32Ret) {
            SAMPLE_LOG_ERR("Idx-%d: COMMON_VENC_Start failed.\n", idx);
            s32Ret = -1;
            goto FREE;
        }

        SampleSendFrameInit(gAliveChn[idx], enType, &gstFrmParam[idx], pCml);

        s32Ret = COMMON_VENC_StartSendFrame(&gstFrmParam[idx]);
        if (AX_SUCCESS != s32Ret) {
            SAMPLE_LOG_ERR("chn-%d: COMMON_VENC_StartSendFrame err, idx=%d\n", gAliveChn[idx], idx);
            s32Ret = -1;
            goto FREE;
        }

        SampleGetStreamInit(gAliveChn[idx], enType, &gstStrmParam[idx], pCml);

        s32Ret = COMMON_VENC_StartGetStream(&gstStrmParam[idx]);
        if (AX_SUCCESS != s32Ret) {
            SAMPLE_LOG_ERR("chn-%d: COMMON_VENC_StartGetStream err, idx=%d\n", gAliveChn[idx], idx);
            s32Ret = -1;
            goto FREE;
        }

        COMMON_VENC_AdjustLoopExit(&gVencCaseLoopExit, gAliveChn[idx]);
    }

    while ((!gLoopExit) && (gVencCaseLoopExit != pCml->vencLoopExit))
        sleep(1);

FREE:

    for (idx = 0; idx < chnNum; idx++) {
        COMMON_VENC_StopSendFrame(&gstFrmParam[idx]);
        COMMON_VENC_SendGetDelay(gAliveChn[idx], gLoopExit, &gstFrmParam[idx], &gstStrmParam[idx]);
        COMMON_VENC_Stop(gAliveChn[idx]);
        COMMON_VENC_StopGetStream(&gstStrmParam[idx]);
    }

    pCml->vencLoopExit = 0;

    return s32Ret;
}

AX_S32 SAMPLE_VENC_CreateChnEx(VENC_CHN *pVeChn, AX_PAYLOAD_TYPE_E enType, SAMPLE_VENC_RC_E rcMode,
                               SAMPLE_VENC_CMD_PARA_T *pstArg)
{
    AX_S32 s32Ret = -1;
    AX_S32 widthSrc;
    AX_S32 heightSrc;
    AX_S32 maxPicWidth;
    AX_S32 maxPicHeight;
    AX_U32 gopLen = 30;
    AX_U32 virILen;

    AX_U32 bitRate = 2000; // kbps
    AX_U16 qpMin = 1;
    AX_U16 qpMax = 51;
    AX_U16 qpMinI = 1;
    AX_U16 qpMaxI = 51;
    AX_U16 u32IQp = 25;
    AX_U16 u32PQp = 30;
    AX_U16 u32FixQp = 30;

    AX_VENC_GOP_MODE_E gopType = AX_VENC_GOPMODE_NORMALP;
    AX_S32 intraQpDelta = 0;

    AX_S32 ctbRcMode;
    AX_S32 qpMapType;
    AX_S32 qpMapBlkType;
    AX_S32 qpMapBlockUnit;

    AX_VENC_CHN_ATTR_T stVencChnAttr;
    memset(&stVencChnAttr, 0, sizeof(AX_VENC_CHN_ATTR_T));

    widthSrc = pstArg->picW;
    heightSrc = pstArg->picH;
    maxPicWidth = pstArg->maxPicW;
    maxPicHeight = pstArg->maxPicH;

    rcMode = pstArg->rcMode;

    gopLen = pstArg->gopLen;
    virILen = pstArg->virILen;
    bitRate = pstArg->bitRate;
    qpMin = pstArg->qpMin;
    qpMax = pstArg->qpMax;
    qpMinI = pstArg->qpMinI;
    qpMaxI = pstArg->qpMaxI;
    gopType = pstArg->gopMode;
    intraQpDelta = pstArg->IQpDelta;
    qpMapType = pstArg->qpMapQpType;
    qpMapBlkType = pstArg->qpMapBlkType;
    qpMapBlockUnit = pstArg->qpMapBlkUnit;
    ctbRcMode = pstArg->ctbRcMode;
    u32IQp = pstArg->IQp;
    u32PQp = pstArg->PQp;
    u32FixQp = pstArg->fixedQp;
    stVencChnAttr.stVencAttr.enType = enType;
    stVencChnAttr.stVencAttr.u32PicWidthSrc = widthSrc; /*the input picture width*/
    stVencChnAttr.stVencAttr.u32PicHeightSrc = heightSrc; /*the input picture height*/
    stVencChnAttr.stVencAttr.u32MaxPicWidth = maxPicWidth;
    stVencChnAttr.stVencAttr.u32MaxPicHeight = maxPicHeight;
    stVencChnAttr.stVencAttr.enLinkMode = pstArg->bLinkMode ? AX_VENC_LINK_MODE : AX_VENC_UNLINK_MODE;
    stVencChnAttr.stVencAttr.u8InFifoDepth = pstArg->inFifoDep;
    stVencChnAttr.stVencAttr.u8OutFifoDepth = pstArg->outFifoDep;
    stVencChnAttr.stVencAttr.u32BufSize = pstArg->strmBufSize;

    stVencChnAttr.stRcAttr.stFrameRate.fSrcFrameRate = pstArg->srcFrameRate;
    stVencChnAttr.stRcAttr.stFrameRate.fDstFrameRate = pstArg->dstFrameRate;

    /* crop setting */
    if (pstArg->bCrop) {
        stVencChnAttr.stVencAttr.stCropCfg.bEnable = AX_TRUE;
        stVencChnAttr.stVencAttr.stCropCfg.stRect.s32X = pstArg->cropX;
        stVencChnAttr.stVencAttr.stCropCfg.stRect.s32Y = pstArg->cropY;
        stVencChnAttr.stVencAttr.stCropCfg.stRect.u32Width = pstArg->cropW;
        stVencChnAttr.stVencAttr.stCropCfg.stRect.u32Height = pstArg->cropH;
    }

    if (pstArg->bCoreCoWork)
        stVencChnAttr.stVencAttr.flag |= AX_VENC_CHN_ENABLE_MULTICORE;

    if (pstArg->bStrmCached)
        stVencChnAttr.stVencAttr.flag |= AX_VENC_STREAM_CACHED;
    if (pstArg->bAttachHdr)
        stVencChnAttr.stVencAttr.flag |= AX_VENC_HEADER_ATTACH_TO_PB;
    if (pstArg->bQpmapCopy)
        stVencChnAttr.stVencAttr.flag |= AX_VENC_QPMAP_MEM_COPY;

    switch (stVencChnAttr.stVencAttr.enType) {
    case PT_H265: {
        if (VENC_STREAM_10BIT == pstArg->strmBitDep) {
            stVencChnAttr.stVencAttr.enProfile = AX_VENC_HEVC_MAIN_10_PROFILE;
            stVencChnAttr.stVencAttr.enStrmBitDepth = AX_VENC_STREAM_BIT_10;
        } else {
            stVencChnAttr.stVencAttr.enProfile = AX_VENC_HEVC_MAIN_PROFILE;
            stVencChnAttr.stVencAttr.enStrmBitDepth = AX_VENC_STREAM_BIT_8;
        }
        stVencChnAttr.stVencAttr.enLevel = AX_VENC_HEVC_LEVEL_5_1;
        stVencChnAttr.stVencAttr.enTier = AX_VENC_HEVC_MAIN_TIER;
        if (rcMode == SAMPLE_RC_CBR) {
            AX_VENC_H265_CBR_T stH265Cbr;
            memset(&stH265Cbr, 0, sizeof(AX_VENC_H265_CBR_T));
            stVencChnAttr.stRcAttr.enRcMode = AX_VENC_RC_MODE_H265CBR;
            stVencChnAttr.stRcAttr.s32FirstFrameStartQp = pstArg->startQp;
            stH265Cbr.u32Gop = gopLen;
            stH265Cbr.u32BitRate = bitRate;
            stH265Cbr.u32MinQp = qpMin;
            stH265Cbr.u32MaxQp = qpMax;
            stH265Cbr.u32MinIQp = qpMinI;
            stH265Cbr.u32MaxIQp = qpMaxI;
            stH265Cbr.s32IntraQpDelta = intraQpDelta;

            stH265Cbr.u32MaxIprop = pstArg->maxIprop;
            stH265Cbr.u32MinIprop = pstArg->minIprop;
            stH265Cbr.u32IdrQpDeltaRange = 10;

            stH265Cbr.stQpmapInfo.enQpmapQpType = qpMapType;
            stH265Cbr.stQpmapInfo.enQpmapBlockType = qpMapBlkType;
            stH265Cbr.stQpmapInfo.enQpmapBlockUnit = qpMapBlockUnit;
            stH265Cbr.stQpmapInfo.enCtbRcMode = ctbRcMode;
            memcpy(&stVencChnAttr.stRcAttr.stH265Cbr, &stH265Cbr, sizeof(AX_VENC_H265_CBR_T));
        } else if (rcMode == SAMPLE_RC_VBR) {
            AX_VENC_H265_VBR_T stH265Vbr;
            memset(&stH265Vbr, 0, sizeof(AX_VENC_H265_VBR_T));
            stVencChnAttr.stRcAttr.enRcMode = AX_VENC_RC_MODE_H265VBR;
            stVencChnAttr.stRcAttr.s32FirstFrameStartQp = pstArg->startQp;
            stH265Vbr.u32Gop = gopLen;
            stH265Vbr.u32MaxBitRate = bitRate;
            stH265Vbr.enVQ = pstArg->vq;
            stH265Vbr.u32MinQp = qpMin;
            stH265Vbr.u32MaxQp = qpMax;
            stH265Vbr.u32MinIQp = qpMinI;
            stH265Vbr.u32MaxIQp = qpMaxI;
            stH265Vbr.s32IntraQpDelta = intraQpDelta;
            stH265Vbr.u32IdrQpDeltaRange = 10;

            stH265Vbr.stQpmapInfo.enQpmapQpType = qpMapType;
            stH265Vbr.stQpmapInfo.enQpmapBlockType = qpMapBlkType;
            stH265Vbr.stQpmapInfo.enQpmapBlockUnit = qpMapBlockUnit;
            stH265Vbr.stQpmapInfo.enCtbRcMode = ctbRcMode;
            memcpy(&stVencChnAttr.stRcAttr.stH265Vbr, &stH265Vbr, sizeof(AX_VENC_H265_VBR_T));
        } else if (rcMode == SAMPLE_RC_AVBR) {
            AX_VENC_H265_AVBR_T stH265AVbr;
            memset(&stH265AVbr, 0, sizeof(AX_VENC_H265_AVBR_T));
            stVencChnAttr.stRcAttr.enRcMode = AX_VENC_RC_MODE_H265AVBR;
            stVencChnAttr.stRcAttr.s32FirstFrameStartQp = pstArg->startQp;
            stH265AVbr.u32Gop = gopLen;
            stH265AVbr.u32MaxBitRate = bitRate;
            stH265AVbr.u32MinQp = qpMin;
            stH265AVbr.u32MaxQp = qpMax;
            stH265AVbr.u32MinIQp = qpMinI;
            stH265AVbr.u32MaxIQp = qpMaxI;
            stH265AVbr.s32IntraQpDelta = intraQpDelta;
            stH265AVbr.u32IdrQpDeltaRange = 10;

            stH265AVbr.stQpmapInfo.enQpmapQpType = qpMapType;
            stH265AVbr.stQpmapInfo.enQpmapBlockType = qpMapBlkType;
            stH265AVbr.stQpmapInfo.enQpmapBlockUnit = qpMapBlockUnit;
            stH265AVbr.stQpmapInfo.enCtbRcMode = ctbRcMode;

            memcpy(&stVencChnAttr.stRcAttr.stH265AVbr, &stH265AVbr, sizeof(AX_VENC_H265_AVBR_T));
        } else if (rcMode == SAMPLE_RC_CVBR) {
            AX_VENC_H265_CVBR_T stH265CVbr;
            memset(&stH265CVbr, 0, sizeof(AX_VENC_H265_CVBR_T));
            stVencChnAttr.stRcAttr.enRcMode = AX_VENC_RC_MODE_H265CVBR;
            stVencChnAttr.stRcAttr.s32FirstFrameStartQp = pstArg->startQp;
            stH265CVbr.u32Gop = gopLen;
            stH265CVbr.u32MaxBitRate = bitRate;

            stH265CVbr.u32MinQp = qpMin;
            stH265CVbr.u32MaxQp = qpMax;
            stH265CVbr.u32MinIQp = qpMinI;
            stH265CVbr.u32MaxIQp = qpMaxI;
            stH265CVbr.u32IdrQpDeltaRange = 10;

            stH265CVbr.u32MaxIprop = pstArg->maxIprop;
            stH265CVbr.u32MinIprop = pstArg->minIprop;

            stH265CVbr.u32MinQpDelta = pstArg->minQpDelta;
            stH265CVbr.u32MaxQpDelta = pstArg->maxQpDelta;

            stH265CVbr.u32LongTermMinBitrate = pstArg->ltMinBt;
            stH265CVbr.u32LongTermMaxBitrate = pstArg->ltMaxBt;
            stH265CVbr.u32LongTermStatTime = pstArg->ltStaTime;
            stH265CVbr.u32ShortTermStatTime = pstArg->shtStaTime;

            stH265CVbr.stQpmapInfo.enQpmapQpType = qpMapType;
            stH265CVbr.stQpmapInfo.enQpmapBlockType = qpMapBlkType;
            stH265CVbr.stQpmapInfo.enQpmapBlockUnit = qpMapBlockUnit;
            stH265CVbr.stQpmapInfo.enCtbRcMode = ctbRcMode;

            memcpy(&stVencChnAttr.stRcAttr.stH265CVbr, &stH265CVbr, sizeof(AX_VENC_H265_CVBR_T));
        } else if (rcMode == SAMPLE_RC_FIXQP) {
            AX_VENC_H265_FIXQP_T stH265FixQp;
            memset(&stH265FixQp, 0, sizeof(AX_VENC_H265_FIXQP_T));
            stVencChnAttr.stRcAttr.enRcMode = AX_VENC_RC_MODE_H265FIXQP;
            stH265FixQp.u32Gop = gopLen;
            stH265FixQp.u32IQp = u32IQp;
            stH265FixQp.u32PQp = u32PQp;
            stH265FixQp.u32BQp = 32;
            memcpy(&stVencChnAttr.stRcAttr.stH265FixQp, &stH265FixQp, sizeof(AX_VENC_H265_FIXQP_T));
        } else if (rcMode == SAMPLE_RC_QPMAP) {
            AX_VENC_H265_QPMAP_T stH265QpMap;
            memset(&stH265QpMap, 0, sizeof(AX_VENC_H265_QPMAP_T));
            stVencChnAttr.stRcAttr.enRcMode = AX_VENC_RC_MODE_H265QPMAP;
            stVencChnAttr.stRcAttr.s32FirstFrameStartQp = pstArg->startQp;

            stH265QpMap.stQpmapInfo.enQpmapQpType = qpMapType;
            stH265QpMap.stQpmapInfo.enQpmapBlockUnit = qpMapBlockUnit;
            stH265QpMap.stQpmapInfo.enQpmapBlockType = qpMapBlkType;
            stH265QpMap.stQpmapInfo.enCtbRcMode = ctbRcMode;

            stH265QpMap.u32Gop = gopLen;
            stH265QpMap.u32TargetBitRate = bitRate;
            memcpy(&stVencChnAttr.stRcAttr.stH265QpMap, &stH265QpMap, sizeof(AX_VENC_H265_QPMAP_T));
        }
        break;
    }

    case PT_H264: {
        if (VENC_STREAM_10BIT == pstArg->strmBitDep) {
            stVencChnAttr.stVencAttr.enProfile = AX_VENC_H264_HIGH_10_PROFILE;
            stVencChnAttr.stVencAttr.enStrmBitDepth = AX_VENC_STREAM_BIT_10;
        } else {
            stVencChnAttr.stVencAttr.enProfile = AX_VENC_H264_MAIN_PROFILE;
            stVencChnAttr.stVencAttr.enStrmBitDepth = AX_VENC_STREAM_BIT_8;
        }
        stVencChnAttr.stVencAttr.enLevel = AX_VENC_H264_LEVEL_5_1;
        if (rcMode == SAMPLE_RC_CBR) {
            AX_VENC_H264_CBR_T stH264Cbr;
            memset(&stH264Cbr, 0, sizeof(AX_VENC_H264_CBR_T));
            stVencChnAttr.stRcAttr.enRcMode = AX_VENC_RC_MODE_H264CBR;
            stVencChnAttr.stRcAttr.s32FirstFrameStartQp = pstArg->startQp;
            stH264Cbr.u32Gop = gopLen;
            stH264Cbr.u32BitRate = bitRate;
            stH264Cbr.u32MinQp = qpMin;
            stH264Cbr.u32MaxQp = qpMax;
            stH264Cbr.u32MinIQp = qpMinI;
            stH264Cbr.u32MaxIQp = qpMaxI;
            stH264Cbr.s32IntraQpDelta = intraQpDelta;
            stH264Cbr.u32IdrQpDeltaRange = 10;
            stH264Cbr.u32MaxIprop = pstArg->maxIprop;
            stH264Cbr.u32MinIprop = pstArg->minIprop;

            stH264Cbr.stQpmapInfo.enQpmapQpType = qpMapType;
            stH264Cbr.stQpmapInfo.enQpmapBlockType = qpMapBlkType;
            stH264Cbr.stQpmapInfo.enQpmapBlockUnit = qpMapBlockUnit;
            stH264Cbr.stQpmapInfo.enCtbRcMode = ctbRcMode;

            memcpy(&stVencChnAttr.stRcAttr.stH264Cbr, &stH264Cbr, sizeof(AX_VENC_H264_CBR_T));
        } else if (rcMode == SAMPLE_RC_VBR) {
            AX_VENC_H264_VBR_T stH264Vbr;
            memset(&stH264Vbr, 0, sizeof(AX_VENC_H264_VBR_T));
            stVencChnAttr.stRcAttr.enRcMode = AX_VENC_RC_MODE_H264VBR;
            stVencChnAttr.stRcAttr.s32FirstFrameStartQp = pstArg->startQp;
            stH264Vbr.u32Gop = gopLen;
            stH264Vbr.u32MaxBitRate = bitRate;
            stH264Vbr.enVQ = pstArg->vq;
            stH264Vbr.u32MinQp = qpMin;
            stH264Vbr.u32MaxQp = qpMax;
            stH264Vbr.u32MinIQp = qpMinI;
            stH264Vbr.u32MaxIQp = qpMaxI;
            stH264Vbr.s32IntraQpDelta = intraQpDelta;
            stH264Vbr.u32IdrQpDeltaRange = 10;

            stH264Vbr.stQpmapInfo.enQpmapQpType = qpMapType;
            stH264Vbr.stQpmapInfo.enQpmapBlockType = qpMapBlkType;
            stH264Vbr.stQpmapInfo.enQpmapBlockUnit = qpMapBlockUnit;
            stH264Vbr.stQpmapInfo.enCtbRcMode = ctbRcMode;

            memcpy(&stVencChnAttr.stRcAttr.stH264Vbr, &stH264Vbr, sizeof(AX_VENC_H264_VBR_T));
        } else if (rcMode == SAMPLE_RC_AVBR) {
            AX_VENC_H264_AVBR_T stH264AVbr;
            memset(&stH264AVbr, 0, sizeof(AX_VENC_H264_AVBR_T));
            stVencChnAttr.stRcAttr.enRcMode = AX_VENC_RC_MODE_H264AVBR;
            stVencChnAttr.stRcAttr.s32FirstFrameStartQp = pstArg->startQp;
            stH264AVbr.u32Gop = gopLen;
            stH264AVbr.u32MaxBitRate = bitRate;
            stH264AVbr.u32MinQp = qpMin;
            stH264AVbr.u32MaxQp = qpMax;
            stH264AVbr.u32MinIQp = qpMinI;
            stH264AVbr.u32MaxIQp = qpMaxI;
            stH264AVbr.s32IntraQpDelta = intraQpDelta;
            stH264AVbr.u32IdrQpDeltaRange = 10;

            stH264AVbr.stQpmapInfo.enQpmapQpType = qpMapType;
            stH264AVbr.stQpmapInfo.enQpmapBlockType = qpMapBlkType;
            stH264AVbr.stQpmapInfo.enQpmapBlockUnit = qpMapBlockUnit;
            stH264AVbr.stQpmapInfo.enCtbRcMode = ctbRcMode;

            memcpy(&stVencChnAttr.stRcAttr.stH264AVbr, &stH264AVbr, sizeof(AX_VENC_H264_AVBR_T));
        } else if (rcMode == SAMPLE_RC_CVBR) {
            AX_VENC_H264_CVBR_T stH264CVbr;
            memset(&stH264CVbr, 0, sizeof(AX_VENC_H264_CVBR_T));
            stVencChnAttr.stRcAttr.enRcMode = AX_VENC_RC_MODE_H264CVBR;
            stVencChnAttr.stRcAttr.s32FirstFrameStartQp = pstArg->startQp;
            stH264CVbr.u32Gop = gopLen;
            stH264CVbr.u32MaxBitRate = bitRate;

            stH264CVbr.u32MinQp = qpMin;
            stH264CVbr.u32MaxQp = qpMax;
            stH264CVbr.u32MinIQp = qpMinI;
            stH264CVbr.u32MaxIQp = qpMaxI;

            stH264CVbr.u32MaxIprop = pstArg->maxIprop;
            stH264CVbr.u32MinIprop = pstArg->minIprop;

            stH264CVbr.u32MinQpDelta = pstArg->minQpDelta;
            stH264CVbr.u32MaxQpDelta = pstArg->maxQpDelta;

            stH264CVbr.u32LongTermMinBitrate = pstArg->ltMinBt;
            stH264CVbr.u32LongTermMaxBitrate = pstArg->ltMaxBt;
            stH264CVbr.u32LongTermStatTime = pstArg->ltStaTime;
            stH264CVbr.u32ShortTermStatTime = pstArg->shtStaTime;
            stH264CVbr.u32IdrQpDeltaRange = 10;

            stH264CVbr.stQpmapInfo.enQpmapQpType = qpMapType;
            stH264CVbr.stQpmapInfo.enQpmapBlockType = qpMapBlkType;
            stH264CVbr.stQpmapInfo.enQpmapBlockUnit = qpMapBlockUnit;
            stH264CVbr.stQpmapInfo.enCtbRcMode = ctbRcMode;

            memcpy(&stVencChnAttr.stRcAttr.stH264CVbr, &stH264CVbr, sizeof(AX_VENC_H264_CVBR_T));
        } else if (rcMode == SAMPLE_RC_FIXQP) {
            AX_VENC_H264_FIXQP_T stH264FixQp;
            memset(&stH264FixQp, 0, sizeof(AX_VENC_H264_FIXQP_T));
            stVencChnAttr.stRcAttr.enRcMode = AX_VENC_RC_MODE_H264FIXQP;
            stH264FixQp.u32Gop = gopLen;
            stH264FixQp.u32IQp = u32IQp;
            stH264FixQp.u32PQp = u32PQp;
            stH264FixQp.u32BQp = 32;
            memcpy(&stVencChnAttr.stRcAttr.stH264FixQp, &stH264FixQp, sizeof(AX_VENC_H264_FIXQP_T));
        } else if (rcMode == SAMPLE_RC_QPMAP) {
            AX_VENC_H264_QPMAP_T stH264QpMap;
            memset(&stH264QpMap, 0, sizeof(AX_VENC_H264_QPMAP_T));
            stVencChnAttr.stRcAttr.enRcMode = AX_VENC_RC_MODE_H264QPMAP;
            stVencChnAttr.stRcAttr.s32FirstFrameStartQp = pstArg->startQp;

            stH264QpMap.stQpmapInfo.enQpmapQpType = qpMapType;
            stH264QpMap.stQpmapInfo.enQpmapBlockUnit = qpMapBlockUnit;
            stH264QpMap.stQpmapInfo.enQpmapBlockType = qpMapBlkType;
            stH264QpMap.stQpmapInfo.enCtbRcMode = ctbRcMode;

            stH264QpMap.u32Gop = gopLen;
            stH264QpMap.u32TargetBitRate = bitRate;
            memcpy(&stVencChnAttr.stRcAttr.stH264QpMap, &stH264QpMap, sizeof(AX_VENC_H264_QPMAP_T));
        }
        break;
    }

    case PT_JPEG: {
        break;
    }

    case PT_MJPEG: {
        if (rcMode == SAMPLE_RC_CBR) {
            AX_VENC_MJPEG_CBR_T stMjpegCbr;
            memset(&stMjpegCbr, 0, sizeof(stMjpegCbr));

            stVencChnAttr.stRcAttr.enRcMode = AX_VENC_RC_MODE_MJPEGCBR;
            stVencChnAttr.stRcAttr.s32FirstFrameStartQp = pstArg->startQp;
            stMjpegCbr.u32StatTime = 1;
            stMjpegCbr.u32BitRate = bitRate;
            stMjpegCbr.u32MinQp = qpMin;
            stMjpegCbr.u32MaxQp = qpMax;
            memcpy(&stVencChnAttr.stRcAttr.stMjpegCbr, &stMjpegCbr, sizeof(AX_VENC_MJPEG_CBR_T));
        } else if (rcMode == SAMPLE_RC_VBR) {
            AX_VENC_MJPEG_VBR_T stMjpegVbr;
            memset(&stMjpegVbr, 0, sizeof(stMjpegVbr));

            stVencChnAttr.stRcAttr.enRcMode = AX_VENC_RC_MODE_MJPEGVBR;
            stVencChnAttr.stRcAttr.s32FirstFrameStartQp = pstArg->startQp;
            stMjpegVbr.u32StatTime = 1;
            stMjpegVbr.u32MaxBitRate = bitRate;
            stMjpegVbr.u32MinQp = qpMin;
            stMjpegVbr.u32MaxQp = qpMax;
            memcpy(&stVencChnAttr.stRcAttr.stMjpegVbr, &stMjpegVbr, sizeof(AX_VENC_MJPEG_VBR_T));
        } else if (rcMode == SAMPLE_RC_FIXQP) {
            AX_VENC_MJPEG_FIXQP_T stMjpegFixQp;

            stVencChnAttr.stRcAttr.enRcMode = AX_VENC_RC_MODE_MJPEGFIXQP;
            stMjpegFixQp.s32FixedQp = u32FixQp;
            memcpy(&stVencChnAttr.stRcAttr.stMjpegFixQp, &stMjpegFixQp, sizeof(AX_VENC_MJPEG_FIXQP_T));
        }
        break;
    }
    default:
        SAMPLE_LOG_ERR("Invalid Codec Format.\n");
        s32Ret = -1;
        goto END;
    }

    /* GOP table setting */
    switch (gopType) {
    case AX_VENC_GOPMODE_NORMALP: {
        stVencChnAttr.stGopAttr.enGopMode = AX_VENC_GOPMODE_NORMALP;
        break;
    }
    case AX_VENC_GOPMODE_ONELTR: {
        /* Normal frame configures */
        stVencChnAttr.stGopAttr.enGopMode = AX_VENC_GOPMODE_ONELTR;
        stVencChnAttr.stGopAttr.stOneLTR.stPicConfig.s32QpOffset = 0;
        stVencChnAttr.stGopAttr.stOneLTR.stPicConfig.f32QpFactor = 0.4624;
        /* long-term reference and special frame configure */
        stVencChnAttr.stGopAttr.stOneLTR.stPicSpecialConfig.s32Interval = virILen;
        stVencChnAttr.stGopAttr.stOneLTR.stPicSpecialConfig.s32QpOffset = -2;
        stVencChnAttr.stGopAttr.stOneLTR.stPicSpecialConfig.f32QpFactor = 0.4624;
        break;
    }
    case AX_VENC_GOPMODE_SVC_T: {
        /* SVC-T Configure */
        static AX_U32 sSvcTGopSize = 4;
        /*SVC-T GOP4*/
        static AX_CHAR *stSvcTCfg[] = {
            "Frame1:  P      1      0       0.4624        2        1           -1          1",
            "Frame2:  P      2      0       0.4624        1        1           -2          1",
            "Frame3:  P      3      0       0.4624        2        2           -1 -3       1 0",
            "Frame4:  P      4      0       0.4624        0        1           -4          1",
            NULL,
        };
        stVencChnAttr.stGopAttr.enGopMode = AX_VENC_GOPMODE_SVC_T;
        stVencChnAttr.stGopAttr.stSvcT.u32GopSize = sSvcTGopSize;
        stVencChnAttr.stGopAttr.stSvcT.s8SvcTCfg = stSvcTCfg;
        break;
    }

    default:
        SAMPLE_LOG_ERR("Invalid gop type(%d).\n", gopType);
        goto END;
    }

    /* create channel */
    s32Ret = AX_VENC_CreateChnEx(pVeChn, &stVencChnAttr);
    if (AX_SUCCESS != s32Ret) {
        SAMPLE_LOG_ERR("AX_VENC_CreateChn failed, ret=0x%x\n", s32Ret);
        goto END;
    }

    return AX_SUCCESS;

END:

    return s32Ret;
}

AX_S32 SAMPLE_VENC_CreateChnEx_Start(VENC_CHN *pVeChn, AX_PAYLOAD_TYPE_E enType, SAMPLE_VENC_RC_E rcMode,
                                     SAMPLE_VENC_CMD_PARA_T *pstArg)
{
    AX_S32 s32Ret;
    AX_VENC_RECV_PIC_PARAM_T stRecvParam;

    s32Ret = SAMPLE_VENC_CreateChnEx(pVeChn, enType, rcMode, pstArg);
    if (AX_SUCCESS != s32Ret) {
        SAMPLE_LOG_ERR("COMM_VENC_Creat faild with%#x!.\n", s32Ret);
        return -1;
    }

    stRecvParam.s32RecvPicNum = -1;
    s32Ret = AX_VENC_StartRecvFrame(*pVeChn, &stRecvParam);
    if (AX_SUCCESS != s32Ret) {
        SAMPLE_LOG_ERR("chn-%d: AX_VENC_StartRecvFrame failed with%#x! \n", *pVeChn, s32Ret);
        return -1;
    }

    return AX_SUCCESS;
}
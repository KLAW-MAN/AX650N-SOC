/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef _SAMPLE_IVPS_SYNC_API_H_
#define _SAMPLE_IVPS_SYNC_API_H_

typedef enum
{
        IVPS_ENGINE_ID_TDP = 0,
        IVPS_ENGINE_ID_VPP,
        IVPS_ENGINE_ID_VGP,
        IVPS_ENGINE_ID_GDC,
        IVPS_ENGINE_ID_BUTT
} IVPS_ENGINE_ID_E;

typedef struct
{
        /* cmmcopy */
        IVPS_ENGINE_ID_E eEngineId;
	AX_U32 nMemSize;
        /* csc */
        /* flip and rotation */
        /* alphablending */
        /* crop resize */
        /* crop resize2 */
        /* crop resize3 */
} SAMPLE_IVPS_SYNC_API_T;

typedef struct
{
	AX_U32 nOsdNum;
} SAMPLE_IVPS_OSD_T;

typedef struct
{
	AX_U32 nCoverNum;
} SAMPLE_IVPS_COVER_T;

typedef struct
{
        SAMPLE_IVPS_OSD_T tOsd;
	SAMPLE_IVPS_COVER_T tCover;
} SAMPLE_IVPS_REGION_T;

typedef struct axSAMPLE_IVPS_ARGS_T {
    SAMPLE_IVPS_GRP_T *pGrp;
    IVPS_ARG_T *ptArg;
    AX_VIDEO_FRAME_T *tOverlay;
    AX_S32 nChnIdx;
} SAMPLE_IVPS_ARGS_T;

extern SAMPLE_IVPS_SYNC_API_T gSampleIvpsSyncApi;
extern SAMPLE_IVPS_REGION_T gSampleIvpsRegion;

/* CPU */
AX_S32 SAMPLE_FillCanvas(AX_IVPS_RGN_CANVAS_INFO_T *ptCanvas);
AX_S32 SAMPLE_DrawCover(const AX_VIDEO_FRAME_T *pstVFrame, const SAMPLE_IVPS_COVER_T *ptIvpsCover, char *strFilePath, char *pCaseId);

/* NPU */
AX_S32 SAMPLE_CropResizeNpu(AX_VIDEO_FRAME_T *ptSrcFrame, char *strFilePath);

/* CmmCopy */
AX_S32 SAMPLE_IVPS_CmmCopy(IVPS_ENGINE_ID_E eEngineId, const AX_VIDEO_FRAME_T *ptSrcFrame, char *strFilePath, char *pCaseId);

/* PyraLite */
AX_S32 SAMPLE_Pyra_Gen(const AX_VIDEO_FRAME_T *ptSrcFrame, char *strFilePath, AX_BOOL bPyraMode);
AX_S32 SAMPLE_Pyra_Rcn(const AX_VIDEO_FRAME_T *ptGauFrame, AX_VIDEO_FRAME_T *ptLapFrame, char *strFilePath, AX_BOOL bPyraMode);
/* Csc */
AX_S32 SAMPLE_IVPS_Csc(IVPS_ENGINE_ID_E eEngineId, const AX_VIDEO_FRAME_T *ptSrcFrame, char *strFilePath, char *pCaseId);

/* Flip and Rotation */
AX_S32 SAMPLE_IVPS_FlipAndRotation(const AX_VIDEO_FRAME_T *ptSrcFrame,
                                   AX_S32 nFlipCode, AX_S32 nRotation, char *strFilePath, char *pCaseId);

/* AlphaBlending */
AX_S32 SAMPLE_IVPS_AlphaBlending(IVPS_ENGINE_ID_E eEngineId, const AX_VIDEO_FRAME_T *ptSrcFrame,
                                 AX_VIDEO_FRAME_T *ptOverlay, AX_U8 nAlpha, char *strFilePath, char *pCaseId);

/* AlphaBlendingAsync */
AX_S32 SAMPLE_IVPS_AlphaBlendingAsync(IVPS_ENGINE_ID_E eEngineId, const AX_VIDEO_FRAME_T *ptSrcFrame,
                                 AX_VIDEO_FRAME_T *ptOverlay, AX_U8 nAlpha, char *strFilePath, char *pCaseId);

/* AlphaBlendingV2 */
AX_S32 SAMPLE_IVPS_AlphaBlendingV2(const AX_VIDEO_FRAME_T *ptSrcFrame, AX_VIDEO_FRAME_T *ptOverlay,
                                   const AX_IVPS_ALPHA_LUT_T *ptSpAlpha, char *strFilePath, char *pCaseId);
/* AlphaBlendingV3 */
AX_S32 SAMPLE_IVPS_AlphaBlendingV3(IVPS_ENGINE_ID_E eEngineId, const AX_VIDEO_FRAME_T *ptSrcFrame,
                                 AX_VIDEO_FRAME_T *ptOverlay, AX_U8 nAlpha, char *strFilePath, char *pCaseId);
/* CropResize */
AX_S32 SAMPLE_IVPS_CropResize(IVPS_ENGINE_ID_E eEngineId, const AX_VIDEO_FRAME_T *ptSrcFrame, char *strFilePath, char *pCaseId);

/* CropResize V2 */
AX_S32 SAMPLE_IVPS_CropResizeV2(IVPS_ENGINE_ID_E eEngineId, const AX_VIDEO_FRAME_T *ptSrcFrame, AX_IVPS_RECT_T tBox[], AX_U32 nNum,
                                AX_S32 nDstStride[], AX_S32 nDstWidth[], AX_S32 nDstHeight[], char *strFilePath, char *pCaseId);

/* CropResize V3 */
AX_S32 SAMPLE_IVPS_CropResizeV3(const AX_VIDEO_FRAME_T *ptSrcFrame,
                                AX_S32 nDstStride[], AX_S32 nDstWidth[],
                                AX_S32 nDstHeight[], AX_U32 nNum, char *strFilePath, char *pCaseId);

/* Mosaic */
AX_S32 SAMPLE_IVPS_DrawMosaic(IVPS_ENGINE_ID_E eEngineId, const AX_VIDEO_FRAME_T *ptSrcFrame, AX_IVPS_RGN_MOSAIC_T tMosaic[], AX_U32 nNum, char *strFilePath, char *pCaseId);

/* OSD */
AX_S32 AX_IVPS_DrawOsd(IVPS_ENGINE_ID_E eEngineId, const AX_VIDEO_FRAME_T *ptSrc, AX_OSD_BMP_ATTR_T arrBmp[], AX_U32 nNum, char *strFilePath, char *pCaseId);

/* LDCH API, From sample_ivps_dewarp.c */
AX_S32 SAMPLE_IVPS_SyncApiLDCH(const IVPS_ARG_T *ptArg, const SAMPLE_IVPS_GRP_T *pSync);

#endif /* _SAMPLE_IVPS_SYNC_API_H_ */

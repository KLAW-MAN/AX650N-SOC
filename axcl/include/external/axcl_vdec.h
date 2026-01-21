/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef __AXCL_VDEC_API_H__
#define __AXCL_VDEC_API_H__

#include "axcl_vdec_type.h"

#ifdef __cplusplus
extern "C" {
#endif

AXCL_EXPORT AX_S32 AXCL_VDEC_Init(const AX_VDEC_MOD_ATTR_T *pstModAttr);
AXCL_EXPORT AX_S32 AXCL_VDEC_Deinit(AX_VOID);

AXCL_EXPORT AX_S32 AXCL_VDEC_ExtractStreamHeaderInfo(const AX_VDEC_STREAM_T *pstStreamBuf, AX_PAYLOAD_TYPE_E enVideoType,
                                                     AX_VDEC_BITSTREAM_INFO_T *pstBitStreamInfo);

AXCL_EXPORT AX_S32 AXCL_VDEC_CreateGrp(AX_VDEC_GRP VdGrp, const AX_VDEC_GRP_ATTR_T *pstGrpAttr);
AXCL_EXPORT AX_S32 AXCL_VDEC_CreateGrpEx(AX_VDEC_GRP *VdGrp, const AX_VDEC_GRP_ATTR_T *pstGrpAttr);
AXCL_EXPORT AX_S32 AXCL_VDEC_DestroyGrp(AX_VDEC_GRP VdGrp);

AXCL_EXPORT AX_S32 AXCL_VDEC_GetGrpAttr(AX_VDEC_GRP VdGrp, AX_VDEC_GRP_ATTR_T *pstGrpAttr);
AXCL_EXPORT AX_S32 AXCL_VDEC_SetGrpAttr(AX_VDEC_GRP VdGrp, const AX_VDEC_GRP_ATTR_T *pstGrpAttr);

AXCL_EXPORT AX_S32 AXCL_VDEC_StartRecvStream(AX_VDEC_GRP VdGrp, const AX_VDEC_RECV_PIC_PARAM_T *pstRecvParam);
AXCL_EXPORT AX_S32 AXCL_VDEC_StopRecvStream(AX_VDEC_GRP VdGrp);

AXCL_EXPORT AX_S32 AXCL_VDEC_QueryStatus(AX_VDEC_GRP VdGrp, AX_VDEC_GRP_STATUS_T *pstGrpStatus);

AXCL_EXPORT AX_S32 AXCL_VDEC_ResetGrp(AX_VDEC_GRP VdGrp);

AXCL_EXPORT AX_S32 AXCL_VDEC_SetGrpParam(AX_VDEC_GRP VdGrp, const AX_VDEC_GRP_PARAM_T *pstGrpParam);
AXCL_EXPORT AX_S32 AXCL_VDEC_GetGrpParam(AX_VDEC_GRP VdGrp, AX_VDEC_GRP_PARAM_T *pstGrpParam);

AXCL_EXPORT AX_S32 AXCL_VDEC_SelectGrp(AX_VDEC_GRP_SET_INFO_T *pstGrpSet, AX_S32 s32MilliSec);

/* s32MilliSec: -1 is block,0 is no block,other positive number is timeout */
AXCL_EXPORT AX_S32 AXCL_VDEC_SendStream(AX_VDEC_GRP VdGrp, const AX_VDEC_STREAM_T *pstStream, AX_S32 s32MilliSec);

AXCL_EXPORT AX_S32 AXCL_VDEC_GetChnFrame(AX_VDEC_GRP VdGrp, AX_VDEC_CHN VdChn, AX_VIDEO_FRAME_INFO_T *pstFrameInfo, AX_S32 s32MilliSec);
AXCL_EXPORT AX_S32 AXCL_VDEC_ReleaseChnFrame(AX_VDEC_GRP VdGrp, AX_VDEC_CHN VdChn, const AX_VIDEO_FRAME_INFO_T *pstFrameInfo);

AXCL_EXPORT AX_S32 AXCL_VDEC_GetUserData(AX_VDEC_GRP VdGrp, AX_VDEC_USERDATA_T *pstUserData);
AXCL_EXPORT AX_S32 AXCL_VDEC_ReleaseUserData(AX_VDEC_GRP VdGrp, const AX_VDEC_USERDATA_T *pstUserData);

AXCL_EXPORT AX_S32 AXCL_VDEC_SetUserPic(AX_VDEC_GRP VdGrp, const AX_VDEC_USRPIC_T *pstUsrPic);
AXCL_EXPORT AX_S32 AXCL_VDEC_EnableUserPic(AX_VDEC_GRP VdGrp);
AXCL_EXPORT AX_S32 AXCL_VDEC_DisableUserPic(AX_VDEC_GRP VdGrp);

AXCL_EXPORT AX_S32 AXCL_VDEC_SetDisplayMode(AX_VDEC_GRP VdGrp, AX_VDEC_DISPLAY_MODE_E enDisplayMode);
AXCL_EXPORT AX_S32 AXCL_VDEC_GetDisplayMode(AX_VDEC_GRP VdGrp, AX_VDEC_DISPLAY_MODE_E *penDisplayMode);

AXCL_EXPORT AX_S32 AXCL_VDEC_AttachPool(AX_VDEC_GRP VdGrp, AX_VDEC_CHN VdChn, AX_POOL PoolId);
AXCL_EXPORT AX_S32 AXCL_VDEC_DetachPool(AX_VDEC_GRP VdGrp, AX_VDEC_CHN VdChn);

AXCL_EXPORT AX_S32 AXCL_VDEC_EnableChn(AX_VDEC_GRP VdGrp, AX_VDEC_CHN VdChn);
AXCL_EXPORT AX_S32 AXCL_VDEC_DisableChn(AX_VDEC_GRP VdGrp, AX_VDEC_CHN VdChn);

AXCL_EXPORT AX_S32 AXCL_VDEC_SetChnAttr(AX_VDEC_GRP VdGrp, AX_VDEC_CHN VdChn, const AX_VDEC_CHN_ATTR_T *pstVdChnAttr);
AXCL_EXPORT AX_S32 AXCL_VDEC_GetChnAttr(AX_VDEC_GRP VdGrp, AX_VDEC_CHN VdChn, AX_VDEC_CHN_ATTR_T *pstVdChnAttr);

AXCL_EXPORT AX_S32 AXCL_VDEC_JpegDecodeOneFrame(AX_VDEC_DEC_ONE_FRM_T *pstParam);

AXCL_EXPORT AX_S32 AXCL_VDEC_GetStreamBufInfo(AX_VDEC_GRP VdGrp, AX_VDEC_STREAM_BUF_INFO_T *pstStreamBufInfo);
AXCL_EXPORT AX_S32 AXCL_VDEC_GetVuiParam(AX_VDEC_GRP VdGrp, AX_VDEC_VUI_PARAM_T *pstVuiParam);

#ifdef __cplusplus
}
#endif

#endif /* __AXCL_VDEC_API_H__ */
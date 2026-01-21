/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef __OSD_CONFIG_H__
#define __OSD_CONFIG_H__

#include <map>
#include <vector>
#include "ElapsedTimer.hpp"
#include "GlobalDef.h"
#include "ax_base_type.h"
#include "string.h"
#define MAX_OSD_CHN_COUNT (3)
#define OSD_ALIGN_X_OFFSET (2)
#define OSD_ALIGN_Y_OFFSET (2)
#define OSD_ALIGN_WIDTH (2)
#define OSD_ALIGN_HEIGHT (2)
#define IS_VALID_CHANNEL(chn) ((chn) >= 0 && (chn) < MAX_WEB_CHANNEL_NUM)
#define IS_VALID_OSD_NUM(num) ((num) >= 0 && (num) <= AX_MAX_VO_NUM)

#define BASE_FONT_SIZE (16)
#define MAX_OSD_TIME_CHAR_LEN (32)
#define MAX_OSD_STR_CHAR_LEN (128)
#define MAX_OSD_WSTR_CHAR_LEN (256)

#define MAX_OSD_POINT_NUM (4)

#ifndef RANGE_CHECK
#define RANGE_CHECK(v, min, max) (((v) < (min)) ? 0 : ((v) > (max)) ? 0 : 1)
#endif

#define CHECK_POINTER(p)             \
    if (!p) {                        \
        return OSD_ERR_NULL_POINTER; \
    }

#define CHECK_CHANNEL(chn)                    \
    if (!IS_VALID_CHANNEL(chn)) {             \
        return OSD_ERR_INVALID_VIDEO_CHANNEL; \
    }

#define CHECK_OSD_NUMBER(num)         \
    if (!IS_VALID_OSD_NUM(num)) {     \
        return OSD_ERR_INVALID_PARAM; \
    }

#define CHECK_OSD_HANDLE(chn, handle)                     \
    if (!IS_VALID_OSD_NUM(handle)) {                      \
        return OSD_ERR_INVALID_HANDLE;                    \
    }                                                     \
    if (!RANGE_CHECK(handle, 0, m_nOsdCfgNum[chn] - 1)) { \
        return OSD_ERR_INVALID_HANDLE;                    \
    }

/* OSD ERROR */
typedef enum {
    OSD_SUCCESS = 0,
    OSD_ERR_NULL_POINTER,
    OSD_ERR_INVALID_HANDLE,
    OSD_ERR_INVALID_VIDEO_CHANNEL,
    OSD_ERR_INVALID_PARAM,
    OSD_ERR_MAX
} OSD_ERR_CODE_E;

/* OSD Align Type */
typedef enum {
    OSD_ALIGN_TYPE_LEFT_TOP,
    OSD_ALIGN_TYPE_RIGHT_TOP,
    OSD_ALIGN_TYPE_LEFT_BOTTOM,
    OSD_ALIGN_TYPE_RIGHT_BOTTOM,
    OSD_ALIGN_TYPE_MAX
} OSD_ALIGN_TYPE_E;

/* OSD Type */
typedef enum {
    OSD_TYPE_TIME,
    OSD_TYPE_PICTURE,
    OSD_TYPE_STRING_CHANNEL,
    OSD_TYPE_STRING_LOCATION,
    OSD_TYPE_PRIVACY,
    OSD_TYPE_RECT,
    OSD_TYPE_MAX
} OSD_TYPE_E;


typedef enum {
    OSD_PRIVACY_TYPE_LINE,
    OSD_PRIVACY_TYPE_RECT,
    OSD_PRIVACY_TYPE_POLYGON,
    OSD_PRIVACY_TYPE_MOSAIC,
    OSD_PRIVACY_TYPE_MAX
} OSD_PRIVACY_TYPE_E;


typedef struct _OSD_PIC_ATTR_T {
    AX_S32 nWidth;
    AX_S32 nHeight;
    AX_CHAR szFileName[128];
} OSD_PIC_ATTR_T;


typedef struct _OSD_STR_ATTR_T {
    AX_S32 nFontSize;
    AX_U32 nColor;
    AX_CHAR szStr[MAX_OSD_STR_CHAR_LEN];

} OSD_STR_ATTR_T;


typedef struct _OSD_TIME_ATTR_T {
    OSD_DATE_FORMAT_E eFormat;
    AX_U32 nFontSize;
    AX_U32 nColor;
} OSD_TIME_ATTR_T;

typedef struct _OSD_POINT_T {
    AX_S16 x;
    AX_S16 y;
} OSD_POINT_T;


typedef struct _OSD_PRIVACY_ATTR_T {
    OSD_PRIVACY_TYPE_E eType;
    AX_U32 nLineWidth;
    AX_U32 nColor;
    AX_BOOL bSolid;
    OSD_POINT_T tPt[MAX_OSD_POINT_NUM];
    AX_U32 nBaseWidth;
    AX_U32 nBaseHeight;
} OSD_PRIVACY_ATTR_T;

typedef struct {
    AX_U32 nLineWidth;
} OSD_RECT_ATTR_T;

/* OSD Config */
typedef struct _OSD_CFG_T {
    AX_BOOL bEnable{AX_FALSE};
    OSD_TYPE_E eType{OSD_TYPE_MAX};
    AX_BOOL bChanged{AX_FALSE};
    OSD_ALIGN_TYPE_E eAlign{OSD_ALIGN_TYPE_LEFT_TOP};
    AX_S32 nBoundaryX;
    AX_S32 nBoundaryY;
    AX_S32 nBoundaryW;
    AX_S32 nBoundaryH;
    AX_S32 nZIndex;
    union {
        OSD_PIC_ATTR_T tPicAttr;
        OSD_STR_ATTR_T tStrAttr;
        OSD_TIME_ATTR_T tTimeAttr;
        OSD_PRIVACY_ATTR_T tPrivacyAttr;
        OSD_RECT_ATTR_T tRectAttr;
    };
} OSD_CFG_T;

typedef struct _OSD_SENSOR_CONFIG_T {
    AX_BOOL bEnable;
    std::map<AX_U8, std::map<AX_U8, std::vector<OSD_CFG_T>>> mapGrpChnConfig;
} OSD_SENSOR_CONFIG_T;

typedef struct _OSD_FONT_STYLE {
    AX_U32 nMagic;
    AX_U32 nTimeFontSize;
    AX_U32 nRectLineWidth;
    AX_U32 nBoundaryX;
} OSD_FONT_STYLE;

class COSDStyle : public CAXSingleton<COSDStyle> {
    friend class CAXSingleton<COSDStyle>;

public:
    virtual AX_BOOL InitOnce() override;
    AX_U32 GetTimeFontSize(AX_U32 nWidth, AX_U32 nHeight);
    AX_U32 GetRectLineWidth(AX_U32 nWidth, AX_U32 nHeight);
    AX_U32 GetBoundaryX(AX_U32 nWidth, AX_U32 nHeight);

private:
    COSDStyle() = default;
    ~COSDStyle() = default;

private:
    /* must be ordered as nMagic */
    const OSD_FONT_STYLE g_arrOsdStyle[16] = {
        {3840 * 2160, 128, 4, 56}, {3072 * 2048, 128, 4, 56}, {3072 * 1728, 128, 4, 48}, {2624 * 1944, 96, 4, 48}, {2688 * 1520, 96, 4, 48},
        {2048 * 1536, 96, 4, 48},  {2304 * 1296, 96, 4, 48},  {1920 * 1080, 48, 2, 24},  {1280 * 720, 48, 2, 20},  {1024 * 768, 32, 2, 16},
        {720 * 576, 32, 2, 12},    {704 * 576, 32, 2, 12},    {640 * 480, 16, 2, 8},     {384 * 288, 16, 2, 2}};

    AX_U32 GetOsdStyleIndex(AX_U32 nWidth, AX_U32 nHeight);
};
#endif  // __OSD_CONFIG_H__

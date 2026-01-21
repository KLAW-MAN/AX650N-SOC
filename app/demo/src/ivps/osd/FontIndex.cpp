/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include "FontIndex.h"
#include "FontEn16.h"
#include "FontZh16.h"


#ifndef FONT_USE_FREETYPE

AX_U16 GetZhGlyphIndex(AX_U16 nUnicode) {
    AX_U32 nCount = sizeof(g_fontZh16Index) / sizeof(g_fontZh16Index[0]);
    for (AX_U32 i = 0; i < nCount; i++) {
        if (g_fontZh16Index[i].nUnicode == nUnicode) {
            return g_fontZh16Index[i].nIndex;
        }
    }
    return 0;
}

AX_U16 GetEnGlyphIndex(AX_U16 nUnicode) {
    AX_U32 nCount = sizeof(g_fontEn16Index) / sizeof(g_fontEn16Index[0]);
    for (AX_U32 i = 0; i < nCount; i++) {
        if (g_fontEn16Index[i].nUnicode == nUnicode) {
            return g_fontEn16Index[i].nIndex;
        } else if (g_fontEn16Index[i].nUnicode > nUnicode) {
            break;
        }
    }

    return 0;
}

AX_S32 GetFontBitmap(AX_U16 nUnicode, FONT_BITMAP_T &bmp) {
    if (nUnicode <= 0x7F) {
        AX_U16 nInd = GetEnGlyphIndex(nUnicode);
        bmp.nWidth = 8;
        bmp.nHeight = 16;
        bmp.pBuffer = (AX_U8 *)(g_fontEn16Glyphs + (bmp.nWidth / 8 * bmp.nHeight) * nInd);
    } else {
        AX_U16 nInd = GetZhGlyphIndex(nUnicode);
        bmp.nWidth = 16;
        bmp.nHeight = 16;
        bmp.pBuffer = (AX_U8 *)(g_fontZh16Glyphs + (bmp.nWidth / 8 * bmp.nHeight) * nInd);
    }

    return 0;
}

#endif //  FONT_USE_FREETYPE

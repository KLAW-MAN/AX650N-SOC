/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include "OsdConfig.h"

AX_BOOL COSDStyle::InitOnce() {
    return AX_TRUE;
}

AX_U32 COSDStyle::GetTimeFontSize(AX_U32 nWidth, AX_U32 nHeight) {
    AX_U32 nIndex = GetOsdStyleIndex(nWidth, nHeight);
    return g_arrOsdStyle[nIndex].nTimeFontSize;
}

AX_U32 COSDStyle::GetRectLineWidth(AX_U32 nWidth, AX_U32 nHeight) {
    AX_U32 nIndex = GetOsdStyleIndex(nWidth, nHeight);
    return g_arrOsdStyle[nIndex].nRectLineWidth;
}

AX_U32 COSDStyle::GetBoundaryX(AX_U32 nWidth, AX_U32 nHeight) {
    AX_U32 nIndex = GetOsdStyleIndex(nWidth, nHeight);
    return g_arrOsdStyle[nIndex].nBoundaryX;
}

AX_U32 COSDStyle::GetOsdStyleIndex(AX_U32 nWidth, AX_U32 nHeight) {
    AX_U32 nMagic = nWidth * nHeight;
    AX_U32 nCount = sizeof(g_arrOsdStyle) / sizeof(g_arrOsdStyle[0]);
    for (AX_U32 i = 0; i < nCount; i++) {
        if (nMagic >= g_arrOsdStyle[i].nMagic) {
            return i;
        }
    }
    return nCount - 1;
}

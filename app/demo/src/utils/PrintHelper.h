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

#include "AXSingleton.h"
#include "AXThread.hpp"
#include "AppLogApi.h"
#include "ElapsedTimer.hpp"
#include "GlobalDef.h"
#include "OptionHelper.h"
#include "ax_isp_api.h"
#include "ax_vin_api.h"

#define PRINT_INTERVAL (10)
#define MAX_CHANNEL_NUM (32)

typedef enum _PRINT_HELPER_MOD_E {
    E_PH_MOD_VENC = 0,
    E_PH_MOD_JENC,
    E_PH_MOD_MJENC,
    E_PH_MOD_VIN_YUV,
    E_PH_MOD_VIN_RAW,
    /* pending */
    E_PH_MOD_MAX
} PRINT_HELPER_MOD_E;

typedef struct _VENC_PRINT_INFO_T {
    AX_U32 nVencTargetFPS[MAX_CHANNEL_NUM];
    AX_U32 nVencReceivedFrames[MAX_CHANNEL_NUM];
    AX_U32 nVencPeriodFrames[MAX_CHANNEL_NUM];
    AX_U32 nVencFinalFrames[MAX_CHANNEL_NUM];

    _VENC_PRINT_INFO_T() {
        memset(this, 0, sizeof(_VENC_PRINT_INFO_T));
    }

    AX_VOID SetTargetFPS(AX_U32 nChn, AX_U32 nTargetFPS) {
        nVencTargetFPS[nChn] = nTargetFPS;
    }

    AX_VOID Add(AX_U32 nChn) {
        if (nChn < 0 || nChn >= MAX_CHANNEL_NUM) {
            return;
        }

        if (nVencReceivedFrames[nChn] > 0) {
            /* Ignore the header frame */
            nVencPeriodFrames[nChn]++;
        }
        nVencReceivedFrames[nChn]++;
    }

    AX_VOID Reset(AX_U32 nChn) {
        nVencPeriodFrames[nChn] = 0;
        nVencReceivedFrames[nChn] = 0;
    }

    AX_VOID ClearAll() {
        memset(this, 0, sizeof(_VENC_PRINT_INFO_T));
    }

    AX_VOID Print() {
        for (AX_U32 i = 0; i < MAX_CHANNEL_NUM; i++) {
            if (nVencReceivedFrames[i] > 0) {
                LOG_M_C("VENC", "[%d] fps %5.2f, recv %d", i, nVencPeriodFrames[i] * 1.0 / PRINT_INTERVAL,
                        nVencReceivedFrames[i] > 0 ? nVencReceivedFrames[i] - 1 : 0); /* Ignore the header frame */
                nVencFinalFrames[i] = nVencPeriodFrames[i];
                nVencPeriodFrames[i] = 0;
            }
        }
    }

    AX_BOOL SLT_CheckResult() {
        /* Check framerate of main path */
        AX_F64 fFPS = nVencPeriodFrames[0] * 1.0 / COptionHelper::GetInstance()->GetSLTFpsCheckFreq();
        if (abs(fFPS - nVencTargetFPS[0]) > COptionHelper::GetInstance()->GetSLTFpsDiff()) {
            printf("[ERROR] FRTDemo-ITS: fps = %.2f\n", fFPS);
            return AX_FALSE;
        } else {
            AX_CHAR szHMS[16] = {0};
            LOG_M_C("VENC", "[%d] [%s] fps %5.2f, recv %d", 0, CElapsedTimer::GetInstance()->GetLocalTime(&szHMS[0], 16),
                    nVencPeriodFrames[0] * 1.0 / COptionHelper::GetInstance()->GetSLTFpsCheckFreq(),
                    nVencReceivedFrames[0] > 0 ? nVencReceivedFrames[0] - 1 : 0); /* Ignore the header frame */

            nVencPeriodFrames[0] = 0;
        }

        return AX_TRUE;
    }

    AX_VOID FinalPrint() {
        for (AX_U32 i = 0; i < MAX_CHANNEL_NUM; i++) {
            if (nVencReceivedFrames[i] > 0) {
                for (AX_U8 j = 0; j < 3; j++) {
                    LOG_M_C("Final VENC", "[%d] fps %5.2f", i, nVencFinalFrames[i] * 1.0 / PRINT_INTERVAL); /* Ignore the header frame */
                }
                nVencFinalFrames[i] = 0;
            }
        }
    }
} VENC_PRINT_INFO_T;

typedef struct _JENC_PRINT_INFO_T {
    AX_U32 nJencReceivedFrames[MAX_CHANNEL_NUM];
    AX_U32 nJencPeriodFrames[MAX_CHANNEL_NUM];
    AX_U32 nJencFinalFrames[MAX_CHANNEL_NUM];

    _JENC_PRINT_INFO_T() {
        memset(this, 0, sizeof(_JENC_PRINT_INFO_T));
    }

    AX_VOID Add(AX_U32 nChn) {
        if (nChn < 0 || nChn >= MAX_CHANNEL_NUM) {
            return;
        }

        if (nJencReceivedFrames[nChn] > 0) {
            /* Ignore the header frame */
            nJencPeriodFrames[nChn]++;
        }
        nJencReceivedFrames[nChn]++;
    }

    AX_VOID Reset(AX_U32 nChn) {
        nJencPeriodFrames[nChn] = 0;
        nJencReceivedFrames[nChn] = 0;
    }

    AX_VOID ClearAll() {
        memset(this, 0, sizeof(_JENC_PRINT_INFO_T));
    }

    AX_VOID Print() {
        for (AX_U32 i = 0; i < MAX_CHANNEL_NUM; i++) {
            if (nJencReceivedFrames[i] > 0) {
                LOG_M_C("JENC", "[%d] fps %5.2f, recv %d", i, nJencPeriodFrames[i] * 1.0 / PRINT_INTERVAL,
                        nJencReceivedFrames[i] > 0 ? nJencReceivedFrames[i] - 1 : 0); /* Ignore the header frame */
                nJencFinalFrames[i] = nJencPeriodFrames[i];
                nJencPeriodFrames[i] = 0;
            }
        }
    }

    AX_VOID FinalPrint() {
        for (AX_U32 i = 0; i < MAX_CHANNEL_NUM; i++) {
            if (nJencReceivedFrames[i] > 0) {
                for (AX_U8 j = 0; j < 3; j++) {
                    LOG_M_C("Final JENC", "[%d] fps %5.2f", i, nJencFinalFrames[i] * 1.0 / PRINT_INTERVAL); /* Ignore the header frame */
                }
                nJencFinalFrames[i] = 0;
            }
        }
    }

} JENC_PRINT_INFO_T;

typedef struct _MJENC_PRINT_INFO_T {
    AX_U32 nMJencReceivedFrames[MAX_CHANNEL_NUM];
    AX_U32 nMJencPeriodFrames[MAX_CHANNEL_NUM];
    AX_U32 nMJencFinalFrames[MAX_CHANNEL_NUM];

    _MJENC_PRINT_INFO_T() {
        memset(this, 0, sizeof(_MJENC_PRINT_INFO_T));
    }

    AX_VOID Add(AX_U32 nChn) {
        if (nChn < 0 || nChn >= MAX_CHANNEL_NUM) {
            return;
        }

        if (nMJencReceivedFrames[nChn] > 0) {
            /* Ignore the header frame */
            nMJencPeriodFrames[nChn]++;
        }
        nMJencReceivedFrames[nChn]++;
    }

    AX_VOID Reset(AX_U32 nChn) {
        nMJencPeriodFrames[nChn] = 0;
        nMJencReceivedFrames[nChn] = 0;
    }

    AX_VOID ClearAll() {
        memset(this, 0, sizeof(_MJENC_PRINT_INFO_T));
    }

    AX_VOID Print() {
        for (AX_U32 i = 0; i < MAX_CHANNEL_NUM; i++) {
            if (nMJencReceivedFrames[i] > 0) {
                LOG_M_C("MJPG", "[%d] fps %5.2f, recv %d", i, nMJencPeriodFrames[i] * 1.0 / PRINT_INTERVAL,
                        nMJencReceivedFrames[i] > 0 ? nMJencReceivedFrames[i] - 1 : 0); /* Ignore the header frame */
                nMJencFinalFrames[i] = nMJencPeriodFrames[i];
                nMJencPeriodFrames[i] = 0;
            }
        }
    }

    AX_VOID FinalPrint() {
        for (AX_U32 i = 0; i < MAX_CHANNEL_NUM; i++) {
            if (nMJencReceivedFrames[i] > 0) {
                for (AX_U8 j = 0; j < 3; j++) {
                    LOG_M_C("Final MJPG", "[%d] fps %5.2f", i, nMJencFinalFrames[i] * 1.0 / PRINT_INTERVAL); /* Ignore the header frame */
                }
                nMJencFinalFrames[i] = 0;
            }
        }
    }
} MJENC_PRINT_INFO_T;

typedef struct _VIN_YUV_PRINT_INFO_T {
    AX_S64 arrYUVReceivedFrames[AX_VIN_MAX_PIPE_NUM][AX_VIN_CHN_ID_MAX];
    AX_S64 arrYUVPeriodFrames[AX_VIN_MAX_PIPE_NUM][AX_VIN_CHN_ID_MAX];
    AX_S64 arrYUVFinalFrames[AX_VIN_MAX_PIPE_NUM][AX_VIN_CHN_ID_MAX];

    _VIN_YUV_PRINT_INFO_T() {
        memset(this, 0, sizeof(_VIN_YUV_PRINT_INFO_T));
    }

    AX_VOID Add(AX_U32 nPipe, AX_U32 nChn) {
        if (nPipe >= AX_VIN_MAX_PIPE_NUM || nChn >= AX_VIN_CHN_ID_MAX) {
            return;
        }

        if (arrYUVReceivedFrames[nPipe][nChn] > 0) {
            /* Ignore the header frame */
            arrYUVPeriodFrames[nPipe][nChn]++;
        }
        arrYUVReceivedFrames[nPipe][nChn]++;
    }

    AX_VOID Reset(AX_U32 nPipe, AX_U32 nChn) {
        if (nPipe >= AX_VIN_MAX_PIPE_NUM || nChn >= AX_VIN_CHN_ID_MAX) {
            return;
        }

        arrYUVReceivedFrames[nPipe][nChn] = 0;
        arrYUVPeriodFrames[nPipe][nChn] = 0;
    }

    AX_VOID ClearAll() {
        memset(this, 0, sizeof(_VIN_YUV_PRINT_INFO_T));
    }

    AX_VOID Print() {
        for (AX_U32 i = 0; i < AX_VIN_MAX_PIPE_NUM; i++) {
            for (AX_U32 j = 0; j < AX_VIN_CHN_ID_MAX; j++) {
                if (arrYUVReceivedFrames[i][j] > 1) {
                    LOG_M_C("VIN_YUV", "[%d][%d] fps %5.2f, recv %d",
                             i, j, arrYUVPeriodFrames[i][j] * 1.0 / PRINT_INTERVAL,
                             arrYUVReceivedFrames[i][j] - 1); /* Ignore the header frame */
                    arrYUVFinalFrames[i][j] = arrYUVPeriodFrames[i][j];
                    arrYUVPeriodFrames[i][j] = 0;
                }
            }
        }
    }

    AX_VOID FinalPrint() {
        for (AX_U32 i = 0; i < AX_VIN_MAX_PIPE_NUM; i++) {
            for (AX_U32 j = 0; j < AX_VIN_CHN_ID_MAX; j++) {
                if (arrYUVReceivedFrames[i][j] > 1) {
                    LOG_M_C("Final VIN_YUV", "[%d][%d] fps %5.2f",
                             i, j, arrYUVFinalFrames[i][j] * 1.0 / PRINT_INTERVAL); /* Ignore the header frame */
                    arrYUVFinalFrames[i][j] = 0;
                }
            }
        }
    }
} VIN_YUV_PRINT_INFO_T;

typedef struct _VIN_RAW_PRINT_INFO_T {
    AX_S64 arrRawReceivedFrames[AX_VIN_MAX_PIPE_NUM];
    AX_S64 arrRawPeriodFrames[AX_VIN_MAX_PIPE_NUM];
    AX_S64 arrRawFinalFrames[AX_VIN_MAX_PIPE_NUM];

    _VIN_RAW_PRINT_INFO_T() {
        memset(this, 0, sizeof(_VIN_RAW_PRINT_INFO_T));
    }

    AX_VOID Add(AX_U32 nPipe) {
        if (nPipe >= AX_VIN_MAX_PIPE_NUM) {
            return;
        }

        if (arrRawReceivedFrames[nPipe] > 0) {
            /* Ignore the header frame */
            arrRawPeriodFrames[nPipe]++;
        }
        arrRawReceivedFrames[nPipe]++;
    }

    AX_VOID Reset(AX_U32 nPipe) {
        if (nPipe >= AX_VIN_MAX_PIPE_NUM) {
            return;
        }

        arrRawReceivedFrames[nPipe] = 0;
        arrRawPeriodFrames[nPipe] = 0;
    }

    AX_VOID ClearAll() {
        memset(this, 0, sizeof(_VIN_RAW_PRINT_INFO_T));
    }

    AX_VOID Print() {
        for (AX_U32 i = 0; i < AX_VIN_MAX_PIPE_NUM; i++) {
            if (arrRawReceivedFrames[i] > 1) {
                LOG_M_C("VIN_RAW", "[%d] fps %5.2f, recv %d",
                        i, arrRawPeriodFrames[i] * 1.0 / PRINT_INTERVAL,
                        arrRawReceivedFrames[i] - 1); /* Ignore the header frame */
                arrRawFinalFrames[i] = arrRawPeriodFrames[i];
                arrRawPeriodFrames[i] = 0;
            }
        }
    }

    AX_VOID FinalPrint() {
        for (AX_U32 i = 0; i < AX_VIN_MAX_PIPE_NUM; i++) {
            if (arrRawReceivedFrames[i] > 1) {
                LOG_M_C("Final VIN_RAW", "[%d] fps %5.2f", i, arrRawFinalFrames[i] * 1.0 / PRINT_INTERVAL); /* Ignore the header frame */
                arrRawFinalFrames[i] = 0;
            }
        }
    }
} VIN_RAW_PRINT_INFO_T;

/**
 * (frame)statistics info printer
 */
class CPrintHelper final : public CAXSingleton<CPrintHelper> {
    friend class CAXSingleton<CPrintHelper>;

public:
    AX_VOID Start();
    AX_VOID Stop();
    AX_VOID Add(PRINT_HELPER_MOD_E eModType, AX_U32 nChn);
    AX_VOID Reset(PRINT_HELPER_MOD_E eModType, AX_U32 nChn);
    AX_VOID Add(PRINT_HELPER_MOD_E eModType, AX_U32 nPipe, AX_U32 nChn);
    AX_VOID Reset(PRINT_HELPER_MOD_E eModType, AX_U32 nPipe, AX_U32 nChn);
    AX_VOID SetSLTTargetFPS(PRINT_HELPER_MOD_E eModType, AX_U32 nChn, AX_U32 nTargetFPS);
    AX_S32 GetSLTResult() const;
    AX_VOID SetSLTResult(AX_S32 nRet);
    AX_S32 GetVencFramerate(AX_U32 nChn);

private:
    CPrintHelper(AX_VOID) = default;
    ~CPrintHelper(AX_VOID) = default;

protected:
    AX_VOID PrintThreadFunc(CPrintHelper* pCaller);
    AX_VOID SLTThreadFunc(CPrintHelper* pCaller);

private:
    AX_BOOL m_bEnableStart{AX_FALSE};
    AX_U64 m_nTickStart{0};
    AX_U64 m_nTickEnd{0};

    std::thread m_hPrintThread;
    AX_BOOL m_bPrintThreadWorking{AX_FALSE};
    AX_BOOL m_bSLTThreadWorking{AX_FALSE};
    AX_S32 m_nSLTResult{-1}; /* -1: Init; 0: OK; 1: Fail; 2: Reach Timeout */

    /* statistics info (pending) */
    VENC_PRINT_INFO_T m_stVencStatisticsInfo;
    JENC_PRINT_INFO_T m_stJencStatisticsInfo;
    MJENC_PRINT_INFO_T m_stMJencStatisticsInfo;
    VIN_YUV_PRINT_INFO_T m_stVinYUVStatisticsInfo;
    VIN_RAW_PRINT_INFO_T m_stVinRawStatisticsInfo;
};

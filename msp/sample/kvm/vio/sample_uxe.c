/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include <string.h>
#include "ax_mipi_rx_api.h"
#include "sample_uxe.h"
#include "sample_log.h"

static AX_UXE_T g_tUxeConfig;
static AX_MIPI_RX_DEV_T g_tMipiRxDev;
static AX_SNS_ATTR_T g_tSnsAttr;
static AX_SNS_CLK_ATTR_T g_tSnsClkAttr;
static AX_VIN_DEV_ATTR_T g_tDevAttr;
static AX_VIN_PIPE_ATTR_T g_tPipeAttr;
static AX_VIN_CHN_ATTR_T g_tChnAttr;

static AX_VOID InitMipiRxAttr(const AX_UXE_T* pstUxe, AX_MIPI_RX_DEV_T* pstMipiRxDev)
{
    /* Referenced by AX_MIPI_RX_SetAttr */
    pstMipiRxDev->eInputMode = AX_INPUT_MODE_MIPI;
    pstMipiRxDev->tMipiAttr.ePhyMode = AX_MIPI_PHY_TYPE_DPHY;
    pstMipiRxDev->tMipiAttr.nDataRate = pstUxe->nMHz;
    if (pstUxe->enVinFmt == AX_FORMAT_BAYER_RAW_8BPP)
    {
        pstMipiRxDev->tMipiAttr.eLaneNum = AX_MIPI_DATA_LANE_8;
    }
    else if (pstUxe->enVinFmt == AX_FORMAT_YUV422_SEMIPLANAR_10BIT_P010)
    {
        pstMipiRxDev->tMipiAttr.eLaneNum = AX_MIPI_DATA_LANE_8;
    }
    else
    {
        pstMipiRxDev->tMipiAttr.eLaneNum = AX_MIPI_DATA_LANE_4;
    }
    KVM_LOG_I("input signal: %dx%d %d fps, mipi data rate: %dM\n", pstUxe->nW, pstUxe->nH, pstUxe->nFps, pstMipiRxDev->tMipiAttr.nDataRate);

    pstMipiRxDev->tMipiAttr.nDataLaneMap[0] = 0;
    pstMipiRxDev->tMipiAttr.nDataLaneMap[1] = 1;
    pstMipiRxDev->tMipiAttr.nDataLaneMap[2] = 2;
    pstMipiRxDev->tMipiAttr.nDataLaneMap[3] = 3;
    pstMipiRxDev->tMipiAttr.nDataLaneMap[4] = -1;
    pstMipiRxDev->tMipiAttr.nDataLaneMap[5] = -1;
    pstMipiRxDev->tMipiAttr.nDataLaneMap[6] = -1;
    pstMipiRxDev->tMipiAttr.nDataLaneMap[7] = -1;
    pstMipiRxDev->tMipiAttr.nClkLane[0] = 1;
    pstMipiRxDev->tMipiAttr.nClkLane[1] = 0;
}

static AX_VOID InitSnsAttr(const AX_UXE_T* pstUxe, AX_SNS_ATTR_T* pstSnsAttr)
{
    /* Referenced by AX_VIN_SetSnsAttr */
    pstSnsAttr->nWidth = pstUxe->nW;
    pstSnsAttr->nHeight = pstUxe->nH;
    pstSnsAttr->fFrameRate = (AX_F32)pstUxe->nFps;
    pstSnsAttr->eSnsMode = AX_SNS_LINEAR_MODE;
    if (pstUxe->enVinFmt == AX_FORMAT_BAYER_RAW_8BPP)
    {
        pstSnsAttr->eRawType = AX_RT_RAW8;
    }
    else
    {
        pstSnsAttr->eRawType = AX_RT_YUV422;
    }
    pstSnsAttr->eBayerPattern = AX_BP_RGGB;
    pstSnsAttr->bTestPatternEnable = AX_FALSE;
}

static AX_VOID InitSnsClkAttr(AX_IMG_FORMAT_E enImgFmt, AX_SNS_CLK_ATTR_T* pstSnsClkAttr)
{
    /* Referenced by AX_VIN_OpenSnsClk */
    if (enImgFmt == AX_FORMAT_BAYER_RAW_8BPP)
    {
        pstSnsClkAttr->nSnsClkIdx = 0;
    }
    else
    {
        pstSnsClkAttr->nSnsClkIdx = 1;
    }
    pstSnsClkAttr->eSnsClkRate = AX_SNS_CLK_24M;
}

static AX_VOID InitDevAttr(AX_UXE_T* pstUxe, AX_VIN_DEV_ATTR_T* pstDevAttr)
{
    /* Referenced by AX_VIN_SetDevAttr */
    pstDevAttr->bImgDataEnable = AX_TRUE;
    pstDevAttr->bNonImgDataEnable = AX_FALSE;
    pstDevAttr->eDevMode = AX_VIN_DEV_OFFLINE;
    if (pstUxe->enVinFmt == AX_FORMAT_BAYER_RAW_8BPP)
    {
        pstDevAttr->eSnsIntfType = AX_SNS_INTF_TYPE_MIPI_RAW;
        pstDevAttr->ePixelFmt = AX_FORMAT_BAYER_RAW_8BPP;
        if (g_tMipiRxDev.tMipiAttr.nDataRate >= 1200)
        {
            /*
                SIF freq 624M for 2pixel/1, max datarate = 624 * 2
                if exceeds, should be divided into several parts to recv.
                Here divide to 2x2 parts for example.
            */

            pstDevAttr->eDevWorkMode = AX_VIN_DEV_WORK_MODE_4MULTIPLEX;
            pstDevAttr->tDevImgRgn[0].nStartX = 0;
            pstDevAttr->tDevImgRgn[0].nStartY = 0;
            pstDevAttr->tDevImgRgn[0].nWidth = pstUxe->nW * 3 / 2;
            pstDevAttr->tDevImgRgn[0].nHeight = pstUxe->nH / 2;

            pstDevAttr->tDevImgRgn[1].nStartX = pstUxe->nW * 3 / 2;
            pstDevAttr->tDevImgRgn[1].nStartY = 0;
            pstDevAttr->tDevImgRgn[1].nWidth = pstUxe->nW * 3 / 2;
            pstDevAttr->tDevImgRgn[1].nHeight = pstUxe->nH / 2;

            pstDevAttr->tDevImgRgn[2].nStartX = 0;
            pstDevAttr->tDevImgRgn[2].nStartY = pstUxe->nH / 2;
            pstDevAttr->tDevImgRgn[2].nWidth = pstUxe->nW * 3 / 2;
            pstDevAttr->tDevImgRgn[2].nHeight = pstUxe->nH / 2;

            pstDevAttr->tDevImgRgn[3].nStartX = pstUxe->nW * 3 / 2;
            pstDevAttr->tDevImgRgn[3].nStartY = pstUxe->nH / 2;
            pstDevAttr->tDevImgRgn[3].nWidth = pstUxe->nW * 3 / 2;
            pstDevAttr->tDevImgRgn[3].nHeight = pstUxe->nH / 2;

            for (AX_U8 i = 0; i < AX_HDR_CHN_NUM; i++)
            {
                pstDevAttr->nWidthStride[i] = pstUxe->nW * 3;
            }

            /* configure the attribute of early reporting of frame interrupts */
            pstUxe->tDevFrmIntAttr.bImgRgnIntEn[0] = AX_FALSE;
            pstUxe->tDevFrmIntAttr.bImgRgnIntEn[1] = AX_FALSE;
            pstUxe->tDevFrmIntAttr.bImgRgnIntEn[2] = AX_FALSE;
            pstUxe->tDevFrmIntAttr.bImgRgnIntEn[3] = AX_TRUE;
        }
        else
        {
            pstDevAttr->eDevWorkMode = AX_VIN_DEV_WORK_MODE_1MULTIPLEX;
            pstDevAttr->tDevImgRgn[0].nStartX = 0;
            pstDevAttr->tDevImgRgn[0].nStartY = 0;
            pstDevAttr->tDevImgRgn[0].nWidth = pstUxe->nW * 3;
            pstDevAttr->tDevImgRgn[0].nHeight = pstUxe->nH;
            pstDevAttr->nWidthStride[0] = pstUxe->nW * 3;
        }
    }
    else
    {
        pstDevAttr->eSnsIntfType = AX_SNS_INTF_TYPE_MIPI_YUV;
        pstDevAttr->ePixelFmt = pstUxe->enVinFmt;
        pstDevAttr->nConvYuv422To420En = 1;
        pstDevAttr->nConvFactor = 2;

        pstDevAttr->eDevWorkMode = AX_VIN_DEV_WORK_MODE_4MULTIPLEX;
        pstUxe->tDevFrmIntAttr.bImgRgnIntEn[0] = AX_TRUE;
        pstUxe->tDevFrmIntAttr.bImgRgnIntEn[1] = AX_TRUE;
        pstUxe->tDevFrmIntAttr.bImgRgnIntEn[2] = AX_TRUE;
        pstUxe->tDevFrmIntAttr.bImgRgnIntEn[3] = AX_TRUE;

        if (pstUxe->enVinFmt == AX_FORMAT_YUV422_SEMIPLANAR_10BIT_P010)
        {
            pstDevAttr->tMipiIntfAttr.szImgVc[0] = 0;
            pstDevAttr->tMipiIntfAttr.szImgDt[0] = AX_MIPI_CSI_DT_YUV422_10BIT;
            pstDevAttr->tMipiIntfAttr.szImgDt[1] = AX_MIPI_CSI_DT_YUV422_10BIT;
            pstDevAttr->tMipiIntfAttr.szImgDt[2] = AX_MIPI_CSI_DT_YUV422_10BIT;
            pstDevAttr->tMipiIntfAttr.szImgDt[3] = AX_MIPI_CSI_DT_YUV422_10BIT;
        } else if (pstUxe->enVinFmt == AX_FORMAT_YUV422_INTERLEAVED_YUYV)
        {
            pstDevAttr->tMipiIntfAttr.szImgVc[0] = 0;
            pstDevAttr->tMipiIntfAttr.szImgDt[0] = AX_MIPI_CSI_DT_YUV422_8BIT;
        }

        /* ROI config */
        pstDevAttr->tDevImgRgn[0].nStartX = 0;
        pstDevAttr->tDevImgRgn[0].nStartY = 0;
        pstDevAttr->tDevImgRgn[0].nWidth = pstUxe->nW;
        pstDevAttr->tDevImgRgn[0].nHeight = pstUxe->nH / 4;
        if (pstUxe->enVinFmt == AX_FORMAT_YUV422_SEMIPLANAR_10BIT_P010)
        {
            pstDevAttr->nWidthStride[0] = pstUxe->nW * 2;
        } else
        {
            pstDevAttr->nWidthStride[0] = pstUxe->nW;
        }

        pstDevAttr->tDevImgRgn[1].nStartX = 0;
        pstDevAttr->tDevImgRgn[1].nStartY = pstUxe->nH / 4 * 1;
        pstDevAttr->tDevImgRgn[1].nWidth = pstUxe->nW;
        pstDevAttr->tDevImgRgn[1].nHeight = pstUxe->nH / 4;
        if (pstUxe->enVinFmt == AX_FORMAT_YUV422_SEMIPLANAR_10BIT_P010)
        {
            pstDevAttr->nWidthStride[1] = pstUxe->nW * 2;
        } else
        {
            pstDevAttr->nWidthStride[1] = pstUxe->nW;
        }

        pstDevAttr->tDevImgRgn[2].nStartX = 0;
        pstDevAttr->tDevImgRgn[2].nStartY = pstUxe->nH / 4 * 2;
        pstDevAttr->tDevImgRgn[2].nWidth = pstUxe->nW;
        pstDevAttr->tDevImgRgn[2].nHeight = pstUxe->nH / 4;
        if (pstUxe->enVinFmt == AX_FORMAT_YUV422_SEMIPLANAR_10BIT_P010)
        {
            pstDevAttr->nWidthStride[2] = pstUxe->nW * 2;
        } else
        {
            pstDevAttr->nWidthStride[2] = pstUxe->nW;
        }

        pstDevAttr->tDevImgRgn[3].nStartX = 0;
        pstDevAttr->tDevImgRgn[3].nStartY = pstUxe->nH / 4 * 3;
        pstDevAttr->tDevImgRgn[3].nWidth = pstUxe->nW;
        pstDevAttr->tDevImgRgn[3].nHeight = pstUxe->nH / 4;
        if (pstUxe->enVinFmt == AX_FORMAT_YUV422_SEMIPLANAR_10BIT_P010)
        {
            pstDevAttr->nWidthStride[3] = pstUxe->nW * 2;
        } else
        {
            pstDevAttr->nWidthStride[3] = pstUxe->nW;
        }
    }

    pstDevAttr->eBayerPattern = AX_BP_RGGB;
    pstDevAttr->eSnsMode = AX_SNS_LINEAR_MODE;
    pstDevAttr->eSnsOutputMode = AX_SNS_NORMAL;
    pstDevAttr->tCompressInfo.enCompressMode = AX_COMPRESS_MODE_NONE;
    pstDevAttr->tCompressInfo.u32CompressLevel = 0;
    pstDevAttr->tFrameRateCtrl.fSrcFrameRate = AX_INVALID_FRMRATE;
    pstDevAttr->tFrameRateCtrl.fDstFrameRate = AX_INVALID_FRMRATE;
}

static AX_VOID InitPipeAttr(const AX_UXE_T* pstUxe, AX_VIN_PIPE_ATTR_T* pstPipeAttr)
{
    AX_VIN_PIPE_ATTR_T tPipeAttr =
    {
        .ePipeWorkMode = AX_VIN_PIPE_NORMAL_MODE0,
        .tPipeImgRgn = {0, 0, pstUxe->nW, pstUxe->nH},
        .eBayerPattern = AX_BP_RGGB,
        .ePixelFmt = AX_FORMAT_BAYER_RAW_10BPP,
        .eSnsMode = AX_SNS_LINEAR_MODE,
        .bAiIspEnable = AX_FALSE,
        .tCompressInfo.enCompressMode = AX_COMPRESS_MODE_NONE,
        .tCompressInfo.u32CompressLevel = 0,
        .tNrAttr = {{AX_FALSE, {AX_COMPRESS_MODE_NONE, 0}}, {AX_FALSE, {AX_COMPRESS_MODE_NONE, 0}}},
        .tMotionAttr.bMotionEst = AX_FALSE
    };

    *pstPipeAttr = tPipeAttr;
}

static AX_VOID InitChnAttr(const AX_UXE_T* pstUxe, AX_VIN_CHN_ATTR_T* pstChnAttr)
{
    AX_VIN_CHN_ATTR_T arrChnAttr =
    {
        .nWidth = pstUxe->nW,
        .nHeight = pstUxe->nH,
        .nWidthStride = pstUxe->nW,
        .eImgFormat = AX_FORMAT_YUV420_SEMIPLANAR,
        .nDepth = 3,
        .tCompressInfo = {AX_COMPRESS_MODE_NONE, 0},
        .tFrameRateCtrl = {AX_INVALID_FRMRATE, AX_INVALID_FRMRATE}
    };

    *pstChnAttr = arrChnAttr;
}


AX_BOOL SAMPLE_UXE_Init(AX_IMG_FORMAT_E enVinFmt, AX_U32 nW, AX_U32 nH, AX_U8 nFps, AX_U16 nMHz)
{
    AX_S32 ret = 0;

    ret = AX_VIN_Init();
    if (0 != ret)
    {
        KVM_LOG_E("AX_VIN_Init fail, ret = 0x%x\n", ret);
        return AX_FALSE;
    }

    ret = AX_MIPI_RX_Init();
    if (0 != ret)
    {
        KVM_LOG_E("AX_MIPI_RX_Init fail, ret = 0x%x\n", ret);
        return AX_FALSE;
    }

    AX_UXE_T tUxeConfig =
    {
        .nDevId = 0,
        .enVinFmt = enVinFmt,
        .tDevFrmIntAttr = {.bImgRgnIntEn = {0}},
        .nW = nW,
        .nH = nH,
        .nFps = nFps,
        .nMHz = nMHz,
        .nMipiDev = 0
    };

    g_tUxeConfig = tUxeConfig;
    memset(&g_tMipiRxDev, 0, sizeof(AX_MIPI_RX_DEV_T));
    memset(&g_tSnsAttr, 0, sizeof(AX_SNS_ATTR_T));
    memset(&g_tSnsClkAttr, 0, sizeof(AX_SNS_CLK_ATTR_T));
    memset(&g_tDevAttr, 0, sizeof(AX_VIN_DEV_ATTR_T));
    memset(&g_tPipeAttr, 0, sizeof(AX_VIN_PIPE_ATTR_T));
    memset(&g_tChnAttr, 0, sizeof(AX_VIN_CHN_ATTR_T));

    InitMipiRxAttr(&g_tUxeConfig, &g_tMipiRxDev);
    InitSnsAttr(&g_tUxeConfig, &g_tSnsAttr);
    InitSnsClkAttr(g_tUxeConfig.enVinFmt, &g_tSnsClkAttr);
    InitDevAttr(&g_tUxeConfig, &g_tDevAttr);
    InitPipeAttr(&g_tUxeConfig, &g_tPipeAttr);
    InitChnAttr(&g_tUxeConfig, &g_tChnAttr);

    return AX_TRUE;
}

AX_BOOL SAMPLE_UXE_Start()
{
    KVM_LOG_I("SAMPLE_UXE_Start +++ enVinFmt = %d\n", g_tUxeConfig.enVinFmt);

    AX_S32 ret = 0;

    AX_LANE_COMBO_MODE_E emode = AX_LANE_COMBO_MODE_0;

    // depends on HW connections
    switch (g_tUxeConfig.enVinFmt)
    {
        case AX_FORMAT_BAYER_RAW_8BPP: /* mipi 8 lane */
            g_tUxeConfig.nDevId = 0;
            g_tUxeConfig.nMipiDev = 0;
            emode = AX_LANE_COMBO_MODE_0;
            break;
        case AX_FORMAT_YUV422_SEMIPLANAR_10BIT_P010: /* mipi 8 lane */
            g_tUxeConfig.nDevId = 4;
            g_tUxeConfig.nMipiDev = 0;
            emode = AX_LANE_COMBO_MODE_0;
            break;
        case AX_FORMAT_YUV422_INTERLEAVED_YUYV: /* mipi 4 lane */
            g_tUxeConfig.nDevId = 4;
            g_tUxeConfig.nMipiDev = 4;
            emode = AX_LANE_COMBO_MODE_4;
            break;
        default:
            KVM_LOG_E("unsupport pixel fmt %d\n", g_tUxeConfig.enVinFmt);
            return AX_FALSE;
    }

    KVM_LOG_I("UXE => [sif %d mipi %d] w: %d h: %d fps: %d nDataRate: %u snsMode: %d rawType: %d\n",
             g_tUxeConfig.nDevId,
             g_tUxeConfig.nMipiDev,
             g_tUxeConfig.nW,
             g_tUxeConfig.nH,
             g_tUxeConfig.nFps,
             g_tUxeConfig.nMHz,
             g_tSnsAttr.eSnsMode,
             g_tSnsAttr.eRawType);

    // MIPI RX
    ret = AX_MIPI_RX_SetLaneCombo(emode);
    if (0 != ret)
    {
        KVM_LOG_E("AX_MIPI_RX_SetLaneCombo fail, ret = 0x%x\n", ret);
        return AX_FALSE;
    }

    ret = AX_MIPI_RX_SetAttr(g_tUxeConfig.nMipiDev, &g_tMipiRxDev);
    if (0 != ret)
    {
        KVM_LOG_E("AX_MIPI_RX_SetAttr fail, ret = 0x%x\n", ret);
        return AX_FALSE;
    }

    ret = AX_MIPI_RX_Reset(g_tUxeConfig.nMipiDev);
    if (0 != ret)
    {
        KVM_LOG_E("AX_MIPI_RX_Reset fail, ret = 0x%x\n", ret);
        return AX_FALSE;
    }

    ret = AX_MIPI_RX_Start(g_tUxeConfig.nMipiDev);
    if (0 != ret)
    {
        KVM_LOG_E("AX_MIPI_RX_Start fail, ret = 0x%x\n", ret);
        return AX_FALSE;
    }

    // DEV
    ret = AX_VIN_CreateDev(g_tUxeConfig.nDevId, &g_tDevAttr);
    if (0 != ret)
    {
        KVM_LOG_E("AX_VIN_CreateDev fail, ret = 0x%x\n", ret);
        return AX_FALSE;
    }

    ret = AX_VIN_SetDevAttr(g_tUxeConfig.nDevId, &g_tDevAttr);
    if (0 != ret)
    {
        KVM_LOG_E("AX_VIN_SetDevAttr fail, ret = 0x%x\n", ret);
        return AX_FALSE;
    }

    AX_VIN_DEV_BIND_PIPE_T tDevBindPipe = {0};
    for (AX_U8 i = 0; i < 1; i++)
    {
        AX_U8 nPipeID = 0;

        tDevBindPipe.nPipeId[i] = nPipeID;
        tDevBindPipe.nNum += 1;

        switch (AX_SNS_LINEAR_MODE)
        {
            case AX_SNS_LINEAR_MODE:
                tDevBindPipe.nHDRSel[i] = 0x1;
                break;
            case AX_SNS_HDR_2X_MODE:
                tDevBindPipe.nHDRSel[i] = 0x1 | 0x2;
                break;
            case AX_SNS_HDR_3X_MODE:
                tDevBindPipe.nHDRSel[i] = 0x1 | 0x2 | 0x4;
                break;
            case AX_SNS_HDR_4X_MODE:
                tDevBindPipe.nHDRSel[i] = 0x1 | 0x2 | 0x4 | 0x8;
                break;
            default:
                tDevBindPipe.nHDRSel[i] = 0x1;
                break;
        }
    }

    ret = AX_VIN_SetDevBindPipe(g_tUxeConfig.nDevId, &tDevBindPipe);
    if (0 != ret)
    {
        KVM_LOG_E("AX_VIN_SetDevBindPipe fail, ret = 0x%x\n", ret);
        return AX_FALSE;
    }

    ret = AX_VIN_SetDevBindMipi(g_tUxeConfig.nDevId, g_tUxeConfig.nMipiDev);
    if (0 != ret)
    {
        KVM_LOG_E("AX_VIN_SetDevBindMipi fail, ret = 0x%x\n", ret);
        return AX_FALSE;
    }

    /* configure the attribute of early reporting of frame interrupts */
    if (AX_VIN_DEV_WORK_MODE_1MULTIPLEX < g_tDevAttr.eDevWorkMode)
    {
        ret = AX_VIN_SetDevFrameInterruptAttr(g_tUxeConfig.nDevId, &g_tUxeConfig.tDevFrmIntAttr);
        if (0 != ret)
        {
            KVM_LOG_E("AX_VIN_SetDevFrameInterruptAttr fail, ret = 0x%x\n", ret);
            return AX_FALSE;
        }
    }

    if (AX_VIN_DEV_OFFLINE == g_tDevAttr.eDevMode)
    {
        AX_VIN_DUMP_ATTR_T tDumpAttr;
        tDumpAttr.bEnable = AX_TRUE;
        tDumpAttr.nDepth = 3;
        ret = AX_VIN_SetDevDumpAttr(g_tUxeConfig.nDevId, AX_VIN_DUMP_QUEUE_TYPE_DEV, &tDumpAttr);
        if (0 != ret)
        {
            KVM_LOG_E("AX_VIN_SetDevDumpAttr fail, ret = 0x%x\n", ret);
            return AX_FALSE;
        }
    }

    ret = AX_VIN_EnableDev(g_tUxeConfig.nDevId);
    if (0 != ret)
    {
        KVM_LOG_E("AX_VIN_EnableDev fail, ret = 0x%x\n", ret);
        return AX_FALSE;
    }

    KVM_LOG_I("[%d] ---\n", g_tUxeConfig.nDevId);
    return AX_TRUE;
}

AX_BOOL SAMPLE_UXE_Stop()
{
    KVM_LOG_I("[Dev:%d] +++\n", g_tUxeConfig.nDevId);

    AX_S32 ret = AX_SUCCESS;

    AX_U8 nDevID = g_tUxeConfig.nDevId;

    ret = AX_VIN_DisableDev(nDevID);
    if (0 != ret)
    {
        KVM_LOG_E("AX_VIN_DisableDev fail, ret = 0x%x\n", ret);
        return AX_FALSE;
    }

    if (AX_VIN_DEV_OFFLINE == g_tDevAttr.eDevMode)
    {
        AX_VIN_DUMP_ATTR_T tDumpAttr;
        tDumpAttr.bEnable = AX_FALSE;
        ret = AX_VIN_SetDevDumpAttr(nDevID, AX_VIN_DUMP_QUEUE_TYPE_DEV, &tDumpAttr);
        if (0 != ret)
        {
            KVM_LOG_E("AX_VIN_SetDevDumpAttr fail, ret = 0x%x\n", ret);
            return AX_FALSE;
        }
    }

    ret = AX_MIPI_RX_Stop(nDevID);
    if (0 != ret)
    {
        KVM_LOG_E("AX_MIPI_RX_Stop fail, ret = 0x%x\n", ret);
        return AX_FALSE;
    }

    ret = AX_VIN_DestroyDev(nDevID);
    if (0 != ret)
    {
        KVM_LOG_E("AX_VIN_DestroyDev fail, ret = 0x%x\n", ret);
        return AX_FALSE;
    }

    ret = AX_MIPI_RX_Reset(g_tUxeConfig.nMipiDev);
    if (0 != ret)
    {
        KVM_LOG_E("AX_MIPI_RX_Reset fail, ret = 0x%x\n", ret);
        return AX_FALSE;
    }

    KVM_LOG_I("[Dev:%d] ---\n", nDevID);
    return AX_TRUE;
}

AX_BOOL SAMPLE_UXE_DeInit()
{
    AX_S32 ret = 0;

    KVM_LOG_I("+++\n");
    ret = AX_VIN_Deinit();
    if (0 != ret)
    {
        KVM_LOG_E("AX_VIN_Deinit failed, ret=0x%x\n", ret);
        return AX_FALSE;
    }
    ret = AX_MIPI_RX_DeInit();
    if (0 != ret)
    {
        KVM_LOG_E("AX_MIPI_RX_DeInit failed, ret=0x%x\n", ret);
        return AX_FALSE;
    }
    KVM_LOG_I("---\n");
    return AX_TRUE;
}

AX_U8 SAMPLE_UXE_GetDevID()
{
    return g_tUxeConfig.nDevId;
}
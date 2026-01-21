/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include <fcntl.h>
#include <linux/fb.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>
#include <pthread.h>

#include "ax_vo_api.h"
#include "sample_vo.h"
#include "sample_queue.h"
#include "sample_log.h"
#include "sample_utils.h"

extern AX_S32 AX_SYS_Usleep(unsigned long usc);
extern AX_S32 AX_SYS_Msleep(unsigned long msc);

#define MAX_DEVICE_COUNT (64)
#define QTGraphicLayer 0
#define MAX_FB_DEIVCE_NUM 7

typedef struct SAMPLE_PIX_ABGR8888_S
{
    AX_U8 u8A;
    AX_U8 u8B;
    AX_U8 u8G;
    AX_U8 u8R;
} SAMPLE_PIX_ABGR8888_T;

typedef struct THREAD_ATTR_S
{
     pthread_mutex_t mutex;
     pthread_cond_t cond;
     AX_BOOL bDataReady;
 }THREAD_ATTR_T;

static THREAD_ATTR_T g_stThreadAttr = {
                                        .mutex = PTHREAD_MUTEX_INITIALIZER,
                                        .cond = PTHREAD_COND_INITIALIZER,
                                        .bDataReady = AX_FALSE
                                      };


typedef struct VO_CALLBACK_STATUS_S
{
    AX_U64 u64SecOld;
    AX_U64 u64USecOld;
    AX_U64 nTime;
}VO_CALLBACK_STATUS_T;

static VO_CALLBACK_STATUS_T g_stVoCallBackStatus =
{
    .u64SecOld = 0,
    .u64USecOld = 0,
    .nTime = 0
};

static AX_VSYNC_CALLBACK_FUNC_T g_stVoCallbackFun;
static SAMPLE_VO_ATTR_T g_stVoAttr;
static AX_POOL g_nLayerPool = AX_INVALID_POOLID;
static AX_BOOL g_bChnEnable[AX_VO_CHN_MAX] = {[0 ... 71] = AX_FALSE};
static AX_BOOL g_bThreadStart = AX_FALSE;
static pthread_t g_tid;
static AX_U32 g_nVoDelayTime = 0;
static AX_U8 g_nFps = 0;

static AX_S32 CrtcOff()
{
    return AX_VO_CrtcOff(g_stVoAttr.dev);
}

static AX_S32 CrtcOn()
{
    return AX_VO_CrtcOn(g_stVoAttr.dev);
}

static AX_S32 InitVOFb(AX_U32 width, AX_U32 height, AX_U8 fb)
{
    AX_S32 s32Fd, s32Ret = -1;
    AX_CHAR cName[32] = {0};

    struct fb_var_screeninfo stScreenInfo = {0};
    struct fb_bitfield stR32 = {24, 8, 0};
    struct fb_bitfield stG32 = {16, 8, 0};
    struct fb_bitfield stB32 = {8, 8, 0};
    struct fb_bitfield stA32 = {0, 8, 0};

    if (fb > (MAX_FB_DEIVCE_NUM - 1))
    {
        KVM_LOG_E("invalid frame buffer id: %d\n", fb);
        return -1;
    }

    sprintf(cName, "/dev/fb%d", fb);

    do
    {
        s32Fd = open(cName, O_RDWR);
        if (s32Fd < 0)
        {
            KVM_LOG_E("open fb device %s failed!\n", cName);
            break;
        }

        s32Ret = ioctl(s32Fd, FBIOGET_VSCREENINFO, &stScreenInfo);
        if (s32Ret < 0)
        {
            KVM_LOG_E("get variable screen info failed, s32Ret = %d\n", s32Ret);
            close(s32Fd);
            s32Fd = -1;
            break;
        }

        stScreenInfo.xres = width;
        stScreenInfo.xres_virtual = width;
        stScreenInfo.yres = height;
        stScreenInfo.yres_virtual = height;
        stScreenInfo.transp = stA32;
        stScreenInfo.red = stR32;
        stScreenInfo.green = stG32;
        stScreenInfo.blue = stB32;
        stScreenInfo.bits_per_pixel = 32;

        s32Ret = ioctl(s32Fd, FBIOPUT_VSCREENINFO, &stScreenInfo);
        if (s32Ret < 0)
        {
            KVM_LOG_E("put variable screen info failed, s32Ret = %d\n", s32Ret);
            close(s32Fd);
            s32Fd = -1;
            break;
        }

        s32Ret = close(s32Fd);
        if (AX_SUCCESS != s32Ret)
        {
            KVM_LOG_E("failed to close fd ,s32Ret = %d\n", s32Ret);
            break;
        }
        s32Fd = -1;
    } while(0);

    return s32Ret;
}

static AX_S32 DrawFbRect(AX_U32 nW, AX_U32 nH, AX_U32 width, AX_U32 height, AX_S32 x, AX_S32 y, AX_U8 fb, SAMPLE_PIX_ABGR8888_T* pstColor)
{
    AX_S32 s32Fp = 0;
    AX_S32 s32Ret = -1;
    AX_LONG lScreensize = 0;
    AX_CHAR *cFbp = NULL;
    AX_LONG lLocation = 0;
    AX_CHAR cFbName[32] = {0};
    struct fb_var_screeninfo stVarInfo;
    struct fb_fix_screeninfo stFixInfo;

    if (fb > (MAX_FB_DEIVCE_NUM - 1))
    {
        KVM_LOG_E("invalid frame buffer id: %d\n", fb);
        return -1;
    }

    sprintf(cFbName, "/dev/fb%d", fb);

    s32Fp = open(cFbName, O_RDWR);
    if (s32Fp < 0)
    {
        KVM_LOG_E("open fb device %s failed!\n", cFbName);
        return 1;
    }

    s32Ret = ioctl(s32Fp, FBIOGET_FSCREENINFO, &stFixInfo);
    if (AX_SUCCESS != s32Ret)
    {
        KVM_LOG_E("failed to read fixed information, s32Ret = %d\n", s32Ret);
        return 2;
    }

    s32Ret = ioctl(s32Fp, FBIOGET_VSCREENINFO, &stVarInfo);
    if (AX_SUCCESS != s32Ret)
    {
        KVM_LOG_E("failed to read variable information, s32Ret = %d\n", s32Ret);
        return 3;
    }

    lScreensize = stVarInfo.xres * stVarInfo.yres * stVarInfo.bits_per_pixel / 8;

    cFbp = (AX_CHAR *)(mmap(0, lScreensize, PROT_READ | PROT_WRITE, MAP_SHARED, s32Fp, 0));
    if (cFbp == NULL)
    {
        KVM_LOG_E("failed to map framebuffer device to memory\n");
        return 4;
    }

    AX_S32 s32BytePerPix = stVarInfo.bits_per_pixel / 8;

    lLocation = x * s32BytePerPix + y * stFixInfo.line_length;
    KVM_LOG_I("pixel size : %d bytes, line_length: %d\n", s32BytePerPix, stFixInfo.line_length);

    memset(cFbp, 0, lScreensize);

    for (int i = 0; i < (AX_S32)width; i++)
    {
        for (int j = 0; j < (AX_S32)height; j++)
        {
            if ((lLocation + i * s32BytePerPix + j * stFixInfo.line_length + 3) < 0
                 || (lLocation + i * s32BytePerPix + j * stFixInfo.line_length) > ((nW * nH) * s32BytePerPix - 1))
            {
                continue;
            }
            cFbp[lLocation + i * s32BytePerPix + j * stFixInfo.line_length] = pstColor->u8A;
            cFbp[lLocation + i * s32BytePerPix + j * stFixInfo.line_length + 1] = pstColor->u8B;
            cFbp[lLocation + i * s32BytePerPix + j * stFixInfo.line_length + 2] = pstColor->u8G;
            cFbp[lLocation + i * s32BytePerPix + j * stFixInfo.line_length + 3] = pstColor->u8R;
        }
    }

    s32Ret = munmap(cFbp, lScreensize);
    if (AX_SUCCESS != s32Ret)
    {
        KVM_LOG_E("munmap failed, s32Ret = %d\n", s32Ret);
        return 5;
    }
    cFbp = NULL;

    s32Ret = close(s32Fp);
    if (AX_SUCCESS != s32Ret)
    {
        KVM_LOG_E("close failed, s32Ret = %d\n", s32Ret);
        return 6;
    }
    s32Fp = -1;

    return 0;
}

static AX_BOOL GetDispInfoFromIntfSync(AX_VO_INTF_SYNC_E eIntfSync, AX_VO_RECT_T* pstArea, AX_U32* pnHz)
{
    pstArea->u32X = 0;
    pstArea->u32Y = 0;
    switch (eIntfSync)
    {
    case AX_VO_OUTPUT_1080P25:
        pstArea->u32Width = 1920;
        pstArea->u32Height = 1080;
        *pnHz = 25;
        break;
    case AX_VO_OUTPUT_1080P30:
        pstArea->u32Width = 1920;
        pstArea->u32Height = 1080;
        *pnHz = 30;
        break;
    case AX_VO_OUTPUT_1080P50:
        pstArea->u32Width = 1920;
        pstArea->u32Height = 1080;
        *pnHz = 50;
        break;
    case AX_VO_OUTPUT_1080P60:
        pstArea->u32Width = 1920;
        pstArea->u32Height = 1080;
        *pnHz = 60;
        break;
    case AX_VO_OUTPUT_3840x2160_25:
        pstArea->u32Width = 3840;
        pstArea->u32Height = 2160;
        *pnHz = 25;
        break;
    case AX_VO_OUTPUT_3840x2160_30:
        pstArea->u32Width = 3840;
        pstArea->u32Height = 2160;
        *pnHz = 30;
        break;
    case AX_VO_OUTPUT_3840x2160_50:
        pstArea->u32Width = 3840;
        pstArea->u32Height = 2160;
        *pnHz = 50;
        break;
    case AX_VO_OUTPUT_3840x2160_60:
        pstArea->u32Width = 3840;
        pstArea->u32Height = 2160;
        *pnHz = 60;
        break;
    case AX_VO_OUTPUT_4096x2160_25:
        pstArea->u32Width = 4096;
        pstArea->u32Height = 2160;
        *pnHz = 25;
        break;
    case AX_VO_OUTPUT_4096x2160_30:
        pstArea->u32Width = 4096;
        pstArea->u32Height = 2160;
        *pnHz = 30;
        break;
    case AX_VO_OUTPUT_4096x2160_50:
        pstArea->u32Width = 4096;
        pstArea->u32Height = 2160;
        *pnHz = 50;
        break;
    case AX_VO_OUTPUT_4096x2160_60:
        pstArea->u32Width = 4096;
        pstArea->u32Height = 2160;
        *pnHz = 60;
        break;
    default:
        KVM_LOG_E("%s: UnSupport device %d\n", __func__, eIntfSync);
        return AX_FALSE;
    }

    return AX_TRUE;
}

static AX_POOL CreateLayerPool(VO_DEV dev, AX_U32 nBlkSize, AX_U32 nBlkCnt)
{
    AX_POOL_CONFIG_T stPoolCfg;
    memset(&stPoolCfg, 0, sizeof(stPoolCfg));
    stPoolCfg.MetaSize = 4096;
    stPoolCfg.CacheMode = POOL_CACHE_MODE_NONCACHE;
    stPoolCfg.BlkSize = nBlkSize;
    stPoolCfg.BlkCnt = nBlkCnt;
    sprintf((AX_CHAR*)stPoolCfg.PoolName, "vo_dev%d_layer_pool\n", dev);
    AX_POOL pool = AX_POOL_CreatePool(&stPoolCfg);
    if (AX_INVALID_POOLID == pool)
    {
        KVM_LOG_E("AX_POOL_CreatePool(blkSize %d, blkCnt %d) fail\n", nBlkSize, nBlkCnt);
    }

    return pool;
}

static AX_BOOL GetVideoLayerCSC(VO_LAYER voLayer, AX_VO_CSC_T* pstVideoCSC)
{
    AX_S32 ret = AX_VO_GetVideoLayerCSC(voLayer, pstVideoCSC);
    if (0 != ret)
    {
        KVM_LOG_E("AX_VO_GetVideoLayerCSC(layer %d) fail, ret = 0x%x\n", voLayer, ret);
        return AX_FALSE;
    }

    return AX_TRUE;
}

static AX_BOOL SetVideoLayerCSC(VO_LAYER voLayer, const AX_VO_CSC_T* pstVideoCSC)
{
    AX_S32 ret = AX_VO_SetVideoLayerCSC(voLayer, pstVideoCSC);
    if (0 != ret)
    {
        KVM_LOG_E("AX_VO_SetVideoLayerCSC(layer %d) fail, ret = 0x%x\n", voLayer, ret);
        return AX_FALSE;
    }
    else
    {
        KVM_LOG_I("set video layer %d csc: matrix %d, luma %d, contrast %d, hue %d, satuature %d\n", voLayer, pstVideoCSC->enCscMatrix,
                  pstVideoCSC->u32Luma, pstVideoCSC->u32Contrast, pstVideoCSC->u32Hue, pstVideoCSC->u32Satuature);
    }

    return AX_TRUE;
}

static AX_BOOL Init(AX_U32 nW, AX_U32 nH, AX_U8 nFps, AX_BOOL bShowFb, AX_BOOL bYuv422_10bit)
{
    KVM_LOG_I("+++\n");
    KVM_LOG_I("nW = %u, nH = %u, nFps = %d\n", nW, nH, nFps);
    static AX_BOOL bVoInit = AX_FALSE;
    if (!bVoInit)
    {
        AX_S32 nRet = AX_VO_Init();
        if (0 != nRet)
        {
            KVM_LOG_E("AX_VO_Init failed nRet = 0x%x\n", nRet);
            return AX_FALSE;
        }
        bVoInit = AX_TRUE;
    }

    g_stVoCallBackStatus.u64SecOld  = 0;
    g_stVoCallBackStatus.u64USecOld = 0;
    g_stVoCallBackStatus.nTime      = 0;

    SAMPLE_VO_ATTR_T stVoAttr =
    {
        .dev = 0,
        .voLayer = -1,
        .s32FBIndex = {-1, -1},
        .enIntfType = AX_VO_INTF_HDMI,
        .enIntfSync = AX_VO_OUTPUT_3840x2160_60,
        .enIntfFmt = AX_VO_OUT_FMT_RGB888,
        .bForceDisplay = AX_TRUE,
        .bLinkVo2Disp = AX_TRUE,
        .enEngineMode = AX_VO_ENGINE_MODE_AUTO,
        .nEngineId = 0,
        .nBgClr = 0x000000, /* black */
        .nLayerDepth = 3,
        .stChnInfo = {
                        .nCount = 1,
                        .arrFps = {[0 ... 71] = 0},
                        .arrChns = {
                                        [0] = {
                                                .bInUseFrOutput = AX_FALSE,
                                                .bKeepPrevFr = AX_FALSE,
                                                .stRect.u32Width = nW,
                                                .stRect.u32Height = nH,
                                                .stRect.u32X = 0,
                                                .stRect.u32Y = 0,
                                                .u32FifoDepth = 1,
                                                .u32Priority = 0
                                              }
                                   },
                    },
        .enDrawMode = AX_VO_PART_MODE_MULTI,
        .enVoMode = AX_VO_MODE_ONLINE,
        .enSyncMode = AX_VO_LAYER_SYNC_PRIMARY,
        .stHdmiAttr = {
                        .bEnableHdmi = AX_TRUE,
                        .enDefaultMode = AX_HDMI_FORCE_HDMI,
                        .eOutCscQuantization = AX_HDMI_QUANTIZATION_FULL_RANGE,
                        .enHdmiOutFmt = AX_HDMI_VIDEO_MODE_RGB444,
                      }
    };

    static AX_BOOL bVoFbInit = AX_FALSE;
    if (bShowFb && !bVoFbInit)
    {
        stVoAttr.s32FBIndex[0] = QTGraphicLayer;
        if (AX_SUCCESS != InitVOFb(nW, nH, QTGraphicLayer))
        {
            KVM_LOG_E("init vo fb failed\n");
            return AX_FALSE;
        }

        SAMPLE_PIX_ABGR8888_T stColor = {.u8A = 128, .u8B = 0, .u8G = 0, .u8R = 255};
        if (AX_SUCCESS != DrawFbRect(nW, nH, 1280, 720, 1280, 720, QTGraphicLayer, &stColor))
        {
            KVM_LOG_E("draw image on frame buffer failed\n");
            return AX_FALSE;
        }
        bVoFbInit = AX_TRUE;
    }

    if (nW == 3840 && nH == 2160 && nFps == 30)
    {
        stVoAttr.enIntfSync = AX_VO_OUTPUT_3840x2160_30;
    }
    else if (nW == 1920 && nH == 1080 && nFps == 60)
    {
        stVoAttr.enIntfSync = AX_VO_OUTPUT_1080P60;
    }


    if (bYuv422_10bit)
    {
        /* YUV422 10BIT */
        stVoAttr.enIntfFmt = AX_VO_OUT_FMT_YUV422_10;
        stVoAttr.stHdmiAttr.enHdmiOutFmt = AX_HDMI_VIDEO_MODE_YUV422;
    }

    AX_U32 nHz;
    AX_VO_RECT_T stArea;
    if (!GetDispInfoFromIntfSync(stVoAttr.enIntfSync, &stArea, &nHz))
    {
        KVM_LOG_E("GetDispInfoFromIntfSync failed\n");
        return AX_FALSE;
    }
    KVM_LOG_I("GetDispInfoFromIntfSync, stVoAttr.enIntfSync = %d, u32Width = %u, u32Height = %u, nHz = %u\n", stVoAttr.enIntfSync, stArea.u32Width, stArea.u32Height, nHz);

    if (AX_VO_PART_MODE_SINGLE == stVoAttr.enDrawMode || AX_VO_MODE_OFFLINE == stVoAttr.enVoMode)
    {
        g_nLayerPool = CreateLayerPool(stVoAttr.dev, stArea.u32Width * stArea.u32Width * 3 / 2, stVoAttr.nLayerDepth);
        if (AX_INVALID_POOLID == g_nLayerPool)
        {
            KVM_LOG_E("CreateLayerPool failed\n");
            return AX_FALSE;
        }
    }

    AX_VO_PUB_ATTR_T stPubAttr;
    memset(&stPubAttr, 0, sizeof(stPubAttr));
    stPubAttr.enIntfType = stVoAttr.enIntfType;
    stPubAttr.enIntfSync = stVoAttr.enIntfSync;
    stPubAttr.enIntfFmt = stVoAttr.enIntfFmt;
    stPubAttr.stHdmiAttr = stVoAttr.stHdmiAttr;
    //stPubAttr.stHdmiAttr.bEnableHdmi = stVoAttr.bForceDisplay;
    stPubAttr.enMode = stVoAttr.enVoMode;
    KVM_LOG_I("AX_VO_SetPubAttr, dev = %d, enIntfType = %d, enIntfSync = %d, enIntfFmt = %d, stHdmiAttr.bEnableHdmi = %d, enMode = %d\n",
               stVoAttr.dev,
               stPubAttr.enIntfType,
               stPubAttr.enIntfSync,
               stPubAttr.enIntfFmt,
               stPubAttr.stHdmiAttr.bEnableHdmi,
               stPubAttr.enMode);
    AX_S32 ret = AX_VO_SetPubAttr(stVoAttr.dev, &stPubAttr);
    if (0 != ret)
    {
        KVM_LOG_E("AX_VO_SetPubAttr(dev %d) fail, ret = 0x%x\n", stVoAttr.dev, ret);
        return AX_FALSE;
    }

    enum { DEV_ENABLED = 0x1, VOLAYER_CREATED = 0x2, VOLAYER_BINDED = 0x4, UILAYER_BINDED = 0x8 };
    AX_U32 nState = 0;

    do
    {
        ret = AX_VO_Enable(stVoAttr.dev);
        if (0 != ret)
        {
            KVM_LOG_E("AX_VO_Enable(dev %d) fail, ret = 0x%x\n", stVoAttr.dev, ret);
            break;
        }
        else
        {
            nState |= DEV_ENABLED;
        }

        if (stVoAttr.s32FBIndex[1] != -1)
        {
            ret = AX_VO_BindGraphicLayer(stVoAttr.s32FBIndex[1], stVoAttr.dev);
            if (0 != ret)
            {
                KVM_LOG_E("AX_VO_BindGraphicLayer(layer %d dev %d) fail, ret = 0x%x\n", stVoAttr.s32FBIndex[1], stVoAttr.dev, ret);
                break;
            }
            else
            {
                nState |= UILAYER_BINDED;
            }
        }

        if (stVoAttr.s32FBIndex[0] != -1)
        {
            ret = AX_VO_BindGraphicLayer(stVoAttr.s32FBIndex[0], stVoAttr.dev);
            if (0 != ret)
            {
                KVM_LOG_E("AX_VO_BindGraphicLayer(layer %d dev %d) fail, ret = 0x%x\n", stVoAttr.s32FBIndex[0], stVoAttr.dev, ret);
                break;
            }
            else
            {
                nState |= UILAYER_BINDED;
            }
        }

        ret = AX_VO_CreateVideoLayer(&stVoAttr.voLayer);
        if (0 != ret)
        {
            KVM_LOG_E("AX_VO_CreateVideoLayer(layer %d) fail, ret = 0x%x\n", stVoAttr.voLayer, ret);
            break;
        }
        else
        {
            nState |= VOLAYER_CREATED;
        }

        AX_VO_VIDEO_LAYER_ATTR_T stLayerAttr;
        memset(&stLayerAttr, 0, sizeof(stLayerAttr));
        stLayerAttr.stDispRect.u32Width = stArea.u32Width;
        stLayerAttr.stDispRect.u32Height = stArea.u32Height;
        stLayerAttr.stImageSize.u32Width = stArea.u32Width;
        stLayerAttr.stImageSize.u32Height = stArea.u32Height;
        stLayerAttr.enPixFmt = AX_FORMAT_YUV420_SEMIPLANAR;
        if (AX_VO_OUT_FMT_YUV422_10 == stVoAttr.enIntfFmt)
        {
            stLayerAttr.enPixFmt = AX_FORMAT_YUV420_SEMIPLANAR_10BIT_P010;
        }
        stLayerAttr.enSyncMode = stVoAttr.enSyncMode;
        stLayerAttr.f32FrmRate = (AX_F32)(nHz);
        stLayerAttr.u32FifoDepth = stVoAttr.nLayerDepth;
        stLayerAttr.u32BkClr = stVoAttr.nBgClr;
        stLayerAttr.enBgFillMode = AX_VO_BG_FILL_ONCE;
        stLayerAttr.enWBMode = AX_VO_LAYER_WB_POOL;
        stLayerAttr.u32PoolId = g_nLayerPool;
        stLayerAttr.u32DispatchMode = stVoAttr.bLinkVo2Disp ? AX_VO_LAYER_OUT_TO_LINK : AX_VO_LAYER_OUT_TO_FIFO;
        stLayerAttr.enPartMode = stVoAttr.enDrawMode;
        stLayerAttr.enBlendMode = AX_VO_BLEND_MODE_DEFAULT;
        stLayerAttr.enEngineMode = stVoAttr.enEngineMode;
        stLayerAttr.u32EngineId = stVoAttr.nEngineId;
        stLayerAttr.bDisplayPreProcess = stVoAttr.bLinkVo2Disp ? AX_FALSE : AX_TRUE;
        ret = AX_VO_SetVideoLayerAttr(stVoAttr.voLayer, &stLayerAttr);
        if (0 != ret)
        {
            KVM_LOG_E("AX_VO_SetVideoLayerAttr(layer %d) fail, ret = 0x%x\n", stVoAttr.voLayer, ret);
            break;
        }
        else
        {
            KVM_LOG_I("video layer %d: [(%d, %d) %dx%d], dispatch mode %d, layer depth %d, part mode %d, tolerance %d\n", stVoAttr.voLayer,
                    stLayerAttr.stDispRect.u32X, stLayerAttr.stDispRect.u32Y, stLayerAttr.stDispRect.u32Width,
                    stLayerAttr.stDispRect.u32Height, stLayerAttr.u32DispatchMode, stLayerAttr.u32FifoDepth, stLayerAttr.enPartMode,
                    stLayerAttr.u32Toleration);
        }

        ret = AX_VO_BindVideoLayer(stVoAttr.voLayer, stVoAttr.dev);
        if (0 != ret)
        {
            KVM_LOG_E("AX_VO_BindVideoLayer(layer %d dev %d) fail, ret = 0x%x\n", stVoAttr.voLayer, stVoAttr.dev, ret);
            break;
        }
        else
        {
            nState |= VOLAYER_BINDED;
        }
    } while (0);

    AX_VO_CSC_T stCsc;
    if (!GetVideoLayerCSC(stVoAttr.voLayer, &stCsc))
    {
        KVM_LOG_E("GetVideoLayerCSC fail\n");
        return AX_FALSE;
    }
    else
    {
        if (!bYuv422_10bit)
        {
            /* fixme: how to get color gamut BT601 or BT709 ? */
            stCsc.enCscMatrix = AX_VO_CSC_MATRIX_BT601_TO_RGB_PC;
        }
        else
        {
            /* fixme: how to configure 10bit csc */
            // stCsc.enCscMatrix = AX_VO_CSC_MATRIX_RGB_TO_BT2020_PC;
        }
        if (!SetVideoLayerCSC(stVoAttr.voLayer, &stCsc))
        {
            KVM_LOG_E("SetVideoLayerCSC fail\n");
            return AX_FALSE;
        }
    }
    g_stVoAttr = stVoAttr;
    KVM_LOG_I("---\n");
    return AX_TRUE;
}

static AX_BOOL EnableVideoLayer(AX_VOID)
{
    AX_S32 ret = AX_VO_EnableVideoLayer(g_stVoAttr.voLayer);
    if (0 != ret)
    {
        KVM_LOG_E("AX_VO_EnableVideoLayer(layer %d) fail, ret = 0x%x\n", g_stVoAttr.voLayer, ret);
        return AX_FALSE;
    }

    KVM_LOG_I("enable layer %d\n", g_stVoAttr.voLayer);
    return AX_TRUE;
}

static AX_BOOL DisableChn(VO_CHN voChn)
{
    if (!g_bChnEnable[voChn])
    {
        return AX_TRUE;
    }

    AX_S32 ret = AX_VO_DisableChn(g_stVoAttr.voLayer, voChn);
    if (0 != ret)
    {
        KVM_LOG_E("AX_VO_DisableChn(layer %d chn %d) fail, ret = 0x%x\n", g_stVoAttr.voLayer, voChn, ret);
        return AX_FALSE;
    }

    g_bChnEnable[voChn] = AX_FALSE;

    KVM_LOG_I("disable chn %d of layer %d\n", voChn, g_stVoAttr.voLayer);
    return AX_TRUE;
}

static AX_VOID DisableChns(AX_U32 nNumChns)
{
    for (VO_CHN j = 0; j < nNumChns; ++j)
    {
        DisableChn(j);
    }

    AX_VO_BatchEnd(g_stVoAttr.voLayer);
}

static AX_BOOL EnableChn(VO_CHN voChn, const AX_VO_CHN_ATTR_T* pstChnAttr)
{
    if (g_bChnEnable[voChn])
    {
        KVM_LOG_E("chn %d of layer %d is enabled, please disable first\n", voChn, g_stVoAttr.voLayer);
        return AX_FALSE;
    }

    AX_S32 ret = AX_VO_SetChnAttr(g_stVoAttr.voLayer, voChn, pstChnAttr);
    if (0 != ret)
    {
        KVM_LOG_E("AX_VO_SetChnAttr(layer %d chn %d) fail, ret = 0x%x\n", g_stVoAttr.voLayer, voChn, ret);
        return AX_FALSE;
    }

    ret = AX_VO_EnableChn(g_stVoAttr.voLayer, voChn);
    if (0 != ret)
    {
        KVM_LOG_E("AX_VO_EnableChn(layer %d chn %d) fail, ret = 0x%x\n", g_stVoAttr.voLayer, voChn, ret);
        return AX_FALSE;
    }

    g_bChnEnable[voChn] = AX_TRUE;

    g_stVoAttr.stChnInfo.arrChns[voChn] = *pstChnAttr;

    KVM_LOG_I("enable chn %d of layer %d [(%d, %d) %dx%d], depth %d prior %d\n", voChn, g_stVoAttr.voLayer, pstChnAttr->stRect.u32X,
              pstChnAttr->stRect.u32Y, pstChnAttr->stRect.u32Width, pstChnAttr->stRect.u32Height, pstChnAttr->u32FifoDepth, pstChnAttr->u32Priority);
    return AX_TRUE;
}

static AX_BOOL SetChnFps(VO_CHN voChn, AX_F32 fps)
{
    AX_S32 ret = AX_VO_SetChnFrameRate(g_stVoAttr.voLayer, voChn, fps);
    if (0 != ret)
    {
        KVM_LOG_E("AX_VO_SetChnFrameRate(layer %d, chn %d, fps %f) fail, ret = 0x%x\n", g_stVoAttr.voLayer, voChn, fps, ret);
        return AX_FALSE;
    }

    KVM_LOG_I("set fps of layer %d chn %d to %f\n", g_stVoAttr.voLayer, voChn, fps);
    return AX_TRUE;
}

static AX_BOOL BatchUpdate(AX_BOOL bEnable, const VO_CHN_INFO_T* pstChnInfo)
{
    if (0 == pstChnInfo->nCount)
    {
        KVM_LOG_E("BatchUpdate fail\n");
        return AX_TRUE;
    }

    AX_S32 ret = AX_VO_BatchBegin(g_stVoAttr.voLayer);
    if (0 != ret)
    {
        KVM_LOG_E("AX_VO_BatchBegin(layer %d) fail, ret = 0x%x\n", g_stVoAttr.voLayer, ret);
        return AX_FALSE;
    }

    for (VO_CHN i = 0; i < pstChnInfo->nCount; ++i)
    {
        if (bEnable)
        {
            if (!SetChnFps(i, pstChnInfo->arrFps[i]))
            {
                DisableChns(i);
                KVM_LOG_E("SetChnFps fail\n");
                return AX_FALSE;
            }

            if (!EnableChn(i, &pstChnInfo->arrChns[i]))
            {
                DisableChns(i);
                KVM_LOG_E("EnableChn fail\n");
                return AX_FALSE;
            }
        }
        else
        {
            if (!DisableChn(i))
            {
                KVM_LOG_E("DisableChn fail\n");
                return AX_FALSE;
            }
        }
    }

    ret = AX_VO_BatchEnd(g_stVoAttr.voLayer);
    if (0 != ret)
    {
        KVM_LOG_E("AX_VO_BatchEnd(layer %d) fail, ret = 0x%x\n", g_stVoAttr.voLayer, ret);
        //return AX_FALSE;
    }

    return AX_TRUE;
}

static AX_BOOL DisableVideoLayer()
{
    AX_S32 ret = AX_VO_DisableVideoLayer(g_stVoAttr.voLayer);
    if (0 != ret)
    {
        KVM_LOG_E("AX_VO_DisableVideoLayer(layer %d) fail, ret = 0x%x\n", g_stVoAttr.voLayer, ret);
        return AX_FALSE;
    }

    KVM_LOG_I("AX_VO_DisableVideoLayer: disable layer %d\n", g_stVoAttr.voLayer);
    return AX_TRUE;
}

static AX_VOID WaitForVoInterrupt()
{
    pthread_mutex_lock(&g_stThreadAttr.mutex);
    while (!g_stThreadAttr.bDataReady)
    {
        pthread_cond_wait(&g_stThreadAttr.cond, &g_stThreadAttr.mutex);
    }
    g_stThreadAttr.bDataReady = AX_FALSE;
    pthread_mutex_unlock(&g_stThreadAttr.mutex);
}

static AX_VOID* CrtcOnProcessThread(AX_VOID* arg)
{
    AX_S32 nRet = pthread_detach(pthread_self());
    if (nRet)
    {
        KVM_LOG_I("pthread_detach fail: %s\n", strerror(nRet));
        exit(-1);
    }
    AX_U64 nTime = SAMPLE_UTILS_GetTickUs();
    KVM_LOG_I("CrtcOn, time = %llu\n", nTime);
    CrtcOn();
    return NULL;
}

static AX_VOID Wakeup()
{
    pthread_mutex_lock(&g_stThreadAttr.mutex);
    g_stThreadAttr.bDataReady = AX_TRUE;
    pthread_mutex_unlock(&g_stThreadAttr.mutex);

    pthread_cond_signal(&g_stThreadAttr.cond);
}

static AX_VOID VoCallBack(AX_U32 u32Seq, AX_U32 u32Sec, AX_U32 u32USec, AX_VOID *pPrivateData)
{
    AX_U64 u64Sec  = u32Sec;
    AX_U64 u64USec = u32USec;

    AX_U16 nMaxTime = 1000000/g_nFps;

    g_nVoDelayTime = 0;
    if (g_stVoCallBackStatus.nTime == 0)
    {
        g_stVoCallBackStatus.u64SecOld = u64Sec;
        g_stVoCallBackStatus.u64USecOld = u64USec;
        g_stVoCallBackStatus.nTime = u64Sec*1000000+u64USec;
    }
    else
    {
        AX_U64 nCurrTime = u64Sec*1000000+u64USec;
        if (nCurrTime < g_stVoCallBackStatus.nTime)
        {
            KVM_LOG_E("VoCallBack u32Seq = %u, vo time(<) = %llu %llu (old: %llu %llu, new: %llu %llu)", u32Seq, nCurrTime, g_stVoCallBackStatus.nTime, g_stVoCallBackStatus.u64SecOld, g_stVoCallBackStatus.u64USecOld, u64Sec, u64USec);
        }
        else
        {
            AX_U64 nDiffTime = nCurrTime - g_stVoCallBackStatus.nTime;
            if (nDiffTime > nMaxTime)
            {
                g_nVoDelayTime = nDiffTime - nMaxTime;
            }
            if (g_nVoDelayTime > 500)
            {
                KVM_LOG_E("VoCallBack u32Seq = %u, vo delay time = %u, (old: %llu %llu, new: %llu %llu)", u32Seq, g_nVoDelayTime, g_stVoCallBackStatus.u64SecOld, g_stVoCallBackStatus.u64USecOld, u64Sec, u64USec);
            }
            g_stVoCallBackStatus.u64SecOld  = u64Sec;
            g_stVoCallBackStatus.u64USecOld = u64USec;
            g_stVoCallBackStatus.nTime = nCurrTime;
        }
    }
    Wakeup();
}

static AX_S32 RegCallbackFunc()
{
    g_stVoCallbackFun.pfnVsyncEventCallback = VoCallBack;
    return AX_VO_VSYNC_RegCallbackFunc(g_stVoAttr.dev, &g_stVoCallbackFun);
}

static AX_S32 UnRegCallbackFunc()
{
    return AX_VO_VSYNC_UnRegCallbackFunc(g_stVoAttr.dev, &g_stVoCallbackFun);
}

static AX_VOID* VoProcessThread(AX_VOID* arg)
{
    KVM_LOG_I("+++ threadId = %lu\n", pthread_self());
    SAMPLE_UTILS_SetSchedPolicy(SCHED_FIFO, 99);
    SAMPLE_UTILS_SetPthreadName("VoThread");
    AX_U64 nSeqNum = 0;
    AX_BOOL bFirstFrame = AX_TRUE;
    AX_U16 nMaxTime = (g_nFps == 60 ? 14500 : 15000*2);
    /*
    The first frame is sent directly to vo for display, and vo interrupt is enabled immediately.
    The following frames are sent to vo for display after waiting for 15ms after receiving the vo interrupt signal.
    */
    while (g_bThreadStart)
    {
        if (bFirstFrame)
        {
            pthread_mutex_lock(&g_stThreadAttr.mutex);
            g_stThreadAttr.bDataReady = AX_FALSE;  // Do not respond to the interrupt callback function，before sending the first frame.
            pthread_mutex_unlock(&g_stThreadAttr.mutex);
        }
        else
        {
            AX_U32 nTimeUs = nMaxTime;
            nTimeUs = nTimeUs - g_nVoDelayTime;
            WaitForVoInterrupt();
            AX_SYS_Usleep(nTimeUs);

        }

        APP_VIDEO_FRAME_T stAppVideoFrame;
        AX_BOOL bExistFrame = AX_FALSE;
        while (AX_TRUE)
        {
            /* abandon old frames */
            APP_VIDEO_FRAME_T stAppVideoFrameTmp;
            AX_BOOL bRet = SAMPLE_QUEUE_Pop(&stAppVideoFrameTmp);
            if (!bRet)
            {
                break;
            }

            if (bExistFrame)
            {
                KVM_LOG_E("drop old frame nSeqNum = %llu\n", stAppVideoFrame.stVideoFrame.u64SeqNum);
                SAMPLE_UTILS_DecRef(&stAppVideoFrame.stVideoFrame);
            }
            stAppVideoFrame = stAppVideoFrameTmp;
            bExistFrame = AX_TRUE;
        }

        if (!bExistFrame)
        {
            if (!bFirstFrame)
            {
                KVM_LOG_E("--->SAMPLE_QUEUE_Pop fail, time = %llu\n", SAMPLE_UTILS_GetTickUs());
            }
            continue;
        }

        if (nSeqNum == 0)
        {
            nSeqNum = stAppVideoFrame.stVideoFrame.u64SeqNum + 1;
        }
        else
        {
            if (nSeqNum != stAppVideoFrame.stVideoFrame.u64SeqNum)
            {
                KVM_LOG_I("to vo frame reverse order appears lastSeqNum = %llu, currentSeqNum = %llu\n", nSeqNum - 1, stAppVideoFrame.stVideoFrame.u64SeqNum);
            }
            nSeqNum = stAppVideoFrame.stVideoFrame.u64SeqNum + 1;
        }

        AX_S32 ret = AX_VO_SendFrame(g_stVoAttr.voLayer, 0, &stAppVideoFrame.stVideoFrame, -1);
        if (0 != ret)
        {
            KVM_LOG_E("--->AX_VO_SendFrame(layer %d, chn 0, timeout -1) fail, ret = 0x%x\n", g_stVoAttr.voLayer, ret);
        }

        SAMPLE_UTILS_DecRef(&stAppVideoFrame.stVideoFrame);

        if (bFirstFrame)
        {
            pthread_t tid;
            AX_S32 nRet = pthread_create(&tid, NULL, CrtcOnProcessThread, NULL);
            if (nRet < 0)
            {
                KVM_LOG_E("pthread_create fail\n");
                return AX_FALSE;
            }
            bFirstFrame = AX_FALSE;
        }
    }
    KVM_LOG_I("---\n");
    return NULL;
}

static AX_BOOL DestoryLayerPool(AX_POOL* pool)
{
    if (AX_INVALID_POOLID == *pool)
    {
        KVM_LOG_E("Invalid pool id");
        return AX_FALSE;
    }

    AX_S32 ret = AX_POOL_DestroyPool(*pool);
    if (0 != ret)
    {
        KVM_LOG_E("AX_POOL_DestroyPool(pool %d) fail, ret = 0x%x", *pool, ret);
        return AX_FALSE;
    }

    *pool = AX_INVALID_POOLID;
    return AX_TRUE;
}

AX_BOOL SAMPLE_VO_Init(AX_U32 nW, AX_U32 nH, AX_U8 nFps, AX_BOOL bShowFb, AX_BOOL bYuv422_10bit)
{
    g_nFps = nFps;
    if (!Init(nW, nH, nFps, bShowFb, bYuv422_10bit))
    {
        KVM_LOG_E("vo Init failed\n");
        return AX_FALSE;
    }
    if (CrtcOff() != 0)
    {
        KVM_LOG_E("vo CrtcOff failed\n");
        return AX_FALSE;
    }

    sleep(1);

    AX_S32 ret = RegCallbackFunc();
    if (ret != 0) {
        KVM_LOG_E("vo RegCallbackFunc failed ret = %d\n", ret);
    }

    return AX_TRUE;
}

AX_BOOL SAMPLE_VO_Start()
{
    KVM_LOG_I("%s: +++\n", __func__);

    if (!EnableVideoLayer())
    {
        KVM_LOG_E("EnableVideoLayer fail\n");
        return AX_FALSE;
    }

    if (!BatchUpdate(AX_TRUE, &g_stVoAttr.stChnInfo))
    {
        KVM_LOG_E("BatchUpdate fail\n");
        DisableVideoLayer();
        return AX_FALSE;
    }

    g_bThreadStart = AX_TRUE;
    AX_S32 nRet = pthread_create(&g_tid, NULL, VoProcessThread, NULL);
    if (nRet < 0)
    {
        KVM_LOG_E("pthread_create fail\n");
        return AX_FALSE;
    }
    KVM_LOG_I("%s: ---\n", __func__);
    return AX_TRUE;
}

AX_BOOL SAMPLE_VO_Stop()
{
    KVM_LOG_I("+++\n");

    g_bThreadStart = AX_FALSE;

    CrtcOn();

    if (pthread_join(g_tid, NULL) != 0)
    {
        KVM_LOG_E("pthread_join fail\n");
        return AX_FALSE;
    }
    if (!BatchUpdate(AX_FALSE, &g_stVoAttr.stChnInfo))
    {
        return AX_FALSE;
    }

    if (!DisableVideoLayer())
    {
        return AX_FALSE;
    }

    if (UnRegCallbackFunc() != 0)
    {
        KVM_LOG_E("vo UnRegCallbackFunc failed\n");
        return AX_FALSE;
    }

    AX_S32 ret;
    ret = AX_VO_UnBindVideoLayer(g_stVoAttr.voLayer, g_stVoAttr.dev);
    if (0 != ret)
    {
        KVM_LOG_E("AX_VO_UnBindVideoLayer(layer %d dev %d) fail, ret = 0x%x\n", g_stVoAttr.voLayer, g_stVoAttr.dev, ret);
        return AX_FALSE;
    }

    if (g_stVoAttr.s32FBIndex[0] != -1)
    {
        ret = AX_VO_UnBindGraphicLayer(g_stVoAttr.s32FBIndex[0], g_stVoAttr.dev);
        if (0 != ret)
        {
            KVM_LOG_E("AX_VO_UnBindGraphicLayer(layer %d dev %d) fail, ret = 0x%x\n", g_stVoAttr.s32FBIndex[0], g_stVoAttr.dev, ret);
        }
    }

    if (g_stVoAttr.s32FBIndex[1] != -1)
    {
        ret = AX_VO_UnBindGraphicLayer(g_stVoAttr.s32FBIndex[1], g_stVoAttr.dev);
        if (0 != ret)
        {
            KVM_LOG_E("AX_VO_UnBindGraphicLayer(layer %d dev %d) fail, ret = 0x%x\n", g_stVoAttr.s32FBIndex[1], g_stVoAttr.dev, ret);
        }
    }

    ret = AX_VO_DestroyVideoLayer(g_stVoAttr.voLayer);
    if (0 != ret)
    {
        KVM_LOG_E("AX_VO_DestroyVideoLayer(layer %d) fail, ret = 0x%x\n", g_stVoAttr.voLayer, ret);
        return AX_FALSE;
    }
    else
    {
        g_stVoAttr.voLayer = (VO_LAYER)-1;
    }

    ret = AX_VO_Disable(g_stVoAttr.dev);
    if (0 != ret)
    {
        KVM_LOG_E("AX_VO_Disable(dev %d) fail, ret = 0x%x\n", g_stVoAttr.dev, ret);
        return AX_FALSE;
    }

    if (AX_INVALID_POOLID != g_nLayerPool)
    {
        if (!DestoryLayerPool(&g_nLayerPool))
        {
            return AX_FALSE;
        }
    }
    KVM_LOG_I("---\n");
    return AX_TRUE;
}

AX_BOOL SAMPLE_VO_DeInit()
{
    KVM_LOG_I("+++\n");

    if (AX_VO_Deinit() != 0)
    {
        KVM_LOG_E("AX_VO_Deinit failed\n");
        return AX_FALSE;
    }
    KVM_LOG_I("---\n");
    return AX_TRUE;
}

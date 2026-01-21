/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>
#include "ax_sys_api.h"
#include "util.h"
#include <ax_dsp_api.h>
#include "xvf_imgproc_structs.h"

#define USE_SAMPLE_FILE

typedef struct {
    unsigned int u32Width;
    unsigned int u32Height;
} SAMPLE_DSP_RESOLUTION_RATION_S;

static SAMPLE_DSP_RESOLUTION_RATION_S resolution_ratio[] = {
    {
        .u32Width = 4096,
        .u32Height = 2160,
    },
    {
        .u32Width = 3840,
        .u32Height = 2160,
    },
    {
        .u32Width = 2560,
        .u32Height = 1600,
    },
    {
        .u32Width = 2560,
        .u32Height = 1440,
    },
    {
        .u32Width = 1920,
        .u32Height = 1200,
    },
    {
        .u32Width = 1920,
        .u32Height = 1080,
    },
    {
        .u32Width = 1600,
        .u32Height = 1200,
    },
    {
        .u32Width = 1600,
        .u32Height = 900,
    },
    {
        .u32Width = 1280,
        .u32Height = 1024,
    },
    {
        .u32Width = 1280,
        .u32Height = 720,
    },
    {
        .u32Width = 1024,
        .u32Height = 768,
    },
    {
        .u32Width = 1024,
        .u32Height = 576,
    },
    {
        .u32Width = 960,
        .u32Height = 540,
    },
    {
        .u32Width = 800,
        .u32Height = 600,
    },
    {
        .u32Width = 800,
        .u32Height = 480,
    },
    {
        .u32Width = 640,
        .u32Height = 480,
    },
    {
        .u32Width = 640,
        .u32Height = 320,
    },
    {
        .u32Width = 540,
        .u32Height = 960,
    },
    {
        .u32Width = 480,
        .u32Height = 640,
    },
    {
        .u32Width = 320,
        .u32Height = 640,
    },
    {
        .u32Width = 320,
        .u32Height = 240,
    },
};

extern AX_S32 DSP_LoadPic(const AX_CHAR *pszBinFileName, AX_VOID *vaddr);
extern AX_S32 DSP_SavePic(const char *pszBinFileName, AX_VOID *vaddr, AX_U32 size);

#define DSP_RESIZE_SRC_PATH       "/opt/data/dsp/resize"
#define DSP_RESIZE_DST_PATH       DSP_RESIZE_SRC_PATH
int SAMPLE_DSP_TestFIKScale_U8(AX_DSP_ID_E dspid)
{
    int ret = -1;
    int i, j, ration_num;
    unsigned int nSrcImgSize;
    char pSrcImgFile[128] = {0};
    char pDstImgFile[128] = {0};
    char pDstPath[64] = {0};
    AX_DSP_IMAGE_T tSrcFrame = {0};
    AX_DSP_IMAGE_T tDstFrame = {0};
    struct stat statbuf;
    struct timeval tv_start, tv_end;
    xvfScale_U8_API_T Scale_U8_Params;

    if (stat(DSP_RESIZE_DST_PATH, &statbuf) != 0) {
        if (mkdir(DSP_RESIZE_DST_PATH, 0755) != 0 && errno != EEXIST) {
            vdsp_err("cannot create folder[%s]\n", DSP_RESIZE_DST_PATH);
            return -1;
        }
    } else {
        vdsp_debug("folder[%s] is exist\n", DSP_RESIZE_DST_PATH);
    }

    ration_num = sizeof(resolution_ratio) / sizeof(SAMPLE_DSP_RESOLUTION_RATION_S);
    vdsp_debug("%s: ration_num %d\n", __func__, ration_num);

    ret = AX_DSP_CV_Init(AX_DSP_ID_0);
    if (0 != ret) {
        vdsp_err("setup_dsp fail, ret=0x%x\n", ret);
        return -1;
    }

    for (j = 0; j < ration_num; j++) {
        sprintf(pSrcImgFile, "%s/%dx%d.nv12.yuv", DSP_RESIZE_SRC_PATH, resolution_ratio[j].u32Width,
                resolution_ratio[j].u32Height);
        tSrcFrame.width = resolution_ratio[j].u32Width;
        tSrcFrame.height = resolution_ratio[j].u32Height;
        tSrcFrame.stride = tSrcFrame.width;
        nSrcImgSize = tSrcFrame.stride * tSrcFrame.height;
        tSrcFrame.u32Size = nSrcImgSize;

#ifdef USE_SAMPLE_FILE
        nSrcImgSize *= 3 / 2;
        if (!LoadImage(pSrcImgFile, (AX_U64 *)&tSrcFrame.BufAddr[0].u64PhyAddr, (AX_VOID **)&tSrcFrame.BufAddr[0].u64VirAddr,
                       &nSrcImgSize)) {
            return -1;
        }
        printf("%d: %dx%d file %s load succ.\n", j, resolution_ratio[j].u32Width, resolution_ratio[j].u32Height,
               pSrcImgFile);
#else
        ret = AX_SYS_MemAlloc((AX_U64 *)&tSrcFrame.BufAddr[0].u64PhyAddr, (AX_VOID **)&tSrcFrame.BufAddr[0].u64VirAddr,
                              tSrcFrame.u32Size, SAMPLE_PHY_MEM_ALIGN_SIZE, NULL);
        if (0 != ret) {
            vdsp_err("%s AX_SYS_MemAlloc(0x%X) fail, ret=0x%x\n", __func__, tSrcFrame.u32Size, ret);
            return -1;
        }
        memset((AX_VOID *)tSrcFrame.BufAddr[0].u64VirAddr, 'Y', tSrcFrame.u32Size);
#endif

        tSrcFrame.BufAddr[0].u32Size = tSrcFrame.stride * tSrcFrame.height;

        memset((AX_VOID *)pDstPath, 0, sizeof(pDstPath));
        sprintf(pDstPath, "%s/src_%dx%d", DSP_RESIZE_DST_PATH, tSrcFrame.width, tSrcFrame.height);
        if (stat(pDstPath, &statbuf) != 0) {
            if (mkdir(pDstPath, 0755) != 0 && errno != EEXIST) {
                vdsp_err("cannot create folder[%s]\n", pDstPath);
                AX_SYS_MemFree(tSrcFrame.BufAddr[0].u64PhyAddr, (AX_VOID *)tSrcFrame.BufAddr[0].u64VirAddr);
                return -1;
            }
        } else {
            vdsp_debug("folder[%s] is exist\n", pDstPath);
        }

        for (i = 0; i < ration_num; i ++) {
            memset(&tDstFrame, 0, sizeof(tDstFrame));
            tDstFrame.width = resolution_ratio[i].u32Width;
            tDstFrame.height = resolution_ratio[i].u32Height;
            tDstFrame.stride  = tDstFrame.width;
            tDstFrame.u32Size = tDstFrame.stride * tDstFrame.height;
            ret = AX_SYS_MemAlloc((AX_U64 *)&tDstFrame.BufAddr[0].u64PhyAddr, (AX_VOID **)&tDstFrame.BufAddr[0].u64VirAddr,
                                  tDstFrame.u32Size, SAMPLE_PHY_MEM_ALIGN_SIZE, NULL);
            if (0 != ret) {
                vdsp_err("%s AX_SYS_MemAlloc(0x%X) fail, ret=0x%x\n", __func__, tDstFrame.u32Size, ret);
                AX_SYS_MemFree(tSrcFrame.BufAddr[0].u64PhyAddr, (AX_VOID *)tSrcFrame.BufAddr[0].u64VirAddr);
                return -1;
            } else {
                tDstFrame.BufAddr[0].u32Size = tDstFrame.stride * tDstFrame.height;
                memset((AX_VOID *)tDstFrame.BufAddr[0].u64VirAddr, 0, tDstFrame.u32Size);
                vdsp_debug("dst %dx%d, stride: %d, phy: 0x%llX\n", tDstFrame.width, tDstFrame.height, tDstFrame.stride,
                           tDstFrame.BufAddr[0].u64PhyAddr);
            }

            Scale_U8_Params.src           = AX_DSP_PhyAddr_To_DSPAddr(dspid, tSrcFrame.BufAddr[0].u64PhyAddr);
            Scale_U8_Params.srcWidth      = tSrcFrame.width;
            Scale_U8_Params.srcHeight     = tSrcFrame.height;
            Scale_U8_Params.srcStride     = tSrcFrame.stride;
            Scale_U8_Params.dst           = AX_DSP_PhyAddr_To_DSPAddr(dspid, tDstFrame.BufAddr[0].u64PhyAddr);
            Scale_U8_Params.dstWidth      = tDstFrame.width;
            Scale_U8_Params.dstHeight     = tDstFrame.height;
            Scale_U8_Params.dstStride     = tDstFrame.stride;
            Scale_U8_Params.tileWidth     = 64;//128;//try to fit your resolution
            Scale_U8_Params.tileHeight    = 8;//64;
            Scale_U8_Params.interpolation = XCV_INTERPOLATION_TYPE_BILINEAR;

            gettimeofday(&tv_start, NULL);
            ret = AX_DSP_CV_FIK(dspid, E_Scale_U8, &Scale_U8_Params, sizeof(Scale_U8_Params));
            gettimeofday(&tv_end, NULL);
            if (0 != ret) {
                vdsp_err("AX_DSP_FIK_Resize fail, ret=0x%x\n", ret);
                goto __END_OF_RESIZE;
            }
            printf("FIK U8 Resize %dx%d -> %dx%d, DSP cost time: tv_usec: %ld\n",
                   tSrcFrame.width, tSrcFrame.height, tDstFrame.width, tDstFrame.height,
                   (tv_end.tv_sec - tv_start.tv_sec) * 1000000 + tv_end.tv_usec - tv_start.tv_usec);

            //save pic
            memset((AX_VOID *)pDstImgFile, 0, sizeof(pDstImgFile));
            sprintf(pDstImgFile, "%s/dst_%dx%d.yuv", pDstPath, tDstFrame.stride, tDstFrame.height);
            DSP_SavePic(pDstImgFile, (AX_VOID *)tDstFrame.BufAddr[0].u64VirAddr, tDstFrame.u32Size);
            printf("%s saved\n", pDstImgFile);
            AX_SYS_MemFree(tDstFrame.BufAddr[0].u64PhyAddr, (AX_VOID *)tDstFrame.BufAddr[0].u64VirAddr);
        }
        AX_SYS_MemFree(tSrcFrame.BufAddr[0].u64PhyAddr, (AX_VOID *)tSrcFrame.BufAddr[0].u64VirAddr);
    }

    if (0 == ret) {
        printf("SAMPLE_DSP%d_TestFIKResize PASS\n", dspid);
        AX_DSP_CV_Exit(dspid);
        return 0;
    }

__END_OF_RESIZE:
    AX_SYS_MemFree(tSrcFrame.BufAddr[0].u64PhyAddr, (AX_VOID *)tSrcFrame.BufAddr[0].u64VirAddr);
    AX_SYS_MemFree(tDstFrame.BufAddr[0].u64PhyAddr, (AX_VOID *)tDstFrame.BufAddr[0].u64VirAddr);

    AX_DSP_CV_Exit(dspid);

    return ret;
}




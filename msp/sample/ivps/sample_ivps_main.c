/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include "sample_ivps_main.h"
static SAMPLE_IVPS_MAIN_T gSampleIvpsMain;

static AX_S32 dsp_init(void)
{
    AX_S32 ret;
    int dspid;
    for (dspid = 0; dspid < 2; dspid++ )
    {
        ret = AX_DSP_PowerOn(dspid);
        if (ret) {
            ALOGE("AX DSP[%d] Poweron error %x\n", dspid, ret);
            return -1;
        }
        ret = AX_DSP_LoadBin(dspid, "/opt/data/dsp/sram.bin", AX_DSP_MEM_TYPE_SRAM);
        if (ret) {
            ALOGE("AX DSP[%d] LoadBin error %x\n", dspid, ret);
            return -1;
        }
        ret = AX_DSP_LoadBin(dspid, "/opt/data/dsp/itcm.bin", AX_DSP_MEM_TYPE_ITCM);
        if (ret) {
            ALOGE("AX DSP[%d] LoadBin error %x\n", dspid, ret);
            return -1;
        }
        ret = AX_DSP_EnableCore(dspid);
        if (ret) {
            ALOGE("AX DSP[%d] Enable Core error %x\n", dspid, ret);
            return -1;
        }
    }
    usleep(100000); //wait dsp ready
    return 0;
}
static AX_S32 dsp_deinit(void)
{
    AX_DSP_DisableCore(AX_DSP_ID_0);
    AX_DSP_PowerOff(AX_DSP_ID_0);
    AX_DSP_DisableCore(AX_DSP_ID_1);
    AX_DSP_PowerOff(AX_DSP_ID_1);
    return 0;
}

AX_S32 main(AX_S32 argc, char *argv[])
{
    AX_S32 ret = IVPS_SUCC;
    IVPS_ARG_T *pIvpsArg = &gSampleIvpsMain.tIvpsArg;
    SAMPLE_IVPS_GRP_T *pGrp = &gSampleIvpsGrp;
    SAMPLE_IVPS_GRP_T *pGrpExt = &gSampleIvpsGrpExt;
    IVPS_BLK_T arrBlkInfo[16];
    char strFilePath[32], *pFrameFile;
    AX_U32 BlkSize;

    printf("IVPS Sample. Build at %s %s\n", __DATE__, __TIME__);

    /* SYS global init */
    CHECK_RESULT(AX_SYS_Init());

    IVPS_SetFilterConfig(pGrp);
    pGrp->bSaveFile = AX_TRUE;
    /* Parse the input parameter */
    IVPS_ArgsParser(argc, argv, pIvpsArg);
    if (pIvpsArg->pPipelineIni)
        IVPS_GrpIniParser(pIvpsArg->pPipelineIni, pGrp);
    else
        IVPS_ChnInfoParser(pIvpsArg, &pGrp->tPipelineAttr);
    if (pIvpsArg->pPipelineExtIni)
        IVPS_GrpIniParser(pIvpsArg->pPipelineExtIni, pGrpExt);
    if (pIvpsArg->pChangeIni)
        IVPS_ChangeIniParser(pIvpsArg->pChangeIni, &gSampleChange);
    if (pIvpsArg->pDewarpIni)
        IVPS_DewarpIniParser(pIvpsArg->pDewarpIni, &gSampleDewarp);

    if (pIvpsArg->bLDCH)
    {
        char FilePath[32];
        pGrp->pFilePath = &FilePath[0];

        /* Source image buffer and info get */
        pFrameFile = FrameInfoGet(pIvpsArg->pFrameInfo, &pGrp->tFrameInput);

        arrBlkInfo[0].nSize = pGrp->tFrameInput.u32FrameSize;
        arrBlkInfo[0].nCnt = 6;

        ret = IVPS_CommonPoolCreate(&arrBlkInfo[0], 1);
        if (ret)
        {
            ALOGE("AX_IVPS_Init failed, ret=0x%x.", ret);
            return -1;
        }

        if (pIvpsArg->nStreamNum <= 0)
            FrameBufGet(0, &pGrp->tFrameInput, pFrameFile);

        printf("INPUT nW:%d nH:%d File:%s\n", pGrp->tFrameInput.u32Width, pGrp->tFrameInput.u32Height, pFrameFile);
        pGrp->pFileName = pFrameFile;
        pGrp->pCaseId = pIvpsArg->pCaseId;
        pGrp->nIvpsStreamNum = pIvpsArg->nStreamNum;
        pGrp->nIvpsRepeatNum = pIvpsArg->nRepeatCount;
        pGrp->nRegionNum = pIvpsArg->nRegionNum;

        strcpy(pGrp->pFilePath, pFrameFile);
        FilePathExtract(pGrp->pFilePath); /* The path is also output frame path */

        AX_IVPS_Init();

        SAMPLE_IVPS_SyncApiLDCH(pIvpsArg, pGrp);

        AX_IVPS_Deinit();

        ret = AX_POOL_ReleaseBlock(pGrp->tFrameInput.u32BlkId[0]);
        if (ret)
        {
            ALOGE("IVPS Release BlkId fail, ret = %x", ret);
        }

        AX_POOL_Exit();

        AX_SYS_Deinit();

        printf("SAMPLE_IVPS_SyncApiLDCH END\n");
        return 0;
    }

    /* Create common pool */
    if (pIvpsArg->bFewPool) {
        arrBlkInfo[0].nSize = CalcImgSize(3840, 3840, 2160, AX_FORMAT_RGBA8888, 16);
        arrBlkInfo[0].nCnt = 10;
        ret = IVPS_CommonPoolCreate(&arrBlkInfo[0], 1);
        if (ret)
        {
            ALOGE("AX_IVPS_Init failed, ret=0x%x.", ret);
            goto error0;
        }
    } else {
        arrBlkInfo[0].nSize = CalcImgSize(1920, 1920, 1080, AX_FORMAT_RGBA8888, 16);
        arrBlkInfo[0].nCnt = 15;
        arrBlkInfo[1].nSize = CalcImgSize(3840, 3840, 2160, AX_FORMAT_RGBA8888, 16);
        arrBlkInfo[1].nCnt = 15;
        arrBlkInfo[2].nSize = CalcImgSize(256, 256, 256, AX_FORMAT_RGBA8888, 16);
        arrBlkInfo[2].nCnt = 36;
        arrBlkInfo[3].nSize = CalcImgSize(7680, 7680, 2160, AX_FORMAT_RGBA8888, 16);
        arrBlkInfo[3].nCnt = 2;
        ret = IVPS_CommonPoolCreate(&arrBlkInfo[0], 4);
        if (ret)
        {
            ALOGE("AX_IVPS_Init failed, ret=0x%x.", ret);
            goto error0;
        }
    }
    pGrp->ePoolSrc = pIvpsArg->ePoolSrc;
    if (POOL_SOURCE_USER == pIvpsArg->ePoolSrc) {
        /* Create user pool, get pool id */
        BlkSize = CalcImgSize(4096, 4096, 3840, AX_FORMAT_RGBA8888, 16);
        pGrp->user_pool_id = IVPS_UserPoolCreate(BlkSize, 10);
    }

    ret = dsp_init();
    if (ret) {
        ALOGW("[DSP]init failed., ret=0x%x.", ret);
    }

    /* IVPS initialization */
    ret = AX_IVPS_Init();
    if (ret)
    {
        ALOGE("AX_IVPS_Init failed, ret=0x%x.", ret);
        goto error1;
    }

    /* Source image buffer and info get */
    pFrameFile = FrameInfoGet(pIvpsArg->pFrameInfo, &pGrp->tFrameInput);

    printf("FrameInfoGet\n");
    if (pIvpsArg->nStreamNum <= 0)
        FrameBufGet(0, &pGrp->tFrameInput, pFrameFile);

    printf("INPUT nW:%d nH:%d File:%s\n", pGrp->tFrameInput.u32Width, pGrp->tFrameInput.u32Height, pFrameFile);
    pGrp->pFileName = pFrameFile;
    pGrp->pCaseId = pIvpsArg->pCaseId;
    pGrp->nIvpsStreamNum = pIvpsArg->nStreamNum;
    pGrp->nIvpsRepeatNum = pIvpsArg->nRepeatCount;
    pGrp->nRegionNum = pIvpsArg->nRegionNum;
    pGrpExt->nIvpsRepeatNum = pIvpsArg->nRepeatCount;
    pGrpExt->tFrameInput = pGrp->tFrameInput;

    pGrp->pFilePath = &strFilePath[0];
    strcpy(pGrp->pFilePath, pFrameFile);
    FilePathExtract(pGrp->pFilePath); /* The path is also output frame path */
    pGrpExt->pFilePath = pGrp->pFilePath;

    ALOGI("nRepeatCount:%d nRegionNum:%d", pIvpsArg->nRepeatCount, pIvpsArg->nRegionNum);

    /* IVPS sync api */
    SAMPLE_IVPS_SyncApi(pIvpsArg, pGrp, &gSampleIvpsSyncApi);

    /* IVPS dewarp api */
    SAMPLE_IVPS_Dewarp(&pGrp->tFrameInput, pGrp->pFilePath, &gSampleDewarp);

    /* IVPS pipeline start */
    if (pIvpsArg->nIvpsGrp) {
        pGrp->nIvpsGrp = pIvpsArg->nIvpsGrp;
    }
    ret = IVPS_StartGrp(pGrp, pIvpsArg->bUserFRC);
    if (ret)
    {
        ALOGE("IVPS_StartGrp failed, ret=0x%x.", ret);
        goto error2;
    }

    /* IVPS region start */
    if (pIvpsArg->nRegionNum > 0)
    {
        /* Start region with parameter */
        if (0 != IVPS_StartRegion(pIvpsArg->nRegionNum))
        {
            ThreadLoopStateSet(AX_TRUE);
        }
        IVPS_UpdateThreadStart(pIvpsArg->nRegionNum, pGrp);
    }

    switch (pIvpsArg->nLinkMode)
    {
    case IVPS_LINK_IVPS:
        SAMPLE_IVPS_LinkIvps(pGrp->nIvpsGrp, 0, pGrpExt);
        IVPS_ThreadStart(pGrp, NULL);
        /* Dynamic change channel attributes */
        if (pIvpsArg->pChangeIni)
        {
            pGrp->bSaveFile = AX_FALSE;
            IVPS_FilterAttrThreadStart(pGrp);
            IVPS_FilterAttrThreadStart(pGrpExt);
        }
        break;
    case IVPS_LINK_VENC:
        SAMPLE_IVPS_LinkVenc(pGrp, AX_TRUE);
        IVPS_ThreadStart(pGrp, NULL);
        break;
    case IVPS_LINK_JENC:
        SAMPLE_IVPS_LinkVenc(pGrp, AX_FALSE);
        IVPS_ThreadStart(pGrp, NULL);
        break;
    default:
        IVPS_ThreadStart(pGrp, pGrp);
        /* Dynamic change channel attributes */
        if (pIvpsArg->pChangeIni)
        {
            pGrp->bSaveFile = AX_FALSE;
            IVPS_FilterAttrThreadStart(pGrp);
        }
        break;
    }

    while (!ThreadLoopStateGet() && (pGrp->nIvpsRepeatNum || pGrp->nIvpsStreamNum))
    {
        sleep(1);
        if (pIvpsArg->bProc) {
            system("cat /proc/ax_proc/ivps");
            system("cat /proc/ax_proc/rgn");
            system("cat /proc/ax_proc/gdc");
            system("cat /proc/ax_proc/vdsp");
        }
    }
    if (pIvpsArg->nRepeatCount)
        sleep(1);

    /* Stop IVPS region */
    if (pIvpsArg->nRegionNum > 0)
    {
        IVPS_UpdateThreadStop(pGrp);
        IVPS_StopRegion();
    }

    ThreadLoopStateSet(AX_TRUE);
    if (pIvpsArg->pChangeIni)
    {
        IVPS_FilterAttrThreadStop(pGrp);
        if (IVPS_LINK_IVPS == pIvpsArg->nLinkMode)
        {
            IVPS_FilterAttrThreadStop(pGrpExt);
        }
    }
    /* Stop IVPS pipeline */
    IVPS_ThreadStop(pGrp);
    IVPS_StopGrp(pGrp);
    /* Stop other pipeline */
    switch (pIvpsArg->nLinkMode)
    {
    case IVPS_LINK_IVPS:
        IVPS_ThreadStop(pGrpExt);
        IVPS_StopGrp(pGrpExt);
        break;
    default:
        break;
    }
error2:
    /* IVPS release */
    AX_IVPS_Deinit();

error1:
    /* POOL release */
    if (POOL_SOURCE_USER == pIvpsArg->ePoolSrc) {
        AX_POOL_DestroyPool(pGrp->user_pool_id);
    }
    AX_POOL_Exit();

error0:
    /* SYS release */
    AX_SYS_Deinit();

    dsp_deinit();
    printf("\nsample test run success\n");
    return 0;
}

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


AX_S32 main(AX_S32 argc, char *argv[])
{
    AX_S32 ret = IVPS_SUCC;
    IVPS_ARG_T *pIvpsArg = &gSampleIvpsMain.tIvpsArg;
    SAMPLE_IVPS_GRP_T *pGrp = &gSampleIvpsGrp;
    SAMPLE_IVPS_GRP_T *pGrpExt = &gSampleIvpsGrpExt;
    IVPS_BLK_T arrBlkInfo[16];
    char strFilePath[128], *pFrameFile;
    AX_U32 BlkSize;

    printf("AXCL IVPS Sample. Build at %s %s\n", __DATE__, __TIME__);
    /* Parse the input parameter */
    pIvpsArg->json = "./axcl.json";
    IVPS_ArgsParser(argc, argv, pIvpsArg);

    /* step01: axcl initialize */
    ret = axclInit(pIvpsArg->json);
    if (AXCL_SUCC != ret) {
        printf("axclInit failed, ret=0x%x\n", ret);
        return ret;
    }

    axclrtDeviceList deviceList;
    ret = axclrtGetDeviceList(&deviceList);
    if (AXCL_SUCC != ret || 0 == deviceList.num) {
        printf("no device is connected\n");
        axclFinalize();
        return ret;
    }

    if (pIvpsArg->nDeviceId < 0 || pIvpsArg->nDeviceId >= (AX_S32)deviceList.num) {
        printf("device index %d is out of connected device num %d\n", pIvpsArg->nDeviceId, deviceList.num);
        axclFinalize();
        return 1;
    }

    printf("device index: %d, bus number: %d\n", pIvpsArg->nDeviceId, deviceList.devices[pIvpsArg->nDeviceId]);
    pIvpsArg->nDeviceId = deviceList.devices[pIvpsArg->nDeviceId];

    /* step02: active device */
    ret = axclrtSetDevice(pIvpsArg->nDeviceId);
    if (AXCL_SUCC != ret) {
        axclFinalize();
        return ret;
    }
    pGrp->nDeviceId = pIvpsArg->nDeviceId;


    /* SYS global init */
    CHECK_RESULT(AXCL_SYS_Init());

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

    /* Create common pool */
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
        ivps_err("AXCL_IVPS_Init failed, ret=0x%x.", ret);
        goto error0;
    }
    pGrp->ePoolSrc = pIvpsArg->ePoolSrc;
    if (POOL_SOURCE_USER == pIvpsArg->ePoolSrc) {
        /* Create user pool, get pool id */
        BlkSize = CalcImgSize(4096, 4096, 3840, AX_FORMAT_RGBA8888, 16);
        pGrp->user_pool_id = IVPS_UserPoolCreate(BlkSize, 10);
    }

    /* IVPS initialization */
    ret = AXCL_IVPS_Init();
    if (ret)
    {
        ivps_err("AXCL_IVPS_Init failed, ret=0x%x.", ret);
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
    FilePathExtract(pGrp->pFilePath);  /* The path is also output frame path */
    pGrpExt->pFilePath = pGrp->pFilePath;

    /* IVPS sync api */
    SAMPLE_IVPS_SyncApi(pIvpsArg, pGrp, &gSampleIvpsSyncApi);

    ALOGI("nRepeatCount:%d nRegionNum:%d", pIvpsArg->nRepeatCount, pIvpsArg->nRegionNum);

    /* IVPS pipeline start */
    if (pIvpsArg->nIvpsGrp) {
        pGrp->nIvpsGrp = pIvpsArg->nIvpsGrp;
    }
    ret = IVPS_StartGrp(pGrp, pIvpsArg->bUserFRC);
    if (ret)
    {
        ivps_err("IVPS_StartGrp failed, ret=0x%x.", ret);
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
    AXCL_IVPS_Deinit();

error1:
    /* POOL release */
    if (POOL_SOURCE_USER == pIvpsArg->ePoolSrc) {
        AXCL_POOL_DestroyPool(pGrp->user_pool_id);
    }
    AXCL_POOL_Exit();

error0:
    /* SYS release */
    AXCL_SYS_Deinit();

    /* step12: deinit axcl */
    ALOGI("axcl deinit");
    axclrtResetDevice(pIvpsArg->nDeviceId);
    axclFinalize();

    printf("\nsample test run success\n");
    return 0;
}

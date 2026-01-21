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
#include <signal.h>
#include <assert.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

#include "common_vdec_api.h"

AX_U64 g_u64GetFrmTag = 0;

int main(int argc, char *argv[])
{
    AX_S32 s32Ret = 0;
    AX_S32 sRet = 0;
    int ret = 0;

    SAMPLE_VDEC_CMD_PARAM_T *pstCmd = NULL;
    SAMPLE_VDEC_CONTEXT_T *pstVdecCtx = NULL;

    s32Ret = AX_SYS_Init();
    if (AX_SUCCESS != s32Ret) {
        SAMPLE_CRIT_LOG("AX_SYS_Init FAILED! ret:0x%x\n", s32Ret);
        goto ERR_RET;
    }

    s32Ret = Sample_VdecTestBenchInit(argc, argv, &pstVdecCtx);
    if (AX_SUCCESS != s32Ret) {
        SAMPLE_CRIT_LOG("Sample_VdecTestBenchInit FAILED! ret:0x%x\n", s32Ret);
        goto ERR_RET_SYS_DEINIT;
    }

    pstCmd = &pstVdecCtx->tCmdParam;
    ret = VdecDefaultParamsSet(pstCmd);
    if (ret) {
        SAMPLE_CRIT_LOG("VdecDefaultParamsSet FAILED! ret:0x%x\n", ret);
        goto ERR_RET_SYS_DEINIT;
    }

    ret = VdecCmdLineParseAndCheck(argc, argv, pstCmd, 0, 0);
    if (ret) {
        SAMPLE_CRIT_LOG("VdecCmdLineParseAndCheck FAILED! ret:0x%x\n", ret);
        goto ERR_RET_SYS_DEINIT;
    }

    g_u64GetFrmTag = 0;

    AX_VDEC_MOD_ATTR_T stModAttr;
    stModAttr.enDecModule = pstCmd->enDecModule;
    stModAttr.u32MaxGroupCount = pstCmd->uMaxGrpCnt;
    stModAttr.bVdecMc = pstCmd->McEnable;
    stModAttr.VdecVirtChn = pstCmd->s32VdecVirtChn;

    s32Ret = AX_VDEC_Init(&stModAttr);
    if (AX_SUCCESS != s32Ret) {
        SAMPLE_CRIT_LOG("AX_VDEC_Init FAILED! ret:0x%x %s\n", s32Ret, AX_VdecRetStr(s32Ret));
        goto ERR_RET_SYS_DEINIT;
    }

    if (pstCmd->bJpegDecOneFrm) {
        s32Ret = Sample_VdecJpegDecodeOneFrame(pstCmd);
        if(0 != s32Ret) {
            SAMPLE_CRIT_LOG("Sample_VdecJpegDecodeOneFrame FAILED! ret:0x%x\n", s32Ret);
            goto ERR_RET_VDEC_DEINIT;
        }

        goto FUNC_QUIT;
    }

    s32Ret = Sample_VdecTestBenchMain(&pstVdecCtx);
    if (AX_SUCCESS != s32Ret) {
        SAMPLE_CRIT_LOG("Sample_VdecTestBenchMain FAILED! ret:0x%x\n", s32Ret);
        goto ERR_RET_VDEC_DEINIT;
    }

    if (NULL != pstVdecCtx) {
        for (int ci = 0; ci < AX_VDEC_MAX_CHN_NUM; ci++) {
            if (!pstCmd->tChnCfg[ci].bChnEnable) continue;
            if (NULL != pstCmd->tChnCfg[ci].pOutputFilePath) {
                AX_OS_MEM_Free(AX_ID_USER, pstCmd->tChnCfg[ci].pOutputFilePath);
                pstCmd->tChnCfg[ci].pOutputFilePath = NULL;
            }

            if (NULL != pstCmd->tChnCfg[ci].pUsrPicFilePath) {
                AX_OS_MEM_Free(AX_ID_USER, pstCmd->tChnCfg[ci].pUsrPicFilePath);
                pstCmd->tChnCfg[ci].pUsrPicFilePath = NULL;
            }
        }

        if (NULL != pstCmd->pInputFilePath) {
            AX_OS_MEM_Free(AX_ID_USER, pstCmd->pInputFilePath);
            pstCmd->pInputFilePath = NULL;
        }

        AX_OS_MEM_Free(AX_ID_USER, pstVdecCtx);
        pstVdecCtx = NULL;
    }

FUNC_QUIT:
    sRet = AX_VDEC_Deinit();
    if (AX_SUCCESS != sRet) {
        SAMPLE_CRIT_LOG("AX_VDEC_Deinit FAILED! ret:0x%x %s\n", sRet, AX_VdecRetStr(sRet));
        return sRet;
    }

    if (g_u64GetFrmTag) {
        SAMPLE_LOG_T("sample_vdec running status: Decode Finished! \n\n");
    } else {
        SAMPLE_CRIT_LOG("sample_vdec running status: Decode Failed! please check code \n\n");
    }

    sRet = AX_SYS_Deinit();
    if (AX_SUCCESS != sRet) {
        SAMPLE_CRIT_LOG("AX_SYS_Deinit FAILED! ret:0x%x\n", sRet);
        return sRet;
    }


    return 0;

ERR_RET_VDEC_DEINIT:
    sRet = AX_VDEC_Deinit();
    if (AX_SUCCESS != sRet) {
        SAMPLE_CRIT_LOG("AX_VDEC_Deinit FAILED! ret:0x%x %s\n", sRet, AX_VdecRetStr(sRet));
    }
ERR_RET_SYS_DEINIT:
    sRet = AX_SYS_Deinit();
    if (AX_SUCCESS != sRet) {
        SAMPLE_CRIT_LOG("AX_SYS_Deinit FAILED! ret:0x%x\n", sRet);
    }

    if (NULL != pstVdecCtx) {
        for (int ci = 0; ci < AX_VDEC_MAX_CHN_NUM; ci++) {
            if (NULL != pstCmd->tChnCfg[ci].pOutputFilePath) {
                AX_OS_MEM_Free(AX_ID_USER, pstCmd->tChnCfg[ci].pOutputFilePath);
                pstCmd->tChnCfg[ci].pOutputFilePath = NULL;
            }

            if (NULL != pstCmd->tChnCfg[ci].pUsrPicFilePath) {
                AX_OS_MEM_Free(AX_ID_USER, pstCmd->tChnCfg[ci].pUsrPicFilePath);
                pstCmd->tChnCfg[ci].pUsrPicFilePath = NULL;
            }
        }

        if (NULL != pstCmd->pInputFilePath) {
            AX_OS_MEM_Free(AX_ID_USER, pstCmd->pInputFilePath);
            pstCmd->pInputFilePath = NULL;
        }

        AX_OS_MEM_Free(AX_ID_USER, pstVdecCtx);
        pstVdecCtx = NULL;
    }

ERR_RET:
    return s32Ret || sRet || ret;
}

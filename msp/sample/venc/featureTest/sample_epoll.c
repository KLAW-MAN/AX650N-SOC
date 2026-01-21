/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include <signal.h>
#include <sys/poll.h>
#include <sys/epoll.h>
#include <sys/time.h>

#include "sample_global.h"
#include "sample_unit_test.h"

#define SAMPLE_TEST_GRP_NUM (1)

static SAMPLE_VENC_SENDFRAME_PARA_T gstFrmParam[MAX_VENC_CHN_NUM];
static SAMPLE_VENC_GETSTREAM_PARA_T gstStrmParam[MAX_VENC_CHN_NUM];

#define SAMPLE_STRM_NAME_LEN     (20)
#define SAMPLE_MAX_EVENTS        MAX_VENC_CHN_NUM

#define VENC_EPOLL_LEVEL_TRIGGER (0)
#define VENC_EPOLL_EDGE_TRIGGER  (1)

static AX_CHAR gSelectStrmName[MAX_VENC_CHN_NUM][SAMPLE_STRM_NAME_LEN];
static FILE *gStrmFile[MAX_VENC_CHN_NUM] = { NULL };

static AX_BOOL gLoopExit = AX_FALSE;
static void SigHandler(AX_S32 signo)
{
    SAMPLE_LOG("catch signal(%d).\n", signo);
    gLoopExit = AX_TRUE;
}

static AX_VOID *SAMPLE_VENC_EpollGetStreamProc(AX_VOID *arg);
static AX_S32 SAMPLE_VENC_EpollStartGetStream(SAMPLE_VENC_GETSTREAM_PARA_T *pstArg);
static AX_S32 SAMPLE_VENC_EpollStopGetStream(SAMPLE_VENC_GETSTREAM_PARA_T *pstArg);
static AX_VOID SampleEpollGetStreamInit(SAMPLE_VENC_GETSTREAM_PARA_T *pstArg, SAMPLE_VENC_CMD_PARA_T *pCml);

AX_S32 UTestEpoll(SAMPLE_VENC_CMD_PARA_T *pCml)
{
    AX_S32 s32Ret;
    AX_U32 chnNum;
    AX_S32 chnIdx;
    AX_PAYLOAD_TYPE_E enType;
    SAMPLE_VENC_RC_E enRcMode = pCml->rcMode;
    AX_U64 gVencCaseLoopExit = 0;

    chnNum = (pCml->defaultUt == VENC_TEST_ALL_CASE) ? UT_DEFAULT_ENC_NUM : pCml->chnNum;

    signal(SIGINT, SigHandler);

    if (chnNum < 1)
        return -1;

    for (chnIdx = 0; chnIdx < chnNum; chnIdx++) {
        memset(&gstFrmParam[chnIdx], 0, sizeof(SAMPLE_VENC_SENDFRAME_PARA_T));
        memset(&gstStrmParam[chnIdx], 0, sizeof(SAMPLE_VENC_GETSTREAM_PARA_T));

        if (pCml->bChnCustom)
            enType = SampleGetCodecType(pCml->codecType);
        else
            enType = SampleGetCodecType(chnIdx % SAMPLE_ALL_CODEC_TYPE);

        if (PT_H264 == enType)
            sprintf(gSelectStrmName[chnIdx], "es_chn%d_ut%d.264", chnIdx, pCml->ut);
        else if (PT_H265 == enType)
            sprintf(gSelectStrmName[chnIdx], "es_chn%d_ut%d.265", chnIdx, pCml->ut);
        else if (PT_MJPEG == enType)
            sprintf(gSelectStrmName[chnIdx], "es_chn%d_ut%d.mjpg", chnIdx, pCml->ut);
        else if (PT_JPEG == enType)
            sprintf(gSelectStrmName[chnIdx], "es_chn%d_ut%d.jpg", chnIdx, pCml->ut);

        if (SampleInvalidEnType(pCml->ut, enType, pCml->rcMode))
            continue;

        if (PT_BUTT == enType) {
            SAMPLE_LOG_ERR("chn-%d: Invalid codec type!\n", chnIdx);
            s32Ret = -1;
            goto FREE;
        }

        s32Ret = COMMON_VENC_Start(chnIdx, enType, enRcMode, pCml);
        if (AX_SUCCESS != s32Ret) {
            SAMPLE_LOG_ERR("chn-%d: COMMON_VENC_Start failed.\n", chnIdx);
            s32Ret = -1;
            goto FREE;
        }

        SampleSendFrameInit(chnIdx, enType, &gstFrmParam[chnIdx], pCml);

        s32Ret = SAMPLE_VENC_StartSendFrame(&gstFrmParam[chnIdx]);
        if (AX_SUCCESS != s32Ret) {
            SAMPLE_LOG_ERR("chn-%d: SAMPLE_VENC_StartSendFrame err.\n", chnIdx);
            s32Ret = -1;
            goto FREE;
        }

        COMMON_VENC_AdjustLoopExit(&gVencCaseLoopExit, chnIdx);
        sleep(1);
    }

    for (AX_S32 grp = 0; grp < SAMPLE_TEST_GRP_NUM; grp++) {
        SampleEpollGetStreamInit(&gstStrmParam[grp], pCml);

        gstStrmParam[grp].startChn = grp;
        gstStrmParam[grp].grpId = ((pCml->grpId + grp) % MAX_VENC_GRP_NUM);

        s32Ret = SAMPLE_VENC_EpollStartGetStream(&gstStrmParam[grp]);
        if (AX_SUCCESS != s32Ret) {
            SAMPLE_LOG_ERR("grp-%d: SAMPLE_VENC_SltStartGetStream err.\n", gstStrmParam[grp].grpId);
            s32Ret = -1;
            goto FREE;
        }

        if (chnNum < 2) {
            SAMPLE_LOG_DEBUG("only one channel, just create one group!\n");
            break;
        }
    }

    while ((!gLoopExit) && (gVencCaseLoopExit != pCml->vencLoopExit))
        sleep(2);

FREE:

    for (AX_S32 chnIdx = 0; chnIdx < chnNum; chnIdx++) {
        SAMPLE_VENC_StopSendFrame(&gstFrmParam[chnIdx]);
        COMMON_VENC_Stop(chnIdx);
    }

    for (AX_S32 grp = 0; grp < SAMPLE_TEST_GRP_NUM; grp++) {
        SAMPLE_VENC_EpollStopGetStream(&gstStrmParam[grp]);
    }

    pCml->vencLoopExit = 0;

    return s32Ret;
}

static AX_S32 SAMPLE_VENC_GetChnIdByFd(AX_S32 fd, AX_S32 fdSet[])
{
    AX_S32 chn = -1;

    if (fd <= 0) {
        SAMPLE_LOG_ERR("Invalid fd(%d).\n", fd);
        return -1;
    }

    for (AX_S32 i = 0; i < MAX_VENC_CHN_NUM; i++) {
        if (fd == fdSet[i]) {
            chn = i;
            break;
        }
    }

    return chn;
}

static AX_VOID *SAMPLE_VENC_EpollGetStreamProc(AX_VOID *arg)
{
    AX_S32 s32Ret = -1;
    AX_VENC_STREAM_T stStream;

    AX_S32 maxFd = 0;
    AX_S32 VencFd[MAX_VENC_CHN_NUM] = { 0 };
    struct epoll_event event;
    struct epoll_event event_set[SAMPLE_MAX_EVENTS];
    int epoll_fd;
    int ready_events;
    AX_VENC_CHN_STATUS_T stStat;

    SAMPLE_VENC_GETSTREAM_PARA_T *pstArg = (SAMPLE_VENC_GETSTREAM_PARA_T *)arg;
    SAMPLE_VENC_CMD_PARA_T *pCml = (SAMPLE_VENC_CMD_PARA_T *)pstArg->ptrPrivate;
    int chnNum = pstArg->chnNum;

    memset(&stStream, 0, sizeof(stStream));
    memset(&event, 0, sizeof(event));
    memset(&stStat, 0, sizeof(stStat));

    SAMPLE_VENC_SetThreadName("VencEpollGetStream");

    epoll_fd = epoll_create(1);
    if (epoll_fd == -1) {
        perror("epoll_create1");
        goto EXIT;
    }

    for (AX_S32 i = 0; i < chnNum; i++) {
        VencFd[i] = AX_VENC_GetFd(i);
        if (VencFd[i] <= 0) {
            SAMPLE_LOG_ERR("VencFd[%d]=%d is invalid.\n", i, VencFd[i]);
            goto EXIT;
        }

        if (maxFd < VencFd[i])
            maxFd = VencFd[i];

        event.events = EPOLLIN;
        event.data.fd = VencFd[i];

        if (VENC_EPOLL_EDGE_TRIGGER == pCml->epollType)
            event.events |= EPOLLET;

        if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, VencFd[i], &event) == -1) {
            perror("epoll_ctl");
            goto EXIT;
        }
    }

    while (pstArg->bGetStrmStart) {
        SAMPLE_LOG_DEBUG("epoll wait +++\n");
        ready_events = epoll_wait(epoll_fd, event_set, SAMPLE_MAX_EVENTS, pstArg->syncType);
        if (ready_events < 0) {
            perror("epoll_wait");
            goto EXIT;
        }

        if (0 == ready_events)
            continue;

        SAMPLE_LOG_DEBUG("epoll wait ---\n");

        for (AX_S32 i = 0; i < ready_events; i++) {
            AX_S32 fd = event_set[i].data.fd;
            AX_S32 chn = SAMPLE_VENC_GetChnIdByFd(fd, VencFd);
            if (chn < 0) {
                SAMPLE_LOG("unknown fd(%d).\n", fd);
                continue;
            }

            do {
                AX_VENC_QueryStatus(chn, &stStat);
                s32Ret = AX_VENC_GetStream(chn, &stStream, 0);
                if (AX_SUCCESS == s32Ret) {
                    if (NULL == gStrmFile[chn])
                        gStrmFile[chn] = fopen(gSelectStrmName[chn], "wb");

                    SAMPLE_VENC_FWRITE(stStream.stPack.pu8Addr, 1, stStream.stPack.u32Len, gStrmFile[chn],
                                       pstArg->bSaveStrm);
                    fflush(gStrmFile[chn]);

                    pstArg->totalGetStream++;

                    SAMPLE_LOG_DEBUG("chn-%d: get stream success, addr=%p, len=%u, codeType=%d, seqNum=%llu.\n", chn,
                                     stStream.stPack.pu8Addr, stStream.stPack.u32Len, stStream.stPack.enCodingType,
                                     stStream.stPack.u64SeqNum);

                    s32Ret = AX_VENC_ReleaseStream(chn, &stStream);
                    if (AX_SUCCESS != s32Ret) {
                        SAMPLE_LOG("chn-%d: AX_VENC_ReleaseStream failed!\n", chn);
                        goto EXIT;
                    }
                } else {
                    SAMPLE_LOG("chn-%d: get stream failed, ret=0x%x\n", chn, s32Ret);
                }
            } while (pCml->epollType && stStat.u32LeftStreamFrames > 0);
        }
    }

EXIT:

    if (-1 != epoll_fd)
        close(epoll_fd);

    for (AX_S32 i = 0; i < pCml->chnNum; i++) {
        if (gStrmFile[i] != NULL) {
            fclose(gStrmFile[i]);
            gStrmFile[i] = NULL;
        }
    }

    SAMPLE_LOG("Epoll total get %llu encoded frames. getStream Exit!\n", pstArg->totalGetStream);

    return (void *)(intptr_t)s32Ret;
}

static AX_S32 SAMPLE_VENC_EpollStartGetStream(SAMPLE_VENC_GETSTREAM_PARA_T *pstArg)
{
    AX_S32 s32Ret = 0;

    s32Ret = pthread_create(&pstArg->getStrmPid, 0, SAMPLE_VENC_EpollGetStreamProc, (AX_VOID *)pstArg);
    if (s32Ret) {
        SAMPLE_LOG_ERR("Select: pthread create err, ret=%d!\n", s32Ret);
        return s32Ret;
    }

    return s32Ret;
}

static AX_S32 SAMPLE_VENC_EpollStopGetStream(SAMPLE_VENC_GETSTREAM_PARA_T *pstArg)
{
    if (pstArg->bGetStrmStart) {
        pstArg->bGetStrmStart = AX_FALSE;
        pthread_join(pstArg->getStrmPid, 0);
    }

    return AX_SUCCESS;
}

static AX_VOID SampleEpollGetStreamInit(SAMPLE_VENC_GETSTREAM_PARA_T *pstArg, SAMPLE_VENC_CMD_PARA_T *pCml)
{
    pstArg->bGetStrmStart = AX_TRUE;
    /* 100ms */
    pstArg->syncType = pCml->syncGet;
    pstArg->testId = pCml->ut;
    pstArg->gopMode = pCml->gopMode;
    pstArg->temporalID = pCml->temporalID;
    pstArg->bGetStrmBufInfo = pCml->bGetStrmBufInfo;
    pstArg->bQueryStatus = pCml->bQueryStatus;
    pstArg->bSaveStrm = pCml->bSaveStrm;
    pstArg->chnNum = pCml->chnNum;
    pstArg->grpId = pCml->grpId;
    pstArg->startChn = 0;
    pstArg->ptrPrivate = (AX_VOID *)pCml;
}
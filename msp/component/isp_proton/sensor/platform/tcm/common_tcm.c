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
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <dlfcn.h>
#include <pthread.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#include "ax_buffer_tool.h"
//#include "common_config.h"
#include "common_vin.h"
#include "common_type.h"
//#include "common_config.h"
#include "common_tcm.h"

AX_S32 AX_TCM_SetSensorVts(AX_U8 nDevId, const AX_VIN_SENSOR_VTS_T *ptSensorVts)
{
    AX_S32 retval = 0;
    int fd = -1;

    fd = open(AX_TCM_NODE, O_RDWR);
    if (fd < 0) {
        printf("Open AX_TCM_NODE fail!\n");
        return -1;
    }
    retval = ioctl(fd, TCM_CMD_DEV_SENSOR_VTS_SET, ptSensorVts);
    if (retval < 0) {
        printf("CMD TCM_CMD_DEV_SENSOR_VTS_SET error!\n");
    }

    close(fd);

    return retval;
}

AX_S32 AX_TCM_SetSyncPowerAttr(AX_U8 nDevId, const AX_VIN_POWER_SYNC_ATTR_T *ptPowerAttr)
{
    AX_S32 retval = 0;
    int fd = -1;

    fd = open(AX_TCM_NODE, O_RDWR);
    if (fd < 0) {
        printf("Open AX_TCM_NODE fail!\n");
        return -1;
    }
    retval = ioctl(fd, TCM_CMD_DEV_SYNC_POWERATTR_SET, ptPowerAttr);
    if (retval < 0) {
        printf("CMD IOC_FTC_DEV_SYNC_POWERATTR_SET error!\n");
    }

    close(fd);

    return retval;
}
AX_S32 AX_TCM_GetSyncPowerAttr(AX_U8 nDevId, const AX_VIN_POWER_SYNC_ATTR_T *ptPowerAttr)
{
    AX_S32 retval = 0;
    int fd = -1; 

    fd = open(AX_TCM_NODE, O_RDWR);
    if (fd < 0) {
        printf("Open AX_TCM_NODE fail!\n");
        return -1;
    }
    retval = ioctl(fd, TCM_CMD_DEV_SYNC_POWERATTR_GET, ptPowerAttr);
    if (retval < 0) {
        printf("CMD IOC_FTC_DEV_SYNC_POWERATTR_SET error!\n");
    }
    printf("GET :%d\n",ptPowerAttr->nSyncDelayElcUs);
    close(fd);

    return retval;
}
AX_S32 AX_TCM_SetVSyncAttr(AX_U8 nDevId, const AX_VIN_SYNC_SIGNAL_ATTR_T *ptVsyncAttr)
{
    AX_S32 retval = 0;
    int fd = -1;

    fd = open(AX_TCM_NODE, O_RDWR);
    if (fd < 0) {
        printf("Open AX_TCM_NODE fail!\n");
        return -1;
    }
    retval = ioctl(fd, TCM_CMD_DEV_VSYNCATTR_SET, ptVsyncAttr);
    if (retval < 0) {
        printf("CMD IOC_FTC_DEV_SYNC_POWERATTR_SET error!\n");
    }

    close(fd);

    return retval;
}
AX_S32 AX_TCM_GetVSyncAttr(AX_U8 nDevId, const AX_VIN_SYNC_SIGNAL_ATTR_T *ptVsyncAttr)
{
    AX_S32 retval = 0;
    int fd = -1;

    fd = open(AX_TCM_NODE, O_RDWR);
    if (fd < 0) {
        printf("Open AX_TCM_NODE fail!\n");
        return -1;
    }
    retval = ioctl(fd, TCM_CMD_DEV_VSYNCATTR_GET, ptVsyncAttr);
    if (retval < 0) {
        printf("CMD IOC_FTC_DEV_SYNC_POWERATTR_SET error!\n");
    }
    close(fd);

    return retval;
}

AX_S32 AX_TCM_EnableVSync(AX_U8 nDevId)
{
    AX_S32 retval = 0;

    int fd = -1;
    AX_BOOL bEable = AX_TRUE;

    fd = open(AX_TCM_NODE, O_RDWR);
    if (fd < 0) {
        printf("Open AX_TCM_NODE fail!\n");
        return -1;
        }
    retval = ioctl(fd, TCM_CMD_DEV_VSYNC_ENABLE_SET, &bEable);
    if (retval < 0) {
        printf("CMD IOC_FTC_DEV_SYNC_POWERATTR_SET error!\n");
    }
    close(fd);

    return retval;
}

AX_S32 AX_TCM_DisableVSync(AX_U8 nDevId)
{
    AX_S32 retval = 0;
    AX_BOOL bEable = AX_FALSE;

    int fd = -1;
    fd = open(AX_TCM_NODE, O_RDWR);
    if (fd < 0) {
        printf("Open AX_TCM_NODE fail!\n");
        return -1;
    }
    retval = ioctl(fd, TCM_CMD_DEV_VSYNC_ENABLE_SET, &bEable);
    if (retval < 0) {
        printf("CMD IOC_FTC_DEV_SYNC_POWERATTR_SET error!\n");
    }
    close(fd);

    return retval;
}

AX_S32 AX_TCM_SetHSyncAttr(AX_U8 nDevId, const AX_VIN_SYNC_SIGNAL_ATTR_T *ptVsyncAttr)
{
    AX_S32 retval = 0;
    int fd = -1;

    fd = open(AX_TCM_NODE, O_RDWR);
    if (fd < 0) {
        printf("Open AX_TCM_NODE fail!\n");
        return -1;
    }
    retval = ioctl(fd, TCM_CMD_DEV_HSYNCATTR_SET, ptVsyncAttr);
    if (retval < 0) {
        printf("CMD IOC_FTC_DEV_SYNC_POWERATTR_SET error!\n");
    }

    close(fd);

    return retval;
}
AX_S32 AX_TCM_GetHSyncAttr(AX_U8 nDevId, const AX_VIN_SYNC_SIGNAL_ATTR_T *ptVsyncAttr)
{
    AX_S32 retval = 0;
    int fd = -1;

    fd = open(AX_TCM_NODE, O_RDWR);
    if (fd < 0) {
        printf("Open AX_TCM_NODE fail!\n");
        return -1;
    }
    retval = ioctl(fd, TCM_CMD_DEV_HSYNCATTR_GET, ptVsyncAttr);
    if (retval < 0) {
        printf("CMD IOC_FTC_DEV_SYNC_POWERATTR_SET error!\n");
    }
    close(fd);

    return retval;
}
AX_S32 AX_TCM_EnableHSync(AX_U8 nDevId)
{
    AX_S32 retval = 0;
    AX_BOOL bEable = AX_TRUE;

    int fd = -1;
    fd = open(AX_TCM_NODE, O_RDWR);
    if (fd < 0) {
        printf("Open AX_TCM_NODE fail!\n");
        return -1;
    }
    retval = ioctl(fd, TCM_CMD_DEV_HSYNC_ENABLE_SET, &bEable);
    if (retval < 0) {
        printf("CMD IOC_FTC_DEV_SYNC_POWERATTR_SET error!\n");
    }
    close(fd);

    return retval;
}

AX_S32 AX_TCM_DisableHSync(AX_U8 nDevId)
{
    AX_S32 retval = 0;
    AX_BOOL bEable = AX_FALSE;

    int fd = -1;
    fd = open(AX_TCM_NODE, O_RDWR);
    if (fd < 0) {
        printf("Open AX_TCM_NODE fail!\n");
        return -1;
    }
    retval = ioctl(fd, TCM_CMD_DEV_HSYNC_ENABLE_SET, &bEable);
    if (retval < 0) {
        printf("CMD IOC_FTC_DEV_SYNC_POWERATTR_SET error!\n");
    }
    close(fd);

    return retval;
}
AX_S32 AX_TCM_SetLightSyncInfo(AX_U8 nDevId, const AX_VIN_LIGHT_SYNC_INFO_T *ptLightSyncInfo)
{
    AX_S32 retval = 0;

    int fd = -1;
    fd = open(AX_TCM_NODE, O_RDWR);
    if (fd < 0) {
        printf("Open AX_TCM_NODE fail!\n");
        return -1;
    }
    retval = ioctl(fd, TCM_CMD_DEV_LIGHTSYNCINFO_SET, ptLightSyncInfo);
    if (retval < 0) {
        printf("CMD IOC_FTC_DEV_SYNC_POWERATTR_SET error!\n");
    }
    close(fd);

    return retval;
}

AX_S32 AX_TCM_GetLightSyncInfo(AX_U8 nDevId, AX_VIN_LIGHT_SYNC_INFO_T *ptLightSyncInfo)
{
    AX_S32 retval = 0;

    int fd = -1;
    fd = open(AX_TCM_NODE, O_RDWR);
    if (fd < 0) {
        printf("Open AX_TCM_NODE fail!\n");
        return -1;
    }
    retval = ioctl(fd, TCM_CMD_DEV_LIGHTSYNCINFO_GET, ptLightSyncInfo);
    if (retval < 0) {
        printf("CMD IOC_FTC_DEV_SYNC_POWERATTR_SET error!\n");
    }
    close(fd);

    return retval;
}

AX_S32 AX_TCM_SetFlashTimingAttr(AX_U8 nDevId, AX_U8 nFlashIdx,
        const AX_VIN_FLASH_LIGHT_TIMING_ATTR_T *ptSnapFlashAttr)
{
    AX_S32 retval = 0;

    int fd = -1;
    AX_LIGHT_SYNC_SIGNAL_T tLightAttr = {0};
    tLightAttr.nLightIdx = nFlashIdx;
    tLightAttr.eLightType = IOC_VIN_SYNC_LIGHT_MODE_FLASH;
    tLightAttr.eLightSyncInv = ptSnapFlashAttr->eFlashSyncInv;
    tLightAttr.nLightDelayTime = ptSnapFlashAttr->nFlashDelayTime;
    tLightAttr.nLightDutyTime = ptSnapFlashAttr->nFlashDutyTime;

    fd = open(AX_TCM_NODE, O_RDWR);
    if (fd < 0) {
        printf("Open AX_TCM_NODE fail!\n");
        return -1;

        }
    retval = ioctl(fd, TCM_CMD_DEV_LIGHTSYNCATTR_SET, &tLightAttr);
    if (retval < 0) {
        printf("CMD IOC_FTC_DEV_SYNC_POWERATTR_SET error!\n");
    }
    close(fd);
    return retval;
}

AX_S32 AX_TCM_GetFlashTimingAttr(AX_U8 nDevId, AX_U8 nFlashIdx,
        AX_VIN_FLASH_LIGHT_TIMING_ATTR_T *ptSnapFlashAttr)
{
    AX_S32 retval = 0;

    int fd = -1;
    AX_LIGHT_SYNC_SIGNAL_T tLightAttr = {0};

    fd = open(AX_TCM_NODE, O_RDWR);
    if (fd < 0) {
        printf("Open AX_TCM_NODE fail!\n");
        return -1;

    }
    retval = ioctl(fd, TCM_CMD_DEV_LIGHTSYNCATTR_SET, &tLightAttr);
    if (retval < 0) {
        printf("CMD IOC_FTC_DEV_SYNC_POWERATTR_SET error!\n");
    }
    close(fd);

    ptSnapFlashAttr->eFlashSyncInv = tLightAttr.eLightSyncInv;
    ptSnapFlashAttr->nFlashDelayTime = tLightAttr.nLightDelayTime;
    ptSnapFlashAttr->nFlashDutyTime = tLightAttr.nLightDutyTime;

    return retval;
}
AX_S32 AX_TCM_TriggerFlash(AX_U8 nDevId, AX_U8 nFlashIdx, const AX_VIN_FLASH_TRIGGER_DATA_T *ptFlashData)
{
    AX_S32 retval = 0;

    int fd = -1;
    AX_LIGHT_SYNC_ENABLE_T tLightEn = {0};
    tLightEn.nLightIdx = nFlashIdx;
    tLightEn.bLightEnable = AX_TRUE;
    tLightEn.eLightType = IOC_VIN_SYNC_LIGHT_MODE_FLASH;
    memcpy(&tLightEn.tTriggerData, ptFlashData, sizeof(tLightEn.tTriggerData));

    fd = open(AX_TCM_NODE, O_RDWR);
    if (fd < 0) {
        printf("Open AX_TCM_NODE fail!\n");
        return -1;

        }
    retval = ioctl(fd, TCM_CMD_DEV_LIGHT_ENABLE_SET, &tLightEn);
    if (retval < 0) {
        printf("CMD IOC_FTC_DEV_SYNC_POWERATTR_SET error!\n");
    }
    close(fd);
    return retval;
}

AX_S32 AX_TCM_InterruptEn(AX_VOID)
{
    AX_S32 retval = 0;

    int fd = -1;

    fd = open(AX_TCM_NODE, O_RDWR);
    if (fd < 0) {
        printf("Open AX_TCM_NODE fail!\n");
        return -1;

        }
    retval = ioctl(fd, TCM_CMD_DEV_FTC_INTERRUPT_EN, AX_NULL);
    if (retval < 0) {
        printf("CMD IOC_FTC_DEV_SYNC_POWERATTR_SET error!\n");
    }
    close(fd);
    return retval;
}
AX_VIN_FLASH_TRIGGER_DATA_T gSc910gsTriggerAttr = {
    .nTriggerNum = 1,
    .nIntervalFrmNum =1,
    .eTriggerSelect =AX_VIN_SYNC_TRIGGER_INSERT,
    .nUserData = 0x00,
};
void* test_trigger(void* arg) {
    while (1) {
        AX_TCM_TriggerFlash(0, LIGHT_FLASH, &gSc910gsTriggerAttr);
        usleep(100000);
    }
    return NULL;
}

AX_S32 COMMON_TCM_StartOutsideDev(AX_U8 devId, AX_VIN_SENSOR_VTS_T *ptSensorVts, AX_VIN_POWER_SYNC_ATTR_T *ptSyncPowerAttr,
                                  AX_VIN_SYNC_SIGNAL_ATTR_T *ptVsyncAttr, AX_VIN_SYNC_SIGNAL_ATTR_T *ptHsyncAttr,
                                  AX_VIN_LIGHT_SYNC_INFO_T *ptLightSyncInfo, AX_VIN_STROBE_LIGHT_TIMING_ATTR_T *ptSnapStrobeAttr,
                                  AX_VIN_FLASH_LIGHT_TIMING_ATTR_T *ptSnapFlashAttr)
{
    AX_S32 axRet;
    pthread_t thread;

    axRet = AX_TCM_SetSensorVts(devId, ptSensorVts);
    if (0 != axRet) {
        COMM_VIN_PRT("AX_TCM_SetSensorVts failed, nRet=0x%x.\n", axRet);
        return -1;
    }

    axRet = AX_TCM_SetSyncPowerAttr(devId, ptSyncPowerAttr);
    if (0 != axRet) {
        COMM_VIN_PRT("AX_VIN_SetSyncTriggerMode failed, nRet=0x%x.\n", axRet);
        return -1;
    }

    axRet = AX_TCM_SetLightSyncInfo(devId, ptLightSyncInfo);
    if (0 != axRet) {
        COMM_VIN_PRT("AX_VIN_SetLightSyncInfo failed, nRet=0x%x.\n", axRet);
        return -1;
    }
    axRet = AX_TCM_SetVSyncAttr(devId, ptVsyncAttr);
    if (0 != axRet) {
        COMM_VIN_PRT("AX_VIN_SetVSyncAttr failed, nRet=0x%x.\n", axRet);
        return -1;
    }

    axRet = AX_TCM_EnableVSync(devId);
    if (0 != axRet) {
        COMM_VIN_PRT("AX_VIN_EnableVSync failed, nRet=0x%x.\n", axRet);
        return -1;
    }

    axRet = AX_TCM_SetHSyncAttr(devId, ptHsyncAttr);
    if (0 != axRet) {
        COMM_VIN_PRT("AX_VIN_SetHSyncAttr failed, nRet=0x%x.\n", axRet);
        return -1;
    }

    axRet = AX_TCM_EnableHSync(devId);
    if (0 != axRet) {
        COMM_VIN_PRT("AX_VIN_EnableHSync failed, nRet=0x%x.\n", axRet);
        return -1;
    }

    axRet = AX_TCM_SetFlashTimingAttr(devId, LIGHT_FLASH, ptSnapFlashAttr);
    if (0 != axRet) {
        COMM_VIN_PRT("AX_VIN_SetFlashTimingAttr failed, nRet=0x%x.\n", axRet);
        return -1;
    }
    sleep(2);

    axRet = AX_TCM_InterruptEn();
    if (0 != axRet) {
        COMM_VIN_PRT("AX_VIN_SetFlashTimingAttr failed, nRet=0x%x.\n", axRet);
        return -1;
    }

    sleep(2);

    if (pthread_create(&thread, NULL, test_trigger, NULL) != 0) {
        printf("Error creating thread.\n");
        return 1;
    }

    return 0;
}

AX_S32 COMMON_TCM_StopOutsideDev(AX_U8 devId)
{
    AX_S32 axRet;

    axRet = AX_TCM_DisableVSync(devId);
    if (0 != axRet) {
        COMM_VIN_PRT("AX_VIN_EnableVSync failed, nRet=0x%x.\n", axRet);
        return -1;
    }

    axRet = AX_TCM_DisableHSync(devId);
    if (0 != axRet) {
        COMM_VIN_PRT("AX_VIN_EnableHSync failed, nRet=0x%x.\n", axRet);
        return -1;
    }

    return 0;
}

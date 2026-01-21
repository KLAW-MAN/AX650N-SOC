/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef __COMMON_TCM_H__
#define __COMMON_TCM_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "ax_vin_api.h"
#include "ax_base_type.h"
#include "ax_mipi_rx_api.h"
#include "ax_isp_3a_plus.h"

#define AX_TCM_NODE "/dev/ax_ftc"
#define AX_TCM_IOC_MAGIC                   'f'
#define LIGHT_FLASH (2)
#define AX_ISP_FRAME_LENGTH_REG_NUM         3

typedef enum _FTC_IOC_ENUM {
    IOC_FTC_DEV_SYNC_POWERATTR_SET = 0xD0,
    IOC_FTC_DEV_SYNC_POWERATTR_GET,
    IOC_FTC_DEV_VSYNCATTR_SET,
    IOC_FTC_DEV_VSYNCATTR_GET,
    IOC_FTC_DEV_VSYNC_ENABLE_SET,
    IOC_FTC_DEV_HSYNCATTR_SET,
    IOC_FTC_DEV_HSYNCATTR_GET,
    IOC_FTC_DEV_HSYNC_ENABLE_SET,
    IOC_FTC_DEV_LIGHTSYNCINFO_SET,
    IOC_FTC_DEV_LIGHTSYNCINFO_GET,
    IOC_FTC_DEV_LIGHTSYNCATTR_SET,
    IOC_FTC_DEV_LIGHTSYNCATTR_GET,
    IOC_FTC_DEV_LIGHT_ENABLE_SET,
    IOC_FTC_DEV_FTC_INTERRUPT_EN,
    IOC_FTC_DEV_SENSOR_VTS_SET,
    IOC_FTC_DEV_FLASH_MAX = 0xDF,
} AX_FTC_IOC_ENUM_E;

#define TCM_CMD_DEV_SYNC_POWERATTR_SET      _IOWR(AX_TCM_IOC_MAGIC, IOC_FTC_DEV_SYNC_POWERATTR_SET, AX_VIN_POWER_SYNC_ATTR_T *)
#define TCM_CMD_DEV_SYNC_POWERATTR_GET      _IOWR(AX_TCM_IOC_MAGIC, IOC_FTC_DEV_SYNC_POWERATTR_GET, AX_VIN_POWER_SYNC_ATTR_T *)

#define TCM_CMD_DEV_VSYNCATTR_SET           _IOWR(AX_TCM_IOC_MAGIC, IOC_FTC_DEV_VSYNCATTR_SET, AX_VIN_SYNC_SIGNAL_ATTR_T *)
#define TCM_CMD_DEV_VSYNCATTR_GET           _IOWR(AX_TCM_IOC_MAGIC, IOC_FTC_DEV_VSYNCATTR_GET, AX_VIN_SYNC_SIGNAL_ATTR_T *)
#define TCM_CMD_DEV_VSYNC_ENABLE_SET        _IOWR(AX_TCM_IOC_MAGIC, IOC_FTC_DEV_VSYNC_ENABLE_SET, AX_BOOL *)

#define TCM_CMD_DEV_HSYNCATTR_SET           _IOWR(AX_TCM_IOC_MAGIC, IOC_FTC_DEV_HSYNCATTR_SET, AX_VIN_SYNC_SIGNAL_ATTR_T *)
#define TCM_CMD_DEV_HSYNCATTR_GET           _IOWR(AX_TCM_IOC_MAGIC, IOC_FTC_DEV_HSYNCATTR_GET, AX_VIN_SYNC_SIGNAL_ATTR_T *)
#define TCM_CMD_DEV_HSYNC_ENABLE_SET        _IOWR(AX_TCM_IOC_MAGIC, IOC_FTC_DEV_HSYNC_ENABLE_SET, AX_BOOL *)

#define TCM_CMD_DEV_LIGHTSYNCINFO_SET       _IOWR(AX_TCM_IOC_MAGIC, IOC_FTC_DEV_LIGHTSYNCINFO_SET, AX_VIN_LIGHT_SYNC_INFO_T *)
#define TCM_CMD_DEV_LIGHTSYNCINFO_GET       _IOWR(AX_TCM_IOC_MAGIC, IOC_FTC_DEV_LIGHTSYNCINFO_GET, AX_VIN_LIGHT_SYNC_INFO_T *)

#define TCM_CMD_DEV_LIGHTSYNCATTR_SET       _IOWR(AX_TCM_IOC_MAGIC, IOC_FTC_DEV_LIGHTSYNCATTR_SET, AX_LIGHT_SYNC_SIGNAL_T *)
#define TCM_CMD_DEV_LIGHTSYNCATTR_GET       _IOWR(AX_TCM_IOC_MAGIC, IOC_FTC_DEV_LIGHTSYNCATTR_GET, AX_LIGHT_SYNC_SIGNAL_T *)
#define TCM_CMD_DEV_LIGHT_ENABLE_SET        _IOWR(AX_TCM_IOC_MAGIC, IOC_FTC_DEV_LIGHT_ENABLE_SET, AX_LIGHT_SYNC_ENABLE_T *)
#define TCM_CMD_DEV_FTC_INTERRUPT_EN        _IOWR(AX_TCM_IOC_MAGIC, IOC_FTC_DEV_FTC_INTERRUPT_EN, AX_VOID *)
#define TCM_CMD_DEV_SENSOR_VTS_SET          _IOWR(AX_TCM_IOC_MAGIC, IOC_FTC_DEV_SENSOR_VTS_SET, AX_VIN_SENSOR_VTS_T *)

typedef enum _ax_sync_polariiy {
    VIN_SYNC_POLARITY_HIGH = 0,        /* the valid horizontal/vertical synchronization signal is high-level */
    VIN_SYNC_POLARITY_LOW,             /* the valid horizontal/vertical synchronization signal is low-level */
} ax_sync_polariiy_e;

typedef enum _ax_sync_light_type_e{
    VIN_SYNC_LIGHT_MODE_STROBE,
    VIN_SYNC_LIGHT_MODE_FLASH,
    VIN_SYNC_LIGHT_MODE_MAX,
} ax_sync_light_type_e;
typedef struct _AX_LIGHT_SYNC_SIGNAL_T {
    AX_U32                      nLightIdx;
    AX_U32                      bLightPinEnable;
    AX_U32                      bLightTimingEnable;
    ax_sync_light_type_e        eLightType;
    ax_sync_polariiy_e          eLightSyncInv;      /* the flash sync signal polarity */
    AX_U32                      nLightDutyTime;
    AX_S32                      nLightDelayTime;
    AX_U8                       nLightFreqRatio;
} AX_LIGHT_SYNC_SIGNAL_T;

typedef enum {
	IOC_VIN_SYNC_LIGHT_MODE_STROBE,
 	IOC_VIN_SYNC_LIGHT_MODE_FLASH,
	IOC_VIN_SYNC_LIGHT_MODE_MAX,
} IOC_VIN_SYNC_LIGHT_TYPE_E;

typedef enum _ax_sync_trigger_selection_e {
    VIN_SYNC_LIGHT_TRIGGER_INSERT,
    VIN_SYNC_LIGHT_TRIGGER_REPLACE_CURRENT,
    VIN_SYNC_LIGHT_TRIGGER_REPLACE_VIDEO,
    VIN_SYNC_LIGHT_TRIGGER_REPLACE_PICTURE,
} ax_sync_trigger_selection_e;

typedef struct _ax_flash_trigger_data {
    AX_U32                      nTriggerNum;
    AX_U32                      nIntervalFrmNum;    /* The number of frames between adjacent triggers for the same flashing light */
    ax_sync_trigger_selection_e eTriggerSelect;
    AX_U64                      nUserData;
    AX_U64                      nTriggerPts;
} ax_flash_trigger_data_t;

typedef struct _AX_LIGHT_SYNC_ENABLE_T {
    AX_U32                      nLightIdx;
    AX_BOOL                     bLightEnable;
    ax_sync_light_type_e        eLightType;
    ax_flash_trigger_data_t     tTriggerData;
} AX_LIGHT_SYNC_ENABLE_T;

typedef struct AX_VIN_SENSOR_VTS_T_ {
    AX_U32                          spi_bus;
    AX_U32                          spi_csn;
    AX_U8                           devid;
    AX_U32                          nRegValueMask[AX_ISP_FRAME_LENGTH_REG_NUM];                      /* Low 8bit: 0xff, high 8bit: 0xff00 */
    AX_U32                          nRegAddr[AX_ISP_FRAME_LENGTH_REG_NUM];
} AX_VIN_SENSOR_VTS_T;

AX_S32 AX_TCM_SetSensorVts(AX_U8 nDevId, const AX_VIN_SENSOR_VTS_T *ptSensorVts);
AX_S32 AX_TCM_SetSyncPowerAttr(AX_U8 nDevId, const AX_VIN_POWER_SYNC_ATTR_T *ptPowerAttr);
AX_S32 AX_TCM_GetSyncPowerAttr(AX_U8 nDevId, const AX_VIN_POWER_SYNC_ATTR_T *ptPowerAttr);
AX_S32 AX_TCM_SetVSyncAttr(AX_U8 nDevId, const AX_VIN_SYNC_SIGNAL_ATTR_T *ptVsyncAttr);
AX_S32 AX_TCM_GetVSyncAttr(AX_U8 nDevId, const AX_VIN_SYNC_SIGNAL_ATTR_T *ptVsyncAttr);
AX_S32 AX_TCM_EnableVSync(AX_U8 nDevId);
AX_S32 AX_TCM_DisableVSync(AX_U8 nDevId);
AX_S32 AX_TCM_SetHSyncAttr(AX_U8 nDevId, const AX_VIN_SYNC_SIGNAL_ATTR_T *ptVsyncAttr);
AX_S32 AX_TCM_GetHSyncAttr(AX_U8 nDevId, const AX_VIN_SYNC_SIGNAL_ATTR_T *ptVsyncAttr);
AX_S32 AX_TCM_EnableHSync(AX_U8 nDevId);
AX_S32 AX_TCM_DisableHSync(AX_U8 nDevId);
AX_S32 AX_TCM_SetLightSyncInfo(AX_U8 nDevId, const AX_VIN_LIGHT_SYNC_INFO_T *ptLightSyncInfo);
AX_S32 AX_TCM_GetLightSyncInfo(AX_U8 nDevId, AX_VIN_LIGHT_SYNC_INFO_T *ptLightSyncInfo);
AX_S32 AX_TCM_SetFlashTimingAttr(AX_U8 nDevId, AX_U8 nFlashIdx,
		const AX_VIN_FLASH_LIGHT_TIMING_ATTR_T *ptSnapFlashAttr);
AX_S32 AX_TCM_GetFlashTimingAttr(AX_U8 nDevId, AX_U8 nFlashIdx,
        AX_VIN_FLASH_LIGHT_TIMING_ATTR_T *ptSnapFlashAttr);
AX_S32 AX_TCM_TriggerFlash(AX_U8 nDevId, AX_U8 nFlashIdx, const AX_VIN_FLASH_TRIGGER_DATA_T *ptFlashData);
AX_S32 AX_TCM_InterruptEn(AX_VOID);
AX_S32 COMMON_TCM_StartOutsideDev(AX_U8 devId, AX_VIN_SENSOR_VTS_T *ptSensorVts, AX_VIN_POWER_SYNC_ATTR_T *ptSyncPowerAttr,
                                  AX_VIN_SYNC_SIGNAL_ATTR_T *ptVsyncAttr, AX_VIN_SYNC_SIGNAL_ATTR_T *ptHsyncAttr,
                                  AX_VIN_LIGHT_SYNC_INFO_T *ptLightSyncInfo, AX_VIN_STROBE_LIGHT_TIMING_ATTR_T *ptSnapStrobeAttr,
                                  AX_VIN_FLASH_LIGHT_TIMING_ATTR_T *ptSnapFlashAttr);
AX_S32 COMMON_TCM_StopOutsideDev(AX_U8 devId);

#ifdef __cplusplus
}
#endif

#endif

/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef __DEF_AX_FTC_EXTERN_H__
#define __DEF_AX_FTC_EXTERN_H__

#include "ax_global_type.h"

#include "osal_ax.h"
//#include "ax_frame.h"
#include "ax_vin_error_code.h"

#include "osal_lib_ax.h"
typedef AX_VOID *vin_node_handle;
typedef AX_S32(*pfn_node_notify_cb)(AX_U32 type, AX_VOID *puserdata, AX_VOID *pdata);
typedef AX_VOID *regio_global_handle_t;
typedef AX_VOID *regio_pipe_handle_t;

#define AX_ISP_SHUTTER_SEQ_NUM                  (60)
#define AX_VIN_LIGHT_FLASH_PIN_NUM              (10)
#define AX_VIN_LIGHT_GPIO_PIN_NUM               (9)
#define AX_ISP_SHUTTER_MAP_NUM                  (8)
#define AX_ISP_FRAME_LENGTH_REG_NUM             (3)

#define AX_VIN_SUCCESS (0)
#define AX_ISP_SUCCESS (0)

#define ISP_SIF_FLASH_REG_OFFSET                    (0X4200)

#define DEF_LIGHT_FLASH_PIN_NUM     (10)

#define AX_OFFSET(STR) ((unsigned int)(AX_ADDR) & (STR))
/* Common val check */
#define VIN_CHECK_PTR(h)                          \
    do {                                            \
        if (AX_NULL == (h)) {                       \
            printk("Error: Pointer is null!\n"); \
            return AX_ERR_VIN_NULL_PTR;           \
        }                                         \
    } while (0)

#define VIN_CHECK_VALUE_MAX(val, max)                              \
    do {                                            \
        if ((val) > (max)) {                                        \
            printk("Error: value:%d beyond the max:(%d)\n",       \
                    (val), (max));                                 \
            return AX_ERR_VIN_ILLEGAL_PARAM;                       \
        }                                                          \
    } while (0)

#define VIN_CHECK_HANDLE(h)                        \
    do {                                            \
        if (AX_NULL == (h)) {                        \
            printk("Error: Handle is invalid\n"); \
            return AX_ERR_VIN_NULL_PTR;            \
        }                                          \
    } while (0)

#define VIN_CHECK_HANDLE_WITH_STRING(h, string)          \
    do {                                            \
        if (AX_NULL == (h)) {                              \
            isp_err("Error: [%s] is null!\n", (string)); \
            return AX_ERR_VIN_NULL_PTR;                  \
        }                                                \
    } while (0)

#define AX_REG32_WRITE(addr, data) *((volatile AX_U32 *)(addr)) = data
#define AX_REG32_READ(addr) *((volatile AX_U32 *)(addr))

#define isp_err(str, arg...)                                                       \
                do {                                                                           \
                        ax_printk(AX_ID_ISP, "ISP", AX_KERN_ERR, "%s(%d)"str, __func__, __LINE__, ##arg);     \
                } while (0)

#define isp_warn(str, arg...)                                                      \
                do {                                                                           \
                        ax_printk(AX_ID_ISP, "ISP", AX_KERN_WARNING, "%s(%d)"str, __func__, __LINE__, ##arg);     \
                } while (0)

#define isp_info(str, arg...)                                                      \
                do {                                                                           \
                        ax_printk(AX_ID_ISP, "ISP", AX_KERN_INFO, "%s(%d)"str, __func__, __LINE__, ##arg);      \
                } while (0)

#define isp_dbg(str, arg...)                                                       \
                do {                                                                           \
                        ax_printk(AX_ID_ISP, "ISP", AX_KERN_DEBUG, "%s(%d)"str, __func__, __LINE__, ##arg);    \
                } while (0)

typedef enum _ax_vin_sync_shutter_mode_e{
    VIN_SYNC_SAMPLE_PIPE_MODE_INVALID,
    VIN_SYNC_SAMPLE_PIPE_MODE_VIDEO,
    VIN_SYNC_SAMPLE_PIPE_MODE_PICTURE,
    VIN_SYNC_SAMPLE_PIPE_MODE_FLASH_SNAP,      /* Snap of flash lamp */
    VIN_SYNC_SAMPLE_PIPE_MODE_USER_DEFINE0,
    VIN_SYNC_SAMPLE_PIPE_MODE_USER_DEFINE1,
    VIN_SYNC_SAMPLE_PIPE_MODE_USER_DEFINE2,
    VIN_SYNC_SAMPLE_PIPE_MODE_USER_DEFINE3,
    VIN_SYNC_SAMPLE_PIPE_MODE_USER_DEFINE4,
    VIN_SYNC_SAMPLE_PIPE_MODE_MAX,
} ax_vin_sync_shutter_mode_e;

typedef struct _ftc_regs {
    void __iomem *sen_reg ;
    void __iomem *reload_reg ;
    void __iomem *seq_reg ;
} ftc_regs;

typedef enum _ax_sync_trigger_mode {
    VIN_SYNC_OUTSIDE_ELEC_ADAPTIVE_TRIGGER,
    VIN_SYNC_OUTSIDE_ELEC_TRIGGER,
    VIN_SYNC_INSIDE_COUNTER_TRIGGER,
    VIN_SYNC_TRIGGER_MODE_MAX,
} ax_vin_sync_trigger_mode_e;

typedef struct _ax_vin_sns_register_info {
    AX_BOOL                         bEnable;
    AX_U32                          nRegValueMask[AX_ISP_FRAME_LENGTH_REG_NUM];                      /* Low 8bit: 0xff, high 8bit: 0xff00 */
    AX_U32                          nRegAddr[AX_ISP_FRAME_LENGTH_REG_NUM];
} ax_vin_sns_register_info_t;

typedef struct _ax_vin_light_shutter_mapping {
    ax_vin_sync_shutter_mode_e      eShutterMode;
    AX_U8                           nPipeId;
} ax_vin_light_shutter_mapping_t;

typedef enum _ax_sync_polariiy {
    VIN_SYNC_POLARITY_HIGH = 0,        /* the valid horizontal/vertical synchronization signal is high-level */
    VIN_SYNC_POLARITY_LOW,             /* the valid horizontal/vertical synchronization signal is low-level */
} ax_sync_polariiy_e;

typedef enum _ax_sync_light_type_e{
    VIN_SYNC_LIGHT_MODE_STROBE,
    VIN_SYNC_LIGHT_MODE_FLASH,
    VIN_SYNC_LIGHT_MODE_MAX,
} ax_sync_light_type_e;

typedef struct ax_sync_power_att {
    ax_vin_sync_trigger_mode_e  eTriggerMode;
    AX_U16                      nGpioElecInPin;
    AX_U16                      nGpioSyncOutPin;
    AX_U16                      nFollowCycle;
    AX_S16                      nFreqTolLeft;       /* frequency Tolerance range, Standard value - nFreqTolLeft */
    AX_S16                      nFreqTolRight;      /* frequency Tolerance range, Standard value + nFreqTolRight */
    AX_U16                      nElecFreq;          /* the outside electric signal frequency */
    AX_U16                      nSyncTriggerFreq;
    AX_U16                      nSyncDelayElcUs;
    AX_U32                      nStrobeGpioNum[AX_VIN_LIGHT_GPIO_PIN_NUM];
} ax_sync_power_attr_t;

typedef struct ax_sensor_vts {
    AX_U32                          spi_bus;
    AX_U32                          spi_csn;
    AX_U8                           devid;
    AX_U32                          nRegValueMask[AX_ISP_FRAME_LENGTH_REG_NUM];
    AX_U32                          nRegAddr[AX_ISP_FRAME_LENGTH_REG_NUM];
} ax_sensor_vts_t;

typedef struct _ax_sync_info {
    AX_U32                          nVts;                               /* unit: line */
    AX_U32                          nHts;                               /* unit: us */
    AX_U32                          nFrameRate;
    AX_U32                          nIntTime[AX_ISP_SHUTTER_MAP_NUM];   /* exposure time, unit: line */
    AX_U32                          nElecToVsyncTime;                   /* elec to uyncTime, unit: line*/
    AX_U32                          nVbbTime;                           /* Vertical back blanking time, unit: line */
    ax_vin_sns_register_info_t      tFrmLengthRegInfo;
    ax_vin_light_shutter_mapping_t  szShutterMap[AX_ISP_SHUTTER_MAP_NUM];
    ax_vin_sync_shutter_mode_e      szShutterMode[AX_ISP_SHUTTER_SEQ_NUM];  /* RW; shutter mode */
} ax_sync_info_t;

typedef struct _ax_sync_signal {
    AX_U32                      nSyncIdx;
    ax_sync_polariiy_e          eSyncInv;          /* the valid sync signal polarity */
    AX_U32                      nSyncFreq;         /* the sync signal frequency */
    AX_U32                      nSyncDutyRatio;    /* the sync signal duty ratio */
} ax_sync_signal_t;

typedef struct _ax_light_sync_signal {
    AX_U32                      nLightIdx;
    AX_U32                      bLightPinEnable;
    AX_U32                      bLightTimingEnable;
    ax_sync_light_type_e        eLightType;
    ax_sync_polariiy_e          eLightSyncInv;      /* the flash sync signal polarity */
    AX_U32                      nLightDutyTime;
    AX_S32                      nLightDelayTime;
    AX_U8                       nLightFreqRatio;
} ax_light_sync_signal_t;

typedef enum _ax_sync_trigger_selection_e {
    VIN_SYNC_LIGHT_TRIGGER_INSERT,
    VIN_SYNC_LIGHT_TRIGGER_REPLACE_CURRENT,
    VIN_SYNC_LIGHT_TRIGGER_REPLACE_VIDEO,
    VIN_SYNC_LIGHT_TRIGGER_REPLACE_PICTURE,
} ax_sync_trigger_selection_e;

typedef enum _ax_os_type {
    AX_OS_NODE_FTC                               = 0x42 << 8 | AX_ID_VIN,
} ax_os_format_e;

typedef enum _VIN_DEV_NODE_CMD {
    DEV_NODE_CMD_INVALID                = 0,
    DEV_NODE_CMD_SNS_INFO               = 1,
    DEV_NODE_CMD_FIND_EXP_INFO          = 10,
    DEV_NODE_CMD_SYNC_POWERATTR_SET     = 11,
    DEV_NODE_CMD_SYNC_POWERATTR_GET     = 12,
    DEV_NODE_CMD_VSYNCATTR_SET          = 13,
    DEV_NODE_CMD_VSYNCATTR_GET          = 14,
    DEV_NODE_CMD_VSYNC_ENABLE_SET       = 15,
    DEV_NODE_CMD_HSYNCATTR_SET          = 16,
    DEV_NODE_CMD_HSYNCATTR_GET          = 17,
    DEV_NODE_CMD_HSYNC_ENABLE_SET       = 18,
    DEV_NODE_CMD_LIGHTSYNCINFO_SET      = 19,
    DEV_NODE_CMD_LIGHTSYNCINFO_GET      = 20,
    DEV_NODE_CMD_LIGHTSYNCATTR_SET      = 21,
    DEV_NODE_CMD_LIGHTSYNCATTR_GET      = 22,
    DEV_NODE_CMD_LIGHT_ENABLE_SET       = 23,
    DEV_NODE_CMD_LIGHT_GROUP_ENABLE_SET = 24,
    DEV_NODE_CMD_SYNC_EXP_PIPEID_GET    = 25,
    DEV_NODE_CMD_SYNC_SNS_INFO_SET      = 26,
    DEV_NODE_CMD_SYNC_FRAME_INFO_SET    = 27,
    DEV_NODE_CMD_FREEZE_COMMBUS         = 28,
    DEV_NODE_CMD_REGISTER_BUS_CALLBACK  = 29,
    DEV_NODE_CMD_SENSOR_VTS_SET         = 30,
    DEV_NODE_CMD_MAX,
} VIN_DEV_NODE_CMD_E;

typedef struct {
    AX_BOOL                         b_freeze_commbus_en;
    AX_S32                          timeout_ms;
} vin_dev_freeze_commbus_t;

typedef struct _vin_ftc_status_note {
    AX_BOOL         enable;
    AX_BOOL         is_updating;
    AX_U64          dbg_pts0;
    AX_U64          dbg_pts1;
    AX_U64          dbg_pts2;
    AX_U64          dbg_pts3;
    AX_U64          seq_num;
    AX_U64          trigger_times;
    AX_U32          event_pace;
    AX_U32          event_pace_max;
    AX_U32          interval_frm_num;
} vin_ftc_status_note_t;

typedef struct _vin_ftc_status_param {
    vin_ftc_status_note_t           event_note[AX_VIN_LIGHT_FLASH_PIN_NUM];
} vin_ftc_status_param_t;

typedef struct _ISP_META_FTC_EVENT {
    AX_U32                      nFlashId;
    AX_U64                      nUserData;
    AX_U64                      nTriggerPts;
} ISP_META_FTC_EVENT_T;

typedef enum _ISP_META_FRAME_TYPE {
    ISP_META_FRAME_TYPE_VIDEO             = 0,
    ISP_META_FRAME_TYPE_PICTURE           = 1,
    ISP_META_FRAME_TYPE_FLASH_SNAP        = 2,
    ISP_META_FRAME_TYPE_USER_DEFINED0     = 3,
    ISP_META_FRAME_TYPE_USER_DEFINED1     = 4,
    ISP_META_FRAME_TYPE_USER_DEFINED2     = 5,
    ISP_META_FRAME_TYPE_USER_DEFINED3     = 6,
    ISP_META_FRAME_TYPE_USER_DEFINED4     = 7,
    ISP_META_FRAME_TYPE_MAX,
} ISP_META_FRAME_TYPE_E;

typedef struct _ISP_META_FTC {
    ISP_META_FRAME_TYPE_E       eFrmType;
    AX_U32                      nFlashActiveNum;                /* Number of flashing lights that remain on */
    ISP_META_FTC_EVENT_T        tFtcEventInfo[DEF_LIGHT_FLASH_PIN_NUM];  /* External device (such as flash) information */
} ISP_META_FTC_T;

typedef struct _vin_dev_ftc_event_info {
    AX_U8                       pipe_id;
    ISP_META_FTC_T              meta_ftc_info;
} vin_dev_ftc_event_info_t;


typedef struct _ax_flash_trigger_data {
    AX_U32                      nTriggerNum;
    AX_U32                      nIntervalFrmNum;    /* The number of frames between adjacent triggers for the same flashing light */
    ax_sync_trigger_selection_e eTriggerSelect;
    AX_U64                      nUserData;
    AX_U64                      nTriggerPts;
} ax_flash_trigger_data_t;

typedef struct _ax_light_sync_enable {
    AX_U32                      nLightIdx;
    AX_BOOL                     bLightEnable;
    ax_sync_light_type_e        eLightType;
    ax_flash_trigger_data_t     tTriggerData;
} ax_light_sync_enable_t;

typedef enum isp_reload_domain {
    ISP_DOMAIN_ROSC_RST = 0,
    ISP_DOMAIN_ITS_RELOAD,
    ISP_DOMAIN_MAX
} isp_reload_domain_e;

typedef struct _sw_reload_0_cfg {
    AX_U8 reservedF[4];
    AX_U8 isp_its_sw_reload;
    AX_U8 reservedB[19];
} sw_reload_0_cfg_s;

typedef union _SW_RELOAD_0_SET {
    struct  {
        AX_U32 reservedF            : 4;    /* [3:0]*/
        AX_U32 isp_its_sw_reload_set_set         : 1;    /* [4]*/
        AX_U32 reservedB                       : 27;    /* [31:5]*/
    } bits;
    AX_U32 reg32;
} SW_RELOAD_0_SET_U;

typedef union _SW_RELOAD_0_CLR {
    struct  {
        AX_U32 reservedF            : 4;    /* [3:0]*/
        AX_U32 isp_its_sw_reload_clr_clr         : 1;    /* [4]*/
        AX_U32 reservedB                       : 27;    /* [31:5]*/
    } bits;
    AX_U32 reg32;
} SW_RELOAD_0_CLR_U;

AX_S32 ISP_DRV_IO_WRITE32(regio_global_handle_t reg_handle, AX_U32 offset, AX_U32 data);
AX_U32 ISP_DRV_IO_READ32(regio_global_handle_t reg_handle, AX_U32 offset);
AX_U32 ISP_DRV_SEQ_READ32(regio_global_handle_t reg_handle, AX_U32 offset);

AX_U32 ISP_DRV_IO_RELOAD_READ32(regio_global_handle_t reg_handle, AX_U32 offset);
AX_S32 ISP_DRV_IO_RELOAD_WRITE32(regio_global_handle_t reg_handle, AX_U32 offset, AX_U32 data);

#endif

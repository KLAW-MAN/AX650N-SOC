/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef __DEF_AX_FTC_PRIV_H__
#define __DEF_AX_FTC_PRIV_H__

//#include "ax_base_type.h"
#include "ax_ftc_extern.h"
#include "osal_dev_ax.h"
#include "ax_global_type.h"



#define ELC_MAX_HOP (16)
#define RB_RWOS_OFFSET (4)
#define ACTIVE_RWOS (2500)
#define AX_FTC_DRIVER_NAME                     "ax_ftc"

#define AX_FTC_IOC_MAGIC                   'f'
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

#define TCM_CMD_DEV_SYNC_POWERATTR_SET      _IOWR(AX_FTC_IOC_MAGIC, IOC_FTC_DEV_SYNC_POWERATTR_SET, ax_sync_power_attr_t *)
#define TCM_CMD_DEV_SYNC_POWERATTR_GET      _IOWR(AX_FTC_IOC_MAGIC, IOC_FTC_DEV_SYNC_POWERATTR_GET, ax_sync_power_attr_t *)
#define TCM_CMD_DEV_VSYNCATTR_SET           _IOWR(AX_FTC_IOC_MAGIC, IOC_FTC_DEV_VSYNCATTR_SET, ax_sync_signal_t *)
#define TCM_CMD_DEV_VSYNCATTR_GET           _IOWR(AX_FTC_IOC_MAGIC, IOC_FTC_DEV_VSYNCATTR_GET, ax_sync_signal_t *)
#define TCM_CMD_DEV_VSYNC_ENABLE_SET        _IOWR(AX_FTC_IOC_MAGIC, IOC_FTC_DEV_VSYNC_ENABLE_SET, AX_BOOL *)
#define TCM_CMD_DEV_HSYNCATTR_SET           _IOWR(AX_FTC_IOC_MAGIC, IOC_FTC_DEV_HSYNCATTR_SET, ax_sync_signal_t *)
#define TCM_CMD_DEV_HSYNCATTR_GET           _IOWR(AX_FTC_IOC_MAGIC, IOC_FTC_DEV_HSYNCATTR_GET, ax_sync_signal_t *)
#define TCM_CMD_DEV_HSYNC_ENABLE_SET        _IOWR(AX_FTC_IOC_MAGIC, IOC_FTC_DEV_HSYNC_ENABLE_SET, AX_BOOL *)
#define TCM_CMD_DEV_LIGHTSYNCINFO_SET       _IOWR(AX_FTC_IOC_MAGIC, IOC_FTC_DEV_LIGHTSYNCINFO_SET, ax_sync_info_t *)
#define TCM_CMD_DEV_LIGHTSYNCINFO_GET       _IOWR(AX_FTC_IOC_MAGIC, IOC_FTC_DEV_LIGHTSYNCINFO_GET, ax_sync_info_t *)
#define TCM_CMD_DEV_LIGHTSYNCATTR_SET       _IOWR(AX_FTC_IOC_MAGIC, IOC_FTC_DEV_LIGHTSYNCATTR_SET, ax_light_sync_signal_t *)
#define TCM_CMD_DEV_LIGHTSYNCATTR_GET       _IOWR(AX_FTC_IOC_MAGIC, IOC_FTC_DEV_LIGHTSYNCATTR_GET, ax_light_sync_signal_t *)
#define TCM_CMD_DEV_LIGHT_ENABLE_SET        _IOWR(AX_FTC_IOC_MAGIC, IOC_FTC_DEV_LIGHT_ENABLE_SET, ax_light_sync_enable_t *)
#define TCM_CMD_DEV_FTC_INTERRUPT_EN        _IOWR(AX_FTC_IOC_MAGIC, IOC_FTC_DEV_FTC_INTERRUPT_EN, AX_VOID *)
#define TCM_CMD_DEV_SENSOR_VTS_SET          _IOWR(AX_FTC_IOC_MAGIC, IOC_FTC_DEV_SENSOR_VTS_SET, ax_sensor_vts_t *)

typedef enum {
    USE_CLOCK_CYCLE_AS_COUNTER = 0,
    USE_LINE_COUNTER_AS_COUNTER = 1,
} sensor_counter_unit_e;

typedef enum vin_ftc_trigger_event_fsm {
    FTC_TRIGGER_EVENT_FSM_IDLE              = 0,
    FTC_TRIGGER_EVENT_FSM_UPDATE            = 1,    /* IOCTL config params */
    FTC_TRIGGER_EVENT_FSM_SNAP_ONCE         = 2,
    FTC_TRIGGER_EVENT_FSM_SNAP_ONGOING      = 3,    /* During the continuous snap process */
} vin_ftc_trigger_event_fsm_t;

typedef enum _vin_ftc_flash_pin_func {
    FTC_PIN_FUNC_0                  = 0,
    FTC_PIN_FUNC_1                  = 1,
    FTC_PIN_FUNC_2                  = 2,
    FTC_PIN_FUNC_4                  = 4,
    FTC_PIN_FUNC_6                  = 6,
} vin_ftc_phy_rx_pin_func_e;

typedef enum _vin_ftc_phy_rx_pin_direction {
    FTC_PIN_DIRECTION_OUT           = 0,
    FTC_PIN_DIRECTION_IN            = 1,
} vin_ftc_phy_rx_pin_direction_e;

typedef struct _vin_ftc_phy_rx_pin_func {
    vin_ftc_phy_rx_pin_func_e       e_elec_pls_func;
    vin_ftc_phy_rx_pin_func_e       e_vsync_func;
    vin_ftc_phy_rx_pin_func_e       e_hsync_func;
    vin_ftc_phy_rx_pin_func_e       e_flash_func[AX_VIN_LIGHT_FLASH_PIN_NUM];
    vin_ftc_phy_rx_pin_func_e       e_strobe_func[AX_VIN_LIGHT_GPIO_PIN_NUM];
} vin_ftc_phy_rx_pin_func_t;

typedef struct _vin_ftc_node_info {
    AX_BOOL                         b_flash_ctrl_en;
    AX_U32                          n_cur_shutter_seq_num;
    ax_vin_sync_shutter_mode_e      sz_cur_shutter_mode[AX_ISP_SHUTTER_SEQ_NUM];
    ax_sync_power_attr_t            t_power_attr;
    ax_sync_info_t                  t_sync_info;
    ax_sync_signal_t                t_vsync_attr;
    ax_sync_signal_t                t_hsync_attr;
    ax_light_sync_signal_t          t_light_flash_sync_attr[AX_VIN_LIGHT_FLASH_PIN_NUM];
    ax_light_sync_signal_t          t_light_gpio_sync_attr[AX_VIN_LIGHT_GPIO_PIN_NUM];
    ax_sensor_vts_t                 t_sensor_vts;
} vin_ftc_node_info_t;

typedef struct _vin_ftc_flash_trigger_event {
    vin_ftc_trigger_event_fsm_t     trigger_fsm;
    /* User parameters */
    AX_U8                           flash_id;
    AX_U32                          trigger_num;
    AX_U32                          interval_frm_num;
    ax_sync_trigger_selection_e     trigger_select;
    AX_U64                          user_data;

    /* driver internal parameters */
    AX_BOOL                         trigger_en;
    AX_U32                          trigger_done_times;
    AX_U64                          trigger_queue[10];  /* value: seqnum that requires the trigger */
} vin_ftc_flash_trigger_event_t;

typedef struct _vin_ftc_node_priv_ctx {
    AX_DEV_T                *ftc_dev;
    AX_SPINLOCK_T                   ftc_cfg_slock;
    AX_U8                           n_dev_id;
    vin_ftc_phy_rx_pin_func_t       t_phy_rx_pin_func;
    vin_ftc_node_info_t             ftc_info;
    //ax_vin_sync_shutter_mode_e      shutter_mode;
    //ISP_SNS_REGS_CFG_TABLE_T        tSnsRegsInfo;
    vin_ftc_flash_trigger_event_t   trigger_event[AX_VIN_LIGHT_FLASH_PIN_NUM];
    AX_BOOL                         b_freeze_commbus_en;
    //vin_dev_bus_callback_t          t_bus_callback;

    /* status parameters */
    //vin_ftc_status_param_t          ftc_status;
} vin_ftc_node_priv_ctx_t;

#endif //__DEF_NODE_FTC_PRIV_H__

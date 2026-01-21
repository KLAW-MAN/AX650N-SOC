/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef __AX_SYNC_SIGNAL_H__
#define __AX_SYNC_SIGNAL_H__

#include "osal_ax.h"

#define FTC_CHIP_NAME    "AX650"

#define FTC_SDK_VER_PRIX "_FTC_V"

#define FTC_VERSION_MAJOR 1
#define FTC_VERSION_MINOR 1
#define FTC_VERSION_MINOR2 1

#define __MAKE_VERSION(a, x, y) #a"."#x"."#y
#define MAKE_VERSION(a, x, y)     __MAKE_VERSION(a, x, y)
#define FTC_VERSION  FTC_CHIP_NAME FTC_SDK_VER_PRIX MAKE_VERSION(FTC_VERSION_MAJOR, FTC_VERSION_MINOR, FTC_VERSION_MINOR2)

typedef enum {
    ax_ftc_blink_idx_0 = 0,
    ax_ftc_blink_idx_1,
    ax_ftc_blink_idx_2,
    ax_ftc_blink_idx_3,
    ax_ftc_blink_idx_4,
    ax_ftc_blink_idx_5,
    ax_ftc_blink_idx_6,
    ax_ftc_blink_idx_7,
    ax_ftc_blink_idx_8,
    ax_ftc_blink_idx_max,
} ax_ftc_blink_idx_e;

/* pin capability information */
typedef struct {
    uint8_t     cmd;
    uint8_t     feedback;
    uint8_t     gpio_elec_in;
    uint8_t     gpio_sync_out;
    uint8_t     gpio_num_blink[ax_ftc_blink_idx_max];
    uint8_t     gpio_num_pack;
    uint8_t     reserved[18];
} ax_ftc_pin_ability_msg_t; // mailbox message length is 32 bytes

/* power management parameters */
typedef struct {
    uint8_t     cmd;
    uint8_t     feedback;

    uint8_t     power_trigger_mode;     /* ax_ftc_power_sync_mode_e */
    uint8_t     follow_cycle;

    uint8_t     tol_left_cyc;           /* frequency Tolerance range, Standard value - nFreqTolLeft */
    uint8_t     tol_right_cyc;          /* frequency Tolerance range, Standard value + nFreqTolRight */

    uint16_t    elec_freq;              /* input frequency */

    uint16_t    sync_trigger_freq;      /* onput frequency */

    uint16_t    elec_phase;             /* power phase, unit: us */

    uint8_t     reserved[20];
} ax_ftc_power_sync_attr_msg_t; // mailbox message length is 32 bytes

/* strobe & flash timing attr */
typedef struct {
    uint8_t     cmd;
    uint8_t     feedback;

    uint8_t     blink_idx;              /* ax_ftc_blink_idx_e */
    uint8_t     blink_enable;           /* 1: enable; 0: disable */

    uint8_t     blink_polarity;         /* ax_ftc_blink_polarity_e */
    uint16_t    blink_duty;             /* duty cycle modulation, unit: permillage 0‰ ~ 1000‰ */

    uint8_t     blink_freq_div;         /* division factor, 2: 2x div; 0x82: 1/2 div */
    uint16_t    blink_delay_duty;       /* phase modulation, unit: permillage 0‰ ~ 1000‰ */

    uint16_t    blink_duty_us;          /* duty time, unit: us*/
    uint16_t    blink_delay_us;         /* phase modulation, unit: us */

    uint8_t     blink_cmd_enable;       /* 1: enable; 0: disable */
    uint8_t     blink_cmd_type;         /* cmd type*/
    uint8_t     blink_cmd_bit_width;
    uint16_t    blink_cmd_time_width;
    uint16_t    blink_cmd_data;         /* cmd data*/

    uint8_t     reserved[11];
} ax_ftc_blink_timing_attr_msg_t; // mailbox message length is 32 bytes

/* pack timing attr */
typedef struct {
    uint8_t     cmd;
    uint8_t     feedback;

    uint8_t     pack_enable;           /* 1: enable; 0: disable */

    uint8_t     pack_polarity;         /* ax_ftc_blink_polarity_e */
    uint16_t    pack_duty;             /* duty cycle modulation, unit: permillage 0‰ ~ 1000‰ */

    uint8_t     pack_freq_div;         /* division factor, 2: 2x div; 0x82: 1/2 div */
    uint16_t    pack_delay_duty;       /* phase modulation, unit: permillage 0‰ ~ 1000‰ */

    uint16_t    pack_delay_us;         /* phase modulation, unit: us */

    uint8_t     reserved[21];
} ax_ftc_pack_timing_attr_msg_t; // mailbox message length is 32 bytes

/* snap event info */
typedef struct {
    uint8_t     cmd;
    uint8_t     feedback;
    uint8_t     snap_idx;
    uint8_t     snap_enable;
    uint8_t     trigger_num;
    uint8_t     reserved[3];

    uint64_t    user_data;

    uint8_t     reserved_1[16];
} ax_ftc_snap_event_info_msg_t; // mailbox message length is 32 bytes

typedef struct {
    uint8_t     cmd;
    uint8_t     feedback;
    uint8_t     debug_info[30];
} ax_ftc_debug_info_msg_t; // mailbox message length is 32 bytes

typedef struct {
    uint8_t     cmd;
    uint8_t     feedback;
    uint8_t     version[30];
} ax_ftc_version_info_msg_t; // mailbox message length is 32 bytes

typedef struct {
    uint8_t     cmd;
    uint8_t     feedback;
    uint8_t     reserved[30];
} ax_ftc_cmd_msg_t; // mailbox message length is 32 bytes

int AX_RISCV_Kernel_setup_riscv(void);

int AX_RISCV_Kernel_shutup_riscv(void);

int AX_RISCV_Kernel_get_cycle_info(uint32_t *cycle_us);

int AX_RISCV_Kernel_send_version_info(void);

int AX_RISCV_Kernel_init(void);

int AX_RISCV_Kernel_deinit(void);

int AX_RISCV_Kernel_start(void);

int AX_RISCV_Kernel_stop(void);

int AX_RISCV_Kernel_set_power_attr(ax_ftc_power_sync_attr_msg_t *power_attr);

int AX_RISCV_Kernel_get_power_attr(ax_ftc_power_sync_attr_msg_t *power_attr);

int AX_RISCV_Kernel_set_pin_info(ax_ftc_pin_ability_msg_t *pin_cfg);

int AX_RISCV_Kernel_get_pin_info(ax_ftc_pin_ability_msg_t *pin_cfg);

int AX_RISCV_Kernel_set_blink_timing_attr(ax_ftc_blink_timing_attr_msg_t *blink_attr);

int AX_RISCV_Kernel_get_blink_timing_attr(ax_ftc_blink_timing_attr_msg_t *blink_attr);

int AX_RISCV_Kernel_set_flash_timing_attr(ax_ftc_blink_timing_attr_msg_t *blink_attr);

int AX_RISCV_Kernel_set_pack_timing_attr(ax_ftc_pack_timing_attr_msg_t *blink_attr);

int AX_RISCV_Kernel_get_pack_timing_attr(ax_ftc_pack_timing_attr_msg_t *blink_attr);

#endif //__AX_SYNC_SIGNAL_H__

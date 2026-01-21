/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef __AX_SYNC_SIGNAL_PRIV_H__
#define __AX_SYNC_SIGNAL_PRIV_H__

#include "osal_ax.h"

#define AX_FTC_CMD_INIT                     0x00
#define AX_FTC_CMD_DEINIT                   0x01

#define AX_FTC_CMD_SET_PIN_INFO             0x02
#define AX_FTC_CMD_GET_PIN_INFO             0x03

#define AX_FTC_CMD_SET_POWER_ATTR           0x04
#define AX_FTC_CMD_GET_POWER_ATTR           0x05

#define AX_FTC_CMD_SET_BLINK_TIMING_ATTR    0x06
#define AX_FTC_CMD_GET_BLINK_TIMING_ATTR    0x07

#define AX_FTC_CMD_START                    0x08
#define AX_FTC_CMD_STOP                     0x09

#define AX_FTC_CMD_VERSION_INFO             0x0a

#define AX_FTC_CMD_SET_PACK_TIMING_ATTR     0x0b
#define AX_FTC_CMD_GET_PACK_TIMING_ATTR     0x0c

#define AX_FTC_CMD_SET_FLASH_TIMING_ATTR    0x0d

#define AX_MBOX_CMD_FLUSH_CACHE             0x0e
#define AX_MBOX_CMD_FB                      0xa0

#define AX_FTC_FB_SUCCESS                   0x00
#define AX_FTC_FB_INTL_2_EXT                0x01
#define AX_FTC_FB_EXT_2_INTL                0x02
#define AX_FTC_FB_CMD_FAIL                  0xa1
#define AX_FTC_FB_STAT_ERR                  0xa2

#endif //__AX_SYNC_SIGNAL_PRIV_H__

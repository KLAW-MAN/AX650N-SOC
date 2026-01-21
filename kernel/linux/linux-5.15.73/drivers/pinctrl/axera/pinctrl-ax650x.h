/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef __PINCTRL_AX650X_H
#define __PINCTRL_AX650X_H

#define AX_PULL_DOWN (1 << 6)
#define AX_PULL_DOWN_BIT 6
#define AX_PULL_UP (1 << 7)
#define AX_PULL_UP_BIT 7
#define AX_DRIVE_STRENGTH  0xf
#define AX_SCHMITT_ENABLE  4

#define FUNCTION_SELECT 16
#define FUNCTION_SELECT_BIT_CLEAR (0X7 << 16)
#define FUNCTION_MAX 1024
#endif /* __PINCTRL_AX650X_H */
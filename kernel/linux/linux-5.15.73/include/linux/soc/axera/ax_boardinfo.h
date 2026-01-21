/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef __AX_BOARDINFO_H
#define __AX_BOARDINFO_H

typedef enum {
	NONE_CHIP_TYPE = 0x0,
	AX650A_CHIP = 0x1,
	AX650N_CHIP = 0x2,
	AX650C_CHIP = 0x3,
	AX750_CHIP = 0x4,
	AX650_CHIP_MAX = 0x5,
} AX_CHIP_TYPE_E;

#endif /* __AX_BOARDINFO_H */
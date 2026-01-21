/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#pragma once
#include "AXException.hpp"
#include "ax_global_type.h"

#ifndef CONST
#define CONST const
#endif

#ifndef CONSTEXPR
#define CONSTEXPR constexpr
#endif

#define INFINITE (-1)

/* val in [low, upp] */
#define IN_RANGE(low, val, upp) ((val) >= (low) && (val) <= (upp))

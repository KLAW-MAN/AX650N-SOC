/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/printk.h>
#include <linux/string.h>
#include <linux/version.h>
#include <linux/random.h>

#include "osal_lib_ax.h"

AX_U64 AX_OSAL_LIB_div_u64(AX_U64 dividend, AX_U32 divisor)
{
	return div_u64(dividend, divisor);
}

EXPORT_SYMBOL(AX_OSAL_LIB_div_u64);

AX_S64 AX_OSAL_LIB_div_s64(AX_S64 dividend, AX_S32 divisor)
{
	return div_s64(dividend, divisor);
}

EXPORT_SYMBOL(AX_OSAL_LIB_div_s64);

AX_U64 AX_OSAL_LIB_div64_u64(AX_U64 dividend, AX_U64 divisor)
{
	return div64_u64(dividend, divisor);
}

EXPORT_SYMBOL(AX_OSAL_LIB_div64_u64);

AX_S64 AX_OSAL_LIB_LIB_div64_s64(AX_S64 dividend, AX_S64 divisor)
{
	return div64_s64(dividend, divisor);
}

EXPORT_SYMBOL(AX_OSAL_LIB_LIB_div64_s64);

AX_U64 AX_OSAL_LIB_div_u64_rem(AX_U64 dividend, AX_U32 divisor)
{
	unsigned int remainder;

	div_u64_rem(dividend, divisor, &remainder);

	return remainder;
}

EXPORT_SYMBOL(AX_OSAL_LIB_div_u64_rem);

AX_S64 AX_OSAL_LIB_div_s64_rem(AX_S64 dividend, AX_S32 divisor)
{
	int remainder;

	div_s64_rem(dividend, divisor, &remainder);

	return remainder;
}

EXPORT_SYMBOL(AX_OSAL_LIB_div_s64_rem);

AX_U64 AX_OSAL_LIB_div64_u64_rem(AX_U64 dividend, AX_U64 divisor)
{
	unsigned long long remainder;

	div64_u64_rem(dividend, divisor, &remainder);

	return remainder;
}

EXPORT_SYMBOL(AX_OSAL_LIB_div64_u64_rem);

AX_U32 AX_OSAL_LIB_random()
{
	return get_random_int();
}

EXPORT_SYMBOL(AX_OSAL_LIB_random);

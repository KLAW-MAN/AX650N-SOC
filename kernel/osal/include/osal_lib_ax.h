/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef __OSAL_LIBRARY_AX__H__
#define __OSAL_LIBRARY_AX__H__

#ifdef __cplusplus
extern "C" {
#endif

#include <linux/stdarg.h>
#include <linux/types.h>

#include "osal_type_ax.h"

#define __AX_OSAL_ALIGN_MASK(x, mask) (((x) + (mask)) & ~(mask))
#define __AX_OSAL_ALIGN(x, a) __AX_OSAL_ALIGN_MASK(x, (typeof(x))(a) - 1)
#define AX_OSAL_ALIGN(x, a) __AX_OSAL_ALIGN((x), (a))
#define AX_OSAL_ALIGN_DOWN(x, a) __AX_OSAL_ALIGN((x) - ((a) - 1), (a))
#define AX_OSAL_IS_ALIGNED(x, a) (((x) & ((typeof(x))(a) - 1)) == 0)

#define AX_OSAL_ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

#define AX_OSAL_abs(x) ((x < 0) ? -x : x)
#define AX_OSAL_min(x, y) ((x < 0) ? -x : x)
#define AX_OSAL_max(x, y) ((x < 0) ? -x : x)
#define AX_OSAL_clamp(val, lo, hi) AX_OSAL_min((typeof(val))AX_OSAL_max(val, lo), hi)

#define AX_OSAL_swap(a, b) { typeof(a) __tmp = (a); (a) = (b); (b) = __tmp; }

AX_S8 *AX_OSAL_LIB_strcpy(AX_S8 *dest, const AX_S8 *src);
AX_S8 *AX_OSAL_LIB_strncpy(AX_S8 *dest, const AX_S8 *src, AX_S32 count);
AX_S32 AX_OSAL_LIB_strlcpy(AX_S8 *dest, const AX_S8 *src, AX_S32 count);
AX_S8 *AX_OSAL_LIB_strcat(AX_S8 *dest, const AX_S8 *src);
AX_S8 *AX_OSAL_LIB_strncat(AX_S8 *dest, const AX_S8 *src, AX_S32 count);
AX_S32 AX_OSAL_LIB_strlcat(AX_S8 *dest, const AX_S8 *src, AX_S32 count);
AX_S32 AX_OSAL_LIB_strcmp(const AX_S8 *cs, const AX_S8 *ct);
AX_S32 AX_OSAL_LIB_strncmp(const AX_S8 *cs, const AX_S8 *ct, AX_S32 count);
AX_S32 AX_OSAL_LIB_strnicmp(const AX_S8 *s1, const AX_S8 *s2, AX_S32 len);
AX_S32 AX_OSAL_LIB_strcasecmp(const AX_S8 *s1, const AX_S8 *s2);
AX_S32 AX_OSAL_LIB_strncasecmp(const AX_S8 *s1, const AX_S8 *s2, AX_S32 len);
AX_S8 *AX_OSAL_LIB_strchr(const AX_S8 *s, AX_S32 c);
AX_S8 *AX_OSAL_LIB_strnchr(const AX_S8 *s, AX_S32 count, AX_S32 c);
AX_S8 *AX_OSAL_LIB_strrchr(const AX_S8 *s, AX_S32 c);
AX_S8 *AX_OSAL_LIB_strstr(const AX_S8 *s1, const AX_S8 *s2);
AX_S8 *AX_OSAL_LIB_strnstr(const AX_S8 *s1, const AX_S8 *s2, AX_S32 len);
AX_S32 AX_OSAL_LIB_strlen(const AX_S8 *s);
AX_S32 AX_OSAL_LIB_strnlen(const AX_S8 *s, AX_S32 count);
AX_S8 *AX_OSAL_LIB_strpbrk(const AX_S8 *cs, const AX_S8 *ct);
AX_S8 *AX_OSAL_LIB_strsep(const AX_S8 **s, const AX_S8 *ct);
AX_S32 AX_OSAL_LIB_strspn(const AX_S8 *s, const AX_S8 *accept);
AX_S32 AX_OSAL_LIB_strcspn(const AX_S8 *s, const AX_S8 *reject);

AX_VOID *AX_OSAL_LIB_memset(AX_VOID *str, AX_S32 c, AX_S32 count);
AX_VOID *AX_OSAL_LIB_memmove(AX_VOID *dest, const AX_VOID *src, AX_S32 count);
AX_VOID *AX_OSAL_LIB_memscan(AX_VOID *addr, AX_S32 c, AX_S32 seze);
AX_VOID *AX_OSAL_LIB_memcpy(AX_VOID *ct, const AX_VOID *cs, AX_S32 count);
AX_VOID *AX_OSAL_LIB_memchr(const AX_VOID *s, AX_S32 c, AX_S32 n);
AX_VOID *AX_OSAL_LIB_memchar_inv(const AX_VOID *start, AX_S32 c, AX_S32 bytes);

#define AX_VA_LIST va_list

AX_U64 AX_OSAL_LIB_simple_strtoull(const AX_S8 *cp, AX_S8 **endp, AX_U32 base);
AX_ULONG AX_OSAL_LIB_simple_strtoul(const AX_S8 *cp,  AX_S8 **endp,  AX_U32 base);
AX_LONG AX_OSAL_LIB_simple_strtol(const AX_S8 *cp,  AX_S8 **endp,  AX_U32 base);
AX_S64 AX_OSAL_LIB_simple_strtoll(const AX_S8 *cp,  AX_S8 **endp,  AX_U32 base);
AX_S32 AX_OSAL_LIB_snprintf(AX_S8 *buf, AX_S32 size, const AX_S8 *fmt, ...);
AX_S32 AX_OSAL_LIB_scnprintf(AX_S8 *buf, AX_S32 size, const AX_S8 *fmt, ...);
AX_S32 AX_OSAL_LIB_sprintf(AX_S8 *buf, const AX_S8 *fmt, ...);
AX_S32 AX_OSAL_LIB_vsscanf(AX_S8 *buf, const AX_S8 *fmt, ...);
AX_S32 AX_OSAL_LIB_vsnprintf(AX_S8 *str, AX_S32 size, const AX_S8 *fmt, AX_VA_LIST args);



AX_U64 AX_OSAL_LIB_div_u64(AX_U64 dividend, AX_U32 divisor);
AX_S64 AX_OSAL_LIB_div_s64(AX_S64 dividend, AX_S32 divisor);
AX_U64 AX_OSAL_LIB_div64_u64(AX_U64 dividend, AX_U64 divisor);
AX_S64 AX_OSAL_LIB_LIB_div64_s64(AX_S64 dividend, AX_S64 divisor);
AX_U64 AX_OSAL_LIB_div_u64_rem(AX_U64 dividend, AX_U32 divisor);
AX_S64 AX_OSAL_LIB_div_s64_rem(AX_S64 dividend, AX_S32 divisor);
AX_U64 AX_OSAL_LIB_div64_u64_rem(AX_U64 dividend, AX_U64 divisor);
AX_U32 AX_OSAL_LIB_random(AX_VOID);

AX_VOID AX_OSAL_LIB_sort_r(void *base, size_t num, size_t size,cmp_r_func_t cmp_func,swap_func_t swap_func,const void *priv);
AX_VOID AX_OSAL_LIB_sort(void *base, size_t num, size_t size,cmp_func_t cmp_func,swap_func_t swap_func);

#ifdef __cplusplus
}
#endif

#endif /*__OSAL_LIBRARY_AX__H__*/


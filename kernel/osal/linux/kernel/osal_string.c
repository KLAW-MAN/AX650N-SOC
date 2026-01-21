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
#include "osal_lib_ax.h"

AX_S8 *AX_OSAL_LIB_strcpy(AX_S8 * dest, const AX_S8 * src)
{
	return strcpy(dest, src);
}

EXPORT_SYMBOL(AX_OSAL_LIB_strcpy);

AX_S8 *AX_OSAL_LIB_strncpy(AX_S8 * dest, const AX_S8 * src, AX_S32 count)
{
	return strncpy(dest, src, count);
}

EXPORT_SYMBOL(AX_OSAL_LIB_strncpy);

AX_S32 AX_OSAL_LIB_strlcpy(AX_S8 * dest, const AX_S8 * src, AX_S32 count)
{
	return strlcpy(dest, src, count);
}

EXPORT_SYMBOL(AX_OSAL_LIB_strlcpy);

AX_S8 *AX_OSAL_LIB_strcat(AX_S8 * dest, const AX_S8 * src)
{
	return strcat(dest, src);
}

EXPORT_SYMBOL(AX_OSAL_LIB_strcat);

AX_S8 *AX_OSAL_LIB_strncat(AX_S8 * dest, const AX_S8 * src, AX_S32 count)
{
	return strncat(dest, src, count);
}

EXPORT_SYMBOL(AX_OSAL_LIB_strncat);

AX_S32 AX_OSAL_LIB_strlcat(AX_S8 * dest, const AX_S8 * src, AX_S32 count)
{
	return strlcat(dest, src, count);
}

EXPORT_SYMBOL(AX_OSAL_LIB_strlcat);

AX_S32 AX_OSAL_LIB_strcmp(const AX_S8 * cs, const AX_S8 * ct)
{
	return strcmp(cs, ct);
}

EXPORT_SYMBOL(AX_OSAL_LIB_strcmp);

AX_S32 AX_OSAL_LIB_strncmp(const AX_S8 * cs, const AX_S8 * ct, AX_S32 count)
{
	return strncmp(cs, ct, count);
}

EXPORT_SYMBOL(AX_OSAL_LIB_strncmp);

AX_S32 AX_OSAL_LIB_strnicmp(const AX_S8 * s1, const AX_S8 * s2, AX_S32 len)
{
	return 0;
}

EXPORT_SYMBOL(AX_OSAL_LIB_strnicmp);

AX_S32 AX_OSAL_LIB_strcasecmp(const AX_S8 * s1, const AX_S8 * s2)
{
	return strcasecmp(s1, s2);
}

EXPORT_SYMBOL(AX_OSAL_LIB_strcasecmp);

AX_S32 AX_OSAL_LIB_strncasecmp(const AX_S8 * s1, const AX_S8 * s2, AX_S32 len)
{
	return strncasecmp(s1, s2, len);
}

EXPORT_SYMBOL(AX_OSAL_LIB_strncasecmp);

AX_S8 *AX_OSAL_LIB_strchr(const AX_S8 * s, AX_S32 c)
{
	return strchr(s, c);
}

EXPORT_SYMBOL(AX_OSAL_LIB_strchr);

AX_S8 *AX_OSAL_LIB_strnchr(const AX_S8 * s, AX_S32 count, AX_S32 c)
{
	return strnchr(s, count, c);
}

EXPORT_SYMBOL(AX_OSAL_LIB_strnchr);

AX_S8 *AX_OSAL_LIB_strrchr(const AX_S8 * s, AX_S32 c)
{
	return strrchr(s, c);
}

EXPORT_SYMBOL(AX_OSAL_LIB_strrchr);

AX_S8 *AX_OSAL_LIB_strstr(const AX_S8 * s1, const AX_S8 * s2)
{
	return strstr(s1, s2);
}

EXPORT_SYMBOL(AX_OSAL_LIB_strstr);

AX_S8 *AX_OSAL_LIB_strnstr(const AX_S8 * s1, const AX_S8 * s2, AX_S32 len)
{
	return strnstr(s1, s2, len);
}

EXPORT_SYMBOL(AX_OSAL_LIB_strnstr);

AX_S32 AX_OSAL_LIB_strlen(const AX_S8 * s)
{
	return strlen(s);
}

EXPORT_SYMBOL(AX_OSAL_LIB_strlen);

AX_S32 AX_OSAL_LIB_strnlen(const AX_S8 * s, AX_S32 count)
{
	return strnlen(s, count);
}

EXPORT_SYMBOL(AX_OSAL_LIB_strnlen);

AX_S8 *AX_OSAL_LIB_strpbrk(const AX_S8 * cs, const AX_S8 * ct)
{
	return strpbrk(cs, ct);
}

EXPORT_SYMBOL(AX_OSAL_LIB_strpbrk);

AX_S8 *AX_OSAL_LIB_strsep(const AX_S8 ** s, const AX_S8 * ct)
{
	return strsep((AX_CHAR **) s, ct);
}

EXPORT_SYMBOL(AX_OSAL_LIB_strsep);

AX_S32 AX_OSAL_LIB_strspn(const AX_S8 * s, const AX_S8 * accept)
{
	return strspn((AX_CHAR *) s, accept);
}

EXPORT_SYMBOL(AX_OSAL_LIB_strspn);

AX_S32 AX_OSAL_LIB_strcspn(const AX_S8 * s, const AX_S8 * reject)
{
	return strcspn((AX_CHAR *) s, reject);
}

EXPORT_SYMBOL(AX_OSAL_LIB_strcspn);

AX_VOID *AX_OSAL_LIB_memset(AX_VOID * str, AX_S32 c, AX_S32 count)
{
	return memset(str, c, count);
}

EXPORT_SYMBOL(AX_OSAL_LIB_memset);

AX_VOID *AX_OSAL_LIB_memmove(AX_VOID * dest, const AX_VOID * src, AX_S32 count)
{
	return memmove(dest, src, count);
}

EXPORT_SYMBOL(AX_OSAL_LIB_memmove);

AX_VOID *AX_OSAL_LIB_memscan(AX_VOID * addr, AX_S32 c, AX_S32 size)
{
	return memscan(addr, c, size);
}

EXPORT_SYMBOL(AX_OSAL_LIB_memscan);

AX_VOID *AX_OSAL_LIB_memcpy(AX_VOID * ct, const AX_VOID * cs, AX_S32 count)
{
	return memcpy(ct, cs, count);
}

EXPORT_SYMBOL(AX_OSAL_LIB_memcpy);

AX_VOID *AX_OSAL_LIB_memchr(const AX_VOID * s, AX_S32 c, AX_S32 n)
{
	return memchr(s, c, n);
}

EXPORT_SYMBOL(AX_OSAL_LIB_memchr);

AX_VOID *AX_OSAL_LIB_memchar_inv(const AX_VOID * start, AX_S32 c, AX_S32 bytes)
{
	return memchr_inv(start, c, bytes);
}

EXPORT_SYMBOL(AX_OSAL_LIB_memchar_inv);

AX_U64 AX_OSAL_LIB_simple_strtoull(const AX_S8 * cp, AX_S8 ** endp, AX_U32 base)
{
	return simple_strtoull(cp, (AX_CHAR **) endp, base);
}

EXPORT_SYMBOL(AX_OSAL_LIB_simple_strtoull);

AX_ULONG AX_OSAL_LIB_simple_strtoul(const AX_S8 * cp, AX_S8 ** endp, AX_U32 base)
{
	return simple_strtoul(cp, (AX_CHAR **) endp, base);
}

EXPORT_SYMBOL(AX_OSAL_LIB_simple_strtoul);

AX_LONG AX_OSAL_LIB_simple_strtol(const AX_S8 * cp, AX_S8 ** endp, AX_U32 base)
{
	return simple_strtol(cp, (AX_CHAR **) endp, base);
}

EXPORT_SYMBOL(AX_OSAL_LIB_simple_strtol);

AX_S64 AX_OSAL_LIB_simple_strtoll(const AX_S8 * cp, AX_S8 ** endp, AX_U32 base)
{
	return simple_strtoll(cp, (AX_CHAR **) endp, base);
}

EXPORT_SYMBOL(AX_OSAL_LIB_simple_strtoll);

AX_S32 AX_OSAL_LIB_snprintf(AX_S8 * buf, AX_S32 size, const AX_S8 * fmt, ...)
{
	va_list args;
	int i;

	va_start(args, fmt);
	i = vsnprintf(buf, size, fmt, args);
	va_end(args);

	return i;
}

EXPORT_SYMBOL(AX_OSAL_LIB_snprintf);

AX_S32 AX_OSAL_LIB_scnprintf(AX_S8 * buf, AX_S32 size, const AX_S8 * fmt, ...)
{
	va_list args;
	int i;

	va_start(args, fmt);
	i = vscnprintf(buf, size, fmt, args);
	va_end(args);

	return i;
}

EXPORT_SYMBOL(AX_OSAL_LIB_scnprintf);

AX_S32 AX_OSAL_LIB_sprintf(AX_S8 * buf, const AX_S8 * fmt, ...)
{
	va_list args;
	int i;

	va_start(args, fmt);
	i = vsnprintf(buf, INT_MAX, fmt, args);
	va_end(args);

	return i;
}

EXPORT_SYMBOL(AX_OSAL_LIB_sprintf);

AX_S32 AX_OSAL_LIB_vsscanf(AX_S8 * buf, const AX_S8 * fmt, ...)
{
	va_list args;
	int i;

	va_start(args, fmt);
	i = vsscanf(buf, fmt, args);
	va_end(args);

	return i;
}

EXPORT_SYMBOL(AX_OSAL_LIB_vsscanf);

AX_S32 AX_OSAL_LIB_vsnprintf(AX_S8 * str, AX_S32 size, const AX_S8 * fmt, AX_VA_LIST args)
{
	return vsnprintf(str, size, fmt, args);
}

EXPORT_SYMBOL(AX_OSAL_LIB_vsnprintf);

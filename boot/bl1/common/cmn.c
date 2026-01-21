#include "cmn.h"
int raise(void)
{
	return 0;
}

u8 readb(u64 addr)
{
	return *(const volatile u8 *)addr;
}

u16 readw(u64 addr)
{
	return *(const volatile u16 *)addr;
}

u32 readl(u64 addr)
{
	return *(const volatile u32 *)addr;
}


void writeb(u8 value, u64 addr)
{
	*(volatile u8 *)addr = value;
}

void writew(u16 value, u64 addr)
{
	*(volatile u16 *)addr = value;
}

void writel(u32 value, u64 addr)
{
	*(volatile u32 *)addr = value;
}

void *ax_memset(void *start, u8 value, u32 len)
{
	u8 *dest = start;

	//may optimize later
	while (len--)
		*dest++ = value;

	return start;
}

void *ax_memcpy(void *dest, const void *src, u32 count)
{
	char *tmp = dest;
	const char *s = src;

	//may optimize later
	while (count--)
		*tmp++ = *s++;
	return dest;
}

u32 ax_strlen(const char *s)
{
	const char *sc;

	for (sc = s; *sc != '\0'; ++sc)
		/* nothing */;
	return sc - s;
}


#ifndef __CMN_H
#define __CMN_H

typedef unsigned char u8;
typedef char s8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long u64;


u8 readb(u64 addr);
u16 readw(u64 addr);
u32 readl(u64 addr);
void writeb(u8 value, u64 addr);
void writew(u16 value, u64 addr);
void writel(u32 value, u64 addr);
void *ax_memset(void *start, u8 value, u32 len);
void *ax_memcpy(void *dest, const void *src, u32 count);
u32 ax_strlen(const char *s);
int dma_memcpy(u32 dst, u32 src, int size);
int dma_memset(u32 dst, u32 value, int size);

#define UNUSED(x)	(void)x
#define NULL		(void *)0

#define BITS_PER_INT				32	/*64-bit CPU, sizeof(int)=4 */
#define BITS_PER_LONG				64	/*64-bit CPU, sizeof(long)=8 */

#define BIT(nr)						(1U << (nr))
#define BIT_UL(nr)					(1UL << (nr))
#define BIT_MASK(nr)				(1U << ((nr) % BITS_PER_INT))
#define BIT_UL_MASK(nr)				(1UL << ((nr) % BITS_PER_LONG))
#define BIT_UL_WORD(nr)				((nr) / BITS_PER_LONG)
#define BITS_PER_BYTE				8

/*
 * Create a contiguous bitmask starting at bit position @l and ending at
 * position @h. For example
 * GENMASK_UL(39, 21) gives us the 64bit vector 0x000000ffffe00000.
 */
#define GENMASK(h, l) \
	(((~0U) << (l)) & (~0U >> (BITS_PER_INT - 1 - (h))))

#define GENMASK_ULL(h, l) \
	(((~0UL) << (l)) & (~0UL >> (BITS_PER_LONG - 1 - (h))))

#endif

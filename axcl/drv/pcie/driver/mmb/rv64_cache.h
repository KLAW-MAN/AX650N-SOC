#pragma once

// Thanks the reference listed below:
// https://github.com/sophgo/zsbl/blob/master/plat/sg2042/cache.c


static void riscv_sync_dcache(void)
{
#if defined(__THEAD_VERSION__)
	asm volatile (".word 0x01a0000b" : : : );
#else
	asm volatile ("fence iorw, iorw" : : : "memory");
#endif
}

void __attribute__((weak)) riscv_sync_icache(void) {
	asm volatile ("fence.i");
}

void __attribute__((weak)) riscv_flush_dcache_range(unsigned long start, unsigned long end)
{
#if defined(__THEAD_VERSION__)
	register unsigned long i asm ("a0");
	for (i = start & ~(L1_CACHE_BYTES - 1); i < end; i += L1_CACHE_BYTES) {
		asm volatile (".word 0x0275000b" : : : );
	}
#else
	register unsigned long i;
	for (i = start & ~(L1_CACHE_BYTES - 1); i < end; i += L1_CACHE_BYTES) {
		asm volatile ("cbo.flush (%0)" : : "r" (i) : );
	}
#endif

	riscv_sync_dcache();
}

void __attribute__((weak)) riscv_invalidate_dcache_range(unsigned long start, unsigned long end)
{
#if defined(__THEAD_VERSION__)
	register unsigned long i asm("a0");
	for (i = start & ~(L1_CACHE_BYTES - 1); i < end; i += L1_CACHE_BYTES) {
		asm volatile (".word 0x0265000b" : : : );
	}
#else
	register unsigned long i asm("a0") = start & ~(L1_CACHE_BYTES - 1);
	for (i = start & ~(L1_CACHE_BYTES - 1); i < end; i += L1_CACHE_BYTES) {
		asm volatile ("cbo.inval (%0)" : : "r" (i) : );
	}
#endif

	riscv_sync_dcache();
}

void __attribute__((weak)) riscv_clean_dcache_range(unsigned long start, unsigned long end)
{
#if defined(__THEAD_VERSION__)
	riscv_flush_dcache_range(start, end);
#else
	register unsigned long i asm("a0") = start & ~(L1_CACHE_BYTES - 1);
	for (i = start & ~(L1_CACHE_BYTES - 1); i < end; i += L1_CACHE_BYTES) {
		asm volatile ("cbo.clean (%0)" : : "r" (i) : );
	}
#endif

	riscv_sync_dcache();
}

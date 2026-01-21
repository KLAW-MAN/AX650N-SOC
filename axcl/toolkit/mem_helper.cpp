/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include "mem_helper.hpp"

namespace axcl {

mem_helper::SIMD mem_helper::simd_type = SIMD::unknown;

void mem_helper::init_check_simd() {
#if (defined(__aarch64__) || defined(__arm__))
    simd_type = SIMD::neon;
#else
    uint32_t eax, ebx, ecx, edx;

    __asm__ __volatile__(
        "cpuid"
        : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx)
        : "a"(1)
    );

    if (edx & (1 << 25)) {
        simd_type = SIMD::sse;
    }
    if (edx & (1 << 26)) {
        simd_type = SIMD::sse;
    }
    if (ecx & (1 << 0)) {
        simd_type = SIMD::sse;
    }
    if (ecx & (1 << 19)) {
        simd_type = SIMD::sse;
    }
    if (ecx & (1 << 20)) {
        simd_type = SIMD::sse;
    }
    if (ecx & (1 << 28)) {
        simd_type = SIMD::avx;
    }
#endif
    printf("simd_type [%d]\n", (uint32_t)simd_type);
}

void mem_helper::memcpy(uint8_t *dest, const uint8_t *src, uint64_t size) {
    switch(simd_type) {
#if (defined(__aarch64__) || defined(__arm__))
        case SIMD::neon:
            neon_memcpy(dest, src, size);
            break;
#else
        case SIMD::sse:
            sse_memcpy(dest, src, size);
            break;
        case SIMD::avx:
            avx_memcpy(dest, src, size);
            break;
#endif
        default:
            memcpy(dest, src, size);
            break;
    }
}

#if (defined(__aarch64__) || defined(__arm__))
void mem_helper::neon_memcpy(uint8_t *dest, const uint8_t *src, uint64_t n) {
    uint8_t *d = dest;
    const uint8_t *s = src;

    while (((uintptr_t)d % 16) && n > 0) {
        *d++ = *s++;
        n--;
    }

    while (n >= 16) {
        vst1q_u8(d, vld1q_u8(s));
        d += 16;
        s += 16;
        n -= 16;
    }

    while (n > 0) {
        *d++ = *s++;
        n--;
    }
}
#else
void mem_helper::sse_memcpy(uint8_t *dest, const uint8_t *src, uint64_t n) {
    uint8_t *d = dest;
    const uint8_t *s = src;

    while (((uintptr_t)d % 16) && n > 0) {
        *d++ = *s++;
        n--;
    }

    __m128i xmm;
    while (n >= 16) {
        xmm = _mm_loadu_si128((__m128i *)s);
        _mm_storeu_si128((__m128i *)d, xmm);
        d += 16;
        s += 16;
        n -= 16;
    }

    while (n > 0) {
        *d++ = *s++;
        n--;
    }
}

void mem_helper::avx_memcpy(uint8_t *dest, const uint8_t *src, uint64_t n) {
    uint8_t *d = dest;
    const uint8_t *s = src;

    while (((uintptr_t)d % 32) && n > 0) {
        *d++ = *s++;
        n--;
    }

    __m256i ymm;
    while (n >= 32) {
        ymm = _mm256_loadu_si256((__m256i *)s);
        _mm256_storeu_si256((__m256i *)d, ymm);
        d += 32;
        s += 32;
        n -= 32;
    }

    while (n > 0) {
        *d++ = *s++;
        n--;
    }
}
#endif

}  // namespace axcl

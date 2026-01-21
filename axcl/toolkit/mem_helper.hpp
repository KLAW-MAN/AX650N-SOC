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

#if (defined(__aarch64__) || defined(__arm__))
#include <arm_neon.h>  // ARM SIMD: NEON
#else
#include <emmintrin.h> // X86 SIMD: SSE2
#include <immintrin.h> // X86 SIMD: AVX
#endif

#include <stdio.h>
#include <stddef.h>
#include <stdint.h>

namespace axcl {

class mem_helper final {
public:
    enum class SIMD {
        neon = 1,
        sse = 2,
        avx = 3,
        unknown = 4
    };

public:
    /**
     * @brief
     */
    static void init_check_simd();

    /**
     * @brief
     */
    static void memcpy(uint8_t *dest, const uint8_t *src, uint64_t size);

    // pending for other memory functions

private:
    mem_helper() = default;
    ~mem_helper() = default;

private:
#if (defined(__aarch64__) || defined(__arm__))
    static void neon_memcpy(uint8_t *dest, const uint8_t *src, uint64_t n);
#else
    static void sse_memcpy(uint8_t *dest, const uint8_t *src, uint64_t n);
    static void avx_memcpy(uint8_t *dest, const uint8_t *src, uint64_t n);
#endif

private:
    static SIMD simd_type;
};

}  // namespace axcl
//
// Copyright 2018 Sepehr Taghdisian (septag@github). All rights reserved.
// License: https://github.com/septag/sx#license-bsd-2-clause
//
// parts of this code is copied from bx library: https://github.com/bkaradzic/bx
// Copyright 2011-2019 Branimir Karadzic. All rights reserved.
// License: https://github.com/bkaradzic/bx#license-bsd-2-clause
//
#pragma once

#include "sx.h"

#define SX_SIMD_NEON 0
#define SX_SIMD_SSE 0

#if !SX_CONFIG_SIMD_DISABLE
#    if defined(__SSE2__) || (SX_COMPILER_MSVC && (SX_ARCH_64BIT || _M_IX86_FP >= 2))
#        include <emmintrin.h>    // __m128i
#        if defined(__SSE4_1__)
#            include <smmintrin.h>
#        endif                    // defined(__SSE4_1__)
#        include <xmmintrin.h>    // __m128
#        undef SX_SIMD_SSE
#        define SX_SIMD_SSE 1
#    elif defined(__ARM_NEON__)
#        include <arm_neon.h>
#        undef SX_SIMD_NEON
#        define SX_SIMD_NEON 1
#    endif    //
#endif // SX_CONFIG_SIMD_DISABLE

#if SX_COMPILER_MSVC
#    define SX_SIMD_INLINE __forceinline
#else
#    define SX_SIMD_INLINE static inline __attribute__((__always_inline__))
#endif

#define SX_SIMD_ENABLED (SX_SIMD_NEON || SX_SIMD_SSE)

#if SX_SIMD_SSE
////////////////////////////////////////////////////////////////////////////////////////////////////
// SSE
typedef __m128 sx_simd_t;

#    define SX__ELEMx 0
#    define SX__ELEMy 1
#    define SX__ELEMz 2
#    define SX__ELEMw 3

#    define sx__simd_implement_swizzle(_x, _y, _z, _w)                                        \
        SX_SIMD_INLINE sx_simd_t sx_simd_swizzle_##_x##_y##_z##_w(sx_simd_t _a) {             \
            return _mm_shuffle_ps(                                                            \
                _a, _a, _MM_SHUFFLE(SX__ELEM##_w, SX__ELEM##_z, SX__ELEM##_y, SX__ELEM##_x)); \
        }

#    define sx__simd_implement_test(_xyzw, _mask)                       \
        SX_SIMD_INLINE bool sx_simd_test_any_##_xyzw(sx_simd_t _test) { \
            return (_mm_movemask_ps(_test) & (_mask)) != 0;             \
        }                                                               \
                                                                        \
        SX_SIMD_INLINE bool sx_simd_test_all_##_xyzw(sx_simd_t _test) { \
            return (_mm_movemask_ps(_test) & (_mask)) == (_mask);       \
        }

#    ifndef SX__SIMD_H_
#        define SX__SIMD_H_
#        include "_simd.h"
#        undef SX__SIMD_H_
#    endif

#    undef SX__ELEMx
#    undef SX__ELEMy
#    undef SX__ELEMz
#    undef SX__ELEMw
#    undef sx__simd_implement_swizzle
#    undef sx__simd_implement_test

SX_SIMD_INLINE sx_simd_t sx_simd_shuffle_xyAB(sx_simd_t _xyzw, sx_simd_t _ABCD) {
    return _mm_movelh_ps(_xyzw, _ABCD);
}

SX_SIMD_INLINE sx_simd_t sx_simd_shuffle_ABxy(sx_simd_t _xyzw, sx_simd_t _ABCD) {
    return _mm_movelh_ps(_ABCD, _xyzw);
}

SX_SIMD_INLINE sx_simd_t sx_simd_shuffle_CDzw(sx_simd_t _xyzw, sx_simd_t _ABCD) {
    return _mm_movehl_ps(_xyzw, _ABCD);
}

SX_SIMD_INLINE sx_simd_t sx_simd_shuffle_zwCD(sx_simd_t _xyzw, sx_simd_t _ABCD) {
    return _mm_movehl_ps(_ABCD, _xyzw);
}

SX_SIMD_INLINE sx_simd_t sx_simd_shuffle_xAyB(sx_simd_t _xyzw, sx_simd_t _ABCD) {
    return _mm_unpacklo_ps(_xyzw, _ABCD);
}

SX_SIMD_INLINE sx_simd_t sx_simd_shuffle_AxBy(sx_simd_t _xyzw, sx_simd_t _ABCD) {
    return _mm_unpacklo_ps(_ABCD, _xyzw);
}

SX_SIMD_INLINE sx_simd_t sx_simd_shuffle_zCwD(sx_simd_t _xyzw, sx_simd_t _ABCD) {
    return _mm_unpackhi_ps(_xyzw, _ABCD);
}

SX_SIMD_INLINE sx_simd_t sx_simd_shuffle_CzDw(sx_simd_t _xyzw, sx_simd_t _ABCD) {
    return _mm_unpackhi_ps(_ABCD, _xyzw);
}

SX_SIMD_INLINE float sx_simd_x(sx_simd_t _a) {
    return _mm_cvtss_f32(_a);
}

SX_SIMD_INLINE float sx_simd_y(sx_simd_t _a) {
    const sx_simd_t yyyy = sx_simd_swizzle_yyyy(_a);
    const float     result = _mm_cvtss_f32(yyyy);
    return result;
}

SX_SIMD_INLINE float sx_simd_z(sx_simd_t _a) {
    const sx_simd_t zzzz = sx_simd_swizzle_zzzz(_a);
    const float     result = _mm_cvtss_f32(zzzz);
    return result;
}

SX_SIMD_INLINE float sx_simd_w(sx_simd_t _a) {
    const sx_simd_t wwww = sx_simd_swizzle_wwww(_a);
    const float     result = _mm_cvtss_f32(wwww);
    return result;
}

SX_SIMD_INLINE sx_simd_t sx_simd_load(const void* _ptr) {
    return _mm_load_ps((const float*)(_ptr));
}

SX_SIMD_INLINE void sx_simd_store(void* _ptr, sx_simd_t _a) {
    _mm_store_ps((float*)(_ptr), _a);
}

SX_SIMD_INLINE void sx_simd_store32(void* _ptr, sx_simd_t _a) {
    _mm_store_ss((float*)(_ptr), _a);
}

SX_SIMD_INLINE void sx_simd_stream(void* _ptr, sx_simd_t _a) {
    _mm_stream_ps((float*)(_ptr), _a);
}

SX_SIMD_INLINE sx_simd_t sx_simd_load4(float _x, float _y, float _z, float _w) {
    return _mm_set_ps(_w, _z, _y, _x);
}

SX_SIMD_INLINE sx_simd_t sx_simd_loadui(uint32_t _x, uint32_t _y, uint32_t _z, uint32_t _w) {
    const __m128i   set = _mm_set_epi32(_w, _z, _y, _x);
    const sx_simd_t result = _mm_castsi128_ps(set);
    return result;
}

SX_SIMD_INLINE sx_simd_t sx_simd_splatx(const void* _ptr) {
    const sx_simd_t x___ = _mm_load_ss((const float*)(_ptr));
    const sx_simd_t result = sx_simd_swizzle_xxxx(x___);
    return result;
}

SX_SIMD_INLINE sx_simd_t sx_simd_splat1(float _a) {
    return _mm_set1_ps(_a);
}

SX_SIMD_INLINE sx_simd_t sx_simd_splatui(uint32_t _a) {
    const __m128i   splat = _mm_set1_epi32(_a);
    const sx_simd_t result = _mm_castsi128_ps(splat);
    return result;
}

SX_SIMD_INLINE sx_simd_t sx_simd_zero() {
    return _mm_setzero_ps();
}

SX_SIMD_INLINE sx_simd_t sx_simd_itof(sx_simd_t _a) {
    const __m128i   itof = _mm_castps_si128(_a);
    const sx_simd_t result = _mm_cvtepi32_ps(itof);
    return result;
}

SX_SIMD_INLINE sx_simd_t sx_simd_ftoi(sx_simd_t _a) {
    const __m128i   ftoi = _mm_cvtps_epi32(_a);
    const sx_simd_t result = _mm_castsi128_ps(ftoi);
    return result;
}

SX_SIMD_INLINE sx_simd_t sx_simd_round(sx_simd_t _a) {
#    if defined(__SSE4_1__)
    return _mm_round_ps(_a, _MM_FROUND_NINT);
#    else
    const __m128i   round = _mm_cvtps_epi32(_a);
    const sx_simd_t result = _mm_cvtepi32_ps(round);
    return result;
#    endif    // defined(__SSE4_1__)
}

SX_SIMD_INLINE sx_simd_t sx_simd_add(sx_simd_t _a, sx_simd_t _b) {
    return _mm_add_ps(_a, _b);
}

SX_SIMD_INLINE sx_simd_t sx_simd_sub(sx_simd_t _a, sx_simd_t _b) {
    return _mm_sub_ps(_a, _b);
}

SX_SIMD_INLINE sx_simd_t sx_simd_mul(sx_simd_t _a, sx_simd_t _b) {
    return _mm_mul_ps(_a, _b);
}

SX_SIMD_INLINE sx_simd_t sx_simd_div(sx_simd_t _a, sx_simd_t _b) {
    return _mm_div_ps(_a, _b);
}

SX_SIMD_INLINE sx_simd_t sx_simd_rcp_est(sx_simd_t _a) {
    return _mm_rcp_ps(_a);
}

SX_SIMD_INLINE sx_simd_t sx_simd_sqrt(sx_simd_t _a) {
    return _mm_sqrt_ps(_a);
}

SX_SIMD_INLINE sx_simd_t sx_simd_rsqrt_est(sx_simd_t _a) {
    return _mm_rsqrt_ps(_a);
}

SX_SIMD_INLINE sx_simd_t sx_simd_dot3(sx_simd_t _a, sx_simd_t _b) {
#    if defined(__SSE4_1__)
    return _mm_dp_ps(_a, _b, 0x77);
#    else
    const sx_simd_t xyzw = sx_simd_mul(_a, _b);
    const sx_simd_t xxxx = sx_simd_swizzle_xxxx(xyzw);
    const sx_simd_t yyyy = sx_simd_swizzle_yyyy(xyzw);
    const sx_simd_t zzzz = sx_simd_swizzle_zzzz(xyzw);
    const sx_simd_t tmp1 = sx_simd_add(xxxx, yyyy);
    return sx_simd_add(zzzz, tmp1);
#    endif    // defined(__SSE4__)
}

SX_SIMD_INLINE sx_simd_t sx_simd_dot(sx_simd_t _a, sx_simd_t _b) {
#    if defined(__SSE4_1__)
    return _mm_dp_ps(_a, _b, 0xFF);
#    else
    const sx_simd_t xyzw = sx_simd_mul(_a, _b);
    const sx_simd_t yzwx = sx_simd_swizzle_yzwx(xyzw);
    const sx_simd_t tmp0 = sx_simd_add(xyzw, yzwx);
    const sx_simd_t zwxy = sx_simd_swizzle_zwxy(tmp0);
    return sx_simd_add(tmp0, zwxy);
#    endif    // defined(__SSE4__)
}

SX_SIMD_INLINE sx_simd_t sx_simd_cmpeq(sx_simd_t _a, sx_simd_t _b) {
    return _mm_cmpeq_ps(_a, _b);
}

SX_SIMD_INLINE sx_simd_t sx_simd_cmplt(sx_simd_t _a, sx_simd_t _b) {
    return _mm_cmplt_ps(_a, _b);
}

SX_SIMD_INLINE sx_simd_t sx_simd_cmple(sx_simd_t _a, sx_simd_t _b) {
    return _mm_cmple_ps(_a, _b);
}

SX_SIMD_INLINE sx_simd_t sx_simd_cmpgt(sx_simd_t _a, sx_simd_t _b) {
    return _mm_cmpgt_ps(_a, _b);
}

SX_SIMD_INLINE sx_simd_t sx_simd_cmpge(sx_simd_t _a, sx_simd_t _b) {
    return _mm_cmpge_ps(_a, _b);
}

SX_SIMD_INLINE sx_simd_t sx_simd_min(sx_simd_t _a, sx_simd_t _b) {
    return _mm_min_ps(_a, _b);
}

SX_SIMD_INLINE sx_simd_t sx_simd_max(sx_simd_t _a, sx_simd_t _b) {
    return _mm_max_ps(_a, _b);
}

SX_SIMD_INLINE sx_simd_t sx_simd_and(sx_simd_t _a, sx_simd_t _b) {
    return _mm_and_ps(_a, _b);
}

SX_SIMD_INLINE sx_simd_t sx_simd_andc(sx_simd_t _a, sx_simd_t _b) {
    return _mm_andnot_ps(_b, _a);
}

SX_SIMD_INLINE sx_simd_t sx_simd_or(sx_simd_t _a, sx_simd_t _b) {
    return _mm_or_ps(_a, _b);
}

SX_SIMD_INLINE sx_simd_t sx_simd_xor(sx_simd_t _a, sx_simd_t _b) {
    return _mm_xor_ps(_a, _b);
}

#elif SX_SIMD_NEON
////////////////////////////////////////////////////////////////////////////////////////////////////
// Neon
typedef float32x4_t sx_simd_t;

#    error "TODO"

#else
////////////////////////////////////////////////////////////////////////////////////////////////////
// Reference
SX_CONSTFN float sx_sqrt(float _a);
SX_CONSTFN float sx_rsqrt(float _a);

typedef union {
    float    fxyzw[4];
    int      ixyzw[4];
    uint32_t uxyzw[4];
} sx_simd_t;

#    define SX__ELEMx 0
#    define SX__ELEMy 1
#    define SX__ELEMz 2
#    define SX__ELEMw 3

#    define sx__simd_implement_swizzle(_x, _y, _z, _w)                            \
        SX_SIMD_INLINE sx_simd_t sx_simd_swizzle_##_x##_y##_z##_w(sx_simd_t _a) { \
            sx_simd_t result;                                                     \
            result.ixyzw[0] = _a.ixyzw[SX__ELEM##_x];                             \
            result.ixyzw[1] = _a.ixyzw[SX__ELEM##_y];                             \
            result.ixyzw[2] = _a.ixyzw[SX__ELEM##_z];                             \
            result.ixyzw[3] = _a.ixyzw[SX__ELEM##_w];                             \
            return result;                                                        \
        }

#    define sx__simd_implement_test(_xyzw, _mask)                                          \
        SX_SIMD_INLINE bool sx_simd_test_any_##_xyzw(sx_simd_t _test) {                    \
            uint32_t tmp = ((_test.uxyzw[3] >> 31) << 3) | ((_test.uxyzw[2] >> 31) << 2) | \
                           ((_test.uxyzw[1] >> 31) << 1) | (_test.uxyzw[0] >> 31);         \
            return 0 != (tmp & (_mask));                                                   \
        }                                                                                  \
                                                                                           \
        SX_SIMD_INLINE bool sx_simd_test_all_##_xyzw(sx_simd_t _test) {                    \
            uint32_t tmp = ((_test.uxyzw[3] >> 31) << 3) | ((_test.uxyzw[2] >> 31) << 2) | \
                           ((_test.uxyzw[1] >> 31) << 1) | (_test.uxyzw[0] >> 31);         \
            return (_mask) == (tmp & (_mask));                                             \
        }

#    ifndef SX__SIMD_H_
#        define SX__SIMD_H_
#        include "_simd.h"
#        undef SX__SIMD_H_
#    endif

SX_SIMD_INLINE sx_simd_t sx_simd_shuffle_xyAB(sx_simd_t _xyzw, sx_simd_t _ABCD) {
    sx_simd_t result;
    result.uxyzw[0] = _xyzw.uxyzw[0];
    result.uxyzw[1] = _xyzw.uxyzw[1];
    result.uxyzw[2] = _ABCD.uxyzw[0];
    result.uxyzw[3] = _ABCD.uxyzw[1];
    return result;
}

SX_SIMD_INLINE sx_simd_t sx_simd_shuffle_ABxy(sx_simd_t _xyzw, sx_simd_t _ABCD) {
    sx_simd_t result;
    result.uxyzw[0] = _ABCD.uxyzw[0];
    result.uxyzw[1] = _ABCD.uxyzw[1];
    result.uxyzw[2] = _xyzw.uxyzw[0];
    result.uxyzw[3] = _xyzw.uxyzw[1];
    return result;
}

SX_SIMD_INLINE sx_simd_t sx_simd_shuffle_CDzw(sx_simd_t _xyzw, sx_simd_t _ABCD) {
    sx_simd_t result;
    result.uxyzw[0] = _ABCD.uxyzw[2];
    result.uxyzw[1] = _ABCD.uxyzw[3];
    result.uxyzw[2] = _xyzw.uxyzw[2];
    result.uxyzw[3] = _xyzw.uxyzw[3];
    return result;
}

SX_SIMD_INLINE sx_simd_t sx_simd_shuffle_zwCD(sx_simd_t _xyzw, sx_simd_t _ABCD) {
    sx_simd_t result;
    result.uxyzw[0] = _xyzw.uxyzw[2];
    result.uxyzw[1] = _xyzw.uxyzw[3];
    result.uxyzw[2] = _ABCD.uxyzw[2];
    result.uxyzw[3] = _ABCD.uxyzw[3];
    return result;
}

SX_SIMD_INLINE sx_simd_t sx_simd_shuffle_xAyB(sx_simd_t _xyzw, sx_simd_t _ABCD) {
    sx_simd_t result;
    result.uxyzw[0] = _xyzw.uxyzw[0];
    result.uxyzw[1] = _ABCD.uxyzw[0];
    result.uxyzw[2] = _xyzw.uxyzw[1];
    result.uxyzw[3] = _ABCD.uxyzw[1];
    return result;
}

SX_SIMD_INLINE sx_simd_t sx_simd_shuffle_AxBy(sx_simd_t _xyzw, sx_simd_t _ABCD) {
    sx_simd_t result;
    result.uxyzw[0] = _xyzw.uxyzw[1];
    result.uxyzw[1] = _ABCD.uxyzw[1];
    result.uxyzw[2] = _xyzw.uxyzw[0];
    result.uxyzw[3] = _ABCD.uxyzw[0];
    return result;
}

SX_SIMD_INLINE sx_simd_t sx_simd_shuffle_zCwD(sx_simd_t _xyzw, sx_simd_t _ABCD) {
    sx_simd_t result;
    result.uxyzw[0] = _xyzw.uxyzw[2];
    result.uxyzw[1] = _ABCD.uxyzw[2];
    result.uxyzw[2] = _xyzw.uxyzw[3];
    result.uxyzw[3] = _ABCD.uxyzw[3];
    return result;
}

SX_SIMD_INLINE sx_simd_t sx_simd_shuffle_CzDw(sx_simd_t _xyzw, sx_simd_t _ABCD) {
    sx_simd_t result;
    result.uxyzw[0] = _ABCD.uxyzw[2];
    result.uxyzw[1] = _xyzw.uxyzw[2];
    result.uxyzw[2] = _ABCD.uxyzw[3];
    result.uxyzw[3] = _xyzw.uxyzw[3];
    return result;
}

SX_SIMD_INLINE float sx_simd_x(sx_simd_t _a) {
    return _a.fxyzw[0];
}

SX_SIMD_INLINE float sx_simd_y(sx_simd_t _a) {
    return _a.fxyzw[1];
}

SX_SIMD_INLINE float sx_simd_z(sx_simd_t _a) {
    return _a.fxyzw[2];
}

SX_SIMD_INLINE float sx_simd_w(sx_simd_t _a) {
    return _a.fxyzw[3];
}

SX_SIMD_INLINE sx_simd_t sx_simd_load(const void* _ptr) {
    const uint32_t* input = (const uint32_t*)(_ptr);
    sx_simd_t       result;
    result.uxyzw[0] = input[0];
    result.uxyzw[1] = input[1];
    result.uxyzw[2] = input[2];
    result.uxyzw[3] = input[3];
    return result;
}

SX_SIMD_INLINE void sx_simd_store(void* _ptr, sx_simd_t _a) {
    uint32_t* result = (uint32_t*)(_ptr);
    result[0] = _a.uxyzw[0];
    result[1] = _a.uxyzw[1];
    result[2] = _a.uxyzw[2];
    result[3] = _a.uxyzw[3];
}

SX_SIMD_INLINE void sx_simd_store32(void* _ptr, sx_simd_t _a) {
    uint32_t* result = (uint32_t*)(_ptr);
    result[0] = _a.uxyzw[0];
}

SX_SIMD_INLINE void sx_simd_stream(void* _ptr, sx_simd_t _a) {
    uint32_t* result = (uint32_t*)(_ptr);
    result[0] = _a.uxyzw[0];
    result[1] = _a.uxyzw[1];
    result[2] = _a.uxyzw[2];
    result[3] = _a.uxyzw[3];
}

SX_SIMD_INLINE sx_simd_t sx_simd_load4(float _x, float _y, float _z, float _w) {
    sx_simd_t result;
    result.fxyzw[0] = _x;
    result.fxyzw[1] = _y;
    result.fxyzw[2] = _z;
    result.fxyzw[3] = _w;
    return result;
}

SX_SIMD_INLINE sx_simd_t sx_simd_loadui(uint32_t _x, uint32_t _y, uint32_t _z, uint32_t _w) {
    sx_simd_t result;
    result.uxyzw[0] = _x;
    result.uxyzw[1] = _y;
    result.uxyzw[2] = _z;
    result.uxyzw[3] = _w;
    return result;
}

SX_SIMD_INLINE sx_simd_t sx_simd_splatx(const void* _ptr) {
    const uint32_t val = *(const uint32_t*)(_ptr);
    sx_simd_t      result;
    result.uxyzw[0] = val;
    result.uxyzw[1] = val;
    result.uxyzw[2] = val;
    result.uxyzw[3] = val;
    return result;
}

SX_SIMD_INLINE sx_simd_t sx_simd_splat1(float _a) {
    return sx_simd_load4(_a, _a, _a, _a);
}

SX_SIMD_INLINE sx_simd_t sx_simd_splatui(uint32_t _a) {
    return sx_simd_loadui(_a, _a, _a, _a);
}

SX_SIMD_INLINE sx_simd_t sx_simd_zero() {
    return sx_simd_loadui(0, 0, 0, 0);
}

SX_SIMD_INLINE sx_simd_t sx_simd_itof(sx_simd_t _a) {
    sx_simd_t result;
    result.fxyzw[0] = (float)_a.ixyzw[0];
    result.fxyzw[1] = (float)_a.ixyzw[1];
    result.fxyzw[2] = (float)_a.ixyzw[2];
    result.fxyzw[3] = (float)_a.ixyzw[3];
    return result;
}

SX_SIMD_INLINE sx_simd_t sx_simd_ftoi(sx_simd_t _a) {
    sx_simd_t result;
    result.ixyzw[0] = (int)_a.fxyzw[0];
    result.ixyzw[1] = (int)_a.fxyzw[1];
    result.ixyzw[2] = (int)_a.fxyzw[2];
    result.ixyzw[3] = (int)_a.fxyzw[3];
    return result;
}

SX_SIMD_INLINE sx_simd_t sx_simd_round(sx_simd_t _a) {
    const sx_simd_t tmp = sx_simd_ftoi(_a);
    const sx_simd_t result = sx_simd_itof(tmp);
    return result;
}

SX_SIMD_INLINE sx_simd_t sx_simd_add(sx_simd_t _a, sx_simd_t _b) {
    sx_simd_t result;
    result.fxyzw[0] = _a.fxyzw[0] + _b.fxyzw[0];
    result.fxyzw[1] = _a.fxyzw[1] + _b.fxyzw[1];
    result.fxyzw[2] = _a.fxyzw[2] + _b.fxyzw[2];
    result.fxyzw[3] = _a.fxyzw[3] + _b.fxyzw[3];
    return result;
}

SX_SIMD_INLINE sx_simd_t sx_simd_sub(sx_simd_t _a, sx_simd_t _b) {
    sx_simd_t result;
    result.fxyzw[0] = _a.fxyzw[0] - _b.fxyzw[0];
    result.fxyzw[1] = _a.fxyzw[1] - _b.fxyzw[1];
    result.fxyzw[2] = _a.fxyzw[2] - _b.fxyzw[2];
    result.fxyzw[3] = _a.fxyzw[3] - _b.fxyzw[3];
    return result;
}

SX_SIMD_INLINE sx_simd_t sx_simd_mul(sx_simd_t _a, sx_simd_t _b) {
    sx_simd_t result;
    result.fxyzw[0] = _a.fxyzw[0] * _b.fxyzw[0];
    result.fxyzw[1] = _a.fxyzw[1] * _b.fxyzw[1];
    result.fxyzw[2] = _a.fxyzw[2] * _b.fxyzw[2];
    result.fxyzw[3] = _a.fxyzw[3] * _b.fxyzw[3];
    return result;
}

SX_SIMD_INLINE sx_simd_t sx_simd_div(sx_simd_t _a, sx_simd_t _b) {
    sx_simd_t result;
    result.fxyzw[0] = _a.fxyzw[0] / _b.fxyzw[0];
    result.fxyzw[1] = _a.fxyzw[1] / _b.fxyzw[1];
    result.fxyzw[2] = _a.fxyzw[2] / _b.fxyzw[2];
    result.fxyzw[3] = _a.fxyzw[3] / _b.fxyzw[3];
    return result;
}

SX_SIMD_INLINE sx_simd_t sx_simd_rcp_est(sx_simd_t _a) {
    sx_simd_t result;
    result.fxyzw[0] = 1.0f / _a.fxyzw[0];
    result.fxyzw[1] = 1.0f / _a.fxyzw[1];
    result.fxyzw[2] = 1.0f / _a.fxyzw[2];
    result.fxyzw[3] = 1.0f / _a.fxyzw[3];
    return result;
}

SX_SIMD_INLINE sx_simd_t sx_simd_sqrt(sx_simd_t _a) {
    sx_simd_t result;
    result.fxyzw[0] = sx_sqrt(_a.fxyzw[0]);
    result.fxyzw[1] = sx_sqrt(_a.fxyzw[1]);
    result.fxyzw[2] = sx_sqrt(_a.fxyzw[2]);
    result.fxyzw[3] = sx_sqrt(_a.fxyzw[3]);
    return result;
}

SX_SIMD_INLINE sx_simd_t sx_simd_rsqrt_est(sx_simd_t _a) {
    sx_simd_t result;
    result.fxyzw[0] = sx_rsqrt(_a.fxyzw[0]);
    result.fxyzw[1] = sx_rsqrt(_a.fxyzw[1]);
    result.fxyzw[2] = sx_rsqrt(_a.fxyzw[2]);
    result.fxyzw[3] = sx_rsqrt(_a.fxyzw[3]);
    return result;
}

SX_SIMD_INLINE sx_simd_t sx_simd_dot3(sx_simd_t _a, sx_simd_t _b) {
#    if defined(__SSE4_1__)
    return _mm_dp_ps(_a, _b, 0x77);
#    else
    const sx_simd_t xyzw = sx_simd_mul(_a, _b);
    const sx_simd_t xxxx = sx_simd_swizzle_xxxx(xyzw);
    const sx_simd_t yyyy = sx_simd_swizzle_yyyy(xyzw);
    const sx_simd_t zzzz = sx_simd_swizzle_zzzz(xyzw);
    const sx_simd_t tmp1 = sx_simd_add(xxxx, yyyy);
    return sx_simd_add(zzzz, tmp1);
#    endif    // defined(__SSE4__)
}

SX_SIMD_INLINE sx_simd_t sx_simd_dot(sx_simd_t _a, sx_simd_t _b) {
#    if defined(__SSE4_1__)
    return _mm_dp_ps(_a, _b, 0xFF);
#    else
    const sx_simd_t xyzw = sx_simd_mul(_a, _b);
    const sx_simd_t yzwx = sx_simd_swizzle_yzwx(xyzw);
    const sx_simd_t tmp0 = sx_simd_add(xyzw, yzwx);
    const sx_simd_t zwxy = sx_simd_swizzle_zwxy(tmp0);
    return sx_simd_add(tmp0, zwxy);
#    endif    // defined(__SSE4__)
}

SX_SIMD_INLINE sx_simd_t sx_simd_cmpeq(sx_simd_t _a, sx_simd_t _b) {
    sx_simd_t result;
    result.ixyzw[0] = _a.fxyzw[0] == _b.fxyzw[0] ? 0xffffffff : 0x0;
    result.ixyzw[1] = _a.fxyzw[1] == _b.fxyzw[1] ? 0xffffffff : 0x0;
    result.ixyzw[2] = _a.fxyzw[2] == _b.fxyzw[2] ? 0xffffffff : 0x0;
    result.ixyzw[3] = _a.fxyzw[3] == _b.fxyzw[3] ? 0xffffffff : 0x0;
    return result;
}

SX_SIMD_INLINE sx_simd_t sx_simd_cmplt(sx_simd_t _a, sx_simd_t _b) {
    sx_simd_t result;
    result.ixyzw[0] = _a.fxyzw[0] < _b.fxyzw[0] ? 0xffffffff : 0x0;
    result.ixyzw[1] = _a.fxyzw[1] < _b.fxyzw[1] ? 0xffffffff : 0x0;
    result.ixyzw[2] = _a.fxyzw[2] < _b.fxyzw[2] ? 0xffffffff : 0x0;
    result.ixyzw[3] = _a.fxyzw[3] < _b.fxyzw[3] ? 0xffffffff : 0x0;
    return result;
}

SX_SIMD_INLINE sx_simd_t sx_simd_cmple(sx_simd_t _a, sx_simd_t _b) {
    sx_simd_t result;
    result.ixyzw[0] = _a.fxyzw[0] <= _b.fxyzw[0] ? 0xffffffff : 0x0;
    result.ixyzw[1] = _a.fxyzw[1] <= _b.fxyzw[1] ? 0xffffffff : 0x0;
    result.ixyzw[2] = _a.fxyzw[2] <= _b.fxyzw[2] ? 0xffffffff : 0x0;
    result.ixyzw[3] = _a.fxyzw[3] <= _b.fxyzw[3] ? 0xffffffff : 0x0;
    return result;
}

SX_SIMD_INLINE sx_simd_t sx_simd_cmpgt(sx_simd_t _a, sx_simd_t _b) {
    sx_simd_t result;
    result.ixyzw[0] = _a.fxyzw[0] > _b.fxyzw[0] ? 0xffffffff : 0x0;
    result.ixyzw[1] = _a.fxyzw[1] > _b.fxyzw[1] ? 0xffffffff : 0x0;
    result.ixyzw[2] = _a.fxyzw[2] > _b.fxyzw[2] ? 0xffffffff : 0x0;
    result.ixyzw[3] = _a.fxyzw[3] > _b.fxyzw[3] ? 0xffffffff : 0x0;
    return result;
}

SX_SIMD_INLINE sx_simd_t sx_simd_cmpge(sx_simd_t _a, sx_simd_t _b) {
    sx_simd_t result;
    result.ixyzw[0] = _a.fxyzw[0] >= _b.fxyzw[0] ? 0xffffffff : 0x0;
    result.ixyzw[1] = _a.fxyzw[1] >= _b.fxyzw[1] ? 0xffffffff : 0x0;
    result.ixyzw[2] = _a.fxyzw[2] >= _b.fxyzw[2] ? 0xffffffff : 0x0;
    result.ixyzw[3] = _a.fxyzw[3] >= _b.fxyzw[3] ? 0xffffffff : 0x0;
    return result;
}

SX_SIMD_INLINE sx_simd_t sx_simd_min(sx_simd_t _a, sx_simd_t _b) {
    sx_simd_t result;
    result.fxyzw[0] = _a.fxyzw[0] < _b.fxyzw[0] ? _a.fxyzw[0] : _b.fxyzw[0];
    result.fxyzw[1] = _a.fxyzw[1] < _b.fxyzw[1] ? _a.fxyzw[1] : _b.fxyzw[1];
    result.fxyzw[2] = _a.fxyzw[2] < _b.fxyzw[2] ? _a.fxyzw[2] : _b.fxyzw[2];
    result.fxyzw[3] = _a.fxyzw[3] < _b.fxyzw[3] ? _a.fxyzw[3] : _b.fxyzw[3];
    return result;
}

SX_SIMD_INLINE sx_simd_t sx_simd_max(sx_simd_t _a, sx_simd_t _b) {
    sx_simd_t result;
    result.fxyzw[0] = _a.fxyzw[0] > _b.fxyzw[0] ? _a.fxyzw[0] : _b.fxyzw[0];
    result.fxyzw[1] = _a.fxyzw[1] > _b.fxyzw[1] ? _a.fxyzw[1] : _b.fxyzw[1];
    result.fxyzw[2] = _a.fxyzw[2] > _b.fxyzw[2] ? _a.fxyzw[2] : _b.fxyzw[2];
    result.fxyzw[3] = _a.fxyzw[3] > _b.fxyzw[3] ? _a.fxyzw[3] : _b.fxyzw[3];
    return result;
}

SX_SIMD_INLINE sx_simd_t sx_simd_and(sx_simd_t _a, sx_simd_t _b) {
    sx_simd_t result;
    result.uxyzw[0] = _a.uxyzw[0] & _b.uxyzw[0];
    result.uxyzw[1] = _a.uxyzw[1] & _b.uxyzw[1];
    result.uxyzw[2] = _a.uxyzw[2] & _b.uxyzw[2];
    result.uxyzw[3] = _a.uxyzw[3] & _b.uxyzw[3];
    return result;
}

SX_SIMD_INLINE sx_simd_t sx_simd_andc(sx_simd_t _a, sx_simd_t _b) {
    sx_simd_t result;
    result.uxyzw[0] = _a.uxyzw[0] & ~_b.uxyzw[0];
    result.uxyzw[1] = _a.uxyzw[1] & ~_b.uxyzw[1];
    result.uxyzw[2] = _a.uxyzw[2] & ~_b.uxyzw[2];
    result.uxyzw[3] = _a.uxyzw[3] & ~_b.uxyzw[3];
    return result;
}

SX_SIMD_INLINE sx_simd_t sx_simd_or(sx_simd_t _a, sx_simd_t _b) {
    sx_simd_t result;
    result.uxyzw[0] = _a.uxyzw[0] | _b.uxyzw[0];
    result.uxyzw[1] = _a.uxyzw[1] | _b.uxyzw[1];
    result.uxyzw[2] = _a.uxyzw[2] | _b.uxyzw[2];
    result.uxyzw[3] = _a.uxyzw[3] | _b.uxyzw[3];
    return result;
}

SX_SIMD_INLINE sx_simd_t sx_simd_xor(sx_simd_t _a, sx_simd_t _b) {
    sx_simd_t result;
    result.uxyzw[0] = _a.uxyzw[0] ^ _b.uxyzw[0];
    result.uxyzw[1] = _a.uxyzw[1] ^ _b.uxyzw[1];
    result.uxyzw[2] = _a.uxyzw[2] ^ _b.uxyzw[2];
    result.uxyzw[3] = _a.uxyzw[3] ^ _b.uxyzw[3];
    return result;
}
#endif        // SX_SIMD_SSE/NEON

SX_SIMD_INLINE sx_simd_t simd_shuffle_xAzC(sx_simd_t _xyzw, sx_simd_t _ABCD) {
    const sx_simd_t xAyB = sx_simd_shuffle_xAyB(_xyzw, _ABCD);
    const sx_simd_t zCwD = sx_simd_shuffle_zCwD(_xyzw, _ABCD);
    const sx_simd_t result = sx_simd_shuffle_xyAB(xAyB, zCwD);
    return result;
}

SX_SIMD_INLINE sx_simd_t simd_shuffle_yBwD(sx_simd_t _xyzw, sx_simd_t _ABCD) {
    const sx_simd_t xAyB = sx_simd_shuffle_xAyB(_xyzw, _ABCD);
    const sx_simd_t zCwD = sx_simd_shuffle_zCwD(_xyzw, _ABCD);
    const sx_simd_t result = sx_simd_shuffle_zwCD(xAyB, zCwD);
    return result;
}

SX_SIMD_INLINE sx_simd_t sx_simd_madd(sx_simd_t _a, sx_simd_t _b, sx_simd_t _c) {
    const sx_simd_t mul = sx_simd_mul(_a, _b);
    const sx_simd_t result = sx_simd_add(mul, _c);
    return result;
}

SX_SIMD_INLINE sx_simd_t sx_simd_rsqrt(sx_simd_t _a) {
    const sx_simd_t one = sx_simd_splat1(1.0f);
    const sx_simd_t sqrt = sx_simd_sqrt(_a);
    const sx_simd_t result = sx_simd_div(one, sqrt);

    return result;
}

SX_SIMD_INLINE sx_simd_t simd_neg(sx_simd_t _a) {
    const sx_simd_t zero = sx_simd_zero();
    const sx_simd_t result = sx_simd_sub(zero, _a);

    return result;
}

SX_SIMD_INLINE sx_simd_t simd_rcp(sx_simd_t _a) {
    const sx_simd_t one = sx_simd_splat1(1.0f);
    const sx_simd_t result = sx_simd_div(one, _a);

    return result;
}

SX_SIMD_INLINE sx_simd_t simd_clamp(sx_simd_t _a, sx_simd_t _min, sx_simd_t _max) {
    const sx_simd_t tmp = sx_simd_min(_a, _max);
    const sx_simd_t result = sx_simd_max(tmp, _min);

    return result;
}

SX_SIMD_INLINE sx_simd_t simd_lerp(sx_simd_t _a, sx_simd_t _b, sx_simd_t _s) {
    const sx_simd_t ba = sx_simd_sub(_b, _a);
    const sx_simd_t result = sx_simd_madd(_s, ba, _a);

    return result;
}

SX_SIMD_INLINE sx_simd_t simd_normalize3(sx_simd_t _a) {
    const sx_simd_t dot3 = sx_simd_dot3(_a, _a);
    const sx_simd_t invSqrt = sx_simd_rsqrt(dot3);
    const sx_simd_t result = sx_simd_mul(_a, invSqrt);

    return result;
}

SX_SIMD_INLINE sx_simd_t simd_cross3(sx_simd_t _a, sx_simd_t _b) {
    // a.yzx * b.zxy - a.zxy * b.yzx == (a * b.yzx - a.yzx * b).yzx
    const sx_simd_t a_yzxw = sx_simd_swizzle_yzxw(_a);
    const sx_simd_t b_yzxw = sx_simd_swizzle_yzxw(_b);
    const sx_simd_t tmp0 = sx_simd_mul(_a, b_yzxw);
    const sx_simd_t tmp1 = sx_simd_sub(tmp0, sx_simd_mul(a_yzxw, _b));
    const sx_simd_t result = sx_simd_swizzle_yzxw(tmp1);
    return result;
}

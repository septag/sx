//
// Copyright 2018 Sepehr Taghdisian (septag@github). All rights reserved.
// License: https://github.com/septag/sx#license-bsd-2-clause
//
// see math.h for full info
//

#pragma once

#include "math-types.h"

#if defined(__SSE2__) || (SX_COMPILER_MSVC && (SX_ARCH_64BIT || _M_IX86_FP >= 2))
#   include <xmmintrin.h>    // __m128
#endif    //

#if defined(__SSE2__) || (SX_COMPILER_MSVC && (SX_ARCH_64BIT || _M_IX86_FP >= 2))
SX_FORCE_INLINE SX_CONSTFN float sx_sqrt(float _a);
SX_FORCE_INLINE SX_CONSTFN float sx_rsqrt(float _a);
#else
SX_API SX_CONSTFN float sx_sqrt(float _a);
SX_API SX_CONSTFN float sx_rsqrt(float _a);
#endif

SX_API SX_CONSTFN float sx_copysign(float _x, float _y);
SX_API SX_CONSTFN float sx_floor(float _f);
SX_API SX_CONSTFN float sx_cos(float _a);
SX_API SX_CONSTFN float sx_acos(float _a);
SX_API SX_CONSTFN float sx_sin(float _a);
SX_API SX_CONSTFN float sx_asin(float _a);
SX_API SX_CONSTFN float sx_atan2(float _y, float _x);
SX_API SX_CONSTFN float sx_exp(float _a);
SX_API SX_CONSTFN float sx_log(float _a);
SX_FORCE_INLINE SX_CONSTFN int sx_nearest_pow2(int n);
SX_FORCE_INLINE SX_CONSTFN bool sx_ispow2(int n);
SX_FORCE_INLINE SX_CONSTFN float sx_torad(float _deg);
SX_FORCE_INLINE SX_CONSTFN float sx_todeg(float _rad);
SX_FORCE_INLINE SX_CONSTFN uint32_t sx_ftob(float _a);
SX_FORCE_INLINE SX_CONSTFN float sx_btof(unsigned int _a);
SX_FORCE_INLINE SX_CONSTFN uint64_t sx_dtob(double _a);
SX_FORCE_INLINE SX_CONSTFN double sx_btod(uint64_t _a);
SX_FORCE_INLINE SX_CONSTFN uint32_t sx_fflip(unsigned int _value);
SX_FORCE_INLINE SX_CONSTFN bool sx_isnan(float _f);
SX_FORCE_INLINE SX_CONSTFN bool sx_isnand(double _f);
SX_FORCE_INLINE SX_CONSTFN bool sx_isfin(float _f);
SX_FORCE_INLINE SX_CONSTFN bool sx_isfind(double _f);
SX_FORCE_INLINE SX_CONSTFN bool sx_isinf(float _f);
SX_FORCE_INLINE SX_CONSTFN bool sx_isinfd(double _f);
SX_FORCE_INLINE SX_CONSTFN float sx_round(float _f);
SX_FORCE_INLINE SX_CONSTFN float sx_ceil(float _f);
SX_FORCE_INLINE SX_CONSTFN float sx_lerp(float _a, float _b, float _t);
SX_FORCE_INLINE SX_CONSTFN float sx_sign(float _a);
SX_FORCE_INLINE SX_CONSTFN float sx_abs(float _a);
SX_FORCE_INLINE SX_CONSTFN float sx_tan(float _a);
SX_FORCE_INLINE SX_CONSTFN float sx_sinh(float _a);
SX_FORCE_INLINE SX_CONSTFN float sx_cosh(float _a);
SX_FORCE_INLINE SX_CONSTFN float sx_tanh(float _a);
SX_FORCE_INLINE SX_CONSTFN float sx_atan(float _a);
SX_FORCE_INLINE SX_CONSTFN float sx_pow(float _a, float _b);
SX_FORCE_INLINE SX_CONSTFN float sx_exp2(float _a);
SX_FORCE_INLINE SX_CONSTFN float sx_log2(float _a);
SX_FORCE_INLINE SX_CONSTFN float sx_trunc(float _a);
SX_FORCE_INLINE SX_CONSTFN float sx_fract(float _a);
SX_FORCE_INLINE SX_CONSTFN float sx_mod(float _a, float _b);
SX_FORCE_INLINE SX_CONSTFN bool sx_equal(float _a, float _b, float _epsilon);
SX_FORCE_INLINE SX_CONSTFN bool sx_equal_arr(const float* _a, const float* _b, int _num,
                                             float _epsilon);
SX_FORCE_INLINE SX_CONSTFN float sx_wrap(float _a, float _wrap);
SX_FORCE_INLINE SX_CONSTFN float sx_wrap_range(float x, float fmin, float fmax);
SX_FORCE_INLINE SX_CONSTFN int sx_iwrap_range(int x, int imin, int imax);
SX_FORCE_INLINE SX_CONSTFN float sx_step(float _a, float _edge);
SX_FORCE_INLINE SX_CONSTFN float sx_pulse(float _a, float _start, float _end);
SX_FORCE_INLINE SX_CONSTFN float sx_saturate(float _n);
SX_FORCE_INLINE SX_CONSTFN float sx_smoothstep(float _a, float _edge1, float _edge2);
SX_FORCE_INLINE SX_CONSTFN float sx_linearstep(float t, float _min, float _max);
SX_FORCE_INLINE SX_CONSTFN float sx_normalize_time(float t, float _max);
SX_FORCE_INLINE SX_CONSTFN float sx_bias(float _time, float _bias);
SX_FORCE_INLINE SX_CONSTFN float sx_gain(float _time, float _gain);
SX_FORCE_INLINE SX_CONSTFN float sx_angle_diff(float _a, float _b);
SX_FORCE_INLINE SX_CONSTFN float sx_angle_lerp(float _a, float _b, float _t);

////////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(__SSE2__) || (SX_COMPILER_MSVC && (SX_ARCH_64BIT || _M_IX86_FP >= 2))
SX_FORCE_INLINE SX_CONSTFN float sx_sqrt(float x)
{
    float r;
    _mm_store_ss(&r, _mm_sqrt_ss( _mm_load_ss(&x)));
    return r;
}

SX_FORCE_INLINE SX_CONSTFN float sx_rsqrt(float x)
{
    float r;
    _mm_store_ss(&r, _mm_rsqrt_ss(_mm_load_ss(&x)));
    return r;
}
#endif

// https://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2
SX_FORCE_INLINE SX_CONSTFN int sx_nearest_pow2(int n)
{
    n--;
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    n++;
    return n;
}

SX_FORCE_INLINE SX_CONSTFN bool sx_ispow2(int n)
{
    return (n & (n - 1)) == 0;
}

SX_FORCE_INLINE SX_CONSTFN float sx_torad(float _deg)
{
    return _deg * SX_PI / 180.0f;
}

SX_FORCE_INLINE SX_CONSTFN float sx_todeg(float _rad)
{
    return _rad * 180.0f / SX_PI;
}

// Packs float to unsigned int
SX_FORCE_INLINE SX_CONSTFN unsigned int sx_ftob(float _a)
{
    union {
        float f;
        unsigned int ui;
    } u = { _a };
    return u.ui;
}

// Unpacks float from unsigned int
SX_FORCE_INLINE SX_CONSTFN float sx_btof(unsigned int _a)
{
    union {
        unsigned int ui;
        float f;
    } u = { _a };
    return u.f;
}

// Packs double to uint64_t
SX_FORCE_INLINE SX_CONSTFN uint64_t sx_dtob(double _a)
{
    union {
        double f;
        uint64_t ui;
    } u = { _a };
    return u.ui;
}

// Unpacks uint64_t to double
SX_FORCE_INLINE SX_CONSTFN double sx_btod(uint64_t _a)
{
    union {
        uint64_t ui;
        double f;
    } u = { _a };
    return u.f;
}

// Returns sortable bit packed float value
// http://archive.fo/2012.12.08-212402/http://stereopsis.com/radix.html
SX_FORCE_INLINE SX_CONSTFN unsigned int sx_fflip(unsigned int _value)
{
    unsigned int mask = -((int32_t)(_value >> 31)) | 0x80000000;
    return _value ^ mask;
}

SX_FORCE_INLINE SX_CONSTFN bool sx_isnan(float _f)
{
    const unsigned int tmp = sx_ftob(_f) & INT32_MAX;
    return tmp > UINT32_C(0x7f800000);
}

SX_FORCE_INLINE SX_CONSTFN bool sx_isnand(double _f)
{
    const uint64_t tmp = sx_dtob(_f) & INT64_MAX;
    return tmp > UINT64_C(0x7ff0000000000000);
}

SX_FORCE_INLINE SX_CONSTFN bool sx_isfin(float _f)
{
    const unsigned int tmp = sx_ftob(_f) & INT32_MAX;
    return tmp < UINT32_C(0x7f800000);
}

SX_FORCE_INLINE SX_CONSTFN bool sx_isfind(double _f)
{
    const uint64_t tmp = sx_dtob(_f) & INT64_MAX;
    return tmp < UINT64_C(0x7ff0000000000000);
}

SX_FORCE_INLINE SX_CONSTFN bool sx_isinf(float _f)
{
    const unsigned int tmp = sx_ftob(_f) & INT32_MAX;
    return tmp == UINT32_C(0x7f800000);
}

SX_FORCE_INLINE SX_CONSTFN bool sx_isinfd(double _f)
{
    const uint64_t tmp = sx_dtob(_f) & INT64_MAX;
    return tmp == UINT64_C(0x7ff0000000000000);
}

SX_FORCE_INLINE SX_CONSTFN float sx_round(float _f)
{
    return sx_floor(_f + 0.5f);
}

SX_FORCE_INLINE SX_CONSTFN float sx_ceil(float _f)
{
    return -sx_floor(-_f);
}

SX_FORCE_INLINE SX_CONSTFN float sx_lerp(float _a, float _b, float _t)
{
    // this version is more precise than: _a + (_b - _a) * _t
    return (1.0f - _t) * _a + _t * _b;
}

SX_FORCE_INLINE SX_CONSTFN float sx_sign(float _a)
{
    return _a < 0.0f ? -1.0f : 1.0f;
}

SX_FORCE_INLINE SX_CONSTFN float sx_abs(float _a)
{
    union {
        float f;
        unsigned int ui;
    } u = { _a };
    u.ui &= 0x7FFFFFFF;
    return u.f;
}

SX_FORCE_INLINE SX_CONSTFN float sx_tan(float _a)
{
    return sx_sin(_a) / sx_cos(_a);
}

SX_FORCE_INLINE SX_CONSTFN float sx_sinh(float _a)
{
    return 0.5f * (sx_exp(_a) - sx_exp(-_a));
}

SX_FORCE_INLINE SX_CONSTFN float sx_cosh(float _a)
{
    return 0.5f * (sx_exp(_a) + sx_exp(-_a));
}

SX_FORCE_INLINE SX_CONSTFN float sx_tanh(float _a)
{
    const float tmp0 = sx_exp(2.0f * _a);
    const float tmp1 = tmp0 - 1.0f;
    const float tmp2 = tmp0 + 1.0f;
    const float result = tmp1 / tmp2;

    return result;
}

SX_FORCE_INLINE SX_CONSTFN float sx_atan(float _a)
{
    return sx_atan2(_a, 1.0f);
}

SX_FORCE_INLINE SX_CONSTFN float sx_pow(float _a, float _b)
{
    return sx_exp(_b * sx_log(_a));
}

SX_FORCE_INLINE SX_CONSTFN float sx_exp2(float _a)
{
    return sx_pow(2.0f, _a);
}

SX_FORCE_INLINE SX_CONSTFN float sx_log2(float _a)
{
    return sx_log(_a) * SX_INVLOG_NAT2;
}

// Returns the nearest integer not greater in magnitude than _a.
SX_FORCE_INLINE SX_CONSTFN float sx_trunc(float _a)
{
    return (float)((int)_a);
}

// Returns the fractional (or decimal) part of _a, which is 0~1
SX_FORCE_INLINE SX_CONSTFN float sx_fract(float _a)
{
    return _a - sx_trunc(_a);
}

// Returns the floating-point remainder of the division operation _a/_b.
SX_FORCE_INLINE SX_CONSTFN float sx_mod(float _a, float _b)
{
    return _a - _b * sx_floor(_a / _b);
}

// http://realtimecollisiondetection.net/blog/?t=89
SX_PRAGMA_DIAGNOSTIC_PUSH()
SX_PRAGMA_DIAGNOSTIC_IGNORED_CLANG_GCC("-Wshadow")
SX_FORCE_INLINE SX_CONSTFN bool sx_equal(float _a, float _b, float _epsilon)
{
    const float lhs = sx_abs(_a - _b);
    float aa = sx_abs(_a);
    float ab = sx_abs(_b);
    const float rhs = _epsilon * sx_max(1.0f, sx_max(aa, ab));
    return lhs <= rhs;
}
SX_PRAGMA_DIAGNOSTIC_POP()

SX_FORCE_INLINE SX_CONSTFN bool sx_equal_arr(const float* _a, const float* _b, int _num,
                                           float _epsilon)
{
    bool result = sx_equal(_a[0], _b[0], _epsilon);
    for (int ii = 1; result && ii < _num; ++ii) {
        result = sx_equal(_a[ii], _b[ii], _epsilon);
    }
    return result;
}

SX_FORCE_INLINE SX_CONSTFN float sx_wrap(float _a, float _wrap)
{
    const float tmp0 = sx_mod(_a, _wrap);
    const float result = tmp0 < 0.0f ? _wrap + tmp0 : tmp0;
    return result;
}

SX_FORCE_INLINE SX_CONSTFN float sx_wrap_range(float x, float fmin, float fmax)
{
    return sx_mod(x, fmax - fmin) + fmin;
}

SX_FORCE_INLINE SX_CONSTFN int sx_iwrap_range(int x, int imin, int imax)
{
    int range = imax - imin + 1;
    if (x < imin)
        x += range * ((imin - x) / range + 1);
    return imin + (x - imin) % range;
}

// Returns 0 if _a < _edge, else 1
SX_FORCE_INLINE SX_CONSTFN float sx_step(float _a, float _edge)
{
    return _a < _edge ? 0.0f : 1.0f;
}

SX_FORCE_INLINE SX_CONSTFN float sx_pulse(float _a, float _start, float _end)
{
    return sx_step(_a, _start) - sx_step(_a, _end);
}

SX_FORCE_INLINE SX_CONSTFN float sx_saturate(float _n)
{
    return sx_clamp(_n, 0.0f, 1.0f);
}

// smooth Hermite interpolation (result = 0..1) when edge0 < x < edge1
SX_FORCE_INLINE SX_CONSTFN float sx_smoothstep(float _a, float _edge1, float _edge2)
{
    sx_assert(_edge1 < _edge2);
    float a = sx_saturate((_a - _edge1) / (_edge2 - _edge1));
    return a * a * (3.0f - 2.0f * a);
}

// like smoothstep but linear
// result is 0..1 when in _min.._max range, 0 if less than _min, 1 if more than _max
SX_FORCE_INLINE SX_CONSTFN float sx_linearstep(float t, float _min, float _max)
{
    sx_assert(_min < _max);
    return sx_saturate((t - _min) / (_max - _min));
}

// used for normalizing time values to 0..1
// based on the assumption that time 't' starts from 0.._max or more than that
SX_FORCE_INLINE SX_CONSTFN float sx_normalize_time(float t, float _max)
{
    sx_assert(_max > 0);
    float nt = t / _max;
    return nt < 1.0f ? nt : 1.0f;
}


// References:
//  - Bias And Gain Are Your Friend
//    http://blog.demofox.org/2012/09/24/bias-and-gain-are-your-friend/
//  - http://demofox.org/biasgain.html
SX_FORCE_INLINE SX_CONSTFN float sx_bias(float _time, float _bias)
{
    return _time / (((1.0f / _bias - 2.0f) * (1.0f - _time)) + 1.0f);
}

SX_FORCE_INLINE SX_CONSTFN float sx_gain(float _time, float _gain)
{
    if (_time < 0.5f)
        return sx_bias(_time * 2.0f, _gain) * 0.5f;

    return sx_bias(_time * 2.0f - 1.0f, 1.0f - _gain) * 0.5f + 0.5f;
}

SX_FORCE_INLINE SX_CONSTFN float sx_angle_diff(float _a, float _b)
{
    const float dist = sx_wrap(_b - _a, SX_PI2);
    return sx_wrap(dist * 2.0f, SX_PI2) - dist;
}

SX_FORCE_INLINE SX_CONSTFN float sx_angle_lerp(float _a, float _b, float _t)
{
    return _a + sx_angle_diff(_a, _b) * _t;
}





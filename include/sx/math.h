//
// Copyright 2018 Sepehr Taghdisian (septag@github). All rights reserved.
// License: https://github.com/septag/sx#license-bsd-2-clause
//
// math.h - 1.1.1  Scalar and Vector math functions
//                 Contains vector primitives and vector/fpu math functions, event functions implemented in libm
//                 Many functions are from bx library (https://github.com/bkaradzic/bx)
// Easings:
//      Reference: https://easings.net/
//                 https://github.com/r-lyeh-archived/tween
//  
// Conventions:
//      The lib prefers Right-Handed system, although there are functions for both LH or RH system for calulating view/projection matrices
//      Rotations are CCW
//      Matrices are Column-Major, but the representation is row-major, which means:
//          mat->m[r][c] -> which R is the row, and C is column index 
//          transform vector (v) by matrix (M) = M.v
//      
//
#pragma once

#ifndef SX_MATH_H_
#define SX_MATH_H_

#include "sx.h"

// Some math constants
SX_API const float SX_PI;
SX_API const float SX_PI2;
SX_API const float SX_INVPI;
SX_API const float SX_PIHALF;
SX_API const float SX_PIQUARTER;
SX_API const float SX_E;
SX_API const float SX_FLOAT_MAX;
SX_API const float SX_FLOAT_MIN;
SX_API const float SX_LOG_NAT10;
SX_API const float SX_INVLOG_NAT2;
SX_API const float SX_LOG_NAT2H;
SX_API const float SX_LOG_NAT2L;
SX_API const float SX_NEAR_ZERO;
SX_API const float SX_SQRT2;

////////////////////////////////////////////////////////////////////////////////////////////////////
// Types/Primitives
typedef union sx_vec4
{
    struct
    {
        float x;
        float y;
        float z;
        float w;
    };

    float f[4];
} sx_vec4;

typedef union sx_vec2
{
    struct
    {
        float x;
        float y;
    };

    float f[2];
} sx_vec2;

typedef union sx_vec3
{
    struct
    {
        float x;
        float y;
        float z;
    };

    float f[3];
} sx_vec3;

typedef union sx_color
{
    struct
    {
        uint8_t r;
        uint8_t g;
        uint8_t b;
        uint8_t a;
    };

    uint32_t n;
} sx_color;


typedef union sx_ivec2
{
    struct
    {
        int x;
        int y;
    };

    int n[2];
} sx_ivec2;

typedef union sx_quat
{
    struct
    {
        float x;
        float y;
        float z;
        float w;
    };

    float f[4];
} sx_quat;

typedef union sx_mat3
{
    struct
    {
        float m11, m21, m31;
        float m12, m22, m32;
        float m13, m23, m33;
    };

    struct
    {
        float fc1[3];
        float fc2[3];
        float fc3[3];
    };

    struct
    {
        sx_vec3 col1;
        sx_vec3 col2;
        sx_vec3 col3;
    };

    float f[9];
} sx_mat3;

typedef union sx_mat4
{
    struct
    {
        float m11, m21, m31, m41;
        float m12, m22, m32, m42;
        float m13, m23, m33, m43;
        float m14, m24, m34, m44;
    };

    struct
    {
        float rc1[4];
        float rc2[4];
        float rc3[4];
        float rc4[4];
    };

    struct
    {
        sx_vec4 col1;
        sx_vec4 col2;
        sx_vec4 col3;
        sx_vec4 col4;
    };

    float f[16];
} sx_mat4;


#if SX_CONFIG_STDMATH
#include <math.h>

SX_INLINE SX_CONSTFN float sx_floor(float _f)
{
    return floorf(_f);
}

SX_INLINE SX_CONSTFN float sx_cos(float _a)
{
    return cosf(_a);
}

SX_INLINE SX_CONSTFN float sx_acos(float _a)
{
    return acosf(_a);
}

SX_INLINE SX_CONSTFN float sx_atan2(float _y, float _x)
{
    return atan2f(_y, _x);
}

SX_INLINE SX_CONSTFN float sx_exp(float _a)
{
    return expf(_a);
}

SX_INLINE SX_CONSTFN float sx_log(float _a)
{
    return logf(_a);
}
#else
#   ifdef __cplusplus
extern "C" {
#   endif

SX_CONSTFN float sx_floor(float _f);
SX_CONSTFN float sx_cos(float _a);
SX_CONSTFN float sx_acos(float _a);
SX_CONSTFN float sx_atan2(float _y, float _x);
SX_CONSTFN float sx_exp(float _a);
SX_CONSTFN float sx_log(float _a);

#   ifdef __cplusplus
}
#   endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

sx_vec3 sx_vec3_calc_normal(const sx_vec3 _va, const sx_vec3 _vb, const sx_vec3 _vc);
sx_vec4 sx_vec3_calc_plane(const sx_vec3 _va, const sx_vec3 _vb, const sx_vec3 _vc);
sx_vec2 sx_vec2_calc_linearfit2D(const sx_vec2* _points, int _num);
sx_vec3 sx_vec3_calc_linearfit3D(const sx_vec3* _points, int _num);

sx_mat4 sx_mat4_quat(const sx_quat _quat);
sx_mat4 sx_mat4_from_normal(const sx_vec3 _normal, float _scale, const sx_vec3 _pos);
sx_mat4 sx_mat4_from_normal_angle(const sx_vec3 _normal, float _scale, const sx_vec3 _pos, float _angle);
sx_mat4 sx_mat4_view_lookat(const sx_vec3 eye, const sx_vec3 target, const sx_vec3 up);
sx_mat4 sx_mat4_view_lookatLH(const sx_vec3 eye, const sx_vec3 target, const sx_vec3 up);
// https://www.3dgep.com/understanding-the-view-matrix/
sx_mat4 sx_mat4_view_FPS(const sx_vec3 eye, float pitch, float yaw);
sx_mat4 sx_mat4_view_arcball(const sx_vec3 move, const sx_quat rot, const sx_vec3 target_pos);
// https://docs.microsoft.com/en-us/windows/desktop/direct3d9/d3dxmatrixperspectivefovlh
sx_mat4 sx_mat4_perspective(float width, float height, float zn, float zf, bool ogl_ndc);
sx_mat4 sx_mat4_perspectiveLH(float width, float height, float zn, float zf, bool ogl_ndc);
sx_mat4 sx_mat4_perspective_offcenter(float xmin, float ymin, float xmax, float ymax, float zn, float zf, bool ogl_ndc);
sx_mat4 sx_mat4_perspective_offcenterLH(float xmin, float ymin, float xmax, float ymax, float zn, float zf, bool ogl_ndc);
sx_mat4 sx_mat4_perspectiveFOV(float fov_y, float aspect, float zn, float zf, bool ogl_ndc);
sx_mat4 sx_mat4_perspectiveFOV_LH(float fov_y, float aspect, float zn, float zf, bool ogl_ndc);
sx_mat4 sx_mat4_ortho(float width, float height, float zn, float zf, float offset, bool ogl_ndc);
sx_mat4 sx_mat4_orthoLH(float width, float height, float zn, float zf, float offset, bool ogl_ndc);
sx_mat4 sx_mat4_ortho_offcenter(float xmin, float ymin, float xmax, float ymax, float zn, float zf, float offset, bool ogl_ndc);
sx_mat4 sx_mat4_ortho_offcenterLH(float xmin, float ymin, float xmax, float ymax, float zn, float zf, float offset, bool ogl_ndc);
sx_mat4 sx_mat4_SRT(float _sx, float _sy, float _sz, float _ax, float _ay, float _az, float _tx, float _ty, float _tz);
sx_mat4 sx_mat4_mul(const sx_mat4* _a, const sx_mat4* _b);
sx_mat4 sx_mat4_inv(const sx_mat4* _a);
/// Inverse for transform-only matrices (column4=0) (mat4x)
sx_mat4 sx_mat4x_inv(const sx_mat4* _a);
sx_quat sx_mat4_calc_quat(const sx_mat4* _mat);

sx_mat3 sx_mat3_inv(const sx_mat3* _a);
sx_mat3 sx_mat3_mul(const sx_mat3* _a, const sx_mat3* _b);
void sx_color_RGBtoHSV(float _hsv[3], const float _rgb[3]);
void sx_color_HSVtoRGB(float _rgb[3], const float _hsv[3]);

#ifdef __cplusplus
}
#endif

// https://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2
SX_INLINE SX_CONSTFN int sx_nearest_pow2(int n)
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

SX_INLINE SX_CONSTFN bool sx_ispow2(int n) 
{
    return (n & (n - 1)) == 0;
}

SX_INLINE SX_CONSTFN float sx_torad(float _deg)
{
    return _deg * SX_PI / 180.0f;
}

SX_INLINE SX_CONSTFN float sx_todeg(float _rad)
{
    return _rad * 180.0f / SX_PI;
}

// Packs float to uint32_t
SX_INLINE SX_CONSTFN uint32_t sx_ftob(float _a)
{
    union { float f; uint32_t ui; } u = { _a };
    return u.ui;    
}

// Unpacks float from uint32_t
SX_INLINE SX_CONSTFN float sx_btof(uint32_t _a)
{
    union { uint32_t ui; float f; } u = { _a };
    return u.f;    
}

// Packs double to uint64_t
SX_INLINE SX_CONSTFN uint64_t sx_dtob(double _a)
{
    union { double f; uint64_t ui; } u = { _a };
    return u.ui;    
}

// Unpacks uint64_t to double
SX_INLINE SX_CONSTFN double sx_btod(uint64_t _a)
{
    union { uint64_t ui; double f; } u = { _a };
    return u.f;    
}

// Returns sortable bit packed float value
// http://archive.fo/2012.12.08-212402/http://stereopsis.com/radix.html
SX_INLINE SX_CONSTFN uint32_t sx_fflip(uint32_t _value)
{
	uint32_t mask = -((int32_t)(_value >> 31)) | 0x80000000;
	return _value ^ mask;    
}

SX_INLINE SX_CONSTFN bool sx_isnan(float _f)
{
	const uint32_t tmp = sx_ftob(_f) & INT32_MAX;
	return tmp > UINT32_C(0x7f800000);
}

SX_INLINE SX_CONSTFN bool sx_isnand(double _f)
{
    const uint64_t tmp = sx_dtob(_f) & INT64_MAX;
    return tmp > UINT64_C(0x7ff0000000000000);
}

SX_INLINE SX_CONSTFN bool sx_isfin(float _f)
{
    const uint32_t tmp = sx_ftob(_f) & INT32_MAX;
    return tmp < UINT32_C(0x7f800000);
}

SX_INLINE SX_CONSTFN bool sx_isfind(double _f)
{
    const uint64_t tmp = sx_dtob(_f) & INT64_MAX;
    return tmp < UINT64_C(0x7ff0000000000000);
}

SX_INLINE SX_CONSTFN bool sx_isinf(float _f)
{
    const uint32_t tmp = sx_ftob(_f) & INT32_MAX;
    return tmp == UINT32_C(0x7f800000);
}

SX_INLINE SX_CONSTFN bool sx_isinfd(double _f)
{
    const uint64_t tmp = sx_dtob(_f) & INT64_MAX;
    return tmp == UINT64_C(0x7ff0000000000000);
}

SX_INLINE SX_CONSTFN float sx_round(float _f)
{
    return sx_floor(_f + 0.5f);
}

SX_INLINE SX_CONSTFN float sx_ceil(float _f)
{
    return -sx_floor(-_f);
}

SX_INLINE SX_CONSTFN float sx_lerp(float _a, float _b, float _t)
{
    return _a + (_b - _a) * _t;
}

SX_INLINE SX_CONSTFN float sx_sign(float _a)
{
	return _a < 0.0f ? -1.0f : 1.0f;
}

SX_INLINE SX_CONSTFN float sx_abs(float _a)
{
    union { float f; uint32_t ui; } u = { _a };
    u.ui &= 0x7FFFFFFF;
    return u.f;
}

SX_INLINE SX_CONSTFN float sx_sin(float _a)
{
    return sx_cos(_a - SX_PIHALF);
}

SX_INLINE SX_CONSTFN float sx_sinh(float _a)
{
    return 0.5f*(sx_exp(_a) - sx_exp(-_a));
}

SX_INLINE SX_CONSTFN float sx_asin(float _a)
{
    return SX_PIHALF - sx_acos(_a);
}

SX_INLINE SX_CONSTFN float sx_cosh(float _a)
{
    return 0.5f*(sx_exp(_a) + sx_exp(-_a) );
}

SX_INLINE SX_CONSTFN float sx_tan(float _a)
{
    return sx_sin(_a) / sx_cos(_a);
}

SX_INLINE SX_CONSTFN float sx_tanh(float _a)
{
    const float tmp0   = sx_exp(2.0f*_a);
    const float tmp1   = tmp0 - 1.0f;
    const float tmp2   = tmp0 + 1.0f;
    const float result = tmp1 / tmp2;

    return result;
}

SX_INLINE SX_CONSTFN float sx_atan(float _a)
{
    return sx_atan2(_a, 1.0f);
}

SX_INLINE SX_CONSTFN float sx_pow(float _a, float _b)
{
    return sx_exp(_b * sx_log(_a));
}

SX_INLINE SX_CONSTFN float sx_exp2(float _a)
{
    return sx_pow(2.0f, _a);
}

SX_INLINE SX_CONSTFN float sx_log2(float _a)
{
    return sx_log(_a) * SX_INVLOG_NAT2;
}

#if SX_CONFIG_STDMATH
SX_INLINE SX_CONSTFN float sx_sqrt(float _a)
{
    return sqrtf(_a);
}

SX_INLINE SX_CONSTFN float sx_rsqrt(float _a)
{
    return 1.0f/sqrtf(_a);
}
#else
// Reference: http://en.wikipedia.org/wiki/Fast_inverse_square_root
SX_INLINE SX_CONSTFN float sx_rsqrt(float _a)
{
    union { float f; uint32_t ui; } u = { _a };
    float y, r;

    y = _a * 0.5f;
    u.ui = 0x5F3759DF - (u.ui >> 1);
    r = u.f;
    r = r * (1.5f - (r * r * y));

    return r;
}

SX_INLINE SX_CONSTFN float sx_sqrt(float _a)
{
    sx_assert (_a >= SX_NEAR_ZERO);
    return 1.0f/sx_rsqrt(_a);
}
#endif


// Returns the nearest integer not greater in magnitude than _a.
SX_INLINE SX_CONSTFN float sx_trunc(float _a)
{
    return (float)((int)_a);
}

// Returns the fractional (or decimal) part of _a, which is 0~1
SX_INLINE SX_CONSTFN float sx_fract(float _a)
{
    return _a - sx_trunc(_a);
}

// Returns the floating-point remainder of the division operation _a/_b.
SX_INLINE SX_CONSTFN float sx_mod(float _a, float _b)
{
    return _a - _b * sx_floor(_a / _b);
}

// http://realtimecollisiondetection.net/blog/?t=89
SX_INLINE SX_CONSTFN bool sx_equal(float _a, float _b, float _epsilon)
{
    const float lhs = sx_abs(_a - _b);
    float aa = sx_abs(_a);
    float ab = sx_abs(_b);
    const float rhs = _epsilon * sx_max(1.0f, sx_max(aa, ab));
    return lhs <= rhs;
}

SX_INLINE SX_CONSTFN bool sx_equal_arr(const float* _a, const float* _b, int _num, float _epsilon)
{
    bool result = sx_equal(_a[0], _b[0], _epsilon);
    for (int ii = 1; result && ii < _num; ++ii) {
        result = sx_equal(_a[ii], _b[ii], _epsilon);
    }
    return result;
}

SX_INLINE SX_CONSTFN float sx_wrap(float _a, float _wrap)
{
    const float tmp0   = sx_mod(_a, _wrap);
    const float result = tmp0 < 0.0f ? _wrap + tmp0 : tmp0;
    return result;    
}

// Returns 0 if _a < _edge, else 1
SX_INLINE SX_CONSTFN float sx_step(float _a, float _edge)
{
    return _a < _edge ? 0.0f : 1.0f;
}

SX_INLINE SX_CONSTFN float sx_pulse(float _a, float _start, float _end)
{
    return sx_step(_a, _start) - sx_step(_a, _end);
}

SX_INLINE SX_CONSTFN float sx_saturate(float _n)
{
    return sx_clamp(_n, 0.0f, 1.0f);
}

// smooth Hermite interpolation (result = 0..1) when edge0 < x < edge1
SX_INLINE SX_CONSTFN float sx_smoothstep(float _a, float _edge1, float _edge2)
{
    sx_assert(_edge1 < _edge2);
    float a = sx_saturate((_a - _edge1)/(_edge2 - _edge1));
    return a*a*(3.0f - 2.0f*a);
}

// like smoothstep but linear
// result is 0..1 when in _min.._max range, 0 if less than _min, 1 if more than _max
SX_INLINE SX_CONSTFN float sx_linearstep(float t, float _min, float _max)
{
    sx_assert(_min < _max);
    return sx_saturate((t - _min)/(_max - _min));
}

// used for normalizing time values to 0..1
// based on the assumption that time 't' starts from 0.._max or more than that
SX_INLINE SX_CONSTFN float sx_normalize_time(float t, float _max)
{
    sx_assert(_max > 0);
    float nt = t / _max;
    return nt < 1.0f ? nt : 1.0f;
}


// References:
//  - Bias And Gain Are Your Friend
//    http://blog.demofox.org/2012/09/24/bias-and-gain-are-your-friend/
//  - http://demofox.org/biasgain.html
SX_INLINE SX_CONSTFN float sx_bias(float _time, float _bias)
{
    return _time / ( ( (1.0f/_bias - 2.0f)*(1.0f - _time) ) + 1.0f);
}

SX_INLINE SX_CONSTFN float sx_gain(float _time, float _gain)
{
    if (_time < 0.5f) 
        return sx_bias(_time * 2.0f, _gain) * 0.5f;

    return sx_bias(_time * 2.0f - 1.0f, 1.0f - _gain) * 0.5f + 0.5f;
}

SX_INLINE SX_CONSTFN float sx_angle_diff(float _a, float _b)
{
    const float dist = sx_wrap(_b - _a, SX_PI2);
	return sx_wrap(dist*2.0f, SX_PI2) - dist;
}

SX_INLINE SX_CONSTFN float sx_angle_lerp(float _a, float _b, float _t)
{
    return _a + sx_angle_diff(_a, _b) * _t;
}

//
SX_INLINE sx_vec3 sx_vec3f(float _x, float _y, float _z) 
{
#ifdef __cplusplus
    return {{_x, _y, _z}};
#else
    return (sx_vec3){.x = _x, .y = _y, .z = _z};
#endif    
}

SX_INLINE sx_vec3 sx_vec3fv(const float* _f)
{
#ifdef __cplusplus
    return {{_f[0], _f[1], _f[2]}};
#else
    return (sx_vec3){.x = _f[0], .y = _f[1], .z = _f[2]};
#endif    
}

SX_INLINE sx_vec3 sx_vec3v2(const sx_vec2 _vec, float _z)
{
    return sx_vec3f(_vec.x, _vec.y, _z);
}

SX_INLINE sx_vec3 sx_vec3splat(float _f)
{
    return sx_vec3f(_f, _f, _f);
}

//
SX_INLINE sx_vec2 sx_vec2f(float _x, float _y) 
{
#ifdef __cplusplus
    return {{_x, _y}};
#else
    return (sx_vec2){.x = _x, .y = _y};
#endif    
}

SX_INLINE sx_vec2 sx_vec2fv(const float* _f)
{
#ifdef __cplusplus
    return {{_f[0], _f[1]}};
#else
    return (sx_vec2){.x = _f[0], .y = _f[1]};
#endif    
}

SX_INLINE sx_vec2 sx_vec2splat(float _f)
{
    return sx_vec2f(_f, _f);
}

//
SX_INLINE sx_vec4 sx_vec4f(float _x, float _y, float _z, float _w) 
{
#ifdef __cplusplus
    return {{_x, _y, _z, _w}};
#else
    return (sx_vec4){.x = _x, .y = _y, .z = _z, .w = _w};
#endif    
}

SX_INLINE sx_vec4 sx_vec4fv(const float* _f)
{
#ifdef __cplusplus
    return {{_f[0], _f[1], _f[2], _f[3]}};
#else
    return (sx_vec4){.x = _f[0], .y = _f[1], .z = _f[2], .w = _f[3]};
#endif    
}

SX_INLINE sx_vec4 sx_vec4v3(const sx_vec3 _vec, float _w)
{
    return sx_vec4f(_vec.x, _vec.y, _vec.z, _w);
}

SX_INLINE sx_vec4 sx_vec4splat(float _f)
{
    return sx_vec4f(_f, _f, _f, _f);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Quaternion
SX_INLINE sx_quat sx_quat4f(float _x, float _y, float _z, float _w)
{
#ifdef __cplusplus
    return {{_x, _y, _z, _w}};
#else
    return (sx_quat){.x=_x, .y=_y, .z=_z, .w=_w};
#endif    
}

SX_INLINE sx_quat sx_quatfv(const float* _f)
{
#ifdef __cplusplus
    return {{_f[0], _f[1], _f[2]}};
#else
    return (sx_quat){.x=_f[0], .y=_f[1], .z=_f[2], .w=_f[3]};
#endif    
}

SX_INLINE sx_quat sx_quat_ident()
{
    return sx_quat4f(0, 0, 0, 1.0f);
}

SX_INLINE sx_vec3 sx_quat_mulXYZ(const sx_quat _qa, const sx_quat _qb)
{
    const float ax = _qa.x;
    const float ay = _qa.y;
    const float az = _qa.z;
    const float aw = _qa.f[3];

    const float bx = _qb.x;
    const float by = _qb.y;
    const float bz = _qb.z;
    const float bw = _qb.w;

    return sx_vec3f(aw * bx + ax * bw + ay * bz - az * by,
                    aw * by - ax * bz + ay * bw + az * bx,
                    aw * bz + ax * by - ay * bx + az * bw);
}

SX_INLINE sx_quat sx_quat_mul(const sx_quat _qa, const sx_quat _qb)
{
    const float ax = _qa.x;
    const float ay = _qa.y;
    const float az = _qa.z;
    const float aw = _qa.w;

    const float bx = _qb.x;
    const float by = _qb.y;
    const float bz = _qb.z;
    const float bw = _qb.w;

    return sx_quat4f(aw * bx + ax * bw + ay * bz - az * by,
                     aw * by - ax * bz + ay * bw + az * bx,
                     aw * bz + ax * by - ay * bx + az * bw,
                     aw * bw - ax * bx - ay * by - az * bz);
}

SX_INLINE sx_quat sx_quat_inv(const sx_quat _quat)
{
    return sx_quat4f(-_quat.x, -_quat.y, -_quat.z, _quat.w);
}

SX_INLINE float sx_quat_dot(const sx_quat _a, const sx_quat _b)
{
    return _a.x*_b.x + _a.y*_b.y + _a.z*_b.z + _a.w*_b.w;
}

SX_INLINE sx_quat sx_quat_norm(const sx_quat _quat)
{
    const float norm = sx_quat_dot(_quat, _quat);
    if (0.0f < norm) {
        const float inv_norm = sx_rsqrt(norm);
        return sx_quat4f(_quat.x * inv_norm, _quat.y * inv_norm, 
                         _quat.z * inv_norm, _quat.w * inv_norm);
    } else {
        sx_assert(0 && "Divide by zero");
        return sx_quat_ident();
    }
}

SX_INLINE sx_vec3 sx_quat_toeuler(const sx_quat _quat)
{
    const float x = _quat.x;
    const float y = _quat.y;
    const float z = _quat.z;
    const float w = _quat.w;

    const float yy = y * y;
    const float zz = z * z;

    const float xx = x * x;
    return sx_vec3f(sx_atan2(2.0f * (x * w - y * z), 1.0f - 2.0f * (xx + zz)),
                    sx_atan2(2.0f * (y * w + x * z), 1.0f - 2.0f * (yy + zz)),
                    sx_asin(2.0f * (x * y + z * w)));
}

SX_INLINE sx_quat sx_quat_rotateaxis(const sx_vec3 _axis, float _angle)
{
    const float ha = _angle * 0.5f;
    const float ca = sx_cos(ha);
    const float sa = sx_sin(ha);
    return sx_quat4f(_axis.x * sa, _axis.y * sa, _axis.z * sa, ca);
}

SX_INLINE sx_quat sx_quat_rotateX(float _ax)
{
    const float hx = _ax * 0.5f;
    const float cx = sx_cos(hx);
    const float sx = sx_sin(hx);
    return sx_quat4f(sx, 0.0f, 0.0f, cx);
}

SX_INLINE sx_quat sx_quat_rotateY(float _ay)
{
    const float hy = _ay * 0.5f;
    const float cy = sx_cos(hy);
    const float sy = sx_sin(hy);
    return sx_quat4f(0.0f, sy, 0.0f, cy);
}

SX_INLINE sx_quat sx_quat_rotateZ(float _az)
{
    const float hz = _az * 0.5f;
    const float cz = sx_cos(hz);
    const float sz = sx_sin(hz);
    return sx_quat4f(0.0f, 0.0f, sz, cz);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Vec3
SX_INLINE sx_vec3 sx_vec3_abs(const sx_vec3 _a)
{
    return sx_vec3f(sx_abs(_a.x), sx_abs(_a.y), sx_abs(_a.z));
}

SX_INLINE sx_vec3 sx_vec3_neg(const sx_vec3 _a)
{
    return sx_vec3f(-_a.x, -_a.y, -_a.z);
}

SX_INLINE sx_vec3 sx_vec3_add(const sx_vec3 _a, const sx_vec3 _b)
{
    return sx_vec3f(_a.x + _b.x,  _a.y + _b.y,  _a.z + _b.z);
}

SX_INLINE sx_vec3 sx_vec3_addf(const sx_vec3 _a, float _b)
{
    return sx_vec3f(_a.x + _b, _a.y + _b, _a.z + _b);
}

SX_INLINE sx_vec3 sx_vec3_sub(const sx_vec3 _a, const sx_vec3 _b)
{
    return sx_vec3f(_a.x - _b.x, _a.y - _b.y, _a.z - _b.z);
}

SX_INLINE sx_vec3 sx_vec3_subf(const sx_vec3 _a, float _b)
{
    return sx_vec3f(_a.x - _b, _a.y - _b, _a.z - _b);
}

SX_INLINE sx_vec3 sx_vec3_mul(const sx_vec3 _a, const sx_vec3 _b)
{
    return sx_vec3f(_a.x * _b.x, _a.y * _b.y, _a.z * _b.z);
}

SX_INLINE sx_vec3 sx_vec3_mulf(const sx_vec3 _a, float _b)
{
    return sx_vec3f(_a.x * _b, _a.y * _b, _a.z * _b);
}

SX_INLINE float sx_vec3_dot(const sx_vec3 _a, const sx_vec3 _b)
{
    return _a.x*_b.x + _a.y*_b.y + _a.z*_b.z;
}

SX_INLINE sx_vec3 sx_vec3_cross(const sx_vec3 _a, const sx_vec3 _b)
{
    return sx_vec3f(_a.y*_b.z - _a.z*_b.y, 
                    _a.z*_b.x - _a.x*_b.z, 
                    _a.x*_b.y - _a.y*_b.x);
}

SX_INLINE float sx_vec3_len(const sx_vec3 _a)
{
    return sx_sqrt(sx_vec3_dot(_a, _a));
}

SX_INLINE sx_vec3 sx_vec3_lerp(const sx_vec3 _a, const sx_vec3 _b, float _t)
{
    return sx_vec3f(sx_lerp(_a.x, _b.x, _t),
                    sx_lerp(_a.y, _b.y, _t),
                    sx_lerp(_a.z, _b.z, _t));
}

SX_INLINE sx_vec3 sx_vec3_norm(const sx_vec3 _a, float* _outlen)
{
    const float len = sx_vec3_len(_a);
    if (len != 0.0f) {
        const float invlen = 1.0f / len;
        if (_outlen)
            *_outlen = len;
        return sx_vec3f(_a.x*invlen, _a.y*invlen, _a.z*invlen);
    } else {
        sx_assert(0 && "Divide by zero");
        return sx_vec3f(0.0f, 0.0f, 0.0f);
    }
}

SX_INLINE sx_vec3 sx_vec3_min(const sx_vec3 _a, const sx_vec3 _b)
{
    return sx_vec3f(sx_min(_a.x, _b.x), sx_min(_a.y, _b.y), sx_min(_a.z, _b.z));
}

SX_INLINE sx_vec3 sx_vec3_max(const sx_vec3 _a, const sx_vec3 _b)
{
    return sx_vec3f(sx_max(_a.x, _b.x), sx_max(_a.y, _b.y), sx_max(_a.z, _b.z));
}

SX_INLINE sx_vec3 sx_vec3_rcp(const sx_vec3 _a)
{
    return sx_vec3f(1.0f/_a.x, 1.0f / _a.y, 1.0f / _a.z);
}

SX_INLINE void sx_vec3_tangent(sx_vec3* _t, sx_vec3* _b, const sx_vec3 _n)
{
    const float nx = _n.x;
    const float ny = _n.y;
    const float nz = _n.z;

    if (sx_abs(nx) > sx_abs(nz)) {
        float inv_len = 1.0f / sx_sqrt(nx*nx + nz*nz);
        *_t = sx_vec3f(-nz * inv_len, 0.0f, nx * inv_len);
    } else {
        float inv_len = 1.0f / sx_sqrt(ny*ny + nz*nz);
        *_t = sx_vec3f(0.0f, nz * inv_len, -ny * inv_len);
    }

    *_b = sx_vec3_cross(_n, *_t);
}

SX_INLINE void sx_vec3_tangent_angle(sx_vec3* _t, sx_vec3* _b, const sx_vec3 _n, float _angle)
{
    sx_vec3_tangent(_t, _b, _n);

    const float sa = sx_sin(_angle);
    const float ca = sx_cos(_angle);

    *_t = sx_vec3f(-sa * _b->x + ca * _t->x, 
                   -sa * _b->y + ca * _t->y, 
                   -sa * _b->z + ca * _t->z);

    *_b = sx_vec3_cross(_n, *_t);
}

SX_INLINE sx_vec3 sx_vec3_fromlatlong(float _u, float _v)
{
    const float phi   = _u * SX_PI2;
    const float theta = _v * SX_PI;

    const float st = sx_sin(theta);
    const float sp = sx_sin(phi);
    const float ct = sx_cos(theta);
    const float cp = sx_cos(phi);

    return sx_vec3f(-st*sp, ct, -st*cp);
}

SX_INLINE sx_vec2 sx_vec3_tolatlong(const sx_vec3 _dir)
{
    const float phi   = sx_atan2(_dir.x, _dir.z);
    const float theta = sx_acos(_dir.y);

    return sx_vec2f((SX_PI + phi)/SX_PI2, theta*SX_PI);
}

SX_INLINE sx_vec3 sx_vec3_mul_quat(const sx_vec3 _vec, const sx_quat _quat)
{
    sx_quat tmp0 = sx_quat_inv(_quat);
    sx_quat qv = sx_quat4f(_vec.x, _vec.y, _vec.z, 0.0f);
    sx_quat tmp1 = sx_quat_mul(tmp0, qv);
    return sx_quat_mulXYZ(tmp1, _quat);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Matrix
SX_INLINE sx_mat4 sx_mat4f(float m11, float m12, float m13, float m14,
                           float m21, float m22, float m23, float m24,
                           float m31, float m32, float m33, float m34,
                           float m41, float m42, float m43, float m44)
{
#ifdef __cplusplus
    return {{ m11, m21, m31, m41,
              m12, m22, m32, m42,
              m13, m23, m33, m43,
              m14, m24, m34, m44 }};
#else
    return (sx_mat4){.m11=m11, .m12=m12, .m13=m13, .m14=m14,
                     .m21=m21, .m22=m22, .m23=m23, .m24=m24,
                     .m31=m31, .m32=m32, .m33=m33, .m34=m34,
                     .m41=m41, .m42=m42, .m43=m43, .m44=m44};
#endif
}

SX_INLINE sx_mat4 sx_mat4fv(const float* _col1, const float* _col2, const float* _col3, const float* _col4)
{
#ifdef __cplusplus
    return {{_col1[0], _col1[1], _col1[2], _col1[3],
             _col2[0], _col2[1], _col2[2], _col2[3],
             _col3[0], _col3[1], _col3[2], _col3[3],
             _col4[0], _col4[1], _col4[2], _col4[3]}};
#else
    return (sx_mat4){.col1 = sx_vec4fv(_col1), .col2 = sx_vec4fv(_col2),
                     .col3 = sx_vec4fv(_col3), .col4 = sx_vec4fv(_col4)};
#endif
}

SX_INLINE sx_mat4 sx_mat4v(const sx_vec4 _col1, const sx_vec4 _col2, const sx_vec4 _col3, const sx_vec4 _col4)
{
#ifdef __cplusplus
    return {{_col1.f[0], _col1.f[1], _col1.f[2], _col1.f[3],
             _col2.f[0], _col2.f[1], _col2.f[2], _col2.f[3],
             _col3.f[0], _col3.f[1], _col3.f[2], _col3.f[3],
             _col4.f[0], _col4.f[1], _col4.f[2], _col4.f[3] }};
#else
    return (sx_mat4){.col1 = _col1, .col2 = _col2, .col3 = _col3, .col4 = _col4};
#endif
}

SX_INLINE sx_mat4 sx_mat4_ident()
{
    return sx_mat4f(1.0f, 0.0f, 0.0f, 0.0f,
                    0.0f, 1.0f, 0.0f, 0.0f,
                    0.0f, 0.0f, 1.0f, 0.0f,
                    0.0f, 0.0f, 0.0f, 1.0f);
}

SX_INLINE sx_vec4 sx_mat4_row1(const sx_mat4* m)
{
    return sx_vec4f(m->m11, m->m12, m->m13, m->m14);
}

SX_INLINE sx_vec4 sx_mat4_row2(const sx_mat4* m)
{
    return sx_vec4f(m->m21, m->m22, m->m23, m->m24);
}

SX_INLINE sx_vec4 sx_mat4_row3(const sx_mat4* m)
{
    return sx_vec4f(m->m31, m->m32, m->m33, m->m34);
}

SX_INLINE sx_vec4 sx_mat4_row4(const sx_mat4* m)
{
    return sx_vec4f(m->m41, m->m42, m->m43, m->m44);
}

SX_INLINE sx_mat4 sx_mat4_translate(float _tx, float _ty, float _tz)
{
    return sx_mat4f(1.0f, 0.0f, 0.0f, _tx,
                    0.0f, 1.0f, 0.0f, _ty,
                    0.0f, 0.0f, 1.0f, _tz,
                    0.0f, 0.0f, 0.0f, 1.0f);
}

SX_INLINE sx_mat4 sx_mat4_scale(float _sx, float _sy, float _sz)
{
    return sx_mat4f(_sx,  0.0f,  0.0f,  0.0f,
                    0.0f,  _sy,  0.0f,  0.0f,
                    0.0f, 0.0f,   _sz,  0.0f,
                    0.0f, 0.0f,  0.0f,  1.0f);
}

SX_INLINE sx_mat4 sx_mat4_scalef(float _scale)
{
    return sx_mat4_scale(_scale, _scale, _scale);
}

SX_INLINE sx_mat4 sx_mat4_rotateX(float _ax)
{
    const float sx = sx_sin(_ax);
    const float cx = sx_cos(_ax);

    return sx_mat4f(1.0f,  0.0f,  0.0f,  0.0f,
                    0.0f,  cx,    -sx ,  0.0f,
                    0.0f,  sx,     cx ,  0.0f,
                    0.0f, 0.0f,  0.0f,  1.0f);
}

SX_INLINE sx_mat4 sx_mat4_rotateY(float _ay)
{
    const float sy = sx_sin(_ay);
    const float cy = sx_cos(_ay);

    return sx_mat4f(cy,    0.0f,  sy,    0.0f,
                    0.0f,  1.0f,  0.0f,  0.0f,
                    -sy,   0.0f,  cy,    0.0f,
                    0.0f,  0.0f,  0.0f,  1.0f);
}

SX_INLINE sx_mat4 sx_mat4_rotateZ(float _az)
{
    const float sz = sx_sin(_az);
    const float cz = sx_cos(_az);

    return sx_mat4f(cz,    -sz,  0.0f,  0.0f,
                    sz,     cz,  0.0f,  0.0f,
                    0.0f, 0.0f,  1.0f,  0.0f,
                    0.0f, 0.0f,  0.0f,  1.0f);
}

SX_INLINE sx_mat4 sx_mat4_rotateXY(float _ax, float _ay)
{
    // TODO: maybe wrong
    const float sx = sx_sin(_ax);
    const float cx = sx_cos(_ax);
    const float sy = sx_sin(_ay);
    const float cy = sx_cos(_ay);

    return sx_mat4f(cy,     0.0f,   sy,     0.0f,
                    sx*sy,  cx,     -sx*cy, 0.0f,
                    -cx*sy, sx,     cx*cy,  0.0f,
                    0.0f,   0.0f,   0.0f,   1.0f);    
}

SX_INLINE sx_mat4 sx_mat4_rotateXYZ(float _ax, float _ay, float _az)
{
    // TODO: maybe wrong
    const float sx = sx_sin(_ax);
    const float cx = sx_cos(_ax);
    const float sy = sx_sin(_ay);
    const float cy = sx_cos(_ay);
    const float sz = sx_sin(_az);
    const float cz = sx_cos(_az);

    return sx_mat4f(cy*cz,             -cy*sz,           sy,     0.0f,
                    cz*sx*sy + cx*sz,  cx*cz - sx*sy*sz, -cy*sx, 0.0f,
                    -cx*cz*sy + sx*sz, cz*sx + cx*sy*sz, cx*cy,  0.0f,
                    0.0f,              0.0f,             0.0f,   1.0f);    
}

SX_INLINE sx_mat4 sx_mat4_rotateZYX(float _ax, float _ay, float _az)
{
    // TODO: maybe wrong
    const float sx = sx_sin(_ax);
    const float cx = sx_cos(_ax);
    const float sy = sx_sin(_ay);
    const float cy = sx_cos(_ay);
    const float sz = sx_sin(_az);
    const float cz = sx_cos(_az);

    return sx_mat4f(cy*cz, cz*sx*sy-cx*sz,   cx*cz*sy+sx*sz,    0.0f,
                    cy*sz, cx*cz + sx*sy*sz, -cz*sx + cx*sy*sz, 0.0f,
                    -sy,   cy*sx,            cx*cy,             0.0f,
                    0.0f,  0.0f,             0.0f,              1.0f);
};

SX_INLINE sx_mat4 sx_mat4_quat_translate(const sx_quat _quat, const sx_vec3 _translation)
{
    sx_mat4 mat = sx_mat4_quat(_quat);
    mat.m14 = -(mat.m11*_translation.x + mat.m12*_translation.y + mat.m13*_translation.z);
    mat.m24 = -(mat.m21*_translation.x + mat.m22*_translation.y + mat.m23*_translation.z);
    mat.m34 = -(mat.m31*_translation.x + mat.m32*_translation.y + mat.m33*_translation.z);
    return mat;
}

SX_INLINE sx_mat4 sx_mat4_quat_translate_HMD(const sx_quat _quat, const sx_vec3 _translation)
{
    return sx_mat4_quat_translate(sx_quat4f(-_quat.x, -_quat.y, _quat.z, _quat.w), _translation);
}

/// multiply vector3 into 4x4 matrix without considering 4th column, which is not used in transform matrices
SX_INLINE sx_vec3 sx_mat4_mul_vec3(const sx_mat4* _mat, const sx_vec3 _vec)
{
    return sx_vec3f(_vec.x*_mat->m11 + _vec.y*_mat->m12 + _vec.z*_mat->m13 + _mat->m14,
                    _vec.x*_mat->m21 + _vec.y*_mat->m22 + _vec.z*_mat->m23 + _mat->m24,
                    _vec.x*_mat->m31 + _vec.y*_mat->m32 + _vec.z*_mat->m33 + _mat->m34);
}

/// multiply vector3 into rotation part of the matrix only (used for normal vectors, etc...)
SX_INLINE sx_vec3 sx_mat4_mul_vec3_xyz0(const sx_mat4* _mat, const sx_vec3 _vec)
{
    return sx_vec3f(_vec.x * _mat->m11 + _vec.y * _mat->m12 + _vec.z * _mat->m13,
                    _vec.x * _mat->m21 + _vec.y * _mat->m22 + _vec.z * _mat->m23,
                    _vec.x * _mat->m31 + _vec.y * _mat->m32 + _vec.z * _mat->m33);
}

SX_INLINE sx_vec3 sx_mat4_mul_vec3_H(const sx_mat4* _mat, const sx_vec3 _vec)
{
    float xx = _vec.x * _mat->m11 + _vec.y * _mat->m12 + _vec.z * _mat->m13 + _mat->m14;
    float yy = _vec.x * _mat->m21 + _vec.y * _mat->m22 + _vec.z * _mat->m23 + _mat->m24;
    float zz = _vec.x * _mat->m31 + _vec.y * _mat->m32 + _vec.z * _mat->m33 + _mat->m34;
    float ww = _vec.x * _mat->m41 + _vec.y * _mat->m42 + _vec.z * _mat->m43 + _mat->m44;
    float iw = sx_sign(ww)/ww;
    return sx_vec3f(xx*iw, yy*iw, zz*iw);
}

SX_INLINE sx_vec4 sx_mat4_mul_vec4(const sx_mat4* _mat, const sx_vec4 _vec)
{
    return sx_vec4f(
        _vec.x * _mat->m11 + _vec.y * _mat->m12 + _vec.z * _mat->m13 + _vec.w * _mat->m14,
        _vec.x * _mat->m12 + _vec.y * _mat->m22 + _vec.z * _mat->m23 + _vec.w * _mat->m24,
        _vec.x * _mat->m31 + _vec.y * _mat->m32 + _vec.z * _mat->m33 + _vec.w * _mat->m34,
        _vec.x * _mat->m41 + _vec.y * _mat->m42 + _vec.z * _mat->m43 + _vec.w * _mat->m44);
}

SX_INLINE sx_vec4 sx_vec4_mul(const sx_vec4 _a, const sx_vec4 _b)
{
    return sx_vec4f(_a.x*_b.x, _a.y*_b.y, _a.z*_b.z, _a.w*_b.w);
}

SX_INLINE sx_vec4 sx_vec4_mulf(const sx_vec4 _a, float _b)
{
    return sx_vec4f(_a.x*_b, _a.y*_b, _a.z*_b, _a.w*_b);
}

SX_INLINE sx_mat4 sx_mat4_transpose(const sx_mat4* _a)
{
    return sx_mat4f( _a->m11, _a->m21, _a->m31, _a->m41,
                     _a->m12, _a->m22, _a->m32, _a->m42,
                     _a->m13, _a->m23, _a->m33, _a->m43,
                     _a->m14, _a->m24, _a->m34, _a->m44 );
}

/// Convert LH to RH projection matrix and vice versa.
SX_INLINE void sx_mat4_proj_flip_handedness(sx_mat4* _dst, const sx_mat4* _src)
{
    _dst->m11 = -_src->m11;
    _dst->m12 = -_src->m12;
    _dst->m13 = -_src->m13;
    _dst->m14 = -_src->m14;
    _dst->m21 =  _src->m21;
    _dst->m22 =  _src->m22;
    _dst->m23 =  _src->m23;
    _dst->m24 =  _src->m24;
    _dst->m31 = -_src->m31;
    _dst->m32 = -_src->m32;
    _dst->m33 = -_src->m33;
    _dst->m34 = -_src->m34;
    _dst->m41 =  _src->m41;
    _dst->m42 =  _src->m42;
    _dst->m43 =  _src->m43;
    _dst->m44 =  _src->m44;
}

/// Convert LH to RH view matrix and vice versa.
SX_INLINE void sx_mat4_view_flip_handedness(sx_mat4* _dst, const sx_mat4* _src)
{
    _dst->m11 = -_src->m11;
    _dst->m12 =  _src->m12;
    _dst->m13 = -_src->m13;
    _dst->m14 =  _src->m14;
    _dst->m21 = -_src->m21;
    _dst->m22 =  _src->m22;
    _dst->m23 = -_src->m23;
    _dst->m24 =  _src->m24;
    _dst->m31 = -_src->m31;
    _dst->m32 =  _src->m32;
    _dst->m33 = -_src->m33;
    _dst->m34 =  _src->m34;
    _dst->m41 = -_src->m41;
    _dst->m42 =  _src->m42;
    _dst->m43 = -_src->m43;
    _dst->m44 =  _src->m44;
}

SX_INLINE SX_CONSTFN float sx_color_tolinear(float _a)
{
    const float lo     = _a / 12.92f;
    const float hi     = sx_pow( (_a + 0.055f) / 1.055f, 2.4f);
    const float result = sx_lerp(hi, lo, _a <= 0.04045f ? 1.0f : 0.0f);
    return result;
}

SX_INLINE SX_CONSTFN float sx_color_togamma(float _a)
{
    const float lo     = _a * 12.92f;
    const float hi     = sx_pow(sx_abs(_a), 1.0f/2.4f) * 1.055f - 0.055f;
    const float result = sx_lerp(hi, lo, _a <= 0.0031308f ? 1.0f : 0.0f);
    return result;
}

//
SX_INLINE sx_mat3 sx_mat3f(float m11, float m12, float m13,
                           float m21, float m22, float m23,
                           float m31, float m32, float m33)
{
#ifdef __cplusplus
    return {{ m11, m21, m31,
              m12, m22, m32,
              m13, m23, m33 }};
#else
    return (sx_mat3){.m11=m11, .m12=m12, .m13=m13,
                     .m21=m21, .m22=m22, .m23=m23,
                     .m31=m31, .m32=m32, .m33=m33};
#endif
}

SX_INLINE sx_mat3 sx_mat3fv(const float* _col1, const float* _col2, const float* _col3)
{
#ifdef __cplusplus
    return {{_col1[0], _col1[1], _col1[2],
             _col2[0], _col2[1], _col2[2],
             _col3[0], _col3[1], _col3[2] }};
#else
    return (sx_mat3){.col1 = sx_vec3fv(_col1), .col2 = sx_vec3fv(_col2), .col3 = sx_vec3fv(_col3)};
#endif
}

SX_INLINE sx_mat3 sx_mat3v(const sx_vec3 _col1, const sx_vec3 _col2, const sx_vec3 _col3)
{
#ifdef __cplusplus
    return {{ _col1.f[0], _col1.f[1], _col1.f[2],
              _col2.f[0], _col2.f[1], _col2.f[2],
              _col3.f[0], _col3.f[1], _col3.f[2] }};
#else
    return (sx_mat3){.col1 = _col1, .col2 = _col2, .col3 = _col3};
#endif
}

SX_INLINE sx_mat3 sx_mat3_ident()
{
    return sx_mat3f(1.0f, 0.0f, 0.0f,
                    0.0f, 1.0f, 0.0f,
                    0.0f, 0.0f, 1.0f);
}

/// multiply vector3 into 4x4 matrix that [x/w, y/w, z/w, 1], used for projections, etc...
SX_INLINE sx_vec3 sx_mat3_mul_vec3(const sx_mat3* _mat, const sx_vec3 _vec)
{
    return sx_vec3f(_vec.x * _mat->m11 + _vec.y * _mat->m12 + _vec.z * _mat->m13,
                    _vec.x * _mat->m21 + _vec.y * _mat->m22 + _vec.z * _mat->m23, 
                    _vec.x * _mat->m31 + _vec.y * _mat->m32 + _vec.z * _mat->m33);    
}

SX_INLINE sx_vec2 sx_mat3_mul_vec2(const sx_mat3* _mat, const sx_vec2 _vec)
{
    return sx_vec2f( _vec.x * _mat->m11 + _vec.y * _mat->m12 + _mat->m13,
                     _vec.x * _mat->m21 + _vec.y * _mat->m22 + _mat->m23);
}

SX_INLINE sx_mat3 sx_mat3_translate(float* result, float x, float y)
{
    return sx_mat3f(1.0f, 0.0f, x,
                    0.0f, 1.0f, y,
                    0.0f, 0.0f, 1.0f);
}

SX_INLINE sx_mat3 sx_mat3_rotate(float* result, float theta)
{
    float c = sx_cos(theta);
    float s = sx_sin(theta);
    return sx_mat3f(c,      -s,     0.0f,
                    s,      c,      0.0f,
                    0.0f,   0.0f,   1.0f);
}

SX_INLINE sx_mat3 sx_mat3_scale(float* result, float sx, float sy)
{
    return sx_mat3f(sx,   0.0f, 0.0f,
                    0.0f, sy,   0.0f,
                    0.0f, 0.0f, 1.0f);    
}

SX_INLINE sx_mat3 sx_mat3_SRT(float _sx, float _sy, float _angle, float _tx, float _ty)
{
    float c = sx_cos(_angle);
    float s = sx_sin(_angle);
    return sx_mat3f(_sx*c,  -_sy*s,     _tx,
                    _sx*s,  _sy*c,      _ty,
                    0.0f,   0.0f,       1.0f);

}
SX_INLINE float sx_vec2_dot(const sx_vec2 _a, const sx_vec2 _b)
{
    return _a.x*_b.x + _a.y*_b.y;
}

SX_INLINE float sx_vec2_len(const sx_vec2 _a)
{
    return sx_sqrt(sx_vec2_dot(_a, _a));
}

SX_INLINE sx_vec2 sx_vec2_norm(const sx_vec2 _a, float* outlen SX_DFLT(NULL))
{
    const float len = sx_vec2_len(_a);
    if (len != 0.0f) {
        const float invlen = 1.0f/len;
        if (outlen) 
            *outlen = len;
        return sx_vec2f(_a.x*invlen, _a.y*invlen);
    } else {
        sx_assert(0 && "Divide by zero");
        return sx_vec2f(0.0f, 0.0f);
    }
}

SX_INLINE sx_vec2 sx_vec2_min(const sx_vec2 _a, const sx_vec2 _b)
{
    return sx_vec2f(sx_min(_a.x, _b.x), sx_min(_a.y, _b.y));
}

SX_INLINE sx_vec2 sx_vec2_max(const sx_vec2 _a, const sx_vec2 _b)
{
    return sx_vec2f(sx_max(_a.x, _b.x), sx_max(_a.y, _b.y));
}

SX_INLINE sx_vec2 sx_vec2_lerp(const sx_vec2 _a, const sx_vec2 _b, float _t)
{
    return sx_vec2f(sx_lerp(_a.x, _b.x, _t), sx_lerp(_a.y, _b.y, _t));
}

SX_INLINE sx_vec2 sx_vec2_abs(const sx_vec2 _a)
{
    return sx_vec2f(sx_abs(_a.x), sx_abs(_a.y));
}

SX_INLINE sx_vec2 sx_vec2_neg(const sx_vec2 _a)
{
    return sx_vec2f(-_a.x, -_a.y);
}

SX_INLINE sx_vec2 sx_vec2_add(const sx_vec2 _a, const sx_vec2 _b)
{
    return sx_vec2f(_a.x + _b.x, _a.y + _b.y);
}

SX_INLINE sx_vec2 sx_vec2_addf(const sx_vec2 _a, float _b)
{
    return sx_vec2f(_a.x + _b, _a.y + _b);
}

SX_INLINE sx_vec2 sx_vec2_sub(const sx_vec2 _a, const sx_vec2 _b)
{
    return sx_vec2f(_a.x - _b.x, _a.y - _b.y);
}

SX_INLINE sx_vec2 sx_vec2_subf(const sx_vec2 _a, float _b)
{
    return sx_vec2f(_a.x - _b, _a.y - _b);
}

SX_INLINE sx_vec2 sx_vec2_mul(const sx_vec2 _a, const sx_vec2 _b)
{
    return sx_vec2f(_a.x * _b.x, _a.y * _b.y);
}

SX_INLINE sx_vec2 sx_vec2_mulf(const sx_vec2 _a, float _b)
{
    return sx_vec2f(_a.x * _b, _a.y * _b);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// easing 
SX_INLINE float sx_easein_quad(float t)
{
    return t*t;
}

SX_INLINE float sx_easeout_quad(float t)
{
    return -(t * (t - 2.0f));
}

SX_INLINE float sx_easeinout_quad(float t)
{
    if(t < 0.5f) {
        return 2.0f * t * t;
    } else {
        return (-2.0f * t * t) + (4.0f * t) - 1.0f;
    }    
}

SX_INLINE float sx_easein_cubic(float t)
{
    return t * t * t; 
}

SX_INLINE float sx_easeout_cubic(float t)
{
    float f = (t - 1.0f);
    return f * f * f + 1.0f;
}

SX_INLINE float sx_easeinout_cubic(float t)
{
    if(t < 0.5f) {
        return 4.0f * t * t * t;
    } else {
        float f = ((2.0f * t) - 2.0f);
        return 0.5f * f * f * f + 1;
    }    
}

SX_INLINE float sx_easein_quart(float t)
{
    return t * t * t * t;
}

SX_INLINE float sx_easeout_quart(float t)
{
    float f = (t - 1.0f);
    return f * f * f * (1.0f - t) + 1.0f;
}

SX_INLINE float sx_easeinout_quart(float t)
{
    if(t < 0.5f) {
        return 8.0f * t * t * t * t;
    } else {
        float f = (t - 1.0f);
        return -8.0f * f * f * f * f + 1.0f;
    }    
}

SX_INLINE float sx_easein_quint(float t)
{
    return t * t * t * t * t;
}

SX_INLINE float sx_easeout_quint(float t)
{
    float f = (t - 1.0f);
    return f * f * f * f * f + 1.0f;
}

SX_INLINE float sx_easeinout_quint(float t)
{
    if(t < 0.5f) {
        return 16.0f * t * t * t * t * t;
    }
    else {
        float f = ((2.0f * t) - 2.0f);
        return  0.5f * f * f * f * f * f + 1.0f;
    }    
}

SX_INLINE float sx_easein_sine(float t)
{
    return sx_sin((t - 1) * SX_PI2) + 1.0f;
}

SX_INLINE float sx_easeout_sine(float t)
{
    return sx_sin(t * SX_PI2);
}

SX_INLINE float sx_easeinout_sine(float t)
{
    return 0.5f * (1.0f - sx_cos(t * SX_PI));    
}

SX_INLINE float sx_easein_circ(float t)
{
    return 1.0f - sx_sqrt(1.0f - (t * t));
}

SX_INLINE float sx_easeout_circ(float t)
{
    return sx_sqrt((2.0f - t) * t);
}

SX_INLINE float sx_easeinout_circ(float t)
{
    if(t < 0.5f) {
        return 0.5f * (1.0f - sx_sqrt(1.0f - 4.0f * (t * t)));
    } else {
        return 0.5f * (sx_sqrt(-((2.0f * t) - 3.0f) * ((2.0f * t) - 1.0f)) + 1.0f);
    }    
}

SX_INLINE float sx_easein_expo(float t)
{
    return (t == 0.0f) ? t : sx_pow(2.0f, 10.0f * (t - 1.0f));
}

SX_INLINE float sx_easeout_expo(float t)
{
    return (t == 1.0f) ? t : 1.0f - sx_pow(2.0f, -10.0f * t);
}

SX_INLINE float sx_easeinout_expo(float t)
{
    if(t == 0.0f || t == 1.0f) 
        return t;
    
    if(t < 0.5f) {
        return 0.5f * sx_pow(2.0f, (20.0f * t) - 10.0f);
    } else {
        return -0.5f * sx_pow(2.0f, (-20.0f * t) + 10.0f) + 1.0f;
    }    
}

SX_INLINE float sx_easein_elastic(float t)
{
    return sx_sin(13.0f * SX_PI2 * t) * sx_pow(2.0f, 10.0f * (t - 1.0f));
}

SX_INLINE float sx_easeout_elastic(float t)
{
    return sx_sin(-13.0f * SX_PI2 * (t + 1.0f)) * sx_pow(2.0f, -10.0f * t) + 1.0f;
}

SX_INLINE float sx_easeinout_elastic(float t)
{
    if(t < 0.5f) {
        return 0.5f * sx_sin(13.0f * SX_PI2 * (2.0f * t)) * sx_pow(2.0f, 10.0f * ((2.0f * t) - 1.0f));
    } else {
        return 0.5f * (sx_sin(-13.0f * SX_PI2 * ((2.0f * t - 1.0f) + 1.0f)) * 
               sx_pow(2.0f, -10.0f * (2.0f * t - 1.0f)) + 2.0f);
    }    
}

SX_INLINE float sx_easein_back(float t)
{
    const float s = 1.70158f;
    return t * t * ((s + 1.0f) * t - s);
}

SX_INLINE float sx_easeout_back(float t)
{
    const float s = 1.70158f;
    return --t, 1.0f * (t*t*((s+1.0f)*t + s) + 1.0f);
}

SX_INLINE float sx_easeinout_back(float t)
{
    const float s = 2.5949095f;
    if (t < 0.5f) {
        return t *= 2.0f, 0.5f * t * t * (t*s+t-s);
    } else {
        return t = t * 2.0f - 2.0f, 0.5f * (2.0f + t*t*(t*s+t+s));
    }    
}

SX_INLINE float sx_easeout_bounce(float t)
{
    static const float kSections[] = {4.0f/11.0f, 8.0f/11.0f, 9/10.0f};
    static const float kA1 =    363.0f/40.0f;
    static const float kA1_b =  99.0f/10.0f;
    static const float kA1_c =  17.0f/5.0f;
    static const float kA2 =    4356.0f/361.0f;
    static const float kA2_b =  35442.0f/1805.0f;
    static const float kA2_c =  16061.0f/1805.0f;
    static const float kA3 =    54.0f/5.0f;
    static const float kA3_b =  513.0f/25.0f;
    static const float kA3_c =  268.0f/25.0f;
    
	if(t < kSections[0]) {
		return (121.0f * t * t)/16.0f;
	} else if(t < kSections[1]) {
		return (kA1 * t * t) - (kA1_b * t) + kA1_c;
	} else if(t < kSections[2]) {
		return (kA2 * t * t) - (kA2_b * t) + kA2_c;
	} else {
		return (kA3 * t * t) - (kA3_b * t) + kA3_c;
	}
}

SX_INLINE float sx_easein_bounce(float t)
{
    return 1.0f - sx_easeout_bounce(1.0f - t);
}

SX_INLINE float sx_easeinout_bounce(float t)
{
	if(t < 0.5f) {
		return 0.5f * sx_easein_bounce(t*2.0f);
	} else {
		return 0.5f * sx_easeout_bounce(t * 2.0f - 1.0f) + 0.5f;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Predefined static primitives
SX_API sx_vec2 SX_VEC2_ZERO;
SX_API sx_vec2 SX_VEC2_UNITX;
SX_API sx_vec2 SX_VEC2_UNITY;

SX_API sx_vec3 SX_VEC3_ZERO;
SX_API sx_vec3 SX_VEC3_UNITX;
SX_API sx_vec3 SX_VEC3_UNITY;
SX_API sx_vec3 SX_VEC3_UNITZ;

SX_API sx_vec4 SX_VEC4_ZERO;
SX_API sx_vec4 SX_VEC4_UNITX;
SX_API sx_vec4 SX_VEC4_UNITY;
SX_API sx_vec4 SX_VEC4_UNITZ;

#endif // SX_MATH_H_

//
// Version history
//      v1.0.0      Initial release
//      v1.1.0      Matrices are now column-major (in memory)
//                  Added SX_VECx_ constants
//      v1.1.1      Fixed through the whole API for RH proj and view calculations
//

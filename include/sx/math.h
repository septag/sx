//
// Copyright 2018 Sepehr Taghdisian (septag@github). All rights reserved.
// License: https://github.com/septag/sx#license-bsd-2-clause
//
// math.h - v1.0 - Fpu and Vector math functions
//                 Contains vector primitives and vector/fpu math functions, event functions implemented in libm
//                 Many functions are from bx library (https://github.com/bkaradzic/bx)
//                 
// TODO: check and try to get rid of 'if' in sqrt/norm functions

#pragma once

#ifndef SX_MATH_H_
#define SX_MATH_H_

#include "sx.h"

// Some math constants
SX_EXTERN const float SX_PI;
SX_EXTERN const float SX_PI2;
SX_EXTERN const float SX_INVPI;
SX_EXTERN const float SX_PIHALF;
SX_EXTERN const float SX_PIQUARTER;
SX_EXTERN const float SX_E;
SX_EXTERN const float SX_FLOAT_MAX;
SX_EXTERN const float SX_FLOAT_MIN;
SX_EXTERN const float SX_LOG_NAT10;
SX_EXTERN const float SX_INVLOG_NAT2;
SX_EXTERN const float SX_LOG_NAT2H;
SX_EXTERN const float SX_LOG_NAT2L;
SX_EXTERN const float SX_NEAR_ZERO;
SX_EXTERN const float SX_SQRT2;

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

#ifdef __cplusplus
    sx_vec4() {}
    explicit sx_vec4(float _x, float _y, float _z, float _w);
    explicit sx_vec4(const float* _f);
#endif
} sx_vec4;

typedef union sx_vec2
{
    struct
    {
        float x;
        float y;
    };

    float f[2];

#ifdef __cplusplus
    sx_vec2() {}
    explicit sx_vec2(float _x, float _y);
    explicit sx_vec2(const float* _f);
#endif
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

#ifdef __cplusplus
    sx_vec3() {}
    explicit sx_vec3(float _x, float _y, float _z);
    explicit sx_vec3(const float* _f);
#endif
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

#ifdef __cplusplus
    sx_color() {}
    explicit sx_color(uint8_t _r, uint8_t _g, uint8_t _b, uint8_t _a = 255);
    explicit sx_color(uint32_t _n);
    explicit sx_color(float _r, float _g, float _b, float _a = 1.0f);
#endif
} sx_color;


typedef union sx_ivec2
{
    struct
    {
        int x;
        int y;
    };

    int n[2];

#ifdef __cplusplus
    sx_ivec2() {}
    explicit sx_ivec2(int _x, int _y);
    explicit sx_ivec2(const int* _n);
#endif
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

#ifdef __cplusplus
    sx_quat() {};
    explicit sx_quat(float _x, float _y, float _z, float _w);
    explicit sx_quat(const float* _f);
#endif
} sx_quat;

typedef union sx_mat3
{
    struct
    {
        float m11, m12, m13;
        float m21, m22, m23;
        float m31, m32, m33;
    };

    struct
    {
        float fr1[3];
        float fr2[3];
        float fr3[3];
    };

    struct
    {
        sx_vec3 row1;
        sx_vec3 row2;
        sx_vec3 row3;
    };

    float f[9];

#ifdef __cplusplus
    sx_mat3() {}
    explicit sx_mat3(float _m11, float _m12, float _m13,
                     float _m21, float _m22, float _m23,
                     float _m31, float _m32, float _m33);
    explicit sx_mat3(const float* _r1, const float* _r2, const float* _r3);
    explicit sx_mat3(const sx_vec3& _row1, const sx_vec3& _row2, const sx_vec3& _row3);
#endif
} sx_mat3;

typedef union sx_mat4
{
    struct
    {
        float m11, m12, m13, m14;
        float m21, m22, m23, m24;
        float m31, m32, m33, m34;
        float m41, m42, m43, m44;
    };

    struct
    {
        float rf1[4];
        float rf2[4];
        float rf3[4];
        float rf4[4];
    };

    struct
    {
        sx_vec4 row1;
        sx_vec4 row2;
        sx_vec4 row3;
        sx_vec4 row4;
    };

    float f[16];

#ifdef __cplusplus
    sx_mat4() {}
    explicit sx_mat4(float _m11, float _m12, float _m13, float _m14,
                     float _m21, float _m22, float _m23, float _m24,
                     float _m31, float _m32, float _m33, float _m34,
                     float _m41, float _m42, float _m43, float _m44);
    explicit sx_mat4(const float* _r1, const float* _r2, const float* _r3, const float* _r4);
    explicit sx_mat4(const sx_vec4& _row1, const sx_vec4& _row2, const sx_vec4& _row3, const sx_vec4& _row4);
#endif
} mat4_t;


SX_EXTERN SX_CONSTFN float sx_floor(float _f);

SX_EXTERN SX_CONSTFN float sx_cos(float _a);

SX_EXTERN SX_CONSTFN float sx_acos(float _a);

SX_EXTERN SX_CONSTFN float sx_atan2(float _y, float _x);

SX_EXTERN SX_CONSTFN float sx_exp(float _a);

SX_EXTERN SX_CONSTFN float sx_log(float _a);

SX_EXTERN void sx_vec3_calc_normal(float _result[3], float _va[3], float _vb[3], float _vc[3]);
SX_EXTERN void sx_vec3_calc_plane(float _result[4], float _va[3], float _vb[3], float _vc[3]);
SX_EXTERN void sx_vec3_calc_linearfit2D(float _result[2], const void* _points, int _stride, int _num);
SX_EXTERN void sx_vec3_calc_linearfit3D(float _result[3], const void* _points, int _stride, int _num);

SX_EXTERN void sx_mat4_quat(float* _result, const float* _quat);
SX_EXTERN void sx_mat4_from_normal(float* _result, const float* _normal, float _scale, const float* _pos);
SX_EXTERN void sx_mat4_from_normal_angle(float* _result, const float* _normal, float _scale, const float* _pos, float _angle);
SX_EXTERN void sx_mat4_lookatLH(float* _result, const float* _eye, const float* _at, const float* _up);
SX_EXTERN void sx_mat4_lookatRH(float* _result, const float* _eye, const float* _at, const float* _up);
SX_EXTERN void sx_mat4_lookat(float* _result, const float* _eye, const float* _at, const float* _up);
SX_EXTERN void sx_mat4_proj(float* _result, float _ut, float _dt, float _lt, float _rt, float _near, float _far, bool _oglNdc);
SX_EXTERN void sx_mat4_proj_fov(float* _result, const float _fov[4], float _near, float _far, bool _oglNdc);
SX_EXTERN void sx_mat4_proj_fovY(float* _result, float _fovy, float _aspect, float _near, float _far, bool _oglNdc);
SX_EXTERN void sx_mat4_projLH(float* _result, float _ut, float _dt, float _lt, float _rt, float _near, float _far, bool _oglNdc);
SX_EXTERN void sx_mat4_projLH_fov(float* _result, const float _fov[4], float _near, float _far, bool _oglNdc);
SX_EXTERN void sx_mat4_projLH_fovY(float* _result, float _fovy, float _aspect, float _near, float _far, bool _oglNdc);
SX_EXTERN void sx_mat4_projRH(float* _result, float _ut, float _dt, float _lt, float _rt, float _near, float _far, bool _oglNdc);
SX_EXTERN void sx_mat4_projRH_fov(float* _result, const float _fov[4], float _near, float _far, bool _oglNdc);
SX_EXTERN void sx_mat4_projRH_fovY(float* _result, float _fovy, float _aspect, float _near, float _far, bool _oglNdc);
SX_EXTERN void sx_mat4_proj_inf_fov(float* _result, const float _fov[4], float _near, bool _oglNdc);
SX_EXTERN void sx_mat4_proj_inf(float* _result, float _ut, float _dt, float _lt, float _rt, float _near, bool _oglNdc);
SX_EXTERN void sx_mat4_proj_inf_fovY(float* _result, float _fovy, float _aspect, float _near, bool _oglNdc);
SX_EXTERN void sx_mat4_proj_infLH(float* _result, float _ut, float _dt, float _lt, float _rt, float _near, bool _oglNdc);
SX_EXTERN void sx_mat4_proj_infLH_fov(float* _result, const float _fov[4], float _near, bool _oglNdc);
SX_EXTERN void sx_mat4_proj_infLH_fovY(float* _result, float _fovy, float _aspect, float _near, bool _oglNdc);
SX_EXTERN void sx_mat4_proj_infRH(float* _result, float _ut, float _dt, float _lt, float _rt, float _near, bool _oglNdc);
SX_EXTERN void sx_mat4_proj_infRH_fov(float* _result, const float _fov[4], float _near, bool _oglNdc);
SX_EXTERN void sx_mat4_proj_infRH_fovY(float* _result, float _fovy, float _aspect, float _near, bool _oglNdc);
SX_EXTERN void sx_mat4_projrev_infLH(float* _result, float _ut, float _dt, float _lt, float _rt, float _near, bool _oglNdc);
SX_EXTERN void sx_mat4_projrev_infLH_fov(float* _result, const float _fov[4], float _near, bool _oglNdc);
SX_EXTERN void sx_mat4_projrev_infLH_fovY(float* _result, float _fovy, float _aspect, float _near, bool _oglNdc);
SX_EXTERN void sx_mat4_projrev_infRH(float* _result, float _ut, float _dt, float _lt, float _rt, float _near, bool _oglNdc);
SX_EXTERN void sx_mat4_projrev_infRH_fov(float* _result, const float _fov[4], float _near, bool _oglNdc);
SX_EXTERN void sx_mat4_projrev_infRH_fovY(float* _result, float _fovy, float _aspect, float _near, bool _oglNdc);
SX_EXTERN void sx_mat4_ortho(float* _result, float _left, float _right, float _bottom, float _top, float _near, float _far, float _offset, bool _oglNdc);
SX_EXTERN void sx_mat4_orthoLH(float* _result, float _left, float _right, float _bottom, float _top, float _near, float _far, float _offset, bool _oglNdc);
SX_EXTERN void sx_mat4_orthoRH(float* _result, float _left, float _right, float _bottom, float _top, float _near, float _far, float _offset, bool _oglNdc);
SX_EXTERN void sx_mat4_rotateX(float* _result, float _ax);
SX_EXTERN void sx_mat4_rotateY(float* _result, float _ay);
SX_EXTERN void sx_mat4_rotateZ(float* _result, float _az);
SX_EXTERN void sx_mat4_rotateXY(float* _result, float _ax, float _ay);
SX_EXTERN void sx_mat4_rotateXYZ(float* _result, float _ax, float _ay, float _az);
SX_EXTERN void sx_mat4_rotateZYX(float* _result, float _ax, float _ay, float _az);
SX_EXTERN void sx_mat4_SRT(float* _result, float _sx, float _sy, float _sz, float _ax, float _ay, float _az, float _tx, float _ty, float _tz);
SX_EXTERN void sx_mat4_mul(float* _result, const float* _a, const float* _b);
SX_EXTERN void sx_mat4_inv(float* _result, const float* _a);
/// Inverse for transform-only matrices (column4=0) (mat4x)
SX_EXTERN void sx_mat4x_inv(float* _result, const float* _a);
SX_EXTERN void sx_mat4_calc_quat(float* _result, const float* _mat);

SX_EXTERN void sx_vec4_mul_mat4(float* _result, const float* _vec, const float* _mat);

SX_EXTERN void sx_mat3_inv(float* _result, const float* _a);
SX_EXTERN void sx_mat3_mul(float* _result, const float* _a, const float* _b);
SX_EXTERN void sx_color_RGBtoHSV(float _hsv[3], const float _rgb[3]);
SX_EXTERN void sx_color_HSVtoRGB(float _rgb[3], const float _hsv[3]);

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
    return _a < 0.0f ? -_a : _a;
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

SX_INLINE SX_CONSTFN float sx_rsqrt(float _a)
{
    return sx_pow(_a, -0.5f);
}

SX_INLINE SX_CONSTFN float sx_sqrt(float _a)
{
    if (_a >= SX_NEAR_ZERO) {
        return 1.0f/sx_rsqrt(_a);
    } else {
        return 0;
    }
}

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

SX_INLINE SX_CONSTFN bool sx_equal(float _a, float _b, float _epsilon)
{
    // http://realtimecollisiondetection.net/blog/?p=89
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
SX_INLINE SX_CONSTFN float sx_step(float _edge, float _a)
{
    return _a < _edge ? 0.0f : 1.0f;
}

// smooth Hermite interpolation (result = 0 ~ 1) when edge0 < x < edge1
SX_INLINE SX_CONSTFN float sx_smoothstep(float _edge1, float _edge2, float _a)
{
    _a = sx_clamp((_a - _edge1)/(_edge2 - _edge1), 0.0f, 1.0f);
    return _a*_a*(3.0f - 2.0f*_a);
}

SX_INLINE SX_CONSTFN float sx_pulse(float _a, float _start, float _end)
{
    return sx_step(_a, _start) - sx_step(_a, _end);
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

////////////////////////////////////////////////////////////////////////////////////////////////////
// Quaternion
SX_INLINE void sx_quat_ident(float* _result)
{
    _result[0] = 0.0f;
    _result[1] = 0.0f;
    _result[2] = 0.0f;
    _result[3] = 1.0f;
}

SX_INLINE void sx_quat_mulXYZ(float* _result, const float* _qa, const float* _qb)
{
    const float ax = _qa[0];
    const float ay = _qa[1];
    const float az = _qa[2];
    const float aw = _qa[3];

    const float bx = _qb[0];
    const float by = _qb[1];
    const float bz = _qb[2];
    const float bw = _qb[3];

    _result[0] = aw * bx + ax * bw + ay * bz - az * by;
    _result[1] = aw * by - ax * bz + ay * bw + az * bx;
    _result[2] = aw * bz + ax * by - ay * bx + az * bw;
}

SX_INLINE void sx_quat_mul(float* _result, const float* _qa, const float* _qb)
{
    const float ax = _qa[0];
    const float ay = _qa[1];
    const float az = _qa[2];
    const float aw = _qa[3];

    const float bx = _qb[0];
    const float by = _qb[1];
    const float bz = _qb[2];
    const float bw = _qb[3];

    _result[0] = aw * bx + ax * bw + ay * bz - az * by;
    _result[1] = aw * by - ax * bz + ay * bw + az * bx;
    _result[2] = aw * bz + ax * by - ay * bx + az * bw;
    _result[3] = aw * bw - ax * bx - ay * by - az * bz;
}

SX_INLINE void sx_quat_inv(float* _result, const float* _quat)
{
    _result[0] = -_quat[0];
    _result[1] = -_quat[1];
    _result[2] = -_quat[2];
    _result[3] =  _quat[3];
}

SX_INLINE float sx_quat_dot(const float* _a, const float* _b)
{
    return _a[0]*_b[0]
            + _a[1]*_b[1]
            + _a[2]*_b[2]
            + _a[3]*_b[3];
}

SX_INLINE void sx_quat_norm(float* _result, const float* _quat)
{
    const float norm = sx_quat_dot(_quat, _quat);
    if (0.0f < norm) {
        const float inv_norm = 1.0f / sx_sqrt(norm);
        _result[0] = _quat[0] * inv_norm;
        _result[1] = _quat[1] * inv_norm;
        _result[2] = _quat[2] * inv_norm;
        _result[3] = _quat[3] * inv_norm;
    } else {
        sx_quat_ident(_result);
    }
}

SX_INLINE void sx_quat_toeuler(float* _result, const float* _quat)
{
    const float x = _quat[0];
    const float y = _quat[1];
    const float z = _quat[2];
    const float w = _quat[3];

    const float yy = y * y;
    const float zz = z * z;

    const float xx = x * x;
    _result[0] = sx_atan2(2.0f * (x * w - y * z), 1.0f - 2.0f * (xx + zz) );
    _result[1] = sx_atan2(2.0f * (y * w + x * z), 1.0f - 2.0f * (yy + zz) );
    _result[2] = sx_asin (2.0f * (x * y + z * w) );
}

SX_INLINE void sx_quat_rotateaxis(float* _result, const float* _axis, float _angle)
{
    const float ha = _angle * 0.5f;
    const float ca = sx_cos(ha);
    const float sa = sx_sin(ha);
    _result[0] = _axis[0] * sa;
    _result[1] = _axis[1] * sa;
    _result[2] = _axis[2] * sa;
    _result[3] = ca;
}

SX_INLINE void sx_quat_rotateX(float* _result, float _ax)
{
    const float hx = _ax * 0.5f;
    const float cx = sx_cos(hx);
    const float sx = sx_sin(hx);
    _result[0] = sx;
    _result[1] = 0.0f;
    _result[2] = 0.0f;
    _result[3] = cx;
}

SX_INLINE void sx_quat_rotateY(float* _result, float _ay)
{
    const float hy = _ay * 0.5f;
    const float cy = sx_cos(hy);
    const float sy = sx_sin(hy);
    _result[0] = 0.0f;
    _result[1] = sy;
    _result[2] = 0.0f;
    _result[3] = cy;
}

SX_INLINE void sx_quat_rotateZ(float* _result, float _az)
{
    const float hz = _az * 0.5f;
    const float cz = sx_cos(hz);
    const float sz = sx_sin(hz);
    _result[0] = 0.0f;
    _result[1] = 0.0f;
    _result[2] = sz;
    _result[3] = cz;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Vec3
SX_INLINE void sx_vec3_abs(float* _result, const float* _a)
{
    _result[0] = sx_abs(_a[0]);
    _result[1] = sx_abs(_a[1]);
    _result[2] = sx_abs(_a[2]);
}

SX_INLINE void sx_vec3_neg(float* _result, const float* _a)
{
    _result[0] = -_a[0];
    _result[1] = -_a[1];
    _result[2] = -_a[2];
}

SX_INLINE void sx_vec3_add(float* _result, const float* _a, const float* _b)
{
    _result[0] = _a[0] + _b[0];
    _result[1] = _a[1] + _b[1];
    _result[2] = _a[2] + _b[2];
}

SX_INLINE void sx_vec3_addf(float* _result, const float* _a, float _b)
{
    _result[0] = _a[0] + _b;
    _result[1] = _a[1] + _b;
    _result[2] = _a[2] + _b;
}

SX_INLINE void sx_vec3_sub(float* _result, const float* _a, const float* _b)
{
    _result[0] = _a[0] - _b[0];
    _result[1] = _a[1] - _b[1];
    _result[2] = _a[2] - _b[2];
}

SX_INLINE void sx_vec3_subf(float* _result, const float* _a, float _b)
{
    _result[0] = _a[0] - _b;
    _result[1] = _a[1] - _b;
    _result[2] = _a[2] - _b;
}

SX_INLINE void sx_vec3_mul(float* _result, const float* _a, const float* _b)
{
    _result[0] = _a[0] * _b[0];
    _result[1] = _a[1] * _b[1];
    _result[2] = _a[2] * _b[2];
}

SX_INLINE void sx_vec3_mulf(float* _result, const float* _a, float _b)
{
    _result[0] = _a[0] * _b;
    _result[1] = _a[1] * _b;
    _result[2] = _a[2] * _b;
}

SX_INLINE float sx_vec3_dot(const float* _a, const float* _b)
{
    return _a[0]*_b[0] + _a[1]*_b[1] + _a[2]*_b[2];
}

SX_INLINE void sx_vec3_cross(float* _result, const float* _a, const float* _b)
{
    _result[0] = _a[1]*_b[2] - _a[2]*_b[1];
    _result[1] = _a[2]*_b[0] - _a[0]*_b[2];
    _result[2] = _a[0]*_b[1] - _a[1]*_b[0];
}

SX_INLINE float sx_vec3_len(const float* _a)
{
    return sx_sqrt(sx_vec3_dot(_a, _a) );
}

SX_INLINE void sx_vec3_lerp(float* _result, const float* _a, const float* _b, float _t)
{
    _result[0] = sx_lerp(_a[0], _b[0], _t);
    _result[1] = sx_lerp(_a[1], _b[1], _t);
    _result[2] = sx_lerp(_a[2], _b[2], _t);
}

SX_INLINE float sx_vec3_norm(float* _result, const float* _a)
{
    const float len = sx_vec3_len(_a);
    if (len >= SX_NEAR_ZERO) {
        const float inv_len = 1.0f/len;
        _result[0] = _a[0] * inv_len;
        _result[1] = _a[1] * inv_len;
        _result[2] = _a[2] * inv_len;
    }
    return len;
}

SX_INLINE void sx_vec3_min(float* _result, const float* _a, const float* _b)
{
    _result[0] = sx_min(_a[0], _b[0]);
    _result[1] = sx_min(_a[1], _b[1]);
    _result[2] = sx_min(_a[2], _b[2]);
}

SX_INLINE void sx_vec3_max(float* _result, const float* _a, const float* _b)
{
    _result[0] = sx_max(_a[0], _b[0]);
    _result[1] = sx_max(_a[1], _b[1]);
    _result[2] = sx_max(_a[2], _b[2]);
}

SX_INLINE void sx_vec3_rcp(float* _result, const float* _a)
{
    _result[0] = 1.0f / _a[0];
    _result[1] = 1.0f / _a[1];
    _result[2] = 1.0f / _a[2];
}

SX_INLINE void sx_vec3_tangent(const float* _n, float* _t, float* _b)
{
    const float nx = _n[0];
    const float ny = _n[1];
    const float nz = _n[2];

    if (sx_abs(nx) > sx_abs(nz)) {
        float inv_len = 1.0f / sx_sqrt(nx*nx + nz*nz);
        _t[0] = -nz * inv_len;
        _t[1] =  0.0f;
        _t[2] =  nx * inv_len;
    } else {
        float inv_len = 1.0f / sx_sqrt(ny*ny + nz*nz);
        _t[0] =  0.0f;
        _t[1] =  nz * inv_len;
        _t[2] = -ny * inv_len;
    }

    sx_vec3_cross(_b, _n, _t);
}

SX_INLINE void sx_vec3_tangent_angle(const float* _n, float _angle, float* _t, float* _b)
{
    sx_vec3_tangent(_n, _t, _b);

    const float sa = sx_sin(_angle);
    const float ca = sx_cos(_angle);

    _t[0] = -sa * _b[0] + ca * _t[0];
    _t[1] = -sa * _b[1] + ca * _t[1];
    _t[2] = -sa * _b[2] + ca * _t[2];

    sx_vec3_cross(_b, _n, _t);
}

SX_INLINE void sx_vec3_fromlatlong(float* _vec, float _u, float _v)
{
    const float phi   = _u * SX_PI2;
    const float theta = _v * SX_PI;

    const float st = sx_sin(theta);
    const float sp = sx_sin(phi);
    const float ct = sx_cos(theta);
    const float cp = sx_cos(phi);

    _vec[0] = -st*sp;
    _vec[1] =  ct;
    _vec[2] = -st*cp;
}

SX_INLINE void sx_vec3_tolatlong(float* _outU, float* _outV, const float* _dir)
{
    const float phi   = sx_atan2(_dir[0], _dir[2]);
    const float theta = sx_acos(_dir[1]);

    *_outU = (SX_PI + phi)/SX_PI2;
    *_outV = theta*SX_PI;
}

SX_INLINE void sx_vec3_mul_quat(float* _result, const float* _vec, const float* _quat)
{
    float tmp0[4];
    sx_quat_inv(tmp0, _quat);

    float qv[4];
    qv[0] = _vec[0];
    qv[1] = _vec[1];
    qv[2] = _vec[2];
    qv[3] = 0.0f;

    float tmp1[4];
    sx_quat_mul(tmp1, tmp0, qv);

    sx_quat_mulXYZ(_result, tmp1, _quat);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Matrix
SX_INLINE void sx_mat4_ident(float* _result)
{
    sx_memset(_result, 0x0, sizeof(float)*16);
    _result[0] = _result[5] = _result[10] = _result[15] = 1.0f;
}

SX_INLINE void sx_mat4_translate(float* _result, float _tx, float _ty, float _tz)
{
    sx_mat4_ident(_result);
    _result[12] = _tx;
    _result[13] = _ty;
    _result[14] = _tz;
}

SX_INLINE void sx_mat4_scale(float* _result, float _sx, float _sy, float _sz)
{
    sx_memset(_result, 0x0, sizeof(float) * 16);
    _result[0]  = _sx;
    _result[5]  = _sy;
    _result[10] = _sz;
    _result[15] = 1.0f;
}

SX_INLINE void sx_mat4_scalef(float* _result, float _scale)
{
    sx_mat4_scale(_result, _scale, _scale, _scale);
}

SX_INLINE void sx_mat4_quat_translate(float* _result, const float* _quat, const float* _translation)
{
    sx_mat4_quat(_result, _quat);
    _result[12] = -(_result[0]*_translation[0] + _result[4]*_translation[1] + _result[ 8]*_translation[2]);
    _result[13] = -(_result[1]*_translation[0] + _result[5]*_translation[1] + _result[ 9]*_translation[2]);
    _result[14] = -(_result[2]*_translation[0] + _result[6]*_translation[1] + _result[10]*_translation[2]);
}

SX_INLINE void sx_mat4_quat_translate_HMD(float* _result, const float* _quat, const float* _translation)
{
    float quat[4];
    quat[0] = -_quat[0];
    quat[1] = -_quat[1];
    quat[2] =  _quat[2];
    quat[3] =  _quat[3];
    sx_mat4_quat_translate(_result, quat, _translation);
}

/// multiply vector3 into 4x4 matrix without considering 4th column, which is not used in transform matrices
SX_INLINE void sx_vec3_mul_mat4(float* _result, const float* _vec, const float* _mat)
{
    _result[0] = _vec[0] * _mat[ 0] + _vec[1] * _mat[4] + _vec[2] * _mat[ 8] + _mat[12];
    _result[1] = _vec[0] * _mat[ 1] + _vec[1] * _mat[5] + _vec[2] * _mat[ 9] + _mat[13];
    _result[2] = _vec[0] * _mat[ 2] + _vec[1] * _mat[6] + _vec[2] * _mat[10] + _mat[14];
}

/// multiply vector3 into rotation part of the matrix only (used for normal vectors, etc...)
SX_INLINE void sx_vec3_mul_mat4_xyz0(float* _result, const float* _vec, const float* _mat)
{
    _result[0] = _vec[0] * _mat[ 0] + _vec[1] * _mat[4] + _vec[2] * _mat[ 8];
    _result[1] = _vec[0] * _mat[ 1] + _vec[1] * _mat[5] + _vec[2] * _mat[ 9];
    _result[2] = _vec[0] * _mat[ 2] + _vec[1] * _mat[6] + _vec[2] * _mat[10];
}

SX_INLINE void sx_vec3_mul_mat4H(float* _result, const float* _vec, const float* _mat)
{
    float xx = _vec[0] * _mat[ 0] + _vec[1] * _mat[4] + _vec[2] * _mat[ 8] + _mat[12];
    float yy = _vec[0] * _mat[ 1] + _vec[1] * _mat[5] + _vec[2] * _mat[ 9] + _mat[13];
    float zz = _vec[0] * _mat[ 2] + _vec[1] * _mat[6] + _vec[2] * _mat[10] + _mat[14];
    float ww = _vec[0] * _mat[ 3] + _vec[1] * _mat[7] + _vec[2] * _mat[11] + _mat[15];
    float invW = sx_sign(ww)/ww;
    _result[0] = xx*invW;
    _result[1] = yy*invW;
    _result[2] = zz*invW;
}

SX_INLINE void sx_vec4_mul(float* _result, const float* _a, const float* _b)
{
    _result[0] = _a[0] * _b[0];
    _result[1] = _a[1] * _b[1];
    _result[2] = _a[2] * _b[2];
    _result[3] = _a[3] * _b[3];
}

SX_INLINE void sx_vec4_mulf(float* _result, const float* _a, float _b)
{
    _result[0] = _a[0] * _b;
    _result[1] = _a[1] * _b;
    _result[2] = _a[2] * _b;
    _result[3] = _a[3] * _b;
}

SX_INLINE void sx_mat4_transpose(float* _result, const float* _a)
{
    _result[ 0] = _a[ 0];
    _result[ 4] = _a[ 1];
    _result[ 8] = _a[ 2];
    _result[12] = _a[ 3];
    _result[ 1] = _a[ 4];
    _result[ 5] = _a[ 5];
    _result[ 9] = _a[ 6];
    _result[13] = _a[ 7];
    _result[ 2] = _a[ 8];
    _result[ 6] = _a[ 9];
    _result[10] = _a[10];
    _result[14] = _a[11];
    _result[ 3] = _a[12];
    _result[ 7] = _a[13];
    _result[11] = _a[14];
    _result[15] = _a[15];
}

/// Convert LH to RH projection matrix and vice versa.
SX_INLINE void sx_mat4_proj_flip_handedness(float* _dst, const float* _src)
{
    _dst[ 0] = -_src[ 0];
    _dst[ 1] = -_src[ 1];
    _dst[ 2] = -_src[ 2];
    _dst[ 3] = -_src[ 3];
    _dst[ 4] =  _src[ 4];
    _dst[ 5] =  _src[ 5];
    _dst[ 6] =  _src[ 6];
    _dst[ 7] =  _src[ 7];
    _dst[ 8] = -_src[ 8];
    _dst[ 9] = -_src[ 9];
    _dst[10] = -_src[10];
    _dst[11] = -_src[11];
    _dst[12] =  _src[12];
    _dst[13] =  _src[13];
    _dst[14] =  _src[14];
    _dst[15] =  _src[15];
}

/// Convert LH to RH view matrix and vice versa.
SX_INLINE void sx_mat4_view_flip_handedness(float* _dst, const float* _src)
{
    _dst[ 0] = -_src[ 0];
    _dst[ 1] =  _src[ 1];
    _dst[ 2] = -_src[ 2];
    _dst[ 3] =  _src[ 3];
    _dst[ 4] = -_src[ 4];
    _dst[ 5] =  _src[ 5];
    _dst[ 6] = -_src[ 6];
    _dst[ 7] =  _src[ 7];
    _dst[ 8] = -_src[ 8];
    _dst[ 9] =  _src[ 9];
    _dst[10] = -_src[10];
    _dst[11] =  _src[11];
    _dst[12] = -_src[12];
    _dst[13] =  _src[13];
    _dst[14] = -_src[14];
    _dst[15] =  _src[15];
}

SX_INLINE SX_CONSTFN float sx_color_tolinear(float _a)
{
    const float lo     = _a / 12.92f;
    const float hi     = sx_pow( (_a + 0.055f) / 1.055f, 2.4f);
    const float result = sx_lerp(hi, lo, _a <= 0.04045f);
    return result;
}

SX_INLINE SX_CONSTFN float sx_color_togamma(float _a)
{
    const float lo     = _a * 12.92f;
    const float hi     = sx_pow(sx_abs(_a), 1.0f/2.4f) * 1.055f - 0.055f;
    const float result = sx_lerp(hi, lo, _a <= 0.0031308f);
    return result;
}

/// multiply vector3 into 4x4 matrix that [x/w, y/w, z/w, 1], used for projections, etc...
SX_INLINE void sx_vec3_mul_mat3(float* _result, const float* _vec, const float* _mat)
{
    _result[0] = _vec[0] * _mat[0] + _vec[1] * _mat[3] + _vec[2] * _mat[6];
    _result[1] = _vec[0] * _mat[1] + _vec[1] * _mat[4] + _vec[2] * _mat[7];
    _result[2] = _vec[0] * _mat[2] + _vec[1] * _mat[5] + _vec[2] * _mat[8];    
}

SX_INLINE void sx_vec2_mul_mat3(float* _result, const float* _vec, const float* _mat)
{
    _result[0] = _vec[0] * _mat[0] + _vec[1] * _mat[3] + _mat[6];
    _result[1] = _vec[0] * _mat[1] + _vec[1] * _mat[4] + _mat[7];
}

SX_INLINE void sx_mat3_ident(float* result) 
{
    sx_memset(result, 0x0, sizeof(float)*9);
    result[0] = 1.0f;
    result[4] = 1.0f;
    result[8] = 1.0f;
}

SX_INLINE void sx_mat3_translate(float* result, float x, float y)
{
    sx_memset(result, 0x00, sizeof(float) * 9);
    result[0] = 1.0f;
    result[4] = 1.0f;
    result[6] = x;
    result[7] = y;
    result[8] = 1.0f;
}

SX_INLINE void sx_mat3_rotate(float* result, float theta)
{
    sx_memset(result, 0x00, sizeof(float) * 9);
    float c = sx_cos(theta);
    float s = sx_sin(theta);
    result[0] = c;     result[1] = -s;
    result[3] = s;     result[4] = c;
    result[8] = 1.0f;
}

SX_INLINE void sx_mat3_scale(float* result, float sx, float sy)
{
    sx_memset(result, 0x00, sizeof(float) * 9);
    result[0] = sx;
    result[4] = sy;
    result[8] = 1.0f;
}

SX_INLINE void sx_mat3_SRT(float* _result, float _sx, float _sy, float _angle, float _tx, float _ty)
{
    sx_memset(_result, 0x00, sizeof(float) * 9);
    float c = sx_cos(_angle);
    float s = sx_sin(_angle);
    _result[0] = c*_sx;     _result[1] = -s;
    _result[3] = s;         _result[4] = c*_sy;
    _result[6] = _tx;       _result[7] = _ty;
    _result[8] = 1.0f;
}

SX_INLINE void sx_mat3_decompose(const float* _mat, float* translation, float* rotation)
{
    translation[0] = _mat[6];
    translation[1] = _mat[7];

    *rotation = sx_atan2(_mat[3], _mat[4]);
}

SX_INLINE float sx_vec2_dot(const float* _a, const float* _b)
{
    return _a[0]*_b[0] + _a[1]*_b[1];
}

SX_INLINE float sx_vec2_len(const float* _a)
{
    return sx_sqrt(sx_vec2_dot(_a, _a));
}

SX_INLINE float sx_vec2_norm(float* _result, const float* _a)
{
    const float len = sx_vec2_len(_a);
    if (len >= SX_NEAR_ZERO) {
        const float inv_len = 1.0f/len;
        _result[0] = _a[0] * inv_len;
        _result[1] = _a[1] * inv_len;
    }
    return len;
}

SX_INLINE void sx_vec2_min(float* _result, const float* _a, const float* _b)
{
    _result[0] = sx_min(_a[0], _b[0]);
    _result[1] = sx_min(_a[1], _b[1]);
}

SX_INLINE void sx_vec2_max(float* _result, const float* _a, const float* _b)
{
    _result[0] = sx_max(_a[0], _b[0]);
    _result[1] = sx_max(_a[1], _b[1]);
}

SX_INLINE void sx_vec2_lerp(float* _result, const float* _a, const float* _b, float _t)
{
    _result[0] = sx_lerp(_a[0], _b[0], _t);
    _result[1] = sx_lerp(_a[1], _b[1], _t);
}

SX_INLINE void sx_vec2_abs(float* _result, const float* _a)
{
    _result[0] = sx_abs(_a[0]);
    _result[1] = sx_abs(_a[1]);
}

SX_INLINE void sx_vec2_neg(float* _result, const float* _a)
{
    _result[0] = -_a[0];
    _result[1] = -_a[1];
}

SX_INLINE void sx_vec2_add(float* _result, const float* _a, const float* _b)
{
    _result[0] = _a[0] + _b[0];
    _result[1] = _a[1] + _b[1];
}

SX_INLINE void sx_vec2_addf(float* _result, const float* _a, float _b)
{
    _result[0] = _a[0] + _b;
    _result[1] = _a[1] + _b;
}

SX_INLINE void sx_vec2_sub(float* _result, const float* _a, const float* _b)
{
    _result[0] = _a[0] - _b[0];
    _result[1] = _a[1] - _b[1];
}

SX_INLINE void sx_vec2_subf(float* _result, const float* _a, float _b)
{
    _result[0] = _a[0] - _b;
    _result[1] = _a[1] - _b;
}

SX_INLINE void sx_vec2_mul(float* _result, const float* _a, const float* _b)
{
    _result[0] = _a[0] * _b[0];
    _result[1] = _a[1] * _b[1];
}

SX_INLINE void sx_vec2_mulf(float* _result, const float* _a, float _b)
{
    _result[0] = _a[0] * _b;
    _result[1] = _a[1] * _b;
}


#endif // SX_MATH_H_

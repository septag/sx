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

SX_CONST_FUNC bool sx_ispow2(int n);
SX_CONST_FUNC int sx_nearest_pow2(int n);

SX_CONST_FUNC float sx_torad(float _deg);
SX_CONST_FUNC float sx_todeg(float _rad);

// Packs float to uint32_t
SX_CONST_FUNC uint32_t sx_ftob(float _a);

// Unpacks float from uint32_t
SX_CONST_FUNC float sx_btof(uint32_t _a);

// Packs double to uint64_t
SX_CONST_FUNC uint64_t sx_dtob(double _a);

// Unpacks uint64_t to double
SX_CONST_FUNC double sx_btod(uint64_t _a);

// Returns sortable bit packed float value
SX_CONST_FUNC uint32_t sx_fflip(uint32_t _value);

SX_CONST_FUNC bool sx_isnan(float _f);

SX_CONST_FUNC bool sx_isnand(double _f);

SX_CONST_FUNC bool sx_isfin(float _f);

SX_CONST_FUNC bool sx_isfind(double _f);

SX_CONST_FUNC bool sx_isinf(float _f);

SX_CONST_FUNC bool sx_isinfd(double _f);

SX_CONST_FUNC float sx_floor(float _f);

SX_CONST_FUNC float sx_round(float _f);

SX_CONST_FUNC float sx_ceil(float _f);

SX_CONST_FUNC float sx_lerp(float _a, float _b, float _t);

SX_CONST_FUNC float sx_sign(float _a);

SX_CONST_FUNC float sx_abs(float _a);

SX_CONST_FUNC float sx_sin(float _a);

SX_CONST_FUNC float sx_sinh(float _a);

SX_CONST_FUNC float sx_asin(float _a);

SX_CONST_FUNC float sx_cos(float _a);

SX_CONST_FUNC float sx_cosh(float _a);

SX_CONST_FUNC float sx_acos(float _a);

SX_CONST_FUNC float sx_tan(float _a);

SX_CONST_FUNC float sx_tanh(float _a);

SX_CONST_FUNC float sx_atan(float _a);

SX_CONST_FUNC float sx_atan2(float _y, float _x);

SX_CONST_FUNC float sx_pow(float _a, float _b);

SX_CONST_FUNC float sx_exp(float _a);

SX_CONST_FUNC float sx_exp2(float _a);

SX_CONST_FUNC float sx_log(float _a);

SX_CONST_FUNC float sx_log2(float _a);

SX_CONST_FUNC float sx_sqrt(float _a);

SX_CONST_FUNC float sx_rsqrt(float _a);

// Returns the nearest integer not greater in magnitude than _a.
SX_CONST_FUNC float sx_trunc(float _a);

// Returns the fractional (or decimal) part of _a, which is 0~1
SX_CONST_FUNC float sx_fract(float _a);

// Returns the floating-point remainder of the division operation _a/_b.
SX_CONST_FUNC float sx_mod(float _a, float _b);

SX_CONST_FUNC bool sx_equal(float _a, float _b, float _epsilon);

SX_CONST_FUNC bool sx_equal_arr(const float* _a, const float* _b, int _num, float _epsilon);

SX_CONST_FUNC float sx_wrap(float _a, float _wrap);

// Returns 0 if _a < _edge, else 1
SX_CONST_FUNC float sx_step(float _edge, float _a);

// smooth Hermite interpolation (result = 0 ~ 1) when edge0 < x < edge1
SX_CONST_FUNC float sx_smoothstep(float _edge1, float _edge2, float _a);

// Makes a pulse signal (0-1), which between _start and _end is 1, other ranges are 0
SX_CONST_FUNC float sx_pulse(float _a, float _start, float _end);

// References:
//  - Bias And Gain Are Your Friend
//    http://blog.demofox.org/2012/09/24/bias-and-gain-are-your-friend/
//  - http://demofox.org/biasgain.html
SX_CONST_FUNC float sx_bias(float _time, float _bias);

SX_CONST_FUNC float sx_gain(float _time, float _gain);

SX_CONST_FUNC float sx_angle_diff(float _a, float _b);

SX_CONST_FUNC float sx_angle_lerp(float _a, float _b, float _t);

////////////////////////////////////////////////////////////////////////////////////////////////////
// Vector math
void sx_vec2_abs(float* _result, const float* _a);
void sx_vec2_neg(float* _result, const float* _a);
void sx_vec2_add(float* _result, const float* _a, const float* _b);
void sx_vec2_addf(float* _result, const float* _a, float _b);
void sx_vec2_sub(float* _result, const float* _a, const float* _b);
void sx_vec2_subf(float* _result, const float* _a, float _b);
void sx_vec2_mul(float* _result, const float* _a, const float* _b);
void sx_vec2_mulf(float* _result, const float* _a, float _b);
float sx_vec2_dot(const float* _a, const float* _b);
float sx_vec2_len(const float* _a);
void sx_vec2_lerp(float* _result, const float* _a, const float* _b, float _t);
float sx_vec2_norm(float* _result, const float* _a);
void sx_vec2_min(float* _result, const float* _a, const float* _b);
void sx_vec2_max(float* _result, const float* _a, const float* _b);
void sx_vec2_rcp(float* _result, const float* _a);
void sx_vec2_mul_mat3(float* _result, const float* _vec, const float* _mat);

void sx_vec3_abs(float* _result, const float* _a);
void sx_vec3_neg(float* _result, const float* _a);
void sx_vec3_add(float* _result, const float* _a, const float* _b);
void sx_vec3_addf(float* _result, const float* _a, float _b);
void sx_vec3_sub(float* _result, const float* _a, const float* _b);
void sx_vec3_subf(float* _result, const float* _a, float _b);
void sx_vec3_mul(float* _result, const float* _a, const float* _b);
void sx_vec3_mulf(float* _result, const float* _a, float _b);
float sx_vec3_dot(const float* _a, const float* _b);
void sx_vec3_cross(float* _result, const float* _a, const float* _b);
float sx_vec3_len(const float* _a);
void sx_vec3_lerp(float* _result, const float* _a, const float* _b, float _t);
float sx_vec3_norm(float* _result, const float* _a);
void sx_vec3_min(float* _result, const float* _a, const float* _b);
void sx_vec3_max(float* _result, const float* _a, const float* _b);
void sx_vec3_rcp(float* _result, const float* _a);
void sx_vec3_tangent(const float* _n, float* _t, float* _b);
void sx_vec3_tangent_angle(const float* _n, float _angle, float* _t, float* _b);
void sx_vec3_fromlatlong(float* _vec, float _u, float _v);
void sx_vec3_tolatlong(float* _outu, float* _outv, const float* _dir);
void sx_vec3_mul_quat(float* _result, const float* _vec, const float* _quat);
/// multiply vector3 into 4x4 matrix without considering 4th column, which is not used in transform matrices
void sx_vec3_mul_mat4(float* _result, const float* _vec, const float* _mat);
/// multiply vector3 into rotation part of the matrix only (used for normal vectors, etc...)
void sx_vec3_mul_mat4_xyz0(float* _result, const float* _vec, const float* _mat);
/// multiply vector3 into 4x4 matrix that [x/w, y/w, z/w, 1], used for projections, etc...
void sx_vec3_mul_mat3(float* _result, const float* _vec, const float* _mat);
void sx_vec3_mul_mat4H(float* _result, const float* _vec, const float* _mat);
void sx_vec3_calc_normal(float _result[3], float _va[3], float _vb[3], float _vc[3]);
void sx_vec3_calc_plane(float _result[4], float _va[3], float _vb[3], float _vc[3]);
void sx_vec3_calc_linearfit2D(float _result[2], const void* _points, int _stride, int _num);
void sx_vec3_calc_linearfit3D(float _result[3], const void* _points, int _stride, int _num);

void sx_quat_ident(float* _result);
void sx_quat_mulXYZ(float* _result, const float* _qa, const float* _qb);
void sx_quat_mul(float* _result, const float* _qa, const float* _qb);
void sx_quat_inv(float* _result, const float* _quat);
float sx_quat_dot(const float* _a, const float* _b);
void sx_quat_norm(float* _result, const float* _quat);
void sx_quat_toeuler(float* _result, const float* _quat);
void sx_quat_rotateaxis(float* _result, const float* _axis, float _angle);
void sx_quat_rotateX(float* _result, float _ax);
void sx_quat_rotateY(float* _result, float _ay);
void sx_quat_rotateZ(float* _result, float _az);

void sx_mat4_ident(float* _result);
void sx_mat4_translate(float* _result, float _tx, float _ty, float _tz);
void sx_mat4_scale(float* _result, float _sx, float _sy, float _sz);
void sx_mat4_scalef(float* _result, float _scale);
void sx_mat4_from_normal(float* _result, const float* _normal, float _scale, const float* _pos);
void sx_mat4_from_normal_angle(float* _result, const float* _normal, float _scale, const float* _pos, float _angle);
void sx_mat4_quat(float* _result, const float* _quat);
void sx_mat4_quat_translate(float* _result, const float* _quat, const float* _translation);
void sx_mat4_quat_translate_HMD(float* _result, const float* _quat, const float* _translation);
void sx_mat4_lookatLH(float* _result, const float* _eye, const float* _at, const float* _up);
void sx_mat4_lookatRH(float* _result, const float* _eye, const float* _at, const float* _up);
void sx_mat4_lookat(float* _result, const float* _eye, const float* _at, const float* _up);
void sx_mat4_proj(float* _result, float _ut, float _dt, float _lt, float _rt, float _near, float _far, bool _oglNdc);
void sx_mat4_proj_fov(float* _result, const float _fov[4], float _near, float _far, bool _oglNdc);
void sx_mat4_proj_fovY(float* _result, float _fovy, float _aspect, float _near, float _far, bool _oglNdc);
void sx_mat4_projLH(float* _result, float _ut, float _dt, float _lt, float _rt, float _near, float _far, bool _oglNdc);
void sx_mat4_projLH_fov(float* _result, const float _fov[4], float _near, float _far, bool _oglNdc);
void sx_mat4_projLH_fovY(float* _result, float _fovy, float _aspect, float _near, float _far, bool _oglNdc);
void sx_mat4_projRH(float* _result, float _ut, float _dt, float _lt, float _rt, float _near, float _far, bool _oglNdc);
void sx_mat4_projRH_fov(float* _result, const float _fov[4], float _near, float _far, bool _oglNdc);
void sx_mat4_projRH_fovY(float* _result, float _fovy, float _aspect, float _near, float _far, bool _oglNdc);
void sx_mat4_proj_inf_fov(float* _result, const float _fov[4], float _near, bool _oglNdc);
void sx_mat4_proj_inf(float* _result, float _ut, float _dt, float _lt, float _rt, float _near, bool _oglNdc);
void sx_mat4_proj_inf_fovY(float* _result, float _fovy, float _aspect, float _near, bool _oglNdc);
void sx_mat4_proj_infLH(float* _result, float _ut, float _dt, float _lt, float _rt, float _near, bool _oglNdc);
void sx_mat4_proj_infLH_fov(float* _result, const float _fov[4], float _near, bool _oglNdc);
void sx_mat4_proj_infLH_fovY(float* _result, float _fovy, float _aspect, float _near, bool _oglNdc);
void sx_mat4_proj_infRH(float* _result, float _ut, float _dt, float _lt, float _rt, float _near, bool _oglNdc);
void sx_mat4_proj_infRH_fov(float* _result, const float _fov[4], float _near, bool _oglNdc);
void sx_mat4_proj_infRH_fovY(float* _result, float _fovy, float _aspect, float _near, bool _oglNdc);
void sx_mat4_projrev_infLH(float* _result, float _ut, float _dt, float _lt, float _rt, float _near, bool _oglNdc);
void sx_mat4_projrev_infLH_fov(float* _result, const float _fov[4], float _near, bool _oglNdc);
void sx_mat4_projrev_infLH_fovY(float* _result, float _fovy, float _aspect, float _near, bool _oglNdc);
void sx_mat4_projrev_infRH(float* _result, float _ut, float _dt, float _lt, float _rt, float _near, bool _oglNdc);
void sx_mat4_projrev_infRH_fov(float* _result, const float _fov[4], float _near, bool _oglNdc);
void sx_mat4_projrev_infRH_fovY(float* _result, float _fovy, float _aspect, float _near, bool _oglNdc);
void sx_mat4_ortho(float* _result, float _left, float _right, float _bottom, float _top, float _near, float _far, float _offset, bool _oglNdc);
void sx_mat4_orthoLH(float* _result, float _left, float _right, float _bottom, float _top, float _near, float _far, float _offset, bool _oglNdc);
void sx_mat4_orthoRH(float* _result, float _left, float _right, float _bottom, float _top, float _near, float _far, float _offset, bool _oglNdc);
void sx_mat4_rotateX(float* _result, float _ax);
void sx_mat4_rotateY(float* _result, float _ay);
void sx_mat4_rotateZ(float* _result, float _az);
void sx_mat4_rotateXY(float* _result, float _ax, float _ay);
void sx_mat4_rotateXYZ(float* _result, float _ax, float _ay, float _az);
void sx_mat4_rotateZYX(float* _result, float _ax, float _ay, float _az);
void sx_mat4_SRT(float* _result, float _sx, float _sy, float _sz, float _ax, float _ay, float _az, float _tx, float _ty, float _tz);
void sx_mat4_mul(float* _result, const float* _a, const float* _b);
void sx_mat4_transpose(float* _result, const float* _a);
void sx_mat4_inv(float* _result, const float* _a);
/// Convert LH to RH projection matrix and vice versa.
void sx_mat4_proj_flip_handedness(float* _dst, const float* _src);
/// Convert LH to RH view matrix and vice versa.
void sx_mat4_view_flip_handedness(float* _dst, const float* _src);
/// Inverse for transform-only matrices (column4=0) (mat4x)
void sx_mat4x_inv(float* _result, const float* _a);
void sx_mat4_calc_quat(float* _result, const float* _mat);

void sx_vec4_mul(float* _result, const float* _a, const float* _b);
void sx_vec4_mulf(float* _result, const float* _a, float _b);
void sx_vec4_mul_mat4(float* _result, const float* _vec, const float* _mat);

void sx_mat3_inv(float* _result, const float* _a);
void sx_mat3_mul(float* _result, const float* _a, const float* _b);
void sx_mat3_ident(float* _result);
void sx_mat3_translate(float* _result, float _tx, float _ty);
void sx_mat3_rotate(float* _result, float _angle);
void sx_mat3_scale(float* _result, float _sx, float _sy);
void sx_mat3_SRT(float* _result, float _sx, float _sy, float _angle, float _tx, float _ty);
void sx_mat3_decompose(const float* _mat, float* translation, float* rotation);

void sx_color_RGBtoHSV(float _hsv[3], const float _rgb[3]);
void sx_color_HSVtoRGB(float _rgb[3], const float _hsv[3]);
SX_CONST_FUNC float sx_color_tolinear(float _a);
SX_CONST_FUNC float sx_color_togamma(float _a);

#include "inline/math.inl"

#endif // SX_MATH_H_

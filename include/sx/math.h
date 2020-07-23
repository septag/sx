//
// Copyright 2018 Sepehr Taghdisian (septag@github). All rights reserved.
// License: https://github.com/septag/sx#license-bsd-2-clause
//
// parts of this code is copied from bx library: https://github.com/bkaradzic/bx
// Copyright 2011-2019 Branimir Karadzic. All rights reserved.
// License: https://github.com/bkaradzic/bx#license-bsd-2-clause
//
//
// math.h - 1.3    Scalar and Vector math functions
//                 Contains vector primitives and vector/fpu math functions, event functions implemented in libm
//                 Many functions are taken from bx library (https://github.com/bkaradzic/bx)
// Easings:
//      Reference: https://easings.net/
//                 https://github.com/r-lyeh-archived/tween
//
// Conventions:
//      The lib prefers Right-Handed system, although there are functions for both LH or RH system
//      for calulating view/projection matrices
//      Rotations are CCW
//      Matrices are Column-Major, but the representation is row-major, which means:
//          mat->m[r][c] -> which R is the row, and C is column index
//          transform vector (v) by matrix (M) = M.v
//          matrix transforms are multiplied in reverse: Scale->Rotation->Translate = TxRxS
//
//
#pragma once

#include "sx.h"

// Some math constants
#define SX_PI 3.1415926535897932384626433832795f
#define SX_PI2 6.2831853071795864769252867665590f
#define SX_INVPI 1.0f / 3.1415926535897932384626433832795f
#define SX_PIHALF 1.5707963267948966192313216916398f
#define SX_PIQUARTER 0.7853981633974483096156608458199f
#define SX_SQRT2 1.4142135623730950488016887242097f
#define SX_LOG_NAT10 2.3025850929940456840179914546844f
#define SX_INVLOG_NAT2 1.4426950408889634073599246810019f
#define SX_LOG_NAT2H 0.6931471805599453094172321214582f
#define SX_LOG_NAT2L 1.90821492927058770002e-10f
#define SX_E 2.7182818284590452353602874713527f
#define SX_NEAR_ZERO (1.0f / (float)(1 << 28))
#define SX_FLOAT_MIN 1.175494e-38f
#define SX_FLOAT_MAX 3.402823e+38f

////////////////////////////////////////////////////////////////////////////////////////////////////
// Types/Primitives
typedef union sx_vec4 {
    struct {
        float x;
        float y;
        float z;
        float w;
    };

    float f[4];
} sx_vec4;

typedef union sx_vec2 {
    struct {
        float x;
        float y;
    };

    float f[2];
} sx_vec2;

typedef union sx_vec3 {
    struct {
        float x;
        float y;
        float z;
    };

    float f[3];
} sx_vec3;

typedef union sx_color {
    struct {
        unsigned char r;
        unsigned char g;
        unsigned char b;
        unsigned char a;
    };

    unsigned int n;
} sx_color;


typedef union sx_ivec2 {
    struct {
        int x;
        int y;
    };

    int n[2];
} sx_ivec2;

typedef union sx_quat {
    struct {
        float x;
        float y;
        float z;
        float w;
    };

    float f[4];
} sx_quat;

typedef union sx_mat3 {
    struct {
        float m11, m21, m31;
        float m12, m22, m32;
        float m13, m23, m33;
    };

    struct {
        float fc1[3];
        float fc2[3];
        float fc3[3];
    };

    struct {
        sx_vec3 col1;
        sx_vec3 col2;
        sx_vec3 col3;
    };

    float f[9];
} sx_mat3;

typedef union sx_mat4 {
    struct {
        float m11, m21, m31, m41;
        float m12, m22, m32, m42;
        float m13, m23, m33, m43;
        float m14, m24, m34, m44;
    };

    struct {
        float rc1[4];
        float rc2[4];
        float rc3[4];
        float rc4[4];
    };

    struct {
        sx_vec4 col1;
        sx_vec4 col2;
        sx_vec4 col3;
        sx_vec4 col4;
    };

    float f[16];
} sx_mat4;

typedef union sx_rect {
    struct {
        float xmin, ymin;
        float xmax, ymax;
    };

    struct {
        sx_vec2 vmin;
        sx_vec2 vmax;
    };

    float f[4];
} sx_rect;

typedef union sx_irect {
    struct {
        int xmin, ymin;
        int xmax, ymax;
    };

    struct {
        sx_ivec2 vmin;
        sx_ivec2 vmax;
    };

    int f[4];
} sx_irect;

typedef union sx_aabb {
    struct {
        float xmin, ymin, zmin;
        float xmax, ymax, zmax;
    };

    struct {
        sx_vec3 vmin;
        sx_vec3 vmax;
    };

    float f[6];
} sx_aabb;

// 3d plane: a*nx + b*ny + c*nz + d = 0
typedef union sx_plane {
    sx_vec4 p;

    struct {
        sx_vec3 normal;
        float dist;
    };

    float f[4];
} sx_plane;

// Simplified 3D transform. by position and rotation
typedef struct sx_tx3d {
    sx_vec3 pos;
    sx_mat3 rot;
} sx_tx3d;

// Box is a 3d primitive (cube), that extents in each X,Y,Z direction and has arbitary transform
// This is different from AABB (sx_aabb) where it is axis-aligned and defined by min/max points
typedef struct sx_box {
    sx_tx3d tx;   // transform (pos = origin of the box, rot = rotation of the box)
    sx_vec3 he;   // half-extent from the origin (0.5*width, 0.5*height, 0.5f*depth)
} sx_box;

SX_API SX_CONSTFN float sx_floor(float _f);
SX_API SX_CONSTFN float sx_cos(float _a);
SX_API SX_CONSTFN float sx_acos(float _a);
SX_API SX_CONSTFN float sx_sin(float _a);
SX_API SX_CONSTFN float sx_asin(float _a);
SX_API SX_CONSTFN float sx_atan2(float _y, float _x);
SX_API SX_CONSTFN float sx_exp(float _a);
SX_API SX_CONSTFN float sx_log(float _a);
SX_API SX_CONSTFN float sx_sqrt(float _a);
SX_API SX_CONSTFN float sx_rsqrt(float _a);

SX_API sx_vec3 sx_plane_normal(const sx_vec3 _va, const sx_vec3 _vb, const sx_vec3 _vc);
SX_API sx_plane sx_plane3p(const sx_vec3 _va, const sx_vec3 _vb, const sx_vec3 _vc);
SX_API sx_plane sx_planenp(const sx_vec3 _normal, const sx_vec3 _p);
SX_API float sx_plane_distance(const sx_plane _plane, const sx_vec3 _p);
SX_API sx_vec3 sx_plane_project_point(const sx_plane _plane, const sx_vec3 _p);
SX_API sx_vec3 sx_plane_origin(const sx_plane _plane);

SX_API sx_vec2 sx_vec2_calc_linearfit2D(const sx_vec2* _points, int _num);
SX_API sx_vec3 sx_vec3_calc_linearfit3D(const sx_vec3* _points, int _num);

SX_API sx_mat4 sx_mat4_from_normal(const sx_vec3 _normal, float _scale, const sx_vec3 _pos);
SX_API sx_mat4 sx_mat4_from_normal_angle(const sx_vec3 _normal, float _scale, const sx_vec3 _pos,
                                         float _angle);
SX_API sx_mat4 sx_mat4_view_lookat(const sx_vec3 eye, const sx_vec3 target, const sx_vec3 up);
SX_API sx_mat4 sx_mat4_view_lookatLH(const sx_vec3 eye, const sx_vec3 target, const sx_vec3 up);
// https://www.3dgep.com/understanding-the-view-matrix/
SX_API sx_mat4 sx_mat4_view_FPS(const sx_vec3 eye, float pitch, float yaw);
SX_API sx_mat4 sx_mat4_view_arcball(const sx_vec3 move, const sx_quat rot,
                                    const sx_vec3 target_pos);
// https://docs.microsoft.com/en-us/windows/desktop/direct3d9/d3dxmatrixperspectivefovlh
SX_API sx_mat4 sx_mat4_perspective(float width, float height, float zn, float zf, bool ogl_ndc);
SX_API sx_mat4 sx_mat4_perspectiveLH(float width, float height, float zn, float zf, bool ogl_ndc);
SX_API sx_mat4 sx_mat4_perspective_offcenter(float xmin, float ymin, float xmax, float ymax,
                                             float zn, float zf, bool ogl_ndc);
SX_API sx_mat4 sx_mat4_perspective_offcenterLH(float xmin, float ymin, float xmax, float ymax,
                                               float zn, float zf, bool ogl_ndc);
SX_API sx_mat4 sx_mat4_perspectiveFOV(float fov_y, float aspect, float zn, float zf, bool ogl_ndc);
SX_API sx_mat4 sx_mat4_perspectiveFOV_LH(float fov_y, float aspect, float zn, float zf,
                                         bool ogl_ndc);
SX_API sx_mat4 sx_mat4_ortho(float width, float height, float zn, float zf, float offset,
                             bool ogl_ndc);
SX_API sx_mat4 sx_mat4_orthoLH(float width, float height, float zn, float zf, float offset,
                               bool ogl_ndc);
SX_API sx_mat4 sx_mat4_ortho_offcenter(float xmin, float ymin, float xmax, float ymax, float zn,
                                       float zf, float offset, bool ogl_ndc);
SX_API sx_mat4 sx_mat4_ortho_offcenterLH(float xmin, float ymin, float xmax, float ymax, float zn,
                                         float zf, float offset, bool ogl_ndc);
SX_API sx_mat4 sx_mat4_SRT(float _sx, float _sy, float _sz, float _ax, float _ay, float _az,
                           float _tx, float _ty, float _tz);
SX_API sx_mat4 sx_mat4_mul(const sx_mat4* _a, const sx_mat4* _b);
SX_API sx_mat4 sx_mat4_inv(const sx_mat4* _a);
/// Inverse for transform-only matrices (column4=0) (mat4x)
SX_API sx_mat4 sx_mat4x_inv(const sx_mat4* _a);
SX_API sx_quat sx_mat4_quat(const sx_mat4* _mat);
SX_API sx_mat4 sx_mat4_project_plane(const sx_vec3 plane_normal);

SX_API sx_mat3 sx_mat3_inv(const sx_mat3* _a);
SX_API sx_mat3 sx_mat3_mul(const sx_mat3* _a, const sx_mat3* _b);

SX_API sx_mat3 sx_quad_mat3(const sx_quat quat);
SX_API sx_mat4 sx_quat_mat4(const sx_quat quat);

SX_API void sx_color_RGBtoHSV(float _hsv[3], const float _rgb[3]);
SX_API void sx_color_HSVtoRGB(float _rgb[3], const float _hsv[3]);

SX_API sx_aabb sx_aabb_transform(const sx_aabb* aabb, const sx_mat4* mat);


// https://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2
static inline SX_CONSTFN int sx_nearest_pow2(int n)
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

static inline SX_CONSTFN bool sx_ispow2(int n)
{
    return (n & (n - 1)) == 0;
}

static inline SX_CONSTFN float sx_torad(float _deg)
{
    return _deg * SX_PI / 180.0f;
}

static inline SX_CONSTFN float sx_todeg(float _rad)
{
    return _rad * 180.0f / SX_PI;
}

// Packs float to unsigned int
static inline SX_CONSTFN unsigned int sx_ftob(float _a)
{
    union {
        float f;
        unsigned int ui;
    } u = { _a };
    return u.ui;
}

// Unpacks float from unsigned int
static inline SX_CONSTFN float sx_btof(unsigned int _a)
{
    union {
        unsigned int ui;
        float f;
    } u = { _a };
    return u.f;
}

// Packs double to uint64_t
static inline SX_CONSTFN uint64_t sx_dtob(double _a)
{
    union {
        double f;
        uint64_t ui;
    } u = { _a };
    return u.ui;
}

// Unpacks uint64_t to double
static inline SX_CONSTFN double sx_btod(uint64_t _a)
{
    union {
        uint64_t ui;
        double f;
    } u = { _a };
    return u.f;
}

// Returns sortable bit packed float value
// http://archive.fo/2012.12.08-212402/http://stereopsis.com/radix.html
static inline SX_CONSTFN unsigned int sx_fflip(unsigned int _value)
{
    unsigned int mask = -((int32_t)(_value >> 31)) | 0x80000000;
    return _value ^ mask;
}

static inline SX_CONSTFN bool sx_isnan(float _f)
{
    const unsigned int tmp = sx_ftob(_f) & INT32_MAX;
    return tmp > UINT32_C(0x7f800000);
}

static inline SX_CONSTFN bool sx_isnand(double _f)
{
    const uint64_t tmp = sx_dtob(_f) & INT64_MAX;
    return tmp > UINT64_C(0x7ff0000000000000);
}

static inline SX_CONSTFN bool sx_isfin(float _f)
{
    const unsigned int tmp = sx_ftob(_f) & INT32_MAX;
    return tmp < UINT32_C(0x7f800000);
}

static inline SX_CONSTFN bool sx_isfind(double _f)
{
    const uint64_t tmp = sx_dtob(_f) & INT64_MAX;
    return tmp < UINT64_C(0x7ff0000000000000);
}

static inline SX_CONSTFN bool sx_isinf(float _f)
{
    const unsigned int tmp = sx_ftob(_f) & INT32_MAX;
    return tmp == UINT32_C(0x7f800000);
}

static inline SX_CONSTFN bool sx_isinfd(double _f)
{
    const uint64_t tmp = sx_dtob(_f) & INT64_MAX;
    return tmp == UINT64_C(0x7ff0000000000000);
}

static inline SX_CONSTFN float sx_round(float _f)
{
    return sx_floor(_f + 0.5f);
}

static inline SX_CONSTFN float sx_ceil(float _f)
{
    return -sx_floor(-_f);
}

static inline SX_CONSTFN float sx_lerp(float _a, float _b, float _t)
{
    // this version is more precise than: _a + (_b - _a) * _t
    return (1.0f - _t) * _a + _t * _b;
}

static inline SX_CONSTFN float sx_sign(float _a)
{
    return _a < 0.0f ? -1.0f : 1.0f;
}

static inline SX_CONSTFN float sx_abs(float _a)
{
    union {
        float f;
        unsigned int ui;
    } u = { _a };
    u.ui &= 0x7FFFFFFF;
    return u.f;
}

static inline SX_CONSTFN float sx_tan(float _a)
{
    return sx_sin(_a) / sx_cos(_a);
}

static inline SX_CONSTFN float sx_sinh(float _a)
{
    return 0.5f * (sx_exp(_a) - sx_exp(-_a));
}

static inline SX_CONSTFN float sx_cosh(float _a)
{
    return 0.5f * (sx_exp(_a) + sx_exp(-_a));
}

static inline SX_CONSTFN float sx_tanh(float _a)
{
    const float tmp0 = sx_exp(2.0f * _a);
    const float tmp1 = tmp0 - 1.0f;
    const float tmp2 = tmp0 + 1.0f;
    const float result = tmp1 / tmp2;

    return result;
}

static inline SX_CONSTFN float sx_atan(float _a)
{
    return sx_atan2(_a, 1.0f);
}

static inline SX_CONSTFN float sx_pow(float _a, float _b)
{
    return sx_exp(_b * sx_log(_a));
}

static inline SX_CONSTFN float sx_exp2(float _a)
{
    return sx_pow(2.0f, _a);
}

static inline SX_CONSTFN float sx_log2(float _a)
{
    return sx_log(_a) * SX_INVLOG_NAT2;
}

// Returns the nearest integer not greater in magnitude than _a.
static inline SX_CONSTFN float sx_trunc(float _a)
{
    return (float)((int)_a);
}

// Returns the fractional (or decimal) part of _a, which is 0~1
static inline SX_CONSTFN float sx_fract(float _a)
{
    return _a - sx_trunc(_a);
}

// Returns the floating-point remainder of the division operation _a/_b.
static inline SX_CONSTFN float sx_mod(float _a, float _b)
{
    return _a - _b * sx_floor(_a / _b);
}

// http://realtimecollisiondetection.net/blog/?t=89
SX_PRAGMA_DIAGNOSTIC_PUSH()
SX_PRAGMA_DIAGNOSTIC_IGNORED_CLANG_GCC("-Wshadow")
static inline SX_CONSTFN bool sx_equal(float _a, float _b, float _epsilon)
{
    const float lhs = sx_abs(_a - _b);
    float aa = sx_abs(_a);
    float ab = sx_abs(_b);
    const float rhs = _epsilon * sx_max(1.0f, sx_max(aa, ab));
    return lhs <= rhs;
}
SX_PRAGMA_DIAGNOSTIC_POP()

static inline SX_CONSTFN bool sx_equal_arr(const float* _a, const float* _b, int _num,
                                           float _epsilon)
{
    bool result = sx_equal(_a[0], _b[0], _epsilon);
    for (int ii = 1; result && ii < _num; ++ii) {
        result = sx_equal(_a[ii], _b[ii], _epsilon);
    }
    return result;
}

static inline SX_CONSTFN float sx_wrap(float _a, float _wrap)
{
    const float tmp0 = sx_mod(_a, _wrap);
    const float result = tmp0 < 0.0f ? _wrap + tmp0 : tmp0;
    return result;
}

static inline SX_CONSTFN float sx_wrap_range(float x, float fmin, float fmax)
{
    return sx_mod(x, fmax - fmin) + fmin;
}

static inline SX_CONSTFN int sx_iwrap_range(int x, int imin, int imax)
{
    int range = imax - imin + 1;
    if (x < imin)
        x += range * ((imin - x) / range + 1);
    return imin + (x - imin) % range;
}

// Returns 0 if _a < _edge, else 1
static inline SX_CONSTFN float sx_step(float _a, float _edge)
{
    return _a < _edge ? 0.0f : 1.0f;
}

static inline SX_CONSTFN float sx_pulse(float _a, float _start, float _end)
{
    return sx_step(_a, _start) - sx_step(_a, _end);
}

static inline SX_CONSTFN float sx_saturate(float _n)
{
    return sx_clamp(_n, 0.0f, 1.0f);
}

// smooth Hermite interpolation (result = 0..1) when edge0 < x < edge1
static inline SX_CONSTFN float sx_smoothstep(float _a, float _edge1, float _edge2)
{
    sx_assert(_edge1 < _edge2);
    float a = sx_saturate((_a - _edge1) / (_edge2 - _edge1));
    return a * a * (3.0f - 2.0f * a);
}

// like smoothstep but linear
// result is 0..1 when in _min.._max range, 0 if less than _min, 1 if more than _max
static inline SX_CONSTFN float sx_linearstep(float t, float _min, float _max)
{
    sx_assert(_min < _max);
    return sx_saturate((t - _min) / (_max - _min));
}

// used for normalizing time values to 0..1
// based on the assumption that time 't' starts from 0.._max or more than that
static inline SX_CONSTFN float sx_normalize_time(float t, float _max)
{
    sx_assert(_max > 0);
    float nt = t / _max;
    return nt < 1.0f ? nt : 1.0f;
}


// References:
//  - Bias And Gain Are Your Friend
//    http://blog.demofox.org/2012/09/24/bias-and-gain-are-your-friend/
//  - http://demofox.org/biasgain.html
static inline SX_CONSTFN float sx_bias(float _time, float _bias)
{
    return _time / (((1.0f / _bias - 2.0f) * (1.0f - _time)) + 1.0f);
}

static inline SX_CONSTFN float sx_gain(float _time, float _gain)
{
    if (_time < 0.5f)
        return sx_bias(_time * 2.0f, _gain) * 0.5f;

    return sx_bias(_time * 2.0f - 1.0f, 1.0f - _gain) * 0.5f + 0.5f;
}

static inline SX_CONSTFN float sx_angle_diff(float _a, float _b)
{
    const float dist = sx_wrap(_b - _a, SX_PI2);
    return sx_wrap(dist * 2.0f, SX_PI2) - dist;
}

static inline SX_CONSTFN float sx_angle_lerp(float _a, float _b, float _t)
{
    return _a + sx_angle_diff(_a, _b) * _t;
}

//
static inline sx_vec3 sx_vec3f(float _x, float _y, float _z)
{
#ifdef __cplusplus
    return { { _x, _y, _z } };
#else
    return (sx_vec3){ .x = _x, .y = _y, .z = _z };
#endif
}

static inline sx_vec3 sx_vec3fv(const float* _f)
{
#ifdef __cplusplus
    return { { _f[0], _f[1], _f[2] } };
#else
    return (sx_vec3){ .x = _f[0], .y = _f[1], .z = _f[2] };
#endif
}

static inline sx_vec3 sx_vec3v2(const sx_vec2 _vec, float _z)
{
    return sx_vec3f(_vec.x, _vec.y, _z);
}

static inline sx_vec3 sx_vec3splat(float _f)
{
    return sx_vec3f(_f, _f, _f);
}

//
static inline sx_vec2 sx_vec2f(float _x, float _y)
{
#ifdef __cplusplus
    return { { _x, _y } };
#else
    return (sx_vec2){ .x = _x, .y = _y };
#endif
}

static inline sx_vec2 sx_vec2fv(const float* _f)
{
#ifdef __cplusplus
    return { { _f[0], _f[1] } };
#else
    return (sx_vec2){ .x = _f[0], .y = _f[1] };
#endif
}

static inline sx_vec2 sx_vec2splat(float _f)
{
    return sx_vec2f(_f, _f);
}

//
static inline sx_vec4 sx_vec4f(float _x, float _y, float _z, float _w)
{
#ifdef __cplusplus
    return { { _x, _y, _z, _w } };
#else
    return (sx_vec4){ .x = _x, .y = _y, .z = _z, .w = _w };
#endif
}

static inline sx_vec4 sx_vec4fv(const float* _f)
{
#ifdef __cplusplus
    return { { _f[0], _f[1], _f[2], _f[3] } };
#else
    return (sx_vec4){ .x = _f[0], .y = _f[1], .z = _f[2], .w = _f[3] };
#endif
}

static inline sx_vec4 sx_vec4v3(const sx_vec3 _vec, float _w)
{
    return sx_vec4f(_vec.x, _vec.y, _vec.z, _w);
}

static inline sx_vec4 sx_vec4splat(float _f)
{
    return sx_vec4f(_f, _f, _f, _f);
}

//
static inline sx_ivec2 sx_ivec2i(int _x, int _y)
{
#ifdef __cplusplus
    return { { _x, _y } };
#else
    return (sx_ivec2){ .x = _x, .y = _y };
#endif
}

static inline sx_ivec2 sx_ivec2iv(const int* _f)
{
#ifdef __cplusplus
    return { { _f[0], _f[1] } };
#else
    return (sx_ivec2){ .x = _f[0], .y = _f[1] };
#endif
}

static inline sx_ivec2 sx_ivec2splat(int _f)
{
    return sx_ivec2i(_f, _f);
}

//
static inline sx_color sx_color4u(unsigned char _r, unsigned char _g, unsigned char _b,
                                  unsigned char _a sx_default(255))
{
#ifdef __cplusplus
    return { { _r, _g, _b, _a } };
#else
    return (sx_color){ .r = _r, .g = _g, .b = _b, .a = _a };
#endif
}

static inline sx_color sx_color4f(float _r, float _g, float _b, float _a sx_default(1.0f))
{
    return sx_color4u((unsigned char)(_r * 255.0f), (unsigned char)(_g * 255.0f),
                      (unsigned char)(_b * 255.0f), (unsigned char)(_a * 255.0f));
}

static inline sx_color sx_colorn(unsigned int _n)
{
#ifdef __cplusplus
    sx_color c;
    c.n = _n;
    return c;
#else
    return (sx_color){ .n = _n };
#endif
}

static inline sx_vec4 sx_color_vec4(sx_color c)
{
    float rcp = 1.0f / 255.0f;
    return sx_vec4f((float)c.r * rcp, (float)c.g * rcp, (float)c.b * rcp, (float)c.a * rcp);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Quaternion
static inline sx_quat sx_quat4f(float _x, float _y, float _z, float _w)
{
#ifdef __cplusplus
    return { { _x, _y, _z, _w } };
#else
    return (sx_quat){ .x = _x, .y = _y, .z = _z, .w = _w };
#endif
}

static inline sx_quat sx_quat4fv(const float* _f)
{
#ifdef __cplusplus
    return { { _f[0], _f[1], _f[2], _f[3] } };
#else
    return (sx_quat){ .x = _f[0], .y = _f[1], .z = _f[2], .w = _f[3] };
#endif
}

static inline sx_quat sx_quat_ident()
{
    return sx_quat4f(0, 0, 0, 1.0f);
}

static inline sx_vec3 sx_quat_mulXYZ(const sx_quat _qa, const sx_quat _qb)
{
    const float ax = _qa.x;
    const float ay = _qa.y;
    const float az = _qa.z;
    const float aw = _qa.f[3];

    const float bx = _qb.x;
    const float by = _qb.y;
    const float bz = _qb.z;
    const float bw = _qb.w;

    return sx_vec3f(aw * bx + ax * bw + ay * bz - az * by, aw * by - ax * bz + ay * bw + az * bx,
                    aw * bz + ax * by - ay * bx + az * bw);
}

// The product of two rotation quaternions will be equivalent to the rotation q followed by
// the rotation p
static inline sx_quat sx_quat_mul(const sx_quat p, const sx_quat q)
{
    // clang-format off
    return sx_quat4f(
        p.f[3] * q.f[0] + p.f[0] * q.f[3] + p.f[1] * q.f[2] - p.f[2] * q.f[1],
        p.f[3] * q.f[1] - p.f[0] * q.f[2] + p.f[1] * q.f[3] + p.f[2] * q.f[0],
        p.f[3] * q.f[2] + p.f[0] * q.f[1] - p.f[1] * q.f[0] + p.f[2] * q.f[3],
        p.f[3] * q.f[3] - p.f[0] * q.f[0] - p.f[1] * q.f[1] - p.f[2] * q.f[2]
    );
    // clang-format on
}

static inline sx_quat sx_quat_inv(const sx_quat _quat)
{
    return sx_quat4f(-_quat.x, -_quat.y, -_quat.z, _quat.w);
}

static inline float sx_quat_dot(const sx_quat _a, const sx_quat _b)
{
    return _a.x * _b.x + _a.y * _b.y + _a.z * _b.z + _a.w * _b.w;
}

static inline sx_quat sx_quat_norm(const sx_quat _quat)
{
    const float norm = sx_quat_dot(_quat, _quat);
    if (0.0f < norm) {
        const float inv_norm = sx_rsqrt(norm);
        return sx_quat4f(_quat.x * inv_norm, _quat.y * inv_norm, _quat.z * inv_norm,
                         _quat.w * inv_norm);
    } else {
        sx_assert(0 && "divide by zero");
        return sx_quat_ident();
    }
}

static inline sx_vec3 sx_quat_toeuler(const sx_quat _quat)
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

static inline sx_quat sx_quat_fromeular(const sx_vec3 _vec3)
{
    float yaw = _vec3.z;
    float pitch = _vec3.y;
    float roll = _vec3.x;

    float cy = sx_cos(yaw * 0.5f);
    float sy = sx_sin(yaw * 0.5f);
    float cp = sx_cos(pitch * 0.5f);
    float sp = sx_sin(pitch * 0.5f);
    float cr = sx_cos(roll * 0.5f);
    float sr = sx_sin(roll * 0.5f);

    sx_quat q;
    q.w = cr * cp * cy + sr * sp * sy;
    q.x = sr * cp * cy - cr * sp * sy;
    q.y = cr * sp * cy + sr * cp * sy;
    q.z = cr * cp * sy - sr * sp * cy;

    return q;
}

static inline sx_quat sx_quat_rotateaxis(const sx_vec3 _axis, float _angle)
{
    const float ha = _angle * 0.5f;
    const float ca = sx_cos(ha);
    const float sa = sx_sin(ha);
    return sx_quat4f(_axis.x * sa, _axis.y * sa, _axis.z * sa, ca);
}

static inline sx_quat sx_quat_rotateX(float _ax)
{
    const float hx = _ax * 0.5f;
    const float cx = sx_cos(hx);
    const float sx = sx_sin(hx);
    return sx_quat4f(sx, 0.0f, 0.0f, cx);
}

static inline sx_quat sx_quat_rotateY(float _ay)
{
    const float hy = _ay * 0.5f;
    const float cy = sx_cos(hy);
    const float sy = sx_sin(hy);
    return sx_quat4f(0.0f, sy, 0.0f, cy);
}

static inline sx_quat sx_quat_rotateZ(float _az)
{
    const float hz = _az * 0.5f;
    const float cz = sx_cos(hz);
    const float sz = sx_sin(hz);
    return sx_quat4f(0.0f, 0.0f, sz, cz);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Vec3
static inline sx_vec3 sx_vec3_abs(const sx_vec3 _a)
{
    return sx_vec3f(sx_abs(_a.x), sx_abs(_a.y), sx_abs(_a.z));
}

static inline sx_vec3 sx_vec3_neg(const sx_vec3 _a)
{
    return sx_vec3f(-_a.x, -_a.y, -_a.z);
}

static inline sx_vec3 sx_vec3_add(const sx_vec3 _a, const sx_vec3 _b)
{
    return sx_vec3f(_a.x + _b.x, _a.y + _b.y, _a.z + _b.z);
}

static inline sx_vec3 sx_vec3_addf(const sx_vec3 _a, float _b)
{
    return sx_vec3f(_a.x + _b, _a.y + _b, _a.z + _b);
}

static inline sx_vec3 sx_vec3_sub(const sx_vec3 _a, const sx_vec3 _b)
{
    return sx_vec3f(_a.x - _b.x, _a.y - _b.y, _a.z - _b.z);
}

static inline sx_vec3 sx_vec3_subf(const sx_vec3 _a, float _b)
{
    return sx_vec3f(_a.x - _b, _a.y - _b, _a.z - _b);
}

static inline sx_vec3 sx_vec3_mul(const sx_vec3 _a, const sx_vec3 _b)
{
    return sx_vec3f(_a.x * _b.x, _a.y * _b.y, _a.z * _b.z);
}

static inline sx_vec3 sx_vec3_mulf(const sx_vec3 _a, float _b)
{
    return sx_vec3f(_a.x * _b, _a.y * _b, _a.z * _b);
}

static inline float sx_vec3_dot(const sx_vec3 _a, const sx_vec3 _b)
{
    return _a.x * _b.x + _a.y * _b.y + _a.z * _b.z;
}

static inline sx_vec3 sx_vec3_cross(const sx_vec3 _a, const sx_vec3 _b)
{
    return sx_vec3f(_a.y * _b.z - _a.z * _b.y, _a.z * _b.x - _a.x * _b.z,
                    _a.x * _b.y - _a.y * _b.x);
}

static inline float sx_vec3_len(const sx_vec3 _a)
{
    return sx_sqrt(sx_vec3_dot(_a, _a));
}

static inline sx_vec3 sx_vec3_lerp(const sx_vec3 _a, const sx_vec3 _b, float _t)
{
    return sx_vec3f(sx_lerp(_a.x, _b.x, _t), sx_lerp(_a.y, _b.y, _t), sx_lerp(_a.z, _b.z, _t));
}

static inline sx_vec3 sx_vec3_norm(const sx_vec3 _a)
{
    const float len = sx_vec3_len(_a);
    if (len > 0.0f) {
        const float invlen = 1.0f / len;
        return sx_vec3f(_a.x * invlen, _a.y * invlen, _a.z * invlen);
    } else {
        sx_assert(0 && "Divide by zero");
        return sx_vec3f(0.0f, 0.0f, 0.0f);
    }
}

static inline sx_vec3 sx_vec3_norm_len(const sx_vec3 _a, float* _outlen)
{
    sx_assert(_outlen);
    const float len = sx_vec3_len(_a);
    if (len > 0.0f) {
        const float invlen = 1.0f / len;
        *_outlen = len;
        return sx_vec3f(_a.x * invlen, _a.y * invlen, _a.z * invlen);
    } else {
        sx_assert(0 && "Divide by zero");
        return sx_vec3f(0.0f, 0.0f, 0.0f);
    }
}

static inline sx_vec3 sx_vec3_min(const sx_vec3 _a, const sx_vec3 _b)
{
    return sx_vec3f(sx_min(_a.x, _b.x), sx_min(_a.y, _b.y), sx_min(_a.z, _b.z));
}

static inline sx_vec3 sx_vec3_max(const sx_vec3 _a, const sx_vec3 _b)
{
    return sx_vec3f(sx_max(_a.x, _b.x), sx_max(_a.y, _b.y), sx_max(_a.z, _b.z));
}

static inline sx_vec3 sx_vec3_rcp(const sx_vec3 _a)
{
    return sx_vec3f(1.0f / _a.x, 1.0f / _a.y, 1.0f / _a.z);
}

static inline void sx_vec3_tangent(sx_vec3* _t, sx_vec3* _b, const sx_vec3 _n)
{
    const float nx = _n.x;
    const float ny = _n.y;
    const float nz = _n.z;

    if (sx_abs(nx) > sx_abs(nz)) {
        float inv_len = 1.0f / sx_sqrt(nx * nx + nz * nz);
        *_t = sx_vec3f(-nz * inv_len, 0.0f, nx * inv_len);
    } else {
        float inv_len = 1.0f / sx_sqrt(ny * ny + nz * nz);
        *_t = sx_vec3f(0.0f, nz * inv_len, -ny * inv_len);
    }

    *_b = sx_vec3_cross(_n, *_t);
}

static inline void sx_vec3_tangent_angle(sx_vec3* _t, sx_vec3* _b, const sx_vec3 _n, float _angle)
{
    sx_vec3_tangent(_t, _b, _n);

    const float sa = sx_sin(_angle);
    const float ca = sx_cos(_angle);

    *_t = sx_vec3f(-sa * _b->x + ca * _t->x, -sa * _b->y + ca * _t->y, -sa * _b->z + ca * _t->z);

    *_b = sx_vec3_cross(_n, *_t);
}

static inline sx_vec3 sx_vec3_fromlatlong(float _u, float _v)
{
    const float phi = _u * SX_PI2;
    const float theta = _v * SX_PI;

    const float st = sx_sin(theta);
    const float sp = sx_sin(phi);
    const float ct = sx_cos(theta);
    const float cp = sx_cos(phi);

    return sx_vec3f(-st * sp, ct, -st * cp);
}

static inline sx_vec2 sx_vec3_tolatlong(const sx_vec3 _dir)
{
    const float phi = sx_atan2(_dir.x, _dir.z);
    const float theta = sx_acos(_dir.y);

    return sx_vec2f((SX_PI + phi) / SX_PI2, theta * SX_PI);
}

static inline sx_vec3 sx_vec3_mul_quat(const sx_vec3 _vec, const sx_quat _quat)
{
    sx_quat tmp0 = sx_quat_inv(_quat);
    sx_quat qv = sx_quat4f(_vec.x, _vec.y, _vec.z, 0.0f);
    sx_quat tmp1 = sx_quat_mul(tmp0, qv);
    return sx_quat_mulXYZ(tmp1, _quat);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Matrix
static inline sx_mat4 sx_mat4f(float m11, float m12, float m13, float m14, float m21, float m22,
                               float m23, float m24, float m31, float m32, float m33, float m34,
                               float m41, float m42, float m43, float m44)
{
#ifdef __cplusplus
    return { { m11, m21, m31, m41, m12, m22, m32, m42, m13, m23, m33, m43, m14, m24, m34, m44 } };
#else
    return (sx_mat4){ .m11 = m11,
                      .m12 = m12,
                      .m13 = m13,
                      .m14 = m14,
                      .m21 = m21,
                      .m22 = m22,
                      .m23 = m23,
                      .m24 = m24,
                      .m31 = m31,
                      .m32 = m32,
                      .m33 = m33,
                      .m34 = m34,
                      .m41 = m41,
                      .m42 = m42,
                      .m43 = m43,
                      .m44 = m44 };
#endif
}

static inline sx_mat4 sx_mat4fv(const float* _col1, const float* _col2, const float* _col3,
                                const float* _col4)
{
#ifdef __cplusplus
    return { { _col1[0], _col1[1], _col1[2], _col1[3], _col2[0], _col2[1], _col2[2], _col2[3],
               _col3[0], _col3[1], _col3[2], _col3[3], _col4[0], _col4[1], _col4[2], _col4[3] } };
#else
    return (sx_mat4){ .col1 = sx_vec4fv(_col1),
                      .col2 = sx_vec4fv(_col2),
                      .col3 = sx_vec4fv(_col3),
                      .col4 = sx_vec4fv(_col4) };
#endif
}

static inline sx_mat4 sx_mat4v(const sx_vec4 _col1, const sx_vec4 _col2, const sx_vec4 _col3,
                               const sx_vec4 _col4)
{
#ifdef __cplusplus
    return { { _col1.f[0], _col1.f[1], _col1.f[2], _col1.f[3], _col2.f[0], _col2.f[1], _col2.f[2],
               _col2.f[3], _col3.f[0], _col3.f[1], _col3.f[2], _col3.f[3], _col4.f[0], _col4.f[1],
               _col4.f[2], _col4.f[3] } };
#else
    return (sx_mat4){ .col1 = _col1, .col2 = _col2, .col3 = _col3, .col4 = _col4 };
#endif
}

static inline sx_mat4 sx_mat4_ident()
{
    return sx_mat4f(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
                    0.0f, 0.0f, 1.0f);
}

static inline sx_vec4 sx_mat4_row1(const sx_mat4* m)
{
    return sx_vec4f(m->m11, m->m12, m->m13, m->m14);
}

static inline sx_vec4 sx_mat4_row2(const sx_mat4* m)
{
    return sx_vec4f(m->m21, m->m22, m->m23, m->m24);
}

static inline sx_vec4 sx_mat4_row3(const sx_mat4* m)
{
    return sx_vec4f(m->m31, m->m32, m->m33, m->m34);
}

static inline sx_vec4 sx_mat4_row4(const sx_mat4* m)
{
    return sx_vec4f(m->m41, m->m42, m->m43, m->m44);
}

static inline sx_mat4 sx_mat4_translate(float _tx, float _ty, float _tz)
{
    return sx_mat4f(1.0f, 0.0f, 0.0f, _tx, 
                    0.0f, 1.0f, 0.0f, _ty, 
                    0.0f, 0.0f, 1.0f, _tz, 
                    0.0f, 0.0f, 0.0f, 1.0f);
}

static inline sx_mat4 sx_mat4_scale(float _sx, float _sy, float _sz)
{
    return sx_mat4f(_sx, 0.0f, 0.0f, 0.0f, 
                    0.0f, _sy, 0.0f, 0.0f, 
                    0.0f, 0.0f, _sz, 0.0f, 
                    0.0f, 0.0f,0.0f, 1.0f);
}

static inline sx_mat4 sx_mat4_scalef(float _scale)
{
    return sx_mat4_scale(_scale, _scale, _scale);
}

static inline sx_mat4 sx_mat4_rotateX(float _ax)
{
    const float sx = sx_sin(_ax);
    const float cx = sx_cos(_ax);

    return sx_mat4f(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, cx, -sx, 0.0f, 0.0f, sx, cx, 0.0f, 0.0f, 0.0f,
                    0.0f, 1.0f);
}

static inline sx_mat4 sx_mat4_rotateY(float _ay)
{
    const float sy = sx_sin(_ay);
    const float cy = sx_cos(_ay);

    return sx_mat4f(cy, 0.0f, sy, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, -sy, 0.0f, cy, 0.0f, 0.0f, 0.0f,
                    0.0f, 1.0f);
}

static inline sx_mat4 sx_mat4_rotateZ(float _az)
{
    const float sz = sx_sin(_az);
    const float cz = sx_cos(_az);

    return sx_mat4f(cz, -sz, 0.0f, 0.0f, sz, cz, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
                    0.0f, 1.0f);
}

static inline sx_mat4 sx_mat4_rotateXY(float _ax, float _ay)
{
    const float sx = sx_sin(_ax);
    const float cx = sx_cos(_ax);
    const float sy = sx_sin(_ay);
    const float cy = sx_cos(_ay);

    return sx_mat4f(cy, 0.0f, sy, 0.0f, sx * sy, cx, -sx * cy, 0.0f, -cx * sy, sx, cx * cy, 0.0f,
                    0.0f, 0.0f, 0.0f, 1.0f);
}

static inline sx_mat4 sx_mat4_rotateXYZ(float _ax, float _ay, float _az)
{
    const float sx = sx_sin(_ax);
    const float cx = sx_cos(_ax);
    const float sy = sx_sin(_ay);
    const float cy = sx_cos(_ay);
    const float sz = sx_sin(_az);
    const float cz = sx_cos(_az);

    return sx_mat4f(cy * cz, -cy * sz, sy, 0.0f, cz * sx * sy + cx * sz, cx * cz - sx * sy * sz,
                    -cy * sx, 0.0f, -cx * cz * sy + sx * sz, cz * sx + cx * sy * sz, cx * cy, 0.0f,
                    0.0f, 0.0f, 0.0f, 1.0f);
}

static inline sx_mat4 sx_mat4_rotateZYX(float _ax, float _ay, float _az)
{
    const float sx = sx_sin(_ax);
    const float cx = sx_cos(_ax);
    const float sy = sx_sin(_ay);
    const float cy = sx_cos(_ay);
    const float sz = sx_sin(_az);
    const float cz = sx_cos(_az);

    return sx_mat4f(cy * cz, cz * sx * sy - cx * sz, cx * cz * sy + sx * sz, 0.0f, cy * sz,
                    cx * cz + sx * sy * sz, -cz * sx + cx * sy * sz, 0.0f, -sy, cy * sx, cx * cy,
                    0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
};

static inline sx_mat4 sx_mat4_quat_translate(const sx_quat _quat, const sx_vec3 _translation)
{
    sx_mat4 mat = sx_quat_mat4(_quat);
    mat.m14 = -(mat.m11 * _translation.x + mat.m12 * _translation.y + mat.m13 * _translation.z);
    mat.m24 = -(mat.m21 * _translation.x + mat.m22 * _translation.y + mat.m23 * _translation.z);
    mat.m34 = -(mat.m31 * _translation.x + mat.m32 * _translation.y + mat.m33 * _translation.z);
    return mat;
}

static inline sx_mat4 sx_mat4_quat_translate_HMD(const sx_quat _quat, const sx_vec3 _translation)
{
    return sx_mat4_quat_translate(sx_quat4f(-_quat.x, -_quat.y, _quat.z, _quat.w), _translation);
}

/// multiply vector3 into 4x4 matrix without considering 4th column, which is not used in transform
/// matrices
static inline sx_vec3 sx_mat4_mul_vec3(const sx_mat4* _mat, const sx_vec3 _vec)
{
    return sx_vec3f(_vec.x * _mat->m11 + _vec.y * _mat->m12 + _vec.z * _mat->m13 + _mat->m14,
                    _vec.x * _mat->m21 + _vec.y * _mat->m22 + _vec.z * _mat->m23 + _mat->m24,
                    _vec.x * _mat->m31 + _vec.y * _mat->m32 + _vec.z * _mat->m33 + _mat->m34);
}

/// multiply vector3 into rotation part of the matrix only (used for normal vectors, etc...)
static inline sx_vec3 sx_mat4_mul_vec3_xyz0(const sx_mat4* _mat, const sx_vec3 _vec)
{
    return sx_vec3f(_vec.x * _mat->m11 + _vec.y * _mat->m12 + _vec.z * _mat->m13,
                    _vec.x * _mat->m21 + _vec.y * _mat->m22 + _vec.z * _mat->m23,
                    _vec.x * _mat->m31 + _vec.y * _mat->m32 + _vec.z * _mat->m33);
}

static inline sx_vec3 sx_mat4_mul_vec3_H(const sx_mat4* _mat, const sx_vec3 _vec)
{
    float xx = _vec.x * _mat->m11 + _vec.y * _mat->m12 + _vec.z * _mat->m13 + _mat->m14;
    float yy = _vec.x * _mat->m21 + _vec.y * _mat->m22 + _vec.z * _mat->m23 + _mat->m24;
    float zz = _vec.x * _mat->m31 + _vec.y * _mat->m32 + _vec.z * _mat->m33 + _mat->m34;
    float ww = _vec.x * _mat->m41 + _vec.y * _mat->m42 + _vec.z * _mat->m43 + _mat->m44;
    float iw = sx_sign(ww) / ww;
    return sx_vec3f(xx * iw, yy * iw, zz * iw);
}

static inline sx_vec4 sx_mat4_mul_vec4(const sx_mat4* _mat, const sx_vec4 _vec)
{
    return sx_vec4f(
        _vec.x * _mat->m11 + _vec.y * _mat->m12 + _vec.z * _mat->m13 + _vec.w * _mat->m14,
        _vec.x * _mat->m21 + _vec.y * _mat->m22 + _vec.z * _mat->m23 + _vec.w * _mat->m24,
        _vec.x * _mat->m31 + _vec.y * _mat->m32 + _vec.z * _mat->m33 + _vec.w * _mat->m34,
        _vec.x * _mat->m41 + _vec.y * _mat->m42 + _vec.z * _mat->m43 + _vec.w * _mat->m44);
}

static inline sx_vec4 sx_vec4_mul(const sx_vec4 _a, const sx_vec4 _b)
{
    return sx_vec4f(_a.x * _b.x, _a.y * _b.y, _a.z * _b.z, _a.w * _b.w);
}

static inline sx_vec4 sx_vec4_mulf(const sx_vec4 _a, float _b)
{
    return sx_vec4f(_a.x * _b, _a.y * _b, _a.z * _b, _a.w * _b);
}

static inline sx_vec4 sx_vec4_add(const sx_vec4 _a, const sx_vec4 _b)
{
    return sx_vec4f(_a.x + _b.x, _a.y + _b.y, _a.z + _b.z, _a.w + _b.w);
}

static inline sx_vec4 sx_vec4_sub(const sx_vec4 _a, const sx_vec4 _b)
{
    return sx_vec4f(_a.x - _b.x, _a.y - _b.y, _a.z - _b.z, _a.w - _b.w);
}

static inline sx_mat4 sx_mat4_transpose(const sx_mat4* _a)
{
    return sx_mat4f(_a->m11, _a->m21, _a->m31, _a->m41, _a->m12, _a->m22, _a->m32, _a->m42, _a->m13,
                    _a->m23, _a->m33, _a->m43, _a->m14, _a->m24, _a->m34, _a->m44);
}


/// Convert LH to RH projection matrix and vice versa.
static inline void sx_mat4_proj_flip_handedness(sx_mat4* _dst, const sx_mat4* _src)
{
    _dst->m11 = -_src->m11;
    _dst->m12 = -_src->m12;
    _dst->m13 = -_src->m13;
    _dst->m14 = -_src->m14;
    _dst->m21 = _src->m21;
    _dst->m22 = _src->m22;
    _dst->m23 = _src->m23;
    _dst->m24 = _src->m24;
    _dst->m31 = -_src->m31;
    _dst->m32 = -_src->m32;
    _dst->m33 = -_src->m33;
    _dst->m34 = -_src->m34;
    _dst->m41 = _src->m41;
    _dst->m42 = _src->m42;
    _dst->m43 = _src->m43;
    _dst->m44 = _src->m44;
}

/// Convert LH to RH view matrix and vice versa.
static inline void sx_mat4_view_flip_handedness(sx_mat4* _dst, const sx_mat4* _src)
{
    _dst->m11 = -_src->m11;
    _dst->m12 = _src->m12;
    _dst->m13 = -_src->m13;
    _dst->m14 = _src->m14;
    _dst->m21 = -_src->m21;
    _dst->m22 = _src->m22;
    _dst->m23 = -_src->m23;
    _dst->m24 = _src->m24;
    _dst->m31 = -_src->m31;
    _dst->m32 = _src->m32;
    _dst->m33 = -_src->m33;
    _dst->m34 = _src->m34;
    _dst->m41 = -_src->m41;
    _dst->m42 = _src->m42;
    _dst->m43 = -_src->m43;
    _dst->m44 = _src->m44;
}

static inline SX_CONSTFN float sx_color_tolinear(float _a)
{
    const float lo = _a / 12.92f;
    const float hi = sx_pow((_a + 0.055f) / 1.055f, 2.4f);
    const float result = sx_lerp(hi, lo, _a <= 0.04045f ? 1.0f : 0.0f);
    return result;
}

static inline SX_CONSTFN float sx_color_togamma(float _a)
{
    const float lo = _a * 12.92f;
    const float hi = sx_pow(sx_abs(_a), 1.0f / 2.4f) * 1.055f - 0.055f;
    const float result = sx_lerp(hi, lo, _a <= 0.0031308f ? 1.0f : 0.0f);
    return result;
}

//
static inline sx_mat3 sx_mat3f(float m11, float m12, float m13, float m21, float m22, float m23,
                               float m31, float m32, float m33)
{
#ifdef __cplusplus
    return { { m11, m21, m31, m12, m22, m32, m13, m23, m33 } };
#else
    return (sx_mat3){ .m11 = m11,
                      .m12 = m12,
                      .m13 = m13,
                      .m21 = m21,
                      .m22 = m22,
                      .m23 = m23,
                      .m31 = m31,
                      .m32 = m32,
                      .m33 = m33 };
#endif
}

static inline sx_mat3 sx_mat3fv(const float* _col1, const float* _col2, const float* _col3)
{
#ifdef __cplusplus
    return { { _col1[0], _col1[1], _col1[2], _col2[0], _col2[1], _col2[2], _col3[0], _col3[1],
               _col3[2] } };
#else
    return (
        sx_mat3){ .col1 = sx_vec3fv(_col1), .col2 = sx_vec3fv(_col2), .col3 = sx_vec3fv(_col3) };
#endif
}

static inline sx_mat3 sx_mat3v(const sx_vec3 _col1, const sx_vec3 _col2, const sx_vec3 _col3)
{
#ifdef __cplusplus
    return { { _col1.f[0], _col1.f[1], _col1.f[2], _col2.f[0], _col2.f[1], _col2.f[2], _col3.f[0],
               _col3.f[1], _col3.f[2] } };
#else
    return (sx_mat3){ .col1 = _col1, .col2 = _col2, .col3 = _col3 };
#endif
}

static inline sx_mat3 sx_mat3_ident()
{
    return sx_mat3f(1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
}

/// multiply vector3 into 4x4 matrix that [x/w, y/w, z/w, 1], used for projections, etc...
static inline sx_vec3 sx_mat3_mul_vec3(const sx_mat3* _mat, const sx_vec3 _vec)
{
    return sx_vec3f(_vec.x * _mat->m11 + _vec.y * _mat->m12 + _vec.z * _mat->m13,
                    _vec.x * _mat->m21 + _vec.y * _mat->m22 + _vec.z * _mat->m23,
                    _vec.x * _mat->m31 + _vec.y * _mat->m32 + _vec.z * _mat->m33);
}

static inline sx_vec2 sx_mat3_mul_vec2(const sx_mat3* _mat, const sx_vec2 _vec)
{
    return sx_vec2f(_vec.x * _mat->m11 + _vec.y * _mat->m12 + _mat->m13,
                    _vec.x * _mat->m21 + _vec.y * _mat->m22 + _mat->m23);
}

static inline sx_mat3 sx_mat3_translate(float x, float y)
{
    return sx_mat3f(1.0f, 0.0f, x, 0.0f, 1.0f, y, 0.0f, 0.0f, 1.0f);
}

static inline sx_mat3 sx_mat3_translatev(const sx_vec2 p)
{
    return sx_mat3_translate(p.x, p.y);
}

static inline sx_mat3 sx_mat3_rotate(float theta)
{
    float c = sx_cos(theta);
    float s = sx_sin(theta);
    return sx_mat3f(c, -s, 0.0f, s, c, 0.0f, 0.0f, 0.0f, 1.0f);
}

static inline sx_mat3 sx_mat3_scale(float sx, float sy)
{
    return sx_mat3f(sx, 0.0f, 0.0f, 0.0f, sy, 0.0f, 0.0f, 0.0f, 1.0f);
}

static inline sx_mat3 sx_mat3_SRT(float sx, float sy, float angle, float tx, float ty)
{
    // scale -> rotate -> translate
    // result of T(translate) * R(rotate) * S(scale)
    float c = sx_cos(angle);
    float s = sx_sin(angle);
    // clang-format off
    return sx_mat3f(sx*c,  -sy*s,  tx, 
                    sx*s,   sy*c,  ty, 
                    0.0f,   0.0f,  1.0f);
    // clang-format on
}

static inline sx_mat3 sx_mat3_transpose(const sx_mat3* _a)
{
    return sx_mat3f(_a->m11, _a->m21, _a->m31, 
                    _a->m12, _a->m22, _a->m32, 
                    _a->m13, _a->m23, _a->m33);
}

//
static inline float sx_vec2_dot(const sx_vec2 _a, const sx_vec2 _b)
{
    return _a.x * _b.x + _a.y * _b.y;
}

static inline float sx_vec2_len(const sx_vec2 _a)
{
    return sx_sqrt(sx_vec2_dot(_a, _a));
}

static inline sx_vec2 sx_vec2_norm(const sx_vec2 _a)
{
    const float len = sx_vec2_len(_a);
    if (len > 0.0f) {
        return sx_vec2f(_a.x / len, _a.y / len);
    } else {
        sx_assert(0 && "Divide by zero");
        return _a;
    }
}

static inline sx_vec2 sx_vec2_norm_len(const sx_vec2 _a, float* outlen)
{
    const float len = sx_vec2_len(_a);
    if (len > 0.0f) {
        *outlen = len;
        return sx_vec2f(_a.x / len, _a.y / len);
    } else {
        sx_assert(0 && "Divide by zero");
        return _a;
    }
}

static inline sx_vec2 sx_vec2_min(const sx_vec2 _a, const sx_vec2 _b)
{
    return sx_vec2f(sx_min(_a.x, _b.x), sx_min(_a.y, _b.y));
}

static inline sx_vec2 sx_vec2_max(const sx_vec2 _a, const sx_vec2 _b)
{
    return sx_vec2f(sx_max(_a.x, _b.x), sx_max(_a.y, _b.y));
}

static inline sx_vec2 sx_vec2_lerp(const sx_vec2 _a, const sx_vec2 _b, float _t)
{
    return sx_vec2f(sx_lerp(_a.x, _b.x, _t), sx_lerp(_a.y, _b.y, _t));
}

static inline sx_vec2 sx_vec2_abs(const sx_vec2 _a)
{
    return sx_vec2f(sx_abs(_a.x), sx_abs(_a.y));
}

static inline sx_vec2 sx_vec2_neg(const sx_vec2 _a)
{
    return sx_vec2f(-_a.x, -_a.y);
}

static inline sx_vec2 sx_vec2_add(const sx_vec2 _a, const sx_vec2 _b)
{
    return sx_vec2f(_a.x + _b.x, _a.y + _b.y);
}

static inline sx_vec2 sx_vec2_addf(const sx_vec2 _a, float _b)
{
    return sx_vec2f(_a.x + _b, _a.y + _b);
}

static inline sx_vec2 sx_vec2_sub(const sx_vec2 _a, const sx_vec2 _b)
{
    return sx_vec2f(_a.x - _b.x, _a.y - _b.y);
}

static inline sx_vec2 sx_vec2_subf(const sx_vec2 _a, float _b)
{
    return sx_vec2f(_a.x - _b, _a.y - _b);
}

static inline sx_vec2 sx_vec2_mul(const sx_vec2 _a, const sx_vec2 _b)
{
    return sx_vec2f(_a.x * _b.x, _a.y * _b.y);
}

static inline sx_vec2 sx_vec2_mulf(const sx_vec2 _a, float _b)
{
    return sx_vec2f(_a.x * _b, _a.y * _b);
}

static inline sx_ivec2 sx_ivec2_add(const sx_ivec2 _a, const sx_ivec2 _b)
{
    return sx_ivec2i(_a.x + _b.x, _a.y + _b.y);
}

static inline sx_ivec2 sx_ivec2_sub(const sx_ivec2 _a, const sx_ivec2 _b)
{
    return sx_ivec2i(_a.x - _b.x, _a.y - _b.y);
}

static inline sx_ivec2 sx_ivec2_min(const sx_ivec2 _a, const sx_ivec2 _b)
{
    return sx_ivec2i(sx_min(_a.x, _b.x), sx_min(_a.y, _b.y));
}

static inline sx_ivec2 sx_ivec2_max(const sx_ivec2 _a, const sx_ivec2 _b)
{
    return sx_ivec2i(sx_max(_a.x, _b.x), sx_max(_a.y, _b.y));
}

static inline sx_rect sx_rectf(float _xmin, float _ymin, float _xmax, float _ymax)
{
#ifdef __cplusplus
    return { { _xmin, _ymin, _xmax, _ymax } };
#else
    return (sx_rect){ .xmin = _xmin, .ymin = _ymin, .xmax = _xmax, .ymax = _ymax };
#endif
}

static inline sx_rect sx_rectwh(float _x, float _y, float _w, float _h)
{
    return sx_rectf(_x, _y, _x + _w, _y + _h);
}

static inline sx_rect sx_rectv(const sx_vec2 _min, const sx_vec2 _max)
{
#ifdef __cplusplus
    return { { _min.x, _min.y, _max.x, _max.y } };
#else
    return (sx_rect){ .vmin = _min, .vmax = _max };
#endif
}

static inline sx_rect sx_rect_expand(const sx_rect rc, const sx_vec2 expand)
{
    return sx_rectf(rc.xmin - expand.x, rc.ymin - expand.y, rc.xmax + expand.x, rc.ymax + expand.y);
}

static inline bool sx_rect_test_point(const sx_rect rc, const sx_vec2 pt)
{
    if (pt.x < rc.xmin || pt.y < rc.ymin || pt.x > rc.xmax || pt.y > rc.ymax)
        return false;
    return true;
}

static inline bool sx_rect_test_rect(const sx_rect rc1, const sx_rect rc2)
{
    if (rc1.xmax < rc2.xmin || rc1.xmin > rc2.xmax)
        return false;
    if (rc1.ymax < rc2.ymin || rc1.ymin > rc2.ymax)
        return false;
    return true;
}

static inline void sx_rect_add_point(sx_rect* rc, const sx_vec2 pt)
{
    rc->vmin = sx_vec2_min(rc->vmin, pt);
    rc->vmax = sx_vec2_max(rc->vmax, pt);
}

/*
 *   2               3
 *   -----------------
 *   |               |
 *   |               |
 *   |               |
 *   |               |
 *   |               |
 *   -----------------
 *   0               1
 */
static inline sx_vec2 sx_rect_corner(const sx_rect* rc, int index)
{
    return sx_vec2f((index & 1) ? rc->xmax : rc->xmin, (index & 2) ? rc->ymax : rc->ymin);
}

static inline void sx_rect_corners(sx_vec2 corners[4], const sx_rect* rc)
{
    for (int i = 0; i < 4; i++)
        corners[0] = sx_rect_corner(rc, i);
}

static inline float sx_rect_width(const sx_rect rc)
{
    return rc.xmax - rc.xmin;
}

static inline float sx_rect_height(const sx_rect rc)
{
    return rc.ymax - rc.ymin;
}

static inline sx_rect sx_rect_move(const sx_rect rc, const sx_vec2 pos) 
{
    return sx_rectv(sx_vec2_add(pos, rc.vmin), sx_vec2_add(pos, rc.vmax));
}

static inline sx_irect sx_irecti(int _xmin, int _ymin, int _xmax, int _ymax)
{
#ifdef __cplusplus
    return { { _xmin, _ymin, _xmax, _ymax } };
#else
    return (sx_irect){ .xmin = _xmin, .ymin = _ymin, .xmax = _xmax, .ymax = _ymax };
#endif
}

static inline sx_irect sx_irectwh(int _x, int _y, int _w, int _h)
{
    return sx_irecti(_x, _y, _x + _w, _y + _h);
}

static inline sx_irect sx_irectv(const sx_ivec2 _min, const sx_ivec2 _max)
{
#ifdef __cplusplus
    return { { _min.x, _min.y, _max.x, _max.y } };
#else
    return (sx_irect){ .vmin = _min, .vmax = _max };
#endif
}

static inline sx_irect sx_irect_expand(const sx_irect rc, const sx_ivec2 expand)
{
    return sx_irecti(rc.xmin - expand.x, rc.ymin - expand.y, rc.xmax + expand.x,
                     rc.ymax + expand.y);
}

static inline bool sx_irect_test_point(const sx_irect rc, const sx_ivec2 pt)
{
    if (pt.x < rc.xmin || pt.y < rc.ymin || pt.x > rc.xmax || pt.y > rc.ymax)
        return false;
    return true;
}

static inline bool sx_irect_test_rect(const sx_irect rc1, const sx_irect rc2)
{
    if (rc1.xmax < rc2.xmin || rc1.xmin > rc2.xmax)
        return false;
    if (rc1.ymax < rc2.ymin || rc1.ymin > rc2.ymax)
        return false;
    return true;
}

static inline void sx_irect_add_point(sx_irect* rc, const sx_ivec2 pt)
{
    rc->vmin = sx_ivec2_min(rc->vmin, pt);
    rc->vmax = sx_ivec2_max(rc->vmax, pt);
}

static inline int sx_irect_width(const sx_irect rc)
{
    return rc.xmax - rc.xmin;
}

static inline int sx_irect_height(const sx_irect rc)
{
    return rc.ymax - rc.ymin;
}

// 
static inline sx_plane sx_planef(float _nx, float _ny, float _nz, float _d)
{
#ifdef __cplusplus
    return { {{_nx, _ny, _nz, _d }} };
#else
    return (sx_plane){ .p = {.x = _nx, .y = _ny, .z = _nz, .w = _d} };
#endif    
}

static inline sx_plane sx_planev(const sx_vec3 _normal, float _d)
{
#ifdef __cplusplus
    return { {{_normal.x, _normal.y, _normal.z, _d }} };
#else
    return (sx_plane){ .p = {.x = _normal.x, .y = _normal.y, .z = _normal.z, .w = _d} };
#endif    
}

/*
 *   2               3 (max)
 *   -----------------
 *   |               |
 *   |               |
 *   |               |
 *   |               |
 *   |               |
 *   -----------------
 *   0 (min)         1
 */
static inline sx_ivec2 sx_irect_corner(const sx_irect* rc, int index)
{
    return sx_ivec2i((index & 1) ? rc->xmax : rc->xmin, (index & 2) ? rc->ymax : rc->ymin);
}

static inline void sx_irect_corners(sx_ivec2 corners[4], const sx_irect* rc)
{
    for (int i = 0; i < 4; i++)
        corners[0] = sx_irect_corner(rc, i);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// AABB
static inline sx_aabb sx_aabbf(float xmin, float ymin, float zmin, float xmax, float ymax,
                               float zmax)
{
#ifdef __cplusplus
    return { { xmin, ymin, zmin, xmax, ymax, zmax } };
#else
    return (sx_aabb){
        .xmin = xmin, .ymin = ymin, .zmin = zmin, .xmax = xmax, .ymax = ymax, .zmax = zmax
    };
#endif
}

static inline sx_aabb sx_aabbv(const sx_vec3 vmin, const sx_vec3 vmax)
{
#ifdef __cplusplus
    return { { vmin.x, vmin.y, vmin.z, vmax.x, vmax.y, vmax.z } };
#else
    return (sx_aabb){ .vmin = vmin, .vmax = vmax };
#endif
}

static inline sx_aabb sx_aabbwhd(float w, float h, float d)
{
    float hw = w * 0.5f;
    float hh = h * 0.5f;
    float hd = d * 0.5f;
    return sx_aabbf(-hw, -hh, -hd, hw, hh, hd);
}

static inline sx_aabb sx_aabb_empty()
{
    return sx_aabbf(SX_FLOAT_MAX, SX_FLOAT_MAX, SX_FLOAT_MAX, 
                    -SX_FLOAT_MAX, -SX_FLOAT_MAX, -SX_FLOAT_MAX);
}

static inline bool sx_aabb_isempty(const sx_aabb* aabb)
{
    return aabb->xmin >= aabb->xmax || aabb->ymin >= aabb->ymax || aabb->zmin >= aabb->zmax;
}

static inline void sx_aabb_add_point(sx_aabb* aabb, const sx_vec3 pt)
{
    aabb->vmin = sx_vec3_min(aabb->vmin, pt);
    aabb->vmax = sx_vec3_max(aabb->vmax, pt);
}

static inline sx_aabb sx_aabb_add(const sx_aabb* aabb1, const sx_aabb* aabb2)
{
    sx_aabb r = *aabb1;
    sx_aabb_add_point(&r, aabb2->vmin);
    sx_aabb_add_point(&r, aabb2->vmax);
    return r;
}

static inline bool sx_aabb_test_point(const sx_aabb* aabb, const sx_vec3 pt)
{
    if (aabb->xmax < pt.x || aabb->xmin > pt.x)
        return false;
    if (aabb->ymax < pt.y || aabb->ymin > pt.y)
        return false;
    if (aabb->zmax < pt.z || aabb->zmin > pt.z)
        return false;
    return true;
}

static inline bool sx_aabb_test(const sx_aabb* aabb1, const sx_aabb* aabb2)
{
    if (aabb1->xmax < aabb2->xmin || aabb1->xmin > aabb2->xmax)
        return false;
    if (aabb1->ymax < aabb2->ymin || aabb1->ymin > aabb2->ymax)
        return false;
    if (aabb1->zmax < aabb2->zmin || aabb1->zmin > aabb2->zmax)
        return false;
    return true;    
}

/*
 *        6                 7
 *        ------------------
 *       /|               /|
 *      / |              / |
 *     /  |             /  |
 *  2 /   |          3 /   |
 *   /----------------/    |
 *   |    |           |    |
 *   |    |           |    |      +Z
 *   |    |           |    |
 *   |    |-----------|----|     |
 *   |   / 4          |   / 5    |  / +Y
 *   |  /             |  /       | /
 *   | /              | /        |/
 *   |/               |/         --------- +X
 *   ------------------
 *  0                 1
 */
static inline sx_vec3 sx_aabb_corner(const sx_aabb* aabb, int index)
{
    sx_assert(index < 8);
    return sx_vec3f((index & 1) ? aabb->vmax.x : aabb->vmin.x,
                    (index & 4) ? aabb->vmax.y : aabb->vmin.y,
                    (index & 2) ? aabb->vmax.z : aabb->vmin.z);
}

static inline void sx_aabb_corners(sx_vec3 corners[8], const sx_aabb* aabb)
{
    for (int i = 0; i < 8; i++)
        corners[i] = sx_aabb_corner(aabb, i);
}

static inline sx_vec3 sx_aabb_extents(const sx_aabb* aabb)
{
    return sx_vec3f(aabb->vmax.x - aabb->vmin.x, aabb->vmax.y - aabb->vmin.y, aabb->vmax.z - aabb->vmin.z);
}

static inline sx_vec3 sx_aabb_center(const sx_aabb* aabb)
{
    return sx_vec3_mulf(sx_vec3_add(aabb->vmin, aabb->vmax), 0.5f);
}

static inline sx_aabb sx_aabb_translate(const sx_aabb* aabb, const sx_vec3 offset)
{
    return sx_aabbv(sx_vec3_add(aabb->vmin, offset), sx_vec3_add(aabb->vmax, offset));
}

static inline sx_aabb sx_aabb_setpos(const sx_aabb* aabb, const sx_vec3 pos)
{
    sx_vec3 e = sx_vec3_mulf(sx_aabb_extents(aabb), 0.5f);
    return sx_aabbf(pos.x - e.x, pos.y - e.y, pos.z - e.z, 
                    pos.x + e.x, pos.y + e.y, pos.z + e.z);
}

static inline sx_aabb sx_aabb_expand(const sx_aabb* aabb, const sx_vec3 expand)
{
    sx_vec3 p = sx_aabb_center(aabb);
    sx_vec3 e = sx_vec3_add(sx_vec3_mulf(sx_aabb_extents(aabb), 0.5f), expand);
    return sx_aabbf(p.x - e.x, p.y - e.y, p.z - e.z, 
                    p.x + e.x, p.y + e.y, p.z + e.z);
}

static inline sx_aabb sx_aabb_scale(const sx_aabb* aabb, const sx_vec3 scale)
{
    sx_vec3 p = sx_aabb_center(aabb);
    sx_vec3 e = sx_vec3_mul(sx_vec3_mulf(sx_aabb_extents(aabb), 0.5f), scale);
    return sx_aabbf(p.x - e.x, p.y - e.y, p.z - e.z, 
                    p.x + e.x, p.y + e.y, p.z + e.z);
}


////////////////////////////////////////////////////////////////////////////////////////////////////
static inline sx_tx3d sx_tx3d_set(const sx_vec3 _pos, const sx_mat3 _rot)
{
#ifdef __cplusplus
    return {_pos, _rot};
#else
    return (sx_tx3d) {.pos = _pos, .rot = _rot};
#endif
}


static inline sx_tx3d sx_tx3d_setf(float x, float y, float z, float rx, float ry, float rz)
{
    sx_mat4 rot = sx_mat4_rotateXYZ(rx, ry, rz);
    return sx_tx3d_set(sx_vec3f(x, y, z), sx_mat3fv(rot.rc1, rot.rc2, rot.rc3));
}

static inline sx_tx3d sx_tx3d_ident(void)
{
    return sx_tx3d_set(sx_vec3splat(0), sx_mat3_ident());
}

static inline sx_tx3d sx_tx3d_mul(const sx_tx3d* txa, const sx_tx3d* txb)
{
    return sx_tx3d_set(sx_vec3_add(sx_mat3_mul_vec3(&txa->rot, txb->pos), txa->pos),
                       sx_mat3_mul(&txa->rot, &txb->rot));
}

static inline sx_vec3 sx_tx3d_mul_vec3(const sx_tx3d* tx, sx_vec3 v)
{
    return sx_vec3_add(sx_mat3_mul_vec3(&tx->rot, v), tx->pos);
}   

static inline sx_vec3 sx_tx3d_mul_vec3_scale(const sx_tx3d* tx, sx_vec3 scale, sx_vec3 v)
{
    return sx_vec3_add(sx_mat3_mul_vec3(&tx->rot, sx_vec3_mul(v, scale)), tx->pos);
}

static inline sx_tx3d sx_tx3d_inverse(const sx_tx3d* tx)
{   
    sx_mat3 rot_inv = sx_mat3_transpose(&tx->rot);
    return sx_tx3d_set(sx_mat3_mul_vec3(&rot_inv, sx_vec3_mulf(tx->pos, -1.0f)), rot_inv);
}

static inline sx_mat4 sx_tx3d_mat4(const sx_tx3d* tx)
{
    return sx_mat4v(sx_vec4v3(tx->rot.col1, 0.0f),
                    sx_vec4v3(tx->rot.col2, 0.0f),
                    sx_vec4v3(tx->rot.col3, 0.0f),
                    sx_vec4v3(tx->pos,      1.0f));
}

static inline sx_tx3d sx_mat4_tx3d(const sx_mat4* mat)
{
    return sx_tx3d_set(sx_vec3fv(mat->col4.f),  sx_mat3fv(mat->col1.f, mat->col2.f, mat->col3.f));
}

////////////////////////////////////////////////////////////////////////////////////////////////////
static inline sx_box sx_box_set(const sx_tx3d tx, sx_vec3 extents)
{
    sx_vec3 he = sx_vec3_mulf(extents, 0.5f);
#ifdef __cplusplus
    return {tx, he};
#else
    return (sx_box) {.tx = tx, .he = he};
#endif    
}

static inline sx_box sx_box_setpne(const sx_vec3 pos, const sx_vec3 extents)
{
    sx_tx3d tx = sx_tx3d_set(pos, sx_mat3_ident());
    return sx_box_set(tx, extents);
}

static inline sx_vec3 sx_box_pos(const sx_box* box)
{
    return box->tx.pos;
}

static inline sx_mat3 sx_box_rot(const sx_box* box)
{
    return box->tx.rot;
}

static inline sx_vec3 sx_box_extents(const sx_box* box)
{
    return sx_vec3_mulf(box->he, 2.0f);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// easing
static inline float sx_easein_quad(float t)
{
    return t * t;
}

static inline float sx_easeout_quad(float t)
{
    return -(t * (t - 2.0f));
}

static inline float sx_easeinout_quad(float t)
{
    if (t < 0.5f) {
        return 2.0f * t * t;
    } else {
        return (-2.0f * t * t) + (4.0f * t) - 1.0f;
    }
}

static inline float sx_easein_cubic(float t)
{
    return t * t * t;
}

static inline float sx_easeout_cubic(float t)
{
    float f = (t - 1.0f);
    return f * f * f + 1.0f;
}

static inline float sx_easeinout_cubic(float t)
{
    if (t < 0.5f) {
        return 4.0f * t * t * t;
    } else {
        float f = ((2.0f * t) - 2.0f);
        return 0.5f * f * f * f + 1;
    }
}

static inline float sx_easein_quart(float t)
{
    return t * t * t * t;
}

static inline float sx_easeout_quart(float t)
{
    float f = (t - 1.0f);
    return f * f * f * (1.0f - t) + 1.0f;
}

static inline float sx_easeinout_quart(float t)
{
    if (t < 0.5f) {
        return 8.0f * t * t * t * t;
    } else {
        float f = (t - 1.0f);
        return -8.0f * f * f * f * f + 1.0f;
    }
}

static inline float sx_easein_quint(float t)
{
    return t * t * t * t * t;
}

static inline float sx_easeout_quint(float t)
{
    float f = (t - 1.0f);
    return f * f * f * f * f + 1.0f;
}

static inline float sx_easeinout_quint(float t)
{
    if (t < 0.5f) {
        return 16.0f * t * t * t * t * t;
    } else {
        float f = ((2.0f * t) - 2.0f);
        return 0.5f * f * f * f * f * f + 1.0f;
    }
}

static inline float sx_easein_sine(float t)
{
    return sx_sin((t - 1) * SX_PI2) + 1.0f;
}

static inline float sx_easeout_sine(float t)
{
    return sx_sin(t * SX_PI2);
}

static inline float sx_easeinout_sine(float t)
{
    return 0.5f * (1.0f - sx_cos(t * SX_PI));
}

static inline float sx_easein_circ(float t)
{
    return 1.0f - sx_sqrt(1.0f - (t * t));
}

static inline float sx_easeout_circ(float t)
{
    return sx_sqrt((2.0f - t) * t);
}

static inline float sx_easeinout_circ(float t)
{
    if (t < 0.5f) {
        return 0.5f * (1.0f - sx_sqrt(1.0f - 4.0f * (t * t)));
    } else {
        return 0.5f * (sx_sqrt(-((2.0f * t) - 3.0f) * ((2.0f * t) - 1.0f)) + 1.0f);
    }
}

static inline float sx_easein_expo(float t)
{
    return (t == 0.0f) ? t : sx_pow(2.0f, 10.0f * (t - 1.0f));
}

static inline float sx_easeout_expo(float t)
{
    return (t == 1.0f) ? t : 1.0f - sx_pow(2.0f, -10.0f * t);
}

static inline float sx_easeinout_expo(float t)
{
    if (t == 0.0f || t == 1.0f)
        return t;

    if (t < 0.5f) {
        return 0.5f * sx_pow(2.0f, (20.0f * t) - 10.0f);
    } else {
        return -0.5f * sx_pow(2.0f, (-20.0f * t) + 10.0f) + 1.0f;
    }
}

static inline float sx_easein_elastic(float t)
{
    return sx_sin(13.0f * SX_PI2 * t) * sx_pow(2.0f, 10.0f * (t - 1.0f));
}

static inline float sx_easeout_elastic(float t)
{
    return sx_sin(-13.0f * SX_PI2 * (t + 1.0f)) * sx_pow(2.0f, -10.0f * t) + 1.0f;
}

static inline float sx_easeinout_elastic(float t)
{
    if (t < 0.5f) {
        return 0.5f * sx_sin(13.0f * SX_PI2 * (2.0f * t)) *
               sx_pow(2.0f, 10.0f * ((2.0f * t) - 1.0f));
    } else {
        return 0.5f * (sx_sin(-13.0f * SX_PI2 * ((2.0f * t - 1.0f) + 1.0f)) *
                           sx_pow(2.0f, -10.0f * (2.0f * t - 1.0f)) +
                       2.0f);
    }
}

static inline float sx_easein_back(float t)
{
    const float s = 1.70158f;
    return t * t * ((s + 1.0f) * t - s);
}

static inline float sx_easeout_back(float t)
{
    const float s = 1.70158f;
    return --t, 1.0f * (t * t * ((s + 1.0f) * t + s) + 1.0f);
}

static inline float sx_easeinout_back(float t)
{
    const float s = 2.5949095f;
    if (t < 0.5f) {
        return t *= 2.0f, 0.5f * t * t * (t * s + t - s);
    } else {
        return t = t * 2.0f - 2.0f, 0.5f * (2.0f + t * t * (t * s + t + s));
    }
}

static inline float sx_easeout_bounce(float t)
{
    static const float kSections[] = { 4.0f / 11.0f, 8.0f / 11.0f, 9 / 10.0f };
    static const float kA1 = 363.0f / 40.0f;
    static const float kA1_b = 99.0f / 10.0f;
    static const float kA1_c = 17.0f / 5.0f;
    static const float kA2 = 4356.0f / 361.0f;
    static const float kA2_b = 35442.0f / 1805.0f;
    static const float kA2_c = 16061.0f / 1805.0f;
    static const float kA3 = 54.0f / 5.0f;
    static const float kA3_b = 513.0f / 25.0f;
    static const float kA3_c = 268.0f / 25.0f;

    if (t < kSections[0]) {
        return (121.0f * t * t) / 16.0f;
    } else if (t < kSections[1]) {
        return (kA1 * t * t) - (kA1_b * t) + kA1_c;
    } else if (t < kSections[2]) {
        return (kA2 * t * t) - (kA2_b * t) + kA2_c;
    } else {
        return (kA3 * t * t) - (kA3_b * t) + kA3_c;
    }
}

static inline float sx_easein_bounce(float t)
{
    return 1.0f - sx_easeout_bounce(1.0f - t);
}

static inline float sx_easeinout_bounce(float t)
{
    if (t < 0.5f) {
        return 0.5f * sx_easein_bounce(t * 2.0f);
    } else {
        return 0.5f * sx_easeout_bounce(t * 2.0f - 1.0f) + 0.5f;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// cpp operators
#ifdef __cplusplus
static inline sx_vec2 operator+(const sx_vec2& a, const sx_vec2& b)
{
    return sx_vec2_add(a, b);
}

static inline sx_vec2 operator-(const sx_vec2& a, const sx_vec2& b)
{
    return sx_vec2_sub(a, b);
}

static inline sx_vec2 operator*(const sx_vec2& v, float k)
{
    return sx_vec2_mulf(v, k);
}

static inline sx_vec2 operator*(float k, const sx_vec2& v)
{
    return sx_vec2_mulf(v, k);
}

static inline sx_vec2 operator*(const sx_vec2& v0, const sx_vec2& v1)
{
    return sx_vec2_mul(v0, v1);
}

static inline sx_ivec2 operator+(const sx_ivec2& a, const sx_ivec2& b)
{
    return sx_ivec2_add(a, b);
}

static inline sx_ivec2 operator-(const sx_ivec2& a, const sx_ivec2& b)
{
    return sx_ivec2_sub(a, b);
}

static inline sx_vec3 operator+(const sx_vec3& v1, const sx_vec3& v2)
{
    return sx_vec3_add(v1, v2);
}

static inline sx_vec3 operator-(const sx_vec3& v1, const sx_vec3& v2)
{
    return sx_vec3_sub(v1, v2);
}

static inline sx_vec3 operator*(const sx_vec3& v, float k)
{
    return sx_vec3_mulf(v, k);
}

static inline sx_vec3 operator*(float k, const sx_vec3& v)
{
    return sx_vec3_mulf(v, k);
}

static inline sx_mat4 operator*(const sx_mat4& a, const sx_mat4& b)
{
    return sx_mat4_mul(&a, &b);
}

static inline sx_mat3 operator*(const sx_mat3& a, const sx_mat3& b)
{
    return sx_mat3_mul(&a, &b);
}

static inline sx_quat operator*(const sx_quat& a, const sx_quat& b)
{
    return sx_quat_mul(a, b);
}
#endif

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

SX_API sx_color SX_COLOR_WHITE;
SX_API sx_color SX_COLOR_BLACK;
SX_API sx_color SX_COLOR_RED;
SX_API sx_color SX_COLOR_YELLOW;
SX_API sx_color SX_COLOR_GREEN;
SX_API sx_color SX_COLOR_BLUE;
SX_API sx_color SX_COLOR_PURPLE;

//
// Version history
//      v1.0.0      Initial release
//      v1.1.0      Matrices are now column-major (in memory)
//                  Added SX_VECx_ constants
//      v1.1.1      Fixed through the whole API for RH proj and view calculations
//      v1.2.0      Added new primitives like color and rect
//      v1.2.1      Moved std-math.h to C unit
//      v1.3        Added tx3d (transform), box and plane primitives

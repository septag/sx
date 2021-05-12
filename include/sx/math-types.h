//
// Copyright 2018 Sepehr Taghdisian (septag@github). All rights reserved.
// License: https://github.com/septag/sx#license-bsd-2-clause
//
// see math.h for full info
//
#pragma once

#include "sx.h"

// Some math constants
#define SX_PI 3.1415926535897932384626433832795f
#define SX_PI2 6.2831853071795864769252867665590f
#define SX_INVPI (1.0f / 3.1415926535897932384626433832795f)
#define SX_PIHALF 1.5707963267948966192313216916398f
#define SX_PIQUARTER 0.7853981633974483096156608458199f
#define SX_SQRT2 1.4142135623730950488016887242097f
#define SX_LOG_NAT10 2.3025850929940456840179914546844f
#define SX_INVLOG_NAT2 1.4426950408889634073599246810019f
#define SX_LOG_NAT2H 0.6931471805599453094172321214582f
#define SX_LOG_NAT2L 1.90821492927058770002e-10f
#define SX_E 2.7182818284590452353602874713527f
#define SX_NEAR_ZERO (1.0f / (float)(1 << 28))
#define SX_FLOAT_MIN (1.175494e-38f)
#define SX_FLOAT_MAX (3.402823e+38f)

SX_PRAGMA_DIAGNOSTIC_PUSH()
SX_PRAGMA_DIAGNOSTIC_IGNORED_MSVC(4201)     // nonstandard extension used : nameless struct/union
SX_PRAGMA_DIAGNOSTIC_IGNORED_MSVC(4204)     // nonstandard extension used: non-constant aggregate initializer

typedef union sx_vec2 {
    struct {
        float x;
        float y;
    };

    float f[2];

    #ifdef __cplusplus
    sx_vec2() = default;
    explicit sx_vec2(float _x, float _y) { x = _x; y = _y; }
    explicit sx_vec2(float _xx) { x = _xx; y = _xx; }
    explicit sx_vec2(const float* _f) { x = _f[0]; y = _f[1]; }
    #endif
} sx_vec2;

typedef union sx_vec3 {
    struct {
        float x;
        float y;
        float z;
    };

    float f[3];

    #ifdef __cplusplus
    sx_vec3() = default;
    explicit sx_vec3(float _x, float _y, float _z)  { x = _x; y = _y; z = _z; }
    explicit sx_vec3(float _xxx) { x = _xxx; y = _xxx; z = _xxx; }
    explicit sx_vec3(const float* _f) { x = _f[0]; y = _f[1]; z = _f[2]; }
    explicit sx_vec3(sx_vec2 v, float _z = 0) { x = v.x; y = v.y; z = _z; }
    #endif
} sx_vec3;

typedef union sx_vec4 {
    struct {
        float x;
        float y;
        float z;
        float w;
    };

    float f[4];

    #ifdef __cplusplus
    sx_vec4() = default;
    explicit sx_vec4(float _x, float _y, float _z, float _w = 1.0f) { x = _x; y = _y; z = _z; w = _w; }
    explicit sx_vec4(float _xxxx) { x = _xxxx; y = _xxxx; z =_xxxx; w = _xxxx; }
    explicit sx_vec4(const float* _f) { x = _f[0]; y = _f[1]; z = _f[2]; w = _f[3]; }
    explicit sx_vec4(sx_vec3 v, float _w = 1.0f) { x = v.x; y = v.y; z= v.z; w = _w; }
    explicit sx_vec4(sx_vec2 v, float _z = 0, float _w = 1.0f) { x = v.x; y = v.y; z = _z; w = _w; }
    #endif
} sx_vec4;

typedef union sx_color {
    struct {
        unsigned char r;
        unsigned char g;
        unsigned char b;
        unsigned char a;
    };

    unsigned int n;

    #ifdef __cplusplus
    sx_color() = default;
    explicit sx_color(unsigned char _r, unsigned char _g, unsigned char _b, unsigned char _a)
    {
        r = _r; g = _g; b = _b; a = _a;
    }

    explicit sx_color(float _r, float _g, float _b, float _a)
    {
        r = (unsigned char)(_r * 255.0f);
        g = (unsigned char)(_g * 255.0f);
        b = (unsigned char)(_b * 255.0f);
        a = (unsigned char)(_a * 255.0f);
    }

    explicit sx_color(unsigned int _n) { n = _n; }
    #endif
} sx_color;


typedef union sx_ivec2 {
    struct {
        int x;
        int y;
    };

    int n[2];

    #ifdef __cplusplus
    sx_ivec2() = default;
    explicit sx_ivec2(int _x, int _y) { x = _x; y = _y; }
    explicit sx_ivec2(const int* _i) { x = _i[0]; y = _i[1]; }
    explicit sx_ivec2(int _xx) { x = _xx; y = _xx; }
    #endif
} sx_ivec2;

typedef union sx_quat {
    struct {
        float x;
        float y;
        float z;
        float w;
    };

    float f[4];

    #ifdef __cplusplus
    sx_quat() = default;
    explicit sx_quat(float _x, float _y, float _z, float _w) { x = _x; y = _y; z = _z; w = _w; }
    explicit sx_quat(const float* _f) { x = _f[0]; y = _f[1]; z = _f[2]; w = _f[3]; }
    #endif
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

    float f[9];

    #ifdef __cplusplus
    sx_mat3() = default;
    explicit sx_mat3(float _m11, float _m12, float _m13, 
                     float _m21, float _m22, float _m23, 
                     float _m31, float _m32, float _m33)
    {
        m11 = _m11;     m12 = _m12;     m13 = _m13;
        m21 = _m21;     m22 = _m22;     m23 = _m23;
        m31 = _m31;     m32 = _m32;     m33 = _m33;
    }
    explicit sx_mat3(const float* _col1, const float* _col2, const float* _col3)
    {
        m11 = _col1[0];     m12 = _col2[0];     m13 = _col3[0];
        m21 = _col1[1];     m22 = _col2[1];     m23 = _col3[1];
        m31 = _col1[2];     m32 = _col2[2];     m33 = _col3[2];
    }
    explicit sx_mat3(sx_vec3 _col1, sx_vec3 _col2, sx_vec3 _col3) :
        sx_mat3(_col1.f, _col2.f, _col3.f)
    {
    }    
    #endif
} sx_mat3;

typedef union sx_mat4 {
    struct {
        float m11, m21, m31, m41;
        float m12, m22, m32, m42;
        float m13, m23, m33, m43;
        float m14, m24, m34, m44;
    };

    struct {
        float fc1[4];
        float fc2[4];
        float fc3[4];
        float fc4[4];
    };

    float f[16];

    #ifdef __cplusplus
    sx_mat4() = default;
    explicit sx_mat4(float _m11, float _m12, float _m13, float _m14, 
                     float _m21, float _m22, float _m23, float _m24, 
                     float _m31, float _m32, float _m33, float _m34, 
                     float _m41, float _m42, float _m43, float _m44)
    {
        m11 = _m11;     m12 = _m12;     m13 = _m13;     m14 = _m14;
        m21 = _m21;     m22 = _m22;     m23 = _m23;     m24 = _m24;
        m31 = _m31;     m32 = _m32;     m33 = _m33;     m34 = _m34;
        m41 = _m41;     m42 = _m42;     m43 = _m43;     m44 = _m44;
    }
    explicit sx_mat4(const float* _col1, const float* _col2, const float* _col3, const float* _col4)
    {
        m11 = _col1[0];     m12 = _col2[0];     m13 = _col3[0];     m14 = _col4[0];
        m21 = _col1[1];     m22 = _col2[1];     m23 = _col3[1];     m24 = _col4[1];
        m31 = _col1[2];     m32 = _col2[2];     m33 = _col3[2];     m34 = _col4[2];
        m41 = _col1[3];     m42 = _col2[3];     m43 = _col3[3];     m44 = _col4[3];
    }
    explicit sx_mat4(sx_vec4 _col1, sx_vec4 _col2, sx_vec4 _col3, sx_vec4 _col4) : 
        sx_mat4(_col1.f, _col2.f, _col3.f, _col4.f)
    {
    }
    #endif
} sx_mat4;

typedef union sx_rect {
    struct {
        float xmin, ymin;
        float xmax, ymax;
    };

    struct {
        float vmin[2];
        float vmax[2];
    };

    float f[4];

    #ifdef __cplusplus
    sx_rect() = default;
    explicit sx_rect(float _xmin, float _ymin, float _xmax, float _ymax) 
    { 
        xmin = _xmin; ymin = _ymin; xmax = _xmax; ymax = _ymax;
    }
    explicit sx_rect(const float* _vmin, const float* _vmax) 
    {
        vmin[0] = _vmin[0];    vmin[1] = _vmin[1];
        vmax[0] = _vmax[0];    vmax[1] = _vmax[1];
    }
    explicit sx_rect(sx_vec2 _vmin, sx_vec2 _vmax) :
        sx_rect(_vmin.f, _vmax.f)
    {
    }
    #endif
} sx_rect;

typedef union sx_irect {
    struct {
        int xmin, ymin;
        int xmax, ymax;
    };

    struct {
        int vmin[3];
        int vmax[3];
    };

    int n[4];

    #ifdef __cplusplus
    sx_irect() = default;
    explicit sx_irect(int _xmin, int _ymin, int _xmax, int _ymax)
    {
        xmin = _xmin;   ymin = _ymin;   xmax = _xmax;   ymax = _ymax;
    }
    explicit sx_irect(const int* _vmin, const int* _vmax)
    {
        vmin[0] = _vmin[0];     vmin[1] = _vmin[1];
        vmax[0] = _vmax[0];     vmax[1] = _vmax[1];
    }
    explicit sx_irect(sx_ivec2 _vmin, sx_ivec2 _vmax) :
        sx_irect(_vmin.n, _vmax.n)
    { 
    }
#endif
} sx_irect;

typedef union sx_aabb {
    struct {
        float xmin, ymin, zmin;
        float xmax, ymax, zmax;
    };

    struct {
        float vmin[3];
        float vmax[3];
    };

    float f[6];

    #ifdef __cplusplus
    sx_aabb() = default;
    explicit sx_aabb(float _xmin, float _ymin, float _zmin, float _xmax, float _ymax, float _zmax) 
    { 
        xmin = _xmin; ymin = _ymin; zmin = _zmin;
        xmax = _xmax; ymax = _ymax; zmax = _zmax;
    }
    explicit sx_aabb(const float* _vmin, const float* _vmax) 
    {
        vmin[0] = _vmin[0];    vmin[1] = _vmin[1];      vmin[2] = _vmin[2];
        vmax[0] = _vmax[0];    vmax[1] = _vmax[1];      vmax[2] = _vmax[2];
    }
    explicit sx_aabb(sx_vec3 _vmin, sx_vec3 _vmax) :
        sx_aabb(_vmin.f, _vmax.f)
    {
    }
    #endif
} sx_aabb;

// 3d plane: a*nx + b*ny + c*nz + d = 0
typedef union sx_plane {
    sx_vec4 p;
    struct {
        float normal[3];
        float dist;
    };
    float f[4];

    #ifdef __cplusplus
    sx_plane() = default;
    explicit sx_plane(float _nx, float _ny, float _nz, float _d) : p(_nx, _ny, _nz, _d) {}
    explicit sx_plane(sx_vec3 _normal, float _d) 
    {
        normal[0] = _normal.x;  normal[1] = _normal.y;  normal[2] = _normal.z;
        dist = _d;
    }
    #endif
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
    sx_vec3 e;    // half-extent from the origin (0.5*width, 0.5*height, 0.5f*depth)
} sx_box;

////////////////////////////////////////////////////////////////////////////////////////////////////
// C-style constructors
SX_FORCE_INLINE sx_vec2 sx_vec2f(float _x, float _y);
SX_FORCE_INLINE sx_vec2 sx_vec2fv(const float* _f);
SX_FORCE_INLINE sx_vec2 sx_vec2splat(float _f);

SX_FORCE_INLINE sx_vec3 sx_vec3f(float _x, float _y, float _z);
SX_FORCE_INLINE sx_vec3 sx_vec3fv(const float* _f);
SX_FORCE_INLINE sx_vec3 sx_vec3v2(sx_vec2 _vec, float _z);
SX_FORCE_INLINE sx_vec3 sx_vec3splat(float _f);

SX_FORCE_INLINE sx_plane sx_planef(float _nx, float _ny, float _nz, float _d);
SX_FORCE_INLINE sx_plane sx_planev(sx_vec3 _normal, float _d);
SX_FORCE_INLINE sx_plane sx_planefv(const float* _f);

SX_FORCE_INLINE sx_mat4 sx_mat4f(float m11, float m12, float m13, float m14, 
                                 float m21, float m22, float m23, float m24, 
                                 float m31, float m32, float m33, float m34,
                                 float m41, float m42, float m43, float m44);
SX_FORCE_INLINE sx_mat4 sx_mat4fv(const float* _col1, const float* _col2, const float* _col3, const float* _col4);
SX_FORCE_INLINE sx_mat4 sx_mat4v(sx_vec4 _col1, sx_vec4 _col2, sx_vec4 _col3, sx_vec4 _col4);
SX_FORCE_INLINE sx_mat4 sx_mat4_ident();

SX_FORCE_INLINE sx_mat3 sx_mat3f(float m11, float m12, float m13, 
                                 float m21, float m22, float m23,
                                 float m31, float m32, float m33);
SX_FORCE_INLINE sx_mat3 sx_mat3fv(const float* _col1, const float* _col2, const float* _col3);
SX_FORCE_INLINE sx_mat3 sx_mat3v(sx_vec3 _col1, sx_vec3 _col2, sx_vec3 _col3);
SX_FORCE_INLINE sx_mat3 sx_mat3_ident();

SX_FORCE_INLINE sx_quat sx_quat4f(float _x, float _y, float _z, float _w);
SX_FORCE_INLINE sx_quat sx_quat4fv(const float* _f);
SX_FORCE_INLINE sx_quat sx_quat_ident();

SX_FORCE_INLINE sx_color sx_color4u(unsigned char _r, unsigned char _g, unsigned char _b,
                                    unsigned char _a sx_default(255));
SX_FORCE_INLINE sx_color sx_color4f(float _r, float _g, float _b, float _a sx_default(1.0f));
SX_FORCE_INLINE sx_color sx_colorn(unsigned int _n);

SX_FORCE_INLINE sx_aabb sx_aabbf(float xmin, float ymin, float zmin, 
                                 float xmax, float ymax, float zmax);
SX_FORCE_INLINE sx_aabb sx_aabbv(sx_vec3 vmin, sx_vec3 vmax);
SX_FORCE_INLINE sx_aabb sx_aabbwhd(float w, float h, float d);
SX_FORCE_INLINE sx_aabb sx_aabb_empty();

SX_FORCE_INLINE sx_rect sx_rectf(float _xmin, float _ymin, float _xmax, float _ymax);
SX_FORCE_INLINE sx_rect sx_rectwh(float _x, float _y, float _w, float _h);
SX_FORCE_INLINE sx_rect sx_rectv(sx_vec2 _min, sx_vec2 _max);

SX_FORCE_INLINE sx_irect sx_irecti(int _xmin, int _ymin, int _xmax, int _ymax);
SX_FORCE_INLINE sx_irect sx_irectwh(int _x, int _y, int _w, int _h);
SX_FORCE_INLINE sx_irect sx_irectv(sx_ivec2 _min, sx_ivec2 _max);

SX_FORCE_INLINE sx_tx3d sx_tx3d_set(sx_vec3 _pos, const sx_mat3 _rot);
SX_FORCE_INLINE sx_tx3d sx_tx3d_ident(void);

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

////////////////////////////////////////////////////////////////////////////////////////////////////
SX_FORCE_INLINE sx_vec3 sx_vec3f(float _x, float _y, float _z)
{
#ifdef __cplusplus
    return sx_vec3(_x, _y, _z);
#else
    return (sx_vec3){ .x = _x, .y = _y, .z = _z };
#endif
}

SX_FORCE_INLINE sx_vec3 sx_vec3fv(const float* _f)
{
#ifdef __cplusplus
    return sx_vec3(_f);
#else
    return (sx_vec3){ .x = _f[0], .y = _f[1], .z = _f[2] };
#endif
}

SX_FORCE_INLINE sx_vec3 sx_vec3v2(sx_vec2 _vec, float _z)
{
    return sx_vec3f(_vec.x, _vec.y, _z);
}

SX_FORCE_INLINE sx_vec3 sx_vec3splat(float _f)
{
    return sx_vec3f(_f, _f, _f);
}

//
SX_FORCE_INLINE sx_vec2 sx_vec2f(float _x, float _y)
{
#ifdef __cplusplus
    return sx_vec2(_x, _y);
#else
    return (sx_vec2){ .x = _x, .y = _y };
#endif
}

SX_FORCE_INLINE sx_vec2 sx_vec2fv(const float* _f)
{
#ifdef __cplusplus
    return sx_vec2(_f);
#else
    return (sx_vec2){ .x = _f[0], .y = _f[1] };
#endif
}

SX_FORCE_INLINE sx_vec2 sx_vec2splat(float _f)
{
    return sx_vec2f(_f, _f);
}

//
SX_FORCE_INLINE sx_vec4 sx_vec4f(float _x, float _y, float _z, float _w)
{
#ifdef __cplusplus
    return sx_vec4(_x, _y, _z, _w);
#else
    return (sx_vec4){ .x = _x, .y = _y, .z = _z, .w = _w };
#endif
}

SX_FORCE_INLINE sx_vec4 sx_vec4fv(const float* _f)
{
#ifdef __cplusplus
    return sx_vec4(_f[0], _f[1], _f[2], _f[3]);
#else
    return (sx_vec4){ .x = _f[0], .y = _f[1], .z = _f[2], .w = _f[3] };
#endif
}

SX_FORCE_INLINE sx_vec4 sx_vec4v3(sx_vec3 _vec, float _w)
{
    return sx_vec4f(_vec.x, _vec.y, _vec.z, _w);
}

SX_FORCE_INLINE sx_vec4 sx_vec4splat(float _f)
{
    return sx_vec4f(_f, _f, _f, _f);
}

//
SX_FORCE_INLINE sx_ivec2 sx_ivec2i(int _x, int _y)
{
#ifdef __cplusplus
    return sx_ivec2(_x, _y);
#else
    return (sx_ivec2){ .x = _x, .y = _y };
#endif
}

SX_FORCE_INLINE sx_ivec2 sx_ivec2iv(const int* _f)
{
#ifdef __cplusplus
    return sx_ivec2(_f);
#else
    return (sx_ivec2){ .x = _f[0], .y = _f[1] };
#endif
}

SX_FORCE_INLINE sx_ivec2 sx_ivec2splat(int _f)
{
    return sx_ivec2i(_f, _f);
}

//
SX_FORCE_INLINE sx_color sx_color4u(unsigned char _r, unsigned char _g, unsigned char _b, unsigned char _a)
{
#ifdef __cplusplus
    return sx_color(_r, _g, _b, _a);
#else
    return (sx_color){ .r = _r, .g = _g, .b = _b, .a = _a };
#endif
}

SX_FORCE_INLINE sx_color sx_color4f(float _r, float _g, float _b, float _a)
{
    return sx_color4u((unsigned char)(_r * 255.0f), (unsigned char)(_g * 255.0f),
                      (unsigned char)(_b * 255.0f), (unsigned char)(_a * 255.0f));
}

SX_FORCE_INLINE sx_color sx_colorn(unsigned int _n)
{
#ifdef __cplusplus
    sx_color c;
    c.n = _n;
    return c;
#else
    return (sx_color){ .n = _n };
#endif
}

SX_FORCE_INLINE sx_vec4 sx_color_vec4(sx_color c)
{
    float rcp = 1.0f / 255.0f;
    return sx_vec4f((float)c.r * rcp, (float)c.g * rcp, (float)c.b * rcp, (float)c.a * rcp);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Quaternion
SX_FORCE_INLINE sx_quat sx_quat4f(float _x, float _y, float _z, float _w)
{
#ifdef __cplusplus
    return sx_quat(_x, _y, _z, _w);
#else
    return (sx_quat){ .x = _x, .y = _y, .z = _z, .w = _w };
#endif
}

SX_FORCE_INLINE sx_quat sx_quat4fv(const float* _f)
{
#ifdef __cplusplus
    return sx_quat(_f);
#else
    return (sx_quat){ .x = _f[0], .y = _f[1], .z = _f[2], .w = _f[3] };
#endif
}

SX_FORCE_INLINE sx_quat sx_quat_ident()
{
    return sx_quat4f(0, 0, 0, 1.0f);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Matrix
SX_FORCE_INLINE sx_mat4 sx_mat4f(float m11, float m12, float m13, float m14, 
                                 float m21, float m22, float m23, float m24, 
                                 float m31, float m32, float m33, float m34,
                                 float m41, float m42, float m43, float m44)
{
#ifdef __cplusplus
    return sx_mat4(m11, m21, m31, m41, m12, m22, m32, m42, m13, m23, m33, m43, m14, m24, m34, m44);
#else
    return (sx_mat4){ .m11 = m11,   .m12 = m12,     .m13 = m13,     .m14 = m14,
                      .m21 = m21,   .m22 = m22,     .m23 = m23,     .m24 = m24,
                      .m31 = m31,   .m32 = m32,     .m33 = m33,     .m34 = m34,
                      .m41 = m41,   .m42 = m42,     .m43 = m43,     .m44 = m44 };
#endif
}

SX_FORCE_INLINE sx_mat4 sx_mat4fv(const float* _col1, const float* _col2, const float* _col3, const float* _col4)
{
#ifdef __cplusplus
    return sx_mat4(_col1, _col2, _col3, _col4);
#else
    return (sx_mat4) { 
        .m11 = _col1[0],     .m12 = _col2[0],     .m13 = _col3[0],     .m14 = _col4[0],
        .m21 = _col1[1],     .m22 = _col2[1],     .m23 = _col3[1],     .m24 = _col4[1],
        .m31 = _col1[2],     .m32 = _col2[2],     .m33 = _col3[2],     .m34 = _col4[2],
        .m41 = _col1[3],     .m42 = _col2[3],     .m43 = _col3[3],     .m44 = _col4[3] };
#endif
}

SX_FORCE_INLINE sx_mat4 sx_mat4v(sx_vec4 _col1, sx_vec4 _col2, sx_vec4 _col3, sx_vec4 _col4)
{
#ifdef __cplusplus
    return sx_mat4(_col1, _col2, _col3, _col4);
#else
    return sx_mat4fv(_col1.f, _col2.f, _col3.f, _col4.f);
#endif
}

SX_FORCE_INLINE sx_mat4 sx_mat4_ident()
{
    return sx_mat4f(1.0f, 0.0f, 0.0f, 0.0f, 
                    0.0f, 1.0f, 0.0f, 0.0f, 
                    0.0f, 0.0f, 1.0f, 0.0f, 
                    0.0f, 0.0f, 0.0f, 1.0f);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
SX_FORCE_INLINE sx_mat3 sx_mat3f(float m11, float m12, float m13, 
                                 float m21, float m22, float m23,
                                 float m31, float m32, float m33)
{
#ifdef __cplusplus
    return sx_mat3(m11, m21, m31, m12, m22, m32, m13, m23, m33);
#else
    return (sx_mat3){ .m11 = m11, .m12 = m12, .m13 = m13,
                      .m21 = m21, .m22 = m22, .m23 = m23,
                      .m31 = m31, .m32 = m32, .m33 = m33 };
#endif
}

SX_FORCE_INLINE sx_mat3 sx_mat3fv(const float* _col1, const float* _col2, const float* _col3)
{
#ifdef __cplusplus
    return sx_mat3(_col1, _col2, _col3);
#else
    return (sx_mat3) { 
        .m11 = _col1[0],     .m12 = _col2[0],     .m13 = _col3[0],
        .m21 = _col1[1],     .m22 = _col2[1],     .m23 = _col3[1],
        .m31 = _col1[2],     .m32 = _col2[2],     .m33 = _col3[2]
    };
#endif
}

SX_FORCE_INLINE sx_mat3 sx_mat3v(sx_vec3 _col1, sx_vec3 _col2, sx_vec3 _col3)
{
#ifdef __cplusplus
    return sx_mat3(_col1, _col2, _col3);
#else
    return sx_mat3fv(_col1.f, _col2.f, _col3.f);
#endif
}

SX_FORCE_INLINE sx_mat3 sx_mat3_ident()
{
    return sx_mat3f(1.0f, 0.0f, 0.0f, 
                    0.0f, 1.0f, 0.0f, 
                    0.0f, 0.0f, 1.0f);
}

SX_FORCE_INLINE sx_rect sx_rectf(float _xmin, float _ymin, float _xmax, float _ymax)
{
#ifdef __cplusplus
    return sx_rect(_xmin, _ymin, _xmax, _ymax);
#else
    return (sx_rect){ .xmin = _xmin, .ymin = _ymin, .xmax = _xmax, .ymax = _ymax };
#endif
}

SX_FORCE_INLINE sx_rect sx_rectwh(float _x, float _y, float _w, float _h)
{
    return sx_rectf(_x, _y, _x + _w, _y + _h);
}

SX_FORCE_INLINE sx_rect sx_rectv(sx_vec2 _min, sx_vec2 _max)
{
#ifdef __cplusplus
    return sx_rect(_min.x, _min.y, _max.x, _max.y);
#else
    return (sx_rect){.xmin = _min.x, .ymin = _min.y, .xmax = _max.x, .ymax = _max.y};
#endif
}

SX_FORCE_INLINE sx_irect sx_irecti(int _xmin, int _ymin, int _xmax, int _ymax)
{
#ifdef __cplusplus
    return sx_irect(_xmin, _ymin, _xmax, _ymax);
#else
    return (sx_irect){ .xmin = _xmin, .ymin = _ymin, .xmax = _xmax, .ymax = _ymax };
#endif
}

SX_FORCE_INLINE sx_irect sx_irectwh(int _x, int _y, int _w, int _h)
{
    return sx_irecti(_x, _y, _x + _w, _y + _h);
}

SX_FORCE_INLINE sx_irect sx_irectv(sx_ivec2 _min, sx_ivec2 _max)
{
#ifdef __cplusplus
    return sx_irect(_min.x, _min.y, _max.x, _max.y);
#else
    return (sx_irect){.xmin = _min.x, .ymin = _min.y, .xmax = _max.x, .ymax = _max.y};
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// AABB
SX_FORCE_INLINE sx_aabb sx_aabbf(float xmin, float ymin, float zmin, float xmax, float ymax,
                               float zmax)
{
#ifdef __cplusplus
    return sx_aabb(xmin, ymin, zmin, xmax, ymax, zmax);
#else
    return (sx_aabb){
        .xmin = xmin, .ymin = ymin, .zmin = zmin, .xmax = xmax, .ymax = ymax, .zmax = zmax
    };
#endif
}

SX_FORCE_INLINE sx_aabb sx_aabbv(sx_vec3 vmin, sx_vec3 vmax)
{
#ifdef __cplusplus
    return sx_aabb(vmin.x, vmin.y, vmin.z, vmax.x, vmax.y, vmax.z);
#else
    return (sx_aabb){.xmin = vmin.x, .ymin = vmin.y, .zmin = vmin.z, .xmax = vmax.x, .ymax = vmax.y, .zmax = vmax.z};
#endif
}

SX_FORCE_INLINE sx_aabb sx_aabbwhd(float w, float h, float d)
{
    float hw = w * 0.5f;
    float hh = h * 0.5f;
    float hd = d * 0.5f;
    return sx_aabbf(-hw, -hh, -hd, hw, hh, hd);
}

SX_FORCE_INLINE sx_aabb sx_aabb_empty()
{
    return sx_aabbf(SX_FLOAT_MAX, SX_FLOAT_MAX, SX_FLOAT_MAX, 
                    -SX_FLOAT_MAX, -SX_FLOAT_MAX, -SX_FLOAT_MAX);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
SX_FORCE_INLINE sx_tx3d sx_tx3d_set(sx_vec3 _pos, const sx_mat3 _rot)
{
#ifdef __cplusplus
    return {_pos, _rot};
#else
    return (sx_tx3d) {.pos = _pos, .rot = _rot};
#endif
}


SX_FORCE_INLINE sx_tx3d sx_tx3d_ident(void)
{
    return sx_tx3d_set(sx_vec3splat(0), sx_mat3_ident());
}

////////////////////////////////////////////////////////////////////////////////////////////////////
SX_FORCE_INLINE sx_box sx_box_set(const sx_tx3d tx, sx_vec3 extents)
{
#ifdef __cplusplus
    return {tx, extents};
#else
    return (sx_box) {.tx = tx, .e = extents};
#endif    
}

SX_FORCE_INLINE sx_box sx_box_setpne(sx_vec3 pos, sx_vec3 extents)
{
    sx_tx3d tx = sx_tx3d_set(pos, sx_mat3_ident());
    return sx_box_set(tx, extents);
}

//
SX_FORCE_INLINE sx_plane sx_planef(float _nx, float _ny, float _nz, float _d)
{
#ifdef __cplusplus
    return sx_plane(_nx, _ny, _nz, _d);
#else
    return (sx_plane){ .p = { .x = _nx, .y = _ny, .z = _nz, .w = _d } };
#endif
}

SX_FORCE_INLINE sx_plane sx_planev(sx_vec3 _normal, float _d)
{
#ifdef __cplusplus
    return sx_plane(_normal, _d);
#else
    return (sx_plane){ .p = { .x = _normal.x, .y = _normal.y, .z = _normal.z, .w = _d } };
#endif
}

SX_FORCE_INLINE sx_plane sx_planefv(const float* _f)
{
#ifdef __cplusplus
    return sx_plane(_f[0], _f[1], _f[2], _f[3]);
#else
    return (sx_plane) { .p = { .x = _f[0], .y = _f[1], .z = _f[2], .w = _f[3] } };
#endif
}

SX_PRAGMA_DIAGNOSTIC_POP()    // ignore msvc warnings

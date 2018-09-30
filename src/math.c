//
// Copyright 2018 Sepehr Taghdisian (septag@github). All rights reserved.
// License: https://github.com/septag/sx#license-bsd-2-clause
//
#include "sx/math.h"

const float SX_PI          = 3.1415926535897932384626433832795f;
const float SX_PI2         = 6.2831853071795864769252867665590f;
const float SX_INVPI       = 1.0f/3.1415926535897932384626433832795f;
const float SX_PIHALF      = 1.5707963267948966192313216916398f;
const float SX_PIQUARTER   = 0.7853981633974483096156608458199f;
const float SX_SQRT2       = 1.4142135623730950488016887242097f;
const float SX_LOG_NAT10   = 2.3025850929940456840179914546844f;
const float SX_INVLOG_NAT2 = 1.4426950408889634073599246810019f;
const float SX_LOG_NAT2H   = 0.6931471805599453094172321214582f;
const float SX_LOG_NAT2L   = 1.90821492927058770002e-10f;
const float SX_E           = 2.7182818284590452353602874713527f;
const float SX_NEAR_ZERO   = 1.0f/(float)(1 << 28);
const float SX_FLOAT_MIN   = 1.175494e-38f;
const float SX_FLOAT_MAX   = 3.402823e+38f;

#ifndef __cplusplus
sx_vec2 SX_VEC2_ZERO  = {.x = 0.0f, .y = 0.0f};
sx_vec2 SX_VEC2_UNITX = {.x = 1.0f, .y = 0.0f};
sx_vec2 SX_VEC2_UNITY = {.x = 0.0f, .y = 1.0f};

sx_vec3 SX_VEC3_ZERO  = {.x = 0.0f, .y = 0.0f, .z = 0.0f};
sx_vec3 SX_VEC3_UNITX = {.x = 1.0f, .y = 0.0f, .z = 0.0f};
sx_vec3 SX_VEC3_UNITY = {.x = 0.0f, .y = 1.0f, .z = 0.0f};
sx_vec3 SX_VEC3_UNITZ = {.x = 0.0f, .y = 0.0f, .z = 1.0f};

sx_vec4 SX_VEC4_ZERO  = {.x = 0.0f, .y = 0.0f, .z = 0.0f, .w = 1.0f};
sx_vec4 SX_VEC4_UNITX = {.x = 1.0f, .y = 0.0f, .z = 0.0f, .w = 1.0f};
sx_vec4 SX_VEC4_UNITY = {.x = 0.0f, .y = 1.0f, .z = 0.0f, .w = 1.0f};
sx_vec4 SX_VEC4_UNITZ = {.x = 0.0f, .y = 0.0f, .z = 1.0f, .w = 1.0f};
#else
sx_vec3 SX_VEC2_ZERO  = {0.0f, 0.0f};
sx_vec3 SX_VEC2_UNITX = {1.0f, 0.0f};
sx_vec3 SX_VEC2_UNITY = {0.0f, 1.0f};

sx_vec3 SX_VEC3_ZERO  = {0.0f, 0.0f, 0.0f};
sx_vec3 SX_VEC3_UNITX = {1.0f, 0.0f, 0.0f};
sx_vec3 SX_VEC3_UNITY = {0.0f, 1.0f, 0.0f};
sx_vec3 SX_VEC3_UNITZ = {0.0f, 0.0f, 1.0f};

sx_vec4 SX_VEC4_ZERO  = {0.0f, 0.0f, 0.0f, 1.0f};
sx_vec4 SX_VEC4_UNITX = {1.0f, 0.0f, 0.0f, 1.0f};
sx_vec4 SX_VEC4_UNITY = {0.0f, 1.0f, 0.0f, 1.0f};
sx_vec4 SX_VEC4_UNITZ = {0.0f, 0.0f, 1.0f, 1.0f};
#endif

#define sx__square(_x)      (_x*_x)
#define sx__mad(_a, _b, _c) _a*_b + _c

#if !SX_CONFIG_STDMATH
SX_CONSTFN float sx_floor(float _a)
{
    if (_a < 0.0f) {
        const float fr = sx_fract(-_a);
        const float result = -_a - fr;

        return -(0.0f != fr
            ? result + 1.0f
            : result);
    }

    return _a - sx_fract(_a);
}

SX_CONSTFN float sx_cos(float _a)
{
    static const float kSinC2  = -0.16666667163372039794921875f;
    static const float kSinC4  =  8.333347737789154052734375e-3f;
    static const float kSinC6  = -1.9842604524455964565277099609375e-4f;
    static const float kSinC8  =  2.760012648650445044040679931640625e-6f;
    static const float kSinC10 = -2.50293279435709337121807038784027099609375e-8f;

    static const float kCosC2  = -0.5f;
    static const float kCosC4  =  4.166664183139801025390625e-2f;
    static const float kCosC6  = -1.388833043165504932403564453125e-3f;
    static const float kCosC8  =  2.47562347794882953166961669921875e-5f;
    static const float kCosC10 = -2.59630184018533327616751194000244140625e-7f;

    const float scaled = _a * 2.0f*SX_INVPI;
    const float real   = sx_floor(scaled);
    const float xx     = _a - real * SX_PIHALF;
    const int32_t bits = (int32_t)real & 3;

    float c0, c2, c4, c6, c8, c10;

    if (bits == 0 || bits == 2) {
        c0  = 1.0f;
        c2  = kCosC2;
        c4  = kCosC4;
        c6  = kCosC6;
        c8  = kCosC8;
        c10 = kCosC10;
    } else {
        c0  = xx;
        c2  = kSinC2;
        c4  = kSinC4;
        c6  = kSinC6;
        c8  = kSinC8;
        c10 = kSinC10;
    }

    const float xsq    = sx__square(xx);
    const float tmp0   = sx__mad(c10,  xsq, c8 );
    const float tmp1   = sx__mad(tmp0, xsq, c6 );
    const float tmp2   = sx__mad(tmp1, xsq, c4 );
    const float tmp3   = sx__mad(tmp2, xsq, c2 );
    const float tmp4   = sx__mad(tmp3, xsq, 1.0f);
    const float result = tmp4 * c0;

    return bits == 1 || bits == 2
        ? -result
        :  result;
}

SX_CONSTFN float sx_acos(float _a)
{
    static const float kAcosC0 =  1.5707288f;
    static const float kAcosC1 = -0.2121144f;
    static const float kAcosC2 =  0.0742610f;
    static const float kAcosC3 = -0.0187293f;

    const float absa   = sx_abs(_a);
    const float tmp0   = sx__mad(kAcosC3, absa, kAcosC2);
    const float tmp1   = sx__mad(tmp0,    absa, kAcosC1);
    const float tmp2   = sx__mad(tmp1,    absa, kAcosC0);
    const float tmp3   = tmp2 * sx_sqrt(1.0f - absa);
    const float negate = (float)(_a < 0.0f);
    const float tmp4   = tmp3 - 2.0f*negate*tmp3;
    const float result = negate*SX_PI + tmp4;

    return result;    
}

SX_CONSTFN float sx_atan2(float _y, float _x)
{
    static const float kAtan2C0 = -0.013480470f;
    static const float kAtan2C1 =  0.057477314f;
    static const float kAtan2C2 = -0.121239071f;
    static const float kAtan2C3 =  0.195635925f;
    static const float kAtan2C4 = -0.332994597f;
    static const float kAtan2C5 =  0.999995630f;

    const float ax     = sx_abs(_x);
    const float ay     = sx_abs(_y);
    const float maxaxy = sx_max(ax, ay);
    const float minaxy = sx_min(ax, ay);

    if (maxaxy == 0.0f)
    {
        return 0.0f*sx_sign(_y);
    }

    const float mxy    = minaxy / maxaxy;
    const float mxysq  = sx__square(mxy);
    const float tmp0   = sx__mad(kAtan2C0, mxysq, kAtan2C1);
    const float tmp1   = sx__mad(tmp0,     mxysq, kAtan2C2);
    const float tmp2   = sx__mad(tmp1,     mxysq, kAtan2C3);
    const float tmp3   = sx__mad(tmp2,     mxysq, kAtan2C4);
    const float tmp4   = sx__mad(tmp3,     mxysq, kAtan2C5);
    const float tmp5   = tmp4 * mxy;
    const float tmp6   = ay > ax   ? SX_PIHALF - tmp5 : tmp5;
    const float tmp7   = _x < 0.0f ? SX_PI     - tmp6 : tmp6;
    const float result = sx_sign(_y)*tmp7;

    return result;
}


static float sx__ldexp(float _a, int32_t _b)
{
    const uint32_t ftob     = sx_ftob(_a);
    const uint32_t masked   = ftob & UINT32_C(0xff800000);
    const uint32_t expsign0 = ((int32_t)masked) >> 23;
    const uint32_t tmp      = (int)expsign0 + (int)_b;
    const uint32_t expsign1 = tmp << 23;
    const uint32_t mantissa = ftob & UINT32_C(0x007fffff);
    const uint32_t bits     = mantissa | expsign1;
    return sx_btof(bits);
}

static float sx__frexp(float _a, int32_t* _outExp)
{
    const uint32_t ftob     = sx_ftob(_a);
    const uint32_t masked0  = ftob & UINT32_C(0x7f800000);
    const uint32_t exp0     = masked0 >> 23;
    const uint32_t masked1  = ftob & UINT32_C(0x807fffff);
    const uint32_t bits     = masked1 | UINT32_C(0x3f000000);
    const float    result   = sx_btof(bits);

    *_outExp = (int32_t)(exp0 - 0x7e);

    return result;
}

SX_CONSTFN float sx_exp(float _a)
{
    static const float kExpC0  =  1.66666666666666019037e-01f;
    static const float kExpC1  = -2.77777777770155933842e-03f;
    static const float kExpC2  =  6.61375632143793436117e-05f;
    static const float kExpC3  = -1.65339022054652515390e-06f;
    static const float kExpC4  =  4.13813679705723846039e-08f;    

    if (sx_abs(_a) <= SX_NEAR_ZERO) {
        return _a + 1.0f;
    }

    const float kk     = sx_round(_a*SX_INVLOG_NAT2);
    const float hi     = _a - kk*SX_LOG_NAT2H;
    const float lo     =      kk*SX_LOG_NAT2L;
    const float hml    = hi - lo;
    const float hmlsq  = sx__square(hml);
    const float tmp0   = sx__mad(kExpC4, hmlsq, kExpC3);
    const float tmp1   = sx__mad(tmp0,   hmlsq, kExpC2);
    const float tmp2   = sx__mad(tmp1,   hmlsq, kExpC1);
    const float tmp3   = sx__mad(tmp2,   hmlsq, kExpC0);
    const float tmp4   = hml - hmlsq * tmp3;
    const float tmp5   = hml*tmp4/(2.0f-tmp4);
    const float tmp6   = 1.0f - ( (lo - tmp5) - hi);
    const float result = sx__ldexp(tmp6, (int32_t)(kk) );

    return result; 
}

SX_CONSTFN float sx_log(float _a)
{
    static const float kLogC0 = 6.666666666666735130e-01f;
    static const float kLogC1 = 3.999999999940941908e-01f;
    static const float kLogC2 = 2.857142874366239149e-01f;
    static const float kLogC3 = 2.222219843214978396e-01f;
    static const float kLogC4 = 1.818357216161805012e-01f;
    static const float kLogC5 = 1.531383769920937332e-01f;
    static const float kLogC6 = 1.479819860511658591e-01f;

    int32_t exp;
    float ff = sx__frexp(_a, &exp);
    if (ff < SX_SQRT2*0.5f) {
        ff *= 2.0f;
        --exp;
    }

    ff -= 1.0f;
    const float kk     = (float)(exp);
    const float hi     = kk*SX_LOG_NAT2H;
    const float lo     = kk*SX_LOG_NAT2L;
    const float ss     = ff / (2.0f + ff);
    const float s2     = sx__square(ss);
    const float s4     = sx__square(s2);

    const float tmp0   = sx__mad(kLogC6, s4, kLogC4);
    const float tmp1   = sx__mad(tmp0,   s4, kLogC2);
    const float tmp2   = sx__mad(tmp1,   s4, kLogC0);
    const float t1     = s2*tmp2;

    const float tmp3   = sx__mad(kLogC5, s4, kLogC3);
    const float tmp4   = sx__mad(tmp3,   s4, kLogC1);
    const float t2     = s4*tmp4;

    const float t12    = t1 + t2;
    const float hfsq   = 0.5f*sx__square(ff);
    const float result = hi - ( (hfsq - (ss*(hfsq+t12) + lo) ) - ff);

    return result;
}
#endif

static void sx__mat4_lookat(sx_mat4* _result, const sx_vec3 _eye, const sx_vec3 _view, const sx_vec3 _up)
{
    sx_vec3 right = sx_vec3_norm(sx_vec3_cross(_up, _view), NULL);
    sx_vec3 up = sx_vec3_cross(_view, right);
    
    // row1
    _result->f[ 0] = right.f[0];
    _result->f[ 4] = up.f[0];
    _result->f[ 9] = _view.f[0];
    _result->f[12] = 0;

    // row2
    _result->f[ 1] = right.f[1];
    _result->f[ 5] = up.f[1];
    _result->f[ 9] = _view.f[1];
    _result->f[13] = 0;

    // row3
    _result->f[ 2] = right.f[2];
    _result->f[ 6] = up.f[2];
    _result->f[10] = _view.f[2];
    _result->f[14] = 0;

    // row4
    _result->f[ 3] = -sx_vec3_dot(right, _eye);
    _result->f[ 7] = -sx_vec3_dot(up, _eye);
    _result->f[11] = -sx_vec3_dot(_view, _eye);
    _result->f[15] = 1.0f;
}

void sx_mat4_lookatLH(sx_mat4* _result, const sx_vec3 _eye, const sx_vec3 _at, const sx_vec3 _up)
{
    sx_vec3 view = sx_vec3_norm(sx_vec3_sub(_at, _eye), NULL);
    sx__mat4_lookat(_result, _eye, view, _up);
}

void sx_mat4_lookatRH(sx_mat4* _result, const sx_vec3 _eye, const sx_vec3 _at, const sx_vec3 _up)
{
    sx_vec3 view = sx_vec3_norm(sx_vec3_sub(_eye, _at), NULL);
    sx__mat4_lookat(_result, _eye, view, _up);
}

void sx_mat4_lookat(sx_mat4* _result, const sx_vec3 _eye, const sx_vec3 _at, const sx_vec3 _up)
{
    sx_mat4_lookatLH(_result, _eye, _at, _up);
}

static void sx__mat4_projXYWH_RH(sx_mat4* _result, float _x, float _y, float _width, float _height, 
                                 float _near, float _far, bool _oglNdc)
{
    const float diff = _far-_near;
    const float aa = _oglNdc ? (     _far+_near)/diff : _far/diff;
    const float bb = _oglNdc ? (2.0f*_far*_near)/diff : _near*aa;

    sx_memset(_result->f, 0x0, sizeof(float)*16);
    _result->f[ 0] = _width;
    _result->f[ 5] = _height;
    _result->f[ 2] = _x;
    _result->f[ 6] = _y;
    _result->f[10] = -aa;
    _result->f[14] = -1.0f;
    _result->f[11] = -bb;
}

static void sx__mat4_projXYWH_LH(sx_mat4* _result, float _x, float _y, float _width, float _height, 
                                 float _near, float _far, bool _oglNdc)
{
    const float diff = _far-_near;
    const float aa = _oglNdc ? (     _far+_near)/diff : _far/diff;
    const float bb = _oglNdc ? (2.0f*_far*_near)/diff : _near*aa;

    sx_memset(_result->f, 0, sizeof(float)*16);
    _result->f[ 0] = _width;
    _result->f[ 5] = _height;
    _result->f[ 2] = -_x;
    _result->f[ 6] = -_y;
    _result->f[10] = aa;
    _result->f[14] = 1.0f;
    _result->f[11] = -bb;
}

static void sx__mat4_proj_RH(sx_mat4* _result, float _ut, float _dt, float _lt, float _rt, float _near, float _far, bool _oglNdc)
{
    const float invDiffRl = 1.0f/(_rt - _lt);
    const float invDiffUd = 1.0f/(_ut - _dt);
    const float width  =  2.0f*_near * invDiffRl;
    const float height =  2.0f*_near * invDiffUd;
    const float xx     = (_rt + _lt) * invDiffRl;
    const float yy     = (_ut + _dt) * invDiffUd;
    sx__mat4_projXYWH_RH(_result, xx, yy, width, height, _near, _far, _oglNdc);
}

static void sx__mat4_proj_LH(sx_mat4* _result, float _ut, float _dt, float _lt, float _rt, float _near, float _far, bool _oglNdc)
{
    const float invDiffRl = 1.0f/(_rt - _lt);
    const float invDiffUd = 1.0f/(_ut - _dt);
    const float width  =  2.0f*_near * invDiffRl;
    const float height =  2.0f*_near * invDiffUd;
    const float xx     = (_rt + _lt) * invDiffRl;
    const float yy     = (_ut + _dt) * invDiffUd;
    sx__mat4_projXYWH_LH(_result, xx, yy, width, height, _near, _far, _oglNdc);
}

static void sx__mat4_proj_fov_LH(sx_mat4* _result, const float _fov[4], float _near, float _far, bool _oglNdc)
{
    sx__mat4_proj_LH(_result, _fov[0], _fov[1], _fov[2], _fov[3], _near, _far, _oglNdc);
}

static void sx__mat4_proj_fov_RH(sx_mat4* _result, const float _fov[4], float _near, float _far, bool _oglNdc)
{
    sx__mat4_proj_RH(_result, _fov[0], _fov[1], _fov[2], _fov[3], _near, _far, _oglNdc);
}

static void sx__mat4_proj_fovY_RH(sx_mat4* _result, float _fovy, float _aspect, float _near, float _far, bool _oglNdc)
{
    const float height = 1.0f/sx_tan(sx_torad(_fovy)*0.5f);
    const float width  = height * 1.0f/_aspect;
    sx__mat4_projXYWH_RH(_result, 0.0f, 0.0f, width, height, _near, _far, _oglNdc);
}

static void sx__mat4_proj_fovY_LH(sx_mat4* _result, float _fovy, float _aspect, float _near, float _far, bool _oglNdc)
{
    const float height = 1.0f/sx_tan(sx_torad(_fovy)*0.5f);
    const float width  = height * 1.0f/_aspect;
    sx__mat4_projXYWH_LH(_result, 0.0f, 0.0f, width, height, _near, _far, _oglNdc);
}

void sx_mat4_proj(sx_mat4* _result, float _ut, float _dt, float _lt, float _rt, float _near, float _far, bool _oglNdc)
{
    sx__mat4_proj_LH(_result, _ut, _dt, _lt, _rt, _near, _far, _oglNdc);
}

void sx_mat4_proj_fov(sx_mat4* _result, const float _fov[4], float _near, float _far, bool _oglNdc)
{
    sx__mat4_proj_fov_LH(_result, _fov, _near, _far, _oglNdc);
}

void sx_mat4_proj_fovY(sx_mat4* _result, float _fovy, float _aspect, float _near, float _far, bool _oglNdc)
{
    sx__mat4_proj_fovY_LH(_result, _fovy, _aspect, _near, _far, _oglNdc);
}

void sx_mat4_projLH(sx_mat4* _result, float _ut, float _dt, float _lt, float _rt, float _near, float _far, bool _oglNdc)
{
    sx__mat4_proj_LH(_result, _ut, _dt, _lt, _rt, _near, _far, _oglNdc);
}

void sx_mat4_projLH_fov(sx_mat4* _result, const float _fov[4], float _near, float _far, bool _oglNdc)
{
    sx__mat4_proj_fov_LH(_result, _fov, _near, _far, _oglNdc);
}

void sx_mat4_projLH_fovY(sx_mat4* _result, float _fovy, float _aspect, float _near, float _far, bool _oglNdc)
{
    sx__mat4_proj_fovY_LH(_result, _fovy, _aspect, _near, _far, _oglNdc);
}

void sx_mat4_projRH(sx_mat4* _result, float _ut, float _dt, float _lt, float _rt, float _near, float _far, bool _oglNdc)
{
    sx__mat4_proj_RH(_result, _ut, _dt, _lt, _rt, _near, _far, _oglNdc);
}

void sx_mat4_projRH_fov(sx_mat4* _result, const float _fov[4], float _near, float _far, bool _oglNdc)
{
   sx__mat4_proj_fov_RH(_result, _fov, _near, _far, _oglNdc);
}

void sx_mat4_projRH_fovY(sx_mat4* _result, float _fovy, float _aspect, float _near, float _far, bool _oglNdc)
{
    sx__mat4_proj_fovY_RH(_result, _fovy, _aspect, _near, _far, _oglNdc);
}

static void sx__mat4_proj_infXYWH_RH(sx_mat4* _result, float _x, float _y, float _width, float _height, float _near, bool _oglNdc)
{
    float aa;
    float bb;
    aa = 1.0f;
    bb = _oglNdc ? 2.0f*_near : _near;

    sx_memset(_result->f, 0, sizeof(float)*16);
    _result->f[ 0] = _width;
    _result->f[ 5] = _height;
    _result->f[ 2] = _x;
    _result->f[ 6] = _y;
    _result->f[10] = -aa;
    _result->f[14] = -1.0f;
    _result->f[11] = -bb;
}

static void sx__mat4_proj_infXYWH_LH(sx_mat4* _result, float _x, float _y, float _width, float _height, float _near, bool _oglNdc)
{
    float aa;
    float bb;
    aa = 1.0f;
    bb = _oglNdc ? 2.0f*_near : _near;

    sx_memset(_result->f, 0, sizeof(float)*16);
    _result->f[ 0] = _width;
    _result->f[ 5] = _height;
    _result->f[ 2] = -_x;
    _result->f[ 6] = -_y;
    _result->f[10] = aa;
    _result->f[14] = 1.0f;
    _result->f[11] = -bb;
}


static void sx__mat4_proj_infXYWH_RH_inv(sx_mat4* _result, float _x, float _y, float _width, float _height, float _near, bool _oglNdc)
{
    float aa;
    float bb;
    aa = _oglNdc ?       -1.0f :   0.0f;
    bb = _oglNdc ? -2.0f*_near : -_near;

    sx_memset(_result->f, 0, sizeof(float)*16);
    _result->f[ 0] = _width;
    _result->f[ 5] = _height;
    _result->f[ 2] = _x;
    _result->f[ 6] = _y;
    _result->f[10] = -aa;
    _result->f[14] = -1.0f;
    _result->f[11] = -bb;
}

static void sx__mat4_proj_infXYWH_LH_inv(sx_mat4* _result, float _x, float _y, float _width, float _height, float _near, bool _oglNdc)
{
    float aa;
    float bb;
    aa = _oglNdc ?       -1.0f :   0.0f;
    bb = _oglNdc ? -2.0f*_near : -_near;

    sx_memset(_result->f, 0, sizeof(float)*16);
    _result->f[ 0] = _width;
    _result->f[ 5] = _height;
    _result->f[ 2] = -_x;
    _result->f[ 6] = -_y;
    _result->f[10] = aa;
    _result->f[14] = 1.0f;
    _result->f[11] = -bb;
}

static void sx__mat4_proj_inf_RH(sx_mat4* _result, float _ut, float _dt, float _lt, float _rt, float _near, bool _oglNdc)
{
    const float invDiffRl = 1.0f/(_rt - _lt);
    const float invDiffUd = 1.0f/(_ut - _dt);
    const float width  =  2.0f*_near * invDiffRl;
    const float height =  2.0f*_near * invDiffUd;
    const float xx     = (_rt + _lt) * invDiffRl;
    const float yy     = (_ut + _dt) * invDiffUd;
    sx__mat4_proj_infXYWH_RH(_result, xx, yy, width, height, _near, _oglNdc);
}

static void sx__mat4_proj_inf_LH(sx_mat4* _result, float _ut, float _dt, float _lt, float _rt, float _near, bool _oglNdc)
{
    const float invDiffRl = 1.0f/(_rt - _lt);
    const float invDiffUd = 1.0f/(_ut - _dt);
    const float width  =  2.0f*_near * invDiffRl;
    const float height =  2.0f*_near * invDiffUd;
    const float xx     = (_rt + _lt) * invDiffRl;
    const float yy     = (_ut + _dt) * invDiffUd;
    sx__mat4_proj_infXYWH_LH(_result, xx, yy, width, height, _near, _oglNdc);
}

static void sx__mat4_proj_inf_RH_inv(sx_mat4* _result, float _ut, float _dt, float _lt, float _rt, float _near, bool _oglNdc)
{
    const float invDiffRl = 1.0f/(_rt - _lt);
    const float invDiffUd = 1.0f/(_ut - _dt);
    const float width  =  2.0f*_near * invDiffRl;
    const float height =  2.0f*_near * invDiffUd;
    const float xx     = (_rt + _lt) * invDiffRl;
    const float yy     = (_ut + _dt) * invDiffUd;
    sx__mat4_proj_infXYWH_RH_inv(_result, xx, yy, width, height, _near, _oglNdc);
}

static void sx__mat4_proj_inf_LH_inv(sx_mat4* _result, float _ut, float _dt, float _lt, float _rt, float _near, bool _oglNdc)
{
    const float invDiffRl = 1.0f/(_rt - _lt);
    const float invDiffUd = 1.0f/(_ut - _dt);
    const float width  =  2.0f*_near * invDiffRl;
    const float height =  2.0f*_near * invDiffUd;
    const float xx     = (_rt + _lt) * invDiffRl;
    const float yy     = (_ut + _dt) * invDiffUd;
    sx__mat4_proj_infXYWH_LH_inv(_result, xx, yy, width, height, _near, _oglNdc);
}

static void sx__mat4_proj_inf_fov_RH(sx_mat4* _result, const float _fov[4], float _near, bool _oglNdc)
{
    sx__mat4_proj_inf_RH(_result, _fov[0], _fov[1], _fov[2], _fov[3], _near, _oglNdc);
}
static void sx__mat4_proj_inf_fov_LH(sx_mat4* _result, const float _fov[4], float _near, bool _oglNdc)
{
    sx__mat4_proj_inf_LH(_result, _fov[0], _fov[1], _fov[2], _fov[3], _near, _oglNdc);
}
static void sx__mat4_proj_inf_fov_RH_inv(sx_mat4* _result, const float _fov[4], float _near, bool _oglNdc)
{
    sx__mat4_proj_inf_RH_inv(_result, _fov[0], _fov[1], _fov[2], _fov[3], _near, _oglNdc);
}
static void sx__mat4_proj_inf_fov_LH_inv(sx_mat4* _result, const float _fov[4], float _near, bool _oglNdc)
{
    sx__mat4_proj_inf_LH_inv(_result, _fov[0], _fov[1], _fov[2], _fov[3], _near, _oglNdc);
}

static void sx__mat4_proj_inf_fovY_RH(sx_mat4* _result, float _fovy, float _aspect, float _near, bool _oglNdc)
{
    const float height = 1.0f/sx_tan(sx_torad(_fovy)*0.5f);
    const float width  = height * 1.0f/_aspect;
    sx__mat4_proj_infXYWH_RH(_result, 0.0f, 0.0f, width, height, _near, _oglNdc);
}
static void sx__mat4_proj_inf_fovY_LH(sx_mat4* _result, float _fovy, float _aspect, float _near, bool _oglNdc)
{
    const float height = 1.0f/sx_tan(sx_torad(_fovy)*0.5f);
    const float width  = height * 1.0f/_aspect;
    sx__mat4_proj_infXYWH_LH(_result, 0.0f, 0.0f, width, height, _near, _oglNdc);
}
static void sx__mat4_proj_inf_fovY_RH_inv(sx_mat4* _result, float _fovy, float _aspect, float _near, bool _oglNdc)
{
    const float height = 1.0f/sx_tan(sx_torad(_fovy)*0.5f);
    const float width  = height * 1.0f/_aspect;
    sx__mat4_proj_infXYWH_RH_inv(_result, 0.0f, 0.0f, width, height, _near, _oglNdc);
}
static void sx__mat4_proj_inf_fovY_LH_inv(sx_mat4* _result, float _fovy, float _aspect, float _near, bool _oglNdc)
{
    const float height = 1.0f/sx_tan(sx_torad(_fovy)*0.5f);
    const float width  = height * 1.0f/_aspect;
    sx__mat4_proj_infXYWH_LH_inv(_result, 0.0f, 0.0f, width, height, _near, _oglNdc);
}

void sx_mat4_proj_inf_fov(sx_mat4* _result, const float _fov[4], float _near, bool _oglNdc)
{
    sx__mat4_proj_inf_fov_LH(_result, _fov, _near, _oglNdc);
}

void sx_mat4_proj_inf(sx_mat4* _result, float _ut, float _dt, float _lt, float _rt, float _near, bool _oglNdc)
{
    sx__mat4_proj_inf_LH(_result, _ut, _dt, _lt, _rt, _near, _oglNdc);
}

void sx_mat4_proj_inf_fovY(sx_mat4* _result, float _fovy, float _aspect, float _near, bool _oglNdc)
{
    sx__mat4_proj_inf_fovY_LH(_result, _fovy, _aspect, _near, _oglNdc);
}

void sx_mat4_proj_infLH(sx_mat4* _result, float _ut, float _dt, float _lt, float _rt, float _near, bool _oglNdc)
{
    sx__mat4_proj_inf_LH(_result, _ut, _dt, _lt, _rt, _near, _oglNdc);
}

void sx_mat4_proj_infLH_fov(sx_mat4* _result, const float _fov[4], float _near, bool _oglNdc)
{
    sx__mat4_proj_inf_fov_LH(_result, _fov, _near, _oglNdc);
}

void sx_mat4_proj_infLH_fovY(sx_mat4* _result, float _fovy, float _aspect, float _near, bool _oglNdc)
{
    sx__mat4_proj_inf_fovY_LH(_result, _fovy, _aspect, _near, _oglNdc);
}

void sx_mat4_proj_infRH(sx_mat4* _result, float _ut, float _dt, float _lt, float _rt, float _near, bool _oglNdc)
{
    sx__mat4_proj_inf_RH(_result, _ut, _dt, _lt, _rt, _near, _oglNdc);
}

void sx_mat4_proj_infRH_fov(sx_mat4* _result, const float _fov[4], float _near, bool _oglNdc)
{
    sx__mat4_proj_inf_fov_RH(_result, _fov, _near, _oglNdc);
}

void sx_mat4_proj_infRH_fovY(sx_mat4* _result, float _fovy, float _aspect, float _near, bool _oglNdc)
{
    sx__mat4_proj_inf_fovY_RH(_result, _fovy, _aspect, _near, _oglNdc);
}

void sx_mat4_projrev_infLH(sx_mat4* _result, float _ut, float _dt, float _lt, float _rt, float _near, bool _oglNdc)
{
    sx__mat4_proj_inf_LH_inv(_result, _ut, _dt, _lt, _rt, _near, _oglNdc);
}

void sx_mat4_projrev_infLH_fov(sx_mat4* _result, const float _fov[4], float _near, bool _oglNdc)
{
    sx__mat4_proj_inf_fov_LH_inv(_result, _fov, _near, _oglNdc);
}

void sx_mat4_projrev_infLH_fovY(sx_mat4* _result, float _fovy, float _aspect, float _near, bool _oglNdc)
{
    sx__mat4_proj_inf_fovY_LH_inv(_result, _fovy, _aspect, _near, _oglNdc);
}

void sx_mat4_projrev_infRH(sx_mat4* _result, float _ut, float _dt, float _lt, float _rt, float _near, bool _oglNdc)
{
    sx__mat4_proj_inf_RH_inv(_result, _ut, _dt, _lt, _rt, _near, _oglNdc);
}

void sx_mat4_projrev_infRH_fov(sx_mat4* _result, const float _fov[4], float _near, bool _oglNdc)
{
    sx__mat4_proj_inf_fov_RH_inv(_result, _fov, _near, _oglNdc);
}

void sx_mat4_projrev_infRH_fovY(sx_mat4* _result, float _fovy, float _aspect, float _near, bool _oglNdc)
{
    sx__mat4_proj_inf_fovY_RH_inv(_result, _fovy, _aspect, _near, _oglNdc);
}

static void sx__mat4_orthoLH(sx_mat4* _result, float _left, float _right, float _bottom, float _top, float _near, float _far, float _offset, bool _oglNdc)
{
    const float aa = 2.0f/(_right - _left);
    const float bb = 2.0f/(_top - _bottom);
    const float cc = (_oglNdc ? 2.0f : 1.0f) / (_far - _near);
    const float dd = (_left + _right )/(_left   - _right);
    const float ee = (_top  + _bottom)/(_bottom - _top  );
    const float ff = _oglNdc
        ? (_near + _far)/(_near - _far)
        :  _near        /(_near - _far);

    sx_memset(_result->f, 0, sizeof(float)*16);
    _result->f[ 0] = aa;
    _result->f[ 5] = bb;
    _result->f[10] = cc;
    _result->f[ 3] = dd + _offset;
    _result->f[ 7] = ee;
    _result->f[11] = ff;
    _result->f[15] = 1.0f;
}

static void sx__mat4_orthoRH(sx_mat4* _result, float _left, float _right, float _bottom, float _top, float _near, float _far, float _offset, bool _oglNdc)
{
    const float aa = 2.0f/(_right - _left);
    const float bb = 2.0f/(_top - _bottom);
    const float cc = (_oglNdc ? 2.0f : 1.0f) / (_far - _near);
    const float dd = (_left + _right )/(_left   - _right);
    const float ee = (_top  + _bottom)/(_bottom - _top  );
    const float ff = _oglNdc
        ? (_near + _far)/(_near - _far)
        :  _near        /(_near - _far);

    sx_memset(_result, 0, sizeof(float)*16);
    _result->f[ 0] = aa;
    _result->f[ 5] = bb;
    _result->f[10] = -cc;
    _result->f[ 3] = dd + _offset;
    _result->f[ 7] = ee;
    _result->f[11] = ff;
    _result->f[15] = 1.0f;
}

void sx_mat4_ortho(sx_mat4* _result, float _left, float _right, float _bottom, float _top, float _near, float _far, float _offset, bool _oglNdc)
{
    sx__mat4_orthoLH(_result, _left, _right, _bottom, _top, _near, _far, _offset, _oglNdc);
}

void sx_mat4_orthoLH(sx_mat4* _result, float _left, float _right, float _bottom, float _top, float _near, float _far, float _offset, bool _oglNdc)
{
    sx__mat4_orthoLH(_result, _left, _right, _bottom, _top, _near, _far, _offset, _oglNdc);
}

void sx_mat4_orthoRH(sx_mat4* _result, float _left, float _right, float _bottom, float _top, float _near, float _far, float _offset, bool _oglNdc)
{
    sx__mat4_orthoRH(_result, _left, _right, _bottom, _top, _near, _far, _offset, _oglNdc);
}

sx_mat4 sx_mat4_SRT(float _sx, float _sy, float _sz, float _ax, float _ay, float _az, float _tx, float _ty, float _tz)
{
    const float sx = sx_sin(_ax);
    const float cx = sx_cos(_ax);
    const float sy = sx_sin(_ay);
    const float cy = sx_cos(_ay);
    const float sz = sx_sin(_az);
    const float cz = sx_cos(_az);

    const float sxsz = sx*sz;
    const float cycz = cy*cz;

    return sx_mat4f( _sx * (cycz - sxsz*sy),  _sx * -cx*sz,  _sx * (cz*sy + cy*sxsz), 0.0f,
                    _sy * (cz*sx*sy + cy*sz), _sy * cx*cz, _sy * (sy*sz -cycz*sx), 0.0f,
                    _sz * -cx*sy, _sz * sx,  _sz * cx*cy, 0.0f, 
                    _tx, _ty, _tz, 1.0f);
}

sx_mat3 sx_mat3_inv(const sx_mat3* _a)
{
    float xx = _a->f[0];
    float xy = _a->f[3];
    float xz = _a->f[6];
    float yx = _a->f[1];
    float yy = _a->f[4];
    float yz = _a->f[7];
    float zx = _a->f[2];
    float zy = _a->f[5];
    float zz = _a->f[8];

    float det = 0.0f;
    det += xx * (yy*zz - yz*zy);
    det -= xy * (yx*zz - yz*zx);
    det += xz * (yx*zy - yy*zx);

    float det_rcp = 1.0f/det;

    return sx_mat3f(+(yy*zz - yz*zy) * det_rcp, -(xy*zz - xz*zy) * det_rcp, +(xy*yz - xz*yy) * det_rcp,
                    -(yx*zz - yz*zx) * det_rcp, +(xx*zz - xz*zx) * det_rcp, -(xx*yz - xz*yx) * det_rcp,
                    +(yx*zy - yy*zx) * det_rcp, -(xx*zy - xy*zx) * det_rcp, +(xx*yy - xy*yx) * det_rcp);
}

sx_mat4 sx_mat4_inv(const sx_mat4* _a)
{
    float xx = _a->f[ 0];
    float xy = _a->f[ 4];
    float xz = _a->f[ 8];
    float xw = _a->f[12];
    float yx = _a->f[ 1];
    float yy = _a->f[ 5];
    float yz = _a->f[ 9];
    float yw = _a->f[13];
    float zx = _a->f[ 2];
    float zy = _a->f[ 6];
    float zz = _a->f[10];
    float zw = _a->f[14];
    float wx = _a->f[ 3];
    float wy = _a->f[ 7];
    float wz = _a->f[11];
    float ww = _a->f[15];

    float det = 0.0f;
    det += xx * (yy*(zz*ww - zw*wz) - yz*(zy*ww - zw*wy) + yw*(zy*wz - zz*wy) );
    det -= xy * (yx*(zz*ww - zw*wz) - yz*(zx*ww - zw*wx) + yw*(zx*wz - zz*wx) );
    det += xz * (yx*(zy*ww - zw*wy) - yy*(zx*ww - zw*wx) + yw*(zx*wy - zy*wx) );
    det -= xw * (yx*(zy*wz - zz*wy) - yy*(zx*wz - zz*wx) + yz*(zx*wy - zy*wx) );

    float det_rcp = 1.0f/det;

    return sx_mat4f(
        +(yy*(zz*ww - wz*zw) - yz*(zy*ww - wy*zw) + yw*(zy*wz - wy*zz) ) * det_rcp,
        -(xy*(zz*ww - wz*zw) - xz*(zy*ww - wy*zw) + xw*(zy*wz - wy*zz) ) * det_rcp,
        +(xy*(yz*ww - wz*yw) - xz*(yy*ww - wy*yw) + xw*(yy*wz - wy*yz) ) * det_rcp,
        -(xy*(yz*zw - zz*yw) - xz*(yy*zw - zy*yw) + xw*(yy*zz - zy*yz) ) * det_rcp,

        -(yx*(zz*ww - wz*zw) - yz*(zx*ww - wx*zw) + yw*(zx*wz - wx*zz) ) * det_rcp,
        +(xx*(zz*ww - wz*zw) - xz*(zx*ww - wx*zw) + xw*(zx*wz - wx*zz) ) * det_rcp,
        -(xx*(yz*ww - wz*yw) - xz*(yx*ww - wx*yw) + xw*(yx*wz - wx*yz) ) * det_rcp,
        +(xx*(yz*zw - zz*yw) - xz*(yx*zw - zx*yw) + xw*(yx*zz - zx*yz) ) * det_rcp,

        +(yx*(zy*ww - wy*zw) - yy*(zx*ww - wx*zw) + yw*(zx*wy - wx*zy) ) * det_rcp,
        -(xx*(zy*ww - wy*zw) - xy*(zx*ww - wx*zw) + xw*(zx*wy - wx*zy) ) * det_rcp,
        +(xx*(yy*ww - wy*yw) - xy*(yx*ww - wx*yw) + xw*(yx*wy - wx*yy) ) * det_rcp,
        -(xx*(yy*zw - zy*yw) - xy*(yx*zw - zx*yw) + xw*(yx*zy - zx*yy) ) * det_rcp,

        -(yx*(zy*wz - wy*zz) - yy*(zx*wz - wx*zz) + yz*(zx*wy - wx*zy) ) * det_rcp,
        +(xx*(zy*wz - wy*zz) - xy*(zx*wz - wx*zz) + xz*(zx*wy - wx*zy) ) * det_rcp,
        -(xx*(yy*wz - wy*yz) - xy*(yx*wz - wx*yz) + xz*(yx*wy - wx*yy) ) * det_rcp,
        +(xx*(yy*zz - zy*yz) - xy*(yx*zz - zx*yz) + xz*(yx*zy - zx*yy) ) * det_rcp);
}

sx_vec2 sx_vec2_calc_linearfit2D(const sx_vec2* _points, int _num)
{
    float sumX  = 0.0f;
    float sumY  = 0.0f;
    float sumXX = 0.0f;
    float sumXY = 0.0f;

    for (int ii = 0; ii < _num; ++ii) {
        float xx = _points[ii].f[0];
        float yy = _points[ii].f[1];
        sumX  += xx;
        sumY  += yy;
        sumXX += xx*xx;
        sumXY += xx*yy;
    }

    // [ sum(x^2) sum(x)    ] [ A ] = [ sum(x*y) ]
    // [ sum(x)   numPoints ] [ B ]   [ sum(y)   ]

    float det = (sumXX*_num - sumX*sumX);
    float invDet = 1.0f/det;

    return sx_vec2f((-sumX * sumY + _num * sumXY) * invDet, 
                    (sumXX * sumY - sumX       * sumXY) * invDet);
}

sx_vec3 sx_vec3_calc_linearfit3D(const sx_vec3* _points, int _num)
{
    float sumX  = 0.0f;
    float sumY  = 0.0f;
    float sumZ  = 0.0f;
    float sumXX = 0.0f;
    float sumXY = 0.0f;
    float sumXZ = 0.0f;
    float sumYY = 0.0f;
    float sumYZ = 0.0f;

    for (int ii = 0; ii < _num; ++ii)
    {
        float xx = _points[ii].f[0];
        float yy = _points[ii].f[1];
        float zz = _points[ii].f[2];

        sumX  += xx;
        sumY  += yy;
        sumZ  += zz;
        sumXX += xx*xx;
        sumXY += xx*yy;
        sumXZ += xx*zz;
        sumYY += yy*yy;
        sumYZ += yy*zz;
    }

    // [ sum(x^2) sum(x*y) sum(x)    ] [ A ]   [ sum(x*z) ]
    // [ sum(x*y) sum(y^2) sum(y)    ] [ B ] = [ sum(y*z) ]
    // [ sum(x)   sum(y)   numPoints ] [ C ]   [ sum(z)   ]

    sx_mat3 mat = sx_mat3f(
        sumXX, sumXY, sumX,
        sumXY, sumYY, sumY,
        sumX,  sumY,  (float)(_num));
    sx_mat3 mat_inv = sx_mat3_inv(&mat);

    return sx_vec3f(mat_inv.f[0]*sumXZ + mat_inv.f[1]*sumYZ + mat_inv.f[2]*sumZ,
                    mat_inv.f[3]*sumXZ + mat_inv.f[4]*sumYZ + mat_inv.f[5]*sumZ,
                    mat_inv.f[6]*sumXZ + mat_inv.f[7]*sumYZ + mat_inv.f[8]*sumZ);
}

void sx_color_RGBtoHSV(float _hsv[3], const float _rgb[3])
{
    const float rr = _rgb[0];
    const float gg = _rgb[1];
    const float bb = _rgb[2];

    const float s0 = sx_step(bb, gg);

    const float px = sx_lerp(bb,        gg,         s0);
    const float py = sx_lerp(gg,        bb,         s0);
    const float pz = sx_lerp(-1.0f,     0.0f,       s0);
    const float pw = sx_lerp(2.0f/3.0f, -1.0f/3.0f, s0);

    const float s1 = sx_step(px, rr);

    const float qx = sx_lerp(px, rr, s1);
    const float qy = py;
    const float qz = sx_lerp(pw, pz, s1);
    const float qw = sx_lerp(rr, px, s1);

    const float dd = qx - sx_min(qw, qy);
    const float ee = 1.0e-10f;

    _hsv[0] = sx_abs(qz + (qw - qy) / (6.0f * dd + ee) );
    _hsv[1] = dd / (qx + ee);
    _hsv[2] = qx;
}

void sx_color_HSVtoRGB(float _rgb[3], const float _hsv[3])
{
    const float hh = _hsv[0];
    const float ss = _hsv[1];
    const float vv = _hsv[2];

    const float px = sx_abs(sx_fract(hh + 1.0f     ) * 6.0f - 3.0f);
    const float py = sx_abs(sx_fract(hh + 2.0f/3.0f) * 6.0f - 3.0f);
    const float pz = sx_abs(sx_fract(hh + 1.0f/3.0f) * 6.0f - 3.0f);

    _rgb[0] = vv * sx_lerp(1.0f, sx_clamp(px - 1.0f, 0.0f, 1.0f), ss);
    _rgb[1] = vv * sx_lerp(1.0f, sx_clamp(py - 1.0f, 0.0f, 1.0f), ss);
    _rgb[2] = vv * sx_lerp(1.0f, sx_clamp(pz - 1.0f, 0.0f, 1.0f), ss);
}

sx_mat3 sx_mat3_mul(const sx_mat3* _a, const sx_mat3* _b)
{
    return sx_mat3fv(sx_mat3_mul_vec3(_a, _b->col1).f,
                     sx_mat3_mul_vec3(_a, _b->col2).f,
                     sx_mat3_mul_vec3(_a, _b->col3).f);
}

// Reference: http://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToQuaternion/
sx_quat sx_mat4_calc_quat(const sx_mat4* _mat)
{
    float trace = _mat->f[0] + _mat->f[5] + _mat->f[10];
    if (trace > 0.00001f) {
        float s = sx_rsqrt(trace + 1.0f) * 0.5f;
        return sx_quat4f((_mat->f[9] - _mat->f[6]) * s, 
                         (_mat->f[2] - _mat->f[8]) * s,
                         (_mat->f[4] - _mat->f[1]) * s,
                         0.25f / s);
    } else {
        if (_mat->f[0] > _mat->f[5] && _mat->f[0] > _mat->f[10]) {
            float s = 2.0f * sx_sqrt(1.0f + _mat->f[0] - _mat->f[5] - _mat->f[10]);
            return sx_quat4f(0.25f * s, 
                             (_mat->f[1] + _mat->f[4]) / s, 
                             (_mat->f[2] + _mat->f[8]) / s, 
                             (_mat->f[9] - _mat->f[6]) / s);
        } else if (_mat->f[5] > _mat->f[10]) {
            float s = 2.0f * sx_sqrt(1.0f + _mat->f[5] - _mat->f[0] - _mat->f[10]);
            return sx_quat4f((_mat->f[1] + _mat->f[4]) / s, 
                            0.25f * s,
                            (_mat->f[6] + _mat->f[9]) / s,
                            (_mat->f[2] - _mat->f[8]) / s);
        } else {
            float s = 2.0f * sx_sqrt(1.0f + _mat->f[10] - _mat->f[0] - _mat->f[5]);
            return sx_quat4f((_mat->f[2] + _mat->f[8]) / s,
                             (_mat->f[6] + _mat->f[9]) / s,
                             0.25f * s,
                             (_mat->f[4] - _mat->f[1]) / s);
        }
    }
}

sx_mat4 sx_mat4x_inv(const sx_mat4* _mat)
{
    float det = (_mat->f[0] * (_mat->f[5]*_mat->f[10] - _mat->f[6]*_mat->f[9]) +
                 _mat->f[1] * (_mat->f[6]*_mat->f[8] - _mat->f[4]*_mat->f[10]) +
                 _mat->f[2] * (_mat->f[4]*_mat->f[9] - _mat->f[5]*_mat->f[8]));
    float det_rcp = 1.0f / det;
    float tx = _mat->f[12];
    float ty = _mat->f[13];
    float tz = _mat->f[14];
    
    sx_mat4 r = sx_mat4f((_mat->f[5]*_mat->f[10] - _mat->f[6]*_mat->f[9]) * det_rcp,
                (_mat->f[2]*_mat->f[9] - _mat->f[1]*_mat->f[10]) * det_rcp,
                (_mat->f[1]*_mat->f[6] - _mat->f[2]*_mat->f[5]) * det_rcp,
                0.0f,
                (_mat->f[6]*_mat->f[8] - _mat->f[4]*_mat->f[10]) * det_rcp,
                (_mat->f[0]*_mat->f[10] - _mat->f[2]*_mat->f[8]) * det_rcp,
                (_mat->f[2]*_mat->f[4] - _mat->f[0]*_mat->f[6]) * det_rcp,
                0,
                (_mat->f[4]*_mat->f[9] - _mat->f[5]*_mat->f[8]) * det_rcp,
                (_mat->f[1]*_mat->f[8] - _mat->f[0]*_mat->f[9]) * det_rcp,
                (_mat->f[0]*_mat->f[5] - _mat->f[1]*_mat->f[4]) * det_rcp,
                0,
                0.0f,
                0.0f,
                0.0f,
                1.0f);

    r.m41 = -(tx*r.f[0] + ty*r.f[4] + tz*r.f[8]);
    r.m42 = -(tx*r.f[1] + ty*r.f[5] + tz*r.f[9]);
    r.m43 = -(tx*r.f[2] + ty*r.f[6] + tz*r.f[10]);

    return r;
}

sx_mat4 sx_mat4_from_normal(const sx_vec3 _normal, float _scale, const sx_vec3 _pos)
{
    sx_vec3 tangent;
    sx_vec3 bitangent;
    sx_vec3_tangent(&tangent, &bitangent, _normal);

    sx_vec4 row1 = sx_vec4v3(sx_vec3_mulf(bitangent, _scale), 0.0f);
    sx_vec4 row2 = sx_vec4v3(sx_vec3_mulf(_normal,   _scale), 0.0f);
    sx_vec4 row3 = sx_vec4v3(sx_vec3_mulf(tangent,   _scale), 0.0f);

    return sx_mat4fv(row1.f, row2.f, row3.f, sx_vec4v3(_pos, 1.0f).f);
}

sx_mat4 sx_mat4_from_normal_angle(const sx_vec3 _normal, float _scale, const sx_vec3 _pos, float _angle)
{
    sx_vec3 tangent;
    sx_vec3 bitangent;
    sx_vec3_tangent_angle(&tangent, &bitangent, _normal, _angle);

    sx_vec4 row1 = sx_vec4v3(sx_vec3_mulf(bitangent, _scale), 0.0f);
    sx_vec4 row2 = sx_vec4v3(sx_vec3_mulf(_normal,   _scale), 0.0f);
    sx_vec4 row3 = sx_vec4v3(sx_vec3_mulf(tangent,   _scale), 0.0f);

    return sx_mat4fv(row1.f, row2.f, row3.f, sx_vec4v3(_pos, 1.0f).f);
}

sx_mat4 sx_mat4_quat(const sx_quat _quat)
{
    const float x = _quat.f[0];
    const float y = _quat.f[1];
    const float z = _quat.f[2];
    const float w = _quat.f[3];

    const float x2  =  x + x;
    const float y2  =  y + y;
    const float z2  =  z + z;
    const float x2x = x2 * x;
    const float x2y = x2 * y;
    const float x2z = x2 * z;
    const float x2w = x2 * w;
    const float y2y = y2 * y;
    const float y2z = y2 * z;
    const float y2w = y2 * w;
    const float z2z = z2 * z;
    const float z2w = z2 * w;

    return sx_mat4f(1.0f - (y2y + z2z),   x2y - z2w,          x2z + y2w,          0.0f,
                    x2y + z2w,            1.0f - (x2x + z2z), y2z - x2w,          0.0f,
                    x2z - y2w,            y2z + x2w,          1.0f - (x2x + y2y), 0.0f,
                    0.0f,                 0.0f,               0.0f,               1.0f);
}

sx_mat4 sx_mat4_mul(const sx_mat4* _a, const sx_mat4* _b)
{
    return sx_mat4fv(sx_mat4_mul_vec4(_a, _b->col1).f,
                     sx_mat4_mul_vec4(_a, _b->col2).f,
                     sx_mat4_mul_vec4(_a, _b->col3).f,
                     sx_mat4_mul_vec4(_a, _b->col4).f);
}

sx_vec3 sx_vec3_calc_normal(const sx_vec3 _va, const sx_vec3 _vb, const sx_vec3 _vc)
{
    sx_vec3 ba = sx_vec3_sub(_vb, _va);
    sx_vec3 ca = sx_vec3_sub(_vc, _va);
    sx_vec3 baca = sx_vec3_cross(ba, ca);

    return sx_vec3_norm(baca, NULL);
}

sx_vec4 sx_vec3_calc_plane(const sx_vec3 _va, const sx_vec3 _vb, const sx_vec3 _vc)
{
    sx_vec3 normal = sx_vec3_calc_normal(_va, _vb, _vc);
    return sx_vec4v3(normal, -sx_vec3_dot(normal, _va));
}
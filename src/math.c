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

#define square(_x) (_x*_x)
#define mad(_a, _b, _c) _a*_b + _c

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

    if (bits == 0
    ||  bits == 2) {
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

    const float xsq    = square(xx);
    const float tmp0   = mad(c10,  xsq, c8 );
    const float tmp1   = mad(tmp0, xsq, c6 );
    const float tmp2   = mad(tmp1, xsq, c4 );
    const float tmp3   = mad(tmp2, xsq, c2 );
    const float tmp4   = mad(tmp3, xsq, 1.0f);
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
    const float tmp0   = mad(kAcosC3, absa, kAcosC2);
    const float tmp1   = mad(tmp0,    absa, kAcosC1);
    const float tmp2   = mad(tmp1,    absa, kAcosC0);
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
    const float mxysq  = square(mxy);
    const float tmp0   = mad(kAtan2C0, mxysq, kAtan2C1);
    const float tmp1   = mad(tmp0,     mxysq, kAtan2C2);
    const float tmp2   = mad(tmp1,     mxysq, kAtan2C3);
    const float tmp3   = mad(tmp2,     mxysq, kAtan2C4);
    const float tmp4   = mad(tmp3,     mxysq, kAtan2C5);
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
    const float hmlsq  = square(hml);
    const float tmp0   = mad(kExpC4, hmlsq, kExpC3);
    const float tmp1   = mad(tmp0,   hmlsq, kExpC2);
    const float tmp2   = mad(tmp1,   hmlsq, kExpC1);
    const float tmp3   = mad(tmp2,   hmlsq, kExpC0);
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
    const float s2     = square(ss);
    const float s4     = square(s2);

    const float tmp0   = mad(kLogC6, s4, kLogC4);
    const float tmp1   = mad(tmp0,   s4, kLogC2);
    const float tmp2   = mad(tmp1,   s4, kLogC0);
    const float t1     = s2*tmp2;

    const float tmp3   = mad(kLogC5, s4, kLogC3);
    const float tmp4   = mad(tmp3,   s4, kLogC1);
    const float t2     = s4*tmp4;

    const float t12    = t1 + t2;
    const float hfsq   = 0.5f*square(ff);
    const float result = hi - ( (hfsq - (ss*(hfsq+t12) + lo) ) - ff);

    return result;
}

static void sx__mat4_lookat(float* _result, const float* _eye, const float* _view, const float* _up)
{
    float up[3] = { 0.0f, 1.0f, 0.0f };
    if (NULL != _up) {
        up[0] = _up[0];
        up[1] = _up[1];
        up[2] = _up[2];
    }

    float tmp[4];
    sx_vec3_cross(tmp, up, _view);

    float right[4];
    sx_vec3_norm(right, tmp);

    sx_vec3_cross(up, _view, right);

    sx_memset(_result, 0, sizeof(float)*16);
    _result[ 0] = right[0];
    _result[ 1] = up[0];
    _result[ 2] = _view[0];

    _result[ 4] = right[1];
    _result[ 5] = up[1];
    _result[ 6] = _view[1];

    _result[ 8] = right[2];
    _result[ 9] = up[2];
    _result[10] = _view[2];

    _result[12] = -sx_vec3_dot(right, _eye);
    _result[13] = -sx_vec3_dot(up, _eye);
    _result[14] = -sx_vec3_dot(_view, _eye);
    _result[15] = 1.0f;
}

void sx_mat4_lookatLH(float* _result, const float* _eye, const float* _at, const float* _up)
{
    float tmp[4];
    sx_vec3_sub(tmp, _at, _eye);

    float view[4];
    sx_vec3_norm(view, tmp);

    sx__mat4_lookat(_result, _eye, view, _up);
}

void sx_mat4_lookatRH(float* _result, const float* _eye, const float* _at, const float* _up)
{
    float tmp[4];
    sx_vec3_sub(tmp, _eye, _at);

    float view[4];
    sx_vec3_norm(view, tmp);

    sx__mat4_lookat(_result, _eye, view, _up);
}

void sx_mat4_lookat(float* _result, const float* _eye, const float* _at, const float* _up)
{
    sx_mat4_lookatLH(_result, _eye, _at, _up);
}

static void sx__mat4_projXYWH_RH(float* _result, float _x, float _y, float _width, float _height, 
                                 float _near, float _far, bool _oglNdc)
{
    const float diff = _far-_near;
    const float aa = _oglNdc ? (     _far+_near)/diff : _far/diff;
    const float bb = _oglNdc ? (2.0f*_far*_near)/diff : _near*aa;

    sx_memset(_result, 0, sizeof(float)*16);
    _result[ 0] = _width;
    _result[ 5] = _height;
    _result[ 8] = _x;
    _result[ 9] = _y;
    _result[10] = -aa;
    _result[11] = -1.0f;
    _result[14] = -bb;
}

static void sx__mat4_projXYWH_LH(float* _result, float _x, float _y, float _width, float _height, 
                                 float _near, float _far, bool _oglNdc)
{
    const float diff = _far-_near;
    const float aa = _oglNdc ? (     _far+_near)/diff : _far/diff;
    const float bb = _oglNdc ? (2.0f*_far*_near)/diff : _near*aa;

    sx_memset(_result, 0, sizeof(float)*16);
    _result[ 0] = _width;
    _result[ 5] = _height;
    _result[ 8] = -_x;
    _result[ 9] = -_y;
    _result[10] = aa;
    _result[11] = 1.0f;
    _result[14] = -bb;
}

static void sx__mat4_proj_RH(float* _result, float _ut, float _dt, float _lt, float _rt, float _near, float _far, bool _oglNdc)
{
    const float invDiffRl = 1.0f/(_rt - _lt);
    const float invDiffUd = 1.0f/(_ut - _dt);
    const float width  =  2.0f*_near * invDiffRl;
    const float height =  2.0f*_near * invDiffUd;
    const float xx     = (_rt + _lt) * invDiffRl;
    const float yy     = (_ut + _dt) * invDiffUd;
    sx__mat4_projXYWH_RH(_result, xx, yy, width, height, _near, _far, _oglNdc);
}

static void sx__mat4_proj_LH(float* _result, float _ut, float _dt, float _lt, float _rt, float _near, float _far, bool _oglNdc)
{
    const float invDiffRl = 1.0f/(_rt - _lt);
    const float invDiffUd = 1.0f/(_ut - _dt);
    const float width  =  2.0f*_near * invDiffRl;
    const float height =  2.0f*_near * invDiffUd;
    const float xx     = (_rt + _lt) * invDiffRl;
    const float yy     = (_ut + _dt) * invDiffUd;
    sx__mat4_projXYWH_LH(_result, xx, yy, width, height, _near, _far, _oglNdc);
}

static void sx__mat4_proj_fov_LH(float* _result, const float _fov[4], float _near, float _far, bool _oglNdc)
{
    sx__mat4_proj_LH(_result, _fov[0], _fov[1], _fov[2], _fov[3], _near, _far, _oglNdc);
}

static void sx__mat4_proj_fov_RH(float* _result, const float _fov[4], float _near, float _far, bool _oglNdc)
{
    sx__mat4_proj_RH(_result, _fov[0], _fov[1], _fov[2], _fov[3], _near, _far, _oglNdc);
}

static void sx__mat4_proj_fovY_RH(float* _result, float _fovy, float _aspect, float _near, float _far, bool _oglNdc)
{
    const float height = 1.0f/sx_tan(sx_torad(_fovy)*0.5f);
    const float width  = height * 1.0f/_aspect;
    sx__mat4_projXYWH_RH(_result, 0.0f, 0.0f, width, height, _near, _far, _oglNdc);
}

static void sx__mat4_proj_fovY_LH(float* _result, float _fovy, float _aspect, float _near, float _far, bool _oglNdc)
{
    const float height = 1.0f/sx_tan(sx_torad(_fovy)*0.5f);
    const float width  = height * 1.0f/_aspect;
    sx__mat4_projXYWH_LH(_result, 0.0f, 0.0f, width, height, _near, _far, _oglNdc);
}

void sx_mat4_proj(float* _result, float _ut, float _dt, float _lt, float _rt, float _near, float _far, bool _oglNdc)
{
    sx__mat4_proj_LH(_result, _ut, _dt, _lt, _rt, _near, _far, _oglNdc);
}

void sx_mat4_proj_fov(float* _result, const float _fov[4], float _near, float _far, bool _oglNdc)
{
    sx__mat4_proj_fov_LH(_result, _fov, _near, _far, _oglNdc);
}

void sx_mat4_proj_fovY(float* _result, float _fovy, float _aspect, float _near, float _far, bool _oglNdc)
{
    sx__mat4_proj_fovY_LH(_result, _fovy, _aspect, _near, _far, _oglNdc);
}

void sx_mat4_projLH(float* _result, float _ut, float _dt, float _lt, float _rt, float _near, float _far, bool _oglNdc)
{
    sx__mat4_proj_LH(_result, _ut, _dt, _lt, _rt, _near, _far, _oglNdc);
}

void sx_mat4_projLH_fov(float* _result, const float _fov[4], float _near, float _far, bool _oglNdc)
{
    sx__mat4_proj_fov_LH(_result, _fov, _near, _far, _oglNdc);
}

void sx_mat4_projLH_fovY(float* _result, float _fovy, float _aspect, float _near, float _far, bool _oglNdc)
{
    sx__mat4_proj_fovY_LH(_result, _fovy, _aspect, _near, _far, _oglNdc);
}

void sx_mat4_projRH(float* _result, float _ut, float _dt, float _lt, float _rt, float _near, float _far, bool _oglNdc)
{
    sx__mat4_proj_RH(_result, _ut, _dt, _lt, _rt, _near, _far, _oglNdc);
}

void sx_mat4_projRH_fov(float* _result, const float _fov[4], float _near, float _far, bool _oglNdc)
{
   sx__mat4_proj_fov_RH(_result, _fov, _near, _far, _oglNdc);
}

void sx_mat4_projRH_fovY(float* _result, float _fovy, float _aspect, float _near, float _far, bool _oglNdc)
{
    sx__mat4_proj_fovY_RH(_result, _fovy, _aspect, _near, _far, _oglNdc);
}

static void sx__mat4_proj_infXYWH_RH(float* _result, float _x, float _y, float _width, float _height, float _near, bool _oglNdc)
{
    float aa;
    float bb;
    aa = 1.0f;
    bb = _oglNdc ? 2.0f*_near : _near;

    sx_memset(_result, 0, sizeof(float)*16);
    _result[ 0] = _width;
    _result[ 5] = _height;
    _result[ 8] = _x;
    _result[ 9] = _y;
    _result[10] = -aa;
    _result[11] = -1.0f;
    _result[14] = -bb;
}

static void sx__mat4_proj_infXYWH_LH(float* _result, float _x, float _y, float _width, float _height, float _near, bool _oglNdc)
{
    float aa;
    float bb;
    aa = 1.0f;
    bb = _oglNdc ? 2.0f*_near : _near;

    sx_memset(_result, 0, sizeof(float)*16);
    _result[ 0] = _width;
    _result[ 5] = _height;
    _result[ 8] = -_x;
    _result[ 9] = -_y;
    _result[10] = aa;
    _result[11] = 1.0f;
    _result[14] = -bb;
}


static void sx__mat4_proj_infXYWH_RH_inv(float* _result, float _x, float _y, float _width, float _height, float _near, bool _oglNdc)
{
    float aa;
    float bb;
    aa = _oglNdc ?       -1.0f :   0.0f;
    bb = _oglNdc ? -2.0f*_near : -_near;

    sx_memset(_result, 0, sizeof(float)*16);
    _result[ 0] = _width;
    _result[ 5] = _height;
    _result[ 8] = _x;
    _result[ 9] = _y;
    _result[10] = -aa;
    _result[11] = -1.0f;
    _result[14] = -bb;
}

static void sx__mat4_proj_infXYWH_LH_inv(float* _result, float _x, float _y, float _width, float _height, float _near, bool _oglNdc)
{
    float aa;
    float bb;
    aa = _oglNdc ?       -1.0f :   0.0f;
    bb = _oglNdc ? -2.0f*_near : -_near;

    sx_memset(_result, 0, sizeof(float)*16);
    _result[ 0] = _width;
    _result[ 5] = _height;
    _result[ 8] = -_x;
    _result[ 9] = -_y;
    _result[10] = aa;
    _result[11] = 1.0f;
    _result[14] = -bb;
}

static void sx__mat4_proj_inf_RH(float* _result, float _ut, float _dt, float _lt, float _rt, float _near, bool _oglNdc)
{
    const float invDiffRl = 1.0f/(_rt - _lt);
    const float invDiffUd = 1.0f/(_ut - _dt);
    const float width  =  2.0f*_near * invDiffRl;
    const float height =  2.0f*_near * invDiffUd;
    const float xx     = (_rt + _lt) * invDiffRl;
    const float yy     = (_ut + _dt) * invDiffUd;
    sx__mat4_proj_infXYWH_RH(_result, xx, yy, width, height, _near, _oglNdc);
}

static void sx__mat4_proj_inf_LH(float* _result, float _ut, float _dt, float _lt, float _rt, float _near, bool _oglNdc)
{
    const float invDiffRl = 1.0f/(_rt - _lt);
    const float invDiffUd = 1.0f/(_ut - _dt);
    const float width  =  2.0f*_near * invDiffRl;
    const float height =  2.0f*_near * invDiffUd;
    const float xx     = (_rt + _lt) * invDiffRl;
    const float yy     = (_ut + _dt) * invDiffUd;
    sx__mat4_proj_infXYWH_LH(_result, xx, yy, width, height, _near, _oglNdc);
}

static void sx__mat4_proj_inf_RH_inv(float* _result, float _ut, float _dt, float _lt, float _rt, float _near, bool _oglNdc)
{
    const float invDiffRl = 1.0f/(_rt - _lt);
    const float invDiffUd = 1.0f/(_ut - _dt);
    const float width  =  2.0f*_near * invDiffRl;
    const float height =  2.0f*_near * invDiffUd;
    const float xx     = (_rt + _lt) * invDiffRl;
    const float yy     = (_ut + _dt) * invDiffUd;
    sx__mat4_proj_infXYWH_RH_inv(_result, xx, yy, width, height, _near, _oglNdc);
}

static void sx__mat4_proj_inf_LH_inv(float* _result, float _ut, float _dt, float _lt, float _rt, float _near, bool _oglNdc)
{
    const float invDiffRl = 1.0f/(_rt - _lt);
    const float invDiffUd = 1.0f/(_ut - _dt);
    const float width  =  2.0f*_near * invDiffRl;
    const float height =  2.0f*_near * invDiffUd;
    const float xx     = (_rt + _lt) * invDiffRl;
    const float yy     = (_ut + _dt) * invDiffUd;
    sx__mat4_proj_infXYWH_LH_inv(_result, xx, yy, width, height, _near, _oglNdc);
}

static void sx__mat4_proj_inf_fov_RH(float* _result, const float _fov[4], float _near, bool _oglNdc)
{
    sx__mat4_proj_inf_RH(_result, _fov[0], _fov[1], _fov[2], _fov[3], _near, _oglNdc);
}
static void sx__mat4_proj_inf_fov_LH(float* _result, const float _fov[4], float _near, bool _oglNdc)
{
    sx__mat4_proj_inf_LH(_result, _fov[0], _fov[1], _fov[2], _fov[3], _near, _oglNdc);
}
static void sx__mat4_proj_inf_fov_RH_inv(float* _result, const float _fov[4], float _near, bool _oglNdc)
{
    sx__mat4_proj_inf_RH_inv(_result, _fov[0], _fov[1], _fov[2], _fov[3], _near, _oglNdc);
}
static void sx__mat4_proj_inf_fov_LH_inv(float* _result, const float _fov[4], float _near, bool _oglNdc)
{
    sx__mat4_proj_inf_LH_inv(_result, _fov[0], _fov[1], _fov[2], _fov[3], _near, _oglNdc);
}

static void sx__mat4_proj_inf_fovY_RH(float* _result, float _fovy, float _aspect, float _near, bool _oglNdc)
{
    const float height = 1.0f/sx_tan(sx_torad(_fovy)*0.5f);
    const float width  = height * 1.0f/_aspect;
    sx__mat4_proj_infXYWH_RH(_result, 0.0f, 0.0f, width, height, _near, _oglNdc);
}
static void sx__mat4_proj_inf_fovY_LH(float* _result, float _fovy, float _aspect, float _near, bool _oglNdc)
{
    const float height = 1.0f/sx_tan(sx_torad(_fovy)*0.5f);
    const float width  = height * 1.0f/_aspect;
    sx__mat4_proj_infXYWH_LH(_result, 0.0f, 0.0f, width, height, _near, _oglNdc);
}
static void sx__mat4_proj_inf_fovY_RH_inv(float* _result, float _fovy, float _aspect, float _near, bool _oglNdc)
{
    const float height = 1.0f/sx_tan(sx_torad(_fovy)*0.5f);
    const float width  = height * 1.0f/_aspect;
    sx__mat4_proj_infXYWH_RH_inv(_result, 0.0f, 0.0f, width, height, _near, _oglNdc);
}
static void sx__mat4_proj_inf_fovY_LH_inv(float* _result, float _fovy, float _aspect, float _near, bool _oglNdc)
{
    const float height = 1.0f/sx_tan(sx_torad(_fovy)*0.5f);
    const float width  = height * 1.0f/_aspect;
    sx__mat4_proj_infXYWH_LH_inv(_result, 0.0f, 0.0f, width, height, _near, _oglNdc);
}

void sx_mat4_proj_inf_fov(float* _result, const float _fov[4], float _near, bool _oglNdc)
{
    sx__mat4_proj_inf_fov_LH(_result, _fov, _near, _oglNdc);
}

void sx_mat4_proj_inf(float* _result, float _ut, float _dt, float _lt, float _rt, float _near, bool _oglNdc)
{
    sx__mat4_proj_inf_LH(_result, _ut, _dt, _lt, _rt, _near, _oglNdc);
}

void sx_mat4_proj_inf_fovY(float* _result, float _fovy, float _aspect, float _near, bool _oglNdc)
{
    sx__mat4_proj_inf_fovY_LH(_result, _fovy, _aspect, _near, _oglNdc);
}

void sx_mat4_proj_infLH(float* _result, float _ut, float _dt, float _lt, float _rt, float _near, bool _oglNdc)
{
    sx__mat4_proj_inf_LH(_result, _ut, _dt, _lt, _rt, _near, _oglNdc);
}

void sx_mat4_proj_infLH_fov(float* _result, const float _fov[4], float _near, bool _oglNdc)
{
    sx__mat4_proj_inf_fov_LH(_result, _fov, _near, _oglNdc);
}

void sx_mat4_proj_infLH_fovY(float* _result, float _fovy, float _aspect, float _near, bool _oglNdc)
{
    sx__mat4_proj_inf_fovY_LH(_result, _fovy, _aspect, _near, _oglNdc);
}

void sx_mat4_proj_infRH(float* _result, float _ut, float _dt, float _lt, float _rt, float _near, bool _oglNdc)
{
    sx__mat4_proj_inf_RH(_result, _ut, _dt, _lt, _rt, _near, _oglNdc);
}

void sx_mat4_proj_infRH_fov(float* _result, const float _fov[4], float _near, bool _oglNdc)
{
    sx__mat4_proj_inf_fov_RH(_result, _fov, _near, _oglNdc);
}

void sx_mat4_proj_infRH_fovY(float* _result, float _fovy, float _aspect, float _near, bool _oglNdc)
{
    sx__mat4_proj_inf_fovY_RH(_result, _fovy, _aspect, _near, _oglNdc);
}

void sx_mat4_projrev_infLH(float* _result, float _ut, float _dt, float _lt, float _rt, float _near, bool _oglNdc)
{
    sx__mat4_proj_inf_LH_inv(_result, _ut, _dt, _lt, _rt, _near, _oglNdc);
}

void sx_mat4_projrev_infLH_fov(float* _result, const float _fov[4], float _near, bool _oglNdc)
{
    sx__mat4_proj_inf_fov_LH_inv(_result, _fov, _near, _oglNdc);
}

void sx_mat4_projrev_infLH_fovY(float* _result, float _fovy, float _aspect, float _near, bool _oglNdc)
{
    sx__mat4_proj_inf_fovY_LH_inv(_result, _fovy, _aspect, _near, _oglNdc);
}

void sx_mat4_projrev_infRH(float* _result, float _ut, float _dt, float _lt, float _rt, float _near, bool _oglNdc)
{
    sx__mat4_proj_inf_RH_inv(_result, _ut, _dt, _lt, _rt, _near, _oglNdc);
}

void sx_mat4_projrev_infRH_fov(float* _result, const float _fov[4], float _near, bool _oglNdc)
{
    sx__mat4_proj_inf_fov_RH_inv(_result, _fov, _near, _oglNdc);
}

void sx_mat4_projrev_infRH_fovY(float* _result, float _fovy, float _aspect, float _near, bool _oglNdc)
{
    sx__mat4_proj_inf_fovY_RH_inv(_result, _fovy, _aspect, _near, _oglNdc);
}

static void sx__mat4_orthoLH(float* _result, float _left, float _right, float _bottom, float _top, float _near, float _far, float _offset, bool _oglNdc)
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
    _result[ 0] = aa;
    _result[ 5] = bb;
    _result[10] = cc;
    _result[12] = dd + _offset;
    _result[13] = ee;
    _result[14] = ff;
    _result[15] = 1.0f;
}

static void sx__mat4_orthoRH(float* _result, float _left, float _right, float _bottom, float _top, float _near, float _far, float _offset, bool _oglNdc)
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
    _result[ 0] = aa;
    _result[ 5] = bb;
    _result[10] = -cc;
    _result[12] = dd + _offset;
    _result[13] = ee;
    _result[14] = ff;
    _result[15] = 1.0f;
}

void sx_mat4_ortho(float* _result, float _left, float _right, float _bottom, float _top, float _near, float _far, float _offset, bool _oglNdc)
{
    sx__mat4_orthoLH(_result, _left, _right, _bottom, _top, _near, _far, _offset, _oglNdc);
}

void sx_mat4_orthoLH(float* _result, float _left, float _right, float _bottom, float _top, float _near, float _far, float _offset, bool _oglNdc)
{
    sx__mat4_orthoLH(_result, _left, _right, _bottom, _top, _near, _far, _offset, _oglNdc);
}

void sx_mat4_orthoRH(float* _result, float _left, float _right, float _bottom, float _top, float _near, float _far, float _offset, bool _oglNdc)
{
    sx__mat4_orthoRH(_result, _left, _right, _bottom, _top, _near, _far, _offset, _oglNdc);
}

void sx_mat4_rotateX(float* _result, float _ax)
{
    const float sx = sx_sin(_ax);
    const float cx = sx_cos(_ax);

    sx_memset(_result, 0, sizeof(float)*16);
    _result[ 0] = 1.0f;
    _result[ 5] = cx;
    _result[ 6] = -sx;
    _result[ 9] = sx;
    _result[10] = cx;
    _result[15] = 1.0f;
}

void sx_mat4_rotateY(float* _result, float _ay)
{
    const float sy = sx_sin(_ay);
    const float cy = sx_cos(_ay);

    sx_memset(_result, 0, sizeof(float)*16);
    _result[ 0] = cy;
    _result[ 2] = sy;
    _result[ 5] = 1.0f;
    _result[ 8] = -sy;
    _result[10] = cy;
    _result[15] = 1.0f;
}

void sx_mat4_rotateZ(float* _result, float _az)
{
    const float sz = sx_sin(_az);
    const float cz = sx_cos(_az);

    sx_memset(_result, 0, sizeof(float)*16);
    _result[ 0] = cz;
    _result[ 1] = -sz;
    _result[ 4] = sz;
    _result[ 5] = cz;
    _result[10] = 1.0f;
    _result[15] = 1.0f;
}

void sx_mat4_rotateXY(float* _result, float _ax, float _ay)
{
    const float sx = sx_sin(_ax);
    const float cx = sx_cos(_ax);
    const float sy = sx_sin(_ay);
    const float cy = sx_cos(_ay);

    sx_memset(_result, 0, sizeof(float)*16);
    _result[ 0] = cy;
    _result[ 2] = sy;
    _result[ 4] = sx*sy;
    _result[ 5] = cx;
    _result[ 6] = -sx*cy;
    _result[ 8] = -cx*sy;
    _result[ 9] = sx;
    _result[10] = cx*cy;
    _result[15] = 1.0f;
}

void sx_mat4_rotateXYZ(float* _result, float _ax, float _ay, float _az)
{
    const float sx = sx_sin(_ax);
    const float cx = sx_cos(_ax);
    const float sy = sx_sin(_ay);
    const float cy = sx_cos(_ay);
    const float sz = sx_sin(_az);
    const float cz = sx_cos(_az);

    sx_memset(_result, 0, sizeof(float)*16);
    _result[ 0] = cy*cz;
    _result[ 1] = -cy*sz;
    _result[ 2] = sy;
    _result[ 4] = cz*sx*sy + cx*sz;
    _result[ 5] = cx*cz - sx*sy*sz;
    _result[ 6] = -cy*sx;
    _result[ 8] = -cx*cz*sy + sx*sz;
    _result[ 9] = cz*sx + cx*sy*sz;
    _result[10] = cx*cy;
    _result[15] = 1.0f;
}

void sx_mat4_rotateZYX(float* _result, float _ax, float _ay, float _az)
{
    const float sx = sx_sin(_ax);
    const float cx = sx_cos(_ax);
    const float sy = sx_sin(_ay);
    const float cy = sx_cos(_ay);
    const float sz = sx_sin(_az);
    const float cz = sx_cos(_az);

    sx_memset(_result, 0, sizeof(float)*16);
    _result[ 0] = cy*cz;
    _result[ 1] = cz*sx*sy-cx*sz;
    _result[ 2] = cx*cz*sy+sx*sz;
    _result[ 4] = cy*sz;
    _result[ 5] = cx*cz + sx*sy*sz;
    _result[ 6] = -cz*sx + cx*sy*sz;
    _result[ 8] = -sy;
    _result[ 9] = cy*sx;
    _result[10] = cx*cy;
    _result[15] = 1.0f;
};

void sx_mat4_SRT(float* _result, float _sx, float _sy, float _sz, float _ax, float _ay, float _az, float _tx, float _ty, float _tz)
{
    const float sx = sx_sin(_ax);
    const float cx = sx_cos(_ax);
    const float sy = sx_sin(_ay);
    const float cy = sx_cos(_ay);
    const float sz = sx_sin(_az);
    const float cz = sx_cos(_az);

    const float sxsz = sx*sz;
    const float cycz = cy*cz;

    _result[ 0] = _sx * (cycz - sxsz*sy);
    _result[ 1] = _sx * -cx*sz;
    _result[ 2] = _sx * (cz*sy + cy*sxsz);
    _result[ 3] = 0.0f;

    _result[ 4] = _sy * (cz*sx*sy + cy*sz);
    _result[ 5] = _sy * cx*cz;
    _result[ 6] = _sy * (sy*sz -cycz*sx);
    _result[ 7] = 0.0f;

    _result[ 8] = _sz * -cx*sy;
    _result[ 9] = _sz * sx;
    _result[10] = _sz * cx*cy;
    _result[11] = 0.0f;

    _result[12] = _tx;
    _result[13] = _ty;
    _result[14] = _tz;
    _result[15] = 1.0f;
}

void sx_mat3_inv(float* _result, const float* _a)
{
    float xx = _a[0];
    float xy = _a[1];
    float xz = _a[2];
    float yx = _a[3];
    float yy = _a[4];
    float yz = _a[5];
    float zx = _a[6];
    float zy = _a[7];
    float zz = _a[8];

    float det = 0.0f;
    det += xx * (yy*zz - yz*zy);
    det -= xy * (yx*zz - yz*zx);
    det += xz * (yx*zy - yy*zx);

    float invDet = 1.0f/det;

    _result[0] = +(yy*zz - yz*zy) * invDet;
    _result[1] = -(xy*zz - xz*zy) * invDet;
    _result[2] = +(xy*yz - xz*yy) * invDet;

    _result[3] = -(yx*zz - yz*zx) * invDet;
    _result[4] = +(xx*zz - xz*zx) * invDet;
    _result[5] = -(xx*yz - xz*yx) * invDet;

    _result[6] = +(yx*zy - yy*zx) * invDet;
    _result[7] = -(xx*zy - xy*zx) * invDet;
    _result[8] = +(xx*yy - xy*yx) * invDet;
}

void sx_mat4_inv(float* _result, const float* _a)
{
    float xx = _a[ 0];
    float xy = _a[ 1];
    float xz = _a[ 2];
    float xw = _a[ 3];
    float yx = _a[ 4];
    float yy = _a[ 5];
    float yz = _a[ 6];
    float yw = _a[ 7];
    float zx = _a[ 8];
    float zy = _a[ 9];
    float zz = _a[10];
    float zw = _a[11];
    float wx = _a[12];
    float wy = _a[13];
    float wz = _a[14];
    float ww = _a[15];

    float det = 0.0f;
    det += xx * (yy*(zz*ww - zw*wz) - yz*(zy*ww - zw*wy) + yw*(zy*wz - zz*wy) );
    det -= xy * (yx*(zz*ww - zw*wz) - yz*(zx*ww - zw*wx) + yw*(zx*wz - zz*wx) );
    det += xz * (yx*(zy*ww - zw*wy) - yy*(zx*ww - zw*wx) + yw*(zx*wy - zy*wx) );
    det -= xw * (yx*(zy*wz - zz*wy) - yy*(zx*wz - zz*wx) + yz*(zx*wy - zy*wx) );

    float invDet = 1.0f/det;

    _result[ 0] = +(yy*(zz*ww - wz*zw) - yz*(zy*ww - wy*zw) + yw*(zy*wz - wy*zz) ) * invDet;
    _result[ 1] = -(xy*(zz*ww - wz*zw) - xz*(zy*ww - wy*zw) + xw*(zy*wz - wy*zz) ) * invDet;
    _result[ 2] = +(xy*(yz*ww - wz*yw) - xz*(yy*ww - wy*yw) + xw*(yy*wz - wy*yz) ) * invDet;
    _result[ 3] = -(xy*(yz*zw - zz*yw) - xz*(yy*zw - zy*yw) + xw*(yy*zz - zy*yz) ) * invDet;

    _result[ 4] = -(yx*(zz*ww - wz*zw) - yz*(zx*ww - wx*zw) + yw*(zx*wz - wx*zz) ) * invDet;
    _result[ 5] = +(xx*(zz*ww - wz*zw) - xz*(zx*ww - wx*zw) + xw*(zx*wz - wx*zz) ) * invDet;
    _result[ 6] = -(xx*(yz*ww - wz*yw) - xz*(yx*ww - wx*yw) + xw*(yx*wz - wx*yz) ) * invDet;
    _result[ 7] = +(xx*(yz*zw - zz*yw) - xz*(yx*zw - zx*yw) + xw*(yx*zz - zx*yz) ) * invDet;

    _result[ 8] = +(yx*(zy*ww - wy*zw) - yy*(zx*ww - wx*zw) + yw*(zx*wy - wx*zy) ) * invDet;
    _result[ 9] = -(xx*(zy*ww - wy*zw) - xy*(zx*ww - wx*zw) + xw*(zx*wy - wx*zy) ) * invDet;
    _result[10] = +(xx*(yy*ww - wy*yw) - xy*(yx*ww - wx*yw) + xw*(yx*wy - wx*yy) ) * invDet;
    _result[11] = -(xx*(yy*zw - zy*yw) - xy*(yx*zw - zx*yw) + xw*(yx*zy - zx*yy) ) * invDet;

    _result[12] = -(yx*(zy*wz - wy*zz) - yy*(zx*wz - wx*zz) + yz*(zx*wy - wx*zy) ) * invDet;
    _result[13] = +(xx*(zy*wz - wy*zz) - xy*(zx*wz - wx*zz) + xz*(zx*wy - wx*zy) ) * invDet;
    _result[14] = -(xx*(yy*wz - wy*yz) - xy*(yx*wz - wx*yz) + xz*(yx*wy - wx*yy) ) * invDet;
    _result[15] = +(xx*(yy*zz - zy*yz) - xy*(yx*zz - zx*yz) + xz*(yx*zy - zx*yy) ) * invDet;
}

void sx_vec3_calc_linearfit2D(float _result[2], const void* _points, int _stride, int _num)
{
    float sumX  = 0.0f;
    float sumY  = 0.0f;
    float sumXX = 0.0f;
    float sumXY = 0.0f;

    const uint8_t* ptr = (const uint8_t*)_points;
    for (int ii = 0; ii < _num; ++ii, ptr += _stride)
    {
        const float* point = (const float*)ptr;
        float xx = point[0];
        float yy = point[1];
        sumX  += xx;
        sumY  += yy;
        sumXX += xx*xx;
        sumXY += xx*yy;
    }

    // [ sum(x^2) sum(x)    ] [ A ] = [ sum(x*y) ]
    // [ sum(x)   numPoints ] [ B ]   [ sum(y)   ]

    float det = (sumXX*_num - sumX*sumX);
    float invDet = 1.0f/det;

    _result[0] = (-sumX * sumY + _num * sumXY) * invDet;
    _result[1] = (sumXX * sumY - sumX       * sumXY) * invDet;
}

void sx_vec3_calc_linearfit3D(float _result[3], const void* _points, int _stride, int _num)
{
    float sumX  = 0.0f;
    float sumY  = 0.0f;
    float sumZ  = 0.0f;
    float sumXX = 0.0f;
    float sumXY = 0.0f;
    float sumXZ = 0.0f;
    float sumYY = 0.0f;
    float sumYZ = 0.0f;

    const uint8_t* ptr = (const uint8_t*)_points;
    for (int ii = 0; ii < _num; ++ii, ptr += _stride)
    {
        const float* point = (const float*)ptr;
        float xx = point[0];
        float yy = point[1];
        float zz = point[2];

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

    float mtx[9] =
    {
        sumXX, sumXY, sumX,
        sumXY, sumYY, sumY,
        sumX,  sumY,  (float)(_num),
    };
    float invMtx[9];
    sx_mat3_inv(invMtx, mtx);

    _result[0] = invMtx[0]*sumXZ + invMtx[1]*sumYZ + invMtx[2]*sumZ;
    _result[1] = invMtx[3]*sumXZ + invMtx[4]*sumYZ + invMtx[5]*sumZ;
    _result[2] = invMtx[6]*sumXZ + invMtx[7]*sumYZ + invMtx[8]*sumZ;
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

void sx_mat3_mul(float* _result, const float* _a, const float* _b)
{
    sx_vec3_mul_mat3(&_result[0], &_a[0], _b);
    sx_vec3_mul_mat3(&_result[3], &_a[3], _b);
    sx_vec3_mul_mat3(&_result[6], &_a[6], _b);
}

// Reference: http://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToQuaternion/
void sx_mat4_calc_quat(float* _result, const float* _mat)
{
    float trace = _mat[0] + _mat[5] + _mat[10];
    if (trace > 0.00001f) {
        float s = 0.5f / sx_sqrt(trace + 1.0f);
        _result[3] = 0.25f / s;
        _result[0] = (_mat[9] - _mat[6]) * s;
        _result[1] = (_mat[2] - _mat[8]) * s;
        _result[2] = (_mat[4] - _mat[1]) * s;
    } else {
        if (_mat[0] > _mat[5] && _mat[0] > _mat[10]) {
            float s = 2.0f * sx_sqrt(1.0f + _mat[0] - _mat[5] - _mat[10]);
            _result[3] = (_mat[9] - _mat[6]) / s;
            _result[0] = 0.25f * s;
            _result[1] = (_mat[1] + _mat[4]) / s;
            _result[2] = (_mat[2] + _mat[8]) / s;
        } else if (_mat[5] > _mat[10]) {
            float s = 2.0f * sx_sqrt(1.0f + _mat[5] - _mat[0] - _mat[10]);
            _result[3] = (_mat[2] - _mat[8]) / s;
            _result[0] = (_mat[1] + _mat[4]) / s;
            _result[1] = 0.25f * s;
            _result[2] = (_mat[6] + _mat[9]) / s;
        } else {
            float s = 2.0f * sx_sqrt(1.0f + _mat[10] - _mat[0] - _mat[5]);
            _result[3] = (_mat[4] - _mat[1]) / s;
            _result[0] = (_mat[2] + _mat[8]) / s;
            _result[1] = (_mat[6] + _mat[9]) / s;
            _result[2] = 0.25f * s;
        }
    }
}

void sx_mat4x_inv(float* _result, const float* _mat)
{
    float det = (_mat[0] * (_mat[5]*_mat[10] - _mat[6]*_mat[9]) +
                 _mat[1] * (_mat[6]*_mat[8] - _mat[4]*_mat[10]) +
                 _mat[2] * (_mat[4]*_mat[9] - _mat[5]*_mat[8]));
    float invDet = 1.0f / det;
    float tx = _mat[12];
    float ty = _mat[13];
    float tz = _mat[14];
    
    _result[0] = (_mat[5]*_mat[10] - _mat[6]*_mat[9]) * invDet;
    _result[1] = (_mat[2]*_mat[9] - _mat[1]*_mat[10]) * invDet;
    _result[2] = (_mat[1]*_mat[6] - _mat[2]*_mat[5]) * invDet;
    _result[3] = 0;
    _result[4] = (_mat[6]*_mat[8] - _mat[4]*_mat[10]) * invDet;
    _result[5] = (_mat[0]*_mat[10] - _mat[2]*_mat[8]) * invDet;
    _result[6] = (_mat[2]*_mat[4] - _mat[0]*_mat[6]) * invDet;
    _result[7] = 0;
    _result[8] = (_mat[4]*_mat[9] - _mat[5]*_mat[8]) * invDet;
    _result[9] = (_mat[1]*_mat[8] - _mat[0]*_mat[9]) * invDet;
    _result[10] = (_mat[0]*_mat[5] - _mat[1]*_mat[4]) * invDet;
    _result[11] = 0;
    _result[12] = -(tx*_result[0] + ty*_result[4] + tz*_result[8]);
    _result[13] = -(tx*_result[1] + ty*_result[5] + tz*_result[9]);
    _result[14] = -(tx*_result[2] + ty*_result[6] + tz*_result[10]);
    _result[15] = 1.0f;
}

void sx_mat4_from_normal(float* _result, const float* _normal, float _scale, const float* _pos)
{
    float tangent[3];
    float bitangent[3];
    sx_vec3_tangent(_normal, tangent, bitangent);

    sx_vec3_mulf(&_result[ 0], bitangent, _scale);
    sx_vec3_mulf(&_result[ 4], _normal,   _scale);
    sx_vec3_mulf(&_result[ 8], tangent,   _scale);

    _result[ 3] = 0.0f;
    _result[ 7] = 0.0f;
    _result[11] = 0.0f;
    _result[12] = _pos[0];
    _result[13] = _pos[1];
    _result[14] = _pos[2];
    _result[15] = 1.0f;
}

void sx_mat4_from_normal_angle(float* _result, const float* _normal, float _scale, const float* _pos, float _angle)
{
    float tangent[3];
    float bitangent[3];
    sx_vec3_tangent_angle(_normal, _angle, tangent, bitangent);

    sx_vec3_mulf(&_result[ 0], bitangent, _scale);
    sx_vec3_mulf(&_result[ 4], _normal,   _scale);
    sx_vec3_mulf(&_result[ 8], tangent,   _scale);

    _result[ 3] = 0.0f;
    _result[ 7] = 0.0f;
    _result[11] = 0.0f;
    _result[12] = _pos[0];
    _result[13] = _pos[1];
    _result[14] = _pos[2];
    _result[15] = 1.0f;
}

void sx_mat4_quat(float* _result, const float* _quat)
{
    const float x = _quat[0];
    const float y = _quat[1];
    const float z = _quat[2];
    const float w = _quat[3];

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

    _result[ 0] = 1.0f - (y2y + z2z);
    _result[ 1] =         x2y - z2w;
    _result[ 2] =         x2z + y2w;
    _result[ 3] = 0.0f;

    _result[ 4] =         x2y + z2w;
    _result[ 5] = 1.0f - (x2x + z2z);
    _result[ 6] =         y2z - x2w;
    _result[ 7] = 0.0f;

    _result[ 8] =         x2z - y2w;
    _result[ 9] =         y2z + x2w;
    _result[10] = 1.0f - (x2x + y2y);
    _result[11] = 0.0f;

    _result[12] = 0.0f;
    _result[13] = 0.0f;
    _result[14] = 0.0f;
    _result[15] = 1.0f;
}

void sx_vec4_mul_mat4(float* _result, const float* _vec, const float* _mat)
{
    _result[0] = _vec[0] * _mat[ 0] + _vec[1] * _mat[4] + _vec[2] * _mat[ 8] + _vec[3] * _mat[12];
    _result[1] = _vec[0] * _mat[ 1] + _vec[1] * _mat[5] + _vec[2] * _mat[ 9] + _vec[3] * _mat[13];
    _result[2] = _vec[0] * _mat[ 2] + _vec[1] * _mat[6] + _vec[2] * _mat[10] + _vec[3] * _mat[14];
    _result[3] = _vec[0] * _mat[ 3] + _vec[1] * _mat[7] + _vec[2] * _mat[11] + _vec[3] * _mat[15];
}

void sx_mat4_mul(float* _result, const float* _a, const float* _b)
{
    sx_vec4_mul_mat4(&_result[ 0], &_a[ 0], _b);
    sx_vec4_mul_mat4(&_result[ 4], &_a[ 4], _b);
    sx_vec4_mul_mat4(&_result[ 8], &_a[ 8], _b);
    sx_vec4_mul_mat4(&_result[12], &_a[12], _b);
}

void sx_vec3_calc_normal(float _result[3], float _va[3], float _vb[3], float _vc[3])
{
    float ba[3];
    sx_vec3_sub(ba, _vb, _va);

    float ca[3];
    sx_vec3_sub(ca, _vc, _va);

    float baxca[3];
    sx_vec3_cross(baxca, ba, ca);

    sx_vec3_norm(_result, baxca);
}

void sx_vec3_calc_plane(float _result[4], float _va[3], float _vb[3], float _vc[3])
{
    float normal[3];
    sx_vec3_calc_normal(normal, _va, _vb, _vc);

    _result[0] = normal[0];
    _result[1] = normal[1];
    _result[2] = normal[2];
    _result[3] = -sx_vec3_dot(normal, _va);
}
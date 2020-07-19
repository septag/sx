//
// Copyright 2018 Sepehr Taghdisian (septag@github). All rights reserved.
// License: https://github.com/septag/sx#license-bsd-2-clause
//
// parts of this code is copied from bx library: https://github.com/bkaradzic/bx
// Copyright 2011-2019 Branimir Karadzic. All rights reserved.
// License: https://github.com/bkaradzic/bx#license-bsd-2-clause
//
#include "sx/math.h"

#if SX_CONFIG_STDMATH
#    include <math.h>
#endif

#ifndef __cplusplus
sx_vec2 SX_VEC2_ZERO = { .x = 0.0f, .y = 0.0f };
sx_vec2 SX_VEC2_UNITX = { .x = 1.0f, .y = 0.0f };
sx_vec2 SX_VEC2_UNITY = { .x = 0.0f, .y = 1.0f };

sx_vec3 SX_VEC3_ZERO = { .x = 0.0f, .y = 0.0f, .z = 0.0f };
sx_vec3 SX_VEC3_UNITX = { .x = 1.0f, .y = 0.0f, .z = 0.0f };
sx_vec3 SX_VEC3_UNITY = { .x = 0.0f, .y = 1.0f, .z = 0.0f };
sx_vec3 SX_VEC3_UNITZ = { .x = 0.0f, .y = 0.0f, .z = 1.0f };

sx_vec4 SX_VEC4_ZERO = { .x = 0.0f, .y = 0.0f, .z = 0.0f, .w = 1.0f };
sx_vec4 SX_VEC4_UNITX = { .x = 1.0f, .y = 0.0f, .z = 0.0f, .w = 1.0f };
sx_vec4 SX_VEC4_UNITY = { .x = 0.0f, .y = 1.0f, .z = 0.0f, .w = 1.0f };
sx_vec4 SX_VEC4_UNITZ = { .x = 0.0f, .y = 0.0f, .z = 1.0f, .w = 1.0f };

sx_color SX_COLOR_WHITE = { .r = 255, .g = 255, .b = 255, .a = 255 };
sx_color SX_COLOR_BLACK = { .r = 0, .g = 0, .b = 0, .a = 255 };
sx_color SX_COLOR_RED = { .r = 255, .g = 0, .b = 0, .a = 255 };
sx_color SX_COLOR_YELLOW = { .r = 255, .g = 255, .b = 0, .a = 255 };
sx_color SX_COLOR_GREEN = { .r = 0, .g = 255, .b = 0, .a = 255 };
sx_color SX_COLOR_BLUE = { .r = 0, .g = 0, .b = 255, .a = 255 };
sx_color SX_COLOR_PURPLE = { .r = 255, .g = 0, .b = 255, .a = 255 };
#else
sx_vec3 SX_VEC2_ZERO = { 0.0f, 0.0f };
sx_vec3 SX_VEC2_UNITX = { 1.0f, 0.0f };
sx_vec3 SX_VEC2_UNITY = { 0.0f, 1.0f };

sx_vec3 SX_VEC3_ZERO = { 0.0f, 0.0f, 0.0f };
sx_vec3 SX_VEC3_UNITX = { 1.0f, 0.0f, 0.0f };
sx_vec3 SX_VEC3_UNITY = { 0.0f, 1.0f, 0.0f };
sx_vec3 SX_VEC3_UNITZ = { 0.0f, 0.0f, 1.0f };

sx_vec4 SX_VEC4_ZERO = { 0.0f, 0.0f, 0.0f, 1.0f };
sx_vec4 SX_VEC4_UNITX = { 1.0f, 0.0f, 0.0f, 1.0f };
sx_vec4 SX_VEC4_UNITY = { 0.0f, 1.0f, 0.0f, 1.0f };
sx_vec4 SX_VEC4_UNITZ = { 0.0f, 0.0f, 1.0f, 1.0f };

sx_color SX_COLOR_WHITE = { 255, 255, 255, 255 };
sx_color SX_COLOR_BLACK = { 0, 0, 0, 255 };
sx_color SX_COLOR_RED = { 255, 0, 0, 255 };
sx_color SX_COLOR_YELLOW = { 255, 255, 0, 255 };
sx_color SX_COLOR_GREEN = { 0, 255, 0, 255 };
sx_color SX_COLOR_BLUE = { 0, 0, 255, 255 };
sx_color SX_COLOR_PURPLE = { 255, 0, 255, 255 };
#endif

#define sx__square(_x) (_x * _x)
#define sx__mad(_a, _b, _c) _a* _b + _c

#if !SX_CONFIG_STDMATH
SX_CONSTFN float sx_floor(float _a)
{
    if (_a < 0.0f) {
        const float fr = sx_fract(-_a);
        const float result = -_a - fr;

        return -(0.0f != fr ? result + 1.0f : result);
    }

    return _a - sx_fract(_a);
}

SX_CONSTFN float sx_cos(float _a)
{
    static const float kSinC2 = -0.16666667163372039794921875f;
    static const float kSinC4 = 8.333347737789154052734375e-3f;
    static const float kSinC6 = -1.9842604524455964565277099609375e-4f;
    static const float kSinC8 = 2.760012648650445044040679931640625e-6f;
    static const float kSinC10 = -2.50293279435709337121807038784027099609375e-8f;

    static const float kCosC2 = -0.5f;
    static const float kCosC4 = 4.166664183139801025390625e-2f;
    static const float kCosC6 = -1.388833043165504932403564453125e-3f;
    static const float kCosC8 = 2.47562347794882953166961669921875e-5f;
    static const float kCosC10 = -2.59630184018533327616751194000244140625e-7f;

    const float scaled = _a * 2.0f * SX_INVPI;
    const float real = sx_floor(scaled);
    const float xx = _a - real * SX_PIHALF;
    const int32_t bits = (int32_t)real & 3;

    float c0, c2, c4, c6, c8, c10;

    if (bits == 0 || bits == 2) {
        c0 = 1.0f;
        c2 = kCosC2;
        c4 = kCosC4;
        c6 = kCosC6;
        c8 = kCosC8;
        c10 = kCosC10;
    } else {
        c0 = xx;
        c2 = kSinC2;
        c4 = kSinC4;
        c6 = kSinC6;
        c8 = kSinC8;
        c10 = kSinC10;
    }

    const float xsq = sx__square(xx);
    const float tmp0 = sx__mad(c10, xsq, c8);
    const float tmp1 = sx__mad(tmp0, xsq, c6);
    const float tmp2 = sx__mad(tmp1, xsq, c4);
    const float tmp3 = sx__mad(tmp2, xsq, c2);
    const float tmp4 = sx__mad(tmp3, xsq, 1.0f);
    const float result = tmp4 * c0;

    return bits == 1 || bits == 2 ? -result : result;
}

SX_CONSTFN float sx_acos(float _a)
{
    static const float kAcosC0 = 1.5707288f;
    static const float kAcosC1 = -0.2121144f;
    static const float kAcosC2 = 0.0742610f;
    static const float kAcosC3 = -0.0187293f;

    const float absa = sx_abs(_a);
    const float tmp0 = sx__mad(kAcosC3, absa, kAcosC2);
    const float tmp1 = sx__mad(tmp0, absa, kAcosC1);
    const float tmp2 = sx__mad(tmp1, absa, kAcosC0);
    const float tmp3 = tmp2 * sx_sqrt(1.0f - absa);
    const float negate = (float)(_a < 0.0f);
    const float tmp4 = tmp3 - 2.0f * negate * tmp3;
    const float result = negate * SX_PI + tmp4;

    return result;
}

SX_CONSTFN float sx_sin(float _a)
{
    return sx_cos(_a - SX_PIHALF);
}

SX_CONSTFN float sx_asin(float _a)
{
    return SX_PIHALF - sx_acos(_a);
}

SX_CONSTFN float sx_atan2(float _y, float _x)
{
    static const float kAtan2C0 = -0.013480470f;
    static const float kAtan2C1 = 0.057477314f;
    static const float kAtan2C2 = -0.121239071f;
    static const float kAtan2C3 = 0.195635925f;
    static const float kAtan2C4 = -0.332994597f;
    static const float kAtan2C5 = 0.999995630f;

    const float ax = sx_abs(_x);
    const float ay = sx_abs(_y);
    const float maxaxy = sx_max(ax, ay);
    const float minaxy = sx_min(ax, ay);

    if (maxaxy == 0.0f) {
        return 0.0f * sx_sign(_y);
    }

    const float mxy = minaxy / maxaxy;
    const float mxysq = sx__square(mxy);
    const float tmp0 = sx__mad(kAtan2C0, mxysq, kAtan2C1);
    const float tmp1 = sx__mad(tmp0, mxysq, kAtan2C2);
    const float tmp2 = sx__mad(tmp1, mxysq, kAtan2C3);
    const float tmp3 = sx__mad(tmp2, mxysq, kAtan2C4);
    const float tmp4 = sx__mad(tmp3, mxysq, kAtan2C5);
    const float tmp5 = tmp4 * mxy;
    const float tmp6 = ay > ax ? SX_PIHALF - tmp5 : tmp5;
    const float tmp7 = _x < 0.0f ? SX_PI - tmp6 : tmp6;
    const float result = sx_sign(_y) * tmp7;

    return result;
}


static float sx__ldexp(float _a, int32_t _b)
{
    const uint32_t ftob = sx_ftob(_a);
    const uint32_t masked = ftob & UINT32_C(0xff800000);
    const uint32_t expsign0 = ((int32_t)masked) >> 23;
    const uint32_t tmp = (int)expsign0 + (int)_b;
    const uint32_t expsign1 = tmp << 23;
    const uint32_t mantissa = ftob & UINT32_C(0x007fffff);
    const uint32_t bits = mantissa | expsign1;
    return sx_btof(bits);
}

static float sx__frexp(float _a, int32_t* _outExp)
{
    const uint32_t ftob = sx_ftob(_a);
    const uint32_t masked0 = ftob & UINT32_C(0x7f800000);
    const uint32_t exp0 = masked0 >> 23;
    const uint32_t masked1 = ftob & UINT32_C(0x807fffff);
    const uint32_t bits = masked1 | UINT32_C(0x3f000000);
    const float result = sx_btof(bits);

    *_outExp = (int32_t)(exp0 - 0x7e);

    return result;
}

SX_CONSTFN float sx_exp(float _a)
{
    static const float kExpC0 = 1.66666666666666019037e-01f;
    static const float kExpC1 = -2.77777777770155933842e-03f;
    static const float kExpC2 = 6.61375632143793436117e-05f;
    static const float kExpC3 = -1.65339022054652515390e-06f;
    static const float kExpC4 = 4.13813679705723846039e-08f;
    static const float kNearZero = SX_NEAR_ZERO;

    if (sx_abs(_a) <= kNearZero) {
        return _a + 1.0f;
    }

    const float kk = sx_round(_a * SX_INVLOG_NAT2);
    const float hi = _a - kk * SX_LOG_NAT2H;
    const float lo = kk * SX_LOG_NAT2L;
    const float hml = hi - lo;
    const float hmlsq = sx__square(hml);
    const float tmp0 = sx__mad(kExpC4, hmlsq, kExpC3);
    const float tmp1 = sx__mad(tmp0, hmlsq, kExpC2);
    const float tmp2 = sx__mad(tmp1, hmlsq, kExpC1);
    const float tmp3 = sx__mad(tmp2, hmlsq, kExpC0);
    const float tmp4 = hml - hmlsq * tmp3;
    const float tmp5 = hml * tmp4 / (2.0f - tmp4);
    const float tmp6 = 1.0f - ((lo - tmp5) - hi);
    const float result = sx__ldexp(tmp6, (int32_t)(kk));

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
    if (ff < SX_SQRT2 * 0.5f) {
        ff *= 2.0f;
        --exp;
    }

    ff -= 1.0f;
    const float kk = (float)(exp);
    const float hi = kk * SX_LOG_NAT2H;
    const float lo = kk * SX_LOG_NAT2L;
    const float ss = ff / (2.0f + ff);
    const float s2 = sx__square(ss);
    const float s4 = sx__square(s2);

    const float tmp0 = sx__mad(kLogC6, s4, kLogC4);
    const float tmp1 = sx__mad(tmp0, s4, kLogC2);
    const float tmp2 = sx__mad(tmp1, s4, kLogC0);
    const float t1 = s2 * tmp2;

    const float tmp3 = sx__mad(kLogC5, s4, kLogC3);
    const float tmp4 = sx__mad(tmp3, s4, kLogC1);
    const float t2 = s4 * tmp4;

    const float t12 = t1 + t2;
    const float hfsq = 0.5f * sx__square(ff);
    const float result = hi - ((hfsq - (ss * (hfsq + t12) + lo)) - ff);

    return result;
}

// Reference: http://en.wikipedia.org/wiki/Fast_inverse_square_root
SX_CONSTFN float sx_rsqrt(float _a)
{
    union {
        float f;
        uint32_t ui;
    } u = { _a };
    float y, r;

    y = _a * 0.5f;
    u.ui = 0x5F3759DF - (u.ui >> 1);
    r = u.f;
    r = r * (1.5f - (r * r * y));

    return r;
}

SX_CONSTFN float sx_sqrt(float _a)
{
    sx_assert(_a >= SX_NEAR_ZERO);
    return 1.0f / sx_rsqrt(_a);
}
#else
SX_CONSTFN float sx_floor(float _f)
{
    return floorf(_f);
}

SX_CONSTFN float sx_cos(float _a)
{
    return cosf(_a);
}

SX_CONSTFN float sx_acos(float _a)
{
    return acosf(_a);
}

SX_CONSTFN float sx_sin(float _a)
{
    return sinf(_a);
}

SX_CONSTFN float sx_asin(float _a)
{
    return asinf(_a);
}

SX_CONSTFN float sx_atan2(float _y, float _x)
{
    return atan2f(_y, _x);
}

SX_CONSTFN float sx_exp(float _a)
{
    return expf(_a);
}

SX_CONSTFN float sx_log(float _a)
{
    return logf(_a);
}

SX_CONSTFN float sx_sqrt(float _a)
{
    return sqrtf(_a);
}

SX_CONSTFN float sx_rsqrt(float _a)
{
    return 1.0f / sqrtf(_a);
}
#endif

sx_mat4 sx_mat4_view_lookat(const sx_vec3 eye, const sx_vec3 target, const sx_vec3 up)
{
    sx_vec3 zaxis = sx_vec3_norm(sx_vec3_sub(target, eye));
    sx_vec3 xaxis = sx_vec3_norm(sx_vec3_cross(zaxis, up));
    sx_vec3 yaxis = sx_vec3_cross(xaxis, zaxis);

    // clang-format off
    return sx_mat4f(xaxis.x,    xaxis.y,    xaxis.z,    -sx_vec3_dot(xaxis, eye), 
                    yaxis.x,    yaxis.y,    yaxis.z,    -sx_vec3_dot(yaxis, eye), 
                    -zaxis.x,   -zaxis.y,   -zaxis.z,    sx_vec3_dot(zaxis, eye),
                    0,          0,          0,           1.0f);
    // clang-format on
}

sx_mat4 sx_mat4_view_lookatLH(const sx_vec3 eye, const sx_vec3 target, const sx_vec3 up)
{
    sx_vec3 zaxis = sx_vec3_norm(sx_vec3_sub(target, eye));
    sx_vec3 xaxis = sx_vec3_norm(sx_vec3_cross(up, zaxis));
    sx_vec3 yaxis = sx_vec3_cross(zaxis, xaxis);

    // clang-format off
    return sx_mat4f(xaxis.x, xaxis.y, xaxis.z, -sx_vec3_dot(xaxis, eye), 
                    yaxis.x, yaxis.y, yaxis.z, -sx_vec3_dot(yaxis, eye), 
                    zaxis.x, zaxis.y, zaxis.z, -sx_vec3_dot(zaxis, eye),
                    0,       0,       0,        1.0f);
    // clang-format on
}

sx_mat4 sx_mat4_view_FPS(const sx_vec3 eye, float pitch, float yaw)
{
    float cos_pitch = sx_cos(pitch);
    float sin_pitch = sx_sin(pitch);
    float cos_yaw = sx_cos(yaw);
    float sin_yaw = sx_sin(yaw);

    sx_vec3 xaxis = sx_vec3f(cos_yaw, 0, -sin_yaw);
    sx_vec3 yaxis = sx_vec3f(sin_yaw * sin_pitch, cos_pitch, cos_yaw * sin_pitch);
    sx_vec3 zaxis = sx_vec3f(sin_yaw * cos_pitch, -sin_pitch, cos_pitch * cos_yaw);

    return sx_mat4f(xaxis.x, xaxis.y, xaxis.z, -sx_vec3_dot(xaxis, eye), yaxis.x, yaxis.y, yaxis.z,
                    -sx_vec3_dot(yaxis, eye), zaxis.x, zaxis.y, zaxis.z, -sx_vec3_dot(zaxis, eye),
                    0, 0, 0, 1.0f);
}

sx_mat4 sx_mat4_view_arcball(const sx_vec3 move, const sx_quat rot, const sx_vec3 target_pos)
{
    // CameraMat = Tobj * Rcam * Tcam;      // move -> rotate around pivot pt -> move to object pos
    // ViewMat = CameraMat(inv) = Tobj(inv) * Rcam(inv) * Tobj(inv)
    sx_mat4 translate_inv = sx_mat4_translate(-move.x, -move.y, -move.z);
    sx_mat4 rotate_inv = sx_quat_mat4(sx_quat_inv(rot));
    sx_mat4 translate_obj_inv = sx_mat4_translate(-target_pos.x, -target_pos.y, -target_pos.z);
    sx_mat4 TR = sx_mat4_mul(&translate_obj_inv, &rotate_inv);
    return sx_mat4_mul(&TR, &translate_inv);
}

// OGL NDC:(x = -1..1, y = -1..1, z = -1..1)
// D3D NDC:(x = -1..1, y = -1..1, z =  0..1)
// Metal NDC:(x = -1..1, y = -1..1, z = 0..1)
sx_mat4 sx_mat4_perspective(float width, float height, float zn, float zf, bool ogl_ndc)
{
    const float d = zf - zn;
    const float aa = ogl_ndc ? (zf + zn) / d : zf / d;
    const float bb = ogl_ndc ? (2.0f * zn * zf) / d : zn * aa;
    return sx_mat4f(width, 0, 0, 0, 0, height, 0, 0, 0, 0, -aa, -bb, 0, 0, -1.0f, 0);
}

sx_mat4 sx_mat4_perspectiveLH(float width, float height, float zn, float zf, bool ogl_ndc)
{
    const float d = zf - zn;
    const float aa = ogl_ndc ? (zf + zn) / d : zf / d;
    const float bb = ogl_ndc ? (2.0f * zn * zf) / d : zn * aa;
    return sx_mat4f(width, 0, 0, 0, 0, height, 0, 0, 0, 0, aa, -bb, 0, 0, 1.0f, 0);
}

sx_mat4 sx_mat4_perspective_offcenter(float xmin, float ymin, float xmax, float ymax, float zn,
                                      float zf, bool ogl_ndc)
{
    const float d = zf - zn;
    const float aa = ogl_ndc ? (zf + zn) / d : zf / d;
    const float bb = ogl_ndc ? (2.0f * zn * zf) / d : zn * aa;
    const float width = xmax - xmin;
    const float height = ymax - ymin;
    return sx_mat4f(width, 0, xmin, 0, 0, height, ymin, 0, 0, 0, -aa, -bb, 0, 0, -1.0f, 0);
}

sx_mat4 sx_mat4_perspective_offcenterLH(float xmin, float ymin, float xmax, float ymax, float zn,
                                        float zf, bool ogl_ndc)
{
    const float d = zf - zn;
    const float aa = ogl_ndc ? (zf + zn) / d : zf / d;
    const float bb = ogl_ndc ? (2.0f * zn * zf) / d : zn * aa;
    const float width = xmax - xmin;
    const float height = ymax - ymin;
    return sx_mat4f(width, 0, -xmin, 0, 0, height, -ymin, 0, 0, 0, aa, -bb, 0, 0, 1.0f, 0);
}

sx_mat4 sx_mat4_perspectiveFOV(float fov_y, float aspect, float zn, float zf, bool ogl_ndc)
{
    const float height = 1.0f / sx_tan(fov_y * 0.5f);
    const float width = height / aspect;
    return sx_mat4_perspective(width, height, zn, zf, ogl_ndc);
}

sx_mat4 sx_mat4_perspectiveFOV_LH(float fov_y, float aspect, float zn, float zf, bool ogl_ndc)
{
    const float height = 1.0f / sx_tan(fov_y * 0.5f);
    const float width = height / aspect;
    return sx_mat4_perspectiveLH(width, height, zn, zf, ogl_ndc);
}

sx_mat4 sx_mat4_ortho(float width, float height, float zn, float zf, float offset, bool ogl_ndc)
{
    const float d = zf - zn;
    const float cc = (ogl_ndc ? 2.0f : 1.0f) / d;
    const float ff = ogl_ndc ? -(zn + zf) / d : -zn / d;

    return sx_mat4f(2.0f / width, 0, 0, offset, 0, 2.0f / height, 0, 0, 0, 0, -cc, ff, 0, 0, 0,
                    1.0f);
}

sx_mat4 sx_mat4_orthoLH(float width, float height, float zn, float zf, float offset, bool ogl_ndc)
{
    const float d = zf - zn;
    const float cc = (ogl_ndc ? 2.0f : 1.0f) / d;
    const float ff = ogl_ndc ? -(zn + zf) / d : -zn / d;

    return sx_mat4f(2.0f / width, 0, 0, offset, 0, 2.0f / height, 0, 0, 0, 0, cc, ff, 0, 0, 0,
                    1.0f);
}

sx_mat4 sx_mat4_ortho_offcenter(float xmin, float ymin, float xmax, float ymax, float zn, float zf,
                                float offset, bool ogl_ndc)
{
    const float width = xmax - xmin;
    const float height = ymax - ymin;
    const float d = zf - zn;
    const float cc = (ogl_ndc ? 2.0f : 1.0f) / d;
    const float dd = (xmin + xmax) / (xmin - xmax);
    const float ee = (ymin + ymax) / (ymin - ymax);
    const float ff = ogl_ndc ? -(zn + zf) / d : -zn / d;

    return sx_mat4f(2.0f / width, 0, 0, dd + offset, 0, 2.0f / height, 0, ee, 0, 0, -cc, ff, 0, 0,
                    0, 1.0f);
}

sx_mat4 sx_mat4_ortho_offcenterLH(float xmin, float ymin, float xmax, float ymax, float zn,
                                  float zf, float offset, bool ogl_ndc)
{
    const float width = xmax - xmin;
    const float height = ymax - ymin;
    const float d = zf - zn;
    const float cc = (ogl_ndc ? 2.0f : 1.0f) / d;
    const float dd = (xmin + xmax) / (xmin - xmax);
    const float ee = (ymin + ymax) / (ymin - ymax);
    const float ff = ogl_ndc ? -(zn + zf) / d : -zn / d;

    return sx_mat4f(2.0f / width, 0, 0, dd + offset, 0, 2.0f / height, 0, ee, 0, 0, cc, ff, 0, 0, 0,
                    1.0f);
}

sx_mat4 sx_mat4_SRT(float _sx, float _sy, float _sz, float _ax, float _ay, float _az, float _tx,
                    float _ty, float _tz)
{
    float sx, cx, sy, cy, sz, cz;

    if (_ax != 0) {
        sx = sx_sin(_ax);
        cx = sx_cos(_ax);
    } else {
        sx = 0;
        cx = 1.0f;
    }

    if (_ay != 0) {
        sy = sx_sin(_ay);
        cy = sx_cos(_ay);
    } else {
        sy = 0;
        cy = 1.0f;
    }

    if (_az != 0) {
        sz = sx_sin(_az);
        cz = sx_cos(_az);
    } else {
        sz = 0;
        cz = 1.0f;
    }

    const float sxsz = sx * sz;
    const float cycz = cy * cz;

    return sx_mat4f(_sx * (cycz - sxsz * sy), _sx * -cx * sz, _sx * (cz * sy + cy * sxsz), _tx,
                    _sy * (cz * sx * sy + cy * sz), _sy * cx * cz, _sy * (sy * sz - cycz * sx), _ty,
                    _sz * -cx * sy, _sz * sx, _sz * cx * cy, _tz, 0.0f, 0.0f, 0.0f, 1.0f);
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
    det += xx * (yy * zz - yz * zy);
    det -= xy * (yx * zz - yz * zx);
    det += xz * (yx * zy - yy * zx);

    float det_rcp = 1.0f / det;

    return sx_mat3f(+(yy * zz - yz * zy) * det_rcp, -(xy * zz - xz * zy) * det_rcp,
                    +(xy * yz - xz * yy) * det_rcp, -(yx * zz - yz * zx) * det_rcp,
                    +(xx * zz - xz * zx) * det_rcp, -(xx * yz - xz * yx) * det_rcp,
                    +(yx * zy - yy * zx) * det_rcp, -(xx * zy - xy * zx) * det_rcp,
                    +(xx * yy - xy * yx) * det_rcp);
}

sx_mat4 sx_mat4_inv(const sx_mat4* _a)
{
    float xx = _a->f[0];
    float xy = _a->f[1];
    float xz = _a->f[2];
    float xw = _a->f[3];
    float yx = _a->f[4];
    float yy = _a->f[5];
    float yz = _a->f[6];
    float yw = _a->f[7];
    float zx = _a->f[8];
    float zy = _a->f[9];
    float zz = _a->f[10];
    float zw = _a->f[11];
    float wx = _a->f[12];
    float wy = _a->f[13];
    float wz = _a->f[14];
    float ww = _a->f[15];

    float det = 0.0f;
    det += xx * (yy * (zz * ww - zw * wz) - yz * (zy * ww - zw * wy) + yw * (zy * wz - zz * wy));
    det -= xy * (yx * (zz * ww - zw * wz) - yz * (zx * ww - zw * wx) + yw * (zx * wz - zz * wx));
    det += xz * (yx * (zy * ww - zw * wy) - yy * (zx * ww - zw * wx) + yw * (zx * wy - zy * wx));
    det -= xw * (yx * (zy * wz - zz * wy) - yy * (zx * wz - zz * wx) + yz * (zx * wy - zy * wx));

    float det_rcp = 1.0f / det;

    return sx_mat4v(
        sx_vec4f(+(yy * (zz * ww - wz * zw) - yz * (zy * ww - wy * zw) + yw * (zy * wz - wy * zz)) *
                     det_rcp,
                 -(xy * (zz * ww - wz * zw) - xz * (zy * ww - wy * zw) + xw * (zy * wz - wy * zz)) *
                     det_rcp,
                 +(xy * (yz * ww - wz * yw) - xz * (yy * ww - wy * yw) + xw * (yy * wz - wy * yz)) *
                     det_rcp,
                 -(xy * (yz * zw - zz * yw) - xz * (yy * zw - zy * yw) + xw * (yy * zz - zy * yz)) *
                     det_rcp),

        sx_vec4f(-(yx * (zz * ww - wz * zw) - yz * (zx * ww - wx * zw) + yw * (zx * wz - wx * zz)) *
                     det_rcp,
                 +(xx * (zz * ww - wz * zw) - xz * (zx * ww - wx * zw) + xw * (zx * wz - wx * zz)) *
                     det_rcp,
                 -(xx * (yz * ww - wz * yw) - xz * (yx * ww - wx * yw) + xw * (yx * wz - wx * yz)) *
                     det_rcp,
                 +(xx * (yz * zw - zz * yw) - xz * (yx * zw - zx * yw) + xw * (yx * zz - zx * yz)) *
                     det_rcp),

        sx_vec4f(+(yx * (zy * ww - wy * zw) - yy * (zx * ww - wx * zw) + yw * (zx * wy - wx * zy)) *
                     det_rcp,
                 -(xx * (zy * ww - wy * zw) - xy * (zx * ww - wx * zw) + xw * (zx * wy - wx * zy)) *
                     det_rcp,
                 +(xx * (yy * ww - wy * yw) - xy * (yx * ww - wx * yw) + xw * (yx * wy - wx * yy)) *
                     det_rcp,
                 -(xx * (yy * zw - zy * yw) - xy * (yx * zw - zx * yw) + xw * (yx * zy - zx * yy)) *
                     det_rcp),

        sx_vec4f(-(yx * (zy * wz - wy * zz) - yy * (zx * wz - wx * zz) + yz * (zx * wy - wx * zy)) *
                     det_rcp,
                 +(xx * (zy * wz - wy * zz) - xy * (zx * wz - wx * zz) + xz * (zx * wy - wx * zy)) *
                     det_rcp,
                 -(xx * (yy * wz - wy * yz) - xy * (yx * wz - wx * yz) + xz * (yx * wy - wx * yy)) *
                     det_rcp,
                 +(xx * (yy * zz - zy * yz) - xy * (yx * zz - zx * yz) + xz * (yx * zy - zx * yy)) *
                     det_rcp));
}

sx_vec2 sx_vec2_calc_linearfit2D(const sx_vec2* _points, int _num)
{
    float sumX = 0.0f;
    float sumY = 0.0f;
    float sumXX = 0.0f;
    float sumXY = 0.0f;

    for (int ii = 0; ii < _num; ++ii) {
        float xx = _points[ii].f[0];
        float yy = _points[ii].f[1];
        sumX += xx;
        sumY += yy;
        sumXX += xx * xx;
        sumXY += xx * yy;
    }

    // [ sum(x^2) sum(x)    ] [ A ] = [ sum(x*y) ]
    // [ sum(x)   numPoints ] [ B ]   [ sum(y)   ]

    float det = (sumXX * _num - sumX * sumX);
    float invDet = 1.0f / det;

    return sx_vec2f((-sumX * sumY + _num * sumXY) * invDet, (sumXX * sumY - sumX * sumXY) * invDet);
}

sx_vec3 sx_vec3_calc_linearfit3D(const sx_vec3* _points, int _num)
{
    float sumX = 0.0f;
    float sumY = 0.0f;
    float sumZ = 0.0f;
    float sumXX = 0.0f;
    float sumXY = 0.0f;
    float sumXZ = 0.0f;
    float sumYY = 0.0f;
    float sumYZ = 0.0f;

    for (int ii = 0; ii < _num; ++ii) {
        float xx = _points[ii].f[0];
        float yy = _points[ii].f[1];
        float zz = _points[ii].f[2];

        sumX += xx;
        sumY += yy;
        sumZ += zz;
        sumXX += xx * xx;
        sumXY += xx * yy;
        sumXZ += xx * zz;
        sumYY += yy * yy;
        sumYZ += yy * zz;
    }

    // [ sum(x^2) sum(x*y) sum(x)    ] [ A ]   [ sum(x*z) ]
    // [ sum(x*y) sum(y^2) sum(y)    ] [ B ] = [ sum(y*z) ]
    // [ sum(x)   sum(y)   numPoints ] [ C ]   [ sum(z)   ]

    sx_mat3 mat = sx_mat3f(sumXX, sumXY, sumX, sumXY, sumYY, sumY, sumX, sumY, (float)(_num));
    sx_mat3 mat_inv = sx_mat3_inv(&mat);

    return sx_vec3f(mat_inv.f[0] * sumXZ + mat_inv.f[1] * sumYZ + mat_inv.f[2] * sumZ,
                    mat_inv.f[3] * sumXZ + mat_inv.f[4] * sumYZ + mat_inv.f[5] * sumZ,
                    mat_inv.f[6] * sumXZ + mat_inv.f[7] * sumYZ + mat_inv.f[8] * sumZ);
}

void sx_color_RGBtoHSV(float _hsv[3], const float _rgb[3])
{
    float K = 0.f;
    float r = _rgb[0];
    float g = _rgb[1];
    float b = _rgb[2];

    if (g < b)
    {
        sx_swap(g, b, float);
        K = -1.f;
    }

    if (r < g)
    {
        sx_swap(r, g, float);
        K = -2.f / 6.f - K;
    }

    float chroma = r - sx_min(g, b);
    _hsv[0] = sx_abs(K + (g - b) / (6.f * chroma + 1e-20f));
    _hsv[1] = chroma / (r + 1e-20f);
    _hsv[2] = r;
}

void sx_color_HSVtoRGB(float _rgb[3], const float _hsv[3])
{
    const float hh = _hsv[0];
    const float ss = _hsv[1];
    const float vv = _hsv[2];

    const float px = sx_abs(sx_fract(hh + 1.0f) * 6.0f - 3.0f);
    const float py = sx_abs(sx_fract(hh + 2.0f / 3.0f) * 6.0f - 3.0f);
    const float pz = sx_abs(sx_fract(hh + 1.0f / 3.0f) * 6.0f - 3.0f);

    _rgb[0] = vv * sx_lerp(1.0f, sx_saturate(px - 1.0f), ss);
    _rgb[1] = vv * sx_lerp(1.0f, sx_saturate(py - 1.0f), ss);
    _rgb[2] = vv * sx_lerp(1.0f, sx_saturate(pz - 1.0f), ss);
}

sx_mat3 sx_mat3_mul(const sx_mat3* _a, const sx_mat3* _b)
{
    return sx_mat3fv(sx_mat3_mul_vec3(_a, _b->col1).f, sx_mat3_mul_vec3(_a, _b->col2).f,
                     sx_mat3_mul_vec3(_a, _b->col3).f);
}

sx_quat sx_mat4_quat(const sx_mat4* m)
{
    float trace, r, rinv;
    sx_quat q;

    trace = m->m11 + m->m22 + m->m33;
    if (trace >= 0.0f) {
        r = sx_sqrt(1.0f + trace);
        rinv = 0.5f / r;

        q.x = rinv * (m->m32 - m->m23);
        q.y = rinv * (m->m13 - m->m31);
        q.z = rinv * (m->m21 - m->m12);
        q.w = r * 0.5f;
    } else if (m->m11 >= m->m22 && m->m11 >= m->m33) {
        r = sx_sqrt(1.0f - m->m22 - m->m33 + m->m11);
        rinv = 0.5f / r;

        q.x = r * 0.5f;
        q.y = rinv * (m->m21 + m->m12);
        q.z = rinv * (m->m31 + m->m13);
        q.w = rinv * (m->m32 - m->m23);
    } else if (m->m22 >= m->m33) {
        r = sx_sqrt(1.0f - m->m11 - m->m33 + m->m22);
        rinv = 0.5f / r;

        q.x = rinv * (m->m21 + m->m12);
        q.y = r * 0.5f;
        q.z = rinv * (m->m32 + m->m23);
        q.w = rinv * (m->m13 - m->m31);
    } else {
        r = sx_sqrt(1.0f - m->m11 - m->m22 + m->m33);
        rinv = 0.5f / r;

        q.x = rinv * (m->m31 + m->m13);
        q.y = rinv * (m->m32 + m->m23);
        q.z = r * 0.5f;
        q.w = rinv * (m->m21 - m->m12);
    }

    return q;
}

sx_mat4 sx_mat4x_inv(const sx_mat4* _mat)
{
    float det = (_mat->m11 * (_mat->m22 * _mat->m33 - _mat->m23 * _mat->m32) +
                 _mat->m12 * (_mat->m23 * _mat->m31 - _mat->m21 * _mat->m33) +
                 _mat->m13 * (_mat->m21 * _mat->m32 - _mat->m22 * _mat->m31));
    float det_rcp = 1.0f / det;
    float tx = _mat->m14;
    float ty = _mat->m24;
    float tz = _mat->m34;

    sx_mat4 r = sx_mat4f((_mat->m22 * _mat->m33 - _mat->m23 * _mat->m32) * det_rcp,
                         (_mat->m13 * _mat->m32 - _mat->m12 * _mat->m33) * det_rcp,
                         (_mat->m12 * _mat->m23 - _mat->m13 * _mat->m22) * det_rcp, 0.0f,
                         (_mat->m23 * _mat->m31 - _mat->m21 * _mat->m33) * det_rcp,
                         (_mat->m11 * _mat->m33 - _mat->m13 * _mat->m31) * det_rcp,
                         (_mat->m13 * _mat->m21 - _mat->m11 * _mat->m23) * det_rcp, 0,
                         (_mat->m21 * _mat->m32 - _mat->m22 * _mat->m31) * det_rcp,
                         (_mat->m12 * _mat->m31 - _mat->m11 * _mat->m32) * det_rcp,
                         (_mat->m11 * _mat->m22 - _mat->m12 * _mat->m21) * det_rcp, 0, 0.0f,
                         0.0f, 0.0f, 1.0f);

    r.f[12] = -(tx * r.m11 + ty * r.m12 + tz * r.m13);
    r.f[13] = -(tx * r.m21 + ty * r.m22 + tz * r.m23);
    r.f[14] = -(tx * r.m31 + ty * r.m32 + tz * r.m33);
    return r;
}

sx_mat4 sx_mat4_from_normal(const sx_vec3 _normal, float _scale, const sx_vec3 _pos)
{
    sx_vec3 tangent;
    sx_vec3 bitangent;
    sx_vec3_tangent(&tangent, &bitangent, _normal);

    sx_vec4 row1 = sx_vec4v3(sx_vec3_mulf(bitangent, _scale), 0.0f);
    sx_vec4 row2 = sx_vec4v3(sx_vec3_mulf(_normal, _scale), 0.0f);
    sx_vec4 row3 = sx_vec4v3(sx_vec3_mulf(tangent, _scale), 0.0f);

    return sx_mat4fv(row1.f, row2.f, row3.f, sx_vec4v3(_pos, 1.0f).f);
}

sx_mat4 sx_mat4_from_normal_angle(const sx_vec3 _normal, float _scale, const sx_vec3 _pos,
                                  float _angle)
{
    sx_vec3 tangent;
    sx_vec3 bitangent;
    sx_vec3_tangent_angle(&tangent, &bitangent, _normal, _angle);

    sx_vec4 row1 = sx_vec4v3(sx_vec3_mulf(bitangent, _scale), 0.0f);
    sx_vec4 row2 = sx_vec4v3(sx_vec3_mulf(_normal, _scale), 0.0f);
    sx_vec4 row3 = sx_vec4v3(sx_vec3_mulf(tangent, _scale), 0.0f);

    return sx_mat4fv(row1.f, row2.f, row3.f, sx_vec4v3(_pos, 1.0f).f);
}

sx_mat4 sx_mat4_project_plane(const sx_vec3 plane_normal)
{
    float xx = plane_normal.x * plane_normal.x;
    float yy = plane_normal.y * plane_normal.y;
    float zz = plane_normal.z * plane_normal.z;
    float xy = plane_normal.x * plane_normal.y;
    float xz = plane_normal.x * plane_normal.z;
    float yz = plane_normal.y * plane_normal.z;

    // clang-format off
    return sx_mat4f(1.0f - xx,      -xy,        -xz,        0.0f,
                    -xy,            1.0f - yy,  -yz,        0.0f,
                    -xz,            -yz,        1.0f - zz,  0.0f,
                    0.0f,           0.0f,       0.0f,       1.0f);
    // clang-format on
}

sx_mat3 sx_quad_mat3(const sx_quat quat)
{
    float norm = sx_sqrt(sx_quat_dot(quat, quat));
    float s = norm > 0.0f ? (2.0f / norm) : 0.0f;

    float x = quat.x;
    float y = quat.y;
    float z = quat.z;
    float w = quat.w;

    float xx = s * x * x;
    float xy = s * x * y;
    float wx = s * w * x;
    float yy = s * y * y;
    float yz = s * y * z;
    float wy = s * w * y;
    float zz = s * z * z;
    float xz = s * x * z;
    float wz = s * w * z;

    // clang-format off
    return sx_mat3f(1.0f - yy - zz,     xy - wz,            xz + wy,
                    xy + wz,            1.0f - xx - zz,     yz - wx,
                    xz - wy,            yz + wx,            1.0f - xx - yy);
    // clang-format on
}

sx_mat4 sx_quat_mat4(const sx_quat quat)
{
    float norm = sx_sqrt(sx_quat_dot(quat, quat));
    float s = norm > 0.0f ? (2.0f / norm) : 0.0f;

    float x = quat.x;
    float y = quat.y;
    float z = quat.z;
    float w = quat.w;

    float xx = s * x * x;
    float xy = s * x * y;
    float wx = s * w * x;
    float yy = s * y * y;
    float yz = s * y * z;
    float wy = s * w * y;
    float zz = s * z * z;
    float xz = s * x * z;
    float wz = s * w * z;

    // clang-format off
    return sx_mat4f(1.0f - yy - zz,     xy - wz,            xz + wy,        0.0f,
                    xy + wz,            1.0f - xx - zz,     yz - wx,        0.0f,
                    xz - wy,            yz + wx,            1.0f - xx - yy, 0.0f,
                    0.0f,               0.0f,               0.0f,           1.0f);
    // clang-format on
}


sx_mat4 sx_mat4_mul(const sx_mat4* _a, const sx_mat4* _b)
{
    return sx_mat4fv(sx_mat4_mul_vec4(_a, _b->col1).f, sx_mat4_mul_vec4(_a, _b->col2).f,
                     sx_mat4_mul_vec4(_a, _b->col3).f, sx_mat4_mul_vec4(_a, _b->col4).f);
}

sx_vec3 sx_plane_normal(const sx_vec3 _va, const sx_vec3 _vb, const sx_vec3 _vc)
{
    sx_vec3 ba = sx_vec3_sub(_vb, _va);
    sx_vec3 ca = sx_vec3_sub(_vc, _va);
    sx_vec3 baca = sx_vec3_cross(ba, ca);

    return sx_vec3_norm(baca);
}

sx_plane sx_plane3p(const sx_vec3 _va, const sx_vec3 _vb, const sx_vec3 _vc)
{
    sx_vec3 normal = sx_plane_normal(_va, _vb, _vc);
    return sx_planev(normal, -sx_vec3_dot(normal, _va));
}

sx_plane sx_planenp(const sx_vec3 _normal, const sx_vec3 _p)
{
    sx_vec3 normal = sx_vec3_norm(_normal);
    float d = sx_vec3_dot(_normal, _p);
    return sx_planev(normal, -d);
}

float sx_plane_distance(const sx_plane _plane, const sx_vec3 _p)
{
    return sx_vec3_dot(_plane.normal, _p) + _plane.dist;
}

sx_vec3 sx_plane_project_point(const sx_plane _plane, const sx_vec3 _p)
{
    return sx_vec3_sub(_p, sx_vec3_mulf(_plane.normal, sx_plane_distance(_plane, _p)));
}

sx_vec3 sx_plane_origin(const sx_plane _plane)
{
    return sx_vec3_mulf(_plane.normal, -_plane.dist);
}

sx_aabb sx_aabb_transform(const sx_aabb* aabb, const sx_mat4* mat)
{
    sx_vec3 minpt;
    sx_vec3 maxpt;
    sx_vec3 t = sx_vec3fv(mat->col4.f);

    minpt.x = maxpt.x = t.x;
    minpt.y = maxpt.y = t.y;
    minpt.z = maxpt.z = t.z;

    if (mat->m11 > 0.0f) {
        minpt.x += mat->m11 * aabb->xmin;
        maxpt.x += mat->m11 * aabb->xmax;
    }    else    {
        minpt.x += mat->m11 * aabb->xmax;
        maxpt.x += mat->m11 * aabb->xmin;
    }

    if (mat->m21 > 0.0f) {
        minpt.y += mat->m21 * aabb->xmin;
        maxpt.y += mat->m21 * aabb->xmax;
    }    else     {
        minpt.y += mat->m21 * aabb->xmax;
        maxpt.y += mat->m21 * aabb->xmin;
    }

    if (mat->m31 > 0.0f)    {
        minpt.z += mat->m31 * aabb->xmin;
        maxpt.z += mat->m31 * aabb->xmax;
    }    else    {
        minpt.z += mat->m31 * aabb->xmax;
        maxpt.z += mat->m31 * aabb->xmin;
    }

    if (mat->m12 > 0.0f) {
        minpt.x += mat->m12 * aabb->ymin;
        maxpt.x += mat->m12 * aabb->ymax;
    }    else     {
        minpt.x += mat->m12 * aabb->ymax;
        maxpt.x += mat->m12 * aabb->ymin;
    }

    if (mat->m22 > 0.0f) {
        minpt.y += mat->m22 * aabb->ymin;
        maxpt.y += mat->m22 * aabb->ymax;
    }    else    {
        minpt.y += mat->m22 * aabb->ymax;
        maxpt.y += mat->m22 * aabb->ymin;
    }

    if (mat->m32 > 0.0f) {
        minpt.z += mat->m32 * aabb->ymin;
        maxpt.z += mat->m32 * aabb->ymax;
    }    else    {
        minpt.z += mat->m32 * aabb->ymax;
        maxpt.z += mat->m32 * aabb->ymin;
    }

    if (mat->m13 > 0.0f) {
        minpt.x += mat->m13 * aabb->zmin;
        maxpt.x += mat->m13 * aabb->zmax;
    }    else     {
        minpt.x += mat->m13 * aabb->zmax;
        maxpt.x += mat->m13 * aabb->zmin;
    }

    if (mat->m23 > 0.0f) {
        minpt.y += mat->m23 * aabb->zmin;
        maxpt.y += mat->m23 * aabb->zmax;
    }    else     {
        minpt.y += mat->m23 * aabb->zmax;
        maxpt.y += mat->m23 * aabb->zmin;
    }

    if (mat->m33 > 0.0f) {
        minpt.z += mat->m33 * aabb->zmin;
        maxpt.z += mat->m33 * aabb->zmax;
    }    else     {
        minpt.z += mat->m33 * aabb->zmax;
        maxpt.z += mat->m33 * aabb->zmin;
    }

    return sx_aabbv(minpt, maxpt);
}

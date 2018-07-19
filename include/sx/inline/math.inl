#ifndef SX_MATH_H_
#   error "This file must only be included from math.h"
#   include "math.h"
#endif

// https://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2
inline SX_CONST_FUNC int sx_nearest_pow2(int n)
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

inline SX_CONST_FUNC bool sx_ispow2(int n) 
{
    return (n & (n - 1)) == 0;
}

inline SX_CONST_FUNC float sx_torad(float _deg)
{
    return _deg * SX_PI / 180.0f;
}

inline SX_CONST_FUNC float sx_todeg(float _rad)
{
    return _rad * 180.0f / SX_PI;
}

// Packs float to uint32_t
inline SX_CONST_FUNC uint32_t sx_ftob(float _a)
{
    union { float f; uint32_t ui; } u = { _a };
    return u.ui;    
}

// Unpacks float from uint32_t
inline SX_CONST_FUNC float sx_btof(uint32_t _a)
{
    union { uint32_t ui; float f; } u = { _a };
    return u.f;    
}

// Packs double to uint64_t
inline SX_CONST_FUNC uint64_t sx_dtob(double _a)
{
    union { double f; uint64_t ui; } u = { _a };
    return u.ui;    
}

// Unpacks uint64_t to double
inline SX_CONST_FUNC double sx_btod(uint64_t _a)
{
    union { uint64_t ui; double f; } u = { _a };
    return u.f;    
}

// Returns sortable bit packed float value
// http://archive.fo/2012.12.08-212402/http://stereopsis.com/radix.html
inline SX_CONST_FUNC uint32_t sx_fflip(uint32_t _value)
{
	uint32_t mask = -((int32_t)(_value >> 31)) | 0x80000000;
	return _value ^ mask;    
}

inline SX_CONST_FUNC bool sx_isnan(float _f)
{
	const uint32_t tmp = sx_ftob(_f) & INT32_MAX;
	return tmp > UINT32_C(0x7f800000);
}

inline SX_CONST_FUNC bool sx_isnand(double _f)
{
    const uint64_t tmp = sx_dtob(_f) & INT64_MAX;
    return tmp > UINT64_C(0x7ff0000000000000);
}

inline SX_CONST_FUNC bool sx_isfin(float _f)
{
    const uint32_t tmp = sx_ftob(_f) & INT32_MAX;
    return tmp < UINT32_C(0x7f800000);
}

inline SX_CONST_FUNC bool sx_isfind(double _f)
{
    const uint64_t tmp = sx_dtob(_f) & INT64_MAX;
    return tmp < UINT64_C(0x7ff0000000000000);
}

inline SX_CONST_FUNC bool sx_isinf(float _f)
{
    const uint32_t tmp = sx_ftob(_f) & INT32_MAX;
    return tmp == UINT32_C(0x7f800000);
}

inline SX_CONST_FUNC bool sx_isinfd(double _f)
{
    const uint64_t tmp = sx_dtob(_f) & INT64_MAX;
    return tmp == UINT64_C(0x7ff0000000000000);
}

inline SX_CONST_FUNC float sx_round(float _f)
{
    return sx_floor(_f + 0.5f);
}

inline SX_CONST_FUNC float sx_ceil(float _f)
{
    return -sx_floor(-_f);
}

inline SX_CONST_FUNC float sx_lerp(float _a, float _b, float _t)
{
    return _a + (_b - _a) * _t;
}

inline SX_CONST_FUNC float sx_sign(float _a)
{
	return _a < 0.0f ? -1.0f : 1.0f;
}

inline SX_CONST_FUNC float sx_abs(float _a)
{
    return _a < 0.0f ? -_a : _a;
}

inline SX_CONST_FUNC float sx_sin(float _a)
{
    return sx_cos(_a - SX_PIHALF);
}

inline SX_CONST_FUNC float sx_sinh(float _a)
{
    return 0.5f*(sx_exp(_a) - sx_exp(-_a));
}

inline SX_CONST_FUNC float sx_asin(float _a)
{
    return SX_PIHALF - sx_acos(_a);
}

inline SX_CONST_FUNC float sx_cosh(float _a)
{
    return 0.5f*(sx_exp(_a) + sx_exp(-_a) );
}

inline SX_CONST_FUNC float sx_tan(float _a)
{
    return sx_sin(_a) / sx_cos(_a);
}

inline SX_CONST_FUNC float sx_tanh(float _a)
{
    const float tmp0   = sx_exp(2.0f*_a);
    const float tmp1   = tmp0 - 1.0f;
    const float tmp2   = tmp0 + 1.0f;
    const float result = tmp1 / tmp2;

    return result;
}

inline SX_CONST_FUNC float sx_atan(float _a)
{
    return sx_atan2(_a, 1.0f);
}

inline SX_CONST_FUNC float sx_pow(float _a, float _b)
{
    return sx_exp(_b * sx_log(_a));
}

inline SX_CONST_FUNC float sx_exp2(float _a)
{
    return sx_pow(2.0f, _a);
}

inline SX_CONST_FUNC float sx_log2(float _a)
{
    return sx_log(_a) * SX_INVLOG_NAT2;
}

inline SX_CONST_FUNC float sx_sqrt(float _a)
{
    if (_a >= SX_NEAR_ZERO) {
        return 1.0f/sx_rsqrt(_a);
    } else {
        return 0;
    }
}

inline SX_CONST_FUNC float sx_rsqrt(float _a)
{
    return sx_pow(_a, -0.5f);
}

// Returns the nearest integer not greater in magnitude than _a.
inline SX_CONST_FUNC float sx_trunc(float _a)
{
    return (float)((int)_a);
}

// Returns the fractional (or decimal) part of _a, which is 0~1
inline SX_CONST_FUNC float sx_fract(float _a)
{
    return _a - sx_trunc(_a);
}

// Returns the floating-point remainder of the division operation _a/_b.
inline SX_CONST_FUNC float sx_mod(float _a, float _b)
{
    return _a - _b * sx_floor(_a / _b);
}

inline SX_CONST_FUNC bool sx_equal(float _a, float _b, float _epsilon)
{
    // http://realtimecollisiondetection.net/blog/?p=89
    const float lhs = sx_abs(_a - _b);
    float aa = sx_abs(_a);
    float ab = sx_abs(_b);
    const float rhs = _epsilon * sx_max(1.0f, sx_max(aa, ab));
    return lhs <= rhs;
}

inline SX_CONST_FUNC bool sx_equal_arr(const float* _a, const float* _b, int _num, float _epsilon)
{
    bool result = sx_equal(_a[0], _b[0], _epsilon);
    for (int ii = 1; result && ii < _num; ++ii) {
        result = sx_equal(_a[ii], _b[ii], _epsilon);
    }
    return result;
}

inline SX_CONST_FUNC float sx_wrap(float _a, float _wrap)
{
    const float tmp0   = sx_mod(_a, _wrap);
    const float result = tmp0 < 0.0f ? _wrap + tmp0 : tmp0;
    return result;    
}

// Returns 0 if _a < _edge, else 1
inline SX_CONST_FUNC float sx_step(float _edge, float _a)
{
    return _a < _edge ? 0.0f : 1.0f;
}

// smooth Hermite interpolation (result = 0 ~ 1) when edge0 < x < edge1
inline SX_CONST_FUNC float sx_smoothstep(float _edge1, float _edge2, float _a)
{
    _a = sx_clamp((_a - _edge1)/(_edge2 - _edge1), 0.0f, 1.0f);
    return _a*_a*(3.0f - 2.0f*_a);
}

inline SX_CONST_FUNC float sx_pulse(float _a, float _start, float _end)
{
    return sx_step(_a, _start) - sx_step(_a, _end);
}

// References:
//  - Bias And Gain Are Your Friend
//    http://blog.demofox.org/2012/09/24/bias-and-gain-are-your-friend/
//  - http://demofox.org/biasgain.html
inline SX_CONST_FUNC float sx_bias(float _time, float _bias)
{
    return _time / ( ( (1.0f/_bias - 2.0f)*(1.0f - _time) ) + 1.0f);
}

inline SX_CONST_FUNC float sx_gain(float _time, float _gain)
{
    if (_time < 0.5f) 
        return sx_bias(_time * 2.0f, _gain) * 0.5f;

    return sx_bias(_time * 2.0f - 1.0f, 1.0f - _gain) * 0.5f + 0.5f;
}

inline SX_CONST_FUNC float sx_angle_diff(float _a, float _b)
{
    const float dist = sx_wrap(_b - _a, SX_PI2);
	return sx_wrap(dist*2.0f, SX_PI2) - dist;
}

inline SX_CONST_FUNC float sx_angle_lerp(float _a, float _b, float _t)
{
    return _a + sx_angle_diff(_a, _b) * _t;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Vec3
inline void sx_vec3_abs(float* _result, const float* _a)
{
    _result[0] = sx_abs(_a[0]);
    _result[1] = sx_abs(_a[1]);
    _result[2] = sx_abs(_a[2]);
}

inline void sx_vec3_neg(float* _result, const float* _a)
{
    _result[0] = -_a[0];
    _result[1] = -_a[1];
    _result[2] = -_a[2];
}

inline void sx_vec3_add(float* _result, const float* _a, const float* _b)
{
    _result[0] = _a[0] + _b[0];
    _result[1] = _a[1] + _b[1];
    _result[2] = _a[2] + _b[2];
}

inline void sx_vec3_addf(float* _result, const float* _a, float _b)
{
    _result[0] = _a[0] + _b;
    _result[1] = _a[1] + _b;
    _result[2] = _a[2] + _b;
}

inline void sx_vec3_sub(float* _result, const float* _a, const float* _b)
{
    _result[0] = _a[0] - _b[0];
    _result[1] = _a[1] - _b[1];
    _result[2] = _a[2] - _b[2];
}

inline void sx_vec3_subf(float* _result, const float* _a, float _b)
{
    _result[0] = _a[0] - _b;
    _result[1] = _a[1] - _b;
    _result[2] = _a[2] - _b;
}

inline void sx_vec3_mul(float* _result, const float* _a, const float* _b)
{
    _result[0] = _a[0] * _b[0];
    _result[1] = _a[1] * _b[1];
    _result[2] = _a[2] * _b[2];
}

inline void sx_vec3_mulf(float* _result, const float* _a, float _b)
{
    _result[0] = _a[0] * _b;
    _result[1] = _a[1] * _b;
    _result[2] = _a[2] * _b;
}

inline float sx_vec3_dot(const float* _a, const float* _b)
{
    return _a[0]*_b[0] + _a[1]*_b[1] + _a[2]*_b[2];
}

inline void sx_vec3_cross(float* _result, const float* _a, const float* _b)
{
    _result[0] = _a[1]*_b[2] - _a[2]*_b[1];
    _result[1] = _a[2]*_b[0] - _a[0]*_b[2];
    _result[2] = _a[0]*_b[1] - _a[1]*_b[0];
}

inline float sx_vec3_len(const float* _a)
{
    return sx_sqrt(sx_vec3_dot(_a, _a) );
}

inline void sx_vec3_lerp(float* _result, const float* _a, const float* _b, float _t)
{
    _result[0] = sx_lerp(_a[0], _b[0], _t);
    _result[1] = sx_lerp(_a[1], _b[1], _t);
    _result[2] = sx_lerp(_a[2], _b[2], _t);
}

inline float sx_vec3_norm(float* _result, const float* _a)
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

inline void sx_vec3_min(float* _result, const float* _a, const float* _b)
{
    _result[0] = sx_min(_a[0], _b[0]);
    _result[1] = sx_min(_a[1], _b[1]);
    _result[2] = sx_min(_a[2], _b[2]);
}

inline void sx_vec3_max(float* _result, const float* _a, const float* _b)
{
    _result[0] = sx_max(_a[0], _b[0]);
    _result[1] = sx_max(_a[1], _b[1]);
    _result[2] = sx_max(_a[2], _b[2]);
}

inline void sx_vec3_rcp(float* _result, const float* _a)
{
    _result[0] = 1.0f / _a[0];
    _result[1] = 1.0f / _a[1];
    _result[2] = 1.0f / _a[2];
}

inline void sx_vec3_tangent(const float* _n, float* _t, float* _b)
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

inline void sx_vec3_tangent_angle(const float* _n, float _angle, float* _t, float* _b)
{
    sx_vec3_tangent(_n, _t, _b);

    const float sa = sx_sin(_angle);
    const float ca = sx_cos(_angle);

    _t[0] = -sa * _b[0] + ca * _t[0];
    _t[1] = -sa * _b[1] + ca * _t[1];
    _t[2] = -sa * _b[2] + ca * _t[2];

    sx_vec3_cross(_b, _n, _t);
}

inline void sx_vec3_fromlatlong(float* _vec, float _u, float _v)
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

inline void sx_vec3_tolatlong(float* _outU, float* _outV, const float* _dir)
{
    const float phi   = sx_atan2(_dir[0], _dir[2]);
    const float theta = sx_acos(_dir[1]);

    *_outU = (SX_PI + phi)/SX_PI2;
    *_outV = theta*SX_PI;
}

inline void sx_vec3_mul_quat(float* _result, const float* _vec, const float* _quat)
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
// Quaternion
inline void sx_quat_ident(float* _result)
{
    _result[0] = 0.0f;
    _result[1] = 0.0f;
    _result[2] = 0.0f;
    _result[3] = 1.0f;
}

inline void sx_quat_mulXYZ(float* _result, const float* _qa, const float* _qb)
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

inline void sx_quat_mul(float* _result, const float* _qa, const float* _qb)
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

inline void sx_quat_inv(float* _result, const float* _quat)
{
    _result[0] = -_quat[0];
    _result[1] = -_quat[1];
    _result[2] = -_quat[2];
    _result[3] =  _quat[3];
}

inline float sx_quat_dot(const float* _a, const float* _b)
{
    return _a[0]*_b[0]
            + _a[1]*_b[1]
            + _a[2]*_b[2]
            + _a[3]*_b[3];
}

inline void sx_quat_norm(float* _result, const float* _quat)
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

inline void sx_quat_toeuler(float* _result, const float* _quat)
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

inline void sx_quat_rotateaxis(float* _result, const float* _axis, float _angle)
{
    const float ha = _angle * 0.5f;
    const float ca = sx_cos(ha);
    const float sa = sx_sin(ha);
    _result[0] = _axis[0] * sa;
    _result[1] = _axis[1] * sa;
    _result[2] = _axis[2] * sa;
    _result[3] = ca;
}

inline void sx_quat_rotateX(float* _result, float _ax)
{
    const float hx = _ax * 0.5f;
    const float cx = sx_cos(hx);
    const float sx = sx_sin(hx);
    _result[0] = sx;
    _result[1] = 0.0f;
    _result[2] = 0.0f;
    _result[3] = cx;
}

inline void sx_quat_rotateY(float* _result, float _ay)
{
    const float hy = _ay * 0.5f;
    const float cy = sx_cos(hy);
    const float sy = sx_sin(hy);
    _result[0] = 0.0f;
    _result[1] = sy;
    _result[2] = 0.0f;
    _result[3] = cy;
}

inline void sx_quat_rotateZ(float* _result, float _az)
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
// Matrix
inline void sx_mat4_ident(float* _result)
{
    memset(_result, 0x0, sizeof(float)*16);
    _result[0] = _result[5] = _result[10] = _result[15] = 1.0f;
}

inline void sx_mat4_translate(float* _result, float _tx, float _ty, float _tz)
{
    sx_mat4_ident(_result);
    _result[12] = _tx;
    _result[13] = _ty;
    _result[14] = _tz;
}

inline void sx_mat4_scale(float* _result, float _sx, float _sy, float _sz)
{
    memset(_result, 0x0, sizeof(float) * 16);
    _result[0]  = _sx;
    _result[5]  = _sy;
    _result[10] = _sz;
    _result[15] = 1.0f;
}

inline void sx_mat4_scalef(float* _result, float _scale)
{
    sx_mat4_scale(_result, _scale, _scale, _scale);
}

inline void sx_mat4_quat_translate(float* _result, const float* _quat, const float* _translation)
{
    sx_mat4_quat(_result, _quat);
    _result[12] = -(_result[0]*_translation[0] + _result[4]*_translation[1] + _result[ 8]*_translation[2]);
    _result[13] = -(_result[1]*_translation[0] + _result[5]*_translation[1] + _result[ 9]*_translation[2]);
    _result[14] = -(_result[2]*_translation[0] + _result[6]*_translation[1] + _result[10]*_translation[2]);
}

inline void sx_mat4_quat_translate_HMD(float* _result, const float* _quat, const float* _translation)
{
    float quat[4];
    quat[0] = -_quat[0];
    quat[1] = -_quat[1];
    quat[2] =  _quat[2];
    quat[3] =  _quat[3];
    sx_mat4_quat_translate(_result, quat, _translation);
}

inline void sx_vec3_mul_mat4(float* _result, const float* _vec, const float* _mat)
{
    _result[0] = _vec[0] * _mat[ 0] + _vec[1] * _mat[4] + _vec[2] * _mat[ 8] + _mat[12];
    _result[1] = _vec[0] * _mat[ 1] + _vec[1] * _mat[5] + _vec[2] * _mat[ 9] + _mat[13];
    _result[2] = _vec[0] * _mat[ 2] + _vec[1] * _mat[6] + _vec[2] * _mat[10] + _mat[14];
}

inline void sx_vec3_mul_mat4_xyz0(float* _result, const float* _vec, const float* _mat)
{
    _result[0] = _vec[0] * _mat[ 0] + _vec[1] * _mat[4] + _vec[2] * _mat[ 8];
    _result[1] = _vec[0] * _mat[ 1] + _vec[1] * _mat[5] + _vec[2] * _mat[ 9];
    _result[2] = _vec[0] * _mat[ 2] + _vec[1] * _mat[6] + _vec[2] * _mat[10];
}

inline void sx_vec3_mul_mat4H(float* _result, const float* _vec, const float* _mat)
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

inline void sx_vec4_mul(float* _result, const float* _a, const float* _b)
{
    _result[0] = _a[0] * _b[0];
    _result[1] = _a[1] * _b[1];
    _result[2] = _a[2] * _b[2];
    _result[3] = _a[3] * _b[3];
}

inline void sx_vec4_mulf(float* _result, const float* _a, float _b)
{
    _result[0] = _a[0] * _b;
    _result[1] = _a[1] * _b;
    _result[2] = _a[2] * _b;
    _result[3] = _a[3] * _b;
}

inline void sx_mat4_transpose(float* _result, const float* _a)
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
inline void sx_mat4_proj_flip_handedness(float* _dst, const float* _src)
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
inline void sx_mat4_view_flip_handedness(float* _dst, const float* _src)
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

inline SX_CONST_FUNC float sx_color_tolinear(float _a)
{
    const float lo     = _a / 12.92f;
    const float hi     = sx_pow( (_a + 0.055f) / 1.055f, 2.4f);
    const float result = sx_lerp(hi, lo, _a <= 0.04045f);
    return result;
}

inline SX_CONST_FUNC float sx_color_togamma(float _a)
{
    const float lo     = _a * 12.92f;
    const float hi     = sx_pow(sx_abs(_a), 1.0f/2.4f) * 1.055f - 0.055f;
    const float result = sx_lerp(hi, lo, _a <= 0.0031308f);
    return result;
}

inline void sx_vec3_mul_mat3(float* _result, const float* _vec, const float* _mat)
{
    _result[0] = _vec[0] * _mat[0] + _vec[1] * _mat[3] + _vec[2] * _mat[6];
    _result[1] = _vec[0] * _mat[1] + _vec[1] * _mat[4] + _vec[2] * _mat[7];
    _result[2] = _vec[0] * _mat[2] + _vec[1] * _mat[5] + _vec[2] * _mat[8];    
}

inline void sx_vec2_mul_mat3(float* _result, const float* _vec, const float* _mat)
{
    _result[0] = _vec[0] * _mat[0] + _vec[1] * _mat[3] + _mat[6];
    _result[1] = _vec[0] * _mat[1] + _vec[1] * _mat[4] + _mat[7];
}

inline void sx_mat3_translate(float* result, float x, float y)
{
    memset(result, 0x00, sizeof(float) * 9);
    result[0] = 1.0f;
    result[4] = 1.0f;
    result[6] = x;
    result[7] = y;
    result[8] = 1.0f;
}

inline void sx_mat3_rotate(float* result, float theta)
{
    memset(result, 0x00, sizeof(float) * 9);
    float c = sx_cos(theta);
    float s = sx_sin(theta);
    result[0] = c;     result[1] = -s;
    result[3] = s;     result[4] = c;
    result[8] = 1.0f;
}

inline void sx_mat3_scale(float* result, float sx, float sy)
{
    memset(result, 0x00, sizeof(float) * 9);
    result[0] = sx;
    result[4] = sy;
    result[8] = 1.0f;
}

inline void sx_mat3_SRT(float* _result, float _sx, float _sy, float _angle, float _tx, float _ty)
{
    memset(_result, 0x00, sizeof(float) * 9);
    float c = sx_cos(_angle);
    float s = sx_sin(_angle);
    _result[0] = c*_sx;     _result[1] = -s;
    _result[3] = s;         _result[4] = c*_sy;
    _result[6] = _tx;       _result[7] = _ty;
    _result[8] = 1.0f;
}

inline void sx_mat3_decompose(const float* _mat, float* translation, float* rotation)
{
    translation[0] = _mat[6];
    translation[1] = _mat[7];

    *rotation = sx_atan2(_mat[3], _mat[4]);
}

inline float sx_vec2_dot(const float* _a, const float* _b)
{
    return _a[0]*_b[0] + _a[1]*_b[1];
}

inline float sx_vec2_len(const float* _a)
{
    return sx_sqrt(sx_vec2_dot(_a, _a));
}

inline float sx_vec2_norm(float* _result, const float* _a)
{
    const float len = sx_vec2_len(_a);
    if (len >= SX_NEAR_ZERO) {
        const float inv_len = 1.0f/len;
        _result[0] = _a[0] * inv_len;
        _result[1] = _a[1] * inv_len;
    }
    return len;
}

inline void sx_vec2_min(float* _result, const float* _a, const float* _b)
{
    _result[0] = sx_min(_a[0], _b[0]);
    _result[1] = sx_min(_a[1], _b[1]);
}

inline void sx_vec2_max(float* _result, const float* _a, const float* _b)
{
    _result[0] = sx_max(_a[0], _b[0]);
    _result[1] = sx_max(_a[1], _b[1]);
}

inline void sx_vec2_lerp(float* _result, const float* _a, const float* _b, float _t)
{
    _result[0] = sx_lerp(_a[0], _b[0], _t);
    _result[1] = sx_lerp(_a[1], _b[1], _t);
}

inline void sx_vec2_abs(float* _result, const float* _a)
{
    _result[0] = sx_abs(_a[0]);
    _result[1] = sx_abs(_a[1]);
}

inline void sx_vec2_neg(float* _result, const float* _a)
{
    _result[0] = -_a[0];
    _result[1] = -_a[1];
}

inline void sx_vec2_add(float* _result, const float* _a, const float* _b)
{
    _result[0] = _a[0] + _b[0];
    _result[1] = _a[1] + _b[1];
}

inline void sx_vec2_addf(float* _result, const float* _a, float _b)
{
    _result[0] = _a[0] + _b;
    _result[1] = _a[1] + _b;
}

inline void sx_vec2_sub(float* _result, const float* _a, const float* _b)
{
    _result[0] = _a[0] - _b[0];
    _result[1] = _a[1] - _b[1];
}

inline void sx_vec2_subf(float* _result, const float* _a, float _b)
{
    _result[0] = _a[0] - _b;
    _result[1] = _a[1] - _b;
}

inline void sx_vec2_mul(float* _result, const float* _a, const float* _b)
{
    _result[0] = _a[0] * _b[0];
    _result[1] = _a[1] * _b[1];
}

inline void sx_vec2_mulf(float* _result, const float* _a, float _b)
{
    _result[0] = _a[0] * _b;
    _result[1] = _a[1] * _b;
}


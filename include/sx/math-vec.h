//
// Copyright 2018 Sepehr Taghdisian (septag@github). All rights reserved.
// License: https://github.com/septag/sx#license-bsd-2-clause
//
// see math.h for full info
//
#pragma once

#include "math-scalar.h"

SX_FORCE_INLINE float   sx_vec2_dot(sx_vec2 _a, sx_vec2 _b);
SX_FORCE_INLINE float   sx_vec2_len(sx_vec2 _a);
SX_FORCE_INLINE sx_vec2 sx_vec2_norm(sx_vec2 _a);
SX_FORCE_INLINE sx_vec2 sx_vec2_norm_len(sx_vec2 _a, float* outlen);
SX_FORCE_INLINE sx_vec2 sx_vec2_min(sx_vec2 _a, sx_vec2 _b);
SX_FORCE_INLINE sx_vec2 sx_vec2_max(sx_vec2 _a, sx_vec2 _b);
SX_FORCE_INLINE sx_vec2 sx_vec2_lerp(sx_vec2 _a, sx_vec2 _b, float _t);
SX_FORCE_INLINE sx_vec2 sx_vec2_abs(sx_vec2 _a);
SX_FORCE_INLINE sx_vec2 sx_vec2_neg(sx_vec2 _a);
SX_FORCE_INLINE sx_vec2 sx_vec2_add(sx_vec2 _a, sx_vec2 _b);
SX_FORCE_INLINE sx_vec2 sx_vec2_addf(sx_vec2 _a, float _b);
SX_FORCE_INLINE sx_vec2 sx_vec2_sub(sx_vec2 _a, sx_vec2 _b);
SX_FORCE_INLINE sx_vec2 sx_vec2_subf(sx_vec2 _a, float _b);
SX_FORCE_INLINE sx_vec2 sx_vec2_mul(sx_vec2 _a, sx_vec2 _b);
SX_FORCE_INLINE sx_vec2 sx_vec2_mulf(sx_vec2 _a, float _b);
SX_API          sx_vec2 sx_vec2_calc_linearfit2D(const sx_vec2* _points, int _num);

SX_FORCE_INLINE sx_vec3 sx_vec3_abs(sx_vec3 _a);
SX_FORCE_INLINE sx_vec3 sx_vec3_neg(sx_vec3 _a);
SX_FORCE_INLINE sx_vec3 sx_vec3_add(sx_vec3 _a, sx_vec3 _b);
SX_FORCE_INLINE sx_vec3 sx_vec3_addf(sx_vec3 _a, float _b);
SX_FORCE_INLINE sx_vec3 sx_vec3_sub(sx_vec3 _a, sx_vec3 _b);
SX_FORCE_INLINE sx_vec3 sx_vec3_subf(sx_vec3 _a, float _b);
SX_FORCE_INLINE sx_vec3 sx_vec3_mul(sx_vec3 _a, sx_vec3 _b);
SX_FORCE_INLINE sx_vec3 sx_vec3_mulf(sx_vec3 _a, float _b);
SX_FORCE_INLINE float   sx_vec3_dot(sx_vec3 _a, sx_vec3 _b);
SX_FORCE_INLINE sx_vec3 sx_vec3_cross(sx_vec3 _a, sx_vec3 _b);
SX_FORCE_INLINE float   sx_vec3_len(sx_vec3 _a);
SX_FORCE_INLINE sx_vec3 sx_vec3_lerp(sx_vec3 _a, sx_vec3 _b, float _t);
SX_FORCE_INLINE sx_vec3 sx_vec3_norm(sx_vec3 _a);
SX_FORCE_INLINE sx_vec3 sx_vec3_norm_len(sx_vec3 _a, float* _outlen);
SX_FORCE_INLINE sx_vec3 sx_vec3_min(sx_vec3 _a, sx_vec3 _b);
SX_FORCE_INLINE sx_vec3 sx_vec3_max(sx_vec3 _a, sx_vec3 _b);
SX_FORCE_INLINE sx_vec3 sx_vec3_rcp(sx_vec3 _a);
SX_FORCE_INLINE void    sx_vec3_tangent(sx_vec3* _t, sx_vec3* _b, sx_vec3 _n);
SX_FORCE_INLINE void    sx_vec3_tangent_angle(sx_vec3* _t, sx_vec3* _b, sx_vec3 _n, float _angle);
SX_FORCE_INLINE sx_vec3 sx_vec3_fromlatlong(float _u, float _v);
SX_FORCE_INLINE sx_vec2 sx_vec3_tolatlong(sx_vec3 _dir);
SX_FORCE_INLINE sx_vec3 sx_vec3_mul_quat(sx_vec3 _vec, sx_quat _quat);
SX_API          sx_vec3 sx_vec3_calc_linearfit3D(const sx_vec3* _points, int _num);

SX_API          sx_vec3  sx_plane_normal(sx_vec3 _va, sx_vec3 _vb, sx_vec3 _vc);
SX_API          sx_plane sx_plane3p(sx_vec3 _va, sx_vec3 _vb, sx_vec3 _vc);
SX_API          sx_plane sx_planenp(sx_vec3 _normal, sx_vec3 _p);
SX_API          float    sx_plane_distance(sx_plane _plane, sx_vec3 _p);
SX_API          sx_vec3  sx_plane_project_point(sx_plane _plane, sx_vec3 _p);
SX_API          sx_vec3  sx_plane_origin(sx_plane _plane);

SX_FORCE_INLINE sx_vec4 sx_mat4_row1(const sx_mat4* m);
SX_FORCE_INLINE sx_vec4 sx_mat4_row2(const sx_mat4* m);
SX_FORCE_INLINE sx_vec4 sx_mat4_row3(const sx_mat4* m);
SX_FORCE_INLINE sx_vec4 sx_mat4_row4(const sx_mat4* m);
SX_FORCE_INLINE sx_mat4 sx_mat4_translate(float _tx, float _ty, float _tz);
SX_FORCE_INLINE sx_mat4 sx_mat4_scale(float _sx, float _sy, float _sz);
SX_FORCE_INLINE sx_mat4 sx_mat4_scalef(float _scale);
SX_FORCE_INLINE sx_mat4 sx_mat4_rotateX(float _ax);
SX_FORCE_INLINE sx_mat4 sx_mat4_rotateY(float _ay);
SX_FORCE_INLINE sx_mat4 sx_mat4_rotateZ(float _az);
SX_FORCE_INLINE sx_mat4 sx_mat4_rotateXY(float _ax, float _ay);
SX_FORCE_INLINE sx_mat4 sx_mat4_rotateXYZ(float _ax, float _ay, float _az);
SX_FORCE_INLINE sx_mat4 sx_mat4_rotateZYX(float _ax, float _ay, float _az);
SX_FORCE_INLINE sx_mat4 sx_mat4_quat_translate(sx_quat _quat, sx_vec3 _translation);
SX_FORCE_INLINE sx_mat4 sx_mat4_quat_translate_HMD(sx_quat _quat, sx_vec3 _translation);
SX_FORCE_INLINE sx_vec3 sx_mat4_mul_vec3(const sx_mat4* _mat, sx_vec3 _vec);
SX_FORCE_INLINE sx_vec3 sx_mat4_mul_vec3_xyz0(const sx_mat4* _mat, sx_vec3 _vec);
SX_FORCE_INLINE sx_vec3 sx_mat4_mul_vec3_H(const sx_mat4* _mat, sx_vec3 _vec);
SX_FORCE_INLINE sx_vec4 sx_mat4_mul_vec4(const sx_mat4* _mat, sx_vec4 _vec);
SX_FORCE_INLINE sx_vec4 sx_vec4_mul(sx_vec4 _a, sx_vec4 _b);
SX_FORCE_INLINE sx_vec4 sx_vec4_mulf(sx_vec4 _a, float _b);
SX_FORCE_INLINE sx_vec4 sx_vec4_add(sx_vec4 _a, sx_vec4 _b);
SX_FORCE_INLINE sx_vec4 sx_vec4_sub(sx_vec4 _a, sx_vec4 _b);
SX_FORCE_INLINE sx_mat4 sx_mat4_transpose(const sx_mat4* _a);
SX_FORCE_INLINE void    sx_mat4_proj_flip_handedness(sx_mat4* _dst, const sx_mat4* _src);
SX_FORCE_INLINE void    sx_mat4_view_flip_handedness(sx_mat4* _dst, const sx_mat4* _src);
SX_API          sx_mat4 sx_mat4_from_normal(sx_vec3 _normal, float _scale, sx_vec3 _pos);
SX_API          sx_mat4 sx_mat4_from_normal_angle(sx_vec3 _normal, float _scale, sx_vec3 _pos, float _angle);
SX_API          sx_mat4 sx_mat4_view_lookat(sx_vec3 eye, sx_vec3 target, sx_vec3 up);
SX_API          sx_mat4 sx_mat4_view_lookatLH(sx_vec3 eye, sx_vec3 target, sx_vec3 up);
SX_API          sx_mat4 sx_mat4_view_FPS(sx_vec3 eye, float pitch, float yaw);
SX_API          sx_mat4 sx_mat4_view_arcball(sx_vec3 move, sx_quat rot, sx_vec3 target_pos);
SX_API          sx_mat4 sx_mat4_perspective(float width, float height, float zn, float zf, bool ogl_ndc);
SX_API          sx_mat4 sx_mat4_perspectiveLH(float width, float height, float zn, float zf, bool ogl_ndc);
SX_API          sx_mat4 sx_mat4_perspective_offcenter(float xmin, float ymin, float xmax, float ymax,
                                                      float zn, float zf, bool ogl_ndc);
SX_API          sx_mat4 sx_mat4_perspective_offcenterLH(float xmin, float ymin, float xmax, float ymax,
                                                        float zn, float zf, bool ogl_ndc);
SX_API          sx_mat4 sx_mat4_perspectiveFOV(float fov_y, float aspect, float zn, float zf, bool ogl_ndc);
SX_API          sx_mat4 sx_mat4_perspectiveFOV_LH(float fov_y, float aspect, float zn, float zf, bool ogl_ndc);
SX_API          sx_mat4 sx_mat4_ortho(float width, float height, float zn, float zf, float offset, bool ogl_ndc);
SX_API          sx_mat4 sx_mat4_orthoLH(float width, float height, float zn, float zf, float offset, bool ogl_ndc);
SX_API          sx_mat4 sx_mat4_ortho_offcenter(float xmin, float ymin, float xmax, float ymax, float zn,
                                                float zf, float offset, bool ogl_ndc);
SX_API          sx_mat4 sx_mat4_ortho_offcenterLH(float xmin, float ymin, float xmax, float ymax, float zn,
                                         float zf, float offset, bool ogl_ndc);
SX_API          sx_mat4 sx_mat4_SRT(float _sx, float _sy, float _sz, float _ax, float _ay, float _az,
                                    float _tx, float _ty, float _tz);
SX_API          sx_mat4 sx_mat4_mul(const sx_mat4* _a, const sx_mat4* _b);
SX_API          sx_mat4 sx_mat4_inv(const sx_mat4* _a);
SX_API          sx_mat4 sx_mat4_inv_transform(const sx_mat4* _a);
SX_API          sx_quat sx_mat4_quat(const sx_mat4* _mat);
SX_API          sx_mat4 sx_mat4_project_plane(sx_vec3 plane_normal);

SX_FORCE_INLINE sx_mat3 sx_mat3_transpose(const sx_mat3* _a);
SX_FORCE_INLINE sx_vec3 sx_mat3_mul_vec3(const sx_mat3* _mat, sx_vec3 _vec);
SX_FORCE_INLINE sx_mat3 sx_mat3_mul_inverse(const sx_mat3* _a, const sx_mat3* _b);
SX_FORCE_INLINE sx_vec3 sx_mat3_mul_vec3_inverse(const sx_mat3* mat, sx_vec3 v);
SX_FORCE_INLINE sx_vec2 sx_mat3_mul_vec2(const sx_mat3* _mat, sx_vec2 _vec);
SX_FORCE_INLINE sx_mat3 sx_mat3_translate(float x, float y);
SX_FORCE_INLINE sx_mat3 sx_mat3_translatev(sx_vec2 p);
SX_FORCE_INLINE sx_mat3 sx_mat3_rotate(float theta);
SX_FORCE_INLINE sx_mat3 sx_mat3_scale(float sx, float sy);
SX_FORCE_INLINE sx_mat3 sx_mat3_SRT(float sx, float sy, float angle, float tx, float ty);
SX_API          sx_mat3 sx_mat3_inv(const sx_mat3* _a);
SX_API          sx_mat3 sx_mat3_mul(const sx_mat3* _a, const sx_mat3* _b);

SX_API          sx_mat3 sx_quat_mat3(sx_quat quat);
SX_API          sx_mat4 sx_quat_mat4(sx_quat quat);
SX_FORCE_INLINE sx_vec3 sx_quat_mulXYZ(sx_quat _qa, sx_quat _qb);
SX_FORCE_INLINE sx_quat sx_quat_mul(sx_quat p, sx_quat q);
SX_FORCE_INLINE sx_quat sx_quat_inv(sx_quat _quat);
SX_FORCE_INLINE float   sx_quat_dot(sx_quat _a, sx_quat _b);
SX_FORCE_INLINE float   sx_quat_angle(sx_quat _a, sx_quat _b);
SX_FORCE_INLINE sx_quat sx_quat_norm(sx_quat _quat);
SX_FORCE_INLINE sx_quat sx_quat_rotateaxis(sx_vec3 _axis, float _angle);
SX_FORCE_INLINE sx_quat sx_quat_rotateX(float _ax);
SX_FORCE_INLINE sx_quat sx_quat_rotateY(float _ay);
SX_FORCE_INLINE sx_quat sx_quat_rotateZ(float _az);
SX_API          sx_quat sx_quat_lerp(sx_quat _a, sx_quat _b, float t);
SX_API          sx_quat sx_quat_slerp(sx_quat _a, sx_quat _b, float t);
SX_API          sx_vec3 sx_quat_toeuler(sx_quat _quat);
SX_API          sx_quat sx_quat_fromeular(sx_vec3 _vec3);

SX_FORCE_INLINE sx_vec4  sx_color_vec4(sx_color c);
SX_API          sx_vec4  sx_color_vec4_tosrgb(sx_vec4 cf);
SX_API          sx_vec4  sx_color_vec4_tolinear(sx_vec4 c);
SX_API          void     sx_color_RGBtoHSV(float _hsv[3], const float _rgb[3]);
SX_API          void     sx_color_HSVtoRGB(float _rgb[3], const float _hsv[3]);

SX_FORCE_INLINE bool    sx_aabb_isempty(const sx_aabb* aabb);
SX_FORCE_INLINE void    sx_aabb_add_point(sx_aabb* aabb, sx_vec3 pt);
SX_FORCE_INLINE sx_aabb sx_aabb_add(const sx_aabb* aabb1, const sx_aabb* aabb2);
SX_FORCE_INLINE bool    sx_aabb_test_point(const sx_aabb* aabb, sx_vec3 pt);
SX_FORCE_INLINE bool    sx_aabb_test(const sx_aabb* aabb1, const sx_aabb* aabb2);
SX_FORCE_INLINE sx_vec3 sx_aabb_corner(const sx_aabb* aabb, int index);
SX_FORCE_INLINE void    sx_aabb_corners(sx_vec3 corners[8], const sx_aabb* aabb);
SX_FORCE_INLINE sx_vec3 sx_aabb_extents(const sx_aabb* aabb);
SX_FORCE_INLINE sx_vec3 sx_aabb_center(const sx_aabb* aabb);
SX_FORCE_INLINE sx_aabb sx_aabb_translate(const sx_aabb* aabb, sx_vec3 offset);
SX_FORCE_INLINE sx_aabb sx_aabb_setpos(const sx_aabb* aabb, sx_vec3 pos);
SX_FORCE_INLINE sx_aabb sx_aabb_expand(const sx_aabb* aabb, sx_vec3 expand);
SX_FORCE_INLINE sx_aabb sx_aabb_scale(const sx_aabb* aabb, sx_vec3 scale);
SX_API          sx_aabb sx_aabb_transform(const sx_aabb* aabb, const sx_mat4* mat);
SX_API          sx_aabb sx_aabb_from_box(const sx_box* box);

SX_FORCE_INLINE sx_tx3d sx_tx3d_setf(float x, float y, float z, float rx, float ry, float rz);

////////////////////////////////////////////////////////////////////////////////////////////////////
// implementation

SX_FORCE_INLINE sx_vec3 sx_quat_mulXYZ(sx_quat _qa, sx_quat _qb)
{
    const float ax = _qa.x;
    const float ay = _qa.y;
    const float az = _qa.z;
    const float aw = _qa.w;

    const float bx = _qb.x;
    const float by = _qb.y;
    const float bz = _qb.z;
    const float bw = _qb.w;

    return sx_vec3f(aw * bx + ax * bw + ay * bz - az * by, 
                    aw * by - ax * bz + ay * bw + az * bx,
                    aw * bz + ax * by - ay * bx + az * bw);
}

// The product of two rotation quaternions will be equivalent to the rotation q followed by
// the rotation p
SX_FORCE_INLINE sx_quat sx_quat_mul(sx_quat p, sx_quat q)
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

SX_FORCE_INLINE sx_quat sx_quat_inv(sx_quat _quat)
{
    return sx_quat4f(-_quat.x, -_quat.y, -_quat.z, _quat.w);
}

SX_FORCE_INLINE float sx_quat_dot(sx_quat _a, sx_quat _b)
{
    return _a.x * _b.x + _a.y * _b.y + _a.z * _b.z + _a.w * _b.w;
}

SX_FORCE_INLINE float sx_quat_angle(sx_quat _a, sx_quat _b)
{
    return sx_acos(sx_min(sx_abs(sx_quat_dot(_a, _b)), 1.0f)) * 2.0f;
}

SX_FORCE_INLINE sx_quat sx_quat_norm(sx_quat _quat)
{
    const float inv_norm = sx_rsqrt(sx_quat_dot(_quat, _quat));
    return sx_quat4f(_quat.x*inv_norm, _quat.y*inv_norm, _quat.z*inv_norm, _quat.w*inv_norm);
}

SX_FORCE_INLINE sx_quat sx_quat_rotateaxis(sx_vec3 _axis, float _angle)
{
    const float ha = _angle * 0.5f;
    const float ca = sx_cos(ha);
    const float sa = sx_sin(ha);
    return sx_quat4f(_axis.x * sa, _axis.y * sa, _axis.z * sa, ca);
}

SX_FORCE_INLINE sx_quat sx_quat_rotateX(float _ax)
{
    const float hx = _ax * 0.5f;
    const float cx = sx_cos(hx);
    const float sx = sx_sin(hx);
    return sx_quat4f(sx, 0.0f, 0.0f, cx);
}

SX_FORCE_INLINE sx_quat sx_quat_rotateY(float _ay)
{
    const float hy = _ay * 0.5f;
    const float cy = sx_cos(hy);
    const float sy = sx_sin(hy);
    return sx_quat4f(0.0f, sy, 0.0f, cy);
}

SX_FORCE_INLINE sx_quat sx_quat_rotateZ(float _az)
{
    const float hz = _az * 0.5f;
    const float cz = sx_cos(hz);
    const float sz = sx_sin(hz);
    return sx_quat4f(0.0f, 0.0f, sz, cz);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Vec3
SX_FORCE_INLINE sx_vec3 sx_vec3_abs(sx_vec3 _a)
{
    return sx_vec3f(sx_abs(_a.x), sx_abs(_a.y), sx_abs(_a.z));
}

SX_FORCE_INLINE sx_vec3 sx_vec3_neg(sx_vec3 _a)
{
    return sx_vec3f(-_a.x, -_a.y, -_a.z);
}

SX_FORCE_INLINE sx_vec3 sx_vec3_add(sx_vec3 _a, sx_vec3 _b)
{
    return sx_vec3f(_a.x + _b.x, _a.y + _b.y, _a.z + _b.z);
}

SX_FORCE_INLINE sx_vec3 sx_vec3_addf(sx_vec3 _a, float _b)
{
    return sx_vec3f(_a.x + _b, _a.y + _b, _a.z + _b);
}

SX_FORCE_INLINE sx_vec3 sx_vec3_sub(sx_vec3 _a, sx_vec3 _b)
{
    return sx_vec3f(_a.x - _b.x, _a.y - _b.y, _a.z - _b.z);
}

SX_FORCE_INLINE sx_vec3 sx_vec3_subf(sx_vec3 _a, float _b)
{
    return sx_vec3f(_a.x - _b, _a.y - _b, _a.z - _b);
}

SX_FORCE_INLINE sx_vec3 sx_vec3_mul(sx_vec3 _a, sx_vec3 _b)
{
    return sx_vec3f(_a.x * _b.x, _a.y * _b.y, _a.z * _b.z);
}

SX_FORCE_INLINE sx_vec3 sx_vec3_mulf(sx_vec3 _a, float _b)
{
    return sx_vec3f(_a.x * _b, _a.y * _b, _a.z * _b);
}

SX_FORCE_INLINE float sx_vec3_dot(sx_vec3 _a, sx_vec3 _b)
{
    return _a.x * _b.x + _a.y * _b.y + _a.z * _b.z;
}

SX_FORCE_INLINE sx_vec3 sx_vec3_cross(sx_vec3 _a, sx_vec3 _b)
{
    return sx_vec3f(_a.y * _b.z - _a.z * _b.y, _a.z * _b.x - _a.x * _b.z,
                    _a.x * _b.y - _a.y * _b.x);
}

SX_FORCE_INLINE float sx_vec3_len(sx_vec3 _a)
{
    return sx_sqrt(sx_vec3_dot(_a, _a));
}

SX_FORCE_INLINE sx_vec3 sx_vec3_lerp(sx_vec3 _a, sx_vec3 _b, float _t)
{
    return sx_vec3f(sx_lerp(_a.x, _b.x, _t), sx_lerp(_a.y, _b.y, _t), sx_lerp(_a.z, _b.z, _t));
}

SX_FORCE_INLINE sx_vec3 sx_vec3_norm(sx_vec3 _a)
{
    return sx_vec3_mulf(_a, sx_rsqrt(sx_vec3_dot(_a, _a)));
}

SX_FORCE_INLINE sx_vec3 sx_vec3_norm_len(sx_vec3 _a, float* _outlen)
{
    sx_assert(_outlen);
    const float len = sx_vec3_len(_a);
    if (len > 0.0f) {
        const float invlen = 1.0f / len;
        *_outlen = len;
        return sx_vec3f(_a.x * invlen, _a.y * invlen, _a.z * invlen);
    } else {
        sx_assertf(0, "Divide by zero");
        return sx_vec3f(0.0f, 0.0f, 0.0f);
    }
}

SX_FORCE_INLINE sx_vec3 sx_vec3_min(sx_vec3 _a, sx_vec3 _b)
{
    return sx_vec3f(sx_min(_a.x, _b.x), sx_min(_a.y, _b.y), sx_min(_a.z, _b.z));
}

SX_FORCE_INLINE sx_vec3 sx_vec3_max(sx_vec3 _a, sx_vec3 _b)
{
    return sx_vec3f(sx_max(_a.x, _b.x), sx_max(_a.y, _b.y), sx_max(_a.z, _b.z));
}

SX_FORCE_INLINE sx_vec3 sx_vec3_rcp(sx_vec3 _a)
{
    return sx_vec3f(1.0f / _a.x, 1.0f / _a.y, 1.0f / _a.z);
}

SX_FORCE_INLINE void sx_vec3_tangent(sx_vec3* _t, sx_vec3* _b, sx_vec3 _n)
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

SX_FORCE_INLINE void sx_vec3_tangent_angle(sx_vec3* _t, sx_vec3* _b, sx_vec3 _n, float _angle)
{
    sx_vec3_tangent(_t, _b, _n);

    const float sa = sx_sin(_angle);
    const float ca = sx_cos(_angle);

    *_t = sx_vec3f(-sa * _b->x + ca * _t->x, -sa * _b->y + ca * _t->y, -sa * _b->z + ca * _t->z);

    *_b = sx_vec3_cross(_n, *_t);
}

SX_FORCE_INLINE sx_vec3 sx_vec3_fromlatlong(float _u, float _v)
{
    const float phi = _u * SX_PI2;
    const float theta = _v * SX_PI;

    const float st = sx_sin(theta);
    const float sp = sx_sin(phi);
    const float ct = sx_cos(theta);
    const float cp = sx_cos(phi);

    return sx_vec3f(-st * sp, -st * cp, ct);
}

SX_FORCE_INLINE sx_vec2 sx_vec3_tolatlong(sx_vec3 _dir)
{
    const float phi = sx_atan2(_dir.x, _dir.y);
    const float theta = sx_acos(_dir.z);

    return sx_vec2f((SX_PI + phi) / SX_PI2, theta * SX_INVPI);
}

SX_FORCE_INLINE sx_vec3 sx_vec3_mul_quat(sx_vec3 _vec, sx_quat _quat)
{
    sx_quat tmp0 = sx_quat_inv(_quat);
    sx_quat qv = sx_quat4f(_vec.x, _vec.y, _vec.z, 0.0f);
    sx_quat tmp1 = sx_quat_mul(qv, tmp0);
    return sx_quat_mulXYZ(_quat, tmp1);
}

SX_FORCE_INLINE sx_vec4 sx_mat4_row1(const sx_mat4* m)
{
    return sx_vec4f(m->m11, m->m12, m->m13, m->m14);
}

SX_FORCE_INLINE sx_vec4 sx_mat4_row2(const sx_mat4* m)
{
    return sx_vec4f(m->m21, m->m22, m->m23, m->m24);
}

SX_FORCE_INLINE sx_vec4 sx_mat4_row3(const sx_mat4* m)
{
    return sx_vec4f(m->m31, m->m32, m->m33, m->m34);
}

SX_FORCE_INLINE sx_vec4 sx_mat4_row4(const sx_mat4* m)
{
    return sx_vec4f(m->m41, m->m42, m->m43, m->m44);
}


SX_FORCE_INLINE sx_mat4 sx_mat4_translate(float _tx, float _ty, float _tz)
{
    return sx_mat4f(1.0f, 0.0f, 0.0f, _tx, 
                    0.0f, 1.0f, 0.0f, _ty, 
                    0.0f, 0.0f, 1.0f, _tz, 
                    0.0f, 0.0f, 0.0f, 1.0f);
}

SX_FORCE_INLINE sx_mat4 sx_mat4_scale(float _sx, float _sy, float _sz)
{
    return sx_mat4f(_sx, 0.0f, 0.0f, 0.0f, 
                    0.0f, _sy, 0.0f, 0.0f, 
                    0.0f, 0.0f, _sz, 0.0f, 
                    0.0f, 0.0f,0.0f, 1.0f);
}

SX_FORCE_INLINE sx_mat4 sx_mat4_scalef(float _scale)
{
    return sx_mat4_scale(_scale, _scale, _scale);
}

SX_FORCE_INLINE sx_mat4 sx_mat4_rotateX(float _ax)
{
    const float sx = sx_sin(_ax);
    const float cx = sx_cos(_ax);

    return sx_mat4f(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, cx, -sx, 0.0f, 0.0f, sx, cx, 0.0f, 0.0f, 0.0f,
                    0.0f, 1.0f);
}

SX_FORCE_INLINE sx_mat4 sx_mat4_rotateY(float _ay)
{
    const float sy = sx_sin(_ay);
    const float cy = sx_cos(_ay);

    return sx_mat4f(cy, 0.0f, sy, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, -sy, 0.0f, cy, 0.0f, 0.0f, 0.0f,
                    0.0f, 1.0f);
}

SX_FORCE_INLINE sx_mat4 sx_mat4_rotateZ(float _az)
{
    const float sz = sx_sin(_az);
    const float cz = sx_cos(_az);

    return sx_mat4f(cz, -sz, 0.0f, 0.0f, sz, cz, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
                    0.0f, 1.0f);
}

SX_FORCE_INLINE sx_mat4 sx_mat4_rotateXY(float _ax, float _ay)
{
    const float sx = sx_sin(_ax);
    const float cx = sx_cos(_ax);
    const float sy = sx_sin(_ay);
    const float cy = sx_cos(_ay);

    return sx_mat4f(cy, 0.0f, sy, 0.0f, sx * sy, cx, -sx * cy, 0.0f, -cx * sy, sx, cx * cy, 0.0f,
                    0.0f, 0.0f, 0.0f, 1.0f);
}

SX_FORCE_INLINE sx_mat4 sx_mat4_rotateXYZ(float _ax, float _ay, float _az)
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

SX_FORCE_INLINE sx_mat4 sx_mat4_rotateZYX(float _ax, float _ay, float _az)
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

SX_FORCE_INLINE sx_mat4 sx_mat4_quat_translate(sx_quat _quat, sx_vec3 _translation)
{
    sx_mat4 mat = sx_quat_mat4(_quat);
    mat.m14 = -(mat.m11 * _translation.x + mat.m12 * _translation.y + mat.m13 * _translation.z);
    mat.m24 = -(mat.m21 * _translation.x + mat.m22 * _translation.y + mat.m23 * _translation.z);
    mat.m34 = -(mat.m31 * _translation.x + mat.m32 * _translation.y + mat.m33 * _translation.z);
    return mat;
}

SX_FORCE_INLINE sx_mat4 sx_mat4_quat_translate_HMD(sx_quat _quat, sx_vec3 _translation)
{
    return sx_mat4_quat_translate(sx_quat4f(-_quat.x, -_quat.y, _quat.z, _quat.w), _translation);
}

/// multiply vector3 into 4x4 matrix without considering 4th column, which is not used in transform
/// matrices
SX_FORCE_INLINE sx_vec3 sx_mat4_mul_vec3(const sx_mat4* _mat, sx_vec3 _vec)
{
    return sx_vec3f(_vec.x * _mat->m11 + _vec.y * _mat->m12 + _vec.z * _mat->m13 + _mat->m14,
                    _vec.x * _mat->m21 + _vec.y * _mat->m22 + _vec.z * _mat->m23 + _mat->m24,
                    _vec.x * _mat->m31 + _vec.y * _mat->m32 + _vec.z * _mat->m33 + _mat->m34);
}

/// multiply vector3 into rotation part of the matrix only (used for normal vectors, etc...)
SX_FORCE_INLINE sx_vec3 sx_mat4_mul_vec3_xyz0(const sx_mat4* _mat, sx_vec3 _vec)
{
    return sx_vec3f(_vec.x * _mat->m11 + _vec.y * _mat->m12 + _vec.z * _mat->m13,
                    _vec.x * _mat->m21 + _vec.y * _mat->m22 + _vec.z * _mat->m23,
                    _vec.x * _mat->m31 + _vec.y * _mat->m32 + _vec.z * _mat->m33);
}

SX_FORCE_INLINE sx_vec3 sx_mat4_mul_vec3_H(const sx_mat4* _mat, sx_vec3 _vec)
{
    float xx = _vec.x * _mat->m11 + _vec.y * _mat->m12 + _vec.z * _mat->m13 + _mat->m14;
    float yy = _vec.x * _mat->m21 + _vec.y * _mat->m22 + _vec.z * _mat->m23 + _mat->m24;
    float zz = _vec.x * _mat->m31 + _vec.y * _mat->m32 + _vec.z * _mat->m33 + _mat->m34;
    float ww = _vec.x * _mat->m41 + _vec.y * _mat->m42 + _vec.z * _mat->m43 + _mat->m44;
    float iw = sx_sign(ww) / ww;
    return sx_vec3f(xx * iw, yy * iw, zz * iw);
}

SX_FORCE_INLINE sx_vec4 sx_mat4_mul_vec4(const sx_mat4* _mat, sx_vec4 _vec)
{
    return sx_vec4f(
        _vec.x * _mat->m11 + _vec.y * _mat->m12 + _vec.z * _mat->m13 + _vec.w * _mat->m14,
        _vec.x * _mat->m21 + _vec.y * _mat->m22 + _vec.z * _mat->m23 + _vec.w * _mat->m24,
        _vec.x * _mat->m31 + _vec.y * _mat->m32 + _vec.z * _mat->m33 + _vec.w * _mat->m34,
        _vec.x * _mat->m41 + _vec.y * _mat->m42 + _vec.z * _mat->m43 + _vec.w * _mat->m44);
}

SX_FORCE_INLINE sx_vec4 sx_vec4_mul(sx_vec4 _a, sx_vec4 _b)
{
    return sx_vec4f(_a.x * _b.x, _a.y * _b.y, _a.z * _b.z, _a.w * _b.w);
}

SX_FORCE_INLINE sx_vec4 sx_vec4_mulf(sx_vec4 _a, float _b)
{
    return sx_vec4f(_a.x * _b, _a.y * _b, _a.z * _b, _a.w * _b);
}

SX_FORCE_INLINE sx_vec4 sx_vec4_add(sx_vec4 _a, sx_vec4 _b)
{
    return sx_vec4f(_a.x + _b.x, _a.y + _b.y, _a.z + _b.z, _a.w + _b.w);
}

SX_FORCE_INLINE sx_vec4 sx_vec4_sub(sx_vec4 _a, sx_vec4 _b)
{
    return sx_vec4f(_a.x - _b.x, _a.y - _b.y, _a.z - _b.z, _a.w - _b.w);
}

SX_FORCE_INLINE sx_mat4 sx_mat4_transpose(const sx_mat4* _a)
{
    return sx_mat4f(_a->m11, _a->m21, _a->m31, _a->m41, _a->m12, _a->m22, _a->m32, _a->m42, _a->m13,
                    _a->m23, _a->m33, _a->m43, _a->m14, _a->m24, _a->m34, _a->m44);
}


/// Convert LH to RH projection matrix and vice versa.
SX_FORCE_INLINE void sx_mat4_proj_flip_handedness(sx_mat4* _dst, const sx_mat4* _src)
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
SX_FORCE_INLINE void sx_mat4_view_flip_handedness(sx_mat4* _dst, const sx_mat4* _src)
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

SX_FORCE_INLINE SX_CONSTFN float sx_color_tolinear(float _a)
{
    const float lo = _a / 12.92f;
    const float hi = sx_pow((_a + 0.055f) / 1.055f, 2.4f);
    const float result = sx_lerp(hi, lo, _a <= 0.04045f ? 1.0f : 0.0f);
    return result;
}

SX_FORCE_INLINE SX_CONSTFN float sx_color_togamma(float _a)
{
    const float lo = _a * 12.92f;
    const float hi = sx_pow(sx_abs(_a), 1.0f / 2.4f) * 1.055f - 0.055f;
    const float result = sx_lerp(hi, lo, _a <= 0.0031308f ? 1.0f : 0.0f);
    return result;
}


SX_FORCE_INLINE sx_mat3 sx_mat3_transpose(const sx_mat3* _a)
{
    return sx_mat3f(_a->m11, _a->m21, _a->m31, 
                    _a->m12, _a->m22, _a->m32, 
                    _a->m13, _a->m23, _a->m33);
}

SX_FORCE_INLINE sx_vec3 sx_mat3_mul_vec3(const sx_mat3* _mat, sx_vec3 _vec)
{
    return sx_vec3f(_vec.x * _mat->m11 + _vec.y * _mat->m12 + _vec.z * _mat->m13,
                    _vec.x * _mat->m21 + _vec.y * _mat->m22 + _vec.z * _mat->m23,
                    _vec.x * _mat->m31 + _vec.y * _mat->m32 + _vec.z * _mat->m33);
}

SX_FORCE_INLINE sx_mat3 sx_mat3_mul_inverse(const sx_mat3* _a, const sx_mat3* _b)
{
    sx_mat3 _atrans = sx_mat3_transpose(_a);
    return sx_mat3_mul(&_atrans, _b);
}

SX_FORCE_INLINE sx_vec3 sx_mat3_mul_vec3_inverse(const sx_mat3* mat, sx_vec3 v)
{
    sx_mat3 rmat = sx_mat3_transpose(mat);
    return sx_mat3_mul_vec3(&rmat, v);
}

SX_FORCE_INLINE sx_vec2 sx_mat3_mul_vec2(const sx_mat3* _mat, sx_vec2 _vec)
{
    return sx_vec2f(_vec.x * _mat->m11 + _vec.y * _mat->m12 + _mat->m13,
                    _vec.x * _mat->m21 + _vec.y * _mat->m22 + _mat->m23);
}

SX_FORCE_INLINE sx_mat3 sx_mat3_translate(float x, float y)
{
    return sx_mat3f(1.0f, 0.0f, x, 0.0f, 1.0f, y, 0.0f, 0.0f, 1.0f);
}

SX_FORCE_INLINE sx_mat3 sx_mat3_translatev(sx_vec2 p)
{
    return sx_mat3_translate(p.x, p.y);
}

SX_FORCE_INLINE sx_mat3 sx_mat3_rotate(float theta)
{
    float c = sx_cos(theta);
    float s = sx_sin(theta);
    return sx_mat3f(c, -s, 0.0f, s, c, 0.0f, 0.0f, 0.0f, 1.0f);
}

SX_FORCE_INLINE sx_mat3 sx_mat3_scale(float sx, float sy)
{
    return sx_mat3f(sx, 0.0f, 0.0f, 0.0f, sy, 0.0f, 0.0f, 0.0f, 1.0f);
}

SX_FORCE_INLINE sx_mat3 sx_mat3_SRT(float sx, float sy, float angle, float tx, float ty)
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

//
SX_FORCE_INLINE float sx_vec2_dot(sx_vec2 _a, sx_vec2 _b)
{
    return _a.x * _b.x + _a.y * _b.y;
}

SX_FORCE_INLINE float sx_vec2_len(sx_vec2 _a)
{
    return sx_sqrt(sx_vec2_dot(_a, _a));
}

SX_FORCE_INLINE sx_vec2 sx_vec2_norm(sx_vec2 _a)
{
    return sx_vec2_mulf(_a, sx_rsqrt(sx_vec2_dot(_a, _a)));
}

SX_FORCE_INLINE sx_vec2 sx_vec2_norm_len(sx_vec2 _a, float* outlen)
{
    const float len = sx_vec2_len(_a);
    if (len > 0.0f) {
        *outlen = len;
        return sx_vec2f(_a.x / len, _a.y / len);
    } else {
        sx_assertf(0, "Divide by zero");
        return _a;
    }
}

SX_FORCE_INLINE sx_vec2 sx_vec2_min(sx_vec2 _a, sx_vec2 _b)
{
    return sx_vec2f(sx_min(_a.x, _b.x), sx_min(_a.y, _b.y));
}

SX_FORCE_INLINE sx_vec2 sx_vec2_max(sx_vec2 _a, sx_vec2 _b)
{
    return sx_vec2f(sx_max(_a.x, _b.x), sx_max(_a.y, _b.y));
}

SX_FORCE_INLINE sx_vec2 sx_vec2_lerp(sx_vec2 _a, sx_vec2 _b, float _t)
{
    return sx_vec2f(sx_lerp(_a.x, _b.x, _t), sx_lerp(_a.y, _b.y, _t));
}

SX_FORCE_INLINE sx_vec2 sx_vec2_abs(sx_vec2 _a)
{
    return sx_vec2f(sx_abs(_a.x), sx_abs(_a.y));
}

SX_FORCE_INLINE sx_vec2 sx_vec2_neg(sx_vec2 _a)
{
    return sx_vec2f(-_a.x, -_a.y);
}

SX_FORCE_INLINE sx_vec2 sx_vec2_add(sx_vec2 _a, sx_vec2 _b)
{
    return sx_vec2f(_a.x + _b.x, _a.y + _b.y);
}

SX_FORCE_INLINE sx_vec2 sx_vec2_addf(sx_vec2 _a, float _b)
{
    return sx_vec2f(_a.x + _b, _a.y + _b);
}

SX_FORCE_INLINE sx_vec2 sx_vec2_sub(sx_vec2 _a, sx_vec2 _b)
{
    return sx_vec2f(_a.x - _b.x, _a.y - _b.y);
}

SX_FORCE_INLINE sx_vec2 sx_vec2_subf(sx_vec2 _a, float _b)
{
    return sx_vec2f(_a.x - _b, _a.y - _b);
}

SX_FORCE_INLINE sx_vec2 sx_vec2_mul(sx_vec2 _a, sx_vec2 _b)
{
    return sx_vec2f(_a.x * _b.x, _a.y * _b.y);
}

SX_FORCE_INLINE sx_vec2 sx_vec2_mulf(sx_vec2 _a, float _b)
{
    return sx_vec2f(_a.x * _b, _a.y * _b);
}

SX_FORCE_INLINE sx_ivec2 sx_ivec2_add(sx_ivec2 _a, sx_ivec2 _b)
{
    return sx_ivec2i(_a.x + _b.x, _a.y + _b.y);
}

SX_FORCE_INLINE sx_ivec2 sx_ivec2_sub(sx_ivec2 _a, sx_ivec2 _b)
{
    return sx_ivec2i(_a.x - _b.x, _a.y - _b.y);
}

SX_FORCE_INLINE sx_ivec2 sx_ivec2_min(sx_ivec2 _a, sx_ivec2 _b)
{
    return sx_ivec2i(sx_min(_a.x, _b.x), sx_min(_a.y, _b.y));
}

SX_FORCE_INLINE sx_ivec2 sx_ivec2_max(sx_ivec2 _a, sx_ivec2 _b)
{
    return sx_ivec2i(sx_max(_a.x, _b.x), sx_max(_a.y, _b.y));
}

SX_FORCE_INLINE sx_rect sx_rectce(sx_vec2 center, sx_vec2 extents)
{
    return sx_rectv(sx_vec2_sub(center, extents), sx_vec2_add(center, extents));
}

SX_FORCE_INLINE sx_rect sx_rect_expand(const sx_rect rc, sx_vec2 expand)
{
    return sx_rectf(rc.xmin - expand.x, rc.ymin - expand.y, rc.xmax + expand.x, rc.ymax + expand.y);
}

SX_FORCE_INLINE bool sx_rect_test_point(const sx_rect rc, sx_vec2 pt)
{
    if (pt.x < rc.xmin || pt.y < rc.ymin || pt.x > rc.xmax || pt.y > rc.ymax)
        return false;
    return true;
}

SX_FORCE_INLINE bool sx_rect_test_rect(const sx_rect rc1, const sx_rect rc2)
{
    if (rc1.xmax < rc2.xmin || rc1.xmin > rc2.xmax)
        return false;
    if (rc1.ymax < rc2.ymin || rc1.ymin > rc2.ymax)
        return false;
    return true;
}

SX_FORCE_INLINE void sx_rect_add_point(sx_rect* rc, sx_vec2 pt)
{
    *rc = sx_rectv(sx_vec2_min(sx_vec2fv(rc->vmin), pt), sx_vec2_max(sx_vec2fv(rc->vmax), pt));
}

SX_FORCE_INLINE sx_rect sx_rect_empty()
{
    return sx_rectf(SX_FLOAT_MAX, SX_FLOAT_MAX, -SX_FLOAT_MAX, -SX_FLOAT_MAX);
}

SX_FORCE_INLINE bool sx_rect_isempty(const sx_rect rc)
{
    return rc.xmin >= rc.xmax || rc.ymin >= rc.ymax;
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
SX_FORCE_INLINE sx_vec2 sx_rect_corner(const sx_rect* rc, int index)
{
    return sx_vec2f((index & 1) ? rc->xmax : rc->xmin, (index & 2) ? rc->ymax : rc->ymin);
}

SX_FORCE_INLINE void sx_rect_corners(sx_vec2 corners[4], const sx_rect* rc)
{
    for (int i = 0; i < 4; i++)
        corners[0] = sx_rect_corner(rc, i);
}

SX_FORCE_INLINE float sx_rect_width(const sx_rect rc)
{
    return rc.xmax - rc.xmin;
}

SX_FORCE_INLINE float sx_rect_height(const sx_rect rc)
{
    return rc.ymax - rc.ymin;
}

SX_FORCE_INLINE sx_vec2 sx_rect_extents(const sx_rect rc)
{
    return sx_vec2_mulf(sx_vec2_sub(sx_vec2fv(rc.vmax), sx_vec2fv(rc.vmin)), 0.5f);
}

SX_FORCE_INLINE sx_vec2 sx_rect_center(const sx_rect rc)
{
    return sx_vec2_mulf(sx_vec2_add(sx_vec2fv(rc.vmin), sx_vec2fv(rc.vmax)), 0.5f);
}

SX_FORCE_INLINE sx_rect sx_rect_move(const sx_rect rc, sx_vec2 pos) 
{
    return sx_rectv(sx_vec2_add(pos, sx_vec2fv(rc.vmin)), sx_vec2_add(pos, sx_vec2fv(rc.vmax)));
}

SX_FORCE_INLINE sx_irect sx_irect_expand(const sx_irect rc, sx_ivec2 expand)
{
    return sx_irecti(rc.xmin - expand.x, rc.ymin - expand.y, rc.xmax + expand.x, rc.ymax + expand.y);
}

SX_FORCE_INLINE bool sx_irect_test_point(const sx_irect rc, sx_ivec2 pt)
{
    if (pt.x < rc.xmin || pt.y < rc.ymin || pt.x > rc.xmax || pt.y > rc.ymax)
        return false;
    return true;
}

SX_FORCE_INLINE bool sx_irect_test_rect(const sx_irect rc1, const sx_irect rc2)
{
    if (rc1.xmax < rc2.xmin || rc1.xmin > rc2.xmax)
        return false;
    if (rc1.ymax < rc2.ymin || rc1.ymin > rc2.ymax)
        return false;
    return true;
}

SX_FORCE_INLINE void sx_irect_add_point(sx_irect* rc, sx_ivec2 pt)
{
    *rc = sx_irectv(sx_ivec2_min(sx_ivec2iv(rc->vmin), pt), sx_ivec2_max(sx_ivec2iv(rc->vmax), pt));
}

SX_FORCE_INLINE int sx_irect_width(const sx_irect rc)
{
    return rc.xmax - rc.xmin;
}

SX_FORCE_INLINE int sx_irect_height(const sx_irect rc)
{
    return rc.ymax - rc.ymin;
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
SX_FORCE_INLINE sx_ivec2 sx_irect_corner(const sx_irect* rc, int index)
{
    return sx_ivec2i((index & 1) ? rc->xmax : rc->xmin, (index & 2) ? rc->ymax : rc->ymin);
}

SX_FORCE_INLINE void sx_irect_corners(sx_ivec2 corners[4], const sx_irect* rc)
{
    for (int i = 0; i < 4; i++)
        corners[0] = sx_irect_corner(rc, i);
}

SX_FORCE_INLINE bool sx_aabb_isempty(const sx_aabb* aabb)
{
    return aabb->xmin >= aabb->xmax || aabb->ymin >= aabb->ymax || aabb->zmin >= aabb->zmax;
}

SX_FORCE_INLINE void sx_aabb_add_point(sx_aabb* aabb, sx_vec3 pt)
{
    *aabb = sx_aabbv(sx_vec3_min(sx_vec3fv(aabb->vmin), pt), sx_vec3_max(sx_vec3fv(aabb->vmax), pt));
}

SX_FORCE_INLINE sx_aabb sx_aabb_add(const sx_aabb* aabb1, const sx_aabb* aabb2)
{
    sx_aabb r = *aabb1;
    sx_aabb_add_point(&r, sx_vec3fv(aabb2->vmin));
    sx_aabb_add_point(&r, sx_vec3fv(aabb2->vmax));
    return r;
}

SX_FORCE_INLINE bool sx_aabb_test_point(const sx_aabb* aabb, sx_vec3 pt)
{
    if (aabb->xmax < pt.x || aabb->xmin > pt.x)
        return false;
    if (aabb->ymax < pt.y || aabb->ymin > pt.y)
        return false;
    if (aabb->zmax < pt.z || aabb->zmin > pt.z)
        return false;
    return true;
}

SX_FORCE_INLINE bool sx_aabb_test(const sx_aabb* aabb1, const sx_aabb* aabb2)
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
SX_FORCE_INLINE sx_vec3 sx_aabb_corner(const sx_aabb* aabb, int index)
{
    sx_assert(index < 8);
    return sx_vec3f((index & 1) ? aabb->xmax : aabb->xmin,
                    (index & 4) ? aabb->ymax : aabb->ymin,
                    (index & 2) ? aabb->zmax : aabb->zmin);
}

SX_FORCE_INLINE void sx_aabb_corners(sx_vec3 corners[8], const sx_aabb* aabb)
{
    for (int i = 0; i < 8; i++)
        corners[i] = sx_aabb_corner(aabb, i);
}

SX_FORCE_INLINE sx_vec3 sx_aabb_extents(const sx_aabb* aabb)
{
    return sx_vec3_mulf(sx_vec3f(aabb->xmax - aabb->xmin, 
                                 aabb->ymax - aabb->ymin,
                                 aabb->zmax - aabb->zmin), 0.5f);
}

SX_FORCE_INLINE sx_vec3 sx_aabb_center(const sx_aabb* aabb)
{
    return sx_vec3_mulf(sx_vec3_add(sx_vec3fv(aabb->vmin), sx_vec3fv(aabb->vmax)), 0.5f);
}

SX_FORCE_INLINE sx_aabb sx_aabb_translate(const sx_aabb* aabb, sx_vec3 offset)
{
    return sx_aabbv(sx_vec3_add(sx_vec3fv(aabb->vmin), offset), sx_vec3_add(sx_vec3fv(aabb->vmax), offset));
}

SX_FORCE_INLINE sx_aabb sx_aabb_setpos(const sx_aabb* aabb, sx_vec3 pos)
{
    sx_vec3 e = sx_aabb_extents(aabb);
    return sx_aabbf(pos.x - e.x, pos.y - e.y, pos.z - e.z, 
                    pos.x + e.x, pos.y + e.y, pos.z + e.z);
}

SX_FORCE_INLINE sx_aabb sx_aabb_expand(const sx_aabb* aabb, sx_vec3 expand)
{
    sx_vec3 p = sx_aabb_center(aabb);
    sx_vec3 e = sx_vec3_add(sx_aabb_extents(aabb), expand);
    return sx_aabbf(p.x - e.x, p.y - e.y, p.z - e.z, 
                    p.x + e.x, p.y + e.y, p.z + e.z);
}

SX_FORCE_INLINE sx_aabb sx_aabb_scale(const sx_aabb* aabb, sx_vec3 scale)
{
    sx_vec3 p = sx_aabb_center(aabb);
    sx_vec3 e = sx_vec3_mul(sx_aabb_extents(aabb), scale);
    return sx_aabbf(p.x - e.x, p.y - e.y, p.z - e.z, 
                    p.x + e.x, p.y + e.y, p.z + e.z);
}

SX_FORCE_INLINE sx_tx3d sx_tx3d_mul(const sx_tx3d* txa, const sx_tx3d* txb)
{
    return sx_tx3d_set(sx_vec3_add(sx_mat3_mul_vec3(&txa->rot, txb->pos), txa->pos),
                       sx_mat3_mul(&txa->rot, &txb->rot));
}

SX_FORCE_INLINE sx_vec3 sx_tx3d_mul_vec3(const sx_tx3d* tx, sx_vec3 v)
{
    return sx_vec3_add(sx_mat3_mul_vec3(&tx->rot, v), tx->pos);
}   

SX_FORCE_INLINE sx_vec3 sx_tx3d_mul_vec3_scale(const sx_tx3d* tx, sx_vec3 scale, sx_vec3 v)
{
    return sx_vec3_add(sx_mat3_mul_vec3(&tx->rot, sx_vec3_mul(v, scale)), tx->pos);
}

SX_FORCE_INLINE sx_tx3d sx_tx3d_inverse(const sx_tx3d* tx)
{   
    sx_mat3 rot_inv = sx_mat3_transpose(&tx->rot);
    return sx_tx3d_set(sx_mat3_mul_vec3(&rot_inv, sx_vec3_mulf(tx->pos, -1.0f)), rot_inv);
}

SX_FORCE_INLINE sx_vec3 sx_tx3d_mul_vec3_inverse(const sx_tx3d* tx, sx_vec3 v)
{   
    sx_mat3 rmat = sx_mat3_transpose(&tx->rot);
    return sx_mat3_mul_vec3(&rmat, sx_vec3_sub(v, tx->pos));
}

SX_FORCE_INLINE sx_tx3d sx_tx3d_mul_inverse(const sx_tx3d* txa, const sx_tx3d* txb)
{
    return sx_tx3d_set(sx_mat3_mul_vec3_inverse(&txa->rot, sx_vec3_sub(txb->pos, txa->pos)), 
                                                sx_mat3_mul_inverse(&txa->rot, &txb->rot));
}

SX_FORCE_INLINE sx_mat4 sx_tx3d_mat4(const sx_tx3d* tx)
{
    return sx_mat4v(sx_vec4v3(sx_vec3fv(tx->rot.fc1), 0.0f),
                    sx_vec4v3(sx_vec3fv(tx->rot.fc2), 0.0f),
                    sx_vec4v3(sx_vec3fv(tx->rot.fc3), 0.0f),
                    sx_vec4v3(tx->pos,      1.0f));
}

SX_FORCE_INLINE sx_tx3d sx_mat4_tx3d(const sx_mat4* mat)
{
    return sx_tx3d_set(sx_vec3fv(mat->fc4),  sx_mat3fv(mat->fc1, mat->fc2, mat->fc3));
}

SX_FORCE_INLINE sx_tx3d sx_tx3d_setf(float x, float y, float z, float rx, float ry, float rz)
{
    sx_mat4 rot = sx_mat4_rotateXYZ(rx, ry, rz);
    return sx_tx3d_set(sx_vec3f(x, y, z), sx_mat3fv(rot.fc1, rot.fc2, rot.fc3));
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// cpp operators
#ifdef __cplusplus
SX_FORCE_INLINE sx_vec2 operator+(sx_vec2 a, sx_vec2 b)
{
    return sx_vec2_add(a, b);
}

SX_FORCE_INLINE sx_vec2 operator-(sx_vec2 a, sx_vec2 b)
{
    return sx_vec2_sub(a, b);
}

SX_FORCE_INLINE sx_vec2 operator*(sx_vec2 v, float k)
{
    return sx_vec2_mulf(v, k);
}

SX_FORCE_INLINE sx_vec2 operator*(float k, sx_vec2 v)
{
    return sx_vec2_mulf(v, k);
}

SX_FORCE_INLINE sx_vec2 operator*(sx_vec2 v0, sx_vec2 v1)
{
    return sx_vec2_mul(v0, v1);
}

SX_FORCE_INLINE sx_ivec2 operator+(sx_ivec2 a, sx_ivec2 b)
{
    return sx_ivec2_add(a, b);
}

SX_FORCE_INLINE sx_ivec2 operator-(sx_ivec2 a, sx_ivec2 b)
{
    return sx_ivec2_sub(a, b);
}

SX_FORCE_INLINE sx_vec3 operator+(sx_vec3 v1, sx_vec3 v2)
{
    return sx_vec3_add(v1, v2);
}

SX_FORCE_INLINE sx_vec3 operator-(sx_vec3 v1, sx_vec3 v2)
{
    return sx_vec3_sub(v1, v2);
}

SX_FORCE_INLINE sx_vec3 operator*(sx_vec3 v, float k)
{
    return sx_vec3_mulf(v, k);
}

SX_FORCE_INLINE sx_vec3 operator*(float k, sx_vec3 v)
{
    return sx_vec3_mulf(v, k);
}

SX_FORCE_INLINE sx_mat4 operator*(const sx_mat4& a, const sx_mat4& b)
{
    return sx_mat4_mul(&a, &b);
}

SX_FORCE_INLINE sx_mat3 operator*(const sx_mat3& a, const sx_mat3& b)
{
    return sx_mat3_mul(&a, &b);
}

SX_FORCE_INLINE sx_quat operator*(sx_quat& a, sx_quat& b)
{
    return sx_quat_mul(a, b);
}
#endif

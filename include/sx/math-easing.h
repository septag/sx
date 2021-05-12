//
// Copyright 2018 Sepehr Taghdisian (septag@github). All rights reserved.
// License: https://github.com/septag/sx#license-bsd-2-clause
//
// see math.h for full info
//
//      Reference: https://easings.net/
//
#pragma once

#include "math-scalar.h"

SX_FORCE_INLINE float sx_easein_quad(float t);
SX_FORCE_INLINE float sx_easeout_quad(float t);
SX_FORCE_INLINE float sx_easeinout_quad(float t);
SX_FORCE_INLINE float sx_easein_cubic(float t);
SX_FORCE_INLINE float sx_easeout_cubic(float t);
SX_FORCE_INLINE float sx_easeinout_cubic(float t);
SX_FORCE_INLINE float sx_easein_quart(float t);
SX_FORCE_INLINE float sx_easeout_quart(float t);
SX_FORCE_INLINE float sx_easeinout_quart(float t);
SX_FORCE_INLINE float sx_easein_quint(float t);
SX_FORCE_INLINE float sx_easeout_quint(float t);
SX_FORCE_INLINE float sx_easeinout_quint(float t);
SX_FORCE_INLINE float sx_easein_sine(float t);
SX_FORCE_INLINE float sx_easeout_sine(float t);
SX_FORCE_INLINE float sx_easeinout_sine(float t);
SX_FORCE_INLINE float sx_easein_circ(float t);
SX_FORCE_INLINE float sx_easeout_circ(float t);
SX_FORCE_INLINE float sx_easeinout_circ(float t);
SX_FORCE_INLINE float sx_easein_expo(float t);
SX_FORCE_INLINE float sx_easeout_expo(float t);
SX_FORCE_INLINE float sx_easeinout_expo(float t);
SX_FORCE_INLINE float sx_easein_elastic(float t);
SX_FORCE_INLINE float sx_easeout_elastic(float t);
SX_FORCE_INLINE float sx_easeinout_elastic(float t);
SX_FORCE_INLINE float sx_easein_back(float t);
SX_FORCE_INLINE float sx_easeout_back(float t);
SX_FORCE_INLINE float sx_easeinout_back(float t);
SX_FORCE_INLINE float sx_easeout_bounce(float t);
SX_FORCE_INLINE float sx_easein_bounce(float t);
SX_FORCE_INLINE float sx_easeinout_bounce(float t);

////////////////////////////////////////////////////////////////////////////////////////////////////
// easing
SX_FORCE_INLINE float sx_easein_quad(float t)
{
    return t * t;
}

SX_FORCE_INLINE float sx_easeout_quad(float t)
{
    return -(t * (t - 2.0f));
}

SX_FORCE_INLINE float sx_easeinout_quad(float t)
{
    if (t < 0.5f) {
        return 2.0f * t * t;
    } else {
        return (-2.0f * t * t) + (4.0f * t) - 1.0f;
    }
}

SX_FORCE_INLINE float sx_easein_cubic(float t)
{
    return t * t * t;
}

SX_FORCE_INLINE float sx_easeout_cubic(float t)
{
    float f = (t - 1.0f);
    return f * f * f + 1.0f;
}

SX_FORCE_INLINE float sx_easeinout_cubic(float t)
{
    if (t < 0.5f) {
        return 4.0f * t * t * t;
    } else {
        float f = ((2.0f * t) - 2.0f);
        return 0.5f * f * f * f + 1;
    }
}

SX_FORCE_INLINE float sx_easein_quart(float t)
{
    return t * t * t * t;
}

SX_FORCE_INLINE float sx_easeout_quart(float t)
{
    float f = (t - 1.0f);
    return f * f * f * (1.0f - t) + 1.0f;
}

SX_FORCE_INLINE float sx_easeinout_quart(float t)
{
    if (t < 0.5f) {
        return 8.0f * t * t * t * t;
    } else {
        float f = (t - 1.0f);
        return -8.0f * f * f * f * f + 1.0f;
    }
}

SX_FORCE_INLINE float sx_easein_quint(float t)
{
    return t * t * t * t * t;
}

SX_FORCE_INLINE float sx_easeout_quint(float t)
{
    float f = (t - 1.0f);
    return f * f * f * f * f + 1.0f;
}

SX_FORCE_INLINE float sx_easeinout_quint(float t)
{
    if (t < 0.5f) {
        return 16.0f * t * t * t * t * t;
    } else {
        float f = ((2.0f * t) - 2.0f);
        return 0.5f * f * f * f * f * f + 1.0f;
    }
}

SX_FORCE_INLINE float sx_easein_sine(float t)
{
    return sx_sin((t - 1) * SX_PI2) + 1.0f;
}

SX_FORCE_INLINE float sx_easeout_sine(float t)
{
    return sx_sin(t * SX_PI2);
}

SX_FORCE_INLINE float sx_easeinout_sine(float t)
{
    return 0.5f * (1.0f - sx_cos(t * SX_PI));
}

SX_FORCE_INLINE float sx_easein_circ(float t)
{
    return 1.0f - sx_sqrt(1.0f - (t * t));
}

SX_FORCE_INLINE float sx_easeout_circ(float t)
{
    return sx_sqrt((2.0f - t) * t);
}

SX_FORCE_INLINE float sx_easeinout_circ(float t)
{
    if (t < 0.5f) {
        return 0.5f * (1.0f - sx_sqrt(1.0f - 4.0f * (t * t)));
    } else {
        return 0.5f * (sx_sqrt(-((2.0f * t) - 3.0f) * ((2.0f * t) - 1.0f)) + 1.0f);
    }
}

SX_FORCE_INLINE float sx_easein_expo(float t)
{
    return (t == 0.0f) ? t : sx_pow(2.0f, 10.0f * (t - 1.0f));
}

SX_FORCE_INLINE float sx_easeout_expo(float t)
{
    return (t == 1.0f) ? t : 1.0f - sx_pow(2.0f, -10.0f * t);
}

SX_FORCE_INLINE float sx_easeinout_expo(float t)
{
    if (t == 0.0f || t == 1.0f)
        return t;

    if (t < 0.5f) {
        return 0.5f * sx_pow(2.0f, (20.0f * t) - 10.0f);
    } else {
        return -0.5f * sx_pow(2.0f, (-20.0f * t) + 10.0f) + 1.0f;
    }
}

SX_FORCE_INLINE float sx_easein_elastic(float t)
{
    return sx_sin(13.0f * SX_PI2 * t) * sx_pow(2.0f, 10.0f * (t - 1.0f));
}

SX_FORCE_INLINE float sx_easeout_elastic(float t)
{
    return sx_sin(-13.0f * SX_PI2 * (t + 1.0f)) * sx_pow(2.0f, -10.0f * t) + 1.0f;
}

SX_FORCE_INLINE float sx_easeinout_elastic(float t)
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

SX_FORCE_INLINE float sx_easein_back(float t)
{
    const float s = 1.70158f;
    return t * t * ((s + 1.0f) * t - s);
}

SX_FORCE_INLINE float sx_easeout_back(float t)
{
    const float s = 1.70158f;
    return --t, 1.0f * (t * t * ((s + 1.0f) * t + s) + 1.0f);
}

SX_FORCE_INLINE float sx_easeinout_back(float t)
{
    const float s = 2.5949095f;
    if (t < 0.5f) {
        return t *= 2.0f, 0.5f * t * t * (t * s + t - s);
    } else {
        return t = t * 2.0f - 2.0f, 0.5f * (2.0f + t * t * (t * s + t + s));
    }
}

SX_FORCE_INLINE float sx_easeout_bounce(float t)
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

SX_FORCE_INLINE float sx_easein_bounce(float t)
{
    return 1.0f - sx_easeout_bounce(1.0f - t);
}

SX_FORCE_INLINE float sx_easeinout_bounce(float t)
{
    if (t < 0.5f) {
        return 0.5f * sx_easein_bounce(t * 2.0f);
    } else {
        return 0.5f * sx_easeout_bounce(t * 2.0f - 1.0f) + 0.5f;
    }
}

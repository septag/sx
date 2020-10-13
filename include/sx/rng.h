//
// Copyright 2018 Sepehr Taghdisian (septag@github). All rights reserved.
// License: https://github.com/septag/sx#license-bsd-2-clause
//
// rng.h - v1.0 - Random number generator
//                Currently has PCG implementation
//                Source: http://www.pcg-random.org
//
//      sx_rng_seed         initialize random generator with a seed number
//      sx_rng_seed_time    initialize random generator with current timestamp
//      sx_rng_gen          generates an integer number [0..UINT_MAX]
//      sx_rng_genf        generates an float number between [0..1]
//      sx_rng_gen_rangei   generates an integer between the specified rang [min..max]
//      sx_rng_gen_rangef()
//
#pragma once

#include "sx.h"

#include <stdint.h>

typedef struct sx_rng {
    uint64_t state[2];
} sx_rng;

SX_API void sx_rng_seed(sx_rng* rng, uint32_t seed);
SX_API void sx_rng_seed_time(sx_rng* rng);
SX_API uint32_t sx_rng_gen(sx_rng* rng);
SX_API float sx_rng_genf(sx_rng* rng);

SX_INLINE int sx_rng_gen_rangei(sx_rng* rng, int _min, int _max)
{
    sx_assert(_min <= _max);

    uint32_t const range = (uint32_t)(_max - _min) + 1;
    return _min + (int)(sx_rng_gen(rng) % range);
}

SX_INLINE float sx_rng_gen_rangef(sx_rng* rng, float _min, float _max)
{
    sx_assert(_min <= _max);

    float const r = sx_rng_genf(rng);
    return _min + r*(_max - _min);
}
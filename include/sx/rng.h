//
// Copyright 2018 Sepehr Taghdisian (septag@github). All rights reserved.
// License: https://github.com/septag/sx#license-bsd-2-clause
//
// rng.h - v1.0 - Random number generator
//                Currently has PCG implementation
//                Source: http://www.pcg-random.org
//
//      sx_rng_seed         initialize and seed hash state
//      sx_rng_gen          generates an integer number (0~UINT32_MAX)
//      sx_rng_gen_f        generates an float number between 0 and 1
//      sx_rng_gen_irange   generates an integer between the specified range
//
#pragma once

#ifndef SX_RNG_H_
#define SX_RNG_H_

#include "sx.h"

typedef struct sx_rng_s
{
    uint64_t state[2];
} sx_rng;

#ifdef __cplusplus
extern "C" {
#endif

void     sx_rng_seed(sx_rng* rng, uint32_t seed);
uint32_t sx_rng_gen(sx_rng* rng);
float    sx_rng_gen_f(sx_rng* rng);
int      sx_rng_gen_irange(sx_rng* rng, int _min, int _max);

#ifdef __cplusplus
}
#endif

#endif  // SX_RNG_H_
//
// Copyright 2018 Sepehr Taghdisian (septag@github). All rights reserved.
// License: https://github.com/septag/sx#license-bsd-2-clause
//
#include <time.h>

#include "sx/rng.h"
#include "sx/hash.h"


// This implementation is taken from: https://github.com/mattiasgustavsson/libs/blob/master/rnd.h
// With a minor optimization in sx_rng_gen_irange

// Convert a randomized uint32_t value to a float value x in the range 0.0f <= x < 1.0f. Contributed by Jonatan Hedborg
SX_PRAGMA_DIAGNOSTIC_PUSH()
SX_PRAGMA_DIAGNOSTIC_IGNORED_CLANG_GCC("-Wstrict-aliasing")
static inline float sx__rng_float_normalized(uint32_t value)
{
    uint32_t exponent = 127;
    uint32_t mantissa = value >> 9;
    uint32_t result = (exponent << 23) | mantissa;
    float fresult = *(float*)(&result);
    return fresult - 1.0f;
}
SX_PRAGMA_DIAGNOSTIC_POP()

static inline uint64_t sx__rng_avalanche64(uint64_t h)
{
    h ^= h >> 33;
    h *= 0xff51afd7ed558ccd;
    h ^= h >> 33;
    h *= 0xc4ceb9fe1a85ec53;
    h ^= h >> 33;
    return h;
}

void sx_rng_seed(sx_rng* rng, uint32_t seed)
{
    uint64_t value = (((uint64_t)seed) << 1ull) | 1ull;    // make it odd
    value = sx__rng_avalanche64(value);
    rng->state[0] = 0ull;
    rng->state[1] = (value << 1ull) | 1ull;
    sx_rng_gen(rng);
    rng->state[0] += sx__rng_avalanche64(value);
    sx_rng_gen(rng);
}

void sx_rng_seed_time(sx_rng* rng)
{
    sx_rng_seed(rng, sx_hash_u64_to_u32(time(NULL)));
}

uint32_t sx_rng_gen(sx_rng* rng)
{
    uint64_t oldstate = rng->state[0];
    rng->state[0] = oldstate * 0x5851f42d4c957f2dull + rng->state[1];
    uint32_t xorshifted = (uint32_t)(((oldstate >> 18ull) ^ oldstate) >> 27ull);
    uint32_t rot = (uint32_t)(oldstate >> 59ull);
    return (xorshifted >> rot) | (xorshifted << ((-(int)rot) & 31));
}

float sx_rng_genf(sx_rng* rng)
{
    return sx__rng_float_normalized(sx_rng_gen(rng));
}



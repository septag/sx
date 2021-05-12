//
// Copyright 2020 Sepehr Taghdisian (septag@github). All rights reserved.
// License: https://github.com/septag/sx#license-bsd-2-clause
//
// bitarray.h - utility data structure to hold arbitary number of bits

#pragma once

#include "allocator.h"

typedef struct sx_bitarray {
    uint64_t* bits;
    int num_bits;
} sx_bitarray;

SX_INLINE bool sx_bitarray_init(sx_bitarray* bar, void* data, int data_size, bool init_val)
{
    if (data_size % sizeof(uint64_t) != 0) {
        sx_assertf(0, "data_size must be a multiple of 8: sizeof(uint64)");
        return false;
    }
    sx_memset(data, init_val ? 0xff : 0x0, data_size);
    bar->bits = (uint64_t*)data;
    bar->num_bits = 64 * (data_size / sizeof(uint64_t));
    return true;
}

SX_INLINE sx_bitarray* sx_bitarray_create(const sx_alloc* alloc, int num_bits, bool init_val)
{
    int aligned_num = sx_align_mask(num_bits, 0x3f);
    size_t total_sz = sizeof(sx_bitarray) + (aligned_num/64)*sizeof(uint64_t);
    sx_bitarray* bar = (sx_bitarray*)sx_malloc(alloc, total_sz);
    if (!bar) {
        sx_out_of_memory();
        return NULL;
    }
    sx_memset(bar, init_val ? 0xff : 0x0, total_sz);
    bar->bits = (uint64_t*)(bar + 1);
    bar->num_bits = num_bits;
    return bar;
}

SX_INLINE void sx_bitarray_destroy(sx_bitarray* bar, const sx_alloc* alloc)
{
    sx_free(alloc, bar);
}

SX_INLINE void sx_bitarray_on(sx_bitarray* bar, int index)
{
    sx_assert(index < bar->num_bits);
    bar->bits[index / 64] |= 1ull << (index & 0x3f);
}

SX_INLINE void sx_bitarray_off(sx_bitarray* bar, int index)
{
    sx_assert(index < bar->num_bits);
    bar->bits[index / 64] &= ~(1ull << (index & 0x3f));
}

SX_INLINE void sx_bitarray_set(sx_bitarray* bar, int index, bool value)
{
    if (value) {
        sx_bitarray_on(bar, index);
    } else {
        sx_bitarray_off(bar, index);
    }
}

SX_INLINE bool sx_bitarray_get(sx_bitarray* bar, int index)
{
    sx_assert(index < bar->num_bits);
    return (bar->bits[index / 64] & (1ull << (index & 0x3f))) != 0;
}

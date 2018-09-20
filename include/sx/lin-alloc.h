//
// Copyright 2018 Sepehr Taghdisian (septag@github). All rights reserved.
// License: https://github.com/septag/sx#license-bsd-2-clause
//
// lin-alloc.h - v1.0 - Generic linear allocator
// 
// sx_linalloc allocates memory from a pre-allocated buffer in a linear fashion
// Pros: Fastest allocator, very low fragmentation
// Cons: Does not have Free operation, only grows
//
// Usage:
//      struct sx_linalloc alloc;
//      sx_linalloc_init(&alloc, preallocated_mem, preallocated_mem_size);
//      ...
//      Pass 'alloc.alloc' to anywhere you want and use sx_alloc macros
//      sx_linalloc_reset(&alloc);  // resets the buffer to zero
//      alloc.peak -> check the maximum growed size
//
// NOTE: realloc functionally of this allocator is stack based
//       so when we do 'realloc' on the same pointer sequentially, the buffer will just grow and no new allocations is made
//       BUT there is a point here for alignment, you should not realloc the same pointer with different alignments on each call
//      
#pragma once

#ifndef SX_LIN_ALLOC_H_
#define SX_LIN_ALLOC_H_

#include "allocator.h"

typedef struct sx_linalloc
{
    sx_alloc alloc;
    uint8_t* ptr;
    uint8_t* last_ptr; 
    int offset;
    int size;
    int peak;
} sx_linalloc;

SX_API void sx_linalloc_init(sx_linalloc* linalloc, void* ptr, int size);
SX_API void sx_linalloc_reset(sx_linalloc* linalloc);

#define sx_define_linalloc_onstack(_name, _size)  \
    uint8_t _name ## _buff_[(_size)];             \
    sx_linalloc _name;                            \
    sx_linalloc_init(&(_name), _name ## _buff_, (_size))

#endif // SX_LIN_ALLOC_H_
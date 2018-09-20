//
// Copyright 2018 Sepehr Taghdisian (septag@github). All rights reserved.
// License: https://github.com/septag/sx#license-bsd-2-clause
//
// stack-alloc: Generic stack allocator
// 
// sx_stackalloc allocates memory from a pre-allocated buffer in a linear fashion
// The difference between stack-alloc and lin-alloc is that stack-alloc have free operation but
// with limitations (see below)
// Suitable for operations that needs loading/unloading of temp memory blocks
//
// Pros: Very Fast allocator, very low fragmentation
// Cons: You have to 'free' in the reverse order of your 'alloc' calls, this is pretty important
//       limitation and will corrupt memory if not done properly. in debug mode it breaks the program
//
// Usage:
//      struct sx_stackalloc salloc;
//      sx_stackalloc_init(&salloc, preallocated_mem, preallocated_mem_size);
//      ...
//      void* p1 = sx_malloc(&salloc.alloc, 18);
//      void* p2 = sx_malloc(&salloc.alloc, 18);
//      ...
//      sx_free(&salloc.alloc, p2);
//      sx_free(&salloc.alloc, p1);
//      alloc.peak -> check the maximum growed size
//
// NOTE: realloc functionally of this allocator is stack based
//       so when we do 'realloc' on the same pointer sequentially, the buffer will just grow and no new allocations is made
//       BUT there is a point here for alignment, you should not realloc the same pointer with different alignments on each call
//      

#pragma once

#ifndef SX_STACK_ALLOC_H_
#define SX_STACK_ALLOC_H_

#include "allocator.h"

typedef struct sx_stackalloc
{
    sx_alloc alloc;
    uint8_t* ptr;
    uint8_t* last_ptr;
    int offset;
    int size;
    int peak;
} sx_stackalloc;

SX_API void sx_stackalloc_init(sx_stackalloc* linalloc, void* ptr, int size);

#define sx_define_stackalloc_onstack(_name, _size)  \
    uint8_t _name ## _buff_[(_size)];  \
    sx_stackalloc _name; \
    sx_stackalloc_init(&(_name), _name ## _buff_, (_size))

#endif  // SX_STACK_ALLOC_H_

//
// Copyright 2018 Sepehr Taghdisian (septag@github). All rights reserved.
// License: https://github.com/septag/sx#license-bsd-2-clause
//
// lin-alloc.h - v1.1 - Generic linear allocator
//
// sx_linalloc: allocates memory from a pre-allocated buffer in a linear fashion
//              Pros: Fastest allocator, almost zero fragmentation
//              Cons: Does not have Free operation, only grows
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
//       so when we do 'realloc' on the same pointer sequentially, the buffer will just grow and no
//       new allocations is made
//       NOTE: there is a point here for alignment, you should not realloc the same pointer with
//       different alignments on each call
// 
// sx_linalloc_growable: same functionality as sx_linalloc, but can grow it's memory based on the given allocator at init
//                       creates new pages with size equal to the 'size' argument given at init upon growing
//
#pragma once

#include "allocator.h"

typedef struct sx_linalloc 
{
    sx_alloc alloc;
    uint8_t* ptr;
    size_t last_ptr_offset;
    size_t offset;
    size_t size;
    size_t peak;
} sx_linalloc;

SX_API void sx_linalloc_init(sx_linalloc* linalloc, void* ptr, size_t size);
SX_API void sx_linalloc_reset(sx_linalloc* linalloc);

#define sx_define_linalloc_onstack(_name, _size) \
    uint8_t _name##_buff_[(_size)];              \
    sx_linalloc _name;                           \
    sx_linalloc_init(&(_name), _name##_buff_, (_size))

typedef struct sx_linalloc_growable_page
{
    uint8_t* ptr;
    size_t size;
    size_t last_ptr_offset;
    size_t offset;
    struct sx_linalloc_growable_page* next;
} sx_linalloc_growable_page;

typedef struct sx_linalloc_growable 
{
    const sx_alloc* main_alloc;
    size_t page_size;
    size_t allocated_size;
    sx_alloc alloc;
    size_t peak;
    sx_linalloc_growable_page* pages;
} sx_linalloc_growable;

SX_API sx_linalloc_growable* sx_linalloc_growable_create(const sx_alloc* alloc, size_t size);
SX_API void sx_linalloc_growable_destroy(sx_linalloc_growable* linalloc);
SX_API void sx_linalloc_growable_reset(sx_linalloc_growable* linalloc);


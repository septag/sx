//
// Copyright 2018 Sepehr Taghdisian (septag@github). All rights reserved.
// License: https://github.com/septag/sx#license-bsd-2-clause
//
// virtual-alloc.h - v1.0 - Virtual memory allocator
//
// Uses OS specific virtual memory functions
//
// virtualalloc: standard sequential virtual memory allocator 
//               Virtual memory can be reserved in large sizes and commited later on demand
//               Very useful for large buffer allocations, instead of allocating directly from heap
//               Memory is only allocate sequentially and will not be reused
//               so the limitation is not to hit the 'reserve_sz' max
//
//      sx_virtualalloc_init        Initialize and reserves virtual memory for the allocator
//                                  Reserved size will not be commited until it's alloacated and touched
//                                  'reserved_sz' value will grow to OS-specific aligned page size
//
//      sx_virtualalloc_release     Releases memory commited and reserved by virtual-allocator
//                                  
// NOTE: When you use the sx_alloc object for allocating, the memory will be commited and you will get a valid pointer
//       But still the memory is not allocated from heap until you touch the contents of the memory block
//       Also the allocation sizes will grow to OS-specific aligned page size (for example 4k), 
//       .. so try to allocate in larger chunks from this type of allocator
//
#pragma once

#ifndef SX_VIRTUAL_ALLOC_H_
#define SX_VISTUAL_ALLOC_H_

#include "allocator.h"

typedef struct sx_virtualalloc
{
    sx_alloc alloc;
    uint8_t* ptr;               // Pointer to reserved memory address
    size_t   offset;
    size_t   reserved_sz;       // Total memory that is reserved (mapped) on init
} sx_virtualalloc;

#ifdef __cplusplus
extern "C" {
#endif

bool sx_virtualalloc_init(sx_virtualalloc* valloc, size_t reserve_sz);
void sx_virtualalloc_release(sx_virtualalloc* valloc);

// TODO: low-level virtual memory api
void* sx_virtual_reserve(size_t reserve_sz);
void  sx_virtual_release(void* ptr);
void  sx_virtual_protect(void* ptr, size_t sz);
void* sx_virtual_commit(void* addr, size_t sz);
void  sx_virtual_decommit(void* addr, size_t sz); 

#ifdef __cplusplus
}
#endif

#endif  // SX_VIRTUAL_ALLOC_H_
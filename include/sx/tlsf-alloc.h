//
// Copyright 2018 Sepehr Taghdisian (septag@github). All rights reserved.
// License: https://github.com/septag/sx#license-bsd-2-clause
//
// tlsf-alloc.h - v1.0 - Two-Level Segregated Fit memory allocator implementation
// Reference: http://www.gii.upv.es/tlsf/main/docs
// Implementation by Matthew Conte: https://github.com/mattconte/tlsf
//
// generic allocator suitable for allocating/deallocating memory blocks in various sizes, with constant time malloc and free O(1)
// This is a wrapper for tlsf implementation around sx_alloc object
//      sx_alloc::user_data is actually tlsf_t handle
//
// Usage:
//      sx_alloc tlsf;
//      char buff[64*1024];
//      sx_tlsfalloc_init(&tlsf, buff, sizeof(buff));   // Create with one 64k pool on stack
//      do_something_with_alloc(&tlsf);
//      sx_tlsfalloc_release(&tlsf);
//
#pragma once

#ifndef SX_TLSF_ALLOC_H_
#define SX_TLSF_ALLOC_H_

#include "allocator.h"

typedef void* sx_tlsf_pool_t;

#ifdef __cplusplus
extern "C" {
#endif

bool sx_tlsfalloc_init(sx_alloc* tlsf, void* mem, int size);
void sx_tlsfalloc_release(sx_alloc* tlsf);

// Pool management, additional pools
sx_tlsf_pool_t sx_tlsfalloc_add_pool(sx_alloc* tlsf, void* mem, int size);
void           sx_tlsfalloc_remove_pool(sx_alloc* tlsf, sx_tlsf_pool_t pool);
sx_tlsf_pool_t sx_tlsfalloc_get_pool(sx_alloc* tlsf);

// Debug
typedef void (sx_tlsfalloc_walk_cb)(void* ptr, size_t size, int used, void* user);
void sx_tlsfalloc_walk_pool(sx_tlsf_pool_t pool, sx_tlsfalloc_walk_cb* walker, void* user);
bool sx_tlsfalloc_check(sx_alloc* tlsf);
bool sx_tlsfalloc_check_pool(sx_tlsf_pool_t pool);

#ifdef __cplusplus
}
#endif

#endif // SX_TLSF_ALLOC_H_
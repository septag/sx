//
// Copyright 2018 Sepehr Taghdisian (septag@github). All rights reserved.
// License: https://github.com/septag/sx#license-bsd-2-clause
//
// bheap.h - v1.0 - Binary heap implmenetation
//                  I discovered this algorithm when reading Randy Paul's [blog](http://www.randygaul.net/)
//                  Which is a partially sorted binary tree and is used for pulling up minimum or maximum values at a time
// Reference: https://en.wikipedia.org/wiki/Binary_heap
//
//  API:
//      sx_bheap_create     create binary heap object with a fixed capacity
//      sx_bheap_destroy    destroy binary heap object
//      sx_bheap_push_min   Push MIN value to binary heap, 'user' is a user-defined pointer
//      sx_bheap_push_max   Push MAX value to binary heap
//      sx_bheap_pop_min    Pop MIN value from binary heap
//      sx_bheap_pop_max    Pop MAX value from binary heap
//      sx_bheap_clear      Clears the binary heap
//      sx_bheap_empty      Returns true if binary heap is empty
//
//  NOTE: Only use either XXX_min or XXX_max APIs for a binary heap, don't mix them
//        use sx_bheap_push_max with sx_bheap_pop_max, or sx_bheap_push_min with sx_bheap_pop_max
//
#pragma once

#ifndef SX_BHEAP_H_
#define SX_BHEAP_H_

#include "sx/allocator.h"

typedef struct sx_bheap_item
{
    int     key;
    void*   user;
} sx_bheap_item;

typedef struct sx_bheap
{
    sx_bheap_item* items;
    int            count;
    int            capacity;
} sx_bheap;

#ifdef __cplusplus
extern "C" {
#endif

sx_bheap*     sx_bheap_create(const sx_alloc* alloc, int capacity);
void          sx_bheap_destroy(sx_bheap* bh, const sx_alloc* alloc);

void          sx_bheap_push_min(sx_bheap* bh, int key, void* user);
sx_bheap_item sx_bheap_pop_min(sx_bheap* bh);

void          sx_bheap_push_max(sx_bheap* bh, int key, void* user);
sx_bheap_item sx_bheap_pop_max(sx_bheap* bh);

void          sx_bheap_clear(sx_bheap* bh);
bool          sx_bheap_empty(sx_bheap* bh);

#ifdef __cplusplus
}
#endif

#endif // SX_BHEAP_H_
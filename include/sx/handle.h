//
// Copyright 2018 Sepehr Taghdisian (septag@github). All rights reserved.
// License: https://github.com/septag/sx#license-bsd-2-clause
//
// handle.h - v1.0 - Handle opaque type (uint16_t) and handle pool
// Reference: https://github.com/bkaradzic/bx/blob/master/include/bx/handlealloc.h
//
//  sx_handle_pool: Reserves handles and manages them (new/delete)
//                  Data can be any array, event multiple pre-allocated arrays. Handles will directly index to them
//          sx_handle_create_pool       allocate and create handle pool with the specified capacity (must be less than UIN16_MAX)
//          sx_handle_destroy_pool      destroy handle pool
//          sx_handle_new               returns a new handle
//                                      returns SX_INVALID_HANDLE with (sx_assert in debug), if pool is full
//          sx_handle_del               deletes the handle and puts it back to the pool
//          sx_handle_at                converts pool index to a valid handle
//          sx_handle_valid             checks if handle is valid
//          sx_handle_full              checks if handle-pool is full
//          sx_handle_grow_pool         doubles the pool capacity
//                                      what this function does is destroying the previous pool and create a new one
//                                      it also copies previous pool's data to the new one
//
//  CAUTION: In case you have to grow the handle-pool, make sure NOT to have multiple pointers to the pool object.
//           Because on grow, it may change the pointer to the handle_pool itself and other references may become invalid
//
//  Example:
//      obj_t* myobjs = new obj_t[1000];
//      sx_handle_pool* handles = sx_handle_create_pool(alloc, 1000);
//      sx_handle_t handle = sx_handle_new(handles);
//      ...
//  Fetch object by handle, simple array read:
//      obj_t* obj = &myobjs[handle];
//      // use ctor caching to init object only once (optional)
//      if (!obj->init) {
//          obj_init(obj);
//          obj->init = true;
//      }
//      ...
//  Iterate through valid handles:
//      for (int i = 0; i < handles->count; i++) {
//          sx_handle_t handle = sx_handle_at(handles, i);
//          obj_t* obj = &myobjs[handle];
//          ...
//      }
//      
//      sx_handle_destroy_poll(handles);
//      delete[] myobjs;
//
#pragma once

#ifndef SX_HANDLE_H_
#define SX_HANDLE_H_

#include "allocator.h"

typedef uint16_t sx_handle_t;

#define SX_INVALID_HANDLE UINT16_MAX;

typedef struct sx_handle_pool_s
{
    int count;
    int capacity;
    sx_handle_t* dense;
    sx_handle_t* sparse;
} sx_handle_pool;

SX_API sx_handle_pool* sx_handle_create_pool(const sx_alloc* alloc, int capacity);
SX_API void sx_handle_destroy_pool(sx_handle_pool* pool, const sx_alloc* alloc);
SX_API bool sx_handle_grow_pool(SX_INOUT sx_handle_pool** ppool, const sx_alloc* alloc);

SX_INLINE sx_handle_t sx_handle_new(sx_handle_pool* pool)
{
    if (pool->count < pool->capacity) {
        sx_handle_t index = pool->count;
        ++pool->count;
        sx_handle_t handle = pool->dense[index];
        pool->sparse[handle] = index;
        return handle;
    } else  {
        sx_assert(0 && "handle pool is full");
    }

    return SX_INVALID_HANDLE;
}

SX_INLINE void sx_handle_del(sx_handle_pool* pool, sx_handle_t handle)
{
    sx_assert(pool->count > 0);

    sx_handle_t index = pool->sparse[handle];
    --pool->count;
    sx_handle_t tmp = pool->dense[pool->count];
    pool->dense[pool->count] = handle;
    pool->sparse[tmp] = index;
    pool->dense[index] = tmp; 
}

SX_INLINE void sx_handle_reset_pool(sx_handle_pool* pool)
{
    pool->count = 0;
    sx_handle_t* dense = pool->dense;
    for (int i = 0, c = pool->capacity; i < c; i++) {
        dense[i] = i;
    }
}

SX_INLINE bool sx_handle_valid(const sx_handle_pool* pool, sx_handle_t handle)
{
    sx_handle_t index = pool->sparse[handle];
    return index < pool->count && pool->dense[index] == handle;
}

SX_INLINE sx_handle_t sx_handle_at(const sx_handle_pool* pool, int index)
{
    sx_assert(index < pool->count);
    return pool->dense[index];
}

SX_INLINE bool sx_handle_full(const sx_handle_pool* pool)
{
    return pool->count == pool->capacity;
}

#endif  // SX_HANDLE_H_
//
// Copyright 2018 Sepehr Taghdisian (septag@github). All rights reserved.
// License: https://github.com/septag/sx#license-bsd-2-clause
//
// parts of this code is copied from bx library: https://github.com/bkaradzic/bx
// Copyright 2011-2019 Branimir Karadzic. All rights reserved.
// License: https://github.com/bkaradzic/bx#license-bsd-2-clause
//
// handle.h - v1.0 - Handle opaque type (uint32_t) and handle pool
//                   useful for exposing internal objects instead of pointers,
//                   with the ability to add/remove and iterate
//                   If remove is not needed, it is recommended to just use plain arrays
//                   handle-pool uses generation counters to validate dead handles
//                   This is actually a combination of the following methods/implmentations
// Reference: http://bitsquid.blogspot.com/2011/09/managing-decoupling-part-4-id-lookup.html
// Reference: https://github.com/bkaradzic/bx/blob/master/include/bx/handlealloc.h
// Reference: https://floooh.github.io/2018/06/17/handles-vs-pointers.html
//
//
//  sx_handle_pool: Reserves handles and manages them (new/delete)
//                  Data can be any array, event multiple pre-allocated arrays.
//                  Handles will directly index to them
//          sx_handle_create_pool       allocate and create handle pool with the specified capacity
//                                      (must be less than UIN16_MAX)
//          sx_handle_destroy_pool      destroy handle pool
//          sx_handle_new               returns a new handle
//                                      returns 0 with (sx_assert in debug), if pool is full is full
//          sx_handle_del               deletes the handle and puts it back to the pool
//          sx_handle_at                converts pool index to a valid handle
//          sx_handle_valid             checks if handle is valid by comparing generation
//          sx_handle_full              checks if handle-pool is full
//          sx_handle_grow_pool         doubles the pool capacity
//                                      what this function does is destroying the previous pool and
//                                      create a new one it also copies previous pool's data to the
//                                      new one
//          sx_handle_index             use this macro to fetch index to the actual array from the
//                                      handle
//          sx_handle_gen               use this macro to fetch generation from the handle. Mainly
//                                      for debugging purposes
//
//  CAUTION: In case you have to grow the handle-pool, make sure NOT to have multiple pointers
//           to the pool object.
//           Because on grow, it may change the pointer to the handle_pool itself and
//           other references may become invalid
//
//  Example:
//      obj_t* myobjs = new obj_t[1000];
//      sx_handle_pool* handles = sx_handle_create_pool(alloc, 1000);
//      sx_handle_t handle = sx_handle_new(handles);
//      ...
//  Fetch object by handle:
//      obj_t* obj = &myobjs[sx_handle_index(handle)];
//      // use ctor caching to init object only once (optional)
//      if (!obj->init) {
//          obj_init(obj);
//          obj->init = true;
//      }
//      ...
//  Iterate through valid handles:
//      for (int i = 0; i < handles->count; i++) {
//          sx_handle_t handle = sx_handle_at(handles, i);
//          obj_t* obj = &myobjs[sx_handle_index(handle)];
//          ...
//      }
//
//      sx_handle_destroy_pool(handles);
//      delete[] myobjs;
//
#pragma once

#include "sx.h"

typedef struct sx_alloc sx_alloc;

typedef uint32_t sx_handle_t;

#define SX_INVALID_HANDLE 0

typedef struct sx_handle_pool {
    int          count;
    int          capacity;
    sx_handle_t* dense;     // [0..count] saves actual handles
    int*         sparse;    // [0..capacity] saves indexes-to-dense for removal lookup
} sx_handle_pool;

SX_API sx_handle_pool* sx_handle_create_pool(const sx_alloc* alloc, int capacity);
SX_API void            sx_handle_destroy_pool(sx_handle_pool* pool, const sx_alloc* alloc);
SX_API bool            sx_handle_grow_pool(sx_handle_pool** ppool, const sx_alloc* alloc);

SX_API const uint32_t k__handle_index_mask;
SX_API const uint32_t k__handle_gen_mask;
SX_API const uint32_t k__handle_gen_shift;

#define sx_handle_index(_h) (int)((_h)&k__handle_index_mask)
#define sx_handle_gen(_h) (int)(((_h) >> SX_CONFIG_HANDLE_GEN_BITS) & k__handle_gen_shift)
#define sx__handle_make(_g, _idx)                                             \
    (uint32_t)((((uint32_t)(_g)&k__handle_gen_mask) << k__handle_gen_shift) | \
               ((uint32_t)(_idx)&k__handle_index_mask))

static inline sx_handle_t sx_handle_new(sx_handle_pool* pool) {
    if (pool->count < pool->capacity) {
        int         index = pool->count++;
        sx_handle_t handle = pool->dense[index];

        // increase generation
        int         gen = sx_handle_gen(handle);
        int         _index = sx_handle_index(handle);
        sx_handle_t new_handle = sx__handle_make(++gen, _index);

        pool->dense[index] = new_handle;
        pool->sparse[_index] = index;
        return new_handle;
    } else {
        sx_assert(0 && "handle pool is full");
    }

    return SX_INVALID_HANDLE;
}

static inline void sx_handle_del(sx_handle_pool* pool, sx_handle_t handle) {
    sx_assert(pool->count > 0);

    int         index = pool->sparse[sx_handle_index(handle)];
    sx_handle_t last_handle = pool->dense[--pool->count];
    pool->dense[pool->count] = handle;
    pool->sparse[sx_handle_index(last_handle)] = index;
    pool->dense[index] = last_handle;
}

static inline void sx_handle_reset_pool(sx_handle_pool* pool) {
    pool->count = 0;
    sx_handle_t* dense = pool->dense;
    for (int i = 0, c = pool->capacity; i < c; i++) {
        dense[i] = sx__handle_make(0, i);
    }
}

static inline bool sx_handle_valid(const sx_handle_pool* pool, sx_handle_t handle) {
    sx_assert(handle);
    int index = pool->sparse[sx_handle_index(handle)];
    return index < pool->count && pool->dense[index] == handle;
}

static inline sx_handle_t sx_handle_at(const sx_handle_pool* pool, int index) {
    sx_assert(index < pool->count);
    return pool->dense[index];
}

static inline bool sx_handle_full(const sx_handle_pool* pool) {
    return pool->count == pool->capacity;
}

#define sx_handle_new_and_grow(_pool, _alloc) \
    (sx_handle_full(_pool) ? sx_handle_grow_pool(&(_pool), _alloc) : 0, sx_handle_new(_pool))

//
// Copyright 2018 Sepehr Taghdisian (septag@github). All rights reserved.
// License: https://github.com/septag/sx#license-bsd-2-clause
//
// pool.h - v1.0 - Object Pool container, new/deletes pointer to fixed size objects
//      sx_pool_create          Allocates object pool with objects of 'item_sz' size and capacity
//                              also aligns the capacity count to 16
//      sx_pool_destroy         Destroys the pool
//      sx_pool_new             Fetches a new memory pointer to an object
//      sx_pool_del             Puts the data pointer back into pool
//      sx_pool_valid           Checks if the object pointer is allocated from the pool
//
// This is a fixed capacity pool that is mainly needed in code, it's not recommended to assume unlimited growable pools
// But just in case we needed that kind of data, we can wrap this in another API and create/link-list multiple pools
//
// TIP: You can use ctor caching pattern on pools, so some data of each object is initialized once
//      obj_t* obj = sx_pool_new(pool);
//      if (!obj->is_init) {
//          obj_init(obj);
//          obj->is_init = true;
//      }
//      sx_pool_del(pool, obj);
//      obj_destroy(obj);
//
#pragma once

#ifndef SX_POOL_H_
#define SX_POOL_H_

#include "allocator.h"

typedef SX_ALIGN_DECL(16, struct) sx_pool
{
    int      iter;
    int      item_sz;
    int      capacity;
    int      _pad1;    
    void**   ptrs;
    uint8_t* buff;
} sx_pool;

SX_INLINE sx_pool* sx_pool_create(const sx_alloc* alloc, int item_sz, int capacity)
{
    sx_assert(item_sz > 0 && "Item size should not be zero");

    capacity = sx_align_mask(capacity, 15);
    uint8_t* buff = (uint8_t*)sx_aligned_malloc(alloc, 
            sizeof(sx_pool) + (item_sz + sizeof(void*))*capacity, 16);
    if (!buff) {
        sx_out_of_memory();
        return NULL;
    }

    sx_pool* pool = (sx_pool*)buff;
    pool->iter = capacity;
    pool->item_sz = item_sz;
    pool->capacity = capacity;

    buff += sizeof(sx_pool);
    pool->ptrs = (void**)buff;
    buff += sizeof(void*)*capacity;
    pool->buff = buff;
    buff += item_sz*capacity;

    for (int i = 0; i < capacity; i++) {
        pool->ptrs[capacity - i - 1] = pool->buff + i*item_sz;
    }

    return pool;
}

SX_INLINE void sx_pool_destroy(sx_pool* pool, const sx_alloc* alloc)
{
    sx_assert(pool);
    pool->capacity = pool->iter = 0;
    sx_aligned_free(alloc, pool, 16);
}

SX_INLINE void* sx_pool_new(sx_pool* pool)
{
    if (pool->iter > 0) {
        return pool->ptrs[--pool->iter];
    } else {
        sx_assert(0 && "Capacity is full");
        return NULL;
    }
}

SX_INLINE bool sx_pool_full(const sx_pool* pool)
{
    return pool->iter == 0;
}

SX_INLINE bool sx_pool_valid(const sx_pool* pool, void* ptr)
{
    uintptr_t uptr = (uintptr_t)ptr;
    bool inbuf = uptr >= (uintptr_t)pool->buff && 
                 uptr < (uintptr_t)(pool->buff + pool->capacity*pool->item_sz);
    bool valid = (uintptr_t)((uint8_t*)ptr - pool->buff) % pool->item_sz == 0;
    return inbuf & valid;
}

SX_INLINE void sx_pool_del(sx_pool* pool, void* ptr)
{
    sx_assert(pool->iter != pool->capacity && "Cannot delete more objects");
    sx_assert(sx_pool_valid(pool, ptr) && "Pointer does not belong to pool");
    pool->ptrs[pool->iter++] = ptr;
}


#endif  // SX_POOL_H_
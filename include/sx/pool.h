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

typedef SX_ALIGN_DECL(16, struct) sx_pool_s
{
    int iter;
    int item_sz;
    int capacity;
    int _pad1;    
    void** ptrs;
    uint8_t* buff;
} sx_pool;

sx_pool* sx_pool_create(const sx_alloc* alloc, int item_sz, int capacity);
void  sx_pool_destroy(sx_pool* pool, const sx_alloc* alloc);
void* sx_pool_new(sx_pool* pool);
void  sx_pool_del(sx_pool* pool, void* ptr);
bool  sx_pool_valid(const sx_pool* pool, void* ptr);

#include "inline/pool.inl"

#endif  // SX_POOL_H_
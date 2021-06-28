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
//      sx_pool_valid_ptr           Checks if the object pointer is allocated from the pool
//
// Note: memory will be zero'd on creation, so every object that you instanciate from the pool will 
//       only be all-zero for the first instance, so you have to manage initialization for object by yourself
//       see the example in the tip below
//
// TIP: You can use ctor caching pattern on pools, so some data of each object is initialized once
//      typedef struct obj_t {
//          bool is_init;       // will be set to zero at pool creation    
//          uint32_t* buffer;   // whatever data
//      } obj_t;
//
//      obj_t* obj = sx_pool_new(pool);
//      if (!obj->is_init) {
//          obj_init(obj);
//          obj->is_init = true;
//      }
//      sx_pool_del(pool, obj);
//      obj_destroy(obj);
//
#pragma once

#include "allocator.h"

typedef struct sx_alloc sx_alloc;

typedef struct sx__pool_page
{
    void** ptrs;
    uint8_t* buff;
    struct sx__pool_page* next;
    int iter;
} sx__pool_page;

typedef struct sx_pool
{
    int item_sz;
    int capacity;
    sx__pool_page* pages;
} sx_pool;

SX_INLINE sx__pool_page* sx__pool_create_page(sx_pool* pool, const sx_alloc* alloc, const char* file, const char* func,
                                              uint32_t line)
{
    int capacity = pool->capacity;
    int item_sz = pool->item_sz;

    uint8_t* buff = (uint8_t*)sx__malloc(alloc, 
                                         sizeof(sx__pool_page) + (item_sz + sizeof(void*)) * capacity, 
                                         16, file, func, line);
    if (!buff) {
        sx_out_of_memory();
        return NULL;
    }

    sx__pool_page* page = (sx__pool_page*)buff;
    buff += sizeof(sx__pool_page);
    page->iter = capacity;
    page->ptrs = (void**)buff;
    buff += sizeof(void*) * capacity;
    page->buff = buff;
    page->next = NULL;
    for (int i = 0; i < capacity; i++) {
        page->ptrs[capacity - i - 1] = page->buff + (size_t)i * (size_t)item_sz;
    }
    sx_memset(page->buff, 0x0, capacity * item_sz);

    return page;
}

SX_INLINE sx_pool* sx__pool_create(const sx_alloc* alloc, int item_sz, int capacity, 
                                   const char* file, const char* func, uint32_t line)
{
    sx_assertf(item_sz > 0, "Item size should not be zero");

    capacity = sx_align_mask(capacity, 15);
    uint8_t* buff = (uint8_t*)sx__malloc(
        alloc, sizeof(sx_pool) + sizeof(sx__pool_page) + (item_sz + sizeof(void*)) * capacity, 16, 
        file, func, line);
    if (!buff) {
        sx_out_of_memory();
        return NULL;
    }

    sx_pool* pool = (sx_pool*)buff;
    buff += sizeof(sx_pool);
    pool->item_sz = item_sz;
    pool->capacity = capacity;
    pool->pages = (sx__pool_page*)buff;
    buff += sizeof(sx__pool_page);

    sx__pool_page* page = pool->pages;
    page->iter = capacity;
    page->ptrs = (void**)buff;
    buff += sizeof(void*) * capacity;
    page->buff = buff;
    page->next = NULL;
    for (int i = 0; i < capacity; i++) {
        page->ptrs[capacity - i - 1] = page->buff + (size_t)i * (size_t)item_sz;
    }
    sx_memset(page->buff, 0x0, capacity * item_sz);

    return pool;
}

SX_INLINE void sx_pool_destroy(sx_pool* pool, const sx_alloc* alloc)
{
    if (pool) {
        sx_assert(pool->pages);

        sx__pool_page* page = pool->pages->next;
        while (page) {
            sx__pool_page* next = page->next;
            sx_aligned_free(alloc, page, 16);
            page = next;
        }
        pool->capacity = 0;
        pool->pages->iter = 0;
        pool->pages->next = NULL;
        sx_aligned_free(alloc, pool, 16);
    }
}

SX_INLINE void* sx_pool_new(sx_pool* pool)
{
    sx__pool_page* page = pool->pages;
    while (page->iter == 0 && page->next)
        page = page->next;

    if (page->iter > 0) {
        return page->ptrs[--page->iter];
    } else {
        sx_assertf(0, "capacity is full");
        return NULL;
    }
}

SX_INLINE bool sx_pool_grow(sx_pool* pool, const sx_alloc* alloc, const char* file, const char* func, uint32_t line)
{
    sx__pool_page* page = sx__pool_create_page(pool, alloc, file, func, line);
    if (page) {
        sx__pool_page* last = pool->pages;
        while (last->next) {
            last = last->next;
        }
        last->next = page;
        return true;
    } else {
        return false;
    }
}

SX_INLINE bool sx_pool_full(const sx_pool* pool)
{
    sx__pool_page* page = pool->pages;
    while (page) {
        if (page->iter > 0)
            return false;
        page = page->next;
    }
    return true;
}

// same as sx_pool_full, but check wether we can have N instances
SX_INLINE bool sx_pool_fulln(const sx_pool* pool, int n)
{
    sx__pool_page* page = pool->pages;
    while (page) {
        if ((page->iter - n) >= 0)
            return false;
        page = page->next;
    }
    return true;
}

SX_INLINE bool sx_pool_valid_ptr(const sx_pool* pool, void* ptr)
{
    uintptr_t uptr = (uintptr_t)ptr;
    sx__pool_page* page = pool->pages;
    int item_sz = pool->item_sz;
    int capacity = pool->capacity;
    while (page) {
        bool inbuf = uptr >= (uintptr_t)page->buff &&
                     uptr < (uintptr_t)(page->buff + (size_t)capacity * (size_t)item_sz);
        if (inbuf)
            return (uintptr_t)((uint8_t*)ptr - page->buff) % item_sz == 0;

        page = page->next;
    }
    return false;
}

SX_INLINE void sx_pool_del(sx_pool* pool, void* ptr)
{
    uintptr_t uptr = (uintptr_t)ptr;
    sx__pool_page* page = pool->pages;
    int item_sz = pool->item_sz;
    int capacity = pool->capacity;

    while (page) {
        if (uptr >= (uintptr_t)page->buff &&
            uptr < (uintptr_t)(page->buff + (size_t)capacity * (size_t)item_sz)) {
            sx_assertf((uintptr_t)((uint8_t*)ptr - page->buff) % item_sz == 0,
                      "ptr is not aligned to items, probably invalid");
            sx_assertf(page->iter != capacity,
                      "cannot delete more objects, possible double delete");

            page->ptrs[page->iter++] = ptr;
            return;
        }

        page = page->next;
    }
    sx_assertf(0, "pointer does not blong to the pool");
}

#define sx_pool_create(_alloc, _item_sz, _capacity) \
    sx__pool_create(_alloc, (_item_sz), (_capacity), __FILE__, SX_FUNCTION, __LINE__)

#define sx_pool_new_and_grow(_pool, _alloc) \
    (sx_pool_full(_pool) ? sx_pool_grow(_pool, _alloc, __FILE__, SX_FUNCTION, __LINE__) : 0, sx_pool_new(_pool))

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
// This is a fixed capacity pool that is mainly needed in code, it's not recommended to assume
// unlimited growable pools
// But just in case we needed that kind of data, we can wrap this in another API and
// create/link-list multiple pools
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

#include "sx.h"

typedef struct sx_alloc sx_alloc;

typedef sx_align_decl(16, struct) sx__pool_page {
    void**                ptrs;
    uint8_t*              buff;
    struct sx__pool_page* next;
    int                   iter;
}
sx__pool_page;

typedef sx_align_decl(16, struct) sx_pool {
    int            item_sz;
    int            capacity;
    sx__pool_page* pages;
}
sx_pool;

static inline sx__pool_page* sx__pool_create_page(sx_pool* pool, const sx_alloc* alloc) {
    int capacity = pool->capacity;
    int item_sz = pool->item_sz;

    uint8_t* buff = (uint8_t*)sx_aligned_malloc(
        alloc, sizeof(sx__pool_page) + (item_sz + sizeof(void*)) * capacity, 16);
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
        page->ptrs[capacity - i - 1] = page->buff + i * item_sz;
    }

    return page;
}

static inline sx_pool* sx_pool_create(const sx_alloc* alloc, int item_sz, int capacity) {
    sx_assert(item_sz > 0 && "Item size should not be zero");

    capacity = sx_align_mask(capacity, 15);
    uint8_t* buff = (uint8_t*)sx_aligned_malloc(
        alloc, sizeof(sx_pool) + sizeof(sx__pool_page) + (item_sz + sizeof(void*)) * capacity, 16);
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
        page->ptrs[capacity - i - 1] = page->buff + i * item_sz;
    }

    return pool;
}

static inline void sx_pool_destroy(sx_pool* pool, const sx_alloc* alloc) {
    sx_assert(pool);
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

static inline void* sx_pool_new(sx_pool* pool) {
    sx__pool_page* page = pool->pages;
    while (page->iter == 0 && page->next) page = page->next;

    if (page->iter > 0) {
        return page->ptrs[--page->iter];
    } else {
        sx_assert(0 && "capacity is full");
        return NULL;
    }
}

static inline bool sx_pool_grow(sx_pool* pool, const sx_alloc* alloc) {
    sx__pool_page* page = sx__pool_create_page(pool, alloc);
    if (page) {
        sx__pool_page* last = pool->pages;
        while (last->next) last = last->next;
        last->next = page;
        return true;
    } else {
        return false;
    }
}

static inline bool sx_pool_full(const sx_pool* pool) {
    sx__pool_page* page = pool->pages;
    while (page) {
        if (page->iter > 0)
            return false;
        page = page->next;
    }
    return true;
}

// same as sx_pool_full, but check wether we can have N instances
static inline bool sx_pool_fulln(const sx_pool* pool, int n) {
    sx__pool_page* page = pool->pages;
    while (page) {
        if ((page->iter - n) >= 0)
            return false;
        page = page->next;
    }
    return true;
}

static inline bool sx_pool_valid_ptr(const sx_pool* pool, void* ptr) {
    uintptr_t      uptr = (uintptr_t)ptr;
    sx__pool_page* page = pool->pages;
    int            item_sz = pool->item_sz;
    int            capacity = pool->capacity;
    while (page) {
        bool inbuf =
            uptr >= (uintptr_t)page->buff && uptr < (uintptr_t)(page->buff + capacity * item_sz);
        if (inbuf)
            return (uintptr_t)((uint8_t*)ptr - page->buff) % item_sz == 0;

        page = page->next;
    }
    return false;
}

static inline void sx_pool_del(sx_pool* pool, void* ptr) {
    uintptr_t      uptr = (uintptr_t)ptr;
    sx__pool_page* page = pool->pages;
    int            item_sz = pool->item_sz;
    int            capacity = pool->capacity;

    while (page) {
        if (uptr >= (uintptr_t)page->buff && uptr < (uintptr_t)(page->buff + capacity * item_sz)) {
            sx_assert((uintptr_t)((uint8_t*)ptr - page->buff) % item_sz == 0 &&
                      "ptr is not aligned to items, probably invalid");
            sx_assert(page->iter != capacity &&
                      "cannot delete more objects, possible double delete");

            page->ptrs[page->iter++] = ptr;
            return;
        }

        page = page->next;
    }
    sx_assert(0 && "pointer does not blong to the pool");
}

#define sx_pool_new_and_grow(_pool, _alloc) \
    (sx_pool_full(_pool) ? sx_pool_grow(_pool, _alloc) : 0, sx_pool_new(_pool))

#ifndef SX_POOL_H_
#   error "This file must only be included from pool.h"
#   include "../pool.h"
#endif

//
inline sx_pool* sx_pool_create(const sx_alloc* alloc, int item_sz, int capacity)
{
    assert(item_sz > 0 && "Item size should not be zero");

    capacity = sx_align_mask(capacity, 15);
    uint8_t* buff = (uint8_t*)sx_aligned_alloc(alloc, 
            sizeof(sx_pool) + (item_sz + sizeof(void*))*capacity, 16);
    if (!buff) {
        SX_OUT_OF_MEMORY;
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

inline void sx_pool_destroy(sx_pool* pool, const sx_alloc* alloc)
{
    assert(pool);
    pool->capacity = pool->iter = 0;
    sx_aligned_free(alloc, pool, 16);
}

inline void* sx_pool_new(sx_pool* pool)
{
    if (pool->iter > 0) {
        return pool->ptrs[--pool->iter];
    } else {
        assert(0 && "Capacity is full");
        return NULL;
    }
}

inline void sx_pool_del(sx_pool* pool, void* ptr)
{
    assert(pool->iter != pool->capacity && "Cannot delete more objects");
    assert(sx_pool_valid(pool, ptr) && "Pointer does not belong to pool");
    pool->ptrs[pool->iter++] = ptr;
}

inline bool sx_pool_valid(const sx_pool* pool, void* ptr)
{
    uintptr_t uptr = (uintptr_t)ptr;
    bool inbuf = uptr >= (uintptr_t)pool->buff && 
                 uptr < (uintptr_t)(pool->buff + pool->capacity*pool->item_sz);
    bool valid = (uintptr_t)((uint8_t*)ptr - pool->buff) % pool->item_sz == 0;
    return inbuf & valid;
}



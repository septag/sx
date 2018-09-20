//
// Copyright 2018 Sepehr Taghdisian (septag@github). All rights reserved.
// License: https://github.com/septag/sx#license-bsd-2-clause
//
#include "sx/handle.h"

sx_handle_pool* sx_handle_create_pool(const sx_alloc* alloc, int capacity)
{
    sx_assert(capacity < UINT16_MAX && "Capacity too high");
    // Align count to 16, for a better aligned internal memory
    int maxsz = sx_align_mask(capacity, 15);

    uint8_t* buff = (uint8_t*)sx_malloc(alloc, 
            sizeof(sx_handle_pool) + sizeof(sx_handle_t)*maxsz*2);
    if (!buff)  {
        sx_out_of_memory();
        return NULL;
    }

    sx_handle_pool* pool = (sx_handle_pool*)buff;
    buff += sizeof(sx_handle_pool);
    pool->dense = (sx_handle_t*)buff;
    buff += sizeof(sx_handle_t)*maxsz;
    pool->sparse = (sx_handle_t*)buff;
    pool->capacity = capacity;
    sx_handle_reset_pool(pool);    

    return pool;
}

void sx_handle_destroy_pool(sx_handle_pool* pool, const sx_alloc* alloc)
{
    if (pool) {
        sx_free(alloc, pool);
    }
}

bool sx_handle_grow_pool(sx_handle_pool** ppool, const sx_alloc* alloc)
{
    sx_handle_pool* pool = *ppool;
    int new_cap = pool->capacity << 1;
    
    sx_handle_pool* new_pool = sx_handle_create_pool(alloc, new_cap);
    if (!new_pool)
        return false;
    new_pool->count = pool->count;
    sx_memcpy(new_pool->dense, pool->dense, sizeof(sx_handle_t)*pool->capacity);
    sx_memcpy(new_pool->sparse, pool->sparse, sizeof(sx_handle_t)*pool->capacity);

    sx_handle_t* dense = pool->dense;
    for (int i = pool->capacity; i < new_cap; i++) {
        dense[i] = i;
    }

    sx_handle_destroy_pool(pool, alloc);
    *ppool = new_pool;
    return true;
}

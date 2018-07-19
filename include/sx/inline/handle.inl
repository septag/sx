#ifndef SX_HANDLE_H_
#   error "This file must only be included from handle.h"
#   include "../handle.h"
#endif

inline sx_handle_t sx_handle_new(sx_handle_pool* pool)
{
    if (pool->count < pool->capacity) {
        sx_handle_t index = pool->count;
        ++pool->count;
        sx_handle_t handle = pool->dense[index];
        pool->sparse[handle] = index;
        return handle;
    } else  {
        assert(0 && "handle pool is full");
    }

    return SX_INVALID_HANDLE;
}

inline void sx_handle_del(sx_handle_pool* pool, sx_handle_t handle)
{
    assert(pool->count > 0);

    sx_handle_t index = pool->sparse[handle];
    --pool->count;
    sx_handle_t tmp = pool->dense[pool->count];
    pool->dense[pool->count] = handle;
    pool->sparse[tmp] = index;
    pool->dense[index] = tmp; 
}

inline void sx_handle_reset_pool(sx_handle_pool* pool)
{
    pool->count = 0;
    sx_handle_t* dense = pool->dense;
    for (int i = 0, c = pool->capacity; i < c; i++) {
        dense[i] = i;
    }
}

inline bool sx_handle_valid(const sx_handle_pool* pool, sx_handle_t handle)
{
    sx_handle_t index = pool->sparse[handle];
    return index < pool->count && pool->dense[index] == handle;
}

inline sx_handle_t sx_handle_at(const sx_handle_pool* pool, int index)
{
    assert(index < pool->count);
    return pool->dense[index];
}

inline bool sx_handle_full(const sx_handle_pool* pool)
{
    return pool->count == pool->capacity;
}


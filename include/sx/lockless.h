#pragma once

#include "sx.h"
#include "atomic.h"

typedef struct sx_alloc sx_alloc;

// Basic spinlock implementation
// interesting reads:
// https://software.intel.com/en-us/articles/a-common-construct-to-avoid-the-contention-of-threads-architecture-agnostic-spin-wait-loops
// https://rigtorp.se/spinlock/
//

typedef sx_align_decl(SX_CACHE_LINE_SIZE, sx_atomic_uint32) sx_lock_t;

SX_FORCE_INLINE void sx_lock_enter(sx_lock_t* lock);
SX_FORCE_INLINE void sx_lock_exit(sx_lock_t* lock);
SX_FORCE_INLINE bool sx_lock_try(sx_lock_t* lock);

// Define these two macros to measure the amount of lock time
#ifndef sx_lock_measure_time_begin
#   define sx_lock_measure_time_begin()
#endif

#ifndef sx_lock_measure_time_end
#    define sx_lock_measure_time_end()
#endif

#ifndef sx_lock_hit_contention
#   define sx_lock_hit_contention()
#endif


// These types will be obsolete and replaced by the above code
// So set SX_CONFIG_OBSOLETE_CODE=0 and update your code as soon as you can
#if SX_CONFIG_OBSOLETE_CODE
#    define sx_lock(_l) sx_lock_enter(_l)
#    define sx_unlock(_l) sx_lock_exit(_l)
#    define sx_trylock(_l) sx_lock_try(_l)
#else
#    define sx_lock(_lock) sx_defer(sx_lock_enter(&_lock), sx_lock_exit(&_lock))
#endif

// single-producer / single-consumer
// growable
typedef struct sx_queue_spsc sx_queue_spsc;
SX_API sx_queue_spsc* sx_queue_spsc_create(const sx_alloc* alloc, int item_sz, int capacity);
SX_API void sx_queue_spsc_destroy(sx_queue_spsc* queue, const sx_alloc* alloc);

SX_API bool sx_queue_spsc_produce(sx_queue_spsc* queue, const void* data);
SX_API bool sx_queue_spsc_consume(sx_queue_spsc* queue, void* data);
SX_API bool sx_queue_spsc_grow(sx_queue_spsc* queue, const sx_alloc* alloc);
SX_API bool sx_queue_spsc_full(const sx_queue_spsc* queue);

#define sx_queue_spsc_produce_and_grow(_queue, _data, _alloc)             \
    (sx_queue_spsc_full(_queue) ? sx_queue_spsc_grow(_queue, _alloc) : 0, sx_queue_spsc_produce(_queue, (_data)))

//--------------------------------------------------------------------------------------------------
SX_FORCE_INLINE void sx_lock_enter(sx_lock_t* lock)
{
    sx_lock_measure_time_begin();
    while (1) {
        if (sx_atomic_exchange32_explicit(lock, 1, SX_ATOMIC_MEMORYORDER_ACQUIRE) == 0)
            return;
        while (sx_atomic_load32_explicit(lock, SX_ATOMIC_MEMORYORDER_RELAXED)) {
            sx_relax_cpu();
            sx_lock_hit_contention();
        }
    }
    sx_lock_measure_time_end();
}

SX_FORCE_INLINE void sx_lock_exit(sx_lock_t* lock)
{
    sx_atomic_store32_explicit(lock, 0, SX_ATOMIC_MEMORYORDER_RELEASE);
}

SX_FORCE_INLINE bool sx_lock_try(sx_lock_t* lock)
{
    return sx_atomic_load32_explicit(lock, SX_ATOMIC_MEMORYORDER_RELAXED) == 0 &&
           sx_atomic_exchange32_explicit(lock, 1, SX_ATOMIC_MEMORYORDER_ACQUIRE) == 0;
}


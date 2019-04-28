//
// Copyright 2018 Sepehr Taghdisian (septag@github). All rights reserved.
// License: https://github.com/septag/sx#license-bsd-2-clause
//
// parts of this code is copied from bx library: https://github.com/bkaradzic/bx
// Copyright 2011-2019 Branimir Karadzic. All rights reserved.
// License: https://github.com/bkaradzic/bx#license-bsd-2-clause
//
// threads.h - v1.0 - Common portable multi-threading primitives
//
//      sx_thread       Portable thread
//      sx_tls          Portable thread-local-storage which you can store a user_data per Tls
//      sx_mutex        Portable OS mutex, use for long-time data locks, for short-time locks use
//                      sx_lock_t in atomics.h
//      sx_sem          Portable OS semaphore. 'post' increases the count. 'wait' waits on semaphore
//                      if count is zero,
//                      else decreases the count and continue
//      sx_signal       Portable OS signals/events. simplified version of the semaphore,
//                      where you 'wait' for signal to be triggered, then in another thread you
//                      'raise' it and 'wait' will continue
//      sx_queue_spsc   Single producer/Single consumer self contained queue
//
#pragma once

#include "sx.h"

typedef struct sx_alloc sx_alloc;

// Thread
typedef struct sx__thread_s sx_thread;

// Thread callback function
typedef int(sx_thread_cb)(void* user_data1, void* user_data2);

SX_API sx_thread* sx_thread_create(const sx_alloc* alloc, sx_thread_cb* callback,
                                   void* user_data1 sx_default(NULL), int stack_sz sx_default(0),
                                   const char* name sx_default(NULL),
                                   void* user_data2 sx_default(NULL));
SX_API int        sx_thread_destroy(sx_thread* thrd, const sx_alloc* alloc);
SX_API bool       sx_thread_running(sx_thread* thrd);
SX_API void       sx_thread_setname(sx_thread* thrd, const char* name);
SX_API void       sx_thread_yield();
SX_API uint32_t sx_thread_tid();

// Tls data
typedef void* sx_tls;

SX_API sx_tls sx_tls_create();
SX_API void   sx_tls_destroy(sx_tls tls);
SX_API void   sx_tls_set(sx_tls tls, void* data);
SX_API void*  sx_tls_get(sx_tls tls);

// Mutex
typedef struct sx_mutex_s {
    sx_align_decl(16, uint8_t) data[64];
} sx_mutex;

SX_API void sx_mutex_init(sx_mutex* mutex);
SX_API void sx_mutex_release(sx_mutex* mutex);
SX_API void sx_mutex_lock(sx_mutex* mutex);
SX_API void sx_mutex_unlock(sx_mutex* mutex);
SX_API bool sx_mutex_trylock(sx_mutex* mutex);

// Semaphore
typedef struct sx_sem_s {
    sx_align_decl(16, uint8_t) data[128];
} sx_sem;

SX_API void sx_semaphore_init(sx_sem* sem);
SX_API void sx_semaphore_release(sx_sem* sem);
SX_API void sx_semaphore_post(sx_sem* sem, int count sx_default(1));
SX_API bool sx_semaphore_wait(sx_sem* sem, int msecs sx_default(-1));

// Signal
typedef struct sx_signal_s {
    sx_align_decl(16, uint8_t) data[128];
} sx_signal;

SX_API void sx_signal_init(sx_signal* sig);
SX_API void sx_signal_release(sx_signal* sig);
SX_API void sx_signal_raise(sx_signal* sig);
SX_API bool sx_signal_wait(sx_signal* sig, int msecs sx_default(-1));

// Lock-Free single-producer/single-consumer self-contained-data queue
typedef struct sx_queue_spsc sx_queue_spsc;
SX_API sx_queue_spsc* sx_queue_spsc_create(const sx_alloc* alloc, int item_sz, int capacity);
SX_API void           sx_queue_spsc_destroy(sx_queue_spsc* queue, const sx_alloc* alloc);
SX_API bool           sx_queue_spsc_produce(sx_queue_spsc* queue, const void* data);
SX_API bool           sx_queue_spsc_consume(sx_queue_spsc* queue, void* data);
SX_API bool           sx_queue_spsc_grow(sx_queue_spsc* queue, const sx_alloc* alloc);
SX_API bool           sx_queue_spsc_full(const sx_queue_spsc* queue);

#define sx_queue_spsc_produce_and_grow(_queue, _data, _alloc) \
    if (!sx_queue_spsc_produce((_queue), (_data))) {          \
        if (sx_queue_spsc_grow((_queue), (_alloc)))           \
            sx_queue_spsc_produce((_queue), (_data));         \
    }

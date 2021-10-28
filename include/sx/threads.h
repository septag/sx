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
SX_API int sx_thread_destroy(sx_thread* thrd, const sx_alloc* alloc);
SX_API bool sx_thread_running(sx_thread* thrd);
SX_API void sx_thread_setname(sx_thread* thrd, const char* name);
SX_API void sx_thread_yield(void);
SX_API uint32_t sx_thread_tid(void);

// Tls data
typedef void* sx_tls;

SX_API sx_tls sx_tls_create(void);
SX_API void sx_tls_destroy(sx_tls tls);
SX_API void sx_tls_set(sx_tls tls, void* data);
SX_API void* sx_tls_get(sx_tls tls);

// Mutex
typedef sx_align_decl(64, struct) sx_mutex_s {
    uint8_t data[64];
} sx_mutex;

SX_API void sx_mutex_init(sx_mutex* mutex);
SX_API void sx_mutex_release(sx_mutex* mutex);
SX_API void sx_mutex_enter(sx_mutex* mutex);
SX_API void sx_mutex_exit(sx_mutex* mutex);
SX_API bool sx_mutex_try(sx_mutex* mutex);

#if SX_CONFIG_OBSOLETE_CODE
#   define sx_mutex_lock(_mtx) sx_mutex_enter(_mtx)
#   define sx_mutex_unlock(_mtx) sx_mutex_exit(_mtx)
#   define sx_mutex_trylock(_mtx) sx_mutex_try(_mtx)
#else
#   define sx_mutex_lock(_mtx) sx_defer(sx_mutex_enter(&_mtx), sx_mutex_exit(&_mtx))
#endif

// Semaphore
typedef sx_align_decl(16, struct) sx_sem_s {
    uint8_t data[128];
} sx_sem;

SX_API void sx_semaphore_init(sx_sem* sem);
SX_API void sx_semaphore_release(sx_sem* sem);
SX_API void sx_semaphore_post(sx_sem* sem, int count sx_default(1));
SX_API bool sx_semaphore_wait(sx_sem* sem, int msecs sx_default(-1));

// Signal
typedef sx_align_decl(16, struct) sx_signal_s {
     uint8_t data[128];
} sx_signal;

SX_API void sx_signal_init(sx_signal* sig);
SX_API void sx_signal_release(sx_signal* sig);
SX_API void sx_signal_raise(sx_signal* sig);
SX_API bool sx_signal_wait(sx_signal* sig, int msecs sx_default(-1));


//
// Copyright 2018 Sepehr Taghdisian (septag@github). All rights reserved.
// License: https://github.com/septag/sx#license-bsd-2-clause
//
// threads.h - v1.0 - Common portable multi-threading primitives
//
//      sx_thread       Portable thread
//      sx_tls          Portable thread-local-storage which you can store a user_data per Tls
//      sx_mutex        Portable OS mutex, use for long-time data locks, for short-time locks use sx_lock_t in atomics.h
//      sx_sem          Portable OS semaphore. 'post' increases the count. 'wait' waits on semaphore if count is zero, 
//                      else decreases the count and continue
//      sx_signal       Portable OS signals/events. simplified version of the semaphore, where you 'wait' for signal to
//                      be triggered, then in another thread you 'raise' it and 'wait' will continue
//      TODO: sx_queue_spsc single-producer, single-consumer queue
//      TODO: sx_queue_mpsc multi-producer, single-consumer queue
//
#pragma once

#ifndef SX_THREADS_H_
#define SX_THREADS_H_

#include "allocator.h"

#ifdef __cplusplus
extern "C" {
#endif

// Thread
typedef struct sx__thread_s sx_thread;

// Thread callback function
typedef int (sx_thread_cb)(void* user_data);

sx_thread* sx_thread_create(const sx_alloc* alloc, sx_thread_cb* callback,
                            void* user_data SX_DFLT(NULL), int stack_sz SX_DFLT(0), const char* name SX_DFLT(NULL));
int sx_thread_destroy(sx_thread* thrd, const sx_alloc* alloc);
bool sx_thread_running(sx_thread* thrd);
void sx_thread_setname(sx_thread* thrd, const char* name);
void sx_thread_yield();
uint32_t sx_thread_tid();

// Tls data
typedef void* sx_tls;

sx_tls sx_tls_create();
void sx_tls_destroy(sx_tls tls);
void sx_tls_set(sx_tls tls, void* data);
void* sx_tls_get(sx_tls tls);

// Mutex
typedef struct sx_mutex_s
{
    SX_ALIGN_DECL(16, uint8_t) data[64];
} sx_mutex;

void sx_mutex_init(sx_mutex* mutex);
void sx_mutex_release(sx_mutex* mutex);
void sx_mutex_lock(sx_mutex* mutex);
void sx_mutex_unlock(sx_mutex* mutex);
bool sx_mutex_trylock(sx_mutex* mutex);

// Semaphore
typedef struct sx_sem_s
{
    SX_ALIGN_DECL(16, uint8_t) data[128];
} sx_sem;

void sx_semaphore_init(sx_sem* sem);
void sx_semaphore_release(sx_sem* sem);
void sx_semaphore_post(sx_sem* sem, int count SX_DFLT(1));
bool sx_semaphore_wait(sx_sem* sem, int msecs SX_DFLT(-1));

// Signal
typedef struct sx_signal_s
{
    SX_ALIGN_DECL(16, uint8_t) data[128];
} sx_signal;

void sx_signal_init(sx_signal* sig);
void sx_signal_release(sx_signal* sig);
void sx_signal_raise(sx_signal* sig);
bool sx_signal_wait(sx_signal* sig, int msecs SX_DFLT(-1));

// Lock-Free single-producer/single-consumer self-contained-data queue
typedef struct sx_queue_spsc sx_queue_spsc;
sx_queue_spsc* sx_queue_spsc_create(const sx_alloc* alloc, int item_sz, int capacity);
void sx_queue_spsc_destroy(sx_queue_spsc* queue, const sx_alloc* alloc);
bool sx_queue_spsc_produce(sx_queue_spsc* queue, const void* data);
bool sx_queue_spsc_consume(sx_queue_spsc* queue, void* data);

#ifdef __cplusplus
}
#endif  // extern "C"

#endif // SX_THREADS_H_

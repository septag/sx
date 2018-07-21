// atomic.h - v1.0 - Atomic primitives and operations
// Reference: http://en.cppreference.com/w/c/atomic
// NOTE: I encountered a problem in MSVC+Clang_C2 where some atomic functions where badly implemented 
//       So I had to implemenet common atomic functions and types myself
//       Later I may re-evaluate the compatibility of stdatomic.h and use that instead
//  
// sx_yield_cpu: yeilds cpu and prevents it from burning
// sx_memory_barrier: performs full memory barrier on the cpu side
//
// sx_lock_t: Common spin lock, use this for short-time data locking, for longer locking times, use mutex
//            Reference: http://en.cppreference.com/w/cpp/atomic/atomic_flag
//            Usage:
//              sx_lock_t lock = sx_lock_init()
//              sx_lock(lock); 
//              ... // write some data
//              sx_unlock(lock);
//
//              if (sx_trylock(lock)) {
//                  ... // write some data
//                  sx_unlock(lock);
//              }
//
#pragma once

#ifndef SX_ATOMIC_H_
#define SX_ATOMIC_H_

#include "sx.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
#include <stdatomic.h>  
typedef atomic_flag sx_lock_t;
#define sx_lock_init()  ATOMIC_FLAG_INIT
#define sx_lock(_l)     while(atomic_flag_test_and_set_explicit(&(_l), memory_order_acquire)) sx_yield_cpu()
#define sx_unlock(_l)   atomic_flag_clear_explicit(&(_l), memory_order_release)
#define sx_trylock(_l)  atomic_flag_test_and_set_explicit(&(_l), memory_order_acquire)
*/

typedef int volatile    sx_atomic_int;
typedef void* volatile  sx_atomic_ptr;
typedef sx_atomic_int   sx_lock_t;

void sx_yield_cpu();

void sx_memory_barrier();
void sx_memory_read_barrier();
void sx_memory_write_barrier();

void sx_compiler_barrier();
void sx_compiler_read_barrier();
void sx_compiler_write_barrier();

// Return first value and add new value
int sx_atomic_fetch_add(sx_atomic_int* a, int b);
// Increment and return result
int sx_atomic_incr(sx_atomic_int* a);
// Decrement and return result
int sx_atomic_decr(sx_atomic_int* a);
// Assigns the value and returns initial value
int sx_atomic_xchg(sx_atomic_int* a, int b);
// Puts the value of 'xchg' into 'a' if a equals comparand, returns initial value of 'a'
int sx_atomic_cas(sx_atomic_int* a, int xchg, int comparand);

// Assigns the pointer and returns initial value
void* sx_atomic_xchg_ptr(sx_atomic_ptr* a, void* b);
// Puts the pointer of 'xchg' into 'a' if a equals comparand, returns initial value of 'a'
void* sx_atomic_cas_ptr(sx_atomic_ptr* a, void* xchg, void* comparand);

// Locking functions
void sx_lock(sx_lock_t* lock);
void sx_unlock(sx_lock_t* lock);
bool sx_trylock(sx_lock_t* lock);

#include "inline/atomic.inl"

#ifdef __cplusplus
}
#endif

#endif // SX_ATOMIC_H_
//
// Copyright 2018 Sepehr Taghdisian (septag@github). All rights reserved.
// License: https://github.com/septag/sx#license-bsd-2-clause
//
// parts of this code is copied from bx library: https://github.com/bkaradzic/bx
// Copyright 2011-2019 Branimir Karadzic. All rights reserved.
// License: https://github.com/bkaradzic/bx#license-bsd-2-clause
//
// atomic.h - v1.1 - Atomic primitives and operations
// Reference: http://en.cppreference.com/w/c/atomic
// GCC: http://gcc.gnu.org/onlinedocs/gcc-4.6.2/gcc/Atomic-Builtins.html
//
// NOTE: I encountered a problem in MSVC+Clang_C2 where some atomic functions where badly
// implemented
//       So I had to implemenet common atomic functions and types myself
//       Later I may re-evaluate the compatibility of stdatomic.h and use that instead
//
// sx_yield_cpu: yeilds cpu and prevents it from burning
// sx_memory_barrier: performs full memory barrier on the cpu side
//
// sx_lock_t: Common spin lock, use this for short-time data locking, for longer locking times, use
// mutex
//            Reference: http://en.cppreference.com/w/cpp/atomic/atomic_flag
//            Usage:
//              sx_lock_t lock = 0;
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

#include "macros.h"
#include <stdint.h>

#if SX_PLATFORM_WINDOWS
// FIXME: I got wierd compiler error on MSVC+Clang_c2, so I had to comment this out
//        Every source must include <windows.h> before including atomic.h
#    include <intrin.h>
#    if SX_COMPILER_MSVC
#        pragma intrinsic(_mm_pause)
#        pragma intrinsic(_mm_mfence)
#        pragma intrinsic(_mm_lfence)
#        pragma intrinsic(_mm_sfence)
#        pragma intrinsic(_ReadWriteBarrier)
#        pragma intrinsic(_ReadBarrier)
#        pragma intrinsic(_WriteBarrier)
#        pragma intrinsic(_InterlockedExchangeAdd)
#        pragma intrinsic(_InterlockedIncrement)
#        pragma intrinsic(_InterlockedDecrement)
#        pragma intrinsic(_InterlockedExchange)
#        pragma intrinsic(_InterlockedCompareExchange)
#        pragma intrinsic(_InterlockedExchangePointer)
#        pragma intrinsic(_InterlockedCompareExchangePointer)
#    endif
#    if SX_ARCH_32BIT
#        include <emmintrin.h>    // _mm_xfence
#    endif
#endif


typedef int volatile sx_atomic_int;
typedef void* volatile sx_atomic_ptr;
typedef int64_t volatile sx_atomic_int64;

SX_FORCE_INLINE void sx_yield_cpu() {
#if SX_PLATFORM_WINDOWS
    _mm_pause();
#else
#    if SX_CPU_X86
    __asm__ __volatile__("pause");
#    elif SX_CPU_ARM && !SX_PLATFORM_RPI /* FIXME: didn't find a workaround for rpi */
    __asm__ __volatile__("yield");
#    endif
#endif
}

SX_FORCE_INLINE void sx_memory_barrier() {
#if SX_PLATFORM_WINDOWS
    _mm_mfence();
#else
    __sync_synchronize();
#endif
}

SX_FORCE_INLINE void sx_memory_read_barrier() {
#if SX_PLATFORM_WINDOWS
    _mm_lfence();
#else
    __sync_synchronize();
#endif
}

SX_FORCE_INLINE void sx_memory_write_barrier() {
#if SX_PLATFORM_WINDOWS
    _mm_sfence();
#else
    __sync_synchronize();
#endif
}

SX_FORCE_INLINE void sx_compiler_barrier() {
#if SX_PLATFORM_WINDOWS
    _ReadWriteBarrier();
#else
#endif
}

SX_FORCE_INLINE void sx_compiler_read_barrier() {
#if SX_PLATFORM_WINDOWS
    _ReadBarrier();
#else
#endif
}

SX_FORCE_INLINE void sx_compiler_write_barrier() {
#if SX_PLATFORM_WINDOWS
    _WriteBarrier();
#else
#endif
}

// int atomic
SX_FORCE_INLINE int sx_atomic_fetch_add(sx_atomic_int* a, int b) {
#if SX_PLATFORM_WINDOWS
    return _InterlockedExchangeAdd((long volatile*)a, b);
#else
    return __sync_fetch_and_add(a, b);
#endif
}

SX_FORCE_INLINE int sx_atomic_add_fetch(sx_atomic_int* a, int b) {
#if SX_PLATFORM_WINDOWS
    return _InterlockedExchangeAdd((long volatile*)a, b) + b;
#else
    return __sync_add_and_fetch(a, b);
#endif
}

SX_FORCE_INLINE int sx_atomic_incr(sx_atomic_int* a) {
#if SX_PLATFORM_WINDOWS
    return _InterlockedIncrement((long volatile*)a);
#else
    return __sync_add_and_fetch(a, 1);
#endif
}

SX_FORCE_INLINE int sx_atomic_decr(sx_atomic_int* a) {
#if SX_PLATFORM_WINDOWS
    return _InterlockedDecrement((long volatile*)a);
#else
    return __sync_sub_and_fetch(a, 1);
#endif
}

SX_FORCE_INLINE int sx_atomic_xchg(sx_atomic_int* a, int b) {
#if SX_PLATFORM_WINDOWS
    return _InterlockedExchange((long volatile*)a, b);
#else
    return __sync_lock_test_and_set(a, b);
#endif
}

SX_FORCE_INLINE int sx_atomic_cas(sx_atomic_int* a, int xchg, int comparand) {
#if SX_PLATFORM_WINDOWS
    return _InterlockedCompareExchange((long volatile*)a, xchg, comparand);
#else
    return __sync_val_compare_and_swap(a, comparand, xchg);
#endif
}

// pointer
SX_FORCE_INLINE void* sx_atomic_xchg_ptr(sx_atomic_ptr* a, void* b) {
#if SX_PLATFORM_WINDOWS
    return _InterlockedExchangePointer((void* volatile*)a, b);
#else
    return __sync_lock_test_and_set(a, b);
#endif
}

SX_FORCE_INLINE void* sx_atomic_cas_ptr(sx_atomic_ptr* a, void* xchg, void* comparand) {
#if SX_PLATFORM_WINDOWS
    return _InterlockedCompareExchangePointer((void* volatile*)a, xchg, comparand);
#else
    return __sync_val_compare_and_swap(a, comparand, xchg);
#endif
}

// Int64 atomic
#if SX_ARCH_64BIT
SX_FORCE_INLINE int64_t sx_atomic_fetch_add64(sx_atomic_int64* a, int64_t b) {
#if SX_PLATFORM_WINDOWS
    return _InterlockedExchangeAdd64(a, b);
#else
    return __sync_fetch_and_add(a, b);
#endif
}

SX_FORCE_INLINE int64_t sx_atomic_add_fetch64(sx_atomic_int64* a, int64_t b) {
#if SX_PLATFORM_WINDOWS
    return _InterlockedExchangeAdd64(a, b) + b;
#else
    return __sync_add_and_fetch(a, b);
#endif
}

SX_FORCE_INLINE int64_t sx_atomic_incr64(sx_atomic_int64* a) {
#if SX_PLATFORM_WINDOWS
    return _InterlockedIncrement64(a);
#else
    return __sync_add_and_fetch(a, 1);
#endif
}

SX_FORCE_INLINE int64_t sx_atomic_decr64(sx_atomic_int64* a) {
#if SX_PLATFORM_WINDOWS
    return _InterlockedDecrement64(a);
#else
    return __sync_sub_and_fetch(a, 1);
#endif
}

SX_FORCE_INLINE int64_t sx_atomic_xchg64(sx_atomic_int64* a, int64_t b) {
#if SX_PLATFORM_WINDOWS
    return _InterlockedExchange64(a, b);
#else
    return __sync_lock_test_and_set(a, b);
#endif
}

SX_FORCE_INLINE int64_t sx_atomic_cas64(sx_atomic_int64* a, int64_t xchg, int64_t comparand) {
#if SX_PLATFORM_WINDOWS
    return _InterlockedCompareExchange64(a, xchg, comparand);
#else
    return __sync_val_compare_and_swap(a, comparand, xchg);
#endif
}
#endif // SX_ARCH_64BIT

#if SX_ARCH_64BIT
typedef sx_atomic_int64 sx_atomic_size;
#define sx_atomic_fetch_add_size sx_atomic_fetch_add64
#define sx_atomic_add_fetch_size sx_atomic_add_fetch64
#define sx_atomic_incr_size      sx_atomic_incr64
#define sx_atomic_decr_size      sx_atomic_decr64
#define sx_atomic_xchg_size      sx_atomic_xchg64
#define sx_atomic_cas_size       sx_atomic_cas64
#else
typedef sx_atomic_int sx_atomic_size;
#define sx_atomic_fetch_add_size sx_atomic_fetch_add
#define sx_atomic_add_fetch_size sx_atomic_add_fetch
#define sx_atomic_incr_size      sx_atomic_incr
#define sx_atomic_decr_size      sx_atomic_decr
#define sx_atomic_xchg_size      sx_atomic_xchg
#define sx_atomic_cas_size       sx_atomic_cas
#endif // SX_ARCH_64BIT

#if !SX_PLATFORM_EMSCRIPTEN && (SX_COMPILER_GCC || SX_COMPILER_CLANG) && \
    __STDC_VERSION__ >= 201112L && !defined(__STDC_NO_ATOMICS__) &&      \
    (SX_COMPILER_CLANG || SX_COMPILER_GCC >= 40900)
#    include <stdatomic.h>
typedef atomic_flag  sx_lock_t;
SX_FORCE_INLINE void sx_lock(sx_lock_t* lock) {
    while (atomic_flag_test_and_set_explicit(lock, memory_order_acquire)) sx_yield_cpu();
}

SX_FORCE_INLINE void sx_unlock(sx_lock_t* lock) {
    atomic_flag_clear_explicit(lock, memory_order_release);
}

SX_FORCE_INLINE void sx_trylock(sx_lock_t* lock) {
    atomic_flag_test_and_set_explicit(lock, memory_order_acquire);
}
#else
typedef sx_atomic_int sx_lock_t;
SX_FORCE_INLINE void sx_unlock(sx_lock_t* lock) {
#    if SX_PLATFORM_WINDOWS
    sx_compiler_barrier();
    *lock = 0;
#    else
    __sync_lock_release(lock);
#    endif
}

SX_FORCE_INLINE int sx_trylock(sx_lock_t* lock) {
#    if SX_PLATFORM_WINDOWS
    int r = sx_atomic_xchg(lock, 1);
    sx_compiler_barrier();
    return r;
#    else
    return __sync_lock_test_and_set(lock, 1);
#    endif
}

SX_FORCE_INLINE void sx_lock(sx_lock_t* lock) {
    while (sx_trylock(lock)) sx_yield_cpu();
}
#endif    // SX_COMPILER_GCC || SX_COMPILER_CLANG

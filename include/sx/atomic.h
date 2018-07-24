//
// Copyright 2018 Sepehr Taghdisian (septag@github). All rights reserved.
// License: https://github.com/septag/sx#license-bsd-2-clause
//
// atomic.h - v1.0 - Atomic primitives and operations
// Reference: http://en.cppreference.com/w/c/atomic
// GCC: http://gcc.gnu.org/onlinedocs/gcc-4.6.2/gcc/Atomic-Builtins.html
//
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

#ifndef SX_ATOMIC_H_
#define SX_ATOMIC_H_

#include "sx.h"
#include "platform.h"

#if SX_PLATFORM_WINDOWS
// FIXME: I got wierd compiler error on MSVC+Clang_c2, so I had to comment this out
//        Every source must include <windows.h> before including atomic.h
#   ifndef _WINDOWS_
#       define WIN32_LEAN_AND_MEAN
#       include <Windows.h>
#   endif
#   include <intrin.h>
#   if SX_COMPILER_MSVC
#       pragma intrinsic(_mm_pause)
#       pragma intrinsic(_mm_mfence)
#       pragma intrinsic(_mm_lfence)
#       pragma intrinsic(_mm_sfence)
#       pragma intrinsic(_ReadWriteBarrier)
#       pragma intrinsic(_ReadBarrier)
#       pragma intrinsic(_WriteBarrier)
#       pragma intrinsic(_InterlockedExchangeAdd)
#       pragma intrinsic(_InterlockedIncrement)
#       pragma intrinsic(_InterlockedDecrement)
#       pragma intrinsic(_InterlockedExchange)
#       pragma intrinsic(_InterlockedCompareExchange)
#       pragma intrinsic(_InterlockedExchangePointer)
#       pragma intrinsic(_InterlockedCompareExchangePointer)
#   endif
#   if SX_ARCH_32BIT
#	    include <emmintrin.h> // _mm_xfence
#   endif
#endif


typedef int volatile    sx_atomic_int;
typedef void* volatile  sx_atomic_ptr;
typedef sx_atomic_int   sx_lock_t;

SX_FORCE_INLINE void sx_yield_cpu()
{
#if SX_PLATFORM_WINDOWS
    _mm_pause();
#else
#   if SX_CPU_X86
    __asm__ __volatile__("pause");
#   elif SX_CPU_ARM
    __asm__ __volatile__("yield");
#   endif
#endif
}

SX_FORCE_INLINE void sx_memory_barrier()
{
#if SX_PLATFORM_WINDOWS
    _mm_mfence();
#else
    __sync_synchronize();
#endif
}

SX_FORCE_INLINE void sx_memory_read_barrier()
{
#if SX_PLATFORM_WINDOWS
    _mm_lfence();
#else
    __sync_synchronize();
#endif
}

SX_FORCE_INLINE void sx_memory_write_barrier()
{
#if SX_PLATFORM_WINDOWS
    _mm_sfence();
#else
    __sync_synchronize();
#endif
}

SX_FORCE_INLINE void sx_compiler_barrier()
{
#if SX_PLATFORM_WINDOWS
    _ReadWriteBarrier();
#else
#endif
}

SX_FORCE_INLINE void sx_compiler_read_barrier()
{
#if SX_PLATFORM_WINDOWS
    _ReadBarrier();
#else
#endif
}

SX_FORCE_INLINE void sx_compiler_write_barrier()
{
#if SX_PLATFORM_WINDOWS
    _WriteBarrier();
#else
#endif
}

SX_FORCE_INLINE int sx_atomic_fetch_add(sx_atomic_int* a, int b)
{
#if SX_PLATFORM_WINDOWS
    return _InterlockedExchangeAdd((LONG volatile*)a, b);
#else
    return __sync_fetch_and_add(a, b);
#endif
}

SX_FORCE_INLINE int sx_atomic_incr(sx_atomic_int* a)
{
#if SX_PLATFORM_WINDOWS
    return _InterlockedIncrement((LONG volatile*)a);
#else
    return __sync_add_and_fetch(a, 1);
#endif
}

SX_FORCE_INLINE int sx_atomic_decr(sx_atomic_int* a)
{
#if SX_PLATFORM_WINDOWS
    return _InterlockedDecrement((LONG volatile*)a);
#else
    return __sync_sub_and_fetch(a, 1);
#endif
}

SX_FORCE_INLINE int sx_atomic_xchg(sx_atomic_int* a, int b)
{
#if SX_PLATFORM_WINDOWS
    return _InterlockedExchange((LONG volatile*)a, b);
#else
    return __sync_lock_test_and_set(a, b);
#endif
}

SX_FORCE_INLINE int sx_atomic_cas(sx_atomic_int* a, int xchg, int comparand)
{
#if SX_PLATFORM_WINDOWS
    return _InterlockedCompareExchange((LONG volatile*)a, xchg, comparand);
#else
    return __sync_val_compare_and_swap(a, comparand, xchg);
#endif
}

SX_FORCE_INLINE void* sx_atomic_xchg_ptr(sx_atomic_ptr* a, void* b)
{
#if SX_PLATFORM_WINDOWS
    return _InterlockedExchangePointer((PVOID volatile*)a, b);
#else
    return __sync_lock_test_and_set(a, b);
#endif
}

SX_FORCE_INLINE void* sx_atomic_cas_ptr(sx_atomic_ptr* a, void* xchg, void* comparand)
{
#if SX_PLATFORM_WINDOWS
    return _InterlockedCompareExchangePointer((PVOID volatile*)a, xchg, comparand);
#else
    return __sync_val_compare_and_swap(a, comparand, xchg);
#endif
}

/*
#include <stdatomic.h>  
typedef atomic_flag sx_lock_t;
#define sx_lock_init()  ATOMIC_FLAG_INIT
#define sx_lock(_l)     while(atomic_flag_test_and_set_explicit(&(_l), memory_order_acquire)) sx_yield_cpu()
#define sx_unlock(_l)   atomic_flag_clear_explicit(&(_l), memory_order_release)
#define sx_trylock(_l)  atomic_flag_test_and_set_explicit(&(_l), memory_order_acquire)
*/

SX_FORCE_INLINE void sx_unlock(sx_lock_t* lock)
{
#if SX_PLATFORM_WINDOWS
    sx_compiler_barrier();
    *lock = 0;
#else
    __sync_lock_release(lock);
#endif
}

SX_FORCE_INLINE int sx_trylock(sx_lock_t* lock)
{
#if SX_PLATFORM_WINDOWS
    int r = sx_atomic_xchg(lock, 1);
    sx_compiler_barrier();
    return r;
#else
    return __sync_lock_test_and_set(lock, 1);
#endif
}

SX_FORCE_INLINE void sx_lock(sx_lock_t* lock)
{
    while (sx_trylock(lock))
        sx_yield_cpu();
}

#endif // SX_ATOMIC_H_
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
// sx_yield_cpu: yields cpu and prevents it from burning
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
#    if SX_ARCH_32BIT && SX_CPU_X86
#       if !SX_COMPILER_MSVC
#          include <x86intrin.h>
#       endif
#    endif
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
#        pragma intrinsic(__rdtsc)
#    endif
#elif SX_PLATFORM_APPLE
#    include <mach/mach_time.h>
#endif

#if !SX_PLATFORM_WINDOWS
#   include <sys/time.h>
#endif

#if defined(__SSE2__)
#    include <emmintrin.h>    // _mm_pause
#endif

SX_PRAGMA_DIAGNOSTIC_PUSH()
SX_PRAGMA_DIAGNOSTIC_IGNORED_CLANG("-Wdeprecated-declarations")

typedef int volatile sx_atomic_int;
typedef void* volatile sx_atomic_ptr;
typedef int64_t volatile sx_atomic_int64;

SX_FORCE_INLINE void sx_yield_cpu(void)
{
#if SX_PLATFORM_WINDOWS
    _mm_pause();
#elif defined(__SSE2__)  // AMD and Intel
    _mm_pause();
#elif defined(__i386__) || defined(__x86_64__)
    asm volatile("pause");
#elif defined(__aarch64__)
    asm volatile("wfe");
#elif defined(__armel__) || defined(__ARMEL__)
    asm volatile ("nop" ::: "memory");  // default operation - does nothing => Might lead to passive spinning.
#elif defined(__arm__) || defined(__aarch64__) // arm big endian / arm64
    __asm__ __volatile__ ("yield" ::: "memory");
#elif defined(__ia64__)  // IA64
    __asm__ __volatile__ ("hint @pause");
#elif defined(__powerpc__) || defined(__ppc__) || defined(__PPC__) // PowerPC
     __asm__ __volatile__ ("or 27,27,27" ::: "memory");
#else  // everything else.
     asm volatile ("nop" ::: "memory");  // default operation - does nothing => Might lead to passive spinning.
#endif
}

// https://github.com/google/benchmark/blob/v1.1.0/src/cycleclock.h
SX_FORCE_INLINE uint64_t sx_cycle_clock()
{
#if SX_PLATFORM_APPLE
    return mach_absolute_time();
#elif SX_PLATFORM_WINDOWS && SX_COMPILER_MSVC
#    if SX_ARCH_32BIT
    _asm rdtsc
#    else
    return __rdtsc();
#    endif
#elif SX_CPU_ARM && SX_ARCH_64BIT
    uint64_t virtual_timer_value;
    asm volatile("mrs %0, cntvct_el0" : "=r"(virtual_timer_value));
    return virtual_timer_value;
#elif SX_CPU_ARM
#   if (__ARM_ARCH >= 6)
    uint32_t pmccntr;
    uint32_t pmuseren;
    uint32_t pmcntenset;
    // Read the user mode perf monitor counter access permissions.
    asm volatile("mrc p15, 0, %0, c9, c14, 0" : "=r"(pmuseren));
    if (pmuseren & 1) {    // Allows reading perfmon counters for user mode code.
        asm volatile("mrc p15, 0, %0, c9, c12, 1" : "=r"(pmcntenset));
        if (pmcntenset & 0x80000000ul) {    // Is it counting?
            asm volatile("mrc p15, 0, %0, c9, c13, 0" : "=r"(pmccntr));
            // The counter is set up to count every 64th cycle
            return (int64_t)pmccntr * 64;    // Should optimize to << 6
        }
    }
#   endif // (__ARM_ARCH >= 6)
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (int64_t)tv.tv_sec * 1000000 + tv.tv_usec;
#elif SX_CPU_X86 && SX_ARCH_32BIT
    int64_t ret;
    __asm__ volatile("rdtsc" : "=A"(ret));
    return ret;
#elif SX_CPU_X86 && SX_ARCH_64BIT
    uint64_t low, high;
    __asm__ volatile("rdtsc" : "=a"(low), "=d"(high));
    return (high << 32) | low;
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

// int atomic
SX_FORCE_INLINE int sx_atomic_fetch_add(sx_atomic_int* a, int b)
{
#if SX_PLATFORM_WINDOWS
    return _InterlockedExchangeAdd((long volatile*)a, b);
#else
    return __sync_fetch_and_add(a, b);
#endif
}

SX_FORCE_INLINE int sx_atomic_add_fetch(sx_atomic_int* a, int b)
{
#if SX_PLATFORM_WINDOWS
    return _InterlockedExchangeAdd((long volatile*)a, b) + b;
#else
    return __sync_add_and_fetch(a, b);
#endif
}

SX_FORCE_INLINE int sx_atomic_incr(sx_atomic_int* a)
{
#if SX_PLATFORM_WINDOWS
    return _InterlockedIncrement((long volatile*)a);
#else
    return __sync_add_and_fetch(a, 1);
#endif
}

SX_FORCE_INLINE int sx_atomic_decr(sx_atomic_int* a)
{
#if SX_PLATFORM_WINDOWS
    return _InterlockedDecrement((long volatile*)a);
#else
    return __sync_sub_and_fetch(a, 1);
#endif
}

SX_FORCE_INLINE int sx_atomic_xchg(sx_atomic_int* a, int b)
{
#if SX_PLATFORM_WINDOWS
    return _InterlockedExchange((long volatile*)a, b);
#else
    return __sync_lock_test_and_set(a, b);
#endif
}

// common use:
// if (sx_atomic_cas(&var, y, x) == x)
// means: if I'm the one changing var from x to y
SX_FORCE_INLINE int sx_atomic_cas(sx_atomic_int* a, int xchg, int comparand)
{
#if SX_PLATFORM_WINDOWS
    return _InterlockedCompareExchange((long volatile*)a, xchg, comparand);
#else
    return __sync_val_compare_and_swap(a, comparand, xchg);
#endif
}

// pointer
SX_FORCE_INLINE void* sx_atomic_xchg_ptr(sx_atomic_ptr* a, void* b)
{
#if SX_PLATFORM_WINDOWS
    return _InterlockedExchangePointer((void* volatile*)a, b);
#else
    return __sync_lock_test_and_set(a, b);
#endif
}

SX_FORCE_INLINE void* sx_atomic_cas_ptr(sx_atomic_ptr* a, void* xchg, void* comparand)
{
#if SX_PLATFORM_WINDOWS
    return _InterlockedCompareExchangePointer((void* volatile*)a, xchg, comparand);
#else
    return __sync_val_compare_and_swap(a, comparand, xchg);
#endif
}

// Int64 atomic
#if SX_ARCH_64BIT
SX_FORCE_INLINE int64_t sx_atomic_fetch_add64(sx_atomic_int64* a, int64_t b)
{
#    if SX_PLATFORM_WINDOWS
    return _InterlockedExchangeAdd64(a, b);
#    else
    return __sync_fetch_and_add(a, b);
#    endif
}

SX_FORCE_INLINE int64_t sx_atomic_add_fetch64(sx_atomic_int64* a, int64_t b)
{
#    if SX_PLATFORM_WINDOWS
    return _InterlockedExchangeAdd64(a, b) + b;
#    else
    return __sync_add_and_fetch(a, b);
#    endif
}

SX_FORCE_INLINE int64_t sx_atomic_incr64(sx_atomic_int64* a)
{
#    if SX_PLATFORM_WINDOWS
    return _InterlockedIncrement64(a);
#    else
    return __sync_add_and_fetch(a, 1);
#    endif
}

SX_FORCE_INLINE int64_t sx_atomic_decr64(sx_atomic_int64* a)
{
#    if SX_PLATFORM_WINDOWS
    return _InterlockedDecrement64(a);
#    else
    return __sync_sub_and_fetch(a, 1);
#    endif
}

SX_FORCE_INLINE int64_t sx_atomic_xchg64(sx_atomic_int64* a, int64_t b)
{
#    if SX_PLATFORM_WINDOWS
    return _InterlockedExchange64(a, b);
#    else
    return __sync_lock_test_and_set(a, b);
#    endif
}

SX_FORCE_INLINE int64_t sx_atomic_cas64(sx_atomic_int64* a, int64_t xchg, int64_t comparand)
{
#    if SX_PLATFORM_WINDOWS
    return _InterlockedCompareExchange64(a, xchg, comparand);
#    else
    return __sync_val_compare_and_swap(a, comparand, xchg);
#    endif
}
#endif    // SX_ARCH_64BIT

#if SX_ARCH_64BIT
typedef sx_atomic_int64 sx_atomic_size;
#    define sx_atomic_fetch_add_size sx_atomic_fetch_add64
#    define sx_atomic_add_fetch_size sx_atomic_add_fetch64
#    define sx_atomic_incr_size sx_atomic_incr64
#    define sx_atomic_decr_size sx_atomic_decr64
#    define sx_atomic_xchg_size sx_atomic_xchg64
#    define sx_atomic_cas_size sx_atomic_cas64
#else
typedef sx_atomic_int sx_atomic_size;
#    define sx_atomic_fetch_add_size sx_atomic_fetch_add
#    define sx_atomic_add_fetch_size sx_atomic_add_fetch
#    define sx_atomic_incr_size sx_atomic_incr
#    define sx_atomic_decr_size sx_atomic_decr
#    define sx_atomic_xchg_size sx_atomic_xchg
#    define sx_atomic_cas_size sx_atomic_cas
#endif    // SX_ARCH_64BIT

SX_PRAGMA_DIAGNOSTIC_POP()

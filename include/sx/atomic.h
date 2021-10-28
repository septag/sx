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
// Note on compare-exchange:
//      common use case:
//          if (sx_atomic_cas(&var, y, x) == x)
//          meaning: if I'm the one changing var from x to y
//
#pragma once

#include "macros.h"
#include <stdint.h>

SX_PRAGMA_DIAGNOSTIC_PUSH()
SX_PRAGMA_DIAGNOSTIC_IGNORED_MSVC(4505) // unreferenced function with internal linkage has been removed
#   include "../../3rdparty/c89atomic/c89atomic.h"
SX_PRAGMA_DIAGNOSTIC_PUSH()

typedef c89atomic_uint32    sx_atomic_uint32;
typedef c89atomic_uint64    sx_atomic_uint64;
#if SX_ARCH_32BIT
    typedef c89atomic_uint32    sx_atomic_ptr;
#elif SX_ARCH_64BIT
    typedef c89atomic_uint64    sx_atomic_ptr;
#endif

#define sx_atomic_order_c11_value(_o) = c89atomic_##_o


typedef enum sx_atomic_memory_order
{
    SX_ATOMIC_MEMORYORDER_RELAXED sx_atomic_order_c11_value(memory_order_relaxed),
    SX_ATOMIC_MEMORYORDER_CONSUME sx_atomic_order_c11_value(memory_order_consume),
    SX_ATOMIC_MEMORYORDER_ACQUIRE sx_atomic_order_c11_value(memory_order_acquire),
    SX_ATOMIC_MEMORYORDER_RELEASE sx_atomic_order_c11_value(memory_order_release),
    SX_ATOMIC_MEMORYORDER_ACQREL sx_atomic_order_c11_value(memory_order_acq_rel),
    SX_ATOMIC_MEMORYORDER_SEQCST sx_atomic_order_c11_value(memory_order_seq_cst)
} sx_atomic_memory_order;

SX_FORCE_INLINE void sx_relax_cpu(void);
SX_FORCE_INLINE uint64_t sx_cycle_clock(void);

SX_FORCE_INLINE void sx_atomic_thread_fence(sx_atomic_memory_order order);
SX_FORCE_INLINE void sx_atomic_signal_fence(sx_atomic_memory_order order);

SX_FORCE_INLINE uint32_t sx_atomic_load32(sx_atomic_uint32* a);
SX_FORCE_INLINE uint64_t sx_atomic_load64(sx_atomic_uint64* a);
SX_FORCE_INLINE void sx_atomic_store32(sx_atomic_uint32* a, uint32_t b);
SX_FORCE_INLINE void sx_atomic_store64(sx_atomic_uint64* a, uint64_t b);

SX_FORCE_INLINE uint32_t sx_atomic_load32_explicit(sx_atomic_uint32* a, sx_atomic_memory_order order);
SX_FORCE_INLINE uint64_t sx_atomic_load64_explicit(sx_atomic_uint64* a, sx_atomic_memory_order order);
SX_FORCE_INLINE void sx_atomic_store32_explicit(sx_atomic_uint32* a, uint32_t b, sx_atomic_memory_order order);
SX_FORCE_INLINE void sx_atomic_store64_explicit(sx_atomic_uint64* a, uint64_t b, sx_atomic_memory_order order);

SX_FORCE_INLINE uint32_t sx_atomic_fetch_add32(sx_atomic_uint32* a, uint32_t b);
SX_FORCE_INLINE uint32_t sx_atomic_fetch_sub32(sx_atomic_uint32* a, uint32_t b);
SX_FORCE_INLINE uint32_t sx_atomic_fetch_or32(sx_atomic_uint32* a, uint32_t b);
SX_FORCE_INLINE uint32_t sx_atomic_fetch_and32(sx_atomic_uint32* a, uint32_t b);
SX_FORCE_INLINE uint32_t sx_atomic_exchange32(sx_atomic_uint32* a, uint32_t b);
SX_FORCE_INLINE uint32_t sx_atomic_compare_exchange32_weak(sx_atomic_uint32* a, uint32_t* expected, uint32_t desired);
SX_FORCE_INLINE uint32_t sx_atomic_compare_exchange32_strong(sx_atomic_uint32* a, uint32_t* expected, uint32_t desired);

SX_FORCE_INLINE uint32_t sx_atomic_fetch_add32_explicit(sx_atomic_uint32* a, uint32_t b, sx_atomic_memory_order order);
SX_FORCE_INLINE uint32_t sx_atomic_fetch_sub32_explicit(sx_atomic_uint32* a, uint32_t b, sx_atomic_memory_order order);
SX_FORCE_INLINE uint32_t sx_atomic_fetch_or32_explicit(sx_atomic_uint32* a, uint32_t b, sx_atomic_memory_order order);
SX_FORCE_INLINE uint32_t sx_atomic_fetch_and32_explicit(sx_atomic_uint32* a, uint32_t b, sx_atomic_memory_order order);
SX_FORCE_INLINE uint32_t sx_atomic_exchange32_explicit(sx_atomic_uint32* a, uint32_t b, sx_atomic_memory_order order);
SX_FORCE_INLINE uint32_t sx_atomic_compare_exchange32_weak_explicit(sx_atomic_uint32* a, uint32_t* expected, uint32_t desired, 
                                                                    sx_atomic_memory_order success, sx_atomic_memory_order fail);
SX_FORCE_INLINE uint32_t sx_atomic_compare_exchange32_strong_explicit(sx_atomic_uint32* a, uint32_t* expected, uint32_t desired, 
                                                                      sx_atomic_memory_order success, sx_atomic_memory_order fail);

SX_FORCE_INLINE uint64_t sx_atomic_fetch_add64(sx_atomic_uint64* a, uint64_t b);
SX_FORCE_INLINE uint64_t sx_atomic_fetch_sub64(sx_atomic_uint64* a, uint64_t b);
SX_FORCE_INLINE uint64_t sx_atomic_exchange64(sx_atomic_uint64* a, uint64_t b);
SX_FORCE_INLINE uint64_t sx_atomic_fetch_or64(sx_atomic_uint64* a, uint64_t b);
SX_FORCE_INLINE uint64_t sx_atomic_fetch_and64(sx_atomic_uint64* a, uint64_t b);
SX_FORCE_INLINE uint64_t sx_atomic_compare_exchange64_weak(sx_atomic_uint64* a, unsigned long long* expected, uint64_t desired);
SX_FORCE_INLINE uint64_t sx_atomic_compare_exchange64_strong(sx_atomic_uint64* a, unsigned long long* expected, uint64_t desired);

SX_FORCE_INLINE uint64_t sx_atomic_fetch_add64_explicit(sx_atomic_uint64* a, uint64_t b, sx_atomic_memory_order order);
SX_FORCE_INLINE uint64_t sx_atomic_fetch_sub64_explicit(sx_atomic_uint64* a, uint64_t b, sx_atomic_memory_order order);
SX_FORCE_INLINE uint64_t sx_atomic_exchange64_explicit(sx_atomic_uint64* a, uint64_t b, sx_atomic_memory_order order);
SX_FORCE_INLINE uint64_t sx_atomic_fetch_or64_explicit(sx_atomic_uint64* a, uint64_t b, sx_atomic_memory_order order);
SX_FORCE_INLINE uint64_t sx_atomic_fetch_and64_explicit(sx_atomic_uint64* a, uint64_t b, sx_atomic_memory_order order);
SX_FORCE_INLINE uint64_t sx_atomic_compare_exchange64_weak_explicit(sx_atomic_uint64* a, unsigned long long* expected, uint64_t desired, 
                                                                    sx_atomic_memory_order success, sx_atomic_memory_order fail);
SX_FORCE_INLINE uint64_t sx_atomic_compare_exchange64_strong_explicit(sx_atomic_uint64* a, unsigned long long* expected, uint64_t desired, 
                                                                      sx_atomic_memory_order success, sx_atomic_memory_order fail);

////////////////////////////////////////////////////////////////////////////////////////////////////
#if SX_PLATFORM_WINDOWS
#    if SX_ARCH_32BIT && SX_CPU_X86
#       if !SX_COMPILER_MSVC
#          include <x86intrin.h>
#       endif
#    endif
#    include <intrin.h>
#    if SX_COMPILER_MSVC
#        pragma intrinsic(_mm_pause)
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

SX_FORCE_INLINE void sx_relax_cpu(void)
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
SX_FORCE_INLINE uint64_t sx_cycle_clock(void)
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

SX_FORCE_INLINE void sx_atomic_thread_fence(sx_atomic_memory_order order)
{
    c89atomic_thread_fence(order);
}

SX_FORCE_INLINE void sx_atomic_signal_fence(sx_atomic_memory_order order)
{
    c89atomic_signal_fence(order);
}

SX_FORCE_INLINE uint32_t sx_atomic_load32(sx_atomic_uint32* a)
{
    return c89atomic_load_32(a);
}

SX_FORCE_INLINE uint64_t sx_atomic_load64(sx_atomic_uint64* a)
{
    return c89atomic_load_64(a);
}

SX_FORCE_INLINE void sx_atomic_store32(sx_atomic_uint32* a, uint32_t b)
{
    c89atomic_store_32(a, b);
}

SX_FORCE_INLINE void sx_atomic_store64(sx_atomic_uint64* a, uint64_t b)
{
    c89atomic_store_64(a, b); 
}

SX_FORCE_INLINE uint32_t sx_atomic_load32_explicit(sx_atomic_uint32* a, sx_atomic_memory_order order)
{
    return c89atomic_load_explicit_32(a, order);
}

SX_FORCE_INLINE uint64_t sx_atomic_load64_explicit(sx_atomic_uint64* a, sx_atomic_memory_order order)
{
    return c89atomic_load_explicit_64(a, order);
}

SX_FORCE_INLINE void sx_atomic_store32_explicit(sx_atomic_uint32* a, uint32_t b, sx_atomic_memory_order order)
{
    c89atomic_store_explicit_32(a, b, order);
}

SX_FORCE_INLINE void sx_atomic_store64_explicit(sx_atomic_uint64* a, uint64_t b, sx_atomic_memory_order order)
{
    c89atomic_store_explicit_64(a, b, order);
}

SX_FORCE_INLINE uint32_t sx_atomic_fetch_add32(sx_atomic_uint32* a, uint32_t b)
{
    return c89atomic_fetch_add_32(a, b);
}

SX_FORCE_INLINE uint32_t sx_atomic_fetch_sub32(sx_atomic_uint32* a, uint32_t b)
{
    return c89atomic_fetch_sub_32(a, b);
}

SX_FORCE_INLINE uint32_t sx_atomic_fetch_or32(sx_atomic_uint32* a, uint32_t b)
{
    return c89atomic_fetch_or_32(a, b);
}

SX_FORCE_INLINE uint32_t sx_atomic_fetch_and32(sx_atomic_uint32* a, uint32_t b)
{
    return c89atomic_fetch_and_32(a, b);
}

SX_FORCE_INLINE uint32_t sx_atomic_exchange32(sx_atomic_uint32* a, uint32_t b)
{
    return c89atomic_exchange_32(a, b);
}

SX_FORCE_INLINE uint32_t sx_atomic_compare_exchange32_weak(sx_atomic_uint32* a, uint32_t* desired, uint32_t expected)
{
    return c89atomic_compare_exchange_weak_32(a, desired, expected);
}

SX_FORCE_INLINE uint32_t sx_atomic_compare_exchange32_strong(sx_atomic_uint32* a, uint32_t* desired, uint32_t expected)
{
    return c89atomic_compare_exchange_strong_32(a, desired, expected);
}

SX_FORCE_INLINE uint32_t sx_atomic_fetch_add32_explicit(sx_atomic_uint32* a, uint32_t b, sx_atomic_memory_order order)
{
    return c89atomic_fetch_add_explicit_32(a, b, order);
}

SX_FORCE_INLINE uint32_t sx_atomic_fetch_sub32_explicit(sx_atomic_uint32* a, uint32_t b, sx_atomic_memory_order order)
{
    return c89atomic_fetch_sub_explicit_32(a, b, order);
}

SX_FORCE_INLINE uint32_t sx_atomic_fetch_or32_explicit(sx_atomic_uint32* a, uint32_t b, sx_atomic_memory_order order)
{
    return c89atomic_fetch_or_explicit_32(a, b, order);
}

SX_FORCE_INLINE uint32_t sx_atomic_fetch_and32_explicit(sx_atomic_uint32* a, uint32_t b, sx_atomic_memory_order order)
{
    return c89atomic_fetch_and_explicit_32(a, b, order);
}

SX_FORCE_INLINE uint32_t sx_atomic_exchange32_explicit(sx_atomic_uint32* a, uint32_t b, sx_atomic_memory_order order)
{
    return c89atomic_exchange_explicit_32(a, b, order);
}

SX_FORCE_INLINE uint32_t sx_atomic_compare_exchange32_weak_explicit(sx_atomic_uint32* a, uint32_t* desired, uint32_t expected, sx_atomic_memory_order success, sx_atomic_memory_order fail)
{
    return c89atomic_compare_exchange_weak_explicit_32(a, desired, expected, success, fail);
}

SX_FORCE_INLINE uint32_t sx_atomic_compare_exchange32_strong_explicit(sx_atomic_uint32* a, uint32_t* desired, uint32_t expected, sx_atomic_memory_order success, sx_atomic_memory_order fail)
{
    return c89atomic_compare_exchange_strong_explicit_32(a, desired, expected, success, fail);
}

SX_FORCE_INLINE uint64_t sx_atomic_fetch_add64(sx_atomic_uint64* a, uint64_t b)
{
    return c89atomic_fetch_add_64(a, b);
}

SX_FORCE_INLINE uint64_t sx_atomic_fetch_sub64(sx_atomic_uint64* a, uint64_t b)
{
    return c89atomic_fetch_sub_64(a, b);
}

SX_FORCE_INLINE uint64_t sx_atomic_exchange64(sx_atomic_uint64* a, uint64_t b)
{
    return c89atomic_exchange_64(a, b);
}

SX_FORCE_INLINE uint64_t sx_atomic_fetch_or64(sx_atomic_uint64* a, uint64_t b)
{
    return c89atomic_fetch_or_64(a, b);
}

SX_FORCE_INLINE uint64_t sx_atomic_fetch_and64(sx_atomic_uint64* a, uint64_t b)
{
    return c89atomic_fetch_and_64(a, b);
}

SX_FORCE_INLINE uint64_t sx_atomic_compare_exchange64_weak(sx_atomic_uint64* a, unsigned long long* expected, uint64_t desired)
{
    return c89atomic_compare_exchange_weak_64(a, expected, desired);
}

SX_FORCE_INLINE uint64_t sx_atomic_compare_exchange64_strong(sx_atomic_uint64* a, unsigned long long* expected, uint64_t desired)
{
    return c89atomic_compare_exchange_strong_64(a, expected, desired);
}

SX_FORCE_INLINE uint64_t sx_atomic_fetch_add64_explicit(sx_atomic_uint64* a, uint64_t b, sx_atomic_memory_order order)
{
    return c89atomic_fetch_add_explicit_64(a, b, order);
}

SX_FORCE_INLINE uint64_t sx_atomic_fetch_sub64_explicit(sx_atomic_uint64* a, uint64_t b, sx_atomic_memory_order order)
{
    return c89atomic_fetch_sub_explicit_64(a, b, order);
}

SX_FORCE_INLINE uint64_t sx_atomic_exchange64_explicit(sx_atomic_uint64* a, uint64_t b, sx_atomic_memory_order order)
{
    return c89atomic_exchange_explicit_64(a, b, order);
}

SX_FORCE_INLINE uint64_t sx_atomic_fetch_or64_explicit(sx_atomic_uint64* a, uint64_t b, sx_atomic_memory_order order)
{
    return c89atomic_fetch_or_explicit_64(a, b, order);
}

SX_FORCE_INLINE uint64_t sx_atomic_fetch_and64_explicit(sx_atomic_uint64* a, uint64_t b, sx_atomic_memory_order order)
{
    return c89atomic_fetch_and_explicit_64(a, b, order);
}

SX_FORCE_INLINE uint64_t sx_atomic_compare_exchange64_weak_explicit(sx_atomic_uint64* a, unsigned long long* expected, uint64_t desired, sx_atomic_memory_order success, sx_atomic_memory_order fail)
{
    return c89atomic_compare_exchange_weak_explicit_64(a, expected, desired, success, fail);
}

SX_FORCE_INLINE uint64_t sx_atomic_compare_exchange64_strong_explicit(sx_atomic_uint64* a, unsigned long long* expected, uint64_t desired, sx_atomic_memory_order success, sx_atomic_memory_order fail)
{
    return c89atomic_compare_exchange_strong_explicit_64(a, expected, desired, success, fail);
}

#if SX_ARCH_64BIT
#    define sx_atomic_loadptr                               sx_atomic_load64
#    define sx_atomic_storeptr                              sx_atomic_store64
#    define sx_atomic_loadptr_explicit                      sx_atomic_load64_explicit
#    define sx_atomic_storeptr_explicit                     sx_atomic_store64_explicit
#    define sx_atomic_fetch_addptr                          sx_atomic_fetch_add64
#    define sx_atomic_fetch_subptr                          sx_atomic_fetch_sub64
#    define sx_atomic_fetch_orptr                           sx_atomic_fetch_or64
#    define sx_atomic_exchangeptr                           sx_atomic_exchange64
#    define sx_atomic_compare_exchangeptr_weak              sx_atomic_compare_exchange64_weak
#    define sx_atomic_compare_exchangeptr_strong            sx_atomic_compare_exchange64_strong
#    define sx_atomic_fetch_addptr_explicit                 sx_atomic_fetch_add64_explicit
#    define sx_atomic_fetch_subptr_explicit                 sx_atomic_fetch_sub64_explicit
#    define sx_atomic_fetch_orptr_explicit                  sx_atomic_fetch_or64_explicit
#    define sx_atomic_fetch_andptr_explicit                 sx_atomic_fetch_and64_explicit
#    define sx_atomic_exchangeptr_explicit                  sx_atomic_exchange64_explicit
#    define sx_atomic_compare_exchangeptr_weak_explicit     sx_atomic_compare_exchange64_weak_explicit
#    define sx_atomic_compare_exchangeptr_strong_explicit   sx_atomic_compare_exchange64_strong_explicit
#else
#    define sx_atomic_loadptr                               sx_atomic_load32
#    define sx_atomic_storeptr                              sx_atomic_store32
#    define sx_atomic_loadptr_explicit                      sx_atomic_load32_explicit
#    define sx_atomic_storeptr_explicit                     sx_atomic_store32_explicit
#    define sx_atomic_fetch_addptr                          sx_atomic_fetch_add32
#    define sx_atomic_fetch_subptr                          sx_atomic_fetch_sub32
#    define sx_atomic_fetch_orptr                           sx_atomic_fetch_or32
#    define sx_atomic_exchangeptr                           sx_atomic_exchange32
#    define sx_atomic_compare_exchangeptr_weak              sx_atomic_compare_exchange32_weak
#    define sx_atomic_compare_exchangeptr_strong            sx_atomic_compare_exchange32_strong
#    define sx_atomic_fetch_addptr_explicit                 sx_atomic_fetch_add32_explicit
#    define sx_atomic_fetch_subptr_explicit                 sx_atomic_fetch_sub32_explicit
#    define sx_atomic_fetch_orptr_explicit                  sx_atomic_fetch_or32_explicit
#    define sx_atomic_fetch_andptr_explicit                 sx_atomic_fetch_and32_explicit
#    define sx_atomic_exchangeptr_explicit                  sx_atomic_exchange32_explicit
#    define sx_atomic_compare_exchangeptr_weak_explicit     sx_atomic_compare_exchange32_weak_explicit
#    define sx_atomic_compare_exchangeptr_strong_explicit   sx_atomic_compare_exchange32_strong_explicit#endif    // SX_ARCH_64BIT
#endif

#ifndef SX_ATOMIC_H_
#   error "This file must only be included from atomic.h"
#endif

#include "../platform.h"

#if SX_PLATFORM_WINDOWS
// FIXME: I got wierd compiler error on MSVC+Clang_c2, so I had to comment this out
//        Every source must include <windows.h> before including atomic.h
//#   if !SX_COMPILER_CLANG
#       include <intrin.h>
//#   endif
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
#	include <emmintrin.h> // _mm_sfence
#   endif

inline void sx_yield_cpu()
{
    _mm_pause();
}

inline void sx_memory_barrier()
{
    _mm_mfence();
}

inline void sx_memory_read_barrier()
{
    _mm_lfence();
}

inline void sx_memory_write_barrier()
{
    _mm_sfence();
}

inline void sx_compiler_barrier()
{
    _ReadWriteBarrier();
}

inline void sx_compiler_read_barrier()
{
    _ReadBarrier();
}

inline void sx_compiler_write_barrier()
{
    _WriteBarrier();
}

inline int sx_atomic_fetch_add(sx_atomic_int* a, int b)
{
    return _InterlockedExchangeAdd((LONG volatile*)a, b);
}

inline int sx_atomic_incr(sx_atomic_int* a)
{
    return _InterlockedIncrement((LONG volatile*)a);
}

inline int sx_atomic_decr(sx_atomic_int* a)
{
    return _InterlockedDecrement((LONG volatile*)a);
}

inline int sx_atomic_xchg(sx_atomic_int* a, int b)
{
    return _InterlockedExchange((LONG volatile*)a, b);
}

inline int sx_atomic_cas(sx_atomic_int* a, int xchg, int comparand)
{
    return _InterlockedCompareExchange((LONG volatile*)a, xchg, comparand);
}

inline void* sx_atomic_xchg_ptr(sx_atomic_ptr* a, void* b)
{
    return _InterlockedExchangePointer((PVOID volatile*)a, b);
}

inline void* sx_atomic_cas_ptr(sx_atomic_ptr* a, void* xchg, void* comparand)
{
    return _InterlockedCompareExchangePointer((PVOID volatile*)a, xchg, comparand);
}

inline void sx_lock(sx_lock_t* lock)
{
    while (!sx_trylock(lock))
        sx_yield_cpu();
}

inline void sx_unlock(sx_lock_t* lock)
{
    sx_compiler_barrier();
    *lock = 0;
}

inline bool sx_trylock(sx_lock_t* lock)
{
    int r = sx_atomic_xchg(lock, 1);
    sx_compiler_barrier();
    return r == 0;
}


#elif SX_COMPILER_GCC || SX_COMPILER_CLANG
inline void sx_yield_cpu()
{
#   if SX_CPU_X86
    __asm__ __volatile__("pause");
#   elif SX_CPU_ARM
    __asm__ __volatile__("yield");
#   endif
}

inline void sx_memory_barrier()
{
    __sync_synchronize();
}
#endif


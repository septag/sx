//
// Copyright 2018 Sepehr Taghdisian (septag@github). All rights reserved.
// License: https://github.com/septag/sx#license-bsd-2-clause
//
// sx.h - v1.0 - Main sx lib entry include file
//               Contains essential stdc includes and library definitions
//
#pragma once

#ifndef SX_SX_H_
#define SX_SX_H_

#include "config.h"

#include <alloca.h>         // alloca
#include <stdarg.h>         // va_list
#include <stdint.h>         // uint32_t
#include <stdbool.h>        // bool
#include <stdlib.h>         // size_t
#include <stddef.h>         // ptrdiff_t
#include <assert.h>         // assert / static_assert
#include <string.h>         // sx_memset

#include "platform.h"
#include "macros.h"

#if !SX_PLATFORM_WINDOWS
#   include <stdnoreturn.h> // noreturn
#else
#   define noreturn 
#endif

#if defined(__cplusplus)
#   if !defined(typeof)
#       define typeof(a) decltype(a)
#   endif
#else
#   ifndef static_assert
#       define static_assert _Static_assert
#   endif
#endif

#define sx_memcpy   memcpy
#define sx_memmove  memmove
#define sx_memset   memset

/// 
/// Intellisense and MSCV editor complains about not returning expression on macros that have {()}
#if SX_COMPILER_GCC || SX_COMPILER_CLANG
#define sx_swap(a, b)  do { \
            typeof(a) __a = (a);  \
            typeof(b) __b = (b);  \
            (void) (&__a == &__b); \
            a = __b;     b = __a; } while(0)

#   define sx_max(a, b)  ({ \
            typeof(a) __a = (a);  \
            typeof(b) __b = (b);  \
            (void) (&__a == &__b); \
            __a > b ? __a : __b; })

#   define sx_min(a, b)  ({ \
            typeof(a) __a = (a);  \
            typeof(b) __b = (b);  \
            (void) (&__a == &__b); \
            __a < b ? __a : __b; })

#   define sx_clamp(v_, min_, max_) ({    \
            typeof(v_)   _v = (v_);       \
            typeof(min_) _min = (min_);   \
            typeof(max_) _max = (max_);   \
            (void) (&_min == &_max);      \
            _v = _v < _max ? _v : _max;   \
            _v > _min ? _v : _min; })
#elif SX_COMPILER_MSVC
// In MSVC, it is likely built with C++, because C11 is not supported
//          So, Use templates instead of macros in that case
//          We do this To evaluate the inputs
#   ifdef __cplusplus 
template<typename Ty>
SX_INLINE void sx_swap(Ty& _a, Ty& _b)
{
    Ty tmp = _a; _a = _b; _b = tmp;
}

template<typename Ty>
SX_INLINE constexpr Ty sx_min(const Ty& _a, const Ty& _b)
{
    return _a < _b ? _a : _b;
}

template<typename Ty>
SX_INLINE constexpr Ty sx_max(const Ty& _a, const Ty& _b)
{
    return _a > _b ? _a : _b;
}

template<typename Ty>
SX_INLINE constexpr Ty sx_clamp(const Ty& _a, const Ty& _min, const Ty& _max)
{
    return sx_max(sx_min(_a, _max), _min);
}
#   else
#       error "typeof in macros Not supported on this compiler, try to build with C++ instead of C"
#       define sx_swap(a, b)             do { a = b; b = a; /* INVALID */ } while (0)
#       define sx_max(a, b)              ((a) > (b) ? a : b)
#       define sx_min(a, b)              ((a) < (b) ? a : b)
#       define sx_clamp(v, min_, max_)   sx_max(sx_min((v), (max_)), (min_))
#   endif   // __cplusplus
#endif

#endif  // SX_SX_H_


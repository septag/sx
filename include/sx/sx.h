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
#include <assert.h>         // assert

#include "platform.h"
#include "macros.h"

#if defined(__cplusplus)
#   if !defined(typeof)
#       define typeof(a) decltype(a)
#   endif
#else
// static_assert doesn't do anything in MSVC + C compiler, because we just don't have it !
#   ifndef static_assert
#       if SX_COMPILER_MSVC
#           define static_assert(_e, _msg)  
#       else
#           define static_assert(_e, _msg) _Static_assert(_e, _msg)
#       endif
#   endif
#endif

// Some libc function overrides
// Use sx_ versions in the code and override if required
#ifndef sx_assert
#   define sx_assert(_e)                assert(_e)
#endif

#ifndef sx_memset
#   include <string.h>         // memset
#   define sx_memset(_dst, _n, _sz)     memset(_dst, _n, _sz)
#endif

#ifndef sx_memcpy 
#   include <string.h>         // memcpy
#   define sx_memcpy(_dst, _src, _n)    memcpy(_dst, _src, _n)
#endif

#ifndef sx_memmove
#   include <string.h>         // memmove
#   define sx_memmove(_dst, _src, _n)   memmove(_dst, _src, _n)
#endif

#ifndef sx_memcmp
#   include <string.h>         // memcmp
#   define sx_memcmp(_p1, _p2, _n)      memcmp(_p1, _p2, _n)
#endif

/// 
/// Intellisense and MSCV editor complains about not returning expression on macros that have {()}
#if SX_COMPILER_GCC || SX_COMPILER_CLANG
#define sx_swap(a, b, _type)  do { _type tmp = a; a = b; b = tmp; } while (0)
#   define sx_max(a, b)  ({                 \
            typeof(a) __a = (a);            \
            typeof(b) __b = (b);            \
            (void) (&__a == &__b);          \
            __a > b ? __a : __b; })

#   define sx_min(a, b)  ({                 \
            typeof(a) __a = (a);            \
            typeof(b) __b = (b);            \
            (void) (&__a == &__b);          \
            __a < b ? __a : __b; })

#   define sx_clamp(v_, min_, max_) ({      \
            typeof(v_)   _v = (v_);         \
            typeof(min_) _min = (min_);     \
            typeof(max_) _max = (max_);     \
            (void) (&_min == &_max);        \
            _v = _v < _max ? _v : _max;     \
            _v > _min ? _v : _min; })
#elif SX_COMPILER_MSVC
// NOTE: Because we have some features lacking in MSVC+C compiler, the max,min,clamp macros does not pre-evaluate the expressions
// So in performance critical code, make sure you re-evaluate the sx_max, sx_min, sx_clamp paramters before passing them to the macros
#       define sx_swap(a, b, _type)      do { _type tmp = a; a = b; b = tmp; } while (0)
#       define sx_max(a, b)              ((a) > (b) ? (a) : (b))
#       define sx_min(a, b)              ((a) < (b) ? (a) : (b))
#       define sx_clamp(v, min_, max_)   sx_max(sx_min((v), (max_)), (min_))
#endif  

#endif  // SX_SX_H_


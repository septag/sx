//
// Copyright 2018 Sepehr Taghdisian (septag@github). All rights reserved.
// License: https://github.com/septag/sx#license-bsd-2-clause
//
// macros.h - v1.0 - Common portable helper macros
// Many of these are stolen from: https://github.com/bkaradzic/bx/blob/master/include/bx/macros.h
//      General rule is that function like macros are in camel_case 
//      and simple preprocessor/code annotations are in UPPER_CASE
#pragma once

#ifndef SX_MACROS_H_
#define SX_MACROS_H_

#include "platform.h"

///
#define sx_makefourcc(_a, _b, _c, _d) ( ( (uint32_t)(_a) | ( (uint32_t)(_b) << 8) | \
                                      ( (uint32_t)(_c) << 16) | ( (uint32_t)(_d) << 24) ) )

///
#define SX_STRINGIZE(_x) SX_STRINGIZE_(_x)
#define SX_STRINGIZE_(_x) #_x

///
// Function decleration code helpers

// Default parameter value, only for C++ and kind of a self documenting for C
#ifdef __cplusplus
#define SX_DFLT(_e) = (_e)
#else
#define SX_DFLT(_e)
#endif

// Argument is out (pointer/reference)
#define SX_OUT      
// Argument is in/out (pointer/reference)
#define SX_INOUT    
// Function returns new pointer (pointer), ptr must be freed by user     
#define SX_NEW_PTR  

///
#define sx_align_mask(_value, _mask) ( ( (_value)+(_mask) ) & ( (~0)&(~(_mask) ) ) )
#define sx_align_16(_value) SX_ALIGN_MASK(_value, 0xf)
#define sx_align_256(_value) SX_ALIGN_MASK(_value, 0xff)
#define sx_align_4096(_value) SX_ALIGN_MASK(_value, 0xfff)

#if defined(__has_feature)
#	define sx_clang_has_feature(_x) __has_feature(_x)
#else
#	define sx_clang_has_feature(_x) 0
#endif // defined(__has_feature)

#if defined(__has_extension)
#	define sx_clang_has_extension(_x) __has_extension(_x)
#else
#	define sx_clang_has_extension(_x) 0
#endif // defined(__has_extension)

#if SX_COMPILER_GCC || SX_COMPILER_CLANG
#	define SX_ALIGN_DECL(_align, _decl) _decl __attribute__( (aligned(_align) ) )
#	define SX_ALLOW_UNUSED              __attribute__( (unused) )
#	define SX_FORCE_INLINE              static inline __attribute__( (__always_inline__) )
#	define SX_FUNCTION                  __PRETTY_FUNCTION__
#	define SX_LIKELY(_x)                __builtin_expect(!!(_x), 1)
#	define SX_UNLIKELY(_x)              __builtin_expect(!!(_x), 0)
#	define SX_NO_INLINE                 __attribute__( (noinline) )
#	define SX_NO_RETURN                 __attribute__( (noreturn) )
#	define SX_CONSTFN                   __attribute__( (const) )
#   define SX_INLINE                    static inline
#	if SX_COMPILER_GCC >= 70000
#		define SX_FALLTHROUGH __attribute__( (fallthrough) )
#	else
#		define SX_FALLTHROUGH SX_NOOP()
#	endif // SX_COMPILER_GCC >= 70000

#	define SX_PRINTF_ARGS(_format, _args) __attribute__( (format(__printf__, _format, _args) ) )

#	define SX_ATTRIBUTE(_x) __attribute__( (_x) )

#	if SX_CRT_MSVC
#		define __stdcall
#	endif // SX_CRT_MSVC
#elif SX_COMPILER_MSVC
#	define SX_ALIGN_DECL(_align, _decl) __declspec(align(_align) ) _decl
#	define SX_ALLOW_UNUSED
#	define SX_FORCE_INLINE              __forceinline
#	define SX_FUNCTION                  __FUNCTION__
#	define SX_LIKELY(_x)                (_x)
#	define SX_UNLIKELY(_x)              (_x)
#	define SX_NO_INLINE                 __declspec(noinline)
#	define SX_NO_RETURN
#	define SX_CONSTFN                   __declspec(noalias)
#	define SX_FALLTHROUGH SX_NOOP()
#	define SX_PRINTF_ARGS(_format, _args)
#	define SX_ATTRIBUTE(_x)
#   define SX_INLINE                    static inline
#else
#	error "Unknown SX_COMPILER_?"
#endif

#if SX_COMPILER_CLANG
#	define SX_PRAGMA_DIAGNOSTIC_PUSH_CLANG_()     _Pragma("clang diagnostic push")
#	define SX_PRAGMA_DIAGNOSTIC_POP_CLANG_()      _Pragma("clang diagnostic pop")
#	define SX_PRAGMA_DIAGNOSTIC_IGNORED_CLANG(_x) _Pragma(SX_STRINGIZE(clang diagnostic ignored _x) )
#else
#	define SX_PRAGMA_DIAGNOSTIC_PUSH_CLANG_()
#	define SX_PRAGMA_DIAGNOSTIC_POP_CLANG_()
#	define SX_PRAGMA_DIAGNOSTIC_IGNORED_CLANG(_x)
#endif // SX_COMPILER_CLANG

#if SX_COMPILER_GCC && SX_COMPILER_GCC >= 40600
#	define SX_PRAGMA_DIAGNOSTIC_PUSH_GCC_()       _Pragma("GCC diagnostic push")
#	define SX_PRAGMA_DIAGNOSTIC_POP_GCC_()        _Pragma("GCC diagnostic pop")
#	define SX_PRAGMA_DIAGNOSTIC_IGNORED_GCC(_x)   _Pragma(SX_STRINGIZE(GCC diagnostic ignored _x) )
#else
#	define SX_PRAGMA_DIAGNOSTIC_PUSH_GCC_()
#	define SX_PRAGMA_DIAGNOSTIC_POP_GCC_()
#	define SX_PRAGMA_DIAGNOSTIC_IGNORED_GCC(_x)
#endif // SX_COMPILER_GCC

#if SX_COMPILER_MSVC
#	define SX_PRAGMA_DIAGNOSTIC_PUSH_MSVC_()     __pragma(warning(push) )
#	define SX_PRAGMA_DIAGNOSTIC_POP_MSVC_()      __pragma(warning(pop) )
#	define SX_PRAGMA_DIAGNOSTIC_IGNORED_MSVC(_x) __pragma(warning(disable:_x) )
#else
#	define SX_PRAGMA_DIAGNOSTIC_PUSH_MSVC_()
#	define SX_PRAGMA_DIAGNOSTIC_POP_MSVC_()
#	define SX_PRAGMA_DIAGNOSTIC_IGNORED_MSVC(_x)
#endif // SX_COMPILER_MSVC

#if SX_COMPILER_CLANG
#	define SX_PRAGMA_DIAGNOSTIC_PUSH              SX_PRAGMA_DIAGNOSTIC_PUSH_CLANG_
#	define SX_PRAGMA_DIAGNOSTIC_POP               SX_PRAGMA_DIAGNOSTIC_POP_CLANG_
#	define SX_PRAGMA_DIAGNOSTIC_IGNORED_CLANG_GCC SX_PRAGMA_DIAGNOSTIC_IGNORED_CLANG
#elif SX_COMPILER_GCC
#	define SX_PRAGMA_DIAGNOSTIC_PUSH              SX_PRAGMA_DIAGNOSTIC_PUSH_GCC_
#	define SX_PRAGMA_DIAGNOSTIC_POP               SX_PRAGMA_DIAGNOSTIC_POP_GCC_
#	define SX_PRAGMA_DIAGNOSTIC_IGNORED_CLANG_GCC SX_PRAGMA_DIAGNOSTIC_IGNORED_GCC
#elif SX_COMPILER_MSVC
#	define SX_PRAGMA_DIAGNOSTIC_PUSH              SX_PRAGMA_DIAGNOSTIC_PUSH_MSVC_
#	define SX_PRAGMA_DIAGNOSTIC_POP               SX_PRAGMA_DIAGNOSTIC_POP_MSVC_
#	define SX_PRAGMA_DIAGNOSTIC_IGNORED_CLANG_GCC(_x)
#endif // SX_COMPILER_

#ifdef __cplusplus
#   define SX_API extern "C" 
#else
#   define SX_API extern
#endif

#define SX_ENABLED(_f)  ((_f) != 0)
#define SX_UNUSED(_a)   (void)(true ? (void)0 : ( (void)(_a) ))

#ifdef __cplusplus
#   define SX_BOOL(_b)   (_b) ? true : false;
#else
#   define SX_BOOL(_b)    _b
#endif

#endif // SX_MACROS_H_
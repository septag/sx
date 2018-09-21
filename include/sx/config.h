//
// Copyright 2018 Sepehr Taghdisian (septag@github). All rights reserved.
// License: https://github.com/septag/sx#license-bsd-2-clause
//
#pragma once

#ifndef SX_CONFIG_H_
#define SX_CONFIG_H_

// Debug is forced to be off, so we undef _DEBUG if it's already defined
#if defined(SX_DEBUG) && !SX_DEBUG
#   ifdef _DEBUG
#       undef _DEBUG
#   endif
#endif

#if defined(_DEBUG)  || (defined(SX_DEBUG) && SX_DEBUG)
#   ifndef SX_CONFIG_DEBUG_ALLOCATOR
#       define SX_CONFIG_DEBUG_ALLOCATOR 1
#   endif

// There is an issue with msvc+clang_c2 where NDEBUG (and some other release flags) are always defined
#   ifdef NDEBUG
#       undef NDEBUG
#   endif

#   ifndef SX_DEBUG
#       define SX_DEBUG 1
#   endif
#endif

#ifndef SX_DEBUG
#   define SX_DEBUG 0
#endif

#ifndef SX_CONFIG_DEBUG_ALLOCATOR
#   define SX_CONFIG_DEBUG_ALLOCATOR 0
#endif

// Natural aligment is the default memory alignment for each platform
// All memory allocators aligns pointers to this value if 'align' parameter is less than natural alignment
#ifndef SX_CONFIG_ALLOCATOR_NATURAL_ALIGNMENT
#	define SX_CONFIG_ALLOCATOR_NATURAL_ALIGNMENT 8
#endif // BX_CONFIG_ALLOCATOR_NATURAL_ALIGNMENT

// Inserts code for hash-table debugging, used only for efficiency tests, see hash.h
#ifndef SX_CONFIG_HASHTBL_DEBUG
#   define SX_CONFIG_HASHTBL_DEBUG 1
#endif

// Use stdc math lib for basic math functions, see math.h
#ifndef SX_CONFIG_STDMATH
#   define SX_CONFIG_STDMATH 1
#endif

// Define anything here, for out of memory exceptions (exit/assert/...)
#ifndef sx_out_of_memory
#   define sx_out_of_memory()       do { sx_assert(0 && "Out of memory!");  exit(-1); } while(0)
#endif

#ifndef sx_data_truncate
#   define sx_data_truncate()       sx_assert(0 && "Data truncated !")
#endif


#if defined(_MSC_VER) && 0
// Macros for stdint.h definitions
// There are some problems with intellisense+gcc and I had to define these (only works in editor, the compiler defines them by default)
// TODO: make some of them with cmake --config
#ifndef __INT32_TYPE__
#   define __INT32_TYPE__ int
#endif

#ifndef __UINT32_TYPE__
#   define __UINT32_TYPE__ unsigned int
#endif

#ifndef __INT64_TYPE__
#   ifdef _MSC_VER
#       define __INT64_TYPE__ __int64
#   else
#       define __INT64_TYPE__ long long
#   endif
#endif

#ifndef __UINT64_TYPE__
#   ifdef _MSC_VER
#       define __UINT64_TYPE__ unsigned __int64
#   else
#       define __UINT64_TYPE__ unsigned long long
#   endif
#endif

#ifndef __INT8_TYPE__
#   define __INT8_TYPE__ char
#endif

#ifndef __UINT8_TYPE__
#   define __UINT8_TYPE__ unsigned char
#endif

#ifndef __INT16_TYPE__
#   define __INT16_TYPE__ short
#endif

#ifndef __UINT16_TYPE__ 
#   define __UINT16_TYPE__ unsigned short
#endif

#ifndef __INTPTR_WIDTH__
#   if defined(__x86_64__) || defined(_M_X64) || defined(__aarch64__) || defined(__64BIT__) || defined(__LP64__)
#       define uintptr_t uint64_t
#       define intptr_t  int64_t
#   else
#       define uintptr_t uint32_t
#       define intptr_t  int32_t
#   endif
#endif

#ifndef __INT_MAX__
#   define __INT_MAX__ 2147483647
#endif

#ifndef __LONG_MAX__
#   if defined(__x86_64__) || defined(_M_X64) || defined(__aarch64__) || defined(__64BIT__) || defined(__LP64__)
#       define __LONG_MAX__ 9223372036854775807L
#   else
#       define __LONG_MAX__ 2147483647L
#   endif
#endif
#endif

#endif  // SX_CONFIG_H_

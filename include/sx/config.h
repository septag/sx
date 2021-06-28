//
// Copyright 2018 Sepehr Taghdisian (septag@github). All rights reserved.
// License: https://github.com/septag/sx#license-bsd-2-clause
//
#pragma once

// indicates that the library is a shared-dll
// If you are using the shared sx library, you should define SX_CONFIG_SHARED_LIB=1
// If you are building the sx library itself, there is no need to modify this, it must be defined by
// SX_SHARED_LIB cmake option
#ifndef SX_CONFIG_SHARED_LIB
#    define SX_CONFIG_SHARED_LIB 0
#endif

// Debug is forced to be off, so we undef _DEBUG if it's already defined
#if defined(SX_DEBUG) && !SX_DEBUG
#    ifdef _DEBUG
#        undef _DEBUG
#    endif
#endif

#if defined(_DEBUG) || (defined(SX_DEBUG) && SX_DEBUG)
#    ifndef SX_CONFIG_DEBUG_ALLOCATOR
#        define SX_CONFIG_DEBUG_ALLOCATOR 1
#    endif

#    ifndef SX_CONFIG_ENABLE_ASSERT
#        define SX_CONFIG_ENABLE_ASSERT 1
#    endif

// There is an issue with msvc+clang_c2 where NDEBUG (and some other release flags) are always
// defined
#    ifdef NDEBUG
#        undef NDEBUG
#    endif

#    ifndef SX_DEBUG
#        define SX_DEBUG 1
#    endif
#endif  // _DEBUG || SX_DEBUG

#ifndef SX_DEBUG
#    define SX_DEBUG 0
#endif

#ifndef SX_CONFIG_DEBUG_ALLOCATOR
#    define SX_CONFIG_DEBUG_ALLOCATOR 0
#endif

#ifndef SX_CONFIG_ENABLE_ASSERT
#    define SX_CONFIG_ENABLE_ASSERT 1
#endif

// define SX_CONFIG_DISABLE_ASSERT_ALWAYS=1 to disable sx_assert_always 
#ifndef SX_CONFIG_DISABLE_ASSERT_ALWAYS
#    define SX_CONFIG_DISABLE_ASSERT_ALWAYS 0
#endif

// Natural aligment is the default memory alignment for each platform
// All memory allocators aligns pointers to this value if 'align' parameter is less than natural
// alignment.
// MacOS/iOS devices seems to be 16-byte aligned by default:
// https://developer.apple.com/library/archive/documentation/Performance/Conceptual/ManagingMemory/Articles/MemoryAlloc.html
#ifndef SX_CONFIG_ALLOCATOR_NATURAL_ALIGNMENT
#    if defined(__APPLE__) && defined(__MACH__)
#        define SX_CONFIG_ALLOCATOR_NATURAL_ALIGNMENT 16
#    else
#        define SX_CONFIG_ALLOCATOR_NATURAL_ALIGNMENT 8
#    endif
#endif    // BX_CONFIG_ALLOCATOR_NATURAL_ALIGNMENT

// Inserts code for hash-table debugging, used only for efficiency tests, see hash.h
#ifndef SX_CONFIG_HASHTBL_DEBUG
#    define SX_CONFIG_HASHTBL_DEBUG 1
#endif

// Use stdc math lib for basic math functions, see math.h
#ifndef SX_CONFIG_STDMATH
#    define SX_CONFIG_STDMATH 1
#endif

#ifndef SX_CONFIG_HANDLE_GEN_BITS
#    define SX_CONFIG_HANDLE_GEN_BITS 14
#endif

#ifndef SX_CONFIG_SIMD_DISABLE
#    define SX_CONFIG_SIMD_DISABLE 0
#endif

#ifndef SX_CONFIG_ARRAY_INIT_SIZE
#   define SX_CONFIG_ARRAY_INIT_SIZE 8
#endif

// This preprocessor applies a trick for hot functions that need to be inlined even in debug builds
// only applies to UC_ALWAYS_INLINE functions, but leaves SX_INLINE not inlined
// And also, the build should be compiled with /Ob1 flag
#ifndef SX_CONFIG_FORCE_INLINE_DEBUG
#   define SX_CONFIG_FORCE_INLINE_DEBUG 0
#endif

#ifndef SX_CONFIG_INCLUDE_BANNED 
#   define SX_CONFIG_INCLUDE_BANNED 0
#endif

#ifndef SX_CONFIG_OBSOLETE_CODE
#   define SX_CONFIG_OBSOLETE_CODE 1
#endif
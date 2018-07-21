// allocator.h - v1.0 - Memory allocation routines and helper functions
//      sx_alloc: main allocation primitive, all memory allocation function must accept this object instead of malloc/free
//                Custom implementaion is only one 'alloc_cb' function (see sx_alloc), and a user_data that is to store
//                any custom data for user allocators.
//                'alloc_cb' provides all three main allocation function inside of itself which is malloc/realloc/free
//                Here's an example minimal custom implementation:
//                      void my_alloc(void* ptr, size_t size, size_t align, const char* file, uint32_t line, void* user_data) {
//                          // 'file' can be NULL in none-debug builds
//                          // if 'align' is zero or less than SX_CONFIG_ALLOCATOR_NATURAL_ALIGNMENT, no alignment is needed
//                          if (size == 0) { free(ptr);  return NULL; } 
//                          else if (ptr == NULL)   { return malloc(size); }    // TODO: handle alignment cases
//                          else { return realloc(ptr, size); }    // TODO: handle alignment cases
//                      }
//
//     sx_alloc, sx_realloc, sx_free, sx_aligned_alloc, sx_aligned_realloc, sx_aligned_free:
//          These macros provide helper to allocation functions, they all require a valid allocator
//          They also skip passing __FILE__ and __LINE__ if SX_CONFIG_DEBUG_ALLOCATOR=0s
//     
//     sx_is_aligned: checks if pointer is aligned to specified alignment
//     sx_align_ptr:  offsets the pointer forward to aligned address with 'extra' bytes reserved
//     sx_alloc_malloc: normal heap (malloc/free) allocator
//     sx_alloc_malloc_leak_detect: normal heap (malloc/free) allocator but with extra leak detection
//                                  use sx_dump_leaks and the end of the program to dump possible memory leaks
//
#pragma once

#ifndef SX_ALLOCATOR_H_
#define SX_ALLOCATOR_H_

#include "sx.h"

#if SX_CONFIG_DEBUG_ALLOCATOR
#	define sx_malloc(_allocator, _size)                        sx__malloc(_allocator, _size, 0, __FILE__, __LINE__)
#	define sx_realloc(_allocator, _ptr, _size)                 sx__realloc(_allocator, _ptr, _size, 0, __FILE__, __LINE__)
#	define sx_free(_allocator, _ptr)                           sx__free(_allocator, _ptr, 0, __FILE__, __LINE__)
#	define sx_aligned_malloc(_allocator, _size, _align)        sx__malloc(_allocator, _size, _align, __FILE__, __LINE__)
#	define sx_aligned_realloc(_allocator, _ptr, _size, _align) sx__realloc(_allocator, _ptr, _size, _align, __FILE__, __LINE__)
#	define sx_aligned_free(_allocator, _ptr, _align)           sx__free(_allocator, _ptr, _align, __FILE__, __LINE__)
#else
#	define sx_malloc(_allocator, _size)                        sx__malloc(_allocator, _size, 0, NULL, 0)
#	define sx_realloc(_allocator, _ptr, _size)                 sx__realloc(_allocator, _ptr, _size, 0, NULL, 0)
#	define sx_free(_allocator, _ptr)                           sx__free(_allocator, _ptr, 0, NULL, 0)
#	define sx_aligned_malloc(_allocator, _size, _align)        sx__malloc(_allocator, _size, _align, NULL, 0)
#	define sx_aligned_realloc(_allocator, _ptr, _size, _align) sx__realloc(_allocator, _ptr, _size, _align, NULL, 0)
#	define sx_aligned_free(_allocator, _ptr, _align)           sx__free(_allocator, _ptr, _align, NULL, 0)
#endif // SX_CONFIG_ALLOCATOR_DEBUG

#ifdef __cplusplus
extern "C" {
#endif

typedef struct sx_alloc 
{
    void* (*alloc_cb)(void* ptr, size_t size, size_t align, const char* file, uint32_t line, void* user_data);
    void* user_data;
} sx_alloc;

inline bool  sx_is_aligned(const void* ptr, size_t align);
inline void* sx_align_ptr(void* ptr, size_t extra, size_t align);

// Default allocator: allocate from heap
const sx_alloc* sx_alloc_malloc;

// Leak checking allocator, useful for debug and SX_CONFIG_DEBUG_ALLOCATOR=1
//sx_alloc* sx_alloc_malloc_leak_detect();
const sx_alloc* sx_alloc_malloc_leak_detect;
void sx_dump_leaks();

#include "inline/allocator.inl"

#ifdef __cplusplus
}
#endif

#endif // SX_ALLOCATOR_H_
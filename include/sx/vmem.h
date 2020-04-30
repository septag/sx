//
// Copyright 2018 Sepehr Taghdisian (septag@github). All rights reserved.
// License: https://github.com/septag/sx#license-bsd-2-clause
//
// virtual-alloc.h - v1.0 - Virtual memory allocator
//
// Uses OS specific virtual memory functions
//
// virtualalloc: standard sequential virtual memory allocator
//               Virtual memory can be reserved in large sizes and commited later on demand
//               Very useful for large buffer allocations, instead of allocating directly from heap
//               Memory is only allocate sequentially and will not be reused
//               so the limitation is not to hit the 'reserve_sz' max
//
//      sx_virtualalloc_init        Initialize and reserves virtual memory for the allocator
//                                  Reserved size will not be commited until it's alloacated and
//                                  touched 'reserved_sz' value will grow to OS-specific aligned
//                                  page size
//
//      sx_virtualalloc_release     Releases memory commited and reserved by virtual-allocator
//
// NOTE: When you use the sx_alloc object for allocating, the memory will be commited and you will
//       get a valid pointer. But still the memory is not allocated from heap until you touch the
//       contents of the memory block.
//       Also the allocation sizes will grow to OS-specific aligned page size (for example 4k),
//       .. so try to allocate in larger chunks from this type of allocator
//
#pragma once

#include "sx.h"

typedef enum sx_vmem_flag {
    SX_VMEM_WATCH = 0x1
} sx_vmem_flag;
typedef uint32_t sx_vmem_flags;

typedef struct sx_vmem_context {
    void* ptr;
    int num_pages;
    int page_size;
    int max_pages;
} sx_vmem_context;

SX_API size_t sx_vmem_get_bytes(int num_pages);
SX_API int sx_vmem_get_needed_pages(size_t bytes);

SX_API bool sx_vmem_init(sx_vmem_context* vmem, sx_vmem_flags flags, int max_pages);
SX_API void sx_vmem_release(sx_vmem_context* vmem);
SX_API void* sx_vmem_commit_page(sx_vmem_context* vmem, int page_id);
SX_API void sx_vmem_free_page(sx_vmem_context* vmem, int page_id);
SX_API void* sx_vmem_commit_pages(sx_vmem_context* vmem, int start_page_id, int num_pages);
SX_API void sx_vmem_free_pages(sx_vmem_context* vmem, int start_page_id, int num_pages);
SX_API void* sx_vmem_get_page(sx_vmem_context* vmem, int page_id);
SX_API size_t sx_vmem_commit_size(sx_vmem_context* vmem);
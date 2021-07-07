//
// Copyright 2020 Sepehr Taghdisian (septag@github). All rights reserved.
// License: https://github.com/septag/sx#license-bsd-2-clause
//
// vmem.h - v1.0 - Virtual memory page allocator
//
// Uses OS specific virtual memory functions
// the vmem set of functions can be used to reserve and commit protected pages of virtual memory 
// from the system. It works by reserving maximum number of pages. So the pointer range will be 
// reserved, but not actually mapped to physical memory.
// After init, you can use commit_page/commit_pages functions to start using specific pages. 
// Pages are defined by their index. So, if you have for example, 4096 maximum pages. You can commit
// and free pages from 0..4095, and you should keep track of free/allocated pages in your program.
// Page memory size is OS/System dependent. You can actually get it's size with `sx_os_pagesz` 
//
//  Functions: 
//      sx_vmem_get_bytes(num_pages)        returns number of bytes allocated for N pages
//      sx_vmem_get_needed_pages(bytes)     returns number of needed pages to N bytes
//                                          you can pass the return value to sx_vmem_init function
//      sx_vmem_init                        initialize the vmem context and reserve `max_pages`
//      sx_vmem_release                     Releases the vmem context and frees all memory
//      sx_vmem_commit_page/pages           Commits `num_pages` that you need to actually allocate 
//                                          and use. Returns the pointer to the start of pages
//                                          start_page_id: Zero based index of the page you want
//                                          num_pages: Number of pages starting from `start_page_id`
//      sx_vmem_free_page/pages             Free a single or a range of pages to be used later again
//                                          Parameters are same as commit functions
//      sx_vmem_commit_size                 Returns total commited bytes. 
//                                          Basically num_pages*page_size
//
#pragma once

#include "sx.h"

typedef struct sx_alloc sx_alloc;

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

typedef struct sx_vmem_watch_result {
    void* ptrs;
    int   num_ptrs;
    const sx_alloc* alloc;
} sx_vmem_watch_result;

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

SX_API sx_vmem_watch_result sx_vmem_watch_writes(sx_vmem_context* vmem, const sx_alloc* alloc, bool clear);
SX_API void sx_vmem_watch_clear(sx_vmem_context* vmem);
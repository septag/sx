//
// Copyright 2018 Sepehr Taghdisian (septag@github). All rights reserved.
// License: https://github.com/septag/sx#license-bsd-2-clause
//

#include "sx/vmem.h"
#include "sx/os.h"
#include "sx/allocator.h"

#if SX_PLATFORM_WINDOWS
#    define VC_EXTRALEAN
#    define WIN32_LEAN_AND_MEAN
SX_PRAGMA_DIAGNOSTIC_PUSH()
SX_PRAGMA_DIAGNOSTIC_IGNORED_MSVC(5105)
#    include <windows.h>
SX_PRAGMA_DIAGNOSTIC_POP()
#elif SX_PLATFORM_POSIX
#    include <sys/mman.h>
#    include <unistd.h>
#    if defined(MAP_ANON) && !defined(MAP_ANONYMOUS)
#        define MAP_ANONYMOUS MAP_ANON
#    endif
#endif


size_t sx_vmem_get_bytes(int num_pages)
{
    return sx_os_pagesz() * num_pages;
}

int sx_vmem_get_needed_pages(size_t bytes)
{
    size_t page_sz = sx_os_pagesz();
    size_t page_cnt = (bytes + page_sz - 1) / page_sz;
    sx_assert(page_cnt < INT32_MAX);
    return (int)page_cnt;
}

#if SX_PLATFORM_WINDOWS
bool sx_vmem_init(sx_vmem_context* vmem, sx_vmem_flags flags, int max_pages)
{
    sx_assert(vmem);
    sx_assert(max_pages > 0);

    vmem->page_size = (int)sx_os_pagesz();
    vmem->num_pages = 0;
    vmem->max_pages = max_pages;
    vmem->ptr =
        VirtualAlloc(NULL, (size_t)vmem->page_size * (size_t)max_pages,
                     MEM_RESERVE | ((flags & SX_VMEM_WATCH) ? MEM_WRITE_WATCH : 0), PAGE_READWRITE);

    if (!vmem->ptr) {
        return false;
    }

    return true;
}

void sx_vmem_release(sx_vmem_context* vmem)
{
    sx_assert(vmem);

    if (vmem->ptr) {
        BOOL r = VirtualFree(vmem->ptr, 0, MEM_RELEASE);
        sx_unused(r);
        sx_assert(r);
    }
    vmem->num_pages = vmem->max_pages = 0;
}

void* sx_vmem_commit_page(sx_vmem_context* vmem, int page_id)
{
    sx_assert(vmem);
    sx_assert(vmem->ptr);
    
    if (page_id >= vmem->max_pages || vmem->num_pages == vmem->max_pages) {
        return NULL;
    }

    void* ptr = (uint8_t*)vmem->ptr + vmem->page_size * page_id;
    if (!VirtualAlloc(ptr, vmem->page_size, MEM_COMMIT, PAGE_READWRITE)) {
        return NULL;
    }

    ++vmem->num_pages;

    return ptr;
}

void sx_vmem_free_page(sx_vmem_context* vmem, int page_id)
{
    sx_assert(vmem);
    sx_assert(vmem->ptr);
    sx_assert(page_id < vmem->max_pages);
    sx_assert(vmem->num_pages > 0);

    void* ptr = (uint8_t*)vmem->ptr + vmem->page_size * page_id;
    BOOL r = VirtualFree(ptr, vmem->page_size, MEM_DECOMMIT);
    sx_unused(r);
    sx_assert(r);
    --vmem->num_pages;
}

void* sx_vmem_commit_pages(sx_vmem_context* vmem, int start_page_id, int num_pages)
{
    sx_assert(vmem);
    sx_assert(vmem->ptr);
    
    if ((start_page_id + num_pages) > vmem->max_pages ||
        (vmem->num_pages + num_pages) > vmem->max_pages)
    {
        return NULL;
    }

    void* ptr = (uint8_t*)vmem->ptr + vmem->page_size * start_page_id;
    if (!VirtualAlloc(ptr, (size_t)vmem->page_size*(size_t)num_pages, MEM_COMMIT, PAGE_READWRITE)) {
        return NULL;
    }
    vmem->num_pages += num_pages;

    return ptr;
}

void sx_vmem_free_pages(sx_vmem_context* vmem, int start_page_id, int num_pages)
{
    sx_assert(vmem);
    sx_assert(vmem->ptr);
    sx_assert((start_page_id + num_pages) <= vmem->max_pages);
    sx_assert(vmem->num_pages >= num_pages);

    if (num_pages > 0) {
        void* ptr = (uint8_t*)vmem->ptr + vmem->page_size * start_page_id;
        BOOL r = VirtualFree(ptr, (size_t)vmem->page_size*(size_t)num_pages, MEM_DECOMMIT);
        sx_unused(r);
        sx_assert(r);
        vmem->num_pages -= num_pages;
    }
}

sx_vmem_watch_result sx_vmem_watch_writes(sx_vmem_context* vmem, const sx_alloc* alloc, bool clear)
{
    sx_assert(vmem);
    sx_assert(alloc);

    DWORD flags = 0;
    if (clear) {
        flags |= WRITE_WATCH_FLAG_RESET;
    }

    void* ptrs = sx_malloc(alloc, sizeof(void*)*vmem->num_pages);
    if (!ptrs) {
        sx_memory_fail();
        return (sx_vmem_watch_result) {0};
    }

    ULONG_PTR num_ptrs;
    DWORD granuality;
    if (GetWriteWatch(flags, vmem->ptr, (size_t)vmem->page_size*(size_t)vmem->num_pages, ptrs, &num_ptrs, &granuality) == 0) {
        return (sx_vmem_watch_result) {
            .alloc = alloc,
            .ptrs = ptrs,
            .num_ptrs = (int)num_ptrs
        };
    } else {
        return (sx_vmem_watch_result) {0};
    }
}

void sx_vmem_watch_clear(sx_vmem_context* vmem)
{
    ResetWriteWatch(vmem->ptr, (size_t)vmem->page_size*(size_t)vmem->num_pages);
}

#elif SX_PLATFORM_POSIX // if SX_PLATFORM_WINDOWS

bool sx_vmem_init(sx_vmem_context* vmem, sx_vmem_flags flags, int max_pages)
{
    sx_assert(vmem);
    sx_assert(max_pages > 0);
    sx_unused(flags);

    vmem->page_size = (int)sx_os_pagesz();
    vmem->num_pages = 0;
    vmem->max_pages = max_pages;
    vmem->ptr = mmap(NULL, (size_t)vmem->page_size * (size_t)max_pages, PROT_NONE,
                     MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (vmem->ptr == MAP_FAILED) {
        vmem->ptr = NULL;
        return false;
    }

    return true;
}

void sx_vmem_release(sx_vmem_context* vmem)
{
    sx_assert(vmem);

    if (vmem->ptr) {
        munmap(vmem->ptr, (size_t)vmem->page_size * (size_t)vmem->max_pages);
    }
    vmem->num_pages = vmem->max_pages = 0;
}

void* sx_vmem_commit_page(sx_vmem_context* vmem, int page_id)
{
    sx_assert(vmem);
    sx_assert(vmem->ptr);
    
    if (page_id >= vmem->max_pages || vmem->num_pages == vmem->max_pages) {
        return NULL;
    }

    void* ptr = (uint8_t*)vmem->ptr + vmem->page_size * page_id;
    if (mprotect(ptr, vmem->page_size, PROT_READ | PROT_WRITE) != 0) {
        sx_assert_always(0);
        return NULL;
    }

    ++vmem->num_pages;
    return ptr;
}

void sx_vmem_free_page(sx_vmem_context* vmem, int page_id)
{
    sx_assert(vmem);
    sx_assert(vmem->ptr);
    sx_assert(page_id < vmem->max_pages);
    sx_assert(vmem->num_pages > 0);

    void* ptr = (uint8_t*)vmem->ptr + vmem->page_size * page_id;
    int r = munmap(ptr, vmem->page_size);
    madvise(ptr, vmem->page_size, MADV_DONTNEED);
    sx_unused(r);
    sx_assert(r == 0);
    --vmem->num_pages;
}

void* sx_vmem_commit_pages(sx_vmem_context* vmem, int start_page_id, int num_pages)
{
    sx_assert(vmem);
    sx_assert(vmem->ptr);
    
    if ((start_page_id + num_pages) > vmem->max_pages ||
        (vmem->num_pages + num_pages) > vmem->max_pages)
    {
        return NULL;
    }

    void* ptr = (uint8_t*)vmem->ptr + vmem->page_size * start_page_id;
    if (mprotect(ptr, (size_t)vmem->page_size*(size_t)num_pages, PROT_READ | PROT_WRITE) != 0) {
        sx_assert_always(0);
        return NULL;
    }

    vmem->num_pages += num_pages;
    return ptr;
}

void sx_vmem_free_pages(sx_vmem_context* vmem, int start_page_id, int num_pages)
{
    sx_assert(vmem);
    sx_assert(vmem->ptr);
    sx_assert((start_page_id + num_pages) <= vmem->max_pages);
    sx_assert(vmem->num_pages >= num_pages);

    if (num_pages > 0) {
        void* ptr = (uint8_t*)vmem->ptr + vmem->page_size * start_page_id;
        int r = madvise(ptr, (size_t)vmem->page_size*(size_t)num_pages, MADV_DONTNEED);
        sx_unused(r);
        sx_assert(r == 0);
        vmem->num_pages -= num_pages;
    }
}

sx_vmem_watch_result sx_vmem_watch_writes(sx_vmem_context* vmem, const sx_alloc* alloc, bool clear)
{
    sx_unused(clear);
    sx_unused(vmem);
    sx_unused(alloc);
    return (sx_vmem_watch_result) {0};
}

void sx_vmem_watch_clear(sx_vmem_context* vmem)
{
    sx_unused(vmem);    
}

#endif // elif SX_PLATFORM_POSIX

void* sx_vmem_get_page(sx_vmem_context* vmem, int page_id)
{
    sx_assert(vmem);
    sx_assert(vmem->ptr);
    sx_assert(page_id < vmem->max_pages);
    
    return (uint8_t*)vmem->ptr + vmem->page_size * page_id;
}

size_t sx_vmem_commit_size(sx_vmem_context* vmem)
{
    return (size_t)vmem->page_size * (size_t)vmem->num_pages;
}


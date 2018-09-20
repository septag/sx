//
// Copyright 2018 Sepehr Taghdisian (septag@github). All rights reserved.
// License: https://github.com/septag/sx#license-bsd-2-clause
//

#include "sx/virtual-alloc.h"

#if SX_PLATFORM_WINDOWS
#   define VC_EXTRALEAN
#   define WIN32_LEAN_AND_MEAN
#	include <windows.h>
#elif SX_PLATFORM_POSIX
#   include <unistd.h>
#   include <sys/mman.h>
#if defined(MAP_ANON) && !defined(MAP_ANONYMOUS)
#   define MAP_ANONYMOUS MAP_ANON
#endif
#endif

// TODO: posix version may have problems, test it

#include "sx/os.h"

void* sx_virtual_reserve(size_t reserve_sz)
{
    sx_assert(reserve_sz % sx_os_pagesz() == 0);
#if SX_PLATFORM_WINDOWS
    return VirtualAlloc(NULL, reserve_sz, MEM_RESERVE, PAGE_READWRITE);
#elif SX_PLATFORM_POSIX
    return mmap(NULL, reserve_sz, PROT_NONE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
#endif
}

void  sx_virtual_release(void* ptr)
{
#if SX_PLATFORM_WINDOWS
    VirtualFree(ptr, 0, MEM_RELEASE);
#elif SX_PLATFORM_POSIX
    munmap(ptr, 0);
#endif
}

void  sx_virtual_protect(void* ptr, size_t sz)
{

}

void* sx_virtual_commit(void* addr, size_t sz)
{
    int page_sz = sx_os_pagesz();
    sx_assert((uintptr_t)addr % page_sz == 0);
    sx_assert(sz % page_sz == 0);

#if SX_PLATFORM_WINDOWS
    return VirtualAlloc(addr, sz, MEM_COMMIT, PAGE_READWRITE);
#elif SX_PLATFORM_POSIX
    return mmap(addr, sz, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS|MAP_FIXED, -1, 0);
#endif
}

void  sx_virtual_decommit(void* addr, size_t sz)
{
    int page_sz = sx_os_pagesz();
    sx_assert((uintptr_t)addr % page_sz == 0);
    sx_assert(sz % page_sz == 0);

#if SX_PLATFORM_WINDOWS
    VirtualFree(addr, sz, MEM_DECOMMIT);
#elif SX_PLATFORM_POSIX
    munmap(addr, sz);
#endif
}

// TODO: VirtualProtect allocations

typedef struct sx__virtualalloc_hdr
{
    size_t      size;
    uint32_t    padding;
} sx__virtualalloc_hdr;

static void* sx__virtualalloc_malloc(sx_virtualalloc* valloc, size_t size, size_t align, const char* file, uint32_t line)
{
    align = sx_max((int)align, SX_CONFIG_ALLOCATOR_NATURAL_ALIGNMENT);
    const size_t total = sx_os_align_pagesz(size + sizeof(size_t) + align);
    if (valloc->offset + size <= valloc->reserved_sz) {
        // Commit the memory requested and save it's size
        uint8_t* ptr = (uint8_t*)sx_virtual_commit(valloc->ptr + valloc->offset, total);
        if (!ptr) {
            sx_out_of_memory();
            return NULL;
        }
        uint8_t* aligned = (uint8_t*)sx_align_ptr(ptr, sizeof(sx__virtualalloc_hdr), align);
        sx__virtualalloc_hdr* hdr = (sx__virtualalloc_hdr*)aligned - 1;
        hdr->size = total;
        hdr->padding = (uint32_t)(aligned - ptr);

        valloc->offset += size;

        return aligned;
    } else {
        sx_out_of_memory();
        return NULL;
    }
}

static void* sx__virtualalloc_cb(void* ptr, size_t size, size_t align, const char* file, uint32_t line, void* user_data)
{
    sx_virtualalloc* valloc = (sx_virtualalloc*)user_data;
    if (size == 0) {
        // free
        if (ptr) {
            sx__virtualalloc_hdr* hdr = (sx__virtualalloc_hdr*)ptr - 1;
            void* old_ptr = (uint8_t*)ptr - hdr->padding;
            sx_virtual_decommit(old_ptr, hdr->size);
        }
        return NULL;
    } else if (ptr == NULL) {
        // malloc
        return sx__virtualalloc_malloc(valloc, size, align, file, line);
    } else {
        // realloc
        void* new_ptr = sx__virtualalloc_malloc(valloc, size, align, file, line);
        if (new_ptr) {
            sx__virtualalloc_hdr* hdr = (sx__virtualalloc_hdr*)ptr - 1;
            sx_memcpy(new_ptr, ptr, sx_min(size, hdr->size));
            void* old_ptr = (uint8_t*)ptr - hdr->padding;
            sx_virtual_decommit(old_ptr, hdr->size);
        }
        return new_ptr;
    }
}

bool sx_virtualalloc_init(sx_virtualalloc* valloc, size_t reserve_sz)
{
    sx_assert(reserve_sz > 0);
    
    // Align size to pages
    reserve_sz = sx_os_align_pagesz(reserve_sz);

    valloc->alloc.alloc_cb = sx__virtualalloc_cb;
    valloc->alloc.user_data = valloc;
    valloc->ptr = (uint8_t*)sx_virtual_reserve(reserve_sz);
    if (!valloc->ptr) {
        sx_out_of_memory();
        return false;
    }
    valloc->offset = 0;
    valloc->reserved_sz = reserve_sz;
    return true;
}

void sx_virtualalloc_release(sx_virtualalloc* valloc)
{
    if (valloc->ptr) {
        sx_virtual_release(valloc->ptr);
        valloc->ptr = NULL;
    }
}

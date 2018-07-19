#include "sx/virtual-alloc.h"

#if SX_PLATFORM_WINDOWS
#   define VC_EXTRALEAN
#   define WIN32_LEAN_AND_MEAN
#	include <windows.h>
#endif

#include "sx/os.h"

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
        uint8_t* ptr = VirtualAlloc(valloc->ptr + valloc->offset, total, MEM_COMMIT, PAGE_READWRITE);
        if (!ptr) {
            SX_OUT_OF_MEMORY;
            return NULL;
        }
        uint8_t* aligned = (uint8_t*)sx_align_ptr(ptr, sizeof(sx__virtualalloc_hdr), align);
        sx__virtualalloc_hdr* hdr = (sx__virtualalloc_hdr*)aligned - 1;
        hdr->size = total;
        hdr->padding = (uint32_t)(aligned - ptr);

        valloc->offset += size;

        return aligned;
    } else {
        SX_OUT_OF_MEMORY;
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
            VirtualFree(old_ptr, hdr->size, MEM_DECOMMIT);
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
            memcpy(new_ptr, ptr, sx_min(size, hdr->size));
            void* old_ptr = (uint8_t*)ptr - hdr->padding;
            VirtualFree(old_ptr, hdr->size, MEM_DECOMMIT);
        }
        return new_ptr;
    }
}

bool sx_virtualalloc_init(sx_virtualalloc* valloc, size_t reserve_sz)
{
    assert(reserve_sz > 0);
    
    // Align size to pages
    reserve_sz = sx_os_align_pagesz(reserve_sz);

    valloc->alloc.alloc_cb = sx__virtualalloc_cb;
    valloc->alloc.user_data = valloc;
    valloc->ptr = (uint8_t*)VirtualAlloc(NULL, reserve_sz, MEM_RESERVE, PAGE_READWRITE);
    if (!valloc->ptr) {
        SX_OUT_OF_MEMORY;
        return false;
    }
    valloc->offset = 0;
    valloc->reserved_sz = reserve_sz;
    return true;
}

void sx_virtualalloc_release(sx_virtualalloc* valloc)
{
    if (valloc->ptr) {
        VirtualFree(valloc->ptr, 0, MEM_RELEASE);
        valloc->ptr = NULL;
    }
}

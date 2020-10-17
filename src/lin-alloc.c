//
// Copyright 2018 Sepehr Taghdisian (septag@github). All rights reserved.
// License: https://github.com/septag/sx#license-bsd-2-clause
//
#include "sx/lin-alloc.h"

typedef struct sx__linalloc_hdr_s {
    uint32_t size;       // size of buffer that requested upon allocation
} sx__linalloc_hdr;

static void* sx__linalloc_malloc(sx_linalloc* alloc, size_t size, uint32_t align)
{
    align = sx_max((int)align, SX_CONFIG_ALLOCATOR_NATURAL_ALIGNMENT);
    size_t total = size + sizeof(sx__linalloc_hdr);
    size_t new_offset = alloc->offset + sizeof(sx__linalloc_hdr);
    if (new_offset % align != 0) {
        size_t aligned_offset = sx_align_mask(new_offset, (size_t)align-1);
        total += (aligned_offset - new_offset);
        new_offset = aligned_offset;
    }

    if ((new_offset + size) > alloc->size) {
        sx_out_of_memory();
        return NULL;
    }

    uint8_t* aligned = alloc->ptr + new_offset;
    sx_assert((uintptr_t)aligned % align == 0);

    // Fill header info
    sx__linalloc_hdr* hdr = (sx__linalloc_hdr*)aligned - 1;
    hdr->size = (uint32_t)size;

    alloc->offset = new_offset + size;
    alloc->peak = sx_max(alloc->peak, alloc->offset);

    return aligned;
}

static void* sx__linalloc_cb(void* ptr, size_t size, uint32_t align, const char* file,
                             const char* func, uint32_t line, void* user_data)
{
    sx_unused(file);
    sx_unused(func);
    sx_unused(line);

    sx_linalloc* linalloc = (sx_linalloc*)user_data;
    void* last_ptr = linalloc->ptr + linalloc->last_ptr_offset;
    if (size > 0) {
        sx_assert(size < UINT32_MAX);
        if (ptr == NULL) {
            // malloc
            void* new_ptr = sx__linalloc_malloc(linalloc, size, align);
            linalloc->last_ptr_offset = (uintptr_t)((uint8_t*)new_ptr - linalloc->ptr);
            return new_ptr;
        } else if (ptr == last_ptr) {
            // Realloc: special case, the memory is continous so we can just grow the buffer without
            // any new allocation
            //          TODO: put some control in alignment checking, so alignment stay constant
            //          between reallocs

            sx__linalloc_hdr* hdr = (sx__linalloc_hdr*)ptr - 1;
            if ((linalloc->last_ptr_offset + size) > linalloc->size) {
                sx_out_of_memory();
                return NULL;
            }

            linalloc->offset = linalloc->last_ptr_offset + size;
            hdr->size = (uint32_t)size;
            return ptr;    // Input pointer does not change
        } else {
            // Realloc: generic, create new allocation and sx_memcpy the previous data into the
            // beginning
            void* new_ptr = sx__linalloc_malloc(linalloc, size, align);
            if (new_ptr) {
                sx__linalloc_hdr* hdr = (sx__linalloc_hdr*)ptr - 1;
                sx_memcpy(new_ptr, ptr, sx_min((uint32_t)size, hdr->size));
                linalloc->last_ptr_offset = (uintptr_t)((uint8_t*)new_ptr - linalloc->ptr);
            }
            return new_ptr;
        }
    }

    // Note: we have no Free operation for linear allocator
    return NULL;
}

void sx_linalloc_init(sx_linalloc* linalloc, void* ptr, size_t size)
{
    sx_assert(linalloc);
    sx_assert(ptr);
    sx_assert(size);

    linalloc->alloc.alloc_cb = sx__linalloc_cb;
    linalloc->alloc.user_data = linalloc;
    linalloc->ptr = (uint8_t*)ptr;
    linalloc->last_ptr_offset = 0;
    linalloc->size = size;
    linalloc->offset = linalloc->peak = 0;
}

void sx_linalloc_reset(sx_linalloc* linalloc)
{
    linalloc->last_ptr_offset = 0;
    linalloc->offset = 0;
}

size_t sx_linalloc_real_alloc_size(size_t size, uint32_t align)
{
    align = sx_max((int)align, SX_CONFIG_ALLOCATOR_NATURAL_ALIGNMENT);
    return sizeof(sx__linalloc_hdr) + size + align;
}
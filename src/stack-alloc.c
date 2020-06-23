//
// Copyright 2018 Sepehr Taghdisian (septag@github). All rights reserved.
// License: https://github.com/septag/sx#license-bsd-2-clause
//
#include "sx/stack-alloc.h"

typedef struct stackalloc_hdr_s {
    uint32_t size;             // size of buffer that requested upon allocation
    uint32_t internal_size;    // actual size that is allocated (with headers and alignment)
    uint32_t prev_offset;
} sx__stackalloc_hdr;

static void* sx__stackalloc_malloc(sx_stackalloc* alloc, size_t size, uint32_t align)
{
    align = sx_max((int)align, SX_CONFIG_ALLOCATOR_NATURAL_ALIGNMENT);
    size_t total = size + sizeof(sx__stackalloc_hdr);
    size_t new_offset = alloc->offset + sizeof(sx__stackalloc_hdr);
    if (new_offset % align != 0) {
        size_t aligned_offset = sx_align_mask(new_offset, (size_t)align-1);
        total += (aligned_offset - new_offset);
    }

    if (alloc->offset + total > (size_t)alloc->size) {
        sx_out_of_memory();
        return NULL;
    }

    uint8_t* ptr = alloc->ptr + alloc->offset;
    uint8_t* aligned = (uint8_t*)sx_align_ptr(ptr, sizeof(sx__stackalloc_hdr), align);
    sx_assert((uintptr_t)aligned % align == 0);

    // Fill header info
    sx__stackalloc_hdr* hdr = (sx__stackalloc_hdr*)aligned - 1;
    hdr->size = (uint32_t)size;
    hdr->internal_size = (uint32_t)total;
    hdr->prev_offset = (uint32_t)alloc->last_ptr_offset;    // TODO: unsafe

    alloc->offset += total;
    alloc->peak = sx_max(alloc->peak, alloc->offset);

    return aligned;
}

static void* sx__stackalloc_cb(void* ptr, size_t size, uint32_t align, const char* file,
                               const char* func, uint32_t line, void* user_data)
{
    sx_unused(file);
    sx_unused(func);
    sx_unused(line);

    sx_stackalloc* stackalloc = (sx_stackalloc*)user_data;
    void* last_ptr = stackalloc->ptr + stackalloc->last_ptr_offset;
    if (size > 0) {
        sx_assert_rel(size < UINT32_MAX);

        if (ptr == NULL) {
            // malloc
            void* new_ptr = sx__stackalloc_malloc(stackalloc, size, align);
            stackalloc->last_ptr_offset = (uintptr_t)((uint8_t*)new_ptr - stackalloc->ptr);
            return new_ptr;
        } else if (ptr == last_ptr) {
            // Realloc: special case, the memory is continous so we can just grow the buffer without
            // any new allocation
            //          TODO: put some control in alignment checking, so alignment stay constant
            //          between reallocs
            if (stackalloc->offset + size > stackalloc->size) {
                sx_out_of_memory();
                return NULL;
            }
            stackalloc->offset += size;
            return ptr;    // Input pointer does not change
        } else {
            // Realloc: generic, create new allocation and sx_memcpy the previous data into the
            // beginning
            void* new_ptr = sx__stackalloc_malloc(stackalloc, size, align);
            if (new_ptr) {
                sx__stackalloc_hdr* hdr = (sx__stackalloc_hdr*)ptr - 1;
                sx_memcpy(new_ptr, ptr, sx_min((uint32_t)size, hdr->size));
                stackalloc->last_ptr_offset = (uintptr_t)((uint8_t*)new_ptr - stackalloc->ptr);
            }
            return new_ptr;
        }
    } else if (ptr) {
        // 'free' operation must be inverse order of the 'alloc' operations (like stack)
        //  example:
        //      p1 = alloc();
        //      p2 = alloc();
        //      free(p2);
        //      free(p1);
        // So we trigger assertion if this order is not applied, for memory safety
        sx_assert(stackalloc->offset > 0);
        if (ptr == last_ptr) {
            sx__stackalloc_hdr* hdr = (sx__stackalloc_hdr*)ptr - 1;
            stackalloc->offset -= hdr->internal_size;
            stackalloc->last_ptr_offset = hdr->prev_offset;    // move to previous allocated pointer
        }
        return NULL;
    }

    sx_assert(0 && "Invalid arguments?!");
    return NULL;
}

void sx_stackalloc_init(sx_stackalloc* stackalloc, void* ptr, size_t size)
{
    sx_assert(stackalloc);
    sx_assert(ptr);
    sx_assert(size);

    stackalloc->alloc.alloc_cb = sx__stackalloc_cb;
    stackalloc->alloc.user_data = stackalloc;
    stackalloc->ptr = (uint8_t*)ptr;
    stackalloc->last_ptr_offset = 0;
    stackalloc->size = size;
    stackalloc->offset = stackalloc->peak = 0;
}

void sx_stackalloc_reset(sx_stackalloc* stackalloc)
{
    sx_assert(stackalloc);
    stackalloc->last_ptr_offset = 0;
    stackalloc->offset = 0;
}

size_t sx_stackalloc_real_alloc_size(size_t size, uint32_t align)
{
    align = sx_max((int)align, SX_CONFIG_ALLOCATOR_NATURAL_ALIGNMENT);
    return sizeof(sx__stackalloc_hdr) + align + size;
}
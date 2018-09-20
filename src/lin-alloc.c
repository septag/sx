//
// Copyright 2018 Sepehr Taghdisian (septag@github). All rights reserved.
// License: https://github.com/septag/sx#license-bsd-2-clause
//
#include "sx/lin-alloc.h"

typedef struct sx__linalloc_hdr_s
{
    int size;       // size of buffer that requested upon allocation
    int padding;    // number of bytes that is padded before the pointer
} sx__linalloc_hdr;

static void* sx__linalloc_malloc(sx_linalloc* alloc, size_t size, size_t align, const char* file, uint32_t line)
{
    align = sx_max((int)align, SX_CONFIG_ALLOCATOR_NATURAL_ALIGNMENT);
    size_t total = size + sizeof(sx__linalloc_hdr) + align;
    if (alloc->offset + total > alloc->size) {
        sx_out_of_memory();
        return NULL;
    }

    uint8_t* ptr = alloc->ptr + alloc->offset;
    uint8_t* aligned = (uint8_t*)sx_align_ptr(ptr, sizeof(sx__linalloc_hdr), align);

    // Fill header info
    sx__linalloc_hdr* hdr = (sx__linalloc_hdr*)aligned - 1;
    hdr->size = (int)size;
    hdr->padding = (int)(aligned - ptr);

    alloc->offset += (int)total;
    alloc->peak = sx_max(alloc->peak, alloc->offset);

    return aligned;
}

static void* sx__linalloc_cb(void* ptr, size_t size, size_t align, const char* file, uint32_t line, void* user_data)
{
    if (size > 0) {
        sx_linalloc* linalloc = (sx_linalloc*)user_data;
        if (ptr == NULL) {
            // malloc
            void* new_ptr = sx__linalloc_malloc(linalloc, size, align, file, line);
            linalloc->last_ptr = (uint8_t*)new_ptr;
            return new_ptr;
        } else if (ptr == linalloc->last_ptr) {
            // Realloc: special case, the memory is continous so we can just grow the buffer without any new allocation
            //          TODO: put some control in alignment checking, so alignment stay constant between reallocs
            if (linalloc->offset + size > linalloc->size) {
                sx_out_of_memory();
                return NULL;
            }
            linalloc->offset += (int)size;
            return ptr;    // Input pointer does not change
        } else {
            // Realloc: generic, create new allocation and sx_memcpy the previous data into the beginning
            void* new_ptr = sx__linalloc_malloc(linalloc, size, align, file, line);
            if (new_ptr) {
                sx__linalloc_hdr* hdr = (sx__linalloc_hdr*)ptr - 1;
                sx_memcpy(new_ptr, ptr, sx_min((int)size, hdr->size));
                linalloc->last_ptr = (uint8_t*)new_ptr;
            }
            return new_ptr;
        }
    }

    // Note: we have no Free operation for linear allocator
    return NULL;
}

void sx_linalloc_init(sx_linalloc* linalloc, void* ptr, int size)
{
    sx_assert(linalloc);
    sx_assert(ptr);
    sx_assert(size);

    linalloc->alloc.alloc_cb = sx__linalloc_cb;
    linalloc->alloc.user_data = linalloc;
    linalloc->ptr = (uint8_t*)ptr;
    linalloc->last_ptr = NULL;
    linalloc->size = size;
    linalloc->offset = linalloc->peak = 0;   
}

void sx_linalloc_reset(sx_linalloc* linalloc)
{
    linalloc->last_ptr = NULL;
    linalloc->offset = 0;
}
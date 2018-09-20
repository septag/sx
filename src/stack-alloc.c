//
// Copyright 2018 Sepehr Taghdisian (septag@github). All rights reserved.
// License: https://github.com/septag/sx#license-bsd-2-clause
//
#include "sx/stack-alloc.h"

typedef struct stackalloc_hdr_s
{
    int size;       // size of buffer that requested upon allocation
    int padding;    // number of bytes that is padded before the pointer
    int inner_size; // actual size that is allocated (with headers and alignment)
    void* prev_ptr; // previous allocated pointer
} sx__stackalloc_hdr;

static void* sx__stackalloc_malloc(sx_stackalloc* alloc, size_t size, size_t align, const char* file, uint32_t line)
{
    align = sx_max((int)align, SX_CONFIG_ALLOCATOR_NATURAL_ALIGNMENT);
    size_t total = size + sizeof(sx__stackalloc_hdr) + align;
    if (alloc->offset + total > alloc->size) {
        sx_out_of_memory();
        return NULL;
    }

    uint8_t* ptr = alloc->ptr + alloc->offset;
    uint8_t* aligned = (uint8_t*)sx_align_ptr(ptr, sizeof(sx__stackalloc_hdr), align);

    // Fill header info
    sx__stackalloc_hdr* hdr = (sx__stackalloc_hdr*)aligned - 1;
    hdr->size = (int)size;
    hdr->padding = (int)(aligned - ptr);
    hdr->inner_size = (int)total;
    hdr->prev_ptr = alloc->last_ptr;

    alloc->offset += (int)total;
    alloc->peak = sx_max(alloc->peak, alloc->offset);

    return aligned;
}

static void* sx__stackalloc_cb(void* ptr, size_t size, size_t align, const char* file, uint32_t line, void* user_data)
{
    sx_stackalloc* stackalloc = (sx_stackalloc*)user_data;
    if (size > 0) {
        if (ptr == NULL) {
            // malloc
            void* new_ptr = sx__stackalloc_malloc(stackalloc, size, align, file, line);
            stackalloc->last_ptr = (uint8_t*)new_ptr;
            return new_ptr;
        } else if (ptr == stackalloc->last_ptr) {
            // Realloc: special case, the memory is continous so we can just grow the buffer without any new allocation
            //          TODO: put some control in alignment checking, so alignment stay constant between reallocs
            if (stackalloc->offset + size > stackalloc->size) {
                sx_out_of_memory();
                return NULL;
            }
            stackalloc->offset += (int)size;
            return ptr;    // Input pointer does not change
        } else {
            // Realloc: generic, create new allocation and sx_memcpy the previous data into the beginning
            void* new_ptr = sx__stackalloc_malloc(stackalloc, size, align, file, line);
            if (new_ptr) {
                sx__stackalloc_hdr* hdr = (sx__stackalloc_hdr*)ptr - 1;
                sx_memcpy(new_ptr, ptr, sx_min((int)size, hdr->size));
                stackalloc->last_ptr = (uint8_t*)new_ptr;
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
        sx_assert(ptr == stackalloc->last_ptr && "Invalid ordering of 'free'. See the comment above");
        sx_assert(stackalloc->offset > 0);
        sx__stackalloc_hdr* hdr = (sx__stackalloc_hdr*)ptr - 1;
        stackalloc->offset -= hdr->inner_size;
        stackalloc->last_ptr = (uint8_t*)hdr->prev_ptr;   // move to previous allocated pointer
        return NULL;
    }

    sx_assert(0 && "Invalid arguments?!");
    return NULL;
}

void sx_stackalloc_init(sx_stackalloc* stackalloc, void* ptr, int size)
{
    sx_assert(stackalloc);
    sx_assert(ptr);
    sx_assert(size);

    stackalloc->alloc.alloc_cb = sx__stackalloc_cb;
    stackalloc->alloc.user_data = stackalloc;
    stackalloc->ptr = (uint8_t*)ptr;
    stackalloc->last_ptr = NULL;
    stackalloc->size = size;
    stackalloc->offset = stackalloc->peak = 0;   
}

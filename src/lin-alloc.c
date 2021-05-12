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
    size_t new_offset = alloc->offset + sizeof(sx__linalloc_hdr);
    if (new_offset % align != 0) {
        size_t aligned_offset = sx_align_mask(new_offset, (size_t)align-1);
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
            sx_assertf(size > hdr->size, "realloc size for this allocator should only grow");

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

////////////////////////////////////////////////////////////////////////////////////////////////////
static void* sx__linalloc_growable_malloc(sx_linalloc_growable* alloc, sx_linalloc_growable_page* page, 
                                          size_t size, uint32_t align)
{
    align = sx_max((int)align, SX_CONFIG_ALLOCATOR_NATURAL_ALIGNMENT);
    size_t total = size + sizeof(sx__linalloc_hdr);
    size_t new_offset = page->offset + sizeof(sx__linalloc_hdr);
    if (new_offset % align != 0) {
        size_t aligned_offset = sx_align_mask(new_offset, (size_t)align-1);
        total += (aligned_offset - new_offset);
        new_offset = aligned_offset;
    }

    if ((new_offset + size) > page->size) {
        // create a new page
        sx_linalloc_growable_page* new_page = 
            sx_malloc(alloc->main_alloc, sizeof(sx_linalloc_growable_page) + sx_max(alloc->page_size, total));
        if (!new_page) {
            sx_out_of_memory();
            return NULL;
        }
        sx_memset(new_page, 0x0, sizeof(sx_linalloc_growable_page));
        new_page->ptr = (uint8_t*)(new_page + 1);
        new_page->size = sx_max(alloc->page_size, total);
        page->next = new_page;
        return sx__linalloc_growable_malloc(alloc, new_page, size, align);
    }

    uint8_t* aligned = page->ptr + new_offset;
    sx_assert((uintptr_t)aligned % align == 0);

    // Fill header info
    sx__linalloc_hdr* hdr = (sx__linalloc_hdr*)aligned - 1;
    hdr->size = (uint32_t)size;

    page->offset = new_offset + size;
    alloc->allocated_size += total;
    alloc->peak = sx_max(alloc->peak, alloc->allocated_size);

    return aligned;
}

static void* sx__linalloc_growable_cb(void* ptr, size_t size, uint32_t align, const char* file,
                                      const char* func, uint32_t line, void* user_data)
{
    sx_unused(file);
    sx_unused(func);
    sx_unused(line);

    sx_linalloc_growable* linalloc = user_data;
    sx_linalloc_growable_page* page = linalloc->pages;
    while (page->next) {
        page = page->next;
    }

    void* last_ptr = page->ptr + page->last_ptr_offset;
    if (size > 0) {
        sx_assert(size < UINT32_MAX);
        if (ptr == NULL) {
            // malloc
            void* new_ptr = sx__linalloc_growable_malloc(linalloc, page, size, align);
            if (page->next) {
                page = page->next;
            }

            page->last_ptr_offset = (uintptr_t)((uint8_t*)new_ptr - page->ptr);
            return new_ptr;
        } else if (ptr == last_ptr) {
            // Realloc: special case, the memory is continous so we can just grow the buffer without any new allocation
            sx__linalloc_hdr* hdr = (sx__linalloc_hdr*)ptr - 1;
            sx_assertf(size > hdr->size, "realloc size for this allocator should only grow");

            if ((page->last_ptr_offset + size) > page->size) {
                // realloc doesn't fit on the current page, create a new page and re-copy the data to the new pointer
                void* new_ptr = sx__linalloc_growable_malloc(linalloc, page, size, align);
                hdr = (sx__linalloc_hdr*)new_ptr - 1;
                sx_assert(page->next);
                page = page->next;
                page->last_ptr_offset = (uintptr_t)((uint8_t*)new_ptr - page->ptr);

                sx_memcpy(new_ptr, ptr, hdr->size);
                ptr = new_ptr;
            } else {
                page->offset = page->last_ptr_offset + size;
                linalloc->allocated_size += (size - hdr->size);
            }

            hdr->size = (uint32_t)size;
            return ptr;    // Input pointer does not change
        } else {
            // Realloc: generic, create new allocation and sx_memcpy the previous data into the
            // beginning
            void* new_ptr = sx__linalloc_growable_malloc(linalloc, page, size, align);
            if (new_ptr) {
                if (page->next) {
                    page = page->next;
                }
                sx__linalloc_hdr* hdr = (sx__linalloc_hdr*)ptr - 1;
                sx_memcpy(new_ptr, ptr, sx_min((uint32_t)size, hdr->size));
                page->last_ptr_offset = (uintptr_t)((uint8_t*)new_ptr - page->ptr);
            }
            return new_ptr;
        }
    }

    // Note: we have no Free operation for linear allocator
    return NULL;
}

sx_linalloc_growable* sx_linalloc_growable_create(const sx_alloc* alloc, size_t size)
{
    sx_assert(alloc);
    sx_assert(size);

    size_t page_size = sx_align_4096(size);
    sx_linalloc_growable* linalloc = 
        sx_malloc(alloc, sizeof(sx_linalloc_growable) + sizeof(sx_linalloc_growable_page) + page_size);
    if (!linalloc) {
        sx_memory_fail();
        return NULL;
    }

    linalloc->main_alloc = alloc;
    linalloc->page_size = page_size;
    linalloc->allocated_size = 0;
    linalloc->peak = 0;
    linalloc->alloc = (sx_alloc) {
        .alloc_cb = sx__linalloc_growable_cb,
        .user_data = linalloc
    };

    // assign the first page
    linalloc->pages = (sx_linalloc_growable_page*)(linalloc + 1);
    sx_memset(linalloc->pages, 0x0, sizeof(sx_linalloc_growable_page));

    linalloc->pages->size = linalloc->page_size;
    linalloc->pages->ptr = (uint8_t*)(linalloc->pages + 1);

    return linalloc;
}

void sx_linalloc_growable_reset(sx_linalloc_growable* linalloc)
{
    sx_assert(linalloc->pages);

    // free all pages except the first one
    sx_linalloc_growable_page* page = linalloc->pages->next;
    while (page) {
        sx_free(linalloc->main_alloc, page);
        page = page->next;
    }
    linalloc->pages->next = NULL;

    linalloc->pages->offset = 0;
    linalloc->pages->last_ptr_offset = 0;
    linalloc->allocated_size = 0;
}

void sx_linalloc_growable_destroy(sx_linalloc_growable* linalloc)
{
    // free all pages except the first one
    if (linalloc->main_alloc) {
        sx_linalloc_growable_reset(linalloc);
        sx_free(linalloc->main_alloc, linalloc);
    }
}

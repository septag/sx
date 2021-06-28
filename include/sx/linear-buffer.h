//
// Copyright 2020 Sepehr Taghdisian (septag@github). All rights reserved.
// License: https://github.com/septag/sx#license-bsd-2-clause
//
// linear-buffer.h - helper custom memory allocator useful for allocating structures with
//                   data arrays in a single allocation call, supports memory alignment
//                   you start by initialization a structure type and then adding array fields
//                   in the end, you allocate memory in a single call, instead of multiple mallocs
// NOTE: use provided macros instead of internal sx__ functions
//
// Example. allocate structure my_data_t with 100 array items of array_of_ints and array_of_floats
//
//      typedef struct my_data_t {
//          int count;
//          int* array_of_ints;
//          float* array_of_floats;
//      };
//      bool* arbitary_data;
//
//      sx_linear_buffer buff;
//      sx_linear_buffer_init(&buff, my_data_t, 0);
//      sx_linear_buffer_addtype(&buff, my_data_t, int, array_of_ints, 100, 0);
//      sx_linear_buffer_addtype(&buff, my_data_t, float, array_of_floats, 100, 0);
//      sx_linear_buffer_addptr(&buff, &arbitary_data, bool, 100, 0); // arbitary_data is no-member
//      // data is allocated in one _malloc_ call instead of 4, all of the pointers will be valid
//      my_data_t* data = sx_linear_buffer_calloc(&buff, heap_alloc);
//      ... // do work
//      sx_free(heap_alloc, data);
//
#pragma once

#include "allocator.h"
#include "string.h"

#ifndef SX_MAX_BUFFER_FIELDS
#    define SX_MAX_BUFFER_FIELDS 32
#endif

typedef struct sx_linear_buffer_field {
    void** pptr;
    size_t offset;
    int offset_in_parent;
} sx_linear_buffer_field;

typedef struct sx_linear_buffer {
    const char* parent_type;
    sx_linear_buffer_field fields[SX_MAX_BUFFER_FIELDS];
    size_t size;                // total size of the buffer (bytes)
    uint32_t parent_align;
    int num_fields;
} sx_linear_buffer;

// internal
SX_INLINE void sx__linear_buffer_init(sx_linear_buffer* buf, const char* parent_type,
                                      size_t parent_size, uint32_t align sx_default(0))
{
    sx_assert(parent_size > 0);

    buf->parent_type = parent_type;

    buf->fields[0].pptr = NULL;
    buf->fields[0].offset = 0;
    buf->fields[0].offset_in_parent = -1;    // parent

    align = sx_max(SX_CONFIG_ALLOCATOR_NATURAL_ALIGNMENT, (int)align);

    buf->size = sx_align_mask(parent_size, align - 1);
    buf->parent_align = align;
    buf->num_fields = 1;
}

SX_INLINE void sx__linear_buffer_add(sx_linear_buffer* buf, size_t size, int offset_in_struct,
                                     void** pptr, uint32_t align)
{
    sx_assert(buf);

    int index = buf->num_fields;
    sx_assert(index < SX_MAX_BUFFER_FIELDS);

    align = align < SX_CONFIG_ALLOCATOR_NATURAL_ALIGNMENT ? SX_CONFIG_ALLOCATOR_NATURAL_ALIGNMENT
                                                          : align;
    size = sx_align_mask(size, align - 1);
    size_t offset = buf->size;
    if (offset % align != 0) {
        offset = sx_align_mask(offset, align - 1);
    }

    sx_linear_buffer_field* field = &buf->fields[index];
    field->pptr = pptr;
    field->offset = offset;
    field->offset_in_parent = offset_in_struct;

    buf->size = offset + size;
    ++buf->num_fields;
}
// internal

// interface macros, use these instead of the API
#define sx_linear_buffer_init(_buf, _struct, _align) \
    sx__linear_buffer_init((_buf), #_struct, sizeof(_struct), (_align))
#define sx_linear_buffer_addtype(_buf, _struct, _type, _name, _count, _align) \
    sx_assert(sx_strequal((_buf)->parent_type, #_struct));                    \
    sx__linear_buffer_add((_buf), sizeof(_type) * (_count),                   \
                          (int)(uintptr_t) & (((_struct*)0)->_name), NULL, (_align))
#define sx_linear_buffer_addptr(_buf, _pptr, _type, _count, _align) \
    sx__linear_buffer_add((_buf), sizeof(_type) * (_count), -1, (void**)(_pptr), (_align))

SX_INLINE void* sx__linear_buffer_calloc(const sx_linear_buffer* buf, const sx_alloc* alloc, 
                                         const char* file, const char* func, uint32_t line)
{
    void* mem = buf->parent_align <= SX_CONFIG_ALLOCATOR_NATURAL_ALIGNMENT
                    ? sx__malloc(alloc, buf->size, 0, file, func, line)
                    : sx_aligned_malloc(alloc, buf->size, buf->parent_align);
    if (!mem) {
        sx_out_of_memory();
        return NULL;
    }
    sx_memset(mem, 0x0, buf->size);
    uint8_t* tmp_mem = (uint8_t*)mem;

    // assign pointers
    for (int i = 1, c = buf->num_fields; i < c; i++) {
        if (buf->fields[i].offset_in_parent != -1) {
            sx_assert(buf->fields[i].pptr == NULL);
            *((void**)(tmp_mem + buf->fields[i].offset_in_parent)) = tmp_mem + buf->fields[i].offset;
        } else {
            sx_assert(buf->fields[i].offset_in_parent == -1);
            *buf->fields[i].pptr = tmp_mem + buf->fields[i].offset;
        }
    }

    return mem;
}

#define sx_linear_buffer_calloc(_buf, _alloc) sx__linear_buffer_calloc((_buf), (_alloc), __FILE__, SX_FUNCTION, __LINE__)

#ifdef __cplusplus
template <typename _T>
struct sx_linear_buffer_t
{
public:
    sx_linear_buffer_t(uint32_t align = 0)
    {
        sx_linear_buffer_init(&this->lb, sizeof(_T), align);
    }
    
    template <typename _F> sx_linear_buffer_t& add_field(int offset_in_struct, size_t count, uint32_t align = 0)
    {
        sx__linear_buffer_add(&this->lb, sizeof(_F)*count, offset_in_struct, nullptr, align);
        return *this;
    }

    template <typename _F> sx_linear_buffer_t& add_external_array(_F** pptr, size_t count, uint32_t align = 0)
    {
        sx__linear_buffer_add(&this->lb, sizeof(_F)*count, -1, (void**)pptr, align);
        return *this;
    }

    _T* calloc(const sx_alloc* alloc)
    {
        return reinterpret_cast<_T*>(sx_linear_buffer_calloc(&this->lb, alloc));
    }
   
private:
    sx_linear_buffer lb;
};
#endif
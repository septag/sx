//
// Copyright 2018 Sepehr Taghdisian (septag@github). All rights reserved.
// License: https://github.com/septag/sx#license-bsd-2-clause
//

#include "sx/ringbuffer.h"
#include "sx/allocator.h"

sx_ringbuffer* sx_ringbuffer_create(const sx_alloc* alloc, int capacity)
{
    sx_ringbuffer* rb = sx_aligned_malloc(alloc, sizeof(sx_ringbuffer) + sx_align_16(capacity), 16);
    rb->capacity = capacity;
    rb->size = rb->end = rb->start = 0;
    return rb;
}

void sx_ringbuffer_destroy(sx_ringbuffer* rb, const sx_alloc* alloc)
{
    sx_aligned_free(alloc, rb, 16);
}

int sx_ringbuffer_expect_write(const sx_ringbuffer* rb)
{
    return rb->capacity - rb->size;
}

void sx_ringbuffer_write(sx_ringbuffer* rb, const void* data, int size)
{
    sx_assert(size > 0);
    sx_assert(size <= sx_ringbuffer_expect_write(rb));

    uint8_t* buff = (uint8_t*)(rb + 1);
    const uint8_t* udata = (const uint8_t*)data;
    int remain = rb->capacity - rb->end;
    if (remain >= size) {
        sx_memcpy(&buff[rb->end], udata, size);
    } else {
        sx_memcpy(&buff[rb->end], udata, remain);
        sx_memcpy(buff, &udata[remain], (size_t)size - remain);
    }

    rb->end = (rb->end + size) % rb->capacity;
    rb->size += size;
}

int sx_ringbuffer_read(sx_ringbuffer* rb, void* data, int size)
{
    sx_assert(size > 0);

    size = sx_min(size, rb->size);
    if (size == 0) {
        return 0;
    }

    if (data) {
        uint8_t* buff = (uint8_t*)(rb + 1);
        uint8_t* udata = (uint8_t*)data;
        int remain = rb->capacity - rb->start;
        if (remain >= size) {
            sx_memcpy(udata, &buff[rb->start], size);
        } else {
            sx_memcpy(udata, &buff[rb->start], remain);
            sx_memcpy(&udata[remain], buff, (size_t)size - (size_t)remain);
        }
    }

    rb->start = (rb->start + size) % rb->capacity;
    rb->size -= size;
    return size;
}

int sx_ringbuffer_read_noadvance(sx_ringbuffer* rb, void* data, int size, int* offset)
{
    sx_assert(size > 0);

    size = sx_min(size, rb->size);
    if (size == 0) {
        return 0;
    }

    sx_assert(data);
    uint8_t* buff = (uint8_t*)(rb + 1);
    uint8_t* udata = (uint8_t*)data;
    int _offset = offset ? *offset : rb->start;
    int remain = rb->capacity - _offset;
    if (remain >= size) {
        sx_memcpy(udata, &buff[_offset], size);
    } else {
        sx_memcpy(udata, &buff[_offset], remain);
        sx_memcpy(&udata[remain], buff, (size_t)size - (size_t)remain);
    }

    if (offset) {
        *offset = (*offset + size) % rb->capacity;
    }
    return size;
}

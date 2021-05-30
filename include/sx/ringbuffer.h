//
// Copyright 2018 Sepehr Taghdisian (septag@github). All rights reserved.
// License: https://github.com/septag/sx#license-bsd-2-clause
//
// Basic ring-buffer (circular buffer)
// https://en.wikipedia.org/wiki/Circular_buffer
//
// Functions:
//      sx_ringbuffer_create        Creates a ring-buffer with specified 'capacity'.
//                                  The whole buffer (struct+data) will be created in 1 malloc
//      sx_ringbuffer_destroy       Destroys the buffer and frees memory
//      sx_ringbuffer_expect_write  Gets maximum expected bytes that we can write w/o overwriting
//                                  the un-read data
//      sx_ringbuffer_write         Write N bytes to the ring-buffer
//      sx_ringbuffer_read          Reads N bytes from the ring-buffer and advances the read pointer
//                                  if `data` argument is NULL, it just advances the pointer 
//      sx_ringbuffer_read_noadvance Reads N bytes from the ring-buffer w/o advancing the read ptr
//                                   This is useful for peeking at data
//                                   'offset' (optional) is the actual offset in the buffer, which you can 
//                                   pass sx_ringbuffer.start as initial parameter, the function 
//                                   will update it after advancing the value, but the read pointer
//                                   inside the buffer won't be changed
//      NOTE: Actual pointer to the buffer data starts right after sx_ringbuffer:
//              void* buff_data = ring_buffer + 1;
//
// Usage:
//  sx_ringbuffer* rb = sx_ringbuffer_create(alloc, 1000);
//      Producer:
//          int num_bytes_to_write = sx_ringbuffer_expect_write(rb);
//          if (num_bytes_to_write > 0) {
//              my_data = sx_malloc(alloc, num_bytes_to_write);
//              sx_ringbuffer_write(rb, my_data, num_bytes_to_write)
//          }
//      Consumer:
//          void* my_data = sx_malloc(alloc, max_data_size);
//          int num_bytes_read = sx_ringbuffer_read(rb, my_data, max_data_size);
//  sx_ringbuffer_destroy(rb, alloc);
//
#pragma once

#include "sx.h"

typedef struct sx_alloc sx_alloc;

typedef sx_align_decl(16, struct) sx_ringbuffer
{
    int capacity;
    int size;     // valid data size
    int start;    // read offset
    int end;      // write offset
} sx_ringbuffer;

SX_API sx_ringbuffer* sx_ringbuffer_create(const sx_alloc* alloc, int capacity);
SX_API void sx_ringbuffer_destroy(sx_ringbuffer* rb, const sx_alloc* alloc);
SX_API int sx_ringbuffer_expect_write(const sx_ringbuffer* rb);
SX_API void sx_ringbuffer_write(sx_ringbuffer* rb, const void* data, int size);
SX_API int sx_ringbuffer_read(sx_ringbuffer* rb, void* data, int size);
SX_API int sx_ringbuffer_read_noadvance(sx_ringbuffer* rb, void* data, int size, int* offset);
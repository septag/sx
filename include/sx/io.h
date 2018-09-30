//
// Copyright 2018 Sepehr Taghdisian (septag@github). All rights reserved.
// License: https://github.com/septag/sx#license-bsd-2-clause
//
// stream-io.h v1.0: Some streaming primitives
//      sx_mem_block: Memory block represents a piece of memory that can also grow with an allocator
//              sx_mem_create_block         allocates the entire continous block object with it's memory. this type CAN NOT grow
//              sx_mem_destroy_block        destroys the allocated block, use only with sx_mem_create_block
//              sx_mem_init_block           allocates a block of memory from allocator, this type can grow later
//              sx_mem_release_block        destroy the memory inside the memory block object, only use with 'sx_mem_init_block'
//              sx_mem_init_block_ptr       initializes a block of memory from pre-allocated memory, this type CAN NOT grow
//              sx_mem_grow                 grows the memory is 'size' bytes, does not affect blocks initialized with _ptr
//              sx_define_mem_block_onstack creates and initializes memory block on stack 
//  
//      sx_mem_writer: Writes to an initialized memory block in streaming manner
//              sx_init_writer              initializes the writer, must be assigned a valid memory block
//              sx_mem_write                writes a piece of data to memory
//              sx_mem_seekw                seeks inside the buffer
//              sx_mem_write_var            helper macro to write single variables
//
//      sx_mem_reader: Reads from a pre-allocated memory in streaming manner
//              sx_init_reader              initializes the read, data must be pre-allocated and alive during the read operations
//              sx_mem_read                 reads a piece of data to memory
//              sx_mem_seekr                seeks inside the buffer
//              sx_mem_read_var             helper macro to read a single variable
//
//      sx_file_writer: Opens a file and writes to it just like stdc file IO
//              sx_file_open_writer         opens a file for writing, can provide optional optional sx_file_open_flags bitsets
//              sx_file_close_writer        closes the file
//              sx_file_write               writes a piece of data to file
//              sx_file_seekw               seeks inside the file
//              sx_file_write_var           helper macro to write single variables
//
//      sx_file_reader: Opens a file and reads from it just like stdc file IO
//              sx_file_open_reader         opens a file for writing, flags does not have any effect in this mode currently
//              sx_file_close_reader        close the file
//              sx_file_read                reads a piece of data from file to memory
//              sx_file_seekr               seeks inside the file
//              sx_file_read_var            helper macro to read a single variable
//              sx_file_load_text           utility function to load text files directly into a block of memory
//                                          the returned data in memory-block is always null-terminated
//              sx_file_load_bin            utility function to load binary files directly into a block of memory
//
#pragma once

#ifndef SX_IO_H_
#define SX_IO_H_

#include "allocator.h"

typedef enum sx_whence
{
    SX_WHENCE_BEGIN = 0,
    SX_WHENCE_CURRENT,
    SX_WHENCE_END
} sx_whence;

typedef enum sx_file_open_flags
{
    SX_FILE_OPEN_APPEND = 0x1    // Used for writing to file only
} sx_file_open_flag;

// sx_mem_block
typedef struct sx_mem_block
{
    const sx_alloc* alloc;
    void* data;
    int size;
    int align;
} sx_mem_block;

#ifdef __cplusplus
extern "C" {
#endif

sx_mem_block* sx_mem_create_block(const sx_alloc* alloc, int size, 
                                  const void* data SX_DFLT(NULL), int align SX_DFLT(0));
void sx_mem_destroy_block(sx_mem_block* mem);

void sx_mem_init_block_ptr(sx_mem_block* mem, void* data, int size);
void* sx_mem_grow(sx_mem_block* mem, int size);

#define sx_define_mem_block_onstack(_name, _size)    \
    uint8_t _name ## _buff_[(_size)];                \
    sx_mem_block _name;                              \
    sx_mem_init_block_ptr(&(_name), _name ## _buff_, (_size));

// sx_mem_writer
typedef struct sx_mem_writer 
{
    sx_mem_block* mem;
    uint8_t* data;
    int64_t pos;
    int64_t top;
    int64_t size;
} sx_mem_writer;

void sx_mem_init_writer(sx_mem_writer* writer, sx_mem_block* mem);
int sx_mem_write(sx_mem_writer* writer, const void* data, int size);
int64_t sx_mem_seekw(sx_mem_writer* writer, int64_t offset, sx_whence whence SX_DFLT(SX_WHENCE_CURRENT));

#define sx_mem_write_var(w, v) sx_mem_write((w), &(v), sizeof(v))

// sx_mem_reader
typedef struct sx_mem_reader 
{
    const uint8_t* data;
    int64_t pos;
    int64_t top;
} sx_mem_reader;

void sx_mem_init_reader(sx_mem_reader* reader, const void* data, int64_t size);
int sx_mem_read(sx_mem_reader* reader, void* data, int size);
int64_t sx_mem_seekr(sx_mem_reader* reader, int64_t offset, sx_whence whence SX_DFLT(SX_WHENCE_CURRENT));

#define sx_mem_read_var(w, v) sx_mem_read((w), &(v), sizeof(v))

// sx_file_writer
typedef struct sx_file_writer
{
    SX_ALIGN_DECL(16, uint8_t) data[16];
} sx_file_writer;

bool sx_file_open_writer(sx_file_writer* writer, const char* filepath, uint32_t flags SX_DFLT(0));
void sx_file_close_writer(sx_file_writer* writer);
int sx_file_write(sx_file_writer* writer, const void* data, int size);
int64_t sx_file_seekw(sx_file_writer* writer, int64_t offset, sx_whence whence SX_DFLT(SX_WHENCE_CURRENT));

#define sx_file_write_var(w, v)     sx_file_write((w), &(v), sizeof(v))
#define sx_file_write_text(w, s)    sx_file_write((w), (s), sx_strlen(s))

// sx_file_reader
typedef struct sx_file_reader
{
    SX_ALIGN_DECL(16, uint8_t) data[16];
} sx_file_reader;

bool sx_file_open_reader(sx_file_reader* reader, const char* filepath);
void sx_file_close_reader(sx_file_reader* reader);
int sx_file_read(sx_file_reader* reader, void* data, int size);
int64_t sx_file_seekr(sx_file_reader* reader, int64_t offset, sx_whence whence SX_DFLT(SX_WHENCE_CURRENT));

sx_mem_block* sx_file_load_text(const sx_alloc* alloc, const char* filepath);
sx_mem_block* sx_file_load_bin(const sx_alloc* alloc, const char* filepath);

#define sx_file_read_var(w, v) sx_file_read((w), &(v), sizeof(v))

#ifdef __cplusplus
}
#endif

#endif // SX_IO_H_
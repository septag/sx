//
// Copyright 2018 Sepehr Taghdisian (septag@github). All rights reserved.
// License: https://github.com/septag/sx#license-bsd-2-clause
//
// stream-io.h v1.0: Some streaming primitives
//      sx_mem_block: Memory block represents a piece of memory that can also grow with an allocator
//              sx_mem_create_block         allocates the entire continous block object with it's
//                                          memory. this type CAN NOT grow
//              sx_mem_destroy_block        destroys the allocated block, use only with
//                                          sx_mem_create_block
//              sx_mem_init_block           allocates a block of memory from allocator,
//                                          this type can grow later
//              sx_mem_release_block        destroy the memory inside the memory block object,
//                                          only use with 'sx_mem_init_block'
//              sx_mem_init_block_ptr       initializes a block of memory from pre-allocated memory,
//                                          this type CAN NOT grow
//              sx_mem_grow                 grows the memory is 'size' bytes, does not affect blocks
//                                          initialized with _ptr
//              sx_define_mem_block_onstack creates and initializes memory block on stack
//
//      sx_mem_writer: Writes to an initialized memory block for streamed writing
//              sx_mem_init_writer          initializes the writer, allocates init_size as start
//              sx_mem_release_writer       releases writer memory
//              sx_mem_write                writes a piece of data to memory
//              sx_mem_seekw                seeks inside the buffer
//              sx_mem_write_var            helper macro to write single variables
//
//      sx_mem_reader: Reads from a pre-allocated memory for streamed reading
//              sx_init_reader              initializes the read, data must be pre-allocated and
//                                          alive during the read operations
//              sx_mem_read                 reads a piece of data to memory
//              sx_mem_seekr                seeks inside the buffer
//              sx_mem_read_var             helper macro to read a single variable
//              sx_mem_get_iff_chunk        helper for reading IFF chunks
//                                          searches `size` bytes from the current location for
//                                          FOURCC chunk type. if `size` <= 0, searches till the end
//                                          if chunk is not found, returning `pos` variable will be
//                                          -1
//
//      sx_file_writer: Opens a file and writes to it just like stdc file IO
//              sx_file_open_writer         opens a file for writing, can provide optional optional
//                                          sx_file_open_flags bitsets
//              sx_file_close_writer        closes the file
//              sx_file_write               writes a piece of data to file
//              sx_file_seekw               seeks inside the file
//              sx_file_write_var           helper macro to write single variables
//
//      sx_file_reader: Opens a file and reads from it just like stdc file IO
//              sx_file_open_reader         opens a file for writing, flags does not have any effect
//                                          in this mode currently
//              sx_file_close_reader        close the file
//              sx_file_read                reads a piece of data from file to memory
//              sx_file_seekr               seeks inside the file
//              sx_file_read_var            helper macro to read a single variable
//              sx_file_load_text           utility function to load text files directly into a
//                                          block of memory the returned data in memory-block is
//                                          always null-terminated
//              sx_file_load_bin            utility function to load binary files directly into a
//                                          block of memory
//
#pragma once

#include "sx.h"

typedef struct sx_alloc sx_alloc;

#ifndef sx_data_truncate
#    define sx_data_truncate() sx_assert_rel(0 && "Data truncated !")
#endif

typedef enum sx_whence { SX_WHENCE_BEGIN = 0, SX_WHENCE_CURRENT, SX_WHENCE_END } sx_whence;

typedef enum sx_file_open_flags {
    SX_FILE_OPEN_APPEND = 0x1    // Used for writing to file only
} sx_file_open_flag;

// sx_mem_block
typedef struct sx_mem_block {
    const sx_alloc* alloc;
    void*           data;
    int             size;
    int             align;
} sx_mem_block;

SX_API sx_mem_block* sx_mem_create_block(const sx_alloc* alloc, int size,
                                         const void* data sx_default(NULL),
                                         int align sx_default(0));
SX_API sx_mem_block* sx_mem_ref_block(const sx_alloc* alloc, int size, void* data);
SX_API void          sx_mem_destroy_block(sx_mem_block* mem);

SX_API void sx_mem_init_block_ptr(sx_mem_block* mem, void* data, int size);
SX_API bool sx_mem_grow(sx_mem_block** pmem, int size);

#define sx_define_mem_block_onstack(_name, _size) \
    uint8_t      _name##_buff_[(_size)];          \
    sx_mem_block _name;                           \
    sx_mem_init_block_ptr(&(_name), _name##_buff_, (_size));

// sx_mem_writer
typedef struct sx_mem_writer {
    sx_mem_block* mem;
    uint8_t*      data;
    int64_t       pos;
    int64_t       top;
    int64_t       size;
} sx_mem_writer;

SX_API void sx_mem_init_writer(sx_mem_writer* writer, const sx_alloc* alloc, int init_size);
SX_API void sx_mem_release_writer(sx_mem_writer* writer);

SX_API int  sx_mem_write(sx_mem_writer* writer, const void* data, int size);
SX_API int64_t sx_mem_seekw(sx_mem_writer* writer, int64_t offset,
                            sx_whence whence sx_default(SX_WHENCE_CURRENT));

#define sx_mem_write_var(w, v) sx_mem_write((w), &(v), sizeof(v))
#define sx_mem_write_text(w, s) sx_mem_write((w), (s), sx_strlen(s))

// sx_mem_reader
typedef struct sx_mem_reader {
    const uint8_t* data;
    int64_t        pos;
    int64_t        top;
} sx_mem_reader;

SX_API void sx_mem_init_reader(sx_mem_reader* reader, const void* data, int64_t size);
SX_API int  sx_mem_read(sx_mem_reader* reader, void* data, int size);
SX_API int64_t sx_mem_seekr(sx_mem_reader* reader, int64_t offset,
                            sx_whence whence sx_default(SX_WHENCE_CURRENT));

typedef struct sx_iff_chunk {
    int64_t  pos;
    uint32_t size;
} sx_iff_chunk;

SX_API sx_iff_chunk sx_mem_get_iff_chunk(sx_mem_reader* reader, int64_t size, uint32_t fourcc);

#define sx_mem_read_var(r, v) sx_mem_read((r), &(v), sizeof(v))

// sx_file_writer
typedef struct sx_file_writer {
    sx_align_decl(16, uint8_t) data[16];
} sx_file_writer;

SX_API bool sx_file_open_writer(sx_file_writer* writer, const char* filepath,
                                uint32_t flags sx_default(0));
SX_API void sx_file_close_writer(sx_file_writer* writer);
SX_API int  sx_file_write(sx_file_writer* writer, const void* data, int size);
SX_API int64_t sx_file_seekw(sx_file_writer* writer, int64_t offset,
                             sx_whence whence sx_default(SX_WHENCE_CURRENT));

#define sx_file_write_var(w, v) sx_file_write((w), &(v), sizeof(v))
#define sx_file_write_text(w, s) sx_file_write((w), (s), sx_strlen(s))

// sx_file_reader
typedef struct sx_file_reader {
    sx_align_decl(16, uint8_t) data[16];
} sx_file_reader;

SX_API bool sx_file_open_reader(sx_file_reader* reader, const char* filepath);
SX_API void sx_file_close_reader(sx_file_reader* reader);
SX_API int  sx_file_read(sx_file_reader* reader, void* data, int size);
SX_API int64_t sx_file_seekr(sx_file_reader* reader, int64_t offset,
                             sx_whence whence sx_default(SX_WHENCE_CURRENT));

SX_API sx_mem_block* sx_file_load_text(const sx_alloc* alloc, const char* filepath);
SX_API sx_mem_block* sx_file_load_bin(const sx_alloc* alloc, const char* filepath);

#define sx_file_read_var(w, v) sx_file_read((w), &(v), sizeof(v))

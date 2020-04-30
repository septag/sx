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

// sx_mem_block
typedef struct sx_mem_block {
    const sx_alloc* alloc;
    void* data;
    int64_t size;
    int align;
} sx_mem_block;

SX_API sx_mem_block* sx_mem_create_block(const sx_alloc* alloc, int64_t size,
                                         const void* data sx_default(NULL),
                                         int align sx_default(0));
SX_API sx_mem_block* sx_mem_ref_block(const sx_alloc* alloc, int64_t size, void* data);
SX_API void sx_mem_destroy_block(sx_mem_block* mem);

SX_API void sx_mem_init_block_ptr(sx_mem_block* mem, void* data, int64_t size);
SX_API bool sx_mem_grow(sx_mem_block** pmem, int64_t size);

#define sx_define_mem_block_onstack(_name, _size) \
    uint8_t _name##_buff_[(_size)];               \
    sx_mem_block _name;                           \
    sx_mem_init_block_ptr(&(_name), _name##_buff_, (_size));

// sx_mem_writer
typedef struct sx_mem_writer {
    sx_mem_block* mem;
    uint8_t* data;
    int64_t pos;
    int64_t top;
    int64_t size;
} sx_mem_writer;

SX_API void sx_mem_init_writer(sx_mem_writer* writer, const sx_alloc* alloc, int init_size);
SX_API void sx_mem_release_writer(sx_mem_writer* writer);

SX_API int sx_mem_write(sx_mem_writer* writer, const void* data, int size);
SX_API int64_t sx_mem_seekw(sx_mem_writer* writer, int64_t offset,
                            sx_whence whence sx_default(SX_WHENCE_CURRENT));

#define sx_mem_write_var(w, v) sx_mem_write((w), &(v), sizeof(v))
#define sx_mem_write_text(w, s) sx_mem_write((w), (s), sx_strlen(s))

// sx_mem_reader
typedef struct sx_mem_reader {
    const uint8_t* data;
    int64_t pos;
    int64_t top;
} sx_mem_reader;

SX_API void sx_mem_init_reader(sx_mem_reader* reader, const void* data, int64_t size);
SX_API int sx_mem_read(sx_mem_reader* reader, void* data, int size);
SX_API int64_t sx_mem_seekr(sx_mem_reader* reader, int64_t offset,
                            sx_whence whence sx_default(SX_WHENCE_CURRENT));
#define sx_mem_read_var(r, v) sx_mem_read((r), &(v), sizeof(v))

typedef struct sx_file {
    sx_align_decl(16, uint8_t) data[32];
} sx_file;

// for proper file buffering (SX_FILE_NOCACHE) alignment requirements under win32, visit:
// https://docs.microsoft.com/en-us/windows/win32/fileio/file-buffering
// as a general rule, if you use SX_FILE_NOCACHE flag, use page aligned memory buffers 
// obtained from sx_os_pagesz(), or allocate memory with virtual memory (virtual-alloc.h)
typedef enum sx_file_open_flag {
    SX_FILE_READ = 0x01,            // open for reading
    SX_FILE_WRITE = 0x02,           // open for writing
    SX_FILE_APPEND = 0x04,          // append to the end of the file (write mode only)
    SX_FILE_NOCACHE = 0x08,         // disable cache
    SX_FILE_WRITE_THROUGH = 0x10,   // write-through
    SX_FILE_SEQ_SCAN = 0x20,        // optimize cache for sequential scan (not used in NOCACHE)
    SX_FILE_RANDOM_ACCESS = 0x40,   // optimize cache for random access (not used in NOCACHE)
    SX_FILE_TEMP = 0x80
} sx_file_open_flag;
typedef uint32_t sx_file_open_flags;

SX_API bool sx_file_open(sx_file* file, const char* filepath, sx_file_open_flags flags);
SX_API void sx_file_close(sx_file* file);

SX_API int64_t sx_file_read(sx_file* file, void* data, int64_t size);
SX_API int64_t sx_file_write(sx_file* file, const void* data, int64_t size);
SX_API int64_t sx_file_seek(sx_file* file, int64_t offset, sx_whence whence);
SX_API int64_t sx_file_size(const sx_file* file);

SX_API sx_mem_block* sx_file_load_text(const sx_alloc* alloc, const char* filepath);
SX_API sx_mem_block* sx_file_load_bin(const sx_alloc* alloc, const char* filepath);

#define sx_file_write_var(w, v) sx_file_write((w), &(v), sizeof(v))
#define sx_file_write_text(w, s) sx_file_write((w), (s), sx_strlen(s))
#define sx_file_read_var(w, v) sx_file_read((w), &(v), sizeof(v))

typedef struct sx_iff_chunk {
    int64_t pos;
    uint32_t size;
    uint32_t fourcc;
    int parent_id;
} sx_iff_chunk;

SX_API sx_iff_chunk sx_mem_get_iff_chunk(sx_mem_reader* reader, int64_t size, uint32_t fourcc);


#if 0
typedef struct sx_file_writer {
    sx_align_decl(16, uint8_t) data[16];
} sx_file_writer;

SX_API bool sx_file_open_writer(sx_file_writer* writer, const char* filepath,
                                sx_file_open_flags flags sx_default(0));
SX_API void sx_file_close_writer(sx_file_writer* writer);
SX_API int sx_file_write(sx_file_writer* writer, const void* data, int size);
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
SX_API int sx_file_read(sx_file_reader* reader, void* data, int size);
SX_API int64_t sx_file_seekr(sx_file_reader* reader, int64_t offset,
                             sx_whence whence sx_default(SX_WHENCE_CURRENT));

SX_API sx_mem_block* sx_file_load_text(const sx_alloc* alloc, const char* filepath);
SX_API sx_mem_block* sx_file_load_bin(const sx_alloc* alloc, const char* filepath);

#define sx_file_read_var(w, v) sx_file_read((w), &(v), sizeof(v))

// IFF file
// https://en.wikipedia.org/wiki/Interchange_File_Format
//
typedef struct sx_iff_chunk {
    int64_t pos;
    uint32_t size;
    uint32_t fourcc;
    int parent_id;
} sx_iff_chunk;

typedef enum sx_iff_type {
    SX_IFFTYPE_MEM,
    SX_IFFTYPE_DISK
} sx_iff_type;

typedef enum sx_iff_flag {
    SX_IFFFLAG_READ_ALL_CHUNKS = 0x1
} sx_iff_flag;
typedef uint32_t sx_iff_flags;

typedef struct sx_iff_reader {
    sx_iff_type type;
    sx_iff_chunk* chunks;    // sx_array
    const sx_alloc* alloc;
    union {
        sx_mem_reader* mem_reader;
        sx_file_reader* disk_reader;
    };
} sx_iff_reader;

SX_API void sx_iff_init_reader(sx_iff_reader* reader, sx_iff_flags flags);
SX_API int sx_iff_get_chunk(sx_iff_reader* reader, uint32_t fourcc, int parent_chunk);
SX_API int sx_iff_get_next_chunk(sx_iff_reader* reader, int chunk_id, int prev_chunk);
SX_API bool sx_iff_read_chunk();

#endif
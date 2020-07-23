//
// Copyright 2018 Sepehr Taghdisian (septag@github). All rights reserved.
// License: https://github.com/septag/sx#license-bsd-2-clause
//
// clang-format off
// io.h v1.0: Some IO primitives like streaming memory reader/writer. File and IFF format implementation
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
//                                          if chunk is not found, returning `pos` variable will be -1                                    -1
//
//      sx_file: file related IO functions
//              sx_file_open                opens a file for writing or reading, initiates file objects
//              sx_file_close               closes an opened file
//              sx_file_read                Reads a buffer from file, offsets file pointer 
//              sx_file_write               Writes a buffer to file, offsets file pointer
//              sx_file_seek                Sets file pointer, offset is in bytes
//              
//              sx_file_load_bin            allocates memory and loads the file data into it
//              sx_file_load_text           Same as binary, but appends a null terminator to the end of the buffer
//              
//              sx_file_write_var           Helper macro: writes a variable to file (no need for sizeof)
//              sx_file_write_text          Helper macro: writes a string to file (no need for strlen)
//              sx_file_read_var            Helper macro: reads a variable from file (no need for sizeof)
//
//      sx_iff_file: binary IFF-like file writer/reader
//                   IFF is an old binary format, that is chunked-based. Each chunk has a FOURCC Id 
//                   and can have multiple chunks as a child. 
//                   There is no TOC, so new chunks can be appended to the end of the file, 
//                   you can even mix different types of data into a single IFF file
//                   Reference: https://en.wikipedia.org/wiki/Interchange_File_Format   
//              
//          sx_iff_init_from_file_reader: initialize IFF file for reading from a disk file. 
//                                        needs an already opened file with read access
//          sx_iff_init_from_file_writer: initialize IFF file for writing to a disk file
//                                        needs an already opened file with write access
//          sx_iff_init_from_mem_reader:  initialize IFF file for reading from memory buffer
//          sx_iff_init_from_mem_writer:  initialize IFF file for writing to memory buffer
//          sx_iff_release:               release any internal arrays and TOCs of chunks
//                                        you should close any file handle (sx_file) after release
//          sx_iff_get_chunk:             searches for a specific chunk with a FOURCC code and 
//                                        a parent_id, parent_id should be 0 if it's on the root level
//                                        Returns the index to sx_iff_file.chunks array if found and -1 if not
//          sx_iff_get_next_chunk:        Searches for the next chunk with the same FOURCC code and 
//                                        the same level (same parent) as the previous one
//                                        The search patterns of multiple chunks would be like:
//                                              int chunk = sx_iff_get_chunk(iff, fourcc, parent_id);
//                                              while (chunk != -1) {
//                                                  sx_iff_read_chunk(chunk, data, ..);
//                                                  chunk = sx_iff_get_next_chunk(iff, chunk);
//                                              }
//          sx_iff_read_chunk:            Read chunk data. You should provide the chunk_id returned 
//                                        by get_chunk family of functions
//          sx_iff_put_chunk:             Writes a chunk to IFF file 
//                                        you should provide parent_id (0 if has no parent)
//                                        and memory to be written
//                                        optionally, you can provide compression values for the chunk
//                                        For example, if you have zipped the data, then you can provide 
//                                        uncompressed_size or hash value to later properly decompress the chunk data
//
// clang-format on                                      
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
    int volatile refcount; 
} sx_mem_block;

SX_API sx_mem_block* sx_mem_create_block(const sx_alloc* alloc, int64_t size,
                                         const void* data sx_default(NULL),
                                         int align sx_default(0));
SX_API sx_mem_block* sx_mem_ref_block(const sx_alloc* alloc, int64_t size, void* data);
SX_API void sx_mem_destroy_block(sx_mem_block* mem);
SX_API void sx_mem_addref(sx_mem_block* mem);

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

SX_API void sx_mem_init_writer(sx_mem_writer* writer, const sx_alloc* alloc, int64_t init_size);
SX_API void sx_mem_release_writer(sx_mem_writer* writer);

SX_API int64_t sx_mem_write(sx_mem_writer* writer, const void* data, int64_t size);
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
SX_API int64_t sx_mem_read(sx_mem_reader* reader, void* data, int64_t size);
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
    SX_FILE_NOCACHE = 0x08,         // disable cache, suitable for large files, best bet is to align buffers to virtual memory pages
    SX_FILE_WRITE_THROUGH = 0x10,   // write-through, write meta information to disk immediately
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

// IFF file
// https://en.wikipedia.org/wiki/Interchange_File_Format
//
#pragma pack(push, 8)
typedef struct sx_iff_chunk {
    int64_t pos;
    int64_t size;
    int64_t uncompressed_size;  // optional: these fields can be used when we have compressed chunks
    uint64_t hash;              // optional: hash value for chunks
    uint32_t fourcc;
    int parent_id;
    uint32_t reserved[2];
} sx_iff_chunk;

typedef enum sx_iff_type {
    SX_IFFTYPE_MEM_READER,
    SX_IFFTYPE_MEM_WRITER,
    SX_IFFTYPE_DISK_READER,
    SX_IFFTYPE_DISK_WRITER
} sx_iff_type;

typedef enum sx_iff_flag {
    SX_IFFFLAG_READ_ALL_CHUNKS = 0x1,   // Read all chunks on initialize. This may be faster for 
                                        // searching through multiple chunks, but not recommended 
                                        // for sequntial reading of binary files
    SX_IFFFLAG_APPEND = 0x2             // initialize IFF as writing and appending to the end
} sx_iff_flag;
typedef uint32_t sx_iff_flags;

typedef struct sx_iff_file {
    sx_iff_type type;
    sx_iff_chunk* chunks;    // sx_array: chunk_id's are references to this array (chunks[chunk_id])
    const sx_alloc* alloc;
    union {
        sx_mem_reader* mread;
        sx_mem_writer* mwrite;
        sx_file* disk;
    };
    bool read_all;
} sx_iff_file;
#pragma pack(pop)

SX_API bool sx_iff_init_from_file_reader(sx_iff_file* iff, sx_file* file, sx_iff_flags flags,
                                         const sx_alloc* alloc);
SX_API bool sx_iff_init_from_file_writer(sx_iff_file* iff, sx_file* file, sx_iff_flags flags,
                                         const sx_alloc* alloc);
SX_API bool sx_iff_init_from_mem_reader(sx_iff_file* iff, sx_mem_reader* mread, sx_iff_flags flags,
                                       const sx_alloc* alloc);
SX_API bool sx_iff_init_from_mem_writer(sx_iff_file* iff, sx_mem_writer* mwrite, sx_iff_flags flags,
                                        const sx_alloc* alloc);
SX_API void sx_iff_release(sx_iff_file* iff);

SX_API int sx_iff_get_chunk(sx_iff_file* iff, uint32_t fourcc, int parent_id);
SX_API int sx_iff_get_next_chunk(sx_iff_file* iff, int start_chunk_id);
SX_API bool sx_iff_read_chunk(sx_iff_file* iff, int chunk_id, void* chunk_data, int64_t size);

SX_API int sx_iff_put_chunk(sx_iff_file* iff, int parent_id, uint32_t fourcc,
                            const void* chunk_data, int64_t size, 
                            int64_t uncompressed_size sx_default(0), 
                            uint64_t hash sx_default(0));

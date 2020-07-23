//
// Copyright 2018 Sepehr Taghdisian (septag@github). All rights reserved.
// License: https://github.com/septag/sx#license-bsd-2-clause
//
#include "sx/io.h"
#include "sx/allocator.h"
#include "sx/atomic.h"
#include "sx/os.h"
#include "sx/array.h"

#include <limits.h>
#include <stdio.h>

#if SX_PLATFORM_WINDOWS
#    define WIN32_LEAN_AND_MEAN
#    include "windows.h"
#elif SX_PLATFORM_POSIX
#    define _LARGEFILE64_SOURCE
#    include <sys/stat.h>
#    include <sys/types.h>
#    include <fcntl.h>
#    include <unistd.h>
#    undef _LARGEFILE64_SOURCE
#    ifndef __O_LARGEFILE
#        define __O_LARGEFILE 0
#    endif
#endif    // SX_

#define SIFF_SIGN sx_makefourcc('S', 'I', 'F', 'F')

sx_mem_block* sx_mem_create_block(const sx_alloc* alloc, int64_t size, const void* data, int align)
{
    align = sx_max(align, SX_CONFIG_ALLOCATOR_NATURAL_ALIGNMENT);
    sx_mem_block* mem = (sx_mem_block*)sx_malloc(alloc, (size_t)size + sizeof(sx_mem_block) + align);
    if (mem) {
        mem->alloc = alloc;
        mem->data = sx_align_ptr(mem + 1, 0, align);
        mem->size = size;
        mem->align = align;
        mem->refcount = 1;
        if (data)
            sx_memcpy(mem->data, data, (size_t)size);
        return mem;
    } else {
        sx_out_of_memory();
        return NULL;
    }
}

sx_mem_block* sx_mem_ref_block(const sx_alloc* alloc, int64_t size, void* data)
{
    sx_mem_block* mem = (sx_mem_block*)sx_malloc(alloc, sizeof(sx_mem_block));
    if (mem) {
        mem->alloc = alloc;
        mem->data = data;
        mem->size = size;
        mem->align = 0;
        mem->refcount = 1;
        return mem;
    } else {
        sx_out_of_memory();
        return NULL;
    }
}

void sx_mem_destroy_block(sx_mem_block* mem)
{
    sx_assert(mem);

    if (sx_atomic_decr(&mem->refcount) == 0) {
        if (mem->alloc) {
            sx_free(mem->alloc, mem);
            mem->alloc = NULL;
        }
    }

    sx_assert(mem->refcount >= 0);
}

void sx_mem_addref(sx_mem_block* mem)
{
    sx_atomic_incr(&mem->refcount);
}


void sx_mem_init_block_ptr(sx_mem_block* mem, void* data, int64_t size)
{
    mem->alloc = NULL;
    mem->data = data;
    mem->size = size;
    mem->align = 0;
}

bool sx_mem_grow(sx_mem_block** pmem, int64_t size)
{
    sx_mem_block* mem = *pmem;
    sx_assert(mem->alloc &&
              "Growable memory must be created with an allocator - sx_mem_create_block");
    sx_assert(size > mem->size && "New size must be greater than the previous one");

    int align = mem->align;
    const sx_alloc* alloc = mem->alloc;
    mem = (sx_mem_block*)sx_realloc(alloc, mem, (size_t)size + sizeof(sx_mem_block) + align);
    if (mem) {
        mem->data = sx_align_ptr(mem + 1, 0, align);
        mem->size = size;
        *pmem = mem;
        return true;
    } else {
        sx_out_of_memory();
        return false;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
void sx_mem_init_writer(sx_mem_writer* writer, const sx_alloc* alloc, int64_t init_size)
{
    sx_assert(alloc);
    if (init_size <= 0)
        init_size = 4096;

    writer->mem = sx_mem_create_block(alloc, init_size, NULL, 0);
    sx_assert(writer->mem);

    writer->data = (uint8_t*)writer->mem->data;
    writer->pos = writer->top = 0;
    writer->size = writer->mem->size;
}

void sx_mem_release_writer(sx_mem_writer* writer)
{
    sx_assert(writer);
    if (writer->mem) {
        sx_mem_destroy_block(writer->mem);
    }
}

int64_t sx_mem_write(sx_mem_writer* writer, const void* data, int64_t size)
{
    sx_mem_block* mem = writer->mem;

    // need to grow memory ?
    int64_t remain = writer->size - writer->pos;
    if (size > remain) {
        if (mem->alloc) {
            int64_t more = size - remain;
            more = sx_align_mask(more, 0xfff);    // align to 4096 bytes
            bool _r = sx_mem_grow(&mem, more + mem->size);
            sx_unused(_r);
            sx_assert(_r);
            writer->mem = mem;
            writer->data = (uint8_t*)mem->data;
            writer->size = mem->size;
        } else {
            size = remain;
            sx_data_truncate();
        }
    }

    sx_memcpy(&writer->data[writer->pos], data, (size_t)size);
    writer->pos += size;
    writer->top = sx_max(writer->top, writer->pos);

    return size;
}

int64_t sx_mem_seekw(sx_mem_writer* writer, int64_t offset, sx_whence whence)
{
    switch (whence) {
    case SX_WHENCE_BEGIN:
        writer->pos = sx_clamp(offset, (int64_t)0ll, writer->top);
        break;
    case SX_WHENCE_CURRENT:
        writer->pos = sx_clamp(writer->pos + offset, (int64_t)0ll, writer->top);
        break;
    case SX_WHENCE_END:
        writer->pos = sx_clamp(writer->top - offset, (int64_t)0ll, writer->top);
        break;
    }
    return writer->pos;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
void sx_mem_init_reader(sx_mem_reader* reader, const void* data, int64_t size)
{
    sx_assert(data);
    sx_assert(size);

    reader->data = (const uint8_t*)data;
    reader->top = size;
    reader->pos = 0;
}

int64_t sx_mem_read(sx_mem_reader* reader, void* data, int64_t size)
{
    int64_t remain = reader->top - reader->pos;
    if (size > remain) {
        size = remain;
        sx_data_truncate();
    }
    sx_memcpy(data, &reader->data[reader->pos], (size_t)size);
    reader->pos += size;
    return size;
}

int64_t sx_mem_seekr(sx_mem_reader* reader, int64_t offset, sx_whence whence)
{
    switch (whence) {
    case SX_WHENCE_BEGIN:
        reader->pos = sx_clamp(offset, (int64_t)0ll, reader->top);
        break;
    case SX_WHENCE_CURRENT:
        reader->pos = sx_clamp(reader->pos + offset, (int64_t)0ll, reader->top);
        break;
    case SX_WHENCE_END:
        reader->pos = sx_clamp((reader->top - offset), (int64_t)0ll, reader->top);
        break;
    }
    return reader->pos;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
#if SX_PLATFORM_WINDOWS

typedef struct sx__file_win32 {
    HANDLE handle;
    sx_file_open_flags flags;
    int64_t size;
} sx__file_win32;

bool sx_file_open(sx_file* file, const char* filepath, sx_file_open_flags flags)
{
    sx_assert((flags & (SX_FILE_READ|SX_FILE_WRITE)) != (SX_FILE_READ|SX_FILE_WRITE));
    sx_assert((flags & (SX_FILE_READ|SX_FILE_WRITE)) != 0);

    sx__file_win32* f = (sx__file_win32*)file;
    sx_memset(f, 0x0, sizeof(sx__file_win32));
    
    uint32_t access_flags = GENERIC_READ;
    uint32_t attrs = FILE_ATTRIBUTE_NORMAL;
    uint32_t create_flags = 0;
    uint32_t share_flags = 0;

    if (flags & SX_FILE_READ) {
        create_flags = OPEN_EXISTING;
        share_flags |= FILE_SHARE_READ;
    } else if (flags & SX_FILE_WRITE) {
        share_flags |= FILE_SHARE_WRITE;
        access_flags |= GENERIC_WRITE;
        create_flags |= (flags & SX_FILE_APPEND) ? OPEN_EXISTING : CREATE_ALWAYS;
    }

    if (flags & SX_FILE_NOCACHE) {
        attrs |= FILE_FLAG_NO_BUFFERING;
    }

    if (flags & SX_FILE_WRITE_THROUGH) {
        attrs |= FILE_FLAG_WRITE_THROUGH;
    }

    if (flags & SX_FILE_SEQ_SCAN) {
        attrs |= FILE_FLAG_SEQUENTIAL_SCAN;
    }

    if (flags & SX_FILE_RANDOM_ACCESS) {
        attrs |= FILE_FLAG_RANDOM_ACCESS;
    }

    if (flags & SX_FILE_TEMP) {
        attrs |= FILE_ATTRIBUTE_TEMPORARY;
    }

    HANDLE hfile = CreateFileA(filepath, access_flags, share_flags, NULL, create_flags, attrs, NULL);
    if (hfile == INVALID_HANDLE_VALUE) {
        return false;
    }

    f->handle = hfile;
    f->flags = flags;
    if (flags & (SX_FILE_READ|SX_FILE_APPEND)) {
        LARGE_INTEGER llsize;
        GetFileSizeEx(hfile, &llsize);
        f->size = (int64_t)llsize.QuadPart;
    } else {
        f->size = 0;
    }

    return true;
}

void sx_file_close(sx_file* file)
{
    sx_assert(file);
    sx__file_win32* f = (sx__file_win32*)file;
    if (f->handle && f->handle != INVALID_HANDLE_VALUE) {
        CloseHandle(f->handle);
        f->handle = NULL;
    }
}

int64_t sx_file_read(sx_file* file, void* data, int64_t size)
{
    sx_assert(file);
    sx__file_win32* f = (sx__file_win32*)file;

    sx_assert(f->handle && f->handle != INVALID_HANDLE_VALUE);
    sx_assert_rel(size < UINT32_MAX);

    if (f->flags & SX_FILE_NOCACHE) {
        static size_t pagesz = 0;
        if (pagesz == 0) {
            pagesz = sx_os_pagesz();
        }
        sx_assert_rel((uintptr_t)data % pagesz == 0 && "buffers must be aligned with NoCache flag");
    }

    DWORD bytes_read;
    if (!ReadFile(f->handle, data, (DWORD)size, &bytes_read, NULL)) {
        return -1;
    }

    return (int64_t)bytes_read;
}

int64_t sx_file_write(sx_file* file, const void* data, int64_t size)
{
    sx_assert(file);
    sx__file_win32* f = (sx__file_win32*)file;

    sx_assert(f->handle && f->handle != INVALID_HANDLE_VALUE);
    sx_assert_rel(size < UINT32_MAX);

    DWORD bytes_written;
    if (!WriteFile(f->handle, data, (DWORD)size, &bytes_written, NULL)) {
        return -1;
    }
    f->size += bytes_written;

    return bytes_written;
}

int64_t sx_file_seek(sx_file* file, int64_t offset, sx_whence whence)
{
    sx_assert(file);

    sx__file_win32* f = (sx__file_win32*)file;

    DWORD move_method = 0;
    switch (whence) {
    case SX_WHENCE_BEGIN:
        move_method = FILE_BEGIN;
        break;
    case SX_WHENCE_CURRENT:
        move_method = FILE_CURRENT;
        break;
    case SX_WHENCE_END:
        sx_assert(offset <= f->size);
        move_method = FILE_END;
        break;
    }

    LARGE_INTEGER large_off;
    LARGE_INTEGER large_ret;
    large_off.QuadPart = (LONGLONG)offset;

    if (SetFilePointerEx(f->handle, large_off, &large_ret, move_method)) {
        return (int64_t)large_ret.QuadPart;
    }

    return -1;
}

int64_t sx_file_size(const sx_file* file)
{
    sx_assert(file);

    sx__file_win32* f = (sx__file_win32*)file;
    return f->size;
}

#elif SX_PLATFORM_POSIX // if SX_PLATFORM_WINDOWS

typedef struct sx__file_posix {
    int id;
    uint32_t flags;
    int64_t size;
} sx__file_posix;

bool sx_file_open(sx_file* file, const char* filepath, sx_file_open_flags flags)
{
    sx_assert((flags & (SX_FILE_READ|SX_FILE_WRITE)) != (SX_FILE_READ|SX_FILE_WRITE));
    sx_assert((flags & (SX_FILE_READ|SX_FILE_WRITE)) != 0);

    sx__file_posix* f = (sx__file_posix*)file;
    sx_memset(f, 0x0, sizeof(sx__file_posix));
    
    int open_flags = __O_LARGEFILE;
    mode_t mode = 0;

    if (flags & SX_FILE_READ) {
        open_flags |= O_RDONLY;
    } else if (flags & SX_FILE_WRITE) {
        open_flags |= O_WRONLY;
        if (flags & SX_FILE_APPEND) {
            open_flags |= O_APPEND;
        } else {
            open_flags |= (O_CREAT | O_TRUNC);
            mode |= (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH); 
        }
    }

#if (SX_PLATFORM_LINUX || SX_PLATFORM_RPI || SX_PLATFORM_ANDROID)
    if (flags & SX_FILE_TEMP) {
        open_flags |= __O_TMPFILE;
    }
#endif // 

    int file_id = open(filepath, open_flags, mode);
    if (file_id == -1) {
        return false;
    }

#if SX_PLATFORM_APPLE
    if (flags & SX_FILE_NOCACHE) {
        if (fcntl(file_id, F_NOCACHE) != 0) {
            return false;
        }
    }
#endif

    struct stat _stat;
    int sr = fstat(file_id, &_stat);
    if (sr != 0) {
        sx_assert(0 && "stat failed!");
        return false;
    }
    f->id = file_id;
    f->flags = flags;
    f->size = (int64_t)_stat.st_size;
    return true;
}

void sx_file_close(sx_file* file)
{
    sx_assert(file);
    sx__file_posix* f = (sx__file_posix*)file;
    if (f->id) {
        close(f->id);
        f->id = 0;
    }
}

int64_t sx_file_read(sx_file* file, void* data, int64_t size)
{
    sx_assert(file);
    sx__file_posix* f = (sx__file_posix*)file;
    sx_assert(f->id && f->id != -1);
    
    if (f->flags & SX_FILE_NOCACHE) {
        static size_t pagesz = 0;
        if (pagesz == 0) {
            pagesz = sx_os_pagesz();
        }
        sx_assert_rel((uintptr_t)data % pagesz == 0 && "buffers must be aligned with NoCache flag");
    }
    return read(f->id, data, (size_t)size);
}

int64_t sx_file_write(sx_file* file, const void* data, int64_t size)
{
    sx_assert(file);
    sx__file_posix* f = (sx__file_posix*)file;

    sx_assert(f->id && f->id != -1);
    int64_t bytes_written = write(f->id, data, (size_t)size);
    if (bytes_written > -1) {
        f->size += bytes_written; 
    }
    return bytes_written;
}

int64_t sx_file_seek(sx_file* file, int64_t offset, sx_whence whence)
{
    sx_assert(file);

    sx__file_posix* f = (sx__file_posix*)file;
    sx_assert(f->id && f->id != -1);

    int _whence = 0;
    switch (whence) {
    case SX_WHENCE_CURRENT:
        _whence = SEEK_CUR;
        break;
    case SX_WHENCE_BEGIN:
        _whence = SEEK_SET;
        break;
    case SX_WHENCE_END:
        _whence = SEEK_END;
    }

    return lseek(f->id, offset, _whence);
}

int64_t sx_file_size(const sx_file* file)
{
    sx_assert(file);

    sx__file_posix* f = (sx__file_posix*)file;
    return f->size;
}

#endif  // elif SX_PLATFORM_POSIX


sx_mem_block* sx_file_load_text(const sx_alloc* alloc, const char* filepath)
{
    sx_file f;
    if (sx_file_open(&f, filepath, SX_FILE_READ)) {
        int64_t size = sx_file_size(&f);
        if (size > 0) {
            sx_mem_block* mem = sx_mem_create_block(alloc, size + 1, NULL, 0);
            if (mem) {
                sx_file_read(&f, mem->data, size);
                sx_file_close(&f);
                ((char*)mem->data)[size] = '\0';    // close the string
                return mem;
            }
        }

        sx_file_close(&f);
    }
    return NULL;
}

sx_mem_block* sx_file_load_bin(const sx_alloc* alloc, const char* filepath)
{
    sx_file f;
    if (sx_file_open(&f, filepath, SX_FILE_READ)) {
        int64_t size = sx_file_size(&f);
        if (size > 0) {
            sx_mem_block* mem = sx_mem_create_block(alloc, size, NULL, 0);
            if (mem) {
                sx_file_read(&f, mem->data, size);
                sx_file_close(&f);
                return mem;
            }
        }

        sx_file_close(&f);
    }
    return NULL;    
}

//
static inline int64_t sx__iff_read(sx_iff_file* iff, void* data, int64_t size)
{
    switch (iff->type) {
    case SX_IFFTYPE_MEM_READER:
        return sx_mem_read(iff->mread, data, size);
    case SX_IFFTYPE_DISK_READER:
    case SX_IFFTYPE_DISK_WRITER:
        return sx_file_read(iff->disk, data, size);
    default:
        sx_assert(0);
        return -1;
    }
}

static inline int64_t sx__iff_write(sx_iff_file* iff, const void* data, int64_t size)
{
    switch (iff->type) {
    case SX_IFFTYPE_MEM_WRITER:
        return sx_mem_write(iff->mwrite, data, size);
    case SX_IFFTYPE_DISK_WRITER:
        return sx_file_write(iff->disk, data, size);
    default:
        sx_assert(0);
        return -1;
    }
}

static inline int64_t sx__iff_seek(sx_iff_file* iff, int64_t offset, sx_whence whence)
{
    switch (iff->type) {
    case SX_IFFTYPE_MEM_READER:
        return sx_mem_seekr(iff->mread, offset, whence);
    case SX_IFFTYPE_MEM_WRITER:
        return sx_mem_seekw(iff->mwrite, offset, whence);
    case SX_IFFTYPE_DISK_WRITER:
    case SX_IFFTYPE_DISK_READER:
        return sx_file_seek(iff->disk, offset, whence);
    default:
        sx_assert(0);
        return -1;
    }
}

static bool sx__iff_read_all_chunks(sx_iff_file* iff)
{
    // start reading from the current pos, and add all chunks to the database
    while(1) {
        sx_iff_chunk chunk;
        int64_t r = sx__iff_read(iff, &chunk, sizeof(chunk));
        if (r < (int64_t)sizeof(chunk)) {
            sx_assert_rel(r == 0 && "file is probably corrupt");
            return r == 0;
        }

        int64_t pos = sx__iff_seek(iff, chunk.size, SX_WHENCE_CURRENT);
        if (pos <= 0 || (pos - chunk.pos) < chunk.size) {
            sx_assert_rel(0 && "file is probably corrupt");
            return false;  
        }

        sx_array_push(iff->alloc, iff->chunks, chunk);
    }    
    sx_assert(0);   // unreachable
    return false;
}

bool sx_iff_init_from_file_reader(sx_iff_file* iff, sx_file* file, sx_iff_flags flags,
                                  const sx_alloc* alloc)
{
    sx_assert(iff);
    sx_assert(file);

    *iff = (sx_iff_file) {
        .type = SX_IFFTYPE_DISK_READER,
        .alloc = alloc,
        .disk = file
    };

    // read first chunk
    sx_iff_chunk first_chunk;
    sx__iff_read(iff, &first_chunk, sizeof(first_chunk));
    if (first_chunk.fourcc != SIFF_SIGN || first_chunk.parent_id != -1 || first_chunk.size) {
        sx_assert(0 && "invalid IFF file format");
        return false;
    }
    sx_array_push(alloc, iff->chunks, first_chunk);

    bool r = true;
    if (flags & SX_IFFFLAG_READ_ALL_CHUNKS) {
        r = sx__iff_read_all_chunks(iff);
        iff->read_all = true;
    }

    return r;
}

bool sx_iff_init_from_file_writer(sx_iff_file* iff, sx_file* file, sx_iff_flags flags,
                                  const sx_alloc* alloc)
{
    sx_assert(iff);
    sx_assert(file);

    *iff = (sx_iff_file) {
        .type = SX_IFFTYPE_DISK_WRITER,
        .alloc = alloc,
        .disk = file
    };

    bool r = true;
    if (flags & SX_IFFFLAG_APPEND) {
        // read first chunk (validate)
        sx__iff_seek(iff, 0, SX_WHENCE_BEGIN);
        sx_iff_chunk first_chunk;
        int64_t bytes_read = sx__iff_read(iff, &first_chunk, sizeof(first_chunk));
        if (bytes_read != sizeof(first_chunk) || first_chunk.fourcc != SIFF_SIGN || 
            first_chunk.parent_id != -1 || first_chunk.size) {
            sx_assert(0 && "invalid IFF file format");
            return false;
        }
        sx_array_push(alloc, iff->chunks, first_chunk);

        if (flags & SX_IFFFLAG_READ_ALL_CHUNKS) {
            r = sx__iff_read_all_chunks(iff);
            iff->read_all = true;
        }

        sx__iff_seek(iff, 0, SX_WHENCE_END);
    } else {
        // write first chunk
        sx_iff_chunk chunk = {
            .fourcc = SIFF_SIGN,
            .parent_id = -1
        };
        sx__iff_write(iff, &chunk, sizeof(chunk));
        sx_array_push(alloc, iff->chunks, chunk);
    }

    return r;
}

bool sx_iff_init_from_mem_reader(sx_iff_file* iff, sx_mem_reader* mread, sx_iff_flags flags,
                                 const sx_alloc* alloc)
{
    sx_assert(iff);
    sx_assert(mread);

    *iff = (sx_iff_file) {
        .type = SX_IFFTYPE_MEM_READER,
        .alloc = alloc,
        .mread = mread
    };

    // read first chunk
    sx_iff_chunk first_chunk;
    sx__iff_read(iff, &first_chunk, sizeof(first_chunk));
    if (first_chunk.fourcc != SIFF_SIGN || first_chunk.parent_id != -1 || first_chunk.size) {
        sx_assert(0 && "invalid IFF file format");
        return false;
    }
    sx_array_push(alloc, iff->chunks, first_chunk);

    bool r = true;
    if (flags & SX_IFFFLAG_READ_ALL_CHUNKS) {
        r = sx__iff_read_all_chunks(iff);
        iff->read_all = true;
    }

    return r;
}

bool sx_iff_init_from_mem_writer(sx_iff_file* iff, sx_mem_writer* mwrite, sx_iff_flags flags,
                                 const sx_alloc* alloc)
{
    sx_assert(iff);
    sx_assert(mwrite);

    *iff = (sx_iff_file) {
        .type = SX_IFFTYPE_MEM_WRITER,
        .alloc = alloc,
        .mwrite = mwrite
    };

    bool r = true;
    if (flags & SX_IFFFLAG_APPEND) {
        // read first chunk (validate)
        sx__iff_seek(iff, 0, SX_WHENCE_BEGIN);
        sx_iff_chunk first_chunk;
        sx__iff_read(iff, &first_chunk, sizeof(first_chunk));
        if (first_chunk.fourcc != SIFF_SIGN || first_chunk.parent_id != -1 || first_chunk.size) {
            sx_assert(0 && "invalid IFF file format");
            return false;
        }
        sx_array_push(alloc, iff->chunks, first_chunk);

        if (flags & SX_IFFFLAG_READ_ALL_CHUNKS) {
            r = sx__iff_read_all_chunks(iff);
            iff->read_all = true;
        }

        sx__iff_seek(iff, 0, SX_WHENCE_END);
    } else {
        // write first chunk
        sx_iff_chunk chunk = {
            .fourcc = SIFF_SIGN,
            .parent_id = -1
        };
        sx__iff_write(iff, &chunk, sizeof(chunk));
        sx_array_push(alloc, iff->chunks, chunk);
    }

    return r;
}

void sx_iff_release(sx_iff_file* iff)
{
    sx_assert(iff);
    sx_array_free(iff->alloc, iff->chunks);
}

int sx_iff_get_chunk(sx_iff_file* iff, uint32_t fourcc, int parent_id)
{
    sx_assert(iff);
    sx_assert(sx_array_count(iff->chunks) > parent_id);

    if (!iff->read_all) {
        // start reading from the current pos, and add all chunks to the database
        while(1) {
            sx_iff_chunk chunk;
            int64_t r = sx__iff_read(iff, &chunk, sizeof(chunk));
            if (r < (int64_t)sizeof(chunk)) {
                sx_assert_rel(r == 0 && "file is probably corrupt");
                iff->read_all = true;
                break;  // EOF
            }

            int64_t pos = sx__iff_seek(iff, chunk.size, SX_WHENCE_CURRENT);
            if (pos <= 0 || (pos - chunk.pos) < chunk.size) {
                sx_assert_rel(0 && "file is probably corrupt");
                break;  
            }

            sx_array_push(iff->alloc, iff->chunks, chunk);
            if (chunk.fourcc == fourcc && parent_id == chunk.parent_id) {
                return sx_array_count(iff->chunks) - 1;
            } 
        }
    } else {
        // search in existing chunks
        for (int i = 0, c = sx_array_count(iff->chunks); i < c; i++) {
            if (iff->chunks[i].fourcc == fourcc && iff->chunks[i].parent_id == parent_id) {
                return i;
            }
        }
    }

    return -1;
}

bool sx_iff_read_chunk(sx_iff_file* iff, int chunk_id, void* chunk_data, int64_t size)
{
    sx_assert(iff);
    sx_assert(sx_array_count(iff->chunks) > chunk_id);

    sx_iff_chunk* chunk = &iff->chunks[chunk_id];
    if (chunk->size != size) {
        sx_assert(0 && "size does not match the actual chunk size");
        return false;
    }

    int64_t pos = sx__iff_seek(iff, chunk->pos, SX_WHENCE_BEGIN);
    sx_unused(pos);
    sx_assert_rel(pos == chunk->pos && "probably file corruption");

    int64_t r = sx__iff_read(iff, chunk_data, size);
    if (r != chunk->size) {
        sx_assert_rel(0 && "corrupt file");
        return false;
    }

    return true;
}

int sx_iff_get_next_chunk(sx_iff_file* iff, int start_chunk_id)
{
    sx_assert(iff);
    sx_assert(sx_array_count(iff->chunks) > start_chunk_id);

    sx_iff_chunk* chunk = &iff->chunks[start_chunk_id];
    int parent_id = chunk->parent_id;

    if (!iff->read_all) {
        return sx_iff_get_chunk(iff, chunk->fourcc, parent_id);
    } else {
        for (int i = start_chunk_id + 1, c = sx_array_count(iff->chunks); i < c; i++) {
            const sx_iff_chunk* test_chunk = &iff->chunks[i];
            if (test_chunk->fourcc == chunk->fourcc && test_chunk->parent_id == parent_id) {
                return i;
            }
        }
        return -1;
    }
}

int sx_iff_put_chunk(sx_iff_file* iff, int parent_id, uint32_t fourcc, const void* chunk_data, 
                     int64_t size, int64_t uncompressed_size, uint64_t hash)
{
    sx_assert(iff);
    sx_assert(sx_array_count(iff->chunks) > parent_id);

    int64_t pos = sx__iff_seek(iff, 0, SX_WHENCE_END);

    sx_iff_chunk chunk = {
        .pos = pos + sizeof(sx_iff_chunk),
        .size = (int64_t)size,
        .uncompressed_size = uncompressed_size,
        .hash = hash,
        .fourcc = fourcc,
        .parent_id = parent_id
    };
    sx__iff_write(iff, &chunk, sizeof(chunk));
    sx__iff_write(iff, chunk_data, size);
    sx_array_push(iff->alloc, iff->chunks, chunk);
    return sx_array_count(iff->chunks) - 1;
}


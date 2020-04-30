//
// Copyright 2018 Sepehr Taghdisian (septag@github). All rights reserved.
// License: https://github.com/septag/sx#license-bsd-2-clause
//
#include "sx/io.h"
#include "sx/allocator.h"
#include "sx/os.h"

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

sx_mem_block* sx_mem_create_block(const sx_alloc* alloc, int64_t size, const void* data, int align)
{
    align = sx_max(align, SX_CONFIG_ALLOCATOR_NATURAL_ALIGNMENT);
    sx_mem_block* mem = (sx_mem_block*)sx_malloc(alloc, size + sizeof(sx_mem_block) + align);
    if (mem) {
        mem->alloc = alloc;
        mem->data = sx_align_ptr(mem + 1, 0, align);
        mem->size = size;
        mem->align = align;
        if (data)
            sx_memcpy(mem->data, data, size);
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
        return mem;
    } else {
        sx_out_of_memory();
        return NULL;
    }
}

void sx_mem_destroy_block(sx_mem_block* mem)
{
    sx_assert(mem);

    if (mem->alloc) {
        sx_free(mem->alloc, mem);
    }
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
    mem = (sx_mem_block*)sx_realloc(alloc, mem, size + sizeof(sx_mem_block) + align);
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
void sx_mem_init_writer(sx_mem_writer* writer, const sx_alloc* alloc, int init_size)
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

int sx_mem_write(sx_mem_writer* writer, const void* data, int size)
{
    sx_mem_block* mem = writer->mem;

    // need to grow memory ?
    int64_t remain = writer->size - writer->pos;
    if (size > (int)remain) {
        if (mem->alloc) {
            int more = size - (int)remain;
            more = sx_align_mask(more, 0xfff);    // align to 4096 bytes
            bool _r = sx_mem_grow(&mem, more + mem->size);
            sx_unused(_r);
            sx_assert(_r);
            writer->mem = mem;
            writer->data = (uint8_t*)mem->data;
            writer->size = mem->size;
        } else {
            size = (int)remain;
            sx_data_truncate();
        }
    }

    sx_memcpy(&writer->data[writer->pos], data, size);
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

int sx_mem_read(sx_mem_reader* reader, void* data, int size)
{
    int64_t remain = reader->top - reader->pos;
    if (size > (int)remain) {
        size = (int)remain;
        sx_data_truncate();
    }
    sx_memcpy(data, &reader->data[reader->pos], size);
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
        if (GetLastError() != ERROR_FILE_NOT_FOUND ) {
            sx_assert_rel(0 && "Unknown CreateFile error");
        }
        return false;
    }

    f->handle = hfile;
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


sx_iff_chunk sx_mem_get_iff_chunk(sx_mem_reader* reader, int64_t size, uint32_t fourcc)
{
    int64_t end = (size > 0) ? sx_min(reader->pos + size, reader->top) : reader->top;
    end -= 8;
    if (reader->pos >= end) {
        return (sx_iff_chunk){ .pos = -1 };
    }

    uint32_t ch = *((uint32_t*)(reader->data + reader->pos));
    if (ch == fourcc) {
        reader->pos += sizeof(uint32_t);
        uint32_t chunk_size;
        sx_mem_read_var(reader, chunk_size);
        return (sx_iff_chunk){ .pos = reader->pos, .size = chunk_size };
    }

    // chunk not found at start position, try to find it in the remaining data by brute-force
    const uint8_t* buff = reader->data;
    for (int64_t offset = reader->pos; offset < end; offset++) {
        ch = *((uint32_t*)(buff + offset));
        if (ch == fourcc) {
            reader->pos = offset + sizeof(uint32_t);
            uint32_t chunk_size;
            sx_mem_read_var(reader, chunk_size);
            return (sx_iff_chunk){ .pos = reader->pos, .size = chunk_size };
        }
    }

    return (sx_iff_chunk){ .pos = -1 };
}


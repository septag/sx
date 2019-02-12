//
// Copyright 2018 Sepehr Taghdisian (septag@github). All rights reserved.
// License: https://github.com/septag/sx#license-bsd-2-clause
//
#include "sx/io.h"
#include "sx/allocator.h"

#include <limits.h>
#include <stdio.h>

#if SX_PLATFORM_WINDOWS
#    define fseeko64 _fseeki64
#    define ftello64 _ftelli64
#elif 0 || SX_PLATFORM_ANDROID || SX_PLATFORM_BSD || SX_PLATFORM_IOS || SX_PLATFORM_OSX || \
    SX_PLATFORM_LINUX || SX_PLATFORM_EMSCRIPTEN
#    define fseeko64 fseeko
#    define ftello64 ftello
#elif SX_PLATFORM_PS4
#    define fseeko64 fseek
#    define ftello64 ftell
#endif    // SX_

sx_mem_block* sx_mem_create_block(const sx_alloc* alloc, int size, const void* data, int align) {
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

sx_mem_block* sx_mem_ref_block(const sx_alloc* alloc, int size, void* data) {
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

void sx_mem_destroy_block(sx_mem_block* mem) {
    sx_assert(mem);

    if (mem->alloc) {
        sx_free(mem->alloc, mem);
    }
}

void sx_mem_init_block_ptr(sx_mem_block* mem, void* data, int size) {
    mem->alloc = NULL;
    mem->data = data;
    mem->size = size;
    mem->align = 0;
}

void* sx_mem_grow(sx_mem_block* mem, int size) {
    sx_assert(mem->alloc && "Growable memory must be created with an allocator!");
    sx_assert(size > mem->size && "New size must be greater than the previous one");

    mem->data = sx_aligned_realloc(mem->alloc, mem->data, size, mem->align);
    mem->size = size;
    return mem->data;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
void sx_mem_init_writer(sx_mem_writer* writer, sx_mem_block* mem) {
    sx_assert(mem && mem->size > 0);

    writer->mem = mem;
    writer->data = (uint8_t*)mem->data;
    writer->pos = writer->top = 0;
    writer->size = mem->size;
}

int sx_mem_write(sx_mem_writer* writer, const void* data, int size) {
    sx_mem_block* mem = writer->mem;

    // need to grow memory ?
    int64_t remain = writer->size - writer->pos;
    if (size > (int)remain) {
        if (mem->alloc) {
            int more = size - (int)remain;
            more = sx_align_mask(more, 0xfff);    // align to 4096 bytes
            writer->data = (uint8_t*)sx_mem_grow(mem, more + mem->size);
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

int64_t sx_mem_seekw(sx_mem_writer* writer, int64_t offset, sx_whence whence) {
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
void sx_mem_init_reader(sx_mem_reader* reader, const void* data, int64_t size) {
    sx_assert(data);
    sx_assert(size);

    reader->data = (const uint8_t*)data;
    reader->top = size;
    reader->pos = 0;
}

int sx_mem_read(sx_mem_reader* reader, void* data, int size) {
    int64_t remain = reader->top - reader->pos;
    if (size > (int)remain) {
        size = (int)remain;
        sx_data_truncate();
    }
    sx_memcpy(data, &reader->data[reader->pos], size);
    reader->pos += size;
    return size;
}

int64_t sx_mem_seekr(sx_mem_reader* reader, int64_t offset, sx_whence whence) {
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
//
typedef struct sx__file_data {
    FILE* f;
} sx__file_data;

bool sx_file_open_writer(sx_file_writer* writer, const char* filepath, uint32_t flags) {
    static_assert(sizeof(writer->data) >= sizeof(sx__file_data), "Invalid data buffer size");

    sx__file_data* data = (sx__file_data*)writer->data;
    data->f = fopen(filepath, (flags & SX_FILE_OPEN_APPEND) ? "ab" : "wb");
    return data->f != NULL;
}

void sx_file_close_writer(sx_file_writer* writer) {
    sx__file_data* data = (sx__file_data*)writer->data;
    if (data->f) {
        fclose(data->f);
        data->f = NULL;
    }
}

int sx_file_write(sx_file_writer* writer, const void* data, int size) {
    sx__file_data* fdata = (sx__file_data*)writer->data;
    int            written = (int)fwrite(data, 1, size, fdata->f);
    if (written != size) {
        sx_data_truncate();
    }
    return written;
}

int64_t sx_file_seekw(sx_file_writer* writer, int64_t offset, sx_whence whence) {
    sx__file_data* fdata = (sx__file_data*)writer->data;
    fseeko64(fdata->f, offset, whence);
    return ftello64(fdata->f);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
bool sx_file_open_reader(sx_file_reader* reader, const char* filepath) {
    static_assert(sizeof(reader->data) >= sizeof(sx__file_data), "Invalid data buffer size");

    sx__file_data* data = (sx__file_data*)reader->data;
    data->f = fopen(filepath, "rb");
    return data->f != NULL;
}

void sx_file_close_reader(sx_file_reader* reader) {
    sx__file_data* data = (sx__file_data*)reader->data;
    if (data->f) {
        fclose(data->f);
        data->f = NULL;
    }
}

int sx_file_read(sx_file_reader* reader, void* data, int size) {
    sx__file_data* fdata = (sx__file_data*)reader->data;
    int            r = (int)fread(data, 1, size, fdata->f);
    if (r < size) {
        sx_data_truncate();
    }
    return r;
}

int64_t sx_file_seekr(sx_file_reader* reader, int64_t offset, sx_whence whence) {
    sx__file_data* fdata = (sx__file_data*)reader->data;
    fseeko64(fdata->f, offset, whence);
    return ftello64(fdata->f);
}

sx_mem_block* sx_file_load_text(const sx_alloc* alloc, const char* filepath) {
    sx_file_reader reader;
    if (sx_file_open_reader(&reader, filepath)) {
        int64_t sz = sx_file_seekr(&reader, 0, SX_WHENCE_END);
        if (sz > 0) {
            sx_assert(sz < INT_MAX - 1);
            sx_file_seekr(&reader, 0, SX_WHENCE_BEGIN);
            sx_mem_block* mem = sx_mem_create_block(alloc, (int)sz + 1, NULL, 0);
            if (mem) {
                sx_file_read(&reader, mem->data, (int)sz);
                sx_file_close_reader(&reader);
                ((char*)mem->data)[sz] = '\0';    // close the string
                return mem;
            }
        }
        sx_file_close_reader(&reader);
    }
    return NULL;
}

sx_mem_block* sx_file_load_bin(const sx_alloc* alloc, const char* filepath) {
    sx_file_reader reader;
    if (sx_file_open_reader(&reader, filepath)) {
        int64_t sz = sx_file_seekr(&reader, 0, SX_WHENCE_END);
        if (sz > 0) {
            sx_assert(sz < INT_MAX - 1);
            sx_file_seekr(&reader, 0, SX_WHENCE_BEGIN);
            sx_mem_block* mem = sx_mem_create_block(alloc, (int)sz, NULL, 0);
            if (mem) {
                sx_file_read(&reader, mem->data, (int)sz);
                sx_file_close_reader(&reader);
                return mem;
            }
        }
        sx_file_close_reader(&reader);
    }
    return NULL;
}

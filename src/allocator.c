//
// Copyright 2018 Sepehr Taghdisian (septag@github). All rights reserved.
// License: https://github.com/septag/sx#license-bsd-2-clause
//
#include "sx/allocator.h"
#include "sx/string.h"

#include <malloc.h>

static void* sx_malloc_cb(void* ptr, size_t size, size_t align, const char* file, uint32_t line, void* user_data);
static void* sx_malloc_leakd_cb(void* ptr, size_t size, size_t align, const char* file, uint32_t line, void* user_data);

static const sx_alloc g_alloc_malloc = {sx_malloc_cb, NULL};
static const sx_alloc g_alloc_malloc_leakd = {sx_malloc_leakd_cb, NULL};

const sx_alloc* sx_alloc_malloc = &g_alloc_malloc;
const sx_alloc* sx_alloc_malloc_leak_detect = &g_alloc_malloc_leakd;

static void* sx_malloc_cb(void* ptr, size_t size, size_t align, const char* file, uint32_t line, void* user_data)
{
    if (size == 0) {
        if (ptr) {
            if (align <= SX_CONFIG_ALLOCATOR_NATURAL_ALIGNMENT) {
                free(ptr);
                return NULL;
            }

#if SX_COMPILER_MSVC
            _aligned_free(ptr);
#else
            sx__aligned_free(&g_alloc_malloc, ptr, align, file, line);
#endif
        }
        return NULL;
    } else if (ptr == NULL) {
        if (align <= SX_CONFIG_ALLOCATOR_NATURAL_ALIGNMENT)
            return malloc(size);

#if SX_COMPILER_MSVC
        return _aligned_malloc(size, align);
#else
        return sx__aligned_alloc(&g_alloc_malloc, size, align, file, line);
#endif 
    } else {
		if (align <= SX_CONFIG_ALLOCATOR_NATURAL_ALIGNMENT)
			return realloc(ptr, size);

#if SX_COMPILER_MSVC
		return _aligned_realloc(ptr, size, align);
#else
		return sx__aligned_realloc(&g_alloc_malloc, ptr, size, align, file, line);
#endif
    }
}

// 
// Leakchecker: https://raw.githubusercontent.com/nothings/stb/master/stb_leakcheck.h
// TODO: make it thread-safe
// stb_leakcheck.h - v0.4 - quick & dirty malloc leak-checking - public domain
// LICENSE
//
//   See end of file.

// if we've already included leakcheck before, undefine the macros
#ifdef malloc
#   undef malloc
#   undef free
#   undef realloc
#endif

#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
typedef struct malloc_info stb_leakcheck_malloc_info;

struct malloc_info
{
    const char *file;
    int line;
    size_t size;
    stb_leakcheck_malloc_info *next, *prev;
};

static stb_leakcheck_malloc_info *mi_head;

static void *stb_leakcheck_malloc(size_t sz, const char *file, int line)
{
    stb_leakcheck_malloc_info *mi = (stb_leakcheck_malloc_info *)malloc(sz + sizeof(*mi));
    if (mi == NULL) return mi;
    mi->file = file;
    mi->line = line;
    mi->next = mi_head;
    if (mi_head)
        mi->next->prev = mi;
    mi->prev = NULL;
    mi->size = (int)sz;
    mi_head = mi;
    return mi+1;
}

static void stb_leakcheck_free(void *ptr)
{
    if (ptr != NULL) {
        stb_leakcheck_malloc_info *mi = (stb_leakcheck_malloc_info *)ptr - 1;
        mi->size = ~mi->size;
#ifndef STB_LEAKCHECK_SHOWALL
        if (mi->prev == NULL) {
            sx_assert(mi_head == mi);
            mi_head = mi->next;
        } else
            mi->prev->next = mi->next;
        if (mi->next)
            mi->next->prev = mi->prev;
#endif
        free(mi);
    }
}

static void *stb_leakcheck_realloc(void *ptr, size_t sz, const char *file, int line)
{
    if (ptr == NULL) {
        return stb_leakcheck_malloc(sz, file, line);
    } else if (sz == 0) {
        stb_leakcheck_free(ptr);
        return NULL;
    } else {
        stb_leakcheck_malloc_info *mi = (stb_leakcheck_malloc_info *)ptr - 1;
        if (sz <= mi->size)
            return ptr;
        else {
#ifdef STB_LEAKCHECK_REALLOC_PRESERVE_MALLOC_FILELINE
            void *q = stb_leakcheck_malloc(sz, mi->file, mi->line);
#else
            void *q = stb_leakcheck_malloc(sz, file, line);
#endif
            if (q) {
                sx_memcpy(q, ptr, mi->size);
                stb_leakcheck_free(ptr);
            }
            return q;
        }
    }
}

static void stblkck_internal_print(const char *reason, const char *file, int line, size_t size, void *ptr)
{
    char filename[64];
    const char* slash = strrchr(file, '/');
    if (!slash)
        slash = strrchr(file, '\\');
    if (slash)
        sx_strcpy(filename, sizeof(filename), slash+1);
    else
        sx_strcpy(filename, sizeof(filename), file);

    char text[512];
    sx_snprintf(text, sizeof(text), "%-6s: %s (%4d): %zd bytes at %p", reason, filename, line, size, ptr);
    puts(text);
}

void sx_dump_leaks()
{
    stb_leakcheck_malloc_info *mi = mi_head;
    while (mi) {
        if ((ptrdiff_t)mi->size >= 0)
            stblkck_internal_print("LEAKED", mi->file, mi->line, mi->size, mi+1);
        mi = mi->next;
    }
#ifdef STB_LEAKCHECK_SHOWALL
    mi = mi_head;
    while (mi) {
        if ((ptrdiff_t)mi->size < 0)
            stblkck_internal_print("FREED", mi->file, mi->line, ~mi->size, mi+1);
        printf("FREED : %s (%4d): %8d bytes at %p\n", mi->file, mi->line, (int)~mi->size, mi+1);
        mi = mi->next;
    }
#endif
}

/*
------------------------------------------------------------------------------
This software is available under 2 licenses -- choose whichever you prefer.
------------------------------------------------------------------------------
ALTERNATIVE A - MIT License
Copyright (c) 2017 Sean Barrett
Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
------------------------------------------------------------------------------
ALTERNATIVE B - Public Domain (www.unlicense.org)
This is free and unencumbered software released into the public domain.
Anyone is free to copy, modify, publish, use, compile, sell, or distribute this
software, either in source code form or as a compiled binary, for any purpose,
commercial or non-commercial, and by any means.
In jurisdictions that recognize copyright laws, the author or authors of this
software dedicate any and all copyright interest in the software to the public
domain. We make this dedication for the benefit of the public at large and to
the detriment of our heirs and successors. We intend this dedication to be an
overt act of relinquishment in perpetuity of all present and future rights to
this software under copyright law.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
------------------------------------------------------------------------------
*/

static void* sx_malloc_leakd_cb(void* ptr, size_t size, size_t align, const char* file, uint32_t line, void* user_data)
{
    if (size ==0) {
        if (ptr) {
            if (align <= SX_CONFIG_ALLOCATOR_NATURAL_ALIGNMENT) {
                stb_leakcheck_free(ptr);
                return NULL;
            }

            sx__aligned_free(&g_alloc_malloc_leakd, ptr, align, file, line);
        }
        return NULL;
    } else if (ptr == NULL) {
        if (align <= SX_CONFIG_ALLOCATOR_NATURAL_ALIGNMENT)
            return stb_leakcheck_malloc(size, file, (int)line);

        return sx__aligned_alloc(&g_alloc_malloc, size, align, file, line);
    } else {
        if (align <= SX_CONFIG_ALLOCATOR_NATURAL_ALIGNMENT)
            return stb_leakcheck_realloc(ptr, size, file, (int)line);

        return sx__aligned_realloc(&g_alloc_malloc_leakd, ptr, size, align, file, line);
    }
}


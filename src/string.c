//
// Copyright 2018 Sepehr Taghdisian (septag@github). All rights reserved.
// License: https://github.com/septag/sx#license-bsd-2-clause
//

#include "sx/string.h"
#include "sx/array.h"

SX_PRAGMA_DIAGNOSTIC_PUSH();
SX_PRAGMA_DIAGNOSTIC_IGNORED_CLANG_GCC("-Wunused-function")
#define STB_SPRINTF_IMPLEMENTATION
#define STB_SPRINTF_STATIC
#include "../3rdparty/stb/stb_printf.h"
SX_PRAGMA_DIAGNOSTIC_POP();

// Mattias' strpool implementation
#define STRPOOL_IMPLEMENTATION
#define STRPOOL_MEMSET( ptr, val, cnt ) ( sx_memset(ptr, val, cnt) )
#define STRPOOL_MEMCPY( dst, src, cnt ) ( sx_memcpy(dst, src, cnt) )
#define STRPOOL_MEMCMP( pr1, pr2, cnt ) ( sx_memcmp(pr1, pr2, cnt) )
#define STRPOOL_STRNICMP(s1, s2, len)   ( sx_strncmpnocase(s1, s2, len) )
#define STRPOOL_MALLOC( ctx, size )     ( sx_malloc((const sx_alloc*)ctx, size) )
#define STRPOOL_FREE( ctx, ptr )        ( sx_free((const sx_alloc*)ctx, ptr) )
#include "../3rdparty/mattias/strpool.h"

typedef struct sx__printf_ctx_s
{
    const sx_alloc* alloc;
    char* buff;
    int len;
    char tmp[STB_SPRINTF_MIN];
} sx__printf_ctx;

int sx_snprintf(char* str, int size, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    int r = sx_vsnprintf(str, size, fmt, args);
    va_end(args);
    return r;
}

int sx_vsnprintf(char* str, int size, const char* fmt, va_list args)
{
    return stbsp_vsnprintf(str, size, fmt, args);
}

char* sx_snprintf_alloc(const sx_alloc* alloc, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    char* str = sx_vsnprintf_alloc(alloc, fmt, args);
    va_end(args);
    return str;
}

static char* sx__vsnprintf_callback(char* buff, void* user, int len)
{
    sx__printf_ctx* ctx = (sx__printf_ctx*)user;
    int len_ = len + 1;     // Reserve one character for null-termination
    sx_array_add(ctx->alloc, ctx->buff, len_);
    sx_memcpy(ctx->buff + ctx->len, ctx->tmp, len);
    ctx->len += len;
    return ctx->tmp;
}

char* sx_vsnprintf_alloc(const sx_alloc* alloc, const char* fmt, va_list args)
{
    sx__printf_ctx ctx;
    ctx.alloc = alloc;
    ctx.buff = NULL;
    ctx.len = 0;
    stbsp_vsprintfcb(sx__vsnprintf_callback, &ctx, ctx.tmp, fmt, args);
    ctx.buff[ctx.len] = '\0';
    return ctx.buff;
}

int sx_strcpy(char* dst, int dst_sz, const char* src)
{
    sx_assert(dst);
    sx_assert(src);

    const int len = sx_strlen(src);
    const int32_t max = dst_sz-1;
    const int32_t num = (len < max ? len : max);
    sx_memcpy(dst, src, num);
    dst[num] = '\0';

    return num;
}

int sx_strlen(const char* str)
{
    sx_assert(str);

    const char* ptr = str;
    for (; *ptr != '\0'; ++ptr) {};
    return (int)(intptr_t)(ptr - str);
}

SX_INLINE int sx__strnlen(const char* str, int _max)
{
    sx_assert(str);

    const char* ptr = str;
    for (; _max > 0 && *ptr != '\0'; ++ptr, --_max) {};
    return (int)(intptr_t)(ptr - str);
}

int sx_strncpy(char* dst, int dst_sz, const char* src, int _num)
{
    sx_assert(dst);
    sx_assert(src);

    const int len = sx__strnlen(src, _num);
    const int32_t max = dst_sz-1;
    const int32_t num = (len < max ? len : max);
    sx_memcpy(dst, src, num);
    dst[num] = '\0';

    return num;
}

sx_strpool* sx_strpool_create(const sx_alloc* alloc, const sx_strpool_config* conf)
{
    strpool_config_t sconf;
    sconf.memctx = (void*)alloc;
    if (!conf) {
        sconf.ignore_case = 0;
        sconf.counter_bits = 12;
        sconf.index_bits = 20;
        sconf.entry_capacity = 4096;
        sconf.block_capacity = 32;
        sconf.block_size = 256*1024;
        sconf.min_length = 23;
    } else {
        sconf.ignore_case = conf->ignore_case;
        sconf.counter_bits = conf->counter_bits;
        sconf.index_bits = conf->index_bits;
        sconf.entry_capacity = conf->entry_capacity;
        sconf.block_capacity = conf->block_capacity;
        sconf.block_size = conf->block_sz_kb*1024;
        sconf.min_length = conf->min_str_len;
    }

    strpool_t* sp = (strpool_t*)sx_malloc(alloc, sizeof(strpool_t));
    if (!sp) {
        SX_OUT_OF_MEMORY;
        return NULL;
    }
    strpool_init(sp, &sconf);
    return sp;
}

bool sx_strcmp(const char* a, const char* b)
{
    int alen = sx_strlen(a);
    int blen = sx_strlen(b);
    if (alen != blen)
        return false;

    int c = sx_min(alen, blen);
    for (int i = 0; i < c; i++) {
        if (a[i] != b[i])
            return false;
    }
    return true;
}

bool sx_strcmpnocase(const char* a, const char* b)
{
    int alen = sx_strlen(a);
    int blen = sx_strlen(b);
    if (alen != blen)
        return false;

    int c = sx_min(alen, blen);
    for (int i = 0; i < c; i++) {
        if (sx_tolowerchar(a[i]) != sx_tolowerchar(b[i]))
            return false;
    }
    return true;
}

bool sx_strncmp(const char* a, const char* b, int num)
{
    int alen = sx_min(num, sx_strlen(a));
    int blen = sx_min(num, sx_strlen(b));
    if (alen != blen)
        return false;

    int c = sx_min(alen, blen);
    for (int i = 0; i < c; i++) {
        if (a[i] != b[i])
            return false;
    }
    return true;
}

bool sx_strncmpnocase(const char* a, const char* b, int num)
{
    int alen = sx_min(num, sx_strlen(a));
    int blen = sx_min(num, sx_strlen(b));
    if (alen != blen)
        return false;

    int c = sx_min(alen, blen);
    for (int i = 0; i < c; i++) {
        if (sx_tolowerchar(a[i]) != sx_tolowerchar(b[i]))
            return false;
    }
    return true;
}

char sx_tolowerchar(char ch)
{
    return ch + (sx_isupperchar(ch) ? 0x20 : 0);
}

char sx_toupperchar(char ch)
{
	return ch - (sx_islowerchar(ch) ? 0x20 : 0);
}

bool sx_isrange(char ch, char from, char to)
{
    return (uint8_t)(ch - from) <= (uint8_t)(to - from);
}

bool sx_isupperchar(char ch)
{
    return sx_isrange(ch, 'A', 'Z');
}

bool sx_islowerchar(char ch)
{
    return sx_isrange(ch, 'a', 'z');
}

void sx_strpool_destroy(sx_strpool* sp, const sx_alloc* alloc)
{
    sx_assert(sp);
    strpool_term(sp);
    sx_free(alloc, sp);
}

void sx_strpool_defrag(sx_strpool* sp)
{
    strpool_defrag(sp);
}

sx_str_t sx_strpool_add(sx_strpool* sp, const char* str, int len)
{
    STRPOOL_U64 handle = strpool_inject(sp, str, len);
    sx_assert((handle & 0xffffffff) == handle && "uint32_t overflow, check index_bits and counter_bits in config!");
    return (uint32_t)handle;
}

void sx_strpool_del(sx_strpool* sp, sx_str_t handle)
{
    strpool_discard(sp, (STRPOOL_U64)handle);
}

int sx_strpool_incref(sx_strpool* sp, sx_str_t handle)
{
    return strpool_incref(sp, (STRPOOL_U64)handle);
}

int sx_strpool_decref(sx_strpool* sp, sx_str_t handle)
{
    return strpool_decref(sp, (STRPOOL_U64)handle);
}

bool sx_strpool_valid(const sx_strpool* sp, sx_str_t handle)
{
    return SX_BOOL(strpool_isvalid(sp, (STRPOOL_U64)handle));
}

int sx_strpool_ref(sx_strpool* sp, sx_str_t handle)
{
    return strpool_getref(sp, (STRPOOL_U64)handle);
}

const char* sx_strpool_cstr(const sx_strpool* sp, sx_str_t handle)
{
    return strpool_cstr(sp, (STRPOOL_U64)handle);
}

int sx_strpool_len(const sx_strpool* sp, sx_str_t handle)
{
    return strpool_length(sp, (STRPOOL_U64)handle);
}

sx_strpool_collate_data sx_strpool_collate(const sx_strpool* sp)
{
    sx_strpool_collate_data d;
    d.first = strpool_collate(sp, &d.count);
    return d;
}

void sx_strpool_collate_free(const sx_strpool* sp, sx_strpool_collate_data data)
{
    sx_assert(data.first);
    strpool_free_collated(sp, data.first);
}

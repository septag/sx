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
//#define STRPOOL_STRNICMP(s1, s2, len)   ( sx_strncmpnocase(s1, s2, len) )
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

int sx_strcat(char* dst, int dst_sz, const char* src)
{
    sx_assert(dst);
    sx_assert(src);
    sx_assert(dst_sz > 0);

    int len = sx_strlen(dst);
    return sx_strcpy(dst + len, dst_sz - len, src);
}

int sx_strncat(char* dst, int dst_sz, const char* src, int _num)
{
    sx_assert(dst);
    sx_assert(src);
    sx_assert(dst_sz > 0);

    int len = sx_strlen(dst);
    return sx_strncpy(dst + len, dst_sz - len, src, _num);
}

bool sx_isspace(char ch)
{
    return  ch == ' ' ||
            ch == '\t' ||
            ch == '\n' ||
            ch == '\r'/* ||
            ch == '\v' ||
            ch == '\f'*/;
}

const char* sx_strrchar(const char* str, char ch) 
{
    sx_assert(str);

    const char* r = NULL;
    while (*str) {
        if (*str != ch) {
            ++str;
            continue;
        }
        r = str;
        ++str;
    }
    return r;
}

const char* sx_strchar(const char* str, char ch)
{
    sx_assert(str);

    while (*str) {
        if (*str != ch) {
            ++str;
            continue;
        }
        return str;            
    }
    return NULL;
}

const char* sx_strstr(const char* str, const char* find)
{
    sx_assert(str);
    sx_assert(find);

    char ch = find[0];
    while (*str) {
        if (*str != ch) {
            ++str;
            continue;
        }            

        // We have the first character, check the rest
        if (sx_strequal(str, find))
            return str;
    }

    return NULL;
}

// https://github.com/clibs/wildcardcmp
bool sx_strstr_wildcard(const char* str, const char* pattern)
{
    sx_assert(str);
    sx_assert(pattern);

    const char *w = NULL;   // last `*`
    const char *s = NULL;   // last checked char

    // loop 1 char at a time
    while (1) {
        if (!*str) {
            if (!*pattern) 
                return true;
            if (!*s) 
                return false;
            str = s++;
            pattern = w;
            continue;
        } else {
            if (*pattern != *str) {
                if ('*' == *pattern) {
                    w = ++pattern;
                    s = str;
                    // "*" -> "foobar"
                    if (*pattern) 
                        continue;
                    return true;
                } else if (w) {
                    str++;
                    // "*ooba*" -> "foobar"
                    continue;
                }
                return false;
            }
        }

        str++;
        pattern++;
    }

    return true;    
}

bool sx_strequal(const char* a, const char* b)
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

bool sx_strequalnocase(const char* a, const char* b)
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

bool sx_strnequal(const char* a, const char* b, int num)
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

bool sx_strnequalnocase(const char* a, const char* b, int num)
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

const char* sx_skip_whitespace(const char* str)
{
    while (*str) {
        if (sx_isspace(*str))
            ++str;
        break;
    }
    return str;
}

const char* sx_skip_word(const char* str)
{
    for (char ch = *str++; ch > 0 && (sx_islowerchar(ch) || sx_isupperchar(ch) || sx_isnumchar(ch) || ch == '_'); 
         ch = *str++) {}
	return str-1;
}

char* sx_trim_whitespace(char* dest, int dest_sz, const char* src)
{
    int len = sx_min(sx_strlen(src), dest_sz-1);
    int offset = 0;
    for (int i = 0; i < len; i++) {
        if (!sx_isspace(src[i]))
            dest[offset++] = src[i];
    }
    dest[offset] = '\0';
    return dest;
}

char* sx_trim(char* dest, int dest_sz, const char* src, const char* trim)
{
    int len = sx_min(sx_strlen(src), dest_sz-1);
    int offset = 0;
    for (int i = 0; i < len; i++) {
        const char* t = trim;
        char sch = src[i];
        bool trim_it = false;
        while (*t) {
            if (sch != *t) {
                ++t;
                continue;
            }
            trim_it = true;
            break;
        }

        if (!trim_it)
            dest[offset++] = src[i];
    }
    dest[offset] = '\0';
    return dest;
}

char* sx_trimchar(char* dest, int dest_sz, const char* src, char trim_ch)
{
    int len = sx_min(sx_strlen(src), dest_sz-1);
    int offset = 0;
    for (int i = 0; i < len; i++) {
        if (trim_ch != src[i])
            dest[offset++] = src[i];
    }
    dest[offset] = '\0';
    return dest;
}

char* sx_replace(char* dest, int dest_sz, const char* src, const char* find, const char* replace)
{
    char f = find[0];
    int flen = sx_strlen(find);
    int rlen = sx_strlen(replace);
    int offset = 0;
    int dest_max = dest_sz - 1;

    while (*src && offset < dest_max) {
        // Found first character, check for rest
        if (f == *src && sx_strequal(src, find)) {
            src += flen;
            int l = sx_min(dest_max - offset, rlen);
            sx_strncpy(dest + offset, dest_max - offset, replace, l);
            offset += l;
        } else {
            dest[offset++] = *src;
            ++src;
        }
    }

    return dest;
}

char* sx_EOL_LF(char* dest, int dest_sz, const char* src)
{
    assert(dest_sz > 0);
    char* end = dest + dest_sz - 1;
    for (char ch = *src++; ch != '\0' && dest < end; ch = *src++) {
        if ('\r' != ch)
            *dest++ = ch;
    }
    *dest = '\0';
    return dest;
}

bool sx_split(char* dest1, int dest1_sz, char* dest2, int dest2_sz, const char* src, char splitch)
{
    const char* sptr = sx_strchar(src, splitch);
    if (sptr) {
        sx_strncpy(dest1, dest1_sz, src, (int)(uintptr_t)(sptr - src));
        sx_strcpy(dest2, dest2_sz, src + 1);
        return true;
    }

    return false;
}

sx_str_block sx_findblock(const char* str, char open, char close)
{
    int count = 0;
    sx_str_block b = {NULL, NULL};

    for (char ch = *str++; ch != '\0' && count >= 0; ch = *str++) {
        if (ch == open) {
            b.start = str-1;
            count++;
        } else if (ch == close) {
            count--;
            if (count == 0) {
                b.end = str-1;
                return b;
            }
        }
    }

    return b;
}

bool sx_isnumchar(char ch)
{
    return sx_isrange(ch, '0', '9');
}

bool sx_isnum(const char* str)
{
    while (*str) {
        if (!sx_isnumchar(*str))
            return false;
        ++str;
    }
    return true;
}

bool sx_ishexchar(char ch)
{
    return sx_isrange(sx_tolowerchar(ch), 'a', 'f') || sx_isnumchar(ch);
}

bool sx_ishex(const char* str)
{   
    while (*str) {
        if (!sx_ishexchar(*str))
            return false;
        ++str;
    }
    return true;    
}

char* sx_tolower(char* dst, int dst_sz, const char* str)
{
    int offset = 0;
    int dst_max = dst_sz - 1;
    while (*str && offset < dst_max) {
        dst[offset++] = sx_tolowerchar(*str);
        ++str;
    }
    dst[offset] = '\0';
    return dst;
}

char* sx_toupper(char* dst, int dst_sz, const char* str)
{
    int offset = 0;
    int dst_max = dst_sz - 1;
    while (*str && offset < dst_max) {
        dst[offset++] = sx_tolowerchar(*str);
        ++str;
    }
    dst[offset] = '\0';
    return dst;
}

bool sx_tobool(const char* str)
{
    char ch = sx_tolowerchar(str[0]);
	return ch == 't' ||  ch == '1';
}

int sx_toint(const char* str)
{
    return atoi(str);
}

uint32_t sx_touint(const char* str)
{
    return strtoul(str, NULL, 10);
}

float sx_tofloat(const char* str)
{
    return strtof(str, NULL);
}

double sx_todouble(const char* str)
{
    return strtod(str, NULL);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
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
        sx_out_of_memory();
        return NULL;
    }
    strpool_init(sp, &sconf);
    return sp;
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

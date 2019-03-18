//
// Copyright 2018 Sepehr Taghdisian (septag@github). All rights reserved.
// License: https://github.com/septag/sx#license-bsd-2-clause
//
// string.h - v1.0.1 - C-String replacement/utility functions
//
#pragma once

#include "sx.h"
#include <stdarg.h>    // va_list

typedef struct sx_alloc sx_alloc;

typedef struct sx_str_block {
    const char* start;
    const char* end;
} sx_str_block;

SX_API int sx_snprintf(char* str, int size, const char* fmt, ...);
SX_API int sx_vsnprintf(char* str, int size, const char* fmt, va_list args);

SX_API char* sx_snprintf_alloc(const sx_alloc* alloc, const char* fmt, ...);
SX_API char* sx_vsnprintf_alloc(const sx_alloc* alloc, const char* fmt, va_list args);

SX_API char* sx_strcpy(char* SX_RESTRICT dst, int dst_sz, const char* SX_RESTRICT src);
SX_API char* sx_strncpy(char* SX_RESTRICT dst, int dst_sz, const char* SX_RESTRICT src, int _num);
SX_API char* sx_strcat(char* SX_RESTRICT dst, int dst_sz, const char* SX_RESTRICT src);
SX_API char* sx_strncat(char* SX_RESTRICT dst, int dst_sz, const char* SX_RESTRICT src, int _num);
SX_API int   sx_strlen(const char* str);
SX_API bool  sx_strequal(const char* SX_RESTRICT a, const char* SX_RESTRICT b);
SX_API bool  sx_strequalnocase(const char* SX_RESTRICT a, const char* SX_RESTRICT b);
SX_API bool  sx_strnequal(const char* SX_RESTRICT a, const char* SX_RESTRICT b, int num);
SX_API bool  sx_strnequalnocase(const char* SX_RESTRICT a, const char* SX_RESTRICT b, int num);

SX_API const char* sx_strrchar(const char* str, char ch);
SX_API const char* sx_strchar(const char* str, char ch);
SX_API const char* sx_strstr(const char* SX_RESTRICT str, const char* SX_RESTRICT find);
SX_API bool        sx_strstr_wildcard(const char* str, const char* pattern);

SX_API const char* sx_skip_whitespace(const char* str);
SX_API const char* sx_skip_word(const char* str);
SX_API char*       sx_trim_whitespace(char* dest, int dest_sz, const char* src);
SX_API char*       sx_trim(char* dest, int dest_sz, const char* src, const char* trim);
SX_API char*       sx_trimchar(char* dest, int dest_sz, const char* src, char trim_ch);
SX_API char*       sx_replace(char* dest, int dest_sz, const char* src, const char* find,
                              const char* replace);
SX_API char*       sx_replacechar(char* dest, int dest_sz, const char* src, const char find,
                                  const char replace);
SX_API char*       sx_EOL_LF(char* dest, int dest_sz, const char* src);
SX_API bool        sx_split(char* dest1, int dest1_sz, char* dest2, int dest2_sz, const char* src,
                            char splitch);
SX_API sx_str_block sx_findblock(const char* str, char open, char close);

SX_API bool  sx_isspace(char ch);
SX_API bool  sx_isrange(char ch, char from, char to);
SX_API bool  sx_isupperchar(char ch);
SX_API bool  sx_isupper(const char* str);
SX_API bool  sx_islowerchar(char ch);
SX_API bool  sx_islower(const char* str);
SX_API bool  sx_isnumchar(char ch);
SX_API bool  sx_isnum(const char* str);
SX_API bool  sx_ishexchar(char ch);
SX_API bool  sx_ishex(const char* str);
SX_API char* sx_tolower(char* dst, int dst_sz, const char* str);
SX_API char* sx_toupper(char* dst, int dst_sz, const char* str);
SX_API char  sx_tolowerchar(char ch);
SX_API char  sx_toupperchar(char ch);
SX_API bool  sx_tobool(const char* str);
SX_API int   sx_toint(const char* str);
SX_API uint32_t sx_touint(const char* str);
SX_API float    sx_tofloat(const char* str);
SX_API double   sx_todouble(const char* str);

// strpool (string interning) implementation
// By Mattias Gustavsson: https://github.com/mattiasgustavsson/libs/blob/master/strpool.h
//
typedef struct sx_strpool_config {
    int ignore_case;       // =1 case insensitive
    int counter_bits;      // counter bits in 32bit value for ref count (def: 12bits -> 4096)
    int index_bits;        // index bits in 32bit value for array index (def: 20bits -> 1 million)
    int entry_capacity;    // preallocate number of string entities (def: 4096)
    int block_capacity;    // number of storage blocks (def: 32), only one is allocated on init
    int block_sz_kb;       // size of each block in kb (def: 256)
    int min_str_len;       // minimum allocated size for each string, in chars (def: 23)
} sx_strpool_config;

typedef struct sx_strpool_collate_data {
    char* first;    // null-terminated strings, after each '\0' comes another str
    int   count;
} sx_strpool_collate_data;

typedef struct strpool_t sx_strpool;
typedef uint32_t         sx_str_t;

SX_API sx_strpool* sx_strpool_create(const sx_alloc*          alloc,
                                     const sx_strpool_config* conf sx_default(NULL));
SX_API void        sx_strpool_destroy(sx_strpool* sp, const sx_alloc* alloc);

SX_API void sx_strpool_defrag(sx_strpool* sp);
SX_API sx_str_t    sx_strpool_add(sx_strpool* sp, const char* str, int len);
SX_API void        sx_strpool_del(sx_strpool* sp, sx_str_t handle);
SX_API int         sx_strpool_incref(sx_strpool* sp, sx_str_t handle);
SX_API int         sx_strpool_decref(sx_strpool* sp, sx_str_t handle);
SX_API bool        sx_strpool_valid(const sx_strpool* sp, sx_str_t handle);
SX_API int         sx_strpool_ref(sx_strpool* sp, sx_str_t handle);
SX_API const char* sx_strpool_cstr(const sx_strpool* sp, sx_str_t handle);
SX_API int         sx_strpool_len(const sx_strpool* sp, sx_str_t handle);

SX_API sx_strpool_collate_data sx_strpool_collate(const sx_strpool* sp);
SX_API void sx_strpool_collate_free(const sx_strpool* sp, sx_strpool_collate_data data);

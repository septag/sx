//
// Copyright 2018 Sepehr Taghdisian (septag@github). All rights reserved.
// License: https://github.com/septag/sx#license-bsd-2-clause
//
// string.h - v1.0.1 - C-String replacement/utility functions
//
#pragma once

#ifndef SX_STRING_H_
#define SX_STRING_H_

#include "allocator.h"

typedef struct sx_str_block
{
    const char* start;
    const char* end;
} sx_str_block;

#ifdef __cplusplus
extern "C" {
#endif

int sx_snprintf(char* str, int size, const char* fmt, ...);
int sx_vsnprintf(char* str, int size, const char* fmt, va_list args);

SX_NEW_PTR char* sx_snprintf_alloc(const sx_alloc* alloc, const char* fmt, ...);
SX_NEW_PTR char* sx_vsnprintf_alloc(const sx_alloc* alloc, const char* fmt, va_list args);

int  sx_strcpy(char* dst, int dst_sz, const char* src);
int  sx_strncpy(char* dst, int dst_sz, const char* src, int _num);
int  sx_strcat(char* dst, int dst_sz, const char* src);
int  sx_strncat(char* dst, int dst_sz, const char* src, int _num);
int  sx_strlen(const char* str);
bool sx_strequal(const char* a, const char* b);
bool sx_strequalnocase(const char* a, const char* b);
bool sx_strnequal(const char* a, const char* b, int num);
bool sx_strnequalnocase(const char* a, const char* b, int num);

const char* sx_strrchar(const char* str, char ch);
const char* sx_strchar(const char* str, char ch);
const char* sx_strstr(const char* str, const char* find);
bool        sx_strstr_wildcard(const char* str, const char* pattern);

const char* sx_skip_whitespace(const char* str);
const char* sx_skip_word(const char* str);
char* sx_trim_whitespace(char* dest, int dest_sz, const char* src);
char* sx_trim(char* dest, int dest_sz, const char* src, const char* trim);
char* sx_trimchar(char* dest, int dest_sz, const char* src, char trim_ch);
char* sx_replace(char* dest, int dest_sz, const char* src, const char* find, const char* replace);
char* sx_EOL_LF(char* dest, int dest_sz, const char* src);
bool  sx_split(char* dest1, int dest1_sz, char* dest2, int dest2_sz, const char* src, char splitch);
sx_str_block sx_findblock(const char* str, char open, char close);

bool     sx_isspace(char ch);
bool     sx_isrange(char ch, char from, char to);
bool     sx_isupperchar(char ch);
bool     sx_isupper(const char* str);
bool     sx_islowerchar(char ch);
bool     sx_islower(const char* str);
bool     sx_isnumchar(char ch);
bool     sx_isnum(const char* str);
bool     sx_ishexchar(char ch);
bool     sx_ishex(const char* str);
char*    sx_tolower(char* dst, int dst_sz, const char* str);
char*    sx_toupper(char* dst, int dst_sz, const char* str);
char     sx_tolowerchar(char ch);
char     sx_toupperchar(char ch);
bool     sx_tobool(const char* str);
int      sx_toint(const char* str);
uint32_t sx_touint(const char* str);
float    sx_tofloat(const char* str);
double   sx_todouble(const char* str);

// strpool (string interning) implementation
// By Mattias Gustavsson: https://github.com/mattiasgustavsson/libs/blob/master/strpool.h
//
typedef struct sx_strpool_config
{
    int ignore_case;        // =1 case insensitive
    int counter_bits;       // counter bits in 32bit value for ref count (def: 12bits -> 4096)
    int index_bits;         // index bits in 32bit value for array index (def: 20bits -> 1 million)
    int entry_capacity;     // preallocate number of string entities (def: 4096) 
    int block_capacity;     // number of storage blocks (def: 32), only one is allocated on init
    int block_sz_kb;        // size of each block in kb (def: 256)
    int min_str_len;        // minimum allocated size for each string, in chars (def: 23)
} sx_strpool_config;

typedef struct sx_strpool_collate_data
{
    char* first;     // null-terminated strings, after each '\0' comes another str
    int   count;
} sx_strpool_collate_data;

typedef struct strpool_t    sx_strpool;
typedef uint32_t            sx_str_t;

sx_strpool* sx_strpool_create(const sx_alloc* alloc, const sx_strpool_config* conf SX_DFLT(NULL));
void        sx_strpool_destroy(sx_strpool* sp, const sx_alloc* alloc);

void        sx_strpool_defrag(sx_strpool* sp);
sx_str_t    sx_strpool_add(sx_strpool* sp, const char* str, int len);
void        sx_strpool_del(sx_strpool* sp, sx_str_t handle);
int         sx_strpool_incref(sx_strpool* sp, sx_str_t handle);
int         sx_strpool_decref(sx_strpool* sp, sx_str_t handle);
bool        sx_strpool_valid(const sx_strpool* sp, sx_str_t handle);
int         sx_strpool_ref(sx_strpool* sp, sx_str_t handle);
const char* sx_strpool_cstr(const sx_strpool* sp, sx_str_t handle);
int         sx_strpool_len(const sx_strpool* sp, sx_str_t handle);

sx_strpool_collate_data sx_strpool_collate(const sx_strpool* sp);
void                    sx_strpool_collate_free(const sx_strpool* sp, sx_strpool_collate_data data);

#ifdef __cplusplus
}
#endif

#endif

// Version history
//      1.0.0   Initial release
//      1.0.1   Bug fixed in sx_strrchar
//
//
// Copyright 2018 Sepehr Taghdisian (septag@github). All rights reserved.
// License: https://github.com/septag/sx#license-bsd-2-clause
//
// string.h - v1.0 - C-String replacement/utility functions
//
#pragma once

#ifndef SX_STRING_H_
#define SX_STRING_H_

#include "allocator.h"

SX_EXTERN int sx_snprintf(char* str, int size, const char* fmt, ...);
SX_EXTERN int sx_vsnprintf(char* str, int size, const char* fmt, va_list args);

SX_EXTERN SX_NEW_PTR char* sx_snprintf_alloc(const sx_alloc* alloc, const char* fmt, ...);
SX_EXTERN SX_NEW_PTR char* sx_vsnprintf_alloc(const sx_alloc* alloc, const char* fmt, va_list args);

SX_EXTERN int sx_strcpy(char* dst, int dst_sz, const char* src);
SX_EXTERN int sx_strncpy(char* dst, int dst_sz, const char* src, int _num);
SX_EXTERN int sx_strlen(const char* str);

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

typedef struct strpool_t sx_strpool;
typedef uint32_t sx_str_t;

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


#endif
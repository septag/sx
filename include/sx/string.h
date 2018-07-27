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

#endif
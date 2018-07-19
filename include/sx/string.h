#pragma once

#ifndef SX_STRING_H_
#define SX_STRING_H_

#include "allocator.h"

#ifdef __cplusplus
extern "C" {
#endif

int sx_snprintf(char* str, int size, const char* fmt, ...);
int sx_vsnprintf(char* str, int size, const char* fmt, va_list args);

SX_NEW_PTR char* sx_snprintf_alloc(const sx_alloc* alloc, const char* fmt, ...);
SX_NEW_PTR char* sx_vsnprintf_alloc(const sx_alloc* alloc, const char* fmt, va_list args);

#define sx_strcpy strcpy_s
#define sx_strcat strcat_s
#define sx_strlen (int)strlen

#ifdef __cplusplus
}
#endif

#endif
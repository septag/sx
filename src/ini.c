//
// Copyright 2018 Sepehr Taghdisian (septag@github). All rights reserved.
// License: https://github.com/septag/sx#license-bsd-2-clause
//
#include "sx/allocator.h"
#include "sx/string.h"

#define INI_IMPLEMENTATION
#define INI_MALLOC(ctx, size)       ( sx_malloc((const sx_alloc*)ctx, size) )
#define INI_FREE(ctx, ptr)          ( sx_free((const sx_alloc*)ctx, ptr) )
#define INI_MEMCPY( dst, src, cnt ) ( sx_memcpy( dst, src, cnt ) )
#define INI_STRLEN( s )             ( sx_strlen( s ) )
// TODO: #define INI_STRNICMP( s1, s2, cnt ) ( my_strnicmp_func( s1, s2, cnt ) )
#include "../3rdparty/mattias/ini.h"

#define _SX_INI_PRIVATE
#include "sx/ini.h"

sx_ini* sx_ini_create(const sx_alloc* alloc)
{
    return (sx_ini*)ini_create((void*)alloc);
}

sx_ini* sx_ini_load(const char* data, const sx_alloc* alloc)
{
    return (sx_ini*)ini_load(data, (void*)alloc);
}
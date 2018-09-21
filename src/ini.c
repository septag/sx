//
// Copyright 2018 Sepehr Taghdisian (septag@github). All rights reserved.
// License: https://github.com/septag/sx#license-bsd-2-clause
//
#include "sx/allocator.h"
#include "sx/string.h"
#include "sx/ini.h"

#define INI_IMPLEMENTATION
#define INI_MALLOC(ctx, size)       ( sx_malloc((const sx_alloc*)ctx, size) )
#define INI_FREE(ctx, ptr)          ( sx_free((const sx_alloc*)ctx, ptr) )
#define INI_MEMCPY( dst, src, cnt ) ( sx_memcpy( dst, src, cnt ) )
#define INI_STRLEN( s )             ( sx_strlen( s ) )
// TODO: #define INI_STRNICMP( s1, s2, cnt ) ( my_strnicmp_func( s1, s2, cnt ) )
#include "../3rdparty/mattias/ini.h"

sx_ini* sx_ini_create(const sx_alloc* alloc)
{
    return (sx_ini*)ini_create((void*)alloc);
}

sx_ini* sx_ini_load(const char* data, const sx_alloc* alloc)
{
    return (sx_ini*)ini_load(data, (void*)alloc);
}

int sx_ini_save(sx_ini const* ini, char* data, int size)
{
    return ini_save(ini, data, size);
}

void sx_ini_destroy(sx_ini* ini)
{
    ini_destroy(ini);
}

int sx_ini_section_count(sx_ini const* ini)
{
    return ini_section_count(ini);
}

char const* sx_ini_section_name(sx_ini const* ini, int section)
{
    return ini_section_name(ini, section);
}

int sx_ini_property_count(sx_ini const* ini, int section)
{
    return ini_property_count(ini, section);
}

char const* sx_ini_property_name(sx_ini const* ini, int section, int property)
{
    return ini_property_name(ini, section, property);
}

char const* sx_ini_property_value(sx_ini const* ini, int section, int property)
{
    return ini_property_value(ini, section, property);
}

int sx_ini_find_section(sx_ini const* ini, char const* name, int name_length)
{
    return ini_find_section(ini, name, name_length);
}

int sx_ini_find_property(sx_ini const* ini, int section, char const* name, int name_length)
{
    return ini_find_property(ini, section, name, name_length);
}

int  sx_ini_section_add(sx_ini* ini, char const* name, int length)
{
    return ini_section_add(ini, name, length);
}

void sx_ini_property_add(sx_ini* ini, int section, char const* name, int name_length, char const* value, int value_length)
{
    ini_property_add(ini, section, name, name_length, value, value_length);
}

void sx_ini_section_remove(sx_ini* ini, int section)
{
    ini_section_remove(ini, section);
}

void sx_ini_property_remove(sx_ini* ini, int section, int property)
{
    ini_property_remove(ini, section, property);
}

void sx_ini_section_name_set(sx_ini* ini, int section, char const* name, int length)
{
    ini_section_name_set(ini, section, name, length);
}

void sx_ini_property_name_set(sx_ini* ini, int section, int property, char const* name, int length)
{
    ini_property_name_set(ini, section, property, name, length);
}

void sx_ini_property_value_set(sx_ini* ini, int section, int property, char const* value, int length)
{
    ini_property_value_set(ini, section, property, value, length);
}

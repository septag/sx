//
// Copyright 2018 Sepehr Taghdisian (septag@github). All rights reserved.
// License: https://github.com/septag/sx#license-bsd-2-clause
//
//
// ini.h - v1.0 - Ini file parser/writer by Mattias Gustavsson
// Reference: https://github.com/mattiasgustavsson/libs/blob/master/ini.h
//
#pragma once

#include "macros.h"

typedef struct sx_alloc sx_alloc;
typedef struct ini_t    sx_ini;

SX_API sx_ini* sx_ini_create(const sx_alloc* alloc);
SX_API sx_ini* sx_ini_load(const char* data, const sx_alloc* alloc);
SX_API int     sx_ini_save(sx_ini const* ini, char* data, int size);
SX_API void    sx_ini_destroy(sx_ini* ini);

SX_API int         sx_ini_section_count(sx_ini const* ini);
SX_API char const* sx_ini_section_name(sx_ini const* ini, int section);

SX_API int         sx_ini_property_count(sx_ini const* ini, int section);
SX_API char const* sx_ini_property_name(sx_ini const* ini, int section, int property);
SX_API char const* sx_ini_property_value(sx_ini const* ini, int section, int property);

SX_API int sx_ini_find_section(sx_ini const* ini, char const* name, int name_length);
SX_API int sx_ini_find_property(sx_ini const* ini, int section, char const* name, int name_length);

SX_API int  sx_ini_section_add(sx_ini* ini, char const* name, int length);
SX_API void sx_ini_property_add(sx_ini* ini, int section, char const* name, int name_length,
                                char const* value, int value_length);
SX_API void sx_ini_section_remove(sx_ini* ini, int section);
SX_API void sx_ini_property_remove(sx_ini* ini, int section, int property);

SX_API void sx_ini_section_name_set(sx_ini* ini, int section, char const* name, int length);
SX_API void sx_ini_property_name_set(sx_ini* ini, int section, int property, char const* name,
                                     int length);
SX_API void sx_ini_property_value_set(sx_ini* ini, int section, int property, char const* value,
                                      int length);

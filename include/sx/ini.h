#pragma once

#ifndef SX_INI_H_
#define SX_INI_H_

#include "../../3rdparty/ini/ini.h"

#include "allocator.h"

typedef struct ini_t sx_ini;

SX_EXTERN sx_ini* sx_ini_create(const sx_alloc* alloc);
SX_EXTERN sx_ini* sx_ini_load(const char* data, const sx_alloc* alloc);

#define sx_ini_save                 ini_save
#define sx_ini_destroy              ini_destroy
#define sx_ini_section_count        ini_section_count
#define sx_ini_section_name         ini_section_name
#define sx_ini_property_count       ini_property_count
#define sx_ini_property_name        ini_property_name
#define sx_ini_property_value       ini_property_value
#define sx_ini_find_section         ini_find_section
#define sx_ini_find_property        ini_find_property
#define sx_ini_section_add          ini_section_add
#define sx_ini_property_add         ini_property_add
#define sx_ini_section_remove       ini_section_remove
#define sx_ini_property_remove      ini_property_remove
#define sx_ini_section_name_set     ini_section_name_set
#define sx_ini_property_name_set    ini_property_name_set
#define sx_ini_property_value_set   ini_property_value_set

#endif // SX_INI_H_


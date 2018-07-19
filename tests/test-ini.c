#include "sx/ini.h"
#include <stdio.h>

static const char* ini_data = 
    "global_var=hello world\n"
    "[general]\n"
    "screen_width=12\n"
    "screen_height=64\n"
    "data_path=/usr/septag\n"
    "; some comment about this section\n"
    "[sx]\n"
    "is_library=1\n";

int main(int argc, char* argv[])
{
    const sx_alloc* alloc = sx_alloc_malloc;
    sx_ini* ini = sx_ini_load(ini_data, alloc);
    if (!ini) {
        puts("Loading ini data failed");
        return -1;
    }

    for (int i = 0; i < sx_ini_section_count(ini); i++) {
        printf("Section: %s\n", sx_ini_section_name(ini, i));
        for (int k = 0; k < sx_ini_property_count(ini, i); k++) {
            printf("\t%s = %s\n", sx_ini_property_name(ini, i, k), sx_ini_property_value(ini, i, k));
        }
    }

    return 0;
}
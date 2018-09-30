//
// Copyright 2018 Sepehr Taghdisian (septag@github). All rights reserved.
// License: https://github.com/septag/sx#license-bsd-2-clause
//
// os.h - v1.1.0 - Common portable OS related functions
//
#pragma once

#ifndef SX_OS_H_
#define SX_OS_H_

#include "sx.h"

#if SX_PLATFORM_OSX
#   define SX_DL_EXT "dylib"
#elif SX_PLATFORM_WINDOWS
#   define SX_DL_EXT "dll"
#else
#   define SX_DL_EXT "so"
#endif

typedef enum sx_file_type
{
    SX_FILE_TYPE_INVALID = 0,
    SX_FILE_TYPE_REGULAR,
    SX_FILE_TYPE_DIRECTORY
} sx_file_type;

typedef struct sx_file_info
{
    sx_file_type type;
    uint64_t     size;
} sx_file_info;

#ifdef __cplusplus
extern "C" {
#endif

int    sx_os_minstacksz();
int    sx_os_maxstacksz();
int    sx_os_pagesz();
size_t sx_os_align_pagesz(size_t size);
char   sx_os_getch();
size_t sx_os_processmem();
void*  sx_os_dlopen(const char* filepath);
void   sx_os_dlclose(void* handle);
void*  sx_os_dlsym(void* handle, const char* symbol);
int    sx_os_chdir(const char* path);
void   sx_os_sleep(int ms);
void*  sx_os_exec(const char* const* argv);

char*  sx_os_path_pwd(char* dst, int size);
char*  sx_os_path_abspath(char* dst, int size, const char* path);
char*  sx_os_path_unixpath(char* dst, int size, const char* path);
char*  sx_os_path_winpath(char* dst, int size, const char* path);
char*  sx_os_path_basename(char* dst, int size, const char* path);
char*  sx_os_path_dirname(char* dst, int size, const char* path);
char*  sx_os_path_splitext(char* ext, int ext_size, char* basename, int basename_size, const char* path);
char*  sx_os_path_ext(char* dst, int size, const char* path);
char*  sx_os_path_join(char* dst, int size, const char* path_a, const char* path_b);
char*  sx_os_path_normcase(char* dst, int size, const char* path);
char*  sx_os_path_normpath(char* dst, int size, const char* path);
char*  sx_os_path_relpath(char* dst, int size, const char* path, const char* start SX_DFLT(NULL));
bool   sx_os_path_exists(const char* path);
bool   sx_os_path_isfile(const char* path);
bool   sx_os_path_isdir(const char* path);

sx_file_info sx_os_stat(const char* filepath);

#ifdef __cplusplus
}
#endif

#endif

// 
// Version history
//      1.0.0   Initial release
//      1.1.0   Added path functions, sx_os_path_xxxx
//
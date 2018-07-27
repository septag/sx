//
// Copyright 2018 Sepehr Taghdisian (septag@github). All rights reserved.
// License: https://github.com/septag/sx#license-bsd-2-clause
//
// os.h - v1.0 - Common portable OS related functions
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

#ifdef __cplusplus
}
#endif

#endif
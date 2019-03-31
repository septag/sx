//
// Copyright 2018 Sepehr Taghdisian (septag@github). All rights reserved.
// License: https://github.com/septag/sx#license-bsd-2-clause
//
// os.h - v1.1.0 - Common portable OS related functions
//
#pragma once

#include "sx.h"

#if SX_PLATFORM_OSX
#    define SX_DLL_EXT ".dylib"
#elif SX_PLATFORM_WINDOWS
#    define SX_DLL_EXT ".dll"
#else
#    define SX_DLL_EXT ".so"
#endif

typedef enum sx_file_type {
    SX_FILE_TYPE_INVALID = 0,
    SX_FILE_TYPE_REGULAR,
    SX_FILE_TYPE_DIRECTORY
} sx_file_type;

typedef struct sx_file_info {
    sx_file_type type;
    uint64_t     size;
    uint64_t     last_modified;    // time_t
} sx_file_info;

typedef struct sx_pinfo {
    union {
        uintptr_t linux_pid;
        void*     win_process_handle;
    };
    void* win_thread_handle;
} sx_pinfo;

SX_API int sx_os_minstacksz();
SX_API int sx_os_maxstacksz();
SX_API int sx_os_pagesz();
SX_API size_t sx_os_align_pagesz(size_t size);
SX_API char   sx_os_getch();
SX_API size_t      sx_os_processmem();
SX_API void*       sx_os_dlopen(const char* filepath);
SX_API void        sx_os_dlclose(void* handle);
SX_API void*       sx_os_dlsym(void* handle, const char* symbol);
SX_API const char* sx_os_dlerr();
SX_API int         sx_os_chdir(const char* path);
SX_API void        sx_os_sleep(int ms);
SX_API sx_pinfo sx_os_exec(const char* const* argv);
SX_API bool     sx_os_copy(const char* src, const char* dest);
SX_API bool     sx_os_rename(const char* src, const char* dest);
SX_API bool     sx_os_del(const char* path, sx_file_type type);
SX_API bool     sx_os_mkdir(const char* path);

SX_API char* sx_os_path_pwd(char* dst, int size);
SX_API char* sx_os_path_abspath(char* dst, int size, const char* path);
SX_API char* sx_os_path_unixpath(char* dst, int size, const char* path);
SX_API char* sx_os_path_winpath(char* dst, int size, const char* path);
SX_API char* sx_os_path_basename(char* dst, int size, const char* path);
SX_API char* sx_os_path_dirname(char* dst, int size, const char* path);
SX_API char* sx_os_path_splitext(char* ext, int ext_size, char* basename, int basename_size,
                                 const char* path);
SX_API char* sx_os_path_ext(char* dst, int size, const char* path);
SX_API char* sx_os_path_join(char* dst, int size, const char* path_a, const char* path_b);
SX_API char* sx_os_path_normcase(char* dst, int size, const char* path);
SX_API char* sx_os_path_normpath(char* dst, int size, const char* path);
SX_API char* sx_os_path_relpath(char* dst, int size, const char* path,
                                const char* start sx_default(NULL));
SX_API bool  sx_os_path_exists(const char* path);
SX_API bool  sx_os_path_isfile(const char* path);
SX_API bool  sx_os_path_isdir(const char* path);
SX_API char* sx_os_path_exepath(char* dst, int size);

SX_API sx_file_info sx_os_stat(const char* filepath);

SX_API int sx_os_numcores();

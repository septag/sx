//
// Copyright 2018 Sepehr Taghdisian (septag@github). All rights reserved.
// License: https://github.com/septag/sx#license-bsd-2-clause
//
#include "sx/os.h"
#include "sx/string.h"
#include <alloca.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#if SX_PLATFORM_WINDOWS
#   define VC_EXTRALEAN
#   define WIN32_LEAN_AND_MEAN
#	include <windows.h>
#	include <Psapi.h>
#	include <direct.h>   	// _getcwd
#elif SX_PLATFORM_POSIX
#   include <unistd.h>
#   include <sys/resource.h>
#   include <termios.h>
#   include <time.h>
#   include <pthread.h>
#	include <limits.h>
#   if !SX_PLATFORM_PS4
#       include <dlfcn.h>   // dlopen, dlclose, dlsym
#   endif
#   if SX_PLATFORM_ANDROID
#       include <malloc.h>  // mallinfo
#   elif SX_PLATFORM_LINUX || \
         SX_PLATFORM_RPI || \
         SX_PLATFORM_STEAMLINK
#       include <sys/syscall.h>
#   elif SX_PLATFORM_OSX
#       include <mach/mach.h>
#   elif SX_PLATFORM_HURD 
#       include <pthread/pthread.h>
#   endif 
#endif

#if SX_PLATFORM_WINDOWS
static const char* k_path_sep = "\\";
#else
static const char* k_path_sep = "/";
#endif

int sx_os_pagesz()
{
#if SX_PLATFORM_WINDOWS
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    return (int)si.dwPageSize;
#elif SX_PLATFORM_POSIX
    return sysconf(_SC_PAGESIZE);    
#endif
}

int sx_os_maxstacksz()
{
#if SX_PLATFORM_WINDOWS
    return 1073741824;  // 1gb
#elif SX_PLATFORM_POSIX
    struct rlimit limit;
    getrlimit(RLIMIT_STACK, &limit);
    return limit.rlim_max;
#endif
}

int sx_os_minstacksz()
{
	return 32768;   // 32kb
}

char sx_os_getch()
{
#if SX_PLATFORM_WINDOWS
    return getchar();
//#elif SX_PLATFORM_EMSCRIPTEN
//	return 0;
#elif SX_PLATFORM_POSIX
    struct termios old_term;
    struct termios new_term;
    tcgetattr(0, &old_term);
    new_term = old_term;
    new_term.c_lflag &= ICANON;
    new_term.c_lflag &= ~ECHO;
    tcsetattr(0, TCSANOW, &new_term);
    char ch = getchar();
    tcsetattr(0, TCSANOW, &old_term);
    return ch;
#endif
}

size_t sx_os_align_pagesz(size_t size)
{
    sx_assert(size > 0);
    int page_sz = sx_os_pagesz();
    size_t page_cnt = (size + page_sz - 1) / page_sz;
    return page_cnt * page_sz;
}

size_t sx_os_processmem()
{
#if SX_PLATFORM_ANDROID
		struct mallinfo mi = mallinfo();
		return mi.uordblks;
#elif SX_PLATFORM_LINUX || SX_PLATFORM_HURD
		FILE* file = fopen("/proc/self/statm", "r");
		if (NULL == file) {
			return 0;
		}

		long pages = 0;
		int items = fscanf(file, "%*s%ld", &pages);
		fclose(file);
		return 1 == items ? pages * sysconf(_SC_PAGESIZE) : 0;
#elif SX_PLATFORM_OSX
#	if defined(MACH_TASK_BASIC_INFO)
		struct mach_task_basic_info info;
		mach_msg_type_number_t infoCount = MACH_TASK_BASIC_INFO_COUNT;

		int const result = task_info(mach_task_self()
				, MACH_TASK_BASIC_INFO
				, (task_info_t)&info
				, &infoCount
				);
#	else
		task_basic_info info;
		mach_msg_type_number_t infoCount = TASK_BASIC_INFO_COUNT;

		int const result = task_info(mach_task_self()
				, TASK_BASIC_INFO
				, (task_info_t)&info
				, &infoCount);
#	endif // defined(MACH_TASK_BASIC_INFO)
		if (KERN_SUCCESS != result) {
			return 0;
		}

		return info.resident_size;
#elif SX_PLATFORM_WINDOWS
		PROCESS_MEMORY_COUNTERS pmc;
		GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc));
		return pmc.WorkingSetSize;
#else
		return 0;
#endif // SX_PLATFORM_*
}

void* sx_os_dlopen(const char* filepath)
{
#if SX_PLATFORM_WINDOWS
	return (void*)LoadLibraryA(filepath);
#elif  SX_PLATFORM_EMSCRIPTEN \
	|| SX_PLATFORM_PS4        \
	|| SX_PLATFORM_XBOXONE    \
	|| SX_PLATFORM_WINRT
	SX_UNUSED(filepath);
	return NULL;
#else
	return dlopen(filepath, RTLD_LOCAL|RTLD_LAZY);
#endif // SX_PLATFORM_
}

void sx_os_dlclose(void* handle)
{
#if SX_PLATFORM_WINDOWS
    FreeLibrary((HMODULE)handle);
#elif SX_PLATFORM_EMSCRIPTEN || \
      SX_PLATFORM_PS4 || \
      SX_PLATFORM_XBOXONE
    SX_UNUSED(handle);
#else
    dlclose(handle);
#endif
}

void* sx_os_dlsym(void* handle, const char* symbol)
{
#if SX_PLATFORM_WINDOWS
    return (void*)GetProcAddress((HMODULE)handle, symbol);
#elif SX_PLATFORM_EMSCRIPTEN || \
      SX_PLATFORM_PS4 || \
      SX_PLATFORM_XBOXONE
    SX_UNUSED(handle);
    SX_UNUSED(symbol);
    return NULL;
#else
    return dlsym(handle, symbol);
#endif
}

int sx_os_chdir(const char* path)
{
#if SX_PLATFORM_PS4     \
 || SX_PLATFORM_XBOXONE \
 || SX_PLATFORM_WINRT   \
 || SX_PLATFORM_ANDROID \
 || Sx_PLATFORM_IOS
	SX_UNUSED(_path);
	return -1;
#elif SX_PLATFORM_WINDOWS
	return SetCurrentDirectory(path);
#else
	return chdir(path);
#endif // SX_COMPILER_
}

void sx_os_sleep(int ms)
{
#if SX_PLATFORM_WINDOWS
    Sleep(ms);
#elif SX_PLATFORM_XBOXONE
    sx_assert(0 && "Sleep not implemented");
#else
    struct timespec req = { (time_t)ms/1000, (long)((ms%1000)*1000000) };
    struct timespec rem = { 0, 0 };
    nanosleep(&req, &rem);
#endif // SX_PLATFORM_
}

void* sx_os_exec(const char* const* argv)
{
#if SX_PLATFORM_LINUX || SX_PLATFORM_HURD
		pid_t pid = fork();

		if (0 == pid) {
			int result = execvp(argv[0], (char* const*)(&argv[1]) );
			SX_UNUSED(result);
			return NULL;
		}

		return (void*)(uintptr_t)pid;
#elif SX_PLATFORM_WINDOWS
		STARTUPINFOA si;
		sx_memset(&si, 0, sizeof(STARTUPINFOA));
		si.cb = sizeof(STARTUPINFOA);

		PROCESS_INFORMATION pi;
		sx_memset(&pi, 0, sizeof(PROCESS_INFORMATION) );

		int total = 0;
		for (int ii = 0; NULL != argv[ii]; ++ii) {
			total += sx_strlen(argv[ii]) + 1;
		}

		char* temp = (char*)alloca(total);
		int len = 0;
		for(int ii = 0; NULL != argv[ii]; ++ii) {
			len += sx_snprintf(&temp[len], sx_max(0, total-len), "%s ", argv[ii]);
		}

		bool ok = !!CreateProcessA(argv[0]
			, temp
			, NULL
			, NULL
			, false
			, 0
			, NULL
			, NULL
			, &si
			, &pi);
		if (ok)	{
			return pi.hProcess;
		}

		return NULL;
#else
		SX_UNUSED(argv);
		return NULL;
#endif // SX_PLATFORM_
}

sx_file_info sx_os_stat(const char* filepath)
{
    sx_assert(filepath);
    sx_file_info info = {SX_FILE_TYPE_INVALID, 0};

#if SX_COMPILER_MSVC
	struct _stat64 st;
	int32_t result = _stat64(filepath, &st);

	if (0 != result)
		return info;

	if (0 != (st.st_mode & _S_IFREG))
		info.type = SX_FILE_TYPE_REGULAR;
	else if (0 != (st.st_mode & _S_IFDIR))
		info.type = SX_FILE_TYPE_DIRECTORY;
#else
	struct stat st;
	int32_t result = stat(filepath, &st);
	if (0 != result)
		return info;

	if (0 != (st.st_mode & S_IFREG))
		info.type = SX_FILE_TYPE_REGULAR;
	else if (0 != (st.st_mode & S_IFDIR))
		info.type = SX_FILE_TYPE_DIRECTORY;
#endif // SX_COMPILER_MSVC

    info.size = st.st_size;
	return info;
}

char* sx_os_path_pwd(char* dst, int size)
{
#if SX_PLATFORM_PS4     \
 || SX_PLATFORM_XBOXONE \
 || SX_PLATFORM_WINRT   \
 || SX_CRT_NONE
		SX_UNUSED(dst);
		SX_UNUSUED(size);
		return NULL;
#elif SX_CRT_MSVC
		return _getcwd(dst, size);
#else
		return getcwd(dst, _size);
#endif // SX_COMPILER_	
}

char* sx_os_path_abspath(char* dst, int size, const char* path)
{
#if SX_PLATFORM_POSIX
	char abs_path[PATH_MAX];
	realpath(path, abs_path);
	sx_strcpy(dst, size, abs_path);
	return dst;
#elif SX_PLATFORM_WINDOWS
	if (GetFullPathNameA(path, (DWORD)size, dst, NULL) == 0)
		dst[0] = '\0';
	return dst;
#else
#	error "Not Implemented"
#endif
}

char* sx_os_path_unixpath(char* dst, int size, const char* path)
{
	int len = sx_strlen(path);
	len = sx_min(len, size-1);

	for (int i = 0; i < len; i++) {
		if (path[i] != '\\')
			dst[i] = path[i];
		else
			dst[i] = '/';
	}
	dst[len] = '\0';
	return dst;
}

char* sx_os_path_winpath(char* dst, int size, const char* path)
{
	int len = sx_strlen(path);
	len = sx_min(len, size-1);

	for (int i = 0; i < len; i++) {
		if (path[i] != '/')
			dst[i] = path[i];
		else
			dst[i] = '\\';
	}
	dst[len] = '\0';
	return dst;
}

char*  sx_os_path_basename(char* dst, int size, const char* path)
{
	const char* r = sx_strrchar(path, '/');
	if (!r)
		r = sx_strrchar(path, '\\');
	if (r) {
		sx_strcpy(dst, size, r + 1);
	} else if (dst != path) {
		sx_strcpy(dst, size, path);
	}
	return dst;
}

char*  sx_os_path_dirname(char* dst, int size, const char* path)
{
	const char* r = sx_strrchar(path, '/');
	if (!r) 
		r = sx_strrchar(path, '\\');
	if (r) {
		int o = (int)(intptr_t)(r - path);
		sx_strncpy(dst, size, path, o);
	} else {
		sx_strcpy(dst, size, path);
	}
	return dst;
}

char* sx_os_path_splitext(char* ext, int ext_size, char* basename, int basename_size, const char* path)
{
	sx_assert(ext != path);
	sx_assert(basename != path);

	int len = sx_strlen(path);
	const char* epos = NULL;
	if (len > 0) {
		const char* start = sx_strrchar(path, '/');
		if (!start)
			start = sx_strrchar(path, '\\');
		if (!start)
			start = path;
		const char* end = &path[len-1];
		for (const char* e = start; e < end; ++e) {
			if (*e != '.') 
				continue;
			sx_strcpy(ext, ext_size, e);
			sx_strncpy(basename, basename_size, path, (int)(intptr_t)(e - path - 1));
			return ext;
		}
	}

	// no extension (.) found
	ext[0] = '\0';	
	sx_strcpy(basename, basename_size, path);
	return ext;
}

char* sx_os_path_ext(char* dst, int size, const char* path)
{
	sx_assert(size > 0);

	int len = sx_strlen(path);
	if (len > 0) {
		const char* start = sx_strrchar(path, '/');
		if (!start)
			start = sx_strrchar(path, '\\');
		if (!start)
			start = path;
		const char* end = &path[len-1];
		for (const char* e = start; e < end; ++e) {
			if (*e != '.') 
				continue;
			sx_strcpy(dst, size, e);
			return dst;
		}
	}

	dst[0] = '\0';	// no extension
	return dst;
}

char*  sx_os_path_join(char* dst, int size, const char* path_a, const char* path_b)
{
	int len = sx_strlen(path_a);
	if (dst != path_a) {
		if (len > 0 && path_a[len-1] == k_path_sep[0]) {
			sx_strcpy(dst, size, path_a);
		} else {
			sx_strcpy(dst, size, path_a);
			sx_strcat(dst, size, k_path_sep);
		}
	}

	if (path_b[0] == k_path_sep[0]) 
		++path_b;
	sx_strcat(dst, size, path_b);		
	return dst;
}

char* sx_os_path_normcase(char* dst, int size, const char* path)
{
#if SX_PLATFORM_WINDOWS
	return sx_tolower(dst, size, path);
#else
	if (dst != path)
		sx_strcpy(dst, size, path);
	return dst;
#endif
}

char* sx_os_path_relpath(char* dst, int size, const char* path, const char* start)
{
	sx_assert(0 && "TODO");
	sx_assert(start != path);

	return NULL;
}

bool sx_os_path_exists(const char* path)
{
	return sx_os_stat(path).type != SX_FILE_TYPE_INVALID;
}

bool sx_os_path_isfile(const char* filepath)
{
	return (sx_os_stat(filepath).type == SX_FILE_TYPE_REGULAR);
}

bool sx_os_path_isdir(const char* filepath)
{
	return (sx_os_stat(filepath).type == SX_FILE_TYPE_DIRECTORY);
}

char* sx_os_path_normpath(char* dst, int size, const char* path)
{
#if SX_PLATFORM_WINDOWS
	return sx_tolower(dst, size, sx_os_path_winpath(dst, size, path));
#else
	return sx_os_path_unixpath(dst, size, path);
#endif
}

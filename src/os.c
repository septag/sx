//
// Copyright 2018 Sepehr Taghdisian (septag@github). All rights reserved.
// License: https://github.com/septag/sx#license-bsd-2-clause
//
#include "sx/os.h"
#include "sx/string.h"
#include <alloca.h>
#include <stdio.h>

#if SX_PLATFORM_WINDOWS
#   define VC_EXTRALEAN
#   define WIN32_LEAN_AND_MEAN
#	include <windows.h>
#	include <Psapi.h>
#elif SX_PLATFORM_POSIX
#   include <unistd.h>
#   include <sys/resource.h>
#   include <termios.h>
#   include <time.h>
#   include <pthread.h>
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
    assert(size > 0);
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
		mach_task_basic_info info;
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
    assert(0 && "Sleep not implemented");
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

		return (void*)(ptr_t)pid;
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
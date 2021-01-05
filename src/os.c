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
#    define VC_EXTRALEAN
#    define WIN32_LEAN_AND_MEAN
// clang-format off
SX_PRAGMA_DIAGNOSTIC_PUSH()
SX_PRAGMA_DIAGNOSTIC_IGNORED_MSVC(5105)
#    include <windows.h>
SX_PRAGMA_DIAGNOSTIC_POP()
#    include <direct.h>    // _getcwd
#    include <psapi.h>
// clang-format on
#elif SX_PLATFORM_POSIX
#    include <dirent.h>    // S_IFREG
#    include <fcntl.h>     // open
#    include <limits.h>
#    include <pthread.h>
#    include <sys/resource.h>
#    include <termios.h>
#    include <time.h>
#    include <unistd.h>
#    if !SX_PLATFORM_PS4
#        include <dlfcn.h>    // dlopen, dlclose, dlsym
#    endif
#    if SX_PLATFORM_ANDROID
#        include <cpu-features.h>    // android_getCpuCount
#        include <malloc.h>          // mallinfo
#        include <sys/sendfile.h>    // sendfile
#    elif SX_PLATFORM_LINUX || SX_PLATFORM_RPI || SX_PLATFORM_STEAMLINK
#        include <linux/limits.h>
#        include <sys/sendfile.h>    // sendfile
#        include <sys/syscall.h>
#        include <sys/types.h>
#    elif SX_PLATFORM_APPLE
//      https://developer.apple.com/library/archive/documentation/System/Conceptual/ManPages_iPhoneOS/man2/sendfile.2.html
#        include <copyfile.h>
#        include <mach/mach.h>
#        include <mach-o/dyld.h>    // _NSGetExecutablePath
#        include <sys/types.h>
#        include <sys/sysctl.h>
#    elif SX_PLATFORM_HURD
#        include <pthread/pthread.h>
#    elif SX_PLATFORM_BSD
#        include <sys/sysctl.h>
#        include <sys/types.h>
#    endif
#endif

#if SX_PLATFORM_WINDOWS
static const char* k_path_sep = "\\";
#else
static const char* k_path_sep = "/";
#endif

size_t sx_os_pagesz(void)
{
#if SX_PLATFORM_WINDOWS
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    return (size_t)si.dwPageSize;
#elif SX_PLATFORM_POSIX
    return (size_t)sysconf(_SC_PAGESIZE);
#endif
}

size_t sx_os_maxstacksz(void)
{
#if SX_PLATFORM_WINDOWS
    return 1073741824;    // 1gb
#elif SX_PLATFORM_POSIX
    struct rlimit limit;
    getrlimit(RLIMIT_STACK, &limit);
    return limit.rlim_max;
#endif
}

size_t sx_os_minstacksz(void)
{
    return 32768;    // 32kb
}

char sx_os_getch(void)
{
#if SX_PLATFORM_WINDOWS
    return (char)(getchar() & 0xff);
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
    size_t page_sz = sx_os_pagesz();
    size_t page_cnt = (size + page_sz - 1) / page_sz;
    return page_cnt * page_sz;
}

size_t sx_os_processmem(void)
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
#    if defined(MACH_TASK_BASIC_INFO)
    struct mach_task_basic_info info;
    mach_msg_type_number_t infoCount = MACH_TASK_BASIC_INFO_COUNT;

    int const result =
        task_info(mach_task_self(), MACH_TASK_BASIC_INFO, (task_info_t)&info, &infoCount);
#    else
    task_basic_info info;
    mach_msg_type_number_t infoCount = TASK_BASIC_INFO_COUNT;

    int const result = task_info(mach_task_self(), TASK_BASIC_INFO, (task_info_t)&info, &infoCount);
#    endif    // defined(MACH_TASK_BASIC_INFO)
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
#endif    // SX_PLATFORM_*
}

void* sx_os_dlopen(const char* filepath)
{
#if SX_PLATFORM_WINDOWS
    return (void*)LoadLibraryA(filepath);
#elif SX_PLATFORM_EMSCRIPTEN || SX_PLATFORM_PS4 || SX_PLATFORM_XBOXONE || SX_PLATFORM_WINRT
    sx_unused(filepath);
    return NULL;
#else
    return dlopen(filepath, RTLD_LOCAL | RTLD_LAZY);
#endif    // SX_PLATFORM_
}

void sx_os_dlclose(void* handle)
{
#if SX_PLATFORM_WINDOWS
    FreeLibrary((HMODULE)handle);
#elif SX_PLATFORM_EMSCRIPTEN || SX_PLATFORM_PS4 || SX_PLATFORM_XBOXONE
    sx_unused(handle);
#else
    dlclose(handle);
#endif
}

void* sx_os_dlsym(void* handle, const char* symbol)
{
#if SX_PLATFORM_WINDOWS
    return (void*)GetProcAddress((HMODULE)handle, symbol);
#elif SX_PLATFORM_EMSCRIPTEN || SX_PLATFORM_PS4 || SX_PLATFORM_XBOXONE
    sx_unused(handle);
    sx_unused(symbol);
    return NULL;
#else
    return dlsym(handle, symbol);
#endif
}

const char* sx_os_dlerr(void)
{
#if SX_PLATFORM_WINDOWS
    return "";
#elif SX_PLATFORM_EMSCRIPTEN || SX_PLATFORM_PS4 || SX_PLATFORM_XBOXONE
    return "";
#else
    return dlerror();
#endif
}

int sx_os_chdir(const char* path)
{
#if SX_PLATFORM_PS4 || SX_PLATFORM_XBOXONE || SX_PLATFORM_WINRT || SX_PLATFORM_ANDROID || SX_PLATFORM_IOS
    sx_unused(path);
    return -1;
#elif SX_PLATFORM_WINDOWS
    return SetCurrentDirectoryA(path);
#else
    return chdir(path);
#endif    // SX_COMPILER_
}

void sx_os_sleep(int ms)
{
#if SX_PLATFORM_WINDOWS
    Sleep(ms);
#else
    struct timespec req = { (time_t)ms / 1000, (long)((ms % 1000) * 1000000) };
    struct timespec rem = { 0, 0 };
    nanosleep(&req, &rem);
#endif    // SX_PLATFORM_
}

sx_pinfo sx_os_exec(const char* const* argv)
{
#if SX_PLATFORM_LINUX || SX_PLATFORM_HURD
    pid_t pid = fork();

    if (0 == pid) {
        int result = execvp(argv[0], (char* const*)(&argv[1]));
        sx_unused(result);
        return (sx_pinfo){ 0 };
    }

    return (sx_pinfo){ .linux_pid = (uintptr_t)pid };
#elif SX_PLATFORM_WINDOWS
    STARTUPINFOA si;
    sx_memset(&si, 0, sizeof(STARTUPINFOA));
    si.cb = sizeof(STARTUPINFOA);

    PROCESS_INFORMATION pi;
    sx_memset(&pi, 0, sizeof(PROCESS_INFORMATION));

    int total = 0;
    for (int ii = 0; NULL != argv[ii]; ++ii) {
        total += sx_strlen(argv[ii]) + 1;
    }

    sx_assert(total <= 32768);
    char* temp = (char*)alloca(total);
    sx_assert(temp);
    int len = 0;
    for (int ii = 0; NULL != argv[ii]; ++ii) {
        len += sx_snprintf(&temp[len], sx_max(0, total - len), "%s ", argv[ii]);
    }

    sx_pinfo pinfo;

    bool ok = !!CreateProcessA(argv[0], temp, NULL, NULL, false, 0, NULL, NULL, &si, &pi);
    if (ok) {
        pinfo.win_process_handle = pi.hProcess;
        pinfo.win_thread_handle = pi.hThread;
    } else {
        pinfo.win_process_handle = NULL;
        pinfo.win_thread_handle = NULL;
    }

    return pinfo;
#else
    sx_unused(argv);
    sx_assertf(0, "not implemented");
    return (sx_pinfo){ {0}, 0 };
#endif    // SX_PLATFORM_
}

bool sx_os_copy(const char* src, const char* dest)
{
#if SX_PLATFORM_WINDOWS
    return CopyFileA(src, dest, FALSE) ? true : false;
#elif SX_PLATFORM_APPLE
    return copyfile(src, dest, NULL, COPYFILE_ALL | COPYFILE_NOFOLLOW_DST) == 0;
#elif SX_PLATFORM_LINUX || SX_PLATFORM_ANDROID || SX_PLATFORM_RPI
    // Reference: http://www.informit.com/articles/article.aspx?p=23618&seqNum=13
    int input, output;
    struct stat src_stat;
    if ((input = open(src, O_RDONLY)) == -1) {
        return false;
    }
    fstat(input, &src_stat);

    if ((output = open(dest, O_WRONLY | O_CREAT, O_NOFOLLOW | src_stat.st_mode)) == -1) {
        close(input);
        return false;
    }

    int result = sendfile(output, input, NULL, src_stat.st_size);
    close(input);
    close(output);
    return result > -1;
#else
    sx_assert(0, "not implemented");
    return false;
#endif
}

bool sx_os_rename(const char* src, const char* dest)
{
#if SX_PLATFORM_WINDOWS
    return MoveFileExA(src, dest, MOVEFILE_WRITE_THROUGH | MOVEFILE_REPLACE_EXISTING) ? true
                                                                                      : false;
#else
    return rename(src, dest) == 0;
#endif
}

bool sx_os_del(const char* path, sx_file_type type)
{
    sx_assert(type != SX_FILE_TYPE_INVALID);
#if SX_PLATFORM_WINDOWS
    if (type == SX_FILE_TYPE_REGULAR)
        return DeleteFileA(path) ? true : false;
    else
        return RemoveDirectoryA(path) ? true : false;
#else
    return type == SX_FILE_TYPE_REGULAR ? (unlink(path) == 0) : (rmdir(path) == 0);
#endif
}

bool sx_os_mkdir(const char* path)
{
#if SX_PLATFORM_WINDOWS
    return CreateDirectoryA(path, NULL) == TRUE;
#else
    return mkdir(path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == 0;
#endif
}

char* sx_os_path_exepath(char* dst, int size)
{
#if SX_PLATFORM_WINDOWS
    GetModuleFileName(NULL, dst, size);
    return dst;
#elif SX_PLATFORM_LINUX || SX_PLATFORM_RPI
    char proc_path[32];
    sx_snprintf(proc_path, sizeof(proc_path), "/proc/%d/exe", getpid());
    size_t bytes = readlink(proc_path, dst, size);
    dst[bytes] = 0;
    return dst;
#elif SX_PLATFORM_OSX
    _NSGetExecutablePath(dst, (uint32_t*)&size);
    return dst;
#else
    sx_unused(dst);
    sx_unused(size);
    sx_assertf(0, "not implemented");
    return NULL;
#endif
}


sx_file_info sx_os_stat(const char* filepath)
{
    sx_assert(filepath);
    sx_file_info info = { SX_FILE_TYPE_INVALID, 0, 0 };

#if SX_PLATFORM_WINDOWS
    WIN32_FILE_ATTRIBUTE_DATA fad;
    if (!GetFileAttributesExA(filepath, GetFileExInfoStandard, &fad)) {
        return info;
    }
    if (fad.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        info.type = SX_FILE_TYPE_DIRECTORY;
    else if (!(fad.dwFileAttributes & (FILE_ATTRIBUTE_DEVICE | FILE_ATTRIBUTE_SYSTEM)))
        info.type = SX_FILE_TYPE_REGULAR;

    LARGE_INTEGER size;
    size.HighPart = fad.nFileSizeHigh;
    size.LowPart = fad.nFileSizeLow;
    info.size = (uint64_t)size.QuadPart;

    LARGE_INTEGER tm;
    tm.HighPart = fad.ftLastWriteTime.dwHighDateTime;
    tm.LowPart = fad.ftLastWriteTime.dwLowDateTime;
    info.last_modified = (uint64_t)(tm.QuadPart / 10000000 - 11644473600LL);
#else // if SX_PLATFORM_WINDOWS
    struct stat st;
    int32_t result = stat(filepath, &st);
    if (0 != result)
        return info;

    if (0 != (st.st_mode & S_IFREG))
        info.type = SX_FILE_TYPE_REGULAR;
    else if (0 != (st.st_mode & S_IFDIR))
        info.type = SX_FILE_TYPE_DIRECTORY;
    info.size = st.st_size;
#    if SX_PLATFORM_OSX || SX_PLATFORM_IOS
    info.last_modified = st.st_mtimespec.tv_sec;
#    else
    info.last_modified = st.st_mtim.tv_sec;
#    endif
#endif    // else SX_PLATFORM_WINDOWS

    return info;
}

char* sx_os_path_pwd(char* dst, int size)
{
#if SX_PLATFORM_PS4 || SX_PLATFORM_XBOXONE || SX_PLATFORM_WINRT || SX_CRT_NONE
    sx_unused(dst);
    sx_unused(size);
    return NULL;
#elif SX_CRT_MSVC
    return _getcwd(dst, size);
#else
    return getcwd(dst, size);
#endif    // SX_COMPILER_
}

char* sx_os_path_abspath(char* dst, int size, const char* path)
{
#if SX_PLATFORM_POSIX
    char abs_path[PATH_MAX];
    if (realpath(path, abs_path) != NULL) {
        sx_strcpy(dst, size, abs_path);
    } else {
        dst[0] = '\0';
    }
    return dst;
#elif SX_PLATFORM_WINDOWS
    if (GetFullPathNameA(path, (DWORD)size, dst, NULL) == 0)
        dst[0] = '\0';
    return dst;
#else
#    error "Not Implemented"
#endif
}

char* sx_os_path_unixpath(char* dst, int size, const char* path)
{
    int len = sx_strlen(path);
    len = sx_min(len, size - 1);

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
    len = sx_min(len, size - 1);

    for (int i = 0; i < len; i++) {
        if (path[i] != '/')
            dst[i] = path[i];
        else
            dst[i] = '\\';
    }
    dst[len] = '\0';
    return dst;
}

char* sx_os_path_basename(char* dst, int size, const char* path)
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

char* sx_os_path_dirname(char* dst, int size, const char* path)
{
    const char* r = sx_strrchar(path, '/');
    if (!r)
        r = sx_strrchar(path, '\\');
    if (r) {
        int o = (int)(intptr_t)(r - path);
        if (dst == path) {
            dst[o] = '\0';
        } else {
            sx_strncpy(dst, size, path, o);
        }
    } else if (dst != path) {
        *dst = '\0';
    }
    return dst;
}

char* sx_os_path_splitext(char* ext, int ext_size, char* basename, int basename_size,
                          const char* path)
{
    sx_assert(ext != path);

    int len = sx_strlen(path);
    if (len > 0) {
        const char* start = sx_strrchar(path, '/');
        if (!start)
            start = sx_strrchar(path, '\\');
        if (!start)
            start = path;
        const char* end = &path[len - 1];
        for (const char* e = start; e < end; ++e) {
            if (*e != '.')
                continue;
            sx_strcpy(ext, ext_size, e);
            if (basename != path)
                sx_strncpy(basename, basename_size, path, (int)(intptr_t)(e - path));
            else
                *((char*)e) = '\0';
            return ext;
        }
    }

    // no extension (.) found
    ext[0] = '\0';
    if (basename != path)
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
        const char* end = &path[len - 1];
        for (const char* e = start; e < end; ++e) {
            if (*e != '.')
                continue;
            sx_strcpy(dst, size, e);
            return dst;
        }
    }

    dst[0] = '\0';    // no extension
    return dst;
}

char* sx_os_path_join(char* dst, int size, const char* path_a, const char* path_b)
{
    sx_assert(dst != path_b);
    int len = sx_strlen(path_a);
    if (dst != path_a) {
        if (len > 0 && path_a[len - 1] == k_path_sep[0]) {
            sx_strcpy(dst, size, path_a);
        } else if (len > 0) {
            sx_strcpy(dst, size, path_a);
            sx_strcat(dst, size, k_path_sep);
        } else {
            dst[0] = '\0';
        }
    } else if (len > 0 && path_a[len - 1] != k_path_sep[0]) {
        sx_strcat(dst, size, k_path_sep);
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
    sx_assert(start != dst);

    const char* sub = sx_strstr(path, start);
    if (sub) {
        int len = sx_strlen(start);
        if (len > 0 && (sub[len] == '/' || sub[len] == '\\'))
            len++;
        if (path != dst)
            sx_strcpy(dst, size, sub + len);
        else
            sx_memmove(dst, sub + len, (size_t)sx_strlen(sub + len) + 1);
        return dst;
    }
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
#elif SX_PLATFORM_APPLE
    return sx_tolower(dst, size, sx_os_path_unixpath(dst, size, path));
#else
    return sx_os_path_unixpath(dst, size, path);
#endif
}

int sx_os_numcores()
{
#if SX_PLATFORM_WINDOWS
    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);
    return sysinfo.dwNumberOfProcessors;
#elif SX_PLATFORM_LINUX || SX_PLATFORM_RPI
    return sysconf(_SC_NPROCESSORS_ONLN);
#elif SX_PLATFORM_ANDROID
    return android_getCpuCount();
#elif SX_PLATFORM_APPLE
    int ncpu;
    size_t ncpu_len = sizeof(ncpu);
    // hw.physicalcpu - The number of physical processors available in the current power management
    // mode. hw.physicalcpu_max - The maximum number of physical processors that could be available
    // this boot. hw.logicalcpu - The number of logical processors available in the current power
    // management mode. hw.logicalcpu_max - The maximum number of logical processors that could be
    // available this boot.
    if (sysctlbyname("hw.logicalcpu", &ncpu, &ncpu_len, NULL, 0) == 0)
        return ncpu;
    return 1;
#elif SX_PLATFORM_BSD
    int ctlarg[2], ncpu;
    size_t len;

    ctlarg[0] = CTL_HW;
    ctlarg[1] = HW_NCPU;
    len = sizeof(ncpu);
    if (sysctl(ctlarg, 2, &ncpu, &len, 0, 0) == 0)
        return ncpu;
    return 1;
#endif
}

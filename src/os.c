#include "sx/os.h"

#if SX_PLATFORM_WINDOWS
#   define VC_EXTRALEAN
#   define WIN32_LEAN_AND_MEAN
#	include <windows.h>
#   include <conio.h>

int sx_os_pagesz()
{
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    return (int)si.dwPageSize;
}

int sx_os_maxstacksz()
{
    return 1073741824;  // 1gb
}

int sx_os_minstacksz()
{
#   if SX_ARCH_64BIT
    return 8192;    
#   else
    return 4096;
#   endif    
}

char sx_os_getch()
{
    return _getch();
}

#elif SX_PLATFORM_POSIX

#include <unistd.h>
#include <sys/resource.h>
#include <stdio.h>
#include <termios.h>

int sx_os_minstacksz()
{
    return 32768;   // 32kb
}

int sx_os_pagesz()
{
    return sysconf(_SC_PAGESIZE);    
}

int sx_os_maxstacksz()
{
    struct rlimit limit;
    getrlimit(RLIMIT_STACK, &limit);
    return limit.rlim_max;
}

char sx_os_getch()
{
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
}
#endif  // SX_PLATFORM_

size_t sx_os_align_pagesz(size_t size)
{
    assert(size > 0);
    int page_sz = sx_os_pagesz();
    size_t page_cnt = (size + page_sz - 1) / page_sz;
    return page_cnt * page_sz;
}



#include "sx/os.h"

#if SX_PLATFORM_WINDOWS
#   define VC_EXTRALEAN
#   define WIN32_LEAN_AND_MEAN
#	include <windows.h>

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

#elif SX_PLATFORM_POSIX

#endif

size_t sx_os_align_pagesz(size_t size)
{
    assert(size > 0);
    int page_sz = sx_os_pagesz();
    size_t page_cnt = (size + page_sz - 1) / page_sz;
    return page_cnt * page_sz;
}



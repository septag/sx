#ifndef SX_OS_H_
#   error "This file must only be included within os.h"
#endif

#if SX_PLATFORM_WINDOWS
inline int sx_os_minstacksz()
{
#   if SX_ARCH_64BIT
    return 8192;    
#   else
    return 4096;
#   endif    
}

#elif SX_PLATFORM_POSIX
inline int sx_os_minstacksz()
{
    return 32768;   // 32kb
}

#endif  // SX_PLATFORM_


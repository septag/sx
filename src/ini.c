#define INI_IMPLEMENTATION

#include "sx/allocator.h"

#define INI_MALLOC(ctx, size)   ( sx_malloc((const sx_alloc*)ctx, size) )
#define INI_FREE(ctx, ptr)      ( sx_free((const sx_alloc*)ctx, ptr) )

#if SX_PLATFORM_WINDOWS
#   define INI_STRNICMP(s1, s2, cnt)   (_strnicmp(s1, s2, cnt))
#else
#   define INI_STRNICMP(s1, s2, cnt)   (strncasecmp(s1, s2, cnt))
#endif
// TODO: INI_MEMCPY, INI_STRLEN
#include "../3rdparty/ini/ini.h"

ini_t* sx_ini_create(const sx_alloc* alloc)
{
    return ini_create((void*)alloc);
}

ini_t* sx_ini_load(const char* data, const sx_alloc* alloc)
{
    return ini_load(data, (void*)alloc);
}
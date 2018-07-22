#include "sx/string.h"
#include "sx/array.h"

SX_PRAGMA_DIAGNOSTIC_PUSH();
SX_PRAGMA_DIAGNOSTIC_IGNORED_CLANG_GCC("-Wunused-function")
#define STB_SPRINTF_IMPLEMENTATION
#define STB_SPRINTF_STATIC
#include "../3rdparty/stb/stb_printf.h"
SX_PRAGMA_DIAGNOSTIC_POP();

typedef struct sx__printf_ctx_s
{
    const sx_alloc* alloc;
    char* buff;
    int len;
    char tmp[STB_SPRINTF_MIN];
} sx__printf_ctx;

int sx_snprintf(char* str, int size, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    int r = sx_vsnprintf(str, size, fmt, args);
    va_end(args);
    return r;
}

int sx_vsnprintf(char* str, int size, const char* fmt, va_list args)
{
    return stbsp_vsnprintf(str, size, fmt, args);
}

char* sx_snprintf_alloc(const sx_alloc* alloc, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    char* str = sx_vsnprintf_alloc(alloc, fmt, args);
    va_end(args);
    return str;
}

static char* sx__vsnprintf_callback(char* buff, void* user, int len)
{
    sx__printf_ctx* ctx = (sx__printf_ctx*)user;
    int len_ = len + 1;     // Reserve one character for null-termination
    sx_array_add(ctx->alloc, ctx->buff, len_);
    memcpy(ctx->buff + ctx->len, ctx->tmp, len);
    ctx->len += len;
    return ctx->tmp;
}

char* sx_vsnprintf_alloc(const sx_alloc* alloc, const char* fmt, va_list args)
{
    sx__printf_ctx ctx;
    ctx.alloc = alloc;
    ctx.buff = NULL;
    ctx.len = 0;
    stbsp_vsprintfcb(sx__vsnprintf_callback, &ctx, ctx.tmp, fmt, args);
    ctx.buff[ctx.len] = '\0';
    return ctx.buff;
}

int sx_strcpy(char* dst, int dst_sz, const char* src)
{
    assert(dst);
    assert(src);

    const int len = sx_strlen(src);
    const int32_t max = dst_sz-1;
    const int32_t num = (len < max ? len : max);
    memcpy(dst, src, num);
    dst[num] = '\0';

    return num;
}

int sx_strlen(const char* str)
{
    assert(str);

    const char* ptr = str;
    for (; *ptr != '\0'; ++ptr) {};
    return (int)(intptr_t)(ptr - str);
}

SX_INLINE int sx__strnlen(const char* str, int _max)
{
    assert(str);

    const char* ptr = str;
    for (; _max > 0 && *ptr != '\0'; ++ptr, --_max) {};
    return (int)(intptr_t)(ptr - str);
}

int sx_strncpy(char* dst, int dst_sz, const char* src, int _num)
{
    assert(dst);
    assert(src);

    const int len = sx__strnlen(src, _num);
    const int32_t max = dst_sz-1;
    const int32_t num = (len < max ? len : max);
    memcpy(dst, src, num);
    dst[num] = '\0';

    return num;
}


//
// Copyright 2018 Sepehr Taghdisian (septag@github). All rights reserved.
// License: https://github.com/septag/sx#license-bsd-2-clause
//
#include "sx/sx.h"
#include "sx/string.h"

#if SX_PLATFORM_ANDROID
#    include <android/log.h>
#elif SX_PLATFORM_WINDOWS
__declspec(dllimport) void __stdcall OutputDebugStringA(const char* _str);
#else
#    include <stdio.h>    // fputs, fflush
#endif

static void sx__default_assert_handler(const char* text, const char* sourcefile, uint32_t line)
{
    char output_text[2048];
    if (sourcefile) {
        sx_snprintf(output_text, sizeof(output_text), "%s(%d): ASSERT FAILURE - ", sourcefile, line);
    } else {
        sx_strcpy(output_text, sizeof(output_text), "ASSERT FAILURE - ");
    }
    char* endptr = sx_strcat(output_text, sizeof(output_text), text);
    sx_unused(endptr);

    // print
#if SX_PLATFORM_ANDROID
    __android_log_write(ANDROID_LOG_DEBUG, "", output_text);
#elif SX_PLATFORM_WINDOWS
    sx_strcat(endptr, sizeof(output_text), "\n");
    OutputDebugStringA(output_text);
#else
    fputs(output_text, stderr);
    fflush(stderr);
#endif
}

static sx_assert_cb* g_assert_handler = sx__default_assert_handler;

void sx_set_assert_callback(sx_assert_cb* callback)
{
    g_assert_handler = callback ? callback : sx__default_assert_handler;
}

void sx__debug_message(const char* sourcefile, uint32_t line, const char* fmt, ...)
{
    char text[2048];

    va_list args;
    va_start(args, fmt);
    sx_vsnprintf(text, sizeof(text), fmt, args);
    va_end(args);

    g_assert_handler(text, sourcefile, line);
}


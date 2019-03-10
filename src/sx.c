//
// Copyright 2018 Sepehr Taghdisian (septag@github). All rights reserved.
// License: https://github.com/septag/sx#license-bsd-2-clause
//
#include "sx/sx.h"

#if SX_PLATFORM_ANDROID
#    include <android/log.h>
#elif SX_PLATFORM_WINDOWS
__declspec(dllimport) void __stdcall OutputDebugStringA(const char* _str);
#elif SX_PLATFORM_APPLE
#    if defined(__OBJC__)
#        import <Foundation/NSObjCRuntime.h>
#    else
#        include <CoreFoundation/CFString.h>
void NSLog(CFStringRef _format, ...);
#    endif    // defined(__OBJC__)
#else
#    include <stdio.h>    // fputs, fflush
#endif

// CRT symbol workaround
#ifdef __cplusplus
extern "C" {
#endif
int _fltused = 0;

void sx__break_program(const char* text) {
    // print
#if SX_PLATFORM_ANDROID
    __android_log_write(ANDROID_LOG_DEBUG, "", text);
#elif SX_PLATFORM_WINDOWS
    OutputDebugStringA(text);
#elif SX_PLATFORM_APPLE
#    if defined(__OBJC__)
    NSLog(@"%s", text);
#    else
    NSLog(__CFStringMakeConstantString("%s"), text);
#    endif
#else
    fputs(text, stderr);
    fflush(stderr);
#endif

    // break
#if SX_COMPILER_MSVC
    __debugbreak();
#elif SX_CPU_ARM
    __buildin_trap();
#elif SX_CPU_X86 && (SX_COMPILER_GCC || SX_COMPILER_CLANG)
    __asm__("int $3");
#else
    int* int3 = (int*)3L;
    *int3 = 3;
#endif
}

#ifdef __cplusplus
}
#endif

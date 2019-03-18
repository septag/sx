//
// Copyright 2018 Sepehr Taghdisian (septag@github). All rights reserved.
// License: https://github.com/septag/sx#license-bsd-2-clause
//
// timer.h - v1.1 - portable high resolution timer
//                  Implementation is sokol_time, just a thin wrapper
//
//    void sx_tm_now();
//        Call once before any other functions to initialize sokol_time
//        (this calls for instance QueryPerformanceFrequency on Windows)
//
//    uint64_t sx_tm_now();
//        Get current point in time in unspecified 'ticks'. The value that
//        is returned has no relation to the 'wall-clock' time and is
//        not in a specific time unit, it is only useful to compute
//        time differences.
//
//    uint64_t sx_tm_diff(uint64_t new, uint64_t old);
//        Computes the time difference between new and old. This will always
//        return a positive, non-zero value.
//
//    uint64_t sx_tm_since(uint64_t start);
//        Takes the current time, and returns the elapsed time since start
//        (this is a shortcut for "sx_tm_diff(sx_tm_now(), start)")
//
//    uint64_t sx_tm_laptime(uint64_t* last_time);
//        This is useful for measuring frame time and other recurring
//        events. It takes the current time, returns the time difference
//        to the value in last_time, and stores the current time in
//        last_time for the next call. If the value in last_time is 0,
//        the return value will be zero (this usually happens on the
//        very first call).
//
//    Use the following functions to convert a duration in ticks into
//    useful time units:

//    double sx_tm_sec(uint64_t ticks);
//    double sx_tm_ms(uint64_t ticks);
//    double sx_tm_us(uint64_t ticks);
//    double sx_tm_ns(uint64_t ticks);
//        Converts a tick value into seconds, milliseconds, microseconds
//        or nanoseconds. Note that not all platforms will have nanosecond
//        or even microsecond precision.
//
// For more information and examples: https://github.com/floooh/sokol
//
#pragma once

#include "macros.h"
#include <stdint.h>

SX_API void sx_tm_init();
SX_API uint64_t sx_tm_now();
SX_API uint64_t sx_tm_diff(uint64_t new_ticks, uint64_t old_ticks);
SX_API uint64_t sx_tm_since(uint64_t start_ticks);
SX_API uint64_t sx_tm_laptime(uint64_t* last_ticks);

SX_API double sx_tm_sec(uint64_t ticks);
SX_API double sx_tm_ms(uint64_t ticks);
SX_API double sx_tm_us(uint64_t ticks);
SX_API double sx_tm_ns(uint64_t ticks);


// Version history
// 1.0      initial release
// 1.1      renamed sx_tm_init to sx_tm_now
//          moved sokol_time.h to the .C file and wraped the functions
//
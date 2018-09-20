//
// Copyright 2018 Sepehr Taghdisian (septag@github). All rights reserved.
// License: https://github.com/septag/sx#license-bsd-2-clause
//

#define SOKOL_IMPL 
#include "../3rdparty/sokol/sokol_time.h"

void sx_tm_init()
{
    stm_setup();
}

uint64_t sx_tm_now()
{
    return stm_now();
}

uint64_t sx_tm_diff(uint64_t new_ticks, uint64_t old_ticks)
{
    return stm_diff(new_ticks, old_ticks);    
}

uint64_t sx_tm_since(uint64_t start_ticks)
{
    return stm_since(start_ticks);
}

uint64_t sx_tm_laptime(uint64_t* last_ticks)
{
    return stm_laptime(last_ticks);
}

double sx_tm_sec(uint64_t ticks)
{
    return stm_sec(ticks);
}

double sx_tm_ms(uint64_t ticks)
{
    return stm_ms(ticks);
}

double sx_tm_us(uint64_t ticks)
{
    return stm_us(ticks);
}

double sx_tm_ns(uint64_t ticks)
{
    return stm_ns(ticks);
}


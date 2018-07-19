// os.h - v1.0 - Common portable OS related functions
//
#pragma once

#ifndef SX_OS_H_
#define SX_OS_H_

#include "sx.h"

#ifdef __cplusplus
extern "C" {
#endif

int sx_os_minstacksz();
int sx_os_maxstacksz();
int sx_os_pagesz();
size_t sx_os_align_pagesz(size_t size);

#ifdef __cplusplus
}
#endif  // extern "C"

#include "inline/os.inl"

#endif
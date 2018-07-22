// os.h - v1.0 - Common portable OS related functions
//
#pragma once

#ifndef SX_OS_H_
#define SX_OS_H_

#include "sx.h"

SX_EXTERN int sx_os_minstacksz();
SX_EXTERN int sx_os_maxstacksz();
SX_EXTERN int sx_os_pagesz();
SX_EXTERN size_t sx_os_align_pagesz(size_t size);
SX_EXTERN char sx_os_getch();

#endif
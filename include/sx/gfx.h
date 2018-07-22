// gfx.h - Low level graphics API wrapper
//         basically includes sokol_gfx and implements it in the gfx.c
//         Different APIs are chosen for each platform:
//              Windows         Direct3D11
//              iOS/MacOS       Metal
//              RaspberryPI     OpenGL-ES2
//              Emscripten      OpenGL-ES2
//              Android         OpenGL-ES3
//              Linux           OpenGL 3.3
//          
//         sg_setup_alloc   Use this for initial setup, do NOT use 'sg_setup' or you'll get a crash
//
// Source: https://github.com/floooh/sokol
//
#pragma once

#ifndef SX_GFX_H_
#define SX_GFX_H_

#include "allocator.h"

#ifndef _SX_GFX_PRIVATE
#   include "../../3rdparty/sokol/sokol_gfx.h"
#endif

// Instead of using sg_setup, use this function to initialize the sokol_gfx
SX_EXTERN void sg_setup_alloc(const sx_alloc* alloc, const sg_desc* desc);

#endif // SX_GFX_H_


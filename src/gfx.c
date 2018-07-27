//
// Copyright 2018 Sepehr Taghdisian (septag@github). All rights reserved.
// License: https://github.com/septag/sx#license-bsd-2-clause
//
#include "sx/allocator.h"

// Choose api based on the platform 
#if SX_PLATFORM_WINDOWS
#   define SOKOL_D3D11
#elif SX_PLATFORM_APPLE
#   define SOKOL_METAL
#elif SX_PLATFORM_RPI || SX_PLATFORM_EMSCRIPTEN
#   define SOKOL_GLES2
#   define GL_GLEXT_PROTOTYPES
#   include <GLES2/gl2.h>
#   include <GLES2/gl2ext.h>
#elif SX_PLATFORM_ANDROID
#   define SOKOL_GLES3
#elif SX_PLATFORM_LINUX
#   define SOKOL_GLCORE33
#   include <GL/glew.h>     // glew must be installed on linux
#else
#   define SOKOL_GLCORE33
#endif

static const sx_alloc* g_gfx_alloc = NULL;

#define SOKOL_MALLOC(s) sx_malloc(g_gfx_alloc, s)
#define SOKOL_FREE(p)   sx_free(g_gfx_alloc, p)
// TODO: SOKOL_LOG, SOKOL_ASSERT, SOKOL_UNREACHABLE, SOKOL_VALIDATE_BEGIN, SOKOL_VALIDATE, SOKOL_VALIDATE_END

SX_PRAGMA_DIAGNOSTIC_PUSH();
SX_PRAGMA_DIAGNOSTIC_IGNORED_CLANG_GCC("-Wunused-function")
SX_PRAGMA_DIAGNOSTIC_IGNORED_CLANG_GCC("-Wunused-variable")

#ifdef __cplusplus
SX_PRAGMA_DIAGNOSTIC_IGNORED_MSVC(4800) // forcing value to bool 'true' or 'false' (performance warning)
#endif

#define SOKOL_IMPL
#include "../3rdparty/sokol/sokol_gfx.h"

SX_PRAGMA_DIAGNOSTIC_POP();

#define _SX_GFX_PRIVATE
#include "sx/gfx.h"

void sg_setup_alloc(const sx_alloc* alloc, const sg_desc* desc)
{
#if SX_PLATFORM_LINUX
    glewExperimental = GL_TRUE;
    GLenum r = glewInit();
    if (r != GLEW_OK) {
        printf("Error :%s\n", glewGetErrorString(r));
    }
    glGetError();
#endif
    g_gfx_alloc = alloc;
    sg_setup(desc);
}
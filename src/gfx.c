#include "sx/allocator.h"

// Choose api based on the platform 
#if SX_PLATFORM_WINDOWS
#   define SOKOL_D3D11
#elif SX_PLATFORM_APPLE
#   define SOKOL_METAL
#elif SX_PLATFORM_RPI || SX_PLATFORM_EMSCRIPTEN
#   define SOKOL_GLES2
#elif SX_PLATFORM_ANDROID
#   define SOKOL_GLES3
#elif SX_PLATFORM_LINUX
#   define SOKOL_GLCORE33
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

#define SOKOL_IMPL
#include "../3rdparty/sokol/sokol_gfx.h"

SX_PRAGMA_DIAGNOSTIC_POP();

void sg_setup_alloc(const sx_alloc* alloc, const sg_desc* desc)
{
    g_gfx_alloc = alloc;
    sg_setup(desc);
}
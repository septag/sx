#include "sx/sx.h"

// Choose api based on the platform
#if SX_PLATFORM_WINDOWS
#   define SOKOL_D3D11
#   define D3D11_NO_HELPERS
#   define CINTERFACE
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

// TODO: SOKOL_ASSERT, SOKOL_LOG, SOKOL_UNREACHABLE, SOKOL_ABORT
SX_PRAGMA_DIAGNOSTIC_PUSH();
SX_PRAGMA_DIAGNOSTIC_IGNORED_CLANG_GCC("-Wunused-function")

#define SOKOL_IMPL
#include "../3rdparty/sokol/sokol_app.h"

SX_PRAGMA_DIAGNOSTIC_POP();

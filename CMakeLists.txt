cmake_minimum_required(VERSION 3.0)
project(sx)

function(sx_remove_compile_options DEST_VAR COMPILER_FLAGS FLAGS)
    separate_arguments(FLAGS)
    foreach(FLAG ${FLAGS})
        string(REPLACE "${FLAG}" "" COMPILER_FLAGS "${COMPILER_FLAGS}")
    endforeach()
    set(${DEST_VAR} ${COMPILER_FLAGS} PARENT_SCOPE)
endfunction()


option(SX_BUILD_TESTS "Build test executables" TRUE)
option(SX_NO_APP "Exclude App (sokol_app) module integration" FALSE)
option(SX_NO_GFX "Exclude Gfx (sokol_gfx) module integration" FALSE)

# Enable Assembler
if (MSVC)
    enable_language(CXX ASM_MASM)
else()
    enable_language(CXX ASM)
endif()

set(SOURCE_FILES src/sx.c
                 src/allocator.c
                 src/threads.c
                 src/lin-alloc.c
                 src/hash.c
                 src/stack-alloc.c
                 src/os.c 
                 src/string.c
                 src/io.c
                 src/cmdline.c 
                 src/hash.c
                 src/handle.c
                 src/timer.c
                 src/gfx.c
                 src/app.c
                 src/rng.c
                 src/ini.c
                 src/virtual-alloc.c
                 src/fiber.c
                 src/math.c 
                 src/jobs.c)
set(INCLUDE_FILES include/sx/allocator.h
                  include/sx/array.h 
                  include/sx/config.h 
                  include/sx/macros.h 
                  include/sx/platform.h 
                  include/sx/sx.h 
                  include/sx/atomic.h
                  include/sx/threads.h
                  include/sx/lin-alloc.h
                  include/sx/hash.h
                  include/sx/stack-alloc.h
                  include/sx/os.h 
                  include/sx/string.h
                  include/sx/handle.h
                  include/sx/pool.h
                  include/sx/io.h
                  include/sx/cmdline.h
                  include/sx/timer.h
                  include/sx/gfx.h
                  include/sx/app.h
                  include/sx/rng.h
                  include/sx/ini.h
                  include/sx/virtual-alloc.h 
                  include/sx/fiber.h
                  include/sx/math.h
                  include/sx/jobs.h)

if (SX_NO_GFX)
    list(REMOVE_ITEM SOURCE_FILES src/gfx.c)
    list(REMOVE_ITEM INCLUDE_FILES include/sx/gfx.h)
endif()

if (SX_NO_APP)
    list(REMOVE_ITEM SOURCE_FILES src/app.c)
    list(REMOVE_ITEM INCLUDE_FILES include/sx/app.h)
endif()

# I don't know if emscripten has 
if (EMSCRIPTEN)
    list(REMOVE_ITEM SOURCE_FILES src/fiber.c)
    list(REMOVE_ITEM SOURCE_FILES include/sx/fiber.h)
endif()

####################################################################################################
# Assembly files for fcontext
if (APPLE)
    # Apple comboned, include based on arch
    set(CPU_ARCH "combined")
    set(ASM_EXT "all_macho_gas.S")
elseif (ANDROID)
    # Android (Arm/x86_64/Arm64)
    if (${CMAKE_SYSTEM_PROCESSOR} MATCHES "arm")
        set(CPU_ARCH "arm")
        set(ASM_EXT "aapcs_elf_gas.S")
    elseif (${CMAKE_SYSTEM_PROCESSOR} MATCHES "aarch64")
        set(CPU_ARCH "arm64")
        set(ASM_EXT "aapcs_elf_gas.S")
    elseif (${CMAKE_SYSTEM_PROCESSOR} MATCHES "i686")
        set(CPU_ARCH "i386")
        set(ASM_EXT "sysv_elf_gas.S")
    elseif (${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86_64")
        set(CPU_ARCH "x86_64")
        set(ASM_EXT "sysv_elf_gas.S")
    endif()
elseif (UNIX)
    # Unix systems (x86/x64)
    if (CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(CPU_ARCH "x86_64")
    else()
        set(CPU_ARCH "i386")
    endif()
    set(ASM_EXT "sysv_elf_gas.S") 
elseif (WIN32)
    # Windows (x86/64)
    if (CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(CPU_ARCH "x86_64")
    else()
        set(CPU_ARCH "i386")
    endif()
    set(ASM_EXT "ms_pe_masm.asm")
endif()

set(ASM_SOURCES "asm/make_${CPU_ARCH}_${ASM_EXT}"
                "asm/jump_${CPU_ARCH}_${ASM_EXT}"
                "asm/ontop_${CPU_ARCH}_${ASM_EXT}")
set_source_files_properties(${ASM_SOURCES} PROPERTIES COMPILE_DEFINITIONS BOOST_CONTEXT_EXPORT=)

if (EMSCRIPTEN) 
    unset(ASM_SOURCES)
endif()
####################################################################################################

add_definitions(-D__STDC_LIMIT_MACROS)
add_definitions(-D__STDC_FORMAT_MACROS)
add_definitions(-D__STDC_CONSTANT_MACROS)
if(MSVC)
    add_definitions(-D_ITERATOR_DEBUG_LEVEL=0)
    add_definitions(-D_HAS_EXCEPTIONS=0)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS=0)
endif()

if ((CMAKE_C_COMPILER_ID MATCHES "Clang") OR (CMAKE_C_COMPILER_ID MATCHES "GNU"))
    add_compile_options(-nostdlib -Wno-deprecated-declarations)

    # MSVC + Clang_C2
    if (MSVC)
        add_compile_options("$<$<CONFIG:Debug>:-g>")
    endif()
    add_compile_options("$<$<CONFIG:Debug>:-D_DEBUG>")
    
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu11")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fno-rtti -fno-exceptions")
    sx_remove_compile_options(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}" "-fexceptions -frtti")
elseif (MSVC) 
    # Build files as CPP, C support in MSVC is crap
    set_source_files_properties(${SOURCE_FILES} PROPERTIES LANGUAGE CXX)
    sx_remove_compile_options(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}" "/EHsc /GR")
    add_compile_options(/GR-)
endif()

add_library(sx ${SOURCE_FILES} ${INCLUDE_FILES} ${ASM_SOURCES})

# compat files include dir
if (MSVC)
    set(COMPAT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include/compat/msvc)
elseif(APPLE)
    if (IOS)
        set(COMPAT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include/compat/ios)
    else()
        set(COMPAT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include/compat/osx)
    endif()
endif()

target_include_directories(sx PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include ${COMPAT_DIR})

# Link system libs
if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    target_link_libraries(sx PUBLIC dl pthread)
    if (NOT SX_NO_GFX)
        target_link_libraries(sx PUBLIC GL GLEW)
    endif()
    if (NOT SX_NO_APP)
        target_link_libraries(sx PUBLIC X11)
    endif()
elseif (WIN32)
    if (NOT SX_NO_GFX)
        target_link_libraries(sx PUBLIC dxgi d3d11)
    endif()
endif()

# Tests
if (SX_BUILD_TESTS)
    add_subdirectory(tests)
endif()



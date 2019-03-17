set(CMAKE_SYSTEM_NAME Linux CACHE INTERNAL "")
set(CMAKE_SYSTEM_VERSION 1 CACHE INTERNAL "")
set(CMAKE_SYSTEM_PROCESSOR arm CACHE INTERNAL "")

set(RPI TRUE CACHE BOOL "")

if (NOT RPI_SYSGCC_ROOT)
    message(FATAL_ERROR "RPI_SYSGCC_ROOT directory is not defined.")
endif()

# cross compiler locations
if (WIN32)
    set(executable_suffix ".exe")
endif()

set(CMAKE_C_COMPILER ${RPI_SYSGCC_ROOT}/bin/arm-linux-gnueabihf-gcc${executable_suffix})
set(CMAKE_CXX_COMPILER ${RPI_SYSGCC_ROOT}/bin/arm-linux-gnueabihf-g++${executable_suffix})

# find path
set(CMAKE_FIND_ROOT_PATH ${RPI_SYSGCC_ROOT}/arm-linux-gnueabihf/sysroot)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

set(CMAKE_CXX_COMPILER_FORCED TRUE)
set(CMAKE_CXX_COMPILER_WORKS TRUE)
set(CMAKE_C_COMPILER_FORCED TRUE)
set(CMAKE_C_COMPILER_WORKS TRUE)


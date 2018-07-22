# SX: Portable base library for C/C++ programs (WIP)
[@septag](https://twitter.com/septagh)

SX is merely a collection of sources and 3rdparty libraries. It is written in C and can be compiled using C++ compiler like MSVC. 
Designed for simplicity and performance, It is tailored more towards system and game programmers.  
Inspired by the works of [Sean Barret](https://github.com/nothings), [Branimir Karadric](https://github.com/bkaradzic), [Andre Weissflog](https://github.com/floooh), [Mathias Gustavsson](https://github.com/mattiasgustavsson), [r-lyeh](https://github.com/r-lyeh), [ocornut](https://github.com/ocornut), I decided to make my own C base library for future projects. I also used and borrowed many of their code inside SX.

This library currently contains:

- allocator.h: basic memory allocation functions and default heap/leak_check allocators. Allocators are used anywhere in the code that needs memory allocation/deallocation. There is no direct memory allocations like _malloc_ inside any function.
- app.h: [sokol_app](https://github.com/floooh/sokol) implementation and wrapper 
- array.h: [stretchy_buffer](https://github.com/nothings/stb/blob/master/stretchy_buffer.h) implementation
- atomic.h: Set of portable atomic types and functions like CAS/Exchange/Incr/...
- cmdline.h: [getopt](https://github.com/wc-duck/getopt) command line parser wrapper
- fiber.h: low-level portable assembly fiber implementation taken from my other project [deboost.context](https://github.com/septag/deboost.context)
- gfx.h: [sokol_gfx](https://github.com/floooh/sokol) implementation and wrapper 
- handlepool.h: Handle pool
- hash.h:  Some nice hash functions and a fast fibonacci multiplicative hash-table
- ini.h: [ini.h](https://github.com/mattiasgustavsson/libs/blob/master/ini.h) implementation for sx
- io.h: Input/output to memory/file IO
- lin-alloc.h: Generic linear allocator
- platform.h: Platform detection macros, taken from [bx](https://github.com/bkaradzic/bx)
- pool.h: Fixed size pool allocator
- rng.h: PCG random number generator
- stack-alloc.h: Generic linear stack based allocator
- string.h: Useful string manipulation functions
- threads.h: Portable threading primitives (Thread, Tls, Mutex, Semaphore, Signal, SpScQueue)
- timer.h: Portable high-res timer, wrapper for [sokol_time](https://github.com/floooh/sokol)
- virtual-alloc.h: Portable virtual memory allocator and functions
- math.h: Vector math primitives and functions, many of the functions are taken from [bx](https://github.com/bkaradzic/bx)
- os.h: Common portable OS related routines

## Build
### Windows
On windows, I'm building with *clang_c2*, to choose clang_c2 toolchain with msvc run this cmake command:

```
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug -G "Visual Studio 14 2015 Win64" -T v140_clang_c2
```

### Linux

#### Dependencies
- Install __glew__ _(libglew-dev)_


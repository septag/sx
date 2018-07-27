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
### CMake options

These are general options for cmake, where you can trim or customize the build:  

- **SX_BUILD_TESTS** (Default=1): Set SX_BUILD_TESTS=0 to skip building test applications
- **SX_NO_GFX** (Default=0): By default SX includes and implements sokol_gfx, set SX_NO_GFX=1 to ignore sokol_gfx in the build
- **SX_NO_APP** (Default=0): By default SX includes and implements sokol_app, set SX_NO_APP=1 to ignore sokol_app in the build

### Windows
On windows, MSVC's support for C99/C11 standard is basically broken and non-usable, the code is compatible with C++ compilers, so in MSVC it uses CPP compiler instead of C:
```
cd build
cmake .. -G "Visual Studio 14 2015 Win64"
```

It is possible to build it with *MSVC + clang_c2* toolset which in that case you have to add the ```-T v140_clang_c2``` switch to cmake command (vs2015)

### Linux

__Dependencies__

### Emscripten

It can be built on emscripten (using the _Emscripten.cmake_ toolchain) with some limitations:

- _threads.h_: support is not yet implemented, blocking primitives like signals and semaphores doesn't seem to work on this platform. Support maybe added in future.
- _fibers.h_: Emscripten doesn't seem to support boost's assembly fibers which I'm currently using, however it is possible to implement async functions using emscripten API, which I'll try to implement in the future.
- _virtual-alloc.h_: Virtual memory allocation functions does not seem to be working, it works like normal malloc, where reserving just pre-allocates all required memory


- __glew__ _(libglew-dev)_: If you are planning to use graphics (OpenGL) module, else use SX_NO_GFX flag in cmake options

[License (BSD 2-clause)](https://github.com/septag/sx/blob/master/LICENSE)
--------------------------------------------------------------------------

<a href="http://opensource.org/licenses/BSD-2-Clause" target="_blank">
<img align="right" src="http://opensource.org/trademarks/opensource/OSI-Approved-License-100x137.png">
</a>

	Copyright 2018 Sepehr Taghdisian. All rights reserved.
	
	https://github.com/septag/sx
	
	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions are met:
	
	   1. Redistributions of source code must retain the above copyright notice,
	      this list of conditions and the following disclaimer.
	
	   2. Redistributions in binary form must reproduce the above copyright notice,
	      this list of conditions and the following disclaimer in the documentation
	      and/or other materials provided with the distribution.
	
	THIS SOFTWARE IS PROVIDED BY COPYRIGHT HOLDER ``AS IS'' AND ANY EXPRESS OR
	IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
	MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
	EVENT SHALL COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
	INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
	BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
	DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
	LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
	OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
	ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

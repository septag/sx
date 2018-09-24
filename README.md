[![Build Status](https://travis-ci.org/septag/sx.svg?branch=master)](https://travis-ci.org/septag/sx)

# SX: Portable base library for C/C++ programs and games
[@septag](https://twitter.com/septagh)

SX is merely a collection of sources and 3rdparty libraries. It is written in C and can be compiled using C++ compiler like MSVC. 
Designed for simplicity and performance, It is tailored more towards system and game programmers.  
Inspired by the works of [Sean Barret](https://github.com/nothings), [Branimir Karadric](https://github.com/bkaradzic), [Andre Weissflog](https://github.com/floooh), [Mathias Gustavsson](https://github.com/mattiasgustavsson), [r-lyeh](https://github.com/r-lyeh), [ocornut](https://github.com/ocornut), I decided to make my own C base library for future projects. I also used and borrowed many of their code inside SX.

This library currently contains these functionalities (by header file):

- *allocator.h:* basic memory allocation functions and default heap/leak_check allocators. Allocators are used anywhere in the code that needs memory allocation/deallocation. There is no direct memory allocations like _malloc_ inside any function.
- *array.h:* [stretchy_buffer](https://github.com/nothings/stb/blob/master/stretchy_buffer.h) implementation
- *atomic.h:* Set of portable atomic types and functions like CAS/Exchange/Incr/...
- *cmdline.h:* [getopt](https://github.com/wc-duck/getopt) command line parser wrapper
- *fiber.h:* Portable fibers/coroutines, backend implementation taken from my other _de-boostified_ project [deboost.context](https://github.com/septag/deboost.context)
	- Includes low-level functions for fibers (_deboostified_ boost.context)
	- Higher level fiber context (fiber-pool) that emulates a smiliar behaviour to unity's coroutines
- *jobs.h:* Fiber based task scheduler. Proof of concept for [Christian Gyrling 2015 GDC Talk](http://gdcvault.com/play/1022186/Parallelizing-the-Naughty-Dog-Engine)
	- Very fast low-level (asm cpu state switch) fibers
	- Wait on job dependencies without task chains or context switches
	- Fixed thread pool
	- Job execution priority support
- *handlepool.h:* Handle pool
- *hash.h:*  Some nice hash functions and a fast fibonacci multiplicative hash-table
- *ini.h:* INI file encoder/decoder. It's [ini.h](https://github.com/mattiasgustavsson/libs/blob/master/ini.h) wrapper for sx
- *io.h:* Input/output to memory/file IO
- *lin-alloc.h:* Generic linear allocator
- *platform.h:* Platform detection macros, taken from [bx](https://github.com/bkaradzic/bx)
- *pool.h:* Fixed size pool allocator
- *rng.h:* PCG random number generator
- *stack-alloc.h:* Generic linear stack based allocator
- *string.h:* Useful C-style string functions
- *threads.h:* Portable threading primitives (Thread, Tls, Mutex, Semaphore, Signal, SpScQueue)
- *timer.h:* Portable high-res timer, wrapper for [sokol_time](https://github.com/floooh/sokol)
- *virtual-alloc.h:* Portable virtual memory allocator and functions
- *math.h:* 
	- Vector(2,3,4), Matrix (3x3, 4x4), Quaternion math
	- Easing functions
- *os.h:* Common portable OS related routines
- *bheap.h:* Binary heap implementation
- *tlsf-alloc.h:* Tlsf (Two-Level Segregated Fit memory) memory allocator

## Build
### Current supported platforms

- *Windows*: Tested on vs2015-cl (cpp) and vs2015-clang_c2 (c11) - x86_64
- *Linux*: Tested on ubuntu, gcc 4.8.4
- *OSX*: Tested on x86_64 Darwin 17.4.0 - LLVM version 9.1.0 (clang-902.0.39.1)
- *Emscripten*: Tested on v1.38.8

### CMake options

These are general options for cmake, where you can trim or customize the build:  

- **SX_BUILD_TESTS** (Default=1): Set SX_BUILD_TESTS=0 to skip building test applications

These are also the macros that you can override in _config.h_ or add them to compile definitions:

- **SX_DEBUG** (Default=0): Forces debug compilation, defaults to 1 if _DEBUG (debug build) is set
- **SX_CONFIG_DEBUG_ALLOCATOR** (Default=0): Allocations include debug information like filename and line numbers
- **SX_CONFIG_ALLOCATOR_NATURAL_ALIGNMENT** (Default=8): All memory allocators aligns pointers to this value if 'align' parameter is less than natural alignment
- **SX_CONFIG_HASHTBL_DEBUG** (Default=0): Inserts code for hash-table debugging, used only for efficiency tests, see hash.h
- **SX_CONFIG_STDMATH** (Default=1): Uses stdc's math library (libm) for basic math functions. Set this to 0 if you want the library use it's own base math functions and drop libm dependency.
- **sx_out_of_memory**: What should the program do if some internal memory allocations fail. see _config.h_ for default implementation
- **sx_data_truncate**: What should the program do if IO operations get truncated and goes out of bound. see _config.h_ for default implementation
- **sx_assert**: Assert replacement, default is clib's _assert_
- **sx_memset**: Memory set replacement, default is clib's _memset_
- **sx_memcpy**: Memory copy replacement, default is clib's _memcpy_
- **sx_memcmp**: Memory compare replacement, default is clib's _memcmp_
- **sx_memmove**: Memory move replacement, default is clib's _memmove_

### Windows
Some C feautures on MSVC's C-compiler is missing, the code can be compiled as cpp also (/TP). It's also compatible with *MSVC + clang_c2* toolset which in that case you have to add the ```-T v140_clang_c2``` switch to cmake command (vs2015)

```
cd build
cmake .. -G "Visual Studio 14 2015 Win64"
```

### Emscripten

It can be built on emscripten (using the _Emscripten.cmake_ toolchain) with some limitations:

- _threads.h_: support is not yet implemented, blocking primitives like signals and semaphores doesn't seem to work on this platform. Support maybe added in future.
- _fibers.h_: Emscripten doesn't seem to support boost's assembly fibers which I'm currently using, however it is possible to implement async functions using emscripten API, which I'll try to implement in the future.
- _virtual-alloc.h_: Virtual memory allocation functions does not seem to be working, it works like normal malloc, where reserving just pre-allocates all required memory


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

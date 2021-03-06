[![Build Status](https://travis-ci.org/septag/sx.svg?branch=master)](https://travis-ci.org/septag/sx)

# SX: Portable base library for C/C++ programs and games
[@septag](https://twitter.com/septagh)

SX is a minimal and performant base C library, runs on different platforms and OSes. Designed to help C developers (and C++ programmers who doesn't like to use "modernized" and templated C++ libs like _stl_ and _boost_) with their multiplatform programs and games.

Inspired by the works of [Sean Barret](https://github.com/nothings), [Branimir Karadric](https://github.com/bkaradzic), [Andre Weissflog](https://github.com/floooh), [Mathias Gustavsson](https://github.com/mattiasgustavsson), [r-lyeh](https://github.com/r-lyeh), [ocornut](https://github.com/ocornut) and of course [the machinery](https://www.ourmachinery.com), I decided to make my own C base library for future projects. I also used and "borrowed" some of their code for SX.

- The API is simple and portable across many other languages
- Heap memory is under control: Almost every entity that allocates from heap requires `allocator` object and stdc's `malloc` and `free` are not used anywhere else.
- Runs on multiple platforms
- Performant. Uses fast algorithms and implementations (some assembly) with plain C-structs and functions. And unlike libraries like `stl`, performance is not much different in debug builds. 
- Fast compile time: It takes about 1 sec to build the lib (it's about 15kloc with 3rdparty sources) on my windows machine and vc14 (cl.exe) compiler. (i7-2nd gen CPU + ninja build). Also headers include minimal headers and system libs for faster compile times in programs that use them.

This library currently contains these functionalities (listed by header files):

- [allocator.h](include/sx/allocator.h): basic memory allocation functions and default heap/leak_check allocators. 
- [array.h](include/sx/array.h): [stretchy_buffer](https://github.com/nothings/stb/blob/master/stretchy_buffer.h) implementation. Also contains a very thin C++ template wrapper over array macros for C++ users.
- [atomic.h](include/sx/atomic.h): Set of portable atomic types and functions like CAS/Exchange/Incr/etc. plus a minimal spinlock implementation.
- [cmdline.h](include/sx/cmdline.h): wrapper over [getopt](https://github.com/wc-duck/getopt) - getopt command line parser
- [fiber.h](include/sx/fiber.h): Portable fibers and coroutines, Assembly backend implementation taken from _de-boostified_ project [deboost.context](https://github.com/septag/deboost.context)
	- Includes low-level functions for fibers (_deboostified_ boost.context)
	- Higher level coroutines (fiber-pool with streamlined coroutine update) that emulates a smiliar behaviour to unity's coroutines
- [jobs.h](include/sx/jobs.h): Fiber based task scheduler. Proof of concept for [Christian Gyrling 2015 GDC Talk](http://gdcvault.com/play/1022186/Parallelizing-the-Naughty-Dog-Engine)
	- Very fast low-level (assembly cpu state switch) fibers
	- Wait on job dependencies without task chains or context switches
	- Fixed thread pool
	- Job priority support
	- Overriadable thread init and shutdown. To initialize your own stuff on each thread
	- Support for tags: each worker thread can be tagged to handle specific class of jobs
- [handle.h](include/sx/handle.h): Handle pool. sparse/dense handle allocator to address array items with handles instead of pointers. With generation counters for validating dead handles.
- [hash.h](include/sx/hash.h):  Some nice hash functions (xxhash/crc32/fnv1a) and a fast fibonacci multiplicative hash-table. Contains a very thin C++ template wrapper for sx_hashtbltval.
- [ini.h](include/sx/ini.h): INI file encoder/decoder. wrapper over [ini.h](https://github.com/mattiasgustavsson/libs/blob/master/ini.h)
- [io.h](include/sx/io.h): Read and write to/from memory and file streams
- [lin-alloc.h](include/sx/lin-alloc.h): Generic linear allocator
- [platform.h](include/sx/platform.h): Platform and compiler detection macros, taken from [bx](https://github.com/bkaradzic/bx)
- [pool.h](include/sx/pool.h): Self-contained pool allocator
- [rng.h](include/sx/rng.h): Random number generators. Currently only implementation is PCG.
- [string.h](include/sx/string.h): Useful C-style string functions including Sean barret's [stb_printf](http://github.com/nothings/stb) implementation. Plus string pool implementation from [mattias](https://github.com/mattiasgustavsson/libs/blob/master/strpool.h)
- [threads.h](include/sx/threads.h): Portable threading primitives:
	- Thread
	- Tls (Thread local storage)
	- Mutex
	- Semaphore
	- Signal
- [timer.h](include/sx/timer.h): Portable high-res timer, wrapper over [sokol_time](https://github.com/floooh/sokol)
- [vmem.h](include/sx/vmem.h): Page based virtual memory allocator
- [math.h](include/sx/math.h): The math library is divided into multiple parts, the main typedefs are in _math-types.h_
	- Standard floating-point and constants: _math-scalar.h_
	- Vector (2,3,4): _math-vec.h_
	- Matrix (3x3, 4x4): _math-vec.h_
	- Quaternion: _math-vec.h_
	- Easing functions: _math-easing.h_
	- AABB: _math-vec.h_
	- Color (RGBA 4 unsigned bytes): _math-vec.h_
	- Cpp operator overrides: _math-vec.h_
- [os.h](include/sx/os.h): Common portable OS related routines
	- Basic system information like available memory and cpu cores count
	- Shared library management
	- Basic file operations
	- Path manipulation functions (c-string)
- [bheap.h](include/sx/bheap.h): Binary heap implementation
- [simd.h](include/sx/simd.h): portable 128bit simd math intrinsics. currently there are two implementations: reference and SSE. ARM Neon will be added soon.
- [ringbuffer.h](include/sx/ringbuffer.h): Basic ring-buffer (circular buffer)
- [lockless.h](include/sx/lockless.h): lockless data structures. 
  - Self-contained single-producer-single-consumer queue
- [linear-buffer.h](include/sx/linear-buffer.h): Helper custom memory allocator useful for allocating structures with arrays of data in a single allocation call. Includes a thin C++ template over it's C-API.
- [bitarray.h](include/sx/bitarray.h): utility data structure to hold arbitary number of bits

## Changes
### v1.1.0 (Nov 2020)
- Divided math lib into multiple headers. _math.h_ still contains all of the math lib, but to improve build times, you can use _math-types.h_ in your headers and include their specific headers in sources.
- MinGW build support
- Faster Sqrt/rsqrt on CPUs with SSE2
- Many math improvements and fixes
- Assert improvements. Besides `sx_assert`, you have `sx_assertf` and `sx_assert_alwaysf` to output (potentially log) error messages with them.
- Memory allocation fail callback: Added `sx_mem_fail_cb` callback registration to override out-of-memory errors
- [BREAKING] IFF file writer/reader improvements and API changes (see io.h)
- _addref_ for memory blocks to add reference count
- [BREAKING] Removed stack-allocator. Use linear-allocator instead
- Fixed minor bug in jobs.h for job-selector
- Better inline implementations. Now many math functions are FORCE_INLINE, you can also use `/Ob1` flag in msvc + DEBUG builds  and set `SX_CONFIG_FORCE_INLINE_DEBUG=1` to force those functions to be inlined and leave others be. Very useful for improving debug runtime performance.

### v1.0.0 (May 2020)
- [BREAKING]: Removed `virtual-alloc.h` and replaced it with the new `vmem.h`. virtual-alloc was not Suitable for general memory allocator. So I ditched it completely and replaced with a cleaner, better virtual memory allocation scheme. which is page based
- [BREAKING]: Removed `tlsf-alloc` and all of it's dependencies. In order to cut the maintainable code and reduce repo size. I decided to remove this and leave it for the user to use the open-source [tlsf allocator](https://github.com/mattconte/tlsf). 

## Build
### Current supported platforms

- **Windows**: Tested on vs2015-cl (cpp) and vs2015-clang_c2 (c11) - x86_64
- **Linux**: Tested on ubuntu, gcc 4.8.4
- **OSX**: Tested on x86_64 Darwin 17.4.0 - LLVM version 9.1.0 (clang-902.0.39.1)
- **Android**: Test with ndk v17.1.4828580
- **Emscripten**: Tested on v1.38.8 - currently with some restrictions. see below
- **RaspberryPI**: Tested on RaspberryPI (model B) - Ubuntu jessie. use `-DCMAKE_TOOLCHAIN_FILE=cmake/rpi.cmake`

### CMake options

These are general options for cmake, where you can trim or customize the build:  

- **SX_BUILD_TESTS** (Default=1): Set SX_BUILD_TESTS=0 to skip building test applications
- **SX_SHARED_LIB** (Default=0): Builds shared library (.dll/.so/.dylib) instead of static

These are also the macros that you can override in _config.h_ or add them to compile definitions:

- **SX_DEBUG** (Default=0): Forces debug compilation, defaults to 1 if _DEBUG (debug build) is set
- **SX_CONFIG_DEBUG_ALLOCATOR** (Default=0): Allocations include debug information like filename and line numbers
- **SX_CONFIG_ALLOCATOR_NATURAL_ALIGNMENT** (Default=8): All memory allocators aligns pointers to this value if 'align' parameter in alloc functions is less than natural alignment
- **SX_CONFIG_HASHTBL_DEBUG** (Default=0): Inserts code for hash-table debugging, used only for efficiency tests, see hash.h
- **SX_CONFIG_STDMATH** (Default=1): Uses stdc's math library (libm) for basic math functions. Set this to 0 if you want the library use it's own base math functions and drop _libm_ dependency.
- **SX_CONFIG_SIMD_DISABLE** (Default=0): Disables platform-specific simd functions and forces them to use fpu reference functions instead.
- **sx_out_of_memory**: What should the program do if some internal memory allocations fail. see _allocator.h_ for default implementation
- **sx_data_truncate**: What should the program do if IO operations get truncated and goes out of bound. see _io.h_ for default implementation
- **sx_assert**: Assert replacement, default is clib's _assert_
- **sx_memset**: Memory set replacement, default is clib's _memset_
- **sx_memcpy**: Memory copy replacement, default is clib's _memcpy_
- **sx_memcmp**: Memory compare replacement, default is clib's _memcmp_
- **sx_memmove**: Memory move replacement, default is clib's _memmove_

### Emscripten

It can be built on emscripten (using the _Emscripten.cmake_ toolchain) with some limitations:

- _threads.h_: support is not yet implemented, blocking primitives like signals and semaphores doesn't seem to work on this platform. Support maybe added in future.
- _fibers.h_: Emscripten doesn't seem to support boost's assembly fibers which I'm currently using, however it is possible to implement async functions using emscripten API, which I'll try to implement in the future.
- _virtual-alloc.h_: Virtual memory allocation functions does not seem to be working, it works like normal malloc, where reserving just pre-allocates all required memory

## Links
Here are some other mini C system libraries (mostly single headers) that you might find useful:

- [sokol](https://github.com/floooh/sokol): Simple STB-style cross-platform libraries for C and C++, written in C. (graphics, sound, application, etc)
- [cj5](https://github.com/septag/cj5): Very minimal single header JSON5 parser in C99, derived from jsmn
- [dmon](https://github.com/septag/dmon): dmon is a tiny C library that monitors changes in a directory. It provides a unified solution to multiple system APIs that exist for each OS. It can also monitor directories recursively.
- [dds-ktx](https://github.com/septag/dds-ktx): Portable single header DDS/KTX reader for C/C++
- [cr](https://github.com/fungos/cr): A single file header-only live reload solution for C
- [stackwalkerc](https://github.com/septag/stackwalkerc): Windows single header stack walker in C 
- [sjson](https://github.com/septag/sjson): Fast and portable C single header json Encoder/Decoder


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

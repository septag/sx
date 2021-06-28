<h4 align="center">C89 compatible atomics.</h4>

<p align="center">
    <a href="https://discord.gg/9vpqbjU"><img src="https://img.shields.io/discord/712952679415939085?label=discord&logo=discord" alt="discord"></a>
    <a href="https://twitter.com/mackron"><img src="https://img.shields.io/twitter/follow/mackron?style=flat&label=twitter&color=1da1f2&logo=twitter" alt="twitter"></a>
</p>

This library aims to implement an equivalent to the C11 atomics library. It's intended to be used as a way to
enable the use of atomics in a mostly consistent manner to modern C, while still enabling compatibility with
older compilers. This is *not* a drop-in replacement for C11 atomics, but is very similar. Only limited testing
has been done so use at your own risk. I'm happy to accept feedback and pull requests with bug fixes.

When compiling with GCC and Clang, this library will translate to a one-to-one wrapper around the `__atomic_*`
intrinsics provided by the compiler.

When compiling with Visual C++ things are a bit more complicated because it does not have support for C11 style
atomic intrinsics. This library will try to use the `_Interlocked*` intrinsics instead, and if unavailable will
use inlined assembly (x86 only).

Supported compilers are Visual Studio back to VC6, GCC and Clang. If you need support for a different compiler
I'm happy to add support (pull requests appreciated). This library currently assumes the `int` data type is
32 bits.


Usage
-----
This is a header-only library. Just add `c89atomic.h` to your source tree and include it:

```c
#include "c89atomic.h"
```

Everything is implemented with defines and inline functions. There are no source files or build scripts to
deal with.


Differences With C11
--------------------
For practicality, this is not a drop-in replacement for C11's `stdatomic.h`. Below are the main differences
between c89atomic and stdatomic.

  * All operations require an explicit size which is specified by the name of the function, and only 8-,
    16-, 32- and 64-bit operations are supported. Objects of an arbitrary sizes are not supported.
  * Some extra APIs are included:
    - `c89atomic_compare_and_swap_*()`
    - `c89atomic_test_and_set_*()`
    - `c89atomic_clear_*()`
  * All APIs are namespaced with `c89`.
  * `c89atomic_*` data types are undecorated.


Compare Exchange
----------------
This library implements a simple compare-and-swap function called `c89atomic_compare_and_swap_*()` which is
slightly different to C11's `atomic_compare_exchange_*()`. This is named differently to distinguish between
the two. `c89atomic_compare_and_swap_*()` returns the old value as the return value, whereas
`atomic_compare_exchange_*()` will return it through a parameter and supports explicit memory orders for
success and failure cases.

With Visual Studio and versions of GCC earlier than 4.7, an implementation of `atomic_compare_exchange_*()`
is included which is implemented in terms of `c89atomic_compare_and_swap_*()`, but there's subtle details to be
aware of with this implementation. Note that the following only applies for Visual Studio and versions of GCC
earlier than 4.7. Later versions of GCC and Clang use the `__atomic_compare_exchange_n()` intrinsic directly
and are not subject to the following.

Below is the 32-bit implementation of `c89atomic_compare_exchange_strong_explicit_32()` which is implemented
the same for the weak versions and other sizes, and only for Visual Studio and old versions of GCC (prior to
4.7):

```c
c89atomic_bool c89atomic_compare_exchange_strong_explicit_32(volatile c89atomic_uint32* dst,
                                                             volatile c89atomic_uint32* expected,
                                                             c89atomic_uint32 desired,
                                                             c89atomic_memory_order successOrder,
                                                             c89atomic_memory_order failureOrder)
{
    c89atomic_uint32 expectedValue;
    c89atomic_uint32 result;

    expectedValue = c89atomic_load_explicit_32(expected, c89atomic_memory_order_seq_cst);
    result = c89atomic_compare_and_swap_32(dst, expectedValue, desired);
    if (result == expectedValue) {
        return 1;
    } else {
        c89atomic_store_explicit_32(expected, result, failureOrder);
        return 0;
    }
}
```

The call to `c89atomic_store_explicit_32()` is not atomic with respect to the main compare-and-swap operation
which may cause problems when `expected` points to memory that is shared between threads. This only becomes an
issue if `expected` can be accessed from multiple threads at the same time which for the most part will never
happen because a compare-and-swap will almost always be used in a loop with a local variable being used for the
expected value.

If the above is a concern, you should consider reworking your code to use `c89atomic_compare_and_swap_*()`
directly, which is atomic and more efficient. Alternatively you'll need to use a lock to synchronize access
to `expected`, upgrade your compiler, or use a different library.



Types and Functions
-------------------
The following types and functions are implemented:
```
+-----------------------------------------+-----------------------------------------------+
| C11 Atomics                             | C89 Atomics                                   |
+-----------------------------------------+-----------------------------------------------+
| #include <stdatomic.h>                  | #include "c89atomic.h"                        |
+-----------------------------------------+-----------------------------------------------+
| memory_order                            | c89atomic_memory_order                        |
|     memory_order_relaxed                |     c89atomic_memory_order_relaxed            |
|     memory_order_consume                |     c89atomic_memory_order_consume            |
|     memory_order_acquire                |     c89atomic_memory_order_acquire            |
|     memory_order_release                |     c89atomic_memory_order_release            |
|     memory_order_acq_rel                |     c89atomic_memory_order_acq_rel            |
|     memory_order_seq_cst                |     c89atomic_memory_order_seq_cst            |
+-----------------------------------------+-----------------------------------------------+
| atomic_flag                             | c89atomic_flag                                |
| atomic_bool                             | c89atomic_bool                                |
| atomic_int8                             | c89atomic_int8                                |
| atomic_uint8                            | c89atomic_uint8                               |
| atomic_int16                            | c89atomic_int16                               |
| atomic_uint16                           | c89atomic_uint16                              |
| atomic_int32                            | c89atomic_int32                               |
| atomic_uint32                           | c89atomic_uint32                              |
| atomic_int64                            | c89atomic_int64                               |
| atomic_uint64                           | c89atomic_uint64                              |
+-----------------------------------------+-----------------------------------------------+
| atomic_flag_test_and_set                | c89atomic_flag_test_and_set                   |
| atomic_flag_test_and_set_explicit       | c89atomic_flag_test_and_set_explicit          |
|                                         | c89atomic_test_and_set_8                      |
|                                         | c89atomic_test_and_set_16                     |
|                                         | c89atomic_test_and_set_32                     |
|                                         | c89atomic_test_and_set_64                     |
|                                         | c89atomic_test_and_set_explicit_8             |
|                                         | c89atomic_test_and_set_explicit_16            |
|                                         | c89atomic_test_and_set_explicit_32            |
|                                         | c89atomic_test_and_set_explicit_64            |
+-----------------------------------------+-----------------------------------------------+
| atomic_flag_clear                       | c89atomic_flag_clear                          |
| atomic_flag_clear_explicit              | c89atomic_flag_clear_explicit                 |
|                                         | c89atomic_clear_8                             |
|                                         | c89atomic_clear_16                            |
|                                         | c89atomic_clear_32                            |
|                                         | c89atomic_clear_64                            |
|                                         | c89atomic_clear_explicit_8                    |
|                                         | c89atomic_clear_explicit_16                   |
|                                         | c89atomic_clear_explicit_32                   |
|                                         | c89atomic_clear_explicit_64                   |
+-----------------------------------------+-----------------------------------------------+
| atomic_store                            | c89atomic_store_8                             |
| atomic_store_explicit                   | c89atomic_store_16                            |
|                                         | c89atomic_store_32                            |
|                                         | c89atomic_store_64                            |
|                                         | c89atomic_store_explicit_8                    |
|                                         | c89atomic_store_explicit_16                   |
|                                         | c89atomic_store_explicit_32                   |
|                                         | c89atomic_store_explicit_64                   |
+-----------------------------------------+-----------------------------------------------+
| atomic_load                             | c89atomic_load_8                              |
| atomic_load_explicit                    | c89atomic_load_16                             |
|                                         | c89atomic_load_32                             |
|                                         | c89atomic_load_64                             |
|                                         | c89atomic_load_explicit_8                     |
|                                         | c89atomic_load_explicit_16                    |
|                                         | c89atomic_load_explicit_32                    |
|                                         | c89atomic_load_explicit_64                    |
+-----------------------------------------+-----------------------------------------------+
| atomic_exchange                         | c89atomic_exchange_8                          |
| atomic_exchange_explicit                | c89atomic_exchange_16                         |
|                                         | c89atomic_exchange_32                         |
|                                         | c89atomic_exchange_64                         |
|                                         | c89atomic_exchange_explicit_8                 |
|                                         | c89atomic_exchange_explicit_16                |
|                                         | c89atomic_exchange_explicit_32                |
|                                         | c89atomic_exchange_explicit_64                |
+-----------------------------------------+-----------------------------------------------+
| atomic_compare_exchange_weak            | c89atomic_compare_exchange_weak_8             |
| atomic_compare_exchange_weak_explicit   | c89atomic_compare_exchange_weak_16            |
| atomic_compare_exchange_strong          | c89atomic_compare_exchange_weak_32            |
| atomic_compare_exchange_strong_explicit | c89atomic_compare_exchange_weak_64            |
|                                         | c89atomic_compare_exchange_weak_explicit_8    |
|                                         | c89atomic_compare_exchange_weak_explicit_16   |
|                                         | c89atomic_compare_exchange_weak_explicit_32   |
|                                         | c89atomic_compare_exchange_weak_explicit_64   |
|                                         | c89atomic_compare_exchange_strong_8           |
|                                         | c89atomic_compare_exchange_strong_16          |
|                                         | c89atomic_compare_exchange_strong_32          |
|                                         | c89atomic_compare_exchange_strong_64          |
|                                         | c89atomic_compare_exchange_strong_explicit_8  |
|                                         | c89atomic_compare_exchange_strong_explicit_16 |
|                                         | c89atomic_compare_exchange_strong_explicit_32 |
|                                         | c89atomic_compare_exchange_strong_explicit_64 |
+-----------------------------------------+-----------------------------------------------+
| atomic_fetch_add                        | c89atomic_fetch_add_8                         |
| atomic_fetch_add_explicit               | c89atomic_fetch_add_16                        |
|                                         | c89atomic_fetch_add_32                        |
|                                         | c89atomic_fetch_add_64                        |
|                                         | c89atomic_fetch_add_explicit_8                |
|                                         | c89atomic_fetch_add_explicit_16               |
|                                         | c89atomic_fetch_add_explicit_32               |
|                                         | c89atomic_fetch_add_explicit_64               |
+-----------------------------------------+-----------------------------------------------+
| atomic_fetch_sub                        | c89atomic_fetch_sub_8                         |
| atomic_fetch_sub_explicit               | c89atomic_fetch_sub_16                        |
|                                         | c89atomic_fetch_sub_32                        |
|                                         | c89atomic_fetch_sub_64                        |
|                                         | c89atomic_fetch_sub_explicit_8                |
|                                         | c89atomic_fetch_sub_explicit_16               |
|                                         | c89atomic_fetch_sub_explicit_32               |
|                                         | c89atomic_fetch_sub_explicit_64               |
+-----------------------------------------+-----------------------------------------------+
| atomic_fetch_or                         | c89atomic_fetch_or_8                          |
| atomic_fetch_or_explicit                | c89atomic_fetch_or_16                         |
|                                         | c89atomic_fetch_or_32                         |
|                                         | c89atomic_fetch_or_64                         |
|                                         | c89atomic_fetch_or_explicit_8                 |
|                                         | c89atomic_fetch_or_explicit_16                |
|                                         | c89atomic_fetch_or_explicit_32                |
|                                         | c89atomic_fetch_or_explicit_64                |
+-----------------------------------------+-----------------------------------------------+
| atomic_fetch_xor                        | c89atomic_fetch_xor_8                         |
| atomic_fetch_xor_explicit               | c89atomic_fetch_xor_16                        |
|                                         | c89atomic_fetch_xor_32                        |
|                                         | c89atomic_fetch_xor_64                        |
|                                         | c89atomic_fetch_xor_explicit_8                |
|                                         | c89atomic_fetch_xor_explicit_16               |
|                                         | c89atomic_fetch_xor_explicit_32               |
|                                         | c89atomic_fetch_xor_explicit_64               |
+-----------------------------------------+-----------------------------------------------+
| atomic_fetch_and                        | c89atomic_fetch_and_8                         |
| atomic_fetch_and_explicit               | c89atomic_fetch_and_16                        |
|                                         | c89atomic_fetch_and_32                        |
|                                         | c89atomic_fetch_and_64                        |
|                                         | c89atomic_fetch_and_explicit_8                |
|                                         | c89atomic_fetch_and_explicit_16               |
|                                         | c89atomic_fetch_and_explicit_32               |
|                                         | c89atomic_fetch_and_explicit_64               |
+-----------------------------------------+-----------------------------------------------+
| atomic_thread_fence()                   | c89atomic_thread_fence                        |
| atomic_signal_fence()                   | c89atomic_signal_fence                        |
+-----------------------------------------+-----------------------------------------------+
| atomic_is_lock_free                     | c89atomic_is_lock_free_8                      |
|                                         | c89atomic_is_lock_free_16                     |
|                                         | c89atomic_is_lock_free_32                     |
|                                         | c89atomic_is_lock_free_64                     |
+-----------------------------------------+-----------------------------------------------+
| (Not Defined)                           | c89atomic_compare_and_swap_8                  |
|                                         | c89atomic_compare_and_swap_16                 |
|                                         | c89atomic_compare_and_swap_32                 |
|                                         | c89atomic_compare_and_swap_64                 |
|                                         | c89atomic_compare_and_swap_ptr                |
|                                         | c89atomic_compiler_fence                      |
+-----------------------------------------+-----------------------------------------------+
```
//
// Copyright 2018 Sepehr Taghdisian (septag@github). All rights reserved.
// License: https://github.com/septag/sx#license-bsd-2-clause
//
// fiber.h - v1.1 - High-performance (asm) portable fibers/coroutines
// Reference: https://github.com/boostorg/context
//            http://www.boost.org/doc/libs/1_60_0/libs/context/doc/html/index.html
//
// API is divided into two parts, low-level (sx_fiber_t) and high-level (sx_coro_context):
//
// Low-level API:
// This is the essential fiber API that is used by jobs.c for multi-threaded job dispatcher
// You can make stack memory and fiber object which holds cpu state and stack pointer
//
//      sx_fiber_stack_init     create fiber stack from virtual memory
//                              size will be aligned to OS page size
//      sx_fiber_stack_release  releases virtual memory allocated by stack
//      sx_fiber_stack_init_ptr initializes stack object without allocating any memory
//                              this function is useful for allocating virtual memory
//                              (see virtual-alloc.h) yourself and pass the pointer to this function
//      sx_fiber_stack          fiber_stack object, must be initialized by 'sx_fiber_stack_init' or
//                              'sx_fiber_stack_init_ptr'
//
//      sx_fiber_create         creates a new OS fiber object with already intiaized stack object
//                              and a fiber callback.
//                              Returns sx_fiber object that can be switch via 'sx_fiber_switch'
//      sx_fiber_switch         Switches to fiber and runs/continues it's function with a user
//      pointer
//                              Returns sx_fiber_transfer from the fiber that is first switched to.
//                              see example
//
// Example:
//      void fiber1_fn(sx_fiber_transfer transfer)  {
//          puts("Fiber1");
//          sx_fiber_switch(transfer.from, transfer.user);  // Get back to where fiber1 is switched
//          puts("Fiber1 - End");
//          sx_fiber_switch(transfer.from, transfer.user);  // Always put this to return to caller
//                                                             at the end of the callback
//      }
//
//      sx_fiber_stack stack1;
//      sx_fiber_stack_init(&stack1, 80*1024);
//      puts("Main");
//      sx_fiber_t fiber = sx_fiber_create(stack1, fiber1_fn);
//      sx_fiber_transfer t = sx_fiber_switch(fiber, NULL);
//      puts("Back to main");
//      sx_fiber_switch(t.from, t.user);
//      puts("End");
//
// Output:
//     Main
//     Fiber1
//     Back to Main
//     Fiber1 - End
//     End
//
// High-level coroutine API:
// This is a more practical API for gameplay programmers, it emulates somewhat unity's coroutines
// Facilitates iterative algorithms and wait timers, ...
// You create a coroutine context, which is actually a fiber pool. On each frame of the game you
// update the context. So when you invoke a coroutine, it will run it like a normal function,
// but you can return in the middle of the it and continue on some other time.
// NOTE that the context API is not thread-safe, so the context-related functions must be called
// within one thread only
//
//      sx_coro_create_context     creates fiber context (fiber pool)
//                                  max_fibers is the maximum number of fibers in the pool
//                                  (actual count of running 'invokes')
//                                  stack_sz is the size of fiber stack is bytes,
//                                  must be more than sx_os_minstacksz()
//      sx_coro_destroy_context    destroys fiber context
//
//  MACROS
//      sx_coro_declare            Declares a coroutine function. example:
//                                 sx_coro_declare(func_name) {}
//      sx_coro_userdata           call this inside coroutine to get `user` pointer passed by invoke
//      sx_coro_invoke             Invokes the coroutine defined by `sx_coro_declare`
//      sx_coro_yield              yields the current coroutine and gets back to it on next update
//      sx_coro_wait               yields the current coroutine and waits for `msecs` milliseconds
//                                 then gets back to the coroutine
//      sx_coro_yieldn             yeilds current coroutine and gets back to it after N updates
//      sx_coro_update             Updates fiber-context state with a delta-time as input.
//                                 In the game this should be called on each frame
//      sx_coro_end                Exits the fiber execution and returns to program,
//                                 This function MUST be called whenever you want to exit the coro
// Example:
//
//        sx_coro_declare(my_test) {
//            sx_coro_context* cctx = sx_coro_userdata();
//            puts("gonna wait for 1 sec");
//            sx_coro_wait(cctx, 1000);
//            puts("gonna continue on next update");
//            sx_coro_yield(cctx);
//            puts("gonna continue on next 3 updates");
//            sx_coro_yieldn(cctx, 3);
//            puts("bye bye");
//            sx_coro_end(cctx);
//        }
//
//        void test_coro() {
//            sx_coro_context* cctx = sx_coro_create_context(sx_alloc_malloc, 60, 1024*1024);
//            sx_assert(cctx);
//            sx_coro_invoke(my_test, cctx);
//            while (1) {
//                sx_coro_update(cctx, delta_time);
//            }
//            sx_coro_destroy_context(cctx, sx_alloc_malloc);
//        }
//
#pragma once

#include "macros.h"

#include <stdbool.h>

typedef struct sx_alloc sx_alloc;

#define SX_FIBER_INVALID NULL

typedef void* sx_fiber_t;

typedef struct sx_fiber_transfer {
    sx_fiber_t from;
    void*      user;
} sx_fiber_transfer;

typedef struct sx_fiber_stack {
    void*        sptr;
    unsigned int ssize;
} sx_fiber_stack;

typedef void(sx_fiber_cb)(sx_fiber_transfer transfer);

// High level context API
typedef struct sx_coro_context sx_coro_context;

SX_API sx_coro_context* sx_coro_create_context(const sx_alloc* alloc, int max_fibers, int stack_sz);
SX_API void             sx_coro_destroy_context(sx_coro_context* ctx, const sx_alloc* alloc);
SX_API void             sx_coro_update(sx_coro_context* ctx, float dt);
SX_API bool             sx_coro_replace_callback(sx_coro_context* ctx, sx_fiber_cb* callback,
                                                 sx_fiber_cb* new_callback);

SX_API void sx__coro_invoke(sx_coro_context* ctx, sx_fiber_cb* callback, void* user);
SX_API void sx__coro_end(sx_coro_context* ctx, sx_fiber_t* pfrom);
SX_API void sx__coro_wait(sx_coro_context* ctx, sx_fiber_t* pfrom, int msecs);
SX_API void sx__coro_yield(sx_coro_context* ctx, sx_fiber_t* pfrom, int nupdates sx_default(1));

// coroutines macros (use these instead of above sx__coro functions)
#define sx_coro_declare(_name) static void coro__##_name(sx_fiber_transfer __transfer)
#define sx_coro_userdata() __transfer.user
#define sx_coro_end(_ctx) sx__coro_end((_ctx), &__transfer.from)
#define sx_coro_wait(_ctx, _msecs) sx__coro_wait((_ctx), &__transfer.from, (_msecs))
#define sx_coro_yield(_ctx) sx__coro_yield((_ctx), &__transfer.from, 1)
#define sx_coro_yieldn(_ctx, _n) sx__coro_yield((_ctx), &__transfer.from, (_n))
#define sx_coro_invoke(_ctx, _name, _user) sx__coro_invoke((_ctx), coro__##_name, (_user))

// Low-level functions
SX_API bool sx_fiber_stack_init(sx_fiber_stack* fstack, unsigned int size sx_default(0));
SX_API void sx_fiber_stack_init_ptr(sx_fiber_stack* fstack, void* ptr, unsigned int size);
SX_API void sx_fiber_stack_release(sx_fiber_stack* fstack);

SX_API sx_fiber_t sx_fiber_create(const sx_fiber_stack stack, sx_fiber_cb* fiber_cb);
SX_API sx_fiber_transfer sx_fiber_switch(const sx_fiber_t to, void* user);

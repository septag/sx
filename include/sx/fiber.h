//
// Copyright 2018 Sepehr Taghdisian (septag@github). All rights reserved.
// License: https://github.com/septag/sx#license-bsd-2-clause
//
// fiber.h - v1.1 - High-performance (asm) portable fibers/coroutines
// Reference: https://github.com/boostorg/context
//            http://www.boost.org/doc/libs/1_60_0/libs/context/doc/html/index.html
//
// API is divided into two parts, low-level (sx_fiber_t) and high-level (sx_fiber_context):
//
// Low-level API:
// This is the essential fiber API that is used by jobs.c for multi-threaded job dispatcher
// You can make stack memory and fiber object which holds cpu state and stack pointer
//
//      sx_fiber_stack_init     create fiber stack from virtual memory
//                              size will be aligned to OS page size
//      sx_fiber_stack_release  releases virtual memory allocated by stack
//      sx_fiber_stack_init_ptr initializes stack object without allocating any memory
//                              this function is useful for allocating virtual memory (see virtual-alloc.h) your self
//                              and pass the pointer to this function
//      sx_fiber_stack          fiber_stack object, must be initialized by 'sx_fiber_stack_init' or 'sx_fiber_stack_init_ptr'
//
//      sx_fiber_create         creates a new OS fiber object with already intiaized stack object and a fiber callback
//                              Returns sx_fiber object that can be switch via 'sx_fiber_switch'
//      sx_fiber_switch         Switches to fiber and runs/continues it's function with a user pointer
//                              Returns sx_fiber_transfer from the fiber that is first switched to. see example
//
// Example:
//      void fiber1_fn(sx_fiber_transfer transfer)  {
//          puts("Fiber1");
//          sx_fiber_switch(transfer.from, transfer.user);    // Get back to where fiber1 is switched
//          puts("Fiber1 - End");
//          sx_fiber_switch(transfer.from, transfer.user);    // Always put this to return to caller at the end of the callback
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
// High-level API:
// This is a more practical API for gameplay programmers, it emulates somewhat unity's coroutines
// Facilitates iterative algorithms and wait timers, ...
// You create a fiber context, which is actually a fiber pool. On each frame of the game you update the context
// So when you invoke a fiber, it will run it like a normal function, but you can return in the middle of the function
// and continue on some other time based on 'sx_fiber_ret_type' enum value.
// Note that the context API is not thread-safe, so the context-related functions must be called within one thread only
//
//      sx_fiber_create_context     creates fiber context (fiber pool) 
//                                  max_fibers is the maximum number of fibers in the pool (actual count of running 'invokes')
//                                  stack_sz is the size of fiber stack is bytes, must be more than sx_os_minstacksz()
//      sx_fiber_destroy_context    destroys fiber context
//      sx_fiber_invoke             Invokes the fiber immediately and runs it's callback
//      sx_fiber_update             Updates fiber-context state with a delta-time as input. In the game this should be called on each frame
//      sx_fiber_return             Exits the fiber execution and returns to program, much like C's 'return' keyword,
//                                  This function must be called whenever you want to exit the fiber callback, even at the end of the function
//                                  'pfrom' is the caller fiber and gets updated, should be the pointer to the sx_fiber_t handle 'transfer.from' in the parameter of the callback
//                                  'sx_fiber_ret_type' indicates how should the fiber continue it's execution on the next updates
//                                      SX_FIBER_RET_FINISH indicates that execution is finished and fiber's work is done
//                                                          This can be called in the middle of fiber callback
//                                                          And must always be called at the end of the function body
//                                      SX_FIBER_RET_PASS   indices that execution must continue on next N frames given by 'arg' parameter
//                                      SX_FIBER_RET_WAIT   waits for N msecs and continue the execution, which is 'arg' paramater
//
#pragma once

#ifndef SX_FIBER_H_
#define SX_FIBER_H_

#include "allocator.h"

#define SX_FIBER_INVALID NULL

typedef void* sx_fiber_t;

typedef struct sx_fiber_transfer
{
    sx_fiber_t from;
    void*      user;
} sx_fiber_transfer;

typedef struct sx_fiber_stack
{
    void*   sptr;
    size_t  ssize;
} sx_fiber_stack;

typedef void (sx_fiber_cb)(sx_fiber_transfer transfer);

// High level context API
typedef struct sx_fiber_context sx_fiber_context;
typedef enum sx_fiber_ret_type
{
    SX_FIBER_RET_NONE = 0,
    SX_FIBER_RET_FINISH,        // Executation is finished
    SX_FIBER_RET_PASS,          // Pass this 'update' to the next N update which is 'arg' in sx_fiber_return
    SX_FIBER_RET_WAIT           // Wait for msecs: 'arg' is msecs in sx_fiber_return
} sx_fiber_ret_type;

#ifdef __cplusplus
extern "C" {
#endif

sx_fiber_context* sx_fiber_create_context(const sx_alloc* alloc, int max_fibers, int stack_sz);
void sx_fiber_destroy_context(sx_fiber_context* ctx, const sx_alloc* alloc);
void sx_fiber_invoke(sx_fiber_context* ctx, sx_fiber_cb callback, void* user);
void sx_fiber_update(sx_fiber_context* ctx, float dt);
void sx_fiber_return(sx_fiber_context* ctx, sx_fiber_t* pfrom, sx_fiber_ret_type type, int arg);

// Low-level functions
bool sx_fiber_stack_init(sx_fiber_stack* fstack, size_t size SX_DFLT(0));
void sx_fiber_stack_init_ptr(sx_fiber_stack* fstack, void* ptr, size_t size);
void sx_fiber_stack_release(sx_fiber_stack* fstack);

sx_fiber_t sx_fiber_create(const sx_fiber_stack stack, sx_fiber_cb* fiber_cb);
sx_fiber_transfer sx_fiber_switch(const sx_fiber_t to, void* user);

#ifdef __cplusplus
}
#endif

#endif // SX_FIBER_H_
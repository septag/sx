//
// Copyright 2018 Sepehr Taghdisian (septag@github). All rights reserved.
// License: https://github.com/septag/sx#license-bsd-2-clause
//
// fiber.h - v1.0 - Low-level, High-performance (asm) portable fibers
// Reference: https://github.com/boostorg/context
//            http://www.boost.org/doc/libs/1_60_0/libs/context/doc/html/index.html
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
#pragma once

#ifndef SX_FIBER_H_
#define SX_FIBER_H_

#include "sx.h"

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

SX_EXTERN bool sx_fiber_stack_init(sx_fiber_stack* fstack, size_t size SX_DFLT(0));
SX_EXTERN void sx_siber_stack_init_ptr(sx_fiber_stack* fstack, void* ptr, size_t size);
SX_EXTERN void sx_fiber_stack_release(sx_fiber_stack* fstack);

SX_EXTERN sx_fiber_t sx_fiber_create(const sx_fiber_stack stack, sx_fiber_cb* fiber_cb);
SX_EXTERN sx_fiber_transfer sx_fiber_switch(const sx_fiber_t to, void* user);

#endif // SX_FIBER_H_
//
// Copyright 2018 Sepehr Taghdisian (septag@github). All rights reserved.
// License: https://github.com/septag/sx#license-bsd-2-clause
//
#include "sx/fiber.h"
#include "sx/allocator.h"
#include "sx/os.h"
#include "sx/pool.h"

#include <stdlib.h>

// TODO: Implement coroutines (fibers) in emscripten
// http://kripken.github.io/emscripten-site/docs/api_reference/emscripten.h.html#c.emscripten_coroutine

#if SX_PLATFORM_WINDOWS
#    define VC_EXTRALEAN
#    define WIN32_LEAN_AND_MEAN
#    include <windows.h>
#elif SX_PLATFORM_POSIX
#    include <sys/mman.h>
#    include <unistd.h>
#    if defined(MAP_ANON) && !defined(MAP_ANONYMOUS)
#        define MAP_ANONYMOUS MAP_ANON
#    endif
#endif

#define DEFAULT_STACK_SIZE 131072    // 120kb

// Fwd declare ASM functions
SX_API sx_fiber_transfer jump_fcontext(sx_fiber_t const, void*);
SX_API sx_fiber_t make_fcontext(void*, size_t, sx_fiber_cb*);

bool sx_fiber_stack_init(sx_fiber_stack* fstack, unsigned int size) {
    if (size == 0)
        size = DEFAULT_STACK_SIZE;
    size = (uint32_t)sx_os_align_pagesz(size);
    void* ptr;

#if SX_PLATFORM_WINDOWS
    ptr = VirtualAlloc(NULL, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!ptr) {
        sx_out_of_memory();
        return false;
    }
    DWORD old_opts;
    VirtualProtect(ptr, sx_os_pagesz(), PAGE_READWRITE | PAGE_GUARD, &old_opts);
#elif SX_PLATFORM_POSIX
    ptr = mmap(0, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (ptr == MAP_FAILED) {
        sx_out_of_memory();
        return false;
    }
    mprotect(ptr, sx_os_pagesz(), PROT_NONE);
#else
    ptr = malloc(size);
    if (!ptr) {
        sx_out_of_memory();
        return false;
    }
#endif

    fstack->sptr = (uint8_t*)ptr + size;    // Move to end of the memory block for stack pointer
    fstack->ssize = size;
    return true;
}

void sx_fiber_stack_init_ptr(sx_fiber_stack* fstack, void* ptr, unsigned int size) {
    int page_sz = sx_os_pagesz();
    sx_assert((uintptr_t)ptr % page_sz == 0 && "buffer size must be dividable to OS page size");
    sx_assert(size % page_sz == 0 && "buffer size must be dividable to OS page size");

    fstack->sptr = ptr;
    fstack->ssize = size;
}

void sx_fiber_stack_release(sx_fiber_stack* fstack) {
    sx_assert(fstack->sptr);
    void* ptr = (uint8_t*)fstack->sptr - fstack->ssize;

#if SX_PLATFORM_WINDOWS
    VirtualFree(ptr, 0, MEM_RELEASE);
#elif SX_PLATFORM_POSIX
    munmap(ptr, fstack->ssize);
#else
    free(ptr);
#endif
}

sx_fiber_t sx_fiber_create(const sx_fiber_stack stack, sx_fiber_cb* fiber_cb) {
    return make_fcontext(stack.sptr, stack.ssize, fiber_cb);
}

sx_fiber_transfer sx_fiber_switch(const sx_fiber_t to, void* user) {
    return jump_fcontext(to, user);
}

typedef enum {
    CORO_RET_NONE = 0,
    CORO_RET_END,      // Executation is finished
    CORO_RET_YIELD,    // Pass this 'update' to the next N update which is 'arg' in
                       // sx_fiber_return
    CORO_RET_WAIT      // Wait for msecs: 'arg' is msecs in sx_fiber_return
} sx_coro_ret_type;

typedef union {
    float tm;
    int   n;
} sx__coro_state_counter;

typedef struct sx__coro_state {
    sx_fiber_t             fiber;
    sx_fiber_stack         stack_mem;
    sx_fiber_cb*           callback;
    void*                  user;
    sx_coro_ret_type       ret_state;
    sx__coro_state_counter arg;
    sx__coro_state_counter counter;
    struct sx__coro_state* next;
    struct sx__coro_state* prev;
} sx__coro_state;

typedef struct sx_coro_context {
    sx_pool*        coro_pool;
    sx__coro_state* run_list;
    sx__coro_state* run_list_last;
    sx__coro_state* cur_coro;
    int             stack_sz;
} sx_coro_context;

static inline void sx__coro_add_list(sx__coro_state** pfirst, sx__coro_state** plast,
                                     sx__coro_state* node) {
    // Add to the end of the list
    if (*plast) {
        (*plast)->next = node;
        node->prev = *plast;
    }
    *plast = node;
    if (*pfirst == NULL)
        *pfirst = node;
}

static inline void sx__coro_remove_list(sx__coro_state** pfirst, sx__coro_state** plast,
                                        sx__coro_state* node) {
    if (node->prev)
        node->prev->next = node->next;
    if (node->next)
        node->next->prev = node->prev;
    if (*pfirst == node)
        *pfirst = node->next;
    if (*plast == node)
        *plast = node->prev;
    node->prev = node->next = NULL;
}

sx_coro_context* sx_coro_create_context(const sx_alloc* alloc, int max_fibers, int stack_sz) {
    sx_assert(max_fibers > 0);
    sx_assert(stack_sz >= sx_os_minstacksz() && "stack size too small");

    sx_coro_context* ctx = (sx_coro_context*)sx_malloc(alloc, sizeof(sx_coro_context));
    if (!ctx) {
        sx_out_of_memory();
        return NULL;
    }
    sx_memset(ctx, 0x0, sizeof(sx_coro_context));

    ctx->coro_pool = sx_pool_create(alloc, sizeof(sx__coro_state), max_fibers);
    if (!ctx->coro_pool) {
        sx_out_of_memory();
        return NULL;
    }
    sx_memset(ctx->coro_pool->pages->buff, 0x0, sizeof(sx__coro_state) * max_fibers);
    ctx->stack_sz = stack_sz;

    return ctx;
}

void sx_coro_destroy_context(sx_coro_context* ctx, const sx_alloc* alloc) {
    sx_assert(ctx);
    if (ctx->coro_pool)
        sx_pool_destroy(ctx->coro_pool, alloc);

    // TODO: release fiber's stack memory

    sx_free(alloc, ctx);
}

void sx__coro_invoke(sx_coro_context* ctx, sx_fiber_cb* callback, void* user) {
    sx__coro_state* fs = (sx__coro_state*)sx_pool_new(ctx->coro_pool);

    if (fs) {
        if (!fs->stack_mem.sptr) {
            // Initialize stack memory
            if (!sx_fiber_stack_init(&fs->stack_mem, ctx->stack_sz)) {
                sx_out_of_memory();
                return;
            }
        }
        fs->fiber = sx_fiber_create(fs->stack_mem, callback);
        fs->callback = callback;
        fs->user = user;
        // Add to the end of the list
        sx__coro_add_list(&ctx->run_list, &ctx->run_list_last, fs);

        ctx->cur_coro = fs;
        fs->fiber = sx_fiber_switch(fs->fiber, user).from;
    }
}

void sx_coro_update(sx_coro_context* ctx, float dt) {
    sx_assert(ctx->cur_coro == NULL);

    sx__coro_state* fs = ctx->run_list;
    while (fs) {
        sx__coro_state* next = fs->next;

        // Check state and take action for pending fibers
        switch (fs->ret_state) {
        case CORO_RET_YIELD: {
            ++fs->counter.n;
            if (fs->counter.n >= fs->arg.n) {
                ctx->cur_coro = fs;
                fs->fiber = sx_fiber_switch(fs->fiber, fs->user).from;
            }
            break;
        }
        case CORO_RET_WAIT: {
            fs->counter.tm += dt;
            if (fs->counter.tm >= fs->arg.tm) {
                ctx->cur_coro = fs;
                fs->fiber = sx_fiber_switch(fs->fiber, fs->user).from;
            }
            break;
        }
        default:
            sx_assert(0 && "Invalid ret type in update loop");
            break;
        }

        fs = next;
    }
}

bool sx_coro_replace_callback(sx_coro_context* ctx, sx_fiber_cb* callback,
                              sx_fiber_cb* new_callback) {
    sx_assert(callback);
    bool r = false;

    sx__coro_state* fs = ctx->run_list;
    while (fs) {
        sx__coro_state* next = fs->next;
        if (fs->callback == callback) {
            if (new_callback) {
                fs->callback = new_callback;
                fs->fiber = sx_fiber_create(fs->stack_mem, new_callback);
                r = true;
            } else {
                sx__coro_remove_list(&ctx->run_list, &ctx->run_list_last, fs);
                sx_pool_del(ctx->coro_pool, fs);
            }
        }
        fs = next;
    }

    return r;
}

static inline void sx__coro_return(sx_coro_context* ctx, sx_fiber_t* pfrom, sx_coro_ret_type type,
                                   int arg) {
    sx_assert(ctx->cur_coro &&
              "You must call this function from within sx_fiber_cb invoked by sx_fiber_invoke");
    sx_assert(type != CORO_RET_NONE && "Invalid enum for type");

    sx__coro_state* fs = ctx->cur_coro;

    // If fiber is finished, just remove it from the list
    if (type == CORO_RET_END) {
        sx__coro_remove_list(&ctx->run_list, &ctx->run_list_last, fs);
        sx_pool_del(ctx->coro_pool, fs);
    } else {
        fs->ret_state = type;
        fs->counter.n = 0;
        if (type == CORO_RET_WAIT)
            fs->arg.tm = ((float)arg) * 0.001f;    // Convert msecs to seconds
        else if (type == CORO_RET_YIELD)
            fs->arg.n = arg;    // Number of next update passes
    }

    ctx->cur_coro = NULL;
    *pfrom = sx_fiber_switch(*pfrom, NULL).from;
}

void sx__coro_end(sx_coro_context* ctx, sx_fiber_t* pfrom) {
    sx__coro_return(ctx, pfrom, CORO_RET_END, 0);
}

void sx__coro_wait(sx_coro_context* ctx, sx_fiber_t* pfrom, int msecs) {
    sx__coro_return(ctx, pfrom, CORO_RET_WAIT, msecs);
}

void sx__coro_yield(sx_coro_context* ctx, sx_fiber_t* pfrom, int nupdates) {
    sx__coro_return(ctx, pfrom, CORO_RET_YIELD, nupdates);
}

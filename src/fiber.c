//
// Copyright 2018 Sepehr Taghdisian (septag@github). All rights reserved.
// License: https://github.com/septag/sx#license-bsd-2-clause
//
#include "sx/fiber.h"
#include "sx/os.h"
#include "sx/pool.h"

// TODO: Implement coroutines (fibers) in emscripten
// http://kripken.github.io/emscripten-site/docs/api_reference/emscripten.h.html#c.emscripten_coroutine

#if SX_PLATFORM_WINDOWS
#   define VC_EXTRALEAN
#   define WIN32_LEAN_AND_MEAN
#	include <windows.h>
#elif SX_PLATFORM_POSIX
#   include <unistd.h>
#   include <sys/mman.h>
#if defined(MAP_ANON) && !defined(MAP_ANONYMOUS)
#   define MAP_ANONYMOUS MAP_ANON
#endif
#endif

#define DEFAULT_STACK_SIZE 131072   // 120kb

// Fwd declare ASM functions
SX_API sx_fiber_transfer jump_fcontext(sx_fiber_t const, void*);
SX_API sx_fiber_t make_fcontext(void*, size_t, sx_fiber_cb*);

bool sx_fiber_stack_init(sx_fiber_stack* fstack, size_t size)
{
    if (size == 0)
        size = DEFAULT_STACK_SIZE;
    size = sx_os_align_pagesz(size);
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
    ptr = mmap(0, size, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
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

void sx_fiber_stack_init_ptr(sx_fiber_stack* fstack, void* ptr, size_t size)
{
    int page_sz = sx_os_pagesz();
    sx_assert((uintptr_t)ptr % page_sz == 0 && "buffer size must be dividable to OS page size");
    sx_assert(size % page_sz == 0 && "buffer size must be dividable to OS page size");

    fstack->sptr = ptr;
    fstack->ssize = size;
}

void sx_fiber_stack_release(sx_fiber_stack* fstack)
{
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

sx_fiber_t sx_fiber_create(const sx_fiber_stack stack, sx_fiber_cb* fiber_cb)
{
    return make_fcontext(stack.sptr, stack.ssize, fiber_cb);
}

sx_fiber_transfer sx_fiber_switch(const sx_fiber_t to, void* user)
{
    return jump_fcontext(to, user);
}

typedef union sx__fiber_state_counter
{
    float tm;
    int   n;
} sx__fiber_state_counter;

typedef struct sx__fiber_state
{
    sx_fiber_t              fiber;
    sx_fiber_stack          stack_mem;
    void*                   user;
    sx_fiber_ret_type       ret_state;
    sx__fiber_state_counter arg;
    sx__fiber_state_counter counter;
    struct sx__fiber_state* next;
    struct sx__fiber_state* prev;
} sx__fiber_state;

typedef struct sx_fiber_context
{
    sx_pool*         fiber_pool;
    sx__fiber_state* run_list;
    sx__fiber_state* run_list_last;
    sx__fiber_state* cur_fiber;
    int              stack_sz;
} sx_fiber_context;

SX_INLINE void sx__fiber_add_list(sx__fiber_state** pfirst, sx__fiber_state** plast, sx__fiber_state* node)
{
    // Add to the end of the list
    if (*plast) {
        (*plast)->next = node;
        node->prev = *plast;
    } 
    *plast = node;
    if (*pfirst == NULL)
        *pfirst = node;
}

SX_INLINE void sx__fiber_remove_list(sx__fiber_state** pfirst, sx__fiber_state** plast, sx__fiber_state* node)
{
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

sx_fiber_context* sx_fiber_create_context(const sx_alloc* alloc, int max_fibers, int stack_sz)
{
    sx_assert(max_fibers > 0);
    sx_assert(stack_sz >= sx_os_minstacksz() && "stack size too small");

    sx_fiber_context* ctx = (sx_fiber_context*)sx_malloc(alloc, sizeof(sx_fiber_context));
    if (!ctx) {
        sx_out_of_memory();
        return NULL;
    }
    sx_memset(ctx, 0x0, sizeof(sx_fiber_context));

    ctx->fiber_pool = sx_pool_create(alloc, sizeof(sx__fiber_state), max_fibers);
    if (!ctx->fiber_pool) {
        sx_out_of_memory();
        return NULL;
    }
    sx_memset(ctx->fiber_pool->buff, 0x0, sizeof(sx__fiber_state)*max_fibers);
    ctx->stack_sz = stack_sz;

    return ctx;
}

void sx_fiber_destroy_context(sx_fiber_context* ctx, const sx_alloc* alloc)
{
    sx_assert(ctx);
    if (ctx->fiber_pool)
        sx_pool_destroy(ctx->fiber_pool, alloc);

    // TODO: release fiber's stack memory

    sx_free(alloc, ctx);
}

void sx_fiber_invoke(sx_fiber_context* ctx, sx_fiber_cb callback, void* user)
{
    sx__fiber_state* fs = (sx__fiber_state*)sx_pool_new(ctx->fiber_pool);

    if (fs) {
        if (!fs->stack_mem.sptr) {
            // Initialize stack memory
            if (!sx_fiber_stack_init(&fs->stack_mem, ctx->stack_sz)) {
                sx_out_of_memory();
                return;
            }            
        }
        fs->fiber = sx_fiber_create(fs->stack_mem, callback);
        fs->user = user;
        // Add to the end of the list
        sx__fiber_add_list(&ctx->run_list, &ctx->run_list_last, fs);

        ctx->cur_fiber = fs;
        fs->fiber = sx_fiber_switch(fs->fiber, user).from;
    }
}

void sx_fiber_update(sx_fiber_context* ctx, float dt)
{
    // Update and fetch fibers only if no fiber is currently running
    if (ctx->cur_fiber == NULL) {
        sx__fiber_state* fs = ctx->run_list;
        while (fs) {
            sx__fiber_state* next = fs->next;

            // Check state and take action for pending fibers
            switch (fs->ret_state) {
            case SX_FIBER_RET_PASS:
            {
                ++fs->counter.n;
                if (fs->counter.n >= fs->arg.n)   {
                    ctx->cur_fiber = fs;
                    fs->fiber = sx_fiber_switch(fs->fiber, fs->user).from;
                }
                break;
            }
            case SX_FIBER_RET_WAIT:
            {
                fs->counter.tm += dt;
                if (fs->counter.tm >= fs->arg.tm) {
                    ctx->cur_fiber = fs;
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
}

void sx_fiber_return(sx_fiber_context* ctx, sx_fiber_t* pfrom, sx_fiber_ret_type type, int arg)
{
    sx_assert(ctx->cur_fiber && "You must call this function from within sx_fiber_cb invoked by sx_fiber_invoke");
    sx_assert(type != SX_FIBER_RET_NONE && "Invalid enum for type");

    sx__fiber_state* fs = ctx->cur_fiber;

    // If fiber is finished, just remove it from the list
    if (type == SX_FIBER_RET_FINISH) {
        sx__fiber_remove_list(&ctx->run_list, &ctx->run_list_last, fs);
        sx_pool_del(ctx->fiber_pool, fs);
    } else {
        fs->ret_state = type;
        fs->counter.n = 0;
        if (type == SX_FIBER_RET_WAIT)
            fs->arg.tm = ((float)arg) * 0.001f;     // Convert msecs to seconds
        else if (type == SX_FIBER_RET_PASS)
            fs->arg.n = arg;                        // Number of next update passes
    }

    ctx->cur_fiber = NULL;
    *pfrom = sx_fiber_switch(*pfrom, NULL).from;
}
#include "sx/fiber.h"
#include "sx/os.h"

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
        SX_OUT_OF_MEMORY;
        return false;
    }
    DWORD old_opts;
    VirtualProtect(ptr, sx_os_pagesz(), PAGE_READWRITE | PAGE_GUARD, &old_opts);
#elif SX_PLATFORM_POSIX
    ptr = mmap(0, size, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
    if (ptr == MAP_FAILED) {
        SX_OUT_OF_MEMORY;
        return false;
    }
    mprotect(ptr, sx_os_pagesz(), PROT_NONE);    
#else
    ptr = malloc(size);
    if (!ptr) {
        SX_OUT_OF_MEMORY;
        return false;
    }
#endif
    
    fstack->sptr = (uint8_t*)ptr + size;    // Move to end of the memory block for stack pointer
    fstack->ssize = size;
    return true;
}

void sx_siber_stack_init_ptr(sx_fiber_stack* fstack, void* ptr, size_t size)
{
    int page_sz = sx_os_pagesz();
    assert((uintptr_t)ptr % page_sz == 0 && "buffer size must be dividable to OS page size");
    assert(size % page_sz == 0 && "buffer size must be dividable to OS page size");

    fstack->sptr = ptr;
    fstack->ssize = size;
}

void sx_fiber_stack_release(sx_fiber_stack* fstack)
{
    assert(fstack->sptr);
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

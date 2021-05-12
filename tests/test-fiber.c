#include "sx/allocator.h"
#include "sx/fiber.h"
#include "sx/os.h"
#include "sx/timer.h"
#include "sx/string.h"
#include <stdio.h>

void fiber1_fn(sx_fiber_transfer transfer)
{
    puts("Fiber1");
    sx_fiber_switch(transfer.from, transfer.user);    // Get back to where fiber1 is switched
    puts("Fiber1 - End");
    sx_fiber_switch(
        transfer.from,
        transfer.user);    // Always put this to return to caller at the end of the callback
}

sx_coro_declare(wait_test)
{
    sx_coro_context* ctx = sx_coro_userdata();
    puts("fiber_wait_fn: Start");

    puts("fiber_wait_fn: Waiting for 3 seconds");
    sx_coro_wait(ctx, 3000);
    puts("fiber_wait_fn: Continue");

    puts("fiber_wait_fn: End");
    sx_coro_end(ctx);    // Always call this !
}

sx_coro_declare(yield_test)
{
    sx_coro_context* ctx = sx_coro_userdata();
    puts("fiber_pass_fn: Start");

    puts("fiber_pass_fn: Pass and continue on next frame ..");
    sx_coro_yield(ctx);
    puts("fiber_pass_fn: Continue");

    puts("fiber_pass_fn: Pass and continue on next 100 frames ..");
    sx_coro_yieldn(ctx, 100);
    puts("fiber_pass_fn: Continue");

    puts("fiber_pass_fn: End");
    sx_coro_end(ctx);    // Always call this !
}


int main(int argc, char* argv[])
{
    const sx_alloc* alloc = sx_alloc_malloc();

    puts("---------------------");
    puts("Low level fiber test:");
    sx_fiber_stack stack1;
    sx_fiber_stack_init(&stack1, 80 * 1024);
    puts("Main");
    sx_fiber_t fiber = sx_fiber_create(stack1, fiber1_fn);
    sx_fiber_transfer t = sx_fiber_switch(fiber, NULL);
    puts("Back to main");
    sx_fiber_switch(t.from, t.user);
    puts("End");
    sx_fiber_stack_release(&stack1);

    //
    puts("---------------------");
    sx_tm_init();
    puts("Fiber context test: ");
    puts("Press CTRL+C to stop");
    sx_coro_context* ctx = sx_coro_create_context(alloc, 10, 32 * 1024);

    // Invoke fibers
    sx_coro_invoke(ctx, wait_test, ctx);
    sx_coro_invoke(ctx, yield_test, ctx);
    uint64_t tick = sx_tm_now();
    float dt = 0;

    while (1) {
        sx_coro_update(ctx, dt);
        sx_os_sleep(10);
        dt = (float)sx_tm_sec(sx_tm_laptime(&tick));
    }
    sx_coro_destroy_context(ctx);

    return 0;
}

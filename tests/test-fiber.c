#include "sx/fiber.h"
#include <stdio.h>

void fiber1_fn(sx_fiber_transfer transfer)  
{
    puts("Fiber1");
    sx_fiber_switch(transfer.from, transfer.user);    // Get back to where fiber1 is switched
    puts("Fiber1 - End");
    sx_fiber_switch(transfer.from, transfer.user);    // Always put this to return to caller at the end of the callback
}

int main(int argc, char* argv[]) 
{
    sx_fiber_stack stack1;
    sx_fiber_stack_init(&stack1, 80*1024);
    puts("Main");
    sx_fiber_t fiber = sx_fiber_create(stack1, fiber1_fn);
    sx_fiber_transfer t = sx_fiber_switch(fiber, NULL);
    puts("Back to main");
    sx_fiber_switch(t.from, t.user);
    puts("End");
    
    return 0;
}

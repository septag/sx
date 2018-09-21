#include "sx/timer.h"
#include <stdio.h>

int main(int argc, char* argv[])
{
    sx_tm_init();

    uint64_t start_tm = sx_tm_now();
    puts("Calculating fibonacci sequence ...");
    uint32_t a = 0;
    uint32_t b = 1;
    int N = 100000;
    for (int i = 0; i < N; i++) {
        uint32_t f = a + b; 
        a = b;
        b = f;
    }
    uint64_t delta_tm = sx_tm_since(start_tm);
    printf("Took %lf us (%lf secs)\n", sx_tm_us(delta_tm), sx_tm_sec(delta_tm));

    return 0;
}
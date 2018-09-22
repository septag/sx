#include "sx/tlsf-alloc.h"
#include "sx/hash.h"
#include "sx/rng.h"

#include <time.h>
#include <stdio.h>

static int walk_counter = 0;
static void tlsf_walk_fn(void* ptr, size_t size, int used, void* user)
{
    printf("%d) %p - %d bytes - used: %d\n", ++walk_counter, ptr, (int)size, used);
}

int main(int argc, char* argv[])
{
    sx_alloc tlsf;
    const sx_alloc* alloc = sx_alloc_malloc;

    const int pool_size = 4*1024*1024;
    void* mem = sx_malloc(alloc, pool_size);
    sx_assert(mem);

    sx_tlsfalloc_init(&tlsf, mem, pool_size);

    sx_rng rng;
    sx_rng_seed(&rng, (uint32_t)time(NULL));

    const int count = 1000;
    void* ptrs[1000];
    for (int i = 0; i < count; i++) {
        int sz = sx_rng_gen_irange(&rng, 100, 4*1024);
        ptrs[i] = sx_malloc(&tlsf, sz);
        sx_assert(ptrs[i]);
    }

    int free_cnt = 0;
    for (int i = 0; i < count; i++) {
        if (sx_rng_gen_irange(&rng, 1, 100) > 50) {
            sx_free(&tlsf, ptrs[i]);
            ++free_cnt;
        }
    }

    sx_assert(sx_tlsfalloc_check(&tlsf));

    // Walk
    sx_tlsfalloc_walk_pool(sx_tlsfalloc_get_pool(&tlsf), tlsf_walk_fn, NULL);

    sx_tlsfalloc_release(&tlsf);
    sx_free(alloc, mem);

    printf("\ntotal allocated: %d\n", count - free_cnt);
    return 0;
}
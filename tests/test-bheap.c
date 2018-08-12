#include "sx/bheap.h"
#include "sx/rng.h"

#include <time.h>
#include <stdio.h>

int main(int argc, char* argv[])
{
    static const int N = 32;

    sx_rng rng;
    sx_rng_seed(&rng, (uint32_t)time(NULL));

    const sx_alloc* alloc = sx_alloc_malloc;
    sx_bheap* bh = sx_bheap_create(alloc, N);

    puts("\nPushing values to _MIN_ binary heap:");
    for (int i = 0; i < N; i++) {
        int n = sx_rng_gen_irange(&rng, 1, 100);
        if (i < N - 1)
            printf("%d - ", n);
        else
            printf("%d\n", n);
        sx_bheap_push_min(bh, n, 0);
    }

    puts("Pop: ");
    for (int i = 0; i < N; i++) {
        sx_bheap_item item = sx_bheap_pop_min(bh);
        if (i < N - 1)
            printf("%d - ", item.key);
        else
            printf("%d\n", item.key);
    }

    puts("\nPushing values to _MAX_ binary heap:");
    for (int i = 0; i < N; i++) {
        int n = sx_rng_gen_irange(&rng, 1, 100);
        if (i < N - 1)
            printf("%d - ", n);
        else
            printf("%d\n", n);
        sx_bheap_push_max(bh, n, 0);
    }

    puts("Pop: ");
    for (int i = 0; i < N; i++) {
        sx_bheap_item item = sx_bheap_pop_max(bh);
        if (i < N - 1)
            printf("%d - ", item.key);
        else
            printf("%d\n", item.key);
    }

    puts("\nPushing values to _MIN_ binary heap with Random pops during push:");
    int num_pop = 0;

    for (int i = 0; i < N; i++) {
        int n = sx_rng_gen_irange(&rng, 1, 100);
        if (i < N - 1)
            printf("%d - ", n);
        else
            printf("%d\n", n);
        sx_bheap_push_min(bh, n, 0);

        if (sx_rng_gen_irange(&rng, 1, 100) < 50) {
            int key = sx_bheap_pop_min(bh).key;
            for (int k = 0; k < bh->count; k++)
                sx_assert(key <= bh->items[k].key);
            num_pop++;
        }
    }

    puts("Pop Remaining: ");
    for (int i = 0; i < N - num_pop; i++) {
        sx_bheap_item item = sx_bheap_pop_min(bh);
        printf("%d - ", item.key);
    }
    puts("");

    puts("\nPushing values to _MAX_ binary heap with Random pops during push:");

    for (int i = 0; i < N; i++) {
        int n = sx_rng_gen_irange(&rng, 1, 100);
        if (i < N - 1)
            printf("%d - ", n);
        else
            printf("%d\n", n);
        sx_bheap_push_max(bh, n, 0);

        if (sx_rng_gen_irange(&rng, 1, 100) < 50) {
            int key = sx_bheap_pop_max(bh).key;
            for (int k = 0; k < bh->count; k++)
                sx_assert(key >= bh->items[k].key);
            num_pop++;
        }
    }

    puts("Pop Remaining: ");
    for (int i = 0; i < N - num_pop; i++) {
        sx_bheap_item item = sx_bheap_pop_max(bh);
        printf("%d - ", item.key);
    }
    puts("");

    return 0;
}

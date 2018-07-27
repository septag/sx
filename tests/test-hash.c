#include "sx/hash.h"
#include "sx/timer.h"
#include "sx/rng.h"
#include "sx/os.h"
#include "sx/string.h"

#include <time.h>
#include <stdio.h>

typedef struct str_item
{
    char str[32];
} str_item;

int main(int argc, char* argv[])
{
    sx_rng rng;
    sx_rng_seed(&rng, time(NULL));
    sx_tm_setup();

    const int num_samples = 10000;
    const sx_alloc* alloc = sx_alloc_malloc;
    sx_hashtbl* tbl = sx_hashtbl_create(alloc, num_samples);
    assert(tbl);

    // Make random string array
    str_item* items = (str_item*)sx_malloc(alloc, sizeof(str_item)*num_samples);
    assert(items);

    puts("Generating random strings ...");
    for (int i = 0; i < num_samples; i++) {
        int num_chars = sx_rng_gen_irange(&rng, 16, 31);
        str_item* item = &items[i];
        for (int ii = 0; ii < num_chars; ii++) {
            char ch = (char)sx_rng_gen_irange(&rng, 48, 122);
            assert(ch >= 48 && ch <= 122);
            item->str[ii] = ch;
        }
        item->str[num_chars] = 0;
    }

    uint64_t start_tm, delta_tm;
    puts("Pushing into hash table ...");
    start_tm = sx_tm_now();

    for (int i = 0; i < num_samples; i++) {
        str_item* item = &items[i];
        sx_hashtbl_add(tbl, sx_hash_fnv32_str(item->str), i);
    }

    delta_tm = sx_tm_since(start_tm);
    printf("Took %lf ms (%lf secs)\n", sx_tm_ms(delta_tm), sx_tm_sec(delta_tm));

    puts("Searching for random items ...");
    start_tm = sx_tm_now();
    for (int i = 0; i < num_samples; i++) {
        int index = sx_rng_gen_irange(&rng, 0, num_samples-1);
        int r = sx_hashtbl_find_get(tbl, sx_hash_fnv32_str(items[index].str), -1);
        if (r != index && r != -1) {
            printf("ERROR: Invalid hash result:\n"
                   "\tRequested item: %s (index: %d)\n" 
                   "\tGot Item: %s (index: %d)\n",
                   items[index].str, index, 
                   items[r].str, r);
        } else if (r == -1) {
            printf("ERROR: Item not found: %s (index: %d)\n", items[index].str, index);
        }
    }
    delta_tm = sx_tm_since(start_tm);
    printf("Took %lf ms (%lf secs)\n", sx_tm_ms(delta_tm), sx_tm_sec(delta_tm));

    sx_os_getch();
    return 0;
}
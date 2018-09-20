#include "sx/hash.h"
#include "sx/timer.h"
#include "sx/rng.h"
#include "sx/os.h"
#include "sx/string.h"

#include <time.h>
#include <stdio.h>

////////////////////////////////////////////////////////////////////////////////////////////////////
// Prime number hash implementation optimized for 10000 entities (which is prime number 13001)
typedef struct sx__hashtbl2
{
    uint32_t* keys;
    int*      values;
    int       count;
    int       capacity;
#if SX_CONFIG_HASHTBL_DEBUG
    int       _miss_cnt;
    int       _probe_cnt;
#endif
} sx__hashtbl2;

static sx__hashtbl2* sx__hashtbl2_create(const sx_alloc* alloc, int capacity)
{
    sx_assert(capacity > 0);
    sx_assert(capacity == 10000);
    capacity = 13001;
    sx__hashtbl2* tbl = (sx__hashtbl2*)sx_malloc(alloc, 
        sizeof(sx__hashtbl2) + capacity*(sizeof(uint32_t) + sizeof(int)) + SX_CONFIG_ALLOCATOR_NATURAL_ALIGNMENT);
    if (!tbl) {
        sx_out_of_memory();
        return NULL;
    }

    tbl->keys = (uint32_t*)sx_align_ptr(tbl + 1, 0, SX_CONFIG_ALLOCATOR_NATURAL_ALIGNMENT);
    tbl->values = (int*)(tbl->keys + capacity);
    tbl->count = 0;
    tbl->capacity = capacity;
#if SX_CONFIG_HASHTBL_DEBUG
    tbl->_miss_cnt = 0;
    tbl->_probe_cnt = 0;
#endif

    // Reset keys
    sx_memset(tbl->keys, 0x0, sizeof(uint32_t)*capacity);

    return tbl;
}

static void sx__hashtbl2_destroy(sx__hashtbl2* tbl, const sx_alloc* alloc)
{
    sx_assert(tbl);
    tbl->count = tbl->capacity = 0;
    sx_free(alloc, tbl);
}

static int sx__hashtbl2_add(sx__hashtbl2* tbl, uint32_t key, int value)
{
    sx_assert(tbl->count < tbl->capacity);

    uint32_t cnt = (uint32_t)tbl->capacity;
    uint32_t h = key % cnt;
    while (tbl->keys[h] != 0) {
        h = (h + 1) % cnt; 
    }

    sx_assert(tbl->keys[h] == 0);  // something went wrong!
    tbl->keys[h] = key;
    tbl->values[h] = value;    
    ++tbl->count;
    return h;    
}

static int sx__hashtbl2_find(const sx__hashtbl2* tbl, uint32_t key)
{
    uint32_t cnt = (uint32_t)tbl->capacity;
    uint32_t h = key % cnt;
    sx_assert(h >= 0 && h < (uint32_t)tbl->capacity);
    if (tbl->keys[h] == key) {
        return h;
    } else {
#if SX_CONFIG_HASHTBL_DEBUG
        sx__hashtbl2* _tbl = (sx__hashtbl2*)tbl;
        ++_tbl->_miss_cnt;
#endif
        // probe lineary in the keys array
        for (uint32_t i = 1; i < cnt; i++) {
            int idx = (h + i) % cnt;
#if SX_CONFIG_HASHTBL_DEBUG
            ++_tbl->_probe_cnt;
#endif
            if (tbl->keys[idx] == key)
                return idx;
        }

        return -1;  // Worst case: Not found!
    }
}

SX_INLINE int sx__hashtbl2_find_get(const sx__hashtbl2* tbl, uint32_t key, int not_found_val)
{
    int index = sx__hashtbl2_find(tbl, key);
    return index != -1 ? tbl->values[index] : not_found_val;
}

typedef struct str_item
{
    char str[32];
} str_item;

int main(int argc, char* argv[])
{
    sx_rng rng;
    sx_rng_seed(&rng, (uint32_t)time(NULL));
    sx_tm_init();

    const int num_samples = 10000;
    const sx_alloc* alloc = sx_alloc_malloc;
    sx_hashtbl* tbl = sx_hashtbl_create(alloc, num_samples);
    sx_assert(tbl);

    // Make random string array
    str_item* items = (str_item*)sx_malloc(alloc, sizeof(str_item)*num_samples);
    sx_assert(items);

    puts("Generating random strings ...");
    for (int i = 0; i < num_samples; i++) {
        str_item* item = &items[i];
        int num_chars = sx_rng_gen_irange(&rng, 16, sizeof(item->str)-1);
        for (int ii = 0; ii < num_chars; ii++) {
            char ch = (char)sx_rng_gen_irange(&rng, 48, 122);
            sx_assert(ch >= 48 && ch <= 122);
            item->str[ii] = ch;
        }
        item->str[num_chars] = 0;
    }

    uint64_t start_tm, delta_tm;
    puts("sx_hashtbl:");
    puts("\tPushing into hash table ...");
    start_tm = sx_tm_now();

    for (int i = 0; i < num_samples; i++) {
        str_item* item = &items[i];
        sx_hashtbl_add(tbl, sx_hash_fnv32_str(item->str), i);
    }

    delta_tm = sx_tm_since(start_tm);
    printf("\tTook %lf ms (%lf secs)\n", sx_tm_ms(delta_tm), sx_tm_sec(delta_tm));

    puts("\tSearching for random items ...");
    start_tm = sx_tm_now();
    for (int i = 0; i < num_samples; i++) {
        int index = sx_rng_gen_irange(&rng, 0, num_samples-1);
        int r = sx_hashtbl_find_get(tbl, sx_hash_fnv32_str(items[index].str), -1);
        if (r != index && r != -1) {
            printf("\tERROR: Invalid hash result:\n"
                   "\t\tRequested item: %s (index: %d)\n" 
                   "\t\tGot Item: %s (index: %d)\n",
                   items[index].str, index, 
                   items[r].str, r);
        } else if (r == -1) {
            printf("\tERROR: Item not found: %s (index: %d)\n", items[index].str, index);
        }
    }
    delta_tm = sx_tm_since(start_tm);
    printf("\tTook %lf ms (%lf secs)\n", sx_tm_ms(delta_tm), sx_tm_sec(delta_tm));
    if (SX_ENABLED(SX_CONFIG_HASHTBL_DEBUG))
        printf("\tTotal miss count: %d\n\tTotal probe count: %d\n", tbl->_miss_cnt, tbl->_probe_cnt);

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // sx__hashtbl2
    sx__hashtbl2* tbl2 = sx__hashtbl2_create(alloc, num_samples);
    sx_assert(tbl2);

    puts("sx_hashtbl2:");
    puts("\tPushing into hash table ...");
    start_tm = sx_tm_now();

    for (int i = 0; i < num_samples; i++) {
        str_item* item = &items[i];
        sx__hashtbl2_add(tbl2, sx_hash_fnv32_str(item->str), i);
    }

    delta_tm = sx_tm_since(start_tm);
    printf("\tTook %lf ms (%lf secs)\n", sx_tm_ms(delta_tm), sx_tm_sec(delta_tm));

    puts("\tSearching for random items ...");
    start_tm = sx_tm_now();
    for (int i = 0; i < num_samples; i++) {
        int index = sx_rng_gen_irange(&rng, 0, num_samples-1);
        int r = sx__hashtbl2_find_get(tbl2, sx_hash_fnv32_str(items[index].str), -1);
        if (r != index && r != -1) {
            printf("\tERROR: Invalid hash result:\n"
                   "\t\tRequested item: %s (index: %d)\n" 
                   "\t\tGot Item: %s (index: %d)\n",
                   items[index].str, index, 
                   items[r].str, r);
        } else if (r == -1) {
            printf("\tERROR: Item not found: %s (index: %d)\n", items[index].str, index);
        }
    }
    delta_tm = sx_tm_since(start_tm);
    printf("\tTook %lf ms (%lf secs)\n", sx_tm_ms(delta_tm), sx_tm_sec(delta_tm));
    if (SX_ENABLED(SX_CONFIG_HASHTBL_DEBUG))
        printf("\tTotal miss count: %d\n\tTotal probe count: %d\n", tbl2->_miss_cnt, tbl2->_probe_cnt);

    sx_hashtbl_destroy(tbl, alloc);
    sx__hashtbl2_destroy(tbl2, alloc);

    sx_os_getch();
    return 0;
}
#include "sx/bheap.h"

static void sx__bheap_heapify_min(sx_bheap* bh, int index)
{
    int _2i = 2*index;
    int _min = index;
    int count = bh->count;
    sx_bheap_item min_item = bh->items[index];

    while (_2i + 1 < count) {
        int left = _2i + 1;
        int right = _2i + 2;

        if (bh->items[left].key < bh->items[_min].key)
            _min = left;
        if (right < count && bh->items[right].key < bh->items[_min].key)
            _min = right;
        if (_min == index)
            break;

        bh->items[index] = bh->items[_min];
        bh->items[_min]  = min_item;
        index = _min;
        _2i = 2*index;
    }
}

static void sx__bheap_heapify_max(sx_bheap* bh, int index)
{
    int _2i = 2*index;
    int _max = index;
    int count = bh->count;
    sx_bheap_item max_item = bh->items[index];

    while (_2i + 1 < count) {
        int left = _2i + 1;
        int right = _2i + 2;

        if (bh->items[left].key > bh->items[_max].key)
            _max = left;
        if (right < count && bh->items[right].key > bh->items[_max].key)
            _max = right;
        if (_max == index)
            break;

        bh->items[index] = bh->items[_max];
        bh->items[_max]  = max_item;
        index = _max;
        _2i = 2*index;
    }
}

sx_bheap* sx_bheap_create(const sx_alloc* alloc, int capacity)
{
    size_t total_sz = sizeof(sx_bheap) + sizeof(sx_bheap_item)*capacity;
    sx_bheap* bh = (sx_bheap*)sx_malloc(alloc, total_sz);
    if (!bh) {
        sx_out_of_memory();
        return NULL;
    }

    bh->items = (sx_bheap_item*)(bh + 1);
    bh->count = 0;
    bh->capacity = capacity;

    return bh;
}

void sx_bheap_destroy(sx_bheap* bh, const sx_alloc* alloc)
{
    sx_assert(bh);
    sx_free(alloc, bh);
}


void sx_bheap_push_min(sx_bheap* bh, int key, void* user)
{
    sx_assert(bh->count < bh->capacity && "BinaryHeap's capacity exceeded");

    // Put the value at the bottom the tree and traverse up    
    int index = bh->count;
    bh->items[index].key = key;
    bh->items[index].user = user;

    while (index > 0) {
        int parent_idx = (index - 1)>>1;
        sx_bheap_item cur = bh->items[index];
        sx_bheap_item parent = bh->items[parent_idx];

        if (cur.key < parent.key) {
            bh->items[index] = parent;
            bh->items[parent_idx] = cur;
        } else {
            break;
        }

        index = parent_idx;
    }
    ++bh->count;
}

sx_bheap_item sx_bheap_pop_min(sx_bheap* bh)
{
    sx_assert(bh && bh->count > 0);

    // Root is the value we want
    sx_bheap_item result = bh->items[0];

    // put the last one on the root and heapify
    int last = bh->count - 1;
    bh->items[0] = bh->items[last];
    bh->count = last;

    sx__bheap_heapify_min(bh, 0);
    return result;
}

void sx_bheap_push_max(sx_bheap* bh, int key, void* user)
{
    sx_assert(bh->count < bh->capacity && "BinaryHeap's capacity exceeded");

    // Put the value at the bottom the tree and traverse up    
    int index = bh->count;
    bh->items[index].key = key;
    bh->items[index].user = user;

    while (index > 0) {
        int parent_idx = (index - 1)>>1;
        sx_bheap_item cur = bh->items[index];
        sx_bheap_item parent = bh->items[parent_idx];

        if (cur.key > parent.key) {
            bh->items[index] = parent;
            bh->items[parent_idx] = cur;
        } else {
            break;
        }

        index = parent_idx;
    }
    ++bh->count;
}

sx_bheap_item sx_bheap_pop_max(sx_bheap* bh)
{
    sx_assert(bh && bh->count > 0);

    // Root is the value we want
    sx_bheap_item result = bh->items[0];

    // put the last one on the root and heapify
    int last = bh->count - 1;
    bh->items[0] = bh->items[last];
    bh->count = last;

    sx__bheap_heapify_max(bh, 0);
    return result;
}

void sx_bheap_clear(sx_bheap* bh)
{
    bh->count = 0;
}

bool sx_bheap_empty(sx_bheap* bh)
{
    return bh->count == 0;
}
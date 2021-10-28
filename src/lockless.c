#include "sx/lockless.h"
#include "sx/atomic.h"
#include "sx/allocator.h"

// single producer/single consumer - self contained queue
// Reference:
// http://www.drdobbs.com/parallel/writing-lock-free-code-a-corrected-queue/210604448?pgno=1
typedef struct sx__queue_spsc_node {
    struct sx__queue_spsc_node* next;
} sx__queue_spsc_node;

typedef struct sx__queue_spsc_bin {
    sx__queue_spsc_node** ptrs;
    uint8_t* buff;
    struct sx__queue_spsc_bin* next;
    int iter;
    int _reserved;
} sx__queue_spsc_bin;

typedef struct sx_queue_spsc {
    sx__queue_spsc_node** ptrs;
    uint8_t* buff;
    int iter;
    int capacity;
    int stride;
    int buff_size;

    sx__queue_spsc_node* first;
    sx_atomic_ptr last;
    sx_atomic_ptr divider;

    sx__queue_spsc_bin* grow_bins;    // linked-list of bins, if queue is grown
} sx_queue_spsc;

static sx__queue_spsc_bin* sx__queue_spsc_create_bin(const sx_alloc* alloc, int item_sz, int capacity)
{
    sx_assert(capacity % 16 == 0);

    uint8_t* buff = (uint8_t*)sx_malloc(
        alloc, sizeof(sx__queue_spsc_bin) + (item_sz + sizeof(void*) + sizeof(sx__queue_spsc_node)) * capacity);
    if (!buff) {
        sx_out_of_memory();
        return NULL;
    }
    sx__queue_spsc_bin* bin = (sx__queue_spsc_bin*)buff;
    buff += sizeof(sx__queue_spsc_bin);
    bin->ptrs = (sx__queue_spsc_node**)buff;
    buff += sizeof(sx__queue_spsc_node*) * capacity;
    bin->buff = buff;
    bin->next = NULL;

    bin->iter = capacity;

    for (int i = 0; i < capacity; i++) {
        bin->ptrs[capacity - i - 1] =
            (sx__queue_spsc_node*)(bin->buff + (sizeof(sx__queue_spsc_node) + item_sz) * i);
    }

    return bin;
}

static void sx__queue_spsc_destroy_bin(sx__queue_spsc_bin* bin, const sx_alloc* alloc)
{
    sx_assert(bin);
    sx_free(alloc, bin);
}

sx_queue_spsc* sx_queue_spsc_create(const sx_alloc* alloc, int item_sz, int capacity)
{
    sx_assert(item_sz > 0);

    capacity = sx_align_mask(capacity, 15);
    uint8_t* buff = (uint8_t*)sx_malloc(
        alloc,
        sizeof(sx_queue_spsc) + (item_sz + sizeof(void*) + sizeof(sx__queue_spsc_node)) * capacity);
    if (!buff) {
        sx_out_of_memory();
        return NULL;
    }

    sx_queue_spsc* queue = (sx_queue_spsc*)buff;
    buff += sizeof(sx_queue_spsc);
    queue->ptrs = (sx__queue_spsc_node**)buff;
    buff += sizeof(sx__queue_spsc_node*) * capacity;
    queue->buff = buff;

    queue->iter = capacity;
    queue->capacity = capacity;
    queue->stride = item_sz;
    queue->buff_size = (item_sz + (int)sizeof(sx__queue_spsc_node))*capacity;

    for (int i = 0; i < capacity; i++) {
        queue->ptrs[capacity - i - 1] =
            (sx__queue_spsc_node*)(queue->buff + (sizeof(sx__queue_spsc_node) + item_sz) * i);
    }

    // initialize dummy node
    sx__queue_spsc_node* node = queue->ptrs[--queue->iter];
    node->next = NULL;
    queue->first = node;
    queue->divider = queue->last = (uintptr_t)node;
    queue->grow_bins = NULL;

    return queue;
}

void sx_queue_spsc_destroy(sx_queue_spsc* queue, const sx_alloc* alloc)
{
    if (queue) {
        if (queue->grow_bins) {
            sx__queue_spsc_bin* bin = queue->grow_bins;
            while (bin) {
                sx__queue_spsc_bin* next = bin->next;
                sx__queue_spsc_destroy_bin(bin, alloc);
                bin = next;
            }
        }

        queue->capacity = queue->iter = 0;
        sx_free(alloc, queue);
    }
}

bool sx_queue_spsc_produce(sx_queue_spsc* queue, const void* data)
{
    sx__queue_spsc_node* node = NULL;
    sx__queue_spsc_bin* node_bin = NULL;
    if (queue->iter > 0) {
        node = queue->ptrs[--queue->iter];
    } else {
        // look in bins
        sx__queue_spsc_bin* bin = queue->grow_bins;
        while (bin && !node) {
            if (bin->iter > 0) {
                node = bin->ptrs[--bin->iter];
                node_bin = bin;
            }
            bin = bin->next;
        }
    }

    if (node) {
        sx_memcpy(node + 1, data, queue->stride);
        node->next = NULL;

        sx__queue_spsc_node* last = (sx__queue_spsc_node*)sx_atomic_exchangeptr(&queue->last, (uintptr_t)node);
        last->next = node;

        // trim/remove un-used nodes up to the divider
        while ((uintptr_t)queue->first != sx_atomic_loadptr_explicit(&queue->divider, SX_ATOMIC_MEMORYORDER_ACQUIRE)) {
            sx__queue_spsc_node* first = (sx__queue_spsc_node*)queue->first;
            queue->first = first->next;

            uintptr_t first_ptr = (uintptr_t)first;
            if (first_ptr >= (uintptr_t)queue->buff && first_ptr < (uintptr_t)(queue->buff + queue->buff_size)) {
                sx_assert(queue->iter != queue->capacity);
                queue->ptrs[queue->iter++] = first;
            } else {
                // find the right bin
                sx__queue_spsc_bin* bin = queue->grow_bins;
                while (bin) {
                    if (first_ptr >= (uintptr_t)bin->buff && first_ptr < (uintptr_t)(bin->buff + queue->buff_size)) {
                        sx_assert(bin->iter != queue->capacity);
                        bin->ptrs[bin->iter++] = first;
                        break;
                    }
                    bin = bin->next;
                }
                sx_assertf(bin, "item does not belong to queue buffers ?!");
            }
        }
        return true;
    } else {
        return false;
    }
}

bool sx_queue_spsc_consume(sx_queue_spsc* queue, void* data)
{
    if (queue->divider != sx_atomic_loadptr_explicit(&queue->last, SX_ATOMIC_MEMORYORDER_ACQUIRE)) {
        sx__queue_spsc_node* divider = (sx__queue_spsc_node*)queue->divider;
        sx_assert(divider->next);
        sx_memcpy(data, divider->next + 1, queue->stride);

        sx_atomic_storeptr_explicit(&queue->divider, (uintptr_t)divider->next, SX_ATOMIC_MEMORYORDER_RELEASE);
        return true;
    }

    return false;
}

bool sx_queue_spsc_grow(sx_queue_spsc* queue, const sx_alloc* alloc)
{
    sx__queue_spsc_bin* bin = sx__queue_spsc_create_bin(alloc, queue->stride, queue->capacity);
    if (bin) {
        if (queue->grow_bins) {
            sx__queue_spsc_bin* last = queue->grow_bins;
            while (last->next) last = last->next;
            last->next = bin;
        } else {
            queue->grow_bins = bin;
        }
        return true;
    } else {
        return false;
    }
}

bool sx_queue_spsc_full(const sx_queue_spsc* queue)
{
    if (queue->iter > 0) {
        return false;
    } else {
        // look in bins
        sx__queue_spsc_bin* bin = queue->grow_bins;
        while (bin) {
            if (bin->iter > 0)
                return false;
            bin = bin->next;
        }
    }

    return true;
}



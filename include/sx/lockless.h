#pragma once

#include "sx.h"

typedef struct sx_alloc sx_alloc;

// single-producer / single-consumer
// growable
typedef struct sx_queue_spsc sx_queue_spsc;
SX_API sx_queue_spsc* sx_queue_spsc_create(const sx_alloc* alloc, int item_sz, int capacity);
SX_API void sx_queue_spsc_destroy(sx_queue_spsc* queue, const sx_alloc* alloc);

SX_API bool sx_queue_spsc_produce(sx_queue_spsc* queue, const void* data);
SX_API bool sx_queue_spsc_consume(sx_queue_spsc* queue, void* data);
SX_API bool sx_queue_spsc_grow(sx_queue_spsc* queue, const sx_alloc* alloc);
SX_API bool sx_queue_spsc_full(const sx_queue_spsc* queue);

#define sx_queue_spsc_produce_and_grow(_queue, _data, _alloc)             \
    (sx_queue_spsc_full(_queue) ? sx_queue_spsc_grow(_queue, _alloc) : 0, sx_queue_spsc_produce(_queue, (_data)))




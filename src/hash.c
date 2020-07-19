//
// Copyright 2018 Sepehr Taghdisian (septag@github). All rights reserved.
// License: https://github.com/septag/sx#license-bsd-2-clause
//
#include "sx/hash.h"
#include "sx/allocator.h"

// https://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2
static inline SX_CONSTFN int sx__nearest_pow2(int n)
{
    n--;
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    n++;
    return n;
}

static inline SX_CONSTFN bool sx__ispow2(int n)
{
    return (n & (n - 1)) == 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
uint32_t sx_hash_u32(uint32_t key)
{
    const uint32_t c2 = 0x27d4eb2d;    // a prime or an odd constant
    key = (key ^ 61) ^ (key >> 16);
    key = key + (key << 3);
    key = key ^ (key >> 4);
    key = key * c2;
    key = key ^ (key >> 15);
    return key;
}

uint64_t sx_hash_u64(uint64_t key)
{
    key = (~key) + (key << 21);    // key = (key << 21) - key - 1;
    key = key ^ (key >> 24);
    key = (key + (key << 3)) + (key << 8);    // key * 265
    key = key ^ (key >> 14);
    key = (key + (key << 2)) + (key << 4);    // key * 21
    key = key ^ (key >> 28);
    key = key + (key << 31);
    return key;
}

uint32_t sx_hash_u64_to_u32(uint64_t key)
{
    key = (~key) + (key << 18);
    key = key ^ (key >> 31);
    key = key * 21;
    key = key ^ (key >> 11);
    key = key + (key << 6);
    key = key ^ (key >> 22);
    return (uint32_t)key;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// XXH
#define XXH_PRIVATE_API
#include "../3rdparty/xxhash/xxhash.h"

uint32_t sx_hash_xxh32(const void* data, size_t len, uint32_t seed)
{
    return XXH32(data, len, seed);
}

uint64_t sx_hash_xxh64(const void* data, size_t len, uint64_t seed)
{
    return XXH64(data, len, seed);
}

sx_hash_xxh32_t* sx_hash_create_xxh32(const sx_alloc* alloc)
{
    return (sx_hash_xxh32_t*)sx_malloc(alloc, sizeof(XXH32_state_t));
}

void sx_hash_destroy_xxh32(sx_hash_xxh32_t* state, const sx_alloc* alloc)
{
    sx_free(alloc, state);
}

void sx_hash_xxh32_init(sx_hash_xxh32_t* state, uint32_t seed)
{
    XXH32_state_t* xstate = (XXH32_state_t*)state;
    XXH32_reset(xstate, seed);
}

void sx_hash_xxh32_update(sx_hash_xxh32_t* state, const void* data, size_t len)
{
    XXH32_state_t* xstate = (XXH32_state_t*)state;
    XXH32_update(xstate, data, len);
}

uint32_t sx_hash_xxh32_digest(sx_hash_xxh32_t* state)
{
    XXH32_state_t* xstate = (XXH32_state_t*)state;
    return XXH32_digest(xstate);
}

sx_hash_xxh64_t* sx_hash_create_xxh64(const sx_alloc* alloc)
{
    return (sx_hash_xxh64_t*)sx_malloc(alloc, sizeof(XXH64_state_t));
}

void sx_hash_destroy_xxh64(sx_hash_xxh64_t* state, const sx_alloc* alloc)
{
    sx_free(alloc, state);
}

void sx_hash_xxh64_init(sx_hash_xxh64_t* state, uint64_t seed)
{
    XXH64_state_t* xstate = (XXH64_state_t*)state;
    XXH64_reset(xstate, seed);
}

void sx_hash_xxh64_update(sx_hash_xxh64_t* state, const void* data, size_t len)
{
    XXH64_state_t* xstate = (XXH64_state_t*)state;
    XXH64_update(xstate, data, len);
}

uint64_t sx_hash_xxh64_digest(sx_hash_xxh64_t* state)
{
    XXH64_state_t* xstate = (XXH64_state_t*)state;
    return XXH64_digest(xstate);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// FNV1a
// http://www.isthe.com/chongo/src/fnv/hash_32a.c

#define FNV1_32_INIT 0x811c9dc5
#define FNV1_32_PRIME 0x01000193
uint32_t sx_hash_fnv32(const void* data, size_t len)
{
    const char* bp = (const char*)data;
    const char* be = bp + len;

    uint32_t hval = FNV1_32_INIT;
    while (bp < be) {
        hval ^= (uint32_t)*bp++;
        hval *= FNV1_32_PRIME;
    }

    return hval;
}

uint32_t sx_hash_fnv32_str(const char* str)
{
    const char* s = str;

    uint32_t hval = FNV1_32_INIT;
    while (*s) {
        hval ^= (uint32_t)*s++;
        hval *= FNV1_32_PRIME;
    }

    return hval;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// CRC32
/*
 * This file is derived from crc32.c from the zlib-1.1.3 distribution
 * by Jean-loup Gailly and Mark Adler.
 */

/* crc32.c -- compute the CRC-32 of a data stream
 * Copyright (C) 1995-1998 Mark Adler
 * For conditions of distribution and use, see copyright notice in zlib.h
 */


/* ========================================================================
 * Table of CRC-32's of all single-byte values (made by make_crc_table)
 */
static const uint32_t crc_table[256] = {
    0x00000000L, 0x77073096L, 0xee0e612cL, 0x990951baL, 0x076dc419L, 0x706af48fL, 0xe963a535L,
    0x9e6495a3L, 0x0edb8832L, 0x79dcb8a4L, 0xe0d5e91eL, 0x97d2d988L, 0x09b64c2bL, 0x7eb17cbdL,
    0xe7b82d07L, 0x90bf1d91L, 0x1db71064L, 0x6ab020f2L, 0xf3b97148L, 0x84be41deL, 0x1adad47dL,
    0x6ddde4ebL, 0xf4d4b551L, 0x83d385c7L, 0x136c9856L, 0x646ba8c0L, 0xfd62f97aL, 0x8a65c9ecL,
    0x14015c4fL, 0x63066cd9L, 0xfa0f3d63L, 0x8d080df5L, 0x3b6e20c8L, 0x4c69105eL, 0xd56041e4L,
    0xa2677172L, 0x3c03e4d1L, 0x4b04d447L, 0xd20d85fdL, 0xa50ab56bL, 0x35b5a8faL, 0x42b2986cL,
    0xdbbbc9d6L, 0xacbcf940L, 0x32d86ce3L, 0x45df5c75L, 0xdcd60dcfL, 0xabd13d59L, 0x26d930acL,
    0x51de003aL, 0xc8d75180L, 0xbfd06116L, 0x21b4f4b5L, 0x56b3c423L, 0xcfba9599L, 0xb8bda50fL,
    0x2802b89eL, 0x5f058808L, 0xc60cd9b2L, 0xb10be924L, 0x2f6f7c87L, 0x58684c11L, 0xc1611dabL,
    0xb6662d3dL, 0x76dc4190L, 0x01db7106L, 0x98d220bcL, 0xefd5102aL, 0x71b18589L, 0x06b6b51fL,
    0x9fbfe4a5L, 0xe8b8d433L, 0x7807c9a2L, 0x0f00f934L, 0x9609a88eL, 0xe10e9818L, 0x7f6a0dbbL,
    0x086d3d2dL, 0x91646c97L, 0xe6635c01L, 0x6b6b51f4L, 0x1c6c6162L, 0x856530d8L, 0xf262004eL,
    0x6c0695edL, 0x1b01a57bL, 0x8208f4c1L, 0xf50fc457L, 0x65b0d9c6L, 0x12b7e950L, 0x8bbeb8eaL,
    0xfcb9887cL, 0x62dd1ddfL, 0x15da2d49L, 0x8cd37cf3L, 0xfbd44c65L, 0x4db26158L, 0x3ab551ceL,
    0xa3bc0074L, 0xd4bb30e2L, 0x4adfa541L, 0x3dd895d7L, 0xa4d1c46dL, 0xd3d6f4fbL, 0x4369e96aL,
    0x346ed9fcL, 0xad678846L, 0xda60b8d0L, 0x44042d73L, 0x33031de5L, 0xaa0a4c5fL, 0xdd0d7cc9L,
    0x5005713cL, 0x270241aaL, 0xbe0b1010L, 0xc90c2086L, 0x5768b525L, 0x206f85b3L, 0xb966d409L,
    0xce61e49fL, 0x5edef90eL, 0x29d9c998L, 0xb0d09822L, 0xc7d7a8b4L, 0x59b33d17L, 0x2eb40d81L,
    0xb7bd5c3bL, 0xc0ba6cadL, 0xedb88320L, 0x9abfb3b6L, 0x03b6e20cL, 0x74b1d29aL, 0xead54739L,
    0x9dd277afL, 0x04db2615L, 0x73dc1683L, 0xe3630b12L, 0x94643b84L, 0x0d6d6a3eL, 0x7a6a5aa8L,
    0xe40ecf0bL, 0x9309ff9dL, 0x0a00ae27L, 0x7d079eb1L, 0xf00f9344L, 0x8708a3d2L, 0x1e01f268L,
    0x6906c2feL, 0xf762575dL, 0x806567cbL, 0x196c3671L, 0x6e6b06e7L, 0xfed41b76L, 0x89d32be0L,
    0x10da7a5aL, 0x67dd4accL, 0xf9b9df6fL, 0x8ebeeff9L, 0x17b7be43L, 0x60b08ed5L, 0xd6d6a3e8L,
    0xa1d1937eL, 0x38d8c2c4L, 0x4fdff252L, 0xd1bb67f1L, 0xa6bc5767L, 0x3fb506ddL, 0x48b2364bL,
    0xd80d2bdaL, 0xaf0a1b4cL, 0x36034af6L, 0x41047a60L, 0xdf60efc3L, 0xa867df55L, 0x316e8eefL,
    0x4669be79L, 0xcb61b38cL, 0xbc66831aL, 0x256fd2a0L, 0x5268e236L, 0xcc0c7795L, 0xbb0b4703L,
    0x220216b9L, 0x5505262fL, 0xc5ba3bbeL, 0xb2bd0b28L, 0x2bb45a92L, 0x5cb36a04L, 0xc2d7ffa7L,
    0xb5d0cf31L, 0x2cd99e8bL, 0x5bdeae1dL, 0x9b64c2b0L, 0xec63f226L, 0x756aa39cL, 0x026d930aL,
    0x9c0906a9L, 0xeb0e363fL, 0x72076785L, 0x05005713L, 0x95bf4a82L, 0xe2b87a14L, 0x7bb12baeL,
    0x0cb61b38L, 0x92d28e9bL, 0xe5d5be0dL, 0x7cdcefb7L, 0x0bdbdf21L, 0x86d3d2d4L, 0xf1d4e242L,
    0x68ddb3f8L, 0x1fda836eL, 0x81be16cdL, 0xf6b9265bL, 0x6fb077e1L, 0x18b74777L, 0x88085ae6L,
    0xff0f6a70L, 0x66063bcaL, 0x11010b5cL, 0x8f659effL, 0xf862ae69L, 0x616bffd3L, 0x166ccf45L,
    0xa00ae278L, 0xd70dd2eeL, 0x4e048354L, 0x3903b3c2L, 0xa7672661L, 0xd06016f7L, 0x4969474dL,
    0x3e6e77dbL, 0xaed16a4aL, 0xd9d65adcL, 0x40df0b66L, 0x37d83bf0L, 0xa9bcae53L, 0xdebb9ec5L,
    0x47b2cf7fL, 0x30b5ffe9L, 0xbdbdf21cL, 0xcabac28aL, 0x53b39330L, 0x24b4a3a6L, 0xbad03605L,
    0xcdd70693L, 0x54de5729L, 0x23d967bfL, 0xb3667a2eL, 0xc4614ab8L, 0x5d681b02L, 0x2a6f2b94L,
    0xb40bbe37L, 0xc30c8ea1L, 0x5a05df1bL, 0x2d02ef8dL
};

/* ========================================================================= */

#define DO1(buf) crc = crc_table[((int)crc ^ (*buf++)) & 0xff] ^ (crc >> 8);
#define DO2(buf) \
    DO1(buf);    \
    DO1(buf);
#define DO4(buf) \
    DO2(buf);    \
    DO2(buf);
#define DO8(buf) \
    DO4(buf);    \
    DO4(buf);

uint32_t sx_hash_crc32(const void* data, size_t len, uint32_t seed)
{
    const uint8_t* buf = (const uint8_t*)data;
    uint32_t crc = seed ^ 0xffffffffL;

    while (len >= 8) {
        DO8(buf);
        len -= 8;
    }

    while (len--) {
        DO1(buf);
    }

    crc ^= 0xffffffffL;

    return crc;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
// https://probablydance.com/2018/06/16/fibonacci-hashing-the-optimization-that-the-world-forgot-or-a-better-alternative-to-integer-modulo/
static inline uint32_t sx__fib_hash(uint32_t h, int bits)
{
    uint64_t h64 = (uint64_t)h;
    h64 ^= (h64 >> bits);
    return (uint32_t)((h64 * 11400714819323198485llu) >> bits);
}

// https://www.exploringbinary.com/number-of-bits-in-a-decimal-integer/
static inline int sx__calc_bitshift(int n)
{
    int c = 0;
    uint32_t un = (uint32_t)n;
    while (un > 1) {
        c++;
        un >>= 1;
    }

    return 64 - c;
}

sx_hashtbl* sx_hashtbl_create(const sx_alloc* alloc, int capacity)
{
    sx_assert(capacity > 0);

    capacity = sx__nearest_pow2(capacity);
    sx_hashtbl* tbl = (sx_hashtbl*)sx_malloc(
        alloc, sizeof(sx_hashtbl) + capacity * (sizeof(uint32_t) + sizeof(int)) +
                   SX_CONFIG_ALLOCATOR_NATURAL_ALIGNMENT);
    if (!tbl) {
        sx_out_of_memory();
        return NULL;
    }

    tbl->keys = (uint32_t*)sx_align_ptr(tbl + 1, 0, SX_CONFIG_ALLOCATOR_NATURAL_ALIGNMENT);
    tbl->values = (int*)(tbl->keys + capacity);
    tbl->_bitshift = sx__calc_bitshift(capacity);
    tbl->count = 0;
    tbl->capacity = capacity;
#if SX_CONFIG_HASHTBL_DEBUG
    tbl->_miss_cnt = 0;
    tbl->_probe_cnt = 0;
#endif

    // Reset keys
    sx_memset(tbl->keys, 0x0, sizeof(uint32_t) * capacity);

    return tbl;
}

void sx_hashtbl_destroy(sx_hashtbl* tbl, const sx_alloc* alloc)
{
    sx_assert(tbl);
    tbl->count = tbl->capacity = 0;
    sx_free(alloc, tbl);
}

bool sx_hashtbl_grow(sx_hashtbl** ptbl, const sx_alloc* alloc)
{
    sx_hashtbl* tbl = *ptbl;
    // Create a new table (double the size), repopulate it and replace previous one
    sx_hashtbl* new_tbl = sx_hashtbl_create(alloc, tbl->capacity << 1);
    if (!new_tbl)
        return false;

    for (int i = 0, c = tbl->capacity; i < c; i++) {
        if (tbl->keys[i] > 0)
            sx_hashtbl_add(new_tbl, tbl->keys[i], tbl->values[i]);
    }

    sx_hashtbl_destroy(tbl, alloc);
    *ptbl = new_tbl;
    return true;
}

void sx_hashtbl_init(sx_hashtbl* tbl, int capacity, uint32_t* keys_ptr, int* values_ptr)
{
    sx_assert(sx__ispow2(capacity) &&
              "Table size must be power of 2, get it from sx_hashtbl_valid_capacity");

    sx_memset(keys_ptr, 0x0, capacity * sizeof(uint32_t));
    sx_memset(values_ptr, 0x0, capacity * sizeof(int));

    tbl->keys = keys_ptr;
    tbl->values = values_ptr;
    tbl->_bitshift = sx__calc_bitshift(capacity);
    tbl->capacity = capacity;
    tbl->count = 0;
#if SX_CONFIG_HASHTBL_DEBUG
    tbl->_miss_cnt = 0;
    tbl->_probe_cnt = 0;
#endif
}

int sx_hashtbl_fixed_size(int capacity)
{
    int cap = sx_hashtbl_valid_capacity(capacity);
    return cap * (sizeof(uint32_t) + sizeof(int));
}

int sx_hashtbl_valid_capacity(int capacity)
{
    return sx__nearest_pow2(capacity);
}

int sx_hashtbl_add(sx_hashtbl* tbl, uint32_t key, int value)
{
    sx_assert(tbl->count < tbl->capacity);

    uint32_t h = sx__fib_hash(key, tbl->_bitshift);
    uint32_t cnt = (uint32_t)tbl->capacity;
    while (tbl->keys[h] != 0) {
        h = (h + 1) % cnt;
    }

    sx_assert(tbl->keys[h] == 0);    // something went wrong!
    tbl->keys[h] = key;
    tbl->values[h] = value;
    ++tbl->count;
    return h;
}

int sx_hashtbl_find(const sx_hashtbl* tbl, uint32_t key)
{
    uint32_t h = sx__fib_hash(key, tbl->_bitshift);
    uint32_t cnt = (uint32_t)tbl->capacity;
    if (tbl->keys[h] == key) {
        return h;
    } else {
#if SX_CONFIG_HASHTBL_DEBUG
        sx_hashtbl* _tbl = (sx_hashtbl*)tbl;
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

        return -1;    // Worst case: Not found!
    }
}

void sx_hashtbl_clear(sx_hashtbl* tbl)
{
    sx_memset(tbl->keys, 0x0, sizeof(uint32_t) * tbl->capacity);
    tbl->count = 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
sx_hashtbl_tval* sx_hashtbltval_create(const sx_alloc* alloc, int capacity, int value_stride)
{
    sx_assert(capacity > 0);

    capacity = sx__nearest_pow2(capacity);
    sx_hashtbl_tval* tbl = (sx_hashtbl_tval*)sx_malloc(
        alloc, sizeof(sx_hashtbl_tval) + capacity * (sizeof(uint32_t) + value_stride) +
                   SX_CONFIG_ALLOCATOR_NATURAL_ALIGNMENT);
    if (!tbl) {
        sx_out_of_memory();
        return NULL;
    }

    tbl->keys = (uint32_t*)sx_align_ptr(tbl + 1, 0, SX_CONFIG_ALLOCATOR_NATURAL_ALIGNMENT);
    tbl->values = (uint8_t*)(tbl->keys + capacity);     // always 8 byte aligned (sizeof(uint32_t)*pow2)
    tbl->_bitshift = sx__calc_bitshift(capacity);
    tbl->value_stride = value_stride;
    tbl->count = 0;
    tbl->capacity = capacity;
#if SX_CONFIG_HASHTBL_DEBUG
    tbl->_miss_cnt = 0;
    tbl->_probe_cnt = 0;
#endif

    // Reset keys
    sx_memset(tbl->keys, 0x0, sizeof(uint32_t) * capacity);

    return tbl;
}

void sx_hashtbltval_destroy(sx_hashtbl_tval* tbl, const sx_alloc* alloc)
{
    sx_assert(tbl);
    tbl->count = tbl->capacity = 0;
    sx_free(alloc, tbl);
}

bool sx_hashtbltval_grow(sx_hashtbl_tval** ptbl, const sx_alloc* alloc)
{
    sx_hashtbl_tval* tbl = *ptbl;
    // Create a new table (double the size), repopulate it and replace previous one
    sx_hashtbl_tval* new_tbl = sx_hashtbltval_create(alloc, tbl->capacity << 1, tbl->value_stride);
    if (!new_tbl) {
        return false;
    }

    for (int i = 0, c = tbl->capacity; i < c; i++) {
        if (tbl->keys[i] > 0) {
            sx_hashtbltval_add(new_tbl, tbl->keys[i], tbl->values + i*tbl->value_stride);
        }
    }

    sx_hashtbltval_destroy(tbl, alloc);
    *ptbl = new_tbl;
    return true;
}

void sx_hashtbltval_init(sx_hashtbl_tval* tbl, int capacity, int value_stride, uint32_t* keys_ptr, void* values_ptr)
{
    sx_assert(sx__ispow2(capacity) &&
              "Table size must be power of 2, get it from sx_hashtbltval_valid_capacity");

    sx_memset(keys_ptr, 0x0, capacity * sizeof(uint32_t));
    sx_memset(values_ptr, 0x0, capacity * value_stride);

    tbl->keys = keys_ptr;
    tbl->values = values_ptr;
    tbl->_bitshift = sx__calc_bitshift(capacity);
    tbl->value_stride = value_stride;
    tbl->capacity = capacity;
    tbl->count = 0;
#if SX_CONFIG_HASHTBL_DEBUG
    tbl->_miss_cnt = 0;
    tbl->_probe_cnt = 0;
#endif
}

int sx_hashtbltval_fixed_size(int capacity, int value_stride)
{
    int cap = sx_hashtbltval_valid_capacity(capacity);
    return cap * (sizeof(uint32_t) + value_stride);
}

int sx_hashtbltval_valid_capacity(int capacity)
{
    return sx__nearest_pow2(capacity);
}

int sx_hashtbltval_add(sx_hashtbl_tval* tbl, uint32_t key, const void* value)
{
    sx_assert(tbl->count < tbl->capacity);

    uint32_t h = sx__fib_hash(key, tbl->_bitshift);
    uint32_t cnt = (uint32_t)tbl->capacity;
    while (tbl->keys[h] != 0) {
        h = (h + 1) % cnt;
    }

    sx_assert(tbl->keys[h] == 0);    // something went wrong!
    tbl->keys[h] = key;
    sx_memcpy(tbl->values + tbl->value_stride*h, value, tbl->value_stride);
    ++tbl->count;
    return h;
}

int sx_hashtbltval_find(const sx_hashtbl_tval* tbl, uint32_t key)
{
    uint32_t h = sx__fib_hash(key, tbl->_bitshift);
    uint32_t cnt = (uint32_t)tbl->capacity;
    if (tbl->keys[h] == key) {
        return h;
    } else {
#if SX_CONFIG_HASHTBL_DEBUG
        sx_hashtbl_tval* _tbl = (sx_hashtbl_tval*)tbl;
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

        return -1;    // Worst case: Not found!
    }
}

void sx_hashtbltval_clear(sx_hashtbl_tval* tbl)
{
    sx_memset(tbl->keys, 0x0, sizeof(uint32_t) * tbl->capacity);
    tbl->count = 0;
}
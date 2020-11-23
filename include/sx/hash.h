//
// Copyright 2018 Sepehr Taghdisian (septag@github). All rights reserved.
// License: https://github.com/septag/sx#license-bsd-2-clause
//
// hash.h - v1.0 - Some nice hash functions and a fast hash-table
// Reference for hash funcs: https://aras-p.info/blog/2016/08/02/Hash-Functions-all-the-way-down/
//
// Hashing functions (Non-cryptographic):
//      sx_hash_xxh32                 xxhash32 - suitable for smaller data, and it's faster in all
//                                               situations
//      sx_hash_xxh64                 xxhash64 - suitable for bigger data, collision of this is
//                                               almost zero
//      sx_hash_fnv32                 Fnv1a hashing, fast and robust for small data (<64 bytes)
//      sx_hash_fnv32_str             utility function for hashing null-terminated strings
//      sx_hash_crc32                 famous CRC32 hash, suitable for files and data that need CRC32
//                                    hash export
//      sx_hash_u32                   bit-shift hashing of uint32, suitable for pushing indexes
//                                    into hash table
//      sx_hash_u64                   bit-shift hashing of uint64, suitable for hashing pointers
//      sx_hash_u64_to_u32            bit-shift hashing of uint64 to uint32, suitable for 64bit
//                                    hashing of pointers
//
//      sx_hash_create_xxh32          create xxh32 hashing state for stream hashing
//      sx_hash_destroy_xxh32         destroy xxh32 hash state
//      sx_hash_xxh32_init            reset and init hash state, do this on the start of every
//                                    stream hash
//      sx_hash_xxh32_update          feed data into hash state
//      sx_hash_xxh32_digest          digest and calculate xxh32 hash of fed data
//
// XXHash Source: https://github.com/Cyan4973/xxHash
// The functions above all have 64bit versions... sx_hash_create_xxh64, etc.
//
// sx_hashtbl: hash-table based on fibonacci mult hashing
//             Reference:
//             https://probablydance.com/2018/06/16/fibonacci-hashing-the-optimization-that-the-world-forgot-or-a-better-alternative-to-integer-modulo/
//
//      sx_hashtbl_create            create and allocate hash-table from allocator,
//                                   capacity will be rounded to power of 2
//      sx_hashtbl_destroy           destroy hash-table that is created with sx_hashtbl_create
//                                   (DO NOT use this on sx_hashtbl_init type of tables)
//      sx_hashtbl_grow              grows hash-table that is created with sx_hashtbl_grow,
//                                   NOTE: pointer to hash-table will change after grow,
//                                         so beware if the table is used in multiple locations
//                                   alloc must be same as the one in create call (DO NOT use this
//                                   on sx_hashtbl_init type of tables)
//      sx_hashtbl_init              initialize the hash-table with pre-allocated memory
//                                          size/capacity of 'keys' and 'values' array must be the
//                                          same, and must be fetched from sx_hashtbl_valid_capacity
//                                          if initialized with `sx_hashtbl_init` the user should
//                                          not call sx_hashtbl_destroy and free the pointers
//      sx_hashtbl_valid_capacity    returns a valid/corrected hash-table capacity from the input,
//                                   use this to pass a valid capacity to sx_hashtbl_init
//      sx_hashtbl_fixed_size        returns size of a fixed size hash-table buffers in bytes
//                                   can be used to allocate internal buffers manually for use in
//                                   sx_hashtbl_init function
//      sx_hashtbl_add               adds a key to the table
//      sx_hashtbl_remove            removes a key from table
//      sx_hashtbl_full              returns true if table is full
//      sx_hashtbl_get               returns the value of an index, similiar to tbl->values[index]
//      sx_hashtbl_find              tries to find the key and returns index, -1 if not found
//                                   NOTE: non-existent keys are the worst case and cause
//                                   performance
//                                         issues. so beware of such scenarios and try to use
//                                         something else if this case happens alot
//      sx_hashtbl_find_get          combines 'find' and 'get', so it returns the actual value based
//                                   on key returns the parameter 'not_found_val' if key is not
//                                   found in table
//      sx_hashtbl_clear             clears the table
// 
// sx_hashtbl_tval: hash-table based on fibonacci mult hashing, but with arbitary value types
//                  the difference between this and norml sx_hashtbl is that the 'value' type is not
//                  integer anymore. It can be any POD type. you just define the size of your type
//      The function are pretty much the same as sx_hashtbl, but with `sx_hashtbltval_` prefix.
//
// NOTE for C++ users:
//      Take a look at `sx_hashtable_t` struct and it's members (C++ only) in this file. It is a very 
//      thin template wrapper over sx_hashtbl_tval for more convenient C++ usage
//
#pragma once

#include "sx.h"

typedef struct sx_alloc sx_alloc;

// XXH hash, we have both 32bit and 64bit versions
// XXH32: suitable for smaller data, and it's faster in all situations
// XXH64: suitable for bigger data, collision of this is almost zero
SX_API uint32_t sx_hash_xxh32(const void* data, size_t len, uint32_t seed);
SX_API uint64_t sx_hash_xxh64(const void* data, size_t len, uint64_t seed);

// FNV1a: suitable for small data (usually less than 32 bytes), mainly small strings
SX_INLINE SX_CONSTEXPR uint32_t sx_hash_fnv32(const void* data, size_t len);
SX_INLINE SX_CONSTEXPR uint32_t sx_hash_fnv32_str(const char* str);

// CRC32: Pretty standard hash, mainly used for files
SX_API uint32_t sx_hash_crc32(const void* data, size_t len, uint32_t seed);

// Integer hash functions: useful for pointer/index hashing
// Reference: https://gist.github.com/badboy/6267743
SX_INLINE SX_CONSTEXPR uint32_t sx_hash_u32(uint32_t key);
SX_INLINE SX_CONSTEXPR uint64_t sx_hash_u64(uint64_t key);
SX_INLINE SX_CONSTEXPR uint32_t sx_hash_u64_to_u32(uint64_t key);

// Streaming (state based) hash using xxhash32
typedef struct sx_hash_xxh32 sx_hash_xxh32_t;

SX_API sx_hash_xxh32_t* sx_hash_create_xxh32(const sx_alloc* alloc);
SX_API void sx_hash_destroy_xxh32(sx_hash_xxh32_t* state, const sx_alloc* alloc);
SX_API void sx_hash_xxh32_init(sx_hash_xxh32_t* state, uint32_t seed);
SX_API void sx_hash_xxh32_update(sx_hash_xxh32_t* state, const void* data, size_t len);
SX_API uint32_t sx_hash_xxh32_digest(sx_hash_xxh32_t* state);

// Streaming (state based) hash using xxhash64
typedef struct sx_hash_xxh64_s sx_hash_xxh64_t;

SX_API sx_hash_xxh64_t* sx_hash_create_xxh64(const sx_alloc* alloc);
SX_API void sx_hash_destroy_xxh64(sx_hash_xxh64_t* state, const sx_alloc* alloc);
SX_API void sx_hash_xxh64_init(sx_hash_xxh64_t* state, uint64_t seed);
SX_API void sx_hash_xxh64_update(sx_hash_xxh64_t* state, const void* data, size_t len);
SX_API uint64_t sx_hash_xxh64_digest(sx_hash_xxh64_t* state);

////////////////////////////////////////////////////////////////////////////////////////////////////
// Hash table
typedef struct sx_hashtbl {
    uint32_t* keys;
    int* values;
    int _bitshift;
    int count;
    int capacity;
#if SX_CONFIG_HASHTBL_DEBUG
    int _miss_cnt;
    int _probe_cnt;
#endif
} sx_hashtbl;

SX_API sx_hashtbl* sx_hashtbl_create(const sx_alloc* alloc, int capacity);
SX_API void sx_hashtbl_destroy(sx_hashtbl* tbl, const sx_alloc* alloc);
SX_API bool sx_hashtbl_grow(sx_hashtbl** ptbl, const sx_alloc* alloc);

SX_API void sx_hashtbl_init(sx_hashtbl* tbl, int capacity, uint32_t* keys_ptr, int* values_ptr);
SX_API int sx_hashtbl_valid_capacity(int capacity);
SX_API int sx_hashtbl_fixed_size(int capacity);

SX_API int sx_hashtbl_add(sx_hashtbl* tbl, uint32_t key, int value);
SX_API int sx_hashtbl_find(const sx_hashtbl* tbl, uint32_t key);
SX_API void sx_hashtbl_clear(sx_hashtbl* tbl);

SX_INLINE int sx_hashtbl_get(const sx_hashtbl* tbl, int index)
{
    sx_assert(index >= 0 && index < tbl->capacity);
    return tbl->values[index];
}

SX_INLINE int sx_hashtbl_find_get(const sx_hashtbl* tbl, uint32_t key, int not_found_val)
{
    int index = sx_hashtbl_find(tbl, key);
    return index != -1 ? tbl->values[index] : not_found_val;
}

SX_INLINE void sx_hashtbl_remove(sx_hashtbl* tbl, int index)
{
    sx_assert(index >= 0 && index < tbl->capacity);

    tbl->keys[index] = 0;
    --tbl->count;
}

SX_INLINE void sx_hashtbl_remove_if_found(sx_hashtbl* tbl, uint32_t key)
{
    int index = sx_hashtbl_find(tbl, key);
    if (index != -1)
        sx_hashtbl_remove(tbl, index);
}


SX_INLINE bool sx_hashtbl_full(const sx_hashtbl* tbl)
{
    return tbl->capacity == tbl->count;
}

#define sx_hashtbl_add_and_grow(_tbl, _key, _value, _alloc)        \
    (sx_hashtbl_full(_tbl) ? sx_hashtbl_grow(&(_tbl), _alloc) : 0, \
     sx_hashtbl_add(_tbl, _key, _value))

////////////////////////////////////////////////////////////////////////////////////////////////////
// Hash table
typedef struct sx_hashtbl_tval {
    uint32_t* keys;
    uint8_t* values;
    int _bitshift;
    int value_stride;
    int count;
    int capacity;
#if SX_CONFIG_HASHTBL_DEBUG
    int _miss_cnt;
    int _probe_cnt;
#endif
} sx_hashtbl_tval;

SX_API sx_hashtbl_tval* sx_hashtbltval_create(const sx_alloc* alloc, int capacity, int value_stride);
SX_API void sx_hashtbltval_destroy(sx_hashtbl_tval* tbl, const sx_alloc* alloc);
SX_API bool sx_hashtbltval_grow(sx_hashtbl_tval** ptbl, const sx_alloc* alloc);

SX_API void sx_hashtbltval_init(sx_hashtbl_tval* tbl, int capacity, int value_stride, uint32_t* keys_ptr, void* values_ptr);
SX_API int sx_hashtbltval_valid_capacity(int capacity);
SX_API int sx_hashtbltval_fixed_size(int capacity, int value_stride);

SX_API int sx_hashtbltval_add(sx_hashtbl_tval* tbl, uint32_t key, const void* value);
SX_API int sx_hashtbltval_find(const sx_hashtbl_tval* tbl, uint32_t key);
SX_API void sx_hashtbltval_clear(sx_hashtbl_tval* tbl);

SX_INLINE const void* sx_hashtbltval_get(const sx_hashtbl_tval* tbl, int index) 
{
    sx_assert(index >= 0);
    sx_assert(index < tbl->capacity);

    return (const void*)(tbl->values + index*tbl->value_stride);
}

SX_INLINE const void* sx_hashtbltval_find_get(const sx_hashtbl_tval* tbl, uint32_t key, const void* not_found_val)
{
    int index = sx_hashtbltval_find(tbl, key);
    return index != -1 ? (const void*)(tbl->values + tbl->value_stride*index) : not_found_val;
}

SX_INLINE void sx_hashtbltval_remove(sx_hashtbl_tval* tbl, int index)
{
    sx_assert(index >= 0 && index < tbl->capacity);

    tbl->keys[index] = 0;
    --tbl->count;
}

SX_INLINE void sx_hashtbltval_remove_if_found(sx_hashtbl_tval* tbl, uint32_t key)
{
    int index = sx_hashtbltval_find(tbl, key);
    if (index != -1) {
        sx_hashtbltval_remove(tbl, index);
    }
}

SX_INLINE bool sx_hashtbltval_full(const sx_hashtbl_tval* tbl)
{
    return tbl->capacity == tbl->count;
}

#define sx_hashtbltval_add_and_grow(_tbl, _key, _value, _alloc)        \
    (sx_hashtbltval_full(_tbl) ? sx_hashtbltval_grow(&(_tbl), _alloc) : 0, \
     sx_hashtbltval_add(_tbl, _key, _value))

// cplusplus minimal template wrapper over hashtbltval
#ifdef __cplusplus
template <typename _T>
struct sx_hashtable_t
{
private:
    sx_hashtbl_tval* ht;
    const sx_alloc* alloc;

public:
    sx_hashtable_t();
    ~sx_hashtable_t();

    bool init(const sx_alloc* alloc, int capacity);
    void release();

    int add(uint32_t key, const _T& value);
    int find(uint32_t key) const;
    void clear();

    const _T& get(int index) const;
    void remove(int index);

    const _T& find_get(uint32_t key, const _T& not_found_value = {0}) const;
    void find_remove(uint32_t key);

    int capacity() const;
    int count() const;
    const uint32_t* keys() const;
    const _T* values() const;
};


////////////////////////////////////////////////////////////////////////////////////////////////////
// impl

template <typename _T>
const _T* sx_hashtable_t<_T>::values() const
{
    return reinterpret_cast<const _T*>(this->ht->values);
}

template <typename _T>
const uint32_t* sx_hashtable_t<_T>::keys() const
{
    return this->ht->keys;
}

template <typename _T>
int sx_hashtable_t<_T>::count() const
{
    return this->ht->count;
}

template <typename _T>
int sx_hashtable_t<_T>::capacity() const
{
    return this->ht->capacity;
}

template <typename _T>
const _T& sx_hashtable_t<_T>::get(int index) const
{
    return *reinterpret_cast<_T*>(this->ht->values + index*this->ht->value_stride);
}


template <typename _T>
void sx_hashtable_t<_T>::remove(int index)
{
    sx_assert_alwaysf(index >= 0 && index < this->ht->capacity, "index out of range");

    this->ht->keys[index] = 0;
    --this->ht->count;
}

template <typename _T>
void sx_hashtable_t<_T>::clear()
{
    sx_hashtbltval_clear(this->ht);
}

template <typename _T>
int sx_hashtable_t<_T>::find(uint32_t key) const 
{
    return sx_hashtbltval_find(this->ht, key);
}

template <typename _T>
int sx_hashtable_t<_T>::add(uint32_t key, const _T& value)
{
    if (this->ht->count == this->ht->capacity) {
        sx_assert(this->alloc);
        bool r = sx_hashtbltval_grow(&this->ht, this->alloc);
        sx_unused(r);
        sx_assert_alwaysf(r, "could not grow hash-table");
    }
    sx_hashtbltval_add(this->ht, key, &value);
}

template <typename _T>
void sx_hashtable_t<_T>::release()
{
    if (this->ht) {
        sx_hashtbltval_destroy(this->ht, this->alloc);
        this->ht = nullptr;
    }
    this->alloc = nullptr;
}

template <typename _T>
bool sx_hashtable_t<_T>::init(const sx_alloc* _alloc, int capacity)
{
    sx_assert_alwaysf(!this->ht, "hash-table already initialized");
    this->ht = sx_hashtbltval_create(_alloc, capacity, (int)sizeof(_T));
    this->alloc = _alloc;
    return this->ht ? true : false;
}

template <typename _T>
sx_hashtable_t<_T>::~sx_hashtable_t()
{
    release();
}

template <typename _T>
sx_hashtable_t<_T>::sx_hashtable_t()
{
    this->ht = nullptr;
    this->alloc = nullptr;
}

template <typename _T>
const _T& sx_hashtable_t<_T>::find_get(uint32_t key, const _T& not_found_value /*= {0}*/) const
{
    int index = sx_hashtbltval_find(this->ht, key);
    return index != -1 ? this->get(index) : not_found_value;
}

template <typename _T>
void sx_hashtable_t<_T>::find_remove(uint32_t key)
{
    int index = sx_hashtbltval_find(this->ht, key);
    if (index != -1) {
        this->remove(index);
    }
}

#endif // __cplusplus

////////////////////////////////////////////////////////////////////////////////////////////////////
// FNV1a
// http://www.isthe.com/chongo/src/fnv/hash_32a.c

#define FNV1_32_INIT 0x811c9dc5
#define FNV1_32_PRIME 0x01000193
SX_INLINE SX_CONSTEXPR uint32_t sx_hash_fnv32(const void* data, size_t len)
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

SX_INLINE SX_CONSTEXPR uint32_t sx_hash_fnv32_str(const char* str)
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
SX_INLINE SX_CONSTEXPR uint32_t sx_hash_u32(uint32_t key)
{
    const uint32_t c2 = 0x27d4eb2d;    // a prime or an odd constant
    key = (key ^ 61) ^ (key >> 16);
    key = key + (key << 3);
    key = key ^ (key >> 4);
    key = key * c2;
    key = key ^ (key >> 15);
    return key;
}

SX_INLINE SX_CONSTEXPR uint64_t sx_hash_u64(uint64_t key)
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

SX_INLINE SX_CONSTEXPR uint32_t sx_hash_u64_to_u32(uint64_t key)
{
    key = (~key) + (key << 18);
    key = key ^ (key >> 31);
    key = key * 21;
    key = key ^ (key >> 11);
    key = key + (key << 6);
    key = key ^ (key >> 22);
    return (uint32_t)key;
}
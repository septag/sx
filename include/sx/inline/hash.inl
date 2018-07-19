#ifndef SX_HASH_H_
#   error "This file must only be included from hash.h"
#   include "../hash.h"
#endif

inline int sx_hashtbl_find_get(const sx_hashtbl* tbl, uint32_t key, int not_found_val)
{
    int index = sx_hashtbl_find(tbl, key);
    return index != -1 ? tbl->values[index] : not_found_val;
}

inline void sx_hashtbl_remove(sx_hashtbl* tbl, int index)
{
    assert(index>=0 && index<tbl->capacity);

    tbl->keys[index] = 0;
    --tbl->count;    
}

inline bool sx_hashtbl_full(const sx_hashtbl* tbl)
{
    return tbl->capacity == tbl->count;
}

inline int sx_hashtbl_get(const sx_hashtbl* tbl, int index)
{
    assert(index>=0 && index<tbl->capacity);
    return tbl->values[index];
}
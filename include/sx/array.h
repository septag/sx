//
// Copyright 2018 Sepehr Taghdisian (septag@github). All rights reserved.
// License: https://github.com/septag/sx#license-bsd-2-clause
//
// array.h - v1.0 - Dynamic arrays
// Source: https://github.com/nothings/stb/blob/master/stretchy_buffer.h
//
// clang-format off
//    Unlike C++ vector<>, the sx_array has the same
//    semantics as an object that you manually malloc and realloc.
//    The pointer may relocate every time you add a new object
//    to it, so you:
//
//         1. can't take long-term pointers to elements of the array
//         2. have to return the pointer from functions which might expand it
//            (either as a return value or by storing it to a ptr-to-ptr)
//
//    Now you can do the following things with this array:
//
//         sx_array_free(sx_alloc* alloc, TYPE *a)          free the array
//         sx_array_count(TYPE *a)                          the number of elements in the array
//         sx_array_push(sx_alloc* alloc, TYPE *a, TYPE v)  adds v on the end of the array, a la push_back
//         sx_array_add(sx_alloc* alloc, TYPE *a, int n)    adds n uninitialized elements at end of array & returns pointer to first added
//         sx_array_last(TYPE *a)                           returns an lvalue of the last item in the array
//         sx_array_pop(TYPE *a, index)                     removes an element from the array and decreases array count
//         sx_array_pop_last(TYPE *a)                       removes last element from the array and decreases array count
//         a[n]                                             access the nth (counting from 0) element of the array
//         sx_array_clear(TYPE* a)                          resets the array count to zero, but does not resize memory
//         sx_array_reserve(sx_alloc* alloc, TYPE* a, int n) reserves N elements in array without incrementing the count
//         sx_array_push_byindex(alloc, TYPE* a, v, index)  receives index, if index is within the array, sets the array element to the value
//                                                          if not, pushes the element into the array
// Usage:
//       NOTE: include "allocator.h" before array.h to prevent warnings and errors
//       sx_array_declare(SomeStruct) = NULL; // SomeStruct *arr = NULL;
//       while (something)
//       {
//          SomeStruct new_one;
//          new_one.whatever = whatever;
//          new_one.whatup   = whatup;
//          new_one.foobar   = barfoo;
//          sx_array_push(arr, new_one);
//       }
//
//     Note that these are all macros and many of them evaluate
//     their arguments more than once (except for 'v' in sx_array_push), so the arguments should
//     be side-effect-free.
//
//     Note that 'TYPE *a' in sx_array_push and sx_array_add must be lvalues
//     so that the library can overwrite the existing pointer if the object has to be reallocated.
//
// NOTE for C++ users:
//     take a look at `sx_array` struct and it's members at the end of this file. It's a thing template 
//     wrapper over array macros for more convenient C++ usage
//
#pragma once

typedef struct sx_alloc sx_alloc;

#define sx_array_free(_alloc, a) ((a) ? sx_free(_alloc, sx__sbraw(a)), 0 : 0)
#define sx_array_push(_alloc, a, v) (sx__sbmaybegrow(_alloc, a, 1), (a)[sx__sbn(a)++] = (v))
#define sx_array_count(a) ((a) ? sx__sbn(a) : 0)
#define sx_array_add(_alloc, a, n) (sx__sbmaybegrow(_alloc, a, n), sx__sbn(a) += (n), &(a)[sx__sbn(a) - (n)])
#define sx_array_last(a) ((a)[sx__sbn(a) - 1])
#define sx_array_pop(a, idx)  do { (a)[idx] = sx_array_last(a);  --sx__sbn(a); } while (0)
#define sx_array_pop_last(a)  do { --sx__sbn(a); } while (0)
#define sx_array_pop_lastn(a, n) do { sx__sbn(a) -= (n); } while (0)
#define sx_array_clear(a) ((a) ? (sx__sbn(a) = 0) : 0)
#define sx_array_reserve(_alloc, a, n) (sx_array_add(_alloc, a, n), sx_array_clear(a))
#define sx_array_push_byindex(_alloc, a, v, _index) \
    do {                                            \
        if ((_index) >= sx_array_count(a))          \
            sx_array_push(_alloc, a, v);            \
        else                                        \
            (a)[(_index)] = (v);                    \
    } while (0)

////////////////////////////////////////////////////////////////////////////////////////////////////
// Internal
#define sx__sbraw(a) ((int*)(a)-2)
#define sx__sbm(a) sx__sbraw(a)[0]
#define sx__sbn(a) sx__sbraw(a)[1]

#define sx__sbneedgrow(a, n) ((a) == 0 || sx__sbn(a) + (n) >= sx__sbm(a))
#define sx__sbmaybegrow(_alloc, a, n) (sx__sbneedgrow(a, (n)) ? sx__sbgrow(_alloc, a, n) : 0)
#define sx__sbgrow(_alloc, a, n)  (*((void**)&(a)) = sx__sbgrowf((a), (n), sizeof(*(a)), (_alloc), __FILE__, __FUNCTION__, __LINE__))

SX_INLINE void* sx__sbgrowf(void* arr, int increment, int itemsize, const sx_alloc* alloc,
                            const char* file, const char* func, int line)
{
    int new_count = arr ? (sx__sbm(arr) << 1) : 0;
    new_count = new_count > SX_CONFIG_ARRAY_INIT_SIZE ? new_count : SX_CONFIG_ARRAY_INIT_SIZE;
    int min_needed = sx_array_count(arr) + increment;
    int m = new_count > min_needed ? new_count : min_needed;
    int* p = (int*)sx__realloc(alloc, arr ? sx__sbraw(arr) : 0,
                               (size_t)itemsize*(size_t)m + sizeof(int)*2, 0, file, func, line);

    if (p) {
        p[0] = m;
        if (!arr)
            p[1] = 0;
        return p + 2;
    } else {
        sx_out_of_memory();
        return 0x0;    // NULL
    }
}

// cpp wrapper (minimal template)
#ifdef __cplusplus
template <typename _T>
struct sx_array 
{
    _T* p;
    const sx_alloc* alloc;

    sx_array() { p = nullptr; alloc = nullptr; }
    explicit sx_array(const sx_alloc* _alloc) : alloc(_alloc), p(nullptr) {} 

    ~sx_array() 
    { 
        if (alloc) {
            sx_array_free(alloc, p); 
            alloc = nullptr;
        }
        p = nullptr;
    }

    void init(const sx_alloc* _alloc, int init_count = 0)
    {
        sx_assert(_alloc);
        this->alloc = _alloc;
        if (init_count > 0) {
            sx_array_reserve(_alloc, p, init_count);
        }
    }

    void release()
    {
        sx_assert(alloc);
        sx_array_free(alloc, p);
        p = nullptr;
        alloc = nullptr;
    }

    void push(const _T& _value) 
    {
        sx_assert(alloc);
        sx_array_push(alloc, p, _value);
    }

    void pop(int _index)
    {
        sx_assert(alloc);
        sx_assert(_index < sx_array_count(p));
        sx_array_pop(alloc, _index);
    }

    void pop_last()
    {
        sx_assert(alloc);
        sx_assert(sx_array_count(p));
        sx_array_pop_last(p);
    }

    void clear()
    {
        sx_assert(alloc);
        sx_array_clear(p);
    }

    int count() const 
    {
        sx_assert(alloc);
        return sx_array_count(p);
    }

    _T* expand(int _count) 
    {
        sx_assert(alloc);
        return sx_array_add(alloc, p, _count);
    }

    void reserve(int _count)
    {
        sx_assert(alloc);
        sx_array_reserve(alloc, p, _count);
    }

    _T& operator[](int _index)
    {
        sx_assert(p);
        return this->p[_index];
    }

    const _T& operator[](int index) const
    {
        sx_assert(p);
        return this->p[index];
    }

    _T* begin() { return &p[0]; }

    _T* end() { return &p[count()]; }

};
#endif // __cplusplus

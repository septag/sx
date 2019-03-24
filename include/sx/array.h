//
// Copyright 2018 Sepehr Taghdisian (septag@github). All rights reserved.
// License: https://github.com/septag/sx#license-bsd-2-clause
//
// array.h - v1.0 - Dynamic arrays
// Source: https://github.com/nothings/stb/blob/master/stretchy_buffer.h
//
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
//         sx_array_free(sx_alloc* alloc, TYPE *a)
//             free the array
//         sx_array_count(TYPE *a)
//             the number of elements in the array
//         sx_array_push(sx_alloc* alloc, TYPE *a, TYPE v)
//             adds v on the end of the array, a la push_back
//         sx_array_add(sx_alloc* alloc, TYPE *a, int n)
//             adds n uninitialized elements at end of array & returns pointer to first added
//         sx_array_last(TYPE *a)
//             returns an lvalue of the last item in the array
//         sx_array_pop(TYPE *a, index)
//             removes an element from the array and decreases array count
//         sx_array_pop_last(TYPE *a)
//             removes last element from the array and decreases array count
//         a[n]
//             access the nth (counting from 0) element of the array
//         sx_array_clear(TYPE* a)
//             resets the array count to zero, but does not resize memory
//         sx_array_reserve(sx_alloc* alloc, TYPE* a, int n)
//             reserves N elements in array without incrementing the count
// Usage:
//       NOTE: include "allocator.h" before array.h to prevent warnings and errors
//       SomeStruct *arr = NULL;
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

#pragma once

typedef struct sx_alloc sx_alloc;

#define sx_array_free(_alloc, a) ((a) ? sx_free(_alloc, sx__sbraw(a)), 0 : 0)
#define sx_array_push(_alloc, a, v) (sx__sbmaybegrow(_alloc, a, 1), (a)[sx__sbn(a)++] = (v))
#define sx_array_count(a) ((a) ? sx__sbn(a) : 0)
#define sx_array_add(_alloc, a, n) \
    (sx__sbmaybegrow(_alloc, a, n), sx__sbn(a) += (n), &(a)[sx__sbn(a) - (n)])
#define sx_array_last(a) ((a)[sx__sbn(a) - 1])
#define sx_array_pop(a, idx)         \
    do {                             \
        (a)[idx] = sx_array_last(a); \
        --sx__sbn(a);                \
    } while (0)
#define sx_array_pop_last(a) \
    do {                     \
        --sx__sbn(a);        \
    } while (0)
#define sx_array_pop_lastn(a, n) \
    do {                         \
        sx__sbn(a) -= (n);       \
    } while (0)
#define sx_array_clear(a) ((a) ? (sx__sbn(a) = 0) : 0)
#define sx_array_reserve(_alloc, a, n) (sx_array_add(_alloc, a, n), sx_array_clear(a))

////////////////////////////////////////////////////////////////////////////////////////////////////
// Internal
#define sx__sbraw(a) ((int*)(a)-2)
#define sx__sbm(a) sx__sbraw(a)[0]
#define sx__sbn(a) sx__sbraw(a)[1]

#define sx__sbneedgrow(a, n) ((a) == 0 || sx__sbn(a) + (n) >= sx__sbm(a))
#define sx__sbmaybegrow(_alloc, a, n) (sx__sbneedgrow(a, (n)) ? sx__sbgrow(_alloc, a, n) : 0)
#define sx__sbgrow(_alloc, a, n) \
    (*((void**)&(a)) =           \
         sx__sbgrowf((a), (n), sizeof(*(a)), (_alloc), __FILE__, __FUNCTION__, __LINE__))

static inline void* sx__sbgrowf(void* arr, int increment, int itemsize, const sx_alloc* alloc,
                                const char* file, const char* func, int line) {
    int  dbl_cur = arr ? (sx__sbm(arr) << 1) : 0;
    int  min_needed = sx_array_count(arr) + increment;
    int  m = dbl_cur > min_needed ? dbl_cur : min_needed;
    int* p = (int*)sx__realloc(alloc, arr ? sx__sbraw(arr) : 0, itemsize * m + sizeof(int) * 2, 0,
                               file, func, line);
    if (p) {
        if (!arr)
            p[1] = 0;
        p[0] = m;
        return p + 2;
    } else {
        sx_out_of_memory();
        return 0x0;    // NULL
    }
}

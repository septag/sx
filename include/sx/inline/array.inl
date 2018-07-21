#ifndef SX_ARRAY_H_
#   error "This file can only be included from array.h"
#endif

#define _sx_sbraw(a) ((int *) (a) - 2)
#define _sx_sbm(a)   _sx_sbraw(a)[0]
#define _sx_sbn(a)   _sx_sbraw(a)[1]

#define _sx_sbneedgrow(a,n)         ((a)==0 || _sx_sbn(a)+(n) >= _sx_sbm(a))
#define _sx_sbmaybegrow(_alloc,a,n) (_sx_sbneedgrow(a,(n)) ? _sx_sbgrow(_alloc,a,n) : 0)
#define _sx_sbgrow(_alloc,a,n)      (*((void **)&(a)) = _sx_sbgrowf((a), (n), sizeof(*(a)), (_alloc)))

static inline void* _sx_sbgrowf(void* arr, int increment, int itemsize, const sx_alloc* alloc)
{
   int dbl_cur = arr ? (_sx_sbm(arr)<<1) : 0;
   int min_needed = sx_array_count(arr) + increment;
   int m = dbl_cur > min_needed ? dbl_cur : min_needed;
   int* p = (int*) sx_realloc(alloc, arr ? _sx_sbraw(arr) : 0, itemsize*m + sizeof(int)*2);
   if (p) {
        if (!arr)
            p[1] = 0;
        p[0] = m;
        return p+2;
   } else {
        SX_OUT_OF_MEMORY;
        return NULL;
   }
}

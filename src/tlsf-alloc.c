#include "sx/tlsf-alloc.h"

#define TLSF_PRIVATE_API
#define tlsf_assert sx_assert
SX_PRAGMA_DIAGNOSTIC_PUSH();
SX_PRAGMA_DIAGNOSTIC_IGNORED_CLANG_GCC("-Wunknown-pragmas")
#include "../3rdparty/tlsf/tlsf.h"
SX_PRAGMA_DIAGNOSTIC_POP();

static void* sx__tlsfalloc_cb(void* ptr, size_t size, size_t align, const char* file, uint32_t line, void* user_data)
{
    tlsf_t tlsf = (tlsf_t)user_data;
    if (size == 0) {
        tlsf_free(tlsf, ptr);
        return NULL;
    } else if (ptr == NULL) {
        if (align <= SX_CONFIG_ALLOCATOR_NATURAL_ALIGNMENT)
            return tlsf_malloc(tlsf, size);

        return tlsf_memalign(tlsf, align, size);
    } else {
		if (align <= SX_CONFIG_ALLOCATOR_NATURAL_ALIGNMENT)
			return tlsf_realloc(tlsf, ptr, size);

        tlsf_free(tlsf, ptr);
		return tlsf_memalign(tlsf, align, size);
    }    
}

bool sx_tlsfalloc_init(sx_alloc* tlsf, void* mem, int size)
{
    tlsf->alloc_cb = sx__tlsfalloc_cb;
    tlsf->user_data = tlsf_create_with_pool(mem, size);
    if (!tlsf->user_data)
        return false;

    return true;
}

void sx_tlsfalloc_release(sx_alloc* tlsf)
{
    tlsf_destroy((tlsf_t)tlsf->user_data);
    tlsf->user_data = NULL;
    tlsf->alloc_cb = NULL;
}

sx_tlsf_pool_t sx_tlsfalloc_add_pool(sx_alloc* tlsf, void* mem, int size)
{
    return tlsf_add_pool((tlsf_t)tlsf->user_data, mem, size);
}

void sx_tlsfalloc_remove_pool(sx_alloc* tlsf, sx_tlsf_pool_t pool)
{
    tlsf_remove_pool((tlsf_t)tlsf->user_data, pool);
}

sx_tlsf_pool_t sx_tlsfalloc_get_pool(sx_alloc* tlsf)
{
    return tlsf_get_pool((tlsf_t)tlsf->user_data);
}

void sx_tlsfalloc_walk_pool(sx_tlsf_pool_t pool, sx_tlsfalloc_walk_cb* walker, void* user)
{
    tlsf_walk_pool((pool_t)pool, (tlsf_walker)walker, user);
}

bool sx_tlsfalloc_check(sx_alloc* tlsf)
{
    return tlsf_check((tlsf_t)tlsf->user_data) == 0;
}

bool sx_tlsfalloc_check_pool(sx_tlsf_pool_t pool)
{
    return tlsf_check_pool((pool_t)pool) == 0;
}

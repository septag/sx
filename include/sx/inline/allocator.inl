#ifndef SX_ALLOCATOR_H_
#   error "This file must only be included from allocator.h"
#endif

inline bool sx_is_aligned(const void* ptr, size_t align)
{
    union { const void* ptr; uintptr_t addr; } un;
    un.ptr = ptr;
    return 0 == (un.addr & (align-1));    
}

static inline void* sx_align_ptr(void* ptr, size_t extra, size_t align)
{
    union { void* ptr; uintptr_t addr; } un;
    un.ptr = ptr;
    uintptr_t unaligned = un.addr + extra; // space for header
    uintptr_t mask = align-1;
    uintptr_t aligned = sx_align_mask(unaligned, mask);
    un.addr = aligned;
    return un.ptr;    
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Internal
static inline void* sx__malloc(const sx_alloc* alloc, size_t size, size_t align, const char* file, uint32_t line) 
{
    return alloc->alloc_cb(NULL, size, align, file, line, alloc->user_data);
}

static inline void sx__free(const sx_alloc* alloc, void* ptr, size_t align, const char* file, uint32_t line)
{
    alloc->alloc_cb(ptr, 0, align, file, line, alloc->user_data);
}

static inline void* sx__realloc(const sx_alloc* alloc, void* ptr, size_t size, size_t align, const char* file, uint32_t line)
{
    return alloc->alloc_cb(ptr, size, align, file, line, alloc->user_data);
}

static inline void* sx__aligned_alloc(const sx_alloc* alloc, size_t size, size_t align, const char* file, uint32_t line)
{
    align = sx_max((int)align, SX_CONFIG_ALLOCATOR_NATURAL_ALIGNMENT);
    const size_t total = size + align + sizeof(uint32_t);
    uint8_t* ptr = (uint8_t*)sx__malloc(alloc, total, 0, file, line);
    uint8_t* aligned = (uint8_t*)sx_align_ptr(ptr, sizeof(uint32_t), align);
    uint32_t* header = (uint32_t*)aligned - 1;
    *header = (uint32_t)(aligned - ptr);
    return aligned;
}

static inline void sx__aligned_free(const sx_alloc* alloc, void* ptr, size_t align, const char* file, uint32_t line)
{
    uint8_t* aligned = (uint8_t*)ptr;
    uint32_t* header = (uint32_t*)aligned - 1;
    ptr = aligned - *header;
    sx__free(alloc, ptr, 0, file, line);    
}

static inline void* sx__aligned_realloc(const sx_alloc* alloc, void* ptr, size_t size, size_t align, 
                                 const char* file, uint32_t line)
{
		if (ptr == NULL)
            return sx__aligned_alloc(alloc, size, align, file, line);

		uint8_t* aligned = (uint8_t*)ptr;
		uint32_t offset = *( (uint32_t*)aligned - 1);
		ptr = aligned - offset;

		align = sx_max(align, sizeof(uint32_t));
		const size_t total = size + align;
		ptr = sx__realloc(alloc, ptr, total, 0, file, line);
		uint8_t* new_aligned = (uint8_t*)sx_align_ptr(ptr, sizeof(uint32_t), align);

		if (new_aligned == aligned)
			return aligned;

		aligned = (uint8_t*)ptr + offset;
		memmove(new_aligned, aligned, size);
		uint32_t* header = (uint32_t*)new_aligned - 1;
		*header = (uint32_t)(new_aligned - (uint8_t*)ptr);
		return new_aligned;
}
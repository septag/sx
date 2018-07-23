#include "sx/virtual-alloc.h"

int main(int argc, char* argv[])
{
    const int total_sz = 1024*1024*1024;
    const int commit_sz = 10*1024*1024;

    void* mem = sx_virtual_reserve(total_sz);
    int* ptr = (int*)sx_virtual_commit(mem, commit_sz);
    int count = commit_sz / sizeof(int);
    for (int i = 0; i < count; i++) {
        ptr[i] = i;
    }

    int* ptr2 = (int*)sx_virtual_commit((uint8_t*)mem + commit_sz, commit_sz);
    for (int i = 0; i < count; i++) {
        ptr2[i] = i;
    }
    sx_virtual_decommit(ptr2, commit_sz);
    sx_virtual_decommit(ptr, commit_sz);

    sx_virtual_release(mem);
    return 0;
}
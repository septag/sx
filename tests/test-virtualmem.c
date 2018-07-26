#include "sx/virtual-alloc.h"
#include "sx/os.h"
#include <stdio.h>

int main(int argc, char* argv[])
{
    const int total_sz = 1024*1024*1024;
    const int commit_sz = 10*1024*1024;
    const size_t _1mb = 1024*1024;

    puts("Reserve 1 gb");
    void* mem = sx_virtual_reserve(total_sz);
    int* ptr = (int*)sx_virtual_commit(mem, commit_sz);
    printf("Process memory: %d mb\n", (int)(sx_os_processmem()/_1mb));
    sx_os_getch();

    puts("Using 10mb memory ...");
    int count = commit_sz / sizeof(int);
    for (int i = 0; i < count; i++) {
        ptr[i] = i;
    }
    printf("Process memory: %d mb\n", (int)(sx_os_processmem()/_1mb));
    sx_os_getch();

    puts("Commit and use 10 mb ...");
    int* ptr2 = (int*)sx_virtual_commit((uint8_t*)mem + commit_sz, commit_sz);
    for (int i = 0; i < count; i++) {
        ptr2[i] = i;
    }
    printf("Process memory: %d mb\n", (int)(sx_os_processmem()/_1mb));
    sx_os_getch();

    puts("Decommit ...");
    sx_virtual_decommit(ptr2, commit_sz);
    sx_virtual_decommit(ptr, commit_sz);

    printf("Process memory: %d mb", (int)(sx_os_processmem()/_1mb));
    sx_os_getch();

    sx_virtual_release(mem);
    return 0;
}
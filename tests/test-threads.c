#if SX_PLATFORM_WINDOWS
# define WIN32_LEAN_AND_MEAN
# include <windows.h>
# include <conio.h>
#endif
#include <stdio.h>

#include "sx/atomic.h"
#include "sx/threads.h"
#include "sx/atomic.h"
#include "sx/os.h"

sx_sem g_sem;
sx_queue_spsc* g_queue = NULL;
bool g_quit = false;

typedef struct work_item
{
    int id;
} work_item;

static int worker_thread_fn(void* user_data)
{
    puts("Thread");

    while (!g_quit) {
        work_item item;
        if (sx_queue_spsc_consume(g_queue, &item)) {
            printf("Received work: %d\n", item.id);
        }
        sx_semaphore_wait(&g_sem, -1);
    }
    return 0;
}

int main(int argc, char* argv[]) 
{
    const sx_alloc* alloc = sx_alloc_malloc;

    g_queue = sx_queue_spsc_create(alloc, sizeof(work_item), 10);
    sx_semaphore_init(&g_sem);
    sx_thread* thrd = sx_thread_create(alloc, worker_thread_fn, NULL, 0, "Consumer");
    if (!thrd) {
        return -1;
    }

    int id = 0;
    puts("Press space to submit an item to working thread");
    puts("Press ESC to close working thread and quit");
    puts("");

    while (!g_quit) {
        char c = sx_os_getch();
        if (c == 32) {
            work_item item;
            item.id = id;
            if (sx_queue_spsc_produce(g_queue, &item)) {
                id++;
                sx_semaphore_post(&g_sem, 1);
            }
        } else if (c == 27) {
            puts("ESC");
            sx_semaphore_post(&g_sem, 1);
            g_quit = true;
        }
    }
   
    sx_thread_destroy(thrd, alloc);
    sx_queue_spsc_destroy(g_queue, alloc);
    sx_semaphore_release(&g_sem);
    return 0;
}
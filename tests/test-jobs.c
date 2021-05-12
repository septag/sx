#include "sx/allocator.h"
#include "sx/jobs.h"
#include "sx/os.h"

#include <stdio.h>
#include <time.h>

static sx_job_context* g_ctx = NULL;

static void thread_init(sx_job_context* ctx, int thread_index, uint32_t thread_id, void* user)
{
    printf("init thread id=0x%x index=%d\n", thread_id, thread_index);
}

static void thread_shutdown(sx_job_context* ctx, int thread_index, uint32_t thread_id, void* user)
{
    printf("shutdown thread id=0x%x index=%d\n", thread_id, thread_index);
}

static void job_wait_fn(int range_start, int range_end, int thread_index, void* user)
{
    puts("Wait Job ...");
    sx_os_sleep(100);
    return;
}

static void job_fib_fn(int range_start, int range_end, int thread_index, void* user)
{
    int* results = user;
    for (int r = range_start; r < range_end; r++) {
        uint32_t a = 0;
        uint32_t b = 1;
        int N = 100000;
        for (int i = 0; i < N; i++) {
            uint32_t f = a + b;
            a = b;
            b = f;
        }

        results[r] = b;
    }

    sx_job_t job = sx_job_dispatch(g_ctx, 2, job_wait_fn, 0x0, 0, 0);
    sx_job_wait_and_del(g_ctx, job);

    job = sx_job_dispatch(g_ctx, 2, job_wait_fn, 0x0, 0, 0);
    sx_job_wait_and_del(g_ctx, job);
}

int main(int argc, char* argv[])
{
    const sx_alloc* alloc = sx_alloc_malloc();
    sx_job_context* ctx = sx_job_create_context(
        alloc, &(sx_job_context_desc){ .thread_init_cb = thread_init,
                                       .thread_shutdown_cb = thread_shutdown });
    if (!ctx) {
        puts("Error: sx_job_create_context failed!");
        return -1;
    }
    printf("jobs: %d worker threads\n", sx_job_num_worker_threads(ctx));
    g_ctx = ctx;

    uint32_t results[16];
    sx_memset(results, 0x0, sizeof(results));

    puts("Dispatching jobs ...");
    sx_job_t jhandle = sx_job_dispatch(ctx, 16, job_fib_fn, results, 0, 0);

    puts("Waiting ...");
    sx_job_wait_and_del(ctx, jhandle);

    puts("Results: ");
    for (int i = 0; i < 16; i++) {
        printf("\t%u\n", results[i]);
    }

    sx_job_destroy_context(ctx, alloc);
    sx_os_getch();
    return 0;
}

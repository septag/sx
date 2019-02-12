#include "sx/allocator.h"
#include "sx/jobs.h"
#include "sx/os.h"
#include "sx/rng.h"

#include <stdio.h>
#include <time.h>

#define MAX_JOBS 32
#define MAX_FIBERS 100
#define STACK_SIZE 128 * 1024

static sx_rng          g_rng;
static sx_job_context* g_ctx = NULL;

static void thread_init(sx_job_context* ctx, int thread_index, uint32_t thread_id, void* user) {
    printf("init thread id=0x%x index=%d\n", thread_id, thread_index);
}

static void thread_shutdown(sx_job_context* ctx, int thread_index, uint32_t thread_id, void* user) {
    printf("shutdown thread id=0x%x index=%d\n", thread_id, thread_index);
}

static void job_wait_fn(int index, void* user) {
    puts("Wait Job ...");
    sx_os_sleep(500);
    return;
}

static void job_fib_fn(int index, void* user) {
    uint32_t a = 0;
    uint32_t b = 1;
    int      N = 100000;
    for (int i = 0; i < N; i++) {
        uint32_t f = a + b;
        a = b;
        b = f;
    }

    const sx_job_desc jobs[] = { { job_wait_fn, NULL, SX_JOB_PRIORITY_HIGH },
                                 { job_wait_fn, NULL, SX_JOB_PRIORITY_HIGH } };
    // if (sx_rng_gen_irange(&g_rng, 1, 100) < 50) {
    sx_job_t job = sx_job_dispatch(g_ctx, jobs, 1, 0);
    sx_job_wait_and_del(g_ctx, job);
    //}

    *((uint32_t*)user) = b;
}

int main(int argc, char* argv[]) {
    sx_rng_seed(&g_rng, (uint32_t)time(NULL));
    const sx_alloc* alloc = sx_alloc_malloc;
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

    sx_job_desc jobs[] = {
        { job_fib_fn, &results[0], SX_JOB_PRIORITY_HIGH },
        { job_fib_fn, &results[1], SX_JOB_PRIORITY_HIGH },
        { job_fib_fn, &results[2], SX_JOB_PRIORITY_HIGH },
        { job_fib_fn, &results[3], SX_JOB_PRIORITY_HIGH },
        { job_fib_fn, &results[4], SX_JOB_PRIORITY_HIGH },
        { job_fib_fn, &results[5], SX_JOB_PRIORITY_HIGH },
        { job_fib_fn, &results[6], SX_JOB_PRIORITY_HIGH },
        { job_fib_fn, &results[7], SX_JOB_PRIORITY_HIGH },
        { job_fib_fn, &results[8], SX_JOB_PRIORITY_HIGH },
        { job_fib_fn, &results[9], SX_JOB_PRIORITY_HIGH },
        { job_fib_fn, &results[10], SX_JOB_PRIORITY_HIGH },
        { job_fib_fn, &results[11], SX_JOB_PRIORITY_HIGH },
        { job_fib_fn, &results[12], SX_JOB_PRIORITY_HIGH },
        { job_fib_fn, &results[13], SX_JOB_PRIORITY_HIGH },
        { job_fib_fn, &results[14], SX_JOB_PRIORITY_HIGH },
        { job_fib_fn, &results[15], SX_JOB_PRIORITY_HIGH },
    };

    puts("Dispatching jobs ...");
    sx_job_t jhandle = sx_job_dispatch(ctx, jobs, 3, 0);
    // sx_os_sleep(1000);

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

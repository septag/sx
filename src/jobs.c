#include "sx/jobs.h"

#include "sx/threads.h"
#include "sx/fiber.h"   
#include "sx/atomic.h"  // yield, sx_lock_t
#include "sx/pool.h"    
#include "sx/os.h"      // sx_os_minstacksz
#include "sx/string.h"  // sx_snprintf

#include <alloca.h>

// TODO: job selector, should do some caching/hashing to select the same thread for the job
//       Maybe hash sx_job_desc and assign thread_id to that
//       Each thread has a waiting_list for themself, so if we find the hash, put the job into thread's waiting_list
//       instead of global waiting_list. The selector, should only try once for it's own waiting_list, if not found
//       push all local list to global list for other threads to pickup the work
//       This method may reduce the context switches in many situations, especially in root jobs


typedef struct sx__job
{
    int             job_index;
    int             done;
    uint32_t        owner_tid;
    sx_fiber_stack  stack_mem;
    sx_fiber_t      fiber;
    sx_job_t        counter;
    sx_job_t        wait_counter;
    sx_job_desc     desc;
    sx_job_context* ctx;
    struct sx__job* next;
    struct sx__job* prev;
} sx__job;

typedef struct sx__job_thread_data
{
    sx__job*        cur_job;
    sx_fiber_stack  selector_stack;
    sx_fiber_t      selector_fiber;
    uint32_t        tid;
    bool            main_thrd;
} sx__job_thread_data;

typedef struct sx_job_context
{
    const sx_alloc* alloc;
    sx_thread**     threads;
    int             num_threads;
    int             stack_sz;
    sx_pool*        job_pool;
    sx_pool*        counter_pool;
    sx__job*        waiting_list[SX_JOB_PRIORITY_COUNT];
    sx__job*        waiting_list_last[SX_JOB_PRIORITY_COUNT];
    sx_lock_t       job_lk;
    sx_lock_t       counter_lk;
    sx_tls          thread_tls;
    int             dummy_counter;
    sx_sem          sem;
    int             quit;
} sx_job_context;

static void sx__del_job(sx_job_context* ctx, sx__job* job)
{
    sx_lock(&ctx->job_lk);
    sx_pool_del(ctx->job_pool, job);
    sx_unlock(&ctx->job_lk);
}

static void fiber_fn(sx_fiber_transfer transfer)
{
    sx__job* job = (sx__job*)transfer.user;
    sx_job_context* ctx = job->ctx;
    sx__job_thread_data* tdata = (sx__job_thread_data*)sx_tls_get(ctx->thread_tls);

    sx_assert(tdata->cur_job == NULL);
    tdata->selector_fiber = transfer.from;
    tdata->cur_job = job;

    // Run the actual job code
    job->desc.callback(job->job_index, job->desc.user);
    job->done = 1;

    // Back to job caller
    sx_fiber_switch(transfer.from, transfer.user);
}

static sx__job* sx__new_job(sx_job_context* ctx, int index, const sx_job_desc* desc, sx_job_t counter)
{
    sx__job* j = (sx__job*)sx_pool_new(ctx->job_pool);

    if (j) {
        j->job_index = index;
        j->owner_tid = 0;
        j->done = 0;
        if (!j->stack_mem.sptr) {
            // Initialize stack memory
            if (!sx_fiber_stack_init(&j->stack_mem, ctx->stack_sz)) {
                sx_out_of_memory();
                return NULL;
            }
        }
        j->fiber = sx_fiber_create(j->stack_mem, fiber_fn);
        j->counter = counter;
        j->wait_counter = &ctx->dummy_counter;
        j->desc = *desc;
        j->ctx = ctx;
        j->next = j->prev = NULL;
    } 
    return j;
}

SX_INLINE void sx__job_add_list(sx__job** pfirst, sx__job** plast, sx__job* node)
{
    // Add to the end of the list
    if (*plast) {
        (*plast)->next = node;
        node->prev = *plast;
    } 
    *plast = node;
    if (*pfirst == NULL)
        *pfirst = node;
}

SX_INLINE void sx__job_remove_list(sx__job** pfirst, sx__job** plast, sx__job* node)
{
    if (node->prev)
        node->prev->next = node->next;
    if (node->next)
        node->next->prev = node->prev;
    if (*pfirst == node)
        *pfirst = node->next;
    if (*plast == node)
        *plast = node->prev;
    node->prev = node->next = NULL;
}

typedef struct sx__job_select_result
{
    sx__job* job;
    bool     waiting_list_alive;
} sx__job_select_result;


static sx__job_select_result sx__job_select(sx_job_context* ctx, uint32_t tid)
{
    sx__job_select_result r;
    r.job = NULL;
    r.waiting_list_alive = false;

    sx_lock(&ctx->job_lk);
    for (int pr = 0; pr < SX_JOB_PRIORITY_COUNT && !r.job; pr++) {
        sx__job* node = ctx->waiting_list[pr];
        while (node) {
            r.waiting_list_alive = true;
            if (*node->wait_counter == 0) { // job must not be waiting/depend on any jobs
                if (node->owner_tid == 0 || node->owner_tid == tid) {
                    r.job = node;
                    sx__job_remove_list(&ctx->waiting_list[pr], &ctx->waiting_list_last[pr], node);
                    break;
                }
            }
            node = node->next;
        }   // while(iterate nodes)
    } // foreach(priority)
    sx_unlock(&ctx->job_lk);

    return r;
}

static void sx__job_selector_main_thrd(sx_fiber_transfer transfer)
{
    sx_job_context* ctx = (sx_job_context*)transfer.user;
    sx__job_thread_data* tdata = (sx__job_thread_data*)sx_tls_get(ctx->thread_tls);
    sx_assert(tdata);

    // Select the best job in the waiting list
    sx__job_select_result r = sx__job_select(ctx, tdata->tid);

    // 
    if (r.job) {
        // Job is a slave (in wait mode), get back to it and remove slave mode
        if (r.job->owner_tid > 0) {
            sx_assert(tdata->cur_job == NULL);
            r.job->owner_tid = 0;
        }

        // Run the job from beginning, or continue after 'wait'
        r.job->fiber = sx_fiber_switch(r.job->fiber, r.job).from;

        // Delete the job and decrement job counter if it's done
        if (r.job->done) {
            tdata->cur_job = NULL;
            sx_atomic_decr(r.job->counter);
            sx__del_job(ctx, r.job);
        }
    }

    tdata->selector_fiber = sx_fiber_create(tdata->selector_stack, sx__job_selector_main_thrd);
    sx_fiber_switch(transfer.from, transfer.user);
}

//
// Threads run this function to pick a job from the list and execute the job fiber
static void sx__job_selector_fn(sx_fiber_transfer transfer)
{
    sx_job_context* ctx = (sx_job_context*)transfer.user;
    sx__job_thread_data* tdata = (sx__job_thread_data*)sx_tls_get(ctx->thread_tls);
    sx_assert(tdata);

    while (!ctx->quit) {
        sx_semaphore_wait(&ctx->sem, -1);   // Wait for a job

        // Select the best job in the waiting list
        sx__job_select_result r = sx__job_select(ctx, tdata->tid);

        // 
        if (r.job) {
            // Job is a slave (in wait mode), get back to it and remove slave mode
            if (r.job->owner_tid > 0) {
                sx_assert(tdata->cur_job == NULL);
                r.job->owner_tid = 0;
            }

            // Run the job from beginning, or continue after 'wait'
            r.job->fiber = sx_fiber_switch(r.job->fiber, r.job).from;

            // Delete the job and decrement job counter if it's done
            if (r.job->done) {
                tdata->cur_job = NULL;
                sx_atomic_decr(r.job->counter);
                sx__del_job(ctx, r.job);
            }
        } else if (r.waiting_list_alive) {
            // If we have a pending job, continue this loop one more time
            sx_semaphore_post(&ctx->sem, 1);
            sx_yield_cpu();
        }
    }

    // Back to caller thread
    sx_fiber_switch(transfer.from, transfer.user);
}

sx_job_t sx_job_dispatch(sx_job_context* ctx, const sx_job_desc* descs, int count)
{
    sx_assert(count > 0);
    sx__job_thread_data* tdata = (sx__job_thread_data*)sx_tls_get(ctx->thread_tls);

    // Create a counter (job handle)
    sx_lock(&ctx->counter_lk);
    sx_job_t counter = (sx_job_t)sx_pool_new(ctx->counter_pool);
    sx_unlock(&ctx->counter_lk);

    if (!counter) {
        sx_assert(0 && "Maximum job instances exceeded");
        return NULL;
    }

    *counter = count;
    sx_assert(tdata && "Dispatch must be called within main thread or job threads");

    // Another job is running on this thread. So depend the current running job to the new dispatches
    if (tdata->cur_job)
        tdata->cur_job->wait_counter = counter;

    // Push jobs to the end of the list, so they can be collected by threads
    sx_lock(&ctx->job_lk);
    for (int i = 0; i < count; i++) {
        sx__job_add_list(&ctx->waiting_list[descs[i].priority], 
                         &ctx->waiting_list_last[descs[i].priority],
                         sx__new_job(ctx, i, &descs[i], counter));
    }
    sx_unlock(&ctx->job_lk);

    // Post to semaphore to worker threads start cur_job
    sx_semaphore_post(&ctx->sem, count);

    return counter;    
} 

void sx_job_wait_del(sx_job_context* ctx, sx_job_t job)
{
    sx__job_thread_data* tdata = (sx__job_thread_data*)sx_tls_get(ctx->thread_tls);

    sx_compiler_read_barrier();
    while (*job > 0) {
        // If thread is running a job, make it slave to the thread so it should only be picked up by this thread
        // And push the job back to waiting_list
        if (tdata->cur_job) {
            sx__job* cur_job = tdata->cur_job;
            tdata->cur_job = NULL;
            cur_job->owner_tid = tdata->tid;

            sx_lock(&ctx->job_lk);
            int list_idx = cur_job->desc.priority;
            sx__job_add_list(&ctx->waiting_list[list_idx], 
                            &ctx->waiting_list_last[list_idx],
                            cur_job);            
            sx_unlock(&ctx->job_lk);

            if (!tdata->main_thrd)
                sx_semaphore_post(&ctx->sem, 1);
        }

        sx_fiber_switch(tdata->selector_fiber, ctx);    // Switch to selector loop
        sx_yield_cpu();
    }

    // All jobs are done, Delete the counter
    sx_lock(&ctx->counter_lk);
    sx_pool_del(ctx->counter_pool, (void*)job);
    sx_unlock(&ctx->counter_lk);
}

bool sx_job_try_del(sx_job_context* ctx, sx_job_t job)
{
    sx_compiler_read_barrier();
    if (*job == 0) {
        // All jobs are done, Delete the counter
        sx_lock(&ctx->counter_lk);
        sx_pool_del(ctx->counter_pool, (void*)job);
        sx_unlock(&ctx->counter_lk);

        return true;
    }

    return false;
}

static sx__job_thread_data* sx__job_create_tdata(const sx_alloc* alloc, uint32_t tid, bool main_thrd)
{
    sx__job_thread_data* tdata = (sx__job_thread_data*)sx_malloc(alloc, sizeof(sx__job_thread_data));
    sx_memset(tdata, 0x0, sizeof(sx__job_thread_data));
    tdata->tid = sx_thread_tid();
    tdata->main_thrd = main_thrd;

    bool r = sx_fiber_stack_init(&tdata->selector_stack, sx_os_minstacksz());
    sx_assert(r && "Not enough memory for temp stacks");
    SX_UNUSED(r);

    return tdata;
}

static void sx__job_destroy_tdata(sx__job_thread_data* tdata, const sx_alloc* alloc)
{
    sx_fiber_stack_release(&tdata->selector_stack);
    sx_free(alloc, tdata);
}

static int sx__job_thread_fn(void* user)
{
    // Create thread data
    sx_job_context* ctx = (sx_job_context*)user;
    sx__job_thread_data* tdata = sx__job_create_tdata(ctx->alloc, sx_thread_tid(), false);
    if (!tdata) {
        sx_assert(tdata && "ThreadData create failed!");
        return -1;
    }
    
    sx_tls_set(ctx->thread_tls, tdata);

    // Get first stack and run selector loop
    sx_fiber_t fiber = sx_fiber_create(tdata->selector_stack, sx__job_selector_fn);
    sx_fiber_switch(fiber, ctx);

    sx_tls_set(ctx->thread_tls, NULL);
    sx__job_destroy_tdata(tdata, ctx->alloc);

    return 0;
}

sx_job_context* sx_job_create_context(const sx_alloc* alloc, int num_threads, int max_jobs, int num_fibers, int fiber_stack_sz)
{
    sx_assert(num_threads >= 0);
    sx_assert(num_fibers > 0);
    sx_assert(fiber_stack_sz >= sx_os_minstacksz() && "stack size too small");

    sx_job_context* ctx = (sx_job_context*)sx_malloc(alloc, sizeof(sx_job_context));
    if (!ctx) {
        sx_out_of_memory();
        return NULL;
    }
    sx_memset(ctx, 0x0, sizeof(sx_job_context));

    ctx->alloc = alloc;
    ctx->num_threads = num_threads;
    ctx->thread_tls = sx_tls_create();
    ctx->stack_sz = fiber_stack_sz;
    sx_semaphore_init(&ctx->sem);

    sx__job_thread_data* main_tdata = sx__job_create_tdata(alloc, sx_thread_tid(), true);
    if (!main_tdata) {
        sx_free(alloc, ctx);
        return NULL;
    }
    sx_tls_set(ctx->thread_tls, main_tdata);
    main_tdata->selector_fiber = sx_fiber_create(main_tdata->selector_stack, sx__job_selector_main_thrd);

    // pools
    ctx->job_pool = sx_pool_create(alloc, sizeof(sx__job), num_fibers);
    ctx->counter_pool = sx_pool_create(alloc, sizeof(int), max_jobs);
    if (!ctx->job_pool || !ctx->counter_pool)
        return NULL;
    sx_memset(ctx->job_pool->buff, 0x0, sizeof(sx__job)*num_fibers);
    sx_memset(ctx->counter_pool->buff, 0x0, sizeof(int)*max_jobs);

    // Worker threads
    if (num_threads > 0) {
        ctx->threads = (sx_thread**)sx_malloc(alloc, sizeof(sx_thread*)*num_threads);
        sx_assert(ctx->threads);

        for (int i = 0; i < num_threads; i++) {
            char name[32];
            sx_snprintf(name, sizeof(name), "sx_job_thread(%d)", i+1);
            ctx->threads[i] = sx_thread_create(alloc, sx__job_thread_fn, ctx, sx_os_minstacksz(), name);
            sx_assert(ctx->threads[i] && "sx_thread_create failed!");
        }
    }

    return ctx;
}

void sx_job_destroy_context(sx_job_context* ctx, const sx_alloc* alloc)
{
    sx_assert(ctx);
    sx_assert(ctx->alloc == alloc);

    // signal selectors to finish the job and quit
    ctx->quit = 1;
    sx_compiler_write_barrier();
    sx_semaphore_post(&ctx->sem, ctx->num_threads+1);

    // shutdown threads
    for (int i = 0; i < ctx->num_threads; i++)
        sx_thread_destroy(ctx->threads[i], alloc);
    sx_free(alloc, ctx->threads);

    sx__job_destroy_tdata((sx__job_thread_data*)sx_tls_get(ctx->thread_tls), alloc);

    // TODO: destroy job_pool's stack memories
    sx_pool_destroy(ctx->job_pool, alloc);
    sx_pool_destroy(ctx->counter_pool, alloc);
    sx_semaphore_release(&ctx->sem);

    sx_free(alloc, ctx);
}



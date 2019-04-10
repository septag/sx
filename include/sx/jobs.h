//
// Copyright 2018 Sepehr Taghdisian (septag@github). All rights reserved.
// License: https://github.com/septag/sx#license-bsd-2-clause
//
// jobs.h - v1.0 - Fiber based job dispatcher
// Reference: http://gdcvault.com/play/1022186/Parallelizing-the-Naughty-Dog-Engine
//
// Job dispatcher is a multi-threaded task scheduler, that uses lightweight fibers to switch
//      contexts and schedule jobs.
//      Jobs are converted into fibers and pushed to a list that are picked up by worker threads
//      Worker threads pickup fibers and switch to them. When dependencies and nested jobs are
//      created, they are immediately rescheduled to threads and replace the current job they are
//      doing. Threads can also get back and continue the job when dependencies are met.
//      This makes this scheduler powerfull in terms of shceduling and not blocking the threads to
//      wait for dependencies there is also no need to create dependency graphs before submitting
//      jobs.
//
// Types:
//      sx_job_priorty      Job priority, higher priority jobs will run sooner
//      sx_job_desc         Job description, required for submitting jobs, includes a job function,
//                          priority and user data
// API:
//      sx_job_create_context       Create the job context (manager)
//                                  - num_threads: number of worker threads, usually one less that
//                                                 number of logical cores.
//                                                 if you set this to 0, no multi-threading will be
//                                                 applied and all the work will be done in the
//                                                 main thread.
//                                                 if num_threads = -1, the dispatcher will
//                                                 automatically spawn num_cpu_cores-1 threads.
//                                  - max_fibers: Total number of fibers that users can create
//                                                This is actually the total number of sx_job_desc
//                                                submitted that are still active
//                                                (see sx_job_dispatch)
//                                  - fiber_stack_sz: Stack size of fibers in bytes
//                                                    This parameter depends on how much work you
//                                                    will do inside job functions.
//                                                    More work requires more stack size or you will
//                                                    get stack overflow exception.
//                                                    Usually a number between 128kb ~ 2mb is
//                                                    sufficient.
//      sx_job_destroy_context      Destroy the job context
//      sx_job_dispatch             (Thread-Safe) Submit bunch of sub-jobs for the scheduler, this
//                                  will return a valid sx_job_t handle that you can later wait on
//                                  submitted jobs are immedietely picked up by worker threads and
//                                  processed
//                                  - descs: job descriptions, each sub-job can contain a callback
//                                           function, user data and execution priority
//                                  - count: number of sub-jots (desc array size) to be submitted
//                                  - tags: (default: 0) assigns work tag for the job. See below for
//                                           more details on the concept of Tags
//                                  NOTE: if max_fibers (running-jobs) is exceeded, job will be
//                                        queued and automatically dispatched later on
//                                        'sx_job_wait_and_del' and 'sx_job_test_and_del'
//      sx_job_wait_and_del         (Thread-Safe) Blocks the program and waits on dispatched job.
//                                  It deletes the sx_job_t handle if the job is done
//                                  NOTE: If the sx_job_t is done this functions returns immediately
//                                        but will do some work if any sub-jobs are remaining and
//                                        sx_job_t is not finished
//      sx_job_test_and_del         (Thread-Safe) This is a non-blocking function,
//                                  which only checks if sx_job_t is finished
//                                  If job is finished, it returns True and deletes the sx_job_t
//                                  handle. If not, the function moves on and returns False
//                                  immediately
//      sx_job_num_worker_threads   Returns number of worker threads running
//                                  (does not include main thread)
//      sx_job_set_current_thread_tags Sets thread-tag for the current running thread.
//                                     better call this inside `sx_job_thread_init_cb` callback
//                                     function. See below for more details on the concept of Tags
//
// clang-format off
//  Tags (Advanced):
//      The concept is that every worker thread can be assigned a tag (which is a uint32_t bitset), and by default, every thread's tag is 0xffffffff
//      You can change each worker-thread tag by calling `sx_job_set_current_thread_tags` function inside `sx_job_thread_init_cb` callback
//      Now by assigning tags to each job dispatch, you can determine exacly which worker-thread (or threads) can pick up and run the job
//      use this wisely, because if job tag bits are not found in thread tags, the job will not be executed ever
//      Example:
//          Worker threads:
//          thread #1 (tag = 0x7 (binary: 0111))        thread #2 (0x7 (binary: 0111))        thread #3 (0x7 (binary: 0111))
//
//          dispatch_job (with tag-bits = 0x4 (0100)) - runs on threads #1 and #2 because the tag bits match
//          dispatch_job (with tag-bits = 0x8 (1000)) - only runs on thread #3
//          dispatch_job (with tag = 0)               - default value. runs on all threads
//          dispatch_job (with tag = 0xffff)          - runs on all threads
// clang-format on
#pragma once

#include "macros.h"
#include <stdbool.h>

typedef struct sx_alloc       sx_alloc;
typedef struct sx_job_context sx_job_context;
typedef volatile int*         sx_job_t;

typedef void(sx_job_cb)(int index, void* user);
typedef void(sx_job_thread_init_cb)(sx_job_context* ctx, int thread_index, unsigned int thread_id,
                                    void* user);
typedef void(sx_job_thread_shutdown_cb)(sx_job_context* ctx, int thread_index,
                                        unsigned int thread_id, void* user);

typedef enum sx_job_priority {
    SX_JOB_PRIORITY_HIGH = 0,
    SX_JOB_PRIORITY_NORMAL,
    SX_JOB_PRIORITY_LOW,
    SX_JOB_PRIORITY_COUNT
} sx_job_priority;

typedef struct sx_job_desc {
    sx_job_cb*      callback;
    void*           user;
    sx_job_priority priority;
} sx_job_desc;

typedef struct sx_job_context_desc {
    int num_threads;    // number of worker threads to spawn (default: num_cpu_cores-1)
    int max_fibers;     // maximum fibers that are allowed to be active at the same time (default:
                        // 64)
    int                    fiber_stack_sz;            // fiber stack size (default: 1mb)
    sx_job_thread_init_cb* thread_init_cb;            // callback function that will be called on
                                                      // initiaslization of each worker thread
    sx_job_thread_shutdown_cb* thread_shutdown_cb;    // callback functions that will be called on
                                                      // the shutdown of each worker thread
    void* thread_user_data;    // user-data to be passed to callback functions above
} sx_job_context_desc;

SX_API sx_job_context* sx_job_create_context(const sx_alloc*            alloc,
                                             const sx_job_context_desc* desc);
SX_API void            sx_job_destroy_context(sx_job_context* ctx, const sx_alloc* alloc);

SX_API sx_job_t sx_job_dispatch(sx_job_context* ctx, const sx_job_desc* descs, int count,
                                unsigned int tags sx_default(0));
SX_API void     sx_job_wait_and_del(sx_job_context* ctx, sx_job_t job);
SX_API bool     sx_job_test_and_del(sx_job_context* ctx, sx_job_t job);
SX_API int      sx_job_num_worker_threads(sx_job_context* ctx);
SX_API void     sx_job_set_current_thread_tags(sx_job_context* ctx, unsigned int tags);
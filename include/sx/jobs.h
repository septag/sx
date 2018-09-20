//
// Copyright 2018 Sepehr Taghdisian (septag@github). All rights reserved.
// License: https://github.com/septag/sx#license-bsd-2-clause
//
// jobs.h - v1.0 - Fiber based job dispatcher
// Reference: http://gdcvault.com/play/1022186/Parallelizing-the-Naughty-Dog-Engine
//
// Job dispatcher is a multi-threaded task scheduler, that uses lightweight fibers to switch contexts and schedule jobs
//      Jobs are converted into fibers and pushed to a list that are picked up by worker threads
//      Worker threads pickup fibers and switch to them. When dependencies and nested jobs are created, they are immediately
//      Rescheduled to threads and replace the current job they are doing. Threads can also get back and continue the job when dependencies are met
//      This makes this scheduler powerfull in terms of shceduling and not blocking the threads to wait for dependencies
//      There is also no need to create dependency graphs before submitting jobs
//  
// Types:
//      sx_job_priorty      Job priority, higher priority jobs will run sooner
//      sx_job_desc         Job description, required for submitting jobs, includes a job function, priority and user data
// API:
//      sx_job_create_context       Create the job context (manager) 
//                                  - num_threads: number of worker threads, usually one less that number of logical cores
//                                                 if you set this to 0, no multi-threading will be applied and all the work
//                                                 will be done in the main thread
//                                  - max_jobs: Maximum number of jobs that are allowed to be spawned
//                                              This is actually the maximumm number of sx_job_t handles that are active
//                                  - num_fibers: Total number of fibers that users can create
//                                                This is actually the total number of sx_job_desc submitted that are still active (see sx_job_dispatch)
//                                  - fiber_stack_sz: Stack size of fibers in bytes
//                                                    This parameter depends on how much work you will do inside job functions
//                                                    More work requires more stack size or you will get stack overflow exception
//                                                    Usually a number between 128kb ~ 2mb is sufficient
//      sx_job_destroy_context      Destroy the job context
//      sx_job_dispatch             (Thread-Safe) Submit bunch of sub-jobs for the scheduler, this will return a valid sx_job_t handle that you can later wait on
//                                  Submitted jobs are immedietely picked up by worker threads and processed
//                                  - descs: job descriptions, each sub-job can contain a callback function, user data and execution priority
//                                  - count: number of sub-jots (desc array size) to be submitted
//      sx_job_wait_del             (Thread-Safe) Blocks the program and waits on dispatched job (sx_job_t). It deletes the sx_job_t handle if the job is done
//                                  NOTE: If the sx_job_t is done this functions returns immediately
//                                        but will do some work if any sub-jobs are remaining and sx_job_t is not finished
//      sx_job_try_del              (Thread-Safe) This is a non-blocking function, which only checks if sx_job_t is finished
//                                  If job is finished, it returns True and deletes the sx_job_t handle
//                                  If not, the function moves on and returns False immediately
//
#pragma once

#ifndef SX_JOBS_H_
#define SX_JOBS_H_

#include "sx/allocator.h"

typedef void (sx_job_cb)(int index, void* user);

typedef enum sx_job_priority
{
    SX_JOB_PRIORITY_HIGH = 0,
    SX_JOB_PRIORITY_NORMAL,
    SX_JOB_PRIORITY_LOW,
    SX_JOB_PRIORITY_COUNT
} sx_job_priority;

typedef struct sx_job_desc
{
    sx_job_cb*      callback;
    void*           user;    
    sx_job_priority priority;   
} sx_job_desc;

typedef volatile int* sx_job_t;

typedef struct sx_job_context sx_job_context;

#ifdef __cplusplus
extern "C" {
#endif

sx_job_context* sx_job_create_context(const sx_alloc* alloc, int num_threads, 
                                      int max_jobs, int num_fibers, int fiber_stack_sz);
void            sx_job_destroy_context(sx_job_context* ctx, const sx_alloc* alloc);

sx_job_t sx_job_dispatch(sx_job_context* ctx, const sx_job_desc* descs, int count);
void     sx_job_wait_del(sx_job_context* ctx, sx_job_t job);
bool     sx_job_try_del(sx_job_context* ctx, sx_job_t job);

#ifdef __cplusplus
}
#endif

#endif // SX_JOBS_H_
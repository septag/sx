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

#ifdef __cplusplus
    sx_job_desc() {}
    explicit sx_job_desc(sx_job_cb* _callback, void* _user, sx_job_priority _priority = SX_JOB_PRIORITY_HIGH) :
        callback(_callback), priority(_priority), user(_user) {}
#endif
} sx_job_desc;

typedef struct sx_job_fiber_desc
{
    int stack_sz;
    int type;
} sx_job_fiber_desc;

typedef volatile int* sx_job_t;

typedef struct sx_job_context sx_job_context;

sx_job_context* sx_job_create_context(const sx_alloc* alloc, int num_threads, 
                                      int max_jobs, int num_fibers, int fiber_stack_sz);
void            sx_job_destroy_context(sx_job_context* ctx, const sx_alloc* alloc);

sx_job_t sx_job_dispatch(sx_job_context* ctx, const sx_job_desc* descs, int count);
void     sx_job_wait_del(sx_job_context* ctx, sx_job_t job);
bool     sx_job_try_del(sx_job_context* ctx, sx_job_t job);

#endif // SX_JOBS_H_
//
// Copyright 2018 Sepehr Taghdisian (septag@github). All rights reserved.
// License: https://github.com/septag/sx#license-bsd-2-clause
//
// parts of this code is copied from bx library: https://github.com/bkaradzic/bx
// Copyright 2011-2019 Branimir Karadzic. All rights reserved.
// License: https://github.com/bkaradzic/bx#license-bsd-2-clause
//
#include "sx/threads.h"
#include "sx/allocator.h"
#include "sx/os.h"
#include "sx/string.h"

#if SX_PLATFORM_APPLE
#    include <dispatch/dispatch.h>
#    include <mach/mach.h>
#    include <pthread.h>
#elif SX_PLATFORM_POSIX
#    define __USE_GNU
#    include <errno.h>
#    include <pthread.h>
#    include <semaphore.h>
#    include <sys/prctl.h>
#    include <time.h>
#    include <unistd.h>
#    if defined(__FreeBSD__)
#        include <pthread_np.h>
#    endif
#    if SX_PLATFORM_LINUX || SX_PLATFORM_RPI
#        include <sys/syscall.h>    // syscall
#    endif
#elif SX_PLATFORM_WINDOWS
// clang-format off
#    define VC_EXTRALEAN
#    define WIN32_LEAN_AND_MEAN
SX_PRAGMA_DIAGNOSTIC_PUSH()
SX_PRAGMA_DIAGNOSTIC_IGNORED_MSVC(5105)
#    include <windows.h>
SX_PRAGMA_DIAGNOSTIC_POP()
#    include <limits.h>
#    include <synchapi.h>
// clang-format on
#endif    // SX_PLATFORM_

#include "sx/atomic.h"

typedef struct sx__mutex_s {
#if SX_PLATFORM_WINDOWS
    CRITICAL_SECTION handle;
#elif SX_PLATFORM_POSIX
    pthread_mutex_t handle;
#endif
} sx__mutex;

typedef struct sx__sem_s {
#if SX_PLATFORM_APPLE
    dispatch_semaphore_t handle;
#elif SX_PLATFORM_POSIX
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int count;
#elif SX_PLATFORM_WINDOWS
    HANDLE handle;
#endif
} sx__sem;

typedef struct sx__signal_s {
#if SX_PLATFORM_WINDOWS
#    if _WIN32_WINNT >= 0x0600
    CRITICAL_SECTION mutex;
    CONDITION_VARIABLE cond;
    int value;
#    else
    HANDLE e;
#    endif
#elif SX_PLATFORM_POSIX
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int value;
#endif
} sx__signal;

typedef struct sx__thread_s {
    sx_sem sem;
    sx_thread_cb* callback;

#if SX_PLATFORM_WINDOWS
    HANDLE handle;
    DWORD thread_id;
#elif SX_PLATFORM_POSIX
    pthread_t handle;
#    if SX_PLATFORM_APPLE
    char name[32];
#    endif
#endif

    void* user_data1;
    void* user_data2;
    int stack_sz;
    bool running;
} sx_thread;

static_assert(sizeof(sx__mutex) <= sizeof(sx_mutex), "sx_mutex size mismatch");
static_assert(sizeof(sx__sem) <= sizeof(sx_sem), "sx_mutex size mismatch");
static_assert(sizeof(sx__signal) <= sizeof(sx_signal), "sx_mutex size mismatch");

// Apple has different implementation for semaphores
#if SX_PLATFORM_APPLE

void sx_semaphore_init(sx_sem* sem)
{
    sx__sem* _sem = (sx__sem*)sem->data;
    _sem->handle = dispatch_semaphore_create(0);
    sx_assertf(_sem->handle != NULL, "dispatch_semaphore_create failed");
}

void sx_semaphore_release(sx_sem* sem)
{
    sx__sem* _sem = (sx__sem*)sem->data;
    if (_sem->handle) {
        dispatch_release(_sem->handle);
        _sem->handle = NULL;
    }
}

void sx_semaphore_post(sx_sem* sem, int count)
{
    sx__sem* _sem = (sx__sem*)sem->data;
    for (int i = 0; i < count; i++) {
        dispatch_semaphore_signal(_sem->handle);
    }
}

bool sx_semaphore_wait(sx_sem* sem, int msecs)
{
    sx__sem* _sem = (sx__sem*)sem->data;
    dispatch_time_t dt = msecs < 0 ? DISPATCH_TIME_FOREVER
                                   : dispatch_time(DISPATCH_TIME_NOW, (int64_t)msecs * 1000000ll);
    return !dispatch_semaphore_wait(_sem->handle, dt);
}
#endif

// Other implementations are either Posix or Win32
#if SX_PLATFORM_POSIX

// Tls
sx_tls sx_tls_create(void)
{
    pthread_key_t key;
    int r = pthread_key_create(&key, NULL);
    sx_assertf(r == 0, "pthread_key_create failed");
    sx_unused(r);
    return (sx_tls)(uintptr_t)key;
}

void sx_tls_destroy(sx_tls tls)
{
    pthread_key_t key = (pthread_key_t)(uintptr_t)tls;
    int r = pthread_key_delete(key);
    sx_assertf(r == 0, "pthread_key_delete failed");
    sx_unused(r);
}

void sx_tls_set(sx_tls tls, void* data)
{
    pthread_key_t key = (pthread_key_t)(uintptr_t)tls;
    int r = pthread_setspecific(key, data);
    sx_assertf(r == 0, "pthread_setspcific failed");
    sx_unused(r);
}

void* sx_tls_get(sx_tls tls)
{
    pthread_key_t key = (pthread_key_t)(uintptr_t)tls;
    return pthread_getspecific(key);
}

// Thread
static void* thread_fn(void* arg)
{
    sx_thread* thrd = (sx_thread*)arg;
    union {
        void* ptr;
        int32_t i;
    } cast;

#    if SX_PLATFORM_APPLE
    if (thrd->name[0])
        sx_thread_setname(thrd, thrd->name);
#    endif

    sx_semaphore_post(&thrd->sem, 1);
    cast.i = thrd->callback(thrd->user_data1, thrd->user_data2);
    return cast.ptr;
}

sx_thread* sx_thread_create(const sx_alloc* alloc, sx_thread_cb* callback, void* user_data1,
                            int stack_sz, const char* name, void* user_data2)
{
    sx_thread* thrd = (sx_thread*)sx_malloc(alloc, sizeof(sx_thread));
    if (!thrd)
        return NULL;

    sx_semaphore_init(&thrd->sem);
    thrd->callback = callback;
    thrd->user_data1 = user_data1;
    thrd->user_data2 = user_data2;
    thrd->stack_sz = sx_max(stack_sz, (int)sx_os_minstacksz());
    thrd->running = true;

    pthread_attr_t attr;
    int r = pthread_attr_init(&attr);
    sx_unused(r);
    sx_assertf(r == 0, "pthread_attr_init failed");
    r = pthread_attr_setstacksize(&attr, thrd->stack_sz);
    sx_assertf(r == 0, "pthread_attr_setstacksize failed");

#    if SX_PLATFORM_APPLE
    thrd->name[0] = 0;
    if (name)
        sx_strcpy(thrd->name, sizeof(thrd->name), name);
#    endif

    r = pthread_create(&thrd->handle, &attr, thread_fn, thrd);
    sx_assertf(r == 0, "pthread_create failed");

    // Ensure that thread callback is running
    sx_semaphore_wait(&thrd->sem, -1);

#    if !SX_PLATFORM_APPLE
    if (name)
        sx_thread_setname(thrd, name);
#    endif

    return thrd;
}

int sx_thread_destroy(sx_thread* thrd, const sx_alloc* alloc)
{
    sx_assertf(thrd->running, "Thread is not running!");

    union {
        void* ptr;
        int32_t i;
    } cast;

    pthread_join(thrd->handle, &cast.ptr);

    sx_semaphore_release(&thrd->sem);

    thrd->handle = 0;
    thrd->running = false;

    sx_free(alloc, thrd);

    return cast.i;
}

bool sx_thread_running(sx_thread* thrd)
{
    return thrd->running;
}

void sx_thread_setname(sx_thread* thrd, const char* name)
{
#    if SX_PLATFORM_APPLE
    sx_unused(thrd);
    pthread_setname_np(name);
#    elif SX_PLATFORM_BSD
#        if defined(__NetBSD__)
    pthread_setname_np(thrd->handle, "%s", (void*)name);
#        else
    pthread_set_name_np(thrd->handle, name);
#        endif    // defined(__NetBSD__)
#    elif (SX_CRT_GLIBC >= 21200) && !SX_PLATFORM_HURD
    pthread_setname_np(thrd->handle, name);
#    elif SX_PLATFORM_LINUX
    sx_unused(thrd);
    prctl(PR_SET_NAME, name, 0, 0, 0);
#    else
    sx_unused(thrd);
    sx_unused(name);
#    endif
}

void sx_thread_yield(void)
{
    sched_yield();
}


// Mutex
void sx_mutex_init(sx_mutex* mutex)
{
    sx__mutex* _m = (sx__mutex*)mutex->data;

    pthread_mutexattr_t attr;
    int r = pthread_mutexattr_init(&attr);
    sx_assert(r == 0);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    r = pthread_mutex_init(&_m->handle, &attr);
    sx_assertf(r == 0, "pthread_mutex_init failed");
    sx_unused(r);
}

void sx_mutex_release(sx_mutex* mutex)
{
    sx__mutex* _m = (sx__mutex*)mutex->data;
    pthread_mutex_destroy(&_m->handle);
}

void sx_mutex_enter(sx_mutex* mutex)
{
    sx__mutex* _m = (sx__mutex*)mutex->data;
    pthread_mutex_lock(&_m->handle);
}

void sx_mutex_exit(sx_mutex* mutex)
{
    sx__mutex* _m = (sx__mutex*)mutex->data;
    pthread_mutex_unlock(&_m->handle);
}

bool sx_mutex_try(sx_mutex* mutex)
{
    sx__mutex* _m = (sx__mutex*)mutex->data;
    return pthread_mutex_trylock(&_m->handle) == 0;
}

// Signal
static inline uint64_t sx__toNs(const struct timespec* _ts)
{
    return _ts->tv_sec * UINT64_C(1000000000) + _ts->tv_nsec;
}

static inline void sx__toTimespecNs(struct timespec* _ts, uint64_t _nsecs)
{
    _ts->tv_sec = _nsecs / UINT64_C(1000000000);
    _ts->tv_nsec = _nsecs % UINT64_C(1000000000);
}

static inline void sx__tm_add(struct timespec* _ts, int32_t _msecs)
{
    uint64_t ns = sx__toNs(_ts);
    sx__toTimespecNs(_ts, ns + (uint64_t)(_msecs)*1000000);
}

void sx_signal_init(sx_signal* sig)
{
    sx__signal* _sig = (sx__signal*)sig->data;
    _sig->value = 0;
    int r = pthread_mutex_init(&_sig->mutex, NULL);
    sx_assertf(r == 0, "pthread_mutex_init failed");

    r = pthread_cond_init(&_sig->cond, NULL);
    sx_assertf(r == 0, "pthread_cond_init failed");

    sx_unused(r);
}

void sx_signal_release(sx_signal* sig)
{
    sx__signal* _sig = (sx__signal*)sig->data;
    pthread_cond_destroy(&_sig->cond);
    pthread_mutex_destroy(&_sig->mutex);
}

void sx_signal_raise(sx_signal* sig)
{
    sx__signal* _sig = (sx__signal*)sig->data;
    int r = pthread_mutex_lock(&_sig->mutex);
    sx_assert(r == 0);
    _sig->value = 1;
    pthread_mutex_unlock(&_sig->mutex);
    pthread_cond_signal(&_sig->cond);
    sx_unused(r);
}

bool sx_signal_wait(sx_signal* sig, int msecs)
{
    sx__signal* _sig = (sx__signal*)sig->data;
    int r = pthread_mutex_lock(&_sig->mutex);
    sx_assert(r == 0);

    if (msecs == -1) {
        r = pthread_cond_wait(&_sig->cond, &_sig->mutex);
    } else {
        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        sx__tm_add(&ts, msecs);
        r = pthread_cond_timedwait(&_sig->cond, &_sig->mutex, &ts);
    }

    bool ok = r == 0;
    if (ok)
        _sig->value = 0;
    r = pthread_mutex_unlock(&_sig->mutex);
    sx_unused(r);
    return ok;
}


// Semaphore (posix only)
#    if !SX_PLATFORM_APPLE
void sx_semaphore_init(sx_sem* sem)
{
    sx__sem* _sem = (sx__sem*)sem->data;
    _sem->count = 0;
    int r = pthread_mutex_init(&_sem->mutex, NULL);
    sx_assertf(r == 0, "pthread_mutex_init failed");

    r = pthread_cond_init(&_sem->cond, NULL);
    sx_assertf(r == 0, "pthread_cond_init failed");

    sx_unused(r);
}

void sx_semaphore_release(sx_sem* sem)
{
    sx__sem* _sem = (sx__sem*)sem->data;
    pthread_cond_destroy(&_sem->cond);
    pthread_mutex_destroy(&_sem->mutex);
}

void sx_semaphore_post(sx_sem* sem, int count)
{
    sx__sem* _sem = (sx__sem*)sem->data;
    int r = pthread_mutex_lock(&_sem->mutex);
    sx_assert(r == 0);

    for (int ii = 0; ii < count; ii++) {
        r = pthread_cond_signal(&_sem->cond);
        sx_assert(r == 0);
    }

    _sem->count += count;
    r = pthread_mutex_unlock(&_sem->mutex);
    sx_assert(r == 0);

    sx_unused(r);
}

bool sx_semaphore_wait(sx_sem* sem, int msecs)
{
    sx__sem* _sem = (sx__sem*)sem->data;
    int r = pthread_mutex_lock(&_sem->mutex);
    sx_assert(r == 0);

    if (msecs == -1) {
        while (r == 0 && _sem->count <= 0) r = pthread_cond_wait(&_sem->cond, &_sem->mutex);
    } else {
        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        sx__tm_add(&ts, msecs);
        while (r == 0 && _sem->count <= 0)
            r = pthread_cond_timedwait(&_sem->cond, &_sem->mutex, &ts);
    }

    bool ok = r == 0;
    if (ok) {
        --_sem->count;
    }
    r = pthread_mutex_unlock(&_sem->mutex);
    sx_unused(r);
    return ok;
}
#    endif
#elif SX_PLATFORM_WINDOWS
// Tls
sx_tls sx_tls_create()
{
    DWORD tls_id = TlsAlloc();
    sx_assertf(tls_id != TLS_OUT_OF_INDEXES, "Failed to create tls!");
    return (sx_tls)(uintptr_t)tls_id;
}

void sx_tls_destroy(sx_tls tls)
{
    TlsFree((DWORD)(uintptr_t)tls);
}

void sx_tls_set(sx_tls tls, void* data)
{
    TlsSetValue((DWORD)(uintptr_t)tls, data);
}

void* sx_tls_get(sx_tls tls)
{
    return TlsGetValue((DWORD)(uintptr_t)tls);
}

// Mutex
void sx_mutex_init(sx_mutex* mutex)
{
    sx__mutex* _m = (sx__mutex*)mutex->data;
    InitializeCriticalSection(&_m->handle);
}

void sx_mutex_release(sx_mutex* mutex)
{
    sx__mutex* _m = (sx__mutex*)mutex->data;
    DeleteCriticalSection(&_m->handle);
}

void sx_mutex_enter(sx_mutex* mutex)
{
    sx__mutex* _m = (sx__mutex*)mutex->data;
    EnterCriticalSection(&_m->handle);
}

void sx_mutex_exit(sx_mutex* mutex)
{
    sx__mutex* _m = (sx__mutex*)mutex->data;
    LeaveCriticalSection(&_m->handle);
}

bool sx_mutex_try(sx_mutex* mutex)
{
    sx__mutex* _m = (sx__mutex*)mutex->data;
    return TryEnterCriticalSection(&_m->handle) == TRUE;
}

// Semaphore
void sx_semaphore_init(sx_sem* sem)
{
    sx__sem* _sem = (sx__sem*)sem->data;
    _sem->handle = CreateSemaphoreA(NULL, 0, LONG_MAX, NULL);
    sx_assertf(_sem->handle != NULL, "Failed to create semaphore");
}

void sx_semaphore_release(sx_sem* sem)
{
    sx__sem* _sem = (sx__sem*)sem->data;
    CloseHandle(_sem->handle);
}

void sx_semaphore_post(sx_sem* sem, int count)
{
    sx__sem* _sem = (sx__sem*)sem->data;
    ReleaseSemaphore(_sem->handle, count, NULL);
}

bool sx_semaphore_wait(sx_sem* sem, int msecs)
{
    sx__sem* _sem = (sx__sem*)sem->data;
    DWORD _msecs = (msecs < 0) ? INFINITE : msecs;
    return WaitForSingleObject(_sem->handle, _msecs) == WAIT_OBJECT_0;
}

// Signal
// https://github.com/mattiasgustavsson/libs/blob/master/thread.h
void sx_signal_init(sx_signal* sig)
{
    sx__signal* _sig = (sx__signal*)sig->data;
#    if _WIN32_WINNT >= 0x0600
    BOOL r = InitializeCriticalSectionAndSpinCount(&_sig->mutex, 32);
    sx_assertf(r, "InitializeCriticalSectionAndSpinCount failed");
    sx_unused(r);
    InitializeConditionVariable(&_sig->cond);
    _sig->value = 0;
#    else
    _sig->e = CreateEvent(NULL, FALSE, FALSE, NULL);
    sx_assertf(_sig->e, "CreateEvent failed");
#    endif
}

void sx_signal_release(sx_signal* sig)
{
    sx__signal* _sig = (sx__signal*)sig->data;
#    if _WIN32_WINNT >= 0x0600
    DeleteCriticalSection(&_sig->mutex);
#    else
    CloseHandle(_sig->e);
#    endif
}

void sx_signal_raise(sx_signal* sig)
{
    sx__signal* _sig = (sx__signal*)sig->data;
#    if _WIN32_WINNT >= 0x0600
    EnterCriticalSection(&_sig->mutex);
    _sig->value = 1;
    LeaveCriticalSection(&_sig->mutex);
    WakeConditionVariable(&_sig->cond);
#    else
    SetEvent(_sig->e);
#    endif
}

bool sx_signal_wait(sx_signal* sig, int msecs)
{
    sx__signal* _sig = (sx__signal*)sig->data;
#    if _WIN32_WINNT >= 0x0600
    bool timed_out = false;
    EnterCriticalSection(&_sig->mutex);
    DWORD _msecs = (msecs < 0) ? INFINITE : msecs;
    while (_sig->value == 0) {
        int r = SleepConditionVariableCS(&_sig->cond, &_sig->mutex, _msecs);
        if (!r && GetLastError() == ERROR_TIMEOUT) {
            timed_out = true;
            break;
        }
    }
    if (!timed_out)
        _sig->value = 0;
    LeaveCriticalSection(&_sig->mutex);
    return !timed_out;
#    else
    return WaitForSingleObject(_sig->e, msecs < 0 ? INFINITE : msecs) == WAIT_OBJECT_0;
#    endif
}

// Thread
static DWORD WINAPI thread_fn(LPVOID arg)
{
    sx_thread* thrd = (sx_thread*)arg;
    thrd->thread_id = GetCurrentThreadId();
    sx_semaphore_post(&thrd->sem, 1);
    return (DWORD)thrd->callback(thrd->user_data1, thrd->user_data2);
}

sx_thread* sx_thread_create(const sx_alloc* alloc, sx_thread_cb* callback, void* user_data1,
                            int stack_sz, const char* name, void* user_data2)
{
    sx_thread* thrd = (sx_thread*)sx_malloc(alloc, sizeof(sx_thread));
    if (!thrd)
        return NULL;

    sx_semaphore_init(&thrd->sem);
    thrd->callback = callback;
    thrd->user_data1 = user_data1;
    thrd->user_data2 = user_data2;
    thrd->stack_sz = sx_max(stack_sz, (int)sx_os_minstacksz());
    thrd->running = true;

    thrd->handle =
        CreateThread(NULL, thrd->stack_sz, (LPTHREAD_START_ROUTINE)thread_fn, thrd, 0, NULL);
    sx_assertf(thrd->handle != NULL, "CreateThread failed");

    // Ensure that thread callback is running
    sx_semaphore_wait(&thrd->sem, -1);

    if (name)
        sx_thread_setname(thrd, name);

    return thrd;
}

int sx_thread_destroy(sx_thread* thrd, const sx_alloc* alloc)
{
    sx_assert(thrd);
    sx_assertf(thrd->running, "Thread is not running!");

    DWORD exit_code;
    WaitForSingleObject(thrd->handle, INFINITE);
    GetExitCodeThread(thrd->handle, &exit_code);
    CloseHandle(thrd->handle);

    sx_semaphore_release(&thrd->sem);

    thrd->handle = INVALID_HANDLE_VALUE;
    thrd->running = false;

    sx_free(alloc, thrd);

    return (int)exit_code;
}

bool sx_thread_running(sx_thread* thrd)
{
    return thrd->running;
}

void sx_thread_yield()
{
    SwitchToThread();
}

#    pragma pack(push, 8)
struct _ThreadName {
    DWORD type;
    LPCSTR name;
    DWORD id;
    DWORD flags;
};
#    pragma pack(pop)
void sx_thread_setname(sx_thread* thrd, const char* name)
{
    struct _ThreadName tn;
    tn.type = 0x1000;
    tn.name = name;
    tn.id = thrd->thread_id;
    tn.flags = 0;

    #if !SX_CRT_MINGW
    __try {
    #endif

        RaiseException(0x406d1388, 0, sizeof(tn) / 4, (ULONG_PTR*)(&tn));

    #if !SX_CRT_MINGW
    } __except (EXCEPTION_EXECUTE_HANDLER) {
    }
    #endif
}

#else
#    error "Not implemented for this platform"
#endif

uint32_t sx_thread_tid(void)
{
#if SX_PLATFORM_WINDOWS
    return GetCurrentThreadId();
#elif SX_PLATFORM_LINUX || SX_PLATFORM_RPI || SX_PLATFORM_STEAMLINK
    return (pid_t)syscall(SYS_gettid);
#elif SX_PLATFORM_APPLE
    return (mach_port_t)pthread_mach_thread_np(pthread_self());
#elif SX_PLATFORM_BSD
    return *(uint32_t*)pthread_self();
#elif SX_PLATFORM_ANDROID
    return gettid();
#elif SX_PLATFORM_HURD
    return (pthread_t)pthread_self();
#else
    sx_assertf(0, "Tid not implemented");
#endif    // SX_PLATFORM_
}


#include "sx/threads.h"
#include "sx/os.h"

#if SX_PLATFORM_APPLE
#	include <dispatch/dispatch.h>
#	include <pthread.h>
#elif SX_PLATFORM_POSIX
#	include <errno.h>
#	include <pthread.h>
#	include <semaphore.h>
#	include <time.h>
#elif  SX_PLATFORM_WINDOWS
#   define VC_EXTRALEAN
#   define WIN32_LEAN_AND_MEAN
#	include <windows.h>
#	include <limits.h>
#endif // SX_PLATFORM_

#if SX_PLATFORM_BSD
#   include <pthread_np.h>
#endif

#if SX_PLATFORM_LINUX && (SX_CRT_GLIBC < 21200)
#   include <sys/prctl.h>
#endif

#include "sx/atomic.h"

typedef struct sx__mutex_s
{
#if SX_PLATFORM_WINDOWS
    CRITICAL_SECTION handle;
#elif SX_PLATFORM_POSIX
    pthread_mutex_t handle;
#endif
} sx__mutex;

typedef struct sx__sem_s
{
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

typedef struct sx__signal_s
{
#if SX_PLATFORM_WINDOWS
    CRITICAL_SECTION mutex;
    CONDITION_VARIABLE cond;
    int value;
#elif SX_PLATFORM_POSIX
#endif
} sx__signal;

typedef struct sx__thread_s
{
    sx_sem sem;
    sx_thread_cb* callback;

#if SX_PLATFORM_WINDOWS
    HANDLE handle;
    DWORD thread_id;
#elif SX_PLATFORM_POSIX
    pthread_t handle;
#   if SX_PLATFORM_APPLE
    char name[32];
#   endif
#endif

    void* user_data;
    int stack_sz;
    bool running;
} sx_thread;

// Apple has different implementation for semaphores
#if SX_PLATFORM_APPLE
#endif

// Other implementations are either Posix or Win32
#if SX_PLATFORM_POSIX
// Thread

// Tls

// Mutex

// Semaphore (posix only)
#   if !SX_PLATFORM_APPLE

#   endif
#elif SX_PLATFORM_WINDOWS

static_assert(sizeof(sx__mutex) <= sizeof(sx_mutex), "sx_mutex size mismatch");
static_assert(sizeof(sx__sem) <= sizeof(sx_sem), "sx_sem size mismatch");
static_assert(sizeof(sx__signal) <= sizeof(sx_signal), "sx_signal size mismatch");

// Tls
sx_tls sx_tls_create()
{
    DWORD tls_id = TlsAlloc();
    assert(tls_id != TLS_OUT_OF_INDEXES && "Failed to create tls");
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

void sx_mutex_lock(sx_mutex* mutex)
{
    sx__mutex* _m = (sx__mutex*)mutex->data;
    EnterCriticalSection(&_m->handle);
}

void sx_mutex_unlock(sx_mutex* mutex)
{
    sx__mutex* _m = (sx__mutex*)mutex->data;
    LeaveCriticalSection(&_m->handle);
}

bool sx_mutex_trylock(sx_mutex* mutex)
{
    sx__mutex* _m = (sx__mutex*)mutex->data;
    return TryEnterCriticalSection(&_m->handle);
}

// Semaphore
void sx_semaphore_init(sx_sem* sem)
{
    sx__sem* _sem = (sx__sem*)sem->data;
    _sem->handle = CreateSemaphoreA(NULL, 0, LONG_MAX, NULL);
    assert(_sem->handle != NULL && "Failed to create semaphore");
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
    InitializeCriticalSectionAndSpinCount(&_sig->mutex, 32);
    InitializeConditionVariable(&_sig->cond);
    _sig->value = 0;
}

void sx_signal_release(sx_signal* sig)
{
    sx__signal* _sig = (sx__signal*)sig->data;
    DeleteCriticalSection(&_sig->mutex);
}

void sx_signal_raise(sx_signal* sig)
{
    sx__signal* _sig = (sx__signal*)sig->data;
    EnterCriticalSection(&_sig->mutex);
    _sig->value = 1;
    LeaveCriticalSection(&_sig->mutex);
    WakeConditionVariable(&_sig->cond);
}

bool sx_signal_wait(sx_signal* sig, int msecs)
{
    sx__signal* _sig = (sx__signal*)sig->data;
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
}

// Thread
static DWORD WINAPI thread_fn(LPVOID arg)
{
    sx_thread* thrd = (sx_thread*)arg;
    thrd->thread_id = GetCurrentThreadId();
    sx_semaphore_post(&thrd->sem, 1);
    return (DWORD)thrd->callback(thrd->user_data);
}

sx_thread* sx_thread_create(const sx_alloc* alloc, sx_thread_cb* callback,
                              void* user_data, int stack_sz, const char* name)
{
    sx_thread* thrd = (sx_thread*)sx_malloc(alloc, sizeof(sx_thread));
    if (!thrd)
        return NULL;

    sx_semaphore_init(&thrd->sem);
    thrd->callback = callback;
    thrd->user_data = user_data;
    thrd->stack_sz = sx_max(stack_sz, sx_os_minstacksz());
    thrd->running = true;

    thrd->handle = CreateThread(NULL, thrd->stack_sz, (LPTHREAD_START_ROUTINE)thread_fn, thrd, 0, NULL);

    // Ensure that thread callback is running
    sx_semaphore_wait(&thrd->sem, -1);

    if (name)
        sx_thread_setname(thrd, name);

    return thrd;
}

int sx_thread_destroy(sx_thread* thrd, const sx_alloc* alloc)
{
    assert(thrd->running && "Thread is not running!");

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

#	pragma pack(push, 8)
struct _ThreadName
{
    DWORD  type;
    LPCSTR name;
    DWORD  id;
    DWORD  flags;
};
#	pragma pack(pop)
void sx_thread_setname(sx_thread* thrd, const char* name)
{
    struct _ThreadName tn;
    tn.type  = 0x1000;
    tn.name  = name;
    tn.id    = thrd->thread_id;
    tn.flags = 0;    

    __try {
        RaiseException(0x406d1388
                , 0
                , sizeof(tn)/4
                , (ULONG_PTR*)(&tn)
                );
    } __except(EXCEPTION_EXECUTE_HANDLER) {
    }    
}

#else
#   error "Not implemented for this platform"
#endif

// Sp-Sc-Queue
// Reference: http://www.drdobbs.com/parallel/writing-lock-free-code-a-corrected-queue/210604448?pgno=1
typedef struct _sx_queue_spsc_node
{
    struct _sx_queue_spsc_node* next;
} _sx_queue_spsc_node;

typedef struct sx_queue_spsc
{
    _sx_queue_spsc_node** ptrs;
    uint8_t*              buff;
    int                   iter;
    int                   capacity;
    int                   stride;

    _sx_queue_spsc_node*  first;
    sx_atomic_ptr         last;
    sx_atomic_ptr         divider;
} sx_queue_spsc;


sx_queue_spsc* sx_queue_spsc_create(const sx_alloc* alloc, int item_sz, int capacity)
{
    assert(item_sz > 0);

    capacity = sx_align_mask(capacity, 15);
    uint8_t* buff = (uint8_t*)sx_malloc(alloc,
                                        sizeof(sx_queue_spsc) + (item_sz + sizeof(void*) + sizeof(_sx_queue_spsc_node))*capacity);
    if (!buff) {
        SX_OUT_OF_MEMORY;
        return NULL;
    }

    sx_queue_spsc* queue = (sx_queue_spsc*)buff;
    buff += sizeof(sx_queue_spsc);
    queue->ptrs = (_sx_queue_spsc_node**)buff;
    buff += sizeof(_sx_queue_spsc_node*)*capacity;
    queue->buff = buff;

    queue->iter = capacity;
    queue->capacity = capacity;
    queue->stride = item_sz;

    for (int i = 0; i < capacity; i++) {
        queue->ptrs[capacity - i - 1] = (_sx_queue_spsc_node*)
            (queue->buff + (sizeof(_sx_queue_spsc_node) + item_sz)*i);
    }

    // initialize dummy node
    _sx_queue_spsc_node* node = queue->ptrs[--queue->iter];
    node->next = NULL;
    queue->first = node;
    queue->divider = queue->last = node;

    return queue;
}

void sx_queue_spsc_destroy(sx_queue_spsc* queue, const sx_alloc* alloc)
{
    assert(queue);
    queue->capacity = queue->iter = 0;
    sx_free(alloc, queue);
}

bool sx_queue_spsc_produce(sx_queue_spsc* queue, const void* data)
{
    if (queue->iter > 0) {
        _sx_queue_spsc_node* node = queue->ptrs[--queue->iter];
        memcpy(node + 1, data, queue->stride);
        node->next = NULL;

        _sx_queue_spsc_node* last = (_sx_queue_spsc_node*)queue->last;
        last->next = node;

        sx_atomic_xchg_ptr(&queue->last, node);

        // trim unsused nodes
        while (queue->first != queue->divider) {
            _sx_queue_spsc_node* first = (_sx_queue_spsc_node*)queue->first;
            queue->first = first->next;
            assert(queue->iter != queue->capacity);
            queue->ptrs[queue->iter++] = first;
        }
        return true;
    } else {
        return false;
    }
}

bool sx_queue_spsc_consume(sx_queue_spsc* queue, void* data)
{
    if (queue->divider != queue->last) {
        _sx_queue_spsc_node* divider = (_sx_queue_spsc_node*)queue->divider;
        assert(divider->next);
        memcpy(data, divider->next + 1, queue->stride);
        
        sx_atomic_xchg_ptr(&queue->divider, divider->next);
        return true;
    }

    return false;
}


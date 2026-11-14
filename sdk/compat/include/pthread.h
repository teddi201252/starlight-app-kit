/* libpeos-compat: POSIX threads over PeOS's (peos/thread.h): a thread is a
 * kernel thread of the process; mutexes and condition variables wait on a
 * word of memory (wait's address items). What libstdc++'s std::thread,
 * std::mutex, std::condition_variable and std::async are built on. */
#pragma once
#include <sched.h>
#include <stdint.h>
#include <sys/types.h>
#include <time.h>

typedef struct peos_pthread *pthread_t;
typedef struct { uint32_t detached; size_t stacksize; } pthread_attr_t;
typedef struct {
    volatile uint32_t state;   /* 0 free, 1 held, 2 held with waiters */
    int kind;                  /* PTHREAD_MUTEX_NORMAL / RECURSIVE / ERRORCHECK */
    pthread_t owner;
    unsigned count;            /* recursive depth */
} pthread_mutex_t;
typedef struct { int kind; } pthread_mutexattr_t;
typedef struct {
    volatile uint32_t seq;
    int clock;
} pthread_cond_t;
typedef struct { int clock; } pthread_condattr_t;
typedef struct {
    pthread_mutex_t lock;
    pthread_cond_t cond;
    int readers;               /* -1: a writer */
    int writers_waiting;
} pthread_rwlock_t;
typedef struct { int unused; } pthread_rwlockattr_t;
typedef volatile uint32_t pthread_once_t;
typedef unsigned pthread_key_t;
typedef volatile uint32_t pthread_spinlock_t;
typedef struct {
    pthread_mutex_t lock;
    pthread_cond_t cond;
    unsigned count, waiting, cycle;
} pthread_barrier_t;
typedef struct { int unused; } pthread_barrierattr_t;
#define PTHREAD_BARRIER_SERIAL_THREAD (-1)

#define PTHREAD_MUTEX_NORMAL 0
#define PTHREAD_MUTEX_RECURSIVE 1
#define PTHREAD_MUTEX_ERRORCHECK 2
#define PTHREAD_MUTEX_DEFAULT PTHREAD_MUTEX_NORMAL
#define PTHREAD_MUTEX_INITIALIZER { 0u, PTHREAD_MUTEX_NORMAL, 0, 0u }
#define PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP { 0u, PTHREAD_MUTEX_RECURSIVE, 0, 0u }
#define PTHREAD_COND_INITIALIZER { 0u, CLOCK_REALTIME }
#define PTHREAD_RWLOCK_INITIALIZER { PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER, 0, 0 }
#define PTHREAD_ONCE_INIT 0u
#define PTHREAD_CREATE_JOINABLE 0
#define PTHREAD_CREATE_DETACHED 1
#define PTHREAD_CANCEL_ENABLE 0
#define PTHREAD_CANCEL_DISABLE 1
#define PTHREAD_PROCESS_PRIVATE 0

#ifdef __cplusplus
extern "C" {
#endif
int pthread_create(pthread_t *out, const pthread_attr_t *attr, void *(*fn)(void *), void *arg);
int pthread_join(pthread_t t, void **ret);
int pthread_detach(pthread_t t);
__attribute__((noreturn)) void pthread_exit(void *ret);
pthread_t pthread_self(void);
int pthread_equal(pthread_t a, pthread_t b);
int pthread_once(pthread_once_t *once, void (*fn)(void));
int pthread_key_create(pthread_key_t *key, void (*destructor)(void *));
int pthread_key_delete(pthread_key_t key);
void *pthread_getspecific(pthread_key_t key);
int pthread_setspecific(pthread_key_t key, const void *value);
int pthread_attr_init(pthread_attr_t *a);
int pthread_attr_destroy(pthread_attr_t *a);
int pthread_attr_setdetachstate(pthread_attr_t *a, int state);
int pthread_attr_getdetachstate(const pthread_attr_t *a, int *state);
int pthread_attr_setstacksize(pthread_attr_t *a, size_t size);
int pthread_attr_getstacksize(const pthread_attr_t *a, size_t *size);
/* Scheduling of a new thread: PeOS's user threads have no policies or
 * priorities. Taken (0), no effect; the getters say SCHED_OTHER, 0. */
#define PTHREAD_INHERIT_SCHED 0
#define PTHREAD_EXPLICIT_SCHED 1
int pthread_attr_setschedpolicy(pthread_attr_t *a, int policy);
int pthread_attr_getschedpolicy(const pthread_attr_t *a, int *policy);
int pthread_attr_setschedparam(pthread_attr_t *a, const struct sched_param *param);
int pthread_attr_getschedparam(const pthread_attr_t *a, struct sched_param *param);
int pthread_attr_setinheritsched(pthread_attr_t *a, int inherit);
int pthread_attr_getinheritsched(const pthread_attr_t *a, int *inherit);
int pthread_mutex_init(pthread_mutex_t *m, const pthread_mutexattr_t *attr);
int pthread_mutex_destroy(pthread_mutex_t *m);
int pthread_mutex_lock(pthread_mutex_t *m);
int pthread_mutex_trylock(pthread_mutex_t *m);
int pthread_mutex_timedlock(pthread_mutex_t *m, const struct timespec *abstime);
int pthread_mutex_clocklock(pthread_mutex_t *m, clockid_t clock, const struct timespec *abstime);
int pthread_mutex_unlock(pthread_mutex_t *m);
int pthread_mutexattr_init(pthread_mutexattr_t *a);
int pthread_mutexattr_destroy(pthread_mutexattr_t *a);
int pthread_mutexattr_settype(pthread_mutexattr_t *a, int kind);
int pthread_mutexattr_gettype(const pthread_mutexattr_t *a, int *kind);
int pthread_cond_init(pthread_cond_t *c, const pthread_condattr_t *attr);
int pthread_cond_destroy(pthread_cond_t *c);
int pthread_cond_wait(pthread_cond_t *c, pthread_mutex_t *m);
int pthread_cond_timedwait(pthread_cond_t *c, pthread_mutex_t *m, const struct timespec *abstime);
int pthread_cond_clockwait(pthread_cond_t *c, pthread_mutex_t *m, clockid_t clock, const struct timespec *abstime);
int pthread_cond_signal(pthread_cond_t *c);
int pthread_cond_broadcast(pthread_cond_t *c);
int pthread_condattr_init(pthread_condattr_t *a);
int pthread_condattr_destroy(pthread_condattr_t *a);
int pthread_condattr_setclock(pthread_condattr_t *a, clockid_t clock);
int pthread_rwlock_init(pthread_rwlock_t *l, const pthread_rwlockattr_t *attr);
int pthread_rwlock_destroy(pthread_rwlock_t *l);
int pthread_rwlock_rdlock(pthread_rwlock_t *l);
int pthread_rwlock_tryrdlock(pthread_rwlock_t *l);
int pthread_rwlock_wrlock(pthread_rwlock_t *l);
int pthread_rwlock_trywrlock(pthread_rwlock_t *l);
int pthread_rwlock_timedrdlock(pthread_rwlock_t *l, const struct timespec *abstime);
int pthread_rwlock_timedwrlock(pthread_rwlock_t *l, const struct timespec *abstime);
int pthread_rwlock_unlock(pthread_rwlock_t *l);
int pthread_barrier_init(pthread_barrier_t *b, const pthread_barrierattr_t *a, unsigned count);
int pthread_barrier_destroy(pthread_barrier_t *b);
int pthread_barrier_wait(pthread_barrier_t *b);
int pthread_setname_np(pthread_t t, const char *name);   /* kept nowhere: 0 */
int pthread_getcpuclockid(pthread_t t, clockid_t *clock);  /* not measured: ENOENT */
int pthread_spin_init(pthread_spinlock_t *l, int pshared);
int pthread_spin_destroy(pthread_spinlock_t *l);
int pthread_spin_lock(pthread_spinlock_t *l);
int pthread_spin_trylock(pthread_spinlock_t *l);
int pthread_spin_unlock(pthread_spinlock_t *l);
int pthread_setcancelstate(int state, int *old);
int pthread_cancel(pthread_t t);
int pthread_getschedparam(pthread_t t, int *policy, struct sched_param *param);
int pthread_setschedparam(pthread_t t, int policy, const struct sched_param *param);
int pthread_sigmask(int how, const void *set, void *old);
#ifdef __cplusplus
}
#endif

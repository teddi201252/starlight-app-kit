/* libpeos-compat: POSIX semaphores. The unnamed ones (sem_init) are real,
 * over pthread's mutex and condition (waiting sleeps: nobody spins). The
 * named ones (sem_open, shared between processes) are not on PeOS:
 * SEM_FAILED, ENOSYS. */
#pragma once
#include <pthread.h>
#include <time.h>

typedef struct {
    pthread_mutex_t lock;
    pthread_cond_t cond;
    unsigned int value;
} sem_t;

#define SEM_FAILED ((sem_t *)0)
#define SEM_VALUE_MAX 0x7fffffff

#ifdef __cplusplus
extern "C" {
#endif
int sem_init(sem_t *sem, int pshared, unsigned int value);
int sem_destroy(sem_t *sem);
int sem_wait(sem_t *sem);
int sem_trywait(sem_t *sem);
int sem_timedwait(sem_t *sem, const struct timespec *abstime);
int sem_post(sem_t *sem);
int sem_getvalue(sem_t *sem, int *value);
sem_t *sem_open(const char *name, int oflag, ...);
int sem_close(sem_t *sem);
int sem_unlink(const char *name);
#ifdef __cplusplus
}
#endif

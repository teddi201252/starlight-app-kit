/* Threads of a PeOS program (d2decomped port, phase 1): what the kernel gives
 * (a thread shares its process's descriptors, memory and folder; waiting on a
 * word of memory inside peos_wait), and a mutex built on it.
 *
 *   peos_thread_spawn(entry, arg, stack_top)   a thread (peos.h); it may set
 *       its own thread-local storage with peos_tls_set, and ends with
 *       peos_thread_exit (peos_exit ends the whole process, every thread).
 *   Join: peos_waitpid(tid): any thread of the process may reap any other,
 *       with no capability (peos_proc_open, to wait on it beside other
 *       descriptors, needs "process").
 *
 *   static peos_mutex_t m = PEOS_MUTEX_INIT;
 *   peos_mutex_lock(&m); ... peos_mutex_unlock(&m);
 *
 * The mutex costs one atomic instruction when nobody else holds it; a thread
 * that finds it held waits in the kernel (peos_wait_word) until woken. */
#ifndef PEOS_THREAD_H
#define PEOS_THREAD_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

#include "peos/event_abi.h"

/* Ends only this thread (the process goes on; the last thread's exit, or
 * peos_exit from any of them, ends it). */
__attribute__((noreturn)) void peos_thread_exit(int code);

/* This thread's %fs base: its thread-local storage block (the x86-64 ABI
 * wants its own address at %fs:0). 0, or -1 if not a user address. */
int64_t peos_tls_set(void *base);

/* Wait while the 32-bit word at addr is `expect` (or until a wake on it, or
 * the timeout): 1 when it changed or was woken, 0 on timeout, -1 if addr is
 * not a 4-aligned user address. Wakes can be spurious: look again. */
int64_t peos_wait_word(const volatile uint32_t *addr, uint32_t expect, uint64_t timeout_ns);
/* Wakes up to n threads of this process waiting on that word: how many. */
int64_t peos_wake(const volatile uint32_t *addr, uint32_t n);
/* peos_wait with words of memory beside the descriptors (count may be 0). */
int64_t peos_wait_addrs(peos_wait_item_t *items, size_t count, peos_wait_addr_t *addrs, size_t naddrs,
                        uint64_t timeout_ns);

/* How many CPUs PeOS runs on (1 if it cannot tell). */
uint32_t peos_cpu_count(void);
/* Whether this program has started a thread: libpeos locks only then. */
int peos_threads_started(void);

typedef struct {
    volatile uint32_t state;   /* 0 free, 1 held, 2 held with someone waiting */
} peos_mutex_t;
#define PEOS_MUTEX_INIT {0u}

void peos_mutex_lock(peos_mutex_t *m);
int peos_mutex_trylock(peos_mutex_t *m);   /* 1 taken, 0 held by another */
void peos_mutex_unlock(peos_mutex_t *m);

#ifdef __cplusplus
}
#endif
#endif

#pragma once

#include <stdint.h>

/* SYS_PROC_LIST: the processes running, one entry per address space (its
 * threads counted, their CPU time summed), readable by any task.
 * SYS_PROC_KILL: ends a process (all its threads); it needs PEOS_CAP_MANAGE,
 * and only a process holding no capability the caller lacks can be ended
 * (never init, pid 1). The process dies at its next syscall or timer tick in
 * user mode; one blocked with no timeout dies when it wakes. */

#define PEOS_PROC_NAME_MAX 32u
#define PEOS_PROC_MAX 64u

#define PEOS_PROC_APP     (1u << 0)   /* started from an app bundle (*.app/app.bin) */
#define PEOS_PROC_KILLING (1u << 1)   /* asked to end, not dead yet */

typedef struct {
    uint32_t pid;
    uint32_t parent;
    uint32_t threads;
    uint32_t flags;                  /* PEOS_PROC_* */
    uint64_t cycles;                 /* TSC cycles run since it started, threads included */
    uint64_t capabilities;           /* what it holds (PEOS_CAP_*) */
    char name[PEOS_PROC_NAME_MAX];   /* the program's file name; for an app, its bundle id */
} peos_proc_info_t;

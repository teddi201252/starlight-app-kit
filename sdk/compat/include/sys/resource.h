/* libpeos-compat: sys/resource.h for ports. PeOS has no nice values and no
 * per-process limits a program can lower: getrlimit says "no limit" (the
 * kernel's own limits stand), setrlimit and setpriority are accepted and do
 * nothing, getpriority is 0, getrusage gives the process's CPU time. */
#pragma once
#include <sys/time.h>
#include <sys/types.h>

typedef unsigned long rlim_t;
#define RLIM_INFINITY (~0ul)
#define RLIM_SAVED_MAX RLIM_INFINITY
#define RLIM_SAVED_CUR RLIM_INFINITY

struct rlimit {
    rlim_t rlim_cur;
    rlim_t rlim_max;
};

#define RLIMIT_CPU 0
#define RLIMIT_FSIZE 1
#define RLIMIT_DATA 2
#define RLIMIT_STACK 3
#define RLIMIT_CORE 4
#define RLIMIT_RSS 5
#define RLIMIT_NPROC 6
#define RLIMIT_NOFILE 7
#define RLIMIT_MEMLOCK 8
#define RLIMIT_AS 9

#define PRIO_PROCESS 0
#define PRIO_PGRP 1
#define PRIO_USER 2

#define RUSAGE_SELF 0
#define RUSAGE_CHILDREN (-1)
#define RUSAGE_THREAD 1

struct rusage {
    struct timeval ru_utime;
    struct timeval ru_stime;
    long ru_maxrss, ru_ixrss, ru_idrss, ru_isrss, ru_minflt, ru_majflt, ru_nswap;
    long ru_inblock, ru_oublock, ru_msgsnd, ru_msgrcv, ru_nsignals, ru_nvcsw, ru_nivcsw;
};

#ifdef __cplusplus
extern "C" {
#endif
int getrlimit(int resource, struct rlimit *rlim);
int setrlimit(int resource, const struct rlimit *rlim);
int getpriority(int which, id_t who);
int setpriority(int which, id_t who, int prio);
int getrusage(int who, struct rusage *usage);
#ifdef __cplusplus
}
#endif

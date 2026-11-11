/* libpeos-compat: time.h for ports, over the base's: timespec, the clocks. */
#pragma once
#ifdef __cplusplus
extern "C" {
#endif
#include <peos-base/time.h>
#ifdef __cplusplus
}
#endif

typedef int clockid_t;
struct timespec {
    time_t tv_sec;
    long tv_nsec;
};
#define CLOCK_REALTIME 0
#define CLOCK_MONOTONIC 1
#define CLOCK_PROCESS_CPUTIME_ID 2
#define CLOCK_THREAD_CPUTIME_ID 3
#define TIME_UTC 1

#ifdef __cplusplus
extern "C" {
#endif
int clock_gettime(clockid_t clock, struct timespec *ts);
int clock_getres(clockid_t clock, struct timespec *res);
int nanosleep(const struct timespec *req, struct timespec *rem);
int timespec_get(struct timespec *ts, int base);
void tzset(void);
#ifdef __cplusplus
}
#endif

/* libpeos-compat: sys/syscall.h. PeOS's own system calls are not Linux's:
 * syscall() knows SYS_gettid (the thread's id) and answers ENOSYS to the
 * rest. */
#pragma once

#define SYS_gettid 186

#ifdef __cplusplus
extern "C" {
#endif
long syscall(long number, ...);
#ifdef __cplusplus
}
#endif

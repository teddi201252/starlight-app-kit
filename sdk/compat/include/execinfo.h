/* libpeos-compat: execinfo (glibc's backtrace) for ports' crash reports.
 * PeOS gives no stack walk to programs: backtrace finds no frames (0), and
 * the reports say so instead of listing them. */
#pragma once
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif
int backtrace(void **buffer, int size);
char **backtrace_symbols(void *const *buffer, int size);
void backtrace_symbols_fd(void *const *buffer, int size, int fd);
#ifdef __cplusplus
}
#endif

/* libpeos-compat: string.h for ports, over the base's. */
#pragma once
#ifdef __cplusplus
extern "C" {
#endif
#include <peos-base/string.h>
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif
int strcoll(const char *a, const char *b);
size_t strxfrm(char *dst, const char *src, size_t n);
int strerror_r(int error, char *buf, size_t n);
char *strtok_r(char *s, const char *delim, char **save);
char *strsignal(int sig);
void *memrchr(const void *s, int c, size_t n);
#ifdef __cplusplus
}
#endif

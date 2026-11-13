/* libpeos-compat: the C stdlib a port expects, over the base's. */
#pragma once
#include <alloca.h>   /* glibc's stdlib.h has alloca too: ports count on it */
#ifdef __cplusplus
extern "C" {
#endif
#include <peos-base/stdlib.h>
#ifdef __cplusplus
}
#endif
#include <stddef.h>

typedef struct { int quot, rem; } div_t;
typedef struct { long quot, rem; } ldiv_t;
typedef struct { long long quot, rem; } lldiv_t;
#define MB_CUR_MAX ((size_t)1)

#ifdef __cplusplus
extern "C" {
#endif
long atol(const char *s);
long long atoll(const char *s);
div_t div(int a, int b);
ldiv_t ldiv(long a, long b);
lldiv_t lldiv(long long a, long long b);
__attribute__((noreturn)) void _Exit(int status);
__attribute__((noreturn)) void quick_exit(int status);
int at_quick_exit(void (*fn)(void));
void *aligned_alloc(size_t align, size_t size);
int posix_memalign(void **out, size_t align, size_t size);
void *memalign(size_t align, size_t size);
int mblen(const char *s, size_t n);
int mbtowc(wchar_t *pwc, const char *s, size_t n);
int wctomb(char *s, wchar_t wc);
size_t mbstowcs(wchar_t *dst, const char *src, size_t n);
size_t wcstombs(char *dst, const wchar_t *src, size_t n);
char *realpath(const char *path, char *resolved);
int mkstemp(char *tmpl);
int mkstemps(char *tmpl, int suffix_len);
int setenv(const char *name, const char *value, int overwrite);
int unsetenv(const char *name);
char *secure_getenv(const char *name);
int rand_r(unsigned int *seed);
#ifdef __cplusplus
}
#endif

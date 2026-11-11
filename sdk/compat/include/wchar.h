/* libpeos-compat: wide characters, only what libstdc++'s headers name
 * (it is built without wchar_t support: --disable-wchar_t). UTF-8 is the
 * only multibyte encoding. */
#pragma once
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
typedef unsigned int wint_t;
typedef struct { unsigned count; unsigned value; } mbstate_t;
#define WEOF ((wint_t)-1)
#ifndef WCHAR_MIN
#define WCHAR_MIN (-2147483647 - 1)
#define WCHAR_MAX 2147483647
#endif
#ifdef __cplusplus
extern "C" {
#endif
size_t wcslen(const wchar_t *s);
int wcscmp(const wchar_t *a, const wchar_t *b);
wchar_t *wmemcpy(wchar_t *d, const wchar_t *s, size_t n);
wchar_t *wmemmove(wchar_t *d, const wchar_t *s, size_t n);
wchar_t *wmemset(wchar_t *d, wchar_t c, size_t n);
int wmemcmp(const wchar_t *a, const wchar_t *b, size_t n);
wchar_t *wmemchr(const wchar_t *s, wchar_t c, size_t n);
wint_t btowc(int c);
int wctob(wint_t c);
int mbsinit(const mbstate_t *ps);
size_t mbrlen(const char *s, size_t n, mbstate_t *ps);
size_t mbrtowc(wchar_t *pwc, const char *s, size_t n, mbstate_t *ps);
size_t wcrtomb(char *s, wchar_t wc, mbstate_t *ps);
#ifdef __cplusplus
}
/* libstdc++ is built without wchar_t, so <cwchar> puts none of these in
 * std::; code that names std::wcslen (CLI11) finds them here */
namespace std {
using ::btowc;
using ::mbrlen;
using ::mbrtowc;
using ::mbsinit;
using ::mbstate_t;
using ::wcrtomb;
using ::wcscmp;
using ::wcslen;
using ::wctob;
using ::wint_t;
using ::wmemchr;
using ::wmemcmp;
using ::wmemcpy;
using ::wmemmove;
using ::wmemset;
}
#endif

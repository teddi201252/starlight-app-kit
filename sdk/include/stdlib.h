#pragma once
#include <stddef.h>
#include "peos/alloc.h"

int abs(int v);
__attribute__((noreturn)) void abort(void);
int atoi(const char *s);
long strtol(const char *s, char **endp, int base);
unsigned long strtoul(const char *s, char **endp, int base);
long long strtoll(const char *s, char **endp, int base);
unsigned long long strtoull(const char *s, char **endp, int base);
double strtod(const char *s, char **endp);
float strtof(const char *s, char **endp);
long double strtold(const char *s, char **endp);
char *strdup(const char *s);
long labs(long v);
long long llabs(long long v);
#define RAND_MAX 0x7fffffff
int rand(void);
void srand(unsigned int seed);
int atexit(void (*fn)(void));
void *bsearch(const void *key, const void *base, size_t count, size_t size,
              int (*compare)(const void *, const void *));
#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1
double atof(const char *s);
__attribute__((noreturn)) void exit(int status);
int system(const char *cmd);
char *getenv(const char *name);
int putenv(char *string);
void qsort(void *base, size_t count, size_t size,
           int (*compare)(const void *, const void *));

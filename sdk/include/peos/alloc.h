#pragma once
#ifdef __cplusplus
extern "C" {
#endif
#include <stddef.h>

void *malloc(size_t size);
void free(void *ptr);
void *realloc(void *ptr, size_t size);
void *calloc(size_t n, size_t sz);
/* Aligned to align (a power of two); freed with free. */
void *peos_memalign(size_t align, size_t size);
#ifdef __cplusplus
}
#endif

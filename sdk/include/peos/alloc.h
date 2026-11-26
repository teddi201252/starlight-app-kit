#pragma once
/* A named guard too: the compat build sees this header from two places
 * (userland/include and its copy under peos-base/), and #pragma once
 * treats them as two files. */
#ifndef PEOS_ALLOC_H
#define PEOS_ALLOC_H
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

/* What malloc holds from the system now (bytes): its arenas (1 MB each)
 * with the pages they gave back, the big blocks that are mappings of their
 * own, and the free bytes in the arenas. Walks the free lists: for a log
 * line now and then, not for every frame. */
typedef struct {
    size_t arenas, arena_bytes;     /* arenas mapped */
    size_t released_bytes;          /* of them, pages given back (peos_mrelease) */
    size_t big_blocks, big_bytes;   /* blocks of their own mapping */
    size_t free_blocks, free_bytes; /* free in the arenas (given back pages included) */
} peos_alloc_stats_t;
void peos_alloc_stats(peos_alloc_stats_t *out);
#ifdef __cplusplus
}
#endif
#endif /* PEOS_ALLOC_H */

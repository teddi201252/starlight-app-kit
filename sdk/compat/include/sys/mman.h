/* libpeos-compat: sys/mman.h for ports. Anonymous private memory only
 * (PeOS's mmap); a mapping of a file descriptor fails with ENODEV: the
 * native ways are peos_shm_map_fd and peos_gpu_bo_map. */
#pragma once
#include <stddef.h>
#include <sys/types.h>

#define PROT_NONE 0x0
#define PROT_READ 0x1
#define PROT_WRITE 0x2
#define PROT_EXEC 0x4
#define MAP_SHARED 0x01
#define MAP_PRIVATE 0x02
#define MAP_FIXED 0x10
#define MAP_ANONYMOUS 0x20
#define MAP_ANON MAP_ANONYMOUS
#define MAP_NORESERVE 0x4000
#define MAP_FAILED ((void *)-1)
#define MADV_NORMAL 0
#define MADV_DONTNEED 4

#ifdef __cplusplus
extern "C" {
#endif
void *mmap(void *addr, size_t len, int prot, int flags, int fd, off_t off);
int munmap(void *addr, size_t len);
int mprotect(void *addr, size_t len, int prot);
int madvise(void *addr, size_t len, int advice);
/* PeOS does not swap: memory is always locked in. */
int mlock(const void *addr, size_t len);
int munlock(const void *addr, size_t len);
#ifdef __cplusplus
}
#endif

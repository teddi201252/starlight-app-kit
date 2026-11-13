/* libpeos-compat: select, over poll (and so over peos_wait). */
#pragma once
#include <sys/time.h>
#include <sys/types.h>

#define FD_SETSIZE 1024

typedef struct {
    unsigned long fds_bits[FD_SETSIZE / (8 * sizeof(unsigned long))];
} fd_set;

#define __FD_BIT(fd) (1ul << ((unsigned)(fd) % (8 * sizeof(unsigned long))))
#define __FD_WORD(fd) ((unsigned)(fd) / (8 * sizeof(unsigned long)))
#define FD_ZERO(set) __builtin_memset((set), 0, sizeof(fd_set))
#define FD_SET(fd, set) ((set)->fds_bits[__FD_WORD(fd)] |= __FD_BIT(fd))
#define FD_CLR(fd, set) ((set)->fds_bits[__FD_WORD(fd)] &= ~__FD_BIT(fd))
#define FD_ISSET(fd, set) (((set)->fds_bits[__FD_WORD(fd)] & __FD_BIT(fd)) != 0)

#ifdef __cplusplus
extern "C" {
#endif
int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);
#ifdef __cplusplus
}
#endif

/* libpeos-compat: poll over peos_wait (the descriptors' readiness). */
#pragma once

#define POLLIN 0x001
#define POLLPRI 0x002
#define POLLOUT 0x004
#define POLLERR 0x008
#define POLLHUP 0x010
#define POLLNVAL 0x020
#define POLLRDNORM 0x040
#define POLLWRNORM 0x100

typedef unsigned long nfds_t;
struct pollfd {
    int fd;
    short events;
    short revents;
};

#ifdef __cplusplus
extern "C" {
#endif
int poll(struct pollfd *fds, nfds_t n, int timeout_ms);
#ifdef __cplusplus
}
#endif

/* libpeos-compat: writev and readv. */
#pragma once
#include <sys/types.h>
struct iovec {
    void *iov_base;
    size_t iov_len;
};
#ifdef __cplusplus
extern "C" {
#endif
ssize_t writev(int fd, const struct iovec *iov, int count);
ssize_t readv(int fd, const struct iovec *iov, int count);
#ifdef __cplusplus
}
#endif

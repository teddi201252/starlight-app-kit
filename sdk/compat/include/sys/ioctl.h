/* libpeos-compat: ioctl. PeOS has no ioctl (native calls instead): it
 * always fails with ENOTTY. */
#pragma once
#include <sys/ioccom.h>

#ifdef __cplusplus
extern "C" {
#endif
int ioctl(int fd, unsigned long request, ...);
#ifdef __cplusplus
}
#endif

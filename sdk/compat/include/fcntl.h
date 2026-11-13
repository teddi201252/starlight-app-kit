/* libpeos-compat: fcntl.h for ports, over the base's flags. */
#pragma once
#ifdef __cplusplus
extern "C" {
#endif
#include <peos-base/fcntl.h>
#ifdef __cplusplus
}
#endif
#include <sys/types.h>

#define O_ACCMODE 0x0003
#define O_EXCL 0x0080
#define O_NOCTTY 0x0100
#define O_NONBLOCK 0x0800
#define O_DIRECTORY 0x10000
#define O_NOFOLLOW 0x20000
#define O_CLOEXEC 0x80000
#define F_DUPFD 0
#define F_DUPFD_CLOEXEC 1030
#define F_GETFD 1
#define F_SETFD 2
#define F_GETFL 3
#define F_SETFL 4
#define FD_CLOEXEC 1
#define AT_FDCWD (-100)
#define AT_SYMLINK_NOFOLLOW 0x100
#define AT_REMOVEDIR 0x200

#ifdef __cplusplus
extern "C" {
#endif
int fcntl(int fd, int cmd, ...);
int creat(const char *path, mode_t mode);
/* Relative to AT_FDCWD or an absolute path only (no directory descriptors). */
int openat(int dirfd, const char *path, int flags, ...);
#ifdef __cplusplus
}
#endif

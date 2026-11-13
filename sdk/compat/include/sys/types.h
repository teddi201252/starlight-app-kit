/* libpeos-compat: sys/types.h for ports, over the base's. */
#pragma once
#ifdef __cplusplus
extern "C" {
#endif
#include <peos-base/sys/types.h>
#ifdef __cplusplus
}
#endif
typedef unsigned int mode_t;
typedef unsigned int uid_t;
typedef unsigned int gid_t;
typedef unsigned long dev_t;
typedef unsigned long ino_t;
typedef unsigned long nlink_t;
typedef long blksize_t;
typedef long blkcnt_t;
typedef unsigned long fsblkcnt_t;
typedef unsigned long fsfilcnt_t;
typedef long suseconds_t;
typedef unsigned int useconds_t;
typedef int id_t;
/* fd_set and select, as glibc gives them with sys/types.h (ENet counts on it) */
#include <sys/select.h>

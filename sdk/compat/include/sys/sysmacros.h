/* libpeos-compat: device numbers (glibc's layout). PeOS has no device
 * numbers: what ports compute with them is only compared. */
#pragma once
#include <sys/types.h>

#define major(dev) ((unsigned)((((dev) >> 32) & 0xfffff000u) | (((dev) >> 8) & 0xfffu)))
#define minor(dev) ((unsigned)((((dev) >> 12) & 0xffffff00u) | ((dev) & 0xffu)))
#define makedev(ma, mi)                                                                                          \
    ((dev_t)((((unsigned long long)(ma) & 0xfffff000ull) << 32) | (((unsigned long long)(ma) & 0xfffull) << 8) | \
             (((unsigned long long)(mi) & 0xffffff00ull) << 12) | ((unsigned long long)(mi) & 0xffull)))

/* libpeos-compat: sys/param.h for ports (OpenSSL includes it). */
#pragma once
#include <limits.h>
#include <endian.h>

#define MAXPATHLEN PATH_MAX
#define MAXHOSTNAMELEN 256
#define NBBY 8
#ifndef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif
#ifndef MAX
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#endif
#define howmany(x, y) (((x) + ((y) - 1)) / (y))
#define roundup(x, y) ((((x) + ((y) - 1)) / (y)) * (y))
#define powerof2(x) ((((x) - 1) & (x)) == 0)

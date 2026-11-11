/* libpeos-compat: limits.h, the base's and the POSIX ones. */
#pragma once
#include <peos-base/limits.h>
#define PATH_MAX 4096
#define NAME_MAX 255
#define SSIZE_MAX LONG_MAX
#define MB_LEN_MAX 16
#define PTHREAD_KEYS_MAX 128
#define PTHREAD_STACK_MIN 16384
#define HOST_NAME_MAX 64
#define IOV_MAX 1024
#define PIPE_BUF 4096
/* the most positional arguments printf takes (%1$d ... %9$d), as musl */
#define NL_ARGMAX 9

/* libpeos-compat: flock. One program per file is the rule on PeOS for now:
 * a lock always succeeds. */
#pragma once

#define LOCK_SH 1
#define LOCK_EX 2
#define LOCK_NB 4
#define LOCK_UN 8

#ifdef __cplusplus
extern "C" {
#endif
int flock(int fd, int op);
#ifdef __cplusplus
}
#endif

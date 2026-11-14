/* libpeos-compat: waitpid over peos_waitpid, for a child started with
 * posix_spawn (there is no fork). One child at a time by its pid: -1 (any
 * child) is ECHILD. A child always "exits": PeOS has no signals. */
#pragma once
#include <sys/types.h>

#define WNOHANG 1
#define WUNTRACED 2

#define WEXITSTATUS(s) (((s) >> 8) & 0xff)
#define WTERMSIG(s) ((s) & 0x7f)
#define WIFEXITED(s) (WTERMSIG(s) == 0)
#define WIFSIGNALED(s) (WTERMSIG(s) != 0 && WTERMSIG(s) != 0x7f)
#define WIFSTOPPED(s) (((s) & 0xff) == 0x7f)

#ifdef __cplusplus
extern "C" {
#endif
pid_t waitpid(pid_t pid, int *status, int options);
pid_t wait(int *status);
#ifdef __cplusplus
}
#endif

/* libpeos-compat: posix_spawn over peos_spawnv, PeOS's own way to start a
 * program (there is no fork). The arguments go as one string, separated by
 * spaces (PeOS's crt0 splits them again: an argument with a space in it
 * becomes two). posix_spawnp looks in /bin for a name without '/'. File
 * actions and attributes are not supported: ENOTSUP when given. */
#pragma once
#include <sys/types.h>

typedef struct { int unused; } posix_spawn_file_actions_t;
typedef struct { int unused; } posix_spawnattr_t;

#ifdef __cplusplus
extern "C" {
#endif
int posix_spawn(pid_t *pid, const char *path, const posix_spawn_file_actions_t *actions,
                const posix_spawnattr_t *attr, char *const argv[], char *const envp[]);
int posix_spawnp(pid_t *pid, const char *file, const posix_spawn_file_actions_t *actions,
                 const posix_spawnattr_t *attr, char *const argv[], char *const envp[]);
int posix_spawn_file_actions_init(posix_spawn_file_actions_t *actions);
int posix_spawn_file_actions_destroy(posix_spawn_file_actions_t *actions);
int posix_spawnattr_init(posix_spawnattr_t *attr);
int posix_spawnattr_destroy(posix_spawnattr_t *attr);
#ifdef __cplusplus
}
#endif

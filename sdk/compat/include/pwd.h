/* libpeos-compat: one user. PeOS has no accounts: getpwuid answers the
 * owner of the machine, with /starlight/config as the home. */
#pragma once
#include <sys/types.h>
struct passwd {
    char *pw_name;
    char *pw_passwd;
    uid_t pw_uid;
    gid_t pw_gid;
    char *pw_gecos;
    char *pw_dir;
    char *pw_shell;
};
#ifdef __cplusplus
extern "C" {
#endif
struct passwd *getpwuid(uid_t uid);
struct passwd *getpwnam(const char *name);
#ifdef __cplusplus
}
#endif

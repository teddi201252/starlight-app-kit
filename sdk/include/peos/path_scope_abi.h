#pragma once

#include <stdint.h>

/* Path scope: which parts of the filesystem a task may touch.
 *
 * A task starts unrestricted (restricted == 0); the capability masks
 * (FS_READ / FS_WRITE) still apply. A scope only ever narrows: once restricted
 * a task can shrink it further but never widen it, and children inherit it.
 * A grant covers its prefix and everything below it (component boundaries:
 * "/home" covers "/home/a" but not "/homework"). */

#define PEOS_PATH_READ   1u   /* open for reading, list, stat, chdir, execute */
#define PEOS_PATH_WRITE  2u   /* create, write, truncate, mkdir, unlink, rmdir, rename */
#define PEOS_PATH_RIGHTS_ALL (PEOS_PATH_READ | PEOS_PATH_WRITE)

#define PEOS_PATH_MAX_GRANTS 8u
#define PEOS_PATH_MAX_PREFIX 96u

typedef struct {
    uint32_t rights;
    char prefix[PEOS_PATH_MAX_PREFIX];   /* absolute, normalized, NUL terminated */
} peos_path_grant_t;

typedef struct {
    uint32_t restricted;
    uint32_t count;
    peos_path_grant_t grants[PEOS_PATH_MAX_GRANTS];
} peos_path_scope_t;

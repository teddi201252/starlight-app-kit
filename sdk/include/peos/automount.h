/* What diskd mounts and where (release 0.16.0, phase 2), as a pure step over
 * the list of disks (peos_disks), so that it is tested on the host
 * (tests/host/test_automount.c).
 *
 * - A removable disk's partitions with a filesystem PeOS reads (a whole
 *   disk only when it has no partitions) go on /media1, /media2, ...: the
 *   first number free at that moment.
 * - Never the EFI partition, never the disk of the root.
 * - One that was mounted here and is unmounted while still in (ejected:
 *   Curiosity, umount) stays out until it is taken out and put in again.
 * - A /mediaN folder made here goes when nothing is mounted on it. */
#pragma once
#ifdef __cplusplus
extern "C" {
#endif
#include <stddef.h>
#include <stdint.h>
#include "peos/disks.h"

enum { AUTOMOUNT_NONE = 0, AUTOMOUNT_MOUNTED = 1, AUTOMOUNT_EJECTED = 2 };

typedef struct {
    char name[24];
    uint8_t state;          // AUTOMOUNT_*
} automount_entry_t;

typedef struct {
    automount_entry_t e[PEOS_DISKS_MAX];
    uint32_t made;          // bit N-1: /mediaN made by diskd and still there
} automount_t;

enum { AUTOMOUNT_DO_MOUNT = 1, AUTOMOUNT_DO_RMDIR = 2 };

typedef struct {
    uint8_t kind;           // AUTOMOUNT_DO_*
    char name[24];          // the device (mount)
    char path[16];          // "/mediaN"
} automount_action_t;

/* Whether d is one diskd mounts by itself (removable, a filesystem, ...). */
int automount_wanted(const peos_disk_info_t *d, const peos_disk_info_t *all, size_t n);
/* The first N from 1 with "/mediaN" mounted by nobody in all, and not in
 * busy (bit N-1); 0 if none up to 32. */
uint32_t automount_free_number(const peos_disk_info_t *all, size_t n, uint32_t busy);
/* One look at the disks: the actions to take now (at most max). Call
 * automount_done after each mount, with whether it worked. */
size_t automount_step(automount_t *st, const peos_disk_info_t *all, size_t n, automount_action_t *out, size_t max);
void automount_done(automount_t *st, const automount_action_t *a, int ok);

#ifdef __cplusplus
}
#endif

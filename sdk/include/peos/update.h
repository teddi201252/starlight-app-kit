/* PeOS's own update (release 0.14.0, phase 5): what the newest version is
 * and what its package holds, read and checked. tools/mkupdate.py makes
 * them; docs/release-0.14.0-plan.md says how they are used.
 *
 *   https://peoverse.com/files/latest.json (and latest.json.sig):
 *     {"format": 1, "version": "0.14.0", "date": "2026-11-10",
 *      "min_peos": "0.13.1", "notes": "https://...",
 *      "package": "https://.../peos-0.14.0-update.tar.gz",
 *      "size": 12687277, "sha256": "<hex>"}
 *
 *   the package's manifest (and manifest.sig), a line each:
 *     peos-update 1 / version 0.14.0 / min_peos 0.13.1 / date 2026-11-10
 *     keep starlight/config                 the user's: never written nor removed
 *     root <sha256> <size> bin/cat          a file of the system on the root
 *     esp <sha256> <size> kernel.elf        one on the EFI partition
 *
 * Both are signed with the update key (ECDSA P-256 over SHA-256, as
 * PeStore's catalog; checked with store_verify against
 * /system/config/update.pub). Pure parsing, checked by tests/host/test_update.c. */
#ifndef PEOS_UPDATE_H
#define PEOS_UPDATE_H

#include <stddef.h>
#include <stdint.h>

#define UPDATE_FORMAT 1
#define UPDATE_PUB_PATH "/system/config/update.pub"
#define UPDATE_LATEST_URL "https://peoverse.com/files/latest.json"
#define UPDATE_MANIFEST_PATH "/system/update/manifest"   /* the installed version's */

typedef struct {
    uint32_t format;
    char version[24];
    char date[16];
    char min_peos[24];
    char notes[256];
    char package[256];
    uint64_t size;
    uint8_t sha256[32];
} update_latest_t;

enum { UPDATE_ROOT = 1, UPDATE_ESP = 2 };

typedef struct {
    uint8_t where;              /* UPDATE_ROOT or UPDATE_ESP */
    uint8_t sha256[32];
    uint64_t size;
    char *path;                 /* "bin/cat", relative, inside the manifest's text */
} update_file_t;

typedef struct {
    uint32_t format;
    char version[24];
    char min_peos[24];
    char date[16];
    char *text;                 /* malloc'd copy the paths point into */
    update_file_t *files;       /* malloc'd */
    uint32_t count;
    char **keep;                /* malloc'd, into text */
    uint32_t nkeep;
} update_manifest_t;

/* 0, or -1 with why in err (English). */
int update_latest_parse(const char *json, size_t len, update_latest_t *out, char *err, size_t errcap);
int update_manifest_parse(const char *text, size_t len, update_manifest_t *out, char *err, size_t errcap);
void update_manifest_free(update_manifest_t *m);

/* The user's (under a keep line): an update never writes nor removes it. */
int update_is_kept(const update_manifest_t *m, const char *path);
const update_file_t *update_manifest_find(const update_manifest_t *m, uint8_t where, const char *path);

/* Files of the system the old version had and the new one has not: they go
 * when updating. Calls fn for each root path of old not in new (and not kept
 * by new); the number of them. */
uint32_t update_removed(const update_manifest_t *old, const update_manifest_t *new_m,
                        void (*fn)(const char *path, void *user), void *user);

/* Can `installed` take this update: newer, and not older than min_peos. */
int update_applies(const char *installed, const char *version, const char *min_peos);

/* ---- Applying it (update_apply.c) ----
 *
 * The package, checked and unpacked in <work>/new (manifest, root/, esp/),
 * goes in place at boot, from init, before anything of the system runs.
 * Every step is a rename or a copy whose being done shows on the disk, so
 * a power cut anywhere is taken up again at the next boot:
 *   a root file:  <root>/p -> <work>/rollback/root/p, then <work>/new/root/p -> <root>/p
 *   gone files:   <root>/p -> <work>/rollback/root/p
 *   the kernel:   copied to <esp>/kernel.new, kernel.elf -> kernel-prev.elf,
 *                 kernel.new -> kernel.elf (its SHA-256 says how far it got)
 *   other EFI files (Limine, fw/): replaced only if they changed; not undone
 *   limine.conf:  an entry "previous" once (kernel-prev.elf, starting
 *                 /bin/peupdate rollback), and a menu while on trial
 *                 (timeout 0 <-> 5, in place)
 * <work>/state says where it is: ready, applying, trial <boots>, done,
 * rollback. On trial, a boot more each time; once the desktop has run
 * (init: update_confirm) it is done, kept for good; after UPDATE_TRIAL_BOOTS
 * boots without, or from the "previous" entry, everything goes back.
 *
 * The files go through update_fs_t, so that the host test can cut the power
 * at every step (tests/host/test_update_apply.c). */

#define UPDATE_TRIAL_BOOTS 3
#define UPDATE_WORK "/.update"
#define UPDATE_ESP_MOUNT "/esp"

typedef struct {
    void *user;
    int (*exists)(void *u, const char *path);                         /* 1 yes, 0 no */
    int (*rename)(void *u, const char *from, const char *to);         /* 0; never over another */
    int (*mkdir_p)(void *u, const char *path);                        /* 0 */
    int (*remove)(void *u, const char *path);                         /* a file: 0, or 0 if not there */
    int (*rmtree)(void *u, const char *path);                         /* a folder and all in it: 0 */
    int (*copy)(void *u, const char *from, const char *to);           /* to a new file: 0 */
    int (*sha256)(void *u, const char *path, uint8_t out[32]);        /* 0 */
    int (*read)(void *u, const char *path, char *buf, size_t cap, size_t *len);   /* 0 */
    int (*write)(void *u, const char *path, const void *data, size_t len);        /* a new file: 0 */
    int (*patch)(void *u, const char *path, size_t off, const void *data, size_t len); /* in place: 0 */
} update_fs_t;

typedef struct {
    const update_fs_t *fs;
    const char *root;           /* "" for / */
    const char *esp;            /* where the EFI partition is mounted: /esp */
    const char *work;           /* /.update */
    char error[160];            /* why it stopped (English, for the log) */
} update_ctx_t;

enum {
    UPDATE_ST_NONE = 0,         /* nothing going on */
    UPDATE_ST_READY,            /* unpacked and checked: apply at the next boot */
    UPDATE_ST_APPLYING,
    UPDATE_ST_TRIAL,            /* in place; waiting for the desktop */
    UPDATE_ST_DONE,             /* kept: cleaning up */
    UPDATE_ST_ROLLBACK,         /* going back */
};

int update_state_read(update_ctx_t *c, int *boots);   /* UPDATE_ST_*, -1 unreadable */
int update_state_write(update_ctx_t *c, int state, int boots);

/* What update_boot did. */
enum { UPDATE_BOOT_NOTHING = 0, UPDATE_BOOT_APPLIED, UPDATE_BOOT_TRIAL, UPDATE_BOOT_ROLLED_BACK,
       UPDATE_BOOT_FINISHED, UPDATE_BOOT_FAILED };

/* At boot, from init: carries on with whatever the state says. APPLIED and
 * ROLLED_BACK want a restart (the other kernel). old may be NULL (no
 * manifest installed: 0.14.0's first update). */
int update_boot(update_ctx_t *c, const update_manifest_t *new_m, const update_manifest_t *old_m);
int update_apply(update_ctx_t *c, const update_manifest_t *new_m, const update_manifest_t *old_m);
/* The desktop ran: the update is kept, the copies of safety go. 0. */
int update_confirm(update_ctx_t *c);
int update_rollback(update_ctx_t *c, const update_manifest_t *new_m, const update_manifest_t *old_m);

/* limine.conf with the entry "previous" added (if it is not there): the
 * first entry again, kernel-prev.elf and run=/bin/peupdate runarg=rollback.
 * The new text in out (NUL-ended); its length, 0 if nothing to add. */
size_t update_limine_with_previous(const char *conf, size_t len, char *out, size_t cap);

#endif

/* The bin of a disk (docs/system-apps-ux-plan.md, phase 4: Curiosity and
 * Orion). What is deleted on a disk stays on that disk, in its root:
 *
 *   <root>/.trash/files/<name>       what was deleted, moved there (a rename:
 *                                    the same disk, no copy)
 *   <root>/.trash/info/<name>.info   "path=<where it was>\ntime=<unix s>\n"
 *
 * <root> is the disk the path is on: /mediaN for a disk mounted there, "/"
 * for the system's own. The info is written before the move and taken away
 * after the move back: a power cut in between leaves an info with nothing
 * beside it, which the list does not show.
 *
 *   char name[PEOS_TRASH_NAME];
 *   if (peos_trash_put("/media1/a.txt", name, sizeof(name)) == 0) ... undo:
 *       peos_trash_restore("/media1", name, "copy", where, sizeof(where));
 *   else ... no bin there (a disk that cannot be written): ask, then delete
 */
#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

#define PEOS_TRASH_PATH 256u
#define PEOS_TRASH_NAME 128u
#define PEOS_TRASH_DIR ".trash"

typedef struct {
    char name[PEOS_TRASH_NAME];    /* its name inside the bin */
    char from[PEOS_TRASH_PATH];    /* where it was */
    uint64_t time_s;               /* when it was deleted (unix seconds) */
    uint64_t size;                 /* a file's bytes (a folder: 0) */
    uint8_t is_dir;
} peos_trash_item_t;

/* The root of the disk path is on: "/mediaN", or "/". */
void peos_trash_root(const char *path, char *root, size_t cap);
/* Moves path into its disk's bin. 0, with its name in the bin in name; -1
 * when it cannot (not there, the root itself or the bin, a disk that cannot
 * be written): nothing moved. */
int peos_trash_put(const char *path, char *name, size_t cap);
/* Puts <name> back where it was (its folder made again if it is gone; a
 * name taken there: "stem (word).ext"). 0 with the path in where, or -1. */
int peos_trash_restore(const char *root, const char *name, const char *word, char *where, size_t cap);
/* What the bin of root holds, the last deleted first: how many (at most cap
 * written in out). */
uint32_t peos_trash_list(const char *root, peos_trash_item_t *out, uint32_t cap);
uint32_t peos_trash_count(const char *root);
/* Gone for good: one of the bin, or all of it. 0, or -1. */
int peos_trash_remove(const char *root, const char *name);
int peos_trash_empty(const char *root);
#ifdef __cplusplus
}
#endif

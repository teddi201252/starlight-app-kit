/* Copying and moving files and folders, a piece at a time (release 0.13.1,
 * phase 3: Curiosity's Copy, Cut and Paste).
 *
 *   peos_fileop_t *op = peos_fileop_begin(PEOS_FILEOP_COPY, "/music/a.flac", "/downloads", "copia", &err);
 *   while (op && peos_fileop_step(op, 8000000ull) == PEOS_FILEOP_RUNNING) { ...draw the progress... }
 *   ... peos_fileop_error(op), peos_fileop_result(op) (where it went) ...
 *   peos_fileop_end(op);
 *
 * What is written goes under a provisional name (".peos-part-<name>") and
 * takes its own only at the end: a copy cut short leaves nothing half done
 * (peos_fileop_end on a running one takes the part away). A name already in
 * the folder is never overwritten: the new one is "<name> (<word>)<.ext>",
 * then "(<word> 2)"... Moving on the same disk is a rename; across disks a
 * copy, and the original goes only once the copy is whole. */
#pragma once

#include <stddef.h>
#include <stdint.h>

typedef enum { PEOS_FILEOP_COPY = 0, PEOS_FILEOP_MOVE = 1 } peos_fileop_kind_t;

enum {
    PEOS_FILEOP_FAILED = -1,
    PEOS_FILEOP_DONE = 0,
    PEOS_FILEOP_RUNNING = 1,
};

typedef enum {
    PEOS_FILEOP_OK = 0,
    PEOS_FILEOP_ERR_SOURCE,       /* it is not there, or cannot be read */
    PEOS_FILEOP_ERR_INTO_ITSELF,  /* a folder into itself or one of its own */
    PEOS_FILEOP_ERR_WRITE,        /* the folder of arrival: full, read only, not allowed */
    PEOS_FILEOP_ERR_TOO_LONG,     /* a path longer than PEOS_FILEOP_PATH - 1 */
    PEOS_FILEOP_ERR_TOO_DEEP,     /* folders inside folders more than PEOS_FILEOP_DEPTH */
    PEOS_FILEOP_ERR_TOO_MANY,     /* a folder with more names than one listing holds */
    PEOS_FILEOP_ERR_NO_MEMORY,
    PEOS_FILEOP_ERR_REMOVE,       /* moved by copying, but the original stayed */
} peos_fileop_error_t;

#define PEOS_FILEOP_PATH 256u
#define PEOS_FILEOP_DEPTH 24u

typedef struct peos_fileop peos_fileop_t;

/* Starts copying or moving src into the folder dst_dir. word: what a copy
 * with a name taken is called ("copia", "copy"). NULL with *err when it
 * cannot even start. Moving into the folder it is in already: done at once,
 * nothing changes. */
peos_fileop_t *peos_fileop_begin(peos_fileop_kind_t kind, const char *src, const char *dst_dir, const char *word,
                                 peos_fileop_error_t *err);
/* Works for about budget_ns: RUNNING, DONE or FAILED. */
int peos_fileop_step(peos_fileop_t *op, uint64_t budget_ns);
/* The bytes done and to do (total 0 while a folder is still being measured). */
void peos_fileop_progress(const peos_fileop_t *op, uint64_t *done, uint64_t *total);
peos_fileop_error_t peos_fileop_error(const peos_fileop_t *op);
/* Where it went (its final path): the name may differ from the source's. */
const char *peos_fileop_result(const peos_fileop_t *op);
/* Frees it; one still running stops, and what it wrote goes away. */
void peos_fileop_end(peos_fileop_t *op);

/* A name not yet in dir for name: name itself, else "stem (word).ext",
 * "stem (word 2).ext"... (a folder: no extension). 0, or -1 if too long. */
int peos_fileop_free_name(const char *dir, const char *name, int is_dir, const char *word, char *out, size_t cap);

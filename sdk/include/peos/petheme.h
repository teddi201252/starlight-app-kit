#pragma once
#ifdef __cplusplus
extern "C" {
#endif
/* A theme to export and import (release 0.16.0, phase 8): a .petheme file
 * is an uncompressed tar (ustar) with
 *   theme.ini   name=, author=, version=, min_peos=, mode=day|night,
 *               color.<key>=r,g,b,a (the keys of theme.cfg), font=<a name,
 *               not a path>, animations=0|1, animation_speed=10..100,
 *               cursor_theme=soft|image, background=<file>, cursor=<file>
 *   <the files theme.ini names> (the background, the cursor image)
 * A .petheme comes from outside: only theme.ini and the files it names are
 * read; names are plain (no folders, no ".."), theme.ini at most 16 KiB, a
 * file at most 16 MiB, values checked. Plain C, no system calls: the host
 * test builds it (tests/host/test_petheme.c). */

#include <stddef.h>
#include <stdint.h>

#define PETHEME_COLORS_MAX 32u
#define PETHEME_INI_MAX (16u * 1024u)
#define PETHEME_FILE_MAX (16u * 1024u * 1024u)
#define PETHEME_NAME_MAX 48u
#define PETHEME_FILE_NAME_MAX 64u

typedef struct {
    char key[24];
    uint8_t rgba[4];
} petheme_color_t;

typedef struct {
    char name[PETHEME_NAME_MAX];
    char author[PETHEME_NAME_MAX];
    char version[16];
    char min_peos[16];
    int mode;                  /* 0 day, 1 night, -1 not said */
    uint32_t ncolors;
    petheme_color_t colors[PETHEME_COLORS_MAX];
    char font[PETHEME_FILE_NAME_MAX];   /* a font's file name, or "" */
    int animations;            /* 0, 1, or -1 */
    int animation_speed;       /* 10..100, or -1 */
    int cursor_image;          /* 0 soft, 1 image, -1 */
    char background_name[PETHEME_FILE_NAME_MAX];
    const uint8_t *background; /* points into the tar (read) or the caller's bytes (write) */
    size_t background_len;
    char cursor_name[PETHEME_FILE_NAME_MAX];
    const uint8_t *cursor;
    size_t cursor_len;
} petheme_t;

void petheme_init(petheme_t *t);
/* Adds or replaces a colour; 0, or -1 when full or the key is bad. */
int petheme_set_color(petheme_t *t, const char *key, const uint8_t rgba[4]);
const petheme_color_t *petheme_color(const petheme_t *t, const char *key);

/* The archive's size (written only if cap is enough: call with cap 0 to
 * know it). */
size_t petheme_write(const petheme_t *t, uint8_t *out, size_t cap);

enum {
    PETHEME_OK = 0,
    PETHEME_ERR_FORMAT = -1,   /* not a tar, a bad header */
    PETHEME_ERR_NO_INI = -2,   /* no theme.ini */
    PETHEME_ERR_UNSAFE = -3,   /* a name with a folder or "..", too big */
    PETHEME_ERR_VALUE = -4,    /* a value out of range */
    PETHEME_ERR_MISSING = -5,  /* theme.ini names a file that is not there */
};
/* Reads a .petheme; *t points into tar for the files. */
int petheme_read(petheme_t *t, const uint8_t *tar, size_t len);

/* A name fit for a file: letters, digits, space, - _ . kept, others "_". */
void petheme_file_name(const char *theme_name, char *out, size_t cap);
#ifdef __cplusplus
}
#endif

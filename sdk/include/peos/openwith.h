/* Which app opens a file (system apps UX plan, section 4, big job 8: one
 * table extension -> app, for Curiosity's "Apri con…" and for Orbit).
 *
 * PeOS's own table first gives the apps that open an extension (Orion the
 * pictures, Orfeus the music, Voyager the texts, Settings the themes); the
 * user's choice ("Usa sempre … per i file .md") is kept in
 * /system/config/openwith.cfg, one "ext=bundle" a line, and comes before.
 *
 *   char app[PEOS_OPENWITH_BUNDLE];
 *   if (peos_openwith_default("idee.md", app, sizeof(app)) == 0) slui_open_file_with(app, path);
 */
#ifndef PEOS_OPENWITH_H
#define PEOS_OPENWITH_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define PEOS_OPENWITH_CFG "/system/config/openwith.cfg"
#define PEOS_OPENWITH_BUNDLE 128u
#define PEOS_OPENWITH_TITLE 64u
#define PEOS_OPENWITH_EXT 16u

typedef struct {
    char bundle[PEOS_OPENWITH_BUNDLE];   /* "/system/apps/voyager.app" */
    char title[PEOS_OPENWITH_TITLE];     /* its name in the language asked */
    uint8_t fits;                        /* it says it opens this kind of file */
    uint8_t is_default;                  /* the one a double click uses */
} peos_openwith_app_t;

/* The extension of name, lower case, without the dot ("md"); "" for none
 * (a name with no dot, or only a leading one). */
void peos_openwith_ext(const char *name, char *ext, size_t cap);
/* The app a double click opens name with: 0 with its bundle in out, -1 none. */
int peos_openwith_default(const char *name, char *out, size_t cap);
/* Opens the files .ext with bundle from now on ("" or NULL: back to PeOS's
 * own table). 0, or -1 when it cannot be written. */
int peos_openwith_set(const char *ext, const char *bundle);
/* Every app of /system/apps and /apps (their names in language, "it"),
 * those that open name first, the default first of all: how many (at most
 * cap written). */
uint32_t peos_openwith_list(const char *name, const char *language, peos_openwith_app_t *out, uint32_t cap);

#ifdef __cplusplus
}
#endif

#endif

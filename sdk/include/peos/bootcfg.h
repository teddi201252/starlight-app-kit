/* The installed system's start settings in its limine.conf (docs/system-apps-
 * ux-plan.md, phase 4, Settings: the resolution on every machine). The
 * installer leaves one line of PEOS_BOOTCFG_LINE_LEN characters in the first
 * entry (tools/mkesp_image.sh); it is rewritten in place, at the same length,
 * so a power cut never leaves the file half written:
 *
 *   "#   resolution:                 "   the firmware's choice (Limine's)
 *   "    resolution: 1920x1080       "   that mode at the next start
 *
 * The live system (an ISO) has no such line: nothing is written there. */
#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

#define PEOS_BOOTCFG_LINE_LEN 32u

/* The line's place in conf (-1: none) and the mode it sets (0x0: none). */
long peos_bootcfg_find(const char *conf, size_t len, uint32_t *w, uint32_t *h);
/* The line for a mode (0x0: none), PEOS_BOOTCFG_LINE_LEN characters and a '\0'. */
void peos_bootcfg_format(char out[PEOS_BOOTCFG_LINE_LEN + 1], uint32_t w, uint32_t h);

/* The installed system's: 1 with the mode set for the next start (0x0: the
 * firmware's), 0 when there is no line to change (a live system). The EFI
 * partition is mounted at /esp when it is not (the "mount" capability). */
int peos_boot_resolution_get(uint32_t *w, uint32_t *h);
/* 0, or -1 when it cannot be written (no installed system, no line). */
int peos_boot_resolution_set(uint32_t w, uint32_t h);
#ifdef __cplusplus
}
#endif

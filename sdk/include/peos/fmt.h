/*
 * Sizes and dates written the same way in every app
 * (docs/system-apps-ux-plan.md 1.6): Curiosity, Orion, Monitor and the rest.
 *
 *   peos_fmt_size      "512 B", "1.4 KB", "145 KB", "2.3 GB" (one decimal
 *                      below 10 of a unit), the decimal mark of the language
 *   peos_fmt_decimal   ',' for the languages that write it so ("it", ...),
 *                      '.' for the others
 *   peos_fmt_datetime  "2026-11-18 10:42", in local time (the zone chosen in
 *                      Settings: localtime_r)
 */
#ifndef PEOS_FMT_H
#define PEOS_FMT_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

char peos_fmt_decimal(const char *language);
void peos_fmt_size(char *out, size_t cap, uint64_t bytes, char decimal);
/* utc_seconds 0: "-". */
void peos_fmt_datetime(char *out, size_t cap, int64_t utc_seconds);

#ifdef __cplusplus
}
#endif

#endif

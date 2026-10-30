#pragma once
/* Character sets (release 0.11.0): text in the encodings a browser meets
 * most, to UTF-8 and back. PeOS's own text is UTF-8; this is for what comes
 * from outside (a page that says charset=windows-1252) and what goes back
 * (a form sent in the page's encoding). */
#include <stddef.h>
#include <stdint.h>

typedef enum {
    PEOS_CS_UNKNOWN = 0,
    PEOS_CS_UTF8,
    PEOS_CS_ASCII,
    PEOS_CS_LATIN1,     /* ISO-8859-1 */
    PEOS_CS_LATIN9,     /* ISO-8859-15 */
    PEOS_CS_CP1252,     /* Windows-1252 (what the web means by "latin1" too) */
    PEOS_CS_UTF16LE,
    PEOS_CS_UTF16BE,
} peos_charset_t;

/* A charset by its name, any case ("utf-8", "ISO-8859-1", "latin1",
 * "windows-1252"...), or PEOS_CS_UNKNOWN. */
peos_charset_t peos_charset_from_name(const char *name, size_t len);

/* in (len bytes, in cs) to UTF-8, in *out (malloc'd, NUL-ended; free it).
 * A byte that is not valid becomes U+FFFD. -> the length, or -1 (memory). */
int64_t peos_charset_to_utf8(peos_charset_t cs, const uint8_t *in, size_t len, char **out);

/* UTF-8 to cs, in *out (malloc'd): what cs cannot hold becomes "&#N;", as a
 * browser sends a form. -> the length, or -1. */
int64_t peos_charset_from_utf8(peos_charset_t cs, const char *in, size_t len, uint8_t **out);

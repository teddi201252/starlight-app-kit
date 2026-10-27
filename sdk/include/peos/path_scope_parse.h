#pragma once

#include <stddef.h>
#include <stdint.h>

#include "peos/path_scope_abi.h"

/* Parsing of the manifest key
 *   paths=/home:rw, /system:r
 * Each item is an absolute path, a colon and r, w or rw. Freestanding so
 * hosts and tests can include it. */

/* Parses `list` into grants[max]; returns the number of grants or -1 on any
 * malformed item (fail closed). An empty list is valid and yields 0 grants. */
static inline int peos_path_parse_list(const char *list, peos_path_grant_t *grants, size_t max) {
    if (!list || !grants) return -1;
    size_t n = 0;
    const char *p = list;
    while (*p != '\0') {
        while (*p == ',' || *p == ' ' || *p == '\t') p++;
        if (*p == '\0') break;
        const char *start = p;
        while (*p != '\0' && *p != ',' && *p != ' ' && *p != '\t') p++;
        size_t len = (size_t)(p - start);

        const char *colon = NULL;
        for (size_t i = len; i > 0; i--) {
            if (start[i - 1u] == ':') { colon = start + i - 1u; break; }
        }
        if (!colon || colon == start || start[0] != '/') return -1;
        size_t plen = (size_t)(colon - start);
        if (plen >= PEOS_PATH_MAX_PREFIX || n >= max) return -1;

        uint32_t rights = 0;
        for (const char *r = colon + 1; r < start + len; r++) {
            if (*r == 'r') rights |= PEOS_PATH_READ;
            else if (*r == 'w') rights |= PEOS_PATH_WRITE;
            else return -1;
        }
        if (rights == 0u) return -1;

        for (size_t i = 0; i < plen; i++) grants[n].prefix[i] = start[i];
        grants[n].prefix[plen] = '\0';
        grants[n].rights = rights;
        n++;
    }
    return (int)n;
}

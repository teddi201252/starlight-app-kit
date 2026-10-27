#pragma once

#include <stddef.h>
#include <stdint.h>

#include "peos/capability.h"

/* Parsing of capability lists as written in app manifests, e.g.
 *   capabilities=fs.read, fs.write, audio
 * Freestanding on purpose so hosts and tests can include it. */

static const struct {
    const char *name;
    peos_cap_mask_t mask;
} peos_cap_names[] = {
    { "fs.read", PEOS_CAP_FS_READ },
    { "fs.write", PEOS_CAP_FS_WRITE },
    { "process", PEOS_CAP_PROCESS },
    { "display", PEOS_CAP_DISPLAY },
    { "input", PEOS_CAP_INPUT },
    { "audio", PEOS_CAP_AUDIO },
    { "network", PEOS_CAP_NETWORK },
    { "mount", PEOS_CAP_MOUNT },
    { "power", PEOS_CAP_POWER },
    { "debug", PEOS_CAP_DEBUG },
    { "shm", PEOS_CAP_SHM },
    { "install", PEOS_CAP_INSTALL },
    { "manage", PEOS_CAP_MANAGE },
    { "nic", PEOS_CAP_NIC },
    { "service", PEOS_CAP_SERVICE },
};

/* Returns 1 and sets *out when [name, name+len) is a known capability. */
static inline int peos_cap_from_name(const char *name, size_t len, peos_cap_mask_t *out) {
    for (size_t i = 0; i < sizeof(peos_cap_names) / sizeof(peos_cap_names[0]); i++) {
        const char *n = peos_cap_names[i].name;
        size_t j = 0;
        while (j < len && n[j] != '\0' && n[j] == name[j]) j++;
        if (j == len && n[j] == '\0') {
            *out = peos_cap_names[i].mask;
            return 1;
        }
    }
    return 0;
}

/* Parses a comma/space separated list. An empty list is valid and means "no
 * rights". Any unknown name fails the whole parse (fail closed): returns -1
 * and leaves *out untouched. */
static inline int peos_cap_parse_list(const char *list, peos_cap_mask_t *out) {
    if (!list || !out) return -1;
    peos_cap_mask_t mask = 0;
    const char *p = list;
    while (*p != '\0') {
        while (*p == ',' || *p == ' ' || *p == '\t') p++;
        if (*p == '\0') break;
        const char *start = p;
        while (*p != '\0' && *p != ',' && *p != ' ' && *p != '\t') p++;
        peos_cap_mask_t one = 0;
        if (!peos_cap_from_name(start, (size_t)(p - start), &one)) return -1;
        mask |= one;
    }
    *out = mask;
    return 0;
}

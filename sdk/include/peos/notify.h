#pragma once

// Desktop notifications (Soft UI plan, phase 7): a short message to the
// "notify" service, which Starlight registers. Starlight shows it in a night
// pill at the top right and counts it on the dock tile of the app whose
// title is `source`.

#include <stdint.h>

#define PEOS_NOTIFY_SERVICE "notify"
#define PEOS_NOTIFY_MAGIC 0x3159544Eu   /* "NTY1" */
#define PEOS_NOTIFY_SOURCE_MAX 32u
#define PEOS_NOTIFY_TEXT_MAX 160u

typedef struct {
    uint32_t magic;
    uint32_t reserved;
    char source[PEOS_NOTIFY_SOURCE_MAX];   // who: an app title, "netd", "Sistema"
    char text[PEOS_NOTIFY_TEXT_MAX];
} peos_notify_msg_t;

// Send a notification; never blocks. 0 if delivered to the service, -1 if
// there is none (no desktop running) or it is busy.
int peos_notify(const char *source, const char *text);

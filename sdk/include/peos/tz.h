// Time zones (release 0.17.0, phase 6): a short list of the IANA names, each
// with its offset from UTC and its summer time rule (European, North
// American, or none). Freestanding: Starlight, the apps and the host tests
// include it. The setting is "timezone=<name>" in Starlight's settings.cfg;
// an unknown name is UTC.
#ifndef PEOS_TZ_H
#define PEOS_TZ_H

#include <stdint.h>

enum { PEOS_TZ_RULE_NONE = 0, PEOS_TZ_RULE_EU = 1, PEOS_TZ_RULE_US = 2 };

typedef struct {
    const char *name;
    int16_t std_min;   // minutes east of UTC, winter
    uint8_t rule;
} peos_tz_t;

static const peos_tz_t peos_tz_list[] = {
    { "UTC", 0, PEOS_TZ_RULE_NONE },
    { "Europe/London", 0, PEOS_TZ_RULE_EU },
    { "Europe/Lisbon", 0, PEOS_TZ_RULE_EU },
    { "Europe/Rome", 60, PEOS_TZ_RULE_EU },
    { "Europe/Paris", 60, PEOS_TZ_RULE_EU },
    { "Europe/Berlin", 60, PEOS_TZ_RULE_EU },
    { "Europe/Madrid", 60, PEOS_TZ_RULE_EU },
    { "Europe/Zurich", 60, PEOS_TZ_RULE_EU },
    { "Europe/Athens", 120, PEOS_TZ_RULE_EU },
    { "Europe/Helsinki", 120, PEOS_TZ_RULE_EU },
    { "Europe/Moscow", 180, PEOS_TZ_RULE_NONE },
    { "America/New_York", -300, PEOS_TZ_RULE_US },
    { "America/Chicago", -360, PEOS_TZ_RULE_US },
    { "America/Denver", -420, PEOS_TZ_RULE_US },
    { "America/Phoenix", -420, PEOS_TZ_RULE_NONE },
    { "America/Los_Angeles", -480, PEOS_TZ_RULE_US },
    { "America/Sao_Paulo", -180, PEOS_TZ_RULE_NONE },
    { "Asia/Dubai", 240, PEOS_TZ_RULE_NONE },
    { "Asia/Kolkata", 330, PEOS_TZ_RULE_NONE },
    { "Asia/Shanghai", 480, PEOS_TZ_RULE_NONE },
    { "Asia/Tokyo", 540, PEOS_TZ_RULE_NONE },
    { "Australia/Brisbane", 600, PEOS_TZ_RULE_NONE },
};
#define PEOS_TZ_COUNT ((uint32_t)(sizeof(peos_tz_list) / sizeof(peos_tz_list[0])))
#define PEOS_TZ_DEFAULT "Europe/Rome"

// The zone called name (its index in peos_tz_list), or 0 (UTC).
static inline uint32_t peos_tz_find(const char *name) {
    if (!name) return 0;
    for (uint32_t i = 0; i < PEOS_TZ_COUNT; i++) {
        const char *a = peos_tz_list[i].name, *b = name;
        while (*a && *a == *b) a++, b++;
        if (*a == 0 && (*b == 0 || *b == '\n' || *b == '\r' || *b == ' ')) return i;
    }
    return 0;
}

// Days since 1970-01-01 of y-m-d (proleptic Gregorian).
static inline int64_t peos_tz_days(int32_t y, uint32_t m, uint32_t d) {
    y -= m <= 2;
    int64_t era = (y >= 0 ? y : y - 399) / 400;
    uint32_t yoe = (uint32_t)(y - era * 400);
    uint32_t doy = (153u * (m + (m > 2 ? (uint32_t)-3 : 9u)) + 2u) / 5u + d - 1u;
    uint32_t doe = yoe * 365u + yoe / 4u - yoe / 100u + doy;
    return era * 146097 + (int64_t)doe - 719468;
}

// The n-th Sunday (n = 1..4) of a month, or the last one (n = 0), in days.
static inline int64_t peos_tz_sunday(int32_t y, uint32_t m, uint32_t n) {
    if (n == 0) {
        uint32_t ny = m == 12 ? (uint32_t)y + 1u : (uint32_t)y, nm = m == 12 ? 1u : m + 1u;
        int64_t last = peos_tz_days((int32_t)ny, nm, 1) - 1;
        int64_t wd = ((last + 4) % 7 + 7) % 7;   // 0 = Sunday
        return last - wd;
    }
    int64_t first = peos_tz_days(y, m, 1);
    int64_t wd = ((first + 4) % 7 + 7) % 7;
    return first + (7 - wd) % 7 + 7 * (int64_t)(n - 1u);
}

// The year of a unix time.
static inline int32_t peos_tz_year(int64_t t) {
    int64_t z = (t >= 0 ? t / 86400 : (t - 86399) / 86400) + 719468;
    int64_t era = (z >= 0 ? z : z - 146096) / 146097;
    uint32_t doe = (uint32_t)(z - era * 146097);
    uint32_t yoe = (doe - doe / 1460u + doe / 36524u - doe / 146096u) / 365u;
    uint32_t doy = doe - (365u * yoe + yoe / 4u - yoe / 100u);
    uint32_t mp = (5u * doy + 2u) / 153u;
    return (int32_t)((int64_t)yoe + era * 400 + (mp >= 10u ? 1 : 0));
}

// Seconds to add to UTC for the local time of zone i at utc.
static inline int64_t peos_tz_offset_at(uint32_t i, int64_t utc) {
    if (i >= PEOS_TZ_COUNT) return 0;
    const peos_tz_t *z = &peos_tz_list[i];
    int64_t std = (int64_t)z->std_min * 60;
    int32_t y = peos_tz_year(utc + std);
    int64_t start, end;
    if (z->rule == PEOS_TZ_RULE_EU) {   // last Sunday of March, of October, at 01:00 UTC
        start = peos_tz_sunday(y, 3, 0) * 86400 + 3600;
        end = peos_tz_sunday(y, 10, 0) * 86400 + 3600;
    } else if (z->rule == PEOS_TZ_RULE_US) {   // second Sunday of March, first of November, 02:00 local
        start = peos_tz_sunday(y, 3, 2) * 86400 + 7200 - std;
        end = peos_tz_sunday(y, 11, 1) * 86400 + 7200 - (std + 3600);
    } else {
        return std;
    }
    return (utc >= start && utc < end) ? std + 3600 : std;
}

static inline int64_t peos_tz_offset(const char *name, int64_t utc) {
    return peos_tz_offset_at(peos_tz_find(name), utc);
}

#endif

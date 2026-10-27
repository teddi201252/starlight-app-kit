#pragma once

// The calendar's events (Soft UI plan, phase 13): the Calendario app writes
// them, Starlight reads them to send the reminders. One event a line:
//
//   AAAA-MM-GG HH:MM HH:MM promemoria tuttoilgiorno titolo|note
//   2026-10-27 10:00 11:00 15 0 Revisione Fase 13|Chiudere i punti rimasti
//
// promemoria: minutes before the start (0 = none); a title or note keeps
// '|', '\' and line breaks as \p, \\ and \n. Lines starting with '#' are
// comments. Dates are days since 1970-01-01, local time.
//
// Header only, so the app, Starlight and the host tests share it. Define
// CAL_FILE_NO_IO before including it to leave out the file functions.

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#define CAL_FILE_PATH "/starlight/config/calendar.cfg"
#define CAL_MAX_EVENTS 256u
#define CAL_TITLE_MAX 64u
#define CAL_NOTES_MAX 160u

typedef struct {
    int64_t day;            // days since 1970-01-01
    uint32_t start_min;     // minutes from midnight
    uint32_t end_min;
    uint32_t remind_min;    // 0 = no reminder
    uint8_t all_day;
    char title[CAL_TITLE_MAX];
    char notes[CAL_NOTES_MAX];
} cal_event_t;

typedef struct {
    cal_event_t events[CAL_MAX_EVENTS];   // in time order
    uint32_t count;
} cal_store_t;

// ------------------------------------------------------------ the calendar

// Days from 1970-01-01 to y-m-d (proleptic Gregorian, month 1..12).
static inline int64_t cal_days_from_civil(int32_t y, int32_t m, int32_t d) {
    y -= m <= 2;
    int64_t era = (y >= 0 ? y : y - 399) / 400;
    int64_t yoe = y - era * 400;
    int64_t doy = (153 * (m + (m > 2 ? -3 : 9)) + 2) / 5 + d - 1;
    int64_t doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;
    return era * 146097 + doe - 719468;
}

static inline void cal_civil_from_days(int64_t z, int32_t *y, int32_t *m, int32_t *d) {
    z += 719468;
    int64_t era = (z >= 0 ? z : z - 146096) / 146097;
    int64_t doe = z - era * 146097;
    int64_t yoe = (doe - doe / 1460 + doe / 36524 - doe / 146096) / 365;
    int64_t doy = doe - (365 * yoe + yoe / 4 - yoe / 100);
    int64_t mp = (5 * doy + 2) / 153;
    int64_t dd = doy - (153 * mp + 2) / 5 + 1;
    int64_t mm = mp < 10 ? mp + 3 : mp - 9;
    *y = (int32_t)(yoe + era * 400 + (mm <= 2));
    *m = (int32_t)mm;
    *d = (int32_t)dd;
}

// 0 = Monday .. 6 = Sunday (1970-01-01 was a Thursday).
static inline uint32_t cal_weekday(int64_t day) {
    int64_t w = (day + 3) % 7;
    return (uint32_t)(w < 0 ? w + 7 : w);
}

static inline int32_t cal_month_days(int32_t y, int32_t m) {
    static const uint8_t k[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    if (m == 2 && ((y % 4 == 0 && y % 100 != 0) || y % 400 == 0)) return 29;
    return k[(m - 1) % 12];
}

// Seconds to add to UTC: 0, or Central European time with the EU summer
// time (from the last Sunday of March to the last Sunday of October, 01:00 UTC).
static inline int64_t cal_utc_offset(int64_t utc, uint8_t eu_central) {
    if (!eu_central) return 0;
    int32_t y, m, d;
    cal_civil_from_days(utc / 86400, &y, &m, &d);
    int64_t mar = cal_days_from_civil(y, 3, 31), oct = cal_days_from_civil(y, 10, 31);
    mar -= (int64_t)((cal_weekday(mar) + 1u) % 7u);   // back to Sunday
    oct -= (int64_t)((cal_weekday(oct) + 1u) % 7u);
    int64_t start = mar * 86400 + 3600, end = oct * 86400 + 3600;
    return (utc >= start && utc < end) ? 7200 : 3600;
}

// ------------------------------------------------------------- the events

static inline int cal_event_before(const cal_event_t *a, const cal_event_t *b) {
    if (a->day != b->day) return a->day < b->day;
    uint32_t sa = a->all_day ? 0u : a->start_min + 1u, sb = b->all_day ? 0u : b->start_min + 1u;
    return sa < sb;
}

// Adds e in time order; 0 if the store is full.
static inline int cal_store_add(cal_store_t *s, const cal_event_t *e) {
    if (s->count >= CAL_MAX_EVENTS) return 0;
    uint32_t at = s->count;
    while (at > 0u && cal_event_before(e, &s->events[at - 1u])) at--;
    memmove(&s->events[at + 1u], &s->events[at], (size_t)(s->count - at) * sizeof(s->events[0]));
    s->events[at] = *e;
    s->count++;
    return 1;
}

static inline void cal_store_remove(cal_store_t *s, uint32_t i) {
    if (i >= s->count) return;
    memmove(&s->events[i], &s->events[i + 1u], (size_t)(s->count - i - 1u) * sizeof(s->events[0]));
    s->count--;
}

static inline const char *cal_parse_uint(const char *p, uint32_t digits, uint32_t *out) {
    uint32_t v = 0u, n = 0u;
    while (*p >= '0' && *p <= '9' && (digits == 0u || n < digits)) {
        v = v * 10u + (uint32_t)(*p++ - '0');
        n++;
    }
    if (n == 0u || (digits && n != digits)) return NULL;
    *out = v;
    return p;
}

static inline const char *cal_parse_time(const char *p, uint32_t *out) {
    uint32_t h = 0u, m = 0u;
    if (!(p = cal_parse_uint(p, 2u, &h)) || *p++ != ':' || !(p = cal_parse_uint(p, 2u, &m))) return NULL;
    if (h > 23u || m > 59u) return NULL;
    *out = h * 60u + m;
    return p;
}

// Reads one escaped field up to '|' or the end of the line.
static inline const char *cal_parse_text(const char *p, char *out, size_t cap) {
    size_t o = 0u;
    while (*p && *p != '\n' && *p != '\r' && *p != '|') {
        char c = *p++;
        if (c == '\\' && *p) {
            char e = *p++;
            c = e == 'n' ? '\n' : e == 'p' ? '|' : e;
        }
        if (o + 1u < cap) out[o++] = c;
    }
    out[o] = '\0';
    return p;
}

// Parses one line into e: 1, or 0 for a comment or a malformed line.
static inline int cal_parse_line(const char *p, cal_event_t *e) {
    memset(e, 0, sizeof(*e));
    uint32_t y = 0u, mo = 0u, d = 0u, all = 0u;
    if (*p == '#') return 0;
    if (!(p = cal_parse_uint(p, 4u, &y)) || *p++ != '-' || !(p = cal_parse_uint(p, 2u, &mo)) || *p++ != '-' ||
        !(p = cal_parse_uint(p, 2u, &d)) || *p++ != ' ') return 0;
    if (mo < 1u || mo > 12u || d < 1u || (int32_t)d > cal_month_days((int32_t)y, (int32_t)mo)) return 0;
    if (!(p = cal_parse_time(p, &e->start_min)) || *p++ != ' ' || !(p = cal_parse_time(p, &e->end_min)) ||
        *p++ != ' ' || !(p = cal_parse_uint(p, 0u, &e->remind_min)) || *p++ != ' ' ||
        !(p = cal_parse_uint(p, 1u, &all)) || *p++ != ' ') return 0;
    e->day = cal_days_from_civil((int32_t)y, (int32_t)mo, (int32_t)d);
    e->all_day = all ? 1u : 0u;
    p = cal_parse_text(p, e->title, sizeof(e->title));
    if (*p == '|') (void)cal_parse_text(p + 1, e->notes, sizeof(e->notes));
    return e->title[0] != '\0';
}

// Fills s from the file's text (in time order, whatever the file's order).
static inline void cal_store_parse(cal_store_t *s, const char *text) {
    s->count = 0u;
    while (text && *text) {
        cal_event_t e;
        if (cal_parse_line(text, &e)) (void)cal_store_add(s, &e);
        while (*text && *text != '\n') text++;
        if (*text) text++;
    }
}

static inline size_t cal_put(char *out, size_t cap, size_t at, const char *s) {
    while (*s) {
        if (at + 1u < cap) out[at] = *s;
        at++;
        s++;
    }
    if (cap) out[at < cap ? at : cap - 1u] = '\0';
    return at;
}

static inline size_t cal_put_escaped(char *out, size_t cap, size_t at, const char *s) {
    for (; *s; s++) {
        char e[3] = { *s, 0, 0 };
        if (*s == '\\') e[1] = '\\';
        else if (*s == '|') { e[0] = '\\'; e[1] = 'p'; }
        else if (*s == '\n') { e[0] = '\\'; e[1] = 'n'; }
        else if (*s == '\r') continue;
        at = cal_put(out, cap, at, e);
    }
    return at;
}

static inline size_t cal_put_num(char *out, size_t cap, size_t at, uint32_t v, uint32_t width) {
    char tmp[12];
    uint32_t n = 0u;
    do {
        tmp[n++] = (char)('0' + v % 10u);
        v /= 10u;
    } while (v && n < sizeof(tmp));
    while (n < width && n < sizeof(tmp)) tmp[n++] = '0';
    char s[12];
    for (uint32_t i = 0u; i < n; i++) s[i] = tmp[n - 1u - i];
    s[n] = '\0';
    return cal_put(out, cap, at, s);
}

// The file's text for s; the length it needs (it fits if < cap).
static inline size_t cal_store_format(const cal_store_t *s, char *out, size_t cap) {
    size_t at = cal_put(out, cap, 0u, "# Calendario: un evento per riga (vedi peos/calendar_file.h)\n");
    for (uint32_t i = 0u; i < s->count; i++) {
        const cal_event_t *e = &s->events[i];
        int32_t y, m, d;
        cal_civil_from_days(e->day, &y, &m, &d);
        at = cal_put_num(out, cap, at, (uint32_t)y, 4u);
        at = cal_put(out, cap, at, "-");
        at = cal_put_num(out, cap, at, (uint32_t)m, 2u);
        at = cal_put(out, cap, at, "-");
        at = cal_put_num(out, cap, at, (uint32_t)d, 2u);
        at = cal_put(out, cap, at, " ");
        at = cal_put_num(out, cap, at, e->start_min / 60u, 2u);
        at = cal_put(out, cap, at, ":");
        at = cal_put_num(out, cap, at, e->start_min % 60u, 2u);
        at = cal_put(out, cap, at, " ");
        at = cal_put_num(out, cap, at, e->end_min / 60u, 2u);
        at = cal_put(out, cap, at, ":");
        at = cal_put_num(out, cap, at, e->end_min % 60u, 2u);
        at = cal_put(out, cap, at, " ");
        at = cal_put_num(out, cap, at, e->remind_min, 1u);
        at = cal_put(out, cap, at, e->all_day ? " 1 " : " 0 ");
        at = cal_put_escaped(out, cap, at, e->title);
        if (e->notes[0]) {
            at = cal_put(out, cap, at, "|");
            at = cal_put_escaped(out, cap, at, e->notes);
        }
        at = cal_put(out, cap, at, "\n");
    }
    return at;
}

#ifndef CAL_FILE_NO_IO
#include "peos/peos.h"

#define CAL_FILE_CAP (CAL_MAX_EVENTS * (CAL_TITLE_MAX + CAL_NOTES_MAX) * 2u + 4096u)

static char g_cal_file_buf[CAL_FILE_CAP];

// 1 if read (a missing file is an empty calendar, and 1 too).
static inline int cal_store_load(cal_store_t *s, const char *path) {
    s->count = 0u;
    uint64_t fd = peos_fopen(path, strlen(path), PEOS_O_READ);
    if (fd == (uint64_t)-1) return 1;
    uint64_t n = peos_fread(fd, g_cal_file_buf, CAL_FILE_CAP - 1u);
    peos_fclose(fd);
    if (n == (uint64_t)-1) return 0;
    g_cal_file_buf[n] = '\0';
    cal_store_parse(s, g_cal_file_buf);
    return 1;
}

static inline int cal_store_save(const cal_store_t *s, const char *path) {
    size_t len = cal_store_format(s, g_cal_file_buf, CAL_FILE_CAP);
    if (len >= CAL_FILE_CAP) return 0;
    uint64_t fd = peos_fopen(path, strlen(path), PEOS_O_CREAT | PEOS_O_TRUNC | PEOS_O_WRITE);
    if (fd == (uint64_t)-1) return 0;
    uint64_t w = peos_fwrite(fd, g_cal_file_buf, len);
    peos_fclose(fd);
    return w == len;
}
#endif

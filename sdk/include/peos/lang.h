#pragma once

// Languages (Technical Alpha plan, phase 4). A language is a folder; its
// sentences are in String.lang:
//
//   # comment
//   key=text
//   greeting=Hello, %s!
//   files.one=%u file
//   files.other=%u files
//   about.body=First line\nsecond line
//
// UTF-8, one sentence per line, the key up to the first '='. Spaces around
// the key and the text are dropped. In the text, \n is a new line, \t a tab
// and \\ a backslash. A key given twice keeps the last text.
//
// The system's sentences: /system/lang/<code>/String.lang. An app's:
// <bundle>/resources/<code>/String.lang, beside its translated images and
// files (slui_text / slui_resource in starlight_ui.h). English, "en", is the
// fallback; then the key itself.
//
// The parser below is freestanding (the host tests include it); loading a
// file is peos_lang_load in libpeos.

#include <stddef.h>
#include <stdint.h>

#define PEOS_LANG_FILE "String.lang"
#define PEOS_LANG_SYSTEM_DIR "/system/lang"
#define PEOS_LANG_FALLBACK "en"
#define PEOS_LANG_CODE_MAX 16u

typedef struct {
    const char *key;
    const char *text;
} peos_lang_entry_t;

// Upper bound of the entries in len bytes: one per line.
static inline uint32_t peos_lang_max_entries(const char *buf, size_t len) {
    uint32_t n = 1u;
    for (size_t i = 0; i < len; i++) n += buf[i] == '\n';
    return n;
}

static inline int peos_lang_key_cmp(const char *a, const char *b) {
    while (*a && *a == *b) {
        a++;
        b++;
    }
    return (int)(unsigned char)*a - (int)(unsigned char)*b;
}

static inline int peos_lang_is_space(char c) {
    return c == ' ' || c == '\t' || c == '\r';
}

// Parses buf (len bytes and room for one more, changed in place: the entries
// point into it) into out[cap], sorted by key. -> the number of entries.
static inline uint32_t peos_lang_parse(char *buf, size_t len, peos_lang_entry_t *out, uint32_t cap) {
    uint32_t n = 0u;
    size_t pos = 0;
    // a UTF-8 byte order mark is not part of the first key
    if (len >= 3u && (unsigned char)buf[0] == 0xEFu && (unsigned char)buf[1] == 0xBBu &&
        (unsigned char)buf[2] == 0xBFu) {
        pos = 3u;
    }
    while (pos < len) {
        size_t start = pos;
        while (pos < len && buf[pos] != '\n') pos++;
        size_t end = pos;
        if (pos < len) buf[pos++] = '\0';

        while (start < end && peos_lang_is_space(buf[start])) start++;
        while (end > start && peos_lang_is_space(buf[end - 1u])) end--;
        if (start == end || buf[start] == '#') continue;
        buf[end] = '\0';

        size_t eq = start;
        while (eq < end && buf[eq] != '=') eq++;
        if (eq == end || eq == start) continue;   // no '=' or no key
        size_t key_end = eq;
        while (key_end > start && peos_lang_is_space(buf[key_end - 1u])) key_end--;
        buf[key_end] = '\0';
        size_t text = eq + 1u;
        while (text < end && peos_lang_is_space(buf[text])) text++;

        // the escapes, in place (the text only gets shorter)
        size_t w = text;
        for (size_t r = text; r < end; r++) {
            char c = buf[r];
            if (c == '\\' && r + 1u < end) {
                char e = buf[r + 1u];
                if (e == 'n') c = '\n';
                else if (e == 't') c = '\t';
                else if (e == '\\') c = '\\';
                else {
                    buf[w++] = c;   // unknown: kept as written
                    continue;
                }
                r++;
            }
            buf[w++] = c;
        }
        buf[w] = '\0';

        if (n < cap) {
            out[n].key = buf + start;
            out[n].text = buf + text;
            n++;
        }
    }

    // insertion sort, stable: of two equal keys the later stays later
    for (uint32_t i = 1u; i < n; i++) {
        peos_lang_entry_t e = out[i];
        uint32_t j = i;
        while (j > 0u && peos_lang_key_cmp(out[j - 1u].key, e.key) > 0) {
            out[j] = out[j - 1u];
            j--;
        }
        out[j] = e;
    }
    return n;
}

// The text of key, or NULL. Of a key given twice, the last one.
static inline const char *peos_lang_find(const peos_lang_entry_t *ents, uint32_t n, const char *key) {
    if (!ents || !key) return NULL;
    uint32_t lo = 0u, hi = n;
    while (lo < hi) {
        uint32_t mid = lo + (hi - lo) / 2u;
        if (peos_lang_key_cmp(ents[mid].key, key) <= 0) lo = mid + 1u;
        else hi = mid;
    }
    // lo: the first entry after key
    if (lo > 0u && peos_lang_key_cmp(ents[lo - 1u].key, key) == 0) return ents[lo - 1u].text;
    return NULL;
}

// A language code is short and plain: letters, digits, '-' and '_' ("it",
// "en", "pt-BR"). Anything else is refused, so it can name a folder.
static inline int peos_lang_code_valid(const char *code) {
    if (!code || !code[0]) return 0;
    size_t i = 0;
    for (; code[i]; i++) {
        char c = code[i];
        int ok = (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '-' || c == '_';
        if (!ok || i + 1u >= PEOS_LANG_CODE_MAX) return 0;
    }
    return 1;
}

// A loaded String.lang (libpeos).
typedef struct {
    char *buf;
    peos_lang_entry_t *ents;
    uint32_t count;
} peos_lang_t;

#ifndef PEOS_LANG_NO_LIBPEOS
// Loads path into *lang (emptied first). 0, or -1 when the file cannot be
// read (lang is then empty: every lookup fails).
int peos_lang_load(peos_lang_t *lang, const char *path);
void peos_lang_free(peos_lang_t *lang);
// The text of key, or NULL.
const char *peos_lang_get(const peos_lang_t *lang, const char *key);
#endif

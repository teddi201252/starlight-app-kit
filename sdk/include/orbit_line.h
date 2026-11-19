#pragma once

// Orbit's line editor: the line being typed, its cursor and the history.
// Pure: keys go in as the characters a terminal sends (ANSI sequences for
// the arrows), and what to show goes out through `write` as text and
// cursor moves (\b-free: CSI n C / CSI n D, spaces over what is left).
// The same sequences work in Starlight's terminal and in the kernel console.

#include <stddef.h>
#include <stdint.h>

#define ORBIT_LINE_MAX 256u          // with the NUL: orbit_shell takes 255 characters
#define ORBIT_HISTORY_MAX 500u
#define ORBIT_SEARCH_MAX 64u

typedef void (*orbit_line_write_fn)(const char *text, size_t len, void *ctx);

// Tab completes the word before the cursor. The completer gets that word
// (its quotes taken away) and whether it is the line's first word (a
// command), and adds every word it could become with orbit_line_match_add
// (a folder ends with '/', so the next Tab goes on inside it).
#define ORBIT_MATCH_LIST 4096u
typedef struct {
    size_t count;
    char common[ORBIT_LINE_MAX];     // what every match starts with
    size_t common_len;
    char list[ORBIT_MATCH_LIST];     // the matches, each ending with a NUL
    size_t list_len;
    size_t dropped;                  // matches that did not fit in list
} orbit_line_matches_t;

typedef void (*orbit_line_complete_fn)(const char *word, int command, orbit_line_matches_t *m, void *ctx);
void orbit_line_match_add(orbit_line_matches_t *m, const char *word);

typedef enum {
    ORBIT_LINE_EDITING = 0,
    ORBIT_LINE_DONE,                 // Enter: orbit_line_text() is the command
    ORBIT_LINE_CANCELLED,            // Ctrl+C: the line was dropped
} orbit_line_status_t;

typedef struct {
    char buf[ORBIT_LINE_MAX];
    size_t len;
    size_t cur;

    // What the terminal shows after the prompt, and where its cursor is.
    char shown[ORBIT_LINE_MAX + ORBIT_SEARCH_MAX + 16u];
    size_t shown_len;
    size_t shown_cur;

    char *hist[ORBIT_HISTORY_MAX];   // oldest first
    size_t hist_count;
    size_t hist_pos;                 // hist_count: the line being written
    char draft[ORBIT_LINE_MAX];      // kept while walking the history

    uint8_t esc;                     // 1 after ESC, 2 inside CSI, 3 after ESC O
    char csi[8];
    size_t csi_len;

    uint8_t searching;               // Ctrl+R
    char query[ORBIT_SEARCH_MAX];
    size_t query_len;
    size_t match;                    // index in hist, or hist_count: none

    uint8_t tabs;                    // Tabs in a row that completed nothing more

    orbit_line_write_fn write;
    void *ctx;
    orbit_line_complete_fn complete;
    void *complete_ctx;
    const char *prompt;              // written again after a list of matches
} orbit_line_t;

void orbit_line_init(orbit_line_t *ol, orbit_line_write_fn write, void *ctx);
void orbit_line_free(orbit_line_t *ol);
// Without a completer Tab does nothing. The prompt is the text the caller
// writes before each line (shown again under a list of matches).
void orbit_line_set_completion(orbit_line_t *ol, orbit_line_complete_fn fn, void *ctx);
void orbit_line_set_prompt(orbit_line_t *ol, const char *prompt);

// A new, empty line (the caller has written the prompt).
void orbit_line_begin(orbit_line_t *ol);
orbit_line_status_t orbit_line_feed(orbit_line_t *ol, char c);
const char *orbit_line_text(const orbit_line_t *ol);

// Empty lines and the same command twice in a row are not kept.
// Returns 1 when it was added.
int orbit_line_history_add(orbit_line_t *ol, const char *line);
void orbit_line_history_clear(orbit_line_t *ol);
size_t orbit_line_history_count(const orbit_line_t *ol);
const char *orbit_line_history_at(const orbit_line_t *ol, size_t i);

// The history as text, one command a line (the file's format).
void orbit_line_history_load(orbit_line_t *ol, const char *text, size_t len);
size_t orbit_line_history_save(const orbit_line_t *ol, char *out, size_t cap);

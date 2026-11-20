#pragma once

#include <stddef.h>
#include <stdint.h>

#include "orbit_line.h"

typedef void (*orbit_shell_write_fn)(const char *text, size_t len, void *ctx);
typedef uint64_t (*orbit_shell_execv_fn)(const char *path, const char *arg_str, void *ctx);
// `history` lists the commands, `history -c` forgets them (the editor keeps them).
typedef void (*orbit_shell_history_fn)(int clear, void *ctx);

// `open <file>`: the desktop opens file (absolute) with the app bundle
// (peos/openwith.h chose it); NULL where there is no desktop.
typedef void (*orbit_shell_open_fn)(const char *bundle, const char *file, void *ctx);

typedef struct {
    orbit_shell_write_fn write;
    orbit_shell_execv_fn execv;
    orbit_shell_history_fn history;
    void *ctx;
    orbit_shell_open_fn open_file;   // last: older callers leave it NULL
} orbit_shell_io_t;

const char *orbit_shell_prompt(void);
void orbit_shell_print_banner(const orbit_shell_io_t *io);
void orbit_shell_execute_line(const orbit_shell_io_t *io, const char *line,
                              int *out_clear_requested, int *out_exit_requested);

// Tab in Orbit (orbit_line_set_completion): the first word is completed
// with Orbit's own commands and the programs in /bin, the others as paths
// (a hidden name only when the word starts with '.').
void orbit_shell_complete(const char *word, int command, orbit_line_matches_t *m);

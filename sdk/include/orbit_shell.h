#pragma once

#include <stddef.h>
#include <stdint.h>

typedef void (*orbit_shell_write_fn)(const char *text, size_t len, void *ctx);
typedef uint64_t (*orbit_shell_execv_fn)(const char *path, const char *arg_str, void *ctx);

typedef struct {
    orbit_shell_write_fn write;
    orbit_shell_execv_fn execv;
    void *ctx;
} orbit_shell_io_t;

const char *orbit_shell_prompt(void);
void orbit_shell_print_banner(const orbit_shell_io_t *io);
void orbit_shell_execute_line(const orbit_shell_io_t *io, const char *line,
                              int *out_clear_requested, int *out_exit_requested);

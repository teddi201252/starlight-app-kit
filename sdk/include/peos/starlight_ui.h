#pragma once

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    SLUI_EVENT_NONE = 0,
    SLUI_EVENT_BUTTON = 1,
    SLUI_EVENT_SLIDER = 2,
    SLUI_EVENT_HOST_ERROR = 3,
    SLUI_EVENT_TOGGLE = 4,
    SLUI_EVENT_SELECT = 5,
    SLUI_EVENT_TEXT_INPUT = 6,
    SLUI_EVENT_SCROLL_V = 7,
    SLUI_EVENT_TEXT_AREA = 8,
    SLUI_EVENT_KEY_DOWN = 9,
    SLUI_EVENT_KEY_UP = 10,
    SLUI_EVENT_KEY_REPEAT = 11,
} slui_event_type_t;

#define SLUI_SDK_VERSION_MAJOR 1u
#define SLUI_SDK_VERSION_MINOR 3u
#define SLUI_SDK_VERSION_PATCH 1u
#define SLUI_SDK_VERSION_U32 ((SLUI_SDK_VERSION_MAJOR << 16) | (SLUI_SDK_VERSION_MINOR << 8) | SLUI_SDK_VERSION_PATCH)

#define SLUI_PROTOCOL_VERSION 1u
#define SLUI_PROTOCOL_MIN_SUPPORTED 1u
#define SLUI_PROTOCOL_MAX_SUPPORTED 1u

#define SLUI_CAP_LABEL        (1u << 0)
#define SLUI_CAP_BUTTON       (1u << 1)
#define SLUI_CAP_SLIDER       (1u << 2)
#define SLUI_CAP_EVENT_BUTTON (1u << 3)
#define SLUI_CAP_EVENT_SLIDER (1u << 4)
#define SLUI_CAP_IMAGE_FILE   (1u << 5)
#define SLUI_CAP_TOGGLE       (1u << 6)
#define SLUI_CAP_SELECT       (1u << 7)
#define SLUI_CAP_TEXT_INPUT   (1u << 8)
#define SLUI_CAP_SCROLL_V     (1u << 9)
#define SLUI_CAP_EVENT_TOGGLE     (1u << 10)
#define SLUI_CAP_EVENT_SELECT     (1u << 11)
#define SLUI_CAP_EVENT_TEXT_INPUT (1u << 12)
#define SLUI_CAP_EVENT_SCROLL_V   (1u << 13)
#define SLUI_CAP_CLIP             (1u << 14)
#define SLUI_CAP_SEPARATOR        (1u << 15)
#define SLUI_CAP_LIST_ITEM        (1u << 16)
#define SLUI_CAP_DISABLED_STATE   (1u << 17)
#define SLUI_CAP_RECT             (1u << 18)
#define SLUI_CAP_RECT_ALPHA       (1u << 19)
#define SLUI_CAP_TEXT_AREA        (1u << 20)
#define SLUI_CAP_EVENT_TEXT_AREA  (1u << 21)
#define SLUI_CAP_EVENT_KEYBOARD   (1u << 22)
#define SLUI_CAP_THEME_TOKENS     (1u << 23)

#define SLUI_MOD_SHIFT (1u << 0)
#define SLUI_MOD_CTRL  (1u << 1)
#define SLUI_MOD_ALT   (1u << 2)
#define SLUI_MOD_SUPER (1u << 3)

#define SLUI_EVENT_TEXT_MAX 4096u

typedef struct {
    slui_event_type_t type;
    uint32_t id;
    uint32_t value;
    uint16_t keycode;
    uint16_t mods;
    char text[SLUI_EVENT_TEXT_MAX];
} slui_event_t;

typedef enum {
    SLUI_COLOR_BG_CANVAS = 0,
    SLUI_COLOR_BG_PANEL,
    SLUI_COLOR_BG_CONTROL,
    SLUI_COLOR_BG_CONTROL_HOVER,
    SLUI_COLOR_BG_CONTROL_ACTIVE,
    SLUI_COLOR_BORDER,
    SLUI_COLOR_BORDER_FOCUS,
    SLUI_COLOR_TEXT_PRIMARY,
    SLUI_COLOR_TEXT_SECONDARY,
    SLUI_COLOR_TEXT_DISABLED,
    SLUI_COLOR_ACCENT,
    SLUI_COLOR_ACCENT_HOVER,
    SLUI_COLOR_ACCENT_ACTIVE,
    SLUI_COLOR_DANGER,
    SLUI_COLOR_SUCCESS,
    SLUI_COLOR_WARNING,
    SLUI_COLOR_COUNT
} slui_color_token_t;

typedef enum {
    SLUI_STACK_HORIZONTAL = 0,
    SLUI_STACK_VERTICAL = 1,
} slui_stack_axis_t;

typedef struct {
    uint32_t x;
    uint32_t y;
    uint32_t w;
    uint32_t h;
    uint32_t spacing;
    uint32_t cursor;
    slui_stack_axis_t axis;
} slui_stack_t;

typedef struct {
    uint32_t x;
    uint32_t y;
    uint32_t w;
    uint32_t h;
    uint32_t cols;
    uint32_t rows;
    uint32_t gap_x;
    uint32_t gap_y;
    uint32_t cursor;
} slui_grid_t;

static inline void slui_stack_begin(slui_stack_t *stack,
                                    uint32_t x, uint32_t y, uint32_t w, uint32_t h,
                                    slui_stack_axis_t axis, uint32_t spacing) {
    if (!stack) return;
    stack->x = x;
    stack->y = y;
    stack->w = w;
    stack->h = h;
    stack->spacing = spacing;
    stack->cursor = 0u;
    stack->axis = axis;
}

static inline int slui_stack_next(slui_stack_t *stack, uint32_t primary_size,
                                  uint32_t *out_x, uint32_t *out_y,
                                  uint32_t *out_w, uint32_t *out_h) {
    if (!stack || !out_x || !out_y || !out_w || !out_h || primary_size == 0u) return 0;

    if (stack->axis == SLUI_STACK_VERTICAL) {
        if (stack->cursor > stack->h) return 0;
        if (primary_size > (stack->h - stack->cursor)) return 0;

        *out_x = stack->x;
        *out_y = stack->y + stack->cursor;
        *out_w = stack->w;
        *out_h = primary_size;
    } else {
        if (stack->cursor > stack->w) return 0;
        if (primary_size > (stack->w - stack->cursor)) return 0;

        *out_x = stack->x + stack->cursor;
        *out_y = stack->y;
        *out_w = primary_size;
        *out_h = stack->h;
    }

    uint32_t next_cursor = stack->cursor + primary_size;
    if (next_cursor < stack->cursor) return 0;
    if (stack->spacing > 0u) {
        if (next_cursor > 0xFFFFFFFFu - stack->spacing) return 0;
        next_cursor += stack->spacing;
    }
    stack->cursor = next_cursor;
    return 1;
}

static inline void slui_grid_begin(slui_grid_t *grid,
                                   uint32_t x, uint32_t y, uint32_t w, uint32_t h,
                                   uint32_t cols, uint32_t rows,
                                   uint32_t gap_x, uint32_t gap_y) {
    if (!grid) return;
    grid->x = x;
    grid->y = y;
    grid->w = w;
    grid->h = h;
    grid->cols = cols;
    grid->rows = rows;
    grid->gap_x = gap_x;
    grid->gap_y = gap_y;
    grid->cursor = 0u;
}

static inline int slui_grid_metrics(const slui_grid_t *grid, uint32_t *cell_w, uint32_t *cell_h) {
    if (!grid || !cell_w || !cell_h || grid->cols == 0u || grid->rows == 0u) return 0;

    uint64_t gaps_x = (uint64_t)(grid->cols - 1u) * (uint64_t)grid->gap_x;
    uint64_t gaps_y = (uint64_t)(grid->rows - 1u) * (uint64_t)grid->gap_y;
    if (gaps_x >= (uint64_t)grid->w || gaps_y >= (uint64_t)grid->h) return 0;

    *cell_w = (uint32_t)(((uint64_t)grid->w - gaps_x) / (uint64_t)grid->cols);
    *cell_h = (uint32_t)(((uint64_t)grid->h - gaps_y) / (uint64_t)grid->rows);
    return (*cell_w > 0u && *cell_h > 0u) ? 1 : 0;
}

static inline int slui_grid_cell(const slui_grid_t *grid, uint32_t col, uint32_t row,
                                 uint32_t *out_x, uint32_t *out_y,
                                 uint32_t *out_w, uint32_t *out_h) {
    if (!grid || !out_x || !out_y || !out_w || !out_h) return 0;
    if (col >= grid->cols || row >= grid->rows) return 0;

    uint32_t cell_w = 0u;
    uint32_t cell_h = 0u;
    if (!slui_grid_metrics(grid, &cell_w, &cell_h)) return 0;

    uint64_t step_x = (uint64_t)cell_w + (uint64_t)grid->gap_x;
    uint64_t step_y = (uint64_t)cell_h + (uint64_t)grid->gap_y;
    uint64_t px = (uint64_t)grid->x + ((uint64_t)col * step_x);
    uint64_t py = (uint64_t)grid->y + ((uint64_t)row * step_y);
    if (px > 0xFFFFFFFFu || py > 0xFFFFFFFFu) return 0;

    *out_x = (uint32_t)px;
    *out_y = (uint32_t)py;
    *out_w = cell_w;
    *out_h = cell_h;
    return 1;
}

static inline int slui_grid_next(slui_grid_t *grid,
                                 uint32_t *out_x, uint32_t *out_y,
                                 uint32_t *out_w, uint32_t *out_h) {
    if (!grid || !out_x || !out_y || !out_w || !out_h) return 0;
    if (grid->cols == 0u || grid->rows == 0u) return 0;

    uint64_t total = (uint64_t)grid->cols * (uint64_t)grid->rows;
    if ((uint64_t)grid->cursor >= total) return 0;

    uint32_t col = grid->cursor % grid->cols;
    uint32_t row = grid->cursor / grid->cols;
    if (!slui_grid_cell(grid, col, row, out_x, out_y, out_w, out_h)) return 0;

    grid->cursor++;
    return 1;
}

void slui_begin_frame(void);
void slui_end_frame(void);

void slui_label(uint32_t x, uint32_t y, const char *text,
                uint8_t r, uint8_t g, uint8_t b);
void slui_label_theme(uint32_t x, uint32_t y, slui_color_token_t token, const char *text);

void slui_button(uint32_t id,
                 uint32_t x, uint32_t y, uint32_t w, uint32_t h,
                 const char *text);
void slui_button_state(uint32_t id,
                       uint32_t x, uint32_t y, uint32_t w, uint32_t h,
                       uint8_t enabled, const char *text);

void slui_slider_u32(uint32_t id,
                     uint32_t x, uint32_t y, uint32_t w,
                     uint32_t min_v, uint32_t max_v, uint32_t value);
void slui_slider_u32_state(uint32_t id,
                           uint32_t x, uint32_t y, uint32_t w,
                           uint32_t min_v, uint32_t max_v, uint32_t value,
                           uint8_t enabled);

void slui_toggle(uint32_t id,
                 uint32_t x, uint32_t y, uint32_t w, uint32_t h,
                 uint8_t value, const char *text);
void slui_toggle_state(uint32_t id,
                       uint32_t x, uint32_t y, uint32_t w, uint32_t h,
                       uint8_t value, uint8_t enabled, const char *text);

void slui_select(uint32_t id,
                 uint32_t x, uint32_t y, uint32_t w, uint32_t h,
                 uint32_t option_count, uint32_t selected,
                 const char *options_pipe);
void slui_select_state(uint32_t id,
                       uint32_t x, uint32_t y, uint32_t w, uint32_t h,
                       uint32_t option_count, uint32_t selected,
                       uint8_t enabled, const char *options_pipe);

void slui_text_input(uint32_t id,
                     uint32_t x, uint32_t y, uint32_t w, uint32_t h,
                     uint32_t max_chars, const char *text);
void slui_text_input_state(uint32_t id,
                           uint32_t x, uint32_t y, uint32_t w, uint32_t h,
                           uint32_t max_chars, uint8_t enabled, const char *text);
void slui_text_area(uint32_t id,
                    uint32_t x, uint32_t y, uint32_t w, uint32_t h,
                    uint32_t max_chars, const char *text);
void slui_text_area_state(uint32_t id,
                          uint32_t x, uint32_t y, uint32_t w, uint32_t h,
                          uint32_t max_chars, uint8_t enabled, const char *text);

void slui_scroll_v(uint32_t id,
                   uint32_t x, uint32_t y, uint32_t w, uint32_t h,
                   uint32_t content_h, uint32_t offset);
void slui_scroll_v_state(uint32_t id,
                         uint32_t x, uint32_t y, uint32_t w, uint32_t h,
                         uint32_t content_h, uint32_t offset, uint8_t enabled);

void slui_clip_push(uint32_t x, uint32_t y, uint32_t w, uint32_t h);
void slui_clip_pop(void);

void slui_separator(uint32_t x, uint32_t y, uint32_t w);
void slui_rect(uint32_t x, uint32_t y, uint32_t w, uint32_t h,
               uint8_t r, uint8_t g, uint8_t b);
void slui_rect_a(uint32_t x, uint32_t y, uint32_t w, uint32_t h,
                 uint8_t r, uint8_t g, uint8_t b, uint8_t a);
void slui_rect_theme(uint32_t x, uint32_t y, uint32_t w, uint32_t h, slui_color_token_t token);
void slui_rect_a_theme(uint32_t x, uint32_t y, uint32_t w, uint32_t h, slui_color_token_t token, uint8_t a);
void slui_list_item(uint32_t id,
                    uint32_t x, uint32_t y, uint32_t w, uint32_t h,
                    uint8_t enabled,
                    const char *title,
                    const char *subtitle);

#define SLUI_DIALOG_MAX_TITLE_LINES 2u
#define SLUI_DIALOG_MAX_MSG_LINES   8u
#define SLUI_DIALOG_LINE_CAP        96u

typedef struct {
    uint32_t x;
    uint32_t y;
    uint32_t w;
    uint32_t h;
    uint32_t text_x;
    uint32_t title_y;
    uint32_t msg_y;
    uint32_t btn_y;
    uint32_t cancel_x;
    uint32_t ok_x;
    uint32_t cancel_w;
    uint32_t ok_w;
    uint32_t title_lines_n;
    uint32_t msg_lines_n;
    char title_lines[SLUI_DIALOG_MAX_TITLE_LINES][SLUI_DIALOG_LINE_CAP];
    char msg_lines[SLUI_DIALOG_MAX_MSG_LINES][SLUI_DIALOG_LINE_CAP];
} slui_dialog_t;

typedef enum {
    SLUI_DIALOG_RESULT_NONE = 0,
    SLUI_DIALOG_RESULT_CANCEL = 1,
    SLUI_DIALOG_RESULT_OK = 2,
} slui_dialog_result_t;

static inline uint32_t slui_dialog_max_u32(uint32_t a, uint32_t b) {
    return (a > b) ? a : b;
}

static inline uint32_t slui_dialog_strlen(const char *s) {
    uint32_t n = 0u;
    if (!s) return 0u;
    while (s[n]) n++;
    return n;
}

static inline void slui_dialog_copy(char *dst, uint32_t cap, const char *src) {
    uint32_t i = 0u;
    if (!dst || cap == 0u) return;
    if (!src) src = "";
    while (i + 1u < cap && src[i]) {
        dst[i] = src[i];
        i++;
    }
    dst[i] = '\0';
}

static inline uint32_t slui_dialog_wrap_lines(const char *src,
                                              uint32_t max_chars_per_line,
                                              char out[][SLUI_DIALOG_LINE_CAP],
                                              uint32_t max_lines) {
    uint32_t lines = 0u;
    const char *p = src ? src : "";
    if (!out || max_lines == 0u || max_chars_per_line == 0u) return 0u;

    while (*p && lines < max_lines) {
        while (*p == ' ') p++;
        if (*p == '\0') break;

        uint32_t n = 0u;
        const char *start = p;
        const char *last_space = NULL;
        while (p[n] && n < max_chars_per_line) {
            if (p[n] == ' ') last_space = &p[n];
            n++;
        }

        if (p[n] == '\0') {
            slui_dialog_copy(out[lines], SLUI_DIALOG_LINE_CAP, start);
            lines++;
            break;
        }

        if (last_space && last_space > start) {
            uint32_t take = (uint32_t)(last_space - start);
            if (take >= SLUI_DIALOG_LINE_CAP) take = SLUI_DIALOG_LINE_CAP - 1u;
            for (uint32_t i = 0u; i < take; i++) out[lines][i] = start[i];
            out[lines][take] = '\0';
            p = last_space + 1;
        } else {
            uint32_t take = max_chars_per_line;
            if (take >= SLUI_DIALOG_LINE_CAP) take = SLUI_DIALOG_LINE_CAP - 1u;
            for (uint32_t i = 0u; i < take; i++) out[lines][i] = start[i];
            out[lines][take] = '\0';
            p = start + take;
        }
        lines++;
    }
    return lines;
}

static inline void slui_dialog_prepare(slui_dialog_t *dlg,
                                       uint32_t viewport_w, uint32_t viewport_h,
                                       const char *title, const char *message,
                                       uint32_t min_w, uint32_t max_w) {
    uint32_t content_chars = 0u;
    uint32_t wanted_w = 0u;
    uint32_t text_max_chars = 0u;
    uint32_t content_lines_n = 0u;
    uint32_t margins = 18u;
    if (!dlg) return;

    if (min_w < 180u) min_w = 180u;
    if (max_w < min_w) max_w = min_w;
    if (viewport_w < min_w) min_w = viewport_w;
    if (max_w > viewport_w) max_w = viewport_w;

    content_chars = slui_dialog_max_u32(slui_dialog_strlen(title), slui_dialog_strlen(message));
    wanted_w = 36u + (content_chars * 8u);
    if (wanted_w < min_w) wanted_w = min_w;
    if (wanted_w > max_w) wanted_w = max_w;

    dlg->w = wanted_w;
    dlg->x = (viewport_w > dlg->w) ? ((viewport_w - dlg->w) / 2u) : 0u;
    dlg->text_x = dlg->x + 18u;

    text_max_chars = (dlg->w > 36u) ? ((dlg->w - 36u) / 8u) : 1u;
    dlg->title_lines_n = slui_dialog_wrap_lines(title, text_max_chars,
                                                dlg->title_lines, SLUI_DIALOG_MAX_TITLE_LINES);
    dlg->msg_lines_n = slui_dialog_wrap_lines(message, text_max_chars,
                                              dlg->msg_lines, SLUI_DIALOG_MAX_MSG_LINES);
    content_lines_n = dlg->title_lines_n + dlg->msg_lines_n;
    if (content_lines_n < 2u) content_lines_n = 2u;

    dlg->h = 92u + (content_lines_n * 12u);
    if (dlg->h < 160u) dlg->h = 160u;
    if (dlg->h > viewport_h) dlg->h = viewport_h;
    dlg->y = (viewport_h > dlg->h) ? ((viewport_h - dlg->h) / 2u) : 0u;

    dlg->title_y = dlg->y + 20u;
    dlg->msg_y = dlg->title_y + (dlg->title_lines_n * 12u) + 8u;
    dlg->btn_y = dlg->y + dlg->h - 40u;

    dlg->cancel_w = 90u;
    dlg->ok_w = 82u;
    {
        uint32_t gap = 12u;
        uint32_t total = dlg->cancel_w + gap + dlg->ok_w + (margins * 2u);
        if (total > dlg->w) {
            dlg->cancel_w = 74u;
            dlg->ok_w = 74u;
            gap = 8u;
        }
        dlg->cancel_x = dlg->x + dlg->w - margins - dlg->ok_w - gap - dlg->cancel_w;
        dlg->ok_x = dlg->x + dlg->w - margins - dlg->ok_w;
    }
}

static inline void slui_dialog_draw(const slui_dialog_t *dlg,
                                    uint32_t viewport_w, uint32_t viewport_h,
                                    uint32_t id_cancel, uint32_t id_ok,
                                    const char *title_color_tag,
                                    const char *msg_color_tag,
                                    const char *cancel_label,
                                    const char *ok_label) {
    (void)title_color_tag;
    (void)msg_color_tag;
    if (!dlg) return;
    if (!cancel_label) cancel_label = "Annulla";
    if (!ok_label) ok_label = "OK";

    slui_rect_a(0u, 0u, viewport_w, viewport_h, 15u, 18u, 24u, 160u);
    slui_rect(dlg->x, dlg->y, dlg->w, dlg->h, 46u, 50u, 62u);
    slui_separator(dlg->x, dlg->y, dlg->w);
    slui_separator(dlg->x, dlg->y + dlg->h - 1u, dlg->w);

    for (uint32_t i = 0u; i < dlg->title_lines_n; i++) {
        slui_label(dlg->text_x, dlg->title_y + (i * 12u), dlg->title_lines[i], 230u, 230u, 230u);
    }
    for (uint32_t i = 0u; i < dlg->msg_lines_n; i++) {
        slui_label(dlg->text_x, dlg->msg_y + (i * 12u), dlg->msg_lines[i], 180u, 180u, 180u);
    }

    slui_button(id_cancel, dlg->cancel_x, dlg->btn_y, dlg->cancel_w, 24u, cancel_label);
    slui_button(id_ok, dlg->ok_x, dlg->btn_y, dlg->ok_w, 24u, ok_label);
}

static inline slui_dialog_result_t slui_dialog_map_event(const slui_event_t *ev,
                                                         uint32_t id_cancel,
                                                         uint32_t id_ok) {
    if (!ev || ev->type != SLUI_EVENT_BUTTON) return SLUI_DIALOG_RESULT_NONE;
    if (ev->id == id_cancel) return SLUI_DIALOG_RESULT_CANCEL;
    if (ev->id == id_ok) return SLUI_DIALOG_RESULT_OK;
    return SLUI_DIALOG_RESULT_NONE;
}

typedef struct {
    uint32_t x;
    uint32_t y;
    uint32_t w;
    uint32_t h;
    uint32_t spectrum_x;
    uint32_t spectrum_y;
    uint32_t spectrum_w;
    uint32_t spectrum_h;
    uint32_t preview_x;
    uint32_t preview_y;
    uint32_t preview_w;
    uint32_t preview_h;
    uint32_t slider_x;
    uint32_t slider_w;
    uint32_t hue_y;
    uint32_t sat_y;
    uint32_t val_y;
    uint32_t btn_y;
    uint32_t cancel_x;
    uint32_t ok_x;
} slui_color_picker_t;

typedef enum {
    SLUI_COLOR_PICKER_NONE = 0,
    SLUI_COLOR_PICKER_CHANGED = 1,
    SLUI_COLOR_PICKER_CANCEL = 2,
    SLUI_COLOR_PICKER_CONFIRM = 3,
} slui_color_picker_result_t;

static inline void slui_color_picker_hsv_to_rgb(uint32_t h, uint32_t s, uint32_t v,
                                                uint8_t *r, uint8_t *g, uint8_t *b) {
    uint32_t hi = 0u, f = 0u, p = 0u, q = 0u, t = 0u;
    if (!r || !g || !b) return;
    if (s > 255u) s = 255u;
    if (v > 255u) v = 255u;
    h %= 360u;
    if (s == 0u) {
        *r = (uint8_t)v; *g = (uint8_t)v; *b = (uint8_t)v;
        return;
    }
    hi = h / 60u;
    f = ((h % 60u) * 255u) / 60u;
    p = (v * (255u - s)) / 255u;
    q = (v * (255u - ((s * f) / 255u))) / 255u;
    t = (v * (255u - ((s * (255u - f)) / 255u))) / 255u;
    switch (hi) {
        case 0u: *r = (uint8_t)v; *g = (uint8_t)t; *b = (uint8_t)p; break;
        case 1u: *r = (uint8_t)q; *g = (uint8_t)v; *b = (uint8_t)p; break;
        case 2u: *r = (uint8_t)p; *g = (uint8_t)v; *b = (uint8_t)t; break;
        case 3u: *r = (uint8_t)p; *g = (uint8_t)q; *b = (uint8_t)v; break;
        case 4u: *r = (uint8_t)t; *g = (uint8_t)p; *b = (uint8_t)v; break;
        default: *r = (uint8_t)v; *g = (uint8_t)p; *b = (uint8_t)q; break;
    }
}

static inline void slui_color_picker_rgb_to_hsv(uint8_t r, uint8_t g, uint8_t b,
                                                uint32_t *h, uint32_t *s, uint32_t *v) {
    uint8_t maxc = r;
    uint8_t minc = r;
    uint32_t delta = 0u;
    int32_t ri = (int32_t)r;
    int32_t gi = (int32_t)g;
    int32_t bi = (int32_t)b;
    if (!h || !s || !v) return;
    if (g > maxc) maxc = g;
    if (b > maxc) maxc = b;
    if (g < minc) minc = g;
    if (b < minc) minc = b;

    *v = (uint32_t)maxc;
    delta = (uint32_t)maxc - (uint32_t)minc;
    if (maxc == 0u) {
        *s = 0u;
        *h = 0u;
        return;
    }
    *s = (delta * 255u) / (uint32_t)maxc;
    if (delta == 0u) {
        *h = 0u;
        return;
    }

    if (maxc == r) {
        int32_t base = (60 * (gi - bi)) / (int32_t)delta;
        if (base < 0) base += 360;
        *h = (uint32_t)base;
    } else if (maxc == g) {
        *h = (uint32_t)(120 + ((60 * (bi - ri)) / (int32_t)delta));
    } else {
        *h = (uint32_t)(240 + ((60 * (ri - gi)) / (int32_t)delta));
    }
    *h %= 360u;
}

static inline void slui_color_picker_prepare(slui_color_picker_t *p,
                                             uint32_t viewport_w, uint32_t viewport_h) {
    if (!p) return;
    p->w = 260u;
    p->h = 248u;
    p->x = (viewport_w > p->w) ? ((viewport_w - p->w) / 2u) : 0u;
    p->y = (viewport_h > p->h) ? ((viewport_h - p->h) / 2u) : 0u;

    p->spectrum_x = p->x + 10u;
    p->spectrum_y = p->y + 30u;
    p->spectrum_w = 146u;
    p->spectrum_h = 44u;
    p->preview_x = p->x + 166u;
    p->preview_y = p->y + 50u;
    p->preview_w = 84u;
    p->preview_h = 22u;
    p->slider_x = p->x + 58u;
    p->slider_w = 192u;
    p->hue_y = p->y + 104u;
    p->sat_y = p->y + 128u;
    p->val_y = p->y + 152u;
    p->btn_y = p->y + 206u;
    p->ok_x = p->x + 180u;
    p->cancel_x = p->x + 104u;
}

static inline void slui_color_picker_draw(const slui_color_picker_t *p,
                                          uint32_t viewport_w, uint32_t viewport_h,
                                          uint32_t id_slider_h, uint32_t id_slider_s, uint32_t id_slider_v, uint32_t id_slider_a,
                                          uint32_t id_cancel, uint32_t id_ok,
                                          uint32_t h, uint32_t s, uint32_t v, uint32_t a,
                                          uint8_t show_alpha,
                                          const char *title) {
    uint8_t pr = 0u, pg = 0u, pb = 0u;
    if (!p) return;
    if (!title) title = "Color picker";
    slui_rect_a_theme(0u, 0u, viewport_w, viewport_h, SLUI_COLOR_BG_CANVAS, 160u);
    slui_rect_theme(p->x, p->y, p->w, p->h, SLUI_COLOR_BG_PANEL);
    slui_label_theme(p->x + 10u, p->y + 10u, SLUI_COLOR_TEXT_PRIMARY, title);

    slui_rect_theme(p->spectrum_x, p->spectrum_y, p->spectrum_w, p->spectrum_h, SLUI_COLOR_BG_CANVAS);
    for (uint32_t i = 0u; i < 12u; i++) {
        uint32_t hh = (i * 359u) / 11u;
        uint8_t rr = 0u, gg = 0u, bb = 0u;
        slui_color_picker_hsv_to_rgb(hh, 255u, 255u, &rr, &gg, &bb);
        slui_rect(p->spectrum_x + 2u + (i * 11u), p->spectrum_y + 2u, 9u, 12u, rr, gg, bb);
    }
    slui_label_theme(p->spectrum_x + 4u, p->spectrum_y + 20u, SLUI_COLOR_TEXT_SECONDARY, "Anteprima spettro");

    slui_color_picker_hsv_to_rgb(h, s, v, &pr, &pg, &pb);
    slui_label_theme(p->preview_x, p->y + 34u, SLUI_COLOR_TEXT_SECONDARY, "Preview");
    slui_rect(p->preview_x, p->preview_y, p->preview_w, p->preview_h, pr, pg, pb);

    slui_label_theme(p->x + 10u, p->hue_y, SLUI_COLOR_TEXT_SECONDARY, "Hue");
    slui_slider_u32(id_slider_h, p->slider_x, p->hue_y, p->slider_w, 0u, 359u, h);
    slui_label_theme(p->x + 10u, p->sat_y, SLUI_COLOR_TEXT_SECONDARY, "Saturazione");
    slui_slider_u32(id_slider_s, p->slider_x, p->sat_y, p->slider_w, 0u, 255u, s);
    slui_label_theme(p->x + 10u, p->val_y, SLUI_COLOR_TEXT_SECONDARY, "Brightness");
    slui_slider_u32(id_slider_v, p->slider_x, p->val_y, p->slider_w, 0u, 255u, v);
    if (show_alpha) {
        uint32_t ay = p->val_y + 24u;
        slui_label_theme(p->x + 10u, ay, SLUI_COLOR_TEXT_SECONDARY, "Alpha");
        slui_slider_u32(id_slider_a, p->slider_x, ay, p->slider_w, 0u, 255u, a);
    }
    slui_button(id_ok, p->ok_x, p->btn_y, 70u, 24u, "Conferma");
    slui_button(id_cancel, p->cancel_x, p->btn_y, 70u, 24u, "Annulla");
}

static inline slui_color_picker_result_t slui_color_picker_handle_event(const slui_event_t *ev,
                                                                        uint32_t id_slider_h,
                                                                        uint32_t id_slider_s,
                                                                        uint32_t id_slider_v,
                                                                        uint32_t id_slider_a,
                                                                        uint32_t id_cancel,
                                                                        uint32_t id_ok,
                                                                        uint32_t *h,
                                                                        uint32_t *s,
                                                                        uint32_t *v,
                                                                        uint32_t *a) {
    if (!ev || !h || !s || !v) return SLUI_COLOR_PICKER_NONE;
    if (ev->type == SLUI_EVENT_BUTTON && ev->id == id_cancel) return SLUI_COLOR_PICKER_CANCEL;
    if (ev->type == SLUI_EVENT_BUTTON && ev->id == id_ok) return SLUI_COLOR_PICKER_CONFIRM;
    if (ev->type == SLUI_EVENT_SLIDER && ev->id == id_slider_h) {
        *h = (ev->value > 359u) ? 359u : ev->value;
        return SLUI_COLOR_PICKER_CHANGED;
    }
    if (ev->type == SLUI_EVENT_SLIDER && ev->id == id_slider_s) {
        *s = (ev->value > 255u) ? 255u : ev->value;
        return SLUI_COLOR_PICKER_CHANGED;
    }
    if (ev->type == SLUI_EVENT_SLIDER && ev->id == id_slider_v) {
        *v = (ev->value > 255u) ? 255u : ev->value;
        return SLUI_COLOR_PICKER_CHANGED;
    }
    if (a && ev->type == SLUI_EVENT_SLIDER && ev->id == id_slider_a) {
        *a = (ev->value > 255u) ? 255u : ev->value;
        return SLUI_COLOR_PICKER_CHANGED;
    }
    return SLUI_COLOR_PICKER_NONE;
}

typedef struct {
    uint32_t x;
    uint32_t y;
    uint32_t w;
    uint32_t h;
    uint32_t content_h;
    uint32_t offset;
    uint32_t cursor_y;
} slui_scroll_view_t;

static inline void slui_scroll_view_begin(slui_scroll_view_t *view,
                                          uint32_t scrollbar_id,
                                          uint32_t x, uint32_t y, uint32_t w, uint32_t h,
                                          uint32_t content_h, uint32_t *offset) {
    if (!view || !offset) return;

    uint32_t max_off = 0u;
    if (content_h > h) max_off = content_h - h;
    if (*offset > max_off) *offset = max_off;

    uint32_t bar_w = (w >= 14u) ? 12u : ((w > 4u) ? (w - 2u) : w);
    uint32_t content_w = (w > (bar_w + 4u)) ? (w - bar_w - 4u) : w;
    uint32_t bar_x = x + ((content_w < w) ? (content_w + 4u) : 0u);

    slui_scroll_v(scrollbar_id, bar_x, y, bar_w, h, content_h, *offset);
    slui_clip_push(x, y, content_w, h);

    view->x = x;
    view->y = y;
    view->w = content_w;
    view->h = h;
    view->content_h = content_h;
    view->offset = *offset;
    view->cursor_y = 0u;
}

static inline int slui_scroll_view_next(slui_scroll_view_t *view,
                                        uint32_t item_h, uint32_t spacing,
                                        uint32_t *out_x, uint32_t *out_y,
                                        uint32_t *out_w, uint32_t *out_h) {
    if (!view || !out_x || !out_y || !out_w || !out_h || item_h == 0u) return 0;

    uint32_t logical_y = view->cursor_y;
    uint32_t item_top = logical_y;
    uint32_t item_bottom = logical_y + item_h;
    if (item_bottom < item_top) item_bottom = 0xFFFFFFFFu;

    uint32_t window_top = view->offset;
    uint32_t window_bottom = view->offset + view->h;
    if (window_bottom < window_top) window_bottom = 0xFFFFFFFFu;

    int visible = (item_bottom > window_top && item_top < window_bottom) ? 1 : 0;

    int64_t y64 = (int64_t)view->y + (int64_t)item_top - (int64_t)window_top;
    uint32_t y = (y64 < 0) ? 0u : ((y64 > 0xFFFFFFFFll) ? 0xFFFFFFFFu : (uint32_t)y64);

    uint32_t next = logical_y + item_h;
    if (next >= logical_y && spacing > 0u && next <= 0xFFFFFFFFu - spacing) {
        next += spacing;
    }
    view->cursor_y = next;

    *out_x = view->x;
    *out_y = y;
    *out_w = view->w;
    *out_h = item_h;
    return visible;
}

static inline void slui_scroll_view_end(const slui_scroll_view_t *view) {
    (void)view;
    slui_clip_pop();
}

void slui_image_file(uint32_t x, uint32_t y, uint32_t w, uint32_t h,
                     const char *path);

int slui_poll_event(slui_event_t *out);
uint32_t slui_sdk_version_u32(void);
uint32_t slui_protocol_version(void);
uint32_t slui_host_protocol_version(void);
uint32_t slui_host_capabilities(void);
const char *slui_last_host_error(void);
void slui_reload_display(void);
void slui_reload_theme(void);
void slui_set_accent(uint8_t r, uint8_t g, uint8_t b);
void slui_set_accent_a(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
void slui_theme_reset_override(void);
int slui_theme_query(slui_color_token_t token, uint8_t *r, uint8_t *g, uint8_t *b);
int slui_theme_query_a(slui_color_token_t token, uint8_t *r, uint8_t *g, uint8_t *b, uint8_t *a);

#ifdef __cplusplus
}
#endif

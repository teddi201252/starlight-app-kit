// Soft UI: a tour of what an app can draw in Starlight's style (SLUI 1.0.5).
//
// - slui_surface: raised cards, inset wells, night panels, neon dots;
// - slui_face + slui_label_align: the skin's fonts, text aligned on a point;
// - slui_ring and slui_graph: a value as a ring, a history as a graph;
// - slui_segment, slui_button_primary, slui_toggle: the Soft UI controls;
// - slui_veil: a modal dialog over the whole window;
// - peos_notify: a notification on the desktop;
// - SLUI_EVENT_RESIZE: three cards in a row on a wide window, in a column
//   on a narrow one.

#include "peos/peos.h"
#include "peos/notify.h"
#include "peos/starlight_ui.h"

#include <stdio.h>
#include <string.h>

#define ID_VIEW     1u
#define ID_LEVEL    2u
#define ID_ADD      3u
#define ID_NOTIFY   4u
#define ID_DIALOG   5u
#define ID_GLOW     6u
#define ID_D_NAME   10u
#define ID_D_CANCEL 11u
#define ID_D_OK     12u

#define HISTORY 24u
#define NEON 0x59u, 0xD0u, 0xFFu
#define NIGHT_INK 0xE8u, 0xF4u, 0xFFu
#define NIGHT_DIM 0x9Fu, 0xB4u, 0xD8u

typedef struct {
    uint32_t view;              // the segmented control
    uint32_t level;             // 0..100, the ring
    uint16_t history[HISTORY];  // 0..1000, the graph
    uint8_t glow;               // the toggle
    uint8_t dialog;
    char name[32];
    char status[64];
} tour_t;

static void card_title(uint32_t x, uint32_t y, const char *text, uint8_t night) {
    slui_face("ui-14");
    if (night) slui_label(x, y, text, NIGHT_DIM);
    else slui_label_theme(x, y, SLUI_COLOR_TEXT_SECONDARY, text);
}

static void render(const tour_t *t) {
    uint32_t W = 0u, H = 0u;
    slui_viewport(&W, &H);
    // 0 x 0 until Starlight says the size (the first SLUI_EVENT_RESIZE), and
    // never less than app.ini's minimum: the layout below subtracts from it.
    if (W < 440u) W = 440u;
    if (H < 520u) H = 520u;
    slui_begin_frame();

    slui_face("ui-28-light");
    slui_label_theme(0u, 10u, SLUI_COLOR_TEXT_PRIMARY, "Soft UI");
    slui_segment(ID_VIEW, W - 260u, 0u, 260u, 44u, 2u, t->view, 1u, "Giorno|Settimana");

    // Three cards: in a row when there is room, else one under the other.
    uint8_t row = W >= 720u;
    uint32_t top = 64u, bottom = H - 76u, gap = 22u;
    uint32_t cw = row ? (W - 2u * gap) / 3u : W;
    uint32_t ch = row ? bottom - top : (bottom - top - 2u * gap) / 3u;
    uint32_t x[3], y[3];
    for (uint32_t i = 0u; i < 3u; i++) {
        x[i] = row ? i * (cw + gap) : 0u;
        y[i] = row ? top : top + i * (ch + gap);
    }

    // 1. A raised card with a ring and a big number.
    slui_surface(SLUI_SURF_RAISED, x[0], y[0], cw, ch, 26u);
    card_title(x[0] + 22u, y[0] + 22u, "LIVELLO", 0u);
    uint32_t ring = ch > 160u ? 110u : (ch > 90u ? ch - 50u : 40u);
    slui_ring(x[0] + 22u, y[0] + 44u, ring, ring / 10u, t->level * 10u, 0xF4u, 0xF5u, 0xF8u, 0x11u, 0x16u, 0x26u);
    char pct[8];
    snprintf(pct, sizeof(pct), "%u%%", (unsigned)t->level);
    slui_face("mono-16");
    slui_label_align_theme(x[0] + 22u + ring / 2u, y[0] + 44u + ring / 2u - 8u, SLUI_ALIGN_CENTER,
                           SLUI_COLOR_TEXT_PRIMARY, pct);
    if (cw > ring + 140u) {
        slui_face("ui-15");
        slui_label_theme(x[0] + ring + 44u, y[0] + 50u, SLUI_COLOR_TEXT_SECONDARY, "Trascina la barra:");
        slui_slider_u32(ID_LEVEL, x[0] + ring + 44u, y[0] + 76u, cw - ring - 66u, 0u, 100u, t->level);
    }

    // 2. A night panel with a graph of the last values.
    slui_surface(SLUI_SURF_NIGHT, x[1], y[1], cw, ch, 26u);
    card_title(x[1] + 22u, y[1] + 22u, t->view ? "SETTIMANA" : "GIORNO", 1u);
    slui_graph(x[1] + 22u, y[1] + 48u, cw - 44u, ch > 110u ? ch - 70u : 40u, t->history, HISTORY,
               NEON, 0x1Cu, 0x2Au, 0x4Au, 255u);
    slui_face("mono-12");
    slui_label_align(x[1] + cw - 22u, y[1] + 22u, SLUI_ALIGN_RIGHT, "ora", NIGHT_DIM);

    // 3. An inset well with a toggle, a neon dot and what happened last.
    slui_surface(SLUI_SURF_INSET, x[2], y[2], cw, ch, 26u);
    card_title(x[2] + 22u, y[2] + 22u, "STATO", 0u);
    slui_surface(t->glow ? SLUI_SURF_NEON : SLUI_SURF_FLAT, x[2] + 22u, y[2] + 52u, 12u, 12u, 6u);
    slui_face("ui-15");
    slui_label_theme(x[2] + 44u, y[2] + 50u, SLUI_COLOR_TEXT_PRIMARY, t->glow ? "Acceso" : "Spento");
    slui_toggle(ID_GLOW, x[2] + cw - 22u - 62u, y[2] + 42u, 62u, 32u, t->glow, "");
    slui_face("ui-14");
    slui_label_theme(x[2] + 22u, y[2] + 90u, SLUI_COLOR_TEXT_SECONDARY, t->status);

    // The buttons: the main one is night.
    uint8_t live = !t->dialog;
    slui_button_state(ID_NOTIFY, 0u, H - 54u, 130u, 44u, live, "Notifica");
    slui_button_state(ID_DIALOG, 142u, H - 54u, 130u, 44u, live, "Dialogo");
    slui_button_primary(ID_ADD, W - 150u, H - 54u, 150u, 44u, live, "+  Nuovo valore");

    // A modal dialog: the veil over all of the window, the dialog on top.
    if (t->dialog) {
        uint32_t dw = W > 460u ? 420u : W - 40u, dh = 230u;
        uint32_t dx = (W - dw) / 2u, dy = H > dh ? (H - dh) / 2u : 0u;
        slui_veil(87u);
        slui_surface(SLUI_SURF_RAISED, dx, dy, dw, dh, 30u);
        slui_face("ui-22-mid");
        slui_label_theme(dx + 28u, dy + 26u, SLUI_COLOR_TEXT_PRIMARY, "Come ti chiami?");
        slui_text_input(ID_D_NAME, dx + 28u, dy + 72u, dw - 56u, 46u, 31u, t->name);
        slui_button(ID_D_CANCEL, dx + dw - 28u - 100u - 14u - 110u, dy + dh - 72u, 110u, 44u, "Annulla");
        slui_button_primary(ID_D_OK, dx + dw - 28u - 100u, dy + dh - 72u, 100u, 44u, 1u, "OK");
    }
    slui_end_frame();
}

static void add_value(tour_t *t) {
    memmove(t->history, t->history + 1, (HISTORY - 1u) * sizeof(t->history[0]));
    // the next value: a step up or down from the last, from the clock's low bits
    int32_t last = t->history[HISTORY - 2u];
    int32_t next = last + (int32_t)(peos_time_ns() / 1000u % 241u) - 120;
    t->history[HISTORY - 1u] = (uint16_t)(next < 50 ? 50 : (next > 950 ? 950 : next));
}

static void on_event(tour_t *t, const slui_event_t *ev) {
    if (t->dialog) {
        if (ev->type == SLUI_EVENT_TEXT_INPUT && ev->id == ID_D_NAME) snprintf(t->name, sizeof(t->name), "%s", ev->text);
        else if (ev->type == SLUI_EVENT_BUTTON && ev->id == ID_D_CANCEL) t->dialog = 0u;
        else if ((ev->type == SLUI_EVENT_BUTTON && ev->id == ID_D_OK) ||
                 (ev->type == SLUI_EVENT_KEY_DOWN && ev->keycode == PEOS_KEY_ENTER)) {
            snprintf(t->status, sizeof(t->status), "Ciao, %s!", t->name[0] ? t->name : "sconosciuto");
            t->dialog = 0u;
        } else if (ev->type == SLUI_EVENT_KEY_DOWN && ev->keycode == PEOS_KEY_ESC) t->dialog = 0u;
        return;
    }
    if (ev->type == SLUI_EVENT_SELECT && ev->id == ID_VIEW) t->view = ev->value;
    else if (ev->type == SLUI_EVENT_SLIDER && ev->id == ID_LEVEL) t->level = ev->value;
    else if (ev->type == SLUI_EVENT_TOGGLE && ev->id == ID_GLOW) t->glow = ev->value ? 1u : 0u;
    else if (ev->type == SLUI_EVENT_BUTTON && ev->id == ID_ADD) add_value(t);
    else if (ev->type == SLUI_EVENT_BUTTON && ev->id == ID_DIALOG) {
        t->dialog = 1u;
        slui_focus(ID_D_NAME);
    } else if (ev->type == SLUI_EVENT_BUTTON && ev->id == ID_NOTIFY) {
        int ok = peos_notify("Soft UI", "Una notifica dall'app di esempio") == 0;
        snprintf(t->status, sizeof(t->status), ok ? "Notifica inviata" : "Nessun desktop per la notifica");
    }
}

int main(void) {
    static tour_t t;
    memset(&t, 0, sizeof(t));
    t.level = 62u;
    static const uint16_t start[HISTORY] = {
        320, 380, 360, 450, 520, 480, 560, 610, 540, 500, 580, 650,
        700, 640, 600, 680, 720, 690, 640, 700, 760, 730, 690, 740,
    };
    memcpy(t.history, start, sizeof(start));
    snprintf(t.status, sizeof(t.status), "Pronto");
    uint8_t dirty = 1u;
    for (;;) {
        if (dirty) {
            render(&t);
            dirty = 0u;
        }
        slui_event_t ev;
        uint8_t got = 0u;
        while (slui_poll_event(&ev)) {
            got = 1u;
            on_event(&t, &ev);
            dirty = 1u;
        }
        if (!got) peos_sleep(10u);
    }
}

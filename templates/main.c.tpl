// {{APP_NAME}}: a Starlight app. It sends what its window's body shows as
// SLUI commands (Starlight draws them, in the Soft UI style), lays it out
// for the size it is given (the window can be resized: SLUI_EVENT_RESIZE)
// and redraws only when something changed.

#include "peos/peos.h"
#include "peos/starlight_ui.h"

#include <stdio.h>

#define ID_COUNT 1u
#define ID_RESET 2u
#define ID_MODE  3u

typedef struct {
    uint32_t clicks;
    uint32_t mode;      // 0 or 1: the segmented control
} app_state_t;

static void render(const app_state_t *s) {
    uint32_t W = 0u, H = 0u;
    slui_viewport(&W, &H);   // the body's size, updated on every resize
    // 0 x 0 until Starlight says the size (the first SLUI_EVENT_RESIZE), and
    // never less than app.ini's minimum: the layout below subtracts from it.
    if (W < 400u) W = 400u;
    if (H < 300u) H = 300u;

    slui_begin_frame();
    // A title in a font of Starlight's skin, a segmented control on the right.
    slui_face("ui-28-light");
    slui_label_theme(0u, 10u, SLUI_COLOR_TEXT_PRIMARY, "{{APP_NAME}}");
    slui_segment(ID_MODE, W - 240u, 0u, 240u, 44u, 2u, s->mode, 1u, "Uno|Due");

    // A raised card filling the rest, a night panel inside it with a number.
    slui_surface(SLUI_SURF_RAISED, 0u, 64u, W, H - 64u, 26u);
    uint32_t pw = W - 48u, ph = H - 64u - 48u - 68u;
    slui_surface(SLUI_SURF_NIGHT, 24u, 88u, pw, ph, 22u);
    char count[16];
    snprintf(count, sizeof(count), "%u", (unsigned)s->clicks);
    slui_face("mono-64");
    slui_label_align(24u + pw / 2u, 88u + ph / 2u - 32u, SLUI_ALIGN_CENTER, count, 0x59u, 0xD0u, 0xFFu);
    slui_face("ui-14");
    slui_label_align(24u + pw / 2u, 88u + ph / 2u + 44u, SLUI_ALIGN_CENTER,
                     s->mode ? "modo Due" : "modo Uno", 0x9Fu, 0xB4u, 0xD8u);

    // The buttons: the group's main one is night (primary).
    slui_button(ID_RESET, W - 24u - 260u, H - 68u, 120u, 44u, "Azzera");
    slui_button_primary(ID_COUNT, W - 24u - 128u, H - 68u, 128u, 44u, 1u, "Conta");
    slui_end_frame();
}

int main(int argc, char **argv) {
    (void)argc;
    (void)argv;

    app_state_t state;
    peos_memset(&state, 0, sizeof(state));
    uint8_t dirty = 1u;

    for (;;) {
        if (dirty) {
            render(&state);
            dirty = 0u;
        }
        slui_event_t ev;
        uint8_t got = 0u;
        while (slui_poll_event(&ev)) {
            got = 1u;
            if (ev.type == SLUI_EVENT_BUTTON && ev.id == ID_COUNT) state.clicks++;
            else if (ev.type == SLUI_EVENT_BUTTON && ev.id == ID_RESET) state.clicks = 0u;
            else if (ev.type == SLUI_EVENT_SELECT && ev.id == ID_MODE) state.mode = ev.value;
            dirty = 1u;   // SLUI_EVENT_RESIZE included: lay out again
        }
        if (!got) peos_sleep(10u);
    }
}

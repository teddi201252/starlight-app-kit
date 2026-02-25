#include "peos/peos.h"
#include "peos/starlight_ui.h"

typedef struct {
    uint32_t clicks;
    uint32_t slider;
    uint8_t toggle_on;
    uint32_t selected;
    uint32_t scroll_y;
    char text[64];
    uint8_t dirty;
} app_state_t;

static void copy_str(char *dst, uint32_t cap, const char *src) {
    if (!dst || cap == 0u) return;
    if (!src) src = "";
    uint32_t i = 0u;
    while (i + 1u < cap && src[i]) {
        dst[i] = src[i];
        i++;
    }
    dst[i] = '\0';
}

static void render_ui(app_state_t *s) {
    if (!s) return;

    slui_begin_frame();
    slui_label(12u, 12u, "Demo Kit - Extended Widgets", 235u, 235u, 235u);
    slui_button(1u, 12u, 40u, 120u, 24u, "Click");
    slui_slider_u32(2u, 12u, 80u, 220u, 0u, 100u, s->slider);
    slui_toggle(3u, 12u, 112u, 220u, 24u, s->toggle_on, "WiFi");
    slui_select(4u, 12u, 144u, 220u, 24u, 3u, s->selected, "Italiano|English|Espanol");
    slui_text_input(5u, 12u, 176u, 220u, 24u, 48u, s->text);

    slui_scroll_view_t sv;
    slui_scroll_view_begin(&sv, 6u, 244u, 40u, 140u, 160u, 480u, &s->scroll_y);
    for (uint32_t i = 0u; i < 24u; i++) {
        uint32_t rx = 0u, ry = 0u, rw = 0u, rh = 0u;
        if (!slui_scroll_view_next(&sv, 14u, 4u, &rx, &ry, &rw, &rh)) continue;
        char row[24];
        row[0] = 'R'; row[1] = 'i'; row[2] = 'g'; row[3] = 'a'; row[4] = ' ';
        row[5] = (char)('0' + ((i / 10u) % 10u));
        row[6] = (char)('0' + (i % 10u));
        row[7] = '\0';
        (void)rw;
        (void)rh;
        slui_label(rx + 2u, ry + 3u, row, 190u, 200u, 220u);
    }
    slui_scroll_view_end(&sv);
    slui_end_frame();
}

int main(int argc, char **argv) {
    (void)argc;
    (void)argv;

    app_state_t state;
    peos_memset(&state, 0, sizeof(state));
    state.slider = 50u;
    state.toggle_on = 0u;
    state.selected = 0u;
    state.scroll_y = 0u;
    copy_str(state.text, (uint32_t)sizeof(state.text), "hostname");
    state.dirty = 1u;

    for (;;) {
        if (state.dirty) {
            render_ui(&state);
            state.dirty = 0u;
        }

        slui_event_t ev;
        if (slui_poll_event(&ev)) {
            if (ev.type == SLUI_EVENT_BUTTON && ev.id == 1u) {
                state.clicks++;
                (void)state.clicks;
                state.dirty = 1u;
            } else if (ev.type == SLUI_EVENT_SLIDER && ev.id == 2u) {
                state.slider = ev.value;
                state.dirty = 1u;
            } else if (ev.type == SLUI_EVENT_TOGGLE && ev.id == 3u) {
                state.toggle_on = ev.value ? 1u : 0u;
                state.dirty = 1u;
            } else if (ev.type == SLUI_EVENT_SELECT && ev.id == 4u) {
                state.selected = ev.value;
                state.dirty = 1u;
            } else if (ev.type == SLUI_EVENT_TEXT_INPUT && ev.id == 5u) {
                copy_str(state.text, (uint32_t)sizeof(state.text), ev.text);
                state.dirty = 1u;
            } else if (ev.type == SLUI_EVENT_SCROLL_V && ev.id == 6u) {
                state.scroll_y = ev.value;
                state.dirty = 1u;
            }
        } else {
            peos_sleep(1u);
        }
    }
}

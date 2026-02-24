#include "peos/peos.h"
#include "peos/starlight_ui.h"

typedef struct {
    uint32_t clicks;
    uint32_t slider;
    uint8_t dirty;
} app_state_t;

static void render_ui(const app_state_t *s) {
    if (!s) return;

    slui_begin_frame();
    slui_label(12u, 12u, "Demo Kit", 235u, 235u, 235u);
    slui_button(1u, 12u, 40u, 120u, 24u, "Click");
    slui_slider_u32(2u, 12u, 80u, 220u, 0u, 100u, s->slider);
    slui_end_frame();
}

int main(int argc, char **argv) {
    (void)argc;
    (void)argv;

    app_state_t state;
    peos_memset(&state, 0, sizeof(state));
    state.slider = 50u;
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
            }
        } else {
            peos_sleep(1u);
        }
    }
}

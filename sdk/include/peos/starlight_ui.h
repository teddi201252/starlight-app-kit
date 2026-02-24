#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    SLUI_EVENT_NONE = 0,
    SLUI_EVENT_BUTTON = 1,
    SLUI_EVENT_SLIDER = 2,
} slui_event_type_t;

typedef struct {
    slui_event_type_t type;
    uint32_t id;
    uint32_t value;
} slui_event_t;

void slui_begin_frame(void);
void slui_end_frame(void);

void slui_label(uint32_t x, uint32_t y, const char *text,
                uint8_t r, uint8_t g, uint8_t b);

void slui_button(uint32_t id,
                 uint32_t x, uint32_t y, uint32_t w, uint32_t h,
                 const char *text);

void slui_slider_u32(uint32_t id,
                     uint32_t x, uint32_t y, uint32_t w,
                     uint32_t min_v, uint32_t max_v, uint32_t value);

int slui_poll_event(slui_event_t *out);

#ifdef __cplusplus
}
#endif

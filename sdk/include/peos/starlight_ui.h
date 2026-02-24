#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    SLUI_EVENT_NONE = 0,
    SLUI_EVENT_BUTTON = 1,
    SLUI_EVENT_SLIDER = 2,
    SLUI_EVENT_HOST_ERROR = 3,
} slui_event_type_t;

#define SLUI_SDK_VERSION_MAJOR 1u
#define SLUI_SDK_VERSION_MINOR 0u
#define SLUI_SDK_VERSION_PATCH 0u
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

void slui_image_file(uint32_t x, uint32_t y, uint32_t w, uint32_t h,
                     const char *path);

int slui_poll_event(slui_event_t *out);
uint32_t slui_sdk_version_u32(void);
uint32_t slui_protocol_version(void);
uint32_t slui_host_protocol_version(void);
uint32_t slui_host_capabilities(void);
const char *slui_last_host_error(void);

#ifdef __cplusplus
}
#endif

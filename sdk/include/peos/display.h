/* The screen's modes (docs/system-apps-ux-plan.md, phase 4, Settings: the
 * resolution on every machine). SYS_FB_MODES tells:
 *  - the mode the system started in (the firmware's, as Limine set it);
 *  - whether a driver changes the mode for real while PeOS runs (virtio-gpu),
 *    and up to which size;
 *  - the modes Limine found (the firmware's): one of them is chosen for the
 *    next start with "resolution: WxH" in limine.conf.
 * A size smaller than the screen needs none of this: Starlight draws at
 * that size and enlarges it (/system/config/display.cfg, width= height=). */
#pragma once
#include <stdint.h>

#define PEOS_FB_MODES_MAX 48u
#define PEOS_FB_MODES_LIVE 0x1u   /* the driver changes the mode now (up to max_w x max_h) */

typedef struct {
    uint16_t w, h;
} peos_fb_mode_t;

typedef struct {
    uint32_t flags;
    uint32_t boot_w, boot_h;     /* the mode PeOS started in */
    uint32_t max_w, max_h;       /* PEOS_FB_MODES_LIVE: the biggest the driver takes */
    uint32_t count;              /* the firmware's modes, in modes[] */
    peos_fb_mode_t modes[PEOS_FB_MODES_MAX];
} peos_fb_modes_t;

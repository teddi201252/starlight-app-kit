// The laptop's power: the battery and the screen's light (release 0.17.0,
// phase 6). Shared by the kernel and the apps.
#pragma once

#include <stdint.h>

#define PEOS_BATTERY_PRESENT  0x1u   // there is a battery
#define PEOS_BATTERY_AC       0x2u   // the power adapter is in
#define PEOS_BATTERY_CHARGING 0x4u   // and charging it
#define PEOS_BATTERY_FULL     0x8u   // the adapter is in and it does not charge: full

typedef struct {
    uint32_t flags;          // PEOS_BATTERY_*
    uint32_t percent;        // 0..100
    int32_t current_ma;      // > 0 charging, < 0 discharging (0: unknown)
    uint32_t remaining_mah;  // 0: unknown
    uint32_t full_mah;
    uint32_t reserved[3];
} peos_battery_t;

#define PEOS_BACKLIGHT_GET 0u
#define PEOS_BACKLIGHT_SET 1u
#define PEOS_BACKLIGHT_MAX 1000u   // the light is in permille

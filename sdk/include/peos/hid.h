#pragma once
/* Virtual HID devices (docs/bluetooth-plan.md, phase 4;
 * docs/gamepad-plan.md, phases 3-4).
 *
 * A HID device the kernel cannot reach by itself (a Bluetooth LE
 * controller: HID over GATT, read by bluetoothd) is made in the kernel by
 * the program that talks to it: peos_hid_create() takes the device's report
 * descriptor (the HID "Report Map") with its USB-style vendor and product
 * IDs, and gives a descriptor. From then on the device is the kernel's like
 * a USB one: a game controller goes to the controllers' hub (peos/gamepad.h)
 * and programs see it there, with bus PEOS_GAMEPAD_BUS_BLUETOOTH.
 *
 * Capability "bluetooth" (bluetoothd's): whoever makes one can press its
 * buttons.
 *
 *  - write(): one input report per write: the report ID first (0 when the
 *    descriptor has no report IDs), then the report's bytes as the device
 *    sent them (at most PEOS_HID_REPORT_MAX). Returns the bytes taken, 0
 *    for a report the descriptor does not have.
 *  - read(): whole peos_hid_output_t records: the reports the kernel wants
 *    sent to the device (the vibration of a controller), oldest first. Wait
 *    with PEOS_WAIT_READ; nothing wakes it otherwise. A reader that does
 *    not keep up loses the oldest (PEOS_HID_OUT_QUEUE of them wait).
 *  - close(): the device is gone (the controller disconnects).
 *
 * -1 from peos_hid_create(): no right, a descriptor too long or one the
 * kernel has no use for (no game controller in it, for now: keyboards and
 * mice over Bluetooth come later), or no room (PEOS_HID_MAX devices).
 */
#include <stdint.h>

#define PEOS_HID_MAX 8u
#define PEOS_HID_DESC_MAX 2048u     /* report descriptor bytes */
#define PEOS_HID_REPORT_MAX 128u    /* an input report's bytes (report ID not counted) */
#define PEOS_HID_OUT_DATA 60u
#define PEOS_HID_OUT_QUEUE 16u

/* How the device is reached (the controllers' PEOS_GAMEPAD_BUS_* values) */
#define PEOS_HID_BUS_USB 1u
#define PEOS_HID_BUS_BLUETOOTH 2u

typedef struct peos_hid_create_info {
    uint16_t vendor;           /* USB vendor and product IDs (Bluetooth: the PnP ID) */
    uint16_t product;
    uint16_t version;
    uint8_t bus;               /* PEOS_HID_BUS_* */
    uint8_t battery;           /* 0..100, 255 = not known */
    uint32_t desc_len;         /* bytes at desc, at most PEOS_HID_DESC_MAX */
    uint32_t reserved;
    uint64_t desc;             /* the report descriptor (a pointer) */
    char name[32];             /* NUL-terminated (the device's name) */
} peos_hid_create_t;           /* 56 bytes */

/* peos_hid_output_t.type */
#define PEOS_HID_OUTPUT 2u     /* an output report (HID's report type 2) */
#define PEOS_HID_FEATURE 3u

typedef struct {
    uint8_t type;              /* PEOS_HID_OUTPUT or PEOS_HID_FEATURE */
    uint8_t report_id;         /* 0 when the descriptor has none */
    uint16_t len;              /* bytes in data, the report ID not included */
    uint8_t data[PEOS_HID_OUT_DATA];
} peos_hid_output_t;           /* 64 bytes */

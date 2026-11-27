#pragma once
/* Game controllers (docs/gamepad-plan.md, phase 1).
 *
 * peos_gamepad_open() (capability "gamepad") gives a descriptor that reads
 * whole peos_gamepad_record_t records and is waited on with PEOS_WAIT_READ.
 * Like the input endpoint, records go only to the most recently opened
 * descriptor that is still open: a program underneath sees nothing until
 * the one above closes.
 *
 * Right after opening, the descriptor holds a CONNECT and a STATE record for
 * every controller already there; then a record each time one comes, goes or
 * changes. A STATE record carries the whole state, so a reader that missed
 * records (queue full) is right again at the next one. Consecutive unread
 * STATE records of a controller whose buttons did not change are merged
 * (the sticks moved): a press is never lost that way.
 *
 * Every controller arrives in the same "standard" layout (the buttons and
 * axes of today's controllers, as SDL's gamepad and the web's Gamepad API).
 * A controller the kernel cannot interpret would come with
 * PEOS_GAMEPAD_KIND_RAW: buttons and axes as they are, the mapping is up to
 * the library (none such yet).
 *
 * Writing a peos_gamepad_cmd_t to the descriptor sends a command: the
 * vibration, for at most `duration_ms` (capped at
 * PEOS_GAMEPAD_RUMBLE_MAX_MS). When the descriptor that made a controller
 * vibrate is closed, the vibration stops.
 */
#include <stdint.h>

#define PEOS_GAMEPAD_MAX 8u            /* controllers at once: slots 0..7 */

/* Record types */
#define PEOS_GAMEPAD_CONNECT    1u     /* info: a controller came (or was there) */
#define PEOS_GAMEPAD_DISCONNECT 2u     /* the controller in this slot went */
#define PEOS_GAMEPAD_STATE      3u     /* state: buttons and axes now */

/* Kinds */
#define PEOS_GAMEPAD_KIND_STANDARD 0u
#define PEOS_GAMEPAD_KIND_RAW      1u

/* How it is attached */
#define PEOS_GAMEPAD_BUS_VIRTUAL   0u  /* the kernel's pretend controller (test images) */
#define PEOS_GAMEPAD_BUS_USB       1u
#define PEOS_GAMEPAD_BUS_BLUETOOTH 2u

/* info.flags */
#define PEOS_GAMEPAD_HAS_RUMBLE         0x01u   /* the two big motors */
#define PEOS_GAMEPAD_HAS_TRIGGER_RUMBLE 0x02u   /* one motor in each trigger (Xbox) */
#define PEOS_GAMEPAD_HAS_BATTERY        0x04u

/* Standard buttons (state.buttons) */
#define PEOS_GAMEPAD_BTN_A          (1u << 0)   /* south */
#define PEOS_GAMEPAD_BTN_B          (1u << 1)   /* east */
#define PEOS_GAMEPAD_BTN_X          (1u << 2)   /* west */
#define PEOS_GAMEPAD_BTN_Y          (1u << 3)   /* north */
#define PEOS_GAMEPAD_BTN_LB         (1u << 4)
#define PEOS_GAMEPAD_BTN_RB         (1u << 5)
#define PEOS_GAMEPAD_BTN_VIEW       (1u << 6)   /* back, select */
#define PEOS_GAMEPAD_BTN_MENU       (1u << 7)   /* start */
#define PEOS_GAMEPAD_BTN_GUIDE      (1u << 8)   /* the Xbox button */
#define PEOS_GAMEPAD_BTN_L3         (1u << 9)   /* left stick pressed */
#define PEOS_GAMEPAD_BTN_R3         (1u << 10)
#define PEOS_GAMEPAD_BTN_UP         (1u << 11)
#define PEOS_GAMEPAD_BTN_DOWN       (1u << 12)
#define PEOS_GAMEPAD_BTN_LEFT       (1u << 13)
#define PEOS_GAMEPAD_BTN_RIGHT      (1u << 14)
#define PEOS_GAMEPAD_BTN_SHARE      (1u << 15)  /* Xbox Series "Share", the PlayStation touchpad */
#define PEOS_GAMEPAD_BTN_ALL        0xFFFFu

/* Standard axes */
#define PEOS_GAMEPAD_AXIS_LX 0u    /* -32768 left .. 32767 right */
#define PEOS_GAMEPAD_AXIS_LY 1u    /* -32768 up .. 32767 down */
#define PEOS_GAMEPAD_AXIS_RX 2u
#define PEOS_GAMEPAD_AXIS_RY 3u
#define PEOS_GAMEPAD_STICKS  4u
#define PEOS_GAMEPAD_TRIGGER_L 0u  /* 0 released .. 65535 pressed all the way */
#define PEOS_GAMEPAD_TRIGGER_R 1u

typedef struct {
    uint32_t buttons;          /* PEOS_GAMEPAD_BTN_* (raw: button n is bit n) */
    int16_t sticks[PEOS_GAMEPAD_STICKS];
    uint16_t triggers[2];
} peos_gamepad_state_t;        /* 16 bytes */

typedef struct {
    uint16_t vendor;           /* USB vendor and product IDs (0 for the pretend one) */
    uint16_t product;
    uint8_t bus;               /* PEOS_GAMEPAD_BUS_* */
    uint8_t flags;             /* PEOS_GAMEPAD_HAS_* */
    uint8_t battery;           /* 0..100, 255 = not known */
    uint8_t reserved;
    char name[32];             /* NUL-terminated */
} peos_gamepad_info_t;         /* 40 bytes */

typedef struct {
    uint16_t type;             /* PEOS_GAMEPAD_CONNECT, _DISCONNECT, _STATE */
    uint8_t slot;              /* 0..PEOS_GAMEPAD_MAX-1 */
    uint8_t kind;              /* PEOS_GAMEPAD_KIND_* */
    uint32_t seq;              /* per slot: grows with every STATE the driver gave */
    uint64_t time_ns;          /* when the driver saw it (same clock as peos_input_record_t) */
    union {
        peos_gamepad_state_t state;
        peos_gamepad_info_t info;
    } u;
} peos_gamepad_record_t;       /* 56 bytes */

/* Commands, written to the descriptor */
#define PEOS_GAMEPAD_CMD_RUMBLE 1u
#define PEOS_GAMEPAD_RUMBLE_MAX_MS 5000u

typedef struct {
    uint16_t type;             /* PEOS_GAMEPAD_CMD_* */
    uint8_t slot;
    uint8_t reserved;
    uint16_t low;              /* the big (low frequency) motor, 0..65535 */
    uint16_t high;             /* the small (high frequency) motor */
    uint16_t left_trigger;     /* the trigger motors, where there are */
    uint16_t right_trigger;
    uint32_t duration_ms;      /* 0 stops it */
} peos_gamepad_cmd_t;          /* 16 bytes */

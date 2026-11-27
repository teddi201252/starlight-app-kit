#pragma once
#ifdef __cplusplus
extern "C" {
#endif
/* bluetoothd (docs/bluetooth-plan.md, phase 2): the Bluetooth stack's
 * service. Started by init with the system; it owns the adapter (the
 * kernel's HCI descriptor, peos/bluetooth.h) and sleeps on descriptors only:
 * the adapter watch, the adapter, its service name and its clients' channels.
 *
 * Clients (Settings, tools) connect to the service name "bluetooth"; the
 * kernel lets only a program with the "bluetooth" capability connect. A
 * connection stays open as long as the client wants: it sends btd_req_t
 * requests (each with its own seq) and reads btd_msg_t messages, fixed
 * size: the answer to each request (kind BTD_MSG_REPLY, the same op and
 * seq), and events pushed to every connected client whenever something
 * changes (kind BTD_MSG_EVENT, op one of BTD_EV_*). Nobody needs to ask
 * again and again: wait for the channel with PEOS_WAIT_READ. A client that
 * reads too slowly loses events (its queue holds 16 messages); LIST and
 * STATUS tell the truth again.
 *
 * Without an adapter bluetoothd answers STATUS with present = 0 and sends
 * BTD_EV_ADAPTER_ADDED when one comes (Settings shows its tab only then).
 *
 * Power: on (the default) or off, remembered across reboots in
 * BTD_POWER_PATH ("power=0" or "power=1"). Off: the controller is reset and
 * left alone (its address and name stay known).
 *
 * The LE search (SCAN_START) is active, lasts arg seconds (0: 10 s, at most
 * BTD_SCAN_MAX_S) and stops by itself (BTD_EV_SCAN_STOPPED); only then does
 * the radio listen. The devices seen stay in a list of BTD_DEVICES_MAX
 * (the oldest replaced); entries not seen for BTD_DEVICE_FORGET_S are
 * dropped when the next search starts.
 *
 * RELEASE is for diagnostic tools that talk HCI themselves (btcheck): while
 * the asking client's channel stays open bluetoothd closes the adapter and
 * reports it removed; when that channel closes, it takes the adapter back.
 */
#include <stdint.h>

#define BTD_SERVICE_NAME "bluetooth"
#define BTD_CONFIG_DIR "/system/config/bluetooth"
#define BTD_POWER_PATH "/system/config/bluetooth/power.cfg"

#define BTD_SCAN_DEFAULT_S 10u
#define BTD_SCAN_MAX_S 30u
#define BTD_DEVICES_MAX 32u
#define BTD_DEVICE_FORGET_S 300u
#define BTD_LIST_PAGE 24u       /* devices in one LIST answer */
#define BTD_NAME_MAX 44u        /* device names, NUL-terminated (longer ones cut) */
#define BTD_CLIENTS_MAX 8u

/* Requests (btd_req_t.op) */
enum {
    BTD_OP_STATUS = 1,       /* -> u.status */
    BTD_OP_POWER = 2,        /* arg 1 on, 0 off; answers once done; -> u.status */
    BTD_OP_SCAN_START = 3,   /* arg: seconds (0: BTD_SCAN_DEFAULT_S, capped at BTD_SCAN_MAX_S) */
    BTD_OP_SCAN_STOP = 4,
    BTD_OP_LIST = 5,         /* arg: the first device; -> count devices in u.devices, total */
    BTD_OP_PAIR = 6,         /* addr, addr_type: BTD_ERR_NOT_YET (phase 3) */
    BTD_OP_FORGET = 7,       /* addr, addr_type: BTD_ERR_NOT_YET (phase 3) */
    BTD_OP_STATS = 8,        /* -> u.stats */
    BTD_OP_RELEASE = 9,      /* let go of the adapter while this client stays connected */
};

/* Events (btd_msg_t.op when kind is BTD_MSG_EVENT) */
enum {
    BTD_EV_ADAPTER_ADDED = 1,     /* u.status: the adapter, ready (identity read) */
    BTD_EV_ADAPTER_REMOVED = 2,   /* u.status: present 0 */
    BTD_EV_POWER_CHANGED = 3,     /* u.status */
    BTD_EV_SCAN_STARTED = 4,      /* u.status (scan_left_ms) */
    BTD_EV_SCAN_STOPPED = 5,      /* u.status; total: the devices in the list */
    BTD_EV_DEVICE_FOUND = 6,      /* u.device: first seen in this search */
    BTD_EV_DEVICE_UPDATED = 7,    /* u.device: its name or appearance changed, or its signal (at most every 2 s) */
};

/* btd_msg_t.status */
enum {
    BTD_OK = 0,
    BTD_ERR = -1,             /* a bad request */
    BTD_ERR_NO_ADAPTER = -2,
    BTD_ERR_OFF = -3,         /* the radio is off */
    BTD_ERR_BUSY = -4,        /* another command is under way: ask again */
    BTD_ERR_HCI = -5,         /* the controller refused or did not answer */
    BTD_ERR_NOT_YET = -6,     /* defined, not done yet (pairing: phase 3) */
};

#define BTD_MSG_REPLY 1u
#define BTD_MSG_EVENT 2u

/* Address types (btd_device_t.addr_type), as HCI has them */
#define BTD_ADDR_PUBLIC 0u
#define BTD_ADDR_RANDOM 1u
#define BTD_ADDR_PUBLIC_ID 2u
#define BTD_ADDR_RANDOM_ID 3u
#define BTD_ADDR_ANONYMOUS 0xFFu

/* btd_device_t.flags */
#define BTD_DEV_NAME_COMPLETE 0x1u   /* the name came whole (AD type 0x09), not shortened (0x08) */
#define BTD_DEV_CONNECTABLE 0x2u
#define BTD_DEV_HAS_APPEARANCE 0x4u

typedef struct {
    uint32_t op;           /* BTD_OP_* */
    uint32_t seq;          /* copied into the answer */
    uint32_t arg;          /* POWER: 0/1; SCAN_START: seconds; LIST: the first index */
    uint8_t addr[6];       /* PAIR, FORGET: the address, as HCI sends it (least significant byte first) */
    uint8_t addr_type;     /* BTD_ADDR_* */
    uint8_t reserved[5];
} btd_req_t;               /* 24 bytes */

typedef struct {
    uint8_t addr[6];       /* least significant byte first (C0:FF:EE:00:00:02 is 02 00 00 EE FF C0) */
    uint8_t addr_type;     /* BTD_ADDR_* */
    uint8_t flags;         /* BTD_DEV_* */
    int8_t rssi;           /* dBm, the last report (127: unknown) */
    int8_t rssi_best;      /* dBm, the strongest seen */
    uint16_t appearance;   /* GAP appearance (0x03C4 a gamepad), 0 if none */
    uint32_t age_ms;       /* since it was last seen */
    uint32_t reports;      /* advertising reports received from it */
    char name[BTD_NAME_MAX];
} btd_device_t;            /* 64 bytes */

typedef struct {
    uint8_t present;       /* an adapter is there and bluetoothd has it */
    uint8_t ready;         /* its identity is read */
    uint8_t powered;       /* on (it can search) */
    uint8_t scanning;
    uint8_t addr[6];       /* the adapter's address, least significant byte first */
    uint8_t index;         /* the kernel's adapter index (peos_bt_open) */
    uint8_t transport;     /* PEOS_BT_TRANSPORT_* */
    uint8_t hci_version;   /* 9: Bluetooth 5.0... */
    uint8_t lmp_version;
    uint16_t hci_revision;
    uint16_t manufacturer;
    uint16_t lmp_subversion;
    uint16_t le_acl_len;   /* LE ACL buffers: bytes each */
    uint8_t le_acl_count;  /* and how many */
    uint8_t reserved;
    uint16_t acl_len;      /* BR/EDR ACL buffers (0 if none) */
    uint16_t acl_count;
    uint32_t scan_left_ms; /* while scanning */
    uint64_t le_features;  /* LE_Read_Local_Supported_Features */
    char adapter[40];      /* the kernel's name for it ("COM2", "LPSS UART 00:1e.0", a USB product) */
    char name[64];         /* the controller's local name, NUL-terminated */
} btd_status_t;            /* 144 bytes */

typedef struct {
    uint32_t wakeups;          /* times bluetoothd's wait returned (its whole life) */
    uint32_t hci_events;       /* HCI events read */
    uint32_t adv_reports;      /* advertising reports parsed */
    uint32_t events_sent;      /* events queued to clients */
    uint32_t events_dropped;   /* events a client's full queue did not take */
    uint32_t clients;          /* connected now */
    uint32_t devices;          /* in the list */
    uint32_t scans;            /* searches started */
} btd_stats_t;

typedef struct {
    uint32_t kind;         /* BTD_MSG_REPLY or BTD_MSG_EVENT */
    uint32_t op;           /* reply: the request's op; event: BTD_EV_* */
    uint32_t seq;          /* reply: the request's seq; event: 0 */
    int32_t status;        /* reply: BTD_OK or BTD_ERR_*; event: 0 */
    uint32_t count;        /* LIST: devices in u.devices */
    uint32_t total;        /* LIST, SCAN_STOPPED: devices in the list */
    union {
        btd_status_t status;
        btd_stats_t stats;
        btd_device_t device;
        btd_device_t devices[BTD_LIST_PAGE];
    } u;
} btd_msg_t;               /* 24 + 1536 bytes: one channel message */

/* Client (libpeos/btdclient.c). */
typedef void (*btd_event_fn)(const btd_msg_t *ev, void *ctx);
/* A connection to bluetoothd: a channel descriptor, or -1 (no bluetoothd,
 * or no "bluetooth" capability). Close it with peos_fclose. */
int64_t btd_connect(void);
/* Sends a request (seq filled in) and waits up to timeout_ms for its answer
 * in *reply; events that come meanwhile go to on_event (may be NULL: then
 * they are dropped). Returns the answer's status, or BTD_ERR when nothing
 * came (or the connection is gone). */
int btd_call(int64_t ch, btd_req_t *req, btd_msg_t *reply, uint64_t timeout_ms, btd_event_fn on_event, void *ctx);
/* One message without waiting: 1 read into *out, 0 none, -1 the
 * connection is gone. */
int btd_read(int64_t ch, btd_msg_t *out);
/* Formats an address (least significant byte first) as "C0:FF:EE:00:00:01"
 * (18 bytes with the NUL). */
void btd_addr_str(const uint8_t addr[6], char out[18]);
#ifdef __cplusplus
}
#endif

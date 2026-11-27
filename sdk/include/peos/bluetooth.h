#pragma once
/* Bluetooth adapters: the HCI transport (docs/bluetooth-plan.md, phase 1).
 *
 * The kernel only carries HCI packets between a program and the
 * controller chip (on UART or USB); the stack (L2CAP, SMP, GATT...) lives
 * in a service, bluetoothd. Everything here needs the capability
 * "bluetooth".
 *
 * peos_bt_open(index) opens adapter `index` (0..PEOS_BT_MAX-1): a
 * descriptor that carries whole HCI packets in H4 form, the packet type
 * byte first (PEOS_BT_H4_*), then the packet as the Core specification
 * writes it (command: opcode, length, parameters; ACL: handle and flags,
 * length, data; event: code, length, parameters). Only one descriptor per
 * adapter at a time: a second peos_bt_open of the same adapter fails until
 * the first one is closed. Packets that arrive while nobody has the adapter
 * open are thrown away (an opener starts from an empty queue).
 *
 *  - read() returns whole packets only: as many as fit in the buffer, one
 *    after the other, each with its type byte and its own length fields to
 *    find the next one. A buffer smaller than the next packet gets 0 bytes
 *    (a buffer of PEOS_BT_PACKET_MAX always takes one). It blocks until a
 *    packet comes unless the descriptor is non-blocking; wait with
 *    PEOS_WAIT_READ. Events and ACL data come up; SCO and ISO packets are
 *    not carried (thrown away).
 *  - write() takes whole packets: commands (PEOS_BT_H4_CMD) and ACL data
 *    (PEOS_BT_H4_ACL), several in one write if they all fit. It returns the
 *    bytes of the whole packets sent; a packet cut by the end of the buffer
 *    is not sent (and not counted), and 0 means the first packet is wrong
 *    (another type, a length field that does not match) or the adapter is
 *    gone. A write may wait for room on the wire (not when non-blocking).
 *  - When the adapter goes away (a USB dongle pulled out) the descriptor
 *    becomes readable: what was queued is read, then reads return 0. The
 *    descriptor stays useless; close it and watch for the next adapter.
 *  - Queues: up to PEOS_BT_RX_QUEUE bytes of packets wait for the reader;
 *    a packet that finds no room is dropped (the newest), counted in the
 *    info record's rx_dropped.
 *
 * peos_bt_open(PEOS_BT_WATCH) opens a watch descriptor (any number of them,
 * any program with "bluetooth"): read() returns whole peos_bt_info_t
 * records, first a PEOS_BT_ADDED for every adapter there now, then one each
 * time an adapter comes (ADDED) or goes (REMOVED). If the reader is so slow
 * that its queue fills, the queue is replaced by a PEOS_BT_RESET record and
 * the ADDED records of the adapters there at that moment: forget the list
 * and start again. Wait with PEOS_WAIT_READ; nothing wakes it otherwise.
 */
#include <stdint.h>

#define PEOS_BT_MAX   4u                /* adapters at once: indexes 0..3 */
#define PEOS_BT_WATCH 0xFFu             /* peos_bt_open(): the watch descriptor */

/* H4 packet types (the first byte of every packet) */
#define PEOS_BT_H4_CMD 0x01u            /* host to controller only */
#define PEOS_BT_H4_ACL 0x02u
#define PEOS_BT_H4_SCO 0x03u            /* not carried */
#define PEOS_BT_H4_EVT 0x04u            /* controller to host only */
#define PEOS_BT_H4_ISO 0x05u            /* not carried */

/* Sizes */
#define PEOS_BT_ACL_DATA_MAX 1024u      /* the biggest ACL data carried (bytes after the 4-byte header) */
#define PEOS_BT_PACKET_MAX   (1u + 4u + PEOS_BT_ACL_DATA_MAX)   /* type + ACL header + data: 1029 */
#define PEOS_BT_RX_QUEUE     16384u     /* bytes of packets waiting for the reader */

/* How the adapter is attached */
#define PEOS_BT_TRANSPORT_VIRTUAL 0u
#define PEOS_BT_TRANSPORT_UART    1u    /* H4 on a serial port (QEMU's COM port, the MacBook's LPSS UART) */
#define PEOS_BT_TRANSPORT_USB     2u    /* a USB device of class 0xE0/0x01/0x01 */

/* Watch record events */
#define PEOS_BT_ADDED   1u              /* an adapter came (or was there when the watch was opened) */
#define PEOS_BT_REMOVED 2u              /* the adapter in this index went */
#define PEOS_BT_RESET   3u              /* the queue overflowed: forget the list, ADDED records follow */

/* info.flags */
#define PEOS_BT_IN_USE 0x1u             /* someone had it open when the record was made */

typedef struct peos_bt_info {
    uint16_t event;        /* watch records: PEOS_BT_ADDED, REMOVED or RESET */
    uint8_t index;         /* 0..PEOS_BT_MAX-1 (the argument of peos_bt_open) */
    uint8_t transport;     /* PEOS_BT_TRANSPORT_* */
    uint16_t vendor;       /* USB vendor:product; for a UART, the PCI IDs of the UART (0 for a COM port) */
    uint16_t product;
    uint32_t generation;   /* grows every time an adapter registers: a new adapter in an old index differs */
    uint32_t flags;        /* PEOS_BT_IN_USE */
    uint32_t rx_dropped;   /* packets dropped so far because the reader's queue was full */
    uint32_t reserved;
    char name[40];         /* "COM2", "LPSS UART 00:1e.0", the USB product string... NUL-terminated */
} peos_bt_info_t;          /* 64 bytes */

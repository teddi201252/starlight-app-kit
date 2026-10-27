#pragma once

#include <stdint.h>

/* Event model ABI (Milestone 2): rights on file descriptors, waiting on sets
 * of descriptors, timer and process descriptors, explicit inheritance.
 * Shared by the kernel (kernel/abi/event_abi.h) and userland. */

/* Rights of one descriptor. They can only be removed (peos_fd_restrict) and a
 * descriptor handed to a child never has more rights than the parent's. */
#define PEOS_FD_RD        0x01u   /* read */
#define PEOS_FD_WR        0x02u   /* write; for timers: arm/disarm */
#define PEOS_FD_WAIT      0x04u   /* wait for it to become ready */
#define PEOS_FD_MAP       0x08u   /* map it (shared memory, later phase) */
#define PEOS_FD_TRANSFER  0x10u   /* send it to another process (later phase) */
#define PEOS_FD_RIGHTS_ALL 0x1Fu

/* What to wait for, and what was ready. */
#define PEOS_WAIT_READ   0x1u     /* data to read, end of stream, timer expired, process exited */
#define PEOS_WAIT_WRITE  0x2u     /* space to write, or the reader went away */
#define PEOS_WAIT_ERROR  0x8u     /* revents only: the descriptor cannot be waited on */

#define PEOS_WAIT_MAX_ITEMS 64u
#define PEOS_WAIT_FOREVER   (~0ull)

typedef struct {
    int32_t fd;
    uint16_t events;     /* requested PEOS_WAIT_* */
    uint16_t revents;    /* filled by the kernel */
} peos_wait_item_t;

/* One descriptor to hand to the next spawned child, besides 0/1/2. */
#define PEOS_SPAWN_MAX_FDS 8u
typedef struct {
    int32_t parent_fd;
    int32_t child_fd;    /* must be >= 3: 0/1/2 are passed as spawn arguments */
    uint32_t rights;     /* rights for the child, a subset of the parent's */
} peos_spawn_fd_t;

/* Channels: a pair of connected descriptors carrying bounded messages of up to
 * PEOS_CHANNEL_MSG_MAX bytes and up to PEOS_CHANNEL_FDS_MAX descriptors. A
 * descriptor can be attached only if the sender holds PEOS_FD_TRANSFER on it;
 * the receiver gets the rights written in the message, never more than the
 * sender had. The sender keeps its own copy. */
/* 2048: a whole UDP datagram with its header, or a good piece of a TCP
 * stream, fits in one message (netd, milestone 4). */
#define PEOS_CHANNEL_MSG_MAX 2048u
#define PEOS_CHANNEL_FDS_MAX 4u
#define PEOS_CHANNEL_NONBLOCK 0x1u

/* Returned (as int64) by send/recv besides a byte count / 0 / -1. */
#define PEOS_CHANNEL_AGAIN  (-2)   /* nonblocking and the queue is full / empty */
#define PEOS_CHANNEL_CLOSED (-3)   /* the other end is closed (and, for recv, nothing is left) */

typedef struct {
    int32_t fd;          /* send: descriptor to attach. recv: the new descriptor */
    uint32_t rights;     /* send: rights to give. recv: rights it came with */
} peos_channel_fd_t;

typedef struct {
    void *data;          /* send: bytes to send. recv: buffer (>= PEOS_CHANNEL_MSG_MAX is always enough) */
    uint32_t len;        /* send: length. recv: in = capacity, out = bytes received */
    uint32_t nfds;       /* send: attached count. recv: out */
    peos_channel_fd_t fds[PEOS_CHANNEL_FDS_MAX];
} peos_channel_msg_t;

/* Input endpoint (peos_input_open): read() returns whole records. Events go
 * only to the most recently opened endpoint that is still open, so a program
 * in the background cannot read the keyboard. Consecutive unread mouse
 * motions are merged into one record. */
#define PEOS_INPUT_KEY   1u
#define PEOS_INPUT_MOUSE 2u

typedef struct {
    uint16_t type;       /* PEOS_INPUT_KEY or PEOS_INPUT_MOUSE */
    uint16_t code;       /* key: keycode, same codes as peos_input_event_t */
    uint8_t pressed;     /* key: 1 down, 0 up */
    uint8_t buttons;     /* mouse: buttons held after this event */
    int8_t wheel_x;      /* mouse */
    int8_t wheel_y;
    int32_t dx;          /* mouse: relative motion */
    int32_t dy;
    uint64_t time_ns;    /* when the driver saw it (same clock as peos_input_event_t) */
} peos_input_record_t;

/* UDP datagrams on a netd connection (peos/net.h): every message is one
 * datagram, prefixed by this header (destination when sending, source when
 * receiving). */
typedef struct {
    uint8_t ip[4];
    uint16_t port;
    uint16_t len;        /* payload bytes after the header (set on read) */
} peos_udp_hdr_t;

/* Network card (peos_nic_open, needs the "nic" capability): every read is one
 * whole Ethernet frame (destination MAC first, no FCS), every write sends one.
 * Exclusive: while the descriptor is open, the kernel's own network stack
 * receives nothing. Wait for READ like any descriptor. */
#define PEOS_NIC_FRAME_MAX 1536u

typedef struct {
    uint8_t mac[6];
    uint8_t link_up;     /* 1 when the cable (or Wi-Fi association) is up */
    uint8_t media;       /* 1 Ethernet, 2 Wi-Fi, 0 unknown */
    uint16_t mtu;        /* largest IP packet: 1500 */
    uint16_t reserved;
    char name[16];       /* driver name, NUL-terminated */
} peos_nic_info_t;

/* Services (peos_svc_register / peos_svc_connect): a name book of channels.
 * The service registers a name and gets a listening descriptor. Every
 * connect creates a new channel: the client gets one end, and the service
 * receives the other on its listening descriptor, as a message carrying a
 * peos_svc_connect_t and one descriptor. When the service exits the name is
 * free again and its clients see their channels closed. Names: 1-31 of
 * a-z 0-9 . _ - */
#define PEOS_SVC_NAME_MAX 32u
#define PEOS_SVC_CONNECT_MAGIC 0x31435653u   /* "SVC1" */

typedef struct {
    uint32_t magic;          /* PEOS_SVC_CONNECT_MAGIC */
    uint32_t client_pid;
    uint64_t client_caps;    /* the client's capabilities when it connected */
} peos_svc_connect_t;

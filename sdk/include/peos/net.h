#pragma once
#ifdef __cplusplus
extern "C" {
#endif
/* The network as a service (milestone 4): netd implements Ethernet, ARP,
 * IPv4, ICMP, UDP, TCP, DHCP and DNS in userland; programs talk to it over
 * channels, through the service registry name "net" (needs "network").
 *
 * Every request uses its own connection to "net": one net_req_t, one
 * net_rep_t back (after the work is done, e.g. when a TCP connection is
 * established). UDP_OPEN and TCP_CONNECT answer with one more descriptor:
 * the connection, a channel of its own.
 *   UDP connection: every message is one datagram, prefixed by
 *     peos_udp_hdr_t (destination when sending, source when receiving).
 *   TCP connection: messages are pieces of the stream. An empty message from
 *     netd is the end of the stream; the channel closing without it is an
 *     error (reset, timeout). Closing it closes the connection.
 * TCP_LISTEN (docs/pegasus-devtools-plan.md, phase 1; needs "net.listen")
 * answers with a listener, a channel of its own too: every connection that
 * finishes its handshake on that port comes on it as one net_tcp_accepted_t
 * message with the connection attached (the same kind of channel as
 * TCP_CONNECT's). NET_OP_TCP_UNLISTEN goes on the listener itself; netd
 * then closes its end. Closing the listener (or the process ending) stops
 * the listening too; connections already accepted stay.
 * The functions below wrap all of it. */

#include <stddef.h>
#include <stdint.h>
#include "peos/event_abi.h"
#include "peos/peos.h"

#define NET_SERVICE_NAME "net"

enum {
    NET_OP_UDP_OPEN = 1,     /* port (0 = any) -> connection */
    NET_OP_TCP_CONNECT = 2,  /* ip, port, timeout_ms -> connection */
    NET_OP_RESOLVE = 3,      /* name, timeout_ms -> ip */
    NET_OP_CONFIG_GET = 4,   /* -> cfg */
    NET_OP_CONFIG_SET = 5,   /* cfg (static address; stops DHCP) */
    NET_OP_DHCP = 6,         /* timeout_ms: get an address now -> cfg */
    NET_OP_PING = 7,         /* ip, timeout_ms -> value = round trip in ns */
    NET_OP_STATS = 8,        /* -> stats */
    NET_OP_CRASH = 9,        /* tests (needs "debug"): netd faults on purpose */
    NET_OP_WAIT_ADDRESS = 10,/* timeout_ms: reply once there is an address (now, if there is) -> cfg */
    NET_OP_HOSTNAME = 11,    /* name: the computer's name, in DHCP (option 12); a lease again now */
    /* More cards at once (docs/multi-netif-plan.md, SLUI SDK 1.0.28). The
     * other operations are about the interface in use (the default route). */
    NET_OP_IF_GET = 12,      /* port = index -> net_if_rep_t (not net_rep_t); NET_ERR past the last */
    NET_OP_IF_CONFIG_SET = 13,/* port = index, reserved = 1: DHCP, 0: cfg by hand */
    NET_OP_TCP_LISTEN = 14,  /* port (needs "net.listen") -> listener */
    NET_OP_TCP_UNLISTEN = 15,/* sent on the listener, not on "net": stop listening */
};

/* On a listener: a connection came, attached as fds[0]. */
typedef struct {
    uint32_t op;             /* NET_OP_TCP_LISTEN */
    uint8_t ip[4];           /* the peer */
    uint16_t port;           /* the peer's port */
    uint16_t local_port;     /* the port listened on */
} net_tcp_accepted_t;

/* A network interface: one card, its link and its address. */
typedef struct {
    uint32_t index;          /* 0, 1...: the card's (peos_nic_open) */
    uint8_t media;           /* 1 cable, 2 Wi-Fi, 0 unknown (peos_nic_info_t) */
    uint8_t link_up;
    uint8_t in_use;          /* the default route goes out of it */
    uint8_t dhcp;            /* 1: the address by DHCP; 0: by hand */
    char name[16];           /* the driver: "e1000", "rtl8169", "bcm4364" */
    peos_netcfg_t cfg;       /* ip (0.0.0.0: none yet), netmask, gateway, dns, mac */
} net_iface_t;

typedef struct {
    uint32_t op;
    int32_t status;
    net_iface_t iface;
} net_if_rep_t;

enum {
    NET_OK = 0,
    NET_ERR = -1,            /* bad request */
    NET_ERR_TIMEOUT = -2,
    NET_ERR_REFUSED = -3,    /* TCP reset, port in use (also a port already listened on) */
    NET_ERR_NO_ADDRESS = -4, /* no IPv4 address yet (DHCP not done) */
    NET_ERR_UNREACHABLE = -5,/* ARP gave no answer */
    NET_ERR_NOT_FOUND = -6,  /* DNS: no such name */
    NET_ERR_BUSY = -7,       /* netd has no room for another connection */
    NET_ERR_DENIED = -8,     /* a capability is missing ("debug", "net.listen") */
    NET_ERR_AGAIN = -9,      /* net_tcp_accept: no connection waiting now */
};

#define NET_NAME_MAX 128u

typedef struct {
    uint32_t op;
    uint32_t timeout_ms;
    uint8_t ip[4];
    uint16_t port;
    uint16_t reserved;
    char name[NET_NAME_MAX];
    peos_netcfg_t cfg;
} net_req_t;

typedef struct {
    uint64_t frames_rx, frames_tx;
    uint64_t udp_rx, udp_tx, udp_dropped;   /* dropped: the client's queue was full */
    uint64_t tcp_segments_rx, tcp_segments_tx, tcp_retransmits;
    uint64_t arp_queued, arp_dropped;       /* packets held for an unresolved address */
    uint32_t udp_open, tcp_open;
    uint32_t dhcp_leases, restarts;
    uint64_t bytes_rx, bytes_tx;            /* whole Ethernet frames */
    /* TCP receive side (release 0.14.0, phase 1): segments that did not start
     * at the next byte expected (ahead: a hole before them; behind: old data
     * sent again), acknowledgements announcing a closed window, and window
     * updates sent once the client read enough. */
    uint64_t tcp_rx_ahead, tcp_rx_behind, tcp_zero_window, tcp_window_updates;
} net_stats_t;

typedef struct {
    uint32_t op;
    int32_t status;          /* NET_OK or NET_ERR_* */
    uint8_t ip[4];
    uint16_t port;
    uint16_t reserved;
    uint64_t value;
    peos_netcfg_t cfg;
    net_stats_t stats;
} net_rep_t;

/* Client library (libpeos). Connections are channel descriptors: wait for
 * PEOS_WAIT_READ on them like on any other. Timeouts are in nanoseconds,
 * 0 = do not wait. */
int64_t net_udp_open(uint16_t port);
/* The same, and the port netd gave (port 0: any free one). */
int64_t net_udp_open_port(uint16_t port, uint16_t *out_port);
int64_t net_udp_send(int64_t conn, const uint8_t ip[4], uint16_t port, const void *buf, size_t len);
/* -> payload bytes, 0 on timeout, -1 on error. */
int64_t net_udp_recv(int64_t conn, void *buf, size_t cap, uint8_t src_ip[4], uint16_t *src_port,
                     uint64_t timeout_ns);

/* -> connection, or a NET_ERR_* value. */
int64_t net_tcp_connect(const uint8_t ip[4], uint16_t port, uint64_t timeout_ns);
/* Blocks until all of it is queued. -> len or -1. */
int64_t net_tcp_send(int64_t conn, const void *buf, size_t len);
/* -> bytes (at most cap), 0 at the end of the stream, NET_ERR_TIMEOUT, or -1
 * (connection broken). Bytes of a piece that did not fit wait for the next
 * call: give the same buffer state per connection (see net_tcp_reader_t). */
typedef struct {
    int64_t conn;
    uint8_t buf[PEOS_CHANNEL_MSG_MAX];
    uint32_t len, pos;
    int eof;
} net_tcp_reader_t;
void net_tcp_reader_init(net_tcp_reader_t *r, int64_t conn);
int64_t net_tcp_read(net_tcp_reader_t *r, void *out, size_t cap, uint64_t timeout_ns);
/* Closes a connection (the same as peos_fclose): what was sent still goes,
 * then the end of the stream. */
void net_tcp_close(int64_t conn);

/* Listening (needs the "net.listen" capability, besides "network").
 *
 * net_tcp_listen -> a listener for that port on every interface, or
 *   NET_ERR_DENIED (no "net.listen"), NET_ERR_REFUSED (the port is already
 *   listened on), NET_ERR_BUSY (netd has no room), NET_ERR (port 0).
 *   The listener is a descriptor: peos_wait on it with PEOS_WAIT_READ is
 *   ready when a connection is waiting to be accepted (or when netd is gone:
 *   then net_tcp_accept returns -1). Up to 16 finished connections wait
 *   there, and up to 4 handshakes at once per port; beyond that new ones are
 *   reset.
 * net_tcp_accept -> a connection (net_tcp_send, net_tcp_reader_init +
 *   net_tcp_read, net_tcp_close, exactly like net_tcp_connect's), or
 *   NET_ERR_AGAIN when none is waiting (it never blocks), or -1 when the
 *   listener is broken (netd restarted: listen again).
 *   net_tcp_accept_peer also gives the peer's address and port.
 * net_tcp_unlisten: stops listening and closes the listener; connections
 *   not accepted yet are reset, those accepted stay. When it returns the
 *   port can be listened on again. Ending the process does the same.
 *
 * Waiting for a connection's data without polling: the connection is a
 * descriptor too. peos_wait on it with PEOS_WAIT_READ is ready when a piece
 * of the stream, the end of the stream, or the break is there; then call
 * net_tcp_read(r, ..., 0) until it returns NET_ERR_TIMEOUT (nothing more
 * now), 0 (the end) or -1 (broken). Do not wait while the reader still holds
 * bytes of a piece (r->pos < r->len): read them first. So one peos_wait can
 * hold the listener and every connection, with the window's own events. */
int64_t net_tcp_listen(uint16_t port);
int64_t net_tcp_accept(int64_t listener);
int64_t net_tcp_accept_peer(int64_t listener, uint8_t peer_ip[4], uint16_t *peer_port);
void net_tcp_unlisten(int64_t listener);

int net_resolve(const char *name, uint8_t out_ip[4], uint64_t timeout_ns);
int net_config_get(peos_netcfg_t *out);
int net_config_set(const peos_netcfg_t *cfg);
int net_dhcp(peos_netcfg_t *out, uint64_t timeout_ns);
/* Waits (up to timeout_ns) until the card has an address, without asking for
 * a new one. NET_OK, or NET_ERR_TIMEOUT. */
int net_wait_address(peos_netcfg_t *out, uint64_t timeout_ns);
/* The time from an NTP server (SNTP, UDP 123): unix nanoseconds, UTC, the
 * network's delay taken off. NET_OK or a NET_ERR_* value. */
int net_sntp(const char *server, uint64_t timeout_ns, uint64_t *unix_ns);
/* -> round trip in ns, or a NET_ERR_* value. */
int64_t net_ping(const uint8_t ip[4], uint64_t timeout_ns);
int net_stats(net_stats_t *out);
/* Tests only (needs "debug"): netd faults on purpose; init starts it again. */
int net_debug_crash(void);
/* The computer's name (Settings): netd sends it in DHCP and asks for the
 * address again, so the router knows it now. NET_OK or a NET_ERR_* value. */
int net_set_hostname(const char *name);
/* Interface index (0, 1...): NET_OK and *out, or NET_ERR past the last one. */
int net_if_get(uint32_t index, net_iface_t *out);
/* An interface's address: by DHCP (dhcp 1; cfg not used), or cfg by hand. */
int net_if_config_set(uint32_t index, const peos_netcfg_t *cfg, int dhcp);
/* The name as it goes on the network: letters, digits and '-', at most 63
 * (spaces become '-'); empty when nothing of it is left. */
void net_hostname_clean(const char *in, char *out, size_t cap);
int net_parse_ipv4(const char *s, uint8_t out[4]);
#ifdef __cplusplus
}
#endif

#pragma once
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
};

enum {
    NET_OK = 0,
    NET_ERR = -1,            /* bad request */
    NET_ERR_TIMEOUT = -2,
    NET_ERR_REFUSED = -3,    /* TCP reset, port in use */
    NET_ERR_NO_ADDRESS = -4, /* no IPv4 address yet (DHCP not done) */
    NET_ERR_UNREACHABLE = -5,/* ARP gave no answer */
    NET_ERR_NOT_FOUND = -6,  /* DNS: no such name */
    NET_ERR_BUSY = -7,       /* netd has no room for another connection */
    NET_ERR_DENIED = -8,
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

int net_resolve(const char *name, uint8_t out_ip[4], uint64_t timeout_ns);
int net_config_get(peos_netcfg_t *out);
int net_config_set(const peos_netcfg_t *cfg);
int net_dhcp(peos_netcfg_t *out, uint64_t timeout_ns);
/* -> round trip in ns, or a NET_ERR_* value. */
int64_t net_ping(const uint8_t ip[4], uint64_t timeout_ns);
int net_stats(net_stats_t *out);
/* Tests only (needs "debug"): netd faults on purpose; init starts it again. */
int net_debug_crash(void);
int net_parse_ipv4(const char *s, uint8_t out[4]);

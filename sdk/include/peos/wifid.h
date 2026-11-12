#pragma once
#ifdef __cplusplus
extern "C" {
#endif
/* wifid (release 0.16.0, phase 5): the saved Wi-Fi networks and joining
 * them again by itself. A service of the system, started by init; it keeps
 * the networks in /system/config/wifi/networks.cfg with their PMK (PBKDF2 of
 * the password and the name), never the password itself.
 *
 * Clients (Impostazioni, "network" capability) ask through the service name
 * "wifi": one connection, one wifid_req_t, one wifid_rep_t back. A join
 * answers as soon as the card took the request; how it goes (link up, wrong
 * password) is told by the Wi-Fi descriptor (peos_wifi_watch) and
 * peos_wifi_status. A network asked to be saved is saved once joined. */

#include <stddef.h>
#include <stdint.h>
#include "peos/wifi.h"

#define WIFID_SERVICE_NAME "wifi"
#define WIFID_SAVED_MAX 16u
#define WIFID_CONFIG_DIR "/system/config/wifi"
#define WIFID_CONFIG_PATH "/system/config/wifi/networks.cfg"

enum {
    WIFID_OP_JOIN = 1,         /* ssid, security, password (8-63; empty: open, or a saved network's PMK), save */
    WIFID_OP_DISCONNECT = 2,   /* and no joining by itself until the next join */
    WIFID_OP_SAVED = 3,        /* -> the saved networks (names only), enabled */
    WIFID_OP_FORGET = 4,       /* ssid */
    WIFID_OP_SET_ENABLED = 5,  /* enabled: Wi-Fi on or off (off: disconnected, kept off) */
};

enum {
    WIFID_OK = 0,
    WIFID_ERR = -1,             /* bad request, or no wifid */
    WIFID_ERR_NO_ADAPTER = -2,  /* no Wi-Fi card */
    WIFID_ERR_UNSUPPORTED = -3, /* WPA3, 802.1X */
    WIFID_ERR_INVAL = -4,       /* a password of the wrong length */
    WIFID_ERR_OFF = -5,         /* Wi-Fi turned off */
};

typedef struct {
    uint32_t op;
    uint8_t ssid_len;
    uint8_t ssid[PEOS_WIFI_SSID_MAX_LEN];
    uint8_t security;   /* PEOS_WIFI_SECURITY_* */
    uint8_t save;
    uint8_t enabled;
    char password[64];  /* NUL-terminated */
} wifid_req_t;

typedef struct {
    uint8_t ssid_len;
    uint8_t ssid[PEOS_WIFI_SSID_MAX_LEN];
    uint8_t security;
} wifid_net_t;

typedef struct {
    uint32_t op;
    int32_t status;     /* WIFID_OK or WIFID_ERR_*, or a PEOS_WIFI_ERR_* of the card */
    uint32_t count;
    uint8_t enabled;
    uint8_t reserved[3];
    wifid_net_t nets[WIFID_SAVED_MAX];
} wifid_rep_t;

/* Client (libpeos/wificlient.c). */
int wifid_join(const uint8_t *ssid, size_t ssid_len, uint8_t security, const char *password, int save);
int wifid_disconnect(void);
int wifid_saved(wifid_rep_t *out);
int wifid_forget(const uint8_t *ssid, size_t ssid_len);
int wifid_set_enabled(int enabled);

/* The saved networks, as wifid keeps them (libpeos/wifi_saved.c: plain C,
 * tested on the host). */
typedef struct {
    uint8_t ssid_len;
    uint8_t ssid[PEOS_WIFI_SSID_MAX_LEN];
    uint8_t security;
    char pmk[65];       /* 64 hex digits, or "" for an open network */
} wifi_saved_net_t;

typedef struct {
    uint8_t enabled;
    uint32_t count;
    wifi_saved_net_t nets[WIFID_SAVED_MAX];   /* the oldest first */
} wifi_saved_t;

/* networks.cfg: "enabled=1" and one "net=<ssid in hex> <security> <pmk|->"
 * per network. Unknown or broken lines are skipped. */
void wifi_saved_parse(wifi_saved_t *out, const char *text, size_t len);
size_t wifi_saved_format(const wifi_saved_t *in, char *out, size_t cap);
int wifi_saved_find(const wifi_saved_t *s, const uint8_t *ssid, size_t ssid_len);
/* Adds or replaces (it becomes the newest); a full list drops the oldest. */
void wifi_saved_put(wifi_saved_t *s, const uint8_t *ssid, size_t ssid_len, uint8_t security, const char *pmk);
int wifi_saved_forget(wifi_saved_t *s, const uint8_t *ssid, size_t ssid_len);
/* The strongest saved network among those in range (supported security, the
 * same as saved): its index in aps, *saved_index its place in s; -1 if none. */
int wifi_saved_best(const wifi_saved_t *s, const peos_wifi_ap_t *aps, size_t n, int *saved_index);
/* Waits between tries while no saved network answers: 10 s, then doubling,
 * at most 5 minutes. */
uint64_t wifi_retry_next_ms(uint64_t prev_ms);
#ifdef __cplusplus
}
#endif

#pragma once
#ifdef __cplusplus
extern "C" {
#endif
#include <stdint.h>

#define PEOS_WIFI_SSID_MAX_LEN 32u
#define PEOS_WIFI_PSK_MAX_LEN 64u
#define PEOS_WIFI_BSSID_LEN 6u
#define PEOS_WIFI_SCAN_MAX_APS 32u
#define PEOS_WIFI_RSSI_UNKNOWN (-127)

typedef enum {
    PEOS_WIFI_STATE_DOWN = 0,
    PEOS_WIFI_STATE_SCANNING = 1,
    PEOS_WIFI_STATE_AUTH = 2,
    PEOS_WIFI_STATE_ASSOC = 3,
    PEOS_WIFI_STATE_UP = 4,
} peos_wifi_state_t;

typedef enum {
    PEOS_WIFI_SECURITY_OPEN = 0,
    PEOS_WIFI_SECURITY_WPA2_PSK = 1,
    PEOS_WIFI_SECURITY_WPA3_SAE = 2,
} peos_wifi_security_t;

typedef enum {
    PEOS_WIFI_BAND_UNKNOWN = 0,
    PEOS_WIFI_BAND_2GHZ = 1,
    PEOS_WIFI_BAND_5GHZ = 2,
} peos_wifi_band_t;

/* One step of the join at a time (wifi step, for diagnosis): see the kernel's wifi_types.h. */
#define PEOS_WIFI_STEP_ALL      0u
#define PEOS_WIFI_STEP_SECURITY 1u
#define PEOS_WIFI_STEP_KEY      2u
#define PEOS_WIFI_STEP_JOIN     3u
/* A PSK of 64 hex digits is the PMK itself (peos_wifi_pmk_hex), not a passphrase. */

typedef struct {
    uint8_t ssid[PEOS_WIFI_SSID_MAX_LEN];
    uint8_t ssid_len;
    uint8_t bssid[PEOS_WIFI_BSSID_LEN];
    int8_t rssi_dbm;
    uint8_t channel;
    uint8_t band;
    uint8_t security;
    uint8_t reserved;
} peos_wifi_ap_t;

typedef struct {
    uint8_t passive;
    uint8_t max_results;
    uint16_t timeout_ms;
} peos_wifi_scan_params_t;

typedef struct {
    uint32_t count;
    peos_wifi_ap_t aps[PEOS_WIFI_SCAN_MAX_APS];
} peos_wifi_scan_results_t;

typedef struct {
    peos_wifi_scan_params_t params;
    peos_wifi_scan_results_t results;
} peos_wifi_scan_req_t;

typedef struct {
    uint8_t ssid_len;
    uint8_t ssid[PEOS_WIFI_SSID_MAX_LEN];
    uint8_t security;
    uint8_t psk_len;
    uint8_t psk[PEOS_WIFI_PSK_MAX_LEN];
    uint8_t bssid[PEOS_WIFI_BSSID_LEN];
    uint8_t use_bssid;
    uint8_t step;      // PEOS_WIFI_STEP_*: 0 the whole join, or one step of it (diagnosis)
    uint8_t reserved[2];
} peos_wifi_connect_params_t;

typedef struct {
    uint8_t state;
    uint8_t link_up;
    uint8_t security;
    uint8_t channel;
    int8_t rssi_dbm;
    uint8_t ssid_len;
    uint8_t ssid[PEOS_WIFI_SSID_MAX_LEN];
    uint8_t bssid[PEOS_WIFI_BSSID_LEN];
    int32_t last_error;
} peos_wifi_status_t;

int64_t peos_wifi_scan(peos_wifi_scan_req_t *req);
int64_t peos_wifi_connect(const peos_wifi_connect_params_t *params);
int64_t peos_wifi_disconnect(void);
int64_t peos_wifi_status(peos_wifi_status_t *status);

const char *peos_wifi_state_name(uint8_t state);
const char *peos_wifi_security_name(uint8_t security);
#ifdef __cplusplus
}
#endif

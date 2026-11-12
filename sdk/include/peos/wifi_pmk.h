#pragma once
#include <stddef.h>
#include <stdint.h>

/*
 * The WPA2 key from a passphrase (IEEE 802.11i): PMK = PBKDF2-HMAC-SHA1
 * (passphrase, SSID, 4096 rounds, 32 bytes). Computed in PeOS so that the
 * join can give the firmware the PMK (64 hex digits) instead of the
 * passphrase, and so that saved networks keep the PMK, not the password.
 */

void peos_sha1(const void *data, size_t len, uint8_t out[20]);
void peos_hmac_sha1(const void *key, size_t key_len, const void *data, size_t len, uint8_t out[20]);
void peos_pbkdf2_hmac_sha1(const void *pass, size_t pass_len, const void *salt, size_t salt_len, uint32_t rounds,
                           uint8_t *out, size_t out_len);

/* The PMK as 64 lowercase hex digits and a NUL. -1 if the passphrase is not
 * 8-63 characters or the SSID not 1-32 bytes. */
int peos_wifi_pmk_hex(const char *passphrase, const uint8_t *ssid, size_t ssid_len, char out[65]);

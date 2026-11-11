/* PeStore's catalog (release 0.14.0, phase 4): what apps there are, read
 * from https://peoverse.com/store/catalog.json, and whether to trust it.
 *
 * The catalog is static files (docs/release-0.14.0-plan.md, phase 4):
 *   catalog.json       the apps, below
 *   catalog.json.sig   ECDSA P-256 over the SHA-256 of catalog.json (DER,
 *                      as `openssl dgst -sha256 -sign` makes it); the public
 *                      key is in /system/config/store.pub
 *   apps/<id>/...      each app's tar, icon and screenshots, each with its
 *                      SHA-256 in the catalog: a changed file is refused
 *
 *   {"format": 1, "updated": "2026-11-10", "featured": "lavagna",
 *    "apps": [{"id": "lavagna", "name": "Lavagna", "version": "1.3.0",
 *              "author": "...", "category": "strumenti",
 *              "summary": {"it": "...", "en": "..."},
 *              "description": {"it": "...", "en": "..."},
 *              "capabilities": ["fs.read", "fs.write", "shm"],
 *              "min_peos": "0.14.0", "date": "2026-11-08",
 *              "bundle": "Lavagna.app", "size": 1887436,
 *              "tar": "apps/lavagna/1.3.0/lavagna.app.tar", "sha256": "<hex>",
 *              "icon": {"path": "apps/lavagna/icon.png", "sha256": "<hex>"},
 *              "shots": [{"path": "...", "sha256": "<hex>"}]}]}
 *
 * Unknown keys are skipped: later catalogs may say more. The parsing is pure
 * (tests/host/test_store.c); store_verify uses mbedTLS. */
#ifndef PEOS_STORE_H
#define PEOS_STORE_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

#define STORE_FORMAT 1
#define STORE_SHOTS_MAX 6

typedef struct {
    char path[192];
    uint8_t sha256[32];
} store_file_t;

typedef struct {
    char id[48];
    char name[64];
    char version[24];
    char author[64];
    char category[24];          /* store_category: strumenti, giochi, media, sviluppo, internet... */
    char summary_it[160], summary_en[160];
    char description_it[1024], description_en[1024];
    char capabilities[160];     /* "fs.read, fs.write, shm" */
    char min_peos[16];
    char date[16];
    char bundle[64];            /* the folder in the tar: <Name>.app */
    uint64_t size;
    store_file_t tar;
    store_file_t icon;
    store_file_t shots[STORE_SHOTS_MAX];
    uint32_t nshots;
} store_app_t;

typedef struct {
    uint32_t format;
    char updated[16];
    char featured[48];
    store_app_t *apps;          /* malloc'd */
    uint32_t count;
} store_catalog_t;

/* 0, or -1 with why in err (English). The catalog is freed with store_catalog_free. */
int store_catalog_parse(const char *json, size_t len, store_catalog_t *out, char *err, size_t errcap);
void store_catalog_free(store_catalog_t *c);
const store_app_t *store_catalog_find(const store_catalog_t *c, const char *id);

/* Versions as "1.10.2": <0, 0, >0 as a is before, the same as, after b. */
int store_version_cmp(const char *a, const char *b);

/* data signed with the private key whose public key (PEM) is pub_pem: 1
 * yes, 0 no (or anything that cannot be read). */
int store_verify(const uint8_t *data, size_t len, const uint8_t *sig, size_t sig_len, const char *pub_pem);
void store_sha256(const void *data, size_t len, uint8_t out[32]);
/* 64 hex digits into 32 bytes: 0, or -1. */
int store_hex32(const char *hex, uint8_t out[32]);

#ifdef __cplusplus
}
#endif
#endif

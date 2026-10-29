#pragma once
/* A small inflate (RFC 1951), with the gzip (RFC 1952) and zlib (RFC 1950)
 * wrappers, for HTTP's Content-Encoding (release 0.11.0, phase 3).
 *
 * It pulls: the compressed bytes come from read(), one call per byte that
 * is needed, and the output goes to write() in pieces as it is made (the
 * page shows while it arrives). Pure, no allocation: the 32 KiB window is
 * in inflate_t. A host test checks it (tests/host/test_http.c). */
#include <stddef.h>
#include <stdint.h>

/* -> a byte 0..255, or -1 at the end of the input (or an error). */
typedef int (*inflate_read_fn)(void *user);
/* -> 0, or not 0 to stop. */
typedef int (*inflate_write_fn)(void *user, const uint8_t *data, size_t len);

typedef enum {
    INFLATE_RAW = 0,     /* deflate, no wrapper */
    INFLATE_ZLIB = 1,    /* "deflate" in HTTP is this, or raw: both are tried */
    INFLATE_GZIP = 2,
} inflate_wrap_t;

enum {
    INFLATE_OK = 0,
    INFLATE_ERR_DATA = -1,      /* not valid compressed data */
    INFLATE_ERR_TRUNCATED = -2, /* the input ended too soon */
    INFLATE_ERR_STOPPED = -3,   /* write() asked to stop */
};

typedef struct {
    inflate_read_fn read;
    inflate_write_fn write;
    void *user;
    uint32_t bitbuf, bitcnt;
    uint8_t window[32768];
    uint32_t wpos;              /* next byte of the window */
    uint32_t flushed;           /* window bytes already given to write() */
    uint64_t total;             /* bytes made */
    int err;
    uint32_t crc, adler;        /* of what was made, for the trailers */
    uint8_t back[2];            /* bytes read to look at the header, given again */
    uint32_t nback;
} inflate_t;

/* Decompresses the whole stream: INFLATE_OK or an INFLATE_ERR_*. */
int inflate_run(inflate_t *z, inflate_wrap_t wrap, inflate_read_fn read, inflate_write_fn write, void *user);

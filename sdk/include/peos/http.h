#pragma once
/* An HTTP/1.1 client (release 0.11.0, phase 3), for appget, the browser and
 * the apps: http:// and https:// (peos/tls.h), connections kept open and
 * used again, chunked bodies, gzip and deflate (peos/inflate.h), redirects,
 * cookies. The body comes in pieces as it arrives.
 *
 *   static int got(void *user, const uint8_t *data, size_t len) { ...; return 0; }
 *   http_request_t req = { .url = "https://example.org/" };
 *   http_result_t res;
 *   if (http_fetch(&req, &res, got, NULL) == 0) printf("%d\n", res.resp.status);
 *
 * The parsing (URLs, the response head, chunk sizes, cookies) is pure and
 * has a host test: tests/host/test_http.c. */
#include <stddef.h>
#include <stdint.h>

#define HTTP_URL_MAX 2048u
#define HTTP_HOST_MAX 256u
#define HTTP_HEAD_MAX 16384u

/* ------------------------------------------------------------------ URLs */

typedef struct {
    char scheme[8];             /* "http" or "https" */
    char host[HTTP_HOST_MAX];   /* lower case; an IPv6 address is refused */
    uint16_t port;              /* 80 or 443 if not given */
    char path[HTTP_URL_MAX];    /* from the first '/', the query included, no #fragment */
} http_url_t;

/* An absolute http(s) URL ("example.org/x" is read as http://example.org/x).
 * -> 0, or -1. */
int http_url_parse(const char *s, http_url_t *out);
/* ref against base, as a browser does: absolute, //host/x, /x, x, ?q, ../x.
 * -> 0, or -1. */
int http_url_resolve(const http_url_t *base, const char *ref, http_url_t *out);
/* "https://example.org/x" (the port only if not the scheme's). -> length,
 * or -1 if it does not fit. */
int http_url_format(const http_url_t *u, char *out, size_t cap);

/* ---------------------------------------------------- the response head */

enum { HTTP_ENC_NONE = 0, HTTP_ENC_GZIP = 1, HTTP_ENC_DEFLATE = 2, HTTP_ENC_OTHER = 3 };

typedef struct {
    int status;                 /* 200, 404... */
    char reason[64];
    int64_t content_length;     /* -1: not given */
    uint8_t chunked;
    uint8_t keep_alive;         /* the server keeps the connection open */
    uint8_t encoding;           /* HTTP_ENC_* */
    char content_type[128];     /* "text/html; charset=utf-8" */
    char location[HTTP_URL_MAX];
    char headers[HTTP_HEAD_MAX];/* the header lines, "Name: value\r\n" each */
    size_t headers_len;
} http_response_t;

/* head: the status line and the headers, up to the empty line. -> 0, -1. */
int http_parse_head(const char *head, size_t len, http_response_t *out);
/* The value of the first header of that name (any case), or NULL. */
const char *http_header(const http_response_t *r, const char *name, char *out, size_t cap);
/* The n-th header of that name, or NULL (Set-Cookie comes many times). */
const char *http_header_n(const http_response_t *r, const char *name, uint32_t n, char *out, size_t cap);
/* "1a;ext" -> 26; -1 if not a chunk size. */
int64_t http_parse_chunk_size(const char *line);

/* ---------------------------------------------------------------- cookies */

#define HTTP_COOKIES_MAX 64u

typedef struct {
    char name[64];
    char value[512];
    char domain[128];
    char path[128];
    uint8_t secure;
    uint8_t host_only;          /* no Domain=: only that host */
    int64_t expires;            /* Unix seconds, 0: for the session */
} http_cookie_t;

/* A jar for a session (the browser's tabs share one). */
typedef struct {
    http_cookie_t c[HTTP_COOKIES_MAX];
    uint32_t n;
} http_cookies_t;

/* One Set-Cookie value from that URL's response. */
void http_cookies_set(http_cookies_t *jar, const http_url_t *from, const char *set_cookie, int64_t now);
/* The Cookie header's value for a request to url ("a=1; b=2"). -> length. */
int http_cookies_header(const http_cookies_t *jar, const http_url_t *url, int64_t now, char *out, size_t cap);

/* ------------------------------------------------------------------ fetch */

/* A piece of the body (decoded: no chunks, no gzip). Not 0 stops. */
typedef int (*http_body_fn)(void *user, const uint8_t *data, size_t len);

typedef struct {
    const char *method;         /* NULL: GET */
    const char *url;
    const char *headers;        /* more lines, "Name: value\r\n" each, or NULL */
    const void *body;           /* POST */
    size_t body_len;
    const char *content_type;   /* of the body */
    uint32_t max_redirects;     /* 0: 10; followed only for GET and HEAD, or a 303 */
    uint8_t no_redirects;       /* the 3xx comes back as it is */
    uint64_t timeout_ns;        /* 0: 15 s, for every wait */
    http_cookies_t *cookies;    /* NULL: none sent, none kept */
    const char *user_agent;     /* NULL: "Mozilla/5.0 (PeOS 0.11; x86_64) PeOS/0.11" */
} http_request_t;

typedef struct {
    http_response_t resp;       /* the last one, after the redirects */
    char url[HTTP_URL_MAX];     /* where it came from in the end */
    char tls[16];               /* "TLSv1.3", "" for http:// */
    uint64_t body_bytes;        /* given to the callback */
    char error[160];            /* why it failed */
} http_result_t;

/* -> 0 when a response came (whatever its status), -1 with res->error.
 * The connection is kept for the next fetch to the same server when the
 * whole body was read. */
int http_fetch(const http_request_t *req, http_result_t *res, http_body_fn body, void *user);
/* Closes the connection kept open, if any. */
void http_close_idle(void);

#pragma once
/* TLS over netd (release 0.11.0, phase 3): mbedTLS 3.6 (userland/ports/
 * mbedtls), TLS 1.3 and 1.2, a client only. The server's certificate is
 * checked against the root certificates in /system/certs/ (every *.pem, the
 * Mozilla bundle among them), for the host name and the time of the RTC.
 *
 *   int64_t tcp = net_tcp_connect(ip, 443, timeout);
 *   tls_conn_t *t = tls_open(tcp, "example.org", timeout, err, sizeof(err));
 *   tls_write(t, req, len); tls_read(t, buf, sizeof(buf), timeout);
 *   tls_close(t);                 // closes the TCP connection too
 *
 * The connection owns the TCP descriptor from tls_open on, failed or not. */
#include <stddef.h>
#include <stdint.h>

#define TLS_CERTS_DIR "/system/certs"

typedef struct tls_conn tls_conn_t;

/* The handshake: a connection, or NULL with why in err ("certificate not
 * trusted", "expired"...). */
tls_conn_t *tls_open(int64_t tcp, const char *host, uint64_t timeout_ns, char *err, size_t err_cap);
/* -> len, or -1 (broken). */
int64_t tls_write(tls_conn_t *t, const void *buf, size_t len);
/* -> bytes, 0 at the end, NET_ERR_TIMEOUT, or -1 (broken, or not authentic). */
int64_t tls_read(tls_conn_t *t, void *buf, size_t cap, uint64_t timeout_ns);
/* "TLSv1.3", "TLSv1.2"; the cipher suite's name. */
const char *tls_version(const tls_conn_t *t);
const char *tls_cipher(const tls_conn_t *t);
void tls_close(tls_conn_t *t);

/* The roots read so far (they are read once per process, at the first
 * tls_open), or -1 if none could be read. */
int tls_root_count(void);

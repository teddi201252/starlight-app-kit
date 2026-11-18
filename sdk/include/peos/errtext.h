/*
 * Errors as people read them (docs/system-apps-ux-plan.md 1.4): libpeos's
 * own errors (the network, HTTPS, the store, updates) are English
 * sentences for the log. An app shows instead the sentence its String.lang
 * has for the error's kind: the same few keys in every app that downloads
 * or installs (PeStore, Pegasus, Settings, the Installer):
 *
 *   err.network     no network, a name not found, a server not reached,
 *                   the connection broken
 *   err.cert        the certificate refused (a connection not used)
 *   err.cert_clock  the certificate not valid yet: the clock is wrong
 *   err.server      the server answered something else than asked
 *   err.bad_data    what came is damaged, not signed or not readable
 *   err.too_new     it needs a newer PeOS
 *   err.space       not enough room on the disk
 *   err.write       a file or folder could not be written
 *   err.memory      out of memory
 *   err.other       anything else (the details are in the log)
 *
 * The English sentence still goes to the log (printf), whole.
 */
#ifndef PEOS_ERRTEXT_H
#define PEOS_ERRTEXT_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    PEOS_ERR_OTHER = 0,
    PEOS_ERR_NETWORK,
    PEOS_ERR_CERT,
    PEOS_ERR_CERT_CLOCK,
    PEOS_ERR_SERVER,
    PEOS_ERR_BAD_DATA,
    PEOS_ERR_TOO_NEW,
    PEOS_ERR_SPACE,
    PEOS_ERR_WRITE,
    PEOS_ERR_MEMORY,
    PEOS_ERR_KIND_COUNT
} peos_err_kind_t;

/* The kind of one of libpeos's English error sentences (NULL or "": other). */
peos_err_kind_t peos_error_kind(const char *raw);
/* Its key in an app's String.lang: "err.network", ... "err.other". */
const char *peos_error_key(const char *raw);
const char *peos_error_kind_key(peos_err_kind_t kind);

#ifdef __cplusplus
}
#endif

#endif

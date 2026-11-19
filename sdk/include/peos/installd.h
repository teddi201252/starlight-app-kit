#pragma once
#ifdef __cplusplus
extern "C" {
#endif
/* The install service (system apps UX plan, phase 4, point 5): installd
 * keeps the queue of the apps to install from the store, so an install
 * goes on when PeStore's window is closed. Started by init; it owns the
 * name "install", and only a client with the "install" capability may
 * connect (PeStore). It installs only what comes from the store's address
 * (/system/config/store.cfg), and the tar must hash to the SHA-256 the
 * client gives (the signed catalog's: PeStore checked the signature).
 *
 * Every request is one connection to "install": one install_req_t, one
 * install_status_t back. INSTALL_OP_WATCH keeps the connection: installd
 * sends an install_status_t on it whenever the queue changes, the running
 * download moves on (at most a few times a second) or an install ends
 * (`last` then names it, with a new `last_seq`). With nobody watching,
 * the end of an install is a desktop notification instead.
 *
 * The running install can be cancelled while it downloads; once it is
 * being put in place (`placing`) it is too late (INSTALL_ERR_LATE): the
 * app it replaces must not be lost half way. */

#include <stdint.h>

#define INSTALL_SERVICE_NAME "install"
#define INSTALL_MAGIC 0x31534E49u   /* "INS1" */
#define INSTALL_QUEUE_MAX 20u       /* the running one included */
#define INSTALL_ID_MAX 48u
#define INSTALL_NAME_MAX 64u
#define INSTALL_URL_MAX 320u

enum {
    INSTALL_OP_STATUS = 1,
    INSTALL_OP_WATCH = 2,     /* keep the connection: statuses come on it */
    INSTALL_OP_ADD = 3,       /* app, name, url, sha256, total: at the end of the queue */
    INSTALL_OP_CANCEL = 4,    /* app: out of the queue, or the running one stopped */
};

enum {
    INSTALL_OK = 0,
    INSTALL_ERR = -1,         /* a bad request */
    INSTALL_ERR_NONE = -2,    /* no installd (or no "install" capability) */
    INSTALL_ERR_FULL = -3,    /* the queue is full */
    INSTALL_ERR_LATE = -4,    /* cancel: it is being put in place already */
    INSTALL_ERR_URL = -5,     /* add: not from the store's address */
    INSTALL_ERR_FAILED = -6,  /* last: the install failed (last_error says why) */
    INSTALL_ERR_CANCELLED = -7, /* last: cancelled */
};

enum { INSTALL_QUEUED = 0, INSTALL_RUNNING = 1 };

typedef struct {
    uint32_t op;
    uint32_t reserved;
    uint64_t total;                  /* add: the tar's size, for the progress (0: unknown) */
    char app[INSTALL_ID_MAX];        /* the catalog's id */
    char name[INSTALL_NAME_MAX];     /* add: its name, for the notification */
    char url[INSTALL_URL_MAX];       /* add: the tar */
    char sha256[72];                 /* add: 64 hex digits */
} install_req_t;

typedef struct {
    char app[INSTALL_ID_MAX];
    uint64_t done;                   /* bytes downloaded (the running one) */
    uint64_t total;
    uint32_t state;                  /* INSTALL_QUEUED / INSTALL_RUNNING */
    uint32_t placing;                /* running: downloaded, being put in place */
} install_entry_t;

typedef struct {
    uint32_t magic;                  /* INSTALL_MAGIC */
    uint32_t op;                     /* the request this answers (0: a watch update) */
    int32_t status;                  /* INSTALL_OK or INSTALL_ERR_* */
    uint32_t count;                  /* entries in queue[], the running one first */
    install_entry_t queue[INSTALL_QUEUE_MAX];
    uint32_t last_seq;               /* grows by one at every install that ends */
    int32_t last_status;             /* INSTALL_OK, _FAILED or _CANCELLED */
    char last_app[INSTALL_ID_MAX];
    char last_bundle[64];            /* ok: the <Name>.app it put in /apps */
    char last_error[128];            /* failed: why (English, peos/errtext.h) */
    uint32_t last_replaced;          /* ok: it replaced one (an update) */
    uint32_t reserved;
} install_status_t;

/* One request: 0 / INSTALL_ERR_*; st (may be NULL) gets the queue. */
int install_call(const install_req_t *req, install_status_t *st);
int install_add(const char *app, const char *name, const char *url, const char *sha256, uint64_t total,
                install_status_t *st);
int install_cancel(const char *app, install_status_t *st);
/* A watching connection (a descriptor to wait on for READ), or -1. The
 * first status comes on it at once. */
int64_t install_watch(void);
/* The newest status waiting on fd: 1 read, 0 none, -1 the service went
 * (close fd, watch again later). */
int install_watch_read(int64_t fd, install_status_t *st);

#ifdef __cplusplus
}
#endif

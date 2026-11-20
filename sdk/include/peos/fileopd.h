#pragma once
#ifdef __cplusplus
extern "C" {
#endif
/* The file operations service (system apps UX plan, phase 4, point 7;
 * design/curiosity/, "Operazioni sui file"): fileopd copies, moves and
 * deletes for good, a piece at a time, so the app that asked (Curiosity)
 * stays free and may even be closed. Deleting a folder a file at a time
 * also leaves the file system to the other programs between two pieces
 * (a folder removed in one call held it for everybody until it was gone).
 *
 * Started by init; it owns the name "fileop". Only a client with the
 * capability "fileop" may connect (Curiosity and its window of the
 * operations; system apps plan, phase 5), since fileopd works on any path.
 *
 * Every request is one connection to "fileop": one fileop_req_t, then for
 * FILEOP_OP_ADD its `count` paths, '\n' after each, in messages of at most
 * PEOS_CHANNEL_MSG_MAX bytes; one fileop_status_t comes back.
 * FILEOP_OP_WATCH keeps the connection: a status comes on it whenever a
 * job starts, asks, ends, or moves on (at most four times a second).
 *
 * The jobs on the same disk go one at a time (the others wait); on other
 * disks they go together. A name already in the folder of arrival is asked
 * about (FILEOP_ASK_CONFLICT): skip it, replace it (the old one goes into
 * its disk's bin, peos/trash.h), or keep both ("name (copy).ext"). An
 * error is asked about too (FILEOP_ASK_ERROR): retry, skip that one, or
 * stop. A copy cut short leaves nothing half written; a delete stopped
 * leaves what is already gone, gone. With nobody watching, a question and
 * the end of a job are a desktop notification. */

#include <stdint.h>

#define FILEOP_SERVICE_NAME "fileop"
#define FILEOP_MAGIC 0x31504F46u      /* "FOP1" */
#define FILEOP_JOBS_MAX 16u           /* in the queue, the running ones included */
#define FILEOP_SHOWN 4u               /* jobs in a status (the first ones) */
#define FILEOP_SOURCES_MAX 1024u      /* paths in one job */
#define FILEOP_PATH_MAX 256u
#define FILEOP_NAME_MAX 48u
#define FILEOP_DIR_MAX 128u
#define FILEOP_SKIPPED_MAX 4u

enum {
    FILEOP_OP_STATUS = 1,
    FILEOP_OP_WATCH = 2,     /* keep the connection: statuses come on it */
    FILEOP_OP_ADD = 3,       /* kind, dst, word, count: the paths follow */
    FILEOP_OP_CANCEL = 4,    /* id (0: all of them) */
    FILEOP_OP_ANSWER = 5,    /* id, answer, all */
};

enum { FILEOP_COPY = 0, FILEOP_MOVE = 1, FILEOP_DELETE = 2 };

enum {
    FILEOP_OK = 0,
    FILEOP_ERR = -1,         /* a bad request */
    FILEOP_ERR_NONE = -2,    /* no fileopd (or not allowed to ask it) */
    FILEOP_ERR_FULL = -3,    /* the queue is full */
};

/* A job's state. */
enum {
    FILEOP_QUEUED = 0,       /* waiting for its disk */
    FILEOP_MEASURING = 1,    /* counting what there is to do */
    FILEOP_RUNNING = 2,
    FILEOP_ASKING = 3,       /* waiting for an answer (ask, in the status) */
    FILEOP_STOPPING = 4,     /* cancelled: taking away what was half written */
};

/* What a job asks. */
enum { FILEOP_ASK_NONE = 0, FILEOP_ASK_CONFLICT = 1, FILEOP_ASK_ERROR = 2 };

/* The answers. A conflict: SKIP, REPLACE, KEEP_BOTH (all: the same for
 * the next ones of that job). An error: RETRY, SKIP, STOP. */
enum {
    FILEOP_ANS_SKIP = 1,
    FILEOP_ANS_REPLACE = 2,
    FILEOP_ANS_KEEP_BOTH = 3,
    FILEOP_ANS_RETRY = 4,
    FILEOP_ANS_STOP = 5,
};

/* Why something could not be done. */
enum {
    FILEOP_WHY_NONE = 0,
    FILEOP_WHY_READ = 1,        /* it cannot be read */
    FILEOP_WHY_GONE = 2,        /* it is not there any more */
    FILEOP_WHY_FULL = 3,        /* the disk of arrival is full */
    FILEOP_WHY_WRITE = 4,       /* the folder of arrival cannot be written */
    FILEOP_WHY_INTO_ITSELF = 5, /* a folder into itself */
    FILEOP_WHY_TOO_LONG = 6,    /* a path too long */
    FILEOP_WHY_TOO_DEEP = 7,    /* folders inside folders too deep */
    FILEOP_WHY_TOO_MANY = 8,    /* a folder with too many names */
    FILEOP_WHY_DELETE = 9,      /* it cannot be deleted */
    FILEOP_WHY_REPLACE = 10,    /* the old one cannot go into the bin */
    FILEOP_WHY_REMOVE = 11,     /* moved by copying, but the original stayed */
};

/* How a job ended. */
enum { FILEOP_END_DONE = 0, FILEOP_END_STOPPED = 1, FILEOP_END_CANCELLED = 2 };

typedef struct {
    uint32_t magic;                  /* FILEOP_MAGIC */
    uint32_t op;
    uint32_t kind;                   /* add: FILEOP_COPY / _MOVE / _DELETE */
    uint32_t id;                     /* cancel, answer: the job */
    uint32_t answer;                 /* FILEOP_ANS_* */
    uint32_t all;                    /* answer: the same for the next ones */
    uint32_t count;                  /* add: the paths that follow */
    uint32_t reserved;
    char dst[FILEOP_PATH_MAX];       /* add, copy or move: the folder of arrival; delete: the folder to show */
    char word[16];                   /* add: a copy's name ("copia", "copy") */
    char label[FILEOP_NAME_MAX];     /* add: what the job is for a person ("Svuota il cestino di STICKA"), or "" */
} fileop_req_t;

typedef struct {
    uint32_t id;
    uint8_t kind;                    /* FILEOP_COPY... */
    uint8_t state;                   /* FILEOP_QUEUED... */
    uint8_t labeled;                 /* first is the request's label: the job's whole name */
    uint8_t reserved;
    uint32_t done;                   /* copy, move: the paths done; delete: the names */
    uint32_t total;
    uint64_t bytes_done;             /* copy, move */
    uint64_t bytes_total;            /* 0 while measuring */
    uint64_t rate;                   /* bytes a second (0: not known yet) */
    uint32_t eta_s;                  /* seconds left (0: not known yet) */
    uint32_t count;                  /* the paths of the job */
    char name[FILEOP_NAME_MAX];      /* what is being done now */
    char first[FILEOP_NAME_MAX];     /* the first path's name */
    char dst[FILEOP_DIR_MAX];        /* copy, move: the folder of arrival; delete: the first one's folder */
} fileop_job_t;

typedef struct {
    uint32_t id;                     /* the job asking (0: none asks) */
    uint32_t ask;                    /* FILEOP_ASK_* */
    uint32_t why;                    /* an error: FILEOP_WHY_* */
    uint32_t more;                   /* a conflict: the next names of the job already there */
    uint64_t old_size, old_mtime_ns; /* a conflict: the one there */
    uint64_t new_size, new_mtime_ns; /* and the one coming */
    uint64_t missing;                /* FULL: bytes missing (0: not known) */
    char name[FILEOP_NAME_MAX];
} fileop_ask_t;

typedef struct {
    uint32_t seq;                    /* grows by one at every job that ends */
    uint32_t id;
    uint8_t kind, end;               /* FILEOP_END_* */
    uint8_t skipped_n;               /* in skipped[] (up to FILEOP_SKIPPED_MAX) */
    uint8_t labeled;                 /* first is the job's label */
    uint32_t done, total;            /* the paths done, of how many */
    uint32_t skipped;                /* all those skipped for an error */
    char dst[FILEOP_DIR_MAX];
    char first[FILEOP_NAME_MAX];
    struct {
        char name[40];
        uint32_t why;
    } skipped_list[FILEOP_SKIPPED_MAX];
} fileop_last_t;

typedef struct {
    uint32_t magic;                  /* FILEOP_MAGIC */
    uint32_t op;                     /* the request this answers (0: a watch update) */
    int32_t status;                  /* FILEOP_OK or FILEOP_ERR_*; add: the new job's id when > 0 */
    uint32_t count;                  /* the jobs (jobs[] has the first FILEOP_SHOWN) */
    fileop_job_t jobs[FILEOP_SHOWN];
    fileop_ask_t ask;
    fileop_last_t last;
} fileop_status_t;

_Static_assert(sizeof(fileop_status_t) <= 2048u, "a status is one channel message");
_Static_assert(sizeof(fileop_req_t) <= 2048u, "a request is one channel message");

/* One request: FILEOP_OK / the new job's id (add) / FILEOP_ERR_*; st (may be NULL) gets the jobs. */
int fileop_call(const fileop_req_t *req, const char *const *paths, uint32_t count, fileop_status_t *st);
/* A job: kind FILEOP_COPY or _MOVE into dst, or FILEOP_DELETE (dst: the
 * folder its window shows, or NULL). word: a copy's name. label: its name
 * for a person, or NULL (then its first path's). Its id (> 0) or
 * FILEOP_ERR_*. */
int fileop_add(uint32_t kind, const char *const *paths, uint32_t count, const char *dst, const char *word,
               const char *label, fileop_status_t *st);
int fileop_cancel(uint32_t id, fileop_status_t *st);
int fileop_answer(uint32_t id, uint32_t answer, int all, fileop_status_t *st);
/* A watching connection (a descriptor to wait on for READ), or -1. The
 * first status comes on it at once. */
int64_t fileop_watch(void);
/* The newest status waiting on fd: 1 read, 0 none, -1 the service went
 * (close fd, watch again later). */
int fileop_watch_read(int64_t fd, fileop_status_t *st);

#ifdef __cplusplus
}
#endif

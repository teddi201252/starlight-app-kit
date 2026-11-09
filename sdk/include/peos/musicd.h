#pragma once
/* The music service (release 0.13.0, phase 5): musicd plays the queue that
 * Orfeus gives it, so the music goes on when Orfeus's window is closed.
 * It owns an audio stream (capability "audio"), reads the files (fs.read)
 * and the name "music" (needs "audio" to connect).
 *
 * The queue is a file (MUSIC_QUEUE_FILE), written by Orfeus and read by
 * musicd on MUSIC_OP_LOAD and when it starts: a header line, then a track a
 * line (music_track_t, fields split by tabs).
 *
 * Every request is one connection to "music": one music_req_t, one
 * music_status_t back (UPCOMING: music_upcoming_t). MUSIC_OP_WATCH keeps the
 * connection: musicd sends a music_status_t on it whenever something
 * changes (a track, playing or paused, a seek, the volume, the queue), never
 * for the clock: the position moves on from pos_ms at at_ns while playing.
 * Starlight and Orfeus watch; the commands are their own connections. */

#include <stddef.h>
#include <stdint.h>

#define MUSIC_SERVICE_NAME "music"
#define MUSIC_QUEUE_FILE "/starlight/config/orfeus-queue.txt"
#define MUSIC_QUEUE_MAGIC "ORFEUS-QUEUE 1"
#define MUSIC_STATUS_MAGIC 0x3153554Du   /* "MUS1" */

enum {
    MUSIC_OP_STATUS = 1,
    MUSIC_OP_WATCH = 2,     /* keep the connection: statuses come on it */
    MUSIC_OP_LOAD = 3,      /* the queue file again; value: the track to be at;
                               flags: MUSIC_LOAD_* */
    MUSIC_OP_PLAY = 4,
    MUSIC_OP_PAUSE = 5,
    MUSIC_OP_TOGGLE = 6,
    MUSIC_OP_NEXT = 7,
    MUSIC_OP_PREV = 8,      /* past 3 s into the track: back to its start */
    MUSIC_OP_SEEK = 9,      /* value64: milliseconds */
    MUSIC_OP_VOLUME = 10,   /* value: 0..100 */
    MUSIC_OP_SHUFFLE = 11,  /* value: 0 / 1 */
    MUSIC_OP_REPEAT = 12,   /* value: MUSIC_REPEAT_* */
    MUSIC_OP_STOP = 13,     /* nothing playing, the queue kept */
    MUSIC_OP_JUMP = 14,     /* value: the track (in the file's order), played */
    MUSIC_OP_UPCOMING = 15, /* -> music_upcoming_t: what comes next, in order */
};

enum {
    MUSIC_LOAD_PLAY = 1u,   /* play it (else as it was: paused stays paused) */
    MUSIC_LOAD_KEEP = 2u,   /* the track at `value` is the one playing: go on, no break */
};

enum { MUSIC_STOPPED = 0, MUSIC_PLAYING = 1, MUSIC_PAUSED = 2 };
enum { MUSIC_REPEAT_OFF = 0, MUSIC_REPEAT_ALL = 1, MUSIC_REPEAT_ONE = 2 };

enum {
    MUSIC_OK = 0,
    MUSIC_ERR = -1,         /* a bad request */
    MUSIC_ERR_NONE = -2,    /* no musicd (or no "audio" capability) */
    MUSIC_ERR_FILE = -3,    /* the track cannot be read or played */
    MUSIC_ERR_EMPTY = -4,   /* the queue is empty */
};

typedef struct {
    uint32_t op;
    uint32_t value;
    uint32_t flags;
    uint32_t reserved;
    uint64_t value64;
} music_req_t;

#define MUSIC_TEXT 96u
#define MUSIC_PATH 256u
#define MUSIC_COVER 160u

typedef struct {
    uint32_t magic;
    uint32_t op;            /* the request this answers (0: a watch update) */
    int32_t status;         /* MUSIC_OK or MUSIC_ERR_* */
    uint32_t state;         /* MUSIC_STOPPED / PLAYING / PAUSED */
    uint32_t index, count;  /* the track playing, in the file's order; the tracks */
    uint32_t volume, shuffle, repeat;
    uint32_t seq;           /* goes up with every queue loaded */
    uint32_t format, kbps;  /* of the track: MUS_FMT_*, kb/s (0 for WAV, FLAC) */
    uint32_t rate;
    uint32_t error;         /* the last track that could not play: MUSIC_ERR_FILE, else 0 */
    uint64_t pos_ms, dur_ms;
    uint64_t at_ns;         /* when pos_ms was true (peos_time_ns) */
    char title[MUSIC_TEXT], artist[MUSIC_TEXT], album[MUSIC_TEXT];
    char path[MUSIC_PATH];
    char cover[MUSIC_COVER];   /* a picture file, or "" (draw one: mus_art_for) */
} music_status_t;

#define MUSIC_UPCOMING_MAX 500u

typedef struct {
    uint32_t magic;
    uint32_t count;
    uint16_t index[MUSIC_UPCOMING_MAX];
} music_upcoming_t;

/* One line of the queue file. */
typedef struct {
    char path[MUSIC_PATH];
    char title[MUSIC_TEXT], artist[MUSIC_TEXT], album[MUSIC_TEXT];
    char cover[MUSIC_COVER];
    uint64_t dur_ms;
} music_track_t;

/* ------------------------------------------------------------ the client */

/* One request; *st gets the answer (it may be NULL). MUSIC_OK or MUSIC_ERR_*. */
int music_call(uint32_t op, uint32_t value, uint32_t flags, uint64_t value64, music_status_t *st);
int music_upcoming(music_upcoming_t *out);
/* A watching connection (its first message is the status now), or < 0. */
int64_t music_watch(void);
/* The latest status waiting on a watching connection: 1 if one came, 0 if
 * none, -1 if the connection is gone (musicd ended: watch again later). */
int music_watch_read(int64_t fd, music_status_t *st);
/* Where the track is now (pos_ms moved on while playing). */
uint64_t music_position_ms(const music_status_t *st, uint64_t now_ns);

/* The queue file: written by Orfeus, read by musicd. `current` is the track
 * the queue is at. 0, or -1. */
int music_queue_write(const char *path, const music_track_t *tracks, uint32_t count, uint32_t current);
/* *tracks is malloc'ed (free it). 0, or -1. */
int music_queue_read(const char *path, music_track_t **tracks, uint32_t *count, uint32_t *current);

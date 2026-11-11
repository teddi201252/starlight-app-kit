#pragma once
#ifdef __cplusplus
extern "C" {
#endif
/* Music files (release 0.13.0, phase 5: Orfeus). Our own decoders, no
 * libraries: WAV (on top of peos/audio.h), FLAC and MP3 (MPEG-1/2/2.5
 * Layer III). They work on the whole file in memory and give 16-bit stereo
 * at the file's own rate; mus_rs_t takes it to the device's 48 kHz.
 *
 * mus_scan() reads what a library needs from the head of a file only: the
 * tags (ID3v2, Vorbis comments, RIFF INFO), the cover inside, the length.
 *
 * Plain C with libc only: the host tests build it too. */

#include <stddef.h>
#include <stdint.h>

enum {
    MUS_FMT_NONE = 0,
    MUS_FMT_WAV = 1,
    MUS_FMT_FLAC = 2,
    MUS_FMT_MP3 = 3,
};

typedef struct {
    uint32_t format;          /* MUS_FMT_* */
    uint32_t rate;            /* samples a second, of the file */
    uint16_t channels;        /* of the file (the output is always stereo) */
    uint16_t bits;            /* of the file (MP3: 16) */
    uint64_t frames;          /* in all, at `rate`; 0 if not known */
    uint32_t kbps;            /* the bit rate (an average for VBR); 0 for WAV/FLAC */
} mus_info_t;

/* The format of these bytes (the start of a file), or MUS_FMT_NONE. */
uint32_t mus_probe(const uint8_t *data, size_t len);
/* By the name: ".flac", ".mp3", ".wav" (any case). */
uint32_t mus_format_of_name(const char *name);
const char *mus_format_name(uint32_t format);   /* "FLAC", "MP3", "WAV" */

/* ------------------------------------------------------------ decoding */

typedef struct mus_dec mus_dec_t;

/* A decoder over data[0..len), which must stay there until mus_close. NULL
 * if it is not a file we can play. */
mus_dec_t *mus_open(const uint8_t *data, size_t len, mus_info_t *info);
/* Up to max frames of interleaved stereo (L R L R...); 0 at the end. */
size_t mus_read(mus_dec_t *d, int16_t *out, size_t max_frames);
/* Go to this frame (of the file's rate); the next read starts there or
 * just before. 0, or -1 if it cannot. */
int mus_seek(mus_dec_t *d, uint64_t frame);
/* Where the next read starts. */
uint64_t mus_tell(const mus_dec_t *d);
void mus_close(mus_dec_t *d);

/* The format-specific decoders behind mus_open (also for the tests). */
mus_dec_t *mus_flac_open(const uint8_t *data, size_t len, mus_info_t *info);
mus_dec_t *mus_mp3_open(const uint8_t *data, size_t len, mus_info_t *info);
mus_dec_t *mus_wav_open(const uint8_t *data, size_t len, mus_info_t *info);

/* ------------------------------------------------------------ to 48 kHz */

/* Stereo s16 at `rate` in, stereo s16 at 48000 out, any rate in between:
 * linear between the samples, carried from one block to the next. */
typedef struct {
    uint32_t rate;
    uint64_t step;            /* input frames per output frame, 32.32 */
    uint64_t pos;             /* the fraction past `prev`, 32.32 */
    int16_t prev[2], cur[2];
    int have;                 /* prev and cur are there */
} mus_rs_t;

void mus_rs_init(mus_rs_t *rs, uint32_t rate);
/* Takes in[0..in_frames), writes up to out_max frames; *used gets the input
 * frames taken (all of them unless out filled up first). Returns the frames
 * written. */
size_t mus_rs_run(mus_rs_t *rs, const int16_t *in, size_t in_frames, size_t *used, int16_t *out, size_t out_max);

/* ------------------------------------------------------------ tags */

#define MUS_TEXT_MAX 128u

typedef struct {
    char title[MUS_TEXT_MAX];
    char artist[MUS_TEXT_MAX];
    char album[MUS_TEXT_MAX];
    char album_artist[MUS_TEXT_MAX];
    char genre[64];
    uint32_t track, disc, year;
    /* The front cover (JPEG or PNG bytes) inside the head given, or 0/0. */
    size_t art_offset, art_len;
} mus_tags_t;

/* From the first head_len bytes of a file of file_size bytes: the tags and
 * the format and length. Returns 0; or the number of bytes of head needed
 * (more than head_len: a large ID3 tag, a cover), to be called again with
 * them; or -1 if it is not a music file. Text is UTF-8. */
int64_t mus_scan(const uint8_t *head, size_t head_len, uint64_t file_size, mus_tags_t *tags, mus_info_t *info);
/* The end of a file, for what mus_scan cannot see from the head: a WAV's
 * LIST INFO after its sound, an MP3's ID3v1 tag (the last 128 bytes). It
 * only fills what is still empty. */
void mus_scan_tail(const uint8_t *tail, size_t len, mus_tags_t *tags);

/* ------------------------------------------------------------ covers */

/* An album without a picture gets one drawn (design/orfeus): a background,
 * a disc and a line in two colours chosen by its name. The same everywhere
 * (Orfeus, Starlight's mini player). */
typedef struct {
    uint32_t bg, fg;          /* 0xRRGGBB */
    uint16_t x, y, s;         /* the disc: left, top and size, in 1/1000 of the side */
} mus_art_t;

void mus_art_for(const char *album, const char *artist, mus_art_t *out);

/* ------------------------------------------------------------ utilities */

/* "4:05", "1:02:03" */
void mus_format_time(char *out, size_t cap, uint64_t ms);
/* A stable 32-bit hash of a string (FNV-1a), for cache names and colours. */
uint32_t mus_hash(const char *s);
#ifdef __cplusplus
}
#endif

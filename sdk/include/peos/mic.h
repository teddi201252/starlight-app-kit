/* The microphone (the home PC plan, 2026-11-22: the Razer headset's and the
 * ALC1220's). A program with the "microphone" capability opens a capture
 * stream: a descriptor that reads 48 kHz, stereo, signed 16-bit frames
 * (little endian, interleaved; a mono microphone gives the same sample on
 * both sides), waited on with PEOS_WAIT_READ: ready when 10 ms or more are
 * there. Nothing runs while no stream is open: the microphone starts with
 * the first one and stops with the last.
 *
 * PEOS_MIC_DEFAULT follows where the sound plays: the USB headset's
 * microphone while one is in, the sound card's otherwise; a stream opened
 * on it moves when the headset is put in or taken out. A stream opened on
 * one microphone (PEOS_MIC_USB, PEOS_MIC_BUILTIN) ends when that one goes:
 * read returns 0 and the descriptor is ready, as at the end of a file.
 *
 * Frames that a reader leaves for more than ~340 ms are dropped (counted in
 * overruns); a read returns whole frames only. */
#pragma once
#include <stdint.h>

#define PEOS_MIC_DEFAULT 0u   /* the USB headset's when one is in, else the sound card's */
#define PEOS_MIC_USB 1u       /* a USB Audio microphone (a headset's) */
#define PEOS_MIC_BUILTIN 2u   /* the sound card's (Intel HDA): its microphone socket */

#define PEOS_MIC_RATE 48000u
#define PEOS_MIC_FRAME_BYTES 4u   /* two signed 16-bit samples */

typedef struct {
    uint32_t present;        /* (1u << PEOS_MIC_USB) | (1u << PEOS_MIC_BUILTIN): those there */
    uint32_t current;        /* what PEOS_MIC_DEFAULT records from now; 0: none */
    uint32_t recording;      /* as present: those running now */
    uint32_t streams;        /* capture streams open */
    uint64_t frames;         /* frames captured since the boot (48 kHz), all microphones */
    uint64_t overruns;       /* frames dropped since the boot: a reader too slow */
    char usb_name[32];       /* "USB 1532:056e" */
    char builtin_name[48];   /* "Realtek ALC1220, rear mic (pin 0x18)" */
    uint32_t usb_rate;       /* the USB microphone's own format (converted to 48 kHz stereo) */
    uint32_t usb_channels;
    uint32_t usb_bits;
    uint32_t _reserved;
    /* 2026-11-22 (Settings, Audio): the volume and the choice. */
    uint32_t volume;         /* 0..100, what every microphone records is multiplied by volume / 50
                                (50: as it comes; 100: twice) */
    uint32_t _reserved2;
    char chosen_name[48];    /* the microphone chosen by hand (peos_audio_choose with
                                PEOS_AUDIO_CHOOSE_INPUT): usb_name, or PEOS_MIC_BUILTIN_NAME; "": automatic */
} peos_mic_info_t;

#define PEOS_MIC_INFO_V1_SIZE 128u   /* up to _reserved */
#define PEOS_MIC_VOLUME_UNITY 50u     /* the volume that leaves the samples as they are */
/* The name the sound card's microphone is chosen by (its own describes the
 * socket, which may change). */
#define PEOS_MIC_BUILTIN_NAME "builtin"

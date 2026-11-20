/* The sound card (docs/system-apps-ux-plan.md, phase 4, Settings: the Audio
 * tab). SYS_AUDIO_INFO tells whether PeOS plays on a card, which one, on
 * which outputs, and the volume. The volume is audio_set_volume, the
 * output peos_audio_set_output (both "audio").
 *
 * The output (phase 5): headphones plugged in take the sound and the
 * speakers (and line outs) go quiet; taken out, the speakers again. With
 * the headphones in, the speakers can be chosen (until they come out).
 * The socket is looked at when a sound starts, every half second while it
 * plays and when this is asked: never while nothing plays. */
#pragma once
#include <stdint.h>

#define PEOS_AUDIO_OUT_SPEAKER 0x1u     /* the computer's speakers */
#define PEOS_AUDIO_OUT_HEADPHONE 0x2u   /* the headphones' socket */
#define PEOS_AUDIO_OUT_LINE 0x4u        /* a line out (speakers of their own) */
#define PEOS_AUDIO_OUT_OTHER 0x8u       /* an output of no known kind */

typedef struct {
    uint32_t present;        /* 1: PeOS plays on a card */
    uint32_t outputs;        /* PEOS_AUDIO_OUT_*: the outputs the card has */
    uint32_t volume;         /* 0..100 */
    uint32_t playing_on;     /* PEOS_AUDIO_OUT_*: where it plays now (0 from a kernel before
                                SLUI SDK 1.0.27: all of outputs) */
    char controller[32];     /* "Intel HD Audio", "Apple T2" */
    char codec[32];          /* "Realtek ALC256"; "" when there is none to name */
    /* SLUI SDK 1.0.27 (a smaller size asks only for the fields above) */
    uint32_t connected;      /* PEOS_AUDIO_OUT_*: those that can play now (the headphones: plugged in) */
    uint32_t chosen;         /* PEOS_AUDIO_OUT_SPEAKER: the speakers chosen with the headphones in;
                                0: the headphones when they are in */
} peos_audio_info_t;

#define PEOS_AUDIO_INFO_V1_SIZE 80u   /* up to codec[] */

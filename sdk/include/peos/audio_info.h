/* The sound card (docs/system-apps-ux-plan.md, phase 4, Settings: the Audio
 * tab). SYS_AUDIO_INFO tells whether PeOS plays on a card, which one, on
 * which outputs, and the volume. Nothing is changed: the volume is
 * audio_set_volume ("audio"). */
#pragma once
#include <stdint.h>

#define PEOS_AUDIO_OUT_SPEAKER 0x1u     /* the computer's speakers */
#define PEOS_AUDIO_OUT_HEADPHONE 0x2u   /* the headphones' socket */
#define PEOS_AUDIO_OUT_LINE 0x4u        /* a line out (speakers of their own) */
#define PEOS_AUDIO_OUT_OTHER 0x8u       /* an output of no known kind */

typedef struct {
    uint32_t present;        /* 1: PeOS plays on a card */
    uint32_t outputs;        /* PEOS_AUDIO_OUT_*: where it plays (all of them at once) */
    uint32_t volume;         /* 0..100 */
    uint32_t reserved;
    char controller[32];     /* "Intel HD Audio", "Apple T2" */
    char codec[32];          /* "Realtek ALC256"; "" when there is none to name */
} peos_audio_info_t;

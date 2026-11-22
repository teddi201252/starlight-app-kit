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
    /* The devices (2026-11-22, Settings, Audio: the outputs and the
     * microphones). The fields above are the computer's card (Intel HDA, the
     * T2) even while a USB device plays; `device` says which one plays. */
    uint32_t devices;        /* PEOS_AUDIO_DEV_*: the devices there */
    uint32_t device;         /* PEOS_AUDIO_DEV_*: the one that plays now (0: none) */
    uint64_t usb_since;      /* when the USB device came: unix seconds (wall clock); 0 none */
    char usb_name[48];       /* the USB device's name: its product ("Razer Kraken V3 X"), else "USB 1532:056e" */
    char chosen_name[48];    /* the device chosen by hand (peos_audio_choose); "": automatic */
} peos_audio_info_t;

#define PEOS_AUDIO_INFO_V1_SIZE 80u   /* up to codec[] */
#define PEOS_AUDIO_INFO_V2_SIZE 88u   /* up to chosen */

/* The devices. A USB device (headphones, speakers, a headset) put in takes
 * the sound; taken out, the card again. One chosen by hand (its name:
 * `controller` for the card, `usb_name` for the USB one) stays while it is
 * there, whatever else comes; when it comes back it is taken again. */
#define PEOS_AUDIO_DEV_CARD 0x1u   /* the computer's card: `controller`, `outputs` */
#define PEOS_AUDIO_DEV_USB 0x2u    /* a USB Audio device: `usb_name` */

/* peos_audio_choose's what */
#define PEOS_AUDIO_CHOOSE_OUTPUT 0u
#define PEOS_AUDIO_CHOOSE_INPUT 1u

/* Every device, one entry each (SYS_AUDIO_DEVICES, 2026-11-22: two USB
 * headsets at once): the outputs (the card, then the USB ones in the order
 * they came), then the microphones (the card's, then the USB ones). With
 * several, the USB one put in last plays (records) unless one is chosen
 * by hand; when it goes, the one before it still there. Two devices of the
 * same name: " 2", " 3" after the second's. */
#define PEOS_AUDIO_DEVF_OUTPUT 0x01u    /* it plays; else it records */
#define PEOS_AUDIO_DEVF_USB 0x02u       /* a USB device; else the card's */
#define PEOS_AUDIO_DEVF_IN_USE 0x04u    /* the one that plays (records) now */
#define PEOS_AUDIO_DEVF_CHOSEN 0x08u    /* chosen by hand (peos_audio_choose) */
#define PEOS_AUDIO_DEVF_RUNNING 0x10u   /* a microphone recording now */
#define PEOS_AUDIO_DEVICES_MAX 12u

typedef struct {
    uint32_t flags;          /* PEOS_AUDIO_DEVF_* */
    uint32_t order;          /* USB: higher came later; 0 the card's */
    uint64_t since;          /* USB: when it came, unix seconds (wall clock); 0 the card's */
    char name[48];           /* what peos_audio_choose takes: the card's controller, the USB product
                                ("Razer BlackShark V2 HS USB"), PEOS_MIC_BUILTIN_NAME for the card's microphone */
} peos_audio_device_t;       /* 64 bytes */

/* SYS_AUDIO_WATCH: a descriptor readable (8 bytes, a change counter) each
 * time a device comes or goes, or a choice or the microphone's volume
 * changes; readable at once the first time. */

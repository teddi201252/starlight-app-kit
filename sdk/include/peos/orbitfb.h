#pragma once
#include <stdint.h>

#define PEOS_ORBITFB_SHM_MAGIC 0x4F464231u
#define PEOS_ORBITFB_SHM_VERSION 1u
#define PEOS_ORBITFB_SHM_SLOTS 3u
#define PEOS_ORBITFB_SHM_HEADER_BYTES 256u
#define PEOS_ORBITFB_FORMAT_XRGB8888 1u
#define PEOS_ORBITFB_FORMAT_RGB565 2u

typedef struct {
    volatile uint32_t magic;
    volatile uint32_t version;
    volatile uint32_t slot_count;
    volatile uint32_t slot_size;
    volatile uint32_t width;
    volatile uint32_t height;
    volatile uint32_t pitch;
    volatile uint32_t format;
    volatile uint32_t ready_seq;
    volatile uint32_t ready_slot;
    volatile uint32_t reserved[54];
} peos_orbitfb_shm_t;

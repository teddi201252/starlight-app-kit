#pragma once

#include <stdint.h>
#include <stddef.h>

// WAV parser and conversion helpers for PeOS userland.

// Common WAV format tags.
#define WAV_FMT_PCM         0x0001u
#define WAV_FMT_MS_ADPCM    0x0002u
#define WAV_FMT_IEEE_FLOAT  0x0003u
#define WAV_FMT_ALAW        0x0006u
#define WAV_FMT_MULAW       0x0007u
#define WAV_FMT_IMA_ADPCM   0x0011u
#define WAV_FMT_EXTENSIBLE  0xFFFEu
#define WAV_MSADPCM_MAX_COEFFS 32u

typedef struct {
    uint16_t audio_format;   // WAV_FMT_*
    uint32_t sample_rate;
    uint16_t channels;
    uint16_t bits_per_sample;
    uint32_t byte_rate;
    uint16_t block_align;
    uint16_t valid_bits_per_sample; // 0 if unknown/not provided
    uint32_t channel_mask;          // extensible layout mask (optional)
    uint16_t samples_per_block;     // ADPCM: decoded samples per block
    uint16_t ms_num_coeffs;         // MS ADPCM coefficient count
    int16_t ms_coeff1[WAV_MSADPCM_MAX_COEFFS];
    int16_t ms_coeff2[WAV_MSADPCM_MAX_COEFFS];
} wav_format_t;

typedef struct {
    wav_format_t fmt;
    const uint8_t *data;
    uint32_t data_size;
    uint32_t frames; // data_size / block_align
} wav_file_t;

// Parse a WAV file and extract format/data.
// Returns 0 on success, -1 on error.
int wav_parse(const void *file_data, size_t file_size, wav_file_t *out);

// Convert parsed WAV samples to the device-native format used by PeOS now:
// 48 kHz, stereo, signed 16-bit little-endian interleaved.
// Allocates *out_pcm with malloc; caller must free().
// Returns 0 on success, -1 on unsupported/invalid format.
int wav_convert_to_s16le_48k_stereo(const wav_file_t *wav, int16_t **out_pcm, uint32_t *out_frames);

// Write PCM samples to /dev/audio0 for playback.
// Returns bytes written or -1 on error.
size_t audio_write(const void *samples, size_t len);
size_t audio_stream_write(const void *samples, size_t len);
size_t audio_stream_avail(void);
int audio_set_volume(uint32_t volume);

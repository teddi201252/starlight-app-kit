#pragma once

#include "SDL.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MIX_MAX_VOLUME 128
#define MIX_CHANNEL_POST (-2)

typedef struct Mix_Chunk {
    int allocated;
    Uint8 *abuf;
    Uint32 alen;
    Uint8 volume;
} Mix_Chunk;

typedef struct Mix_Music Mix_Music;
typedef struct Mix_RWData {
    const Uint8 *mem;
    Uint32 size;
} Mix_RWData;
typedef void (*Mix_EffectFunc_t)(int channel, void *stream, int len, void *udata);
typedef void (*Mix_EffectDone_t)(int channel, void *udata);
typedef void (*Mix_HookMusicFunc_t)(void *udata, Uint8 *stream, int len);

int Mix_OpenAudio(int frequency, Uint16 format, int channels, int chunksize);
int Mix_OpenAudioDevice(int frequency, Uint16 format, int channels, int chunksize, const char *device, int allowed_changes);
void Mix_CloseAudio(void);
int Mix_QuerySpec(int *frequency, Uint16 *format, int *channels);
const char *Mix_GetError(void);
const SDL_version *Mix_Linked_Version(void);

int Mix_AllocateChannels(int numchans);
int Mix_PlayChannelTimed(int channel, Mix_Chunk *chunk, int loops, int ticks);
int Mix_HaltChannel(int channel);
int Mix_Playing(int channel);
int Mix_SetPanning(int channel, Uint8 left, Uint8 right);
int Mix_UnregisterAllEffects(int channel);

int Mix_SetMusicCMD(const char *command);
int Mix_RegisterEffect(int channel, Mix_EffectFunc_t f, Mix_EffectDone_t d, void *arg);
void Mix_HookMusic(Mix_HookMusicFunc_t mix_func, void *arg);
Mix_Music *Mix_LoadMUS(const char *file);
Mix_Music *Mix_LoadMUS_RW(void *src, int freesrc);
void Mix_FreeMusic(Mix_Music *music);
int Mix_PlayMusic(Mix_Music *music, int loops);
int Mix_HaltMusic(void);
int Mix_PlayingMusic(void);
int Mix_SetMusicPosition(double position);
int Mix_VolumeMusic(int volume);

#ifdef __cplusplus
}
#endif

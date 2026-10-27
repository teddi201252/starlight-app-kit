#pragma once

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef uint8_t Uint8;
typedef uint16_t Uint16;
typedef uint32_t Uint32;
typedef uint64_t Uint64;
typedef int8_t Sint8;
typedef int16_t Sint16;
typedef int32_t Sint32;
typedef int64_t Sint64;

typedef int SDL_bool;
#define SDL_FALSE 0
#define SDL_TRUE 1

#if defined(__GNUC__) || defined(__clang__)
#define SDL_Swap16(X) __builtin_bswap16((Uint16)(X))
#define SDL_Swap32(X) __builtin_bswap32((Uint32)(X))
#else
#define SDL_Swap16(X) (Uint16)((((Uint16)(X) & 0x00FFu) << 8) | (((Uint16)(X) & 0xFF00u) >> 8))
#define SDL_Swap32(X) (Uint32)((((Uint32)(X) & 0x000000FFu) << 24) | \
                               (((Uint32)(X) & 0x0000FF00u) << 8)  | \
                               (((Uint32)(X) & 0x00FF0000u) >> 8)  | \
                               (((Uint32)(X) & 0xFF000000u) >> 24))
#endif

#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#define SDL_SwapBE16(X) ((Uint16)(X))
#define SDL_SwapBE32(X) ((Uint32)(X))
#else
#define SDL_SwapBE16(X) SDL_Swap16((X))
#define SDL_SwapBE32(X) SDL_Swap32((X))
#endif

typedef Sint32 SDL_Keycode;

#define SDLK_UNKNOWN 0
#define SDLK_RETURN '\r'
#define SDLK_ESCAPE 27
#define SDLK_BACKSPACE '\b'
#define SDLK_TAB '\t'
#define SDLK_SPACE ' '
#define SDLK_PLUS '+'
#define SDLK_MINUS '-'
#define SDLK_EQUALS '='

#define SDLK_LEFT 0x40000050
#define SDLK_RIGHT 0x4000004F
#define SDLK_UP 0x40000052
#define SDLK_DOWN 0x40000051
#define SDLK_LCTRL 0x400000E0
#define SDLK_RCTRL 0x400000E4
#define SDLK_LSHIFT 0x400000E1
#define SDLK_RSHIFT 0x400000E5
#define SDLK_LALT 0x400000E2
#define SDLK_RALT 0x400000E6
#define SDLK_F1 0x4000003A
#define SDLK_F2 0x4000003B
#define SDLK_F3 0x4000003C
#define SDLK_F4 0x4000003D
#define SDLK_F5 0x4000003E
#define SDLK_F6 0x4000003F
#define SDLK_F7 0x40000040
#define SDLK_F8 0x40000041
#define SDLK_F9 0x40000042
#define SDLK_F10 0x40000043
#define SDLK_F11 0x40000044
#define SDLK_F12 0x40000045

#define SDL_WINDOWPOS_UNDEFINED 0
#define SDL_WINDOW_SHOWN (1u << 0)
#define SDL_RENDERER_ACCELERATED (1u << 1)
#define SDL_TEXTUREACCESS_STATIC 0
#define SDL_TEXTUREACCESS_STREAMING 1
#define SDL_TEXTUREACCESS_TARGET 2
#define SDL_PIXELFORMAT_RGB888 0x16161804u

#define SDL_INIT_TIMER 0x00000001u
#define SDL_INIT_AUDIO 0x00000010u
#define SDL_INIT_VIDEO 0x00000020u
#define SDL_INIT_CDROM 0x00000100u
#define SDL_INIT_JOYSTICK 0x00000200u
#define SDL_INIT_EVENTS 0x00004000u
#define SDL_INIT_EVERYTHING                                                       \
    (SDL_INIT_TIMER | SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_CDROM |         \
     SDL_INIT_JOYSTICK | SDL_INIT_EVENTS)

#define SDL_DISABLE 0
#define SDL_ENABLE 1
#define SDL_QUERY -1

#define SDL_PRESSED 1u
#define SDL_RELEASED 0u

#define SDL_FIRSTEVENT 0
#define SDL_QUIT 0x100
#define SDL_KEYDOWN 0x300
#define SDL_KEYUP 0x301
#define SDL_MOUSEMOTION 0x400

typedef struct SDL_Window SDL_Window;
typedef struct SDL_Renderer SDL_Renderer;
typedef struct SDL_Texture SDL_Texture;
typedef struct SDL_Joystick SDL_Joystick;

typedef struct SDL_version {
    Uint8 major;
    Uint8 minor;
    Uint8 patch;
} SDL_version;

#define SDL_MAJOR_VERSION 2
#define SDL_MINOR_VERSION 0
#define SDL_PATCHLEVEL 0
#define SDL_VERSIONNUM(X, Y, Z) ((X) * 1000 + (Y) * 100 + (Z))
#define SDL_VERSION_ATLEAST(X, Y, Z)                                              \
    (SDL_VERSIONNUM(SDL_MAJOR_VERSION, SDL_MINOR_VERSION, SDL_PATCHLEVEL) >=      \
     SDL_VERSIONNUM((X), (Y), (Z)))

typedef struct SDL_Rect {
    int x;
    int y;
    int w;
    int h;
} SDL_Rect;

typedef struct SDL_Keysym {
    SDL_Keycode sym;
} SDL_Keysym;

typedef struct SDL_KeyboardEvent {
    Uint32 type;
    Uint32 timestamp;
    Uint32 windowID;
    Uint8 state;
    Uint8 repeat;
    Uint8 padding2;
    Uint8 padding3;
    SDL_Keysym keysym;
} SDL_KeyboardEvent;

typedef union SDL_Event {
    Uint32 type;
    SDL_KeyboardEvent key;
    Uint8 padding[64];
} SDL_Event;

typedef Uint16 SDL_AudioFormat;
typedef Uint32 SDL_AudioDeviceID;

#define AUDIO_U8 0x0008u
#define AUDIO_S16LSB 0x8010u
#define AUDIO_S16SYS AUDIO_S16LSB
#define SDL_AUDIO_ALLOW_FREQUENCY_CHANGE 0x00000001
#define SDL_MIX_MAXVOLUME 128

typedef void (*SDL_AudioCallback)(void *userdata, Uint8 *stream, int len);
typedef struct SDL_mutex SDL_mutex;
typedef struct SDL_cond SDL_cond;

typedef struct SDL_AudioSpec {
    int freq;
    SDL_AudioFormat format;
    Uint8 channels;
    Uint8 silence;
    Uint16 samples;
    Uint16 padding;
    Uint32 size;
    SDL_AudioCallback callback;
    void *userdata;
} SDL_AudioSpec;

typedef struct SDL_AudioCVT {
    int needed;
    SDL_AudioFormat src_format;
    SDL_AudioFormat dst_format;
    double rate_incr;
    Uint8 *buf;
    int len;
    int len_cvt;
    int len_mult;
    double len_ratio;
} SDL_AudioCVT;

typedef struct SDL_CDtrack {
    Uint8 id;
    Uint8 type;
    Uint16 unused;
    Uint32 length;
    Uint32 offset;
} SDL_CDtrack;

typedef struct SDL_CD {
    int id;
    int status;
    int numtracks;
    int cur_track;
    int cur_frame;
    SDL_CDtrack track[100];
} SDL_CD;

#define SDL_AUDIO_TRACK 0x00u
#define SDL_DATA_TRACK 0x04u
#define CD_TRAYEMPTY 0
#define CD_STOPPED 1
#define CD_PLAYING 2
#define CD_PAUSED 3
#define CD_ERROR -1
#define CD_FPS 75
#define CD_INDRIVE(status) ((status) > 0)

#define SDL_HAT_CENTERED 0x00u
#define SDL_HAT_UP 0x01u
#define SDL_HAT_RIGHT 0x02u
#define SDL_HAT_DOWN 0x04u
#define SDL_HAT_LEFT 0x08u

int SDL_Init(Uint32 flags);
int SDL_InitSubSystem(Uint32 flags);
Uint32 SDL_WasInit(Uint32 flags);
void SDL_QuitSubSystem(Uint32 flags);
void SDL_Quit(void);

const char *SDL_GetError(void);
int SDL_SetError(const char *fmt, ...);
void SDL_ClearError(void);
void SDL_GetVersion(SDL_version *ver);
const SDL_version *SDL_Linked_Version(void);

SDL_Window *SDL_CreateWindow(const char *title, int x, int y, int w, int h, Uint32 flags);
void SDL_DestroyWindow(SDL_Window *window);
void SDL_SetWindowTitle(SDL_Window *window, const char *title);

SDL_Renderer *SDL_CreateRenderer(SDL_Window *window, int index, Uint32 flags);
void SDL_DestroyRenderer(SDL_Renderer *renderer);
int SDL_RenderClear(SDL_Renderer *renderer);
int SDL_RenderCopy(SDL_Renderer *renderer, SDL_Texture *texture, const SDL_Rect *srcrect, const SDL_Rect *dstrect);
void SDL_RenderPresent(SDL_Renderer *renderer);

SDL_Texture *SDL_CreateTexture(SDL_Renderer *renderer, Uint32 format, int access, int w, int h);
void SDL_DestroyTexture(SDL_Texture *texture);
int SDL_UpdateTexture(SDL_Texture *texture, const SDL_Rect *rect, const void *pixels, int pitch);

int SDL_PollEvent(SDL_Event *event);
int SDL_PushEvent(SDL_Event *event);
void SDL_PumpEvents(void);

void SDL_Delay(Uint32 ms);
Uint32 SDL_GetTicks(void);
Uint64 SDL_GetPerformanceCounter(void);
Uint64 SDL_GetPerformanceFrequency(void);

int SDL_OpenAudio(SDL_AudioSpec *desired, SDL_AudioSpec *obtained);
void SDL_CloseAudio(void);
void SDL_PauseAudio(int pause_on);
void SDL_LockAudio(void);
void SDL_UnlockAudio(void);
SDL_mutex *SDL_CreateMutex(void);
void SDL_DestroyMutex(SDL_mutex *mutex);
int SDL_LockMutex(SDL_mutex *mutex);
int SDL_UnlockMutex(SDL_mutex *mutex);
SDL_cond *SDL_CreateCond(void);
void SDL_DestroyCond(SDL_cond *cond);
int SDL_CondSignal(SDL_cond *cond);
int SDL_CondWait(SDL_cond *cond, SDL_mutex *mutex);
void SDL_MixAudioFormat(Uint8 *dst, const Uint8 *src, SDL_AudioFormat format, Uint32 len, int volume);

SDL_AudioDeviceID SDL_OpenAudioDevice(const char *device, int iscapture,
                                      const SDL_AudioSpec *desired,
                                      SDL_AudioSpec *obtained, int allowed_changes);
void SDL_CloseAudioDevice(SDL_AudioDeviceID dev);
void SDL_PauseAudioDevice(SDL_AudioDeviceID dev, int pause_on);
void SDL_LockAudioDevice(SDL_AudioDeviceID dev);
void SDL_UnlockAudioDevice(SDL_AudioDeviceID dev);
int SDL_QueueAudio(SDL_AudioDeviceID dev, const void *data, Uint32 len);
Uint32 SDL_GetQueuedAudioSize(SDL_AudioDeviceID dev);
void SDL_ClearQueuedAudio(SDL_AudioDeviceID dev);
Uint32 SDL_DequeueAudio(SDL_AudioDeviceID dev, void *data, Uint32 len);

int SDL_NumJoysticks(void);
const char *SDL_JoystickName(int device_index);
const char *SDL_JoystickNameForIndex(int device_index);
SDL_Joystick *SDL_JoystickOpen(int device_index);
void SDL_JoystickClose(SDL_Joystick *joystick);
int SDL_JoystickNumAxes(SDL_Joystick *joystick);
int SDL_JoystickNumHats(SDL_Joystick *joystick);
int SDL_JoystickGetButton(SDL_Joystick *joystick, int button);
Sint16 SDL_JoystickGetAxis(SDL_Joystick *joystick, int axis);
Uint8 SDL_JoystickGetHat(SDL_Joystick *joystick, int hat);
int SDL_JoystickEventState(int state);

int SDL_CDNumDrives(void);
const char *SDL_CDName(int drive);
SDL_CD *SDL_CDOpen(int drive);
int SDL_CDStatus(SDL_CD *cdrom);
int SDL_CDPlayTracks(SDL_CD *cdrom, int start_track, int start_frame, int ntracks, int nframes);
int SDL_CDStop(SDL_CD *cdrom);
int SDL_CDResume(SDL_CD *cdrom);
void SDL_CDClose(SDL_CD *cdrom);

#ifdef __cplusplus
}
#endif

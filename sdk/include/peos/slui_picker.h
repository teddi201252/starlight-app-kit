/*
 * Choose a file (SLUI SDK 1.0.18; docs/system-apps-ux-plan.md, phase 3;
 * design/scegli-un-file/): one window for every app that opens or saves a
 * file. It is drawn inside the app's window, over everything, with the
 * veil: the places on the left (the system, the folders that exist, the
 * disks), the folder's path with "up", the files of the kind asked for
 * (a list, or thumbnails for pictures), the name, Cancel and the action.
 *
 *     slui_picker_t *p = slui_picker_open(&opts);      // when asked
 *     ... in the frame, last:      slui_picker_draw(p, body_w, body_h);
 *     ... every event, first:      r = slui_picker_event(p, &ev);
 *     r == SLUI_PICK_CHOSEN: slui_picker_path(p), then slui_picker_close(p)
 *     r == SLUI_PICK_CANCEL: slui_picker_close(p)
 *
 * Keys: the arrows choose, Enter opens (a folder: goes in), Backspace the
 * folder above (while the name is not being typed), Esc cancels. A double
 * click is Enter. The sentences are the system's (/system/lang/<language>),
 * in the language Starlight told the app.
 */
#ifndef PEOS_SLUI_PICKER_H
#define PEOS_SLUI_PICKER_H

#include <stdint.h>

#include "peos/starlight_ui.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    SLUI_PICK_OPEN = 0,   // an existing file
    SLUI_PICK_SAVE = 1,   // a name in a folder (a file there is replaced only after asking)
} slui_pick_mode_t;

typedef enum {
    SLUI_PICK_NONE = 0,     // still choosing (the event was the picker's, or nothing)
    SLUI_PICK_CANCEL = 1,
    SLUI_PICK_CHOSEN = 2,   // slui_picker_path
} slui_pick_result_t;

typedef struct {
    slui_pick_mode_t mode;
    const char *title;        // "Apri un file"; NULL: the system's "Open a file" / "Save as"
    const char *action;       // the main button ("Apri", "Scegli"); NULL: the system's
    const char *kind;         // what is asked ("Testi"); NULL with exts NULL: every file
    const char *exts;         // ".txt .md .cfg", lower case, space apart; NULL: every file
    const char *dir;          // where to start; NULL or missing: the first place
    const char *name;         // SAVE: the name proposed
    uint8_t thumbs;           // pictures: thumbnails instead of a list
    const char *place_label;  // one place of the app's, first ("Sfondi di sistema"), or NULL
    const char *place_path;
} slui_picker_opts_t;

typedef struct slui_picker slui_picker_t;

/* NULL when out of memory. */
slui_picker_t *slui_picker_open(const slui_picker_opts_t *opts);
void slui_picker_draw(slui_picker_t *p, uint32_t body_w, uint32_t body_h);
slui_pick_result_t slui_picker_event(slui_picker_t *p, const slui_event_t *ev);
/* The file chosen (an absolute path), valid until slui_picker_close. */
const char *slui_picker_path(const slui_picker_t *p);
/* The folder it was in: to start there the next time. */
const char *slui_picker_dir(const slui_picker_t *p);
void slui_picker_close(slui_picker_t *p);

/* Ids the picker uses for its controls: an app keeps its own below this. */
#define SLUI_PICKER_ID_BASE 0x7F000000u

#ifdef __cplusplus
}
#endif

#endif

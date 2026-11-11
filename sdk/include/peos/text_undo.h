// A text field's undo history (release 0.14.0, phase 3; SLUI SDK 1.0.14):
// the states of the field with the caret, to go back to (undo) and forward
// again (redo). Starlight's fields and Pegasus's use it; an app with pixels
// of its own can too, for SLUI_CLIP_UNDO and SLUI_CLIP_REDO (Caps+Z, Caps+Y).
// Letters typed one after the other (no space, pauses under 2 s) are one
// step. Pure (tests/host/test_text_undo.c).
//
//   each pass of the field:  peos_undo_sync(&u, id, text, caret);
//                            ...the keys change text...
//                            peos_undo_record(&u, id, text, caret, peos_time_ns());
//   Caps+Z:                  peos_undo_apply(&u, id, text, &len, cap, &caret, 0);
#ifndef PEOS_TEXT_UNDO_H
#define PEOS_TEXT_UNDO_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define PEOS_UNDO_MAX 32u
#define PEOS_UNDO_TEXT_MAX 65536u           // a longer text keeps no history
#define PEOS_UNDO_TYPING_NS 2000000000ull

typedef struct {
    char *text;
    uint32_t caret;
} peos_undo_step_t;

typedef struct peos_text_undo {
    uint32_t id;                          // the field the history is of
    char *last;                           // the field as it was after its last pass
    uint32_t last_caret;
    peos_undo_step_t undo[PEOS_UNDO_MAX], redo[PEOS_UNDO_MAX];
    uint32_t nundo, nredo;
    uint32_t typing_end;                  // where letters typed last ended, UINT32_MAX: none
    uint64_t typing_ns;
} peos_text_undo_t;

// Before a pass of field id: the history becomes this field's (another
// field's is dropped) and learns the text if it changed from outside.
void peos_undo_sync(peos_text_undo_t *u, uint32_t id, const char *text, uint32_t caret);
// After the pass: a change becomes a step, or joins the letters typed before.
void peos_undo_record(peos_text_undo_t *u, uint32_t id, const char *text, uint32_t caret, uint64_t now_ns);
// Undo (redo: 1): text (cap bytes) takes the step back; *caret where it was.
// 1: it changed.
uint8_t peos_undo_apply(peos_text_undo_t *u, uint32_t id, char *text, uint32_t *len, uint32_t cap, uint32_t *caret,
                      uint8_t redo);
uint8_t peos_undo_can(const peos_text_undo_t *u, uint32_t id, uint8_t redo);
// What comes next is a step of its own, not letters joining the ones typed
// before (a paste: call it before and after recording it).
void peos_undo_break(peos_text_undo_t *u);
void peos_undo_clear(peos_text_undo_t *u);

#ifdef __cplusplus
}
#endif
#endif

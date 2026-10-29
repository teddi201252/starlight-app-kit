# API reference

Everything a Starlight app can use, as of SDK 1.7.0 (SLUI SDK 1.0.6). The
headers in `sdk/include/peos/` are the source of truth; this page says what
each part is for and shows it in use.

- [The shape of an app](#the-shape-of-an-app)
- [app.ini](#appini)
- [Capabilities and paths](#capabilities-and-paths)
- [SLUI: frames and layout](#slui-frames-and-layout)
- [SLUI: controls](#slui-controls)
- [SLUI: drawing](#slui-drawing)
- [SLUI: text and fonts](#slui-text-and-fonts)
- [SLUI: theme colours](#slui-theme-colours)
- [SLUI: events](#slui-events)
- [SLUI: helpers](#slui-helpers)
- [Languages](#languages)
- [libpeos: files and folders](#libpeos-files-and-folders)
- [libpeos: time](#libpeos-time)
- [libpeos: notifications](#libpeos-notifications)
- [libpeos: network](#libpeos-network)
- [libpeos: system information](#libpeos-system-information)
- [Installing an app in a VM](#installing-an-app-in-a-vm)

## The shape of an app

An app is a program (`app.bin`) that Starlight starts in a window. It does not
draw pixels: every frame it sends Starlight a list of things to draw (labels,
buttons, surfaces...), and it gets back events (a click, a slider moved, the
window resized). Starlight draws them in its own style, so every app looks
like the system.

The loop is always the same: draw when something changed, then read events.

```c
#include "peos/peos.h"
#include "peos/starlight_ui.h"

#define ID_HELLO 1u

int main(void) {
    uint32_t clicks = 0;
    uint8_t dirty = 1;
    for (;;) {
        if (dirty) {
            uint32_t w = 0, h = 0;
            slui_viewport(&w, &h);          // 0 x 0 until the first SLUI_EVENT_RESIZE
            if (w < 300) w = 300;
            slui_begin_frame();
            slui_label_theme(20, 20, SLUI_COLOR_TEXT_PRIMARY, clicks ? "Clicked!" : "Hello");
            slui_button_primary(ID_HELLO, 20, 60, w - 40, 44, 1, "Click me");
            slui_end_frame();
            dirty = 0;
        }
        slui_event_t ev;
        uint8_t got = 0;
        while (slui_poll_event(&ev)) {
            got = 1;
            dirty = 1;                       // a resize or a click: draw again
            if (ev.type == SLUI_EVENT_BUTTON && ev.id == ID_HELLO) clicks++;
        }
        if (!got) peos_sleep(10);            // nothing to do: give the CPU back
    }
}
```

Rules worth knowing:

- **Draw only when the state changed.** A frame is a message to Starlight;
  sending one per loop wastes the CPU of both.
- **Every frame is complete.** What is not in the last frame is not on screen.
- **Ids** (the `id` of controls) are yours: any non-zero number, unique within
  a frame. Events carry them back.
- **Coordinates** are pixels from the top left of the window's body (under
  the title bar).
- `main` may return or call `peos_exit(code)`; closing the window ends the
  process.

## app.ini

The bundle's manifest, `<id>.app/app.ini`. `#` and `;` start comments.

| Key | Meaning | Example |
|---|---|---|
| `title` | The name in the dock, the title bar and the app manager. | `title=Calculator` |
| `category` | `sistema`, `strumenti` or `svago`: the app manager's tabs. | `category=strumenti` |
| `exec` | The program, relative to the bundle. | `exec=./app.bin` |
| `args` | Its arguments, one string. | `args=--compact` |
| `capabilities` | What it may do (below). Without the key: `fs.read` only. | `capabilities=fs.read, network` |
| `paths` | Which parts of the file system, read or read-write (below). Without the key: all of it, read only. | `paths=/:r, /home:rw` |
| `icon` | The dock icon: a PNG, or `res:<key>` for a `[resources]` entry. Without one, the title's first letter. | `icon=res:icon` |
| `width`, `height` | The body's size when the window opens. | `width=560` |
| `min_width`, `min_height` | How small the user can make it. | `min_width=400` |
| `max_width`, `max_height` | How large. | `max_width=1200` |
| `resizable` | `0` or `no`: a fixed size. | `resizable=0` |
| `dock` | `0` or `no`: not in the dock until the user pins it (the app manager lists it). | `dock=no` |

The `[resources]` section names files of the bundle, `key=./path`:

```ini
title=Calculator
category=strumenti
exec=./app.bin
capabilities=fs.read
paths=/:r
icon=res:icon
width=360
height=480
min_width=320
min_height=420

[resources]
icon=./resources/icon.png
```

A manifest with an unknown capability or a malformed `paths` is refused: the
app does not show up (see [troubleshooting](troubleshooting.md)).

## Capabilities and paths

An app gets the capabilities its manifest asks for, and no more. A call that
needs one it lacks fails (and the kernel logs `[CAP] denied`).

| Capability | Allows |
|---|---|
| `fs.read` | Reading files and folders (within `paths`). |
| `fs.write` | Creating, writing, renaming and removing them (within `paths` marked `rw`). |
| `network` | The network: TCP, UDP, DNS, ping (`peos/net.h`). |
| `audio` | Playing sound (`peos/audio.h`). |
| `process` | Starting other programs and threads. |
| `shm` | Shared memory. |
| `display` | The framebuffer directly (Starlight's own business: apps draw through SLUI). |
| `input` | Raw keyboard and mouse (likewise: apps get keys as SLUI events). |
| `mount` | Mounting file systems. |
| `power` | Shutting down and restarting. |
| `install` | Installing PeOS on a disk. |
| `manage` | Ending other processes. |
| `debug` | Kernel counters and test hooks. |
| `service` | Registering a name in the service registry. |
| `nic` | Raw Ethernet frames (netd only). |

`paths` lists absolute prefixes with `r`, `w` or `rw`, separated by commas:

```ini
capabilities=fs.read, fs.write
paths=/:r, /home/notes:rw
```

Ask for the least that works: a user reading the manifest should be able to
tell what the app can touch.

## SLUI: frames and layout

```c
void slui_begin_frame(void);
void slui_end_frame(void);
void slui_viewport(uint32_t *w, uint32_t *h);
void slui_clip_push(uint32_t x, uint32_t y, uint32_t w, uint32_t h);
void slui_clip_pop(void);
void slui_focus(uint32_t id);
```

- Everything is drawn between `slui_begin_frame` and `slui_end_frame`, in
  order: later items are on top.
- `slui_viewport` is the body's size from the last `SLUI_EVENT_RESIZE`
  (0 x 0 before the first one: lay out for `app.ini`'s `width` x `height`).
  Resizable windows send it on every resize; lay out from it every frame.
- `slui_clip_push` limits drawing to a rectangle (nested clips intersect);
  `slui_clip_pop` undoes the last one.
- `slui_focus(id)` gives the keyboard focus to a control (a text field when a
  dialog opens).

Two layout helpers place rectangles without arithmetic:

```c
slui_stack_t st;
slui_stack_begin(&st, 20, 20, 300, 400, SLUI_STACK_VERTICAL, 12);   // spacing 12
uint32_t x, y, w, h;
if (slui_stack_next(&st, 44, &x, &y, &w, &h)) slui_button(1, x, y, w, h, "First");
if (slui_stack_next(&st, 44, &x, &y, &w, &h)) slui_button(2, x, y, w, h, "Second");

slui_grid_t g;
slui_grid_begin(&g, 20, 20, 300, 300, 3, 3, 8, 8);   // 3 x 3, gaps 8
for (uint32_t i = 0; i < 9; i++) {
    if (slui_grid_next(&g, &x, &y, &w, &h)) slui_button(10 + i, x, y, w, h, "·");
}
```

`slui_grid_cell(&g, col, row, ...)` gives one cell directly.

## SLUI: controls

Every control has an `id`; the `_state` variants add `enabled` (0 greys it
out and ignores clicks). Text is UTF-8.

| Function | What | Event |
|---|---|---|
| `slui_button(id, x, y, w, h, text)` | A raised button. | `BUTTON` |
| `slui_button_primary(id, x, y, w, h, enabled, text)` | The group's main button (Save, OK): night, neon label. | `BUTTON` |
| `slui_swatch(id, x, y, size, r, g, b, chosen, enabled)` | A round colour choice. | `BUTTON` |
| `slui_segment(id, x, y, w, h, count, selected, enabled, "A\|B\|C")` | A segmented control. | `SELECT` (index) |
| `slui_toggle(id, x, y, w, h, value, text)` | A switch. | `TOGGLE` (0/1) |
| `slui_slider_u32(id, x, y, w, min, max, value)` | A slider. | `SLIDER` (value) |
| `slui_select(id, x, y, w, h, count, selected, "A\|B\|C")` | A drop-down list. | `SELECT` (index) |
| `slui_text_input(id, x, y, w, h, max_chars, text)` | A one-line field. | `TEXT_INPUT` (text) |
| `slui_text_area(id, x, y, w, h, max_chars, text)` | A multi-line field. | `TEXT_AREA` (text) |
| `slui_scroll_v(id, x, y, w, h, content_h, offset)` | A vertical scroll bar. | `SCROLL_V` (offset) |
| `slui_list_item(id, x, y, w, h, enabled, title, subtitle)` | A row of a list. | `BUTTON`, `DOUBLE_CLICK` |
| `slui_context_menu(id, x, y, w, h, count, "Copy\|Paste")` | A right-click menu over an area. | `CONTEXT_MENU` (index) |
| `slui_panel_input(id, x, y, w, h, enabled)` | An invisible area that reports where it is pressed. | `PANEL_INPUT` |

Controls show the value you pass: when the user moves a slider, you get the
event, store the new value, and draw the slider with it in the next frame.

```c
static uint32_t volume = 40;
static uint32_t view = 0;
// in the frame:
slui_segment(ID_VIEW, 20, 20, 240, 44, 2, view, 1, "Day|Week");
slui_slider_u32(ID_VOLUME, 20, 90, 240, 0, 100, volume);
// in the event loop:
if (ev.type == SLUI_EVENT_SLIDER && ev.id == ID_VOLUME) volume = ev.value;
if (ev.type == SLUI_EVENT_SELECT && ev.id == ID_VIEW) view = ev.value;
```

## SLUI: drawing

```c
void slui_surface(slui_surface_kind_t kind, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t radius);
void slui_rect(x, y, w, h, r, g, b);
void slui_rect_a(x, y, w, h, r, g, b, a);
void slui_rect_theme(x, y, w, h, token);
void slui_rect_a_theme(x, y, w, h, token, a);
void slui_gradient_fill_2d(x, y, w, h, /* four corners, RGBA each */ ...);
void slui_separator(x, y, w);
void slui_ring(x, y, size, thick, permille, track_r, track_g, track_b, fill_r, fill_g, fill_b);
void slui_graph(x, y, w, h, const uint16_t *values, count, line_r, line_g, line_b, fill_r, fill_g, fill_b, fill_a);
void slui_veil(uint8_t alpha);
void slui_image_file(x, y, w, h, const char *path);
```

- `slui_surface` draws Starlight's soft surfaces, with their shadows:
  `SLUI_SURF_RAISED` (cards, controls at rest), `RAISED_HI` (hovered,
  chosen), `INSET` (wells, fields), `FLAT`, `NIGHT` (the dark cards),
  `NIGHT_GLOW`, `NIGHT_FLAT`, `NEON` (a lit dot or bar). `radius` rounds the
  corners.
- `slui_ring`: a ring of outer diameter `size`, filled clockwise from the top
  to `permille` (0..1000).
- `slui_graph`: up to 256 values of 0..1000 spread over `w`, an area under a
  line.
- `slui_veil`: a modal dialog's backdrop, over the whole window including the
  title bar; draw the dialog after it.
- `slui_image_file`: a PNG from the file system, scaled into the rectangle.

```c
// a night card with a ring at 72% and a graph of the last values
slui_surface(SLUI_SURF_NIGHT, 0, 0, 300, 180, 26);
slui_ring(20, 30, 110, 11, 720, 0x1C, 0x2A, 0x4A, 0x59, 0xD0, 0xFF);
slui_graph(150, 30, 130, 120, history, 24, 0x59, 0xD0, 0xFF, 0x1C, 0x2A, 0x4A, 255);
```

### Pixels of your own

Since SLUI SDK 1.0.7 (PeOS 0.11) an app can draw its own pixels in part of
its window, beside the SLUI controls: a page, a picture being made, a game
board. It needs the `shm` capability.

```c
void slui_pixels_open(uint32_t w, uint32_t h);
uint32_t *slui_pixels_buffer(uint32_t *w, uint32_t *h, uint32_t *pitch);
void slui_pixels(x, y, w, h, uint32_t sx, uint32_t sy);
void slui_pixels_damage(x, y, w, h);
```

- `slui_pixels_open` asks Starlight for a surface of `w` x `h` pixels; it is
  ready when `SLUI_EVENT_PIXELS` comes (`value` x `value2`, 0 x 0 if there
  was no room). Ask again, larger, when the window grows.
- `slui_pixels_buffer`: its rows, `pitch` bytes apart, each pixel
  `0x00RRGGBB`; `NULL` until the event came.
- `slui_pixels`, in a frame: the part from `sx`,`sy` shown at `x`,`y` of the
  body, `w` x `h`; what is drawn after it goes on top.
- `slui_pixels_damage`: the pixels changed there; Starlight shows them
  without a new frame (a page arriving, an animation).

```c
slui_pixels_open(400, 300);                        // once
// ... SLUI_EVENT_PIXELS came:
uint32_t w, h, pitch;
uint32_t *px = slui_pixels_buffer(&w, &h, &pitch);
for (uint32_t y = 0; y < h; y++)
    for (uint32_t x = 0; x < w; x++)
        ((uint32_t *)((uint8_t *)px + y * pitch))[x] = (x ^ y) & 0xFF;
slui_pixels_damage(0, 0, w, h);
// in every frame:
slui_pixels(20, 60, 400, 300, 0, 0);
```

## SLUI: text and fonts

```c
void slui_label(x, y, const char *text, r, g, b);
void slui_label_theme(x, y, token, const char *text);
void slui_label_align(x, y, slui_align_t align, const char *text, r, g, b);
void slui_label_align_theme(x, y, align, token, const char *text);
void slui_face(const char *name);
void slui_set_font_px(uint32_t font_px);
```

- `slui_label_align`: `SLUI_ALIGN_LEFT` (starts at x), `CENTER` (centred on
  x), `RIGHT` (ends at x).
- `slui_face` picks one of the skin's fonts for the labels after it; `NULL`
  or `""` goes back to the default. Every frame starts with the default.

| Fonts | |
|---|---|
| Outfit (the UI) | `ui-13`, `ui-14`, `ui-15`, `ui-15-mid`, `ui-16`, `ui-16-mid`, `ui-17`, `ui-20`, `ui-22-mid`, `ui-24`, `ui-28-light`, `ui-30-light`, `ui-34-light` |
| JetBrains Mono | `mono-12`, `mono-13`, `mono-14`, `mono-16`, `mono-16-bold`, `mono-20`, `mono-24`, `mono-30-light`, `mono-32`, `mono-64` (digits only) |

```c
slui_face("ui-28-light");
slui_label_theme(0, 10, SLUI_COLOR_TEXT_PRIMARY, "Settings");
slui_face("mono-16");
slui_label_align_theme(w - 20, 14, SLUI_ALIGN_RIGHT, SLUI_COLOR_TEXT_SECONDARY, "v1.2");
```

## SLUI: theme colours

Colours from the theme follow the user's choices (day and night, the accent).
Prefer them to fixed RGB.

| Token | For |
|---|---|
| `SLUI_COLOR_BG_CANVAS`, `BG_PANEL` | Backgrounds. |
| `BG_CONTROL`, `BG_CONTROL_HOVER`, `BG_CONTROL_ACTIVE` | Controls. |
| `BORDER`, `BORDER_FOCUS` | Lines. |
| `TEXT_PRIMARY`, `TEXT_SECONDARY`, `TEXT_DISABLED` | Text. |
| `ACCENT`, `ACCENT_HOVER`, `ACCENT_ACTIVE` | The accent. |
| `DANGER`, `SUCCESS`, `WARNING` | States. |
| `SHADOW_DARK`, `SHADOW_LIGHT`, `NEON` | Soft UI's shadows and neon. |

`slui_theme_query(token, &r, &g, &b)` reads one (for your own drawing);
`slui_set_accent(r, g, b)` changes the accent for this app only,
`slui_theme_reset_override()` drops that.

## SLUI: events

```c
int slui_poll_event(slui_event_t *out);   // 1: one event in *out, 0: none left
```

| `type` | `id` | `value` / `value2` / `text` |
|---|---|---|
| `SLUI_EVENT_BUTTON` | the control | |
| `SLUI_EVENT_DOUBLE_CLICK` | the control | |
| `SLUI_EVENT_SLIDER` | the slider | `value`: the new value |
| `SLUI_EVENT_TOGGLE` | the toggle | `value`: 0 or 1 |
| `SLUI_EVENT_SELECT` | select or segment | `value`: the chosen index |
| `SLUI_EVENT_CONTEXT_MENU` | the menu | `value`: the chosen index |
| `SLUI_EVENT_TEXT_INPUT` | the field | `text`, `text_len`: the whole text now |
| `SLUI_EVENT_TEXT_AREA` | the field | `text`, `text_len` |
| `SLUI_EVENT_SCROLL_V` | the bar | `value`: the new offset |
| `SLUI_EVENT_PANEL_INPUT` | the panel | `value`, `value2`: x and y scaled to 0..255 |
| `SLUI_EVENT_KEY_DOWN`, `KEY_UP`, `KEY_REPEAT` | | `keycode` (`PEOS_KEY_*`), `mods` (`SLUI_MOD_SHIFT`, `CTRL`, `ALT`, `SUPER`) |
| `SLUI_EVENT_RESIZE` | | `value`, `value2`: the body's new width and height |
| `SLUI_EVENT_HOST_ERROR` | | `text`: what Starlight could not do (`slui_last_host_error()` too) |
| `SLUI_EVENT_LANGUAGE` | | `text`: the new language's code |
| `SLUI_EVENT_PIXELS` | | `value`, `value2`: the surface of `slui_pixels_open` (0 x 0: none) |

`text` points into SLUI's buffer: copy it before the next `slui_poll_event`.
Keys reach the app when its window has the focus and no text field takes them.

```c
if (ev.type == SLUI_EVENT_TEXT_INPUT && ev.id == ID_NAME) snprintf(name, sizeof(name), "%s", ev.text);
if (ev.type == SLUI_EVENT_KEY_DOWN && ev.keycode == PEOS_KEY_ESC) dialog_open = 0;
if (ev.type == SLUI_EVENT_KEY_DOWN && ev.keycode == PEOS_KEY_S && (ev.mods & SLUI_MOD_CTRL)) save();
```

## SLUI: helpers

Header-only helpers in `starlight_ui.h`, built on the calls above:

- **Scroll view**: `slui_scroll_view_begin(&v, bar_id, x, y, w, h, content_h,
  &offset)`, then `slui_scroll_view_next(&v, item_h, spacing, &x, &y, &w, &h)`
  per row (1 when the row is visible: draw it), then `slui_scroll_view_end`.
  Update `offset` on `SLUI_EVENT_SCROLL_V`.
- **Dialog**: `slui_dialog_prepare` (size and wrap a title and a message),
  `slui_dialog_draw` (with Annulla / OK), `slui_dialog_map_event` (which
  button). For a dialog in the Soft UI style, see `examples/soft_ui`
  (`slui_veil`, a raised surface, a primary button).
- **Colour picker**: `slui_color_picker_prepare` / `_draw` /
  `_handle_event`, and `slui_color_picker_hsv_to_rgb` / `_rgb_to_hsv`.

Others: `slui_open_app_bundle(path)` asks Starlight to open another app;
`slui_host_capabilities()` says which `SLUI_CAP_*` the running Starlight has.

## Languages

The user chooses PeOS's language in Settings (General > Language). An app
carries its own words, one folder per language in its bundle:

```
myapp.app/
  app.ini
  app.bin
  resources/
    en/String.lang      English: the fallback for every other language
    it/String.lang
    it/welcome.png      a picture with Italian text in it
    icon.png            the same in every language
```

`String.lang` is UTF-8 text, one sentence per line, `key=text` as in
`app.ini`. Spaces around the text are dropped; put it between double quotes
to keep them (`chars="Chars: "`). `#` starts a comment, `\n` is a new line, `\t` a tab, `\\` a
backslash. Values go in as `%s` and `%u`, in the order the code passes them.
For singular and plural give two keys, `key.one` and `key.other`.

```ini
# resources/en/String.lang
title=Notes
greeting=Hello, %s!
notes.one=%u note
notes.other=%u notes
```

```ini
# resources/it/String.lang
title=Note
greeting=Ciao, %s!
notes.one=%u nota
notes.other=%u note
```

| Call | |
|---|---|
| `slui_text("key")` | The sentence in the chosen language, else in English, else the key itself. |
| `slui_text_n("key", n)` | `key.one` when `n` is 1, else `key.other`. |
| `slui_resource("name")` | The path of `resources/<language>/name`, else `resources/en/name`, else `resources/name`. |
| `slui_language()` | The chosen language's code (`"it"`, `"en"`). |

`title=` in an app's `String.lang` is its name in the dock, the app manager
and the title bar, instead of `app.ini`'s `title`. When the user picks
another language, the app gets `SLUI_EVENT_LANGUAGE`: `slui_text` already
answers in the new one, so drawing again is enough. Strings kept from before
(a status line) stay as they were: keep keys, not texts, if that matters.

```c
char line[64];
snprintf(line, sizeof line, slui_text_n("notes", count), (unsigned)count);
slui_label_theme(20, 20, SLUI_COLOR_TEXT_PRIMARY, slui_text("title"));
slui_label_theme(20, 50, SLUI_COLOR_TEXT_SECONDARY, line);
slui_image_file(20, 80, 200, 120, slui_resource("welcome.png"));
```

An app always reads its own bundle, whatever its `paths` say: its languages
need no grant.

**Adding a language** to an app: copy `resources/en/` to
`resources/<code>/` (`fr`, `de`, `pt-BR`...), translate the texts (not the
keys), and run `make check-strings APP_ID=myapp`: it says which keys a
language misses or has in excess, which texts have different `%`-values than
English, and which keys the code asks for that English lacks. The system's
own languages are folders of `/system/lang/` in PeOS (with `language.name`,
the name the Settings list shows); an app may have more or fewer languages
than the system.

## libpeos: files and folders

`peos/peos.h`. Paths are absolute or relative to the working folder; sizes are
bytes. Calls return `(uint64_t)-1` (or a negative `int64_t`) on failure.

| Call | |
|---|---|
| `peos_fopen(path, len, flags)` | Opens; `flags`: `PEOS_O_READ`, or `PEOS_O_WRITE \| PEOS_O_CREAT \| PEOS_O_TRUNC`. → descriptor |
| `peos_fread(fd, buf, len)`, `peos_fwrite(fd, buf, len)` | → bytes |
| `peos_fseek(fd, pos)`, `peos_fsize(fd)`, `peos_fclose(fd)` | |
| `peos_read_file(path, len, out, cap, &size)` | A whole file in one call. |
| `peos_write_file(path, len, data, size, flags)` | Likewise, to write. |
| `peos_stat(path, len, &st, sizeof st)` | Size, times, folder or not. |
| `peos_exists`, `peos_is_file`, `peos_is_dir` | 1 or 0. |
| `peos_mkdir`, `peos_mkdir_p` | A folder; `_p` with its parents. |
| `peos_unlink`, `peos_rmdir(path, len, PEOS_RMDIR_RECURSIVE)`, `peos_rename` | |
| `peos_readdir(path, len, out, cap)` | The entries, one per line, folders ending in `/`. |
| `peos_path_join`, `peos_path_parent`, `peos_path_basename` | |

The C library is there too: `fopen`, `fgets`, `fprintf`, `snprintf`,
`malloc`, `strcmp`... (`stdio.h`, `stdlib.h`, `string.h`).

```c
// keep a note in the user's folder (needs fs.write and a paths=...:rw for it)
const char *path = "/home/notes/today.txt";
peos_mkdir_p("/home/notes", 11);
peos_write_file(path, strlen(path), text, strlen(text), PEOS_O_WRITE | PEOS_O_CREAT | PEOS_O_TRUNC);

char buf[1024];
size_t n = 0;
if (peos_read_file(path, strlen(path), buf, sizeof buf - 1, &n) >= 0) buf[n] = '\0';
```

On the live ISO the file system is in RAM: what an app writes lasts until
the VM is switched off.

## libpeos: time

| Call | |
|---|---|
| `peos_time_ns()` | Nanoseconds since 1970 (wall clock). |
| `peos_time()` | Seconds since 1970. |
| `peos_sleep(ticks)` | Sleeps; a tick is a millisecond. |
| `peos_timer_create()`, `peos_timer_set(fd, first_ns, every_ns)` | A timer descriptor, for `peos_wait`. |

```c
uint64_t t0 = peos_time_ns();
work();
printf("took %u us\n", (unsigned)((peos_time_ns() - t0) / 1000));
```

## libpeos: notifications

`peos/notify.h`:

```c
int peos_notify(const char *source, const char *text);   // 0: shown, -1: no desktop
```

Starlight shows it in a pill at the top right and counts it on the dock tile
of the app whose title is `source`: pass your app's `title`.

```c
peos_notify("Calculator", "Result copied");
```

## libpeos: network

`peos/net.h`, needs `network`. The network stack (netd) does DHCP by itself;
these calls talk to it. Timeouts are in nanoseconds.

| Call | |
|---|---|
| `net_resolve(name, ip, timeout)` | DNS: 0 and the address in `ip`. |
| `net_tcp_connect(ip, port, timeout)` | → a connection, or a `NET_ERR_*` (negative). |
| `net_tcp_send(conn, buf, len)` | → `len`, or -1. |
| `net_tcp_reader_init(&r, conn)`, `net_tcp_read(&r, buf, cap, timeout)` | → bytes, 0 at the end, `NET_ERR_TIMEOUT`, or -1. |
| `net_udp_open(port)`, `net_udp_send`, `net_udp_recv` | Datagrams. |
| `net_ping(ip, timeout)` | → round trip in ns. |
| `net_config_get(&cfg)` | Address, netmask, gateway, DNS, MAC. |
| `peos_fclose(conn)` | Closes a connection. |

```c
#include "peos/net.h"

uint8_t ip[4];
if (net_resolve("example.com", ip, 3000000000ull) != 0) return;
int64_t c = net_tcp_connect(ip, 80, 5000000000ull);
if (c < 0) return;
const char *req = "GET / HTTP/1.0\r\nHost: example.com\r\n\r\n";
net_tcp_send(c, req, strlen(req));
static net_tcp_reader_t r;           // large: keep it off the stack
net_tcp_reader_init(&r, c);
char buf[512];
int64_t n;
while ((n = net_tcp_read(&r, buf, sizeof buf, 5000000000ull)) > 0) handle(buf, (size_t)n);
peos_fclose((uint64_t)c);
```

For the web, use HTTP below rather than TCP by hand.

## libpeos: HTTP and HTTPS

`peos/http.h`, needs `network` (PeOS 0.11). HTTP/1.1 over `http://` and
`https://` (TLS 1.3 and 1.2, the server's certificate checked against the
root certificates in `/system/certs`), redirects followed, gzip and chunked
bodies decoded, the connection kept for the next request to the same
server, cookies in a jar if you give one. The body comes in pieces, as it
arrives.

```c
#include "peos/http.h"

static int got(void *user, const uint8_t *data, size_t len) {
    (void)user;
    peos_write(data, len);
    return 0;                         // not 0: stop
}

static http_result_t res;             // large: keep it off the stack
http_request_t req = { .url = "https://example.org/" };
if (http_fetch(&req, &res, got, NULL) != 0) printf("%s\n", res.error);
else printf("%d, %s\n", res.resp.status, res.tls);
```

| `http_request_t` | |
|---|---|
| `url` | The address. |
| `method`, `body`, `body_len`, `content_type` | `NULL` method: GET; POST with a body. |
| `headers` | More request headers, `"Name: value\r\n"` each. |
| `cookies` | An `http_cookies_t` jar kept between requests, or `NULL`. |
| `timeout_ns` | Each wait (0: 15 s). |
| `no_redirects`, `max_redirects` | A 3xx as it is; how many to follow (0: 10). |

`res.resp` has the status, `content_type`, `location` and the headers
(`http_header(&res.resp, "Name", buf, cap)`); `res.url` where it came from
after the redirects. `http_url_parse` and `http_url_resolve` read and join
addresses as a browser does.

`peos/tls.h` is TLS over a TCP connection of your own (`tls_open`,
`tls_read`, `tls_write`, `tls_close`), and `peos_random(buf, len)`
(`peos/peos.h`, no capability) gives random bytes for keys and tokens.

## libpeos: system information

`peos_sysinfo(&info)` fills `peos_sysinfo_t` (`peos/sysinfo_abi.h`): uptime,
CPU time, memory, disk, volume. `peos_proc_list(out, max)` lists the running
processes (`peos/proc_abi.h`). Neither needs a capability.

```c
peos_sysinfo_t si;
if (peos_sysinfo(&si) == 0) printf("up %u s\n", (unsigned)(si.uptime_ns / 1000000000ull));
```

## Installing an app in a VM

No need to rebuild PeOS: serve the bundle from your machine and install it
from PeOS's terminal.

```bash
make serve APP_ID=myapp          # builds, packs dist/myapp.app.tar, serves it on port 8000
```

It prints the command to type in PeOS's **Terminale** (Orbit), for example:

```
appget http://10.0.2.2:8000/myapp.app.tar
```

`10.0.2.2` is your machine as QEMU and VirtualBox (NAT) see it; for UTM's
shared network or a bridged VM, use the other address it prints. The app goes
into `/apps/myapp.app` and shows up in the dock within a second; running
`appget` again replaces it with the new build. On the live ISO `/apps` is in
RAM: install again after a restart.

`make pack APP_ID=myapp` only writes the tar (`dist/myapp.app.tar`), to serve
it some other way. An app in `/apps` with the name of a system app
(`/system/apps`) is ignored.

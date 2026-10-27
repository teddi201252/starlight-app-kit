# Changelog

All notable changes to this module are documented in this file.

## [1.6.0] - 2026-10-27
- Synced from PeOS 1ff0151 (SLUI SDK 1.0.5) with `scripts/sync_sdk.py`
- The whole `userland/include`: the libc headers (`stdio.h`, `string.h`...),
  SDL, and every `peos/` header (notify, net, capability, path scope, event,
  sysinfo, proc, calendar_file...)
- SLUI: Soft UI controls (`slui_button_primary`, `slui_segment`,
  `slui_swatch`), `SLUI_EVENT_RESIZE`, drawing beyond the controls
  (`slui_surface`, `slui_face`, `slui_label_align`, `slui_ring`,
  `slui_graph`, `slui_veil`)
- `peos_sysinfo` (uptime, memory, disk), `peos_proc_list`, `peos_proc_kill`
- New `libpeos.a` and `crt0.S`

## [1.5.0] - 2026-03-06
- Expose SLUI event text as pointer plus length
- Align SDK text event structures with dynamic host buffers

## [1.4.2] - 2026-03-05
- SDK context-menu API/header updates

## [1.4.1] - 2026-03-05
- SDK/header sync for UI behavior

## [1.4.0] - 2026-03-03
- Add nonblocking fread/fwrite wrappers for pipe-safe UI polling

## [1.3.4] - 2026-03-03
- Maintenance update.

## [1.3.3] - 2026-03-03
- Patch maintenance release.

## [1.3.2] - 2026-03-03
- Expose gradient fill and panel input APIs

## [1.3.1] - 2026-03-03
- RGBA theme APIs and color picker alpha slider

## [1.3.0] - 2026-03-03
- Add SLUI themed primitives, theme query, and per-app accent override APIs

## [1.2.2] - 2026-03-02
- Add keyboard event types/modifiers and text area API updates

## [1.2.1] - 2026-02-25
- SDK UI: slui_dialog_* helpers and slui_rect_a alpha primitive

## [1.2.0] - 2026-02-25
- SDK SLUI: nuove primitive (separator, list item) e varianti *_state

## [1.1.0] - 2026-02-25
- Aligned SDK mouse event structures and UI headers for wheel-enabled input.

## [1.0.1] - 2026-02-25
- SDK API: new widgets/events plus scroll view and clip helpers

## [1.0.0] - 2026-02-24
- Aggiunto il versioning.


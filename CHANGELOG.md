# Changelog

All notable changes to this module are documented in this file.

## [0.8.0] - 2026-10-28
- Languages (Technical Alpha plan, phase 4): `slui_text`, `slui_text_n`,
  `slui_resource`, `slui_language`, `SLUI_EVENT_LANGUAGE`; an app's words in
  `resources/<language>/String.lang`, its name in the dock from `title=`
- The template and `new_app.py` create `resources/en/` and `resources/it/`;
  the examples `soft_ui` and `demo_kit` speak English and Italian
- `scripts/check_strings.py` and `make check-strings`: every language has
  English's keys and %-values, and the code's keys exist
- Guide: Languages in `docs/api-reference.md`, how to add one
- SDK 1.7.0

## [0.7.0] - 2026-10-28
- `make pack` (`dist/<id>.app.tar`) and `make serve`: install an app in a
  running PeOS VM with `appget <url>` in Terminale, no rebuild of PeOS
  (Technical Alpha plan, phase 3)
- `docs/api-reference.md`: the manifest, capabilities and paths, every SLUI
  call and event, libpeos (files, time, notifications, network, system
  information), with an example per topic
- SDK 1.6.1

## [0.6.0] - 2026-10-27
- Realigned with PeOS's Soft UI (Starlight plan, phases 1-13): SDK 1.6.0
- `scripts/sync_sdk.py` and `make sync-sdk`: refresh `sdk/` from a PeOS tree
  (`sdk/SOURCE` records the commit)
- `make peos-add`: put an app into a PeOS tree, built into its image
- Template: Soft UI layout that follows the window's size, primary button,
  segmented control, skin fonts
- `new_app.py`: `--category`, `--width`/`--height`, `--min-width`/
  `--min-height`, `--fixed-size`; the manifest template has the new keys
- New example `soft_ui`; `demo_kit` in step with PeOS's
- Build and link flags as PeOS's own apps (non-executable stack)
- Guides: trying an app in PeOS, resizable windows, troubleshooting

## [0.5.0] - 2026-03-06
- Sync App Kit release with SDK text event structure update

## [0.4.2] - 2026-03-05
- SDK context-menu API/header updates

## [0.4.1] - 2026-03-05
- SDK/header sync for UI behavior

## [0.4.0] - 2026-03-03
- Sync SDK headers for nonblocking file descriptor IO bit

## [0.3.3] - 2026-03-03
- Patch maintenance release.

## [0.3.2] - 2026-03-03
- Sync SDK headers for picker panel interactions

## [0.3.1] - 2026-03-03
- Expose updated SDK headers for RGBA theming

## [0.3.0] - 2026-03-03
- Expose themed SLUI APIs and color tokens in app kit SDK

## [0.2.2] - 2026-03-02
- Sync app-kit for SDK keyboard/text-area updates

## [0.2.1] - 2026-02-25
- App Kit patch release aligned with SDK/UI dialog and alpha updates

## [0.2.0] - 2026-02-25
- Allineamento App Kit con release SDK SLUI 1.2

## [0.1.2] - 2026-02-25
- App Kit synced with SDK 1.0.1 and updated demo for scroll clipping

## [0.1.1] - 2026-02-24
- Allineato all'albero PeOS: risorse in examples/<app>/resources copiate nel bundle dist
- Template/scaffold manifest aggiornati con icon= e sezione [resources]

## [0.1.0] - 2026-02-24
- Aggiunto il versioning.


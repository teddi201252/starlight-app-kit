# Starlight App Kit

Standalone repository to create and distribute `.app` bundles for Starlight,
PeOS's desktop. The workflow builds locally and produces a ready-to-share
bundle in `dist/`.

## What it includes
- the PeOS SDK in `sdk/` (headers, `libpeos.a`, `crt0.S`, `linker.ld`), kept in
  step with PeOS by `scripts/sync_sdk.py` (`sdk/SOURCE` says from which commit)
- app project generator (`scripts/new_app.py`)
- standalone-first Makefile (`Makefile`)
- source + manifest templates (`templates/`)
- examples: `soft_ui` (the Soft UI drawing, a resizable window, a dialog, a
  notification) and `demo_kit` (the classic controls at a fixed size)
- quickstart, API reference and troubleshooting guides (`docs/`)
- `make serve`: install an app in a running PeOS VM over the network, no
  rebuild of PeOS

## Requirements
- Python 3.9+
- toolchain `x86_64-elf-*` installed in `$PATH`
- for `sync-sdk` and `peos-add`: a PeOS source tree (by default the one the
  kit sits in, else `../peos-starlight`; set `PEOS=` otherwise)

## Quick Start

1) Create a new app in the kit:

```bash
python3 scripts/new_app.py --name "Calculator" --id calculator --category strumenti
```

2) Build the standalone bundle:

```bash
make APP_ID=calculator build
```

Output:
- `dist/calculator.app/app.bin`
- `dist/calculator.app/app.ini`
- `dist/calculator.app/resources/*` (if present in project)

3) Try it in PeOS. In a running PeOS VM (the live ISO is enough), serve it
from your machine:

```bash
make APP_ID=calculator serve
```

and type the command it prints in PeOS's Terminale, e.g.
`appget http://10.0.2.2:8000/calculator.app.tar`: the app shows up in the
dock. Run both again after every change.

Or add it to a PeOS tree, which builds it into its image like its own apps:

```bash
make APP_ID=calculator peos-add PEOS=../peos-starlight
make -C ../peos-starlight run-hdd
```

## Generated app project structure
Each app in the kit lives in `examples/<app_id>/`:
- `src/main.c`
- `bundle/app.ini`
- `resources/` (optional app assets copied to `.app/resources/`)
- `README.md`

So you can have multiple apps in the same kit.

## Writing an app for today's Starlight
The template (`templates/main.c.tpl`) shows the pattern:
- render only when state changed, and after every `SLUI_EVENT_RESIZE`: the
  user can resize the window, and `slui_viewport(&w, &h)` gives the body's
  size (0 x 0 until the first resize event: lay out for app.ini's size then);
- the Soft UI controls: `slui_button_primary` (the group's main button),
  `slui_segment`, `slui_swatch`, plus the classic ones;
- drawing beyond the controls (SLUI SDK 1.0.5): `slui_surface` (raised,
  inset, night cards), `slui_face` (the skin's fonts), `slui_label_align`,
  `slui_ring`, `slui_graph`, `slui_veil` (a modal dialog over the whole
  window);
- colours from the theme (`slui_label_theme`, `SLUI_COLOR_*`), so the app
  follows the day and night themes;
- `peos_notify()` (`peos/notify.h`) for a desktop notification.

`app.ini` keys: `title`, `category` (sistema, strumenti, svago), `exec`,
`args`, `capabilities`, `paths`, `icon`, `width`, `height`, `min_width`,
`min_height`, `max_width`, `max_height`, `resizable`, `dock`, and `[resources]`.

Every SLUI call and event, the manifest, the capabilities and libpeos (files,
time, notifications, network) are in [docs/api-reference.md](docs/api-reference.md),
with an example per topic.

## Languages
An app's words are in `resources/<language>/String.lang` (`key=text`), read
with `slui_text("key")`; images and files in a language sit beside them
(`slui_resource("name")`). `new_app.py` creates `en` and `it`; English is the
fallback. `make check-strings APP_ID=myapp` checks that every language has
every sentence. See [Languages](docs/api-reference.md#languages).

## Keeping the SDK in step with PeOS
When PeOS changes its SDK (a new header, new SLUI commands), refresh the kit:

```bash
make -C ../peos-starlight userland/libpeos.a
make sync-sdk PEOS=../peos-starlight
```

then rebuild the examples, bump `VERSION`/`sdk/VERSION` and note it in the
changelogs.

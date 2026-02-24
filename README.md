# Starlight App Kit

Standalone repository to create and distribute `.app` bundles for Starlight.
The workflow builds locally and produces a ready-to-share bundle in `dist/`.

## What it includes
- app project generator (`scripts/new_app.py`)
- standalone-first Makefile (`Makefile`)
- source + manifest templates (`templates/`)
- quickstart and troubleshooting guides (`docs/`)

## Requirements
- Python 3.9+
- toolchain `x86_64-elf-*` installed in `$PATH`

- Local SDK already present in `sdk/` (`include/`, `lib/libpeos.a`, `crt0.S`, `linker.ld`)

## Quick Start

1) Create a new app in the kit:

```bash
python3 scripts/new_app.py --name "Calculator" --id calculator
```

2) Build the standalone bundle:

```bash
make APP_ID=calculator build
```

Output:
- `dist/calculator.app/app.bin`
- `dist/calculator.app/app.ini`

## Generated app project structure
Each app in the kit lives in `examples/<app_id>/`:
- `src/main.c`
- `bundle/app.ini`
- `README.md`

So you can have multiple apps in the same kit.

## UI notes
The template uses the exported SDK (`peos/starlight_ui.h`) with a performant pattern:
- render only when state changes (`dirty`)
- button actions triggered on `mouse_up`

## SDK sync status
The bundled SDK in `sdk/` is aligned with Starlight Phase 5:
- protocol handshake and host capability negotiation
- structured host errors (`SLUI_EVENT_HOST_ERROR`)
- image command support (`slui_image_file`, `IMAGE_FILE` capability)
- explicit SDK/protocol version macros and query helpers

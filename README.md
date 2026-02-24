# Starlight App Kit

Repository standalone per creare e distribuire app `.app` per Starlight.
Il flusso compila localmente e produce un bundle pronto in `dist/`.

## Cosa include
- generator progetto app (`scripts/new_app.py`)
- Makefile standalone-first (`Makefile`)
- template sorgente + manifest (`templates/`)
- guida rapida e troubleshooting (`docs/`)

## Requisiti
- Python 3.9+
- toolchain `x86_64-elf-*` disponibile nel PATH

- SDK locale già presente in `sdk/` (`include/`, `lib/libpeos.a`, `crt0.S`, `linker.ld`)

## Quick Start

1) Crea una nuova app nel kit:

```bash
python3 scripts/new_app.py --name "Calcolatrice" --id calcolatrice
```

2) Build standalone bundle:

```bash
make APP_ID=calcolatrice build
```

Output:
- `dist/calcolatrice.app/app.bin`
- `dist/calcolatrice.app/app.ini`

## Struttura progetto app generato
Ogni app nel kit vive in `examples/<app_id>/`:
- `src/main.c`
- `bundle/app.ini`
- `README.md`

## Note UI
Il template usa l'SDK esportato (`peos/starlight_ui.h`) con pattern performante:
- render solo quando stato cambia (`dirty`)
- trigger bottoni su `mouse_up`

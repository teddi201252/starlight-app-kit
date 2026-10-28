# Quickstart

## 1) Verify environment

```bash
python3 scripts/check_env.py --sdk-dir sdk
```

## 2) Create app

```bash
python3 scripts/new_app.py --name "My App" --id myapp
```

Options: `--category sistema|strumenti|svago`, `--width`/`--height` (the
body's size when the window opens, default 560 x 400), `--min-width`/
`--min-height` (how small the user can make it), `--fixed-size` (not
resizable).

## 3) Build standalone bundle

```bash
make APP_ID=myapp build
```

Bundle is ready in `dist/myapp.app/`.

If your project has `examples/myapp/resources/`, files are copied to
`dist/myapp.app/resources/`.

## 4) Try it in PeOS

Start PeOS in a VM (`make -C ../peos-starlight run-live`, or the ISO in
VirtualBox, VMware, UTM), then serve the app from your machine:

```bash
make APP_ID=myapp serve
```

In PeOS, open Terminale and type the command it prints, e.g.

```
appget http://10.0.2.2:8000/myapp.app.tar
```

To build it into a PeOS image instead:

```bash
make APP_ID=myapp peos-add PEOS=../peos-starlight
make -C ../peos-starlight run-hdd
```

The app shows in the dock and in the app manager. Drag a window corner to
check that your layout follows the size.

## 5) Look at the examples

```bash
make APP_ID=soft_ui build
```

`examples/soft_ui` uses every Soft UI primitive; `examples/demo_kit` the
classic controls.

Done: you can now distribute `dist/myapp.app/` directly (or
`dist/myapp.app.tar`, from `make pack`, for `appget`).

Next: [docs/api-reference.md](api-reference.md), every call with an example.

# Troubleshooting

## Standalone build fails (missing SDK)
- Check that these files exist in `sdk/`:
	- `sdk/include/peos/peos.h`
	- `sdk/include/peos/starlight_ui.h`
	- `sdk/crt0.S`
	- `sdk/linker.ld`
	- `sdk/lib/libpeos.a`

## Build fails because `x86_64-elf-gcc` is missing
- Install/configure the `x86_64-elf-*` cross-toolchain
- Verify with:
	- `x86_64-elf-gcc --version`
	- `x86_64-elf-ld --version`
	- `x86_64-elf-ar --version`

## The window opens but stays see-through, or the app looks frozen
- Starlight waits for the app's first frame. If the layout subtracts from
  the body's size, check what `slui_viewport()` gives before the first
  `SLUI_EVENT_RESIZE`: 0 x 0. `W - 240` is then a huge number, and so is
  everything drawn from it. Lay out for at least app.ini's minimum:

  ```c
  uint32_t W = 0, H = 0;
  slui_viewport(&W, &H);
  if (W < MIN_W) W = MIN_W;
  if (H < MIN_H) H = MIN_H;
  ```

## Controls overlap or get cut when the window is small
- Set `min_width`/`min_height` in `bundle/app.ini` to the smallest size your
  layout works at (or `resizable=0` for a fixed layout).

## A label in a font does not show
- `slui_face()` takes the names of the fonts baked in Starlight's skin
  (`ui-13`, `ui-14`, `ui-15`, `ui-15-mid`, `ui-16`, `ui-16-mid`, `ui-17`,
  `ui-20`, `ui-22-mid`, `ui-24`, `ui-28-light`, `ui-30-light`, `ui-34-light`,
  `mono-12`, `mono-13`, `mono-14`, `mono-16`, `mono-16-bold`, `mono-20`,
  `mono-24`, `mono-30-light`, `mono-32`, `mono-64`). `mono-64` has digits
  only. An unknown name falls back to the default font.

## Headers or functions missing (`stdio.h`, `peos/notify.h`, `slui_surface`)
- The SDK is older than the PeOS you target: `make sync-sdk PEOS=<tree>`.

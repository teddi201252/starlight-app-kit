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

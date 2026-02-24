# Troubleshooting

## Build standalone fallisce (SDK mancante)
- controlla presenza file in `sdk/`:
	- `sdk/include/peos/peos.h`
	- `sdk/include/peos/starlight_ui.h`
	- `sdk/crt0.S`
	- `sdk/linker.ld`
	- `sdk/lib/libpeos.a`

## Build fallisce su `x86_64-elf-gcc` mancante
- installa/configura la cross-toolchain `x86_64-elf-*`
- verifica con:
	- `x86_64-elf-gcc --version`
	- `x86_64-elf-ld --version`
	- `x86_64-elf-ar --version`

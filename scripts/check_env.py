#!/usr/bin/env python3
from __future__ import annotations

import argparse
import shutil
from pathlib import Path


def has_tool(name: str) -> bool:
    return shutil.which(name) is not None


def main() -> int:
    parser = argparse.ArgumentParser(description="Check minimal environment for Starlight app development")
    parser.add_argument("--sdk-dir", default="sdk", help="SDK directory to validate for standalone build")
    args = parser.parse_args()

    ok = True

    for tool in ["python3", "x86_64-elf-gcc", "x86_64-elf-ld", "x86_64-elf-ar", "make"]:
        present = has_tool(tool)
        print(f"{tool}: {'OK' if present else 'MISSING'}")
        ok = ok and present

    sdk_dir = Path(args.sdk_dir)
    sdk_checks = [
        sdk_dir / "include" / "peos" / "peos.h",
        sdk_dir / "include" / "peos" / "starlight_ui.h",
        sdk_dir / "crt0.S",
        sdk_dir / "linker.ld",
        sdk_dir / "lib" / "libpeos.a",
    ]
    sdk_ok = all(p.exists() for p in sdk_checks)
    print(f"sdk-dir: {'OK' if sdk_ok else 'MISSING FILES'} ({sdk_dir})")
    if not sdk_ok:
        print("  Populate sdk/ with include/, lib/libpeos.a, crt0.S, linker.ld")

    ok = ok and sdk_ok

    return 0 if ok else 1


if __name__ == "__main__":
    raise SystemExit(main())

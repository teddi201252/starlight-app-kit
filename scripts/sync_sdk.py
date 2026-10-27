#!/usr/bin/env python3
"""Refresh sdk/ from a PeOS source tree, so apps built with the kit match
the system they run on.

    python3 scripts/sync_sdk.py --peos ../peos-starlight

Copies what an app is built against, as PeOS builds its own apps:
  userland/include/   -> sdk/include/      (peos/*.h, the libc headers, SDL)
  userland/libpeos.a  -> sdk/lib/libpeos.a (build it first: make userland/libpeos.a)
  userland/crt0.S     -> sdk/crt0.S
  userland/linker.ld  -> sdk/linker.ld
and writes sdk/SOURCE: the PeOS commit, the date and the SLUI SDK version.
"""
from __future__ import annotations

import argparse
import datetime
import re
import shutil
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
SDK = ROOT / "sdk"


def slui_version(header: Path) -> str:
    text = header.read_text(encoding="utf-8")
    parts = [re.search(rf"#define SLUI_SDK_VERSION_{k} (\d+)u", text) for k in ("MAJOR", "MINOR", "PATCH")]
    return ".".join(m.group(1) for m in parts) if all(parts) else "?"


def peos_commit(peos: Path) -> str:
    try:
        out = subprocess.run(["git", "-C", str(peos), "rev-parse", "--short", "HEAD"],
                             capture_output=True, text=True, check=True)
        dirty = subprocess.run(["git", "-C", str(peos), "status", "--porcelain", "--", "userland"],
                               capture_output=True, text=True, check=True).stdout.strip()
        return out.stdout.strip() + (" (with uncommitted userland changes)" if dirty else "")
    except (OSError, subprocess.CalledProcessError):
        return "unknown"


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--peos", required=True, type=Path, help="the PeOS source tree (with userland/ built)")
    args = ap.parse_args()
    peos = args.peos.resolve()
    sources = {
        "include": peos / "userland" / "include",
        "lib": peos / "userland" / "libpeos.a",
        "crt0": peos / "userland" / "crt0.S",
        "linker": peos / "userland" / "linker.ld",
    }
    missing = [str(p) for p in sources.values() if not p.exists()]
    if missing:
        print("sync_sdk: missing in the PeOS tree:\n  " + "\n  ".join(missing), file=sys.stderr)
        print("Build the library first: make -C <peos> userland/libpeos.a", file=sys.stderr)
        return 1

    if (SDK / "include").exists():
        shutil.rmtree(SDK / "include")
    shutil.copytree(sources["include"], SDK / "include",
                    ignore=shutil.ignore_patterns(".DS_Store", "*.d", "*.o"))
    (SDK / "lib").mkdir(parents=True, exist_ok=True)
    shutil.copy2(sources["lib"], SDK / "lib" / "libpeos.a")
    shutil.copy2(sources["crt0"], SDK / "crt0.S")
    shutil.copy2(sources["linker"], SDK / "linker.ld")

    version = slui_version(SDK / "include" / "peos" / "starlight_ui.h")
    (SDK / "SOURCE").write_text(
        "# Where sdk/ comes from (written by scripts/sync_sdk.py)\n"
        f"peos_commit={peos_commit(peos)}\n"
        f"synced={datetime.date.today().isoformat()}\n"
        f"slui_sdk={version}\n",
        encoding="utf-8",
    )
    headers = sum(1 for _ in (SDK / "include").rglob("*.h"))
    print(f"sdk/ synced from {peos}: {headers} headers, libpeos.a, crt0.S, linker.ld (SLUI SDK {version})")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

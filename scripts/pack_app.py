#!/usr/bin/env python3
"""Pack a built bundle (dist/<id>.app) into dist/<id>.app.tar for appget.

A plain ustar archive with one top folder, <id>.app/: no macOS metadata, no
owners, every member readable. PeOS's `appget <url>` unpacks it into /apps.
"""

from __future__ import annotations

import argparse
import pathlib
import sys
import tarfile


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("bundle", type=pathlib.Path, help="the built bundle, e.g. dist/myapp.app")
    ap.add_argument("out", type=pathlib.Path, help="the archive to write, e.g. dist/myapp.app.tar")
    args = ap.parse_args()

    bundle = args.bundle
    if not (bundle / "app.ini").is_file():
        print(f"pack_app: {bundle} has no app.ini (run make build first)", file=sys.stderr)
        return 1

    def clean(info: tarfile.TarInfo) -> tarfile.TarInfo | None:
        if pathlib.PurePosixPath(info.name).name.startswith("._") or info.name.endswith(".DS_Store"):
            return None
        info.uid = info.gid = 0
        info.uname = info.gname = ""
        info.mode = 0o755 if info.isdir() else 0o644
        return info

    args.out.parent.mkdir(parents=True, exist_ok=True)
    with tarfile.open(args.out, "w", format=tarfile.USTAR_FORMAT) as tar:
        tar.add(bundle, arcname=bundle.name, filter=clean)
    print(f"Packed {args.out} ({args.out.stat().st_size} bytes)")
    return 0


if __name__ == "__main__":
    sys.exit(main())

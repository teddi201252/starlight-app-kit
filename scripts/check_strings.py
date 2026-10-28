#!/usr/bin/env python3
"""Check the String.lang files of an app (or of the system).

    check_strings.py RESOURCES_DIR [--src SRC_DIR ...]

RESOURCES_DIR holds one folder per language (en/, it/...) with String.lang.
English (en) is the reference: every other language must have the same keys,
and each text the same %-values in the same order. With --src, every key the
code asks for with a literal (slui_text("key"), slui_text_n("key", n) or
sl_tr("key")) must be in English. Exit status 1 on any problem.
"""

from __future__ import annotations

import argparse
import pathlib
import re
import sys

FORMAT = re.compile(r"%(?:%|[-+ #0]*\d*(?:\.\d+)?(?:hh|h|ll|l|z)?[diouxXcsp])")
USES = re.compile(r'\b(slui_text|slui_text_n|sl_tr|sl_tr_n)\(\s*"([^"]+)"')


def parse(path: pathlib.Path) -> dict[str, str]:
    """The same rules as peos/lang.h: key up to the first '=', # comments."""
    out: dict[str, str] = {}
    text = path.read_text(encoding="utf-8")
    if text.startswith("﻿"):
        text = text[1:]
    for line in text.splitlines():
        line = line.strip()
        if not line or line.startswith("#") or "=" not in line:
            continue
        key, value = line.split("=", 1)
        key = key.strip()
        value = value.strip()
        if len(value) >= 2 and value[0] == '"' and value[-1] == '"':
            value = value[1:-1]   # quoted: the spaces inside are the text's
        if key:
            out[key] = value
    return out


def formats(text: str) -> list[str]:
    return [f for f in FORMAT.findall(text) if f != "%%"]


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("resources", type=pathlib.Path)
    ap.add_argument("--src", type=pathlib.Path, action="append", default=[],
                    help="a source folder (or file) whose literal keys must exist")
    args = ap.parse_args()

    langs = {d.name: parse(d / "String.lang") for d in sorted(args.resources.iterdir())
             if d.is_dir() and (d / "String.lang").is_file()} if args.resources.is_dir() else {}
    problems: list[str] = []
    where = str(args.resources)
    if "en" not in langs:
        problems.append(f"{where}: no en/String.lang (English is the fallback)")
    ref = langs.get("en", {})
    for code, table in langs.items():
        if code == "en":
            continue
        for key in sorted(ref.keys() - table.keys()):
            problems.append(f"{where}/{code}: missing {key}")
        for key in sorted(table.keys() - ref.keys()):
            problems.append(f"{where}/{code}: {key} is not in en")
        for key in sorted(ref.keys() & table.keys()):
            if formats(ref[key]) != formats(table[key]):
                problems.append(f"{where}/{code}: {key} has {formats(table[key])}, en has {formats(ref[key])}")

    for src in args.src:
        files = [src] if src.is_file() else sorted(src.rglob("*.c"))
        for f in files:
            for n, line in enumerate(f.read_text(encoding="utf-8", errors="replace").splitlines(), 1):
                code = line.split("//", 1)[0]
                if code.lstrip().startswith(("*", "/*")):
                    continue   # a comment that shows a call
                for fn, key in USES.findall(code):
                    wanted = [key + ".one", key + ".other"] if fn.endswith("_n") else [key]
                    for k in wanted:
                        if k not in ref:
                            problems.append(f"{f}:{n}: {fn}(\"{key}\"): {k} is not in en")

    for p in problems:
        print(p)
    counts = ", ".join(f"{code} {len(t)}" for code, t in langs.items())
    print(f"check_strings: {where}: {counts or 'no languages'}: {'FAIL' if problems else 'ok'}")
    return 1 if problems else 0


if __name__ == "__main__":
    sys.exit(main())

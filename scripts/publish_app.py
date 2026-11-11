#!/usr/bin/env python3
"""Publish an app to a PeStore folder (PeOS 0.14.0): what to upload to
https://peoverse.com/store/ (or to any static host).

    make publish APP_ID=myapp [STORE=store] [KEY=~/.peos/keys/store.key]

It takes the tar `make pack` made (dist/<id>.app.tar), reads the app's
bundle/app.ini and its resources/<lang>/String.lang, and in STORE:

    apps/<id>/<version>/<id>.app.tar   the bundle
    apps/<id>/icon.png                 its icon (app.ini [resources] icon)
    apps/<id>/shots/<n>.png            its screenshots (examples/<id>/store/*.png)
    catalog.json                       the app's entry added or replaced
    catalog.json.sig                   the catalog signed with KEY (ECDSA P-256)

What it reads from app.ini: title, version, author, store_category
(strumenti, giochi, media, sviluppo, internet...), capabilities, min_peos
(default 0.14.0, the first PeOS with PeStore). From String.lang (it and en):
store.summary, one line, and store.description, longer ("\\n" for a new line).

The key: `make store-keys` makes the pair once; the private key stays on the
publisher's machine, never in git; the public one goes in PeOS, in
userland/data/system/config/store.pub. Upload the whole STORE folder as it is
(rsync, scp, the hosting's panel).
"""

from __future__ import annotations

import argparse
import datetime
import hashlib
import json
import pathlib
import re
import shutil
import subprocess
import sys

CATEGORIES = ("strumenti", "giochi", "media", "sviluppo", "internet", "sistema")
FORMAT = 1


def read_ini(path: pathlib.Path) -> tuple[dict, dict]:
    main, res, cur = {}, {}, None
    for raw in path.read_text(encoding="utf-8").splitlines():
        line = raw.strip()
        if not line or line.startswith("#"):
            continue
        if line.startswith("[") and line.endswith("]"):
            cur = line[1:-1].strip().lower()
            continue
        if "=" in line:
            k, v = line.split("=", 1)
            (res if cur == "resources" else main)[k.strip()] = v.strip()
    return main, res


def read_lang(path: pathlib.Path) -> dict:
    out = {}
    if not path.is_file():
        return out
    for raw in path.read_text(encoding="utf-8").splitlines():
        if not raw.strip() or raw.lstrip().startswith("#") or "=" not in raw:
            continue
        k, v = raw.split("=", 1)
        v = v.strip()
        if len(v) >= 2 and v[0] == v[-1] == '"':
            v = v[1:-1]
        out[k.strip()] = v.replace("\\n", "\n")
    return out


def sha256(path: pathlib.Path) -> str:
    h = hashlib.sha256()
    with path.open("rb") as f:
        for block in iter(lambda: f.read(1 << 16), b""):
            h.update(block)
    return h.hexdigest()


def fail(msg: str) -> int:
    print(f"publish: {msg}", file=sys.stderr)
    return 1


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--app-id", help="the app to publish (not with --init)")
    ap.add_argument("--init", action="store_true", help="only the catalog: an empty one if there is none, signed again")
    ap.add_argument("--app-dir", type=pathlib.Path,
                    help="examples/<id> (its bundle/, resources/, store/), or a built <id>.app")
    ap.add_argument("--tar", type=pathlib.Path, help="dist/<id>.app.tar")
    ap.add_argument("--store", type=pathlib.Path, required=True, help="the store folder to fill")
    ap.add_argument("--key", type=pathlib.Path, required=True, help="the private key (PEM)")
    ap.add_argument("--featured", action="store_true", help="put this app in evidence")
    ap.add_argument("--force", action="store_true", help="replace the same version with a different tar")
    ap.add_argument("--min-peos", help="the oldest PeOS it runs on, over app.ini's min_peos (tests: an older one)")
    args = ap.parse_args()
    if args.init:
        return init(args)
    if not args.app_id or not args.tar:
        return fail("--app-id and --tar, or --init")

    app_id = args.app_id
    if not re.fullmatch(r"[a-z0-9._-]+", app_id):
        return fail(f"the id {app_id!r} may hold only a-z 0-9 . _ -")
    app_dir = args.app_dir or pathlib.Path("examples") / app_id
    # a project (examples/<id>: bundle/app.ini) or a built bundle (<id>.app:
    # app.ini at its top, as an app built outside the kit, e.g. a port)
    ini_path = app_dir / "bundle" / "app.ini"
    if not ini_path.is_file():
        ini_path = app_dir / "app.ini"
    if not ini_path.is_file():
        return fail(f"no bundle/app.ini or app.ini in {app_dir}")
    ini, res = read_ini(ini_path)
    if not args.tar.is_file():
        return fail(f"{args.tar} is not there (make pack APP_ID={app_id})")
    if not args.key.is_file():
        return fail(f"no key at {args.key} (make store-keys makes one)")

    version = ini.get("version", "")
    if not re.fullmatch(r"\d+(\.\d+){0,3}", version):
        return fail("app.ini needs version= (as 1.0.0)")
    category = ini.get("store_category", "").lower()
    if category not in CATEGORIES:
        return fail("app.ini needs store_category= (one of " + ", ".join(CATEGORIES) + ")")
    caps = [c.strip() for c in ini.get("capabilities", "fs.read").split(",") if c.strip()]
    texts = {lang: read_lang(app_dir / "resources" / lang / "String.lang") for lang in ("it", "en")}
    for lang in ("it", "en"):
        if not texts[lang].get("store.summary"):
            return fail(f"resources/{lang}/String.lang needs store.summary=")

    store = args.store
    rel_tar = f"apps/{app_id}/{version}/{app_id}.app.tar"
    tar_sha = sha256(args.tar)
    catalog_path = store / "catalog.json"
    if not catalog_path.is_file():
        print(f"publish: no catalog in {store}: a new one with only {app_id} "
              "(to add to the online store, make store-pull first)", file=sys.stderr)
    catalog = json.loads(catalog_path.read_text(encoding="utf-8")) if catalog_path.is_file() else {
        "format": FORMAT, "updated": "", "featured": "", "apps": []}
    if catalog.get("format") != FORMAT:
        return fail(f"{catalog_path}: a format this script does not know")
    old = next((a for a in catalog["apps"] if a.get("id") == app_id), None)
    if old and old.get("version") == version and old.get("sha256") != tar_sha and not args.force:
        return fail(f"{app_id} {version} is published with another tar: raise version= (or --force)")
    if old and old.get("version") and _older(version, old["version"]):
        return fail(f"{app_id} {old['version']} is published: {version} would go back")

    (store / rel_tar).parent.mkdir(parents=True, exist_ok=True)
    shutil.copyfile(args.tar, store / rel_tar)
    # The bundle's folder in the tar (pack_app.py: <id>.app).
    bundle = f"{app_id}.app"

    entry = {
        "id": app_id,
        "name": texts["it"].get("title") or ini.get("title") or app_id,
        "version": version,
        "author": ini.get("author", ""),
        "category": category,
        "summary": {lang: texts[lang]["store.summary"] for lang in ("it", "en")},
        "description": {lang: texts[lang].get("store.description", "") for lang in ("it", "en")},
        "capabilities": caps,
        "min_peos": args.min_peos or ini.get("min_peos", "0.14.0"),
        "date": datetime.date.today().isoformat(),
        "bundle": bundle,
        "size": args.tar.stat().st_size,
        "tar": rel_tar,
        "sha256": tar_sha,
    }
    icon_rel = res.get("icon", "")
    if icon_rel:
        icon_src = app_dir / icon_rel.removeprefix("./")
        if not icon_src.is_file():
            icon_src = app_dir / "bundle" / icon_rel.removeprefix("./")
        if icon_src.is_file():
            dst = store / "apps" / app_id / "icon.png"
            shutil.copyfile(icon_src, dst)
            entry["icon"] = {"path": f"apps/{app_id}/icon.png", "sha256": sha256(dst)}
    shots = sorted((app_dir / "store").glob("*.png"))[:6]
    if shots:
        (store / "apps" / app_id / "shots").mkdir(parents=True, exist_ok=True)
        entry["shots"] = []
        for i, src in enumerate(shots, 1):
            dst = store / "apps" / app_id / "shots" / f"{i}.png"
            shutil.copyfile(src, dst)
            entry["shots"].append({"path": f"apps/{app_id}/shots/{i}.png", "sha256": sha256(dst)})

    catalog["apps"] = [a for a in catalog["apps"] if a.get("id") != app_id] + [entry]
    catalog["apps"].sort(key=lambda a: a["id"])
    catalog["updated"] = datetime.date.today().isoformat()
    if args.featured or not catalog.get("featured"):
        catalog["featured"] = app_id
    catalog_path.parent.mkdir(parents=True, exist_ok=True)
    catalog_path.write_text(json.dumps(catalog, ensure_ascii=False, indent=1) + "\n", encoding="utf-8")

    sig = store / "catalog.json.sig"
    r = subprocess.run(["openssl", "dgst", "-sha256", "-sign", str(args.key), "-out", str(sig), str(catalog_path)])
    if r.returncode != 0:
        return fail("openssl could not sign the catalog")
    print(f"publish: {app_id} {version} in {store} ({entry['size']} bytes); catalog signed")
    print(f"publish: upload all of {store}/ to the store's address (e.g. rsync -av {store}/ host:store/)")
    return 0


def sign(store: pathlib.Path, key: pathlib.Path) -> bool:
    r = subprocess.run(["openssl", "dgst", "-sha256", "-sign", str(key), "-out", str(store / "catalog.json.sig"),
                        str(store / "catalog.json")])
    return r.returncode == 0


def init(args) -> int:
    """The store's first catalog (no apps), or the one there signed again."""
    if not args.key.is_file():
        return fail(f"no key at {args.key} (make store-keys makes one)")
    path = args.store / "catalog.json"
    args.store.mkdir(parents=True, exist_ok=True)
    if not path.is_file():
        catalog = {"format": FORMAT, "updated": datetime.date.today().isoformat(), "featured": "", "apps": []}
        path.write_text(json.dumps(catalog, ensure_ascii=False, indent=1) + "\n", encoding="utf-8")
    if not sign(args.store, args.key):
        return fail("openssl could not sign the catalog")
    print(f"publish: {path} signed; upload all of {args.store}/ to the store's address")
    return 0


def _older(a: str, b: str) -> bool:
    pa = [int(x) for x in a.split(".")]
    pb = [int(x) for x in b.split(".")]
    n = max(len(pa), len(pb))
    return pa + [0] * (n - len(pa)) < pb + [0] * (n - len(pb))


if __name__ == "__main__":
    sys.exit(main())

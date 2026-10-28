#!/usr/bin/env python3
"""Serve dist/<id>.app.tar so a PeOS VM can install it with appget.

Only that one file is served. The script prints the command to type in
PeOS's Terminale for the usual VM networks; Ctrl+C stops it.
"""

from __future__ import annotations

import argparse
import http.server
import pathlib
import socket
import sys


def host_addresses() -> list[str]:
    """This machine's IPv4 addresses (for bridged networks and UTM's shared one)."""
    found: list[str] = []
    try:
        for info in socket.getaddrinfo(socket.gethostname(), None, socket.AF_INET):
            ip = info[4][0]
            if not ip.startswith("127.") and ip not in found:
                found.append(ip)
    except OSError:
        pass
    try:
        # the address of the default route, without sending anything
        with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as s:
            s.connect(("192.0.2.1", 9))
            ip = s.getsockname()[0]
            if not ip.startswith("127.") and ip not in found:
                found.append(ip)
    except OSError:
        pass
    return found


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("archive", type=pathlib.Path, help="e.g. dist/myapp.app.tar")
    ap.add_argument("--port", type=int, default=8000)
    args = ap.parse_args()

    archive = args.archive.resolve()
    if not archive.is_file():
        print(f"serve_app: {archive} not found (run make pack first)", file=sys.stderr)
        return 1
    url_path = "/" + archive.name

    class Handler(http.server.BaseHTTPRequestHandler):
        def do_GET(self) -> None:
            if self.path != url_path:
                self.send_error(404, "only " + url_path + " is served here")
                return
            data = archive.read_bytes()   # read on every request: a rebuild is picked up
            self.send_response(200)
            self.send_header("Content-Type", "application/x-tar")
            self.send_header("Content-Length", str(len(data)))
            self.end_headers()
            self.wfile.write(data)

        def log_message(self, fmt: str, *a) -> None:
            print(f"  {self.client_address[0]} {fmt % a}")

    server = http.server.ThreadingHTTPServer(("0.0.0.0", args.port), Handler)
    print(f"Serving {archive.name} on port {args.port}. In PeOS, open Terminale and type:")
    print()
    print(f"  appget http://10.0.2.2:{args.port}{url_path}     (QEMU, VirtualBox NAT)")
    for ip in host_addresses():
        print(f"  appget http://{ip}:{args.port}{url_path}")
    print()
    print("UTM's shared network: the address of this Mac on bridge100 (often 192.168.64.1).")
    print("Ctrl+C to stop.")
    try:
        server.serve_forever()
    except KeyboardInterrupt:
        pass
    return 0


if __name__ == "__main__":
    sys.exit(main())

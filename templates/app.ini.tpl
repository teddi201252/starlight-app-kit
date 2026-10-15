# Starlight app bundle
title={{APP_NAME}}
exec=./app.bin
args=
# Rights the app may use (fs.read, fs.write, process, display, input, audio,
# network, mount, power, debug, shm, install, manage). Omitted = fs.read only.
capabilities=fs.read
# Parts of the filesystem the app may touch, as path:rights (r, w or rw).
# Omitted = read-only access to everything (/:r).
paths=/:r
icon=
width=480
height=320

[resources]
# icon=./resources/icon.png

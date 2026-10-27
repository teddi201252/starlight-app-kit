# Starlight app bundle
title={{APP_NAME}}
# The app manager's filter: sistema, strumenti or svago.
category={{CATEGORY}}
exec=./app.bin
args=
# Rights the app may use (fs.read, fs.write, process, display, input, audio,
# network, mount, power, debug, shm, install, manage). Omitted = fs.read only.
capabilities=fs.read
# Parts of the filesystem the app may touch, as path:rights (r, w or rw).
# Omitted = read-only access to everything (/:r).
paths=/:r
icon=
# The body's size when the window opens (the frame is Starlight's).
width={{WIDTH}}
height={{HEIGHT}}
# The window can be resized down to min_width x min_height (default: width x
# height) and up to max_width x max_height (default: the screen).
# resizable=0 keeps it at width x height.
{{SIZE_LIMITS}}

[resources]
# icon=./resources/icon.png

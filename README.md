# Edited with LLM, beware of slop. I carefully ensured it works well, but be aware.

# Chip's Workshop (modified fork)

Chip's Workshop is a level editor for *Chip's Challenge* level sets (`.dat` / `.ccl`, MSCC format).
This repository is an **unofficial, modified fork** of Chip's Workshop 2.0.7 by Christopher Elsby.
It fixes rendering on modern Linux desktops and adds a few editing conveniences.

> **Not official.** This fork is not maintained or endorsed by the original author of Chip's Workshop.
> Chip's Workshop is an independent, fan-made tool. It is not affiliated with or endorsed by the owners of
> *Chip's Challenge*, and "Chip's Challenge" is a trademark of its respective owner.

## What's different from the original

All changes are dated 2026 and are marked at the top of each modified source file. They are also listed in
[`ChangeLog`](ChangeLog), and the user's guide ([`doc/chipw.html`](doc/chipw.html)) describes the new features.

### Fixes
- **The map and tile panel didn't redraw on modern Linux.** On wxWidgets 3.2 / GTK3, the map and the
  tile selector didn't update after edits until the window was resized.
  - The main cause was the XIM input method (`GTK_IM_MODULE=xim`). Chip's Workshop now turns it off for
    itself at startup.
  - The editor no longer draws directly onto the window from outside its repaint, which GTK3 doesn't
    support. All drawing now happens in the paint handler.
  - The tileset loads before the first paint, and the tile panel repaints when its tab is shown.
- **Invalid tiles left a hole.** Tiles with an invalid byte value were skipped instead of being drawn.
- **Level saving started from an uninitialised value.** The map compression could write a zero-length run
  into the saved file (for example when the last upper-layer tile was byte `0xFF`).

### New features
- **Map zoom.**
  - Buttons above the map: **−**, **1:1** and **+**.
  - The mouse wheel over the map zooms around the cursor.
  - 1:1 is the size set in *View → Scale tiles*. Zoom levels run from 25% to 800%, limited by the maximum
    tile size (128 px by default; configurable with `MaxTileWidth`, `MaxTileHeight` and `MaxTileArea`).
- **Moving the view.** When the map is larger than the window, a move icon appears in its top-left corner.
  Drag it with the left mouse button to move the view.
  - The cursor stays where you pressed, so you can drag any distance.
  - Clicks on the icon never edit the map.
  - All existing mouse controls are unchanged.
- **Labelled invalid tiles.**
  - Tile bytes above `0x6F` are drawn as black squares showing their hex value (for example `F4`).
  - When an invalid tile sits on top of another tile, its value moves to the top of the tile, and an
    outlined hole shows the tile underneath. If that tile is also invalid, its value is shown in the hole.

### Files changed
`src/app.cpp`, `src/app.h`, `src/bufferscroll.cpp`, `src/bufferscroll.h`, `src/level.cpp`, `src/leveleditids.h`,
`src/leveleditview.cpp`, `src/leveleditview.h`, `src/tileset.cpp`, `src/tileset.h`, `doc/chipw.html`, `ChangeLog`

## Building on Linux

### Requirements
- A C++ compiler and `make`
- wxWidgets 3.x development files (GTK3 build). This fork was developed and tested with wxWidgets 3.2.

For example, on Debian or Ubuntu:

```bash
sudo apt install build-essential libwxgtk3.2-dev
# Only needed if you must regenerate the build system:
sudo apt install autoconf automake
```

### Build and install

```bash
./configure
make
sudo make install
```

This installs the program, the tilesets and the user's guide under `/usr/local`. Then start it with:

```bash
chipw
```

If there is no `configure` script, generate it first with `autoreconf -fi`. This needs the wxWidgets
development files installed, because they provide the `wxwin.m4` macros.

After changing any header file, rebuild from clean with `make clean && make`.

### Running without installing system-wide

The program looks for its tilesets in the install location chosen by `./configure`. If you run `./chipw`
from the build folder without installing, it can't find them and the map stays blank. Either install to a
folder in your home directory:

```bash
./configure --prefix="$HOME/.local"
make
make install
~/.local/bin/chipw
```

or choose a tileset once with *View → Select tileset* (for example `tilesets/default` in this folder).
Chip's Workshop remembers that choice.

### Notes
- If you see a warning about the XIM input method at startup, you can ignore it. Chip's Workshop works
  around it automatically.
- There is also an Inno Setup script (`chipw.iss`) from the original Windows release. This fork has only
  been built and tested on Linux.

## Credits
- **Chip's Workshop:** Copyright 2008–2011, 2021 Christopher Elsby. Original author of the program.
- **Included tile images:** Anders Kaseorg and Michael Hansen (the Tile World tileset), in `tilesets/`.
- **Fork modifications (2026):** the Chip's Workshop fork contributors.

## License

Chip's Workshop is free software: you can redistribute it and/or modify it under the terms of
**version 3 of the GNU General Public License** as published by the Free Software Foundation
(GPL-3.0-only). See [`COPYING`](COPYING) for the full text.

This program is distributed in the hope that it will be useful, but **without any warranty**; without even
the implied warranty of merchantability or fitness for a particular purpose. See the GNU General Public
License for more details.

All modifications in this fork are released under the same license.

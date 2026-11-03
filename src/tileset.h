// Chip's Workshop - a level editor for Chip's Challenge.
// Copyright 2008-2011,2021 Christopher Elsby <chrise@chrise.me.uk>
// Modified 2026 by the Chip's Workshop fork contributors (see README.md for a list of changes).
// This is an unofficial modified version; it is not endorsed by the original author.
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of version 3 of the GNU General Public License as
// published by the Free Software Foundation.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#ifndef CHIPW_TILESET_H_INCLUDED
#define CHIPW_TILESET_H_INCLUDED

#include "tile.h"
#include <wx/defs.h>
#include <wx/string.h>

class wxDC;

namespace ChipW {

class Level;
class TilesetData;

class Tileset {
public:
    Tileset();
    Tileset(const wxString& filename, wxCoord width = -1, wxCoord height = -1);
    Tileset(const Tileset& copy);
    ~Tileset();
    bool IsOk() const {return data != NULL && w > 0 && h > 0;}
    bool LoadFile(const wxString& filename, wxCoord width = -1, wxCoord height = -1);
    bool Resize(wxCoord width = -1, wxCoord height = -1);
    // Checks (without reporting errors) whether tiles of this size are allowed.
    static bool IsSizeSupported(wxCoord width, wxCoord height);
    wxCoord GetTileWidth() const {return w;}
    wxCoord GetTileHeight() const {return h;}
    wxCoord GetNativeTileWidth() const;
    wxCoord GetNativeTileHeight() const;
    Tileset& operator=(const Tileset& copy);
    bool DrawTile(Tile tile, wxDC* target, wxCoord x, wxCoord y);
    bool DrawPair(Tile upper, Tile lower, wxDC* target, wxCoord x, wxCoord y);
    bool DrawLevel(const Level* level, wxDC* target, wxCoord x, wxCoord y, bool drawupper = true, bool drawlower = true);
private:
    // Dimensions.
    wxCoord w, h;
    // Image data.
    TilesetData* data;
};

}

#endif // !CHIPW_TILESET_H_INCLUDED

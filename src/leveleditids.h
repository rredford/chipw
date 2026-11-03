// Chip's Workshop - a level editor for Chip's Challenge.
// Copyright 2008-2011,2021 Christopher Elsby <chrise@chrise.me.uk>
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

#ifndef CHIPW_LEVELEDITIDS_H_INCLUDED
#define CHIPW_LEVELEDITIDS_H_INCLUDED

namespace ChipW {

enum {
    ID_SHOWWIRES = wxID_HIGHEST + 1,
    ID_SHOWUPPER,
    ID_SETTILESET,
    ID_SETTILESIZE,
    ID_EDIT_MONSTERS,
    ID_EDIT_WIRES,
    ID_CLEARSELTILES,
    ID_READONLY,
    ID_SWAPBUTTONS,
    ID_NEWVIEW,
    ID_CLOSEVIEW,
    ID_ROTATE_TILE_ANTICLOCKWISE,
    ID_ROTATE_TILE_CLOCKWISE,
    ID_PLAYTEST_CONFIG,
    ID_PLAYTEST_MIN,
    ID_PLAYTEST_MAX = ID_PLAYTEST_MIN + 8,
};

}

#endif // !CHIPW_LEVELEDITIDS_H_INCLUDED

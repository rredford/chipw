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

#ifndef CHIPW_TILE_H_INCLUDED
#define CHIPW_TILE_H_INCLUDED

#include <wx/defs.h>
#include <wx/string.h>

namespace ChipW {

typedef wxUint8 Tile;
enum {
    TILE_FLOOR,
    TILE_WALL,
    TILE_COMPUTERCHIP,
    TILE_WATER,
    TILE_FIRE,
    TILE_WALL_INV,
    TILE_WALL_N,
    TILE_WALL_W,
    TILE_WALL_S,
    TILE_WALL_E,
    TILE_BLOCK,
    TILE_DIRT,
    TILE_ICE,
    TILE_FORCE_S,
    TILE_BLOCK_N,
    TILE_BLOCK_W,
    TILE_BLOCK_S,
    TILE_BLOCK_E,
    TILE_FORCE_N,
    TILE_FORCE_E,
    TILE_FORCE_W,
    TILE_EXIT,
    TILE_DOOR_B,
    TILE_DOOR_R,
    TILE_DOOR_G,
    TILE_DOOR_Y,
    TILE_ICE_CURVE_SE,
    TILE_ICE_CURVE_SW,
    TILE_ICE_CURVE_NW,
    TILE_ICE_CURVE_NE,
    TILE_WALL_BLUE_FAKE,
    TILE_WALL_BLUE,
    TILE_INVALID_OVERLAYBUF,
    TILE_THIEF,
    TILE_SOCKET,
    TILE_BUTTON_TOGGLE,
    TILE_BUTTON_CLONE,
    TILE_WALL_TOGGLE_ON,
    TILE_WALL_TOGGLE_OFF,
    TILE_BUTTON_TRAP,
    TILE_BUTTON_TANK,
    TILE_TELEPORT,
    TILE_BOMB,
    TILE_TRAP,
    TILE_WALL_INV_SHOWS,
    TILE_GRAVEL,
    TILE_WALL_POPUP,
    TILE_HINT,
    TILE_WALL_SE,
    TILE_CLONER,
    TILE_FORCE_RAND,
    TILE_INVALID_SPLASH,
    TILE_INVALID_CHIP_BURNED,
    TILE_INVALID_CHIP_BOMBED,
    TILE_INVALID_EXPLOSION,
    TILE_INVALID_DISINTEGRATE,
    TILE_INVALID_PGCHIP_ICEBLOCK,
    TILE_INVALID_CHIP_EXIT,
    TILE_INVALID_EXIT_1,
    TILE_INVALID_EXIT_2,
    TILE_INVALID_CHIP_SWIM_N,
    TILE_INVALID_CHIP_SWIM_W,
    TILE_INVALID_CHIP_SWIM_S,
    TILE_INVALID_CHIP_SWIM_E,
    TILE_BUG_N,
    TILE_BUG_W,
    TILE_BUG_S,
    TILE_BUG_E,
    TILE_FIREBALL_N,
    TILE_FIREBALL_W,
    TILE_FIREBALL_S,
    TILE_FIREBALL_E,
    TILE_BALL_N,
    TILE_BALL_W,
    TILE_BALL_S,
    TILE_BALL_E,
    TILE_TANK_N,
    TILE_TANK_W,
    TILE_TANK_S,
    TILE_TANK_E,
    TILE_GLIDER_N,
    TILE_GLIDER_W,
    TILE_GLIDER_S,
    TILE_GLIDER_E,
    TILE_TEETH_N,
    TILE_TEETH_W,
    TILE_TEETH_S,
    TILE_TEETH_E,
    TILE_WALKER_N,
    TILE_WALKER_W,
    TILE_WALKER_S,
    TILE_WALKER_E,
    TILE_BLOB_N,
    TILE_BLOB_W,
    TILE_BLOB_S,
    TILE_BLOB_E,
    TILE_PARAMECIUM_N,
    TILE_PARAMECIUM_W,
    TILE_PARAMECIUM_S,
    TILE_PARAMECIUM_E,
    TILE_KEY_B,
    TILE_KEY_R,
    TILE_KEY_G,
    TILE_KEY_Y,
    TILE_BOOTS_WATER,
    TILE_BOOTS_FIRE,
    TILE_BOOTS_ICE,
    TILE_BOOTS_FORCE,
    TILE_CHIP_N,
    TILE_CHIP_W,
    TILE_CHIP_S,
    TILE_CHIP_E,
    TILE_INVALID,
    TILE_MIN_VALID = TILE_FLOOR,
    TILE_MAX_VALID = TILE_CHIP_E,
    TILE_MIN = 0x00,
    TILE_MAX = 0xFF,
};

const wxUint16 TILE_COUNT = 256;

inline bool IsTile(wxUint32 tile)
    {return tile <= TILE_MAX;}
inline bool IsTileValid(wxUint32 tile)
    {return (tile <= TILE_FORCE_RAND && tile != TILE_INVALID_OVERLAYBUF) || (tile >= TILE_BUG_N && tile <= TILE_CHIP_E);}
inline bool IsTileChip(wxUint32 tile)
    {return tile >= TILE_CHIP_N && tile <= TILE_CHIP_E;}
inline bool IsTileMonster(wxUint32 tile)
    {return tile >= TILE_BUG_N && tile <= TILE_PARAMECIUM_E;}
inline bool IsTileCloneBlock(wxUint32 tile)
    {return tile >= TILE_BLOCK_N && tile <= TILE_BLOCK_E;}
inline bool IsTileBlock(wxUint32 tile)
    {return tile == TILE_BLOCK || IsTileCloneBlock(tile);}
inline bool IsTileWireable(wxUint32 tile)
    {return tile == TILE_TRAP || tile == TILE_BUTTON_TRAP || tile == TILE_CLONER || tile == TILE_BUTTON_CLONE;}
inline bool IsTileUpper(wxUint32 tile)
    {return IsTileChip(tile) || IsTileMonster(tile) || IsTileBlock(tile);}

wxString GetTileName(wxUint32 tile, bool hexcode = false);

enum TileReorientation {
    REORIENTATE_IDENTITY,
    REORIENTATE_ROTATE_ANTICLOCKWISE,
    REORIENTATE_ROTATE_HALFTURN,
    REORIENTATE_ROTATE_CLOCKWISE,
};

Tile ReorientateTile(Tile tile, TileReorientation reorientation);

}

#endif // !CHIPW_TILE_H_INCLUDED

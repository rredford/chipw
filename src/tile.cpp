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

#include "global.h"

#include "tile.h"
#include <wx/string.h>

namespace ChipW {

namespace {

const wxChar* tilenames[] = {
    wxT("empty"),
    wxT("wall"),
    wxT("computer chip"),
    wxT("water"),
    wxT("fire"),
    wxT("invisible wall (permanent)"),
    wxT("thin wall (north)"),
    wxT("thin wall (west)"),
    wxT("thin wall (south)"),
    wxT("thin wall (east)"),
    wxT("block"),
    wxT("dirt"),
    wxT("ice"),
    wxT("force floor (south)"),
    wxT("block (north)"),
    wxT("block (west)"),
    wxT("block (south)"),
    wxT("block (east)"),
    wxT("force floor (north)"),
    wxT("force floor (east)"),
    wxT("force floor (west)"),
    wxT("exit"),
    wxT("door (blue)"),
    wxT("door (red)"),
    wxT("door (green)"),
    wxT("door (yellow)"),
    wxT("ice curve (south <-> east)"),
    wxT("ice curve (south <-> west)"),
    wxT("ice curve (north <-> west)"),
    wxT("ice curve (north <-> east)"),
    wxT("blue wall (fake)"),
    wxT("blue wall (real)"),
    wxT("invalid (overlay buffer)"),
    wxT("thief"),
    wxT("socket"),
    wxT("toggle button"),
    wxT("clone button"),
    wxT("toggle wall (on)"),
    wxT("toggle wall (off)"),
    wxT("trap button"),
    wxT("tank button"),
    wxT("teleport"),
    wxT("bomb"),
    wxT("trap"),
    wxT("invisible wall (appears)"),
    wxT("gravel"),
    wxT("popup wall"),
    wxT("hint"),
    wxT("thin wall (south-east)"),
    wxT("clone machine"),
    wxT("force floor (random)"),
    wxT("invalid (water splash)"),
    wxT("invalid (burned Chip)"),
    wxT("invalid (bombed Chip)"),
    wxT("invalid (TW explosion)"),
    wxT("invalid (TW disintegrate)"),
    wxT("invalid (pgchip ice block)"),
    wxT("invalid (exited Chip)"),
    wxT("invalid (fake exit 1)"),
    wxT("invalid (fake exit 2)"),
    wxT("invalid (swimming Chip, north)"),
    wxT("invalid (swimming Chip, west)"),
    wxT("invalid (swimming Chip, south)"),
    wxT("invalid (swimming Chip, east)"),
    wxT("bug (north)"),
    wxT("bug (west)"),
    wxT("bug (south)"),
    wxT("bug (east)"),
    wxT("fireball (north)"),
    wxT("fireball (west)"),
    wxT("fireball (south)"),
    wxT("fireball (east)"),
    wxT("ball (north)"),
    wxT("ball (west)"),
    wxT("ball (south)"),
    wxT("ball (east)"),
    wxT("tank (north)"),
    wxT("tank (west)"),
    wxT("tank (south)"),
    wxT("tank (east)"),
    wxT("glider (north)"),
    wxT("glider (west)"),
    wxT("glider (south)"),
    wxT("glider (east)"),
    wxT("teeth (north)"),
    wxT("teeth (west)"),
    wxT("teeth (south)"),
    wxT("teeth (east)"),
    wxT("walker (north)"),
    wxT("walker (west)"),
    wxT("walker (south)"),
    wxT("walker (east)"),
    wxT("blob (north)"),
    wxT("blob (west)"),
    wxT("blob (south)"),
    wxT("blob (east)"),
    wxT("paramecium (north)"),
    wxT("paramecium (west)"),
    wxT("paramecium (south)"),
    wxT("paramecium (east)"),
    wxT("key (blue)"),
    wxT("key (red)"),
    wxT("key (green)"),
    wxT("key (yellow)"),
    wxT("water boots"),
    wxT("fire boots"),
    wxT("ice boots"),
    wxT("force boots"),
    wxT("Chip (north)"),
    wxT("Chip (west)"),
    wxT("Chip (south)"),
    wxT("Chip (east)"),
};

const wxChar* hexchars = wxT("0123456789ABCDEF");

const Tile rotations[] = {
    TILE_FLOOR,
    TILE_WALL,
    TILE_COMPUTERCHIP,
    TILE_WATER,
    TILE_FIRE,
    TILE_WALL_INV,
    TILE_WALL_W,
    TILE_WALL_S,
    TILE_WALL_E,
    TILE_WALL_N,
    TILE_BLOCK,
    TILE_DIRT,
    TILE_ICE,
    TILE_FORCE_E,
    TILE_BLOCK_W,
    TILE_BLOCK_S,
    TILE_BLOCK_E,
    TILE_BLOCK_N,
    TILE_FORCE_W,
    TILE_FORCE_N,
    TILE_FORCE_S,
    TILE_EXIT,
    TILE_DOOR_B,
    TILE_DOOR_R,
    TILE_DOOR_G,
    TILE_DOOR_Y,
    TILE_ICE_CURVE_NE,
    TILE_ICE_CURVE_SE,
    TILE_ICE_CURVE_SW,
    TILE_ICE_CURVE_NW,
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
    TILE_INVALID,
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
    TILE_INVALID_CHIP_SWIM_W,
    TILE_INVALID_CHIP_SWIM_S,
    TILE_INVALID_CHIP_SWIM_E,
    TILE_INVALID_CHIP_SWIM_N,
    TILE_BUG_W,
    TILE_BUG_S,
    TILE_BUG_E,
    TILE_BUG_N,
    TILE_FIREBALL_W,
    TILE_FIREBALL_S,
    TILE_FIREBALL_E,
    TILE_FIREBALL_N,
    TILE_BALL_W,
    TILE_BALL_S,
    TILE_BALL_E,
    TILE_BALL_N,
    TILE_TANK_W,
    TILE_TANK_S,
    TILE_TANK_E,
    TILE_TANK_N,
    TILE_GLIDER_W,
    TILE_GLIDER_S,
    TILE_GLIDER_E,
    TILE_GLIDER_N,
    TILE_TEETH_W,
    TILE_TEETH_S,
    TILE_TEETH_E,
    TILE_TEETH_N,
    TILE_WALKER_W,
    TILE_WALKER_S,
    TILE_WALKER_E,
    TILE_WALKER_N,
    TILE_BLOB_W,
    TILE_BLOB_S,
    TILE_BLOB_E,
    TILE_BLOB_N,
    TILE_PARAMECIUM_W,
    TILE_PARAMECIUM_S,
    TILE_PARAMECIUM_E,
    TILE_PARAMECIUM_N,
    TILE_KEY_B,
    TILE_KEY_R,
    TILE_KEY_G,
    TILE_KEY_Y,
    TILE_BOOTS_WATER,
    TILE_BOOTS_FIRE,
    TILE_BOOTS_ICE,
    TILE_BOOTS_FORCE,
    TILE_CHIP_W,
    TILE_CHIP_S,
    TILE_CHIP_E,
    TILE_CHIP_N,
};

}

wxString GetTileName(wxUint32 tile, bool hexcode) {
    if(tile > TILE_MAX)
        return wxT("");
    wxString name;
    if(hexcode || !IsTileValid(tile)) {
        name << wxT("[") << hexchars[(tile / 16) % 16] << hexchars[tile % 16] << wxT("] ");
    }
    if(tile > TILE_MAX_VALID)
        name << wxT("invalid");
    else
        name << tilenames[tile];
    return name;
}

Tile ReorientateTile(Tile tile, TileReorientation reorientation) {
    if(reorientation < REORIENTATE_IDENTITY || reorientation > REORIENTATE_ROTATE_CLOCKWISE)
        return TILE_INVALID;
    for(int i = 0; i < reorientation; ++i) {
        if(tile > TILE_MAX_VALID)
            break;
        tile = rotations[tile];
    }
    return tile;
}

}


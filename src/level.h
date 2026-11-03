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

#ifndef CHIPW_LEVEL_H_INCLUDED
#define CHIPW_LEVEL_H_INCLUDED

#include <wx/defs.h>
#include <wx/string.h>
#include "tile.h"
#include "refcount.h"
#include <list>
#include <string>
#include <istream>
#include <ostream>

namespace ChipW {

struct MonsterRecord {
    wxUint8 x;
    wxUint8 y;
};

inline bool operator==(const MonsterRecord& a, const MonsterRecord& b) {return a.x == b.x && a.y == b.y;}
inline bool operator!=(const MonsterRecord& a, const MonsterRecord& b) {return !(a == b);}
bool operator<(const MonsterRecord& a, const MonsterRecord& b);
inline bool operator>(const MonsterRecord& a, const MonsterRecord& b) {return b < a;}
inline bool operator<=(const MonsterRecord& a, const MonsterRecord& b) {return !(b < a);}
inline bool operator>=(const MonsterRecord& a, const MonsterRecord& b) {return !(a < b);}

struct WireRecord {
    wxUint16 buttonx;
    wxUint16 buttony;
    wxUint16 targetx;
    wxUint16 targety;
};

inline bool operator==(const WireRecord& a, const WireRecord& b)
    {return a.buttonx == b.buttonx && a.buttony == b.buttony && a.targetx == b.targetx && a.targety == b.targety;}
inline bool operator!=(const WireRecord& a, const WireRecord& b) {return !(a == b);}
bool operator<(const WireRecord& a, const WireRecord& b);
inline bool operator>(const WireRecord& a, const WireRecord& b) {return b < a;}
inline bool operator<=(const WireRecord& a, const WireRecord& b) {return !(b < a);}
inline bool operator>=(const WireRecord& a, const WireRecord& b) {return !(a < b);}

class LevelMonitor;

class Level : public RefCounted {
public:
    Level(wxUint16 number = 0);
    LevelMonitor* GetMonitor() const {return monitor;}
    void SetMonitor(LevelMonitor* mon) {monitor = mon;}
    void RemoveMonitor() {monitor = NULL;}
    void RemoveMonitor(LevelMonitor* mon) {if(mon == monitor) monitor = NULL;}
    // Tiles.
    Tile GetUpperTile(wxUint32 x, wxUint32 y) const {return (x < 32 && y < 32) ? upper[x + 32 * y] : TILE_FLOOR;}
    Tile GetLowerTile(wxUint32 x, wxUint32 y) const {return (x < 32 && y < 32) ? lower[x + 32 * y] : TILE_FLOOR;}
    bool HasTile(wxUint32 x, wxUint32 y, Tile tile) const
        {return x < 32 && y < 32 && (upper[x + 32 * y] == tile || lower[x + 32 * y] == tile);}
    bool HasCloner(wxUint32 x, wxUint32 y) const
        {return x < 32 && y < 32 && (upper[x + 32 * y] == TILE_CLONER || lower[x + 32 * y] == TILE_CLONER || (upper[x + 32 * y] == TILE_INVALID_PGCHIP_ICEBLOCK && IsTileCloneBlock(lower[x + 32 * y])));}
    bool PlaceTileUpper(wxUint32 x, wxUint32 y, Tile tile, bool enablemonster = true);
    bool PlaceTileLower(wxUint32 x, wxUint32 y, Tile tile, bool enablemonster = true);
    bool PlaceTileOnly(wxUint32 x, wxUint32 y, Tile tile, bool enablemonster = true)
        {return PlaceTileUpper(x, y, tile, enablemonster) && PlaceTileLower(x, y, TILE_FLOOR);}
    bool ClearTile(wxUint32 x, wxUint32 y)
        {return PlaceTileUpper(x, y, TILE_FLOOR) && PlaceTileLower(x, y, TILE_FLOOR);}
    bool PlaceTileAuto(wxUint32 x, wxUint32 y, Tile tile, bool enablemonster = true);
    bool RemoveProperUpper(wxUint32 x, wxUint32 y);
    wxString GetTileDescription(wxUint32 x, wxUint32 y) const;
    wxString GetMonsterDescription(wxUint32 x, wxUint32 y) const;
    // Counts the chips in the level.
    wxUint16 CountChips(bool checkupper = true, bool checklower = true) const;
    // Returns a random password of 4 capital letters.
    static std::string RandPsw();
    // Monster movement list.
    bool HasMonsterTile(wxUint32 x, wxUint32 y) const
        {return x < 32 && y < 32 && (IsTileMonster(upper[x + 32 * y]) || IsTileMonster(lower[x + 32 * y]));}
    bool IsMonsterEnabled(wxUint32 x, wxUint32 y) const;
    bool AddMonster(wxUint32 x, wxUint32 y, bool dup = false);
    void RemoveMonster(wxUint32 x, wxUint32 y);
    // Wires.
    bool AddTrapWire(const WireRecord& record, bool dup = false);
    bool AddCloneWire(const WireRecord& record, bool dup = false);
    bool SetTrapWire(const WireRecord& record);
    bool SetCloneWire(const WireRecord& record);
    wxUint32 RemoveWire(wxUint32 x, wxUint32 y, bool trapbutton, bool trap, bool clonebutton, bool cloner);
    wxUint32 RemoveWire(wxUint32 x, wxUint32 y) {return RemoveWire(x, y, true, true, true, true);}
    wxUint32 RemoveInvalidWire(wxUint32 x, wxUint32 y)
        {return RemoveWire(x, y, !HasTile(x, y, TILE_BUTTON_TRAP), !HasTile(x, y, TILE_TRAP), !HasTile(x, y, TILE_BUTTON_CLONE), !HasCloner(x, y));}
    wxUint32 RemoveWire(wxUint32 x, wxUint32 y, Tile tile)
        {return RemoveWire(x, y, tile == TILE_BUTTON_TRAP, tile == TILE_TRAP, tile == TILE_BUTTON_CLONE, tile == TILE_CLONER);}
    // Storage.
    bool Save_SubMSCC(std::ostream& stream, bool savelen = true) const;
    bool Load_SubMSCC(std::istream& stream, bool loadlen = true, wxUint16 length = 0);
    // Data.
    wxUint16 levelnumber;
    wxUint16 time;
    wxUint16 chips;
    std::string title;
    std::string hint;
    std::string psw;
    std::list<MonsterRecord> monsters;
    std::list<WireRecord> trapwires;
    std::list<WireRecord> clonewires;
    Tile upper[1024];
    Tile lower[1024];
private:
    LevelMonitor* monitor;
};

}

#endif // !CHIPW_LEVEL_H_INCLUDED

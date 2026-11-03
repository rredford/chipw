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

#include "level.h"
#include "levelchange.h"
#include "tile.h"
#include <cstring>
#include <istream>
#include <ostream>
#include "binstream.h"
#include <wx/defs.h>
#include <wx/log.h>
#include <cstdlib>

namespace ChipW {

bool operator<(const MonsterRecord& a, const MonsterRecord& b) {
    if(a.y < b.y)
        return true;
    if(a.y > b.y)
        return false;
    return a.x < b.x;
}

bool operator<(const WireRecord& a, const WireRecord& b) {
    if(a.buttony < b.buttony)
        return true;
    if(a.buttony > b.buttony)
        return false;
    if(a.buttonx < b.buttonx)
        return true;
    if(a.buttonx > b.buttonx)
        return false;
    if(a.targety < b.targety)
        return true;
    if(a.targety > b.targety)
        return false;
    return a.targetx < b.targetx;
}

Level::Level(wxUint16 number)
 : levelnumber(number), time(0), chips(0), title("Untitled"), psw(RandPsw()), monitor(NULL)
{
    memset(upper, 0, 1024);
    memset(lower, 0, 1024);
}

bool Level::PlaceTileUpper(wxUint32 x, wxUint32 y, Tile tile, bool enablemonster) {
    if(x >= 32 || y >= 32)
        return tile == TILE_FLOOR;
    if(enablemonster && IsTileMonster(tile))
        AddMonster(x, y);
    else if(!IsTileMonster(lower[x + 32 * y]))
        RemoveMonster(x, y);
#if 0
    if(upper[x + 32 * y] == tile)
        return true;
    if(lower[x + 32 * y] != upper[x + 32 * y])
        RemoveWire(x, y, upper[x + 32 * y]);
#endif
    Tile old = upper[x + 32 * y];
    upper[x + 32 * y] = tile;
    RemoveInvalidWire(x, y);
    if(tile != old && monitor != NULL)
        monitor->OnLevelChange(this, new LevelChangeTile(LevelChange::UPPER, x, y, old, tile));
    return true;
}

bool Level::PlaceTileLower(wxUint32 x, wxUint32 y, Tile tile, bool enablemonster) {
    if(x >= 32 || y >= 32)
        return tile == TILE_FLOOR;
    if(enablemonster && IsTileMonster(tile))
        AddMonster(x, y);
    else if(!IsTileMonster(upper[x + 32 * y]))
        RemoveMonster(x, y);
#if 0
    if(lower[x + 32 * y] == tile)
        return true;
    if(upper[x + 32 * y] != lower[x + 32 * y])
        RemoveWire(x, y, lower[x + 32 * y]);
#endif
    Tile old = lower[x + 32 * y];
    lower[x + 32 * y] = tile;
    RemoveInvalidWire(x, y);
    if(tile != old && monitor != NULL)
        monitor->OnLevelChange(this, new LevelChangeTile(LevelChange::LOWER, x, y, old, tile));
    return true;
}

bool Level::PlaceTileAuto(wxUint32 x, wxUint32 y, Tile tile, bool enablemonster) {
    if(x >= 32 || y >= 32)
        return tile == TILE_FLOOR;
    Tile& u = upper[x + 32 * y];
    Tile& l = lower[x + 32 * y];
    if(IsTileUpper(tile)) {
        if(l == TILE_FLOOR && !IsTileUpper(u) && l != u) {
            Tile old = l;
            l = u;
            if(monitor != NULL)
                monitor->OnLevelChange(this, new LevelChangeTile(LevelChange::LOWER, x, y, old, u));
        }
        if(!PlaceTileUpper(x, y, tile, enablemonster))
            return false;
    } else {
        if(IsTileUpper(u)) {
            if(!PlaceTileLower(x, y, tile, enablemonster))
                return false;
        } else {
            if(!PlaceTileOnly(x, y, tile, enablemonster))
                return false;
        }
    }
    return true;
}

bool Level::RemoveProperUpper(wxUint32 x, wxUint32 y) {
    if(x >= 32 || y >= 32)
        return true;
    Tile& u = upper[x + 32 * y];
    Tile& l = lower[x + 32 * y];
    if(!IsTileUpper(u))
        return true;
    return PlaceTileUpper(x, y, l) && PlaceTileLower(x, y, TILE_FLOOR);
#if 0
    Tile tile = l;
    l = u;
    u = tile;
    if(monitor != NULL)
        monitor->OnLevelChange(this, new LevelChangeTile(LevelChange::UPPER, x, y, l, tile));
    if(!PlaceTileLower(x, y, TILE_FLOOR))
        return false;
    return true;
#endif
}

wxString Level::GetTileDescription(wxUint32 x, wxUint32 y) const {
    if(x >= 32 || y >= 32)
        return wxT("INVALID POSITION");
    wxString desc;
    desc << wxT("Upper: ");
    desc << GetTileName(GetUpperTile(x, y)) << wxT(", Lower: ") << GetTileName(GetLowerTile(x, y));
    if(HasMonsterTile(x, y)) {
        if(!IsMonsterEnabled(x, y))
            desc << wxT(", STATIONARY MONSTER");
    } else {
        if(IsMonsterEnabled(x, y))
            desc << wxT(", INVALID MONSTER");
    }
    std::list<WireRecord>::const_iterator it;
    for(it = trapwires.begin(); it != trapwires.end(); ++it) {
        if(it->buttonx == x && it->buttony == y)
            desc << wxT(", TRAP -> (") << (int) it->targetx << wxT(", ") << (int) it->targety << wxT(")");
        else if(it->targetx == x && it->targety == y)
            desc << wxT(", TRAP <- (") << (int) it->buttonx << wxT(", ") << (int) it->buttony << wxT(")");
    }
    for(it = clonewires.begin(); it != clonewires.end(); ++it) {
        if(it->buttonx == x && it->buttony == y)
            desc << wxT(", CLONE -> (") << (int) it->targetx << wxT(", ") << (int) it->targety << wxT(")");
        else if(it->targetx == x && it->targety == y)
            desc << wxT(", CLONE <- (") << (int) it->buttonx << wxT(", ") << (int) it->buttony << wxT(")");
    }
    return desc;
}

wxString Level::GetMonsterDescription(wxUint32 x, wxUint32 y) const {
    if(x >= 32 || y >= 32)
        return wxT("INVALID POSITION");
    wxString desc;
    Tile upper = GetUpperTile(x, y);
    Tile lower = GetLowerTile(x, y);
    if(IsTileMonster(upper)) {
        desc << GetTileName(upper);
        if(IsTileMonster(lower))
            desc << wxT(", ") << GetTileName(lower);
    } else if(IsTileMonster(lower)) {
        desc << GetTileName(lower);
    } else {
        desc << wxT("INVALID MONSTER");
    }
    return desc;
}

wxUint16 Level::CountChips(bool checkupper, bool checklower) const {
    wxUint16 mapchips = 0;
    wxUint16 pos;
    for(pos = 0; pos < 1024; ++pos) {
        if(checkupper && upper[pos] == TILE_COMPUTERCHIP)
            ++mapchips;
        if(checklower && lower[pos] == TILE_COMPUTERCHIP)
            ++mapchips;
    }
    return mapchips;
}

std::string Level::RandPsw() {
    std::string psw;
    for(int i = 0; i < 4; ++i)
        psw.push_back('A' + rand() % 26);
    return psw;
}

bool Level::IsMonsterEnabled(wxUint32 x, wxUint32 y) const {
    if(x >= 256 || y >= 256)
        return false;
    std::list<MonsterRecord>::const_iterator it;
    for(it = monsters.begin(); it != monsters.end(); ++it) {
        if(it->x == x && it->y == y)
            return true;
    }
    return false;
}

bool Level::AddMonster(wxUint32 x, wxUint32 y, bool dup) {
    if(x >= 256 || y >= 256)
        return false;
    MonsterRecord record;
    record.x = x;
    record.y = y;
    bool haveinspos = false;
    std::list<MonsterRecord>::iterator inspos;
    wxUint32 insindex;
    std::list<MonsterRecord>::iterator it;
    wxUint32 index;
    for(it = monsters.begin(), index = 0; it != monsters.end(); ++it, ++index) {
        if(!dup && record == *it)
            return true;
        if(!haveinspos && record <= *it) {
            inspos = it;
            insindex = index;
            haveinspos = true;
            if(dup)
                break;
        }
    }
    if(haveinspos) {
        monsters.insert(inspos, record);
        if(monitor != NULL)
            monitor->OnLevelChange(this, new LevelChangeMonster(LevelChange::ADDMONSTER, insindex, record));
    } else {
        monsters.push_back(record);
        if(monitor != NULL)
            monitor->OnLevelChange(this, new LevelChangeMonster(LevelChange::ADDMONSTER, monsters.size() - 1, record));
    }
    return true;
}

void Level::RemoveMonster(wxUint32 x, wxUint32 y) {
    if(x >= 256 || y >= 256)
        return;
    std::list<MonsterRecord>::iterator it = monsters.begin();
    std::list<MonsterRecord>::iterator it2;
    MonsterRecord record;
    wxUint32 index = 0;
    while(it != monsters.end()) {
        if(it->x == x && it->y == y) {
            it2 = it;
            ++it;
            record = *it2;
            monsters.erase(it2);
            if(monitor != NULL)
                monitor->OnLevelChange(this, new LevelChangeMonster(LevelChange::DELMONSTER, index, record));
        } else {
            ++it;
            ++index;
        }
    }
}

namespace {

bool DoAddWire(CountedPtr<Level> lev, std::list<WireRecord>& wires, LevelMonitor* monitor, wxUint8 type, const WireRecord& record, bool dup) {
    bool haveinspos = false;
    std::list<WireRecord>::iterator inspos;
    wxUint32 insindex;
    std::list<WireRecord>::iterator it;
    wxUint32 index;
    for(it = wires.begin(), index = 0; it != wires.end(); ++it, ++index) {
        if(!dup && record == *it)
            return true;
        if(!haveinspos && record <= *it) {
            inspos = it;
            insindex = index;
            haveinspos = true;
            if(dup)
                break;
        }
    }
    if(haveinspos) {
        wires.insert(inspos, record);
        if(monitor != NULL)
            monitor->OnLevelChange(lev, new LevelChangeWire(type, insindex, record));
    } else {
        wires.push_back(record);
        if(monitor != NULL)
            monitor->OnLevelChange(lev, new LevelChangeWire(type, wires.size() - 1, record));
    }
    return true;
}

class WirePredicate {
public:
    virtual bool operator()(const WireRecord& record) const = 0;
    virtual ~WirePredicate() { }
};

wxUint32 DoRemoveWire(CountedPtr<Level> lev, std::list<WireRecord>& wires, LevelMonitor* monitor, wxUint8 type, const WirePredicate& pred) {
    wxUint32 n = 0;
    std::list<WireRecord>::iterator it, it2;
    WireRecord record;
    wxUint32 index;
    it = wires.begin();
    index = 0;
    while(it != wires.end()) {
        if(pred(*it)) {
            ++n;
            it2 = it;
            ++it;
            record = *it2;
            wires.erase(it2);
            if(monitor != NULL)
                monitor->OnLevelChange(lev, new LevelChangeWire(type, index, record));
        } else {
            ++it;
            ++index;
        }
    }
    return n;
}

class RemoveWirePredicate : public WirePredicate {
public:
    RemoveWirePredicate(wxUint16 newx, wxUint16 newy, bool newbutton, bool newtarget)
        : x(newx), y(newy), button(newbutton), target(newtarget) { }
    bool operator()(const WireRecord& record) const
        {return (button && x == record.buttonx && y == record.buttony) || (target && x == record.targetx && y == record.targety);}
    wxUint16 x, y;
    bool button, target;
};

class SetWirePredicate : public WirePredicate {
public:
    SetWirePredicate(const WireRecord& record)
        : x(record.buttonx), y(record.buttony), x2(record.targetx), y2(record.targety) { }
    bool operator()(const WireRecord& record) const
        {return x == record.buttonx && y == record.buttony && (x2 != record.targetx || y2 != record.targety);}
    wxUint16 x, y;
    wxUint16 x2, y2;
};

}

bool Level::AddTrapWire(const WireRecord& record, bool dup) {
    return DoAddWire(this, trapwires, monitor, LevelChange::ADDTRAPWIRE, record, dup);
}

bool Level::AddCloneWire(const WireRecord& record, bool dup) {
    return DoAddWire(this, clonewires, monitor, LevelChange::ADDCLONEWIRE, record, dup);
}

bool Level::SetTrapWire(const WireRecord& record) {
    DoRemoveWire(this, trapwires, monitor, LevelChange::DELTRAPWIRE, SetWirePredicate(record));
    return DoAddWire(this, trapwires, monitor, LevelChange::ADDTRAPWIRE, record, false);
}

bool Level::SetCloneWire(const WireRecord& record) {
    DoRemoveWire(this, clonewires, monitor, LevelChange::DELCLONEWIRE, SetWirePredicate(record));
    return DoAddWire(this, clonewires, monitor, LevelChange::ADDCLONEWIRE, record, false);
}

wxUint32 Level::RemoveWire(wxUint32 x, wxUint32 y, bool trapbutton, bool trap, bool clonebutton, bool cloner) {
    if(x > 65535 || y > 65535)
        return 0;
    wxUint32 n = 0;
    if(trapbutton || trap)
        n += DoRemoveWire(this, trapwires, monitor, LevelChange::DELTRAPWIRE, RemoveWirePredicate(x, y, trapbutton, trap));
    if(clonebutton || cloner)
        n += DoRemoveWire(this, clonewires, monitor, LevelChange::DELCLONEWIRE, RemoveWirePredicate(x, y, clonebutton, cloner));
    return n;
}

bool Level::Save_SubMSCC(std::ostream& stream, bool savelen) const {
    // Check that the field sizes will fit in a byte.
    if(title.size() > 254) {
        wxLogError(wxT("Title exceeds 254 characters."));
        return false;
    }
    if(hint.size() > 254) {
        wxLogError(wxT("Hint exceeds 254 characters."));
        return false;
    }
    if(psw.size() > 254) {
        wxLogError(wxT("Password exceeds 254 characters."));
        return false;
    }
    if(trapwires.size() > 25) {
        wxLogError(wxT("Trap wire list is longer than 25."));
        return false;
    }
    if(clonewires.size() > 31) {
        wxLogError(wxT("Clone wire list is longer than 31."));
        return false;
    }
    if(monsters.size() > 127) {
        wxLogError(wxT("Monster list is longer than 127."));
        return false;
    }
    // Calculate the length of the data after the map detail.
    wxUint32 sublength = 0;
    if(!title.empty())
        sublength += 3 + title.size();
    if(!hint.empty())
        sublength += 3 + hint.size();
    if(!psw.empty())
        sublength += 3 + psw.size();
    if(!trapwires.empty())
        sublength += 2 + trapwires.size() * 10;
    if(!clonewires.empty())
        sublength += 2 + clonewires.size() * 8;
    if(!monsters.empty())
        sublength += 2 + monsters.size() * 2;
    // This needs to be stored in 2 bytes.
    if(sublength > 65535) {
        wxLogError(wxT("The level data is too long to be saved."));
        return false;
    }
    // Generate the RLE-encoded map detail.
    std::string dataupper;
    std::string datalower;
    {
        wxUint8 tile, len;
        wxUint16 pos;
        // Upper.
        len = 0;
        for(pos = 0; pos < 1024; ++pos) {
            if(len < 255 && upper[pos] == tile) {
                ++len;
            } else {
                if(len <= 3 && tile != 255) {
                    while(len > 0) {
                        dataupper.push_back(tile);
                        --len;
                    }
                } else {
                    dataupper.push_back(255);
                    dataupper.push_back(len);
                    dataupper.push_back(tile);
                }
                tile = upper[pos];
                len = 1;
            }
        }
        if(len <= 3 && tile != 255) {
            while(len > 0) {
                dataupper.push_back(tile);
                --len;
            }
        } else if(len > 0) {
            dataupper.push_back(255);
            dataupper.push_back(len);
            dataupper.push_back(tile);
        }
        // Lower.
        len = 0;
        for(pos = 0; pos < 1024; ++pos) {
            if(len < 255 && lower[pos] == tile) {
                ++len;
            } else {
                if(len <= 3 && tile != 255) {
                    while(len > 0) {
                        datalower.push_back(tile);
                        --len;
                    }
                } else {
                    datalower.push_back(255);
                    datalower.push_back(len);
                    datalower.push_back(tile);
                }
                tile = lower[pos];
                len = 1;
            }
        }
        if(len <= 3 && tile != 255) {
            while(len > 0) {
                datalower.push_back(tile);
                --len;
            }
        } else {
            datalower.push_back(255);
            datalower.push_back(len);
            datalower.push_back(tile);
        }
    }
    // Generate the "encrypted" password.
    std::string encpsw = psw;
    {
        size_t pos;
        for(pos = 0; pos < encpsw.size(); ++pos) {
            if(encpsw[pos] != 0 && (wxUint8) encpsw[pos] != 0x99)
                encpsw[pos] ^= 0x99;
        }
    }
    // Calculate the total length of level data.
    wxUint32 length = sublength + dataupper.size() + datalower.size() + 14;
    // Actually save the data.
    // Length.
    if(savelen) {
        if(length > 65535) {
            wxLogError(wxT("The level data is too long to be saved."));
            return false;
        }
        if(!WriteLE(stream, (wxUint16) length))
            return false;
    }
    // Level number.
    if(!WriteLE(stream, levelnumber))
        return false;
    // Time limit.
    if(!WriteLE(stream, time))
        return false;
    // Chips required.
    if(!WriteLE(stream, chips))
        return false;
    // Field 1: Map detail.
    // Header.
    if(!WriteLE(stream, (wxUint16) 1))
        return false;
    // Length of upper layer.
    if(!WriteLE(stream, (wxUint16) dataupper.size()))
        return false;
    // Upper layer data.
    if(!stream.write(dataupper.data(), dataupper.size()))
        return false;
    // Length of lower layer.
    if(!WriteLE(stream, (wxUint16) datalower.size()))
        return false;
    // Lower layer data.
    if(!stream.write(datalower.data(), datalower.size()))
        return false;
    // Remaining length.
    if(!WriteLE(stream, (wxUint16) sublength))
        return false;
    // Field 3: Level title.
    if(!title.empty()) {
        // Header.
        if(!WriteLE(stream, (wxUint8) 3))
            return false;
        // Length of title + terminating 0 byte.
        if(!WriteLE(stream, (wxUint8) (title.size() + 1)))
            return false;
        // Title.
        if(!stream.write(title.data(), title.size()))
            return false;
        // 0 byte.
        if(!WriteLE(stream, (wxUint8) 0))
            return false;
    }
    // Field 7: Hint text.
    if(!hint.empty()) {
        // Header.
        if(!WriteLE(stream, (wxUint8) 7))
            return false;
        // Length of hint + terminating 0 byte.
        if(!WriteLE(stream, (wxUint8) (hint.size() + 1)))
            return false;
        // Hint.
        if(!stream.write(hint.data(), hint.size()))
            return false;
        // 0 byte.
        if(!WriteLE(stream, (wxUint8) 0))
            return false;
    }
    // Field 6: Password.
    if(!encpsw.empty()) {
        // Header.
        if(!WriteLE(stream, (wxUint8) 6))
            return false;
        // Length of password + terminating 0 byte.
        if(!WriteLE(stream, (wxUint8) (encpsw.size() + 1)))
            return false;
        // Password.
        if(!stream.write(encpsw.data(), encpsw.size()))
            return false;
        // 0 byte.
        if(!WriteLE(stream, (wxUint8) 0))
            return false;
    }
    // Field 4: Trap wires.
    if(!trapwires.empty()) {
        // Header.
        if(!WriteLE(stream, (wxUint8) 4))
            return false;
        // Length of field.
        if(!WriteLE(stream, (wxUint8) (trapwires.size() * 10)))
            return false;
        // Wire data.
        for(std::list<WireRecord>::const_iterator it = trapwires.begin(); it != trapwires.end(); ++it) {
            if(!WriteLE(stream, it->buttonx))
                return false;
            if(!WriteLE(stream, it->buttony))
                return false;
            if(!WriteLE(stream, it->targetx))
                return false;
            if(!WriteLE(stream, it->targety))
                return false;
            if(!WriteLE(stream, (wxUint16) 0))
                return false;
        }
    }
    // Field 5: Clone wires.
    if(!clonewires.empty()) {
        // Header.
        if(!WriteLE(stream, (wxUint8) 5))
            return false;
        // Length of field.
        if(!WriteLE(stream, (wxUint8) (clonewires.size() * 8)))
            return false;
        // Wire data.
        for(std::list<WireRecord>::const_iterator it = clonewires.begin(); it != clonewires.end(); ++it) {
            if(!WriteLE(stream, it->buttonx))
                return false;
            if(!WriteLE(stream, it->buttony))
                return false;
            if(!WriteLE(stream, it->targetx))
                return false;
            if(!WriteLE(stream, it->targety))
                return false;
            // Extra zero intentionally omitted - for some reason it is used in field 4 but not field 5.
        }
    }
    // Field 10: Monsters.
    if(!monsters.empty()) {
        // Header.
        if(!WriteLE(stream, (wxUint8) 10))
            return false;
        // Length of field.
        if(!WriteLE(stream, (wxUint8) (monsters.size() * 2)))
            return false;
        // Monster data.
        for(std::list<MonsterRecord>::const_iterator it = monsters.begin(); it != monsters.end(); ++it) {
            if(!WriteLE(stream, it->x))
                return false;
            if(!WriteLE(stream, it->y))
                return false;
        }
    }
    // The end.
    return true;
}

#define CheckedReadLE(stream, data, length) { \
    if((length) < sizeof(data)) \
        goto overrun; \
    (length) -= sizeof(data); \
    if(!ReadLE((stream), (data))) \
        return false; \
}

#define CheckedReadBuf(stream, buf, bytes, length) { \
    if((length) < (bytes)) \
        goto overrun; \
    (length) -= (bytes); \
    if(!stream.read((buf), (bytes))) \
        return false; \
}

#define CheckedIgnore(stream, bytes, length) { \
    if((length) < (bytes)) \
        goto overrun; \
    (length) -= (bytes); \
    if(!stream.ignore((bytes))) \
        return false; \
}

bool Level::Load_SubMSCC(std::istream& stream, bool loadlen, wxUint16 length) {
    bool clean = true;
    // Length of level data.
    if(loadlen) {
        if(!ReadLE(stream, length))
            return false;
    }
    // Clear level data.
    levelnumber = 0;
    time = 0;
    chips = 0;
    title.clear();
    hint.clear();
    psw.clear();
    monsters.clear();
    trapwires.clear();
    clonewires.clear();
    memset(upper, 0, 1024);
    memset(lower, 0, 1024);
    // Level number.
    CheckedReadLE(stream, levelnumber, length);
    // Time limit.
    CheckedReadLE(stream, time, length);
    // Chips required.
    CheckedReadLE(stream, chips, length);
    // Read fields.
    wxUint8 fieldid;
    wxUint16 sublen;
    wxUint8 tile, len;
    wxUint16 pos;
    char buf[256];
    WireRecord wire;
    MonsterRecord monster;
    while(length > 0) {
        CheckedReadLE(stream, fieldid, length);
        switch(fieldid) {
        case 1:
            // Map detail.
            // Skip extra byte.
            CheckedIgnore(stream, 1, length);
            // Length of upper layer.
            CheckedReadLE(stream, sublen, length);
            if(sublen > length)
                goto overrun;
            length -= sublen;
            // Upper layer data.
            pos = 0;
            while(pos < 1024 && sublen > 0) {
                if(!ReadLE(stream, tile))
                    return false;
                --sublen;
                if(tile == 255) {
                    // RLE
                    if(sublen < 2) {
                        wxLogError(wxT("Incomplete RLE in upper layer."));
                        stream.ignore(sublen);
                        sublen = 0;
                        clean = false;
                        break;
                    }
                    sublen -= 2;
                    if(!ReadLE(stream, len))
                        return false;
                    if(!ReadLE(stream, tile))
                        return false;
                    while(len > 0 && pos < 1024) {
                        upper[pos] = tile;
                        ++pos;
                        --len;
                    }
                    if(len > 0) {
                        wxLogError(wxT("More than 1024 tiles in upper layer."));
                        stream.ignore(sublen);
                        sublen = 0;
                        clean = false;
                    }
                } else {
                    // Plain tile.
                    upper[pos] = tile;
                    ++pos;
                }
            }
            if(pos < 1024) {
                wxLogError(wxT("Fewer than 1024 tiles in upper layer."));
                clean = false;
            } else if(sublen > 0) {
                wxLogError(wxT("More than 1024 tiles in upper layer."));
                stream.ignore(sublen);
                clean = false;
            }
            // Length of lower layer.
            CheckedReadLE(stream, sublen, length);
            if(sublen > length)
                goto overrun;
            length -= sublen;
            // Lower layer data.
            pos = 0;
            while(pos < 1024 && sublen > 0) {
                if(!ReadLE(stream, tile))
                    return false;
                --sublen;
                if(tile == 255) {
                    // RLE
                    if(sublen < 2) {
                        wxLogError(wxT("Incomplete RLE in lower layer."));
                        stream.ignore(sublen);
                        sublen = 0;
                        clean = false;
                        break;
                    }
                    sublen -= 2;
                    if(!ReadLE(stream, len))
                        return false;
                    if(!ReadLE(stream, tile))
                        return false;
                    while(len > 0 && pos < 1024) {
                        lower[pos] = tile;
                        ++pos;
                        --len;
                    }
                    if(len > 0) {
                        wxLogError(wxT("More than 1024 tiles in lower layer."));
                        stream.ignore(sublen);
                        sublen = 0;
                        clean = false;
                    }
                } else {
                    // Plain tile.
                    lower[pos] = tile;
                    ++pos;
                }
            }
            if(pos < 1024) {
                wxLogError(wxT("Fewer than 1024 tiles in lower layer."));
                clean = false;
            } else if(sublen > 0) {
                wxLogError(wxT("More than 1024 tiles in lower layer."));
                stream.ignore(sublen);
                clean = false;
            }
            // Remaining length (not used here).
            CheckedReadLE(stream, sublen, length);
            if(sublen != length) {
                wxLogError(wxT("Mid-level offset disagrees with level length."));
                clean = false;
            }
            break;
        case 3:
            // Title.
            // Length of title string.
            CheckedReadLE(stream, len, length);
            // Title string.
            CheckedReadBuf(stream, buf, len, length);
            buf[len] = 0;
            title = buf;
            break;
        case 7:
            // Hint.
            // Length of hint string.
            CheckedReadLE(stream, len, length);
            // Hint string.
            CheckedReadBuf(stream, buf, len, length);
            buf[len] = 0;
            hint = buf;
            break;
        case 6:
            // Password.
            // Length of password string.
            CheckedReadLE(stream, len, length);
            // Password string.
            CheckedReadBuf(stream, buf, len, length);
            buf[len] = 0;
            psw = buf;
            // Decrypt.
            for(pos = 0; pos < psw.size(); ++pos) {
                if(psw[pos] != 0 && (wxUint8) psw[pos] != 0x99)
                    psw[pos] ^= 0x99;
            }
            break;
        case 4:
            // Trap wires.
            // Length of field.
            CheckedReadLE(stream, len, length);
            if(len > length)
                goto overrun;
            length -= len;
            // Wire data.
            while(len >= 10) {
                if(!ReadLE(stream, wire.buttonx))
                    return false;
                if(!ReadLE(stream, wire.buttony))
                    return false;
                if(!ReadLE(stream, wire.targetx))
                    return false;
                if(!ReadLE(stream, wire.targety))
                    return false;
                trapwires.push_back(wire);
                // And those extra 2 bytes at the end......
                if(!stream.ignore(2))
                    return false;
                len -= 10;
            }
            if(len > 0) {
                wxLogError(wxT("Trap wire field length is %i more than a multiple of 10."), (int) len);
                clean = false;
                stream.ignore(len);
            }
            break;
        case 5:
            // Clone wires.
            // Length of field.
            CheckedReadLE(stream, len, length);
            if(len > length)
                goto overrun;
            length -= len;
            // Wire data.
            while(len >= 8) {
                if(!ReadLE(stream, wire.buttonx))
                    return false;
                if(!ReadLE(stream, wire.buttony))
                    return false;
                if(!ReadLE(stream, wire.targetx))
                    return false;
                if(!ReadLE(stream, wire.targety))
                    return false;
                clonewires.push_back(wire);
                // No extra 2 bytes this time.
                len -= 8;
            }
            if(len > 0) {
                wxLogError(wxT("Clone wire field length is %i more than a multiple of 8."), (int) len);
                clean = false;
                stream.ignore(len);
            }
            break;
        case 10:
            // Monsters.
            // Length of field.
            CheckedReadLE(stream, len, length);
            if(len > length)
                goto overrun;
            length -= len;
            // Monster data.
            while(len >= 2) {
                if(!ReadLE(stream, monster.x))
                    return false;
                if(!ReadLE(stream, monster.y))
                    return false;
                monsters.push_back(monster);
                len -= 2;
            }
            if(len > 0) {
                wxLogError(wxT("Monster field length is %i more than a multiple of 2."), (int) len);
                clean = false;
                stream.ignore(len);
            }
            break;
        default:
            wxLogError(wxT("Unknown field: %i"), (int) fieldid);
            stream.ignore(length);
            return false;
        }
    }
    // The end.
    return clean;
overrun:
    wxLogError(wxT("Level data overrun."));
    stream.ignore(length);
    return false;
}

}


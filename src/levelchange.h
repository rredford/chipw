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

#ifndef CHIPW_LEVELCHANGE_H_INCLUDED
#define CHIPW_LEVELCHANGE_H_INCLUDED

#include <wx/defs.h>
#include "level.h"

namespace ChipW {

class LevelChange {
public:
    virtual ~LevelChange() { }
    bool Do(CountedPtr<Level> level, bool reverse = false);
    bool Undo(CountedPtr<Level> level, bool reverse = false) {return Do(level, !reverse);}
    enum {
        UPPER,
        LOWER,
        ADDMONSTER,
        DELMONSTER,
        ADDTRAPWIRE,
        DELTRAPWIRE,
        ADDCLONEWIRE,
        DELCLONEWIRE,
    };
    wxUint8 type;
    bool isdone;
protected:
    LevelChange(wxUint8 newtype) : type(newtype), isdone(true) { }
    virtual bool PerformChange(CountedPtr<Level> level, bool reverse) = 0;
};

class LevelMonitor {
public:
    virtual ~LevelMonitor() { }
    virtual void OnLevelChange(CountedPtr<Level> level, LevelChange* change, bool isnew = true, bool reverse = false) = 0;
};

class LevelChangeTile : public LevelChange {
public:
    LevelChangeTile(wxUint8 newtype, wxUint8 newx, wxUint8 newy, Tile newbefore, Tile newafter)
        : LevelChange(newtype), x(newx), y(newy), before(newbefore), after(newafter) { }
    wxUint8 x, y;
    Tile before, after;
protected:
    virtual bool PerformChange(CountedPtr<Level> level, bool reverse = false);
};

class LevelChangeMonster : public LevelChange {
public:
    LevelChangeMonster(wxUint8 newtype, wxUint32 newindex, const MonsterRecord& newrecord)
        : LevelChange(newtype), index(newindex), record(newrecord) { }
    wxUint32 index;
    MonsterRecord record;
protected:
    virtual bool PerformChange(CountedPtr<Level> level, bool reverse = false);
};

class LevelChangeWire : public LevelChange {
public:
    LevelChangeWire(wxUint8 newtype, wxUint32 newindex, const WireRecord& newrecord)
        : LevelChange(newtype), index(newindex), record(newrecord) { }
    wxUint32 index;
    WireRecord record;
protected:
    virtual bool PerformChange(CountedPtr<Level> level, bool reverse = false);
};

}

#endif // !CHIPW_LEVELCHANGE_H_INCLUDED

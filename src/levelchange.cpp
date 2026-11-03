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

#include "levelchange.h"
#include "level.h"

namespace ChipW {

bool LevelChange::Do(CountedPtr<Level> level, bool reverse) {
    if(reverse != isdone)
        return true;
    if(level == NULL)
        return false;
    if(!PerformChange(level, reverse))
        return false;
    isdone = !reverse;
    if(level->GetMonitor() != NULL)
        level->GetMonitor()->OnLevelChange(level, this, false, reverse);
    return true;
}

bool LevelChangeTile::PerformChange(CountedPtr<Level> level, bool reverse) {
    if(level == NULL || x >= 32 || y >= 32)
        return false;
    if(type != UPPER && type != LOWER)
        return false;
    Tile& tile = ((type == UPPER) ? level->upper : level->lower)[x + 32 * y];
    if(tile != (reverse ? after : before))
        return false;
    tile = (reverse ? before : after);
    return true;
}

bool LevelChangeMonster::PerformChange(CountedPtr<Level> level, bool reverse) {
    if(level == NULL)
        return false;
    if(type == DELMONSTER)
        reverse = !reverse;
    else if(type != ADDMONSTER)
        return false;
    std::list<MonsterRecord>& list = level->monsters;
    std::list<MonsterRecord>::iterator it;
    wxUint32 i;
    if(reverse) {
        // Remove.
        if(index >= list.size())
            return false;
        it = list.begin();
        for(i = 0; i < index; ++i)
            ++it;
        if(*it != record)
            return false;
        list.erase(it);
    } else {
        // Add.
        if(index > list.size())
            return false;
        if(index == list.size()) {
            list.push_back(record);
        } else {
            it = list.begin();
            for(i = 0; i < index; ++i)
                ++it;
            list.insert(it, record);
        }
    }
    return true;
}

bool LevelChangeWire::PerformChange(CountedPtr<Level> level, bool reverse) {
    if(level == NULL)
        return false;
    if(type == DELTRAPWIRE || type == DELCLONEWIRE)
        reverse = !reverse;
    else if(type != ADDTRAPWIRE && type != ADDCLONEWIRE)
        return false;
    std::list<WireRecord>& list = ((type == ADDTRAPWIRE || type == DELTRAPWIRE) ? level->trapwires : level->clonewires);
    std::list<WireRecord>::iterator it;
    wxUint32 i;
    if(reverse) {
        // Remove.
        if(index >= list.size())
            return false;
        it = list.begin();
        for(i = 0; i < index; ++i)
            ++it;
        if(*it != record)
            return false;
        list.erase(it);
    } else {
        // Add.
        if(index > list.size())
            return false;
        if(index == list.size()) {
            list.push_back(record);
        } else {
            it = list.begin();
            for(i = 0; i < index; ++i)
                ++it;
            list.insert(it, record);
        }
    }
    return true;
}

}


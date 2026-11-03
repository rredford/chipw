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

#include "levelset.h"
#include "level.h"
#include "binstream.h"
#include "ruleset.h"
#include <istream>
#include <ostream>
#include <wx/defs.h>
#include <wx/log.h>

namespace ChipW {

LevelSet::LevelSet()
 : ruleset(RULESET_DEFAULT), monitor(NULL)
{
}

LevelSet::~LevelSet() {
    RemoveAllLevels();
}

void LevelSet::SetMonitor(LevelMonitor* mon) {
    monitor = mon;
    for(std::vector<CountedPtr<Level> >::const_iterator it = levels.begin(); it != levels.end(); ++it) {
        if(*it != NULL)
            (*it)->SetMonitor(mon);
    }
}

void LevelSet::RemoveMonitor() {
    monitor = NULL;
    for(std::vector<CountedPtr<Level> >::const_iterator it = levels.begin(); it != levels.end(); ++it) {
        if(*it != NULL)
            (*it)->RemoveMonitor();
    }
}

void LevelSet::RemoveMonitor(LevelMonitor* mon) {
    if(monitor == mon)
        monitor = NULL;
    for(std::vector<CountedPtr<Level> >::const_iterator it = levels.begin(); it != levels.end(); ++it) {
        if(*it != NULL)
            (*it)->RemoveMonitor(mon);
    }
}

wxUint16 LevelSet::FindLevel(const Level* level) const {
    wxUint16 i;
    for(i = 0; i < levels.size(); ++i) {
        if(levels[i] == level)
            break;
    }
    return i;
}

wxUint16 LevelSet::FindLevelByNumber(wxUint16 levnum) const {
    wxUint16 i;
    for(i = 0; i < levels.size(); ++i) {
        if(levels[i]->levelnumber == levnum)
            break;
    }
    return i;
}

wxUint16 LevelSet::FindLevelByPassword(const std::string& psw) const {
    wxUint16 i;
    for(i = 0; i < levels.size(); ++i) {
        if(levels[i]->psw == psw)
            break;
    }
    return i;
}

bool LevelSet::InsertLevel(CountedPtr<Level> level, wxUint16 index, bool renumber) {
    if(level == NULL)
        return false;
    RemoveLevel(FindLevel(level), renumber);
    if(levels.size() >= 65535)
        return false;
    if(renumber) {
        if(levels.empty())
            level->levelnumber = 0;
        else if(index >= levels.size())
            level->levelnumber = levels.back()->levelnumber;
        else
            level->levelnumber = levels[index]->levelnumber - 1;
    }
    if(index < levels.size()) {
        levels.insert(levels.begin() + index, level);
    } else {
        index = levels.size();
        levels.push_back(level);
    }
    if(renumber) {
        while(index < levels.size()) {
            ++levels[index]->levelnumber;
            ++index;
        }
    }
    level->SetMonitor(monitor);
    return true;
}

CountedPtr<Level> LevelSet::CreateLevel(wxUint16 index, bool renumber) {
    CountedPtr<Level> level = new Level;
    if(!InsertLevel(level, index, renumber))
        return NULL;
    return level;
}

void LevelSet::RemoveLevel(wxUint16 index, bool renumber) {
    if(index >= levels.size())
        return;
    levels.erase(levels.begin() + index);
    if(renumber) {
        while(index < levels.size()) {
            --levels[index]->levelnumber;
            ++index;
        }
    }
}

void LevelSet::RemoveAllLevels() {
    levels.clear();
}

bool LevelSet::Save_MSCC(std::ostream& stream) const {
    // Rule set.
    if(!WriteLE(stream, ruleset))
        return false;
    // Level count.
    if(!WriteLE(stream, (wxUint16) levels.size()))
        return false;
    // The levels.
    for(size_t i = 0; i < levels.size(); ++i) {
        if(!levels[i]->Save_SubMSCC(stream)) {
            wxLogError(wxT("Failed to save level [%i]."), (int) i + 1);
            return false;
        }
    }
    return true;
}

bool LevelSet::Load_MSCC(std::istream& stream) {
    RemoveAllLevels();
    // Rule set.
    if(!ReadLE(stream, ruleset))
        return false;
    // Level count.
    wxUint16 count;
    if(!ReadLE(stream, count))
        return false;
    // The levels.
    bool clean = true;
    CountedPtr<Level> lev;
    wxUint16 n = 1;
    while(stream && n <= count) {
        lev = new Level(n);
        if(!lev->Load_SubMSCC(stream)) {
            wxLogError(wxT("An error occurred loading level [%i]."), (int) n);
            clean = false;
        }
        lev->SetMonitor(monitor);
        levels.push_back(lev);
        ++n;
    }
    return clean;
}

}


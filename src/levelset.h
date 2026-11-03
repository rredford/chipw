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

#ifndef CHIPW_LEVELSET_H_INCLUDED
#define CHIPW_LEVELSET_H_INCLUDED

#include <wx/defs.h>
#include <vector>
#include <istream>
#include <ostream>
#include <string>
#include "level.h"

namespace ChipW {

class LevelMonitor;

class LevelSet {
public:
    LevelSet();
    ~LevelSet();
    void SetMonitor(LevelMonitor* mon);
    void RemoveMonitor();
    void RemoveMonitor(LevelMonitor* mon);
    wxUint16 GetLevelCount() const {return levels.size();}
    CountedPtr<Level> GetLevel(wxUint16 index) {return index < levels.size() ? levels[index] : NULL;}
    const Level* GetLevel(wxUint16 index) const {return index < levels.size() ? levels[index] : NULL;}
    wxUint16 FindLevel(const Level* level) const;
    wxUint16 FindLevelByNumber(wxUint16 levnum) const;
    wxUint16 FindLevelByPassword(const std::string& psw) const;
    bool InsertLevel(CountedPtr<Level> level, wxUint16 index = 65535, bool renumber = true);
    CountedPtr<Level> CreateLevel(wxUint16 index = 65535, bool renumber = true);
    void RemoveLevel(wxUint16 index, bool renumber = true);
    void RemoveAllLevels();
    bool Save_MSCC(std::ostream& stream) const;
    bool Load_MSCC(std::istream& stream);
    wxUint32 ruleset;
private:
    LevelMonitor* monitor;
    std::vector<CountedPtr<Level> > levels;
};

}

#endif // !CHIPW_LEVELSET_H_INCLUDED

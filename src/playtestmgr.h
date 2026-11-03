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

#ifndef CHIPW_PLAYTESTMGR_H_INCLUDED
#define CHIPW_PLAYTESTMGR_H_INCLUDED

#include "listmenumgr.h"
#include "playtesttype.h"
#include <vector>

namespace ChipW {

class PlayTestManager : public ListMenuManager {
public:
    PlayTestManager();
    // Gets the current playtesting types.
    const std::vector<PlayTestType>& GetTypes() const {return types;}
    // Sets the playtesting types, optionally saving to config.
    void SetTypes(std::vector<PlayTestType> newtypes, bool saveconfig = true);
    // Loads the playtesting types from config. This is called by the constructor.
    void LoadTypes() {SetTypes(GetConfTypes());}
    static std::vector<PlayTestType> GetConfTypes();
    // Saves the playtesting types to config.
    void SaveTypes() const {SetConfTypes(GetTypes());}
    static void SetConfTypes(const std::vector<PlayTestType>& types);
private:
    std::vector<PlayTestType> types;
};

}

#endif // !CHIPW_PLAYTESTMGR_H_INCLUDED

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

#ifndef CHIPW_LEVELUPDATEHINT_H_INCLUDED
#define CHIPW_LEVELUPDATEHINT_H_INCLUDED

#include <wx/object.h>
#include <wx/defs.h>
#include "level.h"

namespace ChipW {

class LevelUpdateHint : public wxObject {
public:
    enum Type {
        NONE,
        MONSTER,    // Change of monster status of tile (x, y).
        WIRE,       // Change of wires at (x, y) (and potential change in monsters).
        TILE,       // Change of tile (x, y) in level (and potential change in wires/monster); whole map if invalid x or y.
        LIST,       // Change of level list.
        CREATE,     // Creation of level.
        DESTROY,    // Deletion of level.
    };
    LevelUpdateHint() : type(NONE), level(NULL), x(0xFFFFFFFF), y(0xFFFFFFFF) { }
    Type type;
    CountedPtr<Level> level;
    wxUint32 x, y;
private:
    DECLARE_DYNAMIC_CLASS(LevelUpdateHint)
};

}

#endif // !CHIPW_LEVELUPDATEHINT_H_INCLUDED

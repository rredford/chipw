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

#include "ruleset.h"
#include <string>
#include <sstream>
#include <iomanip>

namespace ChipW {

const RuleSetOrNone RuleSetOrNone::NONE;

wxString GetRuleSetString(RuleSetId ruleset) {
    if(ruleset == RULESET_MS) {
        return wxT("MS");
    } else if(ruleset == RULESET_LYNX) {
        return wxT("Lynx");
    } else if(ruleset == RULESET_PGCHIP) {
        return wxT("pgchip");
    } else if(ruleset == RULESET_PGCHIP_LYNX) {
        return wxT("pgchip (Lynx)");
    } else {
        std::ostringstream stream;
        stream << "0x" << std::hex << std::setfill('0') << std::setw(8) << ruleset;
        return wxString(stream.str().c_str(), wxConvUTF8);
    }
}

RuleSetOrNone GetRuleSetFromString(wxString str) {
    if(str == wxT("MS")) {
        return RULESET_MS;
    } else if(str == wxT("Lynx")) {
        return RULESET_LYNX;
    } else if(str == wxT("pgchip")) {
        return RULESET_PGCHIP;
    } else if(str == wxT("pgchip (Lynx)")) {
        return RULESET_PGCHIP_LYNX;
    } else {
        unsigned long val = 0;
        if(!str.ToULong(&val, 0))
            return RuleSetOrNone::NONE;
        return val;
    }
}

}


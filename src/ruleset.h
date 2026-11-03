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

#ifndef CHIPW_RULESET_H_INCLUDED
#define CHIPW_RULESET_H_INCLUDED

#include <wx/defs.h>
#include <wx/string.h>

namespace ChipW {

typedef wxUint32 RuleSetId;

enum {
    RULESET_MS = 0x0002AAAC,
    RULESET_LYNX = 0x0102AAAC,
    RULESET_PGCHIP = 0x0003AAAC,
    RULESET_PGCHIP_LYNX = 0x0103AAAC,
    RULESET_DEFAULT = RULESET_MS,
};

const unsigned int SUPPORTED_RULESET_COUNT = 4;
const RuleSetId SUPPORTED_RULESETS[] = {RULESET_MS, RULESET_LYNX, RULESET_PGCHIP, RULESET_PGCHIP_LYNX};

struct RuleSetOrNone {
    RuleSetOrNone() : hasruleset(false), ruleset(0) { }
    RuleSetOrNone(RuleSetId newruleset) : hasruleset(true), ruleset(newruleset) { }
    operator bool() const {return hasruleset;}
    bool operator!() const {return !hasruleset;}
    operator RuleSetId() const {return ruleset;}
    bool hasruleset;
    RuleSetId ruleset;
    static const RuleSetOrNone NONE;
};

wxString GetRuleSetString(RuleSetId ruleset);
RuleSetOrNone GetRuleSetFromString(wxString str);

}

#endif // !CHIPW_RULESET_H_INCLUDED

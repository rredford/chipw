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

#ifndef CHIPW_PLAYTESTTYPE_H_INCLUDED
#define CHIPW_PLAYTESTTYPE_H_INCLUDED

#include "ruleset.h"
#include <wx/defs.h>
#include <wx/string.h>

namespace ChipW {

class PlayTestProcess;
class LevelSet;

class PlayTestType {
public:
    // Constructors.
    PlayTestType(wxString newname = wxT(""), wxString newprogram = GetDefaultProgram(), wxString newarguments = GetDefaultArguments(),
        wxString newworkingdir = wxT(""), const RuleSetOrNone& newruleset = RuleSetOrNone::NONE)
        : name(newname), program(newprogram), arguments(newarguments), workingdir(newworkingdir), ruleset(newruleset) { }
    // Starts playtesting the specified levelset, returning the process object on success and NULL on failure.
    PlayTestProcess* Launch(const LevelSet& levelset, wxUint16 levelnumber = 1) const;
    // Defaults.
    static wxString GetDefaultProgram();
    static wxString GetDefaultArguments();
    // Settings.
    wxString name;
    wxString program;
    wxString arguments;
    wxString workingdir;
    RuleSetOrNone ruleset;
private:
    static int defaultprog;
};

}

#endif // !CHIPW_PLAYTESTTYPE_H_INCLUDED

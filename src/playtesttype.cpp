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

#include "playtesttype.h"
#include "playtestproc.h"
#include "levelset.h"
#include <wx/filename.h>
#include <wx/utils.h>

namespace ChipW {

int PlayTestType::defaultprog = 0;

PlayTestProcess* PlayTestType::Launch(const LevelSet& levelset, wxUint16 levelnumber) const {
    LevelSet tmplevelset(levelset);
    if(ruleset)
        tmplevelset.ruleset = ruleset;
    wxString gamecmd;
    if(program.empty())
        gamecmd = arguments;
    else
        gamecmd = PlayTestProcess::GetArgumentQuoted(program) + wxT(" ") + arguments;
    PlayTestProcess* proc = new PlayTestProcess;
    if(!proc->Launch(gamecmd, tmplevelset, levelnumber, workingdir)) {
        delete proc;
        return NULL;
    }
    return proc;
}

wxString PlayTestType::GetDefaultProgram() {
    // We check whether "tworld" exists in PATH and is executable, and thus is likely to work as a command.
    // If so we return it as the default; otherwise, we return the empty string. The result is cached.
    const wxChar* progname = wxT("tworld");
    if(defaultprog == 0) {
        defaultprog = -1;
        wxString pathlist;
        if(!wxGetEnv(wxT("PATH"), &pathlist))
            pathlist.clear();
        size_t beginpos = 0, endpos;
        while(1) {
            endpos = pathlist.find(wxT(':'), beginpos);
            if(endpos < beginpos || endpos > pathlist.size())
                endpos = pathlist.size();
            wxString path = pathlist.substr(beginpos, endpos - beginpos);
            if(!path.empty()) {
                wxFileName filename(path, progname);
                if(filename.FileExists()) {
                    if(filename.IsFileExecutable())
                        defaultprog = 1;
                    break;
                }
            }
            if(endpos >= pathlist.size())
                break;
            beginpos = endpos + 1;
        }
    }
    return defaultprog == 1 ? progname : wxT("");
}

wxString PlayTestType::GetDefaultArguments() {
    return wxT("-pr");
}

}

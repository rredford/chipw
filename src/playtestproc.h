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

#ifndef CHIPW_PLAYTESTPROC_H_INCLUDED
#define CHIPW_PLAYTESTPROC_H_INCLUDED

#include <wx/defs.h>
#include <wx/string.h>
#include <wx/process.h>

namespace ChipW {

class LevelSet;

class PlayTestProcess : public wxProcess {
public:
    PlayTestProcess();
    ~PlayTestProcess();
    static wxString GetArgumentQuoted(wxString arg);
    bool Launch(wxString gamecmd, const LevelSet& levelset, wxUint16 levelnumber = 1, wxString workingdir = wxEmptyString);
private:
    wxString tmpfilename;
};

}

#endif // !CHIPW_PLAYTESTPROC_H_INCLUDED

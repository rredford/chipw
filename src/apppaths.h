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

#ifndef CHIPW_APPPATHS_H_INCLUDED
#define CHIPW_APPPATHS_H_INCLUDED

#include <wx/string.h>

#ifdef CHIPW_DATADIR
namespace ChipW {
inline wxString GetDataDir() {return wxT(CHIPW_DATADIR);}
}
#else
#include <wx/stdpaths.h>
namespace ChipW {
inline wxString GetDataDir() {return wxStandardPaths::Get().GetDataDir();}
}
#endif

#ifdef CHIPW_DOCDIR
namespace ChipW {
inline wxString GetDocDir() {return wxT(CHIPW_DOCDIR);}
}
#else
#include <wx/stdpaths.h>
namespace ChipW {
inline wxString GetDocDir() {return wxStandardPaths::Get().GetDataDir();}
}
#endif

#endif // !CHIPW_APPPATHS_H_INCLUDED

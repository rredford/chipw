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

#include "help.h"
#include "apppaths.h"
#include <wx/utils.h>
#include <wx/msgdlg.h>

namespace ChipW {

void LaunchHelp(const wxString& section) {
    wxString url;
    url << wxT("file://") << GetDocDir() << wxT("/chipw.html");
#if 0 // wxWidgets seems to refuse to start the browser with this...
    if(!section.empty())
        url << wxT("#") << section;
#endif
    if(!wxLaunchDefaultBrowser(url
#if wxCHECK_VERSION(2, 8, 0)
    , wxBROWSER_NEW_WINDOW
#endif
    ))
        wxMessageBox(wxT("Please see chipw.html for help."), wxT("Chip's Workshop Help"), wxOK);
}

}


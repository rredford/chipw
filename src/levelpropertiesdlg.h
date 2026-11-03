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

#ifndef CHIPW_LEVELPROPERTIESDLG_H_INCLUDED
#define CHIPW_LEVELPROPERTIESDLG_H_INCLUDED

#include <wx/dialog.h>
#include <wx/string.h>
#include <wx/event.h>

class wxTextCtrl;

namespace ChipW {

class LevelPropertiesDialog : public wxDialog {
public:
    LevelPropertiesDialog(wxWindow* parent, bool readonly);
    wxString title;
    wxString psw;
    wxString chips;
    wxString countedchips;
    wxString time;
    wxString hint;
    wxString levnum;
    wxUint32 ruleset;
private:
    void OnCommand(wxCommandEvent& event);
    wxTextCtrl* pswbox;
    DECLARE_EVENT_TABLE()
};

}

#endif // !CHIPW_LEVELPROPERTIESDLG_H_INCLUDED

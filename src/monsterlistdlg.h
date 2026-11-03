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

#ifndef CHIPW_MONSTERLISTDLG_H_INCLUDED
#define CHIPW_MONSTERLISTDLG_H_INCLUDED

#include <list>
#include <wx/dialog.h>
#include <wx/event.h>
#include "level.h"

class wxListBox;
class wxTextCtrl;

namespace ChipW {

class MonsterListDialog : public wxDialog {
public:
    MonsterListDialog(wxWindow* parent, CountedPtr<Level> lev, bool isreadonly);
//    void UpdateLevel();
    std::list<MonsterRecord> GetMonsterList() const;
private:
    void OnCommand(wxCommandEvent& event);
    void OnUpdateUI(wxUpdateUIEvent& event);
    void OnListBox(wxCommandEvent& event);
    CountedPtr<Level> level;
    bool readonly;
    wxListBox* monsterlist;
    wxListBox* unaddedlist;
    wxTextCtrl* add_x;
    wxTextCtrl* add_y;
    DECLARE_EVENT_TABLE()
};

}

#endif // !CHIPW_MONSTERLISTDLG_H_INCLUDED

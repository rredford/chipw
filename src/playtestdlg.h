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

#ifndef CHIPW_PLAYTESTDLG_H_INCLUDED
#define CHIPW_PLAYTESTDLG_H_INCLUDED

#include "playtesttype.h"
#include "listeditdlg.h"
#include <wx/dialog.h>
#include <wx/event.h>
#include <wx/object.h>

class wxTextCtrl;

namespace ChipW {

class PlayTestDialog : public wxDialog {
public:
    PlayTestDialog(wxWindow* parent);
    PlayTestType type;
private:
    void OnCommand(wxCommandEvent& event);
    wxTextCtrl* progbox;
    wxTextCtrl* argbox;
    wxTextCtrl* dirbox;
    DECLARE_EVENT_TABLE()
};

class PlayTestListEditItem : public ListEditItem {
public:
    PlayTestListEditItem() { }
    PlayTestListEditItem(const PlayTestType& newtype) : type(newtype) { }
    virtual wxString GetSummaryText() const;
    virtual bool Edit(wxWindow* parent);
    virtual ListEditItem* CreateCopy() const;
    PlayTestType type;
private:
    DECLARE_DYNAMIC_CLASS(PlayTestListEditItem)
};

class PlayTestListEditDialog : public ListEditDialog {
public:
    PlayTestListEditDialog(wxWindow* parent, const std::vector<PlayTestType>& types, unsigned int maxlen = (unsigned int) -1);
    std::vector<PlayTestType> GetTypes() const;
};

}

#endif // !CHIPW_PLAYTESTDLG_H_INCLUDED

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

#include "monsterlistdlg.h"
#include "level.h"
#include <string>
#include <set>
#include <queue>
#include <functional>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/listbox.h>
#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/msgdlg.h>
#include <wx/clntdata.h>
#include <wx/valtext.h>

namespace ChipW {

namespace {

class MonsterRecordData : public wxClientData, public MonsterRecord {
public:
    MonsterRecordData() { }
    MonsterRecordData(const MonsterRecord& copy) {x = copy.x; y = copy.y;}
    MonsterRecordData& operator=(const MonsterRecord& copy) {x = copy.x; y = copy.y; return *this;}
};

}

enum {
    ID_SORT = wxID_HIGHEST + 1,
};

BEGIN_EVENT_TABLE(MonsterListDialog, wxDialog)
    EVT_BUTTON(-1, MonsterListDialog::OnCommand)
    EVT_UPDATE_UI(-1, MonsterListDialog::OnUpdateUI)
    EVT_LISTBOX(-1, MonsterListDialog::OnListBox)
END_EVENT_TABLE()

MonsterListDialog::MonsterListDialog(wxWindow* parent, CountedPtr<Level> lev, bool isreadonly)
 : wxDialog(parent, -1, wxT("Monster list"), wxDefaultPosition, wxDefaultSize),
   level(lev), readonly(isreadonly), monsterlist(NULL), unaddedlist(NULL), add_x(NULL), add_y(NULL)
{
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(new wxStaticText(this, -1, wxT("Monster movement list:")), 0, wxALL, 5);
    monsterlist = new wxListBox(this, -1, wxDefaultPosition, wxSize(50, 100));
    sizer->Add(monsterlist, 1, wxALL | wxGROW, 5);
    wxBoxSizer* listbuttonsizer = new wxBoxSizer(wxHORIZONTAL);
    listbuttonsizer->Add(new wxButton(this, wxID_REMOVE, wxT("Remove")), 1, wxALL | wxGROW, 5);
    listbuttonsizer->Add(new wxButton(this, wxID_UP, wxT("&Up")), 1, wxALL | wxGROW, 5);
    listbuttonsizer->Add(new wxButton(this, wxID_DOWN, wxT("&Down")), 1, wxALL | wxGROW, 5);
    listbuttonsizer->Add(new wxButton(this, ID_SORT, wxT("&Sort")), 1, wxALL | wxGROW, 5);
    sizer->Add(listbuttonsizer, 0, wxGROW);
    wxBoxSizer* addsizer = new wxBoxSizer(wxHORIZONTAL);
    addsizer->Add(new wxStaticText(this, -1, wxT("x:")), 0, wxALL, 5);
    add_x = new wxTextCtrl(this, -1, wxT(""), wxDefaultPosition, wxDefaultSize, 0, wxTextValidator(wxFILTER_NUMERIC));
    addsizer->Add(add_x, 1, wxALL, 5);
    addsizer->Add(new wxStaticText(this, -1, wxT("y:")), 0, wxALL, 5);
    add_y = new wxTextCtrl(this, -1, wxT(""), wxDefaultPosition, wxDefaultSize, 0, wxTextValidator(wxFILTER_NUMERIC));
    addsizer->Add(add_y, 1, wxALL, 5);
    addsizer->Add(new wxButton(this, wxID_ADD, wxT("Add")), 0, wxALL | wxGROW, 5);
    sizer->Add(addsizer, 0, wxGROW);
    sizer->Add(new wxStaticText(this, -1, wxT("Stationary monsters:")), 0, wxALL, 5);
    unaddedlist = new wxListBox(this, -1, wxDefaultPosition, wxSize(50, 100));
    sizer->Add(unaddedlist, 1, wxALL | wxGROW, 5);
#if 0
    wxBoxSizer* buttonsizer = new wxBoxSizer(wxHORIZONTAL);
    wxButton* okbutton = new wxButton(this, wxID_OK, wxT("&OK"));
    okbutton->SetDefault();
    buttonsizer->Add(okbutton, 0, wxALL, 5);
    buttonsizer->Add(new wxButton(this, wxID_CANCEL, wxT("&Cancel")), 0, wxALL, 5);
#endif
    wxSizer* buttonsizer = CreateButtonSizer(wxOK | wxCANCEL);
    if(buttonsizer != NULL)
        sizer->Add(buttonsizer, 0, wxGROW);
    SetSizerAndFit(sizer);
    if(level != NULL) {
        wxString str;
        std::set<MonsterRecord> records;
        for(std::list<MonsterRecord>::const_iterator it = level->monsters.begin(); it != level->monsters.end(); ++it) {
            str.clear();
            str << wxT("(") << (int) it->x << wxT(", ") << (int) it->y << wxT(") - ") << level->GetMonsterDescription(it->x, it->y);
            monsterlist->Append(str, new MonsterRecordData(*it));
            records.insert(*it);
        }
        MonsterRecord record;
        for(record.y = 0; record.y < 32; ++record.y) {
            for(record.x = 0; record.x < 32; ++record.x) {
                if(level->HasMonsterTile(record.x, record.y) && !records.count(record)) {
                    str.clear();
                    str << wxT("(") << (int) record.x << wxT(", ") << (int) record.y << wxT(") - ");
                    str << level->GetMonsterDescription(record.x, record.y);
                    unaddedlist->Append(str, new MonsterRecordData(record));
                }
            }
        }
    }
}

#if 0
void MonsterListDialog::UpdateLevel() {
    if(readonly || level == NULL || monsterlist == NULL)
        return;
    level->monsters.clear();
    for(unsigned int pos = 0; pos < monsterlist->GetCount(); ++pos) {
        if(monsterlist->GetClientObject(pos) != NULL)
            level->monsters.push_back(*((MonsterRecordData*) monsterlist->GetClientObject(pos)));
    }
}
#endif

std::list<MonsterRecord> MonsterListDialog::GetMonsterList() const {
    std::list<MonsterRecord> monsters;
    if(monsterlist != NULL && monsterlist->GetCount() >= 1) {
        unsigned int count = monsterlist->GetCount();
        for(unsigned int pos = 0; pos < count; ++pos) {
            if(monsterlist->GetClientObject(pos) != NULL)
                monsters.push_back(*((MonsterRecordData*) monsterlist->GetClientObject(pos)));
        }
    }
    return monsters;
}

void MonsterListDialog::OnCommand(wxCommandEvent& event) {
    switch(event.GetId()) {
    case wxID_ADD:
    {
        if(readonly || monsterlist == NULL || add_x == NULL || add_y == NULL)
            break;
        unsigned long x, y;
        if(!add_x->GetValue().ToULong(&x) || !add_y->GetValue().ToULong(&y)) {
            wxMessageBox(wxT("Invalid coordinates."), wxT("Add monster"), wxOK | wxICON_ERROR, this);
            break;
        }
        if(x > 255 || y > 255) {
            wxMessageBox(wxT("Only coordinates from 0 to 255 can be stored in the monster list."), wxT("Add monster"), wxOK | wxICON_ERROR, this);
            break;
        }
        wxString str;
        str << wxT("(") << x << wxT(", ") << y << wxT(")");
        if(x >= 32 || y >= 32) {
            if(wxMessageBox(wxT("This point is outside the level map! Are you sure you want to add an invalid monster record for it?"),
            wxT("Add monster"), wxYES_NO | wxICON_QUESTION) != wxYES)
                break;
        } else if(level != NULL && !level->HasMonsterTile(x, y)) {
            if(wxMessageBox(wxT("There is no monster at ") + str + wxT("! Are you sure you want to add an invalid monster record for it?"),
            wxT("Add monster"), wxYES_NO | wxICON_QUESTION) != wxYES)
                break;
        }
        str << wxT(" - ") << level->GetMonsterDescription(x, y);
        int n = monsterlist->FindString(str);
        if(n >= 0 && monsterlist->GetCount() >= 1 && ((unsigned int) n) < ((unsigned int) monsterlist->GetCount())) {
            if(wxMessageBox(wxT("There is already a monster record for this location! Are you sure you want to add a duplicate record?"),
            wxT("Add monster"), wxYES_NO | wxICON_QUESTION) != wxYES)
                break;
        }
        MonsterRecordData* data = new MonsterRecordData;
        data->x = x;
        data->y = y;
        unsigned int pos = 0;
        if(monsterlist->GetCount() >= 1) {
            unsigned int count = monsterlist->GetCount();
            for(pos = 0; pos < count; ++pos) {
                if(monsterlist->GetClientObject(pos) != NULL && *data <= *((MonsterRecordData*) monsterlist->GetClientObject(pos)))
                    break;
            }
        }
        monsterlist->Insert(str, pos, data);
        monsterlist->SetSelection(pos);
        if(unaddedlist != NULL) {
            n = unaddedlist->FindString(str);
            if(n >= 0 && unaddedlist->GetCount() >= 1 && ((unsigned int) n) < ((unsigned int) unaddedlist->GetCount()))
                unaddedlist->Delete(n);
        }
        break;
    }
    case wxID_REMOVE:
    {
        if(readonly || monsterlist == NULL)
            break;
        int n = monsterlist->GetSelection();
        if(n < 0 || monsterlist->GetCount() < 1 || ((unsigned int) n) >= ((unsigned int) monsterlist->GetCount()))
            break;
        wxString str = monsterlist->GetString(n);
        MonsterRecordData* data = NULL;
        if(monsterlist->GetClientObject(n) != NULL)
            data = new MonsterRecordData(*((MonsterRecordData*) monsterlist->GetClientObject(n)));
        monsterlist->Delete(n);
        if(unaddedlist != NULL && level != NULL && data != NULL && !str.empty() && level->HasMonsterTile(data->x, data->y) &&
        monsterlist->FindString(str) == wxNOT_FOUND) {
            unsigned int pos = 0;
            if(unaddedlist->GetCount() > 0) {
                unsigned int count = unaddedlist->GetCount();
                for(pos = 0; pos < count; ++pos) {
                    if(unaddedlist->GetClientObject(pos) != NULL && *data <= *((MonsterRecordData*) unaddedlist->GetClientObject(pos)))
                        break;
                }
            }
            unaddedlist->Insert(str, pos, data);
            unaddedlist->SetSelection(pos);
            if(add_x != NULL) {
                wxString x;
                x << (int) data->x;
                add_x->SetValue(x);
            }
            if(add_y != NULL) {
                wxString y;
                y << (int) data->y;
                add_y->SetValue(y);
            }
        } else {
            delete data;
        }
        break;
    }
    case wxID_UP:
    {
        if(readonly || monsterlist == NULL)
            break;
        int n = monsterlist->GetSelection();
        if(n < 1 || monsterlist->GetCount() < 1 || ((unsigned int) n) >= ((unsigned int) monsterlist->GetCount()))
            break;
        wxString str = monsterlist->GetString(n);
        MonsterRecordData* data = NULL;
        if(monsterlist->GetClientObject(n) != NULL)
            data = new MonsterRecordData(*((MonsterRecordData*) monsterlist->GetClientObject(n)));
        monsterlist->Delete(n);
        monsterlist->Insert(str, n - 1, data);
        monsterlist->SetSelection(n - 1);
        break;
    }
    case wxID_DOWN:
    {
        if(readonly || monsterlist == NULL)
            break;
        int n = monsterlist->GetSelection();
        if(n < 0 || monsterlist->GetCount() < 1 || ((unsigned int) n) >= ((unsigned int) monsterlist->GetCount() - 1))
            break;
        wxString str = monsterlist->GetString(n);
        MonsterRecordData* data = NULL;
        if(monsterlist->GetClientObject(n) != NULL)
            data = new MonsterRecordData(*((MonsterRecordData*) monsterlist->GetClientObject(n)));
        monsterlist->Delete(n);
        monsterlist->Insert(str, n + 1, data);
        monsterlist->SetSelection(n + 1);
        break;
    }
    case ID_SORT:
    {
        if(readonly || monsterlist == NULL || monsterlist->GetCount() < 1)
            break;
        bool dosel = false;
        MonsterRecord sel;
        MonsterRecordData* data;
        if(monsterlist->GetSelection() >= 0 && ((unsigned int) monsterlist->GetSelection()) < ((unsigned int) monsterlist->GetCount())) {
            data = (MonsterRecordData*) monsterlist->GetClientObject(monsterlist->GetSelection());
            if(data != NULL) {
                sel = *data;
                dosel = true;
            }
        }
        std::priority_queue<MonsterRecord, std::vector<MonsterRecord>, std::greater<MonsterRecord> > monsters;
        unsigned int n;
        for(n = 0; n < (unsigned int) monsterlist->GetCount(); ++n) {
            data = (MonsterRecordData*) monsterlist->GetClientObject(n);
            if(data != NULL)
                monsters.push(*data);
        }
        monsterlist->Clear();
        while(!monsters.empty()) {
            wxString str;
            str << wxT("(") << monsters.top().x << wxT(", ") << monsters.top().y << wxT(") - ");
            str << level->GetMonsterDescription(monsters.top().x, monsters.top().y);
            monsterlist->Append(str, new MonsterRecordData(monsters.top()));
            if(dosel && sel == monsters.top() && monsterlist->GetCount() >= 1)
                monsterlist->SetSelection(monsterlist->GetCount() - 1);
            monsters.pop();
        }
        break;
    }
    default:
        event.Skip();
    }
}

void MonsterListDialog::OnUpdateUI(wxUpdateUIEvent& event) {
    switch(event.GetId()) {
    case wxID_ADD:
    {
        event.Enable(false);
        if(readonly || monsterlist == NULL || add_x == NULL || add_y == NULL)
            break;
        unsigned long x, y;
        if(!add_x->GetValue().ToULong(&x) || !add_y->GetValue().ToULong(&y))
            break;
        if(x > 255 || y > 255)
            break;
        event.Enable(true);
        break;
    }
    case wxID_REMOVE:
    {
        event.Enable(false);
        if(readonly || monsterlist == NULL)
            break;
        int n = monsterlist->GetSelection();
        if(n < 0 || monsterlist->GetCount() < 1 || ((unsigned int) n) >= ((unsigned int) monsterlist->GetCount()))
            break;
        event.Enable(true);
        break;
    }
    case wxID_UP:
    {
        event.Enable(false);
        if(readonly || monsterlist == NULL)
            break;
        int n = monsterlist->GetSelection();
        if(n < 1 || monsterlist->GetCount() < 1 || ((unsigned int) n) >= ((unsigned int) monsterlist->GetCount()))
            break;
        event.Enable(true);
        break;
    }
    case wxID_DOWN:
    {
        event.Enable(false);
        if(readonly || monsterlist == NULL)
            break;
        int n = monsterlist->GetSelection();
        if(n < 0 || monsterlist->GetCount() < 1 || ((unsigned int) n) >= ((unsigned int) monsterlist->GetCount() - 1))
            break;
        event.Enable(true);
        break;
    }
    case ID_SORT:
    {
        event.Enable(!readonly && monsterlist != NULL);
        break;
    }
    default:
        event.Skip();
    }
}

#if !wxCHECK_VERSION(2, 8, 0)
#define ChangeValue SetValue
#endif

void MonsterListDialog::OnListBox(wxCommandEvent& event) {
    if(event.GetEventObject() == NULL)
        return;
    if(event.GetEventObject() == unaddedlist) {
        if(event.GetClientObject() != NULL) {
            MonsterRecordData* data = (MonsterRecordData*) event.GetClientObject();
            if(add_x != NULL) {
                wxString x;
                x << (int) data->x;
                add_x->ChangeValue(x);
            }
            if(add_y != NULL) {
                wxString y;
                y << (int) data->y;
                add_y->ChangeValue(y);
            }
        }
    }
}

}

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

#include "listeditdlg.h"
#include <wx/listbox.h>
#include <wx/button.h>
#include <wx/sizer.h>

namespace ChipW {

IMPLEMENT_ABSTRACT_CLASS(ListEditItem, wxObject)

namespace {

class ListEditClientObj : public wxClientData {
public:
    ListEditClientObj(ListEditItem* newitem) : item(newitem) { }
    virtual ~ListEditClientObj() {delete item;}
    ListEditItem* item;
};

}






enum {
#if wxCHECK_VERSION(2, 8, 0)
    ID_EDIT = wxID_EDIT,
#else
    ID_EDIT = wxID_HIGHEST + 1,
#endif
};

BEGIN_EVENT_TABLE(ListEditDialog, wxDialog)
    EVT_BUTTON(-1, ListEditDialog::OnCommand)
    EVT_UPDATE_UI(-1, ListEditDialog::OnUpdateUI)
END_EVENT_TABLE()

ListEditDialog::ListEditDialog(wxWindow* parent, wxWindowID id, const wxString& title, wxClassInfo* newitemtype, unsigned int newmaxlen)
 : wxDialog(parent, id, title), itemtype(newitemtype), maxlen(newmaxlen)
{
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* uppersizer = new wxBoxSizer(wxHORIZONTAL);
    listbox = new wxListBox(this, -1, wxDefaultPosition, wxSize(200, 100));
    uppersizer->Add(listbox, 1, wxALL | wxGROW, 5);
    wxBoxSizer* listbuttonsizer = new wxBoxSizer(wxVERTICAL);
    listbuttonsizer->Add(new wxButton(this, wxID_ADD, wxT("Add")), 0, wxALL | wxGROW, 5);
    listbuttonsizer->Add(new wxButton(this, ID_EDIT, wxT("Edit")), 0, wxALL | wxGROW, 5);
    listbuttonsizer->Add(new wxButton(this, wxID_REMOVE, wxT("Remove")), 0, wxALL | wxGROW, 5);
    listbuttonsizer->Add(new wxButton(this, wxID_UP, wxT("&Up")), 0, wxALL | wxGROW, 5);
    listbuttonsizer->Add(new wxButton(this, wxID_DOWN, wxT("&Down")), 0, wxALL | wxGROW, 5);
    uppersizer->Add(listbuttonsizer, 0, wxGROW);
    sizer->Add(uppersizer, 1, wxGROW);
    wxSizer* buttonsizer = CreateButtonSizer(wxOK | wxCANCEL);
    if(buttonsizer != NULL)
        sizer->Add(buttonsizer, 0, wxGROW);
    SetSizerAndFit(sizer);
}

ListEditDialog::~ListEditDialog() {
}

unsigned int ListEditDialog::GetItemCount() const {
    if(listbox == NULL)
        return 0;
    return listbox->GetCount();
}

ListEditItem* ListEditDialog::GetItem(unsigned int i) const {
    if(i >= GetItemCount())
        return NULL;
    ListEditClientObj* clientobj = (ListEditClientObj*) listbox->GetClientObject(i);
    if(clientobj == NULL)
        return NULL;
    return clientobj->item;
}

void ListEditDialog::AddItem(ListEditItem* newitem) {
    if(newitem != NULL) {
        if(listbox != NULL) {
            int i = listbox->Append(newitem->GetSummaryText(), new ListEditClientObj(newitem));
            if(i >= 0 && (unsigned int) i < (unsigned int) listbox->GetCount()) {
                listbox->SetSelection(i);
                return;
            }
        }
        delete newitem;
    }
}

void ListEditDialog::AddItem(ListEditItem* newitem, unsigned int pos) {
    if(newitem != NULL) {
        if(listbox != NULL) {
            listbox->Insert(newitem->GetSummaryText(), pos, new ListEditClientObj(newitem));
            int i = pos;
            if(i >= 0 && (unsigned int) i < (unsigned int) listbox->GetCount()) {
                listbox->SetSelection(i);
                return;
            }
        }
        delete newitem;
    }
}

void ListEditDialog::DeleteItem(unsigned int i) {
    if(i < GetItemCount()) {
        bool select = listbox->IsSelected(i);
        listbox->Delete(i);
        if(select) {
            if(i < (unsigned int) listbox->GetCount())
                listbox->SetSelection(i);
            else if(listbox->GetCount() > 0)
                listbox->SetSelection(listbox->GetCount() - 1);
        }
    }
}

void ListEditDialog::UpdateItemSummary(unsigned int i) {
    ListEditItem* item = GetItem(i);
    if(item != NULL)
        listbox->SetString(i, item->GetSummaryText());
}

ListEditItem* ListEditDialog::RemoveItem(unsigned int i) {
    if(i >= GetItemCount())
        return NULL;
    // Get a pointer to the item.
    ListEditClientObj* clientobj = (ListEditClientObj*) listbox->GetClientObject(i);
    if(clientobj == NULL)
        return NULL;
    ListEditItem* item = clientobj->item;
    // Clear the client object's pointer so that the item is not deleted.
    clientobj->item = NULL;
    // Now delete the entry from the listbox and return the item.
    DeleteItem(i);
    return item;
}

ListEditItem* ListEditDialog::GetNewItem(bool copyfromsel) const {
    if(itemtype == NULL)
        return NULL;
    ListEditItem* newitem = NULL;
    if(copyfromsel) {
        int sel = listbox->GetSelection();
        if(sel != wxNOT_FOUND && sel >= 0) {
            ListEditItem* item = GetItem(sel);
            if(item != NULL)
                newitem = item->CreateCopy();
        }
    }
    if(newitem == NULL) {
        wxObject* obj = itemtype->CreateObject();
        newitem = wxDynamicCast(obj, ListEditItem);
        if(newitem == NULL)
            delete obj;
    }
    return newitem;
}

void ListEditDialog::OnCommand(wxCommandEvent& event) {
    switch(event.GetId()) {
    case wxID_ADD:
        if(listbox != NULL && GetItemCount() < maxlen) {
            ListEditItem* newitem = GetNewItem();
            if(newitem == NULL)
                break;
            if(newitem->Edit(this))
                AddItem(newitem);
            else
                delete newitem;
        }
        break;
    case ID_EDIT:
        if(listbox != NULL) {
            int sel = listbox->GetSelection();
            if(sel == wxNOT_FOUND || sel < 0)
                break;
            ListEditItem* item = GetItem(sel);
            if(item == NULL)
                break;
            if(item->Edit(this))
                UpdateItemSummary(sel);
        }
        break;
    case wxID_REMOVE:
        if(listbox != NULL) {
            int sel = listbox->GetSelection();
            if(sel == wxNOT_FOUND || sel < 0)
                break;
            DeleteItem(sel);
        }
        break;
    case wxID_UP:
    case wxID_DOWN:
        if(listbox != NULL) {
            int sel = listbox->GetSelection();
            if(sel == wxNOT_FOUND || sel < 0)
                break;
            if(event.GetId() == wxID_UP && sel == 0)
                break;
            if(event.GetId() == wxID_DOWN && (unsigned int) sel == GetItemCount() - 1)
                break;
            ListEditItem* item = RemoveItem(sel);
            if(item == NULL)
                break;
            AddItem(item, sel + (event.GetId() == wxID_UP ? -1 : 1));
        }
        break;
    default:
        event.Skip();
    }
}

void ListEditDialog::OnUpdateUI(wxUpdateUIEvent& event) {
    switch(event.GetId()) {
    case wxID_ADD:
        event.Enable(listbox != NULL && GetItemCount() < maxlen);
        break;
    case ID_EDIT:
    case wxID_REMOVE:
    case wxID_UP:
    case wxID_DOWN:
        event.Enable(false);
        if(listbox != NULL) {
            int sel = listbox->GetSelection();
            if(sel == wxNOT_FOUND || sel < 0)
                break;
            if(event.GetId() == wxID_UP && sel == 0)
                break;
            if(event.GetId() == wxID_DOWN && (unsigned int) sel == GetItemCount() - 1)
                break;
            if(GetItem(sel) == NULL)
                break;
            event.Enable(true);
        }
        break;
    default:
        event.Skip();
    }
}

}


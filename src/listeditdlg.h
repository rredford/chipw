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

#ifndef CHIPW_LISTEDITDLG_H_INCLUDED
#define CHIPW_LISTEDITDLG_H_INCLUDED

#include <wx/string.h>
#include <wx/dialog.h>
#include <wx/clntdata.h>
#include <wx/object.h>
#include <wx/event.h>
#include <vector>

class wxListBox;
class wxWindow;

namespace ChipW {

// Abstract class to represent an item in the list.
class ListEditItem : public wxObject {
public:
    virtual ~ListEditItem() { }
    // Returns the text to represent the item in the list editor.
    virtual wxString GetSummaryText() const = 0;
    // Runs a modal dialog to edit the item and returns whether OK was chosen.
    virtual bool Edit(wxWindow* parent) = 0;
    // Returns a new item using this one as a template.
    // This is used when the Add button is clicked with this item selected.
    // Name attributes should generally be adjusted (e.g. "Name" -> "Copy of Name").
    // If NULL is returned, a new object will be created as if there were no item selected.
    virtual ListEditItem* CreateCopy() const {return NULL;}
protected:
    ListEditItem() { }
private:
    DECLARE_ABSTRACT_CLASS(ListEditItem)
};

class ListEditDialog : public wxDialog {
public:
    // Constructor.
    ListEditDialog(wxWindow* parent, wxWindowID id, const wxString& title, wxClassInfo* newitemtype, unsigned int newmaxlen = (unsigned int) -1);
    // Destructor.
    virtual ~ListEditDialog();
    // Gets the number of items.
    unsigned int GetItemCount() const;
    // Gets the specified item.
    ListEditItem* GetItem(unsigned int i) const;
    // Adds an item, which will then be deleted by this dialog object.
    void AddItem(ListEditItem* newitem);
    void AddItem(ListEditItem* newitem, unsigned int pos);
    // Deletes an item.
    void DeleteItem(unsigned int i);
    // Updates the summary text of an item.
    void UpdateItemSummary(unsigned int i);
    // Removes an item without deleting it, and returns it.
    ListEditItem* RemoveItem(unsigned int i);
    // Creates and returns a new item.
    ListEditItem* GetNewItem(bool copyfromsel = true) const;
private:
    void OnCommand(wxCommandEvent& event);
    void OnUpdateUI(wxUpdateUIEvent& event);
    wxClassInfo* itemtype;
    wxListBox* listbox;
    unsigned int maxlen;
    DECLARE_EVENT_TABLE()
};

}

#endif // !CHIPW_LISTEDITDLG_H_INCLUDED

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

#ifndef CHIPW_LISTMENUMGR_H_INCLUDED
#define CHIPW_LISTMENUMGR_H_INCLUDED

#include <set>
#include <vector>
#include <wx/string.h>

class wxMenu;

namespace ChipW {

class ListMenuManager {
public:
    ListMenuManager(int newidmin, int newidmax, wxString newemptytext) : idmin(newidmin), idmax(newidmax), emptytext(newemptytext) { }
    virtual ~ListMenuManager() { }
    // Adds the current list to the given menu at its current end.
    // The menu will then be automatically updated whenever the list changes.
    // The menu *must* be detached before it is deleted.
    void AddMenu(wxMenu* menu);
    // Stops automatically updating the given menu.
    // This must be called before the menu is deleted.
    void DetachMenu(wxMenu* menu) {menus.erase(menu);}
    // Returns the current list.
    const std::vector<wxString>& GetMenuItems() const {return items;}
    // Changes the list, updating all attached menus.
    void SetMenuItems(std::vector<wxString> newitems);
    // Returns the lowest window ID available for use by the list.
    int GetIdMin() const {return idmin;}
    // Returns the highest window ID available for use by the list.
    int GetIdMax() const {return idmax;}
    // Returns the maximum allowed length of the list.
    unsigned int GetMaxLength() const {return idmin > idmax ? 0 : idmax - idmin + 1;}
    // Checks whether the given ID is available for use by the list.
    bool HasId(int id) const {return id >= idmin && id <= idmax;}
    // Checks whether the given ID is currently in use by the list.
    bool IsIdActive(int id) const {return id < idmin + (int) items.size();}
    // Returns the string used for a placeholder item when the list is empty.
    const wxString& GetEmptyText() const {return emptytext;}
private:
    int idmin, idmax;
    wxString emptytext;
    std::set<wxMenu*> menus;
    std::vector<wxString> items;
};

}

#endif // !CHIPW_LISTMENUMGR_H_INCLUDED

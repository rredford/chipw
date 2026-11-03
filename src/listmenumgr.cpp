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

#include "listmenumgr.h"
#include <wx/menu.h>
#include <wx/string.h>

namespace ChipW {

namespace {

wxString GetItemLabel(int i, const std::vector<wxString>& items, const wxString& emptytext) {
    if(i < 0 || (unsigned int) i >= items.size())
        return emptytext;
    wxString label;
    if(i < 9)
        label << wxT("&");
    label << (i + 1) << wxT(". ") << items[i];
    return label;
}

}

void ListMenuManager::AddMenu(wxMenu* menu) {
    if(menu == NULL || menus.count(menu))
        return;
    unsigned int len = GetMaxLength();
    if(len > items.size())
        len = items.size();
    if(len < 1)
        len = 1;
    for(unsigned int i = 0; i < len; ++i) {
        wxMenuItem* item = menu->Append(idmin + i, GetItemLabel(i, items, emptytext));
        if(item != NULL)
            item->Enable((unsigned int) i < items.size());
    }
    menus.insert(menu);
}

void ListMenuManager::SetMenuItems(std::vector<wxString> newitems) {
    unsigned int len = GetMaxLength();
    if(len > items.size())
        len = items.size();
    if(len < 1)
        len = 1;
    unsigned int newlen = GetMaxLength();
    if(newlen > newitems.size())
        newlen = newitems.size();
    if(newlen < 1)
        newlen = 1;
    for(std::set<wxMenu*>::const_iterator menuit = menus.begin(); menuit != menus.end(); ++menuit) {
        wxMenu* menu = *menuit;
        if(menu == NULL)
            continue;
        // Find the start of the list in the menu.
        const wxMenuItemList& itemlist = menu->GetMenuItems();
        unsigned int indexmin;
        wxMenuItemList::const_iterator itemit;
        for(indexmin = 0, itemit = itemlist.begin(); itemit != itemlist.end(); ++indexmin, ++itemit) {
            wxMenuItem* item = *itemit;
            if(item != NULL && item->GetId() == idmin)
                break;
        }
        // Update items at positions that are already present and should still be present.
        unsigned int i;
        for(i = 0; i < std::min(len, newlen) && itemit != itemlist.end(); ++i, ++itemit) {
            wxMenuItem* item = *itemit;
            if(item == NULL)
                continue;
#if wxCHECK_VERSION(2, 9, 0)
            item->SetItemLabel(GetItemLabel(i, newitems, emptytext));
#else
            item->SetText(GetItemLabel(i, newitems, emptytext));
#endif
            item->Enable((unsigned int) i < newitems.size());
        }
        // Add items if the length has increased.
        for(; i < newlen; ++i) {
            wxMenuItem* item = menu->Insert(indexmin + i, idmin + i, GetItemLabel(i, newitems, emptytext));
            if(item != NULL)
                item->Enable((unsigned int) i < newitems.size());
        }
        // Delete items if the length has decreased.
        for(; i < len; ++i) {
            menu->Destroy(idmin + i);
        }
    }
    items.swap(newitems);
}

}


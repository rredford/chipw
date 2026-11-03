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

#include "leveleditframe.h"
#include "leveleditids.h"
#include "app.h"
#include "playtestmgr.h"
#include <wx/menu.h>
#include <wx/docview.h>

namespace ChipW {

IMPLEMENT_CLASS(LevelEditFrame, SDIDocFrame)
BEGIN_EVENT_TABLE(LevelEditFrame, SDIDocFrame)
    EVT_MENU(-1, LevelEditFrame::OnCommand)
    EVT_BUTTON(-1, LevelEditFrame::OnCommand)
    EVT_UPDATE_UI(-1, LevelEditFrame::OnUpdateUI)
END_EVENT_TABLE()

std::map<wxDocManager*, std::vector<LevelEditFrame*> > LevelEditFrame::spare;

LevelEditFrame* LevelEditFrame::GetSpareOrNew(wxView* view) {
    LevelEditFrame* result = NULL;
    wxDocManager* manager = NULL;
    if(view != NULL)
        manager = view->GetDocumentManager();
    if(manager != NULL) {
        std::map<wxDocManager*, std::vector<LevelEditFrame*> >::iterator it = spare.find(manager);
        if(it != spare.end()) {
            if(!it->second.empty()) {
                result = it->second.back();
                it->second.pop_back();
            }
            if(it->second.empty())
                spare.erase(it);
        }
    }
    if(result == NULL)
        return new LevelEditFrame(view);
    result->SetView(view);
    return result;
}

LevelEditFrame::~LevelEditFrame() {
    ListMenuManager* playtestmgr = wxGetApp().GetPlayTestManager();
    if(playtestmgr != NULL)
        playtestmgr->DetachMenu(playtestmenu);
    if(GetDocumentManager() != NULL) {
        std::map<wxDocManager*, std::vector<LevelEditFrame*> >::iterator it = spare.find(GetDocumentManager());
        if(it != spare.end()) {
            for(std::vector<LevelEditFrame*>::iterator it2 = it->second.begin(); it2 != it->second.end(); ++it2) {
                if(*it2 == this) {
                    it->second.erase(it2);
                    break;
                }
            }
            if(it->second.empty())
                spare.erase(it);
        }
    }
}

bool LevelEditFrame::Create(wxDocManager* manager, wxFrame* parent, bool isspare) {
    if(!SDIDocFrame::Create(manager, parent, -1, wxT("Chip's Workshop"), wxDefaultPosition, wxSize(800, 600), wxDEFAULT_FRAME_STYLE | wxMAXIMIZE))
        return false;
    if(!CreateMenuBar())
        return false;
    CreateStatusBar();
    if(isspare && manager != NULL)
        spare[manager].push_back(this);
    return true;
}

bool LevelEditFrame::Create(wxView* newview, wxFrame* parent) {
    if(!SDIDocFrame::Create(newview, parent, -1, wxT("Chip's Workshop"), wxDefaultPosition, wxSize(800, 600), wxDEFAULT_FRAME_STYLE | wxMAXIMIZE))
        return false;
    if(!CreateMenuBar())
        return false;
    CreateStatusBar();
    return true;
}

bool LevelEditFrame::CreateMenuBar() {
    wxMenuBar* menubar = new wxMenuBar;
    // File menu.
    wxMenu* menu = new wxMenu;
    menu->Append(wxID_NEW, wxT("&New...\tCtrl+N"), wxT("Create a new document."));
    menu->Append(wxID_OPEN, wxT("&Open...\tCtrl+O"), wxT("Open a file."));
    menu->AppendSeparator();
    menu->Append(wxID_SAVE, wxT("&Save...\tCtrl+S"), wxT("Save the current document."));
    menu->Append(wxID_SAVEAS, wxT("Save &As..."), wxT("Save the current document to a new file."));
    menu->Append(wxID_CLOSE, wxT("&Close...\tCtrl+W"), wxT("Close the current document."));
    menu->AppendSeparator();
    menu->Append(wxID_EXIT, wxT("E&xit...\tCtrl+Q"), wxT("Exit the program, closing all open documents."));
    menubar->Append(menu, wxT("&File"));
    // Edit menu.
    menu = new wxMenu;
    menu->Append(wxID_UNDO, wxT("&Undo\tCtrl+Z"), wxT("Undo the last action."));
    menu->Append(wxID_REDO, wxT("&Redo\tCtrl+Y"), wxT("Redo the last undone action."));
    menu->AppendSeparator();
    menu->Append(wxID_CUT, wxT("Cu&t\tCtrl+X"), wxT("Move the selection to the clipboard."));
    menu->Append(wxID_COPY, wxT("&Copy\tCtrl+C"), wxT("Copy the selection to the clipboard."));
    menu->Append(wxID_PASTE, wxT("&Paste\tCtrl+V"), wxT("Insert the contents of the clipboard."));
#if 0
    menu->AppendSeparator();
    menu->AppendCheckItem(ID_SWAPBUTTONS, wxT("&Swap secondary mouse buttons\tCtrl+1"),
        wxT("Switch the functions of the right and middle mouse buttons."));
#endif
    menu->AppendSeparator();
    menu->Append(wxID_PROPERTIES, wxT("&Properties...\tCtrl+P"), wxT("View or adjust the level properties."));
    menu->Append(ID_EDIT_MONSTERS, wxT("&Monsters...\tCtrl+M"), wxT("View or adjust the monster list."));
//    menu->Append(ID_EDIT_WIRES, wxT("&Wires...\tCtrl+W"), wxT("View or adjust the wire list."));
    menu->AppendSeparator();
    menu->Append(ID_CLEARSELTILES, wxT("Clear selecte&d tiles\tDel"), wxT("Remove all tiles in the selected area."));
    menu->AppendSeparator();
    menu->Append(ID_ROTATE_TILE_ANTICLOCKWISE, wxT("Rotate tile antic&lockwise\tCtrl+Left"), wxT("Rotate the currently chosen tile anticlockwise."));
    menu->Append(ID_ROTATE_TILE_CLOCKWISE, wxT("Rotate tile cl&ockwise\tCtrl+Right"), wxT("Rotate the currently chosen tile clockwise."));
    menu->AppendSeparator();
    menu->AppendCheckItem(ID_READONLY, wxT("Re&ad only\tCtrl+R"), wxT("Toggle the ability to edit the level."));
    menubar->Append(menu, wxT("&Edit"));
    // Playtest menu.
    playtestmenu = new wxMenu;
    playtestmenu->Append(ID_PLAYTEST_CONFIG, wxT("&Configure..."), wxT("Choose programs for playtesting."));
    playtestmenu->AppendSeparator();
    ListMenuManager* playtestmgr = wxGetApp().GetPlayTestManager();
    if(playtestmgr != NULL)
        playtestmgr->AddMenu(playtestmenu);
    menubar->Append(playtestmenu, wxT("&Playtest"));
    // View menu.
    menu = new wxMenu;
    menu->AppendCheckItem(ID_SHOWWIRES, wxT("Show &wires\tCtrl+2"), wxT("Toggle the display of wires."));
    menu->AppendCheckItem(ID_SHOWUPPER, wxT("Show &upper layer\tCtrl+3"), wxT("Toggle the display of the upper layer of tiles."));
    menu->AppendSeparator();
    menu->Append(ID_SETTILESET, wxT("Select &tileset...\tCtrl+T"), wxT("Select the tileset for displaying the level map."));
    menu->Append(ID_SETTILESIZE, wxT("&Scale tiles...\tCtrl+Shift+T"), wxT("Change the size of display of the tiles."));
    menu->AppendSeparator();
    menu->Append(ID_NEWVIEW, wxT("&New window\tCtrl+Shift+N"), wxT("Open a new window for the current document."));
    menu->Append(ID_CLOSEVIEW, wxT("&Close window...\tCtrl+Shift+W"), wxT("Close the current window."));
    menubar->Append(menu, wxT("&View"));
    // Help menu.
    menu = new wxMenu;
    menu->Append(wxID_HELP_CONTENTS, wxT("&User's guide\tF1"), wxT("Open the program help file."));
    menu->Append(wxID_ABOUT, wxT("&About..."), wxT("Display the program version number and copyright information."));
    menubar->Append(menu, wxT("&Help"));
    // Set the menu bar.
    SetMenuBar(menubar);
    return true;
}

void LevelEditFrame::OnCommand(wxCommandEvent& event) {
    switch(event.GetId()) {
    case ID_CLOSEVIEW:
        Close();
        break;
    default:
        event.Skip();
    }
}

void LevelEditFrame::OnUpdateUI(wxUpdateUIEvent& event) {
    switch(event.GetId()) {
    case ID_SWAPBUTTONS:
    case wxID_CUT:
    case wxID_COPY:
    case wxID_PASTE:
    case wxID_PROPERTIES:
    case ID_EDIT_MONSTERS:
    case ID_EDIT_WIRES:
    case ID_CLEARSELTILES:
    case ID_ROTATE_TILE_ANTICLOCKWISE:
    case ID_ROTATE_TILE_CLOCKWISE:
    case ID_READONLY:
    case ID_SHOWWIRES:
    case ID_SHOWUPPER:
    case ID_SETTILESET:
    case ID_SETTILESIZE:
    case ID_NEWVIEW:
        event.Enable(GetView() != NULL);
        break;
    default:
        {
            ListMenuManager* playtestmgr = wxGetApp().GetPlayTestManager();
            if(playtestmgr != NULL && playtestmgr->HasId(event.GetId()))
                event.Enable(playtestmgr->IsIdActive(event.GetId()) && GetView() != NULL);
            else
                event.Skip();
        }
    }
}

}


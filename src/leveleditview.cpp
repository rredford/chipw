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

#include "leveleditview.h"
#include "levelsetdoc.h"
#include "leveleditframe.h"
#include "level.h"
#include "tileset.h"
#include "levelupdatehint.h"
#include "levelpropertiesdlg.h"
#include "monsterlistdlg.h"
#include "bufferscroll.h"
#include "leveleditids.h"
#include "apppaths.h"
#include "leveldataobj.h"
#include "app.h"
#include "playtestmgr.h"
#include <wx/statusbr.h>
#include <wx/splitter.h>
#include <wx/panel.h>
#include <wx/notebook.h>
#include <wx/listbox.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/dc.h>
#include <wx/bitmap.h>
#include <wx/dcmemory.h>
#include <wx/dcclient.h>
#include <wx/pen.h>
#include <wx/brush.h>
#include <wx/colour.h>
#include <wx/app.h>
#include <wx/log.h>
#include <wx/confbase.h>
#include <wx/filedlg.h>
#include <wx/textdlg.h>
#include <wx/filename.h>
#include <wx/msgdlg.h>
#include <wx/region.h>
#include <wx/clipbrd.h>

#include <iostream>

namespace ChipW {

class LevelMapWindow : public BufferedScrolledWindow {
public:
    LevelMapWindow(wxWindow* parent, LevelEditView* newview) : BufferedScrolledWindow(parent), view(newview) { }
    virtual void OnDraw(wxDC& dc) {if(view != NULL) view->OnDraw(&dc);}
    void OnMouse(wxMouseEvent& event) {if(view != NULL) view->OnMouse(event);}
    LevelEditView* view;
private:
    DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(LevelMapWindow, BufferedScrolledWindow)
    EVT_MOUSE_EVENTS(LevelMapWindow::OnMouse)
END_EVENT_TABLE()

class TileSelectWindow : public BufferedScrolledWindow {
public:
    TileSelectWindow(wxWindow* parent, LevelEditView* newview) : BufferedScrolledWindow(parent), view(newview) { }
    virtual void OnDraw(wxDC& dc) {if(view != NULL) view->DrawTileSelector(&dc);}
    void OnMouse(wxMouseEvent& event) {if(view != NULL) view->OnMouse(event);}
    LevelEditView* view;
private:
    DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(TileSelectWindow, BufferedScrolledWindow)
    EVT_MOUSE_EVENTS(TileSelectWindow::OnMouse)
END_EVENT_TABLE()

const int TILESEL_WIDTH = 6;
const int TILESEL_HEIGHT = 20;
const LevelEditView::Tool TOOL_NONE = LevelEditView::TOOL_NONE;
const LevelEditView::Tool TOOL_RECTSELECT = LevelEditView::TOOL_RECTSELECT;
const LevelEditView::Tool TOOL_TRAPWIRE = LevelEditView::TOOL_TRAPWIRE;
const LevelEditView::Tool TOOL_CLONEWIRE = LevelEditView::TOOL_CLONEWIRE;
const LevelEditView::Tool tileselgrid[TILESEL_WIDTH * TILESEL_HEIGHT] = {
//  TOOL_RECTSELECT,    TOOL_NONE,              TOOL_NONE,              TOOL_NONE,              TOOL_NONE,              TOOL_NONE,  TOOL_NONE
    TILE_FLOOR,         TILE_WALL_BLUE_FAKE,    TILE_WALL_INV,          TILE_BUTTON_TOGGLE,     TILE_WALL_TOGGLE_OFF, /*TOOL_NONE,*/TOOL_RECTSELECT,
    TILE_WALL,          TILE_WALL_BLUE,         TILE_WALL_INV_SHOWS,    TILE_WALL_POPUP,        TILE_WALL_TOGGLE_ON,  /*TOOL_NONE,*/TOOL_NONE,
    TILE_WALL_SE,       TILE_WALL_W,            TILE_WALL_N,            TILE_WALL_E,            TILE_WALL_S,          /*TOOL_NONE,*/TOOL_NONE,
    TILE_EXIT,          TILE_CHIP_W,            TILE_CHIP_N,            TILE_CHIP_E,            TILE_CHIP_S,          /*TOOL_NONE,*/TOOL_NONE,
    TILE_SOCKET,        TILE_DOOR_B,            TILE_DOOR_R,            TILE_DOOR_Y,            TILE_DOOR_G,          /*TOOL_NONE,*/TOOL_NONE,
    TILE_COMPUTERCHIP,  TILE_KEY_B,             TILE_KEY_R,             TILE_KEY_Y,             TILE_KEY_G,           /*TOOL_NONE,*/TOOL_NONE,
    TILE_BOMB,          TILE_WATER,             TILE_BOOTS_WATER,       TILE_FIRE,              TILE_BOOTS_FIRE,      /*TOOL_NONE,*/TOOL_NONE,
    TILE_THIEF,         TILE_FORCE_RAND,        TILE_BOOTS_FORCE,       TILE_ICE,               TILE_BOOTS_ICE,       /*TOOL_NONE,*/TOOL_NONE,
    TILE_GRAVEL,        TILE_FORCE_E,           TILE_FORCE_S,           TILE_ICE_CURVE_SE,      TILE_ICE_CURVE_SW,    /*TOOL_NONE,*/TOOL_NONE,
    TILE_DIRT,          TILE_FORCE_N,           TILE_FORCE_W,           TILE_ICE_CURVE_NE,      TILE_ICE_CURVE_NW,    /*TOOL_NONE,*/TOOL_NONE,
    TILE_BLOCK,         TILE_BLOCK_W,           TILE_BLOCK_N,           TILE_BLOCK_E,           TILE_BLOCK_S,         /*TOOL_NONE,*/TOOL_NONE,
    TILE_HINT,          TILE_BUG_W,             TILE_BUG_N,             TILE_BUG_E,             TILE_BUG_S,           /*TOOL_NONE,*/TOOL_NONE,
    TILE_TELEPORT,      TILE_PARAMECIUM_W,      TILE_PARAMECIUM_N,      TILE_PARAMECIUM_E,      TILE_PARAMECIUM_S,    /*TOOL_NONE,*/TOOL_NONE,
    TILE_TRAP,          TILE_GLIDER_W,          TILE_GLIDER_N,          TILE_GLIDER_E,          TILE_GLIDER_S,        /*TOOL_NONE,*/TOOL_NONE,
    TOOL_TRAPWIRE,      TILE_BALL_W,            TILE_BALL_N,            TILE_BALL_E,            TILE_BALL_S,          /*TOOL_NONE,*/TOOL_NONE,
    TILE_BUTTON_TRAP,   TILE_FIREBALL_W,        TILE_FIREBALL_N,        TILE_FIREBALL_E,        TILE_FIREBALL_S,      /*TOOL_NONE,*/TOOL_NONE,
    TILE_BUTTON_TANK,   TILE_TANK_W,            TILE_TANK_N,            TILE_TANK_E,            TILE_TANK_S,          /*TOOL_NONE,*/TOOL_NONE,
    TILE_BUTTON_CLONE,  TILE_WALKER_W,          TILE_WALKER_N,          TILE_WALKER_E,          TILE_WALKER_S,        /*TOOL_NONE,*/TOOL_NONE,
    TOOL_CLONEWIRE,     TILE_BLOB_W,            TILE_BLOB_N,            TILE_BLOB_E,            TILE_BLOB_S,          /*TOOL_NONE,*/TOOL_NONE,
    TILE_CLONER,        TILE_TEETH_W,           TILE_TEETH_N,           TILE_TEETH_E,           TILE_TEETH_S,         /*TOOL_NONE,*/TOOL_NONE,
};

IMPLEMENT_DYNAMIC_CLASS(LevelEditView, wxView)
BEGIN_EVENT_TABLE(LevelEditView, wxView)
    EVT_MENU(-1, LevelEditView::OnCommand)
    EVT_BUTTON(-1, LevelEditView::OnCommand)
    EVT_UPDATE_UI(-1, LevelEditView::OnUpdateUI)
    EVT_LISTBOX(-1, LevelEditView::OnListBox)
    EVT_LISTBOX_DCLICK(-1, LevelEditView::OnListBox)
    EVT_IDLE(LevelEditView::OnIdle)
END_EVENT_TABLE()

Tileset LevelEditView::defaulttileset;
bool LevelEditView::loadeddefaulttileset = false;

LevelEditView::LevelEditView()
 :  level(NULL), selx(32), sely(32), selw(0), selh(0), clickx(32), clicky(32), readonly(false), showwires(true), showupper(true),
    activebutton(wxMOUSE_BTN_NONE), buttonswap(false), idleredraw(DRAW_NONE),
    mapwin(NULL), statusbar(NULL), mapbmp(NULL), levellist(NULL), tilelist(NULL), tilesel(NULL)
{
}

LevelEditView::~LevelEditView() {
    delete mapbmp;
    if(mapwin != NULL)
        mapwin->view = NULL;
    if(tilesel != NULL)
        tilesel->view = NULL;
}

bool LevelEditView::OnCreate(wxDocument* doc, long flags) {
    if(!wxView::OnCreate(doc, flags))
        return false;
    // Frame.
    wxWindow* frame = GetFrame();
    if(frame == NULL) {
        frame = LevelEditFrame::GetSpareOrNew(this);
        SetFrame(frame);
    }
    // Status bar.
    if(frame->IsKindOf(CLASSINFO(wxFrame)))
        statusbar = ((wxFrame*) frame)->GetStatusBar();
    // Main splitter window.
    wxSplitterWindow* splitter = new wxSplitterWindow(frame, -1, wxDefaultPosition, wxDefaultSize, wxSP_3D | wxSP_LIVE_UPDATE);
    // Tabs on left.
    wxPanel* notebookpanel = new wxPanel(splitter);
    wxSizer* notebookpanelsizer = new wxBoxSizer(wxVERTICAL);
    wxNotebook* notebook = new wxNotebook(notebookpanel, -1, wxDefaultPosition, wxDefaultSize, wxNB_BOTTOM);
    // Level list.
    if(doc != NULL && doc->IsKindOf(CLASSINFO(LevelSetDoc))) {
        wxPanel* levellistpanel = new wxPanel(notebook);
        wxSizer* levellistpanelsizer = new wxBoxSizer(wxVERTICAL);
        wxSizer* buttonsizer = new wxBoxSizer(wxHORIZONTAL);
        buttonsizer->Add(new wxButton(levellistpanel, wxID_ADD, wxT("+"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT), 1, wxGROW);
        buttonsizer->Add(new wxButton(levellistpanel, wxID_REMOVE, wxT("-"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT), 1, wxGROW);
        buttonsizer->Add(new wxButton(levellistpanel, wxID_UP, wxT("^"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT), 1, wxGROW);
        buttonsizer->Add(new wxButton(levellistpanel, wxID_DOWN, wxT("v"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT), 1, wxGROW);
        buttonsizer->Add(new wxButton(levellistpanel, wxID_PROPERTIES, wxT("..."), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT), 1, wxGROW);
        levellistpanelsizer->Add(buttonsizer, 0, wxALL | wxGROW, 5);
        levellist = new wxListBox(levellistpanel, -1);
        levellistpanelsizer->Add(levellist, 1, wxGROW);
        levellistpanel->SetSizerAndFit(levellistpanelsizer);
        notebook->AddPage(levellistpanel, wxT("Levels"));
    }
    // Tile selector.
    tilesel = new TileSelectWindow(notebook, this);
    tilesel->SetBackgroundColour(wxColour(102, 102, 102));
    notebook->AddPage(tilesel, wxT("Tiles"));
    // Tile list.
    wxPanel* tilelistpanel = new wxPanel(notebook);
    wxSizer* tilelistpanelsizer = new wxBoxSizer(wxVERTICAL);
    tilelist = new wxListBox(tilelistpanel, -1);
    for(Tool tool = TOOL_MIN; tool <= TOOL_MAX; ++tool)
        tilelist->Append(GetToolName(tool, true));
    tilelist->SetSelection(0);
    tilelistpanelsizer->Add(tilelist, 1, wxGROW);
    tilelistpanel->SetSizerAndFit(tilelistpanelsizer);
    notebook->AddPage(tilelistpanel, wxT("IDs"));
    // Set notebook panel sizer.
    notebookpanelsizer->Add(notebook, 1, wxGROW);
    notebookpanel->SetSizerAndFit(notebookpanelsizer);
    // Level map on right.
    mapwin = new LevelMapWindow(splitter, this);
    mapwin->SetBackgroundColour(wxColour(102, 102, 102));
    // Set up splitter.
    splitter->SetMinimumPaneSize(20);
    splitter->SplitVertically(notebookpanel, mapwin, 150);
    // Display the frame.
    splitter->SetSize(frame->GetClientSize());
    frame->Show();
    frame->Raise();
    return true;
}

void LevelEditView::OnChangeFilename() {
    if(GetDocument() != NULL && GetFrame() != NULL) {
#if wxCHECK_VERSION(2, 9, 0)
        wxString name = GetDocument()->GetUserReadableName();
#else
        wxString name;
        GetDocument()->GetPrintableName(name);
#endif
        wxString title = wxT("Chip's Workshop");
        if(!name.empty())
            title += wxT(" - ") + name;
        if(level != NULL && !level->title.empty())
            title += wxT(" (") + wxString(level->title.c_str(), wxConvISO8859_1) + wxT(")");
        if(GetFrame()->IsKindOf(CLASSINFO(wxTopLevelWindow)))
            ((wxTopLevelWindow*) GetFrame())->SetTitle(title);
        else
            GetFrame()->SetLabel(title);
    }
}

bool LevelEditView::OnClose(bool delwin) {
    if(GetDocument() != NULL && GetDocument()->GetViews().size() < 2 && !GetDocument()->Close())
        return false;
    Activate(false);
    if(mapwin != NULL)
        mapwin->view = NULL;
    mapwin = NULL;
    if(tilesel != NULL)
        tilesel->view = NULL;
    tilesel = NULL;
    levellist = NULL;
    tilelist = NULL;
    statusbar = NULL;
    delete mapbmp;
    mapbmp = NULL;
    if(delwin && GetFrame() != NULL) {
        wxWindow* frame = GetFrame();
        SetFrame(NULL);
        frame->Destroy();
    }
    return true;
}

void LevelEditView::OnActivateView(bool activate, wxView *activeView, wxView *deactiveView) {
    if(activate && wxTheApp != NULL && GetFrame() != NULL)
        wxTheApp->SetTopWindow(GetFrame());
}

void LevelEditView::OnCommand(wxCommandEvent& event) {
    LevelSetDoc* doc = GetLevelSetDoc();
    LevelSet* levelset = NULL;
    if(doc != NULL)
        levelset = doc->GetLevelSet();
    LevelUpdateHint hint;
    switch(event.GetId()) {
    case wxID_ADD:
        if(!readonly && doc != NULL && levelset != NULL) {
            CountedPtr<Level> oldlevel = level;
            CountedPtr<Level> newlevel = new Level;
            SetLevel(newlevel);
            if(!doc->InsertLevel(newlevel, levelset->FindLevel(level) + 1))
                SetLevel(oldlevel);
        }
#if 0
        if(!readonly && levelset != NULL) {
            CountedPtr<Level> newlevel = new Level;
            if(levelset->InsertLevel(newlevel)) {
                SetLevel(newlevel);
                hint.type = LevelUpdateHint::CREATE;
                hint.level = newlevel;
                SendUpdate(&hint);
            }
        }
#endif
        break;
    case wxID_REMOVE:
        if(!readonly && doc != NULL && level != NULL) {
            if(wxMessageBox(wxT("Are you sure you want to delete this level?"), wxT("Delete level"), wxYES_NO | wxICON_QUESTION) != wxYES)
                break;
            doc->RemoveLevel(level);
        }
#if 0
        if(!readonly && levelset != NULL && level != NULL) {
            wxUint16 index = levelset->FindLevel(level);
            if(index < levelset->GetLevelCount()) {
                if(wxMessageBox(wxT("Are you sure you want to delete this level?"), wxT("Delete level"), wxYES_NO | wxICON_QUESTION) != wxYES)
                    break;
                levelset->RemoveLevel(index, true);
                hint.type = LevelUpdateHint::DESTROY;
                hint.level = level;
                SendUpdate(&hint);
            }
        }
#endif
        break;
    case wxID_UP:
        if(!readonly && levelset != NULL && level != NULL && doc != NULL) {
            wxUint16 index = levelset->FindLevel(level);
            if(index > 0 && index < levelset->GetLevelCount()) {
                doc->MoveLevel(level, index - 1);
#if 0
                levelset->InsertLevel(level, index - 1);
                hint.type = LevelUpdateHint::LIST;
                hint.level = level;
                SendUpdate(&hint);
#endif
            }
        }
        break;
    case wxID_DOWN:
        if(!readonly && levelset != NULL && level != NULL && doc != NULL) {
            wxUint16 index = levelset->FindLevel(level);
            if(levelset->GetLevelCount() > 0 && index < levelset->GetLevelCount() - 1) {
                doc->MoveLevel(level, index + 1);
#if 0
                levelset->InsertLevel(level, index + 1);
                hint.type = LevelUpdateHint::LIST;
                hint.level = level;
                SendUpdate(&hint);
#endif
            }
        }
        break;
    case wxID_PROPERTIES:
        if(level != NULL && doc != NULL && levelset != NULL) {
            LevelPropertiesDialog dlg(GetFrame(), readonly);
            dlg.title << wxString(level->title.c_str(), wxConvISO8859_1);
            dlg.psw << wxString(level->psw.c_str(), wxConvISO8859_1);
            dlg.chips << level->chips;
            dlg.countedchips << level->CountChips();
            dlg.time << level->time;
            dlg.hint << wxString(level->hint.c_str(), wxConvISO8859_1);
            dlg.levnum << level->levelnumber;
//            dlg.ruleset << levelset->ruleset;
            dlg.ruleset = levelset->ruleset;
            if(dlg.ShowModal() == wxID_OK && !readonly) {
                unsigned long ul;
                std::string title = level->title;
                {
                    const wxWX2MBbuf str = dlg.title.mb_str(wxConvISO8859_1);
                    if((const char*) str != NULL)
                        title = str;
                    else
                        wxLogError(wxT("Failed to set level title."));
                }
                std::string psw = level->psw;
                {
                    const wxWX2MBbuf str = dlg.psw.mb_str(wxConvISO8859_1);
                    if((const char*) str != NULL)
                        psw = str;
                    else
                        wxLogError(wxT("Failed to set level password."));
                }
                wxUint16 chips = level->chips;
                if(dlg.chips.ToULong(&ul) && ul < 65536)
                    chips = ul;
                else
                    wxLogError(wxT("Failed to set chips required."));
                wxUint16 time = level->time;
                if(dlg.time.ToULong(&ul) && ul < 65536)
                    time = ul;
                else
                    wxLogError(wxT("Failed to set time limit."));
                std::string hint = level->hint;
                {
                    const wxWX2MBbuf str = dlg.hint.mb_str(wxConvISO8859_1);
                    if((const char *) str != NULL)
                        hint = str;
                    else
                        wxLogError(wxT("Failed to set level hint."));
                }
                wxUint16 levelnumber = level->levelnumber;
                if(dlg.levnum.ToULong(&ul) && ul < 65536)
                    levelnumber = ul;
                else
                    wxLogError(wxT("Failed to set level number."));
#if 0
                wxUint32 ruleset = levelset->ruleset;
                if(dlg.ruleset.ToULong(&ul))
                    ruleset = ul;
                else
                    wxLogError(wxT("Failed to set rule set."));
#endif
                wxUint32 ruleset = dlg.ruleset;
                doc->SetLevelProperties(level, title, psw, chips, time, hint, levelnumber, ruleset);
#if 0
                hint.type = LevelUpdateHint::LIST;
                hint.level = level;
                SendUpdate(&hint);
#endif
            }
        }
        break;
    case ID_EDIT_MONSTERS:
        if(level != NULL && doc != NULL) {
            MonsterListDialog dlg(GetFrame(), level, readonly);
            if(dlg.ShowModal() == wxID_OK && !readonly)
                doc->SetLevelMonsters(level, dlg.GetMonsterList());
#if 0
            {
                level->monsters = dlg.GetMonsterList();
                hint.type = LevelUpdateHint::MONSTER;
                hint.level = level;
                SendUpdate(&hint);
            }
#endif
        }
        break;
    case ID_EDIT_WIRES:
        break;
    case ID_CLEARSELTILES:
        if(!readonly && level != NULL) {
            if(doc != NULL)
                doc->FinishLevelChangeCommand();
            DoTileActionSel(TILE_FLOOR, wxT("placetile")/*, hint*/);
            if(doc != NULL)
                doc->FinishLevelChangeCommand();
#if 0
            if(hint.type != LevelUpdateHint::NONE)
                SendUpdate(&hint);
#endif
        }
        break;
    case ID_SHOWWIRES:
        showwires = !showwires;
        AddIdleRedraw(DRAW_MAP);
        break;
    case ID_SHOWUPPER:
        showupper = !showupper;
        hint.type = LevelUpdateHint::TILE;
        hint.level = level;
        OnUpdate(this, &hint);
        break;
    case wxID_CUT:
    case wxID_COPY:
        if((event.GetId() != wxID_CUT || !readonly) && level != NULL && wxTheClipboard != NULL) {
            if(HasSelection()) {
                LevelSectionDataObject* sectdataobj = new LevelSectionDataObject;
                if(!sectdataobj->SetSection(level, selx, sely, selw, selh)) {
                    wxLogError(wxT("Failed to copy map section."));
                    delete sectdataobj;
                    break;
                }
                if(!wxTheClipboard->Open()) {
                    wxLogError(wxT("Failed to open the clipboard."));
                    delete sectdataobj;
                    break;
                }
                wxTheClipboard->SetData(sectdataobj);
                wxTheClipboard->Close();
                if(event.GetId() == wxID_CUT) {
                    if(doc != NULL)
                        doc->FinishLevelChangeCommand();
                    DoTileActionSel(TILE_FLOOR, wxT("placetile"));
                    if(doc != NULL)
                        doc->FinishLevelChangeCommand();
                }
            } else {
                LevelsDataObject* levdataobj = new LevelsDataObject;
                if(!levdataobj->AddLevel(level)) {
                    wxLogError(wxT("Failed to copy level."));
                    delete levdataobj;
                    break;
                }
                if(!wxTheClipboard->Open()) {
                    wxLogError(wxT("Failed to open the clipboard."));
                    delete levdataobj;
                    break;
                }
                wxTheClipboard->SetData(levdataobj);
                wxTheClipboard->Close();
                if(event.GetId() == wxID_CUT && doc != NULL)
                    doc->RemoveLevel(level);
            }
        }
        break;
    case wxID_PASTE:
        if(wxTheClipboard != NULL) {
            LevelSectionDataObject* sectdataobj = new LevelSectionDataObject;
            LevelsDataObject* levdataobj = new LevelsDataObject;
            wxDataObjectComposite dataobj;
            dataobj.Add(sectdataobj, HasSelection());
            dataobj.Add(levdataobj, !HasSelection());
            if(!wxTheClipboard->Open()) {
                wxLogError(wxT("Failed to open the clipboard."));
                break;
            }
            bool success = wxTheClipboard->GetData(dataobj);
            wxTheClipboard->Close();
            if(!success) {
                wxBell();
                break;
            }
            if(sectdataobj->HasData()) {
                if(level == NULL || !HasSelection()) {
                    wxMessageBox(wxT("Please select the top-left corner of the destination area, then choose paste."), wxT("Paste map section"));
                    break;
                }
                if(doc != NULL)
                    doc->FinishLevelChangeCommand();
                if(!sectdataobj->PasteToLevel(level, selx, sely)) {
                    if(doc != NULL)
                        doc->CancelLevelChangeCommand();
                    wxBell();
                    break;
                }
                if(doc != NULL)
                    doc->FinishLevelChangeCommand();
                SetSelection(selx, sely, sectdataobj->GetWidth(), sectdataobj->GetHeight());
            } else if(levdataobj->HasData() && doc != NULL) {
                std::vector<CountedPtr<Level> > levels;
                if(!levdataobj->GetLevels(levels) || levels.empty()) {
                    wxBell();
                    break;
                }
                wxUint16 index = levelset->FindLevel(level) + 1;
                CountedPtr<Level> oldlevel = level;
                SetLevel(levels.front());
                if(!doc->InsertLevels(levels, index)) {
                    SetLevel(oldlevel);
                    wxBell();
                }
            } else {
                wxBell();
            }
            break;
        }
    case ID_SETTILESET:
    {
        wxConfigBase* config = wxConfigBase::Get();
        wxString filename = GetDataDir() + wxT("/tilesets/default");
        if(config != NULL)
            config->Read(wxT("Tileset"), &filename);
        wxFileName filenameobj(filename);
        filename = wxFileSelector(wxT("Choose a tileset"), filenameobj.GetPath(), filenameobj.GetFullName(), wxT(""), wxT("*"),
#if wxCHECK_VERSION(2, 8, 0)
            wxFD_OPEN | wxFD_FILE_MUST_EXIST,
#else
            wxOPEN | wxFILE_MUST_EXIST,
#endif
            GetFrame());
        if(filename.empty())
            break;
        Tileset newtileset(filename);
        if(!newtileset.IsOk()) {
            wxLogError(wxT("Failed to open tileset."));
            break;
        }
        if(SetTileset(newtileset)) {
            defaulttileset = tileset;
            if(config != NULL) {
                config->Write(wxT("Tileset"), filename);
                config->DeleteEntry(wxT("TileWidth"));
                config->DeleteEntry(wxT("TileHeight"));
            }
            hint.type = LevelUpdateHint::TILE;
            hint.level = level;
            OnUpdate(this, &hint);
        }
        break;
    }
    case ID_SETTILESIZE:
    {
        wxString sizestr;
        sizestr << GetTileW() << wxT("x") << GetTileH();
        wxString prompt;
        prompt << wxT("Enter the tile size in pixels.\n");
        prompt << wxT("The native tile size of the tileset is ") << tileset.GetNativeTileWidth();
        prompt << wxT("x") << tileset.GetNativeTileHeight() << wxT(".");
        sizestr = wxGetTextFromUser(prompt, wxT("Scale tiles"), sizestr, GetFrame());
        sizestr.Replace(wxT(" "), wxT(""));
        if(!sizestr.empty()) {
            long width, height;
            sizestr.MakeLower();
            size_t pos = sizestr.find(wxT("x"));
            if(pos < sizestr.size()) {
                if(pos == 0 || pos == sizestr.size() - 1 || !wxString(sizestr.substr(0, pos)).ToLong(&width) || width <= 0 ||
                !wxString(sizestr.substr(pos + 1)).ToLong(&height) || height <= 0) {
                    wxLogError(wxT("Invalid tile size."));
                    break;
                }
            } else {
                if(!sizestr.ToLong(&width) || width <= 0) {
                    wxLogError(wxT("Invalid tile size."));
                    break;
                }
                height = width;
            }
            if(SetTileSize(width, height)) {
                defaulttileset = tileset;
                wxConfigBase* config = wxConfigBase::Get();
                if(config != NULL) {
                    config->Write(wxT("TileWidth"), width);
                    config->Write(wxT("TileHeight"), height);
                }
                hint.type = LevelUpdateHint::TILE;
                hint.level = level;
                OnUpdate(this, &hint);
            }
        }
        break;
    }
    case ID_READONLY:
        readonly = !readonly;
        break;
    case ID_SWAPBUTTONS:
        buttonswap = !buttonswap;
        if(activebutton == wxMOUSE_BTN_RIGHT || activebutton == wxMOUSE_BTN_MIDDLE)
            activebutton = wxMOUSE_BTN_NONE;
        break;
    case ID_ROTATE_TILE_ANTICLOCKWISE:
    case ID_ROTATE_TILE_CLOCKWISE:
    {
        if(HasSelectedTile()) {
            Tile sel = GetSelectedTile();
            Tile rotated = ReorientateTile(sel, (event.GetId() == ID_ROTATE_TILE_CLOCKWISE) ? REORIENTATE_ROTATE_CLOCKWISE : REORIENTATE_ROTATE_ANTICLOCKWISE);
            if(rotated == TILE_INVALID && sel != TILE_INVALID)
                wxBell();
            else
                SetSelectedTile(rotated);
        }
        break;
    }
    case ID_NEWVIEW:
        NewView();
        break;
    case ID_CLOSEVIEW:
        Close();
        break;
    default:
        {
            PlayTestManager* playtestmgr = wxGetApp().GetPlayTestManager();
            if(playtestmgr != NULL && playtestmgr->HasId(event.GetId())) {
                if(levelset != NULL) {
                    const PlayTestType& playtesttype = playtestmgr->GetTypes()[event.GetId() - playtestmgr->GetIdMin()];
                    if(!playtesttype.Launch(*levelset, level == NULL ? 1 : level->levelnumber))
                        wxLogError(wxT("Playtesting failed."));
                    // If playtesting started successfully, the process object will delete itself when finished.
                }
            } else {
                event.Skip();
            }
        }
    }
}

void LevelEditView::OnUpdateUI(wxUpdateUIEvent& event) {
    LevelSet* levelset = GetLevelSet();
    switch(event.GetId()) {
    case wxID_ADD:
        event.Enable(!readonly && levelset != NULL);
        break;
    case wxID_REMOVE:
        event.Enable(!readonly && level != NULL && levelset != NULL);
        break;
    case wxID_UP:
        event.Enable(!readonly && level != NULL && levelset != NULL && levelset->FindLevel(level) > 0);
        break;
    case wxID_DOWN:
        event.Enable(!readonly && level != NULL && levelset != NULL && levelset->FindLevel(level) + 1 < levelset->GetLevelCount());
        break;
    case wxID_PROPERTIES:
    case ID_EDIT_MONSTERS:
    case ID_EDIT_WIRES:
        event.Enable(level != NULL);
        break;
    case ID_CLEARSELTILES:
        event.Enable(!readonly && HasSelection());
        break;
    case ID_ROTATE_TILE_ANTICLOCKWISE:
    case ID_ROTATE_TILE_CLOCKWISE:
        event.Enable(HasSelectedTile());
        break;
    case ID_SHOWWIRES:
        event.Enable(true);
        event.Check(showwires);
        break;
    case ID_SHOWUPPER:
        event.Enable(true);
        event.Check(showupper);
        break;
    case wxID_CUT:
        if(readonly) {
            event.Enable(false);
            break;
        }
        // Pass through to wxID_COPY
    case wxID_COPY:
        event.Enable(level != NULL && wxTheClipboard != NULL);
        break;
    case wxID_PASTE:
        event.Enable(wxTheClipboard != NULL && (
            wxTheClipboard->IsSupported(LevelSectionDataObject::GetDataFormat()) ||
            wxTheClipboard->IsSupported(LevelsDataObject::GetDataFormat())
        ));
        break;
    case ID_READONLY:
        event.Enable(true);
        event.Check(readonly);
        break;
    case ID_SWAPBUTTONS:
        event.Enable(true);
        event.Check(buttonswap);
        break;
    default:
        {
            PlayTestManager* playtestmgr = wxGetApp().GetPlayTestManager();
            if(playtestmgr != NULL && playtestmgr->HasId(event.GetId()))
                event.Enable(playtestmgr->IsIdActive(event.GetId()) && levelset != NULL);
            else
                event.Skip();
        }
    }
}

LevelEditView* LevelEditView::NewView(CountedPtr<Level> newlevel) {
    LevelEditView* newview = new LevelEditView;
    newview->SetDocument(GetDocument());
    if(!newview->OnCreate(GetDocument(), 0)) {
        delete newview;
        return NULL;
    }
    newview->SetLevel(newlevel);
    newview->SetTileset(tileset);
    newview->OnUpdate(this);
    return newview;
}

void LevelEditView::OnListBox(wxCommandEvent& event) {
    if(event.GetEventObject() == NULL)
        return;
    if(event.GetEventObject() == levellist) {
        if(event.GetClientData() != NULL) {
            CountedPtr<Level> newlevel = (Level*) event.GetClientData();
            if(event.GetEventType() == wxEVT_COMMAND_LISTBOX_DOUBLECLICKED) {
                NewView(newlevel);
            } else {
                SetLevel(newlevel);
                AddIdleRedraw(DRAW_MAP);
                OnChangeFilename();
            }
        }
    }
}

void LevelEditView::SetLevel(CountedPtr<Level> newlevel) {
    if(newlevel != level) {
        level = newlevel;
        delete mapbmp;
        mapbmp = NULL;
        ClearSelection();
    }
}

LevelSetDoc* LevelEditView::GetLevelSetDoc() const {
    if(GetDocument() == NULL || !GetDocument()->IsKindOf(CLASSINFO(LevelSetDoc)))
        return NULL;
    return (LevelSetDoc*) GetDocument();
}

LevelSet* LevelEditView::GetLevelSet() const {
    if(GetLevelSetDoc() == NULL)
        return NULL;
    return GetLevelSetDoc()->GetLevelSet();
}

bool LevelEditView::SetTileset(const Tileset& tiles) {
    tileset = tiles;
    delete mapbmp;
    mapbmp = NULL;
    if(mapwin != NULL) {
        mapwin->SetScrollbars(GetTileW(), GetTileH(), 32, 32);
        mapwin->Refresh();
    }
    if(tilesel != NULL) {
        tilesel->SetScrollbars(GetTileW(), GetTileH(), TILESEL_WIDTH, TILESEL_HEIGHT);
        tilesel->Refresh();
    }
    return true;
}

bool LevelEditView::SetTileSize(wxCoord w, wxCoord h) {
    return tileset.Resize(w, h) && SetTileset(tileset);
}

wxString LevelEditView::GetToolName(Tool tool, bool hexcode) {
    if(IsTile(tool))
        return GetTileName(tool, hexcode);
    switch(tool) {
    case TOOL_RECTSELECT:
        return wxT("rectangle select");
    case TOOL_TRAPWIRE:
        return wxT("trap wire");
    case TOOL_CLONEWIRE:
        return wxT("clone wire");
    }
    return wxT("");
}

LevelEditView::Tool LevelEditView::GetSelectedTool() const {
    if(tilelist != NULL && tilelist->GetSelection() >= TOOL_MIN && tilelist->GetSelection() <= TOOL_MAX)
        return tilelist->GetSelection();
    return TOOL_NONE;
}

bool LevelEditView::SetSelectedTool(Tool tool) {
    if(tilelist == NULL)
        return false;
    tilelist->SetSelection(tool);
    if(tilesel != NULL)
        tilesel->Refresh();
    return true;
}

void LevelEditView::SetSelection(wxUint32 x, wxUint32 y, wxUint32 w, wxUint32 h) {
    if(x == selx && y == sely && w == selw && h == selh)
        return;
    if(mapwin != NULL && mapbmp != NULL) {
        // Erase (the rendering of) the old selection and draw the new selection.
        wxDC* dc = NewClientDC();
        EraseSelection(dc);
        selx = x;
        sely = y;
        selw = w;
        selh = h;
        DrawSelection(dc);
        delete dc;
        AddIdleRedraw(DRAW_WIRES);
    } else {
        selx = x;
        sely = y;
        selw = w;
        selh = h;
        AddIdleRedraw(DRAW_MAP);
    }
}

void LevelEditView::SetStatusText(const wxString& text, int i) {
    if(statusbar != NULL)
        statusbar->SetStatusText(text, i);
}

bool LevelEditView::InitTileset() {
    if(tileset.IsOk())
        return true;
    if(!loadeddefaulttileset) {
        wxConfigBase* config = wxConfigBase::Get();
        long defaulttilew = -1, defaulttileh = -1;
        if(config != NULL) {
            config->Read(wxT("TileWidth"), &defaulttilew);
            config->Read(wxT("TileHeight"), &defaulttileh);
        }
        wxString filename = GetDataDir() + wxT("/tilesets/default");
        if(config != NULL)
            config->Read(wxT("Tileset"), &filename);
        defaulttileset.LoadFile(filename, defaulttilew, defaulttileh);
        loadeddefaulttileset = true;
    }
    if(defaulttileset.IsOk())
        SetTileset(defaulttileset);
    return tileset.IsOk();
}

wxDC* LevelEditView::NewClientDC() {
    if(mapwin == NULL)
        return NULL;
    wxDC* dc = new wxClientDC(mapwin);
    mapwin->DoPrepareDC(*dc);
    return dc;
}

wxMemoryDC* LevelEditView::NewBackingDC() {
    wxMemoryDC* backingdc = NULL;
    if(mapbmp == NULL) {
        mapbmp = new wxBitmap(32 * GetTileW(), 32 * GetTileH());
        backingdc = new wxMemoryDC;
        if(backingdc != NULL) {
            backingdc->SelectObject(*mapbmp);
            tileset.DrawLevel(level, backingdc, 0, 0, showupper);
        }
    } else {
        backingdc = new wxMemoryDC;
        if(backingdc != NULL)
            backingdc->SelectObject(*mapbmp);
    }
    return backingdc;
}

void LevelEditView::OnDraw(wxDC* dc) {
    if(dc == NULL || level == NULL || !InitTileset())
        return;
    wxMemoryDC* backingdc = NewBackingDC();
    dc->Blit(0, 0, mapbmp->GetWidth(), mapbmp->GetHeight(), backingdc, 0, 0);
    delete backingdc;
    DrawSelection(dc);
    DrawWires(dc);
}

void LevelEditView::DrawSelection(wxDC* dc, wxUint32 x, wxUint32 y, wxUint32 w, wxUint32 h) {
    if(dc != NULL && x < 32 && y < 32 && w > 0 && h > 0) {
        wxCoord thickness = std::max(1, std::min(GetTileW() / 8, GetTileH() / 8));
        wxPen pen(*wxBLACK, thickness);
        pen.SetCap(wxCAP_PROJECTING);
        pen.SetJoin(wxJOIN_ROUND);
        dc->SetPen(pen);
        dc->SetBrush(*wxTRANSPARENT_BRUSH);
        dc->DrawRectangle((wxCoord) x * GetTileW(), (wxCoord) y * GetTileH(), (wxCoord) w * GetTileW(), (wxCoord) h * GetTileH());
        wxPen pen2(*wxWHITE, std::max(1, thickness - 2), wxPENSTYLE_LONG_DASH);
        pen2.SetCap(wxCAP_PROJECTING);
        pen2.SetJoin(wxJOIN_ROUND);
        dc->SetPen(pen2);
        dc->DrawRectangle((wxCoord) x * GetTileW(), (wxCoord) y * GetTileH(), (wxCoord) w * GetTileW(), (wxCoord) h * GetTileH());
        dc->SetBrush(wxNullBrush);
        dc->SetPen(wxNullPen);
    }
}

void LevelEditView::EraseSelection(wxDC* dc, wxUint32 x, wxUint32 y, wxUint32 w, wxUint32 h) {
    if(dc != NULL && x < 32 && y < 32 && w > 0 && h > 0) {
        wxCoord thickness = std::max(1, std::min(GetTileW() / 8, GetTileH() / 8));
        // Get half of the thickness, rounded up to make sure we cover whatever alignment the pen uses.
        wxCoord halfThickness = (thickness + 1) / 2;
        // Now also apply this rounding to the actual thickness, again to make sure we get complete coverage.
        thickness = halfThickness * 2;
        // Determine coordinates.
        wxCoord outerLeft = (wxCoord) x * GetTileW() - halfThickness;
        wxCoord outerRight = (wxCoord) (x + w) * GetTileW() + halfThickness - 1;
        wxCoord innerRight = (wxCoord) (x + w) * GetTileW() - halfThickness - 1;
        wxCoord outerWidth = (wxCoord) w * GetTileW() + thickness;
        wxCoord outerTop = (wxCoord) y * GetTileH() - halfThickness;
        wxCoord innerTop = (wxCoord) y * GetTileH() + halfThickness;
        wxCoord outerBottom = (wxCoord) (y + h) * GetTileH() + halfThickness - 1;
        wxCoord innerBottom = (wxCoord) (y + h) * GetTileH() - halfThickness - 1;
        wxCoord innerHeight = (wxCoord) h * GetTileH() - thickness;
        if(outerRight < 32 * GetTileW() && outerBottom < 32 * GetTileH()) {
            // Redraw the areas of the level map that were covered by the selection border.
            wxMemoryDC* backingdc = NewBackingDC();
            dc->Blit(outerLeft, outerTop, outerWidth, thickness, backingdc, outerLeft, outerTop); // Top border, including corners.
            dc->Blit(outerLeft, innerBottom, outerWidth, thickness, backingdc, outerLeft, innerBottom); // Bottom border, including corners.
            dc->Blit(outerLeft, innerTop, thickness, innerHeight, backingdc, outerLeft, innerTop); // Left border, excluding corners.
            dc->Blit(innerRight, innerTop, thickness, innerHeight, backingdc, innerRight, innerTop); // Right border, excluding corners.
            delete backingdc;
        } else {
            // The selection border went outside the level map area, so clear and redraw everything.
            dc->Clear();
            wxMemoryDC* backingdc = NewBackingDC();
            dc->Blit(0, 0, 32 * GetTileW(), 32 * GetTileH(), backingdc, 0, 0);
            delete backingdc;
        }
    }
}

void LevelEditView::DrawWires(wxDC* dc, bool force) {
    if((force || showwires) && level != NULL) {
        if(!level->trapwires.empty()) {
            wxPen pen(wxColour(116, 96, 0));
            dc->SetPen(pen);
            for(std::list<WireRecord>::const_iterator it = level->trapwires.begin(); it != level->trapwires.end(); ++it) {
                dc->DrawLine((wxCoord) it->buttonx * GetTileW() + GetTileW() / 2, (wxCoord) it->buttony * GetTileH() + GetTileH() / 2,
                            (wxCoord) it->targetx * GetTileW() + GetTileW() / 2, (wxCoord) it->targety * GetTileH() + GetTileH() / 2);
            }
            dc->SetPen(wxNullPen);
        }
        if(!level->clonewires.empty()) {
            wxPen pen(wxColour(200, 0, 0));
            dc->SetPen(pen);
            for(std::list<WireRecord>::const_iterator it = level->clonewires.begin(); it != level->clonewires.end(); ++it) {
                dc->DrawLine((wxCoord) it->buttonx * GetTileW() + GetTileW() / 2, (wxCoord) it->buttony * GetTileH() + GetTileH() / 2,
                            (wxCoord) it->targetx * GetTileW() + GetTileW() / 2, (wxCoord) it->targety * GetTileH() + GetTileH() / 2);
            }
            dc->SetPen(wxNullPen);
        }
    }
}

void LevelEditView::DrawTileSelector(wxDC* dc) {
    if(dc == NULL || !InitTileset())
        return;
    wxCoord x, y;
    wxCoord trapwirex = 0, trapwirey = 0, clonewirex = 0, clonewirey = 0;
    wxCoord selX = -1, selY = -1;
    for(y = 0; y < TILESEL_HEIGHT; ++y) {
        for(x = 0; x < TILESEL_WIDTH; ++x) {
            if(IsTile(tileselgrid[x + TILESEL_WIDTH * y])) {
                tileset.DrawPair(tileselgrid[x + TILESEL_WIDTH * y], TILE_FLOOR, dc, x * GetTileW(), y * GetTileH());
            } else if(tileselgrid[x + TILESEL_WIDTH * y] == TOOL_TRAPWIRE) {
                tileset.DrawPair(TILE_FLOOR, TILE_FLOOR, dc, x * GetTileW(), y * GetTileH());
                trapwirex = x;
                trapwirey = y;
            } else if(tileselgrid[x + TILESEL_WIDTH * y] == TOOL_CLONEWIRE) {
                tileset.DrawPair(TILE_FLOOR, TILE_FLOOR, dc, x * GetTileW(), y * GetTileH());
                clonewirex = x;
                clonewirey = y;
            } else if(tileselgrid[x + TILESEL_WIDTH * y] == TOOL_RECTSELECT) {
                //tileset.DrawPair(TILE_FLOOR, TILE_FLOOR, dc, x * GetTileW(), y * GetTileH());
                dc->SetPen(*wxTRANSPARENT_PEN);
                dc->SetBrush(*wxWHITE_BRUSH);
                dc->DrawRectangle(x * GetTileW(), y * GetTileH(), GetTileW(), GetTileH());
                dc->SetBrush(wxNullBrush);
                dc->SetPen(wxNullPen);
                dc->SetPen(*wxBLACK_DASHED_PEN);
                dc->SetBrush(*wxTRANSPARENT_BRUSH);
                dc->DrawRectangle(x * GetTileW() + GetTileW() / 4, y * GetTileH() + GetTileH() / 4, GetTileW() / 2, GetTileH() / 2);
                dc->SetBrush(wxNullBrush);
                dc->SetPen(wxNullPen);
            } else {
                continue;
            }
            if(tilelist != NULL && tilelist->IsSelected(tileselgrid[x + TILESEL_WIDTH * y])) {
                selX = x;
                selY = y;
            }
        }
    }
    {
        wxPen pen(wxColour(116, 96, 0));
        dc->SetPen(pen);
        dc->DrawLine(   trapwirex * GetTileW() + GetTileW() / 2, (trapwirey - 1) * GetTileH() + GetTileH() / 2,
                        trapwirex * GetTileW() + GetTileW() / 2, (trapwirey + 1) * GetTileH() + GetTileH() / 2);
        dc->SetPen(wxNullPen);
    }
    {
        wxPen pen(wxColour(200, 0, 0));
        dc->SetPen(pen);
        dc->DrawLine(   clonewirex * GetTileW() + GetTileW() / 2, (clonewirey - 1) * GetTileH() + GetTileH() / 2,
                        clonewirex * GetTileW() + GetTileW() / 2, (clonewirey + 1) * GetTileH() + GetTileH() / 2);
        dc->SetPen(wxNullPen);
    }
    if(selX >= 0 && selY >= 0) {
        DrawSelection(dc, selX, selY);
    }
}

#if 0
void LevelEditView::SendUpdate(wxObject* hint, bool self, bool modify) {
    if(self)
        OnUpdate(this, hint);
    wxDocument* doc = GetDocument();
    if(doc != NULL) {
        if(modify)
            doc->Modify(true);
        doc->UpdateAllViews(this, hint);
    }
}
#endif

void LevelEditView::Refresh() {
    if(mapwin != NULL)
        mapwin->Refresh();
    if(tilesel != NULL)
        tilesel->Refresh();
}

void LevelEditView::OnUpdate(wxView* sender, wxObject* hint) {
    LevelUpdateHint* lhint = NULL;
    if(hint != NULL && hint->IsKindOf(CLASSINFO(LevelUpdateHint)))
        lhint = (LevelUpdateHint*) hint;
    if((lhint == NULL || lhint->type == LevelUpdateHint::CREATE || lhint->type == LevelUpdateHint::LIST || lhint->type == LevelUpdateHint::DESTROY) &&
    levellist != NULL && GetDocument() != NULL && GetDocument()->IsKindOf(CLASSINFO(LevelSetDoc))) {
        LevelSet* levelset = ((LevelSetDoc*) GetDocument())->GetLevelSet();
        wxString* choices = new wxString[levelset->GetLevelCount()];
        CountedPtr<Level> lev;
        wxUint16 i;
        for(i = 0; i < levelset->GetLevelCount(); ++i) {
            lev = levelset->GetLevel(i);
            choices[i] << lev->levelnumber << wxT(". ") << wxString(lev->title.c_str(), wxConvISO8859_1);
        }
        levellist->Set(levelset->GetLevelCount(), choices);
        delete[] choices;
        int sel = wxNOT_FOUND;
        for(i = 0; i < levellist->GetCount(); ++i) {
            lev = levelset->GetLevel(i);
            levellist->SetClientData(i, lev);
            if(lev == level)
                sel = i;
        }
        levellist->SetSelection(sel);
        OnChangeFilename();
    }
    if(lhint == NULL) {
        delete mapbmp;
        mapbmp = NULL;
        AddIdleRedraw(DRAW_ALL);
    } else if(lhint->level == level || lhint->level == NULL) {
        if(lhint->type == LevelUpdateHint::TILE && lhint->x < 32 && lhint->y < 32) {
            if(mapbmp != NULL && level != NULL && tileset.IsOk() && mapwin != NULL) {
                // Only redraw the specific tile that has been changed.
                wxMemoryDC dc2;
                dc2.SelectObject(*mapbmp);
                Tile upper, lower;
                if(showupper) {
                    upper = level->GetUpperTile(lhint->x, lhint->y);
                    lower = level->GetLowerTile(lhint->x, lhint->y);
                } else {
                    upper = level->GetLowerTile(lhint->x, lhint->y);
                    lower = TILE_FLOOR;
                }
                tileset.DrawPair(upper, lower, &dc2, lhint->x * GetTileW(), lhint->y * GetTileH());
                wxDC* dc = NewClientDC();
                if(dc != NULL) {
                    dc->Blit(lhint->x * GetTileW(), lhint->y * GetTileH(), GetTileW(), GetTileH(), &dc2, lhint->x * GetTileW(), lhint->y * GetTileH());
                    delete dc;
                }
                AddIdleRedraw(DRAW_MAP);
            } else {
                AddIdleRedraw(DRAW_MAP);
            }
        } else if(lhint->type == LevelUpdateHint::WIRE) {
            if(showwires)
                AddIdleRedraw(DRAW_MAP); // If a wire is removed, the tiles it was drawn over need redrawing.
        } else if(lhint->type == LevelUpdateHint::DESTROY) {
            level = NULL;
            delete mapbmp;
            mapbmp = NULL;
            AddIdleRedraw(DRAW_MAP);
        } else if(lhint->type == LevelUpdateHint::CREATE || lhint->type == LevelUpdateHint::TILE) {
            delete mapbmp;
            mapbmp = NULL;
            AddIdleRedraw(DRAW_MAP);
        }
    }
    wxView::OnUpdate(sender, hint);
}

void LevelEditView::OnIdle(wxIdleEvent& event) {
    event.Skip();
    DrawSpec draw = idleredraw;
    idleredraw = DRAW_NONE; // In case of reentrance.
    if(draw == DRAW_NONE) {
    } else if(draw == DRAW_WIRES) {
        wxDC* dc = NewClientDC();
        DrawWires(dc);
        delete dc;
    } else if(draw == DRAW_MAP) {
        if(mapwin != NULL)
            mapwin->Refresh();
    } else {
        Refresh();
    }
}

void LevelEditView::OnMouse(wxMouseEvent& event) {
    event.Skip(); // Allow normal processing to take place.
    if(event.GetEventObject() == NULL) {
        // This is included in case tilesel or mapwin is NULL.
    } else if(event.GetEventObject() == tilesel) {
        int x = TILESEL_WIDTH, y = TILESEL_HEIGHT;
        if(GetTileW() > 0 && GetTileH() > 0) {
            tilesel->GetViewStart(&x, &y);
            x += event.GetX() / GetTileW();
            y += event.GetY() / GetTileH();
        }
        Tool hover = TOOL_NONE;
        if(x < TILESEL_WIDTH && y < TILESEL_HEIGHT)
            hover = tileselgrid[x + y * TILESEL_WIDTH];
        if(event.ButtonDown() && hover != TOOL_NONE)
            SetSelectedTool(hover);
        Tool sel = GetSelectedTool();
        wxString status;
        status << wxT("[Selected: ") << GetToolName(sel) << wxT("]");
        if(hover != TOOL_NONE)
            status << wxT(" [Cursor: ") << GetToolName(hover) << wxT("]");
        SetStatusText(status);
    } else if(event.GetEventObject() == mapwin) {
        if(level == NULL)
            return;
        int origx = 32, origy = 32;
        if(GetTileW() > 0 && GetTileH() > 0) {
            mapwin->GetViewStart(&origx, &origy);
            origx += event.GetX() / GetTileW();
            origy += event.GetY() / GetTileH();
        }
        Tool sel = GetSelectedTool();
        wxString status;
        status << wxT("[Selected: ") << GetToolName(sel) << wxT("]");
        if(readonly)
            status << wxT(" [READ ONLY]");
        status << wxT(" (") << origx << wxT(", ") << origy << wxT(") - ");
        if(origx < 0)
            origx = 32;
        if(origy < 0)
            origy = 32;
        wxUint32 x = origx, y = origy;
#if 0
        LevelUpdateHint hint;
        hint.level = level;
        hint.x = x;
        hint.y = y;
#endif
        if(event.ButtonDown()) {
            activebutton = event.GetButton();
            if(GetLevelSetDoc() != NULL)
                GetLevelSetDoc()->FinishLevelChangeCommand();
        }
        if(event.ButtonUp()) {
            activebutton = wxMOUSE_BTN_NONE;
            if(GetLevelSetDoc() != NULL)
                GetLevelSetDoc()->FinishLevelChangeCommand();
        } else if(activebutton == wxMOUSE_BTN_LEFT) {
            if(!event.LeftIsDown() || event.RightIsDown() || event.MiddleIsDown())
                activebutton = wxMOUSE_BTN_NONE;
        } else if(activebutton == wxMOUSE_BTN_RIGHT) {
            if(event.LeftIsDown() || !event.RightIsDown() || event.MiddleIsDown())
                activebutton = wxMOUSE_BTN_NONE;
        } else if(activebutton == wxMOUSE_BTN_MIDDLE) {
            if(event.LeftIsDown() || event.RightIsDown() || !event.MiddleIsDown())
                activebutton = wxMOUSE_BTN_NONE;
        }
        if(activebutton == wxMOUSE_BTN_NONE)
            clickx = clicky = 32;
        if(x < 32 && y < 32 && (event.ButtonDown() || event.Dragging())) {
            wxConfigBase* config = wxConfigBase::Get();
            wxString combo;
            if(sel == TOOL_TRAPWIRE || sel == TOOL_CLONEWIRE)
                combo << wxT("Wire");
            else if(sel == TOOL_RECTSELECT)
                combo << wxT("Select");
            if(event.ControlDown())
                combo << wxT("Control");
            if(event.ShiftDown())
                combo << wxT("Shift");
            if(event.AltDown())
                combo << wxT("Alt");
#if 0       // Apparently some systems treat Num Lock as Meta, causing trouble...
            if(event.MetaDown())
                combo << wxT("Meta");
#endif
            if(activebutton == wxMOUSE_BTN_LEFT)
                combo << wxT("Left");
            else if(activebutton == wxMOUSE_BTN_RIGHT)
                combo << (buttonswap ? wxT("Middle") : wxT("Right"));
            else if(activebutton == wxMOUSE_BTN_MIDDLE)
                combo << (buttonswap ? wxT("Right") : wxT("Middle"));
            else
                combo.clear();
            wxString action;
            if(combo == wxT("Left"))
                action = wxT("PlaceTile");
            else if(combo == wxT("ControlLeft"))
                action = wxT("PlaceTileSpecial");
            else if(combo == wxT("ShiftLeft"))
                action = wxT("PlaceTileCombine");
            else if(combo == wxT("ControlShiftLeft"))
                action = wxT("PlaceTileCombineSpecial");
            else if(combo == wxT("Right"))
                action = wxT("PlaceTileLower");
            else if(combo == wxT("ControlRight"))
                action = wxT("PlaceTileLowerSpecial");
            else if(combo == wxT("ShiftRight"))
                action = wxT("PlaceTileUpper");
            else if(combo == wxT("ControlShiftRight"))
                action = wxT("PlaceTileUpperSpecial");
            else if(combo == wxT("SelectLeft") || combo == wxT("SelectMiddle"))
                action = wxT("Select");
            else if(combo == wxT("SelectControlLeft") || combo == wxT("SelectControlMiddle"))
                action = wxT("ClearSelect");
            else if(combo == wxT("WireLeft") || combo == wxT("WireShiftMiddle"))
                action = wxT("PlaceWire");
            else if(combo == wxT("WireControlLeft") || combo == wxT("WireControlShiftMiddle"))
                action = wxT("ClearWires");
            else if(combo == wxT("WireRight"))
                action = wxT("ForceWire");
            else if(combo == wxT("WireControlRight"))
                action = wxT("ForceWireDup");
            else if(combo == wxT("WireShiftRight"))
                action = wxT("SelectSingle");
            else if(combo == wxT("WireControlShiftRight"))
                action = wxT("SelectSingle");
            else if(combo == wxT("Middle") || combo == wxT("WireMiddle"))
                action = wxT("RectSelect");
            else if(combo == wxT("ControlMiddle") || combo == wxT("WireControlMiddle"))
                action = wxT("ClearSelect");
            else if(combo == wxT("ShiftMiddle") || combo == wxT("SelectShiftMiddle"))
                action = wxT("PlaceWire");
            else if(combo == wxT("ControlShiftMiddle") || combo == wxT("SelectControlShiftMiddle"))
                action = wxT("ClearWires");
            if(config != NULL && !combo.empty())
                config->Read(wxT("EditorMouseControls/") + combo, &action);
            action.MakeLower();
            if(action == wxT("forcewiredup") && !event.ButtonDown())
                action = wxT("forcewire");
            bool hadaction = false;
            if((selw > 1 || selh > 1) && x >= selx && x < selx + selw && y >= sely && y < sely + selh && IsTile(sel) && action.substr(0, 9) == wxT("placetile")) {
                hadaction = DoTileActionSel(sel, action/*, hint*/);
//                hint.x = 32;
//                hint.y = 32;
            } else {
                hadaction = DoTileAction(x, y, sel, action/*, hint*/);
            }
            if(hadaction && config != NULL && !config->Exists(wxT("EditorMouseControls/Version")))
                config->Write(wxT("EditorMouseControls/Version"), (long) 1);
        }
        status << level->GetTileDescription(x, y);
        SetStatusText(status);
#if 0
        if(hint.type != LevelUpdateHint::NONE) {
            OnUpdate(this, &hint);
            if(GetDocument() != NULL) {
                GetDocument()->Modify(true);
                GetDocument()->UpdateAllViews(this, &hint);
            }
        }
#endif
    }
}

bool LevelEditView::DoTileAction(wxUint32 x, wxUint32 y, wxUint32 tile, wxString action/*, LevelUpdateHint& hint*/, bool single) {
    if(level == NULL)
        return false;
    bool hadaction = false;
    bool placed = false;
    bool special = false;
    WireRecord wire;
    wire.buttonx = x;
    wire.buttony = y;
    wire.targetx = selx;
    wire.targety = sely;
    if(action.size() >= 7 && action.substr(action.size() - 7) == wxT("special")) {
        special = true;
        action.erase(action.size() - 7);
    }
    if(action == wxT("placetile") && IsTile(tile)) {
        hadaction = true;
        if(!readonly) {
            if(tile == TILE_FLOOR && special)
                placed = level->RemoveProperUpper(x, y);
            else
                placed = level->PlaceTileOnly(x, y, tile, !special);
        }
    } else if(action == wxT("placetilecombine") && IsTile(tile)) {
        hadaction = true;
        if(!readonly)
            placed = level->PlaceTileAuto(x, y, tile, !special);
    } else if(action == wxT("placetilelower") && IsTile(tile)) {
        hadaction = true;
        if(!readonly)
            placed = level->PlaceTileLower(x, y, tile, !special);
    } else if(action == wxT("placetileupper") && IsTile(tile)) {
        hadaction = true;
        if(!readonly)
            placed = level->PlaceTileUpper(x, y, tile, !special);
#if 0
    } else if((tile == TOOL_TRAPWIRE || tile == TOOL_CLONEWIRE) && (action == wxT("placetile") || action == wxT("cleartile"))) {
        hadaction = true;
        if(IsTileWireable(level->GetUpperTile(x, y))) {
            tile = level->GetUpperTile(x, y);
            placed = !readonly;
        } else if(IsTileWireable(level->GetLowerTile(x, y))) {
            tile = level->GetLowerTile(x, y);
            placed = !readonly;
        }
        special = (action == wxT("cleartile"));
#endif
// FIXME: Make sure the line below has correct parenthesisation.
    } else if((action == wxT("rectselect") || (action == wxT("select") && tile == TOOL_RECTSELECT)) && !special) {
        hadaction = true;
        if(clickx >= 32 || clicky >= 32) {
            clickx = x;
            clicky = y;
        }
        wxUint32 newselx, newsely, newselw, newselh;
        if((unsigned int) x < clickx) {
            newselx = x;
            newselw = 1 + clickx - x;
        } else {
            newselx = clickx;
            newselw = 1 + x - clickx;
        }
        if((unsigned int) y < clicky) {
            newsely = y;
            newselh = 1 + clicky - y;
        } else {
            newsely = clicky;
            newselh = 1 + y - clicky;
        }
        SetSelection(newselx, newsely, newselw, newselh);
    } else if(action == wxT("clearselect") && !special) {
        hadaction = true;
        ClearSelection();
    } else if(action == wxT("selectsingle") && !special) {
        hadaction = true;
        SetSelection(x, y);
    } else if((action == wxT("forcewire") || action == wxT("forcewiredup")) && !special && selw == 1 && selh == 1) {
        if(tile == TOOL_TRAPWIRE) {
            hadaction = true;
            if(!readonly)
                level->AddTrapWire(wire, action == wxT("forcewiredup"));
        } else if(tile == TOOL_CLONEWIRE) {
            hadaction = true;
            if(!readonly)
                level->AddCloneWire(wire, action == wxT("forcewiredup"));
        }
    } else if(action == wxT("placewire") && !special) {
        if((tile != TOOL_CLONEWIRE && level->HasTile(x, y, TILE_TRAP)) || (tile != TOOL_TRAPWIRE && level->HasCloner(x, y))) {
            hadaction = true;
            SetSelection(x, y);
        } else if(selw == 1 && selh == 1) {
            if(tile != TOOL_CLONEWIRE && level->HasTile(x, y, TILE_BUTTON_TRAP) && level->HasTile(selx, sely, TILE_TRAP)) {
                hadaction = true;
                if(!readonly)
                    level->SetTrapWire(wire);
            }
            if(tile != TOOL_TRAPWIRE && level->HasTile(x, y, TILE_BUTTON_CLONE) && level->HasCloner(selx, sely)) {
                hadaction = true;
                if(!readonly)
                    level->SetCloneWire(wire);
            }
        }
    } else if(action == wxT("clearwires") && !special) {
        hadaction = true;
        if(!readonly)
            level->RemoveWire(x, y, tile != TOOL_CLONEWIRE, tile != TOOL_CLONEWIRE, tile != TOOL_TRAPWIRE, tile != TOOL_TRAPWIRE);
    }
    if(placed) {
        if(special) {
            if(!readonly)
                level->RemoveWire(x, y, tile);
        } else if(single && (tile == TILE_TRAP || tile == TILE_CLONER)) {
            SetSelection(x, y);
        } else if(selw == 1 && selh == 1) {
            if(tile == TILE_BUTTON_TRAP && level->HasTile(selx, sely, TILE_TRAP)) {
                if(!readonly) {
//                    level->RemoveWire(x, y, tile);
                    level->SetTrapWire(wire);
                }
            } else if(tile == TILE_BUTTON_CLONE && level->HasCloner(selx, sely)) {
                if(!readonly) {
//                    level->RemoveWire(x, y, tile);
                    level->SetCloneWire(wire);
                }
            } else {
                ClearSelection();
            }
#if 0
            } else if(HasSelection()) {
                ClearSelection();
            }
#endif
        } else if(/*HasSelection() &&*/ (x < selx || x >= selx + selw || y < sely || y >= sely + selh)) {
            ClearSelection();
        }
    }
    return hadaction;
}

bool LevelEditView::DoTileActionSel(wxUint32 tile, wxString action/*, LevelUpdateHint& hint*/) {
    if(selx >= 32 || sely >= 32 || selw < 1 || selh < 1)
        return false;
    if(selw == 1 && selh == 1)
        return DoTileAction(selx, sely, tile, action/*, hint*/, true);
    bool hadaction = false;
    for(wxUint32 y = sely; y < sely + selh; ++y) {
        for(wxUint32 x = selx; x < selx + selw; ++x) {
            if(DoTileAction(x, y, tile, action/*, hint*/, false))
                hadaction = true;
        }
    }
    return hadaction;
}

}


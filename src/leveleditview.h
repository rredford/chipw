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

#ifndef CHIPW_LEVELEDITVIEW_H_INCLUDED
#define CHIPW_LEVELEDITVIEW_H_INCLUDED

#include <wx/docview.h>
#include <wx/event.h>
#include "tileset.h"
#include "level.h"

class wxBitmap;
class wxListBox;
class wxStatusBar;
class wxDC;

namespace ChipW {

class LevelSet;
class LevelSetDoc;
class LevelMapWindow;
class TileSelectWindow;
class LevelUpdateHint;

class LevelEditView : public wxView {
public:
    LevelEditView();
    virtual ~LevelEditView();
    virtual bool OnCreate(wxDocument* doc, long flags);
    virtual void OnDraw(wxDC* dc);
    virtual void OnUpdate(wxView* sender, wxObject* hint = NULL);
    virtual bool OnClose(bool delwin);
    virtual void OnActivateView(bool activate, wxView *activeView, wxView *deactiveView);
    virtual void OnChangeFilename();
    void DrawTileSelector(wxDC* dc);
    void OnMouse(wxMouseEvent& event);
    CountedPtr<Level> GetLevel() const {return level;}
    void SetLevel(CountedPtr<Level> newlevel);
    LevelSetDoc* GetLevelSetDoc() const;
    LevelSet* GetLevelSet() const;
    const Tileset& GetTileset() const {return tileset;}
    bool SetTileset(const Tileset& tiles);
    bool InitTileset();
    wxCoord GetTileW() const {return tileset.GetTileWidth();}
    wxCoord GetTileH() const {return tileset.GetTileHeight();}
    bool SetTileSize(wxCoord w = -1, wxCoord h = -1);
    typedef wxUint16 Tool;
    enum {
        TOOL_RECTSELECT = TILE_MAX + 1,
        TOOL_TRAPWIRE,
        TOOL_CLONEWIRE,
        TOOL_NONE,
        TOOL_MIN = TILE_MIN,
        TOOL_MAX = TOOL_NONE - 1,
    };
    static wxString GetToolName(Tool tool, bool hexcode = false);
    Tool GetSelectedTool() const;
    bool SetSelectedTool(Tool tool);
    bool HasSelectedTile() const {return IsTile(GetSelectedTool());}
    Tile GetSelectedTile() const {return GetSelectedTool();}
    bool SetSelectedTile(Tile tile) {return SetSelectedTool(tile);}
    bool HasSelection() const {return selx < 32 && sely < 32 && selw > 0 && selh > 0;}
    bool IsInSelection(wxUint32 x, wxUint32 y) const {return x >= selx && x < (selx + selw) && y >= sely && y < (sely + selh);}
    void SetSelection(wxUint32 x, wxUint32 y, wxUint32 w = 1, wxUint32 h = 1);
    void ClearSelection() {SetSelection(32, 32, 0, 0);}
    void SetStatusText(const wxString& text, int i = 0);
    LevelEditView* NewView(CountedPtr<Level> newlevel);
    LevelEditView* NewView() {return NewView(level);}
    void Refresh();
private:
    void OnCommand(wxCommandEvent& event);
    void OnUpdateUI(wxUpdateUIEvent& event);
    void OnListBox(wxCommandEvent& event);
    void OnIdle(wxIdleEvent& event);
//    void SendUpdate(wxObject* hint = NULL, bool self = true, bool modify = true);
    wxDC* NewClientDC();
    wxMemoryDC* NewBackingDC();
    void DrawSelection(wxDC* dc, wxUint32 x, wxUint32 y, wxUint32 w = 1, wxUint32 h = 1);
    void DrawSelection(wxDC* dc) {DrawSelection(dc, selx, sely, selw, selh);}
    void EraseSelection(wxDC* dc, wxUint32 x, wxUint32 y, wxUint32 w = 1, wxUint32 h = 1);
    void EraseSelection(wxDC* dc) {EraseSelection(dc, selx, sely, selw, selh);}
    void DrawWires(wxDC* dc, bool force = false);
    bool DoTileAction(wxUint32 x, wxUint32 y, wxUint32 tile, wxString action/*, LevelUpdateHint& hint*/, bool single = true);
    bool DoTileActionSel(wxUint32 tile, wxString action/*, LevelUpdateHint& hint*/);
    enum DrawSpec {
        DRAW_NONE,
        DRAW_WIRES,
        DRAW_MAP,
        DRAW_ALL,
    };
    void AddIdleRedraw(DrawSpec spec) {if(spec > idleredraw) idleredraw = spec;}
    CountedPtr<Level> level;
    Tileset tileset;
    static bool loadeddefaulttileset;
    static Tileset defaulttileset;
    wxUint32 selx, sely, selw, selh;
    wxUint32 clickx, clicky;
    bool readonly;
    bool showwires;
    bool showupper;
    int activebutton;
    bool buttonswap;
    DrawSpec idleredraw;
    LevelMapWindow* mapwin;
    wxStatusBar* statusbar;
    wxBitmap* mapbmp;
    wxListBox* levellist;
    wxListBox* tilelist;
    TileSelectWindow* tilesel;
    DECLARE_EVENT_TABLE()
    DECLARE_DYNAMIC_CLASS(LevelEditView)
};

}

#endif // !CHIPW_LEVELEDITVIEW_H_INCLUDED
